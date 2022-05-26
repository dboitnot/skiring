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
AC_OUTPUT
