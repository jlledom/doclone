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

#ifndef FAT16_H_
#define FAT16_H_

#include <string>

#include <doclone/Filesystem.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_FAT16
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Fat16 filesystems.
 */
#define BLKID_REGEXP_FAT16 "^vfat$"

/**
 * \def BLKID_REGEXP_SEC_TYPE_FAT16
 * Regular expression that matches with the SEC_TYPE tag of vfat for Fat16
 * filesystems.
 */
#define BLKID_REGEXP_SEC_TYPE_FAT16 "^msdos$"

/**
 * \class Fat16
 * \brief Operations for Fat16
 *
 * Writes UUID and label
 * \date August, 2011
 */
class Fat16 : public Filesystem {
public:
	Fat16();

	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	void checkSupport();
};
/**@}*/

}

#endif /* FAT16_H_ */
