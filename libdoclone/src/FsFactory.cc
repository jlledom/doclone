/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <doclone/FsFactory.h>

#include <doclone/Logger.h>
#include <doclone/Util.h>
#include <doclone/fs/Ext2.h>
#include <doclone/fs/Ext3.h>
#include <doclone/fs/Ext4.h>
#include <doclone/fs/Fat16.h>
#include <doclone/fs/Fat32.h>
#include <doclone/fs/Hfs.h>
#include <doclone/fs/Hfsp.h>
#include <doclone/fs/Jfs.h>
#include <doclone/fs/LinuxSwap.h>
#include <doclone/fs/NoFS.h>
#include <doclone/fs/Ntfs.h>
#include <doclone/fs/Reiserfs.h>
#include <doclone/fs/Xfs.h>

namespace Doclone {

/**
 * \brief Returns an object whose class is specified by the parameter.
 *
 * The returning pointer is in the heap, and will be freed in
 * Partition::~Partition().
 *
 * \param info
 * 		A struct with the TYPE and SEC_TYPE retrieved by libblkid for a
 * 		filesystem.
 *
 * \return A pointer to a class inherited from Filesystem.
 */
Filesystem *FsFactory::createFilesystem(const blkidInfo &info) {
	Logger *log = Logger::getInstance();
	log->debug("FsFactory::createFilesystem(info=>0x%x) start", &info);

	Filesystem *fs;
	std::string fsName = info.type;
	std::string fsSecName = info.sec_type;

	if(Util::match(fsName, BLKID_REGEXP_EXT2)) {
		fs = new Ext2();
	} else if(Util::match(fsName, BLKID_REGEXP_EXT3)) {
		fs = new Ext3();
	} else if(Util::match(fsName, BLKID_REGEXP_EXT4)) {
		fs = new Ext4();
	} else if(Util::match(fsName, BLKID_REGEXP_FAT16)
			|| Util::match(fsName, BLKID_REGEXP_FAT32)) {
		if(Util::match(fsSecName, BLKID_REGEXP_SEC_TYPE_FAT16)) {
			fs = new Fat16();
		} else {
			fs = new Fat32();
		}
	}else if(Util::match(fsName, BLKID_REGEXP_HFS)) {
		fs = new Hfs();
	} else if(Util::match(fsName, BLKID_REGEXP_HFSP)) {
		fs = new Hfsp();
	} else if(Util::match(fsName, BLKID_REGEXP_JFS)) {
		fs = new Jfs();
	} else if(Util::match(fsName, BLKID_REGEXP_SWAP)) {
		fs = new LinuxSwap();
	} else if(!fsName.compare("nofs")) {
		fs = new NoFS();
	} else if(Util::match(fsName, BLKID_REGEXP_NTFS)) {
		fs = new Ntfs();
	} else if(Util::match(fsName, BLKID_REGEXP_REISERFS)) {
		fs = new Reiserfs();
	} else if(Util::match(fsName, BLKID_REGEXP_XFS)) {
		fs = new Xfs();
	} else {
		fs = new NoFS();
	}

	log->debug("FsFactory::createFilesystem(fs=>0x%x) end", fs);

	return fs;
}

/**
 * \brief Returns an object whose class is specified by the parameter.
 *
 * The returning pointer is in the heap, and will be freed in
 * Partition::~Partition().
 *
 * \param fsName
 * 		The name of the filesystem in libdoclone
 *
 * \return A pointer to a class inherited from Filesystem.
 */
Filesystem *FsFactory::createFilesystem(const std::string &fsName) {
	Logger *log = Logger::getInstance();
	log->debug("FsFactory::createFilesystem(fsName=>%s) start", fsName.c_str());

	Filesystem *fs = 0;

	if(!fsName.compare("ext2")) {
		fs = new Ext2();
	} else if(!fsName.compare("ext3")) {
		fs = new Ext3();
	} else if(!fsName.compare("ext4")) {
		fs = new Ext4();
	} else if(!fsName.compare("fat16")) {
		fs = new Fat16();
	} else if(!fsName.compare("fat32")) {
		fs = new Fat32();
	} else if(!fsName.compare("hfs")) {
		fs = new Hfs();
	} else if(!fsName.compare("hfs+")) {
		fs = new Hfsp();
	} else if(!fsName.compare("jfs")) {
		fs = new Jfs();
	} else if(!fsName.compare("linux-swap")) {
		fs = new LinuxSwap();
	} else if(!fsName.compare("nofs")) {
		fs = new NoFS();
	} else if(!fsName.compare("ntfs")) {
		fs = new Ntfs();
	} else if(!fsName.compare("reiserfs")) {
		fs = new Reiserfs();
	} else if(!fsName.compare("xfs")) {
		fs = new Xfs();
	} else {
		fs = new NoFS();
	}

	log->debug("FsFactory::createFilesystem(fs=>0x%x) end", fs);

	return fs;
}

}
