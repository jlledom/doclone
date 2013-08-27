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

#include <doclone/Logger.h>

#include <stdarg.h>
#include <pthread.h>

#include <config.h>

#include <log4cpp/BasicLayout.hh>

namespace Doclone {

/**
 * \brief Initializes the logger, the priority of the messages is established
 * in the flags of compilation.
 */
Logger::Logger() : _app(0), _layout(0), _cat(0), _backTrace() {
#ifdef LOGDIR
	std::string log_file(LOGDIR);
	log_file.append("/libdoclone.log");

	this->_app = new log4cpp::RollingFileAppender("RollingFileAppender",
			log_file.c_str(), 500 * 1024, 5);
#else
	this->_app = new log4cpp::FileAppender("_", ::dup(fileno(stderr)));
#endif

	this->_layout =  new log4cpp::PatternLayout();
#if (defined DEBUG_LOG) || (defined DEBUG_LOOP)
	this->_layout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S,%l} %p %c %x: %m%n");
#else
	this->_layout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S} %p %c %x: %m%n");
#endif
	this->_app->setLayout(this->_layout);

	this->_cat = &log4cpp::Category::getInstance("libdoclone");
	this->_cat->setAdditivity(false);
	
	// log4cpp::Priority::INFO for release version,
	// log4cpp::Priority::DEBUG to debug,
	// 750 for log functions called by loops
#ifdef DEBUG_LOG
	this->_cat->setPriority(log4cpp::Priority::DEBUG);
#elif DEBUG_LOOP
	this->_cat->setPriority(750);
#else
	this->_cat->setPriority(log4cpp::Priority::INFO);
#endif

	this->_cat->setAppender(this->_app);
}


/**
 * \brief Singleton stuff
 *
 * \return Logger* object
 */
Logger* Logger::getInstance() {
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mutex);

	static Logger instance;

	pthread_mutex_unlock(&mutex);

	return &instance;
}

/**
 * \brief Traces a message with loopDebug priority
 *
 * This function can receive an undefined number of parameters in order to build
 * a message string using the printf() syntax.
 *
 * \param msg
 * 		The message what will be traced.
 * \param ...
 * 		Possible parameters to build the message string, like in printf().
 */
void Logger::loopDebug(const std::string &msg, ...) const {
	char tmpStr[1024];
	
	va_list vl;
	va_start(vl, msg);
	vsnprintf (tmpStr, sizeof(tmpStr), msg.c_str(), vl);
	va_end (vl);
	
	std::string str = tmpStr;
	this->_cat->log(750, str);
}

/**
 * \brief Traces a message with loopDebug priority
 *
 * This function can receive an undefined number of parameters in order to build
 * a message string using the printf() syntax.
 *
 * \param msg
 * 		The message what will be traced.
 * \param ...
 * 		Possible parameters to build the message string, like in printf().
 */
void Logger::debug(const std::string &msg, ...) const {
	char tmpStr[1024];
	
	va_list vl;
	va_start(vl, msg);
	vsnprintf (tmpStr, sizeof(tmpStr), msg.c_str(), vl);
	va_end (vl);
	
	std::string str = tmpStr;
	this->_cat->debug(str);
}

/**
 * \brief Traces a message with info priority
 *
 * This function can receive an undefined number of parameters in order to build
 * a message string using the printf() syntax.
 *
 * \param msg
 * 		The message what will be traced.
 * \param ...
 * 		Possible parameters to build the message string, like in printf().
 */
void Logger::info(const std::string &msg, ...) const {
	char tmpStr[1024];
	
	va_list vl;
	va_start(vl, msg);
	vsnprintf (tmpStr, sizeof(tmpStr), msg.c_str(), vl);
	va_end (vl);
	
	std::string str = tmpStr;
	this->_cat->info(str);
}

/**
 * \brief Traces a message with warning priority
 *
 * This function can receive an undefined number of parameters in order to build
 * a message string using the printf() syntax.
 *
 * \param msg
 * 		The message what will be traced.
 * \param ...
 * 		Possible parameters to build the message string, like in printf().
 */
void Logger::warn(const std::string &msg, ...) {
	char tmpStr[1024];
	
	va_list vl;
	va_start(vl, msg);
	vsnprintf (tmpStr, sizeof(tmpStr), msg.c_str(), vl);
	va_end (vl);
	
	// Append to log
	std::string str = tmpStr;
	this->_cat->warn(str);

	// Notify the views
	this->notifyObservers(str);

	// Add to backtrace
	this->pushBT(str);
}

/**
 * \brief Traces a message with error priority
 *
 * This function can receive an undefined number of parameters in order to build
 * a message string using the printf() syntax.
 *
 * \param msg
 * 		The message what will be traced.
 * \param ...
 * 		Possible parameters to build the message string, like in printf().
 */
void Logger::error(const std::string &msg, ...) {
	char tmpStr[1024];
	
	va_list vl;
	va_start(vl, msg);
	vsnprintf (tmpStr, sizeof(tmpStr), msg.c_str(), vl);
	va_end (vl);
	
	// Append to log
	std::string str = tmpStr;
	this->_cat->error(str);

	// Notify the views
	this->notifyObservers(str);

	// Add to backtrace
	this->pushBT(str);
}

/**
 * \brief Traces a message with fatal priority
 *
 * This function can receive an undefined number of parameters in order to build
 * a message string using the printf() syntax.
 *
 * \param msg
 * 		The message what will be traced.
 * \param ...
 * 		Possible parameters to build the message string, like in printf().
 */
void Logger::fatal(const std::string &msg, ...) {
	char tmpStr[1024];
	
	va_list vl;
	va_start(vl, msg);
	vsnprintf (tmpStr, sizeof(tmpStr), msg.c_str(), vl);
	va_end (vl);
	
	// Append to log
	std::string str = tmpStr;
	this->_cat->fatal(str);

	// Notify the views
	this->notifyObservers(str);

	// Add to backtrace
	this->pushBT(str);
}

/**
 * \brief Adds a new message to the backtrace.
 *
 * \param msg
 * 		The message string.
 */
void Logger::pushBT(const std::string &msg) {
	this->_backTrace.push_back(msg);
}

std::vector<std::string> &Logger::getBT() {
	return this->_backTrace;
}

}
