/*
 * doclone - a frontend for libdoclone
 * Copyright (C) 2013, 2015 Joan Lledó <joanlluislledo@gmail.com>
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

#include <CursesView/Menu.h>

#include <string.h>

#include <CursesView/CursesView.h>
#include <CursesView/BaseScreen.h>

Menu::Menu() :
	_menu(), _win(), _menuWin(), _menuSubWin(), _colorBack(), _colorFore(),
	_colorDisabled(), _options(), _descriptions(), _posX(), _posY(),
	_maxX(BaseScreen::WIN_WIDTH-4), _maxY(BaseScreen::WIN_HEIGHT-4), _width(),
	_height(), _margin() {
}

/**
 * Color the selected option and write its description in the
 * bottom bar
 */
void Menu::focus() const {
	CursesView *curView = CursesView::getInstance();

	ITEM *selectedItem = current_item(this->_menu);
	item_opts_on(selectedItem, O_SELECTABLE);

	if(selectedItem != 0) {
		curView->updateBottomBar(selectedItem->description.str);
	} else {
		curView->updateBottomBar("");
	}
}

/**
 * Paints with the same color all the options. Simulating the loss of the focus
 *
 * bottom bar
 */
void Menu::blur() const {
	ITEM *selectedItem = current_item(this->_menu);
	item_opts_off(selectedItem, O_SELECTABLE);
}

/**
 * Evaluates the key pressed and changes the selected option, the scroll or both.
 *
 * \param c
 * 		Code for the key pressed
 */
void Menu::doAction(int c) const {
	CursesView *curView = CursesView::getInstance();

	// In a menu with no items, there's no need to do anything
	if(this->_menu->items == 0) {
		return;
	}

	switch(c) {
		case KEY_LEFT:  {
			menu_driver(this->_menu, REQ_LEFT_ITEM);

			break;
		}
		case KEY_RIGHT:  {
			menu_driver(this->_menu, REQ_RIGHT_ITEM);

			break;
		}
		case KEY_UP:  {
			menu_driver(this->_menu, REQ_UP_ITEM);

			break;
		}
		case KEY_DOWN:  {
			menu_driver(this->_menu, REQ_DOWN_ITEM);

			break;
		}
		case KEY_NPAGE: {
			menu_driver(this->_menu, REQ_SCR_DPAGE);

			break;
		}
		case KEY_PPAGE: {
			menu_driver(this->_menu, REQ_SCR_UPAGE);

			break;
		}
	}

	curView->updateBottomBar(this->getDescription());
}

/**
 * Shows the menu
 */
void Menu::post() {
	/*
	 * If the user calls this function without giving a list of choices, an
	 * empty one is initialized
	 */
	if(this->_menu == 0) {
		char *emptychoices[] = {
				0
		};
		this->setContent(emptychoices, emptychoices);
	}

	menu_opts_off(this->_menu, O_SHOWDESC);

	short defaultHeight = this->_height?this->_height:this->_menu->nitems;
	short defaultWidth = this->_width?this->_width:this->_menu->namelen;

	if(this->_margin) {
		defaultHeight+=2;
		defaultWidth+=2;
	}

	short subWinHeight = defaultHeight>this->_maxY?
			this->_maxY:defaultHeight;
	short subWinWidth = defaultWidth>this->_maxX?
			this->_maxX:defaultWidth;
	short posY = this->_posY == 0?
			(BaseScreen::WIN_HEIGHT/2)-(subWinHeight/2):this->_posY;
	short posX = this->_posX == 0?
			(BaseScreen::WIN_WIDTH/2)-(subWinWidth/2):this->_posX;

	this->_menuWin = derwin(this->_win,
			subWinHeight, subWinWidth,
			posY, posX);
	wbkgd(this->_menuWin, COLOR_PAIR(CursesView::MENU_BKGD));
	wrefresh(this->_menuWin);

	if(this->_margin) {
		this->_menuSubWin = derwin(this->_menuWin, subWinHeight-2,
				subWinWidth-2, 1, 1);
		set_menu_format(this->_menu, subWinHeight-2, 1);
	} else {
		this->_menuSubWin = derwin(this->_menuWin, subWinHeight,
				subWinWidth, 0, 0);
		set_menu_format(this->_menu, subWinHeight, 1);
	}

	set_menu_win(this->_menu, this->_menuWin);
	set_menu_sub(this->_menu, this->_menuSubWin);
	set_menu_back(this->_menu, this->_colorBack);
	set_menu_fore(this->_menu, this->_colorFore);
	set_menu_grey(this->_menu, this->_colorDisabled);
	set_menu_mark(this->_menu, 0);

	post_menu(this->_menu);
}

/**
 * Hides the menu
 */
void Menu::unpost() const {
	unpost_menu(this->_menu);

	WINDOW *tmpWin = derwin(this->_win, this->_width, this->_height,
			this->_posY, this->_posX);
	wbkgd(tmpWin, COLOR_PAIR(CursesView::WIN_BKGD));
	wrefresh(tmpWin);

	delwin(tmpWin);
}

/**
 * Gets the ordinal for the selected option
 */
int Menu::getIndex() const {
	ITEM *selectedItem = current_item(this->_menu);

	return selectedItem->index;
}

/**
 * Gets the label of the selected option
 */
const char *Menu::getName() const {
	ITEM *selectedItem = current_item(this->_menu);

	return selectedItem->name.str;
}

/**
 * Gets the description of the selected element
 */
const char *Menu::getDescription() const {
	ITEM *selectedItem = current_item(this->_menu);

	return selectedItem->description.str;
}

/**
 * Sets new options of the menu and erases the old options if any
 *
 * \param options
 * 		Array of options
 * \param descriptions
 * 		Array of descriptions (Must be one by option)
 */
void Menu::setContent(const char * const *options,
		const char * const *descriptions){
	int n_choices;

	for(n_choices = 0; options[n_choices]!=0; n_choices++);

	this->deleteMenu();
	this->deleteWindows();

	int size = n_choices+1;

	ITEM **items = new ITEM* [size];
	for(int i = 0; i < size; i++) {
		items[i] = new_item(options[i], descriptions[i]);
	}

	this->_menu = new_menu(items);
}

/**
 * Sets new options of the menu and erases the old options if any
 *
 * \param options
 * 		Vector of options
 * \param descriptions
 * 		Vector of descriptions (Must be one by option)
 */
void Menu::setContent(const std::vector<std::string> &options,
		const std::vector<std::string> &descriptions){
	int size = options.size();

	this->deleteContent();

	this->_options = new char*[size+1];
	this->_descriptions = new char*[size+1];

	int i = 0;
	std::vector<std::string>tmpOptions = options;
	std::vector<std::string>::iterator it;
	for(it = tmpOptions.begin(); it != tmpOptions.end(); ++it, i++) {
		int length=(*it).length()+1;
		this->_options[i]=new char[length];
		snprintf(this->_options[i], length, "%s", (*it).c_str());
	}

	int j = 0;
	std::vector<std::string> tmpDescriptions = descriptions;
	std::vector<std::string>::iterator it2;
	for(it2 = tmpDescriptions.begin(); it2 != tmpDescriptions.end(); ++it2, j++) {
		int length=(*it2).length()+1;
		this->_descriptions[j]=new char[length];
		snprintf(this->_descriptions[j], length, "%s", (*it2).c_str());
	}

	this->_options[i]=0;
	this->_descriptions[j]=0;

	this->setContent(this->_options, this->_descriptions);
}

/**
 * Sets the colors of the menu
 *
 * \param background
 * 		Color pair for the unselected item
 * \param foreground
 * 		Color pair for the selected items
 * \param disabled
 * 		Color pair for the disabled items
 */
void Menu::setColors(const chtype background, const chtype foreground,
		const chtype disabled) {
	this->_colorBack = background;
	this->_colorFore = foreground;
	this->_colorDisabled = disabled;
}

void Menu::setWindow(WINDOW * win) {
	this->_win = win;
}

void Menu::setPosX(const int posX) {
	this->_posX = posX;
}

void Menu::setPosY(const int posY) {
	this->_posY = posY;
}

void Menu::setMaxX(const int maxX) {
	this->_maxX = maxX;
}

void Menu::setMaxY(const int maxY) {
	this->_maxY = maxY;
}

void Menu::setWidth(const int width) {
	this->_width = width;
}

void Menu::setHeight(const int height) {
	this->_height = height;
}

void Menu::setMargin(const bool margin) {
    this->_margin = margin;
}

/**
 * Frees all memory of the menu
 */
void Menu::deleteMenu() {
	if(this->_menu != 0) {
		unpost_menu(this->_menu);

		int size = item_count(this->_menu)+1;

		ITEM **items = menu_items(this->_menu);

		free_menu(this->_menu);

		if(items != 0) {
			for(int i = 0; i < size; i++) {
				free_item(items[i]);
			}

			delete[] items;
		}

		this->_menu = 0;
	}
}

/**
 * Frees all options and descriptions
 */
void Menu::deleteContent() {
	if(this->_options != 0) {
		for(int i = 0; this->_options[i]!=0; i++) {
			delete[] this->_options[i];
			this->_options[i]=0;
		}

		delete[] this->_options;
		this->_options = 0;
	}

	if(this->_descriptions != 0) {
		for(int i = 0; this->_descriptions[i]!=0; i++) {
			delete[] this->_descriptions[i];
			this->_descriptions[i]=0;
		}

		delete[] this->_descriptions;
		this->_descriptions = 0;
	}
}

/**
 * Frees the windows created for this menu
 */
void Menu::deleteWindows() {
	if(this->_menuSubWin != 0) {
		wbkgd(this->_menuSubWin, COLOR_PAIR(CursesView::WIN_BKGD));
		wrefresh(this->_menuSubWin);

		delwin(this->_menuSubWin);
		this->_menuSubWin = 0;
	}

	if(this->_menuWin != 0) {
		wbkgd(this->_menuWin, COLOR_PAIR(CursesView::WIN_BKGD));
		wrefresh(this->_menuWin);

		delwin(this->_menuWin);
		this->_menuWin = 0;
	}
}

Menu::~Menu() {
	this->deleteMenu();
	this->deleteWindows();
	this->deleteContent();
}
