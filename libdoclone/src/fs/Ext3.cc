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

#include <doclone/fs/Ext3.h>
#include <doclone/Logger.h>

#include <glibmm-2.4/glibmm.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \brief Initializes the attributes
 */
Ext3::Ext3() {
	Logger *log = Logger::getInstance();
	log->debug("Ext3::Ext3() start");
	
	this->_type = Doclone::FSTYPE_UNIX;
	this->_mountType = Doclone::MOUNT_NATIVE;
	this->_docloneName="ext3";
	this->_mountName="ext3";
	this->_mountOptions="";
	this->_command="mke2fs";
	this->_formatOptions="-t ext3";
	this->_adminCommand="";
	this->_code = Doclone::FS_EXT3;
	
	this->checkSupport();
	
	log->debug("Ext3::Ext3() start");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void Ext3::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("Ext3::checkSupport() start");
	
	// Mounting support
	this->_mountSupport = true;
	
	// Formatting support
	if(Glib::find_program_in_path(this->_command) .empty()) {
		this->_formatSupport = false;
	}
	else {
		this->_formatSupport = true;
	}
	
	// UUID and label support
	this->_uuidSupport = true;
	this->_labelSupport = true;
	
	log->debug("Ext3::checkSupport() end");
}
/**@}*/

}
