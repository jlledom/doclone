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

#include <CursesView/InsertIpForm.h>

#include <doclone/Clone.h>

#include <CursesView/CursesView.h>
#include <CursesView/ScreenFactory.h>

InsertIpForm::InsertIpForm() {
	// Refresh the text at the top bar
	this->_topBarNotice=_("Select the option and press [Enter] or push <Continue>.");
	this->_topBarNotice.append(" ");
	this->_topBarNotice.append(_("Press [Tab] to move."));

	std::string notice=_("Enter the IP address of the server:");

	mvwprintw(this->_win, 2, 2, notice.c_str());

	this->_menuBack->post();
	this->_menuContinue->post();

	this->_form = new TextInput();
	this->_form->setWindow(this->_win);

	std::string formDescription=_("Type an IP address.");
	formDescription.append(" ");
	formDescription.append(_("Red background means wrong format."));
	this->_form->setDescription(formDescription);

	this->_form->setColors(
			COLOR_PAIR(CursesView::MENU_BKGD),
			COLOR_PAIR(CursesView::NOT_VALID_FIELD));
	this->_form->setType(T_IPV4);
	this->_form->post();

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
int InsertIpForm::getNextScreen() {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = ScreenFactory::NONE;
	std::string ipAddr(this->_form->getValue());

	if((*this->_it) == this->_menuBack) {
		retVal = ScreenFactory::BACK;
	} else {
		dcl->setAddress(ipAddr);

		retVal = ScreenFactory::TARGETMENU;
	}

	return retVal;
}

InsertIpForm::~InsertIpForm() {
	delete this->_form;
}
