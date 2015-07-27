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

#ifndef REISERFS_H_
#define REISERFS_H_

#include <config.h>

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

/**
 * \def DCLNAME_REISERFS
 *
 * Internal name for reiserfs in libdoclone.
 */
#define DCL_NAME_REISERFS "reiserfs"

#ifndef MNT_NAME_REISERFS
/**
 * \def MNT_NAME_REISERFS
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_REISERFS "reiserfs"
#endif

#ifndef MNTOPTIONS_REISERFS
/**
 * \def MNT_OPTIONS_REISERFS
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_REISERFS ""
#endif

#ifndef COMMAND_REISERFS
/**
 * \def COMMAND_REISERFS
 *
 * Command for formatting this fs.
 */
#define COMMAND_REISERFS "mkreiserfs"
#endif

#ifndef COMMAND_OPTIONS_REISERFS
/**
 * \def COMMAND_OPTIONS_REISERFS
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_REISERFS "-f"
#endif

#ifndef ADMIN_REISERFS
/**
 * \def ADMIN_REISERFS
 *
 * Administration command for this fs.
 */
#define ADMIN_REISERFS "reiserfstune"
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_REISERFS
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Reiserfs filesystems.
 */
#define BLKID_REGEXP_REISERFS "^reiserfs$"

/**
 * \class Reiserfs
 * \brief Operations for Reiserfs
 * Writes UUID and label
 * \date August, 2011
 */
class Reiserfs : public Filesystem {
public:
	Reiserfs();

	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	void checkSupport();
};
/**@}*/

}

#endif /* REISERFS_H_ */
