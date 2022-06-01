AC_INIT([skiring], [SNAPSHOT])
AC_PREREQ([2.68])
AC_CONFIG_SRCDIR([skiring.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AC_ARG_VAR([skiring_user],[The skiring "super-user" which owns the skiring database.])
AM_CONDITIONAL([SKIRING_USER_EMPTY], [test -z "$skiring_user"])
AC_ARG_ENABLE([install-hook],
    AS_HELP_STRING([--enable-install-hook], [Enable permission changes and ring init on make install. @<:@default=yes@:>@]),
    [AS_CASE(${enableval}, [yes], [], [no], [],
             [AC_MSG_ERROR([bad value ${enableval} for --enable-install-hook])])]
             [])
AM_CONDITIONAL([ENABLE_INSTALL_HOOK], [test x$enable_install_hook != xno])
AC_OUTPUT
