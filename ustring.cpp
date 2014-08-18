/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
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
}

#include "php_ustring.h"

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

#define PHP_USTRING_FETCH(o) (php_ustring_t*) (((char*)Z_OBJ_P(o)) - XtOffsetOf(php_ustring_t, std))

zend_class_entry *ce_UString;

zend_object_handlers php_ustring_handlers;

/* {{{ proto UString UString::__contruct([string arg , [string codepage = "utf-8"]]) */
PHP_METHOD(UString, __construct)
{
	char *val = NULL, 
	     *codepage = NULL;
	int32_t vlen = 0, 
	        clen = 0;

	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());

	if (!ZEND_NUM_ARGS()) {
	    ustring->val = new UnicodeString();
	    ustring->codepage =
	        STR_COPY(UG(codepage));
	    return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &val, &vlen, &codepage, &clen) != SUCCESS) {
		return;
	}

	if (codepage) {
		ustring->codepage = STR_INIT(codepage, clen, 0);
	} else {
		ustring->codepage = STR_COPY(UG(codepage));
	}

	ustring->val = new UnicodeString(val, vlen, ustring->codepage->val);
}
/* }}} */

/* {{{ proto int UString::length(void) */
PHP_METHOD(UString, length) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());

	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	RETURN_LONG(ustring->val->length());
} /* }}} */

/* {{{ proto bool UString::startsWith(UString needle) */
PHP_METHOD(UString, startsWith) {
	zval *zneedle;
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    UnicodeString needle;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zneedle) != SUCCESS) {
		return;
	}

	switch (Z_TYPE_P(zneedle)) {
		case IS_STRING: 
		    needle = UnicodeString(Z_STRVAL_P(zneedle), (int32_t)Z_STRLEN_P(zneedle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    needle = *(PHP_USTRING_FETCH(zneedle))->val;
		break;
		
		default:
		    return;
	}
	
	RETURN_BOOL(ustring->val->startsWith(needle));
} /* }}} */

/* {{{ proto bool UString::endsWith(UString needle]) */
PHP_METHOD(UString, endsWith) {
	zval *zneedle;
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    UnicodeString needle;
    	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zneedle) != SUCCESS) {
		return;
	}
    
	switch (Z_TYPE_P(zneedle)) {
		case IS_STRING:
		    needle = UnicodeString(Z_STRVAL_P(zneedle), (int32_t) Z_STRLEN_P(zneedle), ustring->codepage->val);
	    break;

		case IS_OBJECT:
		    needle = *(PHP_USTRING_FETCH(zneedle))->val;
		break;
		
		default:
		    return;
	}
	
	RETURN_BOOL(ustring->val->endsWith(needle));
} /* }}} */

/* {{{ proto mixed UString::indexOf(UString needle [, int $offset]) */
PHP_METHOD(UString, indexOf) {
	zval *zneedle;
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    UnicodeString haystack, needle;
    long offset = 0;
    int32_t index = -1;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &zneedle, &offset) != SUCCESS) {
		return;
	}
	
	haystack = (offset > 0) ?
	    UnicodeString(*ustring->val, (int32_t) offset) : *ustring->val;

	switch (Z_TYPE_P(zneedle)) {
		case IS_STRING:
            needle = UnicodeString(Z_STRVAL_P(zneedle), (int32_t) Z_STRLEN_P(zneedle), ustring->codepage->val);
        break;
        
		case IS_OBJECT:
		    needle = *(PHP_USTRING_FETCH(zneedle))->val;
		break;
		
		default:
		    return;
	}
	
	index = haystack.indexOf(needle);
	
	if (index < 0)
	    RETURN_FALSE;
	
	RETURN_LONG(index + offset);
} /* }}} */

/* {{{ proto mixed UString::lastIndexOf(UString needle [, int $offset]) */
PHP_METHOD(UString, lastIndexOf) {
	zval *zneedle;
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    UnicodeString haystack, needle;
    long offset = 0;
    int32_t index = -1;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &zneedle, &offset) != SUCCESS) {
		return;
	}

    haystack = (offset > 0) ?   
        UnicodeString(*ustring->val, (int32_t) offset) : *ustring->val;
    
	switch (Z_TYPE_P(zneedle)) {
		case IS_STRING:
		    needle = UnicodeString(Z_STRVAL_P(zneedle), (int32_t) Z_STRLEN_P(zneedle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    needle = *(PHP_USTRING_FETCH(zneedle))->val;
		break;
		
		default:
		    return;
	}
	
	index = haystack.lastIndexOf(needle);
	
	if (index < 0)
	    RETURN_FALSE;
	
	RETURN_LONG(index + offset);
} /* }}} */

/* {{{ proto UString UString::toLower(void) */
PHP_METHOD(UString, toLower) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    php_ustring_t *ostring;
    
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}
    
    object_init_ex(return_value, ce_UString);
    
    ostring = PHP_USTRING_FETCH(return_value);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->toLower();
    ostring->codepage = STR_COPY(ustring->codepage);
} /* }}} */

/* {{{ proto UString UString::toUpper(void) */
PHP_METHOD(UString, toUpper) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    php_ustring_t *ostring;
    
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	object_init_ex(return_value, ce_UString);
    
    ostring = PHP_USTRING_FETCH(return_value);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->toUpper();
    ostring->codepage = STR_COPY(ustring->codepage);
} /* }}} */

/* {{{ proto UString UString::reverse(void) */
PHP_METHOD(UString, reverse) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    php_ustring_t *ostring;
    
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

    object_init_ex(return_value, ce_UString);
    
    ostring = PHP_USTRING_FETCH(return_value);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->reverse();
    ostring->codepage = STR_COPY(ustring->codepage);
} /* }}} */

/* {{{ proto UString UString::trim(void) */
PHP_METHOD(UString, trim) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    php_ustring_t *ostring;
    
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

    object_init_ex(return_value, ce_UString);
    
    ostring = PHP_USTRING_FETCH(return_value);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->trim();
    ostring->codepage = STR_COPY(ustring->codepage);
} /* }}} */

/* {{{ proto UString UString::replace(UString search, UString replace) */
PHP_METHOD(UString, replace) {
	zval *zsearch, *zreplace;
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
	php_ustring_t *ostring;
	UnicodeString search, replace;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zsearch, &zreplace) != SUCCESS) {
		return;
	}

	switch (Z_TYPE_P(zsearch)) {
		case IS_STRING: {
			search = UnicodeString(Z_STRVAL_P(zsearch), (int32_t) Z_STRLEN_P(zsearch), ustring->codepage->val);

			switch(Z_TYPE_P(zreplace)) {
				case IS_STRING:
					replace = UnicodeString(Z_STRVAL_P(zreplace), (int32_t) Z_STRLEN_P(zreplace), ustring->codepage->val);
				break;

				case IS_OBJECT: {
					replace = *(PHP_USTRING_FETCH(zreplace))->val;
				} break;

				default:
					return;
			}
		}
		break;

		case IS_OBJECT: {
			search = *(PHP_USTRING_FETCH(zsearch))->val;

			switch(Z_TYPE_P(zreplace)) {
				case IS_OBJECT:
					replace = *(PHP_USTRING_FETCH(zreplace))->val;
				break;

				case IS_STRING:
					replace = UnicodeString(Z_STRVAL_P(zreplace), (int32_t) Z_STRLEN_P(zreplace), ustring->codepage->val);
				break;

				default:
					return;
			}
		}
		break;

		default:
			return;
	}
    
    object_init_ex(return_value, ce_UString);
    
    ostring = PHP_USTRING_FETCH(return_value);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->codepage = STR_COPY(ustring->codepage);
	ostring->val->findAndReplace(search, replace);
} /* }}} */

/* {{{ proto UString UString::charAt(int index) */
PHP_METHOD(UString, charAt) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
				  *uchar;
	int32_t index = 0;
	UChar found;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) != SUCCESS) {
		return;
	}

	found = ustring->val->charAt(index);

	if (found) {
		object_init_ex(return_value, ce_UString);

		uchar = PHP_USTRING_FETCH(return_value);
		uchar->val = new UnicodeString(&found);
		uchar->codepage = STR_COPY(ustring->codepage);
	}
} /* }}} */

/* {{{ proto UString UString::substring(int start [, int length]) */
PHP_METHOD(UString, substring) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
				  *ostring;
	long start = -1,
	     length = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &start, &length) != SUCCESS) {
		return;
	}

	if (start < 0)
	    RETURN_FALSE;
	    
	if (length == -1)
	    length = ustring->val->length() - start;
	    
	object_init_ex(return_value, ce_UString);
	
	ostring = PHP_USTRING_FETCH(return_value);
	ostring->codepage = STR_COPY(ustring->codepage);
	ostring->val = new UnicodeString(*ustring->val, (int32_t) start, (int32_t) length);
} /* }}} */

/* {{{ proto UString UString::replaceSlice(UString text [, int start [, int length]]) */
PHP_METHOD(UString, replaceSlice) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
	php_ustring_t *ostring;
	long start    = -1,
		 length   = -1;
	zval    *ztext;
	UnicodeString utext;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &ztext, &start, &length) != SUCCESS) {
		return;
	}

	switch (Z_TYPE_P(ztext)) {
		case IS_STRING:
			utext = UnicodeString(Z_STRVAL_P(ztext), (int32_t) Z_STRLEN_P(ztext), ustring->codepage->val);
			break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(ztext), ce_UString TSRMLS_CC)) {
				return;
			}

			utext = *(PHP_USTRING_FETCH(ztext))->val;
			break;

		default:
			return;
	}

	if (start == -1)
		start = 0;
	if (length == -1)
		length = utext.length() - start;
    
    object_init_ex(return_value, ce_UString);
    
    ostring = PHP_USTRING_FETCH(return_value);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->codepage = STR_COPY(ustring->codepage);
    ostring->val->replace(start, length, utext, 0, utext.length());
} /* }}} */

/* {{{ proto bool UString::contains(UString text) */
PHP_METHOD(UString, contains) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    zval *ztext;
    UnicodeString utext;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ztext) != SUCCESS) {
        return;
    }
    
    switch (Z_TYPE_P(ztext)) {
        case IS_STRING:
            utext = UnicodeString(Z_STRVAL_P(ztext), (int32_t) Z_STRLEN_P(ztext), ustring->codepage->val);
        break;
        
        case IS_OBJECT:
            if (!instanceof_function(Z_OBJCE_P(ztext), ce_UString TSRMLS_CC)) {
                return;
            }
            
            utext = *(PHP_USTRING_FETCH(ztext))->val;
        break;
    }
    
    RETURN_BOOL(ustring->val->indexOf(utext) >= 0);
} /* }}} */

/* {{{ proto UString[] UString::chunk(int $length) */
PHP_METHOD(UString, chunk) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    long length = 0;
    int32_t position = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) != SUCCESS) {
        return;
    }
    
    array_init(return_value);
    
    if (!length) 
        length = 1;
    
    while (position < ustring->val->length()) {
        zval chunked;
        php_ustring_t *uchunk;
        
        object_init_ex(&chunked, ce_UString);
        
        uchunk = PHP_USTRING_FETCH(&chunked);
        uchunk->codepage = STR_COPY(ustring->codepage);
        uchunk->val = new UnicodeString(*ustring->val, position, (int32_t) length);
        
        add_next_index_zval(return_value, &chunked);
        
        position += length;
    }
} /* }}} */

/* {{{ proto UString UString::repeat(int count) */
PHP_METHOD(UString, repeat) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()), 
                  *urepeat;
    long count = 0, current = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &count) != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_UString);
    
    urepeat = PHP_USTRING_FETCH(return_value);
    urepeat->codepage = STR_COPY(ustring->codepage);
    urepeat->val = new UnicodeString(*ustring->val);
    
    while (++current < count) {
        urepeat->val->append((*ustring->val));
    }
} /* }}} */

/* {{{ proto void UString::setDefaultCodepage(string codepage) */
PHP_METHOD(UString, setDefaultCodepage) {
	char *codepage = NULL;
	int32_t clen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &codepage, &clen) != SUCCESS) {
		return;
	}

	STR_RELEASE(UG(codepage));

	UG(codepage) = STR_INIT(codepage, clen, 0);
} /* }}} */

/* {{{ proto string UString::getDefaultCodepage(void) */
PHP_METHOD(UString, getDefaultCodepage) {

	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	RETURN_STR(STR_COPY(UG(codepage)));
} /* }}} */

/* {{{ proto string UString::getCodepage(void) */
PHP_METHOD(UString, getCodepage) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());

	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	RETURN_STR(ustring->codepage);
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

ZEND_BEGIN_ARG_INFO_EX(php_ustring_setDefaultCodepage_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, codepage)
ZEND_END_ARG_INFO() /* }}} */

/* {{{ */
zend_function_entry php_ustring_methods[] = {
	PHP_ME(UString, __construct, php_ustring__construct_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, length, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
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
	PHP_ME(UString, charAt, php_ustring_charAt_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, substring, php_ustring_substring_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, getCodepage, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(UString, setDefaultCodepage, php_ustring_setDefaultCodepage_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(UString, getDefaultCodepage, php_ustring_no_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
}; /* }}} */

/* {{{ */
static inline int php_ustring_cast(zval *zread, zval *zwrite, int type TSRMLS_DC) {
	php_ustring_t *ustring;
	uint32_t length = 0;

	if (type != IS_STRING) {
		return FAILURE;
	}

	ustring = PHP_USTRING_FETCH(zread);

	length = ustring->val->extract
		(0, ustring->val->length(), NULL, length, ustring->codepage->val);

	if (!length) {
		return FAILURE;
	}

	Z_STR_P(zwrite) = STR_ALLOC(length, 0);

	ustring->val->extract(
		0,
		ustring->val->length(),
		(char*) Z_STRVAL_P(zwrite),
		(int32_t) Z_STRLEN_P(zwrite),
		ustring->codepage->val);

	Z_STRVAL_P(zwrite)[Z_STRLEN_P(zwrite)] = 0;
	Z_TYPE_INFO_P(zwrite) = IS_STRING_EX;
    
	return SUCCESS;
}

static inline int php_ustring_operate(zend_uchar opcode, zval *result, zval *op1, zval *op2 TSRMLS_DC) {
	switch (opcode) {
		case ZEND_CONCAT: {
			php_ustring_t *uresult;
			php_ustring_t *uop1, *uop2;

			if (op1 != result) {
				object_init_ex(result, ce_UString);

				uresult = PHP_USTRING_FETCH(result);
				uresult->val = new UnicodeString();

				switch (Z_TYPE_P(op1)) {
					case IS_STRING: {
						uresult->val->append(UnicodeString(Z_STRVAL_P(op1), Z_STRLEN_P(op1)));
					} break;

					case IS_OBJECT: {
						uop1 = PHP_USTRING_FETCH(op1);

						uresult->val->append(*uop1->val);
					} break;

					default:
						return FAILURE;
				}
			} else {
				uresult = PHP_USTRING_FETCH(result);
			}

			switch (Z_TYPE_P(op2)) {
				case IS_STRING: {
					uresult->val->append(UnicodeString(Z_STRVAL_P(op2), Z_STRLEN_P(op2)));
				} break;

				case IS_OBJECT: {
					uop2 = PHP_USTRING_FETCH(op2);

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

static inline void php_ustring_free(zend_object *zobject TSRMLS_DC) {
	php_ustring_t *ustring = (php_ustring_t*)((char*)(zobject) - XtOffsetOf(php_ustring_t, std));

	zend_object_std_dtor(&ustring->std TSRMLS_CC);

	if (ustring->codepage) {
	    STR_RELEASE(ustring->codepage);
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
static inline void php_ustring_globals_ctor(zend_ustring_globals  *ug TSRMLS_DC) {
	ug->codepage = NULL;
} /* }}} */

/* {{{ */
static inline zval* php_ustring_read(zval *object, zval *offset, int type, zval *rv TSRMLS_DC) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(object),
				  *rstring;
	zend_bool clean = 0;

	object_init_ex(rv, ce_UString);

	if (Z_TYPE_P(offset) != IS_LONG) {
		convert_to_long(offset);
		clean = 1;
	}

	rstring = PHP_USTRING_FETCH(rv);
	rstring->val = new UnicodeString
		(*ustring->val, Z_LVAL_P(offset), 1);
	rstring->codepage = STR_COPY(ustring->codepage);

	if (clean)
		zval_ptr_dtor(offset);

	return rv;
} /* }}} */

/* {{{ */
static inline void php_ustring_write(zval *object, zval *offset, zval *zvalue TSRMLS_DC) {
	php_ustring_t *ustring = PHP_USTRING_FETCH(object);
	zend_bool cleanup = 0;
	UnicodeString source;

	if (Z_TYPE_P(offset) != IS_LONG) {
		convert_to_long(offset);
		cleanup = 1;
	}

	if (Z_TYPE_P(zvalue) == IS_STRING) {
		source = UnicodeString(Z_STRVAL_P(zvalue), ustring->codepage->val);
	} else if (Z_TYPE_P(zvalue) == IS_OBJECT) {
		if (instanceof_function(Z_OBJCE_P(zvalue), ce_UString TSRMLS_CC)) {
			source = *(PHP_USTRING_FETCH(zvalue))->val;
		} else {
			if (cleanup)
				zval_ptr_dtor(offset);
			return;
		}
	}

	ustring->val->replace(Z_LVAL_P(offset), source.length(), source);

	if (cleanup)
		zval_ptr_dtor(offset);
} /* }}} */

/* {{{ */
static inline int php_ustring_compare(zval *op1, zval *op2 TSRMLS_DC) {
	UnicodeString us1, us2;

	switch (Z_TYPE_P(op1)) {
		case IS_STRING:
			us1 = UnicodeString(Z_STRVAL_P(op1), Z_STRLEN_P(op1), UG(codepage)->val);
		break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(op1), ce_UString TSRMLS_CC)) {
				return 0;
			}

			us1 = *(PHP_USTRING_FETCH(op1))->val;
		break;

		default:
			return 0;
	}

	switch (Z_TYPE_P(op2)) {
		case IS_STRING:
			us2 = UnicodeString(Z_STRVAL_P(op2), Z_STRLEN_P(op2), UG(codepage)->val);
		break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(op2), ce_UString TSRMLS_CC)) {
				return 0;
			}

			us2 = *(PHP_USTRING_FETCH(op2))->val;
		break;

		default:
			return 0;
	}

	return us1.compare(us2);
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
	php_ustring_t *pstring = PHP_USTRING_FETCH(&uit->zobject);

	return (uit->position < pstring->val->length()) ? SUCCESS : FAILURE;
} /* }}} */

/* {{{ */
static inline zval* php_ustring_iterator_current_data(zend_object_iterator* iterator TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;
	php_ustring_t *ustring,
				  *pstring = PHP_USTRING_FETCH(&uit->zobject);

	object_init_ex(&uit->zdata, ce_UString);

	ustring = PHP_USTRING_FETCH(&uit->zdata);
	ustring->codepage =
		STR_COPY(pstring->codepage);
	ustring->val = new UnicodeString(*pstring->val, uit->position, 1);

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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ustring)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "UString", php_ustring_methods);

	ZEND_INIT_MODULE_GLOBALS(ustring, php_ustring_globals_ctor, NULL);

	ce_UString = zend_register_internal_class(&ce TSRMLS_CC);
	ce_UString->create_object = php_ustring_create;
	ce_UString->get_iterator = php_ustring_iterator;

	memcpy(
		&php_ustring_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));

	php_ustring_handlers.offset   = XtOffsetOf(php_ustring_t, std);
	php_ustring_handlers.free_obj = php_ustring_free;
	php_ustring_handlers.do_operation = php_ustring_operate;
	php_ustring_handlers.cast_object = php_ustring_cast;
	php_ustring_handlers.read_dimension = php_ustring_read;
	php_ustring_handlers.write_dimension = php_ustring_write;
	php_ustring_handlers.compare_objects = php_ustring_compare;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ustring)
{
	UG(codepage) = STR_INIT("UTF-8", sizeof("UTF-8")-1, 0);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ustring)
{
	STR_RELEASE(UG(codepage));

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

/* {{{ ustring_module_entry
 */
zend_module_entry ustring_module_entry = {
	STANDARD_MODULE_HEADER,
	"ustring",
	NULL,
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
