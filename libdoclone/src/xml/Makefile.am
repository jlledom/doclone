# doClone - a library and front end for creating or restoring images of GNU/Linux systems.
# Copyright (C) 2015 Joan Lledó <joanlluislledo@gmail.com>
#
# See the file COPYING for copying conditions.

noinst_LTLIBRARIES=libdcxml.la

libdcxml_la_SOURCES= \
	XMLDocument.cc

libdcxml_la_includedir= \
	$(includedir)/doclone/xml

libdcxml_la_include_HEADERS = \
	$(top_srcdir)/include/doclone/xml/XMLDocument.h

libdcxml_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(XERCESC_CFLAGS)

libdcxml_la_LIBADD = \
	$(XERCESC_LIBS)
