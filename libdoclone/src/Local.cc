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

#include <doclone/Local.h>

#include <doclone/Clone.h>
#include <doclone/Image.h>
#include <doclone/Operation.h>
#include <doclone/PartedDevice.h>
#include <doclone/DiskLabel.h>
#include <doclone/DlFactory.h>
#include <doclone/Util.h>
#include <doclone/exception/NoBlockDeviceException.h>
#include <doclone/exception/CreateImageException.h>
#include <doclone/exception/RestoreImageException.h>

namespace Doclone {

/**
 * \brief Initializes attributes
 *
 * \param image
 * 		Image path
  * \param device
 * 		Device path
 */
Local::Local(const std::string &image, const std::string &device)
	: _image(image), _device(device) {
}

/**
 * \brief Creates a doclone image.
 */
void Local::create() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Local::create() start");

	if(!Util::isBlockDevice(this->_device)) {
		NoBlockDeviceException ex;
		throw ex;
	}

	PartedDevice *pDevice = PartedDevice::getInstance();
	std::string target = pDevice->getPath();
	Disk *dcDisk = DlFactory::createDiskLabel();

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

	image.createImageHeader(dcDisk);

	image.writeImageHeader();

	image.readPartitionsData();

	image.freeWriteArchive();
	image.freeReadArchive();

	Util::closeFile(fd);

	delete dcDisk;

	log->debug("Local::create() end");
}

/**
 * \brief Restores a doclone image.
 */
void Local::restore() const throw(Exception) {
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

	image.readImageHeader();

	image.openImageHeader();

	PartedDevice *pDevice = PartedDevice::getInstance();
	Disk *dcDisk = DlFactory::createDiskLabel(image.getLabelType(),
			pDevice->getPath());

	if(image.canRestoreCheck(this->_device, dcDisk->getSize()) == false) {
		RestoreImageException ex;
		throw ex;
	}

	image.initRestoreOperations(this->_device);

	image.writePartitionTable(this->_device);

	image.writePartitionsData(this->_device);

	if(image.getHeader().image_type==(Doclone::imageType)IMAGE_DISK) {
		dcDisk->setPartitions(image.getPartitions());
		dcDisk->restoreGrub();
	}

	delete dcDisk;

	image.freeWriteArchive();
	image.freeReadArchive();

	Util::closeFile(fd);

	log->debug("Local::restore() end");
}

}
