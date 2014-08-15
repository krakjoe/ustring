/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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

extern "C" {
#    include "php.h"
#    include "php_ini.h"
#    include "ext/standard/info.h"
}

#include "unicode/unistr.h"

#include "php_ustring.h"

typedef struct _php_ustring_t {
    UnicodeString *val;
    zend_object   std;
} php_ustring_t;

#define PHP_USTRING_FETCH(o) (php_ustring_t*) (Z_OBJ_P(o) - XtOffsetOf(php_ustring_t, std))

zend_class_entry *ce_UString;

zend_object_handlers php_ustring_handlers;

/* {{{ proto UString UString::__contruct([string arg [, string codepage [, int length]]]) */
PHP_METHOD(UString, __construct)
{
	char *val = NULL, 
	     *codepage = 0;
	long  vlen = 0, 
	      len = 0,
	      clen = 0;
	
	php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ssl", &val, &vlen, &codepage, &clen, &len) != SUCCESS) {
	    return;
	}
	
	switch (ZEND_NUM_ARGS()) {
	    case 3:
	    case 2:
	        ustring->val = new UnicodeString(val, len ? len : vlen, codepage);
	    break;
	    
	    case 1:
	        ustring->val = new UnicodeString(val, len ? len : vlen);
	    break;
	    
	    case 0:
	        ustring->val = new UnicodeString();
	    break;
	}
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

/* {{{ proto bool UString::startsWith(UString needle [, int start [, int length]]) */
PHP_METHOD(UString, startsWith) {
    zval *zneedle;
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
                  *uneedle;
    int32_t start = -1, 
            length = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &zneedle, &start, &length) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zneedle)) {
        case IS_STRING:
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->startsWith(Z_STRVAL_P(zneedle)));
                case 2:
                    RETURN_BOOL(ustring->val->startsWith(Z_STRVAL_P(zneedle), start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_BOOL(ustring->val->startsWith(Z_STRVAL_P(zneedle), start, length));
            }
        break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->startsWith(*uneedle->val));
                case 2:
                    RETURN_BOOL(ustring->val->startsWith(*uneedle->val, start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_BOOL(ustring->val->startsWith(*uneedle->val, start, length));
            }
        } break;
    }
} /* }}} */

/* {{{ proto bool UString::endsWith(UString needle [, int start [, int length]]) */
PHP_METHOD(UString, endsWith) {
    zval *zneedle;
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
                  *uneedle;
    int32_t start = -1, 
            length = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &zneedle, &start, &length) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zneedle)) {
        case IS_STRING:
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->endsWith(Z_STRVAL_P(zneedle)));
                case 2:
                    RETURN_BOOL(ustring->val->endsWith(Z_STRVAL_P(zneedle), start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_BOOL(ustring->val->endsWith(Z_STRVAL_P(zneedle), start, length));
            }
        break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->endsWith(*uneedle->val));
                case 2:
                    RETURN_BOOL(ustring->val->endsWith(*uneedle->val, start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_BOOL(ustring->val->endsWith(*uneedle->val, start, length));
            }
        } break;
    }
} /* }}} */

/* {{{ proto int UString::indexOf(UString needle [, int start [, int length]]) */
PHP_METHOD(UString, indexOf) {
    zval *zneedle;
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
                  *uneedle;
    int32_t start = -1, 
            length = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &zneedle, &start, &length) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zneedle)) {
        case IS_STRING:
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->indexOf(Z_STRVAL_P(zneedle)));
                case 2:
                    RETURN_LONG(ustring->val->indexOf(Z_STRVAL_P(zneedle), start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_LONG(ustring->val->indexOf(Z_STRVAL_P(zneedle), start, length));
            }
        break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->indexOf(*uneedle->val));
                case 2:
                    RETURN_LONG(ustring->val->indexOf(*uneedle->val, start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_LONG(ustring->val->indexOf(*uneedle->val, start, length));
            }
        } break;
    }
} /* }}} */

/* {{{ proto int UString::lastIndexOf(UString needle [, int start [, int length]]) */
PHP_METHOD(UString, lastIndexOf) {
    zval *zneedle;
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
                  *uneedle;
    int32_t start = -1, 
            length = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &zneedle, &start, &length) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zneedle)) {
        case IS_STRING:
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->lastIndexOf(Z_STRVAL_P(zneedle)));
                case 2:
                    RETURN_LONG(ustring->val->lastIndexOf(Z_STRVAL_P(zneedle), start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_LONG(ustring->val->lastIndexOf(Z_STRVAL_P(zneedle), start, length));
            }
        break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->lastIndexOf(*uneedle->val));
                case 2:
                    RETURN_LONG(ustring->val->lastIndexOf(*uneedle->val, start, Z_STRLEN_P(zneedle) - start));
                case 3:
                    RETURN_LONG(ustring->val->lastIndexOf(*uneedle->val, start, length));
            }
        } break;
    }
} /* }}} */

/* {{{ proto void UString::toLower(void) */
PHP_METHOD(UString, toLower) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    ustring->val->toLower();
} /* }}} */

/* {{{ proto void UString::toUpper(void) */
PHP_METHOD(UString, toUpper) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    ustring->val->toUpper();
} /* }}} */

/* {{{ proto void UString::reverse(void) */
PHP_METHOD(UString, reverse) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    ustring->val->reverse();
} /* }}} */

/* {{{ proto void UString::trim(void) */
PHP_METHOD(UString, trim) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    ustring->val->trim();
} /* }}} */

/* {{{ proto int UString::truncate(int length) */
PHP_METHOD(UString, truncate) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    int32_t length = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) != SUCCESS) {
        return;
    }
    
    ustring->val->truncate(length);

    RETURN_LONG(ustring->val->length());
} /* }}} */

/* {{{ proto int UString::append(string subject [, int start [, int length]]) */
PHP_METHOD(UString, append) {
    zval *zneedle;
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
                  *uneedle;
    int32_t start = -1, 
            length = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &zneedle, &start, &length) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zneedle)) {
        case IS_STRING:
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    ustring->val->append(Z_STRVAL_P(zneedle));
                break;
                   
                case 2:
                    ustring->val->append(Z_STRVAL_P(zneedle), start, Z_STRLEN_P(zneedle) - start);
                break;
                    
                case 3:
                    ustring->val->append(Z_STRVAL_P(zneedle), start, length);
                break;
            }
        break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    ustring->val->append(*uneedle->val);
                break;
                
                case 2:
                    ustring->val->append(*uneedle->val, start, Z_STRLEN_P(zneedle) - start);
                break;

                case 3:
                    ustring->val->append(*uneedle->val, start, length);
                break;
            }
        } break;
    }
    
    RETURN_LONG(ustring->val->length());
} /* }}} */

/* {{{ proto int UString::replace(UString search, UString replace) */
PHP_METHOD(UString, replace) {
    zval *zsearch, *zreplace;
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zsearch, &zreplace) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zsearch)) {
        case IS_STRING: switch(Z_TYPE_P(zreplace)) {
            case IS_STRING:
                ustring->val->findAndReplace(UnicodeString(Z_STRVAL_P(zsearch), Z_STRLEN_P(zsearch)), UnicodeString(Z_STRVAL_P(zreplace), Z_STRLEN_P(zreplace)));
            break;
            
            case IS_OBJECT: {
                ustring->val->findAndReplace(UnicodeString(Z_STRVAL_P(zsearch), Z_STRLEN_P(zsearch)), *(PHP_USTRING_FETCH(zreplace))->val);
            } break;
        } break;
        
        case IS_OBJECT: switch(Z_TYPE_P(zreplace)) {
            case IS_OBJECT: 
                ustring->val->findAndReplace(*(PHP_USTRING_FETCH(zsearch))->val, *(PHP_USTRING_FETCH(zreplace))->val);
            break;
            
            case IS_STRING: 
                ustring->val->findAndReplace(*(PHP_USTRING_FETCH(zsearch))->val, UnicodeString(Z_STRVAL_P(zreplace), Z_STRLEN_P(zreplace)));
            break;
        } break;
    }
    
    RETURN_LONG(ustring->val->length());
} /* }}} */

/* {{{ proto string UString::__toString(void) */
PHP_METHOD(UString, __toString) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis());
    uint32_t targetLength = 0;
    char *target = NULL;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }        
    
    targetLength = ustring->val->extract
        (0, ustring->val->length(), target, targetLength);
    
    if (!targetLength)
        RETURN_NULL();
    
    RETVAL_EMPTY_STRING();
    
    Z_STR_P(return_value) = STR_ALLOC(targetLength+1, 0);

    ustring->val->extract(
        0, 
        ustring->val->length(), 
        (char*) Z_STRVAL_P(return_value), 
        targetLength);

    Z_STRLEN_P(return_value) = targetLength;
    Z_STRVAL_P(return_value)[Z_STRLEN_P(return_value)] = 0;
} /* }}} */

/* {{{ */
ZEND_BEGIN_ARG_INFO_EX(php_ustring_no_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring__construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_std_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, needle)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_truncate_arginfo, 0, 0, 1)   
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_replace_arginfo, 0, 0, 2)   
    ZEND_ARG_INFO(0, search)
    ZEND_ARG_INFO(0, replace)
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
    PHP_ME(UString, truncate, php_ustring_truncate_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(UString, append, php_ustring_std_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(UString, replace, php_ustring_replace_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(UString, __toString, php_ustring_no_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
}; /* }}} */

/* {{{ */
static inline void php_ustring_free(zend_object *zobject TSRMLS_DC) {
    php_ustring_t *ustring = (php_ustring_t*)((char*)(zobject) - XtOffsetOf(php_ustring_t, std));
 
	zend_object_std_dtor(&ustring->std TSRMLS_CC);
	
	delete ustring->val;
}

static inline zend_object* php_ustring_create(zend_class_entry *ce TSRMLS_DC) {
	php_ustring_t *ustring = 
		(php_ustring_t*) ecalloc(1, sizeof(php_ustring_t));
	
	zend_object_std_init(&ustring->std, ce TSRMLS_CC);

    ustring->std.handlers = &php_ustring_handlers;
    
	return &ustring->std;
} /* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ustring)
{
    zend_class_entry ce;
    
	INIT_CLASS_ENTRY(ce, "UString", php_ustring_methods);
	
	ce_UString = zend_register_internal_class(&ce TSRMLS_CC);
	ce_UString->create_object = php_ustring_create;
	
	memcpy(
		&php_ustring_handlers,
		zend_get_std_object_handlers(), 
		sizeof(zend_object_handlers));
		
    php_ustring_handlers.offset   = XtOffsetOf(php_ustring_t, std);
    php_ustring_handlers.free_obj = php_ustring_free;
	
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ustring)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ustring)
{
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
	NULL,
	NULL,
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
