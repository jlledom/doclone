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

#ifndef LANMENU_H_
#define LANMENU_H_

#include <CursesView/BaseScreen.h>
#include <CursesView/Menu.h>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class LanMenu
 *
 * This screen shows a menu where the user can select between Link mode or
 * Unicast/Multicast mode
 *
 * \date October, 2011
 */
class LanMenu : public BaseScreen {
public:
	LanMenu();

	~LanMenu();

protected:
	int getNextScreen();

	/// Menu with the two network modes: Link and Unicast/Multicast
	Menu *_menu;
};
/**@}*/

#endif /* LANMENU_H_ */
