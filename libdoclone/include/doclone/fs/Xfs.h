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

#ifndef XFS_H_
#define XFS_H_

#include <config.h>

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

/**
 * \def DCLNAME_XFS
 *
 * Internal name for xfs in libdoclone.
 */
#define DCL_NAME_XFS "xfs"

#ifndef MNT_NAME_XFS
/**
 * \def MNT_NAME_XFS
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_XFS "xfs"
#endif

#ifndef MNTOPTIONS_XFS
/**
 * \def MNT_OPTIONS_XFS
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_XFS ""
#endif

#ifndef COMMAND_XFS
/**
 * \def COMMAND_XFS
 *
 * Command for formatting this fs.
 */
#define COMMAND_XFS "mkfs.xfs"
#endif

#ifndef COMMAND_OPTIONS_XFS
/**
 * \def COMMAND_OPTIONS_XFS
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_XFS "-f"
#endif

#ifndef ADMIN_XFS
/**
 * \def ADMIN_XFS
 *
 * Administration command for this fs.
 */
#define ADMIN_XFS "xfs_admin"
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_XFS
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Reiserfs filesystems.
 */
#define BLKID_REGEXP_XFS "^xfs$"

/**
 * \class Xfs
 * \brief Operations for Xfs
 *
 * Writes UUID and label
 * \date August, 2011
 */
class Xfs : public Filesystem {
public:
	Xfs();

	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	void checkSupport();
};
/**@}*/

}

#endif /* XFS_H_ */
