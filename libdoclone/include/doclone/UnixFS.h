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

#ifndef UNIXFS_H_
#define UNIXFS_H_

#include <stdint.h>
#include <sys/types.h>

#include <string>
#include <map>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \struct Doclone::unixFile
 * \brief Info for each file in a partition
 *
 * This struct contains some info necessary to restore the file correctly.
 *
 * All this data will be sent over the network, because of this, all its
 * elements have fixed size.
 */
struct unixFile {
	/// The name of the file
	uint8_t name[512];
	/// The permissions and the kind of file (block, regular, link...)
	uint32_t mode;
	/// File owner
	uint32_t uid;
	/// File group
	uint32_t gid;
	/// Size in bytes on disk
	uint64_t size;
	/// Last access time
	uint64_t atime;
	/// Last modification time
	uint64_t mtime;
	/// Selinux attributes
	uint8_t se_xattr[128];
	/// Major & minor, just in case the file were a block device
	uint64_t dev;
	/// Some empty space to add future data keeping the compatibility of the image
	uint8_t dummy[512];
};

/**
 * \class UnixFS
 * \brief Unix type filesystem
 * \date August, 2011
 */
class UnixFS : public Filesystem {
public:
	UnixFS();

	void readDir(const std::string &path) throw(Exception);
	void writeDir(const std::string &path) throw(Exception);

private:
	ssize_t readFileHeader(Doclone::unixFile &file) const throw(Exception);
	ssize_t writeFileHeader(const Doclone::unixFile &file) const throw(Exception);

	void readRegularFile(const std::string &path, const struct stat &filestat) const throw(Exception);
	void readDevice(const std::string &path, const struct stat &filestat) const throw(Exception);
	void readSymlink(const std::string &path, const struct stat &filestat) const throw(Exception);
	void readHardLink(const std::string &path, const struct stat &filestat) throw(Exception);
	void readPipe(const std::string &path, const struct stat &filestat) const throw(Exception);
	
	void writeRegularFile(const std::string &path, Doclone::unixFile &file) const throw(Exception);
	void writeDevice(const std::string &path, Doclone::unixFile &file) const throw(Exception);
	void writeSymlink(const std::string &path, Doclone::unixFile &file) const throw(Exception);
	void writeHardLink(const std::string &path, Doclone::unixFile &file) const throw(Exception);
	void writePipe(const std::string &path, Doclone::unixFile &file) const throw(Exception);
	
	void goToNextDir() const throw(Exception);
	void goToNextFile(off_t fileOffset, uint64_t fileSize) const throw(Exception);

	/// For management of hard links
	std::map<ino_t, std::string> _hardLinks;
};
/**@}*/

}

#endif /* UNIXFS_H_ */
