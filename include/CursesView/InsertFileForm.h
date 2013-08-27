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

#ifndef INSERTFILEFORM_H_
#define INSERTFILEFORM_H_

#include <string>
#include <vector>

#include <CursesView/BaseScreen.h>
#include <CursesView/Menu.h>
#include <CursesView/TextInput.h>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class InsertFileForm
 *
 * Screen to enter the path for the image to be created
 *
 * \date October, 2011
 */
class InsertFileForm : public BaseScreen {
public:
	InsertFileForm();

	~InsertFileForm();

protected:
	void readKeyboard();
	int getNextScreen();

	void updateMenu();
	void getSubDirs(const std::string &path, std::vector<std::string> &subDirs,
			std::vector<std::string> &descriptions) const;

	/// Shows the list of sub-directories
	Menu *_menu;

	/// An input where the user can write the file path directly
	TextInput *_form;

	/// The current path selected by the Menu or the TextInput
	std::string _currentPath;
};
/**@}*/

#endif /* INSERTFILEFORM_H_ */
