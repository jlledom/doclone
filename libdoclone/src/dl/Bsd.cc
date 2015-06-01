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

#include <doclone/dl/Bsd.h>

namespace Doclone {

Bsd::Bsd() {
	this->_labelType = DISK_TYPE_BSD;
	this->_partedName = "bsd";
}

/**
 * \brief This functions doesn't do nothing
 *
 * This function overrides the base class function to do nothing, because the
 * writing of the first 440 bytes in a disk with a BSD partition table can
 * damage it.
 */
void Bsd::writeBootCode() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Bsd::writeBootCode() start");

	log->debug("Bsd::writeBootCode() end");
}

} /* namespace Doclone */
