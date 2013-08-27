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

#include <CursesView/SelectFileForm.h>

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
#include <CursesView/Util.h>

SelectFileForm::SelectFileForm() {
	char currentPath[256];
	getcwd(currentPath, 256);
	std::vector<std::string> subDirs;
	std::vector<std::string> descriptions;
	std::string strPath = std::string(currentPath);
	this->getSubDirs(strPath, subDirs, descriptions);

	// Refresh the text at the top bar
	this->_topBarNotice=_("Select the directory and type the file name.");
	this->_topBarNotice.append(" ");
	this->_topBarNotice.append(_("Press [Tab] to move."));

	std::string notice=_("Current directory:");

	mvwprintw(this->_win, 2, 2, notice.c_str());

	std::string notice2=_("Choose an image file:");

	mvwprintw(this->_win, 6, 2, notice2.c_str());

	this->_menu = new Menu();
	this->_menu->setWindow(this->_win);
	this->_menu->setContent(subDirs, descriptions);
	this->_menu->setColors(
				COLOR_PAIR(CursesView::MENU_BKGD),
				COLOR_PAIR(CursesView::SELECTED_OPTION),
				COLOR_PAIR(CursesView::UNSELECTABLE_OPTION));
	this->_menu->setMargin(true);
	this->_menu->setMaxY(9);
	this->_menu->setPosY(8);
	this->_menu->post();

	this->_menuBack->post();
	this->_menuContinue->post();

	this->_form = new TextInput();
	this->_form->setWindow(this->_win);
	this->_form->setPosY(4);

	this->_form->setColors(
			COLOR_PAIR(CursesView::WIN_BKGD),
			COLOR_PAIR(CursesView::NOT_VALID_FIELD));
	this->_form->post();
	this->_form->setValue(currentPath);

	this->_listWidgets.push_back(this->_menu);
	this->_listWidgets.push_back(this->_menuBack);
	this->_listWidgets.push_back(this->_menuContinue);

	this->_it = this->_listWidgets.begin();

	wrefresh(this->_win);
}

/**
 * This class overrides this function because the event of pressing ENTER key
 * has an especial behavior. If the user press ENTER in the menu, it calls
 * updateMenu() and reads the list of sub-directories and *.doclone files.
 */
void SelectFileForm::readKeyboard() {
	bool readKeyboard = true;

	do {
		int c = getch();

		switch(c) {
		case KEY_ENTER:
		case '\n':
		case '\r': {
			std::string menuVal(this->_form->getValue());
			menuVal.append("/");
			menuVal.append(this->_menu->getName());
			std::string ext(GN_EXTENSION);

			if((*this->_it) == this->_menu) {
				if(Util::endsWith(menuVal, ext)){
					readKeyboard = false;
				} else {
					this->updateMenu();
				}
			} else if((*this->_it) == this->_menuContinue) {
				if(Util::endsWith(menuVal, ext)) {
					readKeyboard = false;
				}
			} else {
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
 * Enters into the selected sub-directory and refreshes the list of
 * sub-directories and *.doclone files.
 */
void SelectFileForm::updateMenu() {
	std::string currentPath(this->_form->getValue());

	if(!strcmp("..", this->_menu->getName())) { // If user selects ".."
		int index = currentPath.find_last_of('/');
		currentPath = currentPath.substr(0, index);
		if(currentPath.length() == 0){ // User move down to root directory
			currentPath.append("/");
		}
	} else {
		if(currentPath.length() == 0) { // If user is in the root directory
			currentPath.append("/");
		} else if(currentPath.at(currentPath.length()-1)!='/') {
			// If the path doesn't end with /
			currentPath.append("/");
		}
		currentPath.append(this->_menu->getName());
	}

	std::vector<std::string> subDirs;
	std::vector<std::string> descriptions;

	this->getSubDirs(currentPath, subDirs, descriptions);
	this->_menu->setContent(subDirs, descriptions);
	this->_menu->post();

	this->_form->setValue(currentPath.c_str());
}

/**
 * Reads the list of sub-directories inside the given path
 */
void SelectFileForm::getSubDirs(const std::string &path, std::vector<std::string> &subDirs,
		std::vector<std::string> &descriptions) {

	std::vector<std::string> images;
	std::vector<std::string> descrImages;

	std::string dirPath = path;
	if(dirPath.empty()) {
		dirPath="/";
	}

	DIR *directory;
	struct dirent *d_file;

	if((directory = opendir(dirPath.c_str())) == 0) {
		if(errno == EACCES) {
			subDirs.push_back("..");
			descriptions.push_back(_("You do not have permission to access this directory"));

			return;
		}
	}

	while ((d_file = readdir (directory)) != 0) {
		struct stat filestat;
		std::string abPath;

		abPath = dirPath;
		abPath.append("/");
		abPath.append(d_file->d_name);

		lstat (abPath.c_str(), &filestat);

		// If the file is hidden, read the next one
		if (d_file->d_name[0]=='.' && strcmp ("..", d_file->d_name)) {
			continue;
		}

		switch (filestat.st_mode & S_IFMT) {
		case S_IFDIR: {
			subDirs.push_back(std::string(d_file->d_name));

			// TO TRANSLATORS: looks like	Enter into /home
			std::string description(_("Enter into"));
			description.append(" ");
			description.append(d_file->d_name);

			descriptions.push_back(description);

			break;
		}
		case S_IFREG : {
			std::string ext = std::string(GN_EXTENSION);
			if(Util::endsWith(abPath, ext)){
				images.push_back(std::string(d_file->d_name));

				// TO TRANSLATORS: looks like	Choose sda.doclone
				std::string description(_("Choose"));
				description.append(" ");
				description.append(d_file->d_name);

				descrImages.push_back(description);
			}

			break;
		}
		}
	}

	closedir (directory);

	sort(subDirs.begin(), subDirs.end());
	sort(descriptions.begin(), descriptions.end());

	sort(images.begin(), images.end());
	sort(descrImages.begin(), descrImages.end());

	std::vector<std::string>::iterator it;
	std::vector<std::string>::iterator it2;
	for(it = images.begin(), it2 = descrImages.begin();
			it != images.end(); ++it, ++it2) {
		subDirs.push_back((*it));
		descriptions.push_back((*it2));
	}
}

/**
 * Reads the state of the widgets and determines what will be the next screen
 *
 * \return The code of the next screen
 */
int SelectFileForm::getNextScreen() {
	CursesView *curView = CursesView::getInstance();
	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	int retVal = ScreenFactory::NONE;

	std::string image(this->_form->getValue());
	image.append("/");
	image.append(this->_menu->getName());

	if((*this->_it) == this->_menuBack) {
		retVal = ScreenFactory::BACK;
	} else {
		dcCursesMode prevFunc = curView->getMode();
		dcl->setImage(image);

		switch (prevFunc) {
		case CURSES_MODE_LOCAL: {
			retVal = ScreenFactory::SELECTDEVICE;
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

SelectFileForm::~SelectFileForm() {
	delete this->_form;
	delete this->_menu;
}
