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

#ifndef CURSESVIEW_UTIL_H_
#define CURSESVIEW_UTIL_H_

#include <string>

/**
 * \addtogroup CursesView
 * @{
 *
 * \class SummaryScreen
 *
 * Some useful functions used by many screens.
 *
 * \date October, 2011
 */
class Util {
public:
	static std::string trim(const std::string &str);
	static std::string ltrim(const std::string &str);
	static std::string rtrim(const std::string &str);

	static bool endsWith(const std::string &orig, const std::string &str);

	static bool match(const std::string &str, const std::string &regEx);
};
/**@}*/

#endif /* CURSESVIEW_UTIL_H_ */
