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
	Partition();
	~Partition();

	// Getters and setters
	Doclone::partType getType() const;
	void setType(Doclone::partType type);
	uint64_t getMinSize() const;
	void setMinSize(uint64_t minSize);
	double getStartPos() const;
	void setStartPos(double startPos);
	double getUsedPart() const;
	void setUsedPart(double usedPart);
	const std::string &getPath() const;
	Doclone::dcFlag getFlags() const;
	void setFlags(Doclone::dcFlag flags);
	void setPath(const std::string &path);
	unsigned int getPartNum() const;
	void setPartNum(unsigned int);
	Filesystem *getFileSystem() const;
	void setFileSystem(Filesystem *fs);
	const std::string &getMountPoint() const;
	const std::string &getRootDir() const;
	void setRootDir(const std::string &rootDir);

	void initFromPath(const std::string &path) throw(Exception);

	void clearSignatures() const throw(Exception);
	void format() const throw(Exception);
	void writeLabel() const throw(Exception);
	void writeUUID() const throw(Exception);
	void writeFlags() const throw(Exception);

	void doMount() throw(Exception);
	void doUmount() throw(Exception);
	bool isMounted() throw(Exception);

	bool isWritable() const throw(Exception);
	bool fitInDevice(bool diskImage) throw(Exception);

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
	/// Root directory of the partition in the image header
	std::string _rootDir;

	void externalMount() throw(Exception);

	// Initialize functions
	void initNum() throw(Exception);
	void initType() throw(Exception);
	void initFS() throw(Exception);
	void initMinSize() throw(Exception);
	void initStartPos() throw(Exception);
	void initUsedPart() throw(Exception);
	void initFlags() throw(Exception);

	uint64_t usedSpace() throw(Exception);
};

}

#endif /* PARTITION_H_ */
