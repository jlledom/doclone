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

#ifndef ABSTRACTOBSERVER_H_
#define ABSTRACTOBSERVER_H_

#include <stdint.h>

#include <string>

#include <doclone/Operation.h>

namespace Doclone {

/**
 * \enum dcEvent
 * \brief Type for general events in the library
 *
 * \var EVT_CANCEL_EXECUTION
 * 	The program detects a SIGINT
 * \var EVT_FINISH_EXECUTION
 * 	Execution successfully finished
 * \var EVT_NEW_CONNECION
 * 	New incoming connection
 */
enum dcEvent {
	EVT_CANCEL_EXECUTION,
	EVT_FINISH_EXECUTION,
	EVT_NEW_CONNECION
};

/**
 * \enum dcTransferEvent
 * \brief Type of the transfer notification
 *
 * \var TRANS_TOTAL_SIZE
 * 		The value of the total size to transfer has changed
 * \var TRANS_TRANSFERRED_BYTES
 * 		The value of the transferred bytes has changed
 */
enum dcTransferEvent {
	TRANS_TOTAL_SIZE,
	TRANS_TRANSFERRED_BYTES
};

/**
 * \enum dcOperationEvent
 * \brief Type of the operation notification
 *
 * \var OPER_ADD
 * 		The subject has added a new pending operation
 * \var OPER_MARK_COMPLETED
 * 		The subject has completed a pending operation
 */
enum dcOperationEvent {
	OPER_ADD,
	OPER_MARK_COMPLETED
};

/**
 * \class AbstractObserver
 *
 * Stuff for the observer pattern, if the user of the library extends this class
 * and overloads its methods, will get the events in runtime.
 *
 * \date September, 2011
 */
class AbstractObserver {
public:
	virtual ~AbstractObserver(){}

	/**
	 * \brief Gets the notification of the transfer events.
	 *
	 * That are "new total size set" and "updated the amount of bytes
	 * transferred"
	 *
	 * \param event
	 * 		The event triggered
	 * \param numBytes
	 * 		Number of bytes to set/update
	 */
	virtual void notify(dcTransferEvent event, uint64_t numBytes)=0;

	/**
	 * \brief Gets the notification of the operation events.
	 *
	 * That may be many, as "partition formatted", "disk label written", etc.
	 *
	 * \param event
	 * 		The event triggered
	 * \param oper
	 * 		The involving operation
	 * \param target
	 * 		The involving target. Like "/dev/sdb" or "/dev/sda3"
	 *
	 */
	virtual void notify(dcOperationEvent event, dcOperationType oper,
				const std::string &target)=0;

	/**
	 * \brief Gets the notification of the general events.
	 *
	 * That may be many, as "SIGINT detected", "New incoming connection", etc.
	 *
	 * \param event
	 * 		The event triggered
	 * \param target
	 * 		The involving target. Like "192.168.1.10"
	 */
	virtual void notify(dcEvent event, const std::string &target)=0;

	/**
	 * \brief Gets the notification of the exceptions.
	 *
	 * It may throw some exceptions that are not important enough to stop
	 * the program, but is useful for the user of library to know them.
	 * This kind of exceptions call this function to send its messages to the
	 * user.
	 *
	 * \param str
	 * 		The message of the exception.
	 */
	virtual void notify(const std::string &str)=0;
};

} /* namespace Doclone */
#endif /* ABSTRACTOBSERVER_H_ */
