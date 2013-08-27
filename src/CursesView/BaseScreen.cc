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

#include <CursesView/BaseScreen.h>

#include <string.h>
#include <libintl.h>

#include <CursesView/CursesView.h>

/**
 * Initializes the base screen
 */
BaseScreen::BaseScreen() : _listWidgets(), _it(), _win(), _panel(), _menuBack(),
		_menuContinue(), _menuCancel(), _menuClose(), _topBarNotice() {
	this->_win = newwin(BaseScreen::WIN_HEIGHT, BaseScreen::WIN_WIDTH, 2, 3);
	wbkgd(this->_win, COLOR_PAIR(CursesView::WIN_BKGD));
	box(this->_win, 0, 0);

	// TO TRANSLATORS: This is the title of the window
	std::string title = std::string(" ").append(_("doclone")).append(" ");

	int titlePosition=(BaseScreen::WIN_WIDTH/2)-(title.length()/2);

	mvwprintw(this->_win, 0, titlePosition, title.c_str());

	this->_panel = new_panel(this->_win);

	update_panels();

	this->initMenuBack();
	this->initMenuContinue();
	this->initMenuCancel();
	this->initMenuClose();
}

/**
 * Shows the screen and reads the user events
 */
int BaseScreen::run() {
	(*this->_it)->focus();
	this->show();

	this->readKeyboard();

	int retVal = this->getNextScreen();

	return retVal;
}

/**
 * Shows the screen
 */
void BaseScreen::show() const {
	CursesView *curView=CursesView::getInstance();
	curView->updateTopBar(this->_topBarNotice);
	show_panel(this->_panel);
	update_panels();
	doupdate();
}

/**
 * Hides the screen
 */
void BaseScreen::hide() const {
	hide_panel(this->_panel);
	update_panels();
	doupdate();
}

/**
 * Reads the user events
 *
 * Some events like pressing the ENTER key or the TAB key are common to most
 * screens. So they are implemented here.
 *
 * If the user press any other key, the control is passed to the doAction()
 * method of the focused widget.
 *
 * Some screens with special needs overwrite this method.
 */
void BaseScreen::readKeyboard() {
	bool readKeyboard = true;

	do {
		int c = getch();

		switch(c) {
		case KEY_ENTER:
		case '\n':
		case '\r': {
			readKeyboard = false;
			break;
		}
		case '\t': {
			(*this->_it)->blur();

			this->_it++;

			if(this->_it == this->_listWidgets.end()) {
				this->_it = this->_listWidgets.begin();
			}

			(*this->_it)->focus();

			break;
		}
		default: {
			(*this->_it)->doAction(c);

			break;
		}
		}

		update_panels();
		doupdate();
	} while(readKeyboard);
}

/**
 * Initializes the <Back> button
 */
void BaseScreen::initMenuBack() {
	char *choices[] = {
			// TO TRANSLATORS: Button of the ncurses window. Please, do not delete the "<>"
			_("<Go back>"),
			0
	};
	char *descriptions[] = {
			_("Go to the previous screen"),
			0
	};

	this->_menuBack = new Menu();
	this->_menuBack->setWindow(this->_win);
	this->_menuBack->setContent(choices, descriptions);
	this->_menuBack->setColors(
				COLOR_PAIR(CursesView::WIN_BKGD),
				COLOR_PAIR(CursesView::NAVIGATION_SELECTED_OPTION),
				COLOR_PAIR(CursesView::NAVIGATION_UNSELECTABLE_OPTION));
	this->_menuBack->setMargin(false);
	this->_menuBack->setMaxY(1);
	this->_menuBack->setPosX(2);
	this->_menuBack->setPosY(BaseScreen::WIN_HEIGHT-2);

	this->_menuBack->blur();
}

/**
 * Initializes the <Continue> button
 */
void BaseScreen::initMenuContinue() {
	char *choices[] = {
			// TO TRANSLATORS: Button of the ncurses window. Please, do not delete the "<>"
			_("<Continue>"),
			0
	};
	char *descriptions[] = {
			_("Continue to the next screen"),
			0
	};

	this->_menuContinue = new Menu();
	this->_menuContinue->setWindow(this->_win);
	this->_menuContinue->setContent(choices, descriptions);
	this->_menuContinue->setColors(
					COLOR_PAIR(CursesView::WIN_BKGD),
					COLOR_PAIR(CursesView::NAVIGATION_SELECTED_OPTION),
					COLOR_PAIR(CursesView::NAVIGATION_UNSELECTABLE_OPTION));
	this->_menuContinue->setMargin(false);
	this->_menuContinue->setMaxY(1);
	this->_menuContinue->setPosX(BaseScreen::WIN_WIDTH-strlen(choices[0])-3);
	this->_menuContinue->setPosY(BaseScreen::WIN_HEIGHT-2);

	this->_menuContinue->blur();
}

/**
 * Initializes the <Cancel> button
 */
void BaseScreen::initMenuCancel() {
	char *choices[] = {
			// TO TRANSLATORS: Button of the ncurses window. Please, do not delete the "<>"
			_("<Cancel>"),
			0
	};
	char *descriptions[] = {
			_("Cancel the current operation"),
			0
	};

	this->_menuCancel = new Menu();
	this->_menuCancel->setWindow(this->_win);
	this->_menuCancel->setContent(choices, descriptions);
	this->_menuCancel->setColors(
					COLOR_PAIR(CursesView::WIN_BKGD),
					COLOR_PAIR(CursesView::NAVIGATION_SELECTED_OPTION),
					COLOR_PAIR(CursesView::NAVIGATION_UNSELECTABLE_OPTION));
	this->_menuCancel->setMargin(false);
	this->_menuCancel->setMaxY(1);
	this->_menuCancel->setPosX(BaseScreen::WIN_WIDTH-strlen(choices[0])-3);
	this->_menuCancel->setPosY(BaseScreen::WIN_HEIGHT-2);

	this->_menuCancel->blur();
}

/**
 * Initializes the <Close> button
 */
void BaseScreen::initMenuClose() {
	char *choices[] = {
			// TO TRANSLATORS: Button of the ncurses window. Please, do not delete the "<>"
			_("<Close>"),
			0
	};
	char *descriptions[] = {
			_("Close doclone"),
			0
	};

	this->_menuClose = new Menu();
	this->_menuClose->setWindow(this->_win);
	this->_menuClose->setContent(choices, descriptions);
	this->_menuClose->setColors(
					COLOR_PAIR(CursesView::WIN_BKGD),
					COLOR_PAIR(CursesView::NAVIGATION_SELECTED_OPTION),
					COLOR_PAIR(CursesView::NAVIGATION_UNSELECTABLE_OPTION));
	this->_menuClose->setMargin(false);
	this->_menuClose->setMaxY(1);
	this->_menuClose->setPosX(BaseScreen::WIN_WIDTH-strlen(choices[0])-3);
	this->_menuClose->setPosY(BaseScreen::WIN_HEIGHT-2);

	this->_menuClose->blur();
}

BaseScreen::~BaseScreen() {
	delete this->_menuBack;
	delete this->_menuContinue;
	delete this->_menuCancel;
	delete this->_menuClose;

	del_panel(this->_panel);

	wclear(this->_win);
	delwin(this->_win);
}

