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

#include "unicode/unistr.h"

#include "backend/api.h"

typedef struct _php_ustring_t {
	UnicodeString *val;
	zend_string   *codepage;
	zend_object   std;
} php_ustring_t;

typedef struct _php_ustring_iterator_t {
	zend_object_iterator zit;
	zval zobject;
	zval zdata;
	zend_int_t position;
} php_ustring_iterator_t;

#define php_ustring_fetch(o) ((php_ustring_t*) (((char*)Z_OBJ_P(o)) - XtOffsetOf(php_ustring_t, std)))

#define STR_PAD_LEFT            0
#define STR_PAD_RIGHT           1
#define STR_PAD_BOTH            2

zend_object_handlers php_ustring_handlers;

static inline void _php_ustring_free(zend_object *zobject TSRMLS_DC) {
	php_ustring_t *ustring = (php_ustring_t*)((char*)(zobject) - XtOffsetOf(php_ustring_t, std));

	zend_object_std_dtor(&ustring->std TSRMLS_CC);

	if (ustring->codepage) {
	    STR_RELEASE(ustring->codepage);
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
	php_ustring_t *ustring = php_ustring_fetch(object),
				  *rstring;
	zend_bool clean = 0;

	object_init_ex(rv, ce_UString);

	if (Z_TYPE_P(offset) != IS_INT) {
		convert_to_int(offset);
		clean = 1;
	}

	rstring = php_ustring_fetch(rv);
	rstring->val = new UnicodeString
		(*ustring->val, Z_IVAL_P(offset), 1);
	rstring->codepage = STR_COPY(ustring->codepage);

	if (clean)
		zval_ptr_dtor(offset);

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

	ustring = php_ustring_fetch(&uit->zdata);
	ustring->codepage =
		STR_COPY(pstring->codepage);
	ustring->val = new UnicodeString(*pstring->val, uit->position, 1);

	return &uit->zdata;
} /* }}} */

/* {{{ */
static inline void _php_ustring_iterator_current_key(zend_object_iterator* iterator, zval *key TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;

	ZVAL_INT(key, uit->position);
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
	zend_int_t length = 0;

	if (type != IS_STRING) {
		return FAILURE;
	}

	ustring = php_ustring_fetch(zread);

	length = ustring->val->extract
		(0, ustring->val->length(), NULL, length, ustring->codepage->val);

	Z_STR_P(zwrite) = STR_ALLOC(length, 0);

	ustring->val->extract(
		0,
		ustring->val->length(),
		(char*) Z_STRVAL_P(zwrite),
		(int32_t) Z_STRSIZE_P(zwrite),
		ustring->codepage->val);

	Z_STRVAL_P(zwrite)[Z_STRSIZE_P(zwrite)] = 0;
	Z_TYPE_INFO_P(zwrite) = IS_STRING_EX;
    
	return SUCCESS;
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
						uresult->val->append(UnicodeString(Z_STRVAL_P(op1), (int32_t) Z_STRSIZE_P(op1)));
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
					uresult->val->append(UnicodeString(Z_STRVAL_P(op2), (int32_t) Z_STRSIZE_P(op2)));
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

static inline void _php_ustring_construct(zval *that, const char *value, zend_size_t vlen, const char *codepage, zend_size_t clen TSRMLS_DC) {
    php_ustring_t* ustring;
    
    if (Z_TYPE_P(that) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(that), ce_UString TSRMLS_CC)) {
        return;
    }
    
    ustring = php_ustring_fetch(that);
    
    if (!codepage) {
        ustring->codepage = STR_COPY(UG(codepage));
    } else ustring->codepage = STR_INIT(codepage, clen, 0);
    
    ustring->val = new UnicodeString(value, vlen, ustring->codepage->val);
}

static inline zend_size_t _php_ustring_length(zval *that TSRMLS_DC) {
    return (php_ustring_fetch(that))->val->length();
}

static inline bool _php_ustring_startsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    uneedle = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    uneedle = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return false;
	}
	
    return ustring->val->startsWith(uneedle);
}

static inline bool _php_ustring_endsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    uneedle = UnicodeString(Z_STRVAL_P(needle), (int32_t)Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    uneedle = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return false;
	}
	
    return ustring->val->endsWith(uneedle);
}

static inline zend_int_t _php_ustring_indexOf(zval *that, zval *needle, zend_int_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else h = *ustring->val;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    if (offset)
		        n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		    else n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    n = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return -1;
	}
	
    return h.indexOf(n);
}

static inline zend_int_t _php_ustring_lastIndexOf(zval *that, zval *needle, zend_int_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else h = *ustring->val;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    if (offset)
		        n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		    else n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    n = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return -1;
	}
	
    return h.lastIndexOf(n);
}

static inline zval* _php_ustring_toLower(zval *that, zval *lowered TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(lowered, ce_UString);
    
    ostring = php_ustring_fetch(lowered);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->toLower();
    ostring->codepage = STR_COPY(ustring->codepage);
    
    return lowered;
}

static inline zval* _php_ustring_toUpper(zval *that, zval *uppered TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(uppered, ce_UString);
    
    ostring = php_ustring_fetch(uppered);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->toUpper();
    ostring->codepage = STR_COPY(ustring->codepage);
    
    return uppered;
}

static inline zval* _php_ustring_reverse(zval *that, zval *reversed TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(reversed, ce_UString);
    
    ostring = php_ustring_fetch(reversed);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->reverse();
    ostring->codepage = STR_COPY(ustring->codepage);

    return reversed;
}

static inline zval* _php_ustring_trim(zval *that, zval *trimmed TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(trimmed, ce_UString);
    
    ostring = php_ustring_fetch(trimmed);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->trim();
    ostring->codepage = STR_COPY(ustring->codepage);
    
    return trimmed;
}

static inline zval* _php_ustring_replace(zval *that, zval *search, zval *replace, zval *replaced TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
                  
     UnicodeString s, r;
    
	switch (Z_TYPE_P(search)) {
		case IS_STRING: {
			s = UnicodeString(Z_STRVAL_P(search), (int32_t) Z_STRSIZE_P(search), ustring->codepage->val);

			switch(Z_TYPE_P(replace)) {
				case IS_STRING:
					r = UnicodeString(Z_STRVAL_P(replace), (int32_t) Z_STRSIZE_P(replace), ustring->codepage->val);
				break;

				case IS_OBJECT: {
					r = *(php_ustring_fetch(replace))->val;
				} break;

				default:
					return NULL;
			}
		}
		break;

		case IS_OBJECT: {
			s = *(php_ustring_fetch(search))->val;

			switch(Z_TYPE_P(replace)) {
				case IS_OBJECT:
					r = *(php_ustring_fetch(replace))->val;
				break;

				case IS_STRING:
					r = UnicodeString(Z_STRVAL_P(replace), (int32_t) Z_STRSIZE_P(replace), ustring->codepage->val);
				break;

				default:
					return NULL;
			}
		}
		break;

		default:
			return NULL;
	}
    
    object_init_ex(replaced, ce_UString);
    
    ostring = php_ustring_fetch(replaced);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->codepage = STR_COPY(ustring->codepage);
	ostring->val->findAndReplace(s, r);
	
	return replaced;
}

static inline zval* _php_ustring_replaceSlice(zval *that, zval *slice, zend_int_t offset, zend_int_t length, zval *replaced TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;

    UnicodeString s;
    
    switch (Z_TYPE_P(slice)) {
		case IS_STRING:
			s = UnicodeString(Z_STRVAL_P(slice), (int32_t) Z_STRSIZE_P(slice), ustring->codepage->val);
			break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(slice), ce_UString TSRMLS_CC)) {
				return NULL;
			}

			s = *(php_ustring_fetch(slice))->val;
			break;

		default:
			return NULL;
	}

	if (offset == -1)
		offset = 0;
	if (length == -1)
		length = s.length() - offset;
    
    object_init_ex(replaced, ce_UString);
    
    ostring = php_ustring_fetch(replaced);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->codepage = STR_COPY(ustring->codepage);
    ostring->val->replace(offset, length, s, 0, s.length());
    
    return replaced;
}

static inline zval* _php_ustring_charAt(zval *that, zend_int_t offset, zval *found TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring = NULL;

    UChar c = ustring->val->charAt(offset);

	if (c) {
		object_init_ex(found, ce_UString);

		ostring = php_ustring_fetch(found);
		ostring->val = new UnicodeString(&c, 1);
		ostring->codepage = STR_COPY(ustring->codepage);
	}
	
	return found;
}

static inline zval* _php_ustring_substring(zval *that, zend_int_t offset, zend_int_t length, zval *sub TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
				  *ostring;
    
	if (offset < 0) {
	    return NULL;
	}
	
	if (length == -1)
	    length = ustring->val->length() - offset;
	    
	object_init_ex(sub, ce_UString);
	
	ostring = php_ustring_fetch(sub);
	ostring->codepage = STR_COPY(ustring->codepage);
	ostring->val = new UnicodeString(*ustring->val, offset, length);
	
	return sub;
}

static inline bool _php_ustring_contains(zval *that, zval *text TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString t;
    
    switch (Z_TYPE_P(text)) {
        case IS_STRING:
            t = UnicodeString(Z_STRVAL_P(text), (int32_t) Z_STRSIZE_P(text), ustring->codepage->val);
        break;
        
        case IS_OBJECT:
            if (!instanceof_function(Z_OBJCE_P(text), ce_UString TSRMLS_CC)) {
                return false;
            }
            
            t = *(php_ustring_fetch(text))->val;
        break;
    }
    
    return (ustring->val->indexOf(t) >= 0);
}

static inline zval* _php_ustring_chunk(zval *that, zend_int_t length, zval *chunks TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);
    zend_int_t position = 0;
    
    array_init(chunks);
    
    if (!length) 
        length = 1;
    
    while (position < ustring->val->length()) {
        zval chunked;
        php_ustring_t *uchunk;
        
        object_init_ex(&chunked, ce_UString);
        
        uchunk = php_ustring_fetch(&chunked);
        uchunk->codepage = STR_COPY(ustring->codepage);
        uchunk->val = new UnicodeString(*ustring->val, position, length);
        
        add_next_index_zval(chunks, &chunked);
        
        position += length;
    }
    
    return chunks;
}

static inline zval* _php_ustring_repeat(zval *that, zend_int_t count, zval *repeated TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(getThis()), 
                  *urepeat;
    zend_int_t current = 0;
    
    object_init_ex(repeated, ce_UString);
    
    urepeat = php_ustring_fetch(repeated);
    urepeat->codepage = STR_COPY(ustring->codepage);
    urepeat->val = new UnicodeString(*ustring->val);
    
    while (++current < count) {
        urepeat->val->append((*ustring->val));
    }
    
    return repeated;
}

static inline int _php_ustring_compare(zval *op1, zval *op2 TSRMLS_DC) {
	UnicodeString us1, us2;

	switch (Z_TYPE_P(op1)) {
		case IS_STRING:
			us1 = UnicodeString(Z_STRVAL_P(op1), (int32_t) Z_STRSIZE_P(op1), UG(codepage)->val);
		break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(op1), ce_UString TSRMLS_CC)) {
				return 0;
			}

			us1 = *(php_ustring_fetch(op1))->val;
		break;

		default:
			return 0;
	}

	switch (Z_TYPE_P(op2)) {
		case IS_STRING:
			us2 = UnicodeString(Z_STRVAL_P(op2), (int32_t) Z_STRSIZE_P(op2), UG(codepage)->val);
		break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(op2), ce_UString TSRMLS_CC)) {
				return 0;
			}

			us2 = *(php_ustring_fetch(op2))->val;
		break;

		default:
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

	switch (Z_TYPE_P(pad)) {
		case IS_STRING:
			padString = UnicodeString(Z_STRVAL_P(pad), (int32_t)Z_STRSIZE_P(pad), UG(codepage)->val);
			break;

		case IS_OBJECT:
			padString = *(php_ustring_fetch(pad))->val;
			break;

		default:
			return NULL;
	}
    padStringLength = padString.length();

	object_init_ex(result, ce_UString);

	ostring = php_ustring_fetch(result);
	ostring->codepage = STR_COPY(ustring->codepage);

	if (padStringLength < 1 || targetLength < 0 || targetLength <= sourceLength) {
		ostring->val = new UnicodeString(*ustring->val);
		return result;
	}

	if (padStringLength == 1 && mode != STR_PAD_BOTH) {
		ostring->val = new UnicodeString(*ustring->val);

		if (mode == STR_PAD_LEFT) {
			ostring->val->padLeading(targetLength, padString.charAt(0));
		} else {
			ostring->val->padTrailing(targetLength, padString.charAt(0));
		}

		return result;
	}

	ostring->val = new UnicodeString();
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
    UnicodeString text = *ustring->val;
    UnicodeString delim;
    int32_t pos, delimLength, count;
    
    if (limit < 0) {
        return NULL;
    }
    
	switch (Z_TYPE_P(delimiter)) {
		case IS_STRING:
			delim = UnicodeString(Z_STRVAL_P(delimiter), (int32_t)Z_STRSIZE_P(delimiter), UG(codepage)->val);
			break;

		case IS_OBJECT:
			delim = *(php_ustring_fetch(delimiter))->val;
			break;

		default:
			return NULL;
	}
    delimLength = delim.length();
    
    array_init(pieces);
    
    count = 0;
    while ((pos = text.indexOf(delim)) != -1) {
        zval piece;
        php_ustring_t *upiece;
        
        object_init_ex(&piece, ce_UString);
        
        upiece = php_ustring_fetch(&piece);
        upiece->codepage = STR_COPY(ustring->codepage);
        upiece->val = new UnicodeString(text, 0, pos);
        
        add_next_index_zval(pieces, &piece);
        
        text.remove(0, pos + delimLength);
        
        if (limit && ++count == limit) {
            break;
        }
    }
    
    zval piece;
    php_ustring_t *upiece;
    
    object_init_ex(&piece, ce_UString);
    
    upiece = php_ustring_fetch(&piece);
    upiece->codepage = STR_COPY(ustring->codepage);
    upiece->val = new UnicodeString(text);
    
    add_next_index_zval(pieces, &piece);

    return pieces;
}

static inline zend_string* _php_ustring_getCodepage(zval *that TSRMLS_DC) {
    return (php_ustring_fetch(that))->codepage;
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
	php_ustring_handlers.read_dimension = _php_ustring_read;
	php_ustring_handlers.compare_objects = _php_ustring_compare;
	php_ustring_handlers.offset   = XtOffsetOf(php_ustring_t, std);
}

php_ustring_backend_t php_ustring_defaults = {
    _php_ustring_construct,
    _php_ustring_length,
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
    _php_ustring_initialize,
};

#endif
