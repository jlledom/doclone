/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2015 Joan Lledó <joanlluislledo@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XMLSTRINGHANDLER_H_
#define XMLSTRINGHANDLER_H_

#include <stdint.h>

#include <vector>

#include <xercesc/util/XMLString.hpp>

#include <doclone/exception/Exception.h>

XERCES_CPP_NAMESPACE_USE

namespace Doclone {

/**
 * \class XMLStringHandler
 *
 * \brief Handling of the memory management for the transcoded strings.
 *
 * \date Junde, 2015
 */
class XMLStringHandler {
public:
	static XMLStringHandler *getInstance();
	~XMLStringHandler();

	//String handling methods
	const XMLCh *toXMLText(const uint8_t *c_str);
	const XMLCh *toXMLText(const char *c_str);
	const uint8_t *toCString(const XMLCh *xmlch);
	const uint8_t *toCString(XMLByte *xmlbyte);
private:
	/// Private constructor for singleton pattern
	XMLStringHandler() throw(Exception);

	///Storage for the transcoded strings
	std::vector<XMLCh*> _listXmlchData;

	///Storage for the C strings
	std::vector<uint8_t*> _listXmlByteData;
};

}

#endif /* XMLSTRINGHANDLER_H_ */
