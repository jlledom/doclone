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

#include <CursesView/LanRoleMenu.h>

#include <libintl.h>

#include <vector>
#include <string>

#include <curses.h>

#include <CursesView/CursesView.h>
#include <CursesView/ScreenFactory.h>

LanRoleMenu::LanRoleMenu() {
	std::string notice=_("Select the role in the connection:");

	mvwprintw(this->_win, 2, 2, notice.c_str());

	// Refresh the text at the top bar
	this->_topBarNotice=_("Select the option and press [Enter] or push <Continue>.");
	this->_topBarNotice.append(" ");
	this->_topBarNotice.append(_("Press [Tab] to move."));

	char *choices[] = {
			_("Server"),
			_("Client"),
			0
	};
	char *descriptions[] = {
			_("Send data to clients"),
			_("Receive data from server"),
			0
	};

	this->_menu = new Menu();
	this->_menu->setWindow(this->_win);
	this->_menu->setContent(choices, descriptions);
	this->_menu->setColors(
				COLOR_PAIR(CursesView::MENU_BKGD),
				COLOR_PAIR(CursesView::SELECTED_OPTION),
				COLOR_PAIR(CursesView::UNSELECTABLE_OPTION));
	this->_menu->setMargin(true);
	this->_menu->post();

	this->_menuBack->post();
	this->_menuContinue->post();

	this->_listWidgets.push_back(this->_menu);
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
int LanRoleMenu::getNextScreen() {
	CursesView *curView = CursesView::getInstance();

	int retVal = ScreenFactory::NONE;

	int index = this->_menu->getIndex();

	if(*this->_it == this->_menuBack) {
		retVal = ScreenFactory::BACK;
	} else {
		dcCursesMode mode = curView->getMode();
		dcCursesRole role = CURSES_ROLE_NONE;

		if(mode == CURSES_MODE_UNICAST) {
			switch(index) {
			case 0: {
				role = CURSES_ROLE_READ;
				curView->setRole(role);

				retVal = ScreenFactory::INSERTNUMBEROFCLIENTS;
				break;
			}
			case 1: {
				role = CURSES_ROLE_WRITE;
				curView->setRole(role);

				retVal = ScreenFactory::INSERTIP;
				break;
			}
			}
		} else if (mode == CURSES_MODE_LINK) {
			switch(index) {
			case 0: {
				role = CURSES_ROLE_READ;
				curView->setRole(role);

				break;
			}
			case 1: {
				role = CURSES_ROLE_WRITE;
				curView->setRole(role);

				break;
			}
			}

			retVal = ScreenFactory::TARGETMENU;
		}
	}

	return retVal;
}

LanRoleMenu::~LanRoleMenu() {
	delete this->_menu;
}
