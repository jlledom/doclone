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

#ifndef DATATRANSFER_H_
#define DATATRANSFER_H_

#include <stdint.h>
#include <sys/types.h>

#include <map>

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
 * This class provides a set of methods to abstract the reading and writing
 * of data. In this program, all the data is written/read using descriptors,
 * whether file descriptors or sockets. Trough this class the library sets if
 * the origin or destination of the data is a descriptor of socket or file. And
 * provides a couple of methods to get the descriptors from any place of the
 * code.
 *
 * For work in multicast, the server establishes a TCP connection with all the
 * receivers. For this reason, this class has the list of descriptors
 * this->_fdd, with its corresponding IP addresses.
 *
 * This class is singleton.
 * \date August, 2011
 */
class DataTransfer : public AbstractSubject {
public:
	~DataTransfer();
	static DataTransfer* getInstance();

	uint64_t transferAllFrom(int fd) throw(Exception);
	uint64_t transferFrom(int fd, uint64_t nBytes) throw(Exception);
	uint64_t transferFrom(const void *buff, uint64_t count) throw(Exception);

	uint64_t transferAllTo(int fd) throw(Exception);
	uint64_t transferTo(int fd, uint64_t nBytes) throw(Exception);
	uint64_t transferTo(void *buff, uint64_t count) throw(Exception);

	static ssize_t readBytes (int s, void *buf, size_t len) throw (Exception);
	static ssize_t recvData (int s, void *buf, size_t len) throw (Exception);
	static ssize_t writeBytes (int s, const void *buf, size_t len) throw (Exception);
	static ssize_t sendData (int s, const void *buf, size_t len) throw (Exception);
	static ssize_t sendMultiple (int s, const void *buf, size_t len) throw (Exception);

	void initFileRead();
	void initSocketRead();
	void initFileWrite();
	void initSocketWrite();
	void initSocketMultiple();

	void setFdo(int fdo);
	int getFdo() const;

	void setFdd(int fdd, std::string host);
	int getFdd() const;

	void setTotalSize(uint64_t size);

	uint64_t getTotalSize() const;
	uint64_t getTransferredBytes() const;

	void pushFdd(int fdd, std::string host);
	const std::map<int,std::string> &getFddList() const;

	void removeFdd(int fdd);

	/// Can be defined as read() or recv()
	Doclone::readFunction getNbytes;

	/// Can be defined as write() or send()
	Doclone::writeFunction putNbytes;
private:
	/// Private constructor is needed in Singleton pattern
	DataTransfer();

	/// The descriptor of origin
	int _fdo;
	/// The list of destination descriptors
	std::map<int,std::string> _fdd;

	/// Total size to transfer
	uint64_t _totalSize;
	/// Transferred bytes at the moment
	uint64_t _transferredBytes;
};

}

#endif /* DATATRANSFER_H_ */
