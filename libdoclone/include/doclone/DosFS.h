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

#ifndef DOSFS_H_
#define DOSFS_H_

#include <doclone/Filesystem.h>

#include <doclone/exception/Exception.h>

#include <stdint.h>
#include <sys/types.h>

#include <string>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \struct Doclone::dosFile
 * \brief Info for each file in the partition
 *
 * This struct contains some info needed to restore the file correctly.
 * doclone currently does not support the cloning of the attributes of DOS
 * files.
 *
 * All this data will be sent over the network, because of this, all its
 * elements have fixed size.
 */
struct dosFile {
	/// The name of the file
	uint8_t name[512];
	/// The permissions
	uint32_t mode;
	/// Size on disk (in bytes)
	uint64_t size;
	/// Last access time
	uint64_t atime;
	/// Last modification time
	uint64_t mtime;
	/// Some empty space to add future data keeping the compatibility of the image
	uint8_t dummy[512];
};


/**
 * \class DosFS
 * \brief DOS type filesystem
 * \date August, 2011
 */
class DosFS : public Filesystem {
public:
	DosFS(){}

	void readDir(const std::string &path) throw(Exception);
	void writeDir(const std::string &path) throw(Exception);

private:
	ssize_t readFileHeader(Doclone::dosFile &file) const throw(Exception);
	ssize_t writeFileHeader(const Doclone::dosFile &file) const throw(Exception);

	void readRegularFile(const std::string &path, const struct stat &filestat) const throw(Exception);
	void writeRegularFile(const std::string &path, const Doclone::dosFile &file) const throw(Exception);

	void goToNextDir() const throw(Exception);
	void goToNextFile(off_t fileOffset, uint64_t fileSize) const throw(Exception);
};
/**@}*/

}

#endif /* DOSFS_H_ */
