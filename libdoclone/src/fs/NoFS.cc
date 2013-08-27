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

#include <doclone/fs/NoFS.h>

#include <doclone/Logger.h>

namespace Doclone {

/**
 * \addtogroup Filesystems
 * @{
 */

/**
 * \brief Initializes the attributes
 */
NoFS::NoFS() {
	Logger *log = Logger::getInstance();
	log->debug("NoFS::NoFS() start");
	
	this->_type = Doclone::FSTYPE_NONE;
	this->_docloneName="";
	this->_mountName="";
	this->_mountOptions="";
	this->_command="";
	this->_formatOptions="";
	this->_code = Doclone::FS_NOFS;
	
	log->debug("NoFS::NoFS() end");
}
/**@}*/

}
