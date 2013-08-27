# doClone - a library and front end for creating or restoring images of GNU/Linux systems.
# Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
#
# See the file COPYING for copying conditions.

noinst_LTLIBRARIES=libdcdisklabel.la

libdcdisklabel_la_SOURCES= \
	Aix.cc \
	Amiga.cc \
	Bsd.cc \
	Dvh.cc \
	Gpt.cc \
	Loop.cc \
	Mac.cc \
	Msdos.cc \
	Pc98.cc \
	Sun.cc

libdcdisklabel_la_includedir= \
	$(includedir)/doclone/dl

libdcdisklabel_la_include_HEADERS = \
	$(top_srcdir)/include/doclone/dl/Aix.h \
	$(top_srcdir)/include/doclone/dl/Amiga.h \
	$(top_srcdir)/include/doclone/dl/Bsd.h \
	$(top_srcdir)/include/doclone/dl/Dvh.h \
	$(top_srcdir)/include/doclone/dl/Gpt.h \
	$(top_srcdir)/include/doclone/dl/Loop.h \
	$(top_srcdir)/include/doclone/dl/Mac.h \
	$(top_srcdir)/include/doclone/dl/Msdos.h \
	$(top_srcdir)/include/doclone/dl/Pc98.h \
	$(top_srcdir)/include/doclone/dl/Sun.h

libdcdisklabel_la_CPPFLAGS = \
	-I$(top_srcdir)/include

libdcdisklabel_la_LIBADD = \
	$(PARTED_LIBS)
