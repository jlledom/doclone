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

#ifndef ABSTRACTSUBJECT_H_
#define ABSTRACTSUBJECT_H_

#include <set>
#include <string>

#include <doclone/observer/AbstractObserver.h>

namespace Doclone {

/**
 * \class AbstractSubject
 *
 * Base class for the subjects observer pattern, when other class extends this
 * one, can notify new events to the possible observers.
 *
 * \date September, 2011
 */
class AbstractSubject {
public:
	AbstractSubject();
	virtual ~AbstractSubject(){}

	void addObserver(AbstractObserver* ob);
	void removeObserver(AbstractObserver* ob);

protected:
	void notifyObservers(dcTransferEvent event, uint64_t numBytes);
	void notifyObservers(dcOperationEvent event, dcOperationType type,
				const std::string &target);
	void notifyObservers(dcEvent event, const std::string &target);
	void notifyObservers(const std::string &message);

	/// List of the observers subscribed
	std::set<AbstractObserver*> _observers;
};

} /* namespace Doclone */
#endif /* ABSTRACTSUBJECT_H_ */
