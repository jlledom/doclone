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

#include <config.h>

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

/**
 * \def DCLNAME_JFS
 *
 * Internal name for jfs in libdoclone.
 */
#define DCL_NAME_JFS "jfs"

#ifndef MNT_NAME_JFS
/**
 * \def MNT_NAME_JFS
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_JFS "jfs"
#endif

#ifndef MNTOPTIONS_JFS
/**
 * \def MNT_OPTIONS_JFS
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_JFS ""
#endif

#ifndef COMMAND_JFS
/**
 * \def COMMAND_JFS
 *
 * Command for formatting this fs.
 */
#define COMMAND_JFS "jfs_mkfs"
#endif

#ifndef COMMAND_OPTIONS_JFS
/**
 * \def COMMAND_OPTIONS_JFS
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_JFS "-q"
#endif

#ifndef ADMIN_JFS
/**
 * \def ADMIN_JFS
 *
 * Administration command for this fs.
 */
#define ADMIN_JFS "jfs_tune"
#endif

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
