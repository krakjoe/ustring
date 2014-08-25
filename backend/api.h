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

#ifndef PHP_USTRING_BACKEND_API_H
#define PHP_USTRING_BACKEND_API_H

/* {{{ */
typedef void         (*php_ustring_construct_f)         (zval *that, const char *value, zend_size_t len, const char *codepage, zend_size_t clen TSRMLS_DC);
typedef zend_size_t  (*php_ustring_length_f)            (zval *that TSRMLS_DC);
typedef bool         (*php_ustring_startsWith_f)        (zval *that, zval *needle TSRMLS_DC);
typedef bool         (*php_ustring_endsWith_f)          (zval *that, zval *needle TSRMLS_DC);
typedef zend_int_t   (*php_ustring_indexOf_f)           (zval *that, zval *needle, zend_int_t offset TSRMLS_DC);
typedef zend_int_t   (*php_ustring_lastIndexOf_f)       (zval *that, zval *needle, zend_int_t offset TSRMLS_DC);
typedef zval*        (*php_ustring_toLower_f)           (zval *that, zval *lowered TSRMLS_DC);
typedef zval*        (*php_ustring_toUpper_f)           (zval *that, zval *uppered TSRMLS_DC);
typedef zval*        (*php_ustring_reverse_f)           (zval *that, zval *reversed TSRMLS_DC);
typedef zval*        (*php_ustring_trim_f)              (zval *that, zval *trimmed TSRMLS_DC);
typedef zval*        (*php_ustring_replace_f)           (zval *that, zval *search, zval *replace, zval *replaced TSRMLS_DC);
typedef zval*        (*php_ustring_replaceSlice_f)      (zval *that, zval *slice, zend_int_t offset, zend_int_t length, zval *replaced TSRMLS_DC);
typedef zval*        (*php_ustring_charAt_f)            (zval *that, zend_int_t offset, zval *found TSRMLS_DC);
typedef zval*        (*php_ustring_substring_f)         (zval *that, zend_int_t offset, zend_int_t length, zval *substring TSRMLS_DC);
typedef bool         (*php_ustring_contains_f)          (zval *that, zval *text TSRMLS_DC);
typedef zval*        (*php_ustring_chunk_f)             (zval *that, zend_int_t length, zval *chunks TSRMLS_DC);
typedef zval*        (*php_ustring_repeat_f)            (zval *that, zend_int_t count, zval *repeated TSRMLS_DC);
typedef zval*        (*php_ustring_pad_f)               (zval *that, int32_t target_len, zval *pad, int mode, zval *padded TSRMLS_DC);
typedef zval*        (*php_ustring_split_f)             (zval *that, zval *delimiter, int32_t limit, zval *pieces TSRMLS_DC);
typedef zend_string* (*php_ustring_getCodepage_f)       (zval *that TSRMLS_DC);
typedef int          (*php_ustring_compare_f)           (zval *op1, zval *op2 TSRMLS_DC);
typedef void         (*php_ustring_initialize_f)        (zend_class_entry **ce TSRMLS_DC);
/* }}} */

/* {{{ */
typedef struct _php_ustring_backend_t {
    php_ustring_construct_f    construct;
    php_ustring_length_f       length;
    php_ustring_startsWith_f   startsWith;
    php_ustring_startsWith_f   endsWith;
    php_ustring_indexOf_f      indexOf;
    php_ustring_indexOf_f      lastIndexOf;
    php_ustring_toLower_f      toLower;
    php_ustring_toUpper_f      toUpper;
    php_ustring_reverse_f      reverse;
    php_ustring_trim_f         trim;
    php_ustring_replace_f      replace;
    php_ustring_replaceSlice_f replaceSlice;
    php_ustring_charAt_f       charAt;
    php_ustring_substring_f    substring;
    php_ustring_contains_f     contains;
    php_ustring_chunk_f        chunk;
    php_ustring_repeat_f       repeat;
	php_ustring_pad_f          pad;
	php_ustring_split_f	       split;
    php_ustring_getCodepage_f  getCodepage;
    php_ustring_compare_f      compare;
    php_ustring_initialize_f   initialize;
} php_ustring_backend_t; /* }}} */

#endif	/* PHP_USTRING_BACKEND_API_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
