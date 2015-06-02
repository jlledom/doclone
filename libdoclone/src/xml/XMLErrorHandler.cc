/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2015 Joan Lledó <joanlluislledo@gmail.com>
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

#include <doclone/xml/XMLErrorHandler.h>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>

#include <doclone/Logger.h>

namespace Doclone {

XERCES_CPP_NAMESPACE_USE

/**
 * \brief Initializes attributes
 */
XMLErrorHandler::XMLErrorHandler() : _errors() {
}

bool XMLErrorHandler::handleError(const DOMError &domError) {
	Logger *log = Logger::getInstance();
	bool retVal = true;

	//Severity level
	DOMError::ErrorSeverity severity = domError.getSeverity();
	uint64_t lineNum = domError.getLocation()->getLineNumber();
	uint64_t colNum = domError.getLocation()->getColumnNumber();

	//Error message
	char *msg = XMLString::transcode(domError.getMessage());

	if(severity == DOMError::DOM_SEVERITY_WARNING) {
		log->warn("WARNING: At line %d, column: %d: %s", lineNum, colNum, msg);
		retVal = true;
	}

	if(severity == DOMError::DOM_SEVERITY_ERROR) {
		log->error("ERROR: At line %d, column: %d: %s", lineNum, colNum, msg);
		retVal = false;
		this->_errors = true;
	}

	if(severity == DOMError::DOM_SEVERITY_FATAL_ERROR) {
		log->fatal("FATAL: At line %d, column: %d: %s", lineNum, colNum, msg);
		retVal = false;
		this->_errors = true;
	}

	XMLString::release(&msg);

	return retVal;
}

/**
 * \brief Disables the error status
 */
void XMLErrorHandler::resetErrors() {
	this->_errors = false;
}

/**
 * \brief Getter for the errors property
 */
bool XMLErrorHandler::getErrors() {
	return this->_errors;
}

}
