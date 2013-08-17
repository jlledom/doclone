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

#ifndef SELECTFILEFORM_H_
#define SELECTFILEFORM_H_

#include <CursesView/BaseScreen.h>
#include <CursesView/Menu.h>
#include <CursesView/TextInput.h>

#include <string>
#include <vector>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class SelectFileForm
 *
 * In this screen the user can select the image file or insert it manually
 *
 * \date October, 2011
 */
class SelectFileForm : public BaseScreen {
public:
	SelectFileForm();

	~SelectFileForm();

protected:
	void readKeyboard();
	int getNextScreen();

	void updateMenu();
	void getSubDirs(const std::string &path, std::vector<std::string> &subDirs,
			std::vector<std::string> &descriptions);

	/// Text input to insert the image path manually
	TextInput *_form;
	/// Menu with the list of image and sub-directories of the current directory
	Menu *_menu;
};
/**@}*/

#endif /* SELECTFILEFORM_H_ */
