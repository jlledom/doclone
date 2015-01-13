/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013, 2015 Joan Lledó <joanlluislledo@gmail.com>
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

#include <doclone/fs/Ext2.h>

#include <ext2fs/ext2fs.h>
#include <uuid/uuid.h>

#include <doclone/Logger.h>
#include <doclone/Util.h>
#include <doclone/exception/Exception.h>
#include <doclone/exception/WriteLabelException.h>
#include <doclone/exception/WriteUuidException.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \brief Initializes the attributes
 */
Ext2::Ext2() {
	Logger *log = Logger::getInstance();
	log->debug("Ext2::Ext2() start");
	
	this->_type = Doclone::FSTYPE_UNIX;
	this->_mountType = Doclone::MOUNT_NATIVE;
	this->_docloneName="ext2";
	this->_mountName="ext2";
	this->_mountOptions="";
	this->_command="mke2fs";
	this->_formatOptions="-t ext2";
	this->_adminCommand="";
	this->_code = Doclone::FS_EXT2;
	
	this->checkSupport();
	
	log->debug("Ext2::Ext2() end");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void Ext2::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("Ext2::checkSupport() start");
	
	// Mounting support
	this->_mountSupport = true;
	
	// Formatting support
	if(Util::find_program_in_path(this->_command).empty()) {
		this->_formatSupport = false;
	}
	else {
		this->_formatSupport = true;
	}
	
	// UUID and label support
	this->_uuidSupport = true;
	this->_labelSupport = true;
	
	log->debug("Ext2::checkSupport() end");
}

/**
 * \brief Writes ext2/3/4 fs label by using e2fslibs
 *
 * \param dev
 * 		The path of the partition
 */
void Ext2::writeLabel(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Ext2::writeLabel(dev=>%s) start", dev.c_str());
	
	ext2_filsys fs;
	ext2_super_block *sb;
	dgrp_t i;
	int set_csum = 0;

	ext2fs_open2 (dev.c_str(), 0, EXT2_FLAG_RW | EXT2_FLAG_JOURNAL_DEV_OK,
				  0, 0, unix_io_manager, &fs);

	if (!fs) {
		WriteLabelException ex(dev);
		ex.logMsg();
	}
	
	sb = fs->super;
	fs->flags &= ~EXT2_FLAG_MASTER_SB_ONLY;
	fs->flags |= EXT2_FLAG_SUPER_ONLY;

	if (sb->s_feature_ro_compat &
		EXT4_FEATURE_RO_COMPAT_GDT_CSUM) {
		/*
		 * Determine if the block group checksums are
		 * correct so we know whether or not to set
		 * them later on.
		 */
		for (i = 0; i < fs->group_desc_count; i++)
			if (!ext2fs_group_desc_csum_verify(fs, i))
				break;
		if (i >= fs->group_desc_count)
			set_csum = 1;
	}

	memset(sb->s_volume_name, 0, sizeof(sb->s_volume_name));
	Util::safe_strncpy(sb->s_volume_name, this->_label.c_str(),
			sizeof(sb->s_volume_name));

	if (set_csum) {
		for (i = 0; i < fs->group_desc_count; i++)
			ext2fs_group_desc_csum_set(fs, i);
		fs->flags &= ~EXT2_FLAG_SUPER_ONLY;
	}

	ext2fs_mark_super_dirty(fs);
	ext2fs_close(fs);
	
	log->debug("Ext2::writeLabel() end");
}

/**
 * \brief Writes ext2/3/4 fs uuid by using e2fslibs
 *
 * \param dev
 * 		The path of the partition
 */
void Ext2::writeUUID(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Ext2::writeUUID(dev=>%s) start", dev.c_str());
	
	ext2_filsys fs;
	ext2_super_block *sb;
	dgrp_t i;
	int set_csum = 0;

	ext2fs_open2 (dev.c_str(), 0, EXT2_FLAG_RW | EXT2_FLAG_JOURNAL_DEV_OK,
				  0, 0, unix_io_manager, &fs);

	if (!fs) {
		WriteUuidException ex(dev);
		ex.logMsg();
	}
	
	sb = fs->super;
	fs->flags &= ~EXT2_FLAG_MASTER_SB_ONLY;
	fs->flags |= EXT2_FLAG_SUPER_ONLY;
	
	if (sb->s_feature_ro_compat &
		EXT4_FEATURE_RO_COMPAT_GDT_CSUM) {
		/*
		 * Determine if the block group checksums are
		 * correct so we know whether or not to set
		 * them later on.
		 */
		for (i = 0; i < fs->group_desc_count; i++)
			if (!ext2fs_group_desc_csum_verify(fs, i))
				break;
		if (i >= fs->group_desc_count)
			set_csum = 1;
	}
	
	// Write the uuid
	if (uuid_parse (this->_uuid.c_str(), sb->s_uuid)<0) {
		WriteUuidException ex(dev);
		ex.logMsg();
	}
	
	if (set_csum) {
		for (i = 0; i < fs->group_desc_count; i++)
			ext2fs_group_desc_csum_set(fs, i);
		fs->flags &= ~EXT2_FLAG_SUPER_ONLY;
	}
	
	ext2fs_mark_super_dirty (fs);
	ext2fs_close(fs);
	
	log->debug("Ext2::writeUUID() end");
}
/**@}*/

}
