# doClone - a library and front end for creating or restoring images of GNU/Linux systems.
# Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
#
# See the file COPYING for copying conditions.

noinst_LTLIBRARIES=libdcconsoleview.la

libdcconsoleview_la_SOURCES= \
	ConsoleView.cc \
	$(top_srcdir)/include/ConsoleView/ConsoleView.h

libdcconsoleview_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libdoclone/include

libdcconsoleview_la_LIBADD = \
	../../libdoclone/src/libdoclone.la
