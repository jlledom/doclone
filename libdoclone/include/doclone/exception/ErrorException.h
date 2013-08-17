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

#ifndef ERROREXCEPTION_H_
#define ERROREXCEPTION_H_

#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \addtogroup Exceptions
 * @{
 *
 * \class ErrorException
 * \brief Error that stops the program.
 *
 * This kind of errors are serious and prevent to continue the program
 * \date August, 2011
 */
class ErrorException : public Exception {
public:
	ErrorException() throw() {
		this->_msg=D_("Executed with errors");
	}

	/**
	 * Logs the message of the exception, with error level
	 *
	 * \param srcFile
	 * 		The name of the source file where the exception has occurs
	 * \param line
	 * 		The line of the source file on exception was thrown
	 * \param function
	 * 		The prototype of the function where the exception was thrown
	 */
	void __logMsg(const char *srcFile, int line, const char *function) const throw() {
		std::stringstream message;
		message
		// In debug mode, also add the file and line where error occurs.
#if defined(DEBUG_LOG) || defined(DEBUG_LOOP)
		<< srcFile << ":" << line << " - " << function << ": "
#endif
		<< this->_msg;

		// Log the error
		Logger *log = Logger::getInstance();
		log->error(message.str());
	}
};
/**@}*/

}

#endif /* ERROREXCEPTION_H_ */
