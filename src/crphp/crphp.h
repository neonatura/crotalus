
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>             /* malloc, free, etc. */
#include <stdio.h>              /* stdin, stdout, stderr */
#include <string.h>             /* strdup */
#include <ctype.h>
#include <time.h>               /* localtime, time */
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>             /* OPEN_MAX */
#include <setjmp.h>

#include "php_crotalus.h" /* php/sapi/crotalu */


const char *http_ver_string(int ver);
const char *http_method_string(int method);

extern unsigned int server_port;

