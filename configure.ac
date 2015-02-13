
AC_PREREQ([2.63])
AC_INIT(crotalus, 2.24, support@neo-natura.com)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE(crotalus, 2.24)
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

AC_CANONICAL_HOST

AC_PROG_INSTALL

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
#AC_C_VAR_FUNC

MAKEINFOHTMLFLAGS = --no-headers --no-split

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

AC_CHECK_LIB([pthread], [pthread_self], HAVE_PTHREAD=1)
AM_CONDITIONAL(HAVE_PTHREAD, test $HAVE_PTHREAD)

AC_CHECK_LIB([libxml2], [xmlInitGlobals], HAVE_LIBXML=1)
AM_CONDITIONAL(HAVE_LIBXML, test $HAVE_LIBXML)

AC_SEARCH_LIBS(socket, socket net)
AC_SEARCH_LIBS(inet_aton, resolv)
AC_SEARCH_LIBS(gethostname, nsl)
AC_SEARCH_LIBS(gethostbyname, nsl)

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h sys/fcntl.h limits.h sys/time.h)
AC_CHECK_HEADERS(getopt.h unistd.h sys/resource.h)

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

#if test -n "$GCC"; then
#  dnl if we are running gcc, use -pipe
#  test -n "$GCC" && CFLAGS="$CFLAGS -pipe"
#
#  AC_MSG_CHECKING(compile and link profiling code)
#  AC_ARG_ENABLE(profiling,
#  [  --enable-profiling      Compile and link profiling code],
#  [
#    if test "$enableval" = "yes" ; then
#      AC_MSG_RESULT(yes)
#      CFLAGS="$CFLAGS -pg"
#      LDFLAGS="$LDFLAGS -g -pg"    
#    else
#      AC_MSG_RESULT(no)
#    fi
#  ],
#  [
#    AC_MSG_RESULT(no)
#  ])
#fi

AC_MSG_CHECKING(whether to enable gunzip support)
AC_ARG_ENABLE(gunzip,
[  --disable-gunzip        Disable use of gunzip],
[
 if test "$enableval" = "yes" ; then
    AC_MSG_RESULT(yes)
    AC_PATH_PROG(GUNZIP, gunzip)
    AC_DEFINE_UNQUOTED(GUNZIP, "$ac_cv_path_GUNZIP", [Define if gunzip can be found])
  else
    AC_MSG_RESULT(no)
  fi
],
[
    AC_MSG_RESULT(yes)
    AC_PATH_PROG(GUNZIP, gunzip)
    AC_DEFINE_UNQUOTED(GUNZIP, "$ac_cv_path_GUNZIP", [Define if gunzip can be found])
])

AC_MSG_CHECKING(whether to enable access control support)
AC_ARG_ENABLE(access-control,
[  --enable-access-control Enable support for allow/deny rules],
[
 if test "$enableval" = "yes" ; then
    AC_MSG_RESULT(yes)
    CFLAGS="$CFLAGS -DACCESS_CONTROL"
    ACCESSCONTROL_SOURCE="access.c"	
  else
    AC_MSG_RESULT(no)
  fi
],
[
    AC_MSG_RESULT(no)
])
AC_SUBST(ACCESSCONTROL_SOURCE)

AC_MSG_CHECKING(whether to compile and link debugging code)
AC_ARG_ENABLE(debug,
[  --disable-debug         Do not compile and link debugging code],
[
  if test "$enableval" = "yes" ; then
    AC_MSG_RESULT(yes)
    LDFLAGS="$LDFLAGS -g"
    test -n "$GCC" && CFLAGS="$CFLAGS -Wall"
  else
    AC_MSG_RESULT(no)
  fi
],
[
    AC_MSG_RESULT(yes)
    LDFLAGS="$LDFLAGS -g"
    test -n "$GCC" && CFLAGS="$CFLAGS -Wall"
])

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
        AC_CHECK_FUNCS(sendfile)
      fi
    ],
    [
      AC_MSG_RESULT(yes)
      AC_CHECK_HEADERS(sys/sendfile.h)
      AC_CHECK_FUNCS(sendfile)
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


# there are three scenarios
# GNU make is installed as "make"
# GNU make is installed as something else we detected
# GNU make is not installed
# Unfortunately, we can't deal with it one way or the other
# Trying multiple AC_OUTPUT confuses autoconf, and using variables
# *in* AC_OUTPUT is even worse.
# *so*, make a default makefile that just forces make to call gmake
# or whatever.

AC_OUTPUT(Makefile php/Makefile src/Makefile docs/Makefile)
