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

#include <CursesView/SummaryScreen.h>

#include <sstream>

#include <doclone/Clone.h>

#include <CursesView/CursesView.h>
#include <CursesView/ScreenFactory.h>

SummaryScreen::SummaryScreen() {
	CursesView *curView = CursesView::getInstance();
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	std::string notice(_("Summary"));

	mvwprintw(this->_win, 2, 2, notice.c_str());

	std::string action;
	// TO TRANSLATORS: Looks like	From: /dev/sda
	std::string from(_("From:"));

	// TO TRANSLATORS: Looks like	To: /dev/sda
	std::string to(_("To:"));

	dcCursesMode mode = curView->getMode();
	dcCursesRole role = curView->getRole();
	dcCursesTarget target = curView->getTarget();

	switch(mode) {
	case CURSES_MODE_LOCAL: {
		if(role == CURSES_ROLE_READ) {
			action=_("Create image");

			from.append(" ");
			from.append(dcl->getDevice());

			to.append(" ");
			to.append(dcl->getImage());

			mvwprintw(this->_win, 6, 2, from.c_str());
			mvwprintw(this->_win, 7, 2, to.c_str());
		} else if(role == CURSES_ROLE_WRITE) {
			action=_("Restore image");

			from.append(" ");
			from.append(dcl->getImage());

			to.append(" ");
			to.append(dcl->getDevice());

			mvwprintw(this->_win, 6, 2, from.c_str());
			mvwprintw(this->_win, 7, 2, to.c_str());
		}

		break;
	}
	case CURSES_MODE_UNICAST: {
		std::string strTarget = target == CURSES_TARGET_DEVICE?
				dcl->getDevice():dcl->getImage();

		if(role == CURSES_ROLE_READ) {
			action=_("Send to client/s");

			from.append(" ");
			from.append(strTarget);

			std::stringstream strNum;
			dcl->getNodesNumber() ?
					strNum << dcl->getNodesNumber() :
					strNum << 1;

			// TO TRANSLATORS: Looks like	Number of receivers: 8
			std::string nor(_("Number of receivers:"));
			nor.append(" ");
			nor.append(strNum.str());

			mvwprintw(this->_win, 6, 2, from.c_str());
			mvwprintw(this->_win, 7, 2, nor.c_str());
		} else if(role == CURSES_ROLE_WRITE) {
			action=_("Receive from server");

			to.append(" ");
			to.append(strTarget);

			// TO TRANSLATORS: Looks like	Server's IP: 192.168.0.1
			std::string ipAddr(_("Server's IP:"));
			ipAddr.append(" ");
			ipAddr.append(dcl->getAddress());

			mvwprintw(this->_win, 6, 2, to.c_str());
			mvwprintw(this->_win, 7, 2, ipAddr.c_str());
		}

		break;
	}
	case CURSES_MODE_LINK: {
		std::string strTarget = target == CURSES_TARGET_DEVICE?
				dcl->getDevice():dcl->getImage();

		if(role == CURSES_ROLE_READ) {
			action=_("Send to chain");

			from.append(" ");
			from.append(strTarget);

			mvwprintw(this->_win, 6, 2, from.c_str());
		} else if(role == CURSES_ROLE_WRITE) {
			action=_("Receive from chain");

			to.append(" ");
			to.append(strTarget);

			mvwprintw(this->_win, 6, 2, to.c_str());
		}

		break;
	}
	default: {
		break;
	}
	}

	mvwprintw(this->_win, 4, 2, action.c_str());

	this->_menuBack->post();
	this->_menuContinue->post();

	this->_listWidgets.push_back(this->_menuBack);
	this->_listWidgets.push_back(this->_menuContinue);

	this->_it = this->_listWidgets.begin();

	wrefresh(this->_win);
}

/**
 * Reads the state of the widgets and determines what will be the next screen
 *
 * \return The code of the next screen
 */
int SummaryScreen::getNextScreen() {
	int retVal = ScreenFactory::NONE;

	if(*this->_it == this->_menuBack) {
		retVal = ScreenFactory::BACK;
	} else {
		retVal = ScreenFactory::PROGRESS;
	}

	return retVal;
}
