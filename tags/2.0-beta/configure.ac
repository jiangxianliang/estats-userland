AC_PREREQ(2.65)
AC_INIT(estats_userland, 2.0)

dnl libestats libtool versioning
LIBESTATS_LT_CURRENT=2
LIBESTATS_LT_REVISION=0
LIBESTATS_LT_AGE=0

AC_SUBST(LIBESTATS_LT_CURRENT)
AC_SUBST(LIBESTATS_LT_REVISION)
AC_SUBST(LIBESTATS_LT_AGE)

dnl Estats versioning
ESTATS_VERSION=$PACKAGE_VERSION
AC_SUBST(ESTATS_VERSION)

dnl Initialize automake
AM_INIT_AUTOMAKE

dnl Specify config header
AM_CONFIG_HEADER(config.h)

dnl Check for programs
AC_CANONICAL_HOST
AC_PROG_CC
AC_ISC_POSIX
AM_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl Initialize libtool
AM_PROG_LIBTOOL

dnl Config options
AC_ARG_ENABLE(warnings,
              AC_HELP_STRING([--enable-warnings],
                             [turn on compile warnings]),
              enable_warnings=$enableval,
              enable_warnings=no)
AC_ARG_ENABLE(warnings-as-errors,
              AC_HELP_STRING([--enable-warnings-as-errors],
                             [make compile warnings errors]),
              enable_warnings_as_errors=$enableval,
              enable_warnings_as_errors=no)
AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [turn on debugging compiler options]),
              enable_debug=$enableval,
              enable_debug=no)
AC_ARG_ENABLE(deprecated-variable-warnings,
              AC_HELP_STRING([--disable-deprecated-variable-warnings],
                             [disable libestats printing warnings when attempting to access deprecated variables]),
              enable_deprecated_variable_warnings=$enableval,
              enable_deprecated_variable_warnings=yes)
AC_ARG_VAR(ESTATS_ROOT_DIR,
           [Directory where the Estats connection info is located (default=/proc/web100)])

dnl Define directories
ESTATS_DOC_DIR=${prefix}/doc/estats
ESTATS_CONF_DIR=${sysconfdir}
ESTATS_INCLUDE_DIR=${includedir}
ESTATS_IMAGE_DIR=${datadir}/estats

AC_SUBST(ESTATS_DOC_DIR)
AC_SUBST(ESTATS_CONF_DIR)
AC_SUBST(ESTATS_INCLUDE_DIR)
AC_SUBST(ESTATS_IMAGE_DIR)

if test "x$ESTATS_ROOT_DIR" = "x"; then
    ESTATS_ROOT_DIR=/proc/web100
    AC_SUBST(ESTATS_ROOT_DIR)
fi
if test ! -d "$ESTATS_ROOT_DIR"; then
    AC_MSG_WARN([$ESTATS_ROOT_DIR: not a directory])
fi

AC_DEFINE_UNQUOTED([ESTATS_ROOT_DIR], "$ESTATS_ROOT_DIR",
                   [Location of the Estats root directory])
AC_DEFINE_UNQUOTED([ESTATS_HEADER_FILE], "$ESTATS_ROOT_DIR/header",
                   [Location of the Estats header file])

dnl Checks for libraries

dnl Checks for header files
AC_CHECK_HEADER(malloc.h,
                [AC_DEFINE([HAVE_MALLOC_H], 1,
                           [Define if malloc.h is found in the system.])],
                [])

dnl Checks for typedefs
AC_TYPE_SOCKLEN_T

dnl Checks for structures
dnl Checks for compiler characteristics

if ! test "x$GCC" = "xyes"; then
    AC_MSG_ERROR(compiling Estats requires GCC)
fi

if test "x$enable_debug" = "xyes"; then
    if test -z "`echo $CFLAGS | grep "\-g" 2> /dev/null`"; then
        CFLAGS="$CFLAGS -g"
    fi
    CFLAGS=`echo $CFLAGS | sed -e 's/-O2//'`
    AC_DEFINE([DEBUG], 1,
              [Define to enable debugging when building.])
else
    AC_DEFINE([NDEBUG], 1,
              [Define to disable debugging (including asserts) when building.])
fi

if test "x$enable_warnings" = "xyes"; then
    for i in -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wextra; do
        if test -z "`echo $CFLAGS | grep "$i" 2> /dev/null`"; then
            CFLAGS="$CFLAGS $i"
        fi
    done
fi

if test "x$enable_warnings_as_errors" = "xyes"; then
    if test -z "`echo $CFLAGS | grep "\-Werror" 2> /dev/null`"; then
        CFLAGS="$CFLAGS -Werror"
    fi
fi

if test "x$enable_deprecated_variable_warnings" = "xyes"; then
    AC_DEFINE([PRINT_DEPRECATED_WARNINGS], 1,
              [Define to enable printing warnings in libestats when accessing deprecated variables.])
fi

dnl Checks for library functions
FUNC_STRLCPY

dnl Checks for system services

dnl GNU make allows us to use the $(strip ...) builtin which eliminates a
dnl large amount of extra whitespace in compile lines.
AC_MSG_CHECKING(whether make is GNU make)
STRIP_BEGIN=
STRIP_END=
if $ac_make --version 2> /dev/null | grep '^GNU Make ' > /dev/null ; then
    STRIP_BEGIN='$(strip $(STRIP_DUMMY)'
    STRIP_END=')'
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi
dnl some Make 3.79 $(strip ) versions are broken and require an empty arg
STRIP_DUMMY=
AC_SUBST(STRIP_DUMMY)
AC_SUBST(STRIP_BEGIN)
AC_SUBST(STRIP_END)

AC_CONFIG_FILES([
Makefile
compat/Makefile
doc/Makefile
doc/man/Makefile
doc/man/man1/Makefile
doc/man/man3/Makefile
doc/man/man7/Makefile
lib/Makefile
lib/estats/Makefile
util/Makefile
util/scripts/Makefile
])
AC_OUTPUT
