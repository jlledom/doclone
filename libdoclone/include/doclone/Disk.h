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

#ifndef DISK_H_
#define DISK_H_

#include <string>
#include <vector>

#include <parted/parted.h>

#include <doclone/Partition.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \class Disk
 * \brief Represents a full disk.
 *
 * All the operations related with disks are implemented right here.
 * This class can delete the partition table, create new partitions,
 * write/read the MBR, etc.
 *
 * \date August, 2011
 */
class Disk {
public:
	Disk();
	virtual ~Disk();

	virtual void readBootCode() throw(Exception);
	virtual void writeBootCode() const throw(Exception);
	virtual void readPartitions() throw(Exception);
	virtual void writePartitions() const throw(Exception);

	void initFromPath(const std::string &path) throw(Exception);
	void restoreGrub() throw(Exception);

	void setPath(const std::string &path);
	const std::string &getPath() const;
	void setSize(uint64_t size);
	uint64_t getSize() const;
	std::vector<Partition*> &getPartitions();
	void setPartitions(const std::vector<Partition*> &parts);
	const void *getBootCode() const;
	void setBootCode(const void *bCode);

protected:
	/// The disk path
	std::string _path;
	/// Size of the disk (bytes)
	uint64_t _size;
	/// Vector of partitions that will be read of written from/to the disk
	std::vector<Partition*> _partitions;
	/// The Master Boot Record of the disk. The first 440 bytes that contains the code to boot
	char _bootCode[440];

	void initSize() throw(Exception);

	PedGeometry *calcGeometry(const PedDisk* pDisk,
			const Partition *part) const throw(Exception);
	PedConstraint *calcConstraint(const PedPartition* pPart,
			uint64_t usedBytes) const throw(Exception);
	void writePartitionToDisk(Partition *part) const throw(Exception);

	virtual void makeLabel() const throw(Exception) = 0;
};

}

#endif /* DISK_H_ */
