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

#ifndef ALIGNPARTITIONEXCEPTION_H_
#define ALIGNPARTITIONEXCEPTION_H_

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class AlignPartitionException
 * \brief The partition cannot be aligned to MiB.
 *
 * Libdoclone tries to align all partitions to MiB, in order to make sure they
 * begins always in a multiple of any sector size. MiB alignment also provides
 * enhanced performance when used with RAID systems and with Solid State Drives.
 * \date August, 2011
 */
class AlignPartitionException: public WarningException {
public:
	AlignPartitionException() throw() {
		this->_msg=D_("The partition cannot be aligned to MiB");
	}

};
/**@}*/

}

#endif /* ALIGNPARTITIONEXCEPTION_H_ */
