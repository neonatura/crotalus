
AC_PREREQ([2.63])
AC_INIT(crotalus, 2.26, support@neo-natura.com)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE(crotalus, 2.26)
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([php/main/internal_functions.c])
AC_CONFIG_FILES([php/main/internal_functions_cli.c])
AC_CONFIG_HEADERS([php/main/build-defs.h])
AC_CONFIG_HEADERS([php/main/php_config.h])
AC_CONFIG_HEADERS([php/Zend/zend_config.h])
AC_CONFIG_HEADERS([php/TSRM/tsrm_config.h])
AC_CONFIG_HEADERS([php/ext/date/lib/timelib_config.h])

# initd scripts #
AC_CONFIG_FILES([scripts/initd/crotalus], [chmod +x scripts/initd/crotalus])

AC_CANONICAL_HOST

AC_PROG_INSTALL

dnl Checks for programs.
AC_PROG_CC
#AC_PROG_CPP
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
#AC_C_VAR_FUNC

AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES(socklen_t)
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)
AC_CHECK_SIZEOF(short, 2)

AC_CHECK_TYPE([uint])

MAKEINFOHTMLFLAGS=--no-headers --no-split

#CHECK_GNU_MAKE
#if test "x$_cv_gnu_make_command" != "x"; then
#  MAKE="$_cv_gnu_make_command"
#  ALLSOURCES="\$^"
#else
#  MAKE="make"
#  ALLSOURCES="\$(.ALLSRC)"
#fi
#AC_SUBST(ALLSOURCES)
#AC_SUBST(MAKE)

dnl AC_MSG_RESULT($host) dnl i686-pc-linux-gnu
dnl AC_MSG_RESULT($host_cpu) dnl i686
dnl AC_MSG_RESULT($host_vendor) dnl pc
dnl AC_MSG_RESULT($host_os) dnl linux-gnu
dnl i386-unknown-freebsd4.2

dnl Checks for libraries.
# AC_SEARCH_LIBS (function, search-libs, [action-if-found], [action-if-not-found], [other-libraries])

AC_CHECK_LIB([pthread], [pthread_create], HAVE_LIBPTHREAD=1)
AM_CONDITIONAL(HAVE_LIBPTHREAD, test $HAVE_LIBPTHREAD)
if test $HAVE_LIBPTHREAD; then
  AC_DEFINE_UNQUOTED(HAVE_LIBPTHREAD, 1, [Define if libpthread can be found])
fi

AC_CHECK_LIB([xml2], [xmlInitGlobals], HAVE_LIBXML=1)
AM_CONDITIONAL(HAVE_LIBXML, test $HAVE_LIBXML)
if test $HAVE_LIBXML; then
  AC_DEFINE_UNQUOTED(HAVE_LIBXML, 1, [Define if libxml2 can be found])
fi

AC_CHECK_LIB([rt], [shm_open], HAVE_LIBRT=1)
AM_CONDITIONAL(HAVE_LIBRT, test $HAVE_LIBRT)
if test $HAVE_LIBRT; then
  AC_DEFINE_UNQUOTED(HAVE_LIBRT, 1, [Define if librt can be found])
fi

AC_CHECK_LIB([share], [get_libshare_version], HAVE_LIBSHARE=1)
AM_CONDITIONAL(HAVE_LIBSHARE, test $HAVE_LIBSHARE)
if test $HAVE_LIBSHARE; then
  AC_DEFINE_UNQUOTED(HAVE_LIBSHARE, 1, [Define if libshare can be found])
fi

# determine whether "libz" library is available in system.
AC_CHECK_LIB([z], [gzdopen], HAVE_LIBZ=1)
AM_CONDITIONAL(HAVE_LIBZ, test $HAVE_LIBZ)
if test $HAVE_LIBZ; then
  AC_DEFINE_UNQUOTED(HAVE_LIBZ, 1, [Define if libz can be found])
fi
# the "libz" library function calls are always available.
AC_DEFINE_UNQUOTED(HAVE_ZLIB, 1, [Define if libz can be used])

AC_SEARCH_LIBS(socket, socket net)
AC_SEARCH_LIBS(inet_aton, resolv)
AC_SEARCH_LIBS(gethostname, nsl)
AC_SEARCH_LIBS(gethostbyname, nsl)

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(stdio.h stdint.h inttypes.h features.h sys/types.h fcntl.h sys/fcntl.h limits.h sys/time.h)
AC_CHECK_HEADERS(getopt.h unistd.h sys/resource.h sys/socket.h string.h arpa/inet.h pwd.h grp.h shadow.h)
AC_CHECK_HEADERS(assert.h netinet/in.h)
AC_CHECK_HEADERS(magic.h wchar.h wctype.h locale.h stdarg.h sys/mman.h sys/loadavg.h \
utime.h \
dirent.h \
alloca.h \
sys/utsname.h \
fnmatch.h \
pthread.h \
)


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
dnl AC_CHECK_TYPE(sa_family_t,unsigned short int)
dnl AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[],[]) (includes, function-body, [action-if-found], [action-if-not-found])
AC_MSG_CHECKING(whether sa_family_t is defined)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/socket.h>
]], [[sa_family_t foo2;]])],[AC_MSG_RESULT(yes)],[
 AC_MSG_RESULT(no)
 AC_DEFINE(DONT_HAVE_SA_FAMILY_T,1,[Define if sa_family_t is not defined])
 
])

dnl Checks for library functions.
AC_FUNC_FNMATCH
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_SETVBUF_REVERSED
AC_CHECK_FUNCS(getcwd strdup strstr strcspn strtol)
AC_CHECK_FUNCS(gethostname gethostbyname socket inet_aton herror inet_addr)
AC_CHECK_FUNCS(scandir alphasort)
AC_CHECK_FUNCS(madvise)
AC_CHECK_FUNCS(strlcpy strlcat strtok_r strerror getgrnam getgrnam_r getpwnam getpwnam_r strnlen readlink)

#AC_CHECK_STRUCT_FOR([
##if TIME_WITH_SYS_TIME
## include <sys/time.h>
## include <time.h>
##else
## if HAVE_SYS_TIME_H
##  include <sys/time.h>
## else
##  include <time.h>
## endif
##endif
#],tm,tm_gmtoff)

if test "$ac_cv_struct_tm_has_tm_gmtoff" = "yes"; then
  AC_DEFINE(HAVE_TM_GMTOFF,1,[Define if struct tm has a tm_gmtoff member])
fi

#AC_CHECK_STRUCT_FOR([
##if TIME_WITH_SYS_TIME
## include <sys/time.h>
## include <time.h>
##else
## if HAVE_SYS_TIME_H
##  include <sys/time.h>
## else
##  include <time.h>
## endif
##endif
#],tm,tm_zone)

if test "$ac_cv_struct_tm_has_tm_zone" = "yes"; then
  AC_DEFINE(HAVE_TM_ZONE,1,[Define if struct tm has tm_zone member])
fi

#AC_CHECK_STRUCT_FOR([
##include <sys/types.h>
##include <netinet/in.h>
#],sockaddr_in,sin_len)

if test "$ac_cv_struct_sockaddr_in_has_sin_len" = "yes"; then
  AC_DEFINE(HAVE_SIN_LEN,1,[Define if struct sockaddr_in has sin_len member])
fi

if test $ac_cv_func_scandir = no; then
  # scandir not defined, add it
  SCANDIR="scandir.o"
  AC_SUBST(SCANDIR)
fi

if test $ac_cv_func_alphasort = no; then
  # alphasort not defined, add it
  ALPHASORT="alphasort.o"
  AC_SUBST(ALPHASORT)
fi

if test $ac_cv_func_strdup = no -o $ac_cv_func_strstr = no; then
  # strdup or strstr not defined
  STRUTIL="strutil.o"
  AC_SUBST(STRUTIL)
fi

AC_MSG_CHECKING(whether to enable gunzip support)
AC_ARG_ENABLE(gunzip,
[  --disable-gunzip        Disable use of web file .gz decompression.],
[
 if test "$enableval" = "yes" ; then
    AC_MSG_RESULT(yes)
    AC_DEFINE_UNQUOTED(GUNZIP, 1, [Define if gunzip can be found])
  else
    AC_MSG_RESULT(no)
  fi
],
[
    AC_MSG_RESULT(yes)
    AC_DEFINE_UNQUOTED(GUNZIP, 1, [Define if gunzip can be found])
])


#AC_MSG_CHECKING(whether to enable php support)
#AC_ARG_ENABLE(php,
#[  --disable-php        Disable use of php],
#[
# if test "$enableval" = "yes" ; then
#    AC_MSG_RESULT(yes)
#    AC_DEFINE_UNQUOTED(PHP, "$ac_cv_path_PHP", [Define if php can be found])
#  else
#    AC_MSG_RESULT(no)
#  fi
#],
#[
#    AC_MSG_RESULT(yes)
#    AC_DEFINE_UNQUOTED(PHP, "$ac_cv_path_PHP", [Define if php can be found])
#])

AC_MSG_CHECKING(whether to enable cgi support)
AC_ARG_ENABLE(cgi,
[  --disable-cgi           Disable use of cgi],
[
 if test "$enableval" = "yes" ; then
    AC_MSG_RESULT(yes)
    AC_DEFINE_UNQUOTED(CGI, 1, [Define if cgi support is enabled])
  else
    AC_MSG_RESULT(no)
  fi
],
[
    AC_MSG_RESULT(yes)
    AC_DEFINE_UNQUOTED(CGI, 1, [Define if cgi support is enabled])
])

AC_MSG_CHECKING(whether to disable access control support)
AC_ARG_ENABLE(access-control,
[  --disable-access-control Disable support for HTACCESS rules],
[
 if test "$enableval" != "no" ; then
    AC_MSG_RESULT(yes)
    ACCESSCONTROL_SOURCE="access.c"	
    access_control=true
    AC_DEFINE_UNQUOTED(ACCESS_CONTROL, 1, Support for HTACCESS rules)
  else
    AC_MSG_RESULT(no)
    access_control=false
  fi
],
[
    AC_MSG_RESULT(yes)
    ACCESSCONTROL_SOURCE="access.c"	
    access_control=true
    AC_DEFINE_UNQUOTED(ACCESS_CONTROL, 1, Support for HTACCESS rules)
])
AC_SUBST(ACCESSCONTROL_SOURCE)
AM_CONDITIONAL(ACCESS_CONTROL, test x"$access_control" != x"false")

AX_CHECK_COMPILE_FLAG([-Wno-unused], [CFLAGS="$CFLAGS -Wno-unused"], [], [])

AC_MSG_CHECKING(whether to compile and link debugging code)
AC_ARG_ENABLE(debug,
[  --enable-debug          Compile and link debugging code],
[
  if test "$enableval" = "no" ; then
    AC_MSG_RESULT(no)
    AX_CHECK_COMPILE_FLAG([-fexpensive-optimizations], [CFLAGS="$CFLAGS -fexpensive-optimizations"], [], [])
    AX_CHECK_COMPILE_FLAG([-ffast-math], [CFLAGS="$CFLAGS -ffast-math"], [], [])
    AX_CHECK_COMPILE_FLAG([-fthread-jumps], [CFLAGS="$CFLAGS -fthread-jumps"], [], [])
    AX_CHECK_COMPILE_FLAG([-funroll-loops], [CFLAGS="$CFLAGS -funroll-loops"], [], [])
    debug=0
  else
    AC_MSG_RESULT(yes)
    debug=1
  fi
],
[
    AC_MSG_RESULT(no)
    AX_CHECK_COMPILE_FLAG([-fexpensive-optimizations], [CFLAGS="$CFLAGS -fexpensive-optimizations"], [], [])
    AX_CHECK_COMPILE_FLAG([-ffast-math], [CFLAGS="$CFLAGS -ffast-math"], [], [])
    AX_CHECK_COMPILE_FLAG([-fthread-jumps], [CFLAGS="$CFLAGS -fthread-jumps"], [], [])
    AX_CHECK_COMPILE_FLAG([-funroll-loops], [CFLAGS="$CFLAGS -funroll-loops"], [], [])
    debug=0
])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"1")
AC_DEFINE([ZEND_DEBUG], [], [Zend Debug])
AC_DEFINE_UNQUOTED([ZEND_DEBUG], [$debug])

AC_MSG_CHECKING(whether to disable verbose/debug logging)
AC_ARG_ENABLE(verbose,
[  --disable-verbose       Do not enable verbose/debug logging],
[
  if test "$enableval" = "yes" ; then
    AC_MSG_RESULT(yes)
  else
    CFLAGS="$CFLAGS -DDISABLE_DEBUG"
    AC_MSG_RESULT(no)
  fi
],
[
    AC_MSG_RESULT(yes)
])

AC_MSG_CHECKING(whether to link with the Dmalloc memory debugger/profiler)
AC_ARG_WITH(dmalloc, 
[  --with-dmalloc          Link with the Dmalloc memory debugger/profiler],
[
  if test "$withval" = "yes"; then
    AC_MSG_RESULT(trying)
    AC_CHECK_LIB(dmalloc, dmalloc_shutdown)
  else
    AC_MSG_RESULT(no)
  fi
],
[
  AC_MSG_RESULT(no)
])

AC_MSG_CHECKING(whether to link with the Electric Fence memory debugger)
AC_ARG_WITH(efence, 
[  --with-efence           Link with the Electric Fence memory debugger],
[
  if test "$withval" = "yes"; then
    AC_MSG_RESULT(trying)
    AC_CHECK_LIB(efence, main)
  else
    AC_MSG_RESULT(no)
  fi
],
[
  AC_MSG_RESULT(no)
])

AC_CHECK_FUNCS(sendfile)
case $host_os in
 *linux*)
   AC_MSG_CHECKING(whether to enable the use of the sendfile(2) system call)
   AC_ARG_ENABLE(sendfile,
    [  --disable-sendfile      Disable the use of the sendfile(2) system call],
    [
      if test "$enableval" = "no" ; then
        AC_MSG_RESULT(no)
      else
        AC_MSG_RESULT(yes)
        AC_CHECK_HEADERS(sys/sendfile.h)
      fi
    ],
    [
      AC_MSG_RESULT(yes)
      AC_CHECK_HEADERS(sys/sendfile.h)
    ])
  ;;
 *) ;;
esac

#POLL_OR_SELECT

if test "$BOA_ASYNC_IO" = "poll"; then
  ASYNCIO_SOURCE="poll.c"
else
  ASYNCIO_SOURCE="select.c"
fi
AC_SUBST(ASYNCIO_SOURCE)

# "__func__" C definition
AC_DEFUN([AC_C_VAR_FUNC],
[AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether $CC recognizes __func__, ac_cv_c_var_func,
AC_TRY_COMPILE(,
[int main() {
char *s = __func__;
}],
AC_DEFINE(HAVE_FUNC,,
  [Define if the C complier supports __func__]) ac_cv_c_var_func=yes, ac_cv_c_var_func=no) )
])

# Handle --with-link-size=N
AC_ARG_WITH(link-size,
        AS_HELP_STRING([--with-link-size=N],
                       [internal link size (2, 3, or 4 allowed; default=2)]),
        , with_link_size=2)
# Check argument to --with-link-size
case "$with_link_size" in
  2|3|4) ;;
  *)
  AC_MSG_ERROR([invalid argument to --with-link-size option])
  ;;
esac
AC_DEFINE_UNQUOTED([LINK_SIZE], [$with_link_size], [
  The value of LINK_SIZE determines the number of bytes used to store
  links as offsets within the compiled regex. The default is 2, which
  allows for compiled patterns up to 64K long. This covers the vast
  majority of cases. However, PCRE can also be compiled to use 3 or 4
  bytes instead. This allows for longer patterns in extreme cases. On
  systems that support it, "configure" can be used to override this default.])

AC_DEFINE([MAX_NAME_SIZE], [32], [
  This limit is parameterized just in case anybody ever wants to
  change it. Care must be taken if it is increased, because it guards
  against integer overflow caused by enormously large patterns.])

AC_DEFINE([MAX_NAME_COUNT], [10000], [
  This limit is parameterized just in case anybody ever wants to
  change it. Care must be taken if it is increased, because it guards
  against integer overflow caused by enormously large patterns.])

AC_MSG_CHECKING(for POSIX threads)
PTHREADS_CHECK()
if test "$beos_threads" = "1"; then
  AC_DEFINE(BETHREADS, 1, Whether to use native BeOS threads)
else    
  if test "$pthreads_working" != "yes"; then
    AC_MSG_ERROR(Your system seems to lack POSIX threads.)
  fi
fi
AC_DEFINE(PTHREADS, 1, Whether to use Pthreads)

case $host_alias in
*netware*)
  PHP_OS="NetWare"
  PHP_UNAME="NetWare"
  AC_DEFINE_UNQUOTED(PHP_OS,"$PHP_OS",[hardcode for each of the cross compiler host])
  AC_DEFINE_UNQUOTED(PHP_UNAME,"$PHP_UNAME",[hardcode for each of the cross compiler host])
  ;;
*)
  PHP_UNAME=`uname -a | xargs`
  AC_DEFINE_UNQUOTED(PHP_UNAME,"$PHP_UNAME",[uname -a output])
  PHP_OS=`uname | xargs`
  AC_DEFINE_UNQUOTED(PHP_OS,"$PHP_OS",[uname output])
  ;;
esac

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
  then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/doxygen.conf])])

AM_CONDITIONAL(HAVE_INITD_DIR, [test -d /etc/rc.d])
INITD_DIR="/etc/rc.d"
AC_SUBST(INITD_DIR)

AC_CHECK_LIB([curl], [curl_global_init], HAVE_CURL=1)
AM_CONDITIONAL(HAVE_CURL, test $HAVE_CURL)


AC_OUTPUT(Makefile lib/ancillary/Makefile lib/zlib/Makefile lib/pcre/Makefile lib/tsrm/Makefile lib/oauth/Makefile lib/oauth/tests/Makefile php/Makefile src/Makefile docs/Makefile)
