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

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <string>

#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \var S_IFHLN
 *
 * A custom defined mode for identify hard links
 */
const unsigned int S_IFHLN = 9<<12;

/**
 * \var EOD
 *
 * Represents end of directory in readDir() and writeDir() functions
 */
const unsigned int EOD = 7<<12;

/**
 * \defgroup Filesystems Supported Filesystems
 * \brief List of supported Filesystems
 *
 * The program support many filesystems with varying levels of compatibility.
 *
 * For each Fs, there is a function called checkSupport that checks if the
 * system has installed the tools or libraries needed to mount, format,
 * read/write fs label and read/write fs uuid.
 *
 * Each object of Partition class has a private pointer to FileSystem class
 * called _fs. This pointer is initialized through the class FsFactory, with a
 * function that receives a string with the name of an fs and returns a pointer
 * to an object with the right type. There is also an overload of this function
 * that receives the TYPE tag of blkid to build the objects.
 *
 * There are two kinds of filesystems, represented as inheritances of FileSystem
 * class: UnixFS and DosFS. Each of these classes have children classes, one for
 * filesystem.
 *
 * The difference between these two classes is only the kind of file types used.
 * An Unix file can be a regular file, a hard link, soft link, device, etc. And
 * a DOS file can only be a file or a directory. In addition, doclone currently
 * does not support the cloning of the attributes of a DOS kind file.
 * @{
 */

/**
 * \enum Doclone::mountType
 * \brief The kind of mount for a file system.
 *
 * A file system can be mounted using the mount function or calling an external
 * command that does it.
 * \var Doclone::mountType::MOUNT_NATIVE
 * The filesystem is mounted with C mount() function
 * \var Doclone::mountType::MOUNT_EXTERNAL
 * The filesystem is mounted with an external program
 */
enum mountType {MOUNT_NATIVE, MOUNT_EXTERNAL};

/**
 * \enum Doclone::fsType
 * \brief The kind of a file system. (Unix or Dos type)
 *
 * A file system can be Unix type, with files, folders, block devices,
 * hard links, etc; or be DOS type, with only files and directories
 * \var Doclone::fsType::FSTYPE_NONE
 * No type, like linux-swap.
 * \var Doclone::fsType::FSTYPE_UNIX
 * Unix type, like reiserfs or ext2.
 * \var Doclone::fsType::FSTYPE_DOS
 * Dos type, like fat32 or ntfs.
 */
enum fsType {FSTYPE_NONE, FSTYPE_UNIX, FSTYPE_DOS};

/**
 * \enum Doclone::fsCode
 * \brief Code to identify the filesystem
 *
 * \var Doclone::fsCode::FS_EXT2
 * \var Doclone::fsCode::FS_EXT3
 * \var Doclone::fsCode::FS_EXT4
 * \var Doclone::fsCode::FS_FAT16
 * \var Doclone::fsCode::FS_FAT32
 * \var Doclone::fsCode::FS_HFS
 * \var Doclone::fsCode::FS_HFSP
 * \var Doclone::fsCode::FS_JFS
 * \var Doclone::fsCode::FS_LINUXSWAP
 * \var Doclone::fsCode::FS_NOFS
 * \var Doclone::fsCode::FS_NTFS
 * \var Doclone::fsCode::FS_REISERFS
 * \var Doclone::fsCode::FS_XFS
 */
enum fsCode {FS_EXT2, FS_EXT3, FS_EXT4, FS_FAT16, FS_FAT32, FS_HFS, FS_HFSP,
	FS_JFS, FS_LINUXSWAP, FS_NOFS, FS_NTFS, FS_REISERFS, FS_XFS};

/**
 * \class Filesystem
 * \brief Common functions for all filesystems.
 *
 * This class must not be instanced as is, but through any of its derived
 * classes.
 *
 * \date August, 2011
 */
class Filesystem {
public:
	Filesystem();
	virtual ~Filesystem() {}

	Doclone::mountType getMountType() const;
	Doclone::fsType getType() const;
	Doclone::fsCode getCode() const;
	const std::string &getdocloneName() const;
	const std::string &getMountName() const;
	const std::string &getMountOptions() const;
	const std::string &getCommand() const;
	const std::string &getFormatOptions() const;
	const std::string &getLabel() const;
	const std::string &getUUID() const;
	bool getMountSupport() const;
	bool getFormatSupport() const;
	bool getUUIDSupport() const;
	bool getLabelSupport() const;
	void setLabel(const std::string &label);
	void setUUID(const std::string &uuid);

	void readLabel(const std::string &dev) throw(Exception);
	void readUUID(const std::string &dev) throw(Exception);

	virtual void writeLabel(const std::string &dev) const throw(Exception) {}
	virtual void writeUUID(const std::string &dev) const throw(Exception) {}

protected:
	/// If the mount is native or external
	Doclone::mountType _mountType;
	/// If this filesystem is unix or dos type
	Doclone::fsType _type;
	/// doclone code for this fs
	Doclone::fsCode _code;
	/// The fs label
	std::string _label;
	/// The fs uuid
	std::string _uuid;
	/// Libdoclone name for this fs
	std::string _docloneName;
	/// Mount name of the fs
	std::string _mountName;
	/// Mount options for this fs
	std::string _mountOptions;
	/// Command to format
	std::string _command;
	/// Options to the format command
	std::string _formatOptions;
	/// Command to admin
	std::string _adminCommand;

	/// If it has mounting support
	bool _mountSupport;
	/// If it has formatting support
	bool _formatSupport;
	/// If it has uuid reading/writing support
	bool _uuidSupport;
	/// If it has label reading/writing support
	bool _labelSupport;
};
/**@}*/

}

#endif /* FILESYSTEM_H_ */
