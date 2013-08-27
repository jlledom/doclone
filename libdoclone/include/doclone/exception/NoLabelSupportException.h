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

#ifndef NOLABELSUPPORTEXCEPTION_H_
#define NOLABELSUPPORTEXCEPTION_H_

#include <string>

#include <doclone/exception/WarningException.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class NoLabelSupportException
 * \brief No support to read/write fs label.
 *
 * Any filesystem does not support the reading or writing of its label
 * \date August, 2011
 */
class NoLabelSupportException : public WarningException {
public:
	/// \param fsName The name of the filesystem
	NoLabelSupportException(const std::string &fsName) throw()
		: _fsName(fsName) {
		// TO TRANSLATORS: looks like	The label will not be cloned for this file system: linux-swap
		std::string msg= D_("The label will not be cloned for this file system:");
		msg.append(" ");
		msg.append(this->_fsName);

		this->_msg = msg;
	}
	~NoLabelSupportException() throw() {}

private:
	/// The name of the filesystem
	const std::string _fsName;
};
/**@}*/

}

#endif /* NOLABELSUPPORTEXCEPTION_H_ */
