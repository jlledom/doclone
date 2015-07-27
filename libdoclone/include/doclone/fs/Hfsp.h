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

#ifndef HFSP_H_
#define HFSP_H_

#include <config.h>

#include <doclone/Filesystem.h>

/**
 * \def DCLNAME_HFSP
 *
 * Internal name for hfs+ in libdoclone.
 */
#define DCL_NAME_HFSP "hfs+"

#ifndef MNT_NAME_HFSP
/**
 * \def MNT_NAME_HFSP
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_HFSP "hfsplus"
#endif

#ifndef MNTOPTIONS_HFSP
/**
 * \def MNT_OPTIONS_HFSP
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_HFSP ""
#endif

#ifndef COMMAND_HFSP
/**
 * \def COMMAND_HFSP
 *
 * Command for formatting this fs.
 */
#define COMMAND_HFSP "mkfs.hfsplus"
#endif

#ifndef COMMAND_OPTIONS_HFSP
/**
 * \def COMMAND_OPTIONS_HFSP
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_HFSP ""
#endif

#ifndef ADMIN_HFSP
/**
 * \def ADMIN_HFSP
 *
 * Administration command for this fs.
 */
#define ADMIN_HFSP ""
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_HFSP
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Hfs-plus filesystems.
 */
#define BLKID_REGEXP_HFSP "^hfs{1}(plus|\\+){1}$"

/**
 * \class Hfsp
 * \brief Operations for Hfsp
 *
 * Only checks the support
 * \date August, 2011
 */
class Hfsp : public Filesystem {
public:
	Hfsp();

private:
	void checkSupport();
};
/**@}*/

}

#endif /* HFSP_H_ */
