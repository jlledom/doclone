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

#ifndef SELECTDEVICEFORM_H_
#define SELECTDEVICEFORM_H_

#include <CursesView/BaseScreen.h>
#include <CursesView/TextInput.h>
#include <CursesView/Menu.h>

#include <form.h>

#include <string>
#include <vector>

/**
 * Regular expression that matches with the lines in /proc/partitions containing
 * information about a device
 */
#define DEV_REGEX "^[\t ]+[0-9]+[\t ]+[0-9]+[\t ]+[0-9]+[\t ]+[a-zA-Z0-9]+$"

/**
 * \addtogroup CursesView
 * @{
 *
 * \class SelectDeviceForm
 *
 * Screen for select the device on which work
 *
 * \date October, 2011
 */
class SelectDeviceForm : public BaseScreen {
public:
	SelectDeviceForm();

	~SelectDeviceForm();

protected:
	int getNextScreen();

	void detectDevices();

	/// List of detected devices
	std::vector<std::string> _deviceList;
	/// List of descriptions for the menu
	std::vector<std::string> _descriptionList;

	/// Text input to insert the device path manually
	TextInput *_form;
	/// Menu with the list of the devices
	Menu *_menu;
};
/**@}*/

#endif /* SELECTDEVICEFORM_H_ */
