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

#ifndef FORMATEXCEPTION_H_
#define FORMATEXCEPTION_H_

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class FormatException
 * \brief Any error formatting a partition
 * \date August, 2011
 */
class FormatException : public WarningException {
public:
	/// \param device The path of the partition
	FormatException(const std::string &device) throw() : _device(device) {
		// TO TRANSLATORS: looks like	Can't format partition: /dev/sdb1
		std::string msg= D_("Can't format partition:");
		msg.append(" ");
		msg.append(this->_device);

		this->_msg = msg;
	}
	~FormatException() throw() {}

private:
	/// The path of the partition
	const std::string _device;
};
/**@}*/

}

#endif /* FORMATEXCEPTION_H_ */
