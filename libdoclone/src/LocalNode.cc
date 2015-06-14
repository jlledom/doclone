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

#include <doclone/LocalNode.h>

#include <doclone/Clone.h>
#include <doclone/Image.h>
#include <doclone/Operation.h>
#include <doclone/PartedDevice.h>
#include <doclone/DiskLabel.h>
#include <doclone/DlFactory.h>
#include <doclone/DataTransfer.h>
#include <doclone/Util.h>
#include <doclone/exception/NoBlockDeviceException.h>
#include <doclone/exception/CreateImageException.h>
#include <doclone/exception/RestoreImageException.h>

namespace Doclone {

/**
 * \brief Creates a doclone image.
 */
void LocalNode::create() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Local::create() start");

	if(!Util::isBlockDevice(this->_device)) {
		NoBlockDeviceException ex;
		throw ex;
	}

	PartedDevice *pDevice = PartedDevice::getInstance();
	std::string target = pDevice->getPath();

	Util::createFile(this->_image);
	int fd = Util::openFile(this->_image);

	Image image;

	image.initDiskReadArchive();
	image.initFdWriteArchive(fd);

	if(Util::isDisk(this->_device)) {
		image.setType(Doclone::IMAGE_DISK);
	}
	else {
		image.setType(Doclone::IMAGE_PARTITION);
	}

	Operation *readPartTableOp = new Operation(
			Doclone::OP_READ_PARTITION_TABLE, target);

	Clone *dcl = Clone::getInstance();
	dcl->addOperation(readPartTableOp);

	image.readPartitionTable(this->_device);

	// Mark the operation to read partition table as completed
	dcl->markCompleted(Doclone::OP_READ_PARTITION_TABLE, target);

	if(image.canCreateCheck() == false) {
		CreateImageException ex;
		throw ex;
	}

	image.initCreateOperations();

	image.saveImageHeader();

	// Initialize the counter of progress
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(image.getSize());

	image.readPartitionsData();

	image.freeWriteArchive();
	image.freeReadArchive();

	Util::closeFile(fd);

	log->debug("Local::create() end");
}

/**
 * \brief Restores a doclone image.
 */
void LocalNode::restore() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Local::restore() start");

	if(!Util::isBlockDevice(this->_device)) {
		NoBlockDeviceException ex;
		throw ex;
	}

	int fd = Util::openFile(this->_image);

	Image image;

	image.initFdReadArchive(fd);
	image.initDiskWriteArchive();

	image.loadImageHeader();

	// Initialize the counter of progress
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(image.getSize());

	if(image.canRestoreCheck(this->_device) == false) {
		RestoreImageException ex;
		throw ex;
	}

	image.initRestoreOperations(this->_device);

	image.writePartitionTable(this->_device);

	image.writePartitionsData(this->_device);

	image.freeWriteArchive();
	image.freeReadArchive();

	Util::closeFile(fd);

	log->debug("Local::restore() end");
}

}
