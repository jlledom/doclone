/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013, 2015 Joan Lledó <joanlluislledo@gmail.com>
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

#ifndef UNICAST_H_
#define UNICAST_H_

#include <string>
#include <vector>

#include <doclone/NetNode.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \class Unicast
 * \brief Implementation of the unicast/multicast server and client.
 *
 * Methods and attributes to clone over network using unicast or multicast.
 * Class inherited from Net.
 * \date August, 2011
 */
class Unicast : public NetNode {
public:
	Unicast();

	void send() throw(Exception);
	void receive() throw(Exception);

private:
	virtual void closeConnection() throw(Exception);

	void tcpServer() throw(Exception);
	void tcpClient() throw(Exception);

	void sendFromImage() throw(Exception);
	void sendFromDevice() throw(Exception);

	void receiveToImage() throw(Exception);
	void receiveToDevice() throw(Exception);

	/// Number of receivers (for server)
	unsigned int _nodesNum;

	///Vector of sockets connected to the client or server
	std::vector<int> _fds;
};

}

#endif /* UNICAST_H_ */
