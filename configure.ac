
AC_PREREQ([2.63])
AC_INIT(crotalus, 2.27, support@neo-natura.com)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE(crotalus, 2.27)
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

AC_CHECK_HEADERS(sys/types.h signal.h ansidecl.h errno.h stdlib.h unistd.h sys/types.h sys/socket.h netinet/in.h arpa/inet.h netdb.h resolv.h arpa/nameser.h fcntl.h sys/time.h sys/select.h strings.h zlib.h math.h)

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
AC_CHECK_SIZEOF(long long, 8)
AC_CHECK_SIZEOF(short, 2)

AC_CHECK_TYPE([uint])

MAKEINFOHTMLFLAGS=--no-headers --no-split

AC_CONFIG_FILES([doc/NEWS])

AC_CHECK_LIB([pthread], [pthread_create], HAVE_LIBPTHREAD=1)
AM_CONDITIONAL(HAVE_LIBPTHREAD, test $HAVE_LIBPTHREAD)
if test $HAVE_LIBPTHREAD; then
  AC_DEFINE_UNQUOTED(HAVE_LIBPTHREAD, 1, [Define if libpthread can be found])
fi

#AC_CHECK_LIB([xml2], [xmlInitGlobals], HAVE_LIBXML=1)
#AM_CONDITIONAL(HAVE_LIBXML, test $HAVE_LIBXML)
#if test $HAVE_LIBXML; then
#  AC_DEFINE_UNQUOTED(HAVE_LIBXML, 1, [Define if libxml2 can be found])
#  AC_DEFINE_UNQUOTED(HAVE_SIMPLEXML, 1, [Define if libxml2 can be found])
#  AC_DEFINE_UNQUOTED(HAVE_DOM, 1, [Define if libxml2 can be found])
#fi
HAVE_LIBXML=1
AM_CONDITIONAL(HAVE_LIBXML, test $HAVE_LIBXML)
AC_DEFINE_UNQUOTED(HAVE_LIBXML, 1, [Define if libxml2 can be found])
AC_DEFINE_UNQUOTED(HAVE_SIMPLEXML, 1, [Define if libxml2 can be found])
AC_DEFINE_UNQUOTED(HAVE_DOM, 1, [Define if libxml2 can be found])

AC_CHECK_LIB([crypt], [crypt], HAVE_CRYPT=1)
AM_CONDITIONAL(HAVE_CRYPT, test $HAVE_CRYPT)
if test $HAVE_CRYPT; then
  AC_DEFINE_UNQUOTED(HAVE_CRYPT, 1, [Define if libcrypt can be found])
fi

AC_CHECK_LIB([rt], [shm_open], HAVE_LIBRT=1)
AM_CONDITIONAL(HAVE_LIBRT, test $HAVE_LIBRT)
if test $HAVE_LIBRT; then
  AC_DEFINE_UNQUOTED(HAVE_LIBRT, 1, [Define if librt can be found])
fi

AC_ARG_WITH([libshare],
  [AS_HELP_STRING([--with-libshare],
  [specify a path to the libshare build directory])],
  [],
  [with_libshare=yes])
LIBSHARE=
AS_IF([test "x$with_libshare" = "xyes"],
  AC_CHECK_LIB([share], [get_libshare_version]))
AC_SUBST([LIBSHARE], $with_libshare)
AM_CONDITIONAL([STATIC_LIBSHARE], test x"$with_libshare" != x"yes")

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
AC_CHECK_HEADERS(stdio.h stdint.h inttypes.h features.h fcntl.h sys/fcntl.h limits.h sys/time.h)
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
AC_CHECK_FUNCS(getcwd strdup strstr strcspn strtol utime symlink)
AC_CHECK_FUNCS(gethostname gethostbyname socket inet_aton herror inet_addr)
AC_CHECK_FUNCS(scandir alphasort)
AC_CHECK_FUNCS(madvise)
AC_CHECK_FUNCS(strlcpy strlcat strtok_r strerror getgrnam getgrnam_r getpwnam getpwnam_r strnlen readlink mkstemp va_copy vfprintf vsnprintf vsprintf)

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
    AX_CHECK_COMPILE_FLAG([-fthread-jumps], [CFLAGS="$CFLAGS -fthread-jumps"], [], [])
    AX_CHECK_COMPILE_FLAG([-funroll-loops], [CFLAGS="$CFLAGS -funroll-loops"], [], [])
    debug=0
])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"1")
AC_DEFINE([ZEND_DEBUG], [], [Zend Debug])
AC_DEFINE_UNQUOTED([ZEND_DEBUG], [$debug])

AC_MSG_CHECKING(whether to disable debug logging)
AC_ARG_ENABLE(debug,
[  --disable-debug       Do not enable debug logging],
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

AC_MSG_CHECKING(whether to disable the php runtime)
AC_ARG_ENABLE([php],
[  --disable-php           Do not enable the php runtime],
[
  if test "$enableval" = "yes" ; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(PHP_RUNTIME, 1, [Enable the PHP runtime])
  else
    AC_MSG_RESULT(no)
  fi
],
[
    AC_MSG_RESULT(yes)
    AC_DEFINE(PHP_RUNTIME, 1, [Enable the PHP runtime])
])
AM_CONDITIONAL(USE_PHP_RUNTIME, [test "x$enable_php" != "xno"])

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
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/doxygen.conf])])

AM_CONDITIONAL(HAVE_INITD_DIR, [test -d /etc/rc.d])
INITD_DIR="/etc/rc.d"
AC_SUBST(INITD_DIR)

AC_CHECK_LIB([curl], [curl_global_init], HAVE_CURL=1)
AM_CONDITIONAL(HAVE_CURL, test $HAVE_CURL)


VERSION_SCRIPT_FLAGS=
# lt_cv_prog_gnu_ld is from libtool 2.+
if test "$lt_cv_prog_gnu_ld" = yes; then
  VERSION_SCRIPT_FLAGS=-Wl,--version-script=
else
  case $host in
  *-*-sunos*) VERSION_SCRIPT_FLAGS="-Wl,-M -Wl,";;
  esac
fi
AC_SUBST(VERSION_SCRIPT_FLAGS)
AM_CONDITIONAL([USE_VERSION_SCRIPT], [test -n "$VERSION_SCRIPT_FLAGS"])


if test "$with_sax1" = "no" ; then
    echo Disabling the older SAX1 interface
    WITH_SAX1=0
    TEST_SAX=
else
    WITH_SAX1=1
    TEST_SAX=SAXtests
fi
AC_SUBST(WITH_SAX1)
AM_CONDITIONAL(WITH_SAX1_SOURCES, test "${WITH_TRIO}" = "1")
AC_SUBST(TEST_SAX)


if test "${NEED_TRIO}" = "1" ; then
    echo Adding trio library for string functions
    WITH_TRIO=1
else
    WITH_TRIO=0
fi
AM_CONDITIONAL(WITH_TRIO_SOURCES, test "${NEED_TRIO}" = "1")
AC_SUBST(WITH_TRIO)


THREAD_LIBS=""
BASE_THREAD_LIBS=""
WITH_THREADS=0
THREAD_CFLAGS=""
TEST_THREADS=""
THREADS_W32=""
WITH_THREAD_ALLOC=0

if test "$with_threads" = "no" ; then
    echo Disabling multithreaded support
else
    echo Enabling multithreaded support

    dnl Default to native threads on Win32
    case $host_os in
       *mingw32*) if test "$with_threads" != "pthread" && test "$with_threads" != "no"; then
               WITH_THREADS="1"
               THREADS_W32="1"
               THREAD_CFLAGS="$THREAD_CFLAGS -DHAVE_WIN32_THREADS"
           fi
       ;;
    esac

    dnl Use pthread by default in other cases
    if test -z "$THREADS_W32"; then
        if test "$with_threads" = "pthread" || test "$with_threads" = "" || test "$with_threads" = "yes" ; then
            AC_CHECK_HEADER(pthread.h,
                AC_CHECK_LIB(pthread, pthread_join,[
                THREAD_LIBS="-lpthread"
                AC_DEFINE([HAVE_LIBPTHREAD], [], [Define if pthread library is there (-lpthread)])
                AC_DEFINE([HAVE_PTHREAD_H], [], [Define if <pthread.h> is there])
                WITH_THREADS="1"]))
        fi
    fi

    case $host_os in
       *cygwin*) THREAD_LIBS=""
       ;;
       *beos*) WITH_THREADS="1"
	   THREAD_CFLAGS="$THREAD_CFLAGS -DHAVE_BEOS_THREADS"
       ;;
       *linux*)
           if test "${GCC}" = "yes" ; then
	       GCC_VERSION=`${CC} --version | head -1 | awk '{print $3}'`
	       GCC_MAJOR=`echo ${GCC_VERSION} | sed 's+\..*++'`
	       GCC_MEDIUM=`echo ${GCC_VERSION} | sed 's+[[0-9]]*\.++' | sed 's+\..*++'`
	       if test "${THREAD_LIBS}" = "-lpthread" ; then
	           if expr ${GCC_MEDIUM} \> 2 \& ${GCC_MAJOR} = 3 > /dev/null
		   then
		       THREAD_LIBS=""
		       BASE_THREAD_LIBS="-lpthread"
		   else
		   if expr ${GCC_MAJOR} \> 3 > /dev/null
		   then
		       THREAD_LIBS=""
		       BASE_THREAD_LIBS="-lpthread"
		   else
		       echo old GCC disabling weak symbols for pthread
		   fi
		   fi
	       fi
	   fi
       ;;
    esac
    if test "$WITH_THREADS" = "1" ; then
	THREAD_CFLAGS="$THREAD_CFLAGS -D_REENTRANT"
	TEST_THREADS="Threadtests"
    fi
fi
if test "$with_thread_alloc" = "yes" -a "$WITH_THREADS" = "1" ; then
    WITH_THREAD_ALLOC=1
fi

AC_SUBST(THREAD_LIBS)
AC_SUBST(BASE_THREAD_LIBS)
AC_SUBST(WITH_THREADS)
AC_SUBST(THREAD_CFLAGS)
AC_SUBST(TEST_THREADS)
AC_SUBST(WITH_THREAD_ALLOC)
AM_CONDITIONAL([THREADS_W32],[test -n "$THREADS_W32"])


dnl Checking whether va_list is an array type
AC_TRY_COMPILE([
#include <stdarg.h>
void a(va_list * ap) {}],[
va_list ap1, ap2; a(&ap1); ap2 = (va_list) ap1],[
  AC_MSG_RESULT(no)],[
  AC_MSG_RESULT(yes)
  AC_DEFINE([VA_LIST_IS_ARRAY], [1],[Define if va_list is an array type])])

dnl Checks for inet libraries:
AC_SEARCH_LIBS(gethostent, [nsl])
AC_SEARCH_LIBS(setsockopt, [socket net network])
AC_SEARCH_LIBS(connect, [inet])

dnl Checking if send() second argument is const.
AC_MSG_CHECKING([for const send() second argument])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/socket.h>]],
    [[(void)send(1,(const char *)"",1,1);]])],
have_send_const_arg2=yes,
have_send_const_arg2=no)
AC_MSG_RESULT($have_send_const_arg2)
if test x"$have_send_const_arg2" = x"yes"; then
    AC_DEFINE([SEND_ARG2_CAST], [],
      [Type cast for the send() function 2nd arg])
else
    AC_DEFINE([SEND_ARG2_CAST], [(char *)],
      [Type cast for the send() function 2nd arg])
fi



# derived from libxml2
AC_ARG_WITH(regexps,
[  --with-regexps          add Regular Expressions support (on)])
AC_ARG_WITH(run_debug,
[  --with-run-debug        add the runtime debugging module (off)])
AC_ARG_WITH(sax1,
[  --with-sax1             add the older SAX1 interface (on)])
AC_ARG_WITH(schematron,
[  --with-schematron       add Schematron support (on)])
AC_ARG_WITH(thread-alloc,
[  --with-thread-alloc     add per-thread memory(off)])
AC_ARG_WITH(tree,
[  --with-tree             add the DOM like tree manipulation APIs (on)])
AC_ARG_WITH(valid,
[  --with-valid            add the DTD validation support (on)])
AC_ARG_WITH(writer,
[  --with-writer           add the xmlWriter saving interface (on)])
AC_ARG_WITH(xpath,
[  --with-xpath            add the XPATH support (on)])
AC_ARG_WITH(xptr,
[  --with-xptr             add the XPointer support (on)])


dnl hard dependancies on options
dnl
if test "$with_schemas" = "yes"
then
    with_pattern=yes
    with_regexps=yes
fi
if test "$with_schematron" = "yes"
then
    with_pattern=yes
    with_tree=yes
    with_xpath=yes
fi
if test "$with_reader" = "yes"
then
    with_push=yes
fi
if test "$with_xptr" = "yes"
then
    with_xpath=yes
fi
#minimal
if test "$with_c14n" = ""
then
  with_c14n=no
fi
if test "$with_catalog" = ""
then
  with_catalog=no
fi
echo So far so good!
if test "$with_debug" = ""
then
  with_debug=no
fi
if test "$with_docbook" = ""
then
  with_docbook=no
fi
if test "$with_fexceptions" = ""
then
  with_fexceptions=no
fi
if test "$with_ftp" = ""
then
  with_ftp=no
fi
if test "$with_history" = ""
then
  with_history=no
fi
if test "$with_html" = ""
then
  with_html=no
fi
if test "$with_http" = ""
then
  with_http=no
fi
if test "$with_iconv" = ""
then
  with_iconv=no
fi
if test "$with_iso8859x" = ""
then
  with_iso8859x=no
fi
if test "$with_legacy" = ""
then
  with_legacy=no
fi
if test "$with_mem_debug" = ""
then
  with_mem_debug=no
fi
if test "$with_output" = ""
then
  with_output=no
fi
if test "$with_pattern" = ""
then
  with_pattern=no
fi
if test "$with_push" = ""
then
  with_push=no
fi
if test "$with_python" = ""
then
  with_python=no
fi
if test "$with_reader" = ""
then
  with_reader=no
fi
if test "$with_readline" = ""
then
  with_readline=no
fi
if test "$with_tree" = ""
then
  with_tree=no
fi
if test "$with_valid" = ""
then
  with_valid=no
fi
if test "$with_writer" = ""
then
  with_writer=no
fi

AC_CONFIG_HEADERS([lib/libxml2/include/libxml/xmlversion.h])


AC_OUTPUT(Makefile lib/libxml2/Makefile lib/ancillary/Makefile lib/zlib/Makefile lib/pcre/Makefile lib/tsrm/Makefile lib/oauth/Makefile lib/oauth/tests/Makefile php/Makefile src/Makefile src/crphp/Makefile doc/Makefile)
