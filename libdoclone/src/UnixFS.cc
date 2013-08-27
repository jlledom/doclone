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

#include <doclone/UnixFS.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <utime.h>
#include <errno.h>
#include <endian.h>

#include <string>
#include <map>

#include <selinux/selinux.h>

#include <doclone/Logger.h>
#include <doclone/DataTransfer.h>
#include <doclone/Util.h>
#include <doclone/exception/Exception.h>
#include <doclone/exception/FileNotFoundException.h>
#include <doclone/exception/OpenFileException.h>
#include <doclone/exception/CreateFileException.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/InvalidImageException.h>
#include <doclone/exception/WriteErrorsInDirectoryException.h>
#include <doclone/exception/ReadErrorsInDirectoryException.h>
#include <doclone/exception/RestoreImageException.h>

namespace Doclone {

/**
 * \brief Initializes the attributes
 */
UnixFS::UnixFS()
	: _hardLinks() {
}

/**
 * \brief Reads the data and metadata of a regular file in a unix filesystem and
 * transfers it to the destination.
 *
 * \param path
 * 		The path of the file
 * \param filestat
 * 		The file metadata
 */
void UnixFS::readRegularFile(const std::string &path, const struct stat &filestat) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::readRegularFile(path=>%s, filestat=>0x%x) start", path.c_str(), &filestat);
	
	Doclone::unixFile file = {};
	security_context_t context;	// Selinux context of this file
	DataTransfer *trns = DataTransfer::getInstance();

	std::string fileName = basename(path.c_str());
	strncpy(reinterpret_cast<char*>(file.name), fileName.c_str(), 512);
	file.mode = filestat.st_mode;
	file.uid = filestat.st_uid;
	file.gid = filestat.st_gid;
	file.size = filestat.st_size;
	file.atime = filestat.st_atime;
	file.mtime = filestat.st_mtime;

	// Selinux extended attributes
	if(getfilecon (path.c_str(), &context)<0) {
		strncpy(reinterpret_cast<char*>(file.se_xattr), "", 0);
	}
	else {
		strncpy (reinterpret_cast<char*>(file.se_xattr), context, 128);
	}
	
	int fdfile;

	this->writeFileHeader(file);

	fdfile = open (path.c_str(), O_RDONLY);
	
	trns->transferFrom(fdfile, file.size);

	close(fdfile);
	
	log->loopDebug("UnixFS::readRegularFile() end");
}

/**
 * \brief Reads the data and metadata of a device file in a unix filesystem and
 * transfers it to the destination.
 *
 * \param path
 * 		The path of the device
 * \param filestat
 * 		The file metadata
 */
void UnixFS::readDevice(const std::string &path, const struct stat &filestat) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::readDevice(path=>%s, filestat=>0x%x) start", path.c_str(), &filestat);
	
	Doclone::unixFile file = {};
	security_context_t context;

	std::string fileName = basename(path.c_str());
	strncpy(reinterpret_cast<char*>(file.name), fileName.c_str(), 512);
	file.mode = filestat.st_mode;
	file.uid = filestat.st_uid;
	file.gid = filestat.st_gid;
	file.size = 0;
	file.atime = filestat.st_atime;
	file.mtime = filestat.st_mtime;
	file.dev = filestat.st_rdev;
	
	// Selinux extended attributes
	if(getfilecon (path.c_str(), &context)<0) {
		strncpy(reinterpret_cast<char*>(file.se_xattr), "", 0);
	}
	else {
		strncpy (reinterpret_cast<char*>(file.se_xattr), context, 128);
	}

	this->writeFileHeader(file);
	
	log->loopDebug("UnixFS::readDevice() end");
}

/**
 * \brief Reads the data and metadata of a symbolic link in a unix filesystem
 * and transfers it to the destination.
 *
 * \param path
 * 		The path of the symbolic link
 * \param filestat
 * 		The file metadata
 */
void UnixFS::readSymlink(const std::string &path, const struct stat &filestat) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::readSymlink(path=>%s, filestat=>0x%x) start", path.c_str(), &filestat);
	
	char linkPath[4096]  = {};
	Doclone::unixFile file  = {};
	security_context_t context;
	int size = 0;
	
	// Read link
	if ((size = readlink (path.c_str(), linkPath, sizeof(linkPath))) < 0) {
		FileNotFoundException ex(path);
		throw ex;
	}
	
	std::string fileName = basename(path.c_str());
	strncpy(reinterpret_cast<char*>(file.name), fileName.c_str(), 512);
	file.size = size;
	file.mode = filestat.st_mode;
	file.uid = filestat.st_uid;
	file.gid = filestat.st_gid;
	file.atime = filestat.st_atime;
	file.mtime = filestat.st_mtime;
	if(lgetfilecon (path.c_str(), &context)<0) {
		strncpy(reinterpret_cast<char*>(file.se_xattr), "", 0);
	}
	else {
		strncpy (reinterpret_cast<char*>(file.se_xattr), context, 128);
	}

	this->writeFileHeader(file);

	DataTransfer *trns = DataTransfer::getInstance();

	trns->transferFrom(linkPath, file.size);
	
	log->loopDebug("UnixFS::readSymlink() end");
}

/**
 * \brief Reads the data and metadata of a hard link in a unix filesystem and
 * transfers it to the destination.
 *
 * \param path
 * 		The path of the hard link
 * \param filestat
 * 		The file metadata
 */
void UnixFS::readHardLink(const std::string &path, const struct stat &filestat) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::readHardLink(path=>%s, filestat=>0x%x) start", path.c_str(), &filestat);
	
	Doclone::unixFile file = {};
	security_context_t context;
	std::map<ino_t, std::string>::iterator it;
	DataTransfer *trns = DataTransfer::getInstance();

	std::string fileName = basename(path.c_str());
	strncpy(reinterpret_cast<char*>(file.name), fileName.c_str(), 512);
	file.mode = filestat.st_mode;
	file.uid = filestat.st_uid;
	file.gid = filestat.st_gid;
	file.size = filestat.st_size;
	file.atime = filestat.st_atime;
	file.mtime = filestat.st_mtime;

	// Selinux extended attributes
	if(getfilecon (path.c_str(), &context)<0) {
		strncpy(reinterpret_cast<char*>(file.se_xattr), "", 0);
	}
	else {
		strncpy (reinterpret_cast<char*>(file.se_xattr), context, 128);
	}
	
	it = this->_hardLinks.find(filestat.st_ino);
	
	if (it == this->_hardLinks.end()) {
		// The hard link isn't already in the map
		int fdfile;

		this->_hardLinks.insert(std::pair<ino_t, std::string>(filestat.st_ino, path.c_str()));

		fdfile = open (path.c_str(), O_RDONLY);

		this->writeFileHeader(file);

		trns->transferFrom(fdfile, file.size);

		close(fdfile);
	} else { // The hard link is in the map
		std::string relPath=(*it).second.substr(this->_mountPoint.length(),
				(*it).second.length());
		char *ptr = const_cast<char*>(relPath.c_str());
		
		file.mode = (filestat.st_mode & 0x0fff) | Doclone::S_IFHLN;
		file.size = (*it).second.length();
		this->writeFileHeader(file);

		DataTransfer *trns = DataTransfer::getInstance();

		trns->transferFrom(ptr, file.size);
	}
	
	log->loopDebug("UnixFS::readHardLink() end");
}

/**
 * \brief Reads the data and metadata of a pipe file in a unix filesystem and
 * transfers it to the destination.
 *
 * \param path
 * 		The path of the pipe
 * \param filestat
 * 		The file metadata
 */
void UnixFS::readPipe(const std::string &path, const struct stat &filestat) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::readPipe(path=>%s, filestat=>0x%x) start", path.c_str(), &filestat);
	
	Doclone::unixFile file = {};
	security_context_t context;

	std::string fileName = basename(path.c_str());
	strncpy(reinterpret_cast<char*>(file.name), fileName.c_str(), 512);
	file.mode = filestat.st_mode;
	file.uid = filestat.st_uid;
	file.gid = filestat.st_gid;
	file.size = 0;
	file.atime = filestat.st_atime;
	file.mtime = filestat.st_mtime;
	
	// Selinux extended attributes
	if(getfilecon (path.c_str(), &context)<0) {
		strncpy(reinterpret_cast<char*>(file.se_xattr), "", 0);
	}
	else {
		strncpy (reinterpret_cast<char*>(file.se_xattr), context, 128);
	}

	this->writeFileHeader(file);
	
	log->loopDebug("UnixFS::readPipe() end");
}

/**
 * \brief Reads the data and metadata of a directory in a unix filesystem.
 *
 * This function is recursive and calls itself for every sub-directory found.
 *
 * \param path
 * 		The path of the directory
 */
void UnixFS::readDir(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::readDir(path=>%s) start", path.c_str());
	
	DIR *directory;
	struct dirent *d_file;		// a file in *directory
	bool errors = false;
	
	if ((directory = opendir (path.c_str())) == 0) {
		FileNotFoundException ex(path);
		throw ex;
	}

	while ((d_file = readdir (directory)) != 0) {
		struct stat filestat;
		std::string abPath;
		
		abPath = path;
		abPath.append(d_file->d_name);
		
		if (lstat (abPath.c_str(), &filestat) < 0) {
			FileNotFoundException ex(abPath);
			throw ex;
		}

		try {
			switch (filestat.st_mode & S_IFMT) {
			case S_IFDIR: {
				if (strcmp (".", d_file->d_name) && strcmp ("..", d_file->d_name)) {
					Doclone::unixFile file = {};
					security_context_t context;

					// write the dir in fd
					strncpy (reinterpret_cast<char*>(file.name),
							d_file->d_name, 512);
					file.mode = filestat.st_mode;
					file.uid = filestat.st_uid;
					file.gid = filestat.st_gid;
					file.size = 0;
					file.atime = filestat.st_atime;
					file.mtime = filestat.st_mtime;
					// Selinux extended attributes
					if(getfilecon (abPath.c_str(), &context)<0) {
						strncpy(reinterpret_cast<char*>(file.se_xattr),
								"", 0);
					} else {
						strncpy (reinterpret_cast<char*>(file.se_xattr),
								context, 128);
					}

					this->writeFileHeader(file);

					/*
					 * If the current folder is a virtual one, bypass
					 */
					if(Util::isVirtualDirectory(abPath.c_str())) {
						/*
						 * Write a empty unixFile object that indicates the end
						 * of the virtual folder
						 */
						Doclone::unixFile eodFil = {};
						eodFil.mode = Doclone::EOD;
						this->writeFileHeader(eodFil);

						continue;
					}

					abPath.push_back('/');
					this->readDir(abPath);
				}
				break;
			}
			case S_IFREG: {
				// If it has more than one hard link
				if (filestat.st_nlink > 1) {
					this->readHardLink(abPath, filestat);
					break;
				}
				// else
				/*
				 * If the current file is a virtual one, bypass
				 */
				if(Util::isLiveFile(abPath.c_str())) {
					continue;
				}

				this->readRegularFile(abPath, filestat);
				break;
			}
			case S_IFCHR:
			case S_IFBLK: {
				this->readDevice(abPath, filestat);
				break;
			}
			case S_IFLNK: {
				this->readSymlink(abPath, filestat);
				break;
			}
			case S_IFIFO:
			case S_IFSOCK: {
				this->readPipe(abPath, filestat);
				break;
			}
			default:
				ReadDataException ex;
				throw ex;
			}
		}catch(const WarningException &ex) {
			errors = true;
		}
	}

	closedir (directory);
	
	// Write a empty unixFile object that indicates the end of this directory
	Doclone::unixFile tmpFil = {};
	tmpFil.mode = Doclone::EOD;
	this->writeFileHeader(tmpFil);
	
	if(errors) {
		ReadErrorsInDirectoryException ex(path);
		ex.logMsg();
	}

	log->loopDebug("UnixFS::readDir() end");
}

/**
 * \brief Writes the data and metadata of a regular file in a unix filesystem
 * and transfers it to the destination.
 *
 * \param path
 * 		The path of the file
 * \param file
 * 		The file metadata
 */
void UnixFS::writeRegularFile(const std::string &path, Doclone::unixFile &file) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::writeRegularFile(path=>%s, file=>0x%x) start", path.c_str(), &file);
	
	DataTransfer *trns = DataTransfer::getInstance();
	int fdfile;
	
	if((fdfile = open (path.c_str(), O_WRONLY | O_CREAT, 0))<0) {
		CreateFileException ex(path);
		throw ex;
	}

	utimbuf time={static_cast<time_t>(file.atime),
			static_cast<time_t>(file.mtime)};
	chown (path.c_str(), file.uid, file.gid);
	utime (path.c_str(), &time);
	chmod (path.c_str(), file.mode);
	
	setfilecon (path.c_str(), reinterpret_cast<char*>(file.se_xattr));

	/*
	 * If this file cannot be transferred for any reason, we need its
	 * position in the image file for pass it by (See the catch clause
	 * bellow).
	 */
	off_t fileOffset = trns->getTransferredBytes();

	try {
		if (file.size != 0) {
			trns->transferTo(fdfile, file.size);
		}
	} catch (WarningException &ex) {
		close(fdfile);

		/*
		 * No space left in the partition for this file. We must pass over
		 * the file data.
		 */
		this->goToNextFile(fileOffset, file.size);

		throw;
	}
	
	close(fdfile);

	log->loopDebug("UnixFS::writeRegularFile() end");
}

/**
 * \brief Writes the data and metadata of a device file in a unix filesystem
 * and transfers it to the destination.
 *
 * \param path
 * 		The path of the device
 * \param file
 * 		The file metadata
 */
void UnixFS::writeDevice(const std::string &path, Doclone::unixFile &file) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::writeDevice(path=>%s, file=>0x%x) start", path.c_str(), &file);
	
	if((mknod (path.c_str(), file.mode, file.dev))<0) {
		CreateFileException ex(path);
		throw ex;
	}
	utimbuf time={static_cast<time_t>(file.atime),
			static_cast<time_t>(file.mtime)};
	chown (path.c_str(), file.uid, file.gid);
	utime (path.c_str(), &time);
	
	setfilecon (path.c_str(), reinterpret_cast<char*>(file.se_xattr));
	
	log->loopDebug("UnixFS::writeDevice() end");
}

/**
 * \brief Writes the data and metadata of a symbolic link in a unix filesystem
 * and transfers it to the destination.
 *
 * \param path
 * 		The path of the symbolic link
 * \param file
 * 		The file metadata
 */
void UnixFS::writeSymlink(const std::string &path, Doclone::unixFile &file) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::writeSymlink(path=>%s, file=>0x%x) start", path.c_str(), &file);
	
	char *linkPath;
	
	linkPath = new char[file.size + 1];	// +1 for '\0'
	
	DataTransfer *trns = DataTransfer::getInstance();

	trns->transferTo(linkPath, file.size);
	linkPath[file.size] = '\0';
	
	// Create the link
	if((symlink (linkPath, path.c_str()))<0) {
		CreateFileException ex(path);
		throw ex;
	}
	
	lchown (path.c_str(), file.uid, file.gid);
	
	lsetfilecon (path.c_str(), reinterpret_cast<char*>(file.se_xattr));

	delete[] linkPath;
	
	log->loopDebug("UnixFS::writeSymlink() end");
}

/**
 * \brief Writes the data and metadata of a hard link in a unix filesystem
 * and transfers it to the destination.
 *
 * \param path
 * 		The path of the hard link
 * \param file
 * 		The file metadata
 */
void UnixFS::writeHardLink(const std::string &path, Doclone::unixFile &file) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::writeHardLink(path=>%s, file=>0x%x) start", path.c_str(), &file);
	
	char linkPath[file.size+1]; //+1 for \0
	
	DataTransfer *trns = DataTransfer::getInstance();

	/*
	 * If this file cannot be transferred for any reason, we need its
	 * position in the image file for pass it by (See the catch clause
	 * bellow).
	 */
	off_t fileOffset = trns->getTransferredBytes();

	try {
		trns->transferTo(linkPath, file.size);
		linkPath[file.size] = '\0';
	} catch (WarningException &ex) {
		/*
		 * No space left in the partition for this file. We must pass over
		 * the file data.
		 */
		this->goToNextFile(fileOffset, file.size);

		throw;
	}
	
	std::string target = path.substr(0, this->_mountPoint.length())+"/"+linkPath;
	
	if((link (target.c_str(), path.c_str()))<0) {
		CreateFileException ex(path);
		throw ex;
	}
	
	lsetfilecon (path.c_str(), reinterpret_cast<char*>(file.se_xattr));
	
	log->loopDebug("UnixFS::writeHardLink() end");
}

/**
 * \brief Writes the data and metadata of a pipe file in a unix filesystem
 * and transfers it to the destination.
 *
 * \param path
 * 		The path of the pipe
 * \param file
 * 		The file metadata
 */
void UnixFS::writePipe(const std::string &path, Doclone::unixFile &file) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::writePipe(path=>%s, file=>0x%x) start", path.c_str(), &file);
	
	if((mknod (path.c_str(), file.mode, 0))<0) {
		CreateFileException ex(path);
		throw ex;
	}
	utimbuf time={static_cast<time_t>(file.atime),
			static_cast<time_t>(file.mtime)};
	chown (path.c_str(), file.uid, file.gid);
	utime (path.c_str(), &time);
	
	setfilecon (path.c_str(), reinterpret_cast<char*>(file.se_xattr));
	
	log->loopDebug("UnixFS::writePipe() end");
}

/**
 * \brief Writes the data and metadata of a directory in a unix filesystem.
 *
 * This function is recursive and calls itself for all sub-directory found.
 *
 * \param path
 * 		The path of the directory
 */
void UnixFS::writeDir(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("UnixFS::writeDir(path=>%s) start", path.c_str());
	
	Doclone::unixFile file;
	std::string abPath;
	bool errors = false;

	while((this->readFileHeader(file)) > 0
			&& (file.mode & S_IFMT) != Doclone::EOD) {
		abPath = path;
		abPath.append(reinterpret_cast<char*>(file.name));

		try {
			switch (file.mode & S_IFMT) {
			case S_IFDIR: {
				if((mkdir (abPath.c_str(), 0))<0) {
					if(errno != EEXIST) {
						this->goToNextDir();
					}
				}
				utimbuf time={static_cast<time_t>(file.atime),
						static_cast<time_t>(file.mtime)};
				chown (abPath.c_str(), file.uid, file.gid);
				utime (abPath.c_str(), &time);
				chmod (abPath.c_str(), file.mode);

				setfilecon (abPath.c_str(), reinterpret_cast<char*>(file.se_xattr));

				abPath.append("/");
				this->writeDir (abPath);	// enter the directory
				break;
			}
			case S_IFREG: {
				this->writeRegularFile(abPath, file);
				break;
			}
			case S_IFBLK:
			case S_IFCHR: {
				this->writeDevice(abPath, file);
				break;
			}
			case S_IFLNK: {
				this->writeSymlink(abPath, file);
				break;
			}
			case S_IFHLN: {
				this->writeHardLink(abPath, file);
				break;
			}
			case S_IFIFO:
			case S_IFSOCK: {
				this->writePipe(abPath, file);
				break;
			}
			default: {
				InvalidImageException ex;
				ex.logMsg();
				throw ex;
			}
			}
		} catch(const WarningException &ex) {
			errors = true;
		}
	}
	
	if(errors) {
		WriteErrorsInDirectoryException ex(path);
		ex.logMsg();
	}

	log->loopDebug("UnixFS::writeDir() end");
}

/**
 * \brief Reads the metadata for a unix file.
 *
 * \param [out] file
 * 		The container of the metadata
 *
 * \return Number of bytes read.
 */
ssize_t UnixFS::readFileHeader(Doclone::unixFile &file) const throw(Exception) {
	Logger *log=Logger::getInstance();
	log->loopDebug("UnixFS::readFileHeader() start");

	DataTransfer *trns=DataTransfer::getInstance();

	ssize_t readedBytes=0;

	memset(&file, 0, sizeof(Doclone::unixFile));

	readedBytes+=trns->transferTo(&file.name, sizeof(uint8_t)*512);
	readedBytes+=trns->transferTo(&file.mode, sizeof(uint32_t));
	readedBytes+=trns->transferTo(&file.uid, sizeof(uint32_t));
	readedBytes+=trns->transferTo(&file.gid, sizeof(uint32_t));
	readedBytes+=trns->transferTo(&file.size, sizeof(uint64_t));
	readedBytes+=trns->transferTo(&file.atime, sizeof(uint64_t));
	readedBytes+=trns->transferTo(&file.mtime, sizeof(uint64_t));
	readedBytes+=trns->transferTo(&file.se_xattr, sizeof(uint8_t)*128);
	readedBytes+=trns->transferTo(&file.dev, sizeof(uint64_t));
	readedBytes+=trns->transferTo(&file.dummy, sizeof(uint8_t)*512);

	file.mode = be32toh(file.mode);
	file.uid = be32toh(file.uid);
	file.gid = be32toh(file.gid);
	file.size = be64toh(file.size);
	file.atime = be64toh(file.atime);
	file.mtime = be64toh(file.mtime);
	file.dev = be64toh(file.dev);

	log->loopDebug("UnixFS::readFileHeader(readedBytes=>%d) end", readedBytes);

	return readedBytes;
}

/**
 * \brief Writes the metadata for a unix file.
 *
 * Cross-platform.
 *
 * \param file
 * 		The container of the metadata
 *
 * \return Number of bytes written.
 */
ssize_t UnixFS::writeFileHeader(const Doclone::unixFile &file) const throw(Exception) {
	Logger *log=Logger::getInstance();
	log->loopDebug("UnixFS::writeFileHeader() start");

	DataTransfer *trns=DataTransfer::getInstance();

	ssize_t writtenBytes=0;

	uint32_t tmpMode = htobe32(file.mode);
	uint32_t tmpUid = htobe32(file.uid);
	uint32_t tmpGid = htobe32(file.gid);
	uint64_t tmpSize = htobe64(file.size);
	uint64_t tmpAtime = htobe64(file.atime);
	uint64_t tmpMtime = htobe64(file.mtime);
	uint64_t tmpDev = htobe64(file.dev);

	writtenBytes+=trns->transferFrom(file.name, sizeof(uint8_t)*512);
	writtenBytes+=trns->transferFrom(&tmpMode, sizeof(uint32_t));
	writtenBytes+=trns->transferFrom(&tmpUid, sizeof(uint32_t));
	writtenBytes+=trns->transferFrom(&tmpGid, sizeof(uint32_t));
	writtenBytes+=trns->transferFrom(&tmpSize, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(&tmpAtime, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(&tmpMtime, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(file.se_xattr, sizeof(uint8_t)*128);
	writtenBytes+=trns->transferFrom(&tmpDev, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(file.dummy, sizeof(uint8_t)*512);

	log->loopDebug("UnixFS::writeFileHeader(writtenBytes=>%d) end", &writtenBytes);

	return writtenBytes;
}

/**
 * \brief When writing a directory fails, it must pass over
 * to the next directory.
 */
void UnixFS::goToNextDir() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("UnixFS::goToNextDir() start");

	DataTransfer *trns = DataTransfer::getInstance();

	int dirIndex = 1;
	Doclone::unixFile file;

	do {
		this->readFileHeader(file);

		switch (file.mode & S_IFMT) {
			case S_IFDIR: {
				dirIndex++;

				break;
			}
			case Doclone::EOD: {
				dirIndex--;

				break;
			}
		}

		int devNullFd = Util::openFile(DEV_NULL_PATH);

		// It must pass over the file data.
		trns->transferTo(devNullFd, file.size);
	}while(dirIndex>0);

	log->debug("UnixFS::goToNextDir() end");
}

/**
 * \brief When writing a file fails, it must pass over to the next file.
 */
void UnixFS::goToNextFile(off_t fileOffset, uint64_t fileSize)
		const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("UnixFS::goToNextFile(fileOffset=> %d, fileSize=>%d) start",
			fileOffset, fileSize);

	DataTransfer *trns = DataTransfer::getInstance();

	// Written bytes when the error happens
	off_t errorOffset = trns->getTransferredBytes();

	// Written bytes of this file
	off_t writtenBytes = errorOffset - fileOffset;

	// Remaining bytes to complete this file
	off_t remainingBytes= fileSize - writtenBytes;

	int devNullFd = Util::openFile(DEV_NULL_PATH);

	trns->transferTo(devNullFd, remainingBytes);

	log->debug("UnixFS::goToNextFile() end");
}

}
