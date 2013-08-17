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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <log4cpp/Category.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>

#include <doclone/observer/AbstractSubject.h>

#include <string>
#include <vector>

namespace Doclone {

/**
 * \class Logger
 * \brief The logger class of the project. Singleton pattern.
 *
 * This class is used to write the traces of the execution of the program,
 * and showing the messages of the exceptions.
 *
 * It uses the library log4cpp for this purpose, and implements six levels of
 * messages:
 *
 * FATAL: Some function has made a big mistake. Currently unused.
 * ERROR: Some Error happened and the program cannot continue.
 * WARNING: Some Error happened but the program can control it and continue
 *  working.
 * INFO: Some useful info for the user. Currently unused.
 * DEBUG: Traces the thread of execution of the program, each function inform
 *  trough this level when it starts o ends. Only if the programmer compiles the
 *  code with the -DDEBUG_LOG flag.
 * LOOP DEBUG: Same as above, but there are some functions that are executed
 *  from a loop and can be called hundreds of times. This kind of messages
 *  really bother. For see this kind of messages, must compile with the
 *  -DDEBUG_LOOP flag.
 *
 * This class also implements a backtrace for the exceptions of the program. All
 * exceptions are written in a vector, and there is a function to get it.
 *
 * \date August, 2011
 */
class Logger : public AbstractSubject {
public:
	static Logger* getInstance();
	
	void loopDebug(const std::string &msg, ...) const;
	void debug(const std::string &msg, ...) const;
	void info(const std::string &msg, ...) const;
	void warn(const std::string &msg, ...);
	void error(const std::string &msg, ...);
	void fatal(const std::string &msg, ...);
	
	std::vector<std::string> &getBT();

private:
	Logger();
	void pushBT(const std::string &msg);
	
	// log4cpp stuff
	log4cpp::RollingFileAppender *_app;
	log4cpp::PatternLayout* _layout;
	log4cpp::Category *_cat; // I love cats

	/**
	 * The backtrace with all the messages of all the exceptions thrown during
	 * the execution
	 */
	std::vector<std::string> _backTrace;
};

}

#endif /* LOGGER_H_ */
