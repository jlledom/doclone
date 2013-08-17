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

#ifndef CANCELEXCEPTION_H_
#define CANCELEXCEPTION_H_

#include <doclone/exception/ErrorException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class CancelException
 * \brief The user has canceled the job or any error has occurred.
 * \date January, 2012
 */
class CancelException : public ErrorException {
public:
	CancelException() throw() {
		this->_msg=D_("The job was canceled by the user");
	}

};
/**@}*/

}

#endif /* CANCELEXCEPTION_H_ */
