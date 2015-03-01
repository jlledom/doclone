/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013-2015 Joan Lledó <joanlluislledo@gmail.com>
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

#include <doclone/DataTransfer.h>

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include <doclone/Logger.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/WriteDataException.h>
#include <doclone/exception/ReceiveDataException.h>
#include <doclone/exception/SendDataException.h>
#include <doclone/exception/BrokenPipeException.h>
#include <doclone/exception/InitializationException.h>

namespace Doclone {

/**
 * \brief Initializes the attributes
 */
DataTransfer::DataTransfer()
	:  getNbytes(0), putNbytes(0), _totalSize(0), _transferredBytes(0) {
}

/**
 * \brief Clear this->_fdd map
 */
DataTransfer::~DataTransfer() {
	//this->_fdd.clear();
}

/**
 * \brief Singleton stuff
 *
 * \return Pointer to a DataTransfer object
 */
DataTransfer* DataTransfer::getInstance() {
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mutex);

	static DataTransfer instance;

	pthread_mutex_unlock(&mutex);

	return &instance;
}

uint64_t DataTransfer::archiveToBuf(struct archive *arIn, std::string &target) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::archiveToBuf(arIn=>0x%x, buff=>%s) start", arIn, target.c_str());

	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = Doclone::BUFFER_SIZE;
	target = "";

	while ((nbytes = archive_read_data(arIn, buf, Doclone::BUFFER_SIZE)) > 0) {
		target.append(buf);

		this->_transferredBytes += nbytes;

		// Every certain amount of Mbytes, notify the views
		if((this->_transferredBytes%(Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT)) == 0) {
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	// If the transfer stopped due to an error
	if(nbytes < 0) {
		ReadDataException ex;
		throw ex;
	}

	log->loopDebug("DataTransfer::archiveToBuf(_transferredBytes=>%d) end", this->_transferredBytes);
	return this->_transferredBytes;
}

uint64_t DataTransfer::bufToArchive(const std::string &source, struct archive *arOut) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::bufToArchive(source=>%s, arOut=>0x%x) start", source.c_str(), arOut);

	archive_write_data(arOut, source.c_str(), source.length());

	this->_transferredBytes += source.length();

	// Every certain amount of Mbytes, notify the views
	if((this->_transferredBytes%(Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT)) == 0) {
		this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
				this->_transferredBytes);
	}

	log->loopDebug("DataTransfer::bufToArchive(_transferredBytes=>%d) end", this->_transferredBytes);
	return this->_transferredBytes;
}

uint64_t DataTransfer::fdToArchive(int fd, struct archive *arOut) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferFile(fd=>%d, arOut=>0x%x) start", fd, arOut);

	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = 0;

	while ((nbytes = (*this->getNbytes) (fd, buf, Doclone::BUFFER_SIZE)) > 0) {
		archive_write_data(arOut, buf, nbytes);

		this->_transferredBytes += nbytes;

		// Every certain amount of Mbytes, notify the views
		if((this->_transferredBytes%(Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT)) == 0) {
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::fdToArchive(_transferredBytes=>%d) end", this->_transferredBytes);
	return this->_transferredBytes;
}

/**
 * \brief Transfers all the data from fdin to fdout.
 *
 * \param fdin
 * 		Origin descriptor
 * \param fdout
 * 		Destinarion descriptor
 *
 * \return Number of bytes sent
 */
uint64_t DataTransfer::copyData(struct archive *arIn, struct archive *arOut) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::copyData(arIn=>0x%x, arOut=>0x%x) start", arIn, arOut);

	int r;
	const void *buff;
	size_t size;
	off_t offset;

	while ((r = archive_read_data_block(arIn, &buff, &size, &offset)) != ARCHIVE_EOF) {
		if (r < ARCHIVE_OK) {
			ReadDataException ex;
			throw ex;
		}

		r = archive_write_data_block(arOut, buff, size, offset);
		if (r < ARCHIVE_OK) {
			WriteDataException ex;
			throw ex;
		}

		this->_transferredBytes += size;

		// Every certain amount of Mbytes, notify the views
		if((this->_transferredBytes%(Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT)) == 0) {
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::copyData(_transferredBytes=>%d) end", this->_transferredBytes);
	return this->_transferredBytes;
}

/**
 * \brief Transfers all the data from fdin to fdout.
 *
 * \param fdin
 * 		Origin descriptor
 * \param fdout
 * 		Destinarion descriptor
 *
 * \return Number of bytes sent
 */
uint64_t DataTransfer::copyData(int fdin, int fdout) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::copyData(fdin=>%d, fdout=>%d) start", fdin, fdout);

	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = Doclone::BUFFER_SIZE;

	while ((nbytes = (*this->getNbytes) (fdin, buf, Doclone::BUFFER_SIZE)) > 0) {
		(*this->putNbytes) (fdout, buf, nbytes);

		this->_transferredBytes += nbytes;

		// Every certain amount of Mbytes, notify the views
		if((this->_transferredBytes%(Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT)) == 0) {
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::copyData(_transferredBytes=>%d) end", this->_transferredBytes);
	return this->_transferredBytes;
}

void DataTransfer::initLocalRead() {
	this->getNbytes = DataTransfer::readBytes;
}

void DataTransfer::initSocketRead() {
	this->getNbytes = DataTransfer::recvData;
}

void DataTransfer::initLocalWrite() {
	this->putNbytes = DataTransfer::writeBytes;
}

void DataTransfer::initSocketWrite() {
	this->putNbytes = DataTransfer::sendData;
}

/**
 * \brief Reads data locally.
 *
 * Reads [len] bytes of data from [s] and writes them in [buf]
 *
 * \param s
 * 		Origin descriptor
 * \param [out] buf
 * 		Buffer of data
 * \param len
 * 		Number of bytes to read
 *
 * \return Number of bytes read
 */
ssize_t DataTransfer::readBytes (int s, void *buf, size_t len) throw (Exception) {
	ssize_t nbytes = read(s, buf, len);

	if(nbytes<0) {
		ReadDataException ex;
		throw ex;
	}

	return nbytes;
}

/**
 * \brief Reads data over the network.
 *
 * Reads [len] bytes of data from [s] and writes them in [buf]
 *
 * \param s
 * 		Origin descriptor
 * \param [out] buf
 * 		Buffer of data
 * \param len
 * 		Number of bytes to receive
 *
 * \return Number of bytes received
 */
ssize_t DataTransfer::recvData (int s, void *buf, size_t len) throw (Exception) {
	ssize_t nbytes = recv(s, buf, len, MSG_WAITALL);

	if(nbytes<0) {
		ReceiveDataException ex;
		throw ex;
	}

	return nbytes;
}

/**
 * \brief Writes data locally.
 *
 * Transfers [len] bytes of data from [buf] to [s]
 *
 * \param s
 * 		Destination descriptor
 * \param buf
 * 		Buffer of data
 * \param len
 * 		Number of bytes to write
 *
 * \return Number of bytes written
 */
ssize_t DataTransfer::writeBytes (int s, const void *buf, size_t len) throw (Exception) {
	ssize_t nbytes = write(s, buf, len);

	if(nbytes<0) {
		WriteDataException ex;
		throw ex;
	}

	return nbytes;
}

/**
 * \brief Sends data over the network.
 *
 * Transfers [len] bytes of data from [buf] to [s]
 *
 * \param s
 * 		Destination descriptor
 * \param buf
 * 		Buffer of data
 * \param len
 * 		Number of bytes to send
 *
 * \return Number of bytes sent
 */
ssize_t DataTransfer::sendData (int s, const void *buf, size_t len) throw (Exception) {
	ssize_t nbytes = send(s, buf, len, 0);

	if(nbytes<0) {
		//FIXME: Pass the human-readable IP address
		SendDataException ex("TODO");
		throw ex;
	}

	return nbytes;
}

/**
 * \brief This setter calls notify()
 */
void DataTransfer::setTotalSize(uint64_t size) {
	this->_totalSize = size;

	this->notifyObservers(Doclone::TRANS_TOTAL_SIZE, this->_totalSize);
}

uint64_t DataTransfer::getTotalSize() const {
	return this->_totalSize;
}

uint64_t DataTransfer::getTransferredBytes() const {
	return this->_transferredBytes;
}

}

