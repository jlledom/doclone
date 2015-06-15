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

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <sys/types.h>

#include <string>

#include <archive.h>
#include <archive_entry.h>

#include <xercesc/dom/DOM.hpp>

#include <doclone/Util.h>
#include <doclone/DiskLabel.h>
#include <doclone/Partition.h>
#include <doclone/xml/XMLDocument.h>
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
	~Image();

	bool isValid() const throw(Exception);

	void initDiskReadArchive();
	void initFdReadArchive(const int fdin) throw(Exception);
	void initDiskWriteArchive();
	void initFdWriteArchive(std::vector<int> &fds) throw(Exception);
	void initFdWriteArchive(const int fdout) throw(Exception);

	void freeReadArchive();
	void freeWriteArchive();

	void loadImageSizeFromHeader() throw(Exception);
	void loadImageHeader() throw(Exception);
	void saveImageHeader() throw(Exception);

	void readPartitionsData() throw(Exception);
	void writePartitionsData(const std::string &device) throw(Exception);

	void readPartitionTable(const std::string &device) throw(Exception);
	void writePartitionTable(const std::string &device) throw(Exception);

	bool canCreateCheck() const throw(Exception);
	bool canRestoreCheck(const std::string &device) const throw(Exception);

	void initCreateOperations() throw(Exception);
	void initRestoreOperations(const std::string &device) const throw(Exception);

	uint64_t getSize() const;
	Doclone::imageType getType() const;
	void setType(Doclone::imageType type);
	Disk *getDisk();
	struct archive *getArchiveIn() const;
	const std::vector<struct archive *> &getArchivesOut() const;

private:
	/// Image size
	uint64_t _size;
	/// Image type (disk or partition)
	Doclone::imageType _type;
	/// If the image has data or only a partition table
	bool _noData;
	/// Disk to work on
	DiskLabel *_disk;
	/// The reading archive object
	struct archive *_archiveIn;
	/// Vector of writing archive objects
	std::vector<struct archive *> _archivesOut;

	bool fitInDisk() const throw(Exception);

	void readPartition(int index) throw(Exception);
	void writePartition(int index) const throw(Exception);

	void readDataFromDisk(struct archive_entry_linkresolver *lResolv,
			const std::string &path, const std::string &imgRootDir,
			size_t mPointLength) throw(Exception);
	void writeDataToDisk() throw(Exception);
};

}

#endif /* IMAGE_H_ */
