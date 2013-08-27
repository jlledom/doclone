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

#include <CursesView/TextInput.h>

#include <CursesView/CursesView.h>
#include <CursesView/BaseScreen.h>
#include <CursesView/Util.h>

TextInput::TextInput() :
	_form(), _win(), _fields(), _posX(), _posY(),
	_width(BaseScreen::WIN_WIDTH-4), _colorFore(), _colorError(),
	_type(T_TEXT), _description(), _regExp(), _nullable() {
}

/**
 * Places the cursor in the same place on where it was, validates the content
 * and updates the bottom bar.
 */
void TextInput::focus() const {
	CursesView *curView = CursesView::getInstance();

	form_driver(this->_form, REQ_SFIRST_FIELD);
	form_driver(this->_form, REQ_END_LINE);

	this->validate();

	curView->updateBottomBar(this->_description.c_str());
}

/**
 * On blur, validate the content
 */
void TextInput::blur() const {
	this->validate();
}

/**
 * Evaluates the key pressed. RIGHT and LEFT arrows move the cursor,
 * SUPR and BAKCSPACE delete next and previous character. Finally validate
 * the content.
 *
 * \param c
 * 		Code for the key pressed
 */
void TextInput::doAction(int c) const {
	switch(c) {
		case KEY_LEFT:  {
			form_driver(this->_form, REQ_PREV_CHAR);

			break;
		}
		case KEY_RIGHT:  {
			form_driver(this->_form, REQ_NEXT_CHAR);

			break;
		}
		case KEY_BACKSPACE: {
			form_driver(this->_form, REQ_DEL_PREV);

			break;
		}
		case KEY_DC: {
			form_driver(this->_form, REQ_DEL_CHAR);

			break;
		}
		default: {
			form_driver(this->_form, c);

			break;
		}
	}

	this->validate();
}

/**
 * Shows the ncurses form
 */
void TextInput::post() {
	this->_fields = new FIELD*[2];

	short fieldWidth = this->_width;
	short posY = this->_posY == 0?
				(BaseScreen::WIN_HEIGHT/2):this->_posY;
	short posX = this->_posX == 0?
			(BaseScreen::WIN_WIDTH/2)-(fieldWidth/2):this->_posX;

	this->_fields[0] = new_field(1, fieldWidth, 0, 0, 0, 0);
	this->_fields[1] = 0;

	field_opts_off(this->_fields[0], O_AUTOSKIP);
	field_opts_off(this->_fields[0], O_STATIC);

	set_field_back(this->_fields[0], this->_colorFore);
	set_field_fore(this->_fields[0], this->_colorFore);

	this->_form = new_form(this->_fields);

	set_form_win(this->_form, this->_win);
	set_form_sub(this->_form, derwin(this->_win, 1, fieldWidth, posY, posX));

	post_form(this->_form);
}

/**
 * Hides the ncurses form
 */
void TextInput::unpost() const {
	unpost_form(this->_form);

	WINDOW *tmpWin = derwin(this->_win, this->_width, 1,
			this->_posY, this->_posX);
	wbkgd(tmpWin, COLOR_PAIR(CursesView::WIN_BKGD));
	wrefresh(tmpWin);

	delwin(tmpWin);
}

/**
 * Determines the appropriate regular expression and checks if it matches with
 * the content of the field.
 */
void TextInput::validate() const {
	bool valid = false;

	std::string value(this->getValue());

	switch(this->_type) {
	case T_TEXT: {
		if(!this->_regExp.empty()) {
			valid = Util::match(value, this->_regExp);
		} else {
			valid = true;

			if(value.empty()) {
				valid = this->_nullable;
			}
		}

		break;
	}
	case T_NUM: {
		std::string regExp(NUM_REGEX);
		valid = Util::match(value, regExp);
		break;
	}
	case T_IPV4: {
		std::string regExp(IPV4_REGEX);
		valid = Util::match(value, regExp);
		break;
	}
	}

	// If is valid, the background is set blue. Else, is set as red.
	if(valid) {
		set_field_back(this->_fields[0], this->_colorFore);
		set_field_fore(this->_fields[0], this->_colorFore);
	} else {
		set_field_back(this->_fields[0], this->_colorError);
		set_field_fore(this->_fields[0], this->_colorError);
	}
}

/**
 * Gets the content of the field
 */
const char *TextInput::getValue() const {
	form_driver(this->_form, REQ_VALIDATION);

	char *tmpBuf = field_buffer(this->_fields[0], 0);
	std::string buffer(tmpBuf);
	buffer = Util::trim(buffer);
	int length = buffer.length();
	tmpBuf[length]=0;

	return tmpBuf;
}

void TextInput::setValue(const char *value) {
	set_field_buffer(this->_fields[0], 0, value);
}

/**
 * Sets the colors of the input
 *
 * \param foreground
 * 		Color pair for a valid field
 * \param error
 * 		Color pair for a non-valid field
 */
void TextInput::setColors(const chtype foreground, const chtype error) {
	this->_colorFore = foreground;
	this->_colorError = error;
}

void TextInput::setType(const inputType type) {
	this->_type = type;
}

void TextInput::setRegExp(const std::string &regExp) {
	this->_regExp = regExp;
}

void TextInput::setNullable(const bool nullable) {
	this->_nullable = nullable;
}

void TextInput::setWindow(WINDOW * win) {
	this->_win = win;
}

void TextInput::setPosX(const int posX){
	this->_posX = posX;
}

void TextInput::setPosY(const int posY) {
	this->_posY = posY;
}

void TextInput::setWidth(const int width) {
	this->_width = width;
}

const std::string &TextInput::getDescription() const {
    return this->_description;
}

void TextInput::setDescription(const std::string &description) {
    this->_description = description;
}

TextInput::~TextInput() {
	if(this->_form != 0) {
		unpost_form(this->_form);

		delwin(this->_form->sub);

		free_form(this->_form);
		free_field(this->_fields[0]);

		delete[] this->_fields;
	}
}
