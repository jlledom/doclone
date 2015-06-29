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

#include <doclone/Disk.h>

#include <sstream>
#include <fstream>
#include <vector>

#include <parted/parted.h>

#include <doclone/Clone.h>
#include <doclone/Logger.h>
#include <doclone/Partition.h>
#include <doclone/PartedDevice.h>
#include <doclone/DlFactory.h>
#include <doclone/Grub.h>
#include <doclone/Util.h>
#include <doclone/exception/WarningException.h>
#include <doclone/exception/CreatePartitionException.h>
#include <doclone/exception/NoAccessToDeviceException.h>
#include <doclone/exception/WriteDataException.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/FileNotFoundException.h>
#include <doclone/exception/AlignPartitionException.h>

namespace Doclone {

/**
 * \brief Initialize attributes.
 */
Disk::Disk()
		: _path(), _size(), _partitions(), _bootCode() {
}

// Getters and setters
const std::string &Disk::getPath() const {
	return this->_path;
}

void Disk::setPath(const std::string &path) {
	this->_path = path;
}

uint64_t Disk::getSize() const {
	return this->_size;
}

void Disk::setSize(uint64_t size) {
	this->_size = size;
}

std::vector<Partition*> &Disk::getPartitions() {
	return this->_partitions;
}

void Disk::setPartitions(const std::vector<Partition*> &parts) {
	this->_partitions = parts;
}

const char *Disk::getBootCode() const {
	return this->_bootCode;
}

void Disk::setBootCode(const char *bCode) {
	memcpy(this->_bootCode, bCode, sizeof(this->_bootCode));
}

/**
 * \brief Initializes the disk from its path.
 *
 * \param path
 * 		The disk path. e.g. /dev/sda
 */
void Disk::initFromPath(const std::string &path) throw(Exception) {
	this->_path = path;
	this->initSize();
}

/**
 * \brief Reads the first Doclone::MBR_SIZE bytes of the disk
 */
void Disk::readBootCode() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::readBootCode() start");

	std::ifstream fstr(this->_path.c_str(), std::fstream::in|std::fstream::binary);

	if(!fstr) {
		NoAccessToDeviceException ex(this->_path);
		throw ex;
	}

	try {
		fstr.seekg(0, std::ios_base::beg);
		fstr.read(this->_bootCode, Doclone::MBR_SIZE);
		fstr.close();

	}catch(...) {
		ReadDataException ex;
		ex.logMsg();
	}

	log->debug("Disk::readBootCode() end");
}

/**
 * \brief Writes the first Doclone::MBR_SIZE bytes of the disk
 */
void Disk::writeBootCode() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::writeBootCode() start");

	std::ofstream fstr(this->_path.c_str(), std::fstream::out|std::fstream::binary);

	if(!fstr) {
		FileNotFoundException ex(this->_path);
		throw ex;
	}

	try {
		fstr.seekp(0, std::ios_base::beg);
		fstr.write(this->_bootCode, Doclone::MBR_SIZE);
		fstr.close();

	}catch(...) {
		WriteDataException ex;
		throw ex;
	}

	log->debug("Disk::writeBootCode() end");
}

/**
 * \brief Reads all the partitions of the disk
 *
 * Reads all the partitions of the disk and fills the vector of partitions with
 * Partition* objects. All these pointers are destroyed in the destructor of
 * Image.
 */
void Disk::readPartitions() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::readPartitions() start");

	this->_partitions.clear();

	PedPartition *pedPart = 0;

	PartedDevice *pedDev = PartedDevice::getInstance();
	pedDev->open();
	PedDisk *pDisk = pedDev->getDisk();

	while ((pedPart = ped_disk_next_partition (pDisk, pedPart))) {
		if (ped_partition_is_active (pedPart)) {
			try {
				std::string path=
						Util::buildPartPath(pedPart->disk->dev->path, pedPart->num);
				Partition *part = new Partition();
				part->initFromPath(path);
				this->_partitions.push_back(part);
			}
			catch(const WarningException &ex) {
				continue;
			}
		}
	}

	pedDev->close();

	log->debug("Disk::readPartitions() end");
}

/**
 * \brief Calculates the size of the device in bytes.
 */
void Disk::initSize() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::initSize() start");

	PedDevice *pDevice = ped_device_get(this->_path.c_str());

	if(!pDevice) {
		NoAccessToDeviceException ex(this->_path);
		throw ex;
	}

	this->_size=(pDevice->length * pDevice->sector_size);

	log->debug("Disk::initSize() end");
}

/**
 * \brief Calculates the geometry of a partition
 *
 * Calculates the starting and ending sectors in the disk of the given
 * partition. This library always create partitions aligned to MiB.
 *
 * \param pDisk
 * 		The disk where the partition will be placed
 * \param part
 * 		Pointer to partition that will be created
 * \return PedGeometry* object with the bounds of the partition
 */
PedGeometry *Disk::calcGeometry(const PedDisk* pDisk,const Partition *part)
		const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::calcGeometry(pDisk=>0x%x, part=>0x%x) start", pDisk, part);

	/*
	 * Libdoclone aligns partitions to MiB, aligning them also to sectors of
	 * 4096 bytes.
	 *
	 * This variable contains the amount of sectors that fits on a MiB for this
	 * disk.
	 */
	PedSector mibAlignedMultiple=(1024*1024)/pDisk->dev->sector_size;

	double usedPart = part->getUsedPart();
	double startPos = part->getStartPos();

	PedSector startSector = (pDisk->dev->length * startPos);
	PedSector endSector = startSector + (pDisk->dev->length * usedPart);

	// If the start sector is not aligned to MiB
	if((startSector % mibAlignedMultiple)!=0) {
		// The partition begins in the next sector aligned to MiB
		startSector += (mibAlignedMultiple-(startSector % mibAlignedMultiple));
	}

	//If the end sector is not aligned to MiB
	if((endSector % mibAlignedMultiple)!=0) {
		// The end is also aligned as possible
		endSector += (mibAlignedMultiple-(endSector % mibAlignedMultiple));
	}

	// If a partition starts before the first MiB, must be moved.
	if(startSector<mibAlignedMultiple) {
		startSector = mibAlignedMultiple;
	}

	// If one partition exceeds the end of the disk, must be cut.
	if(endSector > pDisk->dev->length) {
		endSector = pDisk->dev->length;
	}

	PedGeometry *pedGeom = ped_geometry_new(pDisk->dev, startSector,
			endSector-startSector);

	log->debug("Disk::calcGeometry(pedGeom=>0x%x) end", pedGeom);

	return pedGeom;
}

/**
 * \brief Calculates the constraint for the given partition.
 *
 * This library will create this partition aligned to MiB. This alignment can
 * move the partition some KiB to the right and then the next partition can
 * overlap it, for that, this function will provide a constraint to create it
 * at the first available MiB aligned sector.
 *
 * \param pPart
 * 		The partition
 * \return Constraint* object with the bounds of the partition
 */
PedConstraint *Disk::calcConstraint(const PedPartition* pPart,
		uint64_t usedBytes) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::calcConstraint(pPart=>0x%x, usedByes=>%d) start",
			pPart, usedBytes);

	/*
	 * Libdoclone aligns partitions to MiB and by the way align them also to
	 * sectors of 4096 bytes.
	 *
	 * This variable contains the amount of sectors that fit on a MiB for this
	 * disk.
	 */
	PedSector mibAlignedMultiple=(1024*1024)/pPart->disk->dev->sector_size;

	// Setting the minimum size of the partition
	PedSector usedSectors = 1;
	if(usedBytes > 0) {
		usedSectors = usedBytes / pPart->disk->dev->sector_size;
	}

	PedAlignment *startAlign = ped_alignment_new(pPart->geom.start,
			mibAlignedMultiple);
	PedAlignment *endAlign = ped_alignment_new(pPart->geom.end,
			mibAlignedMultiple);
	PedGeometry *startRange = ped_geometry_new(pPart->disk->dev,
			pPart->geom.start, pPart->geom.length);
	PedGeometry *endRange = ped_geometry_new(pPart->disk->dev,
			pPart->geom.end, (pPart->disk->dev->length-pPart->geom.end));
	PedSector minSize = usedSectors+(pPart->geom.length*0.05);
	PedSector maxSize = pPart->geom.length+(pPart->geom.length*0.1);

	PedConstraint *pConstraint = ped_constraint_new(startAlign, endAlign,
			startRange, endRange, minSize, maxSize);

	ped_geometry_destroy(startRange);
	ped_geometry_destroy(endRange);
	ped_alignment_destroy(startAlign);
	ped_alignment_destroy(endAlign);

	log->debug("Disk::calcConstraint(pedGeom=>0x%x) end", pConstraint);

	return pConstraint;
}

/**
 * \brief Creates a new partition in the disk
 *
 * \param part
 * 		The partition that will be created
 * 		The number and path of this object will be initialized
 */
void Disk::writePartitionToDisk(Partition *part) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::writePartitionToDisk(part=>0x%x) start", part);

	PedPartitionType type;
	PedPartition *pedPart = 0 ;
	PedConstraint *constraint = 0 ;
	PedFileSystemType* fsType = 0 ;

	PartedDevice *pedDev = PartedDevice::getInstance();
	pedDev->open();
	PedDisk *pDisk = pedDev->getDisk();

	switch ( part->getType() ) {
		case Doclone::PARTITION_PRIMARY: {
			type = PED_PARTITION_NORMAL ;
			break ;
		}
		case Doclone::PARTITION_LOGICAL: {
			type = PED_PARTITION_LOGICAL ;
			break ;
		}
		case Doclone::PARTITION_EXTENDED: {
			type = PED_PARTITION_EXTENDED ;
			break ;
		}
		default	:
			type = PED_PARTITION_FREESPACE;
			break;
	}

	if ( part->getType() != Doclone::PARTITION_EXTENDED )
		fsType = ped_file_system_type_get( "ext2" ) ;

	PedGeometry *geom = this->calcGeometry(pDisk, part);

	pedPart = ped_partition_new(pDisk, type, fsType, geom->start, geom->end );

	if (pedPart) {
		constraint = this->calcConstraint(pedPart, part->getMinSize());

		if (constraint) {

			if (ped_disk_add_partition(pDisk, pedPart, constraint)) {
				pedDev->commit();
			}
			else {
				AlignPartitionException ex;
				ex.logMsg();

				/*
				 * If the addition of the new partition fails, maybe it is
				 * because the alignment to MiB is too restrictive for this
				 * situation. So we try again without alignment.
				 */
				constraint = ped_constraint_any(pDisk->dev);

				if (ped_disk_add_partition(pDisk, pedPart, constraint)) {
					pedDev->commit();
				} else {
					// I give up
					CreatePartitionException exc;
					throw exc;
				}
			}

			// Set the new partition number and path
			part->setPartNum(pedPart->num);
			part->setPath(Util::buildPartPath(this->_path, pedPart->num));

			ped_constraint_destroy(constraint);
		}
	} else {
		CreatePartitionException ex;
		throw ex;
	}

	pedDev->close();

	ped_geometry_destroy(geom);

	log->debug("Disk::writePartitionToDisk() end");
}

/**
 * \brief Creates in the disk all partitions in the vector
 */
void Disk::writePartitions() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::writePartitions() start");

	this->makeLabel();

	Clone *dcl = Clone::getInstance();
	dcl->markCompleted(Doclone::OP_MAKE_DISKLABEL, this->_path);

	for (unsigned int i = 0; i< this->_partitions.size(); i++) {
		Partition *part = this->_partitions[i];

		if(part->getUsedPart() == 0) {
			continue;
		}

		this->writePartitionToDisk(part);

		if(part->getType() != Doclone::PARTITION_EXTENDED) {
			//Remove old superblocks and signatures if any
			part->clearSignatures();
		}

		std::stringstream target;
		target << this->_path << ", #" << (i+1);
		dcl->markCompleted(Doclone::OP_CREATE_PARTITION, target.str());
	}

	log->debug("Disk::writePartitions() end");
}

/**
 * \brief Restores the Grub bootloader
 */
void Disk::restoreGrub() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Disk::restoreGrub() start");

	try {
		Grub grub(this);
		grub.install();
	}catch(const WarningException &ex) {
		this->writeBootCode();
	}

	log->debug("Disk::restoreGrub() end");
}

/**
 * \brief Clears the partitions vector
 */
Disk::~Disk() {
	for(unsigned int i=0; i<this->_partitions.size(); i++) {
		Partition *part = this->_partitions.at(i);
		delete part;
	}
	this->_partitions.clear();
}

}
