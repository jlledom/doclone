dnl doClone - a library to creating or restoring images of GNU/Linux systems.
dnl Copyright (C) 2013, 2015 Joan Lledó <joanlluislledo@gmail.com>
dnl
dnl See the file COPYING for copying conditions.

AC_INIT([libdoclone],[0.7.1], [joanlluislledo@gmail.com])
AM_INIT_AUTOMAKE([1.11 parallel-tests -Wall -Werror])
AM_SILENT_RULES
LT_INIT
AC_PROG_CXX
AM_GNU_GETTEXT_VERSION([0.18.1])
AM_GNU_GETTEXT([external])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AC_TRY_RUN([
#include <stdlib.h>

int main () {
	int retVal = -1;

	if(__STDC_IEC_559__) {
		retVal = 0;
	} else {
		retVal = 1;
	}

    return retVal;
}
], [], AC_MSG_ERROR([The implementation is not IEEE-754 compliant]))

PKG_CHECK_MODULES([PARTED], [libparted >= 2.2])
PKG_CHECK_MODULES([E2FS], [ext2fs >= 1.41.3])
PKG_CHECK_MODULES([UUID], [uuid >= 1.41.3])
PKG_CHECK_MODULES([BLKID], [blkid >= 1.41.3])
PKG_CHECK_MODULES([ARCHIVE], [libarchive >= 3.0.4])
PKG_CHECK_MODULES([XERCESC], [xerces-c >= 3.1.1])
PKG_CHECK_MODULES([LOG4CPP], [log4cpp >= 1.0])

# Allow alternate log directory
logdir="${localstatedir}/log/libdoclone"
AC_ARG_WITH(logdir,
	AS_HELP_STRING([--with-logdir=DIR],
		       [system logging directory [[LOCALSTATEDIR/log/libdoclone]]]),
[case "$with_logdir" in
      "") AC_MSG_ERROR([invalid logdir specified]) ;;
      *) logdir="$with_logdir" ;;
esac])
AC_SUBST(logdir)

AC_CONFIG_FILES([
	Makefile \
	src/Makefile  \
	src/dl/Makefile \
	src/fs/Makefile \
	src/xml/Makefile \
	po/Makefile.in \
	examples/Makefile \
	examples/README \
	libdoclone.pc
	])
AC_OUTPUT
