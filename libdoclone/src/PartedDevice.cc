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

#include <doclone/PartedDevice.h>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <parted/parted.h>

#include <doclone/Logger.h>
#include <doclone/exception/CommitException.h>
#include <doclone/exception/NoAccessToDeviceException.h>

namespace Doclone {

/**
 * \brief Initializes the attributes
 */
PartedDevice::PartedDevice(): _openings(0), _path(), _pDevice(0), _pDisk(0) {
}

/**
 * \brief Closes the opened device
 */
PartedDevice::~PartedDevice() {

	if(this->_pDisk) {
		ped_disk_destroy( this->_pDisk );
		this->_pDisk = 0;
	}

	if(this->_pDevice) {
		ped_device_destroy( this->_pDevice );
		this->_pDevice = 0;
	}

	this->_openings = 0;
}

/**
 * \brief Singleton stuff
 *
 * \return A PartedDevice object
 */
PartedDevice* PartedDevice::getInstance() {
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mutex);

	static PartedDevice instance;

	pthread_mutex_unlock(&mutex);

	return &instance;
}

std::string PartedDevice::getPath() {
	return this->_path;
}

PedDevice *PartedDevice::getDevice() {
	return this->_pDevice;
}

PedDisk *PartedDevice::getDisk() {
	return this->_pDisk;
}

PedPartition *PartedDevice::getPartition(unsigned int numPartition) const {
	return ped_disk_get_partition(this->_pDisk, numPartition);
}

void PartedDevice::setDisk(PedDisk *pDisk) {
	this->_pDisk = pDisk;
}

/**
 * \brief Gets the size of the opened device
 *
 * \return Size in bytes
 */
uint64_t PartedDevice::getDevSize() const {
	Logger *log = Logger::getInstance();
	log->debug("PartedDevice::getDevSize() start");

	uint64_t devSize=(this->_pDevice->length * this->_pDevice->sector_size);

	log->debug("PartedDevice::getDevSize(devSize=>%d) end", devSize);
	return devSize;
}

/**
 * \brief Gets the size of the given partition in the opened device
 *
 * \return Size in bytes
 */
uint64_t PartedDevice::getPartitionSize(unsigned int numPartition) const {
	Logger *log = Logger::getInstance();
	log->debug("PartedDevice::getPartitionSize(numPartition=>%d) start", numPartition);

	PedPartition *pPart = this->getPartition(numPartition);
	uint64_t devSize=(pPart->geom.length * this->_pDevice->sector_size);

	log->debug("PartedDevice::getPartitionSize(devSize=>%d) end", devSize);
	return devSize;
}

/**
 * \brief Sets the disk which will be opened
 */
void PartedDevice::initialize(const std::string &device) {
	Logger *log = Logger::getInstance();
	log->debug("PartedDevice::initialize(device=>%s) start", device.c_str());

	this->_path = device;

	log->debug("PartedDevice::initialize() end");
}

/**
 * \brief Opens a disk and its device with libparted.
 *
 * If it is opened, don't make anything.
 */
void PartedDevice::open() throw(Exception){
	Logger *log = Logger::getInstance();
	log->debug("PartedDevice::open() start");

	if(this->_openings > 0) {
		this->_openings++;
		return;
	}

	if(!this->_pDevice) {
		this->_pDevice = ped_device_get( this->_path.c_str());

		if(!this->_pDevice) {
			NoAccessToDeviceException ex(this->_path);
			throw ex;
		}

		if(!ped_device_open(this->_pDevice)) {
			NoAccessToDeviceException ex(this->_path);
			throw ex;
		}
	}

	if(!this->_pDisk) {
		this->_pDisk = ped_disk_new(this->_pDevice);
	}

	this->_openings =1 ;

	log->debug("PartedDevice::open() end");
}

/**
 * \brief Closes a disk and its device with libparted.
 *
 * If it is closed, don't make anything.
 */
void PartedDevice::close() {
	Logger *log = Logger::getInstance();
	log->debug("PartedDevice::close() start");

	if(this->_openings>1) {
		this->_openings--;
		return;
	}

	if(this->_pDisk) {
		ped_disk_destroy(this->_pDisk);
		this->_pDisk = 0;
	}

	if(this->_pDevice) {
		ped_device_close(this->_pDevice);
		ped_device_destroy(this->_pDevice);
		this->_pDevice = 0;
	}

	this->_openings = 0;

	log->debug("PartedDevice::close() end");
}

/**
 * \brief Commits the changes to disk.
 */
void PartedDevice::commit() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("PartedDevice::commit() start");

	if(!ped_disk_commit_to_dev(this->_pDisk)) {
		CommitException ex;
		throw ex;
	}

	int i = 0;

	// Wait up to 30 seconds if the device is busy
	while(!ped_disk_commit_to_os (this->_pDisk)) {
		if (errno == EBUSY) {
			if(i<30) {
				 // Device busy, let's wait one more second
				sleep(1);
				i++;
				continue;
			}
			else {
				CommitException ex;
				ex.logMsg();
				throw ex;
			}
		}
		else {
			CommitException ex;
			ex.logMsg();
			throw ex;
		}
	}

	log->debug("PartedDevice::commit() end");
}

}
