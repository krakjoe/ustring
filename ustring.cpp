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

extern "C" {
#    include "php.h"
#    include "php_ini.h"
#    include "ext/standard/info.h"
#    include "php_ustring.h"
#    include "api.h"
}

ZEND_DECLARE_MODULE_GLOBALS(ustring);

#ifdef ZTS
#define UG(v) TSRMG(ustring_globals_id, zend_ustring_globals *, v)
#else
#define UG(v) (ustring_globals.v)
#endif

/*
 * Do some magical ifdef'ing maybe ?
 */
#include "backend/icu.cpp"

PHP_USTRING_API php_ustring_backend_t *php_ustring_backend = &php_ustring_defaults;

PHP_USTRING_API void php_ustring_construct(zval *that, const char *value, size_t vlen, const char *codepage, size_t clen TSRMLS_DC) {
    php_ustring_backend->construct(that, value, vlen, codepage, clen TSRMLS_CC);
}

PHP_USTRING_API size_t php_ustring_length(zval *that TSRMLS_DC) {
    return php_ustring_backend->length(that TSRMLS_CC);
}

PHP_USTRING_API bool php_ustring_startsWith(zval *that, zval *needle TSRMLS_DC) {
    return php_ustring_backend->startsWith(that, needle TSRMLS_CC);
}

PHP_USTRING_API bool php_ustring_endsWith(zval *that, zval *needle TSRMLS_DC) {
    return php_ustring_backend->endsWith(that, needle TSRMLS_CC);
}

PHP_USTRING_API int32_t php_ustring_indexOf(zval *that, zval *needle, int32_t offset TSRMLS_DC) {
    return php_ustring_backend->indexOf(that, needle, offset TSRMLS_CC);
}

PHP_USTRING_API int32_t php_ustring_lastIndexOf(zval *that, zval *needle, int32_t offset TSRMLS_DC) {
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

PHP_USTRING_API zval* php_ustring_replaceSlice(zval *that, zval *slice, int32_t offset, int32_t length, zval *replaced TSRMLS_DC) {
    return php_ustring_backend->replaceSlice(that, slice, offset, length, replaced TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_charAt(zval *that, int32_t offset, zval *found TSRMLS_DC) {
    return php_ustring_backend->charAt(that, offset, found TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_substring(zval *that, int32_t offset, int32_t length, zval *sub TSRMLS_DC) {
    return php_ustring_backend->substring(that, offset, length, sub TSRMLS_CC);
}

PHP_USTRING_API bool php_ustring_contains(zval *that, zval *text TSRMLS_DC) {
    return php_ustring_backend->contains(that, text TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_chunk(zval *that, int32_t length, zval *chunks TSRMLS_DC) {
    return php_ustring_backend->chunk(that, length, chunks TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_repeat(zval *that, int32_t count, zval *repeated TSRMLS_DC) {
    return php_ustring_backend->repeat(that, count, repeated TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_pad(zval *that, int32_t target_len, zval *pad, int mode, zval *padded TSRMLS_DC) {
	return php_ustring_backend->pad(that, target_len, pad, mode, padded TSRMLS_CC);
}

PHP_USTRING_API zval* php_ustring_split(zval *that, zval *delimiter, int32_t limit, zval *pieces TSRMLS_DC) {
	return php_ustring_backend->split(that, delimiter, limit, pieces TSRMLS_CC);
}

PHP_USTRING_API zend_string* php_ustring_getCodepage(zval *that TSRMLS_DC) {
    return php_ustring_backend->getCodepage(that TSRMLS_CC);
}

PHP_USTRING_API int php_ustring_compare(zval *op1, zval *op2 TSRMLS_DC) {
	return php_ustring_backend->compare(op1, op2 TSRMLS_CC);
}

PHP_USTRING_API void php_ustring_setDefaultCodepage(const char *value, int32_t len TSRMLS_DC) {
    zend_string_release(UG(codepage) TSRMLS_CC);

	UG(codepage) = 
	    zend_string_init(value, len, 0 TSRMLS_CC);
}

PHP_USTRING_API zend_string* php_ustring_getDefaultCodepage(TSRMLS_D) {
    return UG(codepage);
}

/* {{{ proto UString UString::__contruct([string arg , [string codepage = "utf-8"]]) */
PHP_METHOD(UString, __construct)
{
	char *value = NULL, 
	     *codepage = NULL;
	size_t    vlen = 0, 
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

	RETURN_LONG(php_ustring_length(getThis() TSRMLS_CC));
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
	char *codepage = NULL;
	size_t  clen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &codepage, &clen) != SUCCESS) {
		return;
	}

	php_ustring_setDefaultCodepage(codepage, (int32_t)clen TSRMLS_CC);
} /* }}} */

/* {{{ proto string UString::getDefaultCodepage(void) */
PHP_METHOD(UString, getDefaultCodepage) {
    zend_string *dc;
    
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}
	
	dc = php_ustring_getDefaultCodepage(TSRMLS_C);

    RETURN_STR(zend_string_copy(dc TSRMLS_CC));
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

	INIT_CLASS_ENTRY(ce, "UString", php_ustring_methods);

	ZEND_INIT_MODULE_GLOBALS(ustring, php_ustring_globals_ctor, NULL);

	ce_UString = zend_register_internal_class(&ce TSRMLS_CC);
    
    php_ustring_backend->initialize(&ce_UString TSRMLS_CC);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ustring)
{
	UG(codepage) = zend_string_init("UTF-8", sizeof("UTF-8")-1, 0 TSRMLS_CC);

	ZVAL_NEW_STR(&UG(defaultpad), zend_string_init(" ", 1, 0 TSRMLS_CC));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ustring)
{
	zend_string_release(UG(codepage) TSRMLS_CC);

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

/* {{{ proto UString u(string value) */
PHP_FUNCTION(u) {
    char *val;
    size_t len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, &len) != SUCCESS) {
        return;
    }
    
    object_init_ex(return_value, ce_UString);
    
    php_ustring_construct
        (return_value, val, len, UG(codepage)->val, UG(codepage)->len TSRMLS_CC);
} /* }}} */

ZEND_BEGIN_ARG_INFO_EX(php_ustring_u_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, string)
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
