/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013, 2014 Joan Lledó <joanlluislledo@gmail.com>
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

#include <doclone/fs/Jfs.h>

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
Jfs::Jfs() {
	Logger *log = Logger::getInstance();
	log->debug("Jfs::Jfs() start");

	this->_type = Doclone::FSTYPE_UNIX;
	this->_mountType = Doclone::MOUNT_NATIVE;
	this->_docloneName = DCL_NAME_JFS;
	this->_mountName = MNT_NAME_JFS;
	this->_mountOptions = MNT_OPTIONS_JFS;
	this->_command = COMMAND_JFS;
	this->_formatOptions = COMMAND_OPTIONS_JFS;
	this->_adminCommand = ADMIN_JFS;
	this->_code = Doclone::FS_JFS;

	this->checkSupport();

	log->debug("Jfs::Jfs() end");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void Jfs::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("Jfs::checkSupport() start");

	// Mounting support
	this->_mountSupport = true;

	// Formatting support
	if(Util::find_program_in_path(this->_command) .empty()) {
		this->_formatSupport = false;
	}
	else {
		this->_formatSupport = true;
	}

	// UUID and label support
	if(Util::find_program_in_path(this->_adminCommand) .empty()) {
		this->_uuidSupport = false;
		this->_labelSupport = false;
	}
	else {
		this->_uuidSupport = true;
		this->_labelSupport = true;
	}

	log->debug("Jfs::checkSupport() end");
}

/**
 * \brief Sets fs label by using an external tool
 *
 * \param dev
 * 		The path of the partition
 */
void Jfs::writeLabel(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Jfs::writeLabel(dev=>%s) start", dev.c_str());

	std::string cmdline=this->_adminCommand;
	cmdline.append(" ")
	.append(dev)
	.append(" -L ")
	.append(this->_label)
	.append(" ")
	.append(">/dev/null 2>&1");

	int exitValue;
	Util::spawn_command_line_sync(cmdline, &exitValue, 0);

	if (exitValue<0) {
		WriteLabelException ex(dev);
		throw ex;
	}

	log->debug("Jfs::writeLabel() end");
}

/**
 * \brief Sets fs uuid by using an external tool
 *
 * \param dev
 * 		The path of the partition
 */
void Jfs::writeUUID(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Jfs::writeUUID(dev=>%s) start", dev.c_str());

	std::string cmdline=this->_adminCommand;
	cmdline.append(" ")
	.append(dev)
	.append(" -U ")
	.append(this->_uuid)
	.append(" ")
	.append(">/dev/null 2>&1");

	int exitValue;
	Util::spawn_command_line_sync(cmdline, &exitValue, 0);

	if (exitValue<0) {
		WriteUuidException ex(dev);
		throw ex;
	}

	log->debug("Jfs::writeUUID() end");
}
/**@}*/

}
