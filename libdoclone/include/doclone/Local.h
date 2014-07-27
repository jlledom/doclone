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

#ifndef LOCAL_H_
#define LOCAL_H_

#include <string>

#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \class Local
 * \brief Implementation of the local mode.
 *
 * Methods and attributes to clone an image locally.
 * \date January, 2012
 */
class Local {
public:
	Local(const std::string &image, const std::string &device);
	~Local(){}

	void create() const throw(Exception);
	void restore() const throw(Exception);
private:
	/// The path of the image
	const std::string _image;

	// The path of the device
	const std::string _device;
};

}
#endif /* LOCAL_H_ */
