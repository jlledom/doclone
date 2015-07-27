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

#include <doclone/fs/Ext4.h>

#include <doclone/Logger.h>
#include <doclone/Util.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \brief Initializes the attributes
 */
Ext4::Ext4() {
	Logger *log = Logger::getInstance();
	log->debug("Ext4::Ext4() start");

	this->_type = Doclone::FSTYPE_UNIX;
	this->_mountType = Doclone::MOUNT_NATIVE;
	this->_docloneName = DCL_NAME_EXT4;
	this->_mountName = MNT_NAME_EXT4;
	this->_mountOptions = MNT_OPTIONS_EXT4;
	this->_command = COMMAND_EXT4;
	this->_formatOptions = COMMAND_OPTIONS_EXT4;
	this->_adminCommand = ADMIN_EXT4;
	this->_code = Doclone::FS_EXT4;

	this->checkSupport();

	log->debug("Ext4::Ext4() end");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void Ext4::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("Ext4::checkSupport() start");

	// Mounting support
	this->_mountSupport = true;

	// Formatting support
	if(Util::find_program_in_path(this->_command) .empty()) {
		this->_formatSupport = false;
	}
	else {
		this->_formatSupport = true;
	}

	// uuid and label support
	this->_uuidSupport = true;
	this->_labelSupport = true;

	log->debug("Ext4::checkSupport() end");
}
/**@}*/

}
