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

#ifndef TOOMUCHOPARTITIONSEXCEPTION_H_
#define TOOMUCHOPARTITIONSEXCEPTION_H_

#include <stdint.h>

#include <sstream>

#include <doclone/exception/ErrorException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class TooMuchPartitionsException
 * \brief The number of partitions in an image is greater than the allowed.
 *
 * The max number of partitions that is allowed to work with is determined by
 * the kernel device driver used to access the disk. If the number of partitions
 * on the image file is greater than the supported by the driver, the execution
 * must stop.
 *
 * \date March, 2012
 */
class TooMuchPartitionsException : public ErrorException {
public:
	/// \param numParts The number of partitions in the image
	TooMuchPartitionsException(uint8_t numParts, uint8_t maxParts) throw()
		: _numParts(numParts), _maxParts(maxParts) {
		std::stringstream msg;

		/*
		 * TO TRANSLATORS: looks like	The image contains 19 partitions, but
		 * this kernel can only handle up to 16.
		 */
		msg << D_("The image contains") << " " << this->_numParts;
		msg << " " << D_("partitions, but this kernel can only handle up to");
		msg << " " << this->_maxParts;

		this->_msg = msg.str();
	}

	~TooMuchPartitionsException() throw() {}

private:
	/// The number of partitions in the image
	const uint8_t _numParts;
	/// The max number of partitions for this kernel driver
	const uint8_t _maxParts;
};
/**@}*/

}

#endif /* TOOMUCHOPARTITIONSEXCEPTION_H_ */
