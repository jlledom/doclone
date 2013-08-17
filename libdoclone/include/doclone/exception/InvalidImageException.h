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

#ifndef INVALIDIMAGE_H_
#define INVALIDIMAGE_H_

#include <doclone/exception/ErrorException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class InvalidImageException
 * \brief Invalid doclone image.
 *
 * The introduced file is not a valid doclone image
 * \date August, 2011
 */
class InvalidImageException : public ErrorException {
public:
	InvalidImageException() throw() {
		this->_msg=D_("Invalid image");
	}

};
/**@}*/

}

#endif /* INVALIDIMAGE_H_ */
