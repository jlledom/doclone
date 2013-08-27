# doClone - a library and front end for creating or restoring images of GNU/Linux systems.
# Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
#
# See the file COPYING for copying conditions.

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
