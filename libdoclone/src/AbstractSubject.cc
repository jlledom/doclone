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

#include <doclone/observer/AbstractSubject.h>

namespace Doclone {

AbstractSubject::AbstractSubject(): _observers() {
}

/**
 * Subscribes a new observer
 *
 * \param ob
 * 		An object whose class extends AbstractObserver
 */
void AbstractSubject::addObserver(AbstractObserver* ob) {
	this->_observers.insert(ob);
}

/**
 * Unsubscribes an observer
 *
 * \param ob
 * 		An object whose class extends AbstractObserver
 */
void AbstractSubject::removeObserver(AbstractObserver* ob) {
	this->_observers.erase(ob);
}

/**
 * Calls the method notify() in all observers, for the transfer events
 *
 * \param event
 * 		The event triggered
 * \param numBytes
 * 		Number of bytes to set/update
 */
void AbstractSubject::notifyObservers(dcTransferEvent event, uint64_t numBytes) {

	 std::set<AbstractObserver*>::iterator itr;

	for ( itr = this->_observers.begin();
		  itr != this->_observers.end(); itr++ ) {
		(*itr)->notify(event, numBytes);
	}
}

/**
 * Calls the method notify() in all observers, for the operation events
 *
 * \param event
 * 		The event triggered
 * \param type
 * 		The involving operation
 * \param target
 * 		The involving target. Like "/dev/sdb" or "/deb/sda3"
 */
void AbstractSubject::notifyObservers(dcOperationEvent event,
		dcOperationType type, const std::string &target) {

	 std::set<AbstractObserver*>::iterator itr;

	for ( itr = this->_observers.begin();
		  itr != this->_observers.end(); itr++ ) {
		(*itr)->notify(event, type, target);
	}
}

/**
 * Calls the method notify() in all observers, for the general events
 *
 * \param event
 * 		The event triggered
 * \param target
 * 		The involving target. Like "/dev/sdb" or "/dev/sda3"
 */
void AbstractSubject::notifyObservers(dcEvent event,
		const std::string &target) {

	 std::set<AbstractObserver*>::iterator itr;

	for ( itr = this->_observers.begin();
		  itr != this->_observers.end(); itr++ ) {
		(*itr)->notify(event, target);
	}
}

/**
 * Calls the method notify() in all observers, for the error messages
 *
 * \param message
 * 		The error message
 */
void AbstractSubject::notifyObservers(const std::string &message) {

	 std::set<AbstractObserver*>::iterator itr;

	for ( itr = this->_observers.begin();
		  itr != this->_observers.end(); itr++ ) {
		(*itr)->notify(message);
	}
}

} /* namespace Doclone */
