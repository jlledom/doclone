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

#include <doclone/Operation.h>

namespace Doclone {

/**
 * \brief Only initialization of attributes
 */
Operation::Operation(dcOperationType type, const std::string &target)
	: _type(type), _completed(false), _target(target) {}

// Getters and setters
void Operation::setType(dcOperationType type) {
	this->_type = type;
}

dcOperationType Operation::getType() const {
	return this->_type;
}

void Operation::setCompleted(bool completed) {
	this->_completed = completed;
}

bool Operation::getCompleted() const {
	return this->_completed;
}

void Operation::setTarget(const std::string &target) {
	this->_target = target;
}

const std::string &Operation::getTarget() const{
	return this->_target;
}

}
