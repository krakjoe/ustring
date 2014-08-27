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

#ifndef PHP_USTRING_H
#define PHP_USTRING_H

#include <php.h>

extern zend_module_entry ustring_module_entry;
#define phpext_ustring_ptr &ustring_module_entry

#define PHP_USTRING_VERSION "0.1.0"

#ifdef ZTS
#    include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(ustring)
	zend_string *codepage;
	zval defaultpad;
ZEND_END_MODULE_GLOBALS(ustring)

#define STR_PAD_LEFT            0
#define STR_PAD_RIGHT           1
#define STR_PAD_BOTH            2

#endif	/* PHP_USTRING_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
