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

#ifndef TEXTINPUT_H_
#define TEXTINPUT_H_

#include <CursesView/CursesWidget.h>

#include <form.h>

#include <string>

/**
 * Regular expression that matches with a number
 */
#define NUM_REGEX "^[0-9]+$"

/**
 * Regular expression that matches with an IP address v4
 */
#define IPV4_REGEX "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"

/**
 * \enum inputType
 * \brief Type of the text input validation
 *
 * An input can validate its content by using some regular expressions. This
 * enumerator defines which type of validation is set. If T_TEXT is selected,
 * the content will be validated using this->_regexp, if exists.
 *
 * \var T_TEXT
 * 	No type specified, use the this->_regExp for validation, if any
 * \var T_NUM
 * 	Only numbers accepted
 * \var T_IPV4
 * 	Only IPv4 address accepted
 */
enum inputType {
	T_TEXT,
	T_NUM,
	T_IPV4
};

/**
 * \addtogroup CursesView
 * @{
 *
 * \class TextInput
 *
 * C++ wrapper for the FORM ncurses widget
 *
 * \date October, 2011
 */
class TextInput: public CursesWidget {
public:
	TextInput();

	void focus() const;
	void blur() const;
	void doAction(int c) const;
	void post();
	void unpost() const;

	const char *getValue() const;
	void setValue(const char *value);

	void setColors(chtype foreground, chtype error);
	void setType(inputType type);
	void setRegExp(const std::string &regExp);
	void setNullable(bool nullable);

	void setWindow(WINDOW * win);
	void setPosX(int posX);
	void setPosY(int posY);
	void setWidth(int width);
	const std::string &getDescription() const;
	void setDescription(const std::string &description);

	~TextInput();

protected:
	void validate() const;

	/// The ncurses form
	FORM *_form;
	/// The window where is the input
	WINDOW *_win;
	/// Fields of the form, this class always have only one.
	FIELD **_fields;

	/// X position of the form inside this->_win
	int _posX;
	/// Y position of the form inside this->_win
	int _posY;
	/// Width of the input. The height will be always 1
	int _width;

	/// Color pair for a valid field
	chtype _colorFore;
	/// Color pair for a non valid field
	chtype _colorError;

	/// The type of this input
	inputType _type;

	/// The description of input
	std::string _description;
	/// String regular expression for validation if the input is of T_TEXT type
	std::string _regExp;

	/// Indicates if this input can be empty
	bool _nullable;
};
/**@}*/

#endif /* TEXTINPUT_H_ */
