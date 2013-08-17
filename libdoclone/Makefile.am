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

SUBDIRS= po src examples
ACLOCAL_AMFLAGS = -I m4 --install

pcdir = $(libdir)/pkgconfig
pc_DATA = libdoclone.pc

docdir = $(datarootdir)/doc/@PACKAGE@

doc_DATA = \
	README

EXTRA_DIST = \
	config.rpath \
	README

install-exec-hook:
	test -z "$(logdir)" || $(MKDIR_P) "$(logdir)"
