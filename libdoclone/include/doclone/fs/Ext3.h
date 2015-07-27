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

#ifndef EXT3_H_
#define EXT3_H_

#include <config.h>

#include <doclone/fs/Ext2.h>

/**
 * \def DCLNAME_EXT3
 *
 * Internal name for ext3 in libdoclone.
 */
#define DCL_NAME_EXT3 "ext3"

#ifndef MNT_NAME_EXT3
/**
 * \def MNT_NAME_EXT3
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_EXT3 "ext3"
#endif

#ifndef MNTOPTIONS_EXT3
/**
 * \def MNT_OPTIONS_EXT3
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_EXT3 ""
#endif

#ifndef COMMAND_EXT3
/**
 * \def COMMAND_EXT3
 *
 * Command for formatting this fs.
 */
#define COMMAND_EXT3 "mke2fs"
#endif

#ifndef COMMAND_OPTIONS_EXT3
/**
 * \def COMMAND_OPTIONS_EXT3
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_EXT3 "-t ext3"
#endif

#ifndef ADMIN_EXT3
/**
 * \def ADMIN_EXT3
 *
 * Administration command for this fs.
 */
#define ADMIN_EXT3 ""
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_EXT3
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Ext3 filesystems.
 */
#define BLKID_REGEXP_EXT3 "^ext3$"

/**
 * \class Ext3
 * \brief Inherited from Ext2
 * \date August, 2011
 */
class Ext3 : public Ext2 {
public:
	Ext3();

private:
	void checkSupport();
};
/**@}*/

}

#endif /* EXT3_H_ */
