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
#include <netinet/in.h>
#include <arpa/inet.h>
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
	:  getNbytes(0), putNbytes(0), _totalSize(0), _transferredBytes(0),
	   _transferNotificationsCount(0) {
	this->_notificationPointSize = Doclone::BUFFER_SIZE*Doclone::UPDATE_QUOTIENT;
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
 * \brief Reads data from the archive and appends it in the target
 *
 * Only for text buffers
 *
 * \param arIn
 * 		Pointer to the archive file
 * \param target
 * 		String where data will be written
 *
 * \return Number of transferred bytes
 */
uint64_t DataTransfer::archiveToBuf(struct archive *arIn, std::string &target) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::archiveToBuf(arIn=>0x%x, buff=>%s) start", arIn, target.c_str());

	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = Doclone::BUFFER_SIZE;
	unsigned int totalNbytes = 0;
	target = "";

	while ((nbytes = archive_read_data(arIn, buf, Doclone::BUFFER_SIZE)) > 0) {
		target.append(buf);

		this->_transferredBytes += nbytes;
		totalNbytes += nbytes;

		// Notify the views if it crosses a notification point
		if(this->_transferredBytes >
			(this->_notificationPointSize * this->_transferNotificationsCount)) {
			this->_transferNotificationsCount++;
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	// If the transfer stopped due to an error
	if(nbytes < 0) {
		ReadDataException ex;
		throw ex;
	}

	log->loopDebug("DataTransfer::archiveToBuf(totalNbytes=>%d) end", totalNbytes);
	return totalNbytes;
}

/**
 * \brief Reads data from the source and writes it in the archives
 *
 * Only for text buffers
 *
 * \param source
 * 		Text to be written
 * \param outArchives
 * 		Vector of archives where data will be written
 *
 * \return Number of transferred bytes
 */
uint64_t DataTransfer::bufToArchive(const std::string &source,
		std::vector<struct archive*> &outArchives) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::bufToArchive(source=>%s, outArchives=>0x%x) start", source.c_str(), &outArchives);

	int r;
	unsigned int totalNbytes = 0;

	std::vector<struct archive*>::iterator it;
	for(it = outArchives.begin(); it != outArchives.end(); ++it) {
		r = archive_write_data(*it, source.c_str(), source.length());
		if (r < ARCHIVE_OK) {
			WriteDataException ex;
			throw ex;
		}
	}

	this->_transferredBytes += source.length();
	totalNbytes += source.length();

	// Notify the views if it crosses a notification point
	if(this->_transferredBytes >
		(this->_notificationPointSize * this->_transferNotificationsCount)) {
		this->_transferNotificationsCount++;
		this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
				this->_transferredBytes);
	}

	log->loopDebug("DataTransfer::bufToArchive(totalNbytes=>%d) end", totalNbytes);
	return totalNbytes;
}

/**
 * \brief Reads data from the file descriptor and writes it in the archives
 *
 * Only for text buffers
 *
 * \param fd
 * 		Source file descriptor
 * \param outArchives
 * 		Vector of archives where data will be written
 *
 * \return Number of transferred bytes
 */
uint64_t DataTransfer::fdToArchive(int fd, std::vector<struct archive*> &outArchives) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::transferFile(fd=>%d, outArchives=>0x%x) start", fd, &outArchives);

	int r;
	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = 0;
	unsigned int totalNbytes = 0;

	while ((nbytes = (*this->getNbytes) (fd, buf, Doclone::BUFFER_SIZE)) > 0) {
		std::vector<struct archive*>::iterator it;
		for(it = outArchives.begin(); it != outArchives.end(); ++it) {
			r = archive_write_data(*it, buf, nbytes);
			if (r < ARCHIVE_OK) {
				WriteDataException ex;
				throw ex;
			}
		}

		this->_transferredBytes += nbytes;
		totalNbytes += nbytes;

		// Notify the views if it crosses a notification point
		if(this->_transferredBytes >
			(this->_notificationPointSize * this->_transferNotificationsCount)) {
			this->_transferNotificationsCount++;
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::fdToArchive(totalNbytes=>%d) end", totalNbytes);
	return totalNbytes;
}

/**
 * \brief Writes the libarchive entry of a file in many archives
 *
 * \param entry
 * 		The libarchive entry
 * \param outArchives
 * 		Vector of archives where data will be written
 */
void DataTransfer::copyHeader(struct archive_entry *entry,
		std::vector<struct archive*> &outArchives) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::copyHeader(entry=>0x%x) start", entry);

	int r;

	std::vector<struct archive*>::iterator it;
	for(it = outArchives.begin(); it != outArchives.end(); ++it) {
		r = archive_write_header(*it, entry);
		if (r < ARCHIVE_OK) {
			WriteDataException ex;
			throw ex;
		}
	}

	log->loopDebug("DataTransfer::fdToArchive() end");
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
uint64_t DataTransfer::copyData(struct archive *arIn, std::vector<struct archive *> &outArchives) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::copyData(arIn=>0x%x, outArchives=>0x%x) start", arIn, &outArchives);

	int r;
	const void *buff;
	size_t size;
	off_t offset;
	unsigned int totalNbytes = 0;

	while ((r = archive_read_data_block(arIn, &buff, &size, &offset)) != ARCHIVE_EOF) {
		if (r < ARCHIVE_OK) {
			ReadDataException ex;
			throw ex;
		}

		std::vector<struct archive*>::iterator it;
		for(it = outArchives.begin(); it != outArchives.end(); ++it) {
			r = archive_write_data_block(*it, buff, size, offset);
			if (r < ARCHIVE_OK) {
				WriteDataException ex;
				throw ex;
			}
		}

		this->_transferredBytes += size;
		totalNbytes += size;

		// Notify the views if it crosses a notification point
		if(this->_transferredBytes >
			(this->_notificationPointSize * this->_transferNotificationsCount)) {
			this->_transferNotificationsCount++;
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::copyData(totalNbytes=>%d) end", totalNbytes);
	return totalNbytes;
}

/**
 * \brief Transfers all the data from fdin to all out file descriptors.
 *
 * \param fdin
 * 		Origin descriptor
 * \param outFds
 * 		Vector of destination descriptors
 *
 * \return Number of bytes sent
 */
uint64_t DataTransfer::copyData(int fdin, std::vector<int> &outFds) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::copyData(fdin=>%d, outFds=>0x%x) start", fdin, &outFds);

	char buf[Doclone::BUFFER_SIZE];
	unsigned int nbytes = Doclone::BUFFER_SIZE;
	unsigned int totalNbytes = 0;

	while ((nbytes = (*this->getNbytes) (fdin, buf, Doclone::BUFFER_SIZE)) > 0) {
		std::vector<int>::iterator it;
		for(it = outFds.begin(); it != outFds.end(); ++it) {
			(*this->putNbytes) (*it, buf, nbytes);
		}

		this->_transferredBytes += nbytes;
		totalNbytes += nbytes;

		// Notify the views if it crosses a notification point
		if(this->_transferredBytes >
			(this->_notificationPointSize * this->_transferNotificationsCount)) {
			this->_transferNotificationsCount++;
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::copyData(totalNbytes=>%d) end", totalNbytes);
	return totalNbytes;
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
	unsigned int totalNbytes = 0;

	while ((nbytes = (*this->getNbytes) (fdin, buf, Doclone::BUFFER_SIZE)) > 0) {
		(*this->putNbytes) (fdout, buf, nbytes);

		this->_transferredBytes += nbytes;
		totalNbytes += nbytes;

		// Notify the views if it crosses a notification point
		if(this->_transferredBytes >
			(this->_notificationPointSize * this->_transferNotificationsCount)) {
			this->_transferNotificationsCount++;
			this->notifyObservers(Doclone::TRANS_TRANSFERRED_BYTES,
					this->_transferredBytes);
		}
	}

	log->loopDebug("DataTransfer::copyData(totalNbytes=>%d) end", totalNbytes);
	return totalNbytes;
}

/**
 * \brief Makes getNbytes point to local reading function
 */
void DataTransfer::initLocalRead() {
	this->getNbytes = DataTransfer::readBytes;
}

/**
 * \brief Makes getNbytes point to net receiving function
 */
void DataTransfer::initSocketRead() {
	this->getNbytes = DataTransfer::recvData;
}

/**
 * \brief Makes getNbytes point to local writing function
 */
void DataTransfer::initLocalWrite() {
	this->putNbytes = DataTransfer::writeBytes;
}

/**
 * \brief Makes getNbytes point to net sending function
 */
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
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::readBytes(s=>%d, buf=>0x%x, len=>%d) start", s, buf, len);

	ssize_t nbytes = read(s, buf, len);

	if(nbytes<0) {
		ReadDataException ex;
		throw ex;
	}

	log->loopDebug("DataTransfer::readBytes(nbytes=>%d) end", nbytes);
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
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::recvData(s=>%d, buf=>0x%x, len=>%d) start", s, buf, len);

	ssize_t nbytes = recv(s, buf, len, MSG_WAITALL);

	if(nbytes<0) {
		ReceiveDataException ex;
		throw ex;
	}

	log->loopDebug("DataTransfer::recvData(nbytes=>%d) end", nbytes);
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
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::writeBytes(s=>%d, buf=>0x%x, len=>%d) start", s, buf, len);

	ssize_t nbytes = write(s, buf, len);

	if(nbytes<0) {
		WriteDataException ex;
		throw ex;
	}

	log->loopDebug("DataTransfer::writeBytes(nbytes=>%d) end", nbytes);
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
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::sendData(s=>%d, buf=>0x%x, len=>%d) start", s, buf, len);

	ssize_t nbytes = send(s, buf, len, 0);

	if(nbytes<0) {
		struct sockaddr_in addr;
		socklen_t addr_size = sizeof(struct sockaddr_in);
		getsockname(s, (struct sockaddr *)&addr, &addr_size);
		SendDataException ex(inet_ntoa(addr.sin_addr));
		throw ex;
	}

	log->loopDebug("DataTransfer::sendData(nbytes=>%d) end", nbytes);
	return nbytes;
}

/**
 * \brief Sends data over the network.
 *
 * Transfers [len] bytes of data from [buf] to each element in [fds]
 *
 * \param s
 * 		Vector of destination descriptors
 * \param buf
 * 		Buffer of data
 * \param len
 * 		Number of bytes to send
 *
 * \return Number of bytes sent
 */
ssize_t DataTransfer::sendData (std::vector<int> &fds, const void *buf,
		size_t len) throw (Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("DataTransfer::sendData(fds=>0x%x, buf=>0x%x, len=>%d) start", &fds, buf, len);

	ssize_t nbytes = 0;
	std::vector<int>::iterator it;
	for(it = fds.begin(); it != fds.end(); ++it) {
		nbytes = send(*it, buf, len, 0);

		if(nbytes<0) {
			struct sockaddr_in addr;
			socklen_t addr_size = sizeof(struct sockaddr_in);
			getsockname(*it, (struct sockaddr *)&addr, &addr_size);
			SendDataException ex(inet_ntoa(addr.sin_addr));
			throw ex;
		}
	}

	log->loopDebug("DataTransfer::sendData(nbytes=>%d) end", nbytes);
	return nbytes;
}

/**
 * \brief This setter calls notify()
 */
void DataTransfer::setTotalSize(uint64_t size) {
	this->_totalSize = size;

	this->notifyObservers(Doclone::TRANS_TOTAL_SIZE, this->_totalSize);
}

}

