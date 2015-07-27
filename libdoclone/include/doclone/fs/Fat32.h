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

#ifndef FAT32_H_
#define FAT32_H_

#include <config.h>

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

/**
 * \def DCLNAME_FAT32
 *
 * Internal name for fat32 in libdoclone.
 */
#define DCL_NAME_FAT32 "fat32"

#ifndef MNT_NAME_FAT32
/**
 * \def MNT_NAME_FAT16
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_FAT32 "vfat"
#endif

#ifndef MNTOPTIONS_FAT32
/**
 * \def MNT_OPTIONS_FAT32
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_FAT32 ""
#endif

#ifndef COMMAND_FAT32
/**
 * \def COMMAND_FAT32
 *
 * Command for formatting this fs.
 */
#define COMMAND_FAT32 "mkdosfs"
#endif

#ifndef COMMAND_OPTIONS_FAT32
/**
 * \def COMMAND_OPTIONS_FAT32
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_FAT32 "-F 32"
#endif

#ifndef ADMIN_FAT32
/**
 * \def ADMIN_FAT32
 *
 * Administration command for this fs.
 */
#define ADMIN_FAT32 ""
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_FAT32
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Fat32 filesystems.
 */
#define BLKID_REGEXP_FAT32 "^vfat$"

/**
 * \class Fat32
 * \brief Fat32 operations
 *
 * Writes uuid and label
 * \date August, 2011
 */
class Fat32 : public Filesystem {
public:
	Fat32();

	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	void checkSupport();
};
/**@}*/

}

#endif /* FAT32_H_ */
