dnl doClone - a library and front end for creating or restoring images of GNU/Linux systems.
dnl Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
dnl
dnl See the file COPYING for copying conditions.

AC_INIT([doclone],[0.7.0], [joanlluislledo@gmail.com])
AM_INIT_AUTOMAKE([1.11 parallel-tests -Wall -Werror])
AM_SILENT_RULES
LT_INIT
AC_PROG_CXX
AM_GNU_GETTEXT_VERSION([0.18.1])
AM_GNU_GETTEXT([external])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_SUBDIRS([libdoclone])

AC_CHECK_LIB([ncurses], [initscr], [HAVE_NCURSES=1], [HAVE_NCURSES=0])
AC_CHECK_LIB([ncursesw], [initscr], [HAVE_NCURSES=1], [HAVE_NCURSES=0])

AM_CONDITIONAL([USE_NCURSES], [test "$HAVE_NCURSES" -eq 1])
AM_COND_IF([USE_NCURSES], [AC_DEFINE([HAVE_NCURSES], [1], [Define if using libncurses.])],
           [AC_MSG_WARN(Libncurses is not installed. The curses GUI will not be built.)])
           
AC_CONFIG_FILES([
	Makefile \
	src/Makefile \
	src/ConsoleView/Makefile \
	src/CursesView/Makefile \
	po/Makefile.in \
	man/Makefile
	])
AC_OUTPUT
