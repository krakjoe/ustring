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

ZEND_DECLARE_MODULE_GLOBALS(ustring);

#ifdef ZTS
#define UG(v) TSRMG(ustring_globals_id, zend_ustring_globals *, v)
#else
#define UG(v) (ustring_globals.v)
#endif

#define php_ustring_fetch(o) ((php_ustring_t*) (((char*)Z_OBJ_P(o)) - XtOffsetOf(php_ustring_t, std)))

zend_object_handlers php_ustring_handlers;

/*
 * Do some magical ifdef'ing maybe ?
 */
#include "backend/icu.cpp"

PHP_USTRING_API php_ustring_handlers_t *php_ustring_backend = &php_ustring_defaults;

PHP_USTRING_API void php_ustring_set_backend(php_ustring_handlers_t *handlers TSRMLS_DC) {
    php_ustring_backend = handlers;
}

PHP_USTRING_API void* php_ustring_value(zval *that TSRMLS_DC) {
    return php_ustring_backend->value(that TSRMLS_CC);
}

PHP_USTRING_API void php_ustring_construct(zval *that, const char *value, zend_size_t vlen, const char *codepage, zend_size_t clen TSRMLS_DC) {
    php_ustring_backend->construct(that, value, vlen, codepage, clen TSRMLS_CC);
}

PHP_USTRING_API zend_size_t php_ustring_length(zval *that TSRMLS_DC) {
    return php_ustring_backend->length(that TSRMLS_CC);
}

PHP_USTRING_API bool php_ustring_startsWith(zval *that, zval *needle TSRMLS_DC) {
    return php_ustring_backend->startsWith(that, needle TSRMLS_CC);
}

PHP_USTRING_API bool php_ustring_endsWith(zval *that, zval *needle TSRMLS_DC) {
    return php_ustring_backend->endsWith(that, needle TSRMLS_CC);
}

PHP_USTRING_API zend_int_t php_ustring_indexOf(zval *that, zval *needle, zend_int_t offset TSRMLS_DC) {
    return php_ustring_backend->indexOf(that, needle, offset TSRMLS_CC);
}

PHP_USTRING_API zend_int_t php_ustring_lastIndexOf(zval *that, zval *needle, zend_int_t offset TSRMLS_DC) {
    return php_ustring_backend->lastIndexOf(that, needle, offset TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_toLower(zval *that, zval *lowered TSRMLS_DC) {
    return php_ustring_backend->toLower(that, lowered TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_toUpper(zval *that, zval *uppered TSRMLS_DC) {
    return php_ustring_backend->toUpper(that, uppered TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_reverse(zval *that, zval *reversed TSRMLS_DC) {
    return php_ustring_backend->reverse(that, reversed TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_trim(zval *that, zval *trimmed TSRMLS_DC) {
    return php_ustring_backend->trim(that, trimmed TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_replace(zval *that, zval *search, zval *replace, zval *replaced TSRMLS_DC) {
    return php_ustring_backend->replace(that, search, replace, replaced TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_replaceSlice(zval *that, zval *slice, zend_int_t offset, zend_int_t length, zval *replaced TSRMLS_DC) {
    return php_ustring_backend->replaceSlice(that, slice, offset, length, replaced TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_charAt(zval *that, zend_int_t offset, zval *found TSRMLS_DC) {
    return php_ustring_backend->charAt(that, offset, found TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_substring(zval *that, zend_int_t offset, zend_int_t length, zval *sub TSRMLS_DC) {
    return php_ustring_backend->substring(that, offset, length, sub);
}

PHP_USTRING_API bool php_ustring_contains(zval *that, zval *text TSRMLS_DC) {
    return php_ustring_backend->contains(that, text TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_chunk(zval *that, zend_int_t length, zval *chunks TSRMLS_DC) {
    return php_ustring_backend->chunk(that, length, chunks TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_repeat(zval *that, zend_int_t count, zval *repeated TSRMLS_DC) {
    return php_ustring_backend->repeat(that, count, repeated TSRMLS_CC);
}

PHP_USTRING_API zend_string* php_ustring_getCodepage(zval *that TSRMLS_DC) {
    return php_ustring_backend->getCodepage(that TSRMLS_CC);
}

PHP_USTRING_API int php_ustring_compare(zval *op1, zval *op2 TSRMLS_DC) {
	return php_ustring_backend->compare(op1, op2 TSRMLS_CC);
}

PHP_USTRING_API void php_ustring_setDefaultCodepage(const char *value, zend_int_t len TSRMLS_DC) {
    STR_RELEASE(UG(codepage));

	UG(codepage) = 
	    STR_INIT(value, len, 0);
}

PHP_USTRING_API zend_string* php_ustring_getDefaultCodepage(TSRMLS_D) {
    return UG(codepage);
}

/* {{{ proto UString UString::__contruct([string arg , [string codepage = "utf-8"]]) */
PHP_METHOD(UString, __construct)
{
	char *value = NULL, 
	     *codepage = NULL;
	zend_size_t    vlen = 0, 
	               clen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &value, &vlen, &codepage, &clen) != SUCCESS) {
		return;
	}

	php_ustring_construct(getThis(), value, vlen, codepage, clen TSRMLS_CC);
}
/* }}} */

/* {{{ proto int UString::length(void) */
PHP_METHOD(UString, length) {
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	RETURN_INT(php_ustring_length(getThis() TSRMLS_CC));
} /* }}} */

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
    zend_int_t offset = 0;
    zend_int_t index = -1;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &needle, &offset) != SUCCESS) {
		return;
	}
	
	index = php_ustring_indexOf(getThis(), needle, offset TSRMLS_CC);
	
	if (index < 0)
	    RETURN_FALSE;
	    
	RETURN_INT(index + offset);
} /* }}} */

/* {{{ proto mixed UString::lastIndexOf(UString needle [, int $offset]) */
PHP_METHOD(UString, lastIndexOf) {
	zval *needle;
    zend_int_t offset = 0;
    zend_int_t index = -1;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &needle, &offset) != SUCCESS) {
		return;
	}

	index = php_ustring_lastIndexOf(getThis(), needle, offset TSRMLS_CC);

	if (index < 0)
	    RETURN_FALSE;
	    
	RETURN_INT(index + offset);
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
    zend_int_t start, length;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &text, &start, &length) != SUCCESS) {
		return;
	}

	php_ustring_replaceSlice(getThis(), text, start, length, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::charAt(int index) */
PHP_METHOD(UString, charAt) {
	zend_int_t index;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) != SUCCESS) {
		return;
	}

	php_ustring_charAt(getThis(), index, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::substring(int start [, int length]) */
PHP_METHOD(UString, substring) {
	zend_int_t start = -1,
	           length = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &start, &length) != SUCCESS) {
		return;
	}
	
	php_ustring_substring(getThis(), start, length, return_value TSRMLS_CC);
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
    zend_int_t length = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) != SUCCESS) {
        return;
    }
    
    php_ustring_chunk(getThis(), length, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto UString UString::repeat(int count) */
PHP_METHOD(UString, repeat) {
    zend_int_t count = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &count) != SUCCESS) {
        return;
    }
    
    php_ustring_repeat(getThis(), count, return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto string UString::getCodepage(void) */
PHP_METHOD(UString, getCodepage) {
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	RETURN_STR(php_ustring_getCodepage(getThis() TSRMLS_CC));
} /* }}} */

/* {{{ proto void UString::setDefaultCodepage(string codepage) */
PHP_METHOD(UString, setDefaultCodepage) {
	char *codepage = NULL;
	zend_size_t  clen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &codepage, &clen) != SUCCESS) {
		return;
	}

	php_ustring_setDefaultCodepage(codepage, clen TSRMLS_CC);
} /* }}} */

/* {{{ proto string UString::getDefaultCodepage(void) */
PHP_METHOD(UString, getDefaultCodepage) {

	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

    RETURN_STR(STR_COPY(php_ustring_getDefaultCodepage(TSRMLS_C)));
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
	zend_int_t length = 0;

	if (type != IS_STRING) {
		return FAILURE;
	}

	ustring = php_ustring_fetch(zread);

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
		(int32_t) Z_STRSIZE_P(zwrite),
		ustring->codepage->val);

	Z_STRVAL_P(zwrite)[Z_STRSIZE_P(zwrite)] = 0;
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

	ustring = php_ustring_fetch(&uit->zdata);
	ustring->codepage =
		STR_COPY(pstring->codepage);
	ustring->val = new UnicodeString(*pstring->val, uit->position, 1);

	return &uit->zdata;
} /* }}} */

/* {{{ */
static inline void php_ustring_iterator_current_key(zend_object_iterator* iterator, zval *key TSRMLS_DC) {
	php_ustring_iterator_t *uit =
		(php_ustring_iterator_t*) iterator;

	ZVAL_INT(key, uit->position);
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
