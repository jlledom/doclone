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

#include <doclone/DosFS.h>

#include <unistd.h>
#include <sys/types.h>
#include <utime.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <endian.h>

#include <string>

#include <doclone/Logger.h>
#include <doclone/DataTransfer.h>
#include <doclone/Util.h>
#include <doclone/exception/FileNotFoundException.h>
#include <doclone/exception/CreateFileException.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/InvalidImageException.h>
#include <doclone/exception/WriteErrorsInDirectoryException.h>
#include <doclone/exception/ReadErrorsInDirectoryException.h>
#include <doclone/exception/RestoreImageException.h>

namespace Doclone {

/**
 * \brief Reads the data and metadata of a regular file in a DOS filesystem and
 * transfers it to the destination.
 *
 * \param path
 * 		The path of the file
 * \param filestat
 * 		The file metadata
 */
void DosFS::readRegularFile(const std::string &path, const struct stat &filestat) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DosFS::readRegularFile(path=>%s, filestat=>0x%x) start", path.c_str(), &filestat);

	Doclone::dosFile file = {};
	DataTransfer *trns = DataTransfer::getInstance();

	std::string fileName = basename(path.c_str());
	strncpy(reinterpret_cast<char*>(file.name), fileName.c_str(), 512);
	file.mode = filestat.st_mode;
	file.size = filestat.st_size;
	file.atime = filestat.st_atime;
	file.mtime = filestat.st_mtime;
	
	int fdfile;

	this->writeFileHeader(file);

	fdfile = open (path.c_str(), O_RDONLY);

	trns->transferFrom(fdfile, file.size);

	close(fdfile);
	
	log->loopDebug("DosFS::readRegularFile() end");
}

/**
 * \brief Reads the data and metadata of a directory in a DOS filesystem.
 *
 * This function is recursive and calls itself for every sub-directory found.
 *
 * \param path
 * 		The path of the directory
 */
void DosFS::readDir(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DosFS::readDir(path=>%s) start", path.c_str());
	
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
					Doclone::dosFile file = {};

					// Write the directory on descriptor
					strncpy (reinterpret_cast<char*>(file.name),
							d_file->d_name, 512);
					file.mode = filestat.st_mode;
					file.size = 0;
					file.atime = filestat.st_atime;
					file.mtime = filestat.st_mtime;

					this->writeFileHeader(file);

					/*
					 * If the current folder is a virtual one or is the mount
					 * point of another partition, bypass it.
					 */
					if(Util::isVirtualDirectory(abPath.c_str())
						|| Util::isMountPoint(abPath)) {
						/*
						 * Write a empty unixFile object that indicates the end
						 * of the virtual folder
						 */
						Doclone::dosFile eodFil = {};
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
				/*
				 * If the current file is a virtual one, bypass
				 */
				if(Util::isLiveFile(abPath.c_str())) {
					continue;
				}

				this->readRegularFile(abPath, filestat);
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

	// Write a empty dosFile object that indicates the end of this directory
	Doclone::dosFile tmpFil = {};
	tmpFil.mode = Doclone::EOD;
	this->writeFileHeader(tmpFil);
	
	if(errors) {
		ReadErrorsInDirectoryException ex(path);
		ex.logMsg();
	}

	log->loopDebug("DosFS::readDir() end");
}

/**
 * \brief Writes the data and metadata of a regular file in a DOS filesystem and
 * transfers it to the destination.
 *
 * \param path
 * 		The path of the file
 * \param file
 * 		The file metadata
 */
void DosFS::writeRegularFile(const std::string &path, const Doclone::dosFile &file) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DosFS::writeRegularFile(path=>%s, file=>0x%x) start", path.c_str(), &file);
	
	DataTransfer *trns = DataTransfer::getInstance();
	int fdfile;
	
	if((fdfile = open (path.c_str(), O_WRONLY | O_CREAT, 0))<0) {
		CreateFileException ex(path);
		throw ex;
	}
	utimbuf time= {static_cast<time_t>(file.atime),
			static_cast<time_t>(file.mtime)};
	utime (path.c_str(), &time);
	chmod (path.c_str(), file.mode);

	/*
	 * If this file cannot be transferred for any reason, we need its position
	 * in the image file to pass it by (See the catch clause bellow).
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
	
	log->loopDebug("DosFS::writeRegularFile() end");
}

/**
 * \brief Writes the data and metadata of a directory in a DOS filesystem.
 *
 * This function is recursive and calls itself for every sub-directory found.
 *
 * \param path
 * 		The path of the directory
 */
void DosFS::writeDir(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DosFS::writeDir(path=>%s start", path.c_str());
	
	Doclone::dosFile file;
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
				utime (abPath.c_str(), &time);
				chmod (abPath.c_str(), file.mode);

				abPath.append("/");
				this->writeDir (abPath);	// enter the directory
				break;
			}
			case S_IFREG: {
				this->writeRegularFile(abPath, file);
				break;
			}
			default: {
				InvalidImageException ex;
				ex.logMsg();
				throw ex;
			}
			}
		}catch(const WarningException &ex) {
			errors = true;
		}
	}
	
	if(errors) {
		WriteErrorsInDirectoryException ex(path);
		ex.logMsg();
	}

	log->loopDebug("DosFS::writeDir() end");
}

/**
 * \brief Reads a metadata for a DOS file.
 *
 * Cross-platform
 *
 * \param [out] file
 * 		The container of the metadata
 *
 * \return Number of bytes read.
 */
ssize_t DosFS::readFileHeader(Doclone::dosFile &file) const throw(Exception) {
	Logger *log=Logger::getInstance();
	log->loopDebug("DosFS::readFileHeader() start");

	DataTransfer *trns=DataTransfer::getInstance();

	ssize_t readedBytes=0;

	memset(&file, 0, sizeof(Doclone::dosFile));

	readedBytes+=trns->transferTo(&file.name, sizeof(int8_t)*512);
	readedBytes+=trns->transferTo(&file.mode, sizeof(int32_t));
	readedBytes+=trns->transferTo(&file.size, sizeof(int64_t));
	readedBytes+=trns->transferTo(&file.atime, sizeof(int64_t));
	readedBytes+=trns->transferTo(&file.mtime, sizeof(int64_t));
	readedBytes+=trns->transferTo(&file.dummy, sizeof(uint8_t)*512);

	// The integer types must be converted to the host byte order
	file.mode = be32toh(file.mode);
	file.size = be64toh(file.size);
	file.atime = be64toh(file.atime);
	file.mtime = be64toh(file.mtime);

	log->loopDebug("DosFS::readFileHeader(readedBytes=>%d) end", readedBytes);

	return readedBytes;
}

/**
 * \brief Writes a metadata for a DOS file.
 *
 * Cross-platform.
 *
 * \param file
 * 		The container of the metadata
 *
 * \return Number of bytes written.
 */
ssize_t DosFS::writeFileHeader(const Doclone::dosFile &file) const throw(Exception) {
	Logger *log=Logger::getInstance();
	log->loopDebug("DosFS::writeFileHeader() start");

	DataTransfer *trns=DataTransfer::getInstance();

	ssize_t writtenBytes=0;

	// Convert to big endian before send/write
	uint32_t tmpMode = htobe32(file.mode);
	uint64_t tmpSize = htobe64(file.size);
	uint64_t tmpAtime = htobe64(file.atime);
	uint64_t tmpMtime = htobe64(file.mtime);

	writtenBytes+=trns->transferFrom(reinterpret_cast<const void *>(file.name), sizeof(int8_t)*512);
	writtenBytes+=trns->transferFrom(&tmpMode, sizeof(uint32_t));
	writtenBytes+=trns->transferFrom(&tmpSize, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(&tmpAtime, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(&tmpMtime, sizeof(uint64_t));
	writtenBytes+=trns->transferFrom(file.dummy, sizeof(uint8_t)*512);

	log->loopDebug("DosFS::writeFileHeader(writtenBytes=>%d) end", &writtenBytes);

	return writtenBytes;
}

/**
 * \brief When writing a directory fails, if it is working locally, it must
 * pass over to the next directory.
 */
void DosFS::goToNextDir() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("DosFS::goToNextDir() start");

	DataTransfer *trns = DataTransfer::getInstance();

	int dirIndex = 1;
	Doclone::dosFile file;

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

		// We must pass over the file data.
		trns->transferTo(devNullFd, file.size);
	}while(dirIndex>0);

	log->debug("DosFS::goToNextDir() end");
}

/**
 * \brief When writing a file fails, it must pass over to the next file.
 */
void DosFS::goToNextFile(off_t fileOffset, uint64_t fileSize)
		const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("DosFS::goToNextFile(fileOffset=> %d, fileSize=>%d) start",
			fileOffset, fileSize);

	DataTransfer *trns = DataTransfer::getInstance();

	// Written bytes when the error happens
	off_t errorOffset = trns->getTransferredBytes();

	// Written bytes of this file
	off_t writtenBytes = errorOffset - fileOffset;

	// Remaining bytes for complete this file
	off_t remainingBytes= fileSize - writtenBytes;

	int devNullFd = Util::openFile(DEV_NULL_PATH);

	trns->transferTo(devNullFd, remainingBytes);

	log->debug("DosFS::goToNextFile() end");
}

}
