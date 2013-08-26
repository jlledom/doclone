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

#ifndef SPAWNPROCESSEXCEPTION_H_
#define SPAWNPROCESSEXCEPTION_H_

#include <doclone/exception/WarningException.h>

#include <string>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class SpawnProcessException
 * \brief Trying to spawn a process failed.
 *
 * \date August, 2013
 */
class SpawnProcessException : public WarningException {
public:
	/// \param fileName The path of the file
	SpawnProcessException(const std::string &filePath) throw()
		: _processName(filePath) {
		// TO TRANSLATORS: looks like	Can't spawn process: mkfs.ext4
		std::string msg= D_("Can't spawn process:");
		msg.append(" ");
		msg.append(this->_processName);

		this->_msg = msg;
	}
	~SpawnProcessException() throw() {}

private:
	/// The name of the process
	const std::string _processName;
};
/**@}*/

}

#endif /* SPAWNPROCESSEXCEPTION_H_ */
