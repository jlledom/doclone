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

#ifndef WRITEERRORSINDIRECTORYEXCEPTION_H_
#define WRITEERRORSINDIRECTORYEXCEPTION_H_

#include <doclone/exception/WarningException.h>

#include <string>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class WriteErrorsInDirectoryException
 * \brief Error writing to a directory.
 * \date August, 2011
 */
class WriteErrorsInDirectoryException : public WarningException {
public:
	/// \param directory The name of the directory
	WriteErrorsInDirectoryException(const std::string &directory) throw()
		: _directory(directory) {
		// TO TRANSLATORS: looks like	Some errors writing in directory: /etc
		std::string msg= D_("Some errors writing in directory:");
		msg.append(" ");
		msg.append(this->_directory);

		this->_msg = msg;
	}
	~WriteErrorsInDirectoryException() throw() {}

private:
	/// The path of the directory
	const std::string _directory;
};
/**@}*/

}

#endif /* WRITEERRORSINDIRECTORYEXCEPTION_H_ */
