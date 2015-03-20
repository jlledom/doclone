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

#ifndef XMLDOCUMENT_H_
#define XMLDOCUMENT_H_

#include <stdint.h>

#include <vector>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include <doclone/exception/Exception.h>

XERCES_CPP_NAMESPACE_USE

namespace Doclone {

///XML feature for DOM Level 3
const char XML_FEATURE[] = "Core";

///Root node of the document
const char XML_ROOT_ELEMENT[] = "image";

///Public Identifier for doclone image XMLs
const char XML_PUBLIC_ID[] = "-//DOCLONE//DOCLONE IMAGE 1.0//EN";

///System identifier for doclone image XMLs (DTD file location)
const char XML_SYSTEM_ID[] = "http://doclone.nongnu.org/xml/image-1.0.dtd";

/**
 * \class XMLDocument
 *
 * \brief XML document to store de metadata of a doclone image
 *
 * The main purpose of this class is to provide an interface for the reduced
 * set of functionality that doclone needs and delegate the handling of the
 * memory management for the transcoded strings.
 *
 * \date February, 2015
 */
class XMLDocument {
public:
	XMLDocument();
	~XMLDocument();

	//Write methods
	void createNew();
	DOMElement *getRootElement();
	DOMElement *createElement(DOMElement *parent, const char *name) throw(Exception);
	DOMElement *createElement(DOMElement *parent, const char *name, const uint8_t *value) throw(Exception);
	DOMElement *createElement(DOMElement *parent, const char *name, const uint8_t value) throw(Exception);
	DOMElement *createElement(DOMElement *parent, const char *name, const uint16_t value) throw(Exception);
	DOMElement *createElement(DOMElement *parent, const char *name, const uint64_t value) throw(Exception);
	DOMElement *createBinaryElement(DOMElement *parent, const char *name, const uint8_t *buf, size_t len) throw(Exception);
	const char *serialize(std::string &buf);

	//Read methods
	void openFromMem(const char *buf);
	const DOMElement *getElement(const DOMElement *parent, const char *name);
	const DOMNodeList *getElements(const DOMElement *parent, const char *name);
	const uint8_t *getElementValueCString(const DOMElement *parent, const char *name);
	const uint8_t getElementValueU8(const DOMElement *parent, const char *name);
	const uint16_t getElementValueU16(const DOMElement *parent, const char *name);
	const uint64_t getElementValueU64(const DOMElement *parent, const char *name);
	const uint8_t *getElementValueBinary(const DOMElement *parent, const char *name);
private:
	//String handling methods
	const XMLCh *toXMLText(const uint8_t *c_str);
	const XMLCh *toXMLText(const char *c_str);
	const uint8_t *toCString(const XMLCh *xmlch);

	///XML document
	DOMDocument *_doc;

	/// Xerces XML parser
	DOMLSParser *_parser;

	///Storage for the transcoded strings
	std::vector<XMLCh*> _listXmlchData;

	///Storage for the C strings
	std::vector<uint8_t*> _listXmlByteData;
};

}

#endif /* XMLDOCUMENT_H_ */
