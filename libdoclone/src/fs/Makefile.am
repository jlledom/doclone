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

noinst_LTLIBRARIES=libdcfilesystem.la

libdcfilesystem_la_SOURCES= \
	Ext2.cc \
	Ext3.cc \
	Ext4.cc \
	Fat16.cc \
	Fat32.cc \
	Hfs.cc \
	Hfsp.cc \
	Jfs.cc \
	LinuxSwap.cc \
	NoFS.cc \
	Ntfs.cc \
	Reiserfs.cc \
	Xfs.cc

libdcfilesystem_la_includedir= \
	$(includedir)/doclone/fs

libdcfilesystem_la_include_HEADERS = \
	$(top_srcdir)/include/doclone/fs/Ext2.h \
	$(top_srcdir)/include/doclone/fs/Ext3.h \
	$(top_srcdir)/include/doclone/fs/Ext4.h \
	$(top_srcdir)/include/doclone/fs/Fat16.h \
	$(top_srcdir)/include/doclone/fs/Fat32.h \
	$(top_srcdir)/include/doclone/fs/Hfs.h \
	$(top_srcdir)/include/doclone/fs/Hfsp.h \
	$(top_srcdir)/include/doclone/fs/Jfs.h \
	$(top_srcdir)/include/doclone/fs/LinuxSwap.h \
	$(top_srcdir)/include/doclone/fs/NoFS.h \
	$(top_srcdir)/include/doclone/fs/Ntfs.h \
	$(top_srcdir)/include/doclone/fs/Reiserfs.h \
	$(top_srcdir)/include/doclone/fs/Xfs.h

libdcfilesystem_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(GLIBMM_CFLAGS) \
	$(LOG4CPP_CFLAGS)

libdcfilesystem_la_LIBADD = \
	$(GLIBMM_LIBS) \
	$(E2FS_LIBS) \
	$(BLKID_LIBS) \
	$(UUID_LIBS) \
	$(LOG4CPP_LIBS)
