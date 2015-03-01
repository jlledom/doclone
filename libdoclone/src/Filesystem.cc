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
#include <sys/types.h>
#include <dirent.h>

#include <string>

#include <blkid/blkid.h>
#include <archive.h>
#include <archive_entry.h>

#include <doclone/Logger.h>
#include <doclone/DataTransfer.h>
#include <doclone/Util.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/FileNotFoundException.h>
#include <doclone/exception/ReadErrorsInDirectoryException.h>
#include <doclone/exception/WriteErrorsInDirectoryException.h>

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
		blkid_put_cache(cache);
		log->debug("Filesystem::readLabel(label=>) end");
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
		blkid_put_cache(cache);
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

void Filesystem::readDir(struct archive *in, struct archive *out,
		struct archive_entry_linkresolver *lResolv,
		const std::string &path, const std::string &imgRootDir) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("Filesystem::readDir(in=>0x%x, out=>0x%x, lResolv=>0x%x, path=>%s, imgRootDir) start",
			in, out, lResolv, path.c_str(), imgRootDir.c_str());

	DIR *directory;
	struct dirent *d_file; // a file in *directory
	struct archive_entry *entry;
	bool errors = false;

	if ((directory = opendir (path.c_str())) == 0) {
		FileNotFoundException ex(path);
		throw ex;
	}

	while ((d_file = readdir (directory)) != 0) {
		struct stat filestat;
		std::string abPath;

		abPath = path;
		abPath.append(d_file->d_name);

		//Path of the file in the image
		std::string relPath = imgRootDir + abPath.substr(this->_mountPoint.length());

		if (lstat (abPath.c_str(), &filestat) < 0) {
			FileNotFoundException ex(abPath);
			throw ex;
		}

		try {
			int fdin = open (abPath.c_str(), O_RDONLY);

			entry = archive_entry_new();
			archive_entry_update_pathname_utf8(entry, relPath.c_str());
			archive_read_disk_entry_from_file(in, entry, fdin, &filestat);

			switch (archive_entry_filetype(entry)) {
			case AE_IFDIR: {
				if (strcmp (".", d_file->d_name) && strcmp ("..", d_file->d_name)) {

					archive_write_header(out, entry);

					/*
					 * If the current folder is a virtual one or is the mount
					 * point of another partition, bypass it.
					 */
					if(Util::isVirtualDirectory(abPath.c_str())
						|| Util::isMountPoint(abPath)) {

						continue;
					}

					abPath.push_back('/');
					this->readDir(in, out, lResolv, abPath.c_str(), imgRootDir);
				}
				break;
			}
			case AE_IFLNK: {
				char linkPath[4096] = {};
				ssize_t size = 0;

				// Read link
				if ((size = readlink (abPath.c_str(), linkPath, sizeof(linkPath))) < 0) {
					FileNotFoundException ex(path);
					throw ex;
				} else {
						archive_entry_update_symlink_utf8(entry, linkPath);
						archive_write_header(out, entry);
				}

				break;
			}
			case AE_IFIFO:
			case AE_IFSOCK:
			case AE_IFCHR:
			case AE_IFBLK:
			case AE_IFREG: {
				struct archive_entry *sparse;
				if(archive_entry_nlink(entry) > 1) {
					archive_entry_linkify(lResolv, &entry, &sparse);
				}
				if(entry != 0) {
					archive_write_header(out, entry);
				}

				/*
				 * If the current file is a virtual one, bypass
				 */
				if(Util::isLiveFile(abPath.c_str())) {
					continue;
				}

				// else
				if(archive_entry_size(entry) > 0) {
					DataTransfer *trns = DataTransfer::getInstance();
					trns->fdToArchive(fdin, out);
				}

				break;
			}
			default:
				ReadDataException ex;
				throw ex;
			}

			archive_entry_free(entry);
			close(fdin);
		}catch(const WarningException &ex) {
			errors = true;
		}
	}

	closedir (directory);

	if(errors) {
		ReadErrorsInDirectoryException ex(path);
		ex.logMsg();
	}

	log->loopDebug("Filesystem::readDir() end");
}

void Filesystem::writeDir(struct archive *in, struct archive *out,
		const std::string &path, const std::string &imgRootDir) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("Filesystem::writeDir(in=>0x%x, out=>0x%x, path=>%s, imgRootDir=>%s) start",
			in, out, path.c_str(), imgRootDir.c_str());

	std::string abPath;
	bool errors = false;
	struct archive_entry *entry;

	try {
		while(archive_read_next_header(in, &entry) == ARCHIVE_OK) {
			abPath = this->_mountPoint;
			abPath.append(archive_entry_pathname(entry));

			//Remove the image root dir from the restoration path
			abPath.replace(abPath.find(imgRootDir), imgRootDir.length(), "");

			archive_entry_update_pathname_utf8(entry, abPath.c_str());
			if(archive_entry_hardlink(entry)!=0 && archive_entry_size(entry)==0) {
					std::string hardLinkPath = this->_mountPoint;
					hardLinkPath.append(archive_entry_hardlink(entry));
					archive_entry_update_hardlink_utf8(entry, hardLinkPath.c_str());
			}
			archive_write_header(out, entry);

			DataTransfer *trns = DataTransfer::getInstance();
			trns->copyData(in, out);
		}
	} catch(const WarningException &ex) {
		errors = true;
	}

	if(errors) {
		WriteErrorsInDirectoryException ex(path);
		ex.logMsg();
	}

	log->loopDebug("Filesystem::writeDir() end");
}

}
