/*
 * doclone - a frontend for libdoclone
 * Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONSOLEVIEW_H_
#define CONSOLEVIEW_H_

#include <stdint.h>

#include <string>

#include <doclone/Clone.h>

/**
 * \enum dcConsoleFunction
 * \brief Set of functions of doclone for console view
 *
 * \var CONSOLE_NONE
 * 	No function selected
 * \var CONSOLE_CREATE
 * 	Create image
 * \var CONSOLE_RESTORE
 * 	Restore Image
 * \var CONSOLE_LINK_SEND
 * 	Sender in link mode
 * \var CONSOLE_LINK_RECEIVE
 * 	Receiver in link mode
 * \var CONSOLE_SEND
 * 	Server in multicast mode
 * \var CONSOLE_RECEIVE
 * 	Cliente in multicast mode
 */
enum dcConsoleFunction {
	CONSOLE_NONE,
	CONSOLE_CREATE,
	CONSOLE_RESTORE,
	CONSOLE_LINK_SEND,
	CONSOLE_LINK_RECEIVE,
	CONSOLE_SEND,
	CONSOLE_RECEIVE
};

/**
 * \class ConsoleView
 *
 * This class contains all the methods needed for implement a console view:
 * reads parameters from bash, listens the library events and shows messages in
 * stdout or stderr.
 *
 * doclone is a frontend for libdoclone that has two views: the console view,
 * implemented in this class and the ncurses view, implemented in many screens,
 * with one class by screen.
 *
 * \date October, 2011
 */
class ConsoleView : public Doclone::AbstractObserver {
public:
	ConsoleView();

	void notify(Doclone::dcTransferEvent event, uint64_t numBytes);
	void notify(Doclone::dcOperationEvent event,
			Doclone::dcOperationType type, const std::string &target);
	void notify(Doclone::dcEvent event, const std::string &target);
	void notify(const std::string &str);

	void initView(int argc, char **argv) const;

private:
	void version() const;
	void usage (FILE * stream, int code, const char * cmd) const;

	/// Total amount of bytes to be transferred (for calculate the percentage)
	uint64_t _totalSize;

	/// Percentage of data transferred at the moment
	int _percent;
};

#endif /* CONSOLEVIEW_H_ */
