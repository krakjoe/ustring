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

#ifndef PHP_USTRING_H
#define PHP_USTRING_H

extern zend_module_entry ustring_module_entry;
#define phpext_ustring_ptr &ustring_module_entry

#define PHP_USTRING_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_USTRING_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_USTRING_API __attribute__ ((visibility("default")))
#else
#	define PHP_USTRING_API
#endif

extern "C" {
#include "php.h"

#ifdef ZTS
#    include "TSRM.h"
#endif
}

#ifdef ZTS
#define UG(v) TSRMG(ustring_globals_id, zend_ustring_globals *, v)
#else
#define UG(v) (ustring_globals.v)
#endif

ZEND_BEGIN_MODULE_GLOBALS(ustring)
    zend_string *codepage;
ZEND_END_MODULE_GLOBALS(ustring)

#endif	/* PHP_USTRING_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
