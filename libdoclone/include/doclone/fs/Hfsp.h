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

#include <doclone/UnixFS.h>

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
class Hfsp : public UnixFS {
public:
	Hfsp();

private:
	void checkSupport();
};
/**@}*/

}

#endif /* HFSP_H_ */
