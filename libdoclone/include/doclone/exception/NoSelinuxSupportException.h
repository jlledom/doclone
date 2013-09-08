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

#ifndef NOSELINUXSUPPORTEXCEPTION_H_
#define NOSELINUXSUPPORTEXCEPTION_H_

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class NoSelinuxSupportException
 * \brief No support to read/write Selinux attributes
 *
 * The libselinux headers was not installed in the system when doclone was built.
 * \date September, 2013
 */
class NoSelinuxSupportException : public WarningException {
public:
	NoSelinuxSupportException() throw() {
		this->_msg = D_("Selinux attributes will not be cloned due to Doclone was built without support for Selinux");
	}
};
/**@}*/

}

#endif /* NOSELINUXSUPPORTEXCEPTION_H_ */
