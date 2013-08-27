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

#ifndef FILENOTFOUNDEXCEPTION_H_
#define FILENOTFOUNDEXCEPTION_H_

#include <string>

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class FileNotFoundException
 * \brief File not found
 * \date August, 2011
 */
class FileNotFoundException : public WarningException {
public:
	/// \param file The name of the file
	FileNotFoundException(const std::string &file) throw() : _file(file) {
		// TO TRANSLATORS: looks like	File not found: /etc/mtab
		std::string msg= D_("File not found:");
		msg.append(" ");
		msg.append(this->_file);

		this->_msg = msg;
	}
	~FileNotFoundException() throw() {}

private:
	/// The name of the file
	const std::string _file;
};
/**@}*/

}

#endif /* FILENOTFOUNDEXCEPTION_H_ */
