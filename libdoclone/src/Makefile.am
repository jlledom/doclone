# doClone - a library and front end for creating or restoring images of GNU/Linux systems.
# Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
#
# See the file COPYING for copying conditions.

SUBDIRS = fs dl .

noinst_LTLIBRARIES= \
	libdcexception.la \
	libdcobserver.la

lib_LTLIBRARIES=libdoclone.la

libdcexception_la_SOURCES= \
	$(top_srcdir)/include/doclone/exception/AlignPartitionException.h \
	$(top_srcdir)/include/doclone/exception/BrokenPipeException.h \
	$(top_srcdir)/include/doclone/exception/CancelException.h \
	$(top_srcdir)/include/doclone/exception/CloseConnectionException.h \
	$(top_srcdir)/include/doclone/exception/CloseFileException.h \
	$(top_srcdir)/include/doclone/exception/CommitException.h \
	$(top_srcdir)/include/doclone/exception/ConnectionException.h \
	$(top_srcdir)/include/doclone/exception/CreateFileException.h \
	$(top_srcdir)/include/doclone/exception/CreateImageException.h \
	$(top_srcdir)/include/doclone/exception/CreatePartitionException.h \
	$(top_srcdir)/include/doclone/exception/ErrorException.h \
	$(top_srcdir)/include/doclone/exception/Exception.h \
	$(top_srcdir)/include/doclone/exception/FileNotFoundException.h \
	$(top_srcdir)/include/doclone/exception/FormatException.h \
	$(top_srcdir)/include/doclone/exception/GrubException.h \
	$(top_srcdir)/include/doclone/exception/InvalidImageException.h \
	$(top_srcdir)/include/doclone/exception/MakeLabelException.h \
	$(top_srcdir)/include/doclone/exception/MountException.h \
	$(top_srcdir)/include/doclone/exception/NoAccessToDeviceException.h \
	$(top_srcdir)/include/doclone/exception/NoBlockDeviceException.h \
	$(top_srcdir)/include/doclone/exception/NoDeviceDriverRecognizedException.h \
	$(top_srcdir)/include/doclone/exception/NoFitInDeviceException.h \
	$(top_srcdir)/include/doclone/exception/NoFsToolFoundException.h \
	$(top_srcdir)/include/doclone/exception/NoLabelSupportException.h \
	$(top_srcdir)/include/doclone/exception/NoMountSupportException.h \
	$(top_srcdir)/include/doclone/exception/NoSelinuxSupportException.h \
	$(top_srcdir)/include/doclone/exception/NoUuidSupportException.h \
	$(top_srcdir)/include/doclone/exception/OpenFileException.h \
	$(top_srcdir)/include/doclone/exception/ReadDataException.h \
	$(top_srcdir)/include/doclone/exception/ReadErrorsInDirectoryException.h \
	$(top_srcdir)/include/doclone/exception/ReceiveDataException.h \
	$(top_srcdir)/include/doclone/exception/RestoreImageException.h \
	$(top_srcdir)/include/doclone/exception/SendDataException.h \
	$(top_srcdir)/include/doclone/exception/SigAbrtException.h \
	$(top_srcdir)/include/doclone/exception/SignalCaughtException.h \
	$(top_srcdir)/include/doclone/exception/SpawnProcessException.h \
	$(top_srcdir)/include/doclone/exception/TooMuchPartitionsException.h \
	$(top_srcdir)/include/doclone/exception/UmountException.h \
	$(top_srcdir)/include/doclone/exception/WarningException.h \
	$(top_srcdir)/include/doclone/exception/WriteDataException.h \
	$(top_srcdir)/include/doclone/exception/WriteErrorsInDirectoryException.h \
	$(top_srcdir)/include/doclone/exception/WriteLabelException.h \
	$(top_srcdir)/include/doclone/exception/WriteUuidException.h \
	$(top_srcdir)/include/doclone/exception/WrongImageTypeException.h

libdcexception_la_includedir= \
	$(includedir)/doclone/exception

libdcexception_la_include_HEADERS = \
	$(top_srcdir)/include/doclone/exception/AlignPartitionException.h \
	$(top_srcdir)/include/doclone/exception/BrokenPipeException.h \
	$(top_srcdir)/include/doclone/exception/CancelException.h \
	$(top_srcdir)/include/doclone/exception/CloseConnectionException.h \
	$(top_srcdir)/include/doclone/exception/CloseFileException.h \
	$(top_srcdir)/include/doclone/exception/CommitException.h \
	$(top_srcdir)/include/doclone/exception/ConnectionException.h \
	$(top_srcdir)/include/doclone/exception/CreateFileException.h \
	$(top_srcdir)/include/doclone/exception/CreateImageException.h \
	$(top_srcdir)/include/doclone/exception/CreatePartitionException.h \
	$(top_srcdir)/include/doclone/exception/ErrorException.h \
	$(top_srcdir)/include/doclone/exception/Exception.h \
	$(top_srcdir)/include/doclone/exception/FileNotFoundException.h \
	$(top_srcdir)/include/doclone/exception/FormatException.h \
	$(top_srcdir)/include/doclone/exception/GrubException.h \
	$(top_srcdir)/include/doclone/exception/InvalidImageException.h \
	$(top_srcdir)/include/doclone/exception/MakeLabelException.h \
	$(top_srcdir)/include/doclone/exception/MountException.h \
	$(top_srcdir)/include/doclone/exception/NoAccessToDeviceException.h \
	$(top_srcdir)/include/doclone/exception/NoBlockDeviceException.h \
	$(top_srcdir)/include/doclone/exception/NoDeviceDriverRecognizedException.h \
	$(top_srcdir)/include/doclone/exception/NoFitInDeviceException.h \
	$(top_srcdir)/include/doclone/exception/NoFsToolFoundException.h \
	$(top_srcdir)/include/doclone/exception/NoLabelSupportException.h \
	$(top_srcdir)/include/doclone/exception/NoMountSupportException.h \
	$(top_srcdir)/include/doclone/exception/NoSelinuxSupportException.h \
	$(top_srcdir)/include/doclone/exception/NoUuidSupportException.h \
	$(top_srcdir)/include/doclone/exception/OpenFileException.h \
	$(top_srcdir)/include/doclone/exception/ReadDataException.h \
	$(top_srcdir)/include/doclone/exception/ReadErrorsInDirectoryException.h \
	$(top_srcdir)/include/doclone/exception/ReceiveDataException.h \
	$(top_srcdir)/include/doclone/exception/RestoreImageException.h \
	$(top_srcdir)/include/doclone/exception/SendDataException.h \
	$(top_srcdir)/include/doclone/exception/SigAbrtException.h \
	$(top_srcdir)/include/doclone/exception/SignalCaughtException.h \
	$(top_srcdir)/include/doclone/exception/SpawnProcessException.h \
	$(top_srcdir)/include/doclone/exception/TooMuchPartitionsException.h \
	$(top_srcdir)/include/doclone/exception/UmountException.h \
	$(top_srcdir)/include/doclone/exception/WarningException.h \
	$(top_srcdir)/include/doclone/exception/WriteDataException.h \
	$(top_srcdir)/include/doclone/exception/WriteErrorsInDirectoryException.h \
	$(top_srcdir)/include/doclone/exception/WriteLabelException.h \
	$(top_srcdir)/include/doclone/exception/WriteUuidException.h \
	$(top_srcdir)/include/doclone/exception/WrongImageTypeException.h

libdcobserver_la_SOURCES= \
	$(top_srcdir)/include/doclone/observer/AbstractObserver.h \
	$(top_srcdir)/include/doclone/observer/AbstractSubject.h

libdcobserver_la_includedir= \
	$(includedir)/doclone/observer

libdcobserver_la_include_HEADERS = \
	$(top_srcdir)/include/doclone/observer/AbstractObserver.h \
	$(top_srcdir)/include/doclone/observer/AbstractSubject.h

libdoclone_la_SOURCES= \
	AbstractSubject.cc \
	Clone.cc \
	clone.cc \
	DataTransfer.cc \
	Disk.cc \
	DiskLabel.cc \
	DlFactory.cc \
	DosFS.cc \
	Filesystem.cc \
	FsFactory.cc \
	Grub.cc \
	Image.cc \
	Link.cc \
	Local.cc \
	Logger.cc \
	Net.cc \
	Operation.cc \
	PartedDevice.cc \
	Partition.cc \
	Unicast.cc \
	UnixFS.cc \
	Util.cc \
	$(top_srcdir)/include/doclone/Clone.h \
	$(top_srcdir)/include/doclone/clone.h \
	$(top_srcdir)/include/doclone/DataTransfer.h \
	$(top_srcdir)/include/doclone/Disk.h \
	$(top_srcdir)/include/doclone/DiskLabel.h \
	$(top_srcdir)/include/doclone/DlFactory.h \
	$(top_srcdir)/include/doclone/DosFS.h \
	$(top_srcdir)/include/doclone/Filesystem.h \
	$(top_srcdir)/include/doclone/FsFactory.h \
	$(top_srcdir)/include/doclone/Grub.h \
	$(top_srcdir)/include/doclone/Image.h \
	$(top_srcdir)/include/doclone/Link.h \
	$(top_srcdir)/include/doclone/Local.h \
	$(top_srcdir)/include/doclone/Logger.h \
	$(top_srcdir)/include/doclone/Net.h \
	$(top_srcdir)/include/doclone/Operation.h \
	$(top_srcdir)/include/doclone/PartedDevice.h \
	$(top_srcdir)/include/doclone/Partition.h \
	$(top_srcdir)/include/doclone/Unicast.h \
	$(top_srcdir)/include/doclone/UnixFS.h \
	$(top_srcdir)/include/doclone/Util.h

libdoclone_la_includedir= \
	$(includedir)/doclone

libdoclone_la_include_HEADERS = \
	$(top_srcdir)/include/doclone/Clone.h \
	$(top_srcdir)/include/doclone/clone.h \
	$(top_srcdir)/include/doclone/DataTransfer.h \
	$(top_srcdir)/include/doclone/Disk.h \
	$(top_srcdir)/include/doclone/DosFS.h \
	$(top_srcdir)/include/doclone/Filesystem.h \
	$(top_srcdir)/include/doclone/FsFactory.h \
	$(top_srcdir)/include/doclone/Grub.h \
	$(top_srcdir)/include/doclone/Image.h \
	$(top_srcdir)/include/doclone/Link.h \
	$(top_srcdir)/include/doclone/Local.h \
	$(top_srcdir)/include/doclone/Logger.h \
	$(top_srcdir)/include/doclone/Net.h \
	$(top_srcdir)/include/doclone/Operation.h \
	$(top_srcdir)/include/doclone/PartedDevice.h \
	$(top_srcdir)/include/doclone/Partition.h \
	$(top_srcdir)/include/doclone/Unicast.h \
	$(top_srcdir)/include/doclone/UnixFS.h \
	$(top_srcdir)/include/doclone/Util.h

libdoclone_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-DLOCALEDIR=\"$(localedir)\" \
	-DLOGDIR=\"$(logdir)\" \
	$(GLIBMM_CFLAGS) \
	$(LOG4CPP_CFLAGS)

libdoclone_la_LIBADD = \
	fs/libdcfilesystem.la \
	dl/libdcdisklabel.la \
	$(PARTED_LIBS) \
	$(GLIBMM_LIBS) \
	$(LOG4CPP_LIBS) \
	$(LIBINTL)

if USE_SELINUX
libdoclone_la_LIBADD += $(SELINUX_LIBS)
endif

