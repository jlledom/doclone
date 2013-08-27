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

#ifndef UMOUNTEXCEPTION_H_
#define UMOUNTEXCEPTION_H_

#include <string>

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class UmountException
 * \brief Error unmounting a partition
 * \date August, 2011
 */
class UmountException : public WarningException {
public:
	/// \param path The path of the partition
	UmountException(const std::string &path) throw() : _path(path) {
		// TO TRANSLATORS: looks like	Can't unmount partition. Mounted in: /tmp/doclone-NgvlyF
		std::string msg= D_("Can't unmount partition. Mounted in:");
		msg.append(" ");
		msg.append(this->_path);

		this->_msg = msg;
	}
	~UmountException() throw() {}

private:
	/// The path of the partition
	const std::string _path;
};
/**@}*/

}

#endif /* UMOUNTEXCEPTION_H_ */
