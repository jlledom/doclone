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

#include <doclone/Net.h>

#include <unistd.h>

#include <doclone/Logger.h>
#include <doclone/Clone.h>

namespace Doclone {

/**
 * \brief Sets the maximum number of nodes and the network interface
 * to be used if any.
 */
Net::Net(): _srcIPs() {
	Clone *dcl = Clone::getInstance();

	unsigned int nodes = dcl->getNodesNumber();
	if(nodes == 0) {
		this->_linksNum = Doclone::LINKS_NUM;
		this->_nodesNum = 1;
	}
	else {
		this->_linksNum = nodes;
		this->_nodesNum = nodes;
	}

	this->_interface = dcl->getInterface();
}

}
