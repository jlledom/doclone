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

#ifndef NTFS_H_
#define NTFS_H_

#include <config.h>

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

/**
 * \def DCLNAME_NTFS
 *
 * Internal name for ntfs in libdoclone.
 */
#define DCL_NAME_NTFS "ntfs"

#ifndef MNT_NAME_NTFS
/**
 * \def MNT_NAME_SWAP
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_NTFS "ntfs"
#endif

#ifndef MNTOPTIONS_NTFS
/**
 * \def MNT_OPTIONS_NTFS
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_NTFS ""
#endif

#ifndef COMMAND_NTFS
/**
 * \def COMMAND_NTFS
 *
 * Command for formatting this fs.
 */
#define COMMAND_NTFS "mkntfs"
#endif

#ifndef COMMAND_OPTIONS_NTFS
/**
 * \def COMMAND_OPTIONS_NTFS
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_NTFS "-Q"
#endif

#ifndef ADMIN_NTFS
/**
 * \def ADMIN_NTFS
 *
 * Administration command for this fs.
 */
#define ADMIN_NTFS "ntfslabel"
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_NTFS
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Ntfs filesystems.
 */
#define BLKID_REGEXP_NTFS "^ntfs$"

/**
 * \class Ntfs
 * \brief Operations for Ntfs
 *
 * Writes UUID and label
 * \date August, 2011
 */
class Ntfs : public Filesystem {
public:
	Ntfs();

	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	void checkSupport();
};
/**@}*/

}

#endif /* NTFS_H_ */
