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

#ifndef NOFITINDEVICEEXCEPTION_H_
#define NOFITINDEVICEEXCEPTION_H_

#include <doclone/exception/ErrorException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class NoFitInDeviceException
 * \brief The image doesn't fits in the device.
 *
 * The image is bigger than the device or some partition is bigger than the
 * space proportionally assigned to it.
 * \date August, 2011
 */
class NoFitInDeviceException : public ErrorException {
public:
	NoFitInDeviceException() throw() {
		this->_msg=D_("Image doesn't fit in device");
	}

};
/**@}*/

}

#endif /* NOFITINDEVICEEXCEPTION_H_ */
