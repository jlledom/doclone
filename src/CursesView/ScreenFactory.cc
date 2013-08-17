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

#include <CursesView/ScreenFactory.h>
#include <CursesView/LanMenu.h>
#include <CursesView/SelectDeviceForm.h>
#include <CursesView/SelectFileForm.h>
#include <CursesView/InsertFileForm.h>
#include <CursesView/LanRoleMenu.h>
#include <CursesView/LocalMenu.h>
#include <CursesView/ScopeMenu.h>
#include <CursesView/TargetMenu.h>
#include <CursesView/InsertIpForm.h>
#include <CursesView/InsertNumOfClientsForm.h>
#include <CursesView/SummaryScreen.h>
#include <CursesView/ProgressScreen.h>

/**
 * Returns an pointer to the next screen
 *
 * The memory of this object will be freed in CursesView::flowControl()
 *
 * \param screenCode
 * 		The code of the screen that will be created
 *
 * \return
 * 		A pointer to a BaseScreen object
 */
BaseScreen *ScreenFactory::createScreen(int screenCode) {

	BaseScreen *scr;

	switch(screenCode) {
	case ScreenFactory::SCOPEMENU: {
		scr = new ScopeMenu();
		break;
	}
	case ScreenFactory::LOCALMENU: {
		scr = new LocalMenu();
		break;
	}
	case ScreenFactory::SELECTDEVICE: {
		scr = new SelectDeviceForm();
		break;
	}
	case ScreenFactory::SELECTFILE: {
		scr = new SelectFileForm();
		break;
	}
	case ScreenFactory::INSERTFILE: {
		scr = new InsertFileForm();
		break;
	}
	case ScreenFactory::LANMENU: {
		scr = new LanMenu();
		break;
	}
	case ScreenFactory::LANROLEMENU: {
		scr = new LanRoleMenu();
		break;
	}
	case ScreenFactory::TARGETMENU: {
		scr = new TargetMenu();
		break;
	}
	case ScreenFactory::INSERTNUMBEROFCLIENTS: {
		scr = new InsertNumOfClientsForm();
		break;
	}
	case ScreenFactory::INSERTIP: {
		scr = new InsertIpForm();
		break;
	}
	case ScreenFactory::SUMMARY: {
		scr = new SummaryScreen();
		break;
	}
	case ScreenFactory::PROGRESS: {
		scr = new ProgressScreen();
		break;
	}
	default: {
		scr = 0;
		break;
	}
	}

	return scr;
}

