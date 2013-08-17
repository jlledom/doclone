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

#ifndef SCREENFACTORY_H_
#define SCREENFACTORY_H_

#include <CursesView/BaseScreen.h>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class ScreenFactory
 *
 * Factory for BaseScreen inherited classes
 *
 * \date October, 2011
 */
class ScreenFactory {
public:
	static const int BACK=-1;
	static const int NONE = 0;
	static const int SCOPEMENU = 1;
	static const int LOCALMENU = 2;
	static const int LANMENU = 3;
	static const int SELECTDEVICE = 4;
	static const int SELECTFILE = 5;
	static const int INSERTFILE = 6;
	static const int LANROLEMENU = 7;
	static const int TARGETMENU = 8;
	static const int INSERTNUMBEROFCLIENTS = 9;
	static const int INSERTIP = 10;
	static const int SUMMARY = 11;
	static const int PROGRESS = 12;

	static BaseScreen *createScreen(int screenCode);
};
/**@}*/

#endif /* SCREENFACTORY_H_ */
