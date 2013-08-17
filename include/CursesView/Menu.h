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

#ifndef MENU_H_
#define MENU_H_

#include <CursesView/CursesWidget.h>

#include <ncurses.h>
#include <menu.h>

#include <string>
#include <vector>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class Menu
 *
 * C++ wrapper to simplify the use of the MENU widget of ncurses.
 *
 * \date October, 2011
 */
class Menu : public CursesWidget {
public:
	Menu();

	void focus() const;
	void blur() const;
	void doAction(int c) const;
	void post();
	void unpost() const;

	int getIndex() const;
	const char *getName() const;
	const char *getDescription() const;

	void setContent(const char * const *options, const char * const *descriptions);
	void setContent(const std::vector<std::string> &options,
			const std::vector<std::string> &descriptions);
	void setColors(chtype background, chtype foreground, chtype disabled);

	void setWindow(WINDOW * win);
	void setPosX(int posX);
	void setPosY(int posY);
	void setMaxX(int maxX);
	void setMaxY(int maxY);
	void setWidth(int width);
	void setHeight(int height);
	void setMargin(bool margin);

	~Menu();

protected:
	void deleteMenu();
	void deleteContent();
	void deleteWindows();

	/// The Menu object
	MENU *_menu;

	/// The window on the menu will be showed
	WINDOW *_win;
	/// The man window of the menu
	WINDOW *_menuWin;
	/// A sub-window for set a margin around the menu options
	WINDOW *_menuSubWin;
	/// Color pair of the unselected options
	chtype _colorBack;
	/// Color pair of the selected options
	chtype _colorFore;
	/// Color pair of the disabled options
	chtype _colorDisabled;

	/// Array of options in the menu
	char **_options;
	/// Array of the descriptions of the menu. One by each option.
	char **_descriptions;

	/// X coordinate inside the window pointed by this->_win
	int _posX;
	/// Y coordinate inside the window pointed by this->_win
	int _posY;
	/// Max width of the menu
	int _maxX;
	/// Max height of the menu
	int _maxY;
	/// Width of the menu
	int _width;
	/// Height of the menu
	int _height;
	/// Margin of the menu
	bool _margin;
};
/**@}*/

#endif /* MENU_H_ */
