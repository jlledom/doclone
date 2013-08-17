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

#include <CursesView/Util.h>

#include <regex.h>

#include <algorithm>

/**
 * Trims white spaces at both sides of a string
 *
 * \param str
 * 		String to trim
 * \return
 * 		String with the result
 */
std::string Util::trim(const std::string &str) {
	std::string s = str;
	return Util::ltrim(Util::rtrim(s));
}

/**
 * Trims white spaces at left side of a string
 *
 * \param str
 * 		String to trim
 * \return
 * 		String with the result
 */
std::string Util::ltrim(const std::string &str) {
	std::string s = str;
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

/**
 * Trims white spaces at right side of a string
 *
 * \param str
 * 		String to trim
 * \return
 * 		String with the result
 */
std::string Util::rtrim(const std::string &str) {
	std::string s = str;
    s.erase(std::find_if(s.rbegin(), s.rend(),
    		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

/**
 * Checks if a string ends with the substring passed as parameter
 *
 * \param orig
 * 		Original string
 * \param str
 * 		Substring to compare with
 *
 * \return
 * 		True or False
 */
bool Util::endsWith(const std::string &orig, const std::string &str) {
	if (orig.length() >= str.length()) {
		return (0 == orig.compare (orig.length() - str.length(),
				str.length(), str));
	} else {
		return false;
	}
}

/**
 * Checks if a string matches with the regular expression passed as parameter
 *
 * \param orig
 * 		Original string
 * \param str
 * 		Regular expression to compare with
 *
 * \return
 * 		True or False
 */
bool Util::match(const std::string &str, const std::string &regEx) {
	bool retVal = false;

	regex_t preg ;
	int nmatch = 1 ;
	regmatch_t pmatch[  nmatch ] ;
	int rc = regcomp( &preg, regEx.c_str(),
			REG_EXTENDED | REG_ICASE | REG_NEWLINE ) ;
	if(rc == 0) {
		if (regexec(&preg, str.c_str(), nmatch, pmatch, 0) == 0) {
			// If it matches
			retVal = true;
		}
	}

	regfree(&preg);

	return retVal;
}
