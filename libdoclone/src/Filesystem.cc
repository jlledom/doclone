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

#include <doclone/Filesystem.h>

#include <string.h>

#include <string>

#include <blkid/blkid.h>

#include <doclone/Logger.h>

namespace Doclone {

/**
 * \brief Initializes the attributes
 */
Filesystem::Filesystem()
	: _mountType(), _type(), _code(), _label(), _uuid(), _docloneName(),
	  _mountName(), _mountOptions(), _command(), _formatOptions(),
	  _adminCommand(), _mountSupport(), _formatSupport(), _uuidSupport(),
	  _labelSupport(){
}

// Getters and setters
Doclone::mountType Filesystem::getMountType() const {
	return this->_mountType;
}

Doclone::fsType Filesystem::getType() const {
	return this->_type;
}
Doclone::fsCode Filesystem::getCode() const {
	return this->_code;
}
void Filesystem::setLabel(const std::string &label) {
	this->_label = label;
}
void Filesystem::setUUID(const std::string &uuid) {
	this->_uuid = uuid;
}
const std::string &Filesystem::getdocloneName() const {
	return this->_docloneName;
}
const std::string &Filesystem::getMountName() const {
	return this->_mountName;
}
const std::string &Filesystem::getMountOptions() const {
	return this->_mountOptions;
}
const std::string &Filesystem::getCommand() const {
	return this->_command;
}
const std::string &Filesystem::getFormatOptions() const {
	return this->_formatOptions;
}
bool Filesystem::getMountSupport() const {
	return this->_mountSupport;
}
bool Filesystem::getFormatSupport() const {
	return this->_formatSupport;
}
bool Filesystem::getUUIDSupport() const {
	return this->_uuidSupport;
}
bool Filesystem::getLabelSupport() const {
	return this->_labelSupport;
}
const std::string &Filesystem::getMountPoint() const {
	return this->_mountPoint;
}
void Filesystem::setMountPoint(const std::string &path) {
	this->_mountPoint = path;
}

/**
 * \brief Reads a filesystem label by using libblkid
 *
 * \param dev
 * 		The path of the device where filesystem is
 */
std::string Filesystem::readLabel(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Filesystem::readLabel(dev=>%s) start", dev.c_str());

	std::string label;
	const char *type, *value;
	blkid_cache cache = 0;

	blkid_get_cache(&cache, "/dev/null"); //Do not use blkid cache
	blkid_dev blDev = blkid_get_dev(cache, dev.c_str(), BLKID_DEV_NORMAL);
	if(!blDev) {
		return "";
	}
	blkid_tag_iterate iter = blkid_tag_iterate_begin(blDev);

	while (blkid_tag_next(iter, &type, &value) == 0) {
		if (!strcmp(type, "LABEL")) {
			label = value;
		}
	}
	blkid_tag_iterate_end(iter);
	blkid_put_cache(cache);

	log->debug("Filesystem::readLabel(label=>%s) end", label.c_str());
	return label;
}

/**
 * \brief Reads a filesystem uuid by using libblkid
 *
 * \param dev
 * 		The path of the device where filesystem is
 */
std::string Filesystem::readUUID(const std::string &dev) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Filesystem::readUUID(dev=>%s) start", dev.c_str());

	std::string uuid;
	const char *type, *value;
	blkid_cache cache = 0;

	blkid_get_cache(&cache, "/dev/null"); //Do not use blkid cache
	blkid_dev blDev = blkid_get_dev(cache, dev.c_str(), BLKID_DEV_NORMAL);
	if(!blDev) {
		log->debug("Filesystem::readUUID(uuid=>) end");
		return "";
	}
	blkid_tag_iterate iter = blkid_tag_iterate_begin(blDev);

	while (blkid_tag_next(iter, &type, &value) == 0) {
		if (!strcmp(type, "UUID")) {
			uuid = value;
		}
	}
	blkid_tag_iterate_end(iter);
	blkid_put_cache(cache);

	log->debug("Filesystem::readUUID(uuid=>%s) end", uuid.c_str());
	return uuid;
}

}
