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

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include <archive.h>

#include <doclone/Util.h>
#include <doclone/DiskLabel.h>
#include <doclone/Partition.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \enum imageType
 *
 * \brief An image can be a disk or a partition
 * \var IMAGE_DISK
 * 	Image of a disk
 * \var IMAGE_PARTITION
 *  Image of a partition
 */
enum imageType { IMAGE_DISK, IMAGE_PARTITION };

/**
 * \struct imageHeader
 * \brief Metadata of the image
 *
 * Will be part of the image file.
 */
struct imageHeader {
	/// Number of partitions in the image
	uint8_t num_partitions;
	/// Estimated Total size of the resultant image, in bytes
	uint64_t image_size;
	/// Value of imageType enumerator: Disk or partition image
	uint8_t image_type;
	/// Disk boot_code (if any)
	uint8_t boot_code[440];
	/// Value of diskLabelType enumerator.
	uint8_t disk_type;
};

/**
 * \class Image
 * \brief Represents a doclone image.
 *
 * Performs actions common to all processes of creation and restoration of an
 * image, either locally or over the network.
 *
 * Also performs a check to test if the image can be created or restored in the
 * specified device.
 *
 * \date August, 2011
 */
class Image {
public:
	Image();
	virtual ~Image();
	
	bool isValid() const throw(Exception);

	void initDiskReadArchive();
	void initFdReadArchive(const int fdin) throw(Exception);
	void initDiskWriteArchive();
	void initFdWriteArchive(const int fdout) throw(Exception);

	void freeReadArchive();
	void freeWriteArchive();

	void openImageHeader() throw(Exception);
	void createImageHeader(Disk *dcDev) throw(Exception);

	void readImageHeader(const std::string &device) throw(Exception);
	void writeImageHeader() const throw(Exception);

	void readPartitionsData() const throw(Exception);
	void writePartitionsData() const throw(Exception);

	void readPartitionTable(const std::string &device) throw(Exception);
	void writePartitionTable(const std::string &device) throw(Exception);

	bool canCreateCheck() const throw(Exception);
	bool canRestoreCheck(const std::string &device, uint64_t size) const throw(Exception);

	void initCreateOperations() throw(Exception);
	void initRestoreOperations(const std::string &device) const throw(Exception);

    Doclone::imageType getType() const;
    void setType(Doclone::imageType type);

    Doclone::imageHeader getHeader() const;
    void setHeader(Doclone::imageHeader header);

    const std::vector<Partition*> &getPartitions() const;
    Doclone::diskLabelType getLabelType() const;

    struct archive *getArchiveIn() const;
    struct archive *getArchiveOut() const;

private:
	/// Image size
	uint64_t _size;
	/// Image type (disk or partition)
	Doclone::imageType _type;
	/// The type of the disk label (only used if the disk is a partition)
	Doclone::diskLabelType _labelType;
	/// If the image has data or only a partition table
	bool _noData;
	/// Metadata of the image
	Doclone::imageHeader _header;
	/// All partitions of the image
	std::vector<Partition*> _partitions;
	/// Information about all partitions of the image
	std::vector<Doclone::partInfo> _partsInfo;
	/// The reading archive object
	struct archive *_archiveIn;
	/// The writing archive object
	struct archive *_archiveOut;

	bool fitInDisk(uint64_t size) const throw(Exception);

	void readPartition(int index) const throw(Exception);
	void writePartition(int index) const throw(Exception);
};

}

#endif /* IMAGE_H_ */
