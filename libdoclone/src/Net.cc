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
#include <doclone/DataTransfer.h>
#include <doclone/exception/CloseConnectionException.h>

namespace Doclone {

/**
 * \brief Sets the maximum number of nodes and links
 */
Net::Net(): _srcIP() {
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
}

/**
 * \brief Closes the opened connections
 */
void Net::closeConnection() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Net::closeConnection() start");

	DataTransfer *trns = DataTransfer::getInstance();

	int fdo = trns->getFdo();
	std::map<int,std::string> fdds(trns->getFddList());

	if(fdo) {
		if(close(fdo)<0) {
			CloseConnectionException ex;
			ex.logMsg();
		}
	}

	std::map<int,std::string>::iterator it;
	for(it = fdds.begin();it != fdds.end();++it) {
		if(close((*it).first)<0) {
			CloseConnectionException ex;
			ex.logMsg();
		}
	}

	log->debug("Net::closeConnection() end");
}

}
