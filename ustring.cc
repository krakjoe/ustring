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

extern "C" {
#    include "php.h"
#    include "php_ini.h"
#    include "ext/standard/info.h"
ZEND_DECLARE_MODULE_GLOBALS(ustring);
}

#include "unicode/unistr.h"

#include "php_ustring.h"

typedef struct _php_ustring_t {
    UnicodeString *val;
    zend_string   *codepage;
    zend_object   std;
} php_ustring_t;

#define PHP_USTRING_FETCH(o) (php_ustring_t*) (((char*)Z_OBJ_P(o)) - XtOffsetOf(php_ustring_t, std))

zend_class_entry *ce_UString;

zend_object_handlers php_ustring_handlers;

/* {{{ proto UString UString::__contruct([string arg , [string codepage = "utf-8"]]) */
PHP_METHOD(UString, __construct)
{
	char *val = NULL, 
	     *codepage = NULL;
	long  vlen = 0,
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
	} else ustring->codepage = STR_COPY(UG(codepage));
	
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
        case IS_STRING: {
            UnicodeString needle = UnicodeString(Z_STRVAL_P(zneedle), Z_STRLEN_P(zneedle), ustring->codepage->val);
            
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->startsWith(needle));
                case 2:
                    RETURN_BOOL(ustring->val->startsWith(needle, start, needle.length()));
                case 3:
                    RETURN_BOOL(ustring->val->startsWith(needle, start, length));
            }
        } break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->startsWith(*uneedle->val));
                case 2:
                    RETURN_BOOL(ustring->val->startsWith(*uneedle->val, start, uneedle->val->length()));
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
        case IS_STRING: {
            UnicodeString needle = UnicodeString(Z_STRVAL_P(zneedle), Z_STRLEN_P(zneedle), ustring->codepage->val);
            
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->endsWith(needle));
                case 2:
                    RETURN_BOOL(ustring->val->endsWith(needle, start, needle.length()));
                case 3:
                    RETURN_BOOL(ustring->val->endsWith(needle, start, length));
            }
        } break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_BOOL(ustring->val->endsWith(*uneedle->val));
                case 2:
                    RETURN_BOOL(ustring->val->endsWith(*uneedle->val, start, uneedle->val->length()));
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
        case IS_STRING: {
            UnicodeString needle = UnicodeString(Z_STRVAL_P(zneedle), Z_STRLEN_P(zneedle), ustring->codepage->val);
            
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->indexOf(needle));
                case 2:
                    RETURN_LONG(ustring->val->indexOf(needle, start, needle.length()));
                case 3:
                    RETURN_LONG(ustring->val->indexOf(needle, start, length));
            }
        } break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->indexOf(*uneedle->val));
                case 2:
                    RETURN_LONG(ustring->val->indexOf(*uneedle->val, start, uneedle->val->length()));
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
        case IS_STRING: {
            UnicodeString needle = UnicodeString(Z_STRVAL_P(zneedle), Z_STRLEN_P(zneedle), ustring->codepage->val);
            
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->lastIndexOf(needle));
                case 2:
                    RETURN_LONG(ustring->val->lastIndexOf(needle, start, needle.length()));
                case 3:
                    RETURN_LONG(ustring->val->lastIndexOf(needle, start, length));
            }
        } break;
        
        case IS_OBJECT: {
            uneedle = PHP_USTRING_FETCH(zneedle);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    RETURN_LONG(ustring->val->lastIndexOf(*uneedle->val));
                case 2:
                    RETURN_LONG(ustring->val->lastIndexOf(*uneedle->val, start, uneedle->val->length()));
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

/* {{{ proto int UString::append(string data [, int start [, int length]]) */
PHP_METHOD(UString, append) {
    zval *zdata;
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
                  *udata;
    int32_t start = -1, 
            length = -1;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &zdata, &start, &length) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zdata)) {
        case IS_STRING: {
            UnicodeString data = UnicodeString(Z_STRVAL_P(zdata), Z_STRLEN_P(zdata), ustring->codepage->val);
            
            switch (ZEND_NUM_ARGS()) {
                case 1:
                    ustring->val->append(data);
                break;
                   
                case 2:
                    ustring->val->append(data, start, data.length());
                break;
                    
                case 3:
                    ustring->val->append(data, start, length);
                break;
            }
        } break;
        
        case IS_OBJECT: {
            udata = PHP_USTRING_FETCH(zdata);

            switch (ZEND_NUM_ARGS()) {
                case 1:
                    ustring->val->append(*udata->val);
                break;
                
                case 2:
                    ustring->val->append(*udata->val, start, udata->val->length());
                break;

                case 3:
                    ustring->val->append(*udata->val, start, length);
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
    UnicodeString search, replace;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zsearch, &zreplace) != SUCCESS) {
        return;
    }

    switch (Z_TYPE_P(zsearch)) {
        case IS_STRING: {
            search = UnicodeString(Z_STRVAL_P(zsearch), Z_STRLEN_P(zsearch), ustring->codepage->val);
            
            switch(Z_TYPE_P(zreplace)) {
                case IS_STRING:
                    replace = UnicodeString(Z_STRVAL_P(zreplace), Z_STRLEN_P(zreplace), ustring->codepage->val);
                break;
                
                case IS_OBJECT: {
                    replace = *(PHP_USTRING_FETCH(zreplace))->val;
                } break;
                
                default:
                    return;
            }
        } break;
        
        case IS_OBJECT: {   
            search = *(PHP_USTRING_FETCH(zsearch))->val;
            
            switch(Z_TYPE_P(zreplace)) {
                case IS_OBJECT: 
                    replace = *(PHP_USTRING_FETCH(zreplace))->val;
                break;
                
                case IS_STRING: 
                    replace = UnicodeString(Z_STRVAL_P(zreplace), Z_STRLEN_P(zreplace), ustring->codepage->val);
                break;
                
                default:
                    return;
            }
        } break;
        
        default:
            return;
    }
    
    ustring->val->findAndReplace(search, replace);
    
    RETURN_LONG(ustring->val->length());
} /* }}} */

/* {{{ proto UString UString::charAt(int index) */
PHP_METHOD(UString, charAt) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(getThis()),
                  *uchar;
    long index = 0;
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
    char *codepage = NULL;
    int32_t clen = 0;
    
    if (zend_parse_parameters_none() != SUCCESS) {
        return;
    }
    
    RETURN_STR(STR_COPY(UG(codepage)));
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
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_truncate_arginfo, 0, 0, 1)   
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(php_ustring_replace_arginfo, 0, 0, 2)   
    ZEND_ARG_INFO(0, search)
    ZEND_ARG_INFO(0, replace)
ZEND_END_ARG_INFO() 

ZEND_BEGIN_ARG_INFO_EX(php_ustring_charAt_arginfo, 0, 0, 1)   
    ZEND_ARG_INFO(0, index)
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
    PHP_ME(UString, truncate, php_ustring_truncate_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(UString, append, php_ustring_std_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(UString, replace, php_ustring_replace_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(UString, charAt, php_ustring_charAt_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(UString, setDefaultCodepage, php_ustring_setDefaultCodepage_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(UString, getDefaultCodepage, php_ustring_no_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_FE_END
}; /* }}} */

/* {{{ */
static inline HashTable* php_ustring_debug(zval *object, int *temp TSRMLS_DC) {
    php_ustring_t *ustring = PHP_USTRING_FETCH(object);
    HashTable     *udebug;
    
    ALLOC_HASHTABLE(udebug);
    zend_hash_init(udebug, 8, NULL, ZVAL_PTR_DTOR, 0);
    
    
    
    return udebug;
}

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
    
    Z_STR_P(zwrite) = STR_ALLOC(length+1, 0);
    Z_STRLEN_P(zwrite) = length;
    
    ustring->val->extract(
        0,
        ustring->val->length(),
        (char*) Z_STRVAL_P(zwrite),
        Z_STRLEN_P(zwrite),
        ustring->codepage->val);

    Z_STRVAL_P(zwrite)[Z_STRLEN_P(zwrite)] = 0;
    
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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ustring)
{
    zend_class_entry ce;
    
	INIT_CLASS_ENTRY(ce, "UString", php_ustring_methods);
	
	ZEND_INIT_MODULE_GLOBALS(ustring, php_ustring_globals_ctor, NULL);
	
	ce_UString = zend_register_internal_class(&ce TSRMLS_CC);
	ce_UString->create_object = php_ustring_create;
	
	memcpy(
		&php_ustring_handlers,
		zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));
		
    php_ustring_handlers.offset   = XtOffsetOf(php_ustring_t, std);
    php_ustring_handlers.free_obj = php_ustring_free;
	php_ustring_handlers.do_operation = php_ustring_operate;
	php_ustring_handlers.cast_object = php_ustring_cast;
	
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ustring)
{
    UG(codepage) = STR_INIT("UTF-8", sizeof("UTF-8")-1, 0);
    
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
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
