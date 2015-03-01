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

#ifndef NOACCESSTODEVICEEXCEPTION_H_
#define NOACCESSTODEVICEEXCEPTION_H_

#include <string>

#include <doclone/exception/ErrorException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class NoAccessToDeviceException
 * \brief Impossible to access to a device file.
 *
 * It has been impossible to access to a device file, probably a permissions error.
 * \date August, 2011
 */
class NoAccessToDeviceException : public ErrorException {
public:
	/// \param deviceName The path of the device to be accessed
	NoAccessToDeviceException(const std::string &deviceName) throw() :
		_deviceName(deviceName) {
		// TO TRANSLATORS: looks like	Can't access to device: /dev/sda
		std::string msg=D_("Can't access to device:");
		msg.append(" ");
		msg.append(this->_deviceName);

		this->_msg = msg;
	}
	~NoAccessToDeviceException() throw() {}

private:
	/// The device path
	const std::string _deviceName;
};
/**@}*/

}

#endif /* NOACCESSTODEVICEEXCEPTION_H_ */
