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

#include <doclone/fs/Fat32.h>
#include <doclone/Logger.h>
#include <doclone/Util.h>

#include <doclone/exception/Exception.h>
#include <doclone/exception/WriteLabelException.h>
#include <doclone/exception/WriteUuidException.h>

#include <endian.h>
#include <glibmm-2.4/glibmm.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \brief Initializes the attributes
 */
Fat32::Fat32() {
	Logger *log = Logger::getInstance();
	log->debug("Fat32::Fat32() start");
	
	this->_type = Doclone::FSTYPE_DOS;
	this->_mountType = Doclone::MOUNT_NATIVE;
	this->_docloneName="fat32";
	this->_mountName="vfat";
	this->_mountOptions="";
	this->_command="mkdosfs";
	this->_formatOptions="-F 32";
	this->_adminCommand="";
	this->_code = Doclone::FS_FAT32;
	
	this->checkSupport();
	
	log->debug("Fat32::Fat32() end");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void Fat32::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("Fat32::checkSupport() start");
	
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
	
	log->debug("Fat32::checkSupport() end");
}

/**
 * \brief Sets fs label by writing directly in the device
 *
 * \param dev
 * 		The path of the partition
 */
void Fat32::writeLabel(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Fat32::writeLabel(dev=>%s) start", dev.c_str(), this->_label.c_str());
	
	try {
		Util::writeBinData(dev, this->_label.c_str(), 0x47, 11);
	}
	catch(const Exception &e) {
		WriteLabelException ex(dev);
		ex.logMsg();
	}
	
	log->debug("Fat32::writeLabel() end");
}

/**
 * \brief Sets fs uuid by writing directly in the device
 *
 * \param dev
 * 		The path of the partition
 */
void Fat32::writeUUID(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Fat32::writeUUID(dev=>%s) start", dev.c_str());
	
	try {
		// Delete the hyphen from the Vol ID. Example, 2CB4-B7C5 -> 2CB4B7C5
		std::string result;
		result.resize(this->_uuid.size());
		std::remove_copy(this->_uuid.begin(),
				this->_uuid.end(), result.begin(), '-');

		ssize_t size = result.size();
		char resultUuid[size];
		result.copy(resultUuid, size);

		char *pos = resultUuid; //Pointer to scour the string
		unsigned int valSize = size >> 1;
		unsigned char val[valSize]; // Integer value of the ID
		while( *pos ) {
			if( !((pos-resultUuid) & 1) ) { // If the position is even
				sscanf(pos, "%02x",
						reinterpret_cast<unsigned int*>(&val[(pos-resultUuid) >> 1]));
			}
			++pos;
		}

		/*
		 * The Fat32 vol. id must be written in little endian, but the original
		 * id retrieved by blkid is in big endian, so we must convert it.
		 */
		uint32_t *p = reinterpret_cast<uint32_t *>(val);
		uint32_t tmpInt = Util::swapEndian(*p);
		Util::writeBinData(dev, &tmpInt, 0x43, valSize);
	}
	catch(const Exception &e) {
		WriteUuidException ex(dev);
		ex.logMsg();
	}
	
	log->debug("Fat32::writeUUID() end");
}
/**@}*/

}
