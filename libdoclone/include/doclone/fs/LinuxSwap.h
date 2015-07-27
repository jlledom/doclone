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

#ifndef LINUXSWAP_H_
#define LINUXSWAP_H_

#include <config.h>

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

/**
 * \def DCLNAME_SWAP
 *
 * Internal name for linux-swap in libdoclone.
 */
#define DCL_NAME_SWAP "linux-swap"

#ifndef MNT_NAME_SWAP
/**
 * \def MNT_NAME_SWAP
 *
 * Name of the fs for the mount command.
 */
#define MNT_NAME_SWAP ""
#endif

#ifndef MNTOPTIONS_SWAP
/**
 * \def MNT_OPTIONS_SWAP
 *
 * Mounting options for this fs.
 */
#define MNT_OPTIONS_SWAP ""
#endif

#ifndef COMMAND_SWAP
/**
 * \def COMMAND_SWAP
 *
 * Command for formatting this fs.
 */
#define COMMAND_SWAP "mkswap"
#endif

#ifndef COMMAND_OPTIONS_SWAP
/**
 * \def COMMAND_OPTIONS_SWAP
 *
 * Parameters of the command for formatting.
 */
#define COMMAND_OPTIONS_SWAP "-v1"
#endif

#ifndef ADMIN_SWAP
/**
 * \def ADMIN_SWAP
 *
 * Administration command for this fs.
 */
#define ADMIN_SWAP ""
#endif

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_SWAP
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Swap filesystems.
 */
#define BLKID_REGEXP_SWAP "^(linux-)?swap{1}(\\((v1|new|v0|old)\\))?$"

/**
 * \class LinuxSwap
 * \brief Operations for Linux-Swap
 *
 * Writes UUID and label
 * \date August, 2011
 */
class LinuxSwap : public Filesystem {
public:
	LinuxSwap();

	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	void checkSupport();
};
/**@}*/

}

#endif /* LINUXSWAP_H_ */
