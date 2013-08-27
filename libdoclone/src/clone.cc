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

#include <string.h>

#include <doclone/Clone.h>
#include <doclone/clone.h>

/**
 * \ingroup CWrapperAPI
 * \brief For compatibility with AC_CHECK_LIB
 *
 * In configure.ac, the user of the library can check if libdoclone is installed
 * by either calling the PKG_CHECK_MODULES macro or calling AC_CHECK_LIB macro
 * by this way:
 * 		AC_CHECK_LIB([doclone], [libdoclone_is_present],
 * 			[action-if-found], [action-if-not-found])
 */
int libdoclone_is_present(void) {
	return 1;
}

/**
 * \ingroup CWrapperAPI
 * \brief Creates a new dc_doclone object in the heap and returns its address
 *
 * \return Pointer to the newly dc_doclone object
 */
dc_doclone *doclone_new() {
	dc_doclone *dcObj = new dc_doclone();
	dcObj->_observer = new DefaultObserver();

	return dcObj;
}

/**
 * \ingroup CWrapperAPI
 * \brief Destroy the given dc_doclone object
 *
 * If this function is not called, the object will never be freed
 */
void doclone_destroy(dc_doclone *dc_obj) {
	delete reinterpret_cast<DefaultObserver *>(dc_obj->_observer);
	delete dc_obj;
}

/**
 * \ingroup CWrapperAPI
 * \brief Creates an image of a device.
 *
 * Both image and device path must be set before calling this function.
 *
 * \return 0 if the process has success, -1 if any error happen
 */
int doclone_create(const dc_doclone *dc_obj) {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = 0;

	try {
		const char *image = reinterpret_cast<const char *>(dc_obj->_image);
		dcl->setImage(image);

		const char *device = reinterpret_cast<const char *>(dc_obj->_device);
		dcl->setDevice(device);

		dcl->create();
	} catch(const Doclone::Exception &ex) {
		ex.logMsg();
		retVal = -1;
	}

	return retVal;
}

/**
 * \ingroup CWrapperAPI
 * \brief Restores an image in a device.
 *
 * Both image and device path must be set before calling this function.
 *
 * \return 0 if the process has success, -1 if any error happen
 */
int doclone_restore(const dc_doclone *dc_obj) {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = 0;

	try {
		const char *image = reinterpret_cast<const char *>(dc_obj->_image);
		dcl->setImage(image);

		const char *device = reinterpret_cast<const char *>(dc_obj->_device);
		dcl->setDevice(device);

		dcl->restore();
	} catch(const Doclone::Exception &ex) {
		ex.logMsg();
		retVal = -1;
	}

	return retVal;
}

/**
 * \ingroup CWrapperAPI
 * \brief Sends an image or a device to the network.
 *
 * The number of receivers and either image or device path must be set
 * before calling this function.
 *
 * \return 0 if the process has success, -1 if any error happen
 */
int doclone_send(const dc_doclone *dc_obj) {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = 0;

	try {
		const char *image = reinterpret_cast<const char *>(dc_obj->_image);
		dcl->setImage(image);

		const char *device = reinterpret_cast<const char *>(dc_obj->_device);
		dcl->setDevice(device);

		dcl->setNodesNumber(dc_obj->_nodesNumber);

		dcl->send();
	} catch(const Doclone::Exception &ex) {
		ex.logMsg();
		retVal = -1;
	}

	return retVal;
}

/**
 * \ingroup CWrapperAPI
 * \brief Receives an image or a device from the network.
 *
 * Server's IP and either image or device path must be set before calling this
 * function.
 *
 * \return 0 if the process has success, -1 if any error happen
 */
int doclone_receive(const dc_doclone *dc_obj) {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = 0;

	try {
		const char *image = reinterpret_cast<const char *>(dc_obj->_image);
		dcl->setImage(image);

		const char *device = reinterpret_cast<const char *>(dc_obj->_device);
		dcl->setDevice(device);

		const char *address = reinterpret_cast<const char *>(dc_obj->_address);
		dcl->setAddress(address);

		dcl->receive();
	} catch(const Doclone::Exception &ex) {
		ex.logMsg();
		retVal = -1;
	}

	return retVal;
}

/**
 * \ingroup CWrapperAPI
 * \brief Sends an image or a device to the network in link mode.
 *
 * Image or device path must be set before calling this function.
 *
 * \return 0 if the process has success, -1 if any error happen
 */
int doclone_chain_origin(const dc_doclone *dc_obj) {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = 0;

	try {
		const char *image = reinterpret_cast<const char *>(dc_obj->_image);
		dcl->setImage(image);

		const char *device = reinterpret_cast<const char *>(dc_obj->_device);
		dcl->setDevice(device);

		dcl->chainOrigin();
	} catch(const Doclone::Exception &ex) {
		ex.logMsg();
		retVal = -1;
	}

	return retVal;
}

/**
 * \ingroup CWrapperAPI
 * \brief Receives an image or a device from the network in link mode.
 *
 * Image or device path must be set before calling this function.
 *
 * \return 0 if the process has success, -1 if any error happen
 */
int doclone_chain_link(const dc_doclone *dc_obj) {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

		int retVal = 0;

		try {
			const char *image = reinterpret_cast<const char *>(dc_obj->_image);
			dcl->setImage(image);

			const char *device = reinterpret_cast<const char *>(dc_obj->_device);
			dcl->setDevice(device);

			dcl->chainLink();
		} catch(const Doclone::Exception &ex) {
			ex.logMsg();
			retVal = -1;
		}

		return retVal;
}

/**
 * \ingroup CWrapperAPI
 * \brief Through this function the user can set its callback for the transfer
 * events of the library
 */
void doclone_subscribe_to_tranfer_events(dc_doclone *dc_obj,
		transferCallback call) {
	DefaultObserver *obs =
			reinterpret_cast<DefaultObserver *>(dc_obj->_observer);
	obs->setTCallback(call);
}

/**
 * \ingroup CWrapperAPI
 * \brief Through this function the user can set its callback for the operation
 * events of the library
 */
void doclone_subscribe_to_operation_events(dc_doclone *dc_obj,
		operationCallback call) {
	DefaultObserver *obs =
			reinterpret_cast<DefaultObserver *>(dc_obj->_observer);
	obs->setOCallback(call);
}

/**
 * \brief Through this function the user can set its callback for the general
 * events of the library
 */
void doclone_subscribe_to_general_events(dc_doclone *dc_obj,
		generalCallback call) {
	DefaultObserver *obs =
			reinterpret_cast<DefaultObserver *>(dc_obj->_observer);
	obs->setGCallback(call);
}

/**
 * \ingroup CWrapperAPI
 * \brief Through this function the user can set its callback for listen the
 * notifications of the exceptions
 */
void doclone_subscribe_to_notifications(dc_doclone *dc_obj,
		notificationCallback call) {
	DefaultObserver *obs =
			reinterpret_cast<DefaultObserver *>(dc_obj->_observer);
	obs->setNCallback(call);
}

/*
 * Setters for the properties of the dc_doclone object
 */

/**
 * \ingroup CWrapperAPI
 * \brief Sets the image path of the given dc_doclone object
 */
void doclone_set_image(dc_doclone *dc_obj, const char *image) {
	char *dst = reinterpret_cast<char *>(dc_obj->_image);
	strncpy(dst, image, sizeof(dc_obj->_image));
}

/**
 * \ingroup CWrapperAPI
 * \brief Sets the device path of the given dc_doclone object
 */
void doclone_set_device(dc_doclone *dc_obj, const char *device) {
	char *dst = reinterpret_cast<char *>(dc_obj->_device);
	strncpy(dst, device, sizeof(dc_obj->_device));
}

/**
 * \ingroup CWrapperAPI
 * \brief Sets the IP address of the server for the given dc_doclone object
 */
void doclone_set_address(dc_doclone *dc_obj, const char *address) {
	char *dst = reinterpret_cast<char *>(dc_obj->_address);
	strncpy(dst, address, sizeof(dc_obj->_address));
}

/**
 * \ingroup CWrapperAPI
 * \brief Sets the number of receivers of the given dc_doclone object
 *
 * Useful only if this object will be used to be a server
 */
void doclone_set_nodes_number(dc_doclone *dc_obj, unsigned int number) {
	dc_obj->_nodesNumber = number;
}

/**
 * \ingroup CWrapperAPI
 * \brief Sets the empty flag of the given dc_doclone object
 */
void doclone_set_empty(dc_doclone *dc_obj, unsigned short empty) {
	dc_obj->_empty = empty;
}

/**
 * \ingroup CWrapperAPI
 * \brief Sets the force flag of the given dc_doclone object
 */
void doclone_set_force(dc_doclone *dc_obj, unsigned short force) {
	dc_obj->_force = force;
}

/*
 * C wrapper for callback functions
 */

/**
 * \ingroup CWrapperAPI
 * \brief Initializes the attributes and subscribes itself for all the libdoclone
 * events
 */
DefaultObserver::DefaultObserver(): _tCallback(), _oCallback(), _gCallback(),
		_nCallback() {
	// We subscribe to all subjects
	Doclone::Clone *dcl = Doclone::Clone::getInstance();
	Doclone::DataTransfer *trans = Doclone::DataTransfer::getInstance();
	Doclone::Logger *log = Doclone::Logger::getInstance();
	trans->addObserver(this);
	dcl->addObserver(this);
	log->addObserver(this);
}

/**
 * \ingroup CWrapperAPI
 * \brief Listens the transfer events and calls the user-defined C callback
 * if any
 */
void DefaultObserver::notify(Doclone::dcTransferEvent event,
		const uint64_t numBytes) {
	if(this->_tCallback) {
		(*this->_tCallback)(static_cast<dcTransferEvent>(event), numBytes);
	}
}

/**
 * \ingroup CWrapperAPI
 * \brief Listens the operation events and calls the user-defined C callback
 * if any
 */
void DefaultObserver::notify(Doclone::dcOperationEvent event,
		Doclone::dcOperationType type, const std::string &target) {
	if(this->_oCallback) {
		(*this->_oCallback)(static_cast<dcOperationEvent>(event),
				static_cast<dcOperationType>(type), target.c_str());
	}
}

/**
 * \ingroup CWrapperAPI
 * \brief Listens the general events and calls the user-defined C callback
 * if any
 */
void DefaultObserver::notify(Doclone::dcEvent event,
		const std::string &target) {
	if(this->_gCallback) {
		(*this->_gCallback)(static_cast<dcEvent>(event), target.c_str());
	}
}

/**
 * \ingroup CWrapperAPI
 * \brief Listens the exception messages and calls the user-defined C callback
 * if any
 */
void DefaultObserver::notify(const std::string &str) {
	if(this->_nCallback) {
		(*this->_nCallback)(str.c_str());
	}
}

/*
 * Setters for the callback functions
 */

void DefaultObserver::setTCallback(const transferCallback call) {
	this->_tCallback = call;
}

void DefaultObserver::setOCallback(const operationCallback call) {
	this->_oCallback = call;
}

void DefaultObserver::setGCallback(const generalCallback call) {
	this->_gCallback = call;
}

void DefaultObserver::setNCallback(const notificationCallback call) {
	this->_nCallback = call;
}
