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

#include <doclone/fs/LinuxSwap.h>
#include <doclone/Logger.h>
#include <doclone/Util.h>

#include <doclone/exception/Exception.h>
#include <doclone/exception/WriteLabelException.h>
#include <doclone/exception/WriteUuidException.h>

#include <uuid/uuid.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \brief Initializes the attributes
 */
LinuxSwap::LinuxSwap() {
	Logger *log = Logger::getInstance();
	log->debug("LinuxSwap::LinuxSwap() start");
	
	this->_type = Doclone::FSTYPE_NONE;
	this->_mountType = Doclone::MOUNT_NATIVE;
	this->_docloneName="linux-swap";
	this->_mountName="";
	this->_mountOptions="";
	this->_command="mkswap";
	this->_formatOptions="-v1";
	this->_adminCommand="";
	this->_code = Doclone::FS_LINUXSWAP;
	
	this->checkSupport();
	
	log->debug("LinuxSwap::LinuxSwap() end");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void LinuxSwap::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("LinuxSwap::checkSupport() start");
	
	// Mounting support
	this->_mountSupport = false;
	
	// Formatting support
	if(Util::find_program_in_path(this->_command) .empty()) {
		this->_formatSupport = false;
	}
	else {
		this->_formatSupport = true;
	}
	
	// UUID and label support
	this->_uuidSupport = true;

	// Label support
	this->_labelSupport = true;
	
	log->debug("LinuxSwap::checkSupport() end");
}

/**
 * \brief Sets fs label by writing directly in the device
 *
 * \param dev
 * 		The path of the partition
 */
void LinuxSwap::writeLabel(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("LinuxSwap::writeLabel(dev=>%s) start", dev.c_str());
	try {
		Util::writeBinData(dev,this->_label.c_str(), 0x41C, 16);
	}
	catch(const Exception &e) {
		WriteLabelException ex(dev);
		ex.logMsg();
	}

	log->debug("LinuxSwap::writeLabel() end");
}

/**
 * \brief Sets fs uuid by writing directly in the device
 *
 * \param dev
 * 		The path of the partition
 */
void LinuxSwap::writeUUID(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("LinuxSwap::writeUUID(dev=>%s) start", dev.c_str());

	try {
		uuid_t binUuid;

		uuid_parse(this->_uuid.c_str(), binUuid);

		Util::writeBinData(dev, &binUuid, 0x40C, 16);
	}
	catch(const Exception &e) {
		WriteUuidException ex(dev);
		ex.logMsg();
	}

	log->debug("LinuxSwap::writeUUID() end");
}
/**@}*/

}
