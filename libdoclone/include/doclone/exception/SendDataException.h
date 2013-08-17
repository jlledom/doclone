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

#ifndef SENDDATAEXCEPTION_H_
#define SENDDATAEXCEPTION_H_

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class SendDataException
 * \brief Any error sending data
 * \date August, 2011
 */
class SendDataException: public WarningException {
public:
	/// \param host The receiver's host name or IP address
	SendDataException(const std::string &host) throw() : _host(host) {
		// TO TRANSLATORS: looks like	Can't send data to 192.168.1.10
		std::string msg=D_("Can't send data to");
		msg.append(" ");
		msg.append(this->_host);

		this->_msg = msg;
	}
	~SendDataException() throw() {}

private:
	/// The receiver's host or IP
	const std::string _host;
};
/**@}*/

}

#endif /* SENDDATAEXCEPTION_H_ */
