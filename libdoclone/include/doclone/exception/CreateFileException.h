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

#ifndef CREATEFILEEXCEPTION_H_
#define CREATEFILEEXCEPTION_H_

#include <doclone/exception/WarningException.h>

#include <string>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class CreateFileException
 * \brief Any error trying to create a new file.
 *
 * Has been impossible to create a new file, probably a permissions error.
 * \date August, 2011
 */
class CreateFileException : public WarningException {
public:
	/// \param file The name of the file
	CreateFileException(const std::string &file) throw() : _file(file) {
		// TO TRANSLATORS: looks like	Cant' create file: /etc/mtab
		std::string msg= D_("Cant' create file:");
		msg.append(" ");
		msg.append(this->_file);
		this->_msg = msg;
	}
	~CreateFileException() throw() {}

private:
	/// The name of the file
	const std::string _file;
};
/**@}*/

}

#endif /* CREATEFILEEXCEPTION_H_ */
