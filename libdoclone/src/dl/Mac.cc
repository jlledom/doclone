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

#include <doclone/dl/Mac.h>

#include <doclone/PartedDevice.h>
#include <doclone/Util.h>
#include <doclone/exception/WarningException.h>

namespace Doclone {

Mac::Mac(const std::string &path) : DiskLabel(path) {
	this->_labelType = DISK_TYPE_MAC;
	this->_partedName = "mac";
}

/**
 * \brief Reads all the partitions of the disk
 *
 * Reads all the partitions of the disk and fills the vector of partitions with
 * Partition* objects.
 *
 * It ignores the first partition, which is created by libparted while creating
 * a MAC partition table.
 *
 * All these pointers are destroyed in the destructor of Image.
 */
void Mac::readPartitions() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Mac::readPartitions() start");

	this->_partitions.clear();

	PedPartition *pedPart = 0;

	PartedDevice *pedDev = PartedDevice::getInstance();
	pedDev->open();
	PedDisk *pDisk = pedDev->getDisk();

	while ((pedPart = ped_disk_next_partition (pDisk, pedPart))) {
		/*
		 * The extended partition is created automatically by libparted when
		 * the disklabel is created and it must not be cloned.
		 */
		if (ped_partition_is_active (pedPart)
				&& pedPart->num != 1) {
			try {
				std::string path=
						Util::buildPartPath(pedPart->disk->dev->path, pedPart->num);
				// These Partition objects will be destroyed in Image::~Image()
				Partition *part = new Partition (path);
				this->_partitions.push_back(part);
			}
			catch(const WarningException &ex) {
				continue;
			}
		}
	}

	pedDev->close();

	log->debug("Mac::readPartitions() end");
}

/**
 * \brief This function doesn't do anything
 *
 * This function overrides the base class function to do nothing, because
 * the writing of the first 440 bytes in a disk with a MAC partition table can
 * damage it.
 */
void Mac::writeBootCode() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Mac::writeBootCode() start");

	log->debug("Mac::writeBootCode() end");
}

} /* namespace Doclone */
