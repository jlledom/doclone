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

#include <CursesView/SelectDeviceForm.h>
#include <CursesView/CursesView.h>
#include <CursesView/ScreenFactory.h>
#include <CursesView/Util.h>

#include <doclone/Clone.h>

#include <sys/types.h>
#include <regex.h>
#include <libintl.h>
#include <curses.h>
#include <string.h>

#include <fstream>

SelectDeviceForm::SelectDeviceForm() {
	this->detectDevices();

	// Refresh the text at the top bar
	this->_topBarNotice=_("Select the option and press [Enter] or push <Continue>.");
	this->_topBarNotice.append(" ");
	this->_topBarNotice.append(_("Press [Tab] to move."));

	std::string notice=_("Select the device:");

	mvwprintw(this->_win, 2, 2, notice.c_str());

	std::string notice2=_("Or type it manually:");

	mvwprintw(this->_win, BaseScreen::WIN_HEIGHT-6, 2, notice2.c_str());

	this->_menu = new Menu();
	this->_menu->setWindow(this->_win);
	this->_menu->setContent(this->_deviceList, this->_descriptionList);
	this->_menu->setColors(
				COLOR_PAIR(CursesView::MENU_BKGD),
				COLOR_PAIR(CursesView::SELECTED_OPTION),
				COLOR_PAIR(CursesView::UNSELECTABLE_OPTION));
	this->_menu->setMargin(true);
	this->_menu->setMaxY(9);
	this->_menu->setPosY(4);
	this->_menu->post();

	this->_menuBack->post();
	this->_menuContinue->post();

	this->_form = new TextInput();
	this->_form->setWindow(this->_win);
	this->_form->setPosY(BaseScreen::WIN_HEIGHT-4);

	std::string formDescription=_("Please, type the path of the device");
	this->_form->setDescription(formDescription);

	this->_form->setColors(
			COLOR_PAIR(CursesView::MENU_BKGD),
			COLOR_PAIR(CursesView::NOT_VALID_FIELD));
	this->_form->setNullable(true);
	this->_form->post();

	this->_listWidgets.push_back(this->_menu);
	this->_listWidgets.push_back(this->_form);
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
int SelectDeviceForm::getNextScreen() {
	CursesView *curView = CursesView::getInstance();
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = ScreenFactory::NONE;

	std::string inputVal(this->_form->getValue());
	std::string devicePath(this->_menu->getName());

	if(*this->_it == this->_menuBack) {
		retVal = ScreenFactory::BACK;
	} else {
		if(*this->_it == this->_menu) {
			dcl->setDevice(devicePath);
		} else if(inputVal.empty()) {
			dcl->setDevice(devicePath);
		} else {
			dcl->setDevice(inputVal);
		}

		dcCursesMode mode = curView->getMode();

		switch(mode) {
		case CURSES_MODE_LOCAL: {
			dcCursesRole role = curView->getRole();

			if(role == CURSES_ROLE_READ) {
				retVal = ScreenFactory::INSERTFILE;
			} else if(role == CURSES_ROLE_WRITE) {
				retVal = ScreenFactory::SUMMARY;
			}

			break;
		}
		case CURSES_MODE_UNICAST:
		case CURSES_MODE_LINK: {
			retVal = ScreenFactory::SUMMARY;
			break;
		}
		default: {
			break;
		}
		}
	}

	return retVal;
}

/**
 * Reads /proc/partitions to fill the list of partitions
 */
void SelectDeviceForm::detectDevices() {
	std::ifstream proc_partitions( "/proc/partitions" ) ;

	if ( proc_partitions ) {
		std::string line ;

		while ( getline( proc_partitions, line ) ) {
			std::string label("/dev/");
			std::string regExp(DEV_REGEX);
			if (Util::match(line, regExp)) {
				char nompt[100] = {};
				int maj, min, tam;
				sscanf (line.c_str(), " %d %d %d %[^\n ]", &maj, &min, &tam, nompt);
				label.append(nompt);

				this->_deviceList.push_back(label);

				// TO TRANSLATORS: Looks like	Select /dev/sda1
				std::string description(_("Select"));
				description.append(" ").append(label);
				this->_descriptionList.push_back(description);
			}
		}
	}
}

SelectDeviceForm::~SelectDeviceForm() {
	delete this->_form;
	delete this->_menu;
}
