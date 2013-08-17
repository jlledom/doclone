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

#ifndef INSERTNUMOFCLIENTSFORM_H_
#define INSERTNUMOFCLIENTSFORM_H_

#include <CursesView/BaseScreen.h>
#include <CursesView/TextInput.h>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class InsertNumOfClientsForm
 *
 * Screen to enter the number of receivers in multicast mode
 *
 * \date October, 2011
 */
class InsertNumOfClientsForm : public BaseScreen {
public:
	InsertNumOfClientsForm();

	~InsertNumOfClientsForm();

protected:
	int getNextScreen();

	/// Text input where the user enters the number of clients
	TextInput *_form;
};
/**@}*/

#endif /* INSERTNUMOFCLIENTSFORM_H_ */
