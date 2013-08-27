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

#ifndef NODEVICEDRIVERRECOGNIZEDEXCEPTION_H_
#define NODEVICEDRIVERRECOGNIZEDEXCEPTION_H_

#include <stdint.h>

#include <sstream>

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class NoDeviceDriverRecognizedException
 * \brief Libdoclone can't determine which device driver is being used.
 *
 * If this exception occurs, the library will work as if using the scsi driver,
 * with the limit of 15 partitions by disk.
 *
 * \date June, 2012
 */
class NoDeviceDriverRecognizedException : public WarningException {
public:
	NoDeviceDriverRecognizedException() throw() {
		this->_msg=D_("The library can't determine whether this device is SCSI or IDE. Using SCSI by default.");
	}

};
/**@}*/

}

#endif /* NODEVICEDRIVERRECOGNIZEDEXCEPTION_H_ */
