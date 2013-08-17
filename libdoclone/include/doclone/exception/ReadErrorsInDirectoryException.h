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

#ifndef READERRORSINDIRECTORYEXCEPTION_H_
#define READERRORSINDIRECTORYEXCEPTION_H_

#include <doclone/exception/WarningException.h>

#include <string>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class ReadErrorsInDirectoryException
 * \brief Error reading a directory
 * \date August, 2011
 */
class ReadErrorsInDirectoryException : public WarningException {
public:
	/// \param directory The name of the directory
	ReadErrorsInDirectoryException(const std::string &directory) throw()
		: _directory(directory) {
		// TO TRANSLATORS: looks like	Some errors reading directory: /etc
		std::string msg= D_("Some errors reading directory:");
		msg.append(" ");
		msg.append(this->_directory);

		this->_msg = msg;
	}
	~ReadErrorsInDirectoryException() throw() {}

private:
	/// The path of the directory
	const std::string _directory;
};
/**@}*/

}

#endif /* READERRORSINDIRECTORYEXCEPTION_H_ */
