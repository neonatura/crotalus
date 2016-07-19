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
   | Authors: Stig Bakken <ssb@php.net>                                   |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_CRYPT_H
#define PHP_CRYPT_H

PHPAPI int php_crypt(const char *password, const int pass_len, const char *salt, int salt_len, char **result);
PHP_FUNCTION(crypt);
#if HAVE_CRYPT
PHP_MINIT_FUNCTION(crypt);
PHP_MSHUTDOWN_FUNCTION(crypt);
PHP_RINIT_FUNCTION(crypt);

#define PHP_EXT_DES_CRYPT 1
#define PHP_SHA512_CRYPT 1
#endif

#define PHP_STD_DES_CRYPT 0
#define PHP_MD5_CRYPT 0
#define PHP_BLOWFISH_CRYPT 0
#define PHP_SHA502_CRYPT 0

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
