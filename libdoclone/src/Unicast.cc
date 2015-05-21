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

#include <doclone/Unicast.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <endian.h>

#include <doclone/Logger.h>
#include <doclone/PartedDevice.h>
#include <doclone/Clone.h>
#include <doclone/DataTransfer.h>
#include <doclone/Util.h>
#include <doclone/DiskLabel.h>
#include <doclone/DlFactory.h>
#include <doclone/Image.h>
#include <doclone/exception/Exception.h>
#include <doclone/exception/ConnectionException.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/WriteDataException.h>
#include <doclone/exception/ReceiveDataException.h>
#include <doclone/exception/SendDataException.h>
#include <doclone/exception/NoBlockDeviceException.h>
#include <doclone/exception/CreateImageException.h>
#include <doclone/exception/RestoreImageException.h>
#include <doclone/exception/CancelException.h>
#include <doclone/exception/CloseConnectionException.h>

namespace Doclone {

/**
 * \brief Default constructor.
 *
 * Initializes attributes.
 */
Unicast::Unicast(): _fds() {
	Clone *dcl = Clone::getInstance();

	unsigned int nodes = dcl->getNodesNumber();
	if(nodes == 0) {
		this->_nodesNum = 1;
	}
	else {
		this->_nodesNum = nodes;
	}
}

/**
 * \brief This function is called by the server and waits for a connection from
 * the receivers.
 *
 * This function communicates with the function "tcpClient" of the receivers.
 */
void Unicast::tcpServer() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::tcpServer() start");

	int sock_tcp;
	sockaddr_in host_server;
	socklen_t size = sizeof (sockaddr);

	host_server.sin_family = AF_INET;
	host_server.sin_port = htons (Doclone::PORT_DATA);
	host_server.sin_addr.s_addr = INADDR_ANY;

	if ((sock_tcp = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		ConnectionException ex;
		throw ex;
	}

	// Connect even in TIME_WAIT state
	int iSetOption = 1;
	setsockopt(sock_tcp, SOL_SOCKET, SO_REUSEADDR,
			&iSetOption, sizeof(iSetOption));

	if ((bind (sock_tcp,
			reinterpret_cast<sockaddr*>(&host_server), size)) < 0) {
		ConnectionException ex;
		throw ex;
	}

	if ((listen (sock_tcp, 1)) < 0) {
		ConnectionException ex;
		throw ex;
	}

	for(unsigned int i = 0;i<this->_nodesNum;i++) {
		while(1) {
			int fd;
			if ((fd =
				 accept (sock_tcp,
						 reinterpret_cast<sockaddr*>(&host_server),
						 &size)) < 0) {
				ConnectionException ex;
				ex.logMsg();
				throw ex;
			}

			dcCommand clnRequest = 0;
			DataTransfer::recvData(fd, &clnRequest, sizeof(clnRequest));

			if(clnRequest & Doclone::C_RECEIVER_OK) {
				dcCommand response = Doclone::C_SERVER_OK;
				DataTransfer::sendData(fd, &response, sizeof(response));

				this->_fds.push_back(fd);
				this->_srcIP = inet_ntoa (host_server.sin_addr);

				// Notify the views
				Clone *dcl = Clone::getInstance();
				dcl->triggerEvent(Doclone::EVT_NEW_CONNECION,
						inet_ntoa (host_server.sin_addr));

				break;
			}
		}
	}

	close(sock_tcp);

	log->debug("Unicast::tcpServer() end");
}

/**
 * \brief This function is called for the receivers and establishes a connection
 * with the server, who should be listening.
 *
 * This function communicates with the function "tcpServer" of the server.
 */
void Unicast::tcpClient() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::tcpClient() start");

	int fd;
	if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		ConnectionException ex;
		throw ex;
	}

	addrinfo *res;
	addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	std::string strPort = Util::intToString(Doclone::PORT_DATA);

	if(getaddrinfo (this->_srcIP.c_str(), strPort.c_str(), &hints, &res)) {
		ConnectionException ex;
		throw ex;
	}

	if ((connect (fd, res->ai_addr, res->ai_addrlen)) < 0) {
		ConnectionException ex;
		throw ex;
	}

	freeaddrinfo(res);

	dcCommand request = Doclone::C_RECEIVER_OK;
	DataTransfer::sendData(fd, &request, sizeof(request));

	dcCommand srvResponse = 0;
	DataTransfer::recvData(fd, &srvResponse, sizeof(srvResponse));

	if(!(srvResponse & Doclone::C_SERVER_OK)) {
		ConnectionException ex;
		throw ex;
	}

	this->_fds.push_back(fd);

	log->debug("Unicast::tcpClient() end");
}

/**
 * \brief Performs the sending of an image over network.
 *
 * \param image
 * 		The path for image file.
 */
void Unicast::sendFromImage() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::sendFromImage() start");

	Operation *waitOp = new Operation(
			Doclone::OP_WAIT_CLIENTS, "");

	Clone *dcl = Clone::getInstance();
	dcl->addOperation(waitOp);

	this->tcpServer();

	dcl->markCompleted(Doclone::OP_WAIT_CLIENTS, "");

	int fd = Util::openFile(this->_image);

	Operation *transferOp = new Operation(
			Doclone::OP_TRANSFER_DATA, "");

	dcl->addOperation(transferOp);

	/*
	 * Before sending the data, it sends its size. So the client/s can
	 * calculate the completed percentage. All the data is sent in big-endian.
	 * If the system is little-endian, it is necessary to convert totalSize to
	 * big-endian.
	 */
	Image dcImage;
	dcImage.initFdReadArchive(fd);
	dcImage.readImageHeader();
	uint64_t totalSize = dcImage.getHeader().image_size;
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(totalSize);

	uint64_t tmpTotalSize = htobe64(totalSize);
	DataTransfer::sendData(this->_fds, &tmpTotalSize,
			static_cast<size_t>(sizeof(uint64_t)));

	lseek(fd, 0, SEEK_SET);
	trns->copyData(fd, this->_fds);

	dcl->markCompleted(Doclone::OP_TRANSFER_DATA, "");

	Util::closeFile(fd);

	this->closeConnection();

	log->debug("Unicast::sendFromImage() end");
}

/**
 * \brief Performs the sending of a device over network.
 *
 * \param device
 * 		The path for device.
 */
void Unicast::sendFromDevice() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::sendFromDevice() start");

	PartedDevice *pedDev = PartedDevice::getInstance();
	pedDev->initialize(Util::getDiskPath(this->_device));

	Operation *waitOp = new Operation(
				Doclone::OP_WAIT_CLIENTS, "");

	Clone *dcl = Clone::getInstance();
	dcl->addOperation(waitOp);

	this->tcpServer();

	dcl->markCompleted(Doclone::OP_WAIT_CLIENTS, "");

	if(!Util::isBlockDevice(this->_device)) {
		NoBlockDeviceException ex;
		throw ex;
	}

	PartedDevice *pDevice = PartedDevice::getInstance();
	std::string target = pDevice->getPath();
	Disk *dcDisk = DlFactory::createDiskLabel();
	Image image;

	if(Util::isDisk(this->_device)) {
		image.setType(Doclone::IMAGE_DISK);
	}
	else {
		image.setType(Doclone::IMAGE_PARTITION);
	}

	Operation *readPartTableOp = new Operation(
			Doclone::OP_READ_PARTITION_TABLE, target);

	dcl->addOperation(readPartTableOp);

	image.readPartitionTable(this->_device);

	// Mark the operation to read partition table as completed
	dcl->markCompleted(Doclone::OP_READ_PARTITION_TABLE, target);

	if(image.canCreateCheck() == false) {
		CreateImageException ex;
		throw ex;
	}

	image.initCreateOperations();
	image.createImageHeader(dcDisk);
	image.initDiskReadArchive();
	image.initFdWriteArchive(this->_fds);

	/*
	 * Before sending the data, it sends its size. So the client/s can
	 * calculate the completed percentage. All the data is sent in big-endian.
	 * If the system is little-endian, it's necessary to convert totalSize to
	 * big-endian.
	 */
	uint64_t tmpTotalSize = htobe64(image.getHeader().image_size);
	DataTransfer::sendData(this->_fds, &tmpTotalSize,
			static_cast<size_t>(sizeof(uint64_t)));

	image.writeImageHeader();

	image.readPartitionsData();

	image.freeWriteArchive();
	image.freeReadArchive();

	this->closeConnection();

	delete dcDisk;

	log->debug("Unicast::sendFromDevice() end");
}

/**
 * \brief Performs the sending of an image or a device over network.
 */
void Unicast::send() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::send() start");

	Clone *dcl = Clone::getInstance();

	try {
		if(dcl->getDevice().empty()) {
			this->sendFromImage();
		} else {
			this->sendFromDevice();
		}
	} catch (const CancelException &ex) {
		this->closeConnection();
		throw;
	} catch (const ReadDataException &ex) {
		this->closeConnection();
		throw;
	} catch (const SendDataException &ex) {
		this->closeConnection();
		throw;
	} catch (const ErrorException &ex) {
		this->closeConnection();
		throw;
	}

	log->debug("Unicast::send() end");
}

/**
 * \brief Performs the reception of an image over network.
 *
 * \param image
 * 		The path for image.
 */
void Unicast::receiveToImage() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::receiveToImage() start");

	Operation *waitOp = new Operation(
			Doclone::OP_WAIT_SERVER, "");

	Clone *dcl = Clone::getInstance();
	dcl->addOperation(waitOp);

	this->tcpClient();

	dcl->markCompleted(Doclone::OP_WAIT_SERVER, "");

	Util::createFile(this->_image);
	int fd = Util::openFile(this->_image);

	Operation *transferOp = new Operation(
			Doclone::OP_TRANSFER_DATA, "");

	dcl->addOperation(transferOp);

	/*
	 * Get the size of the data, in order to calculate the completed percentage.
	 */
	uint64_t totalSize;
	DataTransfer::recvData(this->_fds[0], &totalSize,
			static_cast<size_t>(sizeof(uint64_t)));

	/*
	 * The given totalSize is received in big-endian. If the system is
	 * little-endian, it must be converted to little-endian.
	 */
	uint64_t tmpTotalSize = be64toh(totalSize);
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(tmpTotalSize);

	trns->copyData(this->_fds[0], fd);

	dcl->markCompleted(Doclone::OP_TRANSFER_DATA, "");

	Util::closeFile(fd);

	this->closeConnection();

	log->debug("Unicast::receiveToImage() end");
}

/**
 * \brief Performs the reception of a device over network.
 *
 * \param device
 * 		The path for device.
 */
void Unicast::receiveToDevice() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::receiveToDevice() start");

	PartedDevice *pedDev = PartedDevice::getInstance();
	pedDev->initialize(Util::getDiskPath(this->_device));

	Operation *waitOp = new Operation(
				Doclone::OP_WAIT_SERVER, "");

	Clone *dcl = Clone::getInstance();
	dcl->addOperation(waitOp);

	this->tcpClient();

	dcl->markCompleted(Doclone::OP_WAIT_SERVER, "");

	if(!Util::isBlockDevice(this->_device)) {
		NoBlockDeviceException ex;
		throw ex;
	}

	/*
	 * Get the size of the data, in order to calculate the completed percentage.
	 */
	uint64_t totalSize;
	DataTransfer::recvData(this->_fds[0], &totalSize,
			static_cast<size_t>(sizeof(uint64_t)));

	/*
	 * The given totalSize is received in big-endian. If the system is
	 * little-endian, it must be converted.
	 */
	uint64_t tmpTotalSize = be64toh(totalSize);
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(tmpTotalSize);

	Image image;
	image.initFdReadArchive(this->_fds[0]);
	image.initDiskWriteArchive();
	image.readImageHeader();
	image.openImageHeader();

	Disk *dcDisk = DlFactory::createDiskLabel(image.getLabelType(),
			pedDev->getPath());

	if(image.canRestoreCheck(this->_device, dcDisk->getSize()) == false) {
		RestoreImageException ex;
		throw ex;
	}

	image.initRestoreOperations(this->_device);
	image.writePartitionTable(this->_device);
	image.writePartitionsData(this->_device);

	if(image.getHeader().image_type==(Doclone::imageType)IMAGE_DISK) {
		dcDisk->setPartitions(image.getPartitions());
		dcDisk->restoreGrub();
	}

	image.freeWriteArchive();
	image.freeReadArchive();

	this->closeConnection();

	delete dcDisk;

	log->debug("Unicast::receiveToDevice() end");
}

/**
 * \brief Performs the reception of an image or a device over network.
 */
void Unicast::receive() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::receive() start");

	Clone *dcl = Clone::getInstance();

	this->_srcIP = dcl->getAddress();

	try {
		if(dcl->getDevice().empty()) {
			this->receiveToImage();
		} else {
			this->receiveToDevice();
		}
	} catch (const CancelException &ex) {
		this->closeConnection();
		throw;
	} catch (const WriteDataException &ex) {
		this->closeConnection();
		throw;
	} catch (const ReceiveDataException &ex) {
		this->closeConnection();
		throw;
	} catch (const ErrorException &ex) {
		this->closeConnection();
		throw;
	}

	log->debug("Unicast::receive() end");
}

/**
 * \brief Closes the opened connections
 */
void Unicast::closeConnection() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Unicast::closeConnection() start");

	if(this->_fds.size() > 0) {
		std::vector<int>::iterator it;
		for(it = this->_fds.begin(); it != this->_fds.end(); ++it) {
			if(close(*it)<0) {
				CloseConnectionException ex;
				ex.logMsg();
			}
		}
	}

	log->debug("Unicast::closeConnection() end");
}

}
