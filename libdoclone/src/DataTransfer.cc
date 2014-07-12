/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013, 2014 Joan Lledó <joanlluislledo@gmail.com>
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

namespace Doclone {

/**
 * \brief Initializes the attributes
 */
DataTransfer::DataTransfer()
	:  getNbytes(0), putNbytes(0), _fdo(0), _fdd(), _totalSize(0),
	   _transferredBytes(0) {
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

/**
 * \brief Transfers all the data from the descriptor passed as parameter to
 * this->_fdd
 *
 * \param fd
 * 		Origin file Descriptor
 *
 * \return Number of bytes sent
 */
uint64_t DataTransfer::transferAllFrom(int fd) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferAllFrom(fd=>%d) start", fd);

	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = Doclone::BUFFER_SIZE;

	while ((nbytes = (*this->getNbytes) (fd, buf, Doclone::BUFFER_SIZE)) > 0) {
		(*this->putNbytes) ((*this->_fdd.begin()).first, buf, nbytes);

		this->_transferredBytes += nbytes;

		// Every certain amount of Mbytes, notify the views
		if((this->_transferredBytes%(Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT)) == 0) {
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::transferAllFrom(_transferredBytes=>%d) end", this->_transferredBytes);
	return this->_transferredBytes;
}

/**
 * \brief Transfers [count] bytes of data from the descriptor passed as
 * paramater to this->_fdd
 *
 * \param fd
 * 		Origin file descriptor
 * \param count
 * 		Number of bytes to send
 * \return Number of bytes sent
 */
uint64_t DataTransfer::transferFrom(int fd, uint64_t count) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferFrom(fd=>%d, count=>%d) start", fd, count);

	char buf[Doclone::BUFFER_SIZE];
	uint64_t nbytes = 0;
	uint64_t written_bytes = 0;

	while (written_bytes < count) {
		nbytes = Doclone::BUFFER_SIZE;

		if(written_bytes+nbytes>count) {
			nbytes = count-written_bytes;
		}

		nbytes = (*this->getNbytes) (fd, buf, nbytes);

		(*this->putNbytes) ((*this->_fdd.begin()).first, buf, nbytes);

		written_bytes += nbytes;
	}

	this->_transferredBytes += written_bytes;

	// Every certain amount of Mbytes, notify the views
	this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
			this->_transferredBytes);

	log->loopDebug("DataTransfer::transferFrom(written_bytes=>%d) end", written_bytes);
	return written_bytes;
}

/**
 * \brief Transfers [count] bytes of data from the pointer passed as parameter
 * to this->_fdd
 *
 * \param buff
 * 		Origin pointer
 *
 * \return Number of bytes sent
 */
uint64_t DataTransfer::transferFrom(const void *buff, uint64_t count) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferAllFrom(buff=>0x%x, count=>%d) start", buff, count);

	uint64_t written_bytes = 0;

	written_bytes = (*this->putNbytes) ((*this->_fdd.begin()).first, buff, count);

	log->loopDebug("DataTransfer::transferAllFrom(_transferredBytes=>%d) end", written_bytes);
	return written_bytes;
}

/**
 * \brief Transfers all the data from this->_fdo to the file descriptor passed
 * as parameter.
 *
 * If this->_fdd is not null, sends a copy of data through it (for link mode).
 *
 * \param fd
 * 		Descriptor of the destiny
 * \return Number of bytes sent
 */
uint64_t DataTransfer::transferAllTo(int fd) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferAllTo(fd=>%d) start", fd);

	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = Doclone::BUFFER_SIZE;

	while ((nbytes = (*this->getNbytes) (this->_fdo, buf, Doclone::BUFFER_SIZE)) > 0) {
		// If we are in link mode and there are links behind us
		if (!this->_fdd.empty()) {
			if ((send ((*this->_fdd.begin()).first, buf, nbytes, 0)) < 0) {
				SendDataException ex((*this->_fdd.begin()).second);
				throw ex;
			}
		}

		nbytes = (*this->putNbytes) (fd, buf, nbytes);
		this->_transferredBytes += nbytes;

		// Every certain amount of Mbytes, notify the views
		if((this->_transferredBytes%(Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT)) == 0) {
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::transferAllTo(_transferredBytes=>%d) end", this->_transferredBytes);
	return this->_transferredBytes;
}

/**
 * \brief Transfers [count] bytes of data from this->_fdo to the file descriptor
 * passed as parameter.
 *
 * If this->_fdd is not null, sends a copy of data through it (for link mode).
 *
 * \param fd
 * 		Descriptor of the destiny
 * \param count
 * 		Number of bytes that will be sent
 * \return Number of bytes sent
 */
uint64_t DataTransfer::transferTo(int fd, uint64_t count) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferTo(fd=>%d, count=>%d) start", fd, count);

	char buf[Doclone::BUFFER_SIZE];
	uint64_t rBytes = 0;
	uint64_t written_bytes = 0;

	try{
		while (written_bytes < count) {
			uint64_t nbytes = Doclone::BUFFER_SIZE;

			if(written_bytes+nbytes>count) {
				nbytes = count-written_bytes;
			}
			rBytes = (*this->getNbytes) (this->_fdo, buf, nbytes);

			// If we are in link mode and there are links behind us
			if (!this->_fdd.empty()) {
				if ((send ((*this->_fdd.begin()).first, buf, rBytes, 0)) < 0) {
					SendDataException ex((*this->_fdd.begin()).second);
					throw ex;
				}
			}

			uint64_t wBytes = (*this->putNbytes) (fd, buf, rBytes);

			if(wBytes < rBytes) {
				// No space left in disk.
				WriteDataException ex;
				throw ex;
			}

			written_bytes += wBytes;
		}

		this->_transferredBytes += written_bytes;
	} catch (WarningException &ex) {
		/*
		 * The data has been read but not written, we must set _transferredBytes
		 * properly to continue with the job.
		 *
		 * this->_transferredBytes is used to calculate the offset at where the
		 * error has happen. It's very important in order to pass this file by.
		 */
		this->_transferredBytes += written_bytes + rBytes;
		throw;
	}

	// Every certain amount of Mbytes, notify the views
	this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
				this->_transferredBytes);

	log->loopDebug("DataTransfer::transferTo(written_bytes=>%d) end", written_bytes);
	return written_bytes;
}

/**
 * \brief Transfers [count] bytes of data from this->_fdo to the given pointer.
 *
 * If this->_fdd is not null, sends a copy of data over it (for link mode).
 *
 * \param fd
 * 		Descriptor of the destiny
 * \param count
 * 		Number of bytes that will be sent
 * \return Number of bytes sent
 */
uint64_t DataTransfer::transferTo(void *buff, uint64_t count) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferTo(buff=>0x%x, count=>%d) start", buff, count);

	uint64_t read_bytes = 0;

	read_bytes = (*this->getNbytes) (this->_fdo, buff, count);

	// If we are in link mode and there are links behind us
	if (!this->_fdd.empty()) {
		if ((send ((*this->_fdd.begin()).first, buff, read_bytes, 0)) < 0) {
			SendDataException ex((*this->_fdd.begin()).second);
			throw ex;
		}
	}

	this->_transferredBytes += read_bytes;

	log->loopDebug("DataTransfer::transferTo(read_bytes=>%d) end", read_bytes);
	return read_bytes;
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
		DataTransfer *trns = DataTransfer::getInstance();

		std::map<int,std::string> fdd(trns->getFddList());
		SendDataException ex(fdd[s]);
		throw ex;
	}

	return nbytes;
}

/**
 * \brief Writes data over network, from one origin to multiple destinations.
 *
 * Transfers [len] bytes of data from [buf] to all descriptors in
 * DataTransfer::getFddList(). The parameter [s] is not used, it's here because
 * this function will be called using the pointer to function 'putNbytes'
 * that accepts that parameter. So in this function, this parameter is ignored.
 *
 * \param s
 * 		Not used, preserved by compatibility
 * \param buf
 * 		Buffer of data
 * \param len
 * 		Number of bytes to write
 *
 * \return Number of bytes written
 */
ssize_t DataTransfer::sendMultiple(int s, const void *buf, size_t len) throw (Exception) {
	ssize_t nbytes = 0;
	DataTransfer *trns = DataTransfer::getInstance();

	std::map<int,std::string> fdds(trns->getFddList());
	std::map<int,std::string>::iterator it;
	for(it = fdds.begin(); it != fdds.end(); ++it) {
		/*
		 * MSG_NOSIGNAL is here to prevent the sending of SIGPIPE signal,
		 * which would be captured, interrupting the program.
		 */
		nbytes = send((*it).first, buf, len, MSG_NOSIGNAL);

		if(nbytes<0) {
			// If the sending fails, just remove this descriptor from the list.
			DataTransfer *trns = DataTransfer::getInstance();
			trns->removeFdd((*it).first);

			SendDataException ex((*it).second);
			ex.logMsg();

			// If all receivers fail
			if(trns->getFddList().size() == 0) {
				BrokenPipeException ex;
				ex.logMsg();
				throw ex;
			}
		}
	}

	return nbytes;
}

/**
 * \brief Sets getNbytes to execute readBytes
 */
void DataTransfer::initFileRead() {
	Logger *log = Logger::getInstance();
	log->debug("DataTransfer::initFileRead() start");

	this->getNbytes = DataTransfer::readBytes;

	log->debug("DataTransfer::initFileRead() end");
}

/**
 * \brief Sets getNbytes to execute recvData
 */
void DataTransfer::initSocketRead() {
	Logger *log = Logger::getInstance();
	log->debug("DataTransfer::initSocketRead() start");

	this->getNbytes = DataTransfer::recvData;

	log->debug("DataTransfer::initSocketRead() end");
}

/**
 * \brief Sets putNbytes to execute writeBytes
 */
void DataTransfer::initFileWrite() {
	Logger *log = Logger::getInstance();
	log->debug("DataTransfer::initFileWrite() start");

	this->putNbytes = DataTransfer::writeBytes;

	log->debug("DataTransfer::initFileWrite() end");
}

/**
 * \brief Sets getNbytes to execute sendData
 */
void DataTransfer::initSocketWrite() {
	Logger *log = Logger::getInstance();
	log->debug("DataTransfer::initSocketWrite() start");

	this->putNbytes = DataTransfer::sendData;

	log->debug("DataTransfer::initSocketWrite() end");
}

/**
 * \brief Sets getNbytes to execute sendMultiple
 */
void DataTransfer::initSocketMultiple() {
	Logger *log = Logger::getInstance();
	log->debug("DataTransfer::initSocketMultiple() start");

	this->putNbytes = DataTransfer::sendMultiple;

	log->debug("DataTransfer::initSocketMultiple() end");
}

void DataTransfer::setFdo(int fdo) {
	this->_fdo = fdo;
}

int DataTransfer::getFdo() const {
	return this->_fdo;
}

/**
 * \brief Clears the list of receivers and inserts the passed descriptor
 *
 * \param fdd
 * 		The descriptor that will be the only one in the list of receivers
 */
void DataTransfer::setFdd(int fdd, std::string host) {
	this->_fdd.clear();
	this->_fdd.insert(std::pair<int,std::string>(fdd,host));
}

/**
 * \brief Gets the first descriptor in the list of receivers
 *
 * \return A descriptor
 */
int DataTransfer::getFdd() const {
	return (*this->_fdd.begin()).first;
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

/**
 * \brief Adds a descriptor at the end of the list of receivers
 *
 * \param fdd
 * 		A new receiver
 */
void DataTransfer::pushFdd(int fdd, std::string host) {
	this->_fdd.insert(std::pair<int,std::string>(fdd,host));
}

const std::map<int,std::string> &DataTransfer::getFddList() const {
	return this->_fdd;
}

/**
 * \brief Removes a receivers from the list
 *
 * 	If one receiver fails, calling this function will remove it from the
 * 	list, so the server can continue sending to the other receivers.
 *
 * \param fdd
 * 		The receiver to be removed
 */
void DataTransfer::removeFdd(int fdd) {
	Logger *log = Logger::getInstance();
	log->debug("DataTransfer::initSocketWrite(%d) start", fdd);

	this->_fdd.erase(fdd);

	log->debug("DataTransfer::initSocketWrite() start");
}

}

