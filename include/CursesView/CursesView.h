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

#ifndef CURSESVIEW_H_
#define CURSESVIEW_H_

#include <pthread.h>

#include <string>
#include <vector>

#include <curses.h>
#include <menu.h>

#define GN_EXTENSION "doclone"

/**
 * \enum dcCursesMode
 * \brief Set of modes of doclone for ncurses view. This enumerator is used to
 * determine what screen and in what order will be shown.
 *
 * \var CURSES_MODE_NONE
 * 	No mode selected
 * \var CURSES_MODE_LOCAL
 * 	Local mode
 * \var CURSES_MODE_LINK
 * 	Network work in link mode
 * \var CURSES_MODE_UNICAST
 * 	Network work in unicast/multicast mode
 */
enum dcCursesMode {
	CURSES_MODE_NONE,
	CURSES_MODE_LOCAL,
	CURSES_MODE_LINK,
	CURSES_MODE_UNICAST,
};

/**
 * \enum dcCursesRole
 * \brief Set of roles of doclone for ncurses view. This enumerator is used to
 * determine what screen and in what order will be shown.
 *
 * \var CURSES_ROLE_NONE
 * 	No role selected
 * \var CURSES_ROLE_READ
 * 	Read from disk to image/network
 * \var CURSES_ROLE_WRITE
 * 	Write from image/network to disk
 */
enum dcCursesRole {
	CURSES_ROLE_NONE,
	CURSES_ROLE_READ,
	CURSES_ROLE_WRITE
};

/**
 * \enum dcCursesTarget
 * \brief Set of targets of doclone for ncurses view. This enumerator is used to
 * determine what screen and in what order will be shown.
 *
 * \var CURSES_TARGET_NONE
 * 	No target selected
 * \var CURSES_TARGET_DEVICE
 * 	The target on read/write is a device
 * \var CURSES_TARGET_FILE
 * 	The target on read/write is a file
 */
enum dcCursesTarget {
	CURSES_TARGET_NONE,
	CURSES_TARGET_DEVICE,
	CURSES_TARGET_FILE
};

/**
 * \defgroup CursesView CursesView
 * \brief Classes of the ncurses view.
 *
 * The curses view is organized in several screens, which are presented to the
 * user dynamically in the correct order according to the actions the user is
 * performing.
 *
 * The function CursesView::flowControl() is able to direct the creation and
 * presentation of displays, basing on the value returned by the method
 * getNextString() of each screen.
 *
 * All screens derive from the same base class BaseScreen, which declares a
 * method getNextScreen() that returns the code of the next screen. Each screen
 * runs its implementation of getNextScreen() when complete its work and returns
 * the code of the next screen based in the actions the user has made​​.
 *
 * There are also two types of widgets to build forms. The menu and text input.
 * These two widgets are implemented in the classes Menu and TextInput
 * respectively, which are specializations of the abstract class CursesWidget.
 * @{
 */

/**
 * \class CursesView
 *
 * This class initializes ncurses and controls the flow of the different screens
 * of the view. Singleton class.
 *
 * doclone is a frontend for libdoclone that has two views: the console view,
 * implemented in ConsoleView and the ncurses view, implemented in many screens,
 * with one class by screen.
 *
 * \date October, 2011
 */
class CursesView {
public:
	static const unsigned short WIN_BKGD = 1;
	static const unsigned short MENU_BKGD = 2;
	static const unsigned short SELECTED_OPTION = 3;
	static const unsigned short NAVIGATION_SELECTED_OPTION = 4;
	static const unsigned short UNSELECTABLE_OPTION = 5;
	static const unsigned short NAVIGATION_UNSELECTABLE_OPTION = 6;
	static const unsigned short PANEL_UNSELECTABLE_OPTION = 7;
	static const unsigned short NOT_VALID_FIELD = 8;
	static const unsigned short PROGRESS_BAR = 9;

	static CursesView *getInstance();
	virtual ~CursesView();

	void flowControl() const;

	void updateTopBar(const std::string &text) const;
	void updateBottomBar(const std::string &text) const;

    dcCursesMode getMode() const;
    void setMode(dcCursesMode mode);

    dcCursesRole getRole() const;
    void setRole(dcCursesRole role);

    dcCursesTarget getTarget() const;
	void setTarget(dcCursesTarget target);

protected:
	CursesView();

	void initScreen();

	/// The ncurses window used to simulate a top bar
	WINDOW *_topBar;
	/// The ncurses window used to simulate a bottom bar
	WINDOW *_bottomBar;

	/// Selected mode
	dcCursesMode _mode;
	/// Selected role
	dcCursesRole _role;
	/// Selected target
	dcCursesTarget _target;
};
/**@}*/

#endif /* CURSESVIEW_H_ */
