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

#include <doclone/Image.h>
#include <doclone/Clone.h>
#include <doclone/Logger.h>
#include <doclone/Operation.h>
#include <doclone/DataTransfer.h>
#include <doclone/DlFactory.h>

#include <doclone/exception/Exception.h>
#include <doclone/exception/ErrorException.h>
#include <doclone/exception/WarningException.h>
#include <doclone/exception/InvalidImageException.h>
#include <doclone/exception/WrongImageTypeException.h>
#include <doclone/exception/NoFsToolFoundException.h>
#include <doclone/exception/NoUuidSupportException.h>
#include <doclone/exception/NoLabelSupportException.h>
#include <doclone/exception/NoMountSupportException.h>
#include <doclone/exception/NoFitInDeviceException.h>
#include <doclone/exception/TooMuchPartitionsException.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <endian.h>

#include <sstream>
#include <string>

namespace Doclone {

/**
 * \brief Initializes attributes
 */
Image::Image(): _size(), _type(), _labelType(),_header(), _partitions(),
		_partsInfo() {
	Clone *dcl = Clone::getInstance();
	this->_noData = dcl->getEmpty();
}

/**
 * \brief Frees the memory of the vector of partitions, and clears it.
 */
Image::~Image() {
	for(std::vector<Partition*>::iterator it = this->_partitions.begin();
			it != this->_partitions.end();++it) {
		if(*it != 0) {
			delete *it;
			*it = 0;
		}
	}

	this->_partitions.clear();
}

/**
 * \brief Checks if the image entered by the user is really a doclone image.
 *
 * To determine if the passed file is a valid .doclone image, this function amounts
 * every min_size of each partition and compare it with the header image_size.
 * If the amount of min_size's is equal to image_size, the passed file is a
 * valid .doclone image.
 */
bool Image::isValid() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::isValid() start");

	uint64_t amount_bytes = 0;
	int i;
	bool retValue;

	for (i = 0; i<this->_header.num_partitions; i++) {
		amount_bytes += this->_partsInfo.at(i).min_size;
	}

	if (amount_bytes != this->_header.image_size) {
		retValue = false;
	}
	else {
		retValue = true;
	}
	
	log->debug("Image::isValid(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Checks if image fits in the specified device
 *
 * \param dcDisk
 * 		For get the size of the disk.
 */
bool Image::fitInDisk(uint64_t size) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::fitInDisk(size=>%d) start", size);
	
	bool retValue = this->_size < size;
	
	log->debug("Image::fitInDisk(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Opens the metadata of the image and fills the values of the
 * attributes.
 *
 * Opposite function of createImageHeader.
 */
void Image::openImageHeader() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::openImageHeader() start");

	if(!this->isValid()) {
		InvalidImageException ex;
		throw ex;
	}

	this->_size = this->_header.image_size;

	this->_type = static_cast<Doclone::imageType>(this->_header.image_type);

	this->_labelType =
			static_cast<Doclone::diskLabelType>(this->_header.disk_type);

	for (int i = 0; i<this->_header.num_partitions; i++) {
		Partition *part = new Partition(this->_partsInfo.at(i));

		this->_partitions.push_back(part);
	}

	log->debug("Image::openImageHeader() end");
}

/**
 * \brief Gets the necessary metadata and writes it into the image header.
 *
 * Opposite of openImageHeader.
 *
 * \param dcDisk
 * 		The disk on which we will work.
 */
void Image::createImageHeader(Disk *dcDisk) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::createImageHeader(dcDisk=>0x%x) start", dcDisk);
	
	// Initialize to 0;
	memset(&this->_header, 0, sizeof(this->_header));
	
	this->_header.num_partitions = this->_partitions.size();

	this->_header.image_size = 0;
	
	for(int i = 0;i<this->_header.num_partitions;i++) {
		this->_partitions[i]->createPartInfo();
		this->_partsInfo.push_back(this->_partitions[i]->getPartition());
		this->_header.image_size += this->_partsInfo.at(i).min_size;
	}
	
	this->_header.image_type = this->_type;
	
	this->_header.disk_type = this->_labelType;

	if(this->_type == Doclone::IMAGE_DISK) {
		dcDisk->readBootCode();
		memcpy(this->_header.boot_code, dcDisk->getBootCode(), sizeof(this->_header.boot_code));
	}

	log->debug("Image::createImageHeader() end");
}

/**
 * \brief Reads the all the necessary metadata in the image header.
 *
 * \return Number of bytes read
 */
ssize_t Image::readImageHeader(const std::string &device) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::readImageHeader() start");

	DataTransfer *trns = DataTransfer::getInstance();
	ssize_t readBytes = 0;

	readBytes+=
			trns->transferTo(&this->_header.num_partitions, sizeof(uint8_t));

	if (this->_header.num_partitions > Util::getNumberOfMinors(device)) {
		TooMuchPartitionsException ex(this->_header.num_partitions,
				Util::getNumberOfMinors(device));
		ex.logMsg();
		throw ex;
	}

	readBytes+=trns->transferTo(&this->_header.image_size, sizeof(uint64_t));
	readBytes+=trns->transferTo(&this->_header.image_type, sizeof(uint8_t));
	readBytes+=trns->transferTo(&this->_header.boot_code, sizeof(uint8_t)*440);
	readBytes+=trns->transferTo(&this->_header.disk_type, sizeof(uint8_t));
	readBytes+=trns->transferTo(&this->_header.dummy, sizeof(uint8_t)*512);

	this->_header.image_size = be64toh(this->_header.image_size);

	for(int i = 0; i<this->_header.num_partitions; i++) {
		Doclone::partInfo part = {};

		readBytes+=trns->transferTo(&part.flags, sizeof(dcFlag));
		readBytes+=trns->transferTo(&part.start_pos, sizeof(uint64_t));
		readBytes+=trns->transferTo(&part.used_part, sizeof(uint64_t));
		readBytes+=trns->transferTo(&part.type, sizeof(uint8_t));
		readBytes+=trns->transferTo(&part.min_size, sizeof(uint64_t));
		readBytes+=trns->transferTo(&part.fsName, sizeof(uint8_t)*32);
		readBytes+=trns->transferTo(&part.label, sizeof(uint8_t)*28);
		readBytes+=trns->transferTo(&part.uuid, sizeof(uint8_t)*37);
		readBytes+=trns->transferTo(&part.dummy, sizeof(uint8_t)*512);

		/*
		 * The integers are stored in big-endian, so if the system is
		 * little-endian, we must swap it.
		 */
		part.flags = be16toh(part.flags);
		part.start_pos = be64toh(part.start_pos);
		part.used_part = be64toh(part.used_part);
		part.min_size = be64toh(part.min_size);

		this->_partsInfo.push_back(part);
	}

	log->debug("Image::readImageHeader(readBytes=>%d) end", readBytes);

	return readBytes;
}

/**
 * \brief Writes the necessary metadata in the image header.
 *
 * \return Number of bytes written
 */
ssize_t Image::writeImageHeader() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::writeImageHeader() start");

	DataTransfer *trns = DataTransfer::getInstance();
	ssize_t writtenBytes = 0;

	writtenBytes+=
			trns->transferFrom(&this->_header.num_partitions, sizeof(uint8_t));

	uint64_t tmpImageSize = htobe64(this->_header.image_size);

	writtenBytes+=trns->transferFrom(&tmpImageSize, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(&this->_header.image_type, sizeof(uint8_t));
	writtenBytes+=trns->transferFrom(&this->_header.boot_code, sizeof(uint8_t)*440);
	writtenBytes+=trns->transferFrom(&this->_header.disk_type, sizeof(uint8_t));
	writtenBytes+=trns->transferFrom(&this->_header.dummy, sizeof(uint8_t)*512);

	for(int i = 0;i<this->_header.num_partitions;i++) {
		Doclone::partInfo part = this->_partsInfo.at(i);

		// The integers will be written in big endian.
		dcFlag tmpFlags = htobe16(part.flags);
		uint64_t tmpStartPos = htobe64(part.start_pos);
		uint64_t tmpUsedPart = htobe64(part.used_part);
		uint64_t tmpMinSize = htobe64(part.min_size);

		writtenBytes+=trns->transferFrom(&tmpFlags, sizeof(dcFlag));
		writtenBytes+=trns->transferFrom(&tmpStartPos, sizeof(uint64_t));
		writtenBytes+=trns->transferFrom(&tmpUsedPart, sizeof(uint64_t));
		writtenBytes+=trns->transferFrom(&part.type, sizeof(uint8_t));
		writtenBytes+=trns->transferFrom(&tmpMinSize, sizeof(uint64_t));
		writtenBytes+=trns->transferFrom(&part.fsName, sizeof(uint8_t)*32);
		writtenBytes+=trns->transferFrom(&part.label, sizeof(uint8_t)*28);
		writtenBytes+=trns->transferFrom(&part.uuid, sizeof(uint8_t)*37);
		writtenBytes+=trns->transferFrom(&part.dummy, sizeof(uint8_t)*512);
	}

	log->debug("Image::writeImageHeader(writtenBytes=>%d) end", writtenBytes);

	return writtenBytes;
}

/**
 * \brief Reads and transfers all the data of a partition
 *
 * \param index
 * 		The order of the partition (in the vector of Partition*)
 */
void Image::readPartition(int index) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::readPartition(numPart=>%d) start", index);
	
	Partition *part = this->_partitions[index];
	Clone *dcl = Clone::getInstance();
	
	if(!this->_noData) {
		part->read();

		dcl->markCompleted(Doclone::OP_READ_DATA, part->getPath());

	}

	log->debug("Image::readPartition() end");
}


/**
 * \brief Gets and writes all the data for a partition
 *
 * \param index
 * 		The order of the partition (in the vector of Partition*)
 */
void Image::writePartition(int index) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::writePartition(numPart=>%d) start", index);

	Partition *part = this->_partitions[index];

	if(!this->_noData) {
		part->write();
	}

	log->debug("Image::writePartition() end");
}

/**
 * \brief Reads the data of all partitions in the vector
 */
void Image::readPartitionsData() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::readPartitionsData() start");

	// Initialize the counter of progress
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(this->_header.image_size);

	for(unsigned int i = 0;i<this->_partitions.size(); i++) {
		try {
			this->readPartition(i);
		}catch(const WarningException &ex) {
			if(this->_partitions.size() == 1) {
				throw;
			}

			continue;
		}
	}

	log->debug("Image::readPartitionsData() end");
}

/**
 * \brief Writes the data of all partitions in the vector
 */
void Image::writePartitionsData() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::writePartitionsData() start");

	Clone *dcl = Clone::getInstance();

	// Initialize the counter of progress
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(this->_header.image_size);

	if(this->_type == Doclone::IMAGE_DISK) {
		for(int i = 0;i<this->_header.num_partitions
			&& this->_partitions[i]->getPartition().used_part != 0; i++) {
			this->writePartition(i);

			std::stringstream target;
			target << Util::getDiskPath(this->_partitions[i]->getPath()) << ", #" << (i+1);
			dcl->markCompleted(Doclone::OP_WRITE_DATA, target.str());
		}
	} else {
		this->writePartition(0);

		std::stringstream target;
		target << this->_partitions[0]->getPath();
		dcl->markCompleted(Doclone::OP_WRITE_DATA, target.str());
	}

	log->debug("Image::writePartitionsData() end");
}

/**
 * \brief Reads the partition table and fills the vector of Partition*
 *
 * \param device
 * 		The device which we must read.
 */
void Image::readPartitionTable(const std::string &device) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::readPartitionTable(device=>%s) start", device.c_str());

	if(this->_type == Doclone::IMAGE_DISK) {
		Disk *dcDisk = DlFactory::createDiskLabel();

		this->_labelType = dynamic_cast<DiskLabel*>(dcDisk)->getLabelType();

		dcDisk->readPartitions();
		this->_partitions = dcDisk->getPartitions();

		delete dcDisk;
	} else {
		// This Partition object is destroyed in Image::~Image()
		Partition *part = new Partition (device);
		this->_partitions.push_back(part);
	}

	log->debug("Image::readPartitionTable() end");
}

/**
 * \brief Reads the vector of partitions to create the real partitions in disk.
 *
 * \param device
 * 		The device on which write.
 */
void Image::writePartitionTable(const std::string &device) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::writePartitionTable(device=>%s) start", device.c_str());

	Clone *dcl = Clone::getInstance();

	if(this->_type == Doclone::IMAGE_DISK) {
		Disk *dcDisk = DlFactory::createDiskLabel(this->_labelType, device);

		dcDisk->setPartitions(this->_partitions);
		dcDisk->writePartitions();

		for (int i = 0;i<this->_header.num_partitions &&
			this->_partitions[i]->getPartition().used_part != 0; i++) {
			Partition *part = this->_partitions[i];

			std::stringstream target;
			target << device << ", #" << (i+1);

			part->format();
			dcl->markCompleted(Doclone::OP_FORMAT_PARTITION,
					target.str());

			part->writeFlags();
			dcl->markCompleted(Doclone::OP_WRITE_PARTITION_FLAGS,
					target.str());

			part->writeLabel();
			dcl->markCompleted(Doclone::OP_WRITE_FS_LABEL,
					target.str());

			part->writeUUID();
			dcl->markCompleted(Doclone::OP_WRITE_FS_UUID,
					target.str());
		}

		delete dcDisk;

	} else {
		this->_partitions[0]->setPath(device);
		this->_partitions[0]->setPartNum(Util::getPartNum(device));

		std::stringstream target;
		target << device;

		this->_partitions[0]->format();
		dcl->markCompleted(Doclone::OP_FORMAT_PARTITION, target.str());

		this->_partitions[0]->writeFlags();
		dcl->markCompleted(Doclone::OP_WRITE_PARTITION_FLAGS,
				target.str());

		this->_partitions[0]->writeLabel();
		dcl->markCompleted(Doclone::OP_WRITE_FS_LABEL, target.str());

		this->_partitions[0]->writeUUID();
		dcl->markCompleted(Doclone::OP_WRITE_FS_UUID, target.str());
	}

	log->debug("Image::writePartitionTable() end");
}

/**
 * \brief Checks if the system has installed all the necessary external tools to
 * create an image of a disk or a partition.
 *
 * \return True or False
 */
bool Image::canCreateCheck() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::canCreateCheck() start");

	bool retValue = true;

	try {
		// For each partition
		for(unsigned int i = 0;i<this->_partitions.size()
				&& this->_partitions[i]->getPartition().used_part != 0; i++) {
			Partition *part = this->_partitions[i];

			Filesystem *fs = part->getFileSystem();
			if(fs->getCode() != Doclone::FS_NOFS) {
				bool formatSupport = fs->getFormatSupport();
				bool labelSupport = fs->getLabelSupport();
				bool uuidSupport = fs->getUUIDSupport();
				bool mountSupport = fs->getMountSupport();

				if(mountSupport == false) {
					if(fs->getCode() != Doclone::FS_LINUXSWAP) {
						NoMountSupportException ex(fs->getdocloneName());
						ex.logMsg();
					}
				}

				if(formatSupport == false) {
					NoFsToolFoundException ex(fs->getCommand());
					ex.logMsg();
					throw ex;
				}

				if(labelSupport == false) {
					NoLabelSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}

				if(uuidSupport == false) {
					NoUuidSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}
			}
		}
	} catch (const ErrorException &ex) {
		retValue = false;
	}

	log->debug("Image::canCreateCheck(retValue=>%d) end", retValue);

	return retValue;
}

/**
 * \brief Checks if the system has installed all the necessary external tools to
 * restore an image in a disk or a partition.
 *
 * \para dcDisk
 * 		Disk on which work
 * \return True or False
 */
bool Image::canRestoreCheck(const std::string &device, uint64_t size) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::canRestoreCheck(device=>%s, size=>%d) start", device.c_str(), size);

	bool retValue = true;

	try {
		switch(this->_type) {
			case Doclone::IMAGE_DISK: {
				if(!Util::isDisk(device)) {
					WrongImageTypeException ex;
					ex.logMsg();
					throw ex;
				}

				break;
			}
			case Doclone::IMAGE_PARTITION: {
				if(Util::isDisk(device)) {
					WrongImageTypeException ex;
					ex.logMsg();
					throw ex;
				}

				break;
			}
			default: {
				InvalidImageException ex;
				ex.logMsg();
				throw ex;
			}
		}

		// Check if it fills in the entire device
		if(!this->fitInDisk(size)) {
			NoFitInDeviceException ex;
			ex.logMsg();
			throw ex;
		}

		for(int i = 0;i<this->_header.num_partitions
				&& this->_partitions[i]->getPartition().used_part != 0; i++) {
			Partition *part = this->_partitions[i];

			// Check if every partition fills in its assigned space in this disk
			if(!part->fitInDevice()) {
				Clone *dcl = Clone::getInstance();
				if(dcl->getForce() == false) {
					NoFitInDeviceException ex;
					ex.logMsg();
					throw ex;
				}
			}

			Filesystem *fs = part->getFileSystem();
			if(fs->getCode() != Doclone::FS_NOFS) {
				bool formatSupport = fs->getFormatSupport();
				bool labelSupport = fs->getLabelSupport();
				bool uuidSupport = fs->getUUIDSupport();
				bool mountSupport = fs->getMountSupport();

				if(mountSupport == false) {
					if(fs->getCode() != Doclone::FS_LINUXSWAP) {
						NoMountSupportException ex(fs->getdocloneName());
						ex.logMsg();
						throw ex;
					}
				}

				if(formatSupport == false) {
					NoFsToolFoundException ex(fs->getCommand());
					ex.logMsg();
					throw ex;
				}

				if(labelSupport == false) {
					NoLabelSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}

				if(uuidSupport == false) {
					NoUuidSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}
			}
		}
	} catch (const NoMountSupportException &ex) {
		retValue = false;
	} catch (const ErrorException &ex) {
		retValue = false;
	}

	log->debug("Image::canRestoreCheck(retValue=>%d) end", retValue);

	return retValue;
}

/**
 * \brief Makes a list of all the operations to be performed in the process of
 * image creation.
 */
void Image::initCreateOperations() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::initCreateOperations() start");

	Clone *dcl = Clone::getInstance();

	if(this->_noData == false) {
		std::vector<Partition *>::iterator it;
		for (it = this->_partitions.begin(); it != this->_partitions.end(); ++it) {
			Partition *part=(*it);

			if(part->isWritable() == true) {
				std::string dataTarget = part->getPath();

				// All these Operation objects are deleted in doclone::~doclone()
				Operation *dataOp= new Operation(Doclone::OP_READ_DATA,
						dataTarget);

				dcl->addOperation(dataOp);
			}
		}
	}

	log->debug("Image::initCreateOperations() end");
}

/**
 * \brief Makes a list of all the operations to be performed in the process of
 * image restoration.
 *
 * \param device
 * 		device on which restore the data
 */
void Image::initRestoreOperations(const std::string &device) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::initRestoreOperations(device=>%s) start", device.c_str());

	Clone *dcl = Clone::getInstance();

	std::string target = device;

	if(this->_header.image_type == Doclone::IMAGE_DISK) {
		Operation *diskLabelOp = new Operation(Doclone::OP_MAKE_DISKLABEL,
				target);
		dcl->addOperation(diskLabelOp);

		for (int i = 0;i<this->_header.num_partitions &&
				this->_partitions[i]->getPartition().used_part != 0; i++) {
			std::stringstream partTarget;
			partTarget << target << ", #" << (i+1);

			Operation *createPartOp = new Operation(Doclone::OP_CREATE_PARTITION,
					partTarget.str());
			dcl->addOperation(createPartOp);
		}
	}

	for (int i = 0;i<this->_header.num_partitions &&
			this->_partitions[i]->getPartition().used_part != 0; i++) {
		Partition *part = this->_partitions[i];
		Filesystem *fs = part->getFileSystem();

		std::stringstream partTarget;

		if(this->_header.image_type == Doclone::IMAGE_DISK) {
			partTarget << target << ", #" << (i+1);
		} else {
			partTarget << target;
		}

		// All these Operation objects are deleted in doclone::~doclone()
		Operation *formatPartOp = new Operation(Doclone::OP_FORMAT_PARTITION,
				partTarget.str());
		dcl->addOperation(formatPartOp);

		Operation *writeFSFlags = new Operation(Doclone::OP_WRITE_PARTITION_FLAGS,
				partTarget.str());
		dcl->addOperation(writeFSFlags);

		if(fs->getLabelSupport() == true) {
			Operation *writeFSLabelOp = new Operation(Doclone::OP_WRITE_FS_LABEL,
							partTarget.str());
					dcl->addOperation(writeFSLabelOp);
		}

		if(fs->getUUIDSupport() == true) {
			Operation *writeFSUuid = new Operation(Doclone::OP_WRITE_FS_UUID,
					partTarget.str());
			dcl->addOperation(writeFSUuid);
		}
	}

	for (int i = 0;i<this->_header.num_partitions &&
				this->_partitions[i]->getPartition().used_part != 0; i++) {
		Partition *part = this->_partitions[i];

		std::stringstream partTarget;

		if(this->_header.image_type == Doclone::IMAGE_DISK) {
			partTarget << target << ", #" << (i+1);
		} else {
			partTarget << target;
		}

		if(this->_noData == false && part->isWritable() == true) {
			Operation *dataOp = new Operation(Doclone::OP_WRITE_DATA,
					partTarget.str());

			dcl->addOperation(dataOp);
		}
	}

	log->debug("Image::initRestoreOperations() end");
}

Doclone::imageType Image::getType() const {
	return this->_type;
}

void Image::setType(Doclone::imageType type) {
	this->_type = type;
}

Doclone::imageHeader Image::getHeader() const {
	return this->_header;
}

void Image::setHeader(Doclone::imageHeader header) {
	this->_header = header;
}

const std::vector<Partition*> &Image::getPartitions() const {
	return this->_partitions;
}

Doclone::diskLabelType Image::getLabelType() const {
	return this->_labelType;
}

}
