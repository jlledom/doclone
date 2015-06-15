/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2015 Joan Lledó <joanlluislledo@gmail.com>
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

#ifndef NODE_H_
#define NODE_H_

#include <string>

#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \class Node
 * \brief Abstract class for all local and network nodes
 * \date May, 2015
 */
class Node {
public:
	Node();
	virtual ~Node() {}

protected:
	uint64_t getImageSize(const int fd) throw(Exception);

	/// The path of the image
	std::string _image;

	// The path of the device
	std::string _device;
};

}

#endif /* NODE_H_ */
