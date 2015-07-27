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

#ifndef EXT4_H_
#define EXT4_H_

#include <config.h>

#include <doclone/fs/Ext2.h>

/**
 * \def DCLNAME_EXT4
 *
 * Internal name for ext4 in libdoclone.
 */
#define DCL_NAME_EXT4 "ext4"

#ifndef MNT_NAME_EXT4
/**
 * \def MNT_NAME_EXT4
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_EXT4 "ext4"
#endif

#ifndef MNTOPTIONS_EXT4
/**
 * \def MNT_OPTIONS_EXT4
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_EXT4 ""
#endif

#ifndef COMMAND_EXT4
/**
 * \def COMMAND_EXT4
 *
 * Command for formatting this fs.
 */
#define COMMAND_EXT4 "mke2fs"
#endif

#ifndef COMMAND_OPTIONS_EXT4
/**
 * \def COMMAND_OPTIONS_EXT4
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_EXT4 "-t ext4 -O extent"
#endif

#ifndef ADMIN_EXT4
/**
 * \def ADMIN_EXT4
 *
 * Administration command for this fs.
 */
#define ADMIN_EXT4 ""
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_EXT4
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Ext4 filesystems.
 */
#define BLKID_REGEXP_EXT4 "^ext4{1}(dev)?$"

/**
 * \class Ext4
 * \brief Inherited from Ext2
 * \date August, 2011
 */
class Ext4 : public Ext2 {
public:
	Ext4();

private:
	void checkSupport();
};
/**@}*/

}

#endif /* EXT4_H_ */
