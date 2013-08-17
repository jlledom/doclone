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

#ifndef BASESCREEN_H_
#define BASESCREEN_H_

#include <CursesView/CursesWidget.h>
#include <CursesView/Menu.h>

#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#include <vector>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class BaseScreen
 *
 * Abstract class with all common methods for all screens.
 *
 * \date October, 2011
 */
class BaseScreen {
public:
	static const unsigned short WIN_HEIGHT = 20;
	static const unsigned short WIN_WIDTH = 74;

	BaseScreen();
	virtual ~BaseScreen();

	virtual int run();
	void hide() const;

protected:
	virtual void readKeyboard();
	void show() const;

	/**
	 * Read the state of the forms of the screen to determine what the user
	 * wants.
	 *
	 * \return The code of the next screen to be built.
	 * Codes are in ScreenFactory class.
	 */
	virtual int getNextScreen()=0;

	void initMenuBack();
	void initMenuContinue();
	void initMenuCancel();
	void initMenuClose();

	/// List of widgets of the screen
	std::vector<CursesWidget*> _listWidgets;

	/// Iterator that indicates which widget has the focus
	std::vector<CursesWidget*>::iterator _it;

	/// The window where this window will be shown
	WINDOW *_win;
	/// The panel containing the widgets of this screen
	PANEL *_panel;

	/// Button <Back>. A button is a menu with one element.
	Menu *_menuBack;
	/// Button <Continue>. A button is a menu with one element.
	Menu *_menuContinue;
	/// Button <Cancel>. A button is a menu with one element.
	Menu *_menuCancel;
	/// Button <Close>. A button is a menu with one element.
	Menu *_menuClose;

	/// The text of the top bar of this screen
	std::string _topBarNotice;
};
/**@}*/

#endif /* BASESCREEN_H_ */
