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

#include <doclone/Clone.h>

#include <locale.h>
#include <libintl.h>
#include <pthread.h>

#include <config.h>

#include <doclone/Logger.h>
#include <doclone/Local.h>
#include <doclone/DataTransfer.h>
#include <doclone/PartedDevice.h>
#include <doclone/Util.h>
#include <doclone/Unicast.h>
#include <doclone/Link.h>
#include <doclone/exception/Exception.h>
#include <doclone/exception/ErrorException.h>

namespace Doclone {

/**
 * \brief Initializes gettext, signal handlers and some attributes of this class
 */
Clone::Clone(): _image(), _device(), _address(), _nodesNumber(0),
		_empty(false), _force(), _operations() {
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);

	Util::signalCapture();

	this->_nodesNumber = 0;
	this->_empty = false;
}

/**
 * \ingroup CPPAPI
 * \brief Returns a pointer to a Doclone::Clone object
 *
 * \return A pointer to a doclone object
 */
Clone* Clone::getInstance() {
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mutex);

	static Clone instance;

	pthread_mutex_unlock(&mutex);

	return &instance;
}

/**
 * \ingroup CPPAPI
 * \brief Creates an image of a device.
 *
 * Both image an device path must be set before calling this function.
 */
void Clone::create() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::create() start");

	try {
		PartedDevice *pedDev = PartedDevice::getInstance();
		pedDev->initialize(Util::getDiskPath(this->_device));
		DataTransfer *trns = DataTransfer::getInstance();

		trns->initFileRead();
		trns->initFileWrite();

		Util::createFile(this->_image);
		int fd = Util::openFile(this->_image);

		trns->setFdd(fd, "");

		Local local(this->_image, this->_device);

		local.create();

		Util::closeFile(fd);
	} catch(const ErrorException &ex) {
		// Alert to view
		this->notifyObservers(Doclone::EVT_CANCEL_EXECUTION, "");

		throw;
	}

	// Notify to view
	this->notifyObservers(Doclone::EVT_FINISH_EXECUTION, "");

	log->debug("doclone::create() end");
}

/**
 * \ingroup CPPAPI
 * \brief Restores an image in a device.
 *
 * Both image an device path must be set before calling this function.
 */
void Clone::restore() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::restore() start");

	try {
		PartedDevice *pedDev = PartedDevice::getInstance();
		pedDev->initialize(Util::getDiskPath(this->_device));

		DataTransfer *trns = DataTransfer::getInstance();

		trns->initFileRead();
		trns->initFileWrite();

		int fd = Util::openFile(this->_image);

		trns->setFdo(fd);

		Local local(this->_image, this->_device);

		local.restore();

		Util::closeFile(fd);
	} catch(const ErrorException &ex) {
		// Alert to view
		this->notifyObservers(Doclone::EVT_CANCEL_EXECUTION, "");

		throw;
	}

	// Notify to view
	this->notifyObservers(Doclone::EVT_FINISH_EXECUTION, "");

	log->debug("doclone::restore() end");
}

/**
 * \ingroup CPPAPI
 * \brief Sends an image or a device to the network.
 *
 * The number of receivers and either image or device path must be set
 * before calling this function.
 */
void Clone::send() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::send() start");

	DataTransfer *trns = DataTransfer::getInstance();
	trns->initFileRead();

	if(this->_nodesNumber == 0
			|| this->_nodesNumber == 1) {
		trns->initSocketWrite();
	}
	else {
		trns->initSocketMultiple();
	}

	try {
		Unicast unicast;

		unicast.send();
	} catch(const ErrorException &ex) {
		// Alert to view
		this->notifyObservers(Doclone::EVT_CANCEL_EXECUTION, "");

		throw;
	}

	// Notify to view
	this->notifyObservers(Doclone::EVT_FINISH_EXECUTION, "");

	log->debug("doclone::send() end");
}

/**
 * \ingroup CPPAPI
 * \brief Receives an image or a device to the network.
 *
 * Server's IP and either image or device path must be set before calling this
 * function.
 */
void Clone::receive() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::receive() start");

	DataTransfer *trns = DataTransfer::getInstance();
	trns->initSocketRead();
	trns->initFileWrite();

	try {
		Unicast unicast;

		unicast.receive();
	} catch(const ErrorException &ex) {
		// Alert to view
		this->notifyObservers(Doclone::EVT_CANCEL_EXECUTION, "");

		throw;
	}

	// Notify to view
	this->notifyObservers(Doclone::EVT_FINISH_EXECUTION, "");

	log->debug("doclone::receive() end");
}

/**
 * \ingroup CPPAPI
 * \brief Sends an image or a device to the network in link mode.
 *
 * Image or device path must be set before calling this function.
 */
void Clone::chainOrigin() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::chainOrigin() start");

	DataTransfer *trns = DataTransfer::getInstance();
	trns->initFileRead();
	trns->initSocketWrite();

	try {
		Link lnk;

		lnk.send();
	} catch(const ErrorException &ex) {
		// Alert to view
		this->notifyObservers(Doclone::EVT_CANCEL_EXECUTION, "");

		throw;
	}

	// Notify to view
	this->notifyObservers(Doclone::EVT_FINISH_EXECUTION, "");

	log->debug("doclone::send() end");
}

/**
 * \ingroup CPPAPI
 * \brief Receives an image or a device to the network in link mode.
 *
 * Image or device path must be set before calling this function.
 */
void Clone::chainLink() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::chainLink() start");

	DataTransfer *trns = DataTransfer::getInstance();
	trns->initSocketRead();
	trns->initFileWrite();

	try {
		Link lnk;

		lnk.receive();
	} catch(const ErrorException &ex) {
		// Alert to view
		this->notifyObservers(Doclone::EVT_CANCEL_EXECUTION, "");

		throw;
	}

	// Notify to view
	this->notifyObservers(Doclone::EVT_FINISH_EXECUTION, "");

	log->debug("doclone::chainLink() end");
}

bool Clone::getEmpty() const {
	return this->_empty;
}

/**
 * \ingroup CPPAPI
 * \brief Sets the no-data mode on/off
 */
void Clone::setEmpty(bool empty) {
	this->_empty = empty;
}

unsigned int Clone::getNodesNumber() const {
	return this->_nodesNumber;
}

/**
 * \ingroup CPPAPI
 * \brief Sets the number of receivers in the network
 *
 * \param nodesNumber
 * 		Number of nodes
 */
void Clone::setNodesNumber(unsigned int nodesNumber) {
	this->_nodesNumber = nodesNumber;
}

const std::string &Clone::getDevice() const {
	return this->_device;
}

/**
 * \ingroup CPPAPI
 * \brief Sets the device on which work
 *
 * \param device
 * 		Device path
 */
void Clone::setDevice(const std::string &device) {
	this->_device = device;
}

const std::string &Clone::getImage() const {
	return this->_image;
}

/**
 * \ingroup CPPAPI
 * \brief Sets the image on which work
 *
 * \param image
 * 		Image path
 */
void Clone::setImage(const std::string &image) {
	this->_image = image;
}

const std::string &Clone::getAddress() const {
	return this->_address;
}

/**
 * \ingroup CPPAPI
 * \brief Sets the IP address of the server in unicast/multicast mode.
 *
 * \param address
 * 		IP Address
 */
void Clone::setAddress(const std::string &address) {
	this->_address = address;
}

bool Clone::getForce() const {
	return this->_force;
}

/**
 * \ingroup CPPAPI
 * \brief Sets the force mode on/off
 *
 * \param force
 * 		true = on; false = off
 */
void Clone::setForce(bool force) {
	this->_force = force;
}

/**
 * \brief Adds a pending operation to the vector
 *
 * \param op
 * 		Operation that will be added.
 */
void Clone::addOperation(Operation *op) {
	this->_operations.push_back(op);

	this->notifyObservers(Doclone::OPER_ADD, op->getType(), op->getTarget());
}

/**
 * \brief Marks one operation as completed.
 *
 * The parameters are necessary to identify the operation to be marked.
 *
 * \param type
 * 		The type of the operation.
 * \param target
 * 		The target of the operation.
 */
void Clone::markCompleted(dcOperationType type, const std::string &target) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::markCompleted(type=>%d, target=>%s) start", type, target.c_str());

	Operation *op = this->getOperation(type, target);

	if(op != 0) {
		op->setCompleted(true);

		this->notifyObservers(Doclone::OPER_MARK_COMPLETED, type, target);
	}

	log->debug("doclone::markCompleted() end");
}

/**
 * \brief Gets the operation required using its type and target to identify it
 *
 * \param type
 * 		The type of the operation.
 * \param target
 * 		The target of the operation.
 *
 * \return A pointer to Operation object.
 */
Operation* Clone::getOperation(dcOperationType type, const std::string &target) {
	Logger *log = Logger::getInstance();
	log->debug("doclone::getOperation(type=>%d, target=>%s) start", type, target.c_str());

	Operation *op = 0;

	std::vector<Operation *>::iterator it;
	for(it = this->_operations.begin();it != this->_operations.end();++it) {
		Operation* tmpOp=(*it);
		std::string opTarget = tmpOp->getTarget();
		dcOperationType opType = tmpOp->getType();

		if(opType == type && target.compare(opTarget) == 0) {
			op = tmpOp;
			break;
		}
	}

	log->debug("doclone::getOperation() start");

	return op;
}

/**
 * \brief Deletes all the objects in the vector of operations and clears it.
 */
Clone::~Clone() {
	for(std::vector<Operation*>::iterator it = this->_operations.begin();
			it != this->_operations.end();++it) {
		if(*it != 0) {
			delete *it;
			*it = 0;
		}
	}

	this->_operations.clear();
}

/**
 * \brief Used by the library to trigger a general event
 */
void Clone::triggerEvent(dcEvent event, const std::string &target) {
	this->notifyObservers(event, target);
}

}
