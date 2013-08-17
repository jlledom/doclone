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

#ifndef CURSESWIDGET_H_
#define CURSESWIDGET_H_

#define _(string) gettext(string)

/**
 * \addtogroup CursesView
 * @{
 *
 * \class CursesWidget
 *
 * Represents a widget of ncurses
 *
 * Abstract class that provides all the common operations for all widgets
 *
 * \date October, 2011
 */
class CursesWidget {
public:
	/**
	 * Operations to be performed when the widget gets the focus
	 */
	virtual void focus() const = 0;

	/**
	 * Operations to be performed when the widget loses the focus
	 */
	virtual void blur() const = 0;

	/**
	 * Evaluates the key pressed and performs the particular action
	 * for this widget
	 */
	virtual void doAction(int c) const = 0;

	/**
	 * Shows the widget in the screen
	 */
	virtual void post()=0;

	/**
	 * Hides the widget
	 */
	virtual void unpost() const = 0;

	virtual ~CursesWidget(){}
};
/**@}*/

#endif /* CURSESWIDGET_H_ */
