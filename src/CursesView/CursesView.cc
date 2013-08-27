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

#include <CursesView/CursesView.h>

#include <libintl.h>

#include <string>
#include <vector>

#include <CursesView/ScreenFactory.h>

/**
 * \brief Singleton stuff
 *
 * \return A pointer to a CursesView object
 */
CursesView* CursesView::getInstance() {
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mutex);

	static CursesView instance;

	pthread_mutex_unlock(&mutex);

	return &instance;
}

/**
 * \brief Initializes ncurses.
 */
CursesView::CursesView(): _mode(), _role(), _target() {
	initscr();

	nonl();
	cbreak();
	noecho();
	keypad(stdscr, true);

	if (has_colors()) {
		start_color();

		init_pair(CursesView::WIN_BKGD, COLOR_BLACK, COLOR_WHITE);
		init_pair(CursesView::MENU_BKGD, COLOR_WHITE, COLOR_BLUE);
		init_pair(CursesView::SELECTED_OPTION, COLOR_WHITE, COLOR_RED);
		init_pair(CursesView::NAVIGATION_SELECTED_OPTION, COLOR_WHITE, COLOR_RED);
		init_pair(CursesView::UNSELECTABLE_OPTION, COLOR_WHITE, COLOR_RED);
		init_pair(CursesView::NAVIGATION_UNSELECTABLE_OPTION, COLOR_BLACK, COLOR_WHITE);
		init_pair(CursesView::PANEL_UNSELECTABLE_OPTION, COLOR_WHITE, COLOR_YELLOW);
		init_pair(CursesView::NOT_VALID_FIELD, COLOR_YELLOW, COLOR_RED);
		init_pair(CursesView::PROGRESS_BAR, COLOR_RED, COLOR_RED);

		assume_default_colors(COLOR_BLUE, COLOR_BLUE);
	}

	this->initScreen();
}

/**
 * \brief Draws an empty screen.
 */
void CursesView::initScreen() {
	int maxY;
	int maxX;
	getmaxyx(stdscr, maxY, maxX);

	this->_topBar = subwin(stdscr, 1, maxX, 0, 0);
	wbkgd(this->_topBar, COLOR_PAIR(CursesView::WIN_BKGD));

	this->_bottomBar = subwin(stdscr, 1, maxX, maxY-1, 0);
	wbkgd(this->_bottomBar, COLOR_PAIR(CursesView::WIN_BKGD));

	// Refresh the text at the top bar
	std::string topBarNotice=_("Select the option and press [Enter] or push <Continue>.");
	topBarNotice.append(" ");
	topBarNotice.append(_("Press [Tab] to move."));
	mvwprintw(this->_topBar,0, 0, topBarNotice.c_str());

	wrefresh(this->_topBar);
	wrefresh(this->_bottomBar);

	refresh();
}

/**
 * \brief Writes a new text in the top bar.
 *
 * \param text
 * 		The text to write
 */
void CursesView::updateTopBar(const std::string &text) const {
	wclear(this->_topBar);

	mvwprintw(this->_topBar,0, 0, text.c_str());
	wrefresh(this->_topBar);
}

/**
 * \brief Writes a new text in the bottom bar.
 *
 * \param text
 * 		The text to write
 */
void CursesView::updateBottomBar(const std::string &text) const {
	wclear(this->_bottomBar);

	mvwprintw(this->_bottomBar,0, 0, text.c_str());
	wrefresh(this->_bottomBar);
}

/**
 * \brief Creates, executes and destroys all the screens in the proper order
 */
void CursesView::flowControl() const {
	std::vector<BaseScreen*> screens;
	BaseScreen *currentScreen = ScreenFactory::createScreen(ScreenFactory::SCOPEMENU);
	screens.push_back(currentScreen);

	int screenCode = ScreenFactory::NONE;
	while((screenCode = currentScreen->run()) != ScreenFactory::NONE) {
		currentScreen->hide();

		if(screenCode == ScreenFactory::BACK) {
			BaseScreen *elem = screens.back();

			delete elem;
			screens.pop_back();

			currentScreen = screens.back();
		} else {
			currentScreen = ScreenFactory::createScreen(screenCode);
			screens.push_back(currentScreen);
		}
	}

	std::vector<BaseScreen*>::iterator it;
	for(it = screens.begin(); it != screens.end(); ++it) {
		delete (*it);
	}
}

dcCursesMode CursesView::getMode() const {
    return this->_mode;
}

void CursesView::setMode(const dcCursesMode mode) {
	this->_mode = mode;
}

dcCursesRole CursesView::getRole() const {
	return this->_role;
}

void CursesView::setRole(const dcCursesRole role) {
	this->_role = role;
}

dcCursesTarget CursesView::getTarget() const {
	return this->_target;
}

void CursesView::setTarget(const dcCursesTarget target) {
	this->_target = target;
}

CursesView::~CursesView() {
	wclear(this->_topBar);
	delwin(this->_topBar);

	wclear(this->_bottomBar);
	delwin(this->_bottomBar);

	endwin();
}
