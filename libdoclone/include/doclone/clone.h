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

#ifndef _CLONE_H_
#define _CLONE_H_

/**
 * \mainpage Libdoclone - library for cloning and making backups of GNU/Linux systems
 *
 * <h1>What is libdoclone?</h1>
 *
 * Libdoclone is a free project developed in C++ for creating or restoring
 * images of GNU/Linux systems. It can also work into a LAN to transfer data
 * between a group of computers at the same time. This tool provides an easy way
 * for managing the installation of GNU/Linux in offices or schools, even
 * between machines with different hardware like hard-disk size, using only the
 * needed space for the data.
 *
 * Libdoclone uses <a href="http://www.gnu.org/software/parted/">libparted</a>
 *
 * <h1>License</h1>
 * Copyright (C) 2013-2015 Joan Lledó
 * &lt;<a href="mailto:joanlluislledo@gmail.com">joanlluislledo@gmail.com</a>&gt;
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdint.h>

/* Include this headers only in C++
 *
 * For C, libdoclone has another API defined bellow
 */
#ifdef __cplusplus

#include <doclone/DataTransfer.h>
#include <doclone/Logger.h>
#include <doclone/observer/AbstractObserver.h>
#include <doclone/Operation.h>
#include <doclone/exception/Exception.h>

#endif //__cplusplus

/*
 * C Wrapper API
 */

/**
 * \defgroup CWrapperAPI C wrapper API
 * \brief C API for libdoclone.
 *
 * The main type of the C API is the dc_doclone struct. This struct provides a
 * way for calling the methods of libdoclone from a C program. To create an object,
 * the function doclone_new() must be called by this way:
 *
 * \code
 * 	dc_doclone *dc_obj = doclone_new();
 * \endcode
 *
 * After using a dc_doclone object, its memory can be freed by calling:
 *
 * \code
 * 	doclone_destroy(dc_obj);
 * \endcode
 *
 * It is possible to subscribe the events of the library. These can be used to
 * know the operations progress and possible incidences.
 * To do this, some functions must be implemented with the proper prototype.
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
 * 		pending operations and mark them as completed during the execution time.
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
 * Subscribing to any of these events can be performed by calling one of the
 * following functions:
 *
 * \code
 * doclone_subscribe_to_tranfer_events(dc_doclone *dc_obj, transferCallback call);
 * doclone_subscribe_to_operation_events(dc_doclone *dc_obj, operationCallback call);
 * doclone_subscribe_to_general_events(dc_doclone *dc_obj, generalCallback call);
 * doclone_subscribe_to_notifications(dc_doclone *dc_obj, notificationCallback call);
 * \endcode
 *
 * All these functions receive a pointer to the dc_doclone object as first
 * parameter, and a pointer to a callback function as second. The callback
 * function passed as parameter must have the proper prototype. These prototypes
 * are specified by some defined types:
 *
 * 	\code
 * 	transferCallback:  void (*transferCallback) (dcTransferEvent event, uint64_t numBytes);
 *
 * 	operationCallback: void (*operationCallback) (dcOperationEvent event, dcOperationType type, const char *target);
 *
 * 	generalCallback: void (*generalCallback) (dcEvent event, const char *target);
 *
 * 	notificationCallback void (*notificationCallback) (const char *str);
 * 	\endcode
 *
 * Note that dcTransferEvent, dcOperationEvent, dcOperationType and dcEvent are
 * just a C wrapper for Doclone::dcTransferEvent, Doclone::dcOperationEvent,
 * Doclone::dcOperationType and Doclone::dcEvent enums of the library.
 *
 * After declaring one function with the proper prototype, it is possible to
 * use it as callback to listen the library events, for example:
 *
 * \code
 * 	void notify(const char *str) {
 * 		dprintf(2, "%s\n", str);
 * 	}
 *
 * 	...
 *
 * 	doclone_subscribe_to_notifications(dc_obj, &notify);
 * 	\endcode
 *
 * By this way it can listen to all the events of the library:
 *
 * \code
 * 	doclone_subscribe_to_tranfer_events(dc_obj, &notifyTransfer);
 * 	doclone_subscribe_to_operation_events(dc_obj, &notifyOperation);
 * 	doclone_subscribe_to_general_events(dc_obj, &notifyGeneral);
 * 	doclone_subscribe_to_notifications(dc_obj, &notify);
 * \endcode
 *
 * The next step is to set all the dc_doclone object required properties before
 * performing the work. These are:
 *
 * - image (char*): It is the image file path to work with
 * - device (char*): The device path to be read or written
 * - address (char*): the server IP address
 * - nodes number (int): The number of receivers
 * - empty (int): Clone the partition table without reading/writing the data (true or false)
 * - force (int): Force working even if the image doesn't fit in the destination device (true or false)
 *
 * These are some functions for configuring those properties:
 *
 * \code
 * 	void doclone_set_image(dc_doclone *dc_obj, const char *image);
 * 	void doclone_set_device(dc_doclone *dc_obj, const char *device);
 * 	void doclone_set_address(dc_doclone *dc_obj, const char *address);
 * 	void doclone_set_interface(dc_doclone *dc_obj, const char *interface);
 * 	void doclone_set_nodes_number(dc_doclone *dc_obj, unsigned int number);
 * 	void doclone_set_empty(dc_doclone *dc_obj, unsigned short empty);
 * 	void doclone_set_force(dc_doclone *dc_obj, unsigned short force);
 * \endcode
 *
 * The last step is to call one of the functions that perform the work:
 *
 * \code
 * 	int doclone_create(const dc_doclone *dc_obj);
 * 	int doclone_restore(const dc_doclone *dc_obj);
 * 	int doclone_send(const dc_doclone *dc_obj);
 * 	int doclone_receive(const dc_doclone *dc_obj);
 * 	int doclone_chain_origin(const dc_doclone *dc_obj);
 * 	int doclone_chain_link(const dc_doclone *dc_obj);
 * \endcode
 *
 * All of these functions receive a pointer to a dc_doclone object which
 * attributes must be properly set, and return 0 if everything is OK, or -1 if
 * something has failed.
 *
 * These are some examples of use:
 *
 * - Creating or restoring an image of/into a device:
 * 		- Set the image and the device of the dc_doclone object:
 * 		 \code
 * 			doclone_set_image(dc_obj, "/home/user/image.doclone");
 * 			doclone_set_device(dc_obj, "/dev/sdb");
 * 		\endcode
 * 		- Then call the create function:
 * 		\code
 * 			doclone_create(dc_obj);
 * 		\endcode
 * 		- Or the restore function:
 * 		\code
 * 			doclone_restore(dc_obj);
 * 		\endcode
 *
 * - Sending an image over network:
 * 		- For the server, set the number of receivers and the image to read.
 * 		\code
 * 			doclone_set_nodes_number(dc_obj, 1);
 * 			doclone_set_image(dc_obj, "/home/user/image.doclone");
 * 		\endcode
 * 		- Then call the send function:
 * 		\code
 * 			doclone_send(dc_obj);
 * 		\endcode
 *
 * 		- For the client, set the server IP address and the device to write:
 *   	\code
 * 			doclone_set_address(dc_obj, "192.168.0.10");
 * 			doclone_set_device(dc_obj, "/dev/sdb");
 *
 * 			doclone_receive(dc_obj);
 * 		\endcode
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum dcTransferEvent
 * \brief C wrapper for Doclone::dcTransferEvent
 *
 * \var TRANS_TOTAL_SIZE
 * 		The value of the total size to be transferred has changed
 * \var TRANS_TRANSFERRED_BYTES
 * 		The value of the transferred bytes has changed
 */
typedef enum dcTransferEvent {
	TRANS_TOTAL_SIZE,
	TRANS_TRANSFERRED_BYTES
} dcTransferEvent;

/**
 * \enum dcOperationEvent
 * \brief C wrapper for Doclone::dcOperationEvent
 *
 * \var OPER_ADD
 * 		The subject has added a new operation pending
 * \var OPER_MARK_COMPLETED
 * 		The subject has completed an operation pending
 */
typedef enum dcOperationEvent {
	OPER_ADD,
	OPER_MARK_COMPLETED
} dcOperationEvent;

/**
 * \enum dcOperationType
 * \brief C wrapper for Doclone::dcOperationType
 *
 * \var OP_NONE
 * 	No operation
 * \var OP_READ_PARTITION_TABLE
 * 	Reading the partition table
 * \var OP_MAKE_DISKLABEL
 * 	Writing a new disklabel
 * \var OP_CREATE_PARTITION
 * 	Creating a new partition
 * \var OP_FORMAT_PARTITION
 * 	Formatting a partition
 * \var OP_WRITE_PARTITION_FLAGS
 * 	Writing the flags of a partition
 * \var OP_WRITE_FS_LABEL
 * 	Writing the label of a filesystem
 * \var OP_WRITE_FS_UUID
 * 	Writing the uuid of a filesystem
 * \var OP_READ_DATA
 * 	Reading the data of a partition
 * \var OP_WRITE_DATA
 * 	Writing the data of a partition
 * \var OP_GRUB_INSTALL
 * 	Installing GRUB
 * \var OP_TRANSFER_DATA
 * 	Sending or receiving data
 * \var OP_WAIT_SERVER
 * 	Waiting for connect to server
 * \var OP_WAIT_CLIENTS
 * 	Waiting for connect to client/s
 */
typedef enum dcOperationType {
	OP_NONE,
	OP_READ_PARTITION_TABLE,
	OP_MAKE_DISKLABEL,
	OP_CREATE_PARTITION,
	OP_FORMAT_PARTITION,
	OP_WRITE_PARTITION_FLAGS,
	OP_WRITE_FS_LABEL,
	OP_WRITE_FS_UUID,
	OP_READ_DATA,
	OP_WRITE_DATA,
	OP_GRUB_INSTALL,
	OP_TRANSFER_DATA,
	OP_WAIT_SERVER,
	OP_WAIT_CLIENTS
} dcOperationType;

/**
 * \enum dcEvent
 * \brief C wrapper for Doclone::dcEvent
 *
 * \var EVT_CANCEL_EXECUTION
 * 	The program detects a SIGINT
 * \var EVT_FINISH_EXECUTION
 * 	Execution successfully finished
 * \var EVT_NEW_CONNECION
 * 	New incoming connection
 */
typedef enum dcEvent {
	EVT_CANCEL_EXECUTION,
	EVT_FINISH_EXECUTION,
	EVT_NEW_CONNECION
} dcEvent;

/**
 * \typedef transferCallback
 *
 * The prototype of the function to be called when a transfer event has
 * occurred.
 */
typedef void (*transferCallback) (dcTransferEvent event,
		uint64_t numBytes);

/**
 * \typedef operationCallback
 *
 * The prototype of the function to be called when an operation event has
 * occurred.
 */
typedef void (*operationCallback) (dcOperationEvent event, dcOperationType type,
		const char *target);

/**
 * \typedef generalCallback
 *
 * The prototype of the function to be called when a general event has occurred.
 */
typedef void (*generalCallback) (dcEvent event, const char *target);

/**
 * \typedef notificationCallback
 *
 * The prototype of the function to be called when an exception occurs and
 * notifies its message.
 */
typedef void (*notificationCallback) (const char *str);

/**
 * \struct dc_doclone
 * \brief Main object of the C wrapper API of libdoclone
 */
typedef struct dc_doclone {
	/// Image path entered by the user
	char _image[512];
	/// Device path entered by the user
	char _device[512];
	/// Ip address entered by the user
	char _address[20];
	/// Ip address of the network interface entered by the user
	char _interface[20];
	/// Number of receivers entered by the user
	uint32_t _nodesNumber;
	/// Empty mode enabled/disabled
	uint8_t _empty;
	/// Mode force enabled/disabled
	uint8_t _force;
	/// Event subscriber object
	void * _observer;
} dc_doclone;


/*
 * AC_CHECK_LIB Macro compatibility
 */
int libdoclone_is_present();

/*
 * Memory management for the dc_doclone object
 */
dc_doclone *doclone_new();
void doclone_destroy(dc_doclone *dc_obj);

/*
 * Operations that can be performed
 */
int doclone_create(const dc_doclone *dc_obj);
int doclone_restore(const dc_doclone *dc_obj);
int doclone_send(const dc_doclone *dc_obj);
int doclone_receive(const dc_doclone *dc_obj);
int doclone_chain_origin(const dc_doclone *dc_obj);
int doclone_chain_link(const dc_doclone *dc_obj);

/*
 * Setters for the dc_doclone object
 */
void doclone_set_image(dc_doclone *dc_obj, const char *image);
void doclone_set_device(dc_doclone *dc_obj, const char *device);
void doclone_set_address(dc_doclone *dc_obj, const char *address);
void doclone_set_interface(dc_doclone *dc_obj, const char *interface);
void doclone_set_nodes_number(dc_doclone *dc_obj, unsigned int number);
void doclone_set_empty(dc_doclone *dc_obj, unsigned short empty);
void doclone_set_force(dc_doclone *dc_obj, unsigned short force);

/*
 * Functions for set the callbacks of libdoclone events
 */
void doclone_subscribe_to_tranfer_events(dc_doclone *dc_obj,
		transferCallback call);
void doclone_subscribe_to_operation_events(dc_doclone *dc_obj,
		operationCallback call);
void doclone_subscribe_to_general_events(dc_doclone *dc_obj,
		generalCallback call);
void doclone_subscribe_to_notifications(dc_doclone *dc_obj,
		notificationCallback call);

/**@}*/

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * \class DefaultObserver
 *
 * Convenience class for implement a C wrapper for the observer pattern
 *
 * \date April, 2012
 */
class DefaultObserver : public Doclone::AbstractObserver {
public:
	DefaultObserver();

	void notify(Doclone::dcTransferEvent event, uint64_t numBytes);
	void notify(Doclone::dcOperationEvent event,
			Doclone::dcOperationType type, const std::string &target);
	void notify(Doclone::dcEvent event, const std::string &target);
	void notify(const std::string &str);

	void setTCallback(const transferCallback call);
	void setOCallback(const operationCallback call);
	void setGCallback(const generalCallback call);
	void setNCallback(const notificationCallback call);

private:
	/// Callback function for the transfer events
	transferCallback _tCallback;
	/// Callback function for the operation events
	operationCallback _oCallback;
	/// Callback functions for the general events
	generalCallback _gCallback;
	/// Callback function for the notifications of the exceptions
	notificationCallback _nCallback;
};

#endif //__cplusplus

#endif /* _CLONE_H_ */
