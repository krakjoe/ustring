/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2014 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: krakjoe@php.net                                              |
  +----------------------------------------------------------------------+
*/
#ifndef PHP_USTRING_BACKEND_ICU
#define PHP_USTRING_BACKEND_ICU

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "unicode/unistr.h"

extern "C" {
#    include "php.h"
#    include "php_ini.h"
#    include "ext/standard/info.h"
#    include "php_ustring.h"
#    include "../api.h"
#    include "backend/api.h"
}

typedef struct _php_ustring_t {
	UnicodeString *val;
	zend_string   *codepage;
	zend_object   std;
} php_ustring_t;

typedef struct _php_ustring_iterator_t {
	zend_object_iterator zit;
	zval zobject;
	zval zdata;
	int32_t position;
} php_ustring_iterator_t;

#define php_ustring_fetch(o) ((php_ustring_t*) (((char*)Z_OBJ_P(o)) - XtOffsetOf(php_ustring_t, std)))

static inline php_ustring_t *php_ustring_copy(php_ustring_t *target, php_ustring_t *source)
{
	target->val = new UnicodeString(*source->val);
	target->codepage = zend_string_copy(source->codepage);

	return target;
}

static inline php_ustring_t *php_ustring_copy_ex(php_ustring_t *target, php_ustring_t *source, int32_t offset, int32_t length)
{
	target->val = new UnicodeString(*source->val, offset, length);
	target->codepage = zend_string_copy(source->codepage);

	return target;
}

#define php_ustring_zval_copy_ex(target, source, offset, length) \
	_php_ustring_zval_copy_ex(target, source, offset, length TSRMLS_CC)

static inline zval *_php_ustring_zval_copy_ex(zval *target, php_ustring_t *source, int32_t offset, int32_t length TSRMLS_DC)
{
	object_init_ex(target, ce_UString);

	php_ustring_copy_ex(php_ustring_fetch(target), source, offset, length);

	return target;
}

#define php_ustring_unistr_from_zval(target, source, codepage) \
	_php_ustring_unistr_from_zval(target, source, codepage TSRMLS_CC)

static inline int _php_ustring_unistr_from_zval(UnicodeString *target, zval *source, zend_string *codepage TSRMLS_DC)
{
	switch (Z_TYPE_P(source)) {
		case IS_STRING:
			*target = UnicodeString(Z_STRVAL_P(source), (int32_t) Z_STRLEN_P(source), codepage->val);
			break;

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(source), ce_UString TSRMLS_CC)) {
				*target = *(php_ustring_fetch(source))->val;
				break;
			}

		default:
			return FAILURE;
	}

	return SUCCESS;
}

zend_object_handlers php_ustring_handlers;

static inline void _php_ustring_free(zend_object *zobject TSRMLS_DC) {
	php_ustring_t *ustring = (php_ustring_t*)((char*)(zobject) - XtOffsetOf(php_ustring_t, std));

	zend_object_std_dtor(&ustring->std TSRMLS_CC);

	if (ustring->codepage) {
	    zend_string_release(ustring->codepage);
	}

	delete ustring->val;
}

static inline zend_object* _php_ustring_create(zend_class_entry *ce TSRMLS_DC) {
	php_ustring_t *ustring =
		(php_ustring_t*) ecalloc(1, sizeof(php_ustring_t));

	zend_object_std_init(&ustring->std, ce TSRMLS_CC);

	ustring->std.handlers = &php_ustring_handlers;

	return &ustring->std;
} /* }}} */

/* {{{ */
static inline zval* _php_ustring_read(zval *object, zval *offset, int type, zval *rv TSRMLS_DC) {
	zend_bool clean = 0;

	if (Z_TYPE_P(offset) != IS_LONG) {
		convert_to_long(offset);
		clean = 1;
	}

	php_ustring_zval_copy_ex(rv, php_ustring_fetch(object), Z_LVAL_P(offset), 1);

	if (clean) {
		zval_ptr_dtor(offset);
	}

	return rv;
} /* }}} */

/* {{{ */
static inline void _php_ustring_iterator_dtor(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit = (php_ustring_iterator_t*) iterator;

	zval_ptr_dtor(&uit->zobject);

	if (Z_TYPE(uit->zdata) != IS_UNDEF) {
		zval_ptr_dtor(&uit->zdata);
		ZVAL_UNDEF(&uit->zdata);
	}

	zend_iterator_dtor(iterator TSRMLS_CC);
} /* }}} */

/* {{{ */
static inline int _php_ustring_iterator_validate(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;
	php_ustring_t *pstring = php_ustring_fetch(&uit->zobject);

	return (uit->position < pstring->val->length()) ? SUCCESS : FAILURE;
} /* }}} */

/* {{{ */
static inline zval* _php_ustring_iterator_current_data(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;
	php_ustring_t *ustring,
				  *pstring = php_ustring_fetch(&uit->zobject);

	object_init_ex(&uit->zdata, ce_UString);

	php_ustring_copy_ex(php_ustring_fetch(&uit->zdata), pstring, uit->position, 1);

	return &uit->zdata;
} /* }}} */

/* {{{ */
static inline void _php_ustring_iterator_current_key(zend_object_iterator* iterator, zval *key TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;

	ZVAL_LONG(key, uit->position);
} /* }}} */

/* {{{ */
static inline void _php_ustring_iterator_move_forward(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;

	uit->position++;
} /* }}} */

/* {{{ */
zend_object_iterator_funcs php_ustring_iterator_funcs = {
	_php_ustring_iterator_dtor,
	_php_ustring_iterator_validate,
	_php_ustring_iterator_current_data,
	_php_ustring_iterator_current_key,
	_php_ustring_iterator_move_forward,
	NULL
}; /* }}} */

/* {{{ */
static inline zend_object_iterator* _php_ustring_iterator(zend_class_entry *ce, zval *zobject, int by_ref TSRMLS_DC) {
	php_ustring_iterator_t *uit = (php_ustring_iterator_t*) emalloc(sizeof(php_ustring_iterator_t));

	zend_iterator_init((zend_object_iterator*)uit TSRMLS_CC);

	uit->zit.funcs = &php_ustring_iterator_funcs;
	uit->position = 0;

	ZVAL_COPY(&uit->zobject, zobject);
	ZVAL_UNDEF(&uit->zdata);

	return (zend_object_iterator*) uit;
} /* }}} */

static inline int _php_ustring_cast(zval *zread, zval *zwrite, int type TSRMLS_DC) {
	php_ustring_t *ustring;
	int32_t length = 0;

	if (type != IS_STRING) {
		return FAILURE;
	}

	ustring = php_ustring_fetch(zread);

	length = ustring->val->extract
		(0, ustring->val->length(), NULL, 0, ustring->codepage->val);

	if (length) {
		zend_string *output = zend_string_alloc(length, 0);

		ustring->val->extract(
			0,
			ustring->val->length(),
			(char*)output->val,
			length,
			ustring->codepage->val);

		output->val[length] = '\0';

		ZVAL_NEW_STR(zwrite, output);
	} else {
		ZVAL_EMPTY_STRING(zwrite);
	}

	return SUCCESS;
}

static zval *_php_ustring_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv TSRMLS_DC) 
{
	if (type != BP_VAR_IS && type != BP_VAR_R) {
		php_error(E_ERROR, "Retrieval of UString properties for modification is unsupported" TSRMLS_CC);
	}
	
	/* length property doesn't actually exist */
	if (Z_TYPE_P(member) == IS_STRING && strcmp(Z_STRVAL_P(member), "length") == 0) {
		ZVAL_LONG(rv, php_ustring_fetch(object)->val->length());
		return rv;
	}
	
	return std_object_handlers.read_property(object, member, type, cache_slot, rv TSRMLS_CC);
}

static void _php_ustring_write_property(zval *object, zval *member, zval *value, void **cache_slot TSRMLS_DC) 
{
	php_error(E_ERROR, "Writing to UString properties is unsupported" TSRMLS_CC);
}

static int _php_ustring_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot TSRMLS_DC) 
{
	if (Z_TYPE_P(member) == IS_STRING && strcmp(Z_STRVAL_P(member), "length") == 0) {
		switch (has_set_exists) {
			/* 0 (has) whether property exists and is not NULL */
			/* 2 (exists) whether property exists */
			case 0:
			case 2:
				return 1;
			/* 1 (set) whether property exists and is true */
			case 1:
				return !!php_ustring_fetch(object)->val->length();
		}
	}
	
	return std_object_handlers.has_property(object, member, has_set_exists, cache_slot TSRMLS_CC);
}

static void _php_ustring_unset_property(zval *object, zval *member, void **cache_slot TSRMLS_DC) 
{
	php_error(E_ERROR, "UString properties cannot be unset" TSRMLS_CC);
}

static inline int _php_ustring_operate(zend_uchar opcode, zval *result, zval *op1, zval *op2 TSRMLS_DC) {
	switch (opcode) {
		case ZEND_CONCAT: {
			php_ustring_t *uresult;
			php_ustring_t *uop1, *uop2;

			if (op1 != result) {
				object_init_ex(result, ce_UString);

				uresult = php_ustring_fetch(result);
				uresult->val = new UnicodeString();

				switch (Z_TYPE_P(op1)) {
					case IS_STRING: {
						uresult->val->append(UnicodeString(Z_STRVAL_P(op1), (int32_t) Z_STRLEN_P(op1)));
					} break;

					case IS_OBJECT: {
						uop1 = php_ustring_fetch(op1);

						uresult->val->append(*uop1->val);
					} break;

					default:
						return FAILURE;
				}
			} else {
				uresult = php_ustring_fetch(result);
			}

			switch (Z_TYPE_P(op2)) {
				case IS_STRING: {
					uresult->val->append(UnicodeString(Z_STRVAL_P(op2), (int32_t) Z_STRLEN_P(op2)));
				} break;

				case IS_OBJECT: {
					uop2 = php_ustring_fetch(op2);

					uresult->val->append(*uop2->val);
				} break;

				default:
					return FAILURE;
			}

			return SUCCESS;
		} break;
	}

	return FAILURE;
}

static inline void _php_ustring_construct(zval *that, const char *value, int32_t vlen, const char *codepage, int32_t clen TSRMLS_DC) {
    php_ustring_t* ustring;
    
    if (Z_TYPE_P(that) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(that), ce_UString TSRMLS_CC)) {
        return;
    }

    ustring = php_ustring_fetch(that);
 
    if (!codepage) {
        ustring->codepage = zend_string_copy(UG(codepage));
    } else {
		ustring->codepage = zend_string_init(codepage, clen, 0);
	}
    
    ustring->val = new UnicodeString(value, vlen, ustring->codepage->val);
}

static inline bool _php_ustring_startsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;

	if (php_ustring_unistr_from_zval(&uneedle, needle, ustring->codepage) == FAILURE) {
		return false;
	}

    return ustring->val->startsWith(uneedle);
}

static inline bool _php_ustring_endsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;

	if (php_ustring_unistr_from_zval(&uneedle, needle, ustring->codepage) == FAILURE) {
		return false;
	}

    return ustring->val->endsWith(uneedle);
}

static inline int32_t _php_ustring_indexOf(zval *that, zval *needle, int32_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else {
		h = *ustring->val;
	}

	if (php_ustring_unistr_from_zval(&n, needle, ustring->codepage) == FAILURE) {
		return -1;
	}

    return h.indexOf(n);
}

static inline int32_t _php_ustring_lastIndexOf(zval *that, zval *needle, int32_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else {
		h = *ustring->val;
	}

	if (php_ustring_unistr_from_zval(&n, needle, ustring->codepage) == FAILURE) {
		return -1;
	}

    return h.lastIndexOf(n);
}

static inline zval* _php_ustring_toLower(zval *that, zval *lowered TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(lowered, ce_UString);

	ostring = php_ustring_copy(php_ustring_fetch(lowered), php_ustring_fetch(that));
	ostring->val->toLower();
   
    return lowered;
}

static inline zval* _php_ustring_toUpper(zval *that, zval *uppered TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(uppered, ce_UString);

	ostring = php_ustring_copy(php_ustring_fetch(uppered), php_ustring_fetch(that));
    ostring->val->toUpper();
    
    return uppered;
}

static inline zval* _php_ustring_reverse(zval *that, zval *reversed TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(reversed, ce_UString);

    ostring = php_ustring_copy(php_ustring_fetch(reversed), php_ustring_fetch(that));
    ostring->val->reverse();

    return reversed;
}

static inline zval* _php_ustring_trim(zval *that, zval *trimmed TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(trimmed, ce_UString);

    ostring = php_ustring_copy(php_ustring_fetch(trimmed), php_ustring_fetch(that));
    ostring->val->trim();
    
    return trimmed;
}

static inline zval* _php_ustring_replace(zval *that, zval *search, zval *replace, zval *replaced TSRMLS_DC) {
	php_ustring_t *ustring = php_ustring_fetch(that), *ostring;
	UnicodeString s, r;

	if (php_ustring_unistr_from_zval(&s, search, ustring->codepage) == FAILURE ||
			php_ustring_unistr_from_zval(&r, replace, ustring->codepage) == FAILURE) {
		return NULL;
	}
    
    object_init_ex(replaced, ce_UString);
    
    ostring = php_ustring_copy(php_ustring_fetch(replaced), ustring);
	ostring->val->findAndReplace(s, r);
	
	return replaced;
}

static inline zval* _php_ustring_replaceSlice(zval *that, zval *slice, int32_t offset, int32_t length, zval *replaced TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that), *ostring;
    UnicodeString s;

	if (php_ustring_unistr_from_zval(&s, slice, ustring->codepage) == FAILURE) {
		return NULL;
	}

	if (offset == -1) {
		offset = 0;
	}
	if (length == -1) {
		length = s.length() - offset;
	}
    
    object_init_ex(replaced, ce_UString);
    
    ostring = php_ustring_copy(php_ustring_fetch(replaced), ustring);
    ostring->val->replace(offset, length, s, 0, s.length());
    
    return replaced;
}

static inline zval* _php_ustring_charAt(zval *that, int32_t offset, zval *found TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring = NULL;

    UChar c = ustring->val->charAt(offset);

	if (c) {
		object_init_ex(found, ce_UString);

		ostring = php_ustring_fetch(found);
		ostring->val = new UnicodeString(&c, 1);
		ostring->codepage = zend_string_copy(ustring->codepage);
	}
	
	return found;
}

static inline zval* _php_ustring_substring(zval *that, int32_t offset, int32_t length, zval *sub TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that), *ostring;
    
	if (offset < 0) {
	    return NULL;
	}
	
	if (length == -1) {
	    length = ustring->val->length() - offset;
	}

	object_init_ex(sub, ce_UString);
	
	php_ustring_copy_ex(php_ustring_fetch(sub), ustring, offset, length);
	
	return sub;
}

static inline bool _php_ustring_contains(zval *that, zval *text TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString t;

	if (php_ustring_unistr_from_zval(&t, text, ustring->codepage) == FAILURE) {
		return false;
	}
    
    return (ustring->val->indexOf(t) >= 0);
}

static inline zval* _php_ustring_chunk(zval *that, int32_t length, zval *chunks TSRMLS_DC) {
	php_ustring_t *ustring = php_ustring_fetch(that);
    int32_t position = 0;
    
    array_init(chunks);
    
    if (!length) 
        length = 1;
    
    while (position < ustring->val->length()) {
        zval chunked;
        php_ustring_t *uchunk;
        
        object_init_ex(&chunked, ce_UString);
        
        php_ustring_copy_ex(php_ustring_fetch(&chunked), ustring, position, length);
        
        add_next_index_zval(chunks, &chunked);
        
        position += length;
    }
    
    return chunks;
}

static inline zval* _php_ustring_repeat(zval *that, int32_t count, zval *repeated TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(getThis()), 
                  *urepeat;
    int32_t current = 0;
    
    object_init_ex(repeated, ce_UString);
    
    urepeat = php_ustring_copy(php_ustring_fetch(repeated), ustring);
    
    while (++current < count) {
        urepeat->val->append((*ustring->val));
    }
    
    return repeated;
}

static inline int _php_ustring_compare(zval *op1, zval *op2 TSRMLS_DC) {
	UnicodeString us1, us2;

	if (php_ustring_unistr_from_zval(&us1, op1, UG(codepage)) == FAILURE ||
			php_ustring_unistr_from_zval(&us2, op2, UG(codepage)) == FAILURE) {
		return 0;
	}

	return us1.compare(us2);
}

static inline zval* _php_ustring_pad(zval *that, int32_t targetLength, zval *pad, int mode, zval *result TSRMLS_DC) {
	php_ustring_t *ustring = php_ustring_fetch(that),
	              *ostring;
	UnicodeString padString;
	int32_t       sourceLength = ustring->val->length();
	int32_t       padLength, leftPadLength, rightPadLength, padStringLength, i;

	if (php_ustring_unistr_from_zval(&padString, pad, ustring->codepage) == FAILURE) {
		return NULL;
	}
    padStringLength = padString.length();

	object_init_ex(result, ce_UString);

	ostring = php_ustring_fetch(result);

	if (padStringLength < 1 || targetLength < 0 || targetLength <= sourceLength) {
		php_ustring_copy(ostring, ustring);
		return result;
	}

	if (padStringLength == 1 && mode != STR_PAD_BOTH) {
		php_ustring_copy(ostring, ustring);

		if (mode == STR_PAD_LEFT) {
			ostring->val->padLeading(targetLength, padString.charAt(0));
		} else {
			ostring->val->padTrailing(targetLength, padString.charAt(0));
		}

		return result;
	}

	ostring->val = new UnicodeString();
	ostring->codepage = zend_string_copy(ustring->codepage);

	padLength = targetLength - sourceLength;

	switch (mode) {
		case STR_PAD_RIGHT:
			leftPadLength = 0;
			rightPadLength = padLength;
			break;

		case STR_PAD_LEFT:
			leftPadLength = padLength;
			rightPadLength = 0;
			break;

		case STR_PAD_BOTH:
			leftPadLength = padLength / 2;
			rightPadLength = padLength - leftPadLength;
			break;
	}

	for (i = 0; i < leftPadLength; ++i) {
		ostring->val->append(padString.charAt(i % padStringLength));
	}
	ostring->val->append(*ustring->val);
	for (i = 0; i < rightPadLength; ++i) {
		ostring->val->append(padString.charAt(i % padStringLength));
	}

	return result;
}

static inline zval* _php_ustring_split(zval *that, zval *delimiter, int32_t limit, zval *pieces TSRMLS_DC) {
	php_ustring_t *ustring = php_ustring_fetch(that);
	UnicodeString delim;
	int32_t pos, start, delimLength, count;
	zval piece;

	if (php_ustring_unistr_from_zval(&delim, delimiter, ustring->codepage) == FAILURE) {
		return NULL;
	}
	delimLength = delim.length();

	if (delimLength == 0 || limit < 0) {
		return NULL;
	}

	array_init(pieces);

	if (ustring->val->length() == 0) {
		if (limit >= 0) {
			add_next_index_zval(pieces, php_ustring_zval_copy_ex(&piece, ustring, 0, 0));
		}
		return pieces;
	}

	if (limit < 0) {
		return NULL;
	} else if (limit <= 1) {
		add_next_index_zval(pieces, php_ustring_zval_copy_ex(&piece, ustring, 0, ustring->val->length()));
		return pieces;
	}

	start = 0; pos = ustring->val->indexOf(delim, 0);

	if (pos == -1) {
		add_next_index_zval(pieces, php_ustring_zval_copy_ex(&piece, ustring, 0, ustring->val->length()));
	} else {
		do {
			add_next_index_zval(pieces, php_ustring_zval_copy_ex(&piece, ustring, start, pos - start));
			start = pos + delimLength;
		} while ((pos = ustring->val->indexOf(delim, start)) != -1 && --limit > 1);

		if (start <= ustring->val->length()) {
			add_next_index_zval(pieces, php_ustring_zval_copy_ex(&piece, ustring, start, ustring->val->length() - start));
		}
	}

	return pieces;
}

static inline zend_string* _php_ustring_getCodepage(zval *that TSRMLS_DC) {
    return (php_ustring_fetch(that))->codepage;
}

static inline HashTable* _php_ustring_debug(zval *that, int *is_temp TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);
    HashTable *info = NULL;
    int32_t length = ustring->val->length(), 
            position = 0;

    if (length) {
        ALLOC_HASHTABLE(info);
        zend_hash_init
            (info, length, NULL, ZVAL_PTR_DTOR, 0);

        while (position < length) {
            int32_t wanted = ustring->val->extract
		        (position, 1, NULL, ustring->codepage->val);
		    zval zchar;

		    Z_STR(zchar) = zend_string_alloc(wanted, 0);

		    ustring->val->extract(
		        position, 
		        1, (char*) Z_STRVAL(zchar), wanted, 
		        ustring->codepage->val);

		    Z_STRVAL(zchar)[Z_STRLEN(zchar)] = 0;
		    Z_TYPE_INFO(zchar) = IS_STRING_EX;

		    zend_hash_next_index_insert(info, &zchar);

		    position++;
        }
    }

    *is_temp = 1;

    return info;    
}

static inline void _php_ustring_initialize(zend_class_entry **pce TSRMLS_DC) {
    (*pce)->create_object = _php_ustring_create;
	(*pce)->get_iterator =  _php_ustring_iterator;

	memcpy(
		&php_ustring_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));

	php_ustring_handlers.free_obj = _php_ustring_free;
	php_ustring_handlers.do_operation = _php_ustring_operate;
	php_ustring_handlers.cast_object = _php_ustring_cast;
	php_ustring_handlers.read_property = _php_ustring_read_property;
	php_ustring_handlers.write_property = _php_ustring_write_property;
	php_ustring_handlers.has_property = _php_ustring_has_property;
	php_ustring_handlers.unset_property = _php_ustring_unset_property;
	php_ustring_handlers.read_dimension = _php_ustring_read;
	php_ustring_handlers.compare_objects = _php_ustring_compare;
	php_ustring_handlers.get_debug_info = _php_ustring_debug;
	php_ustring_handlers.offset   = XtOffsetOf(php_ustring_t, std);
}

php_ustring_backend_t php_ustring_defaults = {
    _php_ustring_construct,
    _php_ustring_startsWith,
    _php_ustring_endsWith,
    _php_ustring_indexOf,
    _php_ustring_lastIndexOf,
    _php_ustring_toLower,
    _php_ustring_toUpper,
    _php_ustring_reverse,
    _php_ustring_trim,
    _php_ustring_replace,
    _php_ustring_replaceSlice,
    _php_ustring_charAt,
    _php_ustring_substring,
    _php_ustring_contains,
    _php_ustring_chunk,
    _php_ustring_repeat,
	_php_ustring_pad,
    _php_ustring_split,
    _php_ustring_getCodepage,
    _php_ustring_compare,
    _php_ustring_debug,
    _php_ustring_initialize,
};

#endif
