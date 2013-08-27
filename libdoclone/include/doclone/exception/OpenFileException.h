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

#ifndef OPENFILEEXCEPTION_H_
#define OPENFILEEXCEPTION_H_

#include <string>

#include <doclone/exception/ErrorException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class OpenFileException
 * \brief Cannot open a file.
 *
 * Probably a permissions error.
 * \date August, 2011
 */
class OpenFileException : public ErrorException {
public:
	/// \param fileName The path of the file
	OpenFileException(const std::string &filePath) throw()
		: _filePath(filePath) {
		// TO TRANSLATORS: looks like	Can't open file: /home/user/sdb.doclone
		std::string msg= D_("Can't open file:");
		msg.append(" ");
		msg.append(this->_filePath);

		this->_msg = msg;
	}
	~OpenFileException() throw() {}

private:
	/// The path of the file
	const std::string _filePath;
};
/**@}*/

}

#endif /* OPENFILEEXCEPTION_H_ */
