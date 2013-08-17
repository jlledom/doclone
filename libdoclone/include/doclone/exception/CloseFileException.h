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

#ifndef CLOSEFILEEXCEPTION_H_
#define CLOSEFILEEXCEPTION_H_

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class CloseFileException
 * \brief Any error closing a file descriptor
 * \date August, 2011
 */
class CloseFileException : public WarningException {
public:
	CloseFileException() throw() {
		this->_msg=D_("Can't close a file descriptor");
	}

};
/**@}*/

}

#endif /* CLOSEFILEEXCEPTION_H_ */
