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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <libintl.h>

#include <string>
#include <exception>

#include <doclone/Logger.h>

#ifndef GETTEXT_DOMAIN
#define GETTEXT_DOMAIN "libdoclone"
#endif

/**
 * \def D_(string)
 *
 * Gettext stuff.
 */
#define D_(string) dgettext(GETTEXT_DOMAIN, string)

/**
 * \def logMsg
 *
 * The information of the source code where the exception was thrown is passed
 * automatically.
 */
#define logMsg() __logMsg (__FILE__, __LINE__, __PRETTY_FUNCTION__)

namespace Doclone {

/**
 * \defgroup Exceptions Exceptions
 * \brief List of exceptions.
 *
 * This library handles some potential problems that may arise during execution.
 * These problems can be mild or severe, mild problems extend WarningException,
 * and the severe problem extend ErrorException class.
 *
 * The serious problems stop execution. The light errors can be controlled
 * and the execution can continue.
 *
 * The program has a backtrace, located in the class Logger, recording all
 * errors that occurred.
 *
 * It allows the library user to decide what to do with error messages.
 * Although in any case, messages always be notified to the views when they
 * occur.
 * @{
 *
 * \class Exception
 * \brief General exception class
 * \date August, 2011
 */
class Exception : public std::exception {
public:
	Exception() throw() : _msg(D_("General error")) {}

	/**
	 * Logs the message of the exception, with fatal level.
	 * Declared as virtual, for be re-implemented in the inherited classes.
	 *
	 * \param srcFile
	 * 		The name of the source file where the exception has occurs
	 * \param line
	 * 		The line of the source file on exception was thrown
	 * \param function
	 * 		The prototype of the function where the exception was thrown
	 */
	virtual void __logMsg(const char *srcFile, int line, const char *function)
		const throw() {
		std::stringstream message;
		message
		// In debug mode, also add the file and line where error occurs.
#if defined(DEBUG_LOG) || defined(DEBUG_LOOP)
		<< srcFile << ":" << line << " - " << function << ": "
#endif
		<< this->_msg;

		// Log the error
		Logger *log = Logger::getInstance();
		log->fatal(message.str());
	}

	virtual ~Exception() throw(){}

	/**
	 * Returns the log message.
	 */
	const char *what() const throw() {
		return this->_msg.c_str();
	}

protected:
	std::string _msg;
};
/**@}*/

}

#endif /* EXCEPTION_H_ */
