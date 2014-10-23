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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "unicode/unistr.h"

extern "C" {
#    include "php.h"
#    include "php_ini.h"
#    include "ext/standard/info.h"
#    include "php_ustring.h"
#    include "api.h"
}

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_DECLARE_MODULE_GLOBALS(ustring);

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

PHP_USTRING_API zend_class_entry *ce_UString;

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

static inline zval *php_ustring_zval_copy(zval *target, php_ustring_t *source, int32_t offset, int32_t length TSRMLS_DC)
{
	object_init_ex(target, ce_UString);

	php_ustring_copy_ex(php_ustring_fetch(target), source, offset, length);

	return target;
}

static inline int php_ustring_from_zval(UnicodeString *target, zval *source, zend_string *codepage TSRMLS_DC)
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

static inline void php_ustring_free(zend_object *zobject TSRMLS_DC) {
	php_ustring_t *ustring = (php_ustring_t*)((char*)(zobject) - XtOffsetOf(php_ustring_t, std));

	zend_object_std_dtor(&ustring->std TSRMLS_CC);

	if (ustring->codepage) {
	    zend_string_release(ustring->codepage);
	}

	delete ustring->val;
}

static inline zend_object* php_ustring_create(zend_class_entry *ce TSRMLS_DC) {
	php_ustring_t *ustring =
		(php_ustring_t*) ecalloc(1, sizeof(php_ustring_t));

	zend_object_std_init(&ustring->std, ce TSRMLS_CC);

	ustring->std.handlers = &php_ustring_handlers;

	return &ustring->std;
} /* }}} */

/* {{{ */
static inline zval* php_ustring_read(zval *object, zval *offset, int type, zval *rv TSRMLS_DC) {
	zend_bool clean = 0;

	if (Z_TYPE_P(offset) != IS_LONG) {
		convert_to_long(offset);
		clean = 1;
	}

	php_ustring_zval_copy(rv, php_ustring_fetch(object), Z_LVAL_P(offset), 1 TSRMLS_CC);

	if (clean) {
		zval_ptr_dtor(offset);
	}

	return rv;
} /* }}} */

/* {{{ */
static inline void php_ustring_iterator_dtor(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit = (php_ustring_iterator_t*) iterator;

	zval_ptr_dtor(&uit->zobject);

	if (Z_TYPE(uit->zdata) != IS_UNDEF) {
		zval_ptr_dtor(&uit->zdata);
		ZVAL_UNDEF(&uit->zdata);
	}

	zend_iterator_dtor(iterator TSRMLS_CC);
} /* }}} */

/* {{{ */
static inline int php_ustring_iterator_validate(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;
	php_ustring_t *pstring = php_ustring_fetch(&uit->zobject);

	return (uit->position < pstring->val->length()) ? SUCCESS : FAILURE;
} /* }}} */

/* {{{ */
static inline zval* php_ustring_iterator_current_data(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;
	php_ustring_t *ustring,
				  *pstring = php_ustring_fetch(&uit->zobject);

	object_init_ex(&uit->zdata, ce_UString);

	php_ustring_copy_ex(php_ustring_fetch(&uit->zdata), pstring, uit->position, 1);

	return &uit->zdata;
} /* }}} */

/* {{{ */
static inline void php_ustring_iterator_current_key(zend_object_iterator* iterator, zval *key TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;

	ZVAL_LONG(key, uit->position);
} /* }}} */

/* {{{ */
static inline void php_ustring_iterator_move_forward(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;

	uit->position++;
} /* }}} */

/* {{{ */
zend_object_iterator_funcs php_ustring_iterator_funcs = {
	php_ustring_iterator_dtor,
	php_ustring_iterator_validate,
	php_ustring_iterator_current_data,
	php_ustring_iterator_current_key,
	php_ustring_iterator_move_forward,
	NULL
}; /* }}} */

/* {{{ */
static inline zend_object_iterator* php_ustring_iterator(zend_class_entry *ce, zval *zobject, int by_ref TSRMLS_DC) {
	php_ustring_iterator_t *uit = (php_ustring_iterator_t*) emalloc(sizeof(php_ustring_iterator_t));

	zend_iterator_init((zend_object_iterator*)uit TSRMLS_CC);

	uit->zit.funcs = &php_ustring_iterator_funcs;
	uit->position = 0;

	ZVAL_COPY(&uit->zobject, zobject);
	ZVAL_UNDEF(&uit->zdata);

	return (zend_object_iterator*) uit;
} /* }}} */

static inline int php_ustring_cast(zval *zread, zval *zwrite, int type TSRMLS_DC) {
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

static zval *php_ustring_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv TSRMLS_DC) 
{
	/* length property doesn't actually exist */
	if (Z_TYPE_P(member) == IS_STRING && strcmp(Z_STRVAL_P(member), "length") == 0) {
	        if (type != BP_VAR_IS && type != BP_VAR_R) {
        	        php_error(E_ERROR, "Retrieval of length property for modification is unsupported" TSRMLS_CC);
        	}
		ZVAL_LONG(rv, php_ustring_fetch(object)->val->length());
		return rv;
	}
	
	return std_object_handlers.read_property(object, member, type, cache_slot, rv TSRMLS_CC);
}

static void php_ustring_write_property(zval *object, zval *member, zval *value, void **cache_slot TSRMLS_DC) 
{
       /* length property doesn't actually exist */
       if (Z_TYPE_P(member) == IS_STRING && strcmp(Z_STRVAL_P(member), "length") == 0) {
		php_error(E_ERROR, "Writing to length property is unsupported" TSRMLS_CC);
	}
	
	return std_object_handlers.write_property(object, member, value, cache_slot TSRMLS_CC);
}

static int php_ustring_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot TSRMLS_DC) 
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

static void php_ustring_unset_property(zval *object, zval *member, void **cache_slot TSRMLS_DC) 
{
       /* length property doesn't actually exist */
       if (Z_TYPE_P(member) == IS_STRING && strcmp(Z_STRVAL_P(member), "length") == 0) {
		php_error(E_ERROR, "length property cannot be unset" TSRMLS_CC);
	}
	
	return std_object_handlers.unset_property(object, member, cache_slot TSRMLS_CC);
}

static inline int php_ustring_operate(zend_uchar opcode, zval *result, zval *op1, zval *op2 TSRMLS_DC) {
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

PHP_USTRING_API void php_ustring_construct(zval *that, zend_string *value, zend_string *codepage TSRMLS_DC) {
   php_ustring_t* ustring;
    
    if (Z_TYPE_P(that) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(that), ce_UString TSRMLS_CC)) {
        return;
    }

    ustring = php_ustring_fetch(that);
 
    if (!codepage) {
        ustring->codepage = zend_string_copy(UG(codepage));
    } else {
		ustring->codepage = zend_string_copy(codepage);
	}
    
    ustring->val = new UnicodeString(value->val, value->len, ustring->codepage->val);
}

PHP_USTRING_API bool php_ustring_startsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;

	if (php_ustring_from_zval(&uneedle, needle, ustring->codepage TSRMLS_CC) == FAILURE) {
		return false;
	}

    return ustring->val->startsWith(uneedle);
}

PHP_USTRING_API bool php_ustring_endsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;

	if (php_ustring_from_zval(&uneedle, needle, ustring->codepage TSRMLS_CC) == FAILURE) {
		return false;
	}

    return ustring->val->endsWith(uneedle);
}

PHP_USTRING_API int32_t php_ustring_indexOf(zval *that, zval *needle, int32_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else {
		h = *ustring->val;
	}

	if (php_ustring_from_zval(&n, needle, ustring->codepage TSRMLS_CC) == FAILURE) {
		return -1;
	}

    return h.indexOf(n);
}

PHP_USTRING_API int32_t php_ustring_lastIndexOf(zval *that, zval *needle, int32_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else {
		h = *ustring->val;
	}

	if (php_ustring_from_zval(&n, needle, ustring->codepage TSRMLS_CC) == FAILURE) {
		return -1;
	}

    return h.lastIndexOf(n);
}

PHP_USTRING_API zval* php_ustring_toLower(zval *that, zval *lowered TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(lowered, ce_UString);

	ostring = php_ustring_copy(php_ustring_fetch(lowered), php_ustring_fetch(that));
	ostring->val->toLower();
   
    return lowered;
}

PHP_USTRING_API zval* php_ustring_toUpper(zval *that, zval *uppered TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(uppered, ce_UString);

	ostring = php_ustring_copy(php_ustring_fetch(uppered), php_ustring_fetch(that));
    ostring->val->toUpper();
    
    return uppered;
}

PHP_USTRING_API zval* php_ustring_reverse(zval *that, zval *reversed TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(reversed, ce_UString);

    ostring = php_ustring_copy(php_ustring_fetch(reversed), php_ustring_fetch(that));
    ostring->val->reverse();

    return reversed;
}

PHP_USTRING_API zval* php_ustring_trim(zval *that, zval *trimmed TSRMLS_DC) {
    php_ustring_t *ostring;
    
    object_init_ex(trimmed, ce_UString);

    ostring = php_ustring_copy(php_ustring_fetch(trimmed), php_ustring_fetch(that));
    ostring->val->trim();
    
    return trimmed;
}

PHP_USTRING_API zval* php_ustring_replace(zval *that, zval *search, zval *replace, zval *replaced TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that), *ostring;
	UnicodeString s, r;

	if (php_ustring_from_zval(&s, search, ustring->codepage TSRMLS_CC) == FAILURE ||
			php_ustring_from_zval(&r, replace, ustring->codepage TSRMLS_CC) == FAILURE) {
		return NULL;
	}
    
    object_init_ex(replaced, ce_UString);
    
    ostring = php_ustring_copy(php_ustring_fetch(replaced), ustring);
	ostring->val->findAndReplace(s, r);
	
	return replaced;
}

PHP_USTRING_API zval* php_ustring_replaceSlice(zval *that, zval *slice, int32_t offset, int32_t length, zval *replaced TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that), *ostring;
    UnicodeString s;

	if (php_ustring_from_zval(&s, slice, ustring->codepage TSRMLS_CC) == FAILURE) {
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

PHP_USTRING_API zval* php_ustring_charAt(zval *that, int32_t offset, zval *found TSRMLS_DC) {
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

PHP_USTRING_API zval* php_ustring_substring(zval *that, int32_t offset, int32_t length, zval *sub TSRMLS_DC) {
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

PHP_USTRING_API bool php_ustring_contains(zval *that, zval *text TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString t;

	if (php_ustring_from_zval(&t, text, ustring->codepage TSRMLS_CC) == FAILURE) {
		return false;
	}
    
    return (ustring->val->indexOf(t) >= 0);
}

PHP_USTRING_API zval* php_ustring_chunk(zval *that, int32_t length, zval *chunks TSRMLS_DC) {
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

PHP_USTRING_API zval* php_ustring_repeat(zval *that, int32_t count, zval *repeated TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that), 
                  *urepeat;
    int32_t current = 0;
    
    object_init_ex(repeated, ce_UString);
    
    urepeat = php_ustring_copy(php_ustring_fetch(repeated), ustring);
    
    while (++current < count) {
        urepeat->val->append((*ustring->val));
    }
    
    return repeated;
}

PHP_USTRING_API zval* php_ustring_pad(zval *that, int32_t targetLength, zval *pad, int mode, zval *result TSRMLS_DC) {
	php_ustring_t *ustring = php_ustring_fetch(that),
	              *ostring;
	UnicodeString padString;
	int32_t       sourceLength = ustring->val->length();
	int32_t       padLength, leftPadLength, rightPadLength, padStringLength, i;

	if (php_ustring_from_zval(&padString, pad, ustring->codepage TSRMLS_CC) == FAILURE) {
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

PHP_USTRING_API zval* php_ustring_split(zval *that, zval *delimiter, int32_t limit, zval *pieces TSRMLS_DC) {
	php_ustring_t *ustring = php_ustring_fetch(that);
	UnicodeString delim;
	int32_t pos, start, delimLength, count;
	zval piece;

	if (php_ustring_from_zval(&delim, delimiter, ustring->codepage TSRMLS_CC) == FAILURE) {
		return NULL;
	}
	delimLength = delim.length();

	if (delimLength == 0 || limit < 0) {
		return NULL;
	}

	array_init(pieces);

	if (ustring->val->length() == 0) {
		if (limit >= 0) {
			add_next_index_zval(pieces, php_ustring_zval_copy(&piece, ustring, 0, 0 TSRMLS_CC));
		}
		return pieces;
	}

	if (limit < 0) {
		return NULL;
	} else if (limit <= 1) {
		add_next_index_zval(pieces, php_ustring_zval_copy(&piece, ustring, 0, ustring->val->length() TSRMLS_CC));
		return pieces;
	}

	start = 0; pos = ustring->val->indexOf(delim, 0);

	if (pos == -1) {
		add_next_index_zval(pieces, php_ustring_zval_copy(&piece, ustring, 0, ustring->val->length() TSRMLS_CC));
	} else {
		do {
			add_next_index_zval(pieces, php_ustring_zval_copy(&piece, ustring, start, pos - start TSRMLS_CC));
			start = pos + delimLength;
		} while ((pos = ustring->val->indexOf(delim, start)) != -1 && --limit > 1);

		if (start <= ustring->val->length()) {
			add_next_index_zval(pieces, php_ustring_zval_copy(&piece, ustring, start, ustring->val->length() - start TSRMLS_CC));
		}
	}

	return pieces;
}

PHP_USTRING_API zend_string* php_ustring_getCodepage(zval *that TSRMLS_DC) {
    return (php_ustring_fetch(that))->codepage;
}

static inline HashTable* php_ustring_debug(zval *that, int *is_temp TSRMLS_DC) {
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

PHP_USTRING_API int php_ustring_compare(zval *op1, zval *op2 TSRMLS_DC) {
    UnicodeString us1, us2;

	if (php_ustring_from_zval(&us1, op1, UG(codepage) TSRMLS_CC) == FAILURE ||
			php_ustring_from_zval(&us2, op2, UG(codepage) TSRMLS_CC) == FAILURE) {
		return 0;
	}

	return us1.compare(us2);
}

PHP_USTRING_API void php_ustring_setDefaultCodepage(zend_string *codepage TSRMLS_DC) {
    if (UG(codepage)) {
        zend_string_release(UG(codepage));
    }

	UG(codepage) = zend_string_copy(codepage);
}

PHP_USTRING_API zend_string* php_ustring_getDefaultCodepage(TSRMLS_D) {
    return UG(codepage);
}

/* {{{ proto UString UString::__contruct([string arg , [string codepage = "utf-8"]]) */
PHP_METHOD(UString, __construct)
{
	zend_string *value, *codepage = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|S", &value, &codepage) != SUCCESS) {
		return;
	}

	php_ustring_construct(getThis(), value, codepage TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool UString::startsWith(UString needle) */
PHP_METHOD(UString, startsWith) {
	zval *needle;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &needle) != SUCCESS) {
		return;
	}

	RETURN_BOOL(php_ustring_startsWith(getThis(), needle TSRMLS_CC));
} /* }}} */

/* {{{ proto bool UString::endsWith(UString needle]) */
PHP_METHOD(UString, endsWith) {
	zval *needle;
    	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &needle) != SUCCESS) {
		return;
	}
	
	RETURN_BOOL(php_ustring_endsWith(getThis(), needle TSRMLS_CC));
} /* }}} */

/* {{{ proto mixed UString::indexOf(UString needle [, int $offset]) */
PHP_METHOD(UString, indexOf) {
	zval *needle;
    zend_long offset = 0;
    int32_t index = -1;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &needle, &offset) != SUCCESS) {
		return;
	}
	
	index = php_ustring_indexOf(getThis(), needle, (int32_t)offset TSRMLS_CC);
	
	if (index < 0) {
	    RETURN_FALSE;
	}

	RETURN_LONG(index + offset);
} /* }}} */

/* {{{ proto mixed UString::lastIndexOf(UString needle [, int $offset]) */
PHP_METHOD(UString, lastIndexOf) {
	zval *needle;
    zend_long offset = 0;
    int32_t index = -1;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &needle, &offset) != SUCCESS) {
		return;
	}

	index = php_ustring_lastIndexOf(getThis(), needle, (int32_t)offset TSRMLS_CC);

	if (index < 0) {
		RETURN_FALSE;
	}

	RETURN_LONG(index + offset);
} /* }}} */

/* {{{ proto UString UString::toLower(void) */
PHP_METHOD(UString, toLower) {
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}
    
    php_ustring_toLower(getThis(), return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::toUpper(void) */
PHP_METHOD(UString, toUpper) {
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	php_ustring_toUpper(getThis(), return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::reverse(void) */
PHP_METHOD(UString, reverse) {
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

    php_ustring_reverse(getThis(), return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::trim(void) */
PHP_METHOD(UString, trim) {
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

    php_ustring_trim(getThis(), return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::replace(UString search, UString replace) */
PHP_METHOD(UString, replace) {
	zval *search, *replace;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &search, &replace) != SUCCESS) {
		return;
	}

    php_ustring_replace(getThis(), search, replace, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::replaceSlice(UString text [, int start [, int length]]) */
PHP_METHOD(UString, replaceSlice) {
    zval *text;
    zend_long start, length;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &text, &start, &length) != SUCCESS) {
		return;
	}

	php_ustring_replaceSlice(getThis(), text, (int32_t)start, (int32_t)length, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::charAt(int index) */
PHP_METHOD(UString, charAt) {
	zend_long index;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) != SUCCESS) {
		return;
	}

	php_ustring_charAt(getThis(), (int32_t)index, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::substring(int start [, int length]) */
PHP_METHOD(UString, substring) {
	zend_long start = -1, length = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &start, &length) != SUCCESS) {
		return;
	}

	php_ustring_substring(getThis(), (int32_t)start, (int32_t)length, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto bool UString::contains(UString text) */
PHP_METHOD(UString, contains) {
    zval *text;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &text) != SUCCESS) {
        return;
    }
    
    RETURN_BOOL(php_ustring_contains(getThis(), text TSRMLS_CC));
} /* }}} */

/* {{{ proto UString[] UString::chunk(int $length) */
PHP_METHOD(UString, chunk) {
    zend_long length = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) != SUCCESS) {
        return;
    }
    
    php_ustring_chunk(getThis(), length, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::repeat(int count) */
PHP_METHOD(UString, repeat) {
    zend_long count = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &count) != SUCCESS) {
        return;
    }
    
    php_ustring_repeat(getThis(), (int32_t)count, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::pad(int length, UString pad = " ", int mode = STR_PAD_RIGHT) */
PHP_METHOD(UString, pad) {
	zend_long length;
	zval*      pad = NULL;
	zend_long mode = STR_PAD_RIGHT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|zl", &length, &pad, &mode) != SUCCESS) {
		return;
	}

	if (!pad) {
		pad = &UG(defaultpad);
	}

	php_ustring_pad(getThis(), (int32_t)length, pad, (int32_t)mode, return_value TSRMLS_CC);
}

/* {{{ proto array UString::split(UString delimiter, int limit = NULL) */
PHP_METHOD(UString, split) {
    zval*      delimiter;
	zend_long limit = INT_MAX;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &delimiter, &limit) != SUCCESS) {
		return;
	}

	php_ustring_split(getThis(), delimiter, (int32_t)limit, return_value TSRMLS_CC);
}

/* {{{ proto string UString::getCodepage(void) */
PHP_METHOD(UString, getCodepage) {
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	RETURN_STR(php_ustring_getCodepage(getThis() TSRMLS_CC));
} /* }}} */

/* {{{ proto void UString::setDefaultCodepage(string codepage) */
PHP_METHOD(UString, setDefaultCodepage) {
	zend_string *codepage = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &codepage) != SUCCESS) {
		return;
	}

	php_ustring_setDefaultCodepage(codepage TSRMLS_CC);
} /* }}} */

/* {{{ proto string UString::getDefaultCodepage(void) */
PHP_METHOD(UString, getDefaultCodepage) {
    zend_string *dc;
    
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}
	
	dc = php_ustring_getDefaultCodepage(TSRMLS_C);

    RETURN_STR(zend_string_copy(dc));
} /* }}} */

/* {{{ */
ZEND_BEGIN_ARG_INFO_EX(php_ustring_no_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring__construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, codepage)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_std_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, needle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_replace_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, search)
	ZEND_ARG_INFO(0, replace)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_charAt_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_substring_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_replaceSlice_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, text)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_contains_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_chunk_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_repeat_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_pad_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, pad)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()
                                            
ZEND_BEGIN_ARG_INFO_EX(php_ustring_split_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_setDefaultCodepage_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, codepage)
ZEND_END_ARG_INFO() /* }}} */

/* {{{ */
zend_function_entry php_ustring_methods[] = {
	PHP_ME(UString, __construct, php_ustring__construct_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, startsWith, php_ustring_std_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, endsWith, php_ustring_std_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, indexOf, php_ustring_std_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, lastIndexOf, php_ustring_std_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, toLower, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, toUpper, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, reverse, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, trim, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, replace, php_ustring_replace_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, replaceSlice, php_ustring_replaceSlice_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, contains, php_ustring_contains_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, chunk, php_ustring_chunk_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, repeat, php_ustring_repeat_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, pad, php_ustring_pad_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, split, php_ustring_split_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, charAt, php_ustring_charAt_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, substring, php_ustring_substring_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, getCodepage, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, setDefaultCodepage, php_ustring_setDefaultCodepage_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(UString, getDefaultCodepage, php_ustring_no_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
}; /* }}} */

/* {{{ */
static inline void php_ustring_globals_ctor(zend_ustring_globals  *ug TSRMLS_DC) {
	ug->codepage = NULL;
} /* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ustring)
{
	zend_class_entry ce;
	zval tmp;

	INIT_CLASS_ENTRY(ce, "UString", php_ustring_methods);

	ZEND_INIT_MODULE_GLOBALS(ustring, php_ustring_globals_ctor, NULL);

	ce_UString = zend_register_internal_class(&ce TSRMLS_CC);
	ce_UString->create_object = php_ustring_create;
	ce_UString->get_iterator =  php_ustring_iterator;

	memcpy(
		&php_ustring_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));

	php_ustring_handlers.free_obj = php_ustring_free;
	php_ustring_handlers.do_operation = php_ustring_operate;
	php_ustring_handlers.cast_object = php_ustring_cast;
	php_ustring_handlers.read_property = php_ustring_read_property;
	php_ustring_handlers.write_property = php_ustring_write_property;
	php_ustring_handlers.has_property = php_ustring_has_property;
	php_ustring_handlers.unset_property = php_ustring_unset_property;
	php_ustring_handlers.read_dimension = php_ustring_read;
	php_ustring_handlers.compare_objects = php_ustring_compare;
	php_ustring_handlers.get_debug_info = php_ustring_debug;
	php_ustring_handlers.offset   = XtOffsetOf(php_ustring_t, std);
	
	/* This property is given a NULL value and declared so it'll reflect properly, but we don't actually implement it as a real property, and we instead use getters and setters */
	ZVAL_NULL(&tmp);
	zend_declare_property(ce_UString, "length", 6, &tmp, ZEND_ACC_PUBLIC TSRMLS_CC);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ustring)
{
	UG(codepage) = zend_string_init("UTF-8", sizeof("UTF-8")-1, 0);

	ZVAL_NEW_STR(&UG(defaultpad), zend_string_init(" ", 1, 0));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ustring)
{
	zend_string_release(UG(codepage));

	zval_dtor(&UG(defaultpad));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ustring)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ustring support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto UString u(string value [, string codepage]) */
PHP_FUNCTION(u) {
    zend_string *value, *codepage = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|S", &value, &codepage) != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_UString);
    
    php_ustring_construct
        (return_value, value, codepage TSRMLS_CC);
} /* }}} */

ZEND_BEGIN_ARG_INFO_EX(php_ustring_u_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, string)
    ZEND_ARG_INFO(0, codepage)
ZEND_END_ARG_INFO()

zend_function_entry php_ustring_functions[] = {
    PHP_FE(u, php_ustring_u_arginfo)
    PHP_FE_END
};

/* {{{ ustring_module_entry
 */
zend_module_entry ustring_module_entry = {
	STANDARD_MODULE_HEADER,
	"ustring",
	php_ustring_functions,
	PHP_MINIT(ustring),
	NULL,
	PHP_RINIT(ustring),
	PHP_RSHUTDOWN(ustring),
	PHP_MINFO(ustring),
	PHP_USTRING_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_USTRING
extern "C" {
	ZEND_GET_MODULE(ustring)
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
