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

#ifndef UNICAST_H_
#define UNICAST_H_

#include <doclone/Net.h>

#include <doclone/exception/Exception.h>

#include <string>

namespace Doclone {

/**
 * \class Unicast
 * \brief Implementation of the unicast/multicast mode.
 *
 * Methods and attributes to clone over network using unicast or multicast.
 * Class inherited from Net.
 * \date August, 2011
 */
class Unicast : public Net {
public:
	void send() throw(Exception);
	void receive() throw(Exception);

private:
	void tcpServer() throw(Exception);
	void tcpClient() const throw(Exception);

	void sendFromImage(const std::string &image) throw(Exception);
	void sendFromDevice(const std::string &device) throw(Exception);

	void receiveToImage(const std::string &image) const throw(Exception);
	void receiveToDevice(const std::string &device) const throw(Exception);
};

}

#endif /* UNICAST_H_ */
