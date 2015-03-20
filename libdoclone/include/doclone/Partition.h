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

#ifndef PARTITION_H_
#define PARTITION_H_

#define TMP_PREFIX P_tmpdir "/doclone-XXXXXX"
#define TMP_PREFIX_REGEXP "^/tmp/doclone-[a-zA-Z0-9]{6}$"

#include <stdint.h>
#include <sys/types.h>

#include <string>

#include <parted/parted.h>

#include <doclone/Filesystem.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \typedef dcFlag
 * Type that represents a partition flag
 *
 * Libdoclone supports up to 14 flags:
 * BOOT, ROOT, SWAP, HIDDEN, RAID, LVM, LBA, HPSERVICE, PALO, PREP,
 * MSFT_RESERVED, BIOS_GRUB, APPLE_TV_RECOVERY, and DIAG
 *
 * The flags are stored in this type by bit-level manipulation:
 * F_BOOT = 1 << 0
 * F_ROOT = 1 << 1
 * F_SWAP = 1 << 2
 *  ... and so on.
 */
typedef uint16_t dcFlag;

const dcFlag F_BOOT					= 1 << 0;
const dcFlag F_ROOT					= 1 << 1;
const dcFlag F_SWAP					= 1 << 2;
const dcFlag F_HIDDEN				= 1 << 3;
const dcFlag F_RAID					= 1 << 4;
const dcFlag F_LVM					= 1 << 5;
const dcFlag F_LBA					= 1 << 6;
const dcFlag F_HPSERVICE			= 1 << 7;
const dcFlag F_PALO					= 1 << 8;
const dcFlag F_PREP					= 1 << 9;
const dcFlag F_MSFT_RESERVED		= 1 << 10;
const dcFlag F_BIOS_GRUB			= 1 << 11;
const dcFlag F_APPLE_TV_RECOVERY	= 1 << 12;
const dcFlag F_DIAG					= 1 << 13;

/**
 * \enum partType
 * \brief Type of the partition (Primary, logical or extended)
 *
 * \var PARTITION_PRIMARY
 * 	Primary partition
 * \var PARTITION_LOGICAL
 * 	Logical partition
 * \var PARTITION_EXTENDED
 * 	Extended partition
 */
enum partType {PARTITION_PRIMARY, PARTITION_LOGICAL, PARTITION_EXTENDED};

/**
 * \struct partInfo
 * \brief Contains all the data necessary to restore a partition in the
 * destination
 *
 * All its data is contained in fixed-size variables, in order to make the
 * transfer over the network safer and compatible with 32 and 64 bits
 * processors.
 */
struct partInfo {
	/// Flags for this partition
	dcFlag flags;
	/// Percentage that represents the position of this partition in the disk
	uint64_t start_pos;
	/// Percentage of disk used by the partition
	uint64_t used_part;
	/// Value of partType enumerator: Logical, primary, extended
	uint8_t type;
	/// Minimum size of the partition (bytes)
	uint64_t min_size;
	/// The name for filesystem in libdoclone
	uint8_t fs_name[32];
	/// The partition's label
	uint8_t label[28];
	/// The partition's uuid
	uint8_t uuid[37];
	/// Root directory of the partition in the image
	uint8_t root_dir[32];
};

/**
 * \class Partition
 * \brief Partition of the hard disk.
 *
 * All the operations related with partitions are implemented right here.
 * This class can mount, unmount, format, write/read uuid, labels and data, etc.
 *
 * \date August, 2011
 */
class Partition {
public:
	Partition(const std::string &path) throw(Exception);
	Partition(Doclone::partInfo header) throw(Exception);
	~Partition();
	
	// Getters and setters
	Doclone::partType getType() const;
	uint64_t getMinSize() const;
	double getStartPos() const;
	double getUsedPart() const;
	Doclone::partInfo getPartition() const;
	void setPath(const std::string &path);
	const std::string &getPath() const;
	void setPartNum(unsigned int);
	unsigned int getPartNum() const;
	Filesystem * getFileSystem();
	const std::string &getMountPoint() const;
	
	void format() const throw(Exception);
	void writeLabel() const throw(Exception);
	void writeUUID() const throw(Exception);
	void writeFlags() const throw(Exception);
	
	void doMount() throw(Exception);
	void doUmount() throw(Exception);
	
	void createPartInfo() throw(Exception);
	
	bool isWritable() const throw(Exception);
	bool fitInDevice() throw(Exception);

private:
	/// The partition path
	std::string _path;
	/// Partition number
	unsigned int _partNum;
	/// Used space in the partition (bytes)
	uint64_t _minSize;
	/// Start Position (% of the disk)
	double _startPos;
	/// Used space (%)
	double _usedPart;
	/// Filesystem of the partition
	Filesystem *_fs;
	/// Type of partition (primary, logical, extended)
	Doclone::partType _type;
	/// Flags of the partition
	Doclone::dcFlag _flags;
	/// Path to the mount point
	std::string _mountPoint;
	/// Metadata of the partition
	Doclone::partInfo _partition;
	
	bool isMounted() throw(Exception);
	void externalMount() throw(Exception);
	
	// Initialize functions
	void initNum() throw(Exception);
	void initType() throw(Exception);
	void initFS() throw(Exception);
	void initMinSize() throw(Exception);
	void initStartPos() throw(Exception);
	void initUsedPart() throw(Exception);
	void initFlags() throw(Exception);
	void initLabel() throw(Exception);
	void initUUID() throw(Exception);
	
	uint64_t usedSpace() throw(Exception);
};

}

#endif /* PARTITION_H_ */

