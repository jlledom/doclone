dnl doClone - a library and front end for creating or restoring images of GNU/Linux systems.
dnl Copyright (C) 2013, 2015 Joan Lledó <joanlluislledo@gmail.com>
dnl
dnl See the file COPYING for copying conditions.

AC_INIT([doclone],[0.8.0], [joanlluislledo@gmail.com])
AM_INIT_AUTOMAKE([1.14 parallel-tests -Wall -Werror])
AM_SILENT_RULES
AM_PROG_AR
LT_INIT
AC_PROG_CXX
AM_GNU_GETTEXT_VERSION([0.19.3])
AM_GNU_GETTEXT([external])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_SUBDIRS([libdoclone])

AC_CONFIG_FILES([
	Makefile \
	src/Makefile \
	src/ConsoleView/Makefile \
	po/Makefile.in \
	man/Makefile
	])
AC_OUTPUT
