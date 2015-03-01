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

#ifndef JFS_H_
#define JFS_H_

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_JFS
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Jfs filesystems.
 */
#define BLKID_REGEXP_JFS "^jfs$"

/**
 * \class Jfs
 * \brief Operations for Jfs
 *
 * Writes UUID and label
 * \date August, 2011
 */
class Jfs : public Filesystem {
public:
	Jfs();
	
	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	void checkSupport();
};
/**@}*/

}

#endif /* JFS_H_ */
