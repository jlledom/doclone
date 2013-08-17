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

#include <doclone/fs/Xfs.h>
#include <doclone/Logger.h>

#include <doclone/exception/Exception.h>
#include <doclone/exception/WriteLabelException.h>
#include <doclone/exception/WriteUuidException.h>

#include <glibmm-2.4/glibmm.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \brief Initializes the attributes
 */
Xfs::Xfs() {
	Logger *log = Logger::getInstance();
	log->debug("Xfs::Xfs() start");
	
	this->_type = Doclone::FSTYPE_UNIX;
	this->_mountType = Doclone::MOUNT_NATIVE;
	this->_docloneName="xfs";
	this->_mountName="xfs";
	this->_mountOptions="";
	this->_command="mkfs.xfs";
	this->_formatOptions="-f";
	this->_adminCommand="xfs_admin";
	this->_code = Doclone::FS_XFS;
	
	this->checkSupport();
	
	log->debug("Xfs::Xfs() end");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void Xfs::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("Xfs::checkSupport() start");
	
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
	if(Glib::find_program_in_path(this->_adminCommand).empty()) {
		this->_uuidSupport = false;
		this->_labelSupport = false;
	}
	else {
		this->_uuidSupport = true;
		this->_labelSupport = true;
	}
	
	log->debug("Xfs::checkSupport() end");
}

/**
 * \brief Sets fs label by using external tool
 *
 * \param dev
 * 		The path of the partition
 */
void Xfs::writeLabel(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Xfs::writeLabel(dev=>%s) start", dev.c_str());
	
	std::string cmdline=this->_adminCommand;
	cmdline.append(" -L ")
	.append(this->_label)
	.append(" ")
	.append(dev)
	.append(" ")
	.append(">/dev/null 2>&1");
	
	int exitValue;
	Glib::spawn_command_line_sync( "sh -c '" + cmdline + 
										"'", 0, 0, &exitValue );
	if (exitValue<0) {
		WriteLabelException ex(dev);
		ex.logMsg();
	}
	
	log->debug("Xfs::writeLabel() end");
}

/**
 * \brief Sets fs uuid by using an external tool
 *
 * \param dev
 * 		The path of the partition
 */
void Xfs::writeUUID(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Xfs::writeUUID(dev=>%s) start", dev.c_str());
	
	std::string cmdline=this->_adminCommand;
	cmdline.append(" -U ")
	.append(this->_uuid)
	.append(" ")
	.append(dev)
	.append(">/dev/null 2>&1");
	
	int exitValue;
	Glib::spawn_command_line_sync( "sh -c '" + cmdline + 
										"'", 0, 0, &exitValue );
	if (exitValue<0) {
		WriteUuidException ex(dev);
		ex.logMsg();
	}
	
	log->debug("Xfs::writeUUID() end");
}
/**@}*/

}
