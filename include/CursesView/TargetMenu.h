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

#ifndef TARGETMENU_H_
#define TARGETMENU_H_

#include <CursesView/BaseScreen.h>
#include <CursesView/Menu.h>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class SummaryScreen
 *
 * This screen has a menu where the user can choose between working on a device
 * or an image file
 *
 * \date October, 2011
 */
class TargetMenu : public BaseScreen {
public:
	TargetMenu();

	~TargetMenu();

protected:
	int getNextScreen();

	/// Menu with all the options
	Menu *_menu;
};
/**@}*/

#endif /* TARGETMENU_H_ */
