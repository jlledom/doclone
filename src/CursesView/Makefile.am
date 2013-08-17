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

noinst_LTLIBRARIES=libdccursesview.la

libdccursesview_la_SOURCES= \
	ActionPanel.cc \
	BaseScreen.cc \
	CursesView.cc \
	InsertFileForm.cc \
	InsertIpForm.cc \
	InsertNumOfClientsForm.cc \
	LanMenu.cc \
	LanRoleMenu.cc \
	LocalMenu.cc \
	Menu.cc \
	ProgressScreen.cc \
	ScopeMenu.cc \
	ScreenFactory.cc \
	SelectDeviceForm.cc \
	SelectFileForm.cc \
	SummaryScreen.cc \
	TargetMenu.cc \
	TextInput.cc \
	Util.cc \
	$(top_srcdir)/include/CursesView/ActionPanel.h \
	$(top_srcdir)/include/CursesView/BaseScreen.h \
	$(top_srcdir)/include/CursesView/CursesView.h \
	$(top_srcdir)/include/CursesView/InsertFileForm.h \
	$(top_srcdir)/include/CursesView/InsertIpForm.h \
	$(top_srcdir)/include/CursesView/InsertNumOfClientsForm.h \
	$(top_srcdir)/include/CursesView/CursesWidget.h \
	$(top_srcdir)/include/CursesView/LanMenu.h \
	$(top_srcdir)/include/CursesView/LanRoleMenu.h \
	$(top_srcdir)/include/CursesView/LocalMenu.h \
	$(top_srcdir)/include/CursesView/Menu.h \
	$(top_srcdir)/include/CursesView/ProgressScreen.h \
	$(top_srcdir)/include/CursesView/ScopeMenu.h \
	$(top_srcdir)/include/CursesView/ScreenFactory.h \
	$(top_srcdir)/include/CursesView/SelectDeviceForm.h \
	$(top_srcdir)/include/CursesView/SelectFileForm.h \
	$(top_srcdir)/include/CursesView/SummaryScreen.h \
	$(top_srcdir)/include/CursesView/TargetMenu.h \
	$(top_srcdir)/include/CursesView/TextInput.h \
	$(top_srcdir)/include/CursesView/Util.h

libdccursesview_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libdoclone/include

libdccursesview_la_LIBADD = \
	../../libdoclone/src/libdoclone.la \
	-lmenuw \
	-lformw \
	-lpanelw \
	-lncursesw
