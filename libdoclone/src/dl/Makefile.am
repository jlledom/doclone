#
#   Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 3, or (at
#   your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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
