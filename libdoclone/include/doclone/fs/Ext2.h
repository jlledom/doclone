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

#ifndef EXT2_H_
#define EXT2_H_

#include <string>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \def BLKID_REGEXP_EXT2
 * Regular expression that matches with all the TYPE constants of libblkid
 * for Ext2 filesystems.
 */
#define BLKID_REGEXP_EXT2 "^ext2$"

/**
 * \class Ext2
 * \brief Ext2 operations.
 *
 * Functions to write the label and uuid of a ext2/3/4 filesystem
 * \date August, 2011
 */
class Ext2 : public Filesystem {
public:
	Ext2();

	void writeLabel(const std::string &dev) const throw(Exception);
	void writeUUID(const std::string &dev) const throw(Exception);

private:
	virtual void checkSupport();
};
/**@}*/

}

#endif /* EXT2_H_ */
