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

#ifndef FSFACTORY_H_
#define FSFACTORY_H_

#include <doclone/Filesystem.h>

#include <string>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \struct blkidInfo
 * \brief A couple of tags obtained with libblkid for a filesystem
 */
struct blkidInfo {
	/// The name of the filesystem
	std::string type;
	/// Secondary type of the filesystem
	std::string sec_type;
};

/**
 * \class FsFactory
 * \brief Factory class to create filesystem classes.
 *
 * Has two factory functions that receive the parted name for a filesystem and
 * return a pointer to an object with the right type.
 *
 * \date August, 2011
 */
class FsFactory {
public:
	static Filesystem *createFilesystem(const blkidInfo &info);
	static Filesystem *createFilesystem(const std::string &fsName);
};
/**@}*/

}

#endif /* FSFACTORY_H_ */
