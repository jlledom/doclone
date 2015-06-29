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

#ifndef CLONE_H_
#define CLONE_H_

#include <stdint.h>

#include <string>
#include <vector>

#include <doclone/Operation.h>
#include <doclone/observer/AbstractSubject.h>

#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \defgroup CPPAPI C++ API
 * \brief C++ API for libdoclone.
 *
 * This class provides a way for calling the methods of libdoclone from a C++
 * program. To create an object, the static method getInstance() must be called
 * by this way:
 *
 * \code
 * Doclone::Clone *dcl = Doclone::Clone::getInstance();
 * \endcode
 *
 * It is possible to subscribe the events of the library. These can be used to
 * know the operations progress and possible incidences. To do this, some
 * functions must be implemented with the proper prototype.
 *
 * First of all, These are the four kinds of libdoclone events:
 *
 * - The transfer events:
 * 		Events that inform about the process of data read/write or send/receive.
 * 		There are only two transfer events: The TRANSFERRED_BYTES event
 * 		and the TOTAL_SIZE event.
 * 		- The TRANSFERRED_BYTES can be raised several times during the program
 * 		execution, and informs about the amount of bytes transferred until
 * 		now.
 * 		- The TOTAL_SIZE is raised only once in the execution time, when the
 *		library has determined the total amount of bytes that should be
 *		transferred. This can be useful for creating a progress bar.
 *
 * - The operation events:
 * 		One operation is one action that the library will perform. There are
 * 		many different operations, such as reading partition table of a disc,
 * 		writing the UUID of a file system, installing GRUB, etc. This kind of
 * 		event can have two states: Pending or Completed. The library can add new
 * 		pending operations and mark them as completed many times during the
 * 		execution time.
 *
 * - The general events:
 * 		These are simple events that can happen during the program execution,
 * 		such as receiving a connection from other libdoclone instance or
 * 		interrupting the program by order of the user.
 *
 * - The notification of the exceptions:
 * 		In case of something doesn't work as expected, for example, when the
 * 		library tries to write in a folder which access is denied or in case of
 * 		connection error, then the library raises an exception until the highest
 * 		level of the calling hierarchy. Since C doesn't support exceptions, all
 * 		of them are captured by the library, but their error messages are sent
 * 		to the listening views.
 *
 * Subscribing to any of these events can be performed by extending the
 * Doclone::AbstractObserver class and overriding all its notify() methods:
 *
 * \code
 * 	void notify(Doclone::dcTransferEvent event, uint64_t numBytes);
 * 	void notify(Doclone::dcOperationEvent event,
 * 		Doclone::dcOperationType type, const std::string &target);
 * 	void notify(Doclone::dcEvent event, const std::string &target);
 * 	void notify(const std::string &str);
 * \endcode
 *
 * For example:
 *
 * \code
 * 	class MyObserver : public Doclone::AbstractObserver {
 * 	public:
 * 		MyObserver();
 *
 * 		void notify(Doclone::dcTransferEvent event, uint64_t numBytes);
 * 		void notify(Doclone::dcOperationEvent event,
 * 				Doclone::dcOperationType type, const std::string &target);
 * 		void notify(Doclone::dcEvent event, const std::string &target);
 * 		void notify(const std::string &str);
 * 	};
 *
 * 	void MyObserver::notify(const std::string &str) {
 * 		std::cerr << "Something has happened:" << " " << str << std::endl;
 * 	}
 * \endcode
 *
 * The next step is to set all the doclone object required properties before
 * performing the work. These are:
 *
 * - image (char*): It is the image file path to work with
 * - device (char*): The device path to be read or written
 * - address (char*): The server IP address
 * - nodes number (int): The number of receivers
 * - empty (int): Clone the partition table without reading/writing the data (true or false)
 * - force (int): Force working even if the image doesn't fit in the destination device (true or false)
 *
 * These are some setters for configuring the properties:
 *
 * \code
 * 	void setEmpty(bool empty);
 * 	void setNodesNumber(unsigned int nodesNum);
 * 	void setDevice(const std::string &device);
 * 	void setImage(const std::string &image);
 * 	void setAddress(const std::string &address);
 * 	void setForce(bool force);
 * \endcode
 *
 * The last step is to call one of the methods that perform the work:
 *
 * \code
 * 	void create() throw(Exception);
 * 	void restore() throw(Exception);
 * 	void send() throw(Exception);
 * 	void receive() throw(Exception);
 * 	void chainOrigin() throw(Exception);
 * 	void chainLink() throw(Exception);
 * \endcode
 *
 * All this methods raise an exception if anything goes wrong. The library has
 * two kinds of exceptions, the Warning Exceptions and the Error Exceptions. The
 * Warning Exceptions don't stop the execution and are captured in the library,
 * but their messages are sent to the views. These messages are received by the
 * views with "void notify(const std::string &str)" method override. The Error
 * Exceptions are defined for severe errors, these ones always stop the
 * execution and are raised to the caller. The library user must decide what to
 * do with them.
 *
 * These are some examples of use:
 *
 * - Creating or restoring an image of/into a device:
 * 		- Set the image and the device on the doclone object:
 * 		\code
 * 			Doclone::Clone *dcl = Doclone::Clone::getInstace();
 *
 * 			dcl->setImage("/home/user/image.doclone");
 * 			dcl->setDevice("/dev/sdf");
 * 		\endcode
 * 		- Then call the create function:
 * 		\code
 * 			try {
 * 				dcl->create();
 * 			} catch (const Doclone::Exception &ex) {
 * 				// Do something
 * 			}
 * 		\endcode
 * 		- Or call the restore function:
 * 		\code
 * 			try {
 * 				dcl->restore();
 * 			} catch (const Doclone::Exception &ex) {
 * 				// Do something
 * 			}
 * 		\endcode
 *
 * - Sending an image over network:
 * 		- For the server, set the number of receivers and the image to read.
 * 		\code
 * 			dcl->setNodesNumber(1);
 * 			dcl->setImage("/home/user/image.doclone");
 * 		\endcode
 * 		- Then call the send function:
 * 		\code
 * 			try {
 * 				dcl->send();
 * 			} catch (const Doclone::Exception &ex) {
 * 				// Do something
 * 			}
 * 		\endcode
 *
 * 		- For the client, set the server IP address and the device to write:
 * 		\code
 * 			dcl->setAddress("192.168.0.10");
 * 			dcl->setDevice("/dev/sdb");
 *
 * 			try {
 * 				dcl->receive();
 * 			} catch (const Doclone::Exception &ex) {
 * 				// Do something
 * 			}
 * 		\endcode
 */

/**
 * \class Clone
 * \brief Interface of the library.
 *
 * This class provides an interface for a library user, such as a frontend or a
 * controller, to interact with the library and read its state.
 * This program has 4 main uses:
 *
 * - Create a image of the disk/partition
 * - Restore a image of the disk/partition
 * - Read/create and send an image over the network
 * - Receive and restore/write image to the disk
 *
 * All the network operations can be performed using the unicast/multicast mode
 * (Recommended) or the link mode.
 *
 * For use it, first at all, the library user must set the parameters, like
 * image path, device path, no-data, etc.
 *
 * After, the library user must call to the proper function.
 *
 * During the execution, the state of the execution can be read by using the
 * function getOperations(), that returns a vector with a list of all
 * the operations that must be performed, and the current state of them.
 *
 * Also during the execution, the library user can call to the function
 * getTransferredBytes() to know how much data has been written/read at until
 * the moment.
 *
 * This class is singleton.
 */
class Clone : public AbstractSubject {
public:
	static Clone* getInstance();
	~Clone();

	void create() throw(Exception);
	void restore() throw(Exception);
	void send() throw(Exception);
	void receive() throw(Exception);
	void chainOrigin() throw(Exception);
	void chainLink() throw(Exception);

	bool getEmpty() const;
	void setEmpty(bool empty);
	unsigned int getNodesNumber() const;
	void setNodesNumber(unsigned int nodesNum);
	const std::string &getDevice() const;
	void setDevice(const std::string &device);
	const std::string &getImage() const;
	void setImage(const std::string &image);
	const std::string &getAddress() const;
	void setAddress(const std::string &address);
	const std::string &getInterface() const;
	void setInterface(const std::string &interface);
	bool getForce() const;
	void setForce(bool force);

	void addOperation(Operation *op);
	void markCompleted(dcOperationType type, const std::string &target);
	Operation* getOperation(dcOperationType type, const std::string &target);

	void triggerEvent(dcEvent event, const std::string &target);
protected:
	/// Private constructor for singleton pattern
	Clone();

	/// Image path entered by the user
	std::string _image;
	/// Device path entered by the user
	std::string _device;
	/// Ip address entered by the user
	std::string _address;
	/// Ip address of the interface to be used in the link mode
	std::string _interface;
	/// Number of receivers entered by the user
	unsigned int _nodesNumber;
	/// Empty mode enabled/disabled
	bool _empty;
	/// Mode force enabled/disabled
	bool _force;

	/// Vector with the state of the execution
	std::vector<Operation *> _operations;
};

}

#endif /* CLONE_H_ */
