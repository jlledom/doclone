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

#include <CursesView/InsertFileForm.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <libintl.h>
#include <string.h>
#include <errno.h>

#include <algorithm>

#include <doclone/Clone.h>

#include <CursesView/CursesView.h>
#include <CursesView/ScreenFactory.h>

InsertFileForm::InsertFileForm() {
	char currentPath[256];
	getcwd(currentPath, 256);
	this->_currentPath = std::string(currentPath);
	std::vector<std::string> subDirs;
	std::vector<std::string> descriptions;
	this->getSubDirs(this->_currentPath, subDirs, descriptions);

	// Refresh the text at the top bar
	this->_topBarNotice=_("Select the directory and type the file name.");
	this->_topBarNotice.append(" ");
	this->_topBarNotice.append(_("Press [Tab] to move."));

	std::string notice=_("Choose a directory:");

	mvwprintw(this->_win, 2, 2, notice.c_str());

	std::string notice2=_("Insert the file name:");

	mvwprintw(this->_win, BaseScreen::WIN_HEIGHT-6, 2, notice2.c_str());

	this->_menu = new Menu();
	this->_menu->setWindow(this->_win);
	this->_menu->setContent(subDirs, descriptions);
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

	std::string formDescription=_("Please, type image path.");
	formDescription.append(" ");
	// TO TRANLATORS: looks like	You must insert a filename ending with .doclone
	formDescription.append(_("Sorry, you must insert a filename ending with ."));
	formDescription.append(GN_EXTENSION);
	this->_form->setDescription(formDescription);

	this->_form->setColors(
			COLOR_PAIR(CursesView::MENU_BKGD),
			COLOR_PAIR(CursesView::NOT_VALID_FIELD));

	this->_form->setType(T_TEXT);
	std::string regEx("\\.");
	regEx.append(GN_EXTENSION);
	regEx.append("$");
	this->_form->setRegExp(regEx);

	this->_form->post();
	this->_form->setValue(currentPath);

	this->_listWidgets.push_back(this->_menu);
	this->_listWidgets.push_back(this->_form);
	this->_listWidgets.push_back(this->_menuBack);
	this->_listWidgets.push_back(this->_menuContinue);

	this->_it = this->_listWidgets.begin();

	wrefresh(this->_win);
}

/**
 * This class overrides this function because the ENTER key should not end the
 * listening of the user events. If the focus is this->_menu, ENTER key
 * indicates that the user wish to enter into the selected directory.
 */
void InsertFileForm::readKeyboard() {
	bool readKeyboard = true;

	do {
		int c = getch();

		switch(c) {
		case KEY_ENTER:
		case '\n':
		case '\r': {
			// If the focus is in this->_menu, enter directory
			if((*this->_it) == this->_menu) {
				this->updateMenu();
			} else { // If not, go to the next screen
				readKeyboard = false;
			}
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
 * Enters into the selected directory and updates the menu to show the
 * sub-directories. Sets this->_currentPath and updates the content of the text
 * input.
 */
void InsertFileForm::updateMenu() {
	if(!strcmp("..", this->_menu->getName())) { // If user selects ".."
		int index = this->_currentPath.find_last_of('/');
		this->_currentPath = this->_currentPath.substr(0, index);
		if(this->_currentPath.length() == 0){ // User move down to root directory
			this->_currentPath.append("/");
		}
	} else {
		if(this->_currentPath.length() == 0) { // If user is in the root directory
			this->_currentPath.append("/");
		} else if(this->_currentPath.at(this->_currentPath.length()-1)!='/') {
			// If the path doesn't end with '/', we add it.
			this->_currentPath.append("/");
		}
		this->_currentPath.append(this->_menu->getName());
	}

	std::vector<std::string> subDirs;
	std::vector<std::string> descriptions;

	this->getSubDirs(this->_currentPath, subDirs, descriptions);
	this->_menu->setContent(subDirs, descriptions);
	this->_menu->post();

	this->_form->setValue(this->_currentPath.c_str());
}

/**
 * Opens the directory passed as parameter and reads all the sub-directories
 *
 * \param path
 * 		The path of the directory to be read
 * \param subDirs
 * 		The vector where the list of sub-directories will be written
 * \param descriptions
 * 		The vector where the descriptions will be written
 */
void InsertFileForm::getSubDirs(const std::string &path, std::vector<std::string> &subDirs,
		std::vector<std::string> &descriptions) const {

	std::string filePath = path;
	if(filePath.empty()) {
		filePath="/";
	}

	DIR *directory;
	struct dirent *d_file;

	if((directory = opendir(filePath.c_str())) == 0) {
		if(errno == EACCES) {
			subDirs.push_back("..");
			descriptions.push_back(_("Sorry, you do not have permission to access this directory"));

			return;
		}
	}

	while ((d_file = readdir (directory)) != 0) {
		struct stat filestat;
		std::string abPath;

		abPath = filePath;
		abPath.append("/");
		abPath.append(d_file->d_name);

		lstat (abPath.c_str(), &filestat);

		if ((filestat.st_mode & S_IFMT) == S_IFDIR) {
			if (!strcmp ("..", d_file->d_name) || // if equals ".."
					d_file->d_name[0]!='.') { // Don't show hidden folders
				subDirs.push_back(std::string(d_file->d_name));

				// TO TRANSLATORS: looks like	Enter into /home
				std::string description(_("Enter into"));
				description.append(" ");
				description.append(d_file->d_name);

				descriptions.push_back(description);
			}
		}
	}

	closedir (directory);

	sort(subDirs.begin(), subDirs.end());
	sort(descriptions.begin(), descriptions.end());
}

/**
 * Reads the state of the widgets and determines what will be the next screen
 *
 * \return The code of the next screen
 */
int InsertFileForm::getNextScreen() {
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = ScreenFactory::NONE;

	std::string inputVal(this->_form->getValue());

	if(*this->_it == this->_menuBack) {
		retVal = ScreenFactory::BACK;
	} else {
		dcl->setImage(inputVal);

		retVal = ScreenFactory::SUMMARY;
	}

	return retVal;
}

InsertFileForm::~InsertFileForm() {
	delete this->_form;
	delete this->_menu;
}


