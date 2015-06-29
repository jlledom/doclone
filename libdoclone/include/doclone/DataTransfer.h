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

#ifndef DATATRANSFER_H_
#define DATATRANSFER_H_

#include <stdint.h>
#include <sys/types.h>

#include <map>
#include <vector>

#include <archive.h>

#include <doclone/observer/AbstractSubject.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \typedef dcBuffSize
 *
 * Size of data buffer
 */
typedef off_t dcBuffSize;

/**
 * \var BUFFER_SIZE
 *
 * Size of data buffer
 */
const dcBuffSize BUFFER_SIZE = 10240;

/**
 * \var UPDATE_QUOTIENT
 *
 * When the remainder of the current transferred bytes divided by
 * (BUFFER_SIZE * UPDATE_QUOTIENT) equals 0, it notifies the observer.
 */
const unsigned int UPDATE_QUOTIENT = 15360;

/**
 * \typedef readFunction
 *
 * A pointer to a function. This kind of pointer can point to read() or recv()
 * function as required.
 */
typedef ssize_t (*readFunction) (int, void*, size_t);

/**
 * \typedef writeFunction
 *
 * A pointer to a function. This kind of pointer can point to write() or
 * send() function as required.
 */
typedef ssize_t (*writeFunction) (int, const void*, size_t);

/**
 * \class DataTransfer
 * \brief Singleton interface to get, set up and use the file/socket descriptors
 *
 * This class provides a set of utility methods for abstracting the reading and
 * writing of data with libarchive.
 *
 * For work in multicast, the server establishes a TCP connection with all the
 * receivers. For this reason, this class has the map of descriptors
 * this->_fdd, with its corresponding IP addresses.
 *
 * This class is singleton.
 * \date August, 2011
 */
class DataTransfer : public AbstractSubject {
public:
	~DataTransfer() {}
	static DataTransfer* getInstance();

	uint64_t archiveToBuf(struct archive *arIn, std::string &target) throw(Exception);
	uint64_t bufToArchive(const std::string &source, std::vector<struct archive*> &outArchives) throw(Exception);
	uint64_t fdToArchive(int fd, std::vector<struct archive*> &outArchives) throw(Exception);
	uint64_t copyData(struct archive *arIn, std::vector<struct archive *> &outArchives) throw(Exception);
	uint64_t copyData(int fdin, std::vector<int> &outFds) throw(Exception);
	uint64_t copyData(int fdin, int fdout) throw(Exception);
	void copyHeader(struct archive_entry *entry, std::vector<struct archive*> &outArchives) throw(Exception);

	void initLocalRead();
	void initSocketRead();
	void initLocalWrite();
	void initSocketWrite();

	static ssize_t readBytes (int s, void *buf, size_t len) throw (Exception);
	static ssize_t recvData (int s, void *buf, size_t len) throw (Exception);
	static ssize_t writeBytes (int s, const void *buf, size_t len) throw (Exception);
	static ssize_t sendData (int s, const void *buf, size_t len) throw (Exception);
	static ssize_t sendData (std::vector<int> &fds, const void *buf, size_t len) throw (Exception);

	void setTotalSize(const uint64_t size);

	uint64_t getTotalSize() const;
	uint64_t getTransferredBytes() const;

	/// Can be defined as read() or recv()
	Doclone::readFunction getNbytes;

	/// Can be defined as write() or send()
	Doclone::writeFunction putNbytes;
private:
	/// Private constructor is needed in Singleton pattern
	DataTransfer();

	/// Total size to transfer
	uint64_t _totalSize;
	/// Transferred bytes at the moment
	uint64_t _transferredBytes;
	/// Observers are be notified every time this amount of bytes is written
	uint32_t _notificationPointSize;
	/// Number of times the observers have been notified at the moment
	uint32_t _transferNotificationsCount;
};

}

#endif /* DATATRANSFER_H_ */
