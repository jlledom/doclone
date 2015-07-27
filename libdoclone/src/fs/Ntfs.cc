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

#include <doclone/fs/Ntfs.h>

#include <endian.h>

#include <algorithm>

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
Ntfs::Ntfs() {
	Logger *log = Logger::getInstance();
	log->debug("Ntfs::Ntfs() start");

	this->_type = Doclone::FSTYPE_DOS;
	this->_mountType = Doclone::MOUNT_EXTERNAL;
	this->_docloneName = DCL_NAME_NTFS;
	this->_mountName = MNT_NAME_NTFS;
	this->_mountOptions = MNT_OPTIONS_NTFS;
	this->_command = COMMAND_NTFS;
	this->_formatOptions = COMMAND_OPTIONS_NTFS;
	this->_adminCommand = ADMIN_NTFS;
	this->_code = Doclone::FS_NTFS;

	this->checkSupport();

	log->debug("Ntfs::Ntfs() end");
}

/**
 * \brief Checks the level of support of this filesystem
 */
void Ntfs::checkSupport() {
	Logger *log = Logger::getInstance();
	log->debug("Ntfs::checkSupport() start");

	// Mounting support
	if(Util::find_program_in_path("mount."+this->_mountName).empty()) {
		this->_mountSupport = false;
	}
	else {
		this->_mountSupport = true;
	}

	// Formatting support
	if(Util::find_program_in_path(this->_command).empty()) {
		this->_formatSupport = false;
	}
	else {
		this->_formatSupport = true;
	}

	// UUID support
	this->_uuidSupport = true;

	// Label support
	if(Util::find_program_in_path(this->_adminCommand).empty()) {
		this->_labelSupport = false;
	}
	else {
		this->_labelSupport = true;
	}

	log->debug("Ntfs::checkSupport() end");
}

/**
 * \brief Sets fs label by using external tool
 *
 * \param dev
 * 		The path of the partition
 */
void Ntfs::writeLabel(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Ntfs::writeLabel(dev=>%s) start", dev.c_str());

	std::string cmdline=this->_adminCommand;
	cmdline.append(" ")
	.append(dev)
	.append(" ")
	.append(this->_label)
	.append(" ")
	.append(">/dev/null 2>&1");

	int exitValue;
	Util::spawn_command_line_sync(cmdline, &exitValue, 0);

	if (exitValue<0) {
		WriteLabelException ex(dev);
		throw ex;
	}

	log->debug("Ntfs::writeLabel() end");
}

/**
 * \brief Sets fs uuid by writing directly in the device
 *
 * \param dev
 * 		The path of the partition
 */
void Ntfs::writeUUID(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Ntfs::writeUUID(dev=>%s) start", dev.c_str());

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
		 * The Ntfs vol. id must be written in little endian, but the original
		 * id retrieved by blkid is in big endian, so we must convert it.
		 */
		uint64_t *p = reinterpret_cast<uint64_t *>(val);
		uint64_t tmpInt = Util::swapEndian(*p);
		Util::writeBinData(dev, &tmpInt, 0x48, valSize);
	}
	catch(const Exception &e) {
		WriteUuidException ex(dev);
		throw ex;
	}

	log->debug("Ntfs::writeUUID() end");
}
/**@}*/

}
