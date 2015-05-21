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

#ifndef LINK_H_
#define LINK_H_

#include <string>

#include <doclone/NetNode.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \class Link
 * \brief Implementation of the link mode.
 *
 * Methods and attributes to clone over network using the link mode.
 *
 * In the link mode, there are two kinds of actors, the sender and the
 * receivers.
 *
 * The nodes are arranged in a single line, like a hub, each one receives
 * the data of the previous one in the chain, sending it to he next one.
 *
 * The receivers must run first. Then the sender sends an UDP message and wait
 * for responses. Each response is a link that is waiting for data. Here, the
 * sender sends to each receiver the IP of the next node in the chain to form
 * the network.
 *
 * Class inherited from Net.
 * \date August, 2011
 */
class Link : public NetNode {
public:
	Link();

	void send() throw(Exception);
	void receive() throw(Exception);

private:
	virtual void closeConnection() throw(Exception);

	int answer() const throw(Exception);
	int netScan() const throw(Exception);

	void linkServer() throw(Exception);
	void linkClient() throw(Exception);

	void sendFromImage() throw(Exception);
	void sendFromDevice() throw(Exception);

	void receiveToImage() throw(Exception);
	void receiveToDevice() throw(Exception);

	///Previous link socket
	int _fdin;

	///Next link socket
	int _fdout;

	/// Max number of links in the chain
	unsigned int _linksNum;

	/// Ip address of the interface to be used in the link mode
	std::string _interface;

	/// Next link IP (Human readable)
	std::string _dstIP;
};

}

#endif /* LINK_H_ */
