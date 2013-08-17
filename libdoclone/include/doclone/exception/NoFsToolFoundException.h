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

#ifndef NOFSTOOLSFOUNDEXCEPTION_H_
#define NOFSTOOLSFOUNDEXCEPTION_H_

#include <doclone/exception/ErrorException.h>

#include <string>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class NoFsToolFoundException
 * \brief Any needed external tool is missing
 * \date August, 2011
 */
class NoFsToolFoundException : public ErrorException {
public:
	/// \param command The missing command
	NoFsToolFoundException(const std::string &command) throw()
		: _command(command) {
		// TO TRANSLATORS: looks like	A necessary tool to restore was not found: mke2fs
		std::string msg= D_("A necessary tool to restore was not found:");
		msg.append(" ");
		msg.append(this->_command);

		this->_msg = msg;
	}
	~NoFsToolFoundException() throw() {}
private:
	/// The missing command
	const std::string _command;
};
/**@}*/

}

#endif /* NOFSTOOLSFOUNDEXCEPTION_H_ */
