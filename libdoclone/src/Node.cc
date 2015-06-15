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

#include <doclone/Node.h>

#include <string>

#include <doclone/Clone.h>
#include <doclone/Image.h>

namespace Doclone {

/**
 * \brief Sets the image and the device path
 */
Node::Node(): _image(), _device() {
	Clone *dcl = Clone::getInstance();

	this->_image = dcl->getImage();
	this->_device = dcl->getDevice();
}

/**
 * \brief Opens an image header just for reading its size from the XML header
 *
 * \return The size of the image
 */
uint64_t Node::getImageSize(const int fd) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::getImageSize(fd=>%d) start", fd);

	Image dcImage;
	dcImage.initFdReadArchive(fd);
	dcImage.loadImageSizeFromHeader();

	uint64_t retVal = dcImage.getSize();

	dcImage.freeReadArchive();
	lseek(fd, 0, SEEK_SET);

	log->debug("Image::getImageSize(retVal=>%d) end", retVal);
	return retVal;
}

}
