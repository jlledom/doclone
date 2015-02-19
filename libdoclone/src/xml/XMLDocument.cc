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

#include <doclone/xml/XMLDocument.h>

#include <stdint.h>

#include <iostream>
#include <vector>
#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/Base64.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include <doclone/exception/Exception.h>

namespace Doclone {

XERCES_CPP_NAMESPACE_USE

/**
 * Class constructor.
 *
 * Initializes the Xerced library and the class attributes
 */
XMLDocument::XMLDocument() : _doc(), _parser(),_listXmlchData(), _listXmlByteData() {
	// Initialize the XML4C2 system.
    try {
        XMLPlatformUtils::Initialize();
    } catch(const XMLException& toCatch) {
        //TODO: Definir i llançar excepció
    }
}

/**
 * \brief Transforms a C string into a transcoded XML string
 *
 * \param c_str Pointer to the C string to be transcoded
 *
 * \return A transcoded string
 */
const XMLCh *XMLDocument::toXMLText(const uint8_t *c_str) {
	return this->toXMLText(reinterpret_cast<const char *>(c_str));
}

/**
 * \brief Transforms a C string into a transcoded XML string
 *
 * \param c_str Pointer to the C string to be transcoded
 *
 * \return An transcoded string
 */
const XMLCh *XMLDocument::toXMLText(const char *c_str) {
	XMLCh *retVal = XMLString::transcode(c_str);
	this->_listXmlchData.push_back(retVal);

	return retVal;
}

/**
 * \brief Transforms a transcoded XML string into a C string
 *
 * \param c_str Pointer to the transcoded XML string
 *
 * \return An old C string
 */
const uint8_t *XMLDocument::toCString(const XMLCh *xmlch) {
	TranscodeToStr trns(xmlch, "UTF-8");
	uint8_t *c_str = reinterpret_cast<uint8_t *>(trns.adopt());

	this->_listXmlByteData.push_back(c_str);

	return c_str;
}

/**
 * \brief Creates a new XML document for writing and sotres it in this->_doc
 *
 * \param rootNode The root node of the new XML
 */
void XMLDocument::createNew(const char* rootNode) {
	DOMImplementation *impl =
			DOMImplementationRegistry::getDOMImplementation(
					this->toXMLText(XML_FEATURE));

	DOMDocumentType *docType =
			impl->createDocumentType(this->toXMLText(rootNode),
			this->toXMLText(XML_PUBLIC_ID),
			this->toXMLText(XML_SYSTEM_ID));

	this->_doc = impl->createDocument(0, this->toXMLText(rootNode), docType);
}

/**
 * \brief Returns de root element of this document
 *
 * \return Root element of the XML
 */
DOMElement *XMLDocument::getRootElement() {
	return this->_doc->getDocumentElement();
}

/**
 * \brief Creates a new child element for in the given parent
 *
 * \param parent The parent of the new element
 * \param name The name of the new element
 *
 * \return The new element
 */
DOMElement *XMLDocument::createElement(DOMElement *parent,
		const char *name) throw(Exception) {

	DOMElement *prodElem = 0;

	try {
		prodElem = this->_doc->createElement(this->toXMLText(name));
		parent->appendChild(prodElem);

	} catch(const OutOfMemoryException &ex) {
		//TODO: Definir i llançar excepció
	} catch(const DOMException &ex) {
		//TODO: Definir i llançar excepció
	}

	return prodElem;
}

/**
 * \brief Creates a new child element for in the given parent and sets its content.
 *
 * The content of the new element will be a text string.
 *
 * \param parent The parent of the new element
 * \param name The name of the new element
 * \param value The content of the new element
 *
 * \return The new element
 */
DOMElement *XMLDocument::createElement(DOMElement *parent, const char *name,
		const uint8_t *value) throw(Exception) {

	DOMElement *prodElem = 0;

	try {
		prodElem = this->_doc->createElement(this->toXMLText(name));
		parent->appendChild(prodElem);

		DOMText *prodDataVal = this->_doc->createTextNode(this->toXMLText(value));
		prodElem->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &ex) {
		//TODO: Definir i llançar excepció
	} catch(const DOMException &ex) {
		//TODO: Definir i llançar excepció
	}

	return prodElem;
}

/**
 * \brief Creates a new child element for in the given parent and sets its content.
 *
 * The content of the new element will be a single byte.
 *
 * \param parent The parent of the new element
 * \param name The name of the new element
 * \param value The content of the new element
 *
 * \return The new element
 */
DOMElement *XMLDocument::createElement(DOMElement *parent, const char *name,
		const uint8_t value) throw(Exception) {

	DOMElement *prodElem = 0;

	try {
		prodElem = this->_doc->createElement(this->toXMLText(name));
		parent->appendChild(prodElem);

		char strValue[4];
		XMLString::binToText(static_cast<unsigned int>(value), strValue, 8, 10);
		DOMText *prodDataVal = this->_doc->createTextNode(this->toXMLText(strValue));
		prodElem->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &ex) {
		//TODO: Definir i llançar excepció
	} catch(const DOMException &ex) {
		//TODO: Definir i llançar excepció
	}

	return prodElem;
}

/**
 * \brief Creates a new child element for in the given parent and sets its content.
 *
 * The content of the new element will be an 16-bit unsigned number.
 *
 * \param parent The parent of the new element
 * \param name The name of the new element
 * \param value The content of the new element
 *
 * \return The new element
 */
DOMElement *XMLDocument::createElement(DOMElement *parent, const char *name,
		const uint16_t value) throw(Exception) {

	DOMElement *prodElem = 0;

	try {
		prodElem = this->_doc->createElement(this->toXMLText(name));
		parent->appendChild(prodElem);

		char strValue[8];
		XMLString::binToText(static_cast<unsigned int>(value), strValue, 16, 10);
		DOMText *prodDataVal =
				this->_doc->createTextNode(this->toXMLText(strValue));
		prodElem->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &ex) {
		//TODO: Definir i llançar excepció
	} catch(const DOMException &ex) {
		//TODO: Definir i llançar excepció
	}

	return prodElem;
}

/**
 * \brief Creates a new child element for in the given parent and sets its content.
 *
 * The content of the new element will be a 64-bit unsigned number.
 *
 * \param parent The parent of the new element
 * \param name The name of the new element
 * \param value The content of the new element
 *
 * \return The new element
 */
DOMElement *XMLDocument::createElement(DOMElement *parent, const char *name,
		const uint64_t value) throw(Exception) {

	DOMElement *prodElem = 0;

	try {
		prodElem = this->_doc->createElement(this->toXMLText(name));
		parent->appendChild(prodElem);

		char strValue[32];
		XMLString::binToText(static_cast<unsigned long>(value), strValue, 32, 10);
		DOMText *prodDataVal =
				this->_doc->createTextNode(this->toXMLText(strValue));
		prodElem->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &ex) {
		//TODO: Definir i llançar excepció
	} catch(const DOMException &ex) {
		//TODO: Definir i llançar excepció
	}

	return prodElem;
}

/**
 * \brief Creates a new child element for in the given parent and sets its content.
 *
 * The content of the new element will be Base64 encoded binary data.
 *
 * \param parent The parent of the new element
 * \param name The name of the new element
 * \param buf Pointer to the binary data
 * \param len Size of the data in bytes
 *
 * \return The new element
 */
DOMElement *XMLDocument::createBinaryElement(DOMElement *parent,
		const char *name, const uint8_t *buf, size_t len) throw(Exception) {

	DOMElement *prodElem = 0;

	try {
		prodElem = this->_doc->createElement(this->toXMLText(name));
		parent->appendChild(prodElem);

		XMLSize_t outputSize;
		XMLByte *base64str =
				Base64::encode(reinterpret_cast<const XMLByte *>(buf), len, &outputSize);
		DOMText *prodDataVal =
				this->_doc->createTextNode(this->toXMLText(
						reinterpret_cast<const uint8_t *>(base64str)));
		prodElem->appendChild(prodDataVal);
		delete base64str;

		//If buffer have not been completely encoded
		if(outputSize != len) {
			prodElem = 0;
		}
	} catch(const OutOfMemoryException &ex) {
		//TODO: Definir i llançar excepció
	} catch(const DOMException &ex) {
		//TODO: Definir i llançar excepció
	}

	return prodElem;
}

/**
 * \brief Serializes the XML document into a std::string
 *
 * \param buf Target of the serialized string
 *
 * \return C string representation of the target
 */
const char *XMLDocument::serialize(std::string &buf) {

    DOMImplementationLS *implLS =
    		DOMImplementationRegistry::getDOMImplementation(this->toXMLText("LS"));

    DOMLSSerializer *serializer = implLS->createLSSerializer();

    if (serializer->getDomConfig()->canSetParameter(
    		XMLUni::fgDOMWRTFormatPrettyPrint, true)) {
    	serializer->getDomConfig()->setParameter(
    			XMLUni::fgDOMWRTFormatPrettyPrint, true);
	}

    MemBufFormatTarget target;

    DOMLSOutput *output = implLS->createLSOutput();
    output->setByteStream(&target);

    serializer->write(this->_doc, output);

	buf = reinterpret_cast<const char*>(target.getRawBuffer());

    serializer->release();
    output->release();

	return buf.c_str();
}

/**
 * \brief Populates this->_doc with the XML file contained in the given buffer.
 *
 * \param buf Source XML text
 */
void XMLDocument::openFromMem(const char *buf) {
	DOMImplementationLS *implLS =
			DOMImplementationRegistry::getDOMImplementation(this->toXMLText("LS"));

    this->_parser =
    		implLS->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMConfiguration *config = this->_parser->getDomConfig();

	config->setParameter(XMLUni::fgDOMNamespaces, true);
    config->setParameter(XMLUni::fgXercesSchema, true);
	config->setParameter(XMLUni::fgXercesHandleMultipleImports, true);
    config->setParameter(XMLUni::fgXercesSchemaFullChecking, true);

	this->_parser->resetDocumentPool();

	MemBufInputSource source(reinterpret_cast<const XMLByte*>(buf),
			strlen(buf), XML_ROOT_ELEMENT);

    DOMLSInput *input = implLS->createLSInput();
    input->setByteStream(&source);
	this->_doc = this->_parser->parse(input);
}

/**
 * \brief Returns the text content of the given element
 *
 * \param parent The parent of the element
 * \param name The name of the element
 *
 * \return A C string with the text content of the element
 */
const uint8_t *XMLDocument::getElementValueCString(const DOMElement *parent,
		const char *name) {

	const uint8_t *retVal = 0;

	DOMNodeList *nodeList = parent->getElementsByTagName(this->toXMLText(name));

	if(nodeList->getLength() == 1) {
		const XMLCh *content = nodeList->item(0)->getTextContent();
		if(XMLString::stringLen(content)!=0) {
			retVal = this->toCString(content);
		}
	}

	return retVal;
}

/**
 * \brief Returns the content of the given element.
 *
 * The returned content will be a single byte.
 *
 * \param parent The parent of the element
 * \param name The name of the element
 *
 * \return A byte with the content of the element
 */
const uint8_t XMLDocument::getElementValueU8(const DOMElement *parent,
		const char *name) {
	uint8_t retVal = 0;

	DOMNodeList *nodeList = parent->getElementsByTagName(this->toXMLText(name));

	if(nodeList->getLength() == 1) {
		unsigned int buf;
		XMLString::textToBin(nodeList->item(0)->getTextContent(), buf);
		retVal = static_cast<uint8_t>(buf);
	}

	return retVal;
}

/**
 * \brief Returns the content of the given element.
 *
 * The returned content will be a 16-bit unsigned number.
 *
 * \param parent The parent of the element
 * \param name The name of the element
 *
 * \return A 16-bit unsigned number with the content of the element
 */
const uint16_t XMLDocument::getElementValueU16(const DOMElement *parent,
		const char *name) {
	uint16_t retVal = 0;

	DOMNodeList *nodeList = parent->getElementsByTagName(this->toXMLText(name));

	if(nodeList->getLength() == 1) {
		unsigned int buf;
		XMLString::textToBin(nodeList->item(0)->getTextContent(), buf);
		retVal = static_cast<uint16_t>(buf);
	}

	return retVal;
}

/**
 * \brief Returns the content of the given element.
 *
 * The returned content will be a 64-bit unsigned number.
 *
 * \param parent The parent of the element
 * \param name The name of the element
 *
 * \return A 64-bit unsigned number with the content of the element
 */
const uint64_t XMLDocument::getElementValueU64(const DOMElement *parent,
		const char *name) {
	uint64_t retVal = 0;

	DOMNodeList *nodeList = parent->getElementsByTagName(this->toXMLText(name));

	if(nodeList->getLength() == 1) {
		unsigned int buf;
		XMLString::textToBin(nodeList->item(0)->getTextContent(), buf);
		retVal = static_cast<uint64_t>(buf);
	}

	return retVal;
}

/**
 * \brief Returns the content of the given element.
 *
 * The content of the given element must be a Base64 encoded string and the
 * returned value will be its byte array representation.
 *
 * \param parent The parent of the element
 * \param name The name of the element
 *
 * \return A byte array with the content of the element
 */
const uint8_t *XMLDocument::getElementValueBinary(const DOMElement *parent,
		const char *name) {
	uint8_t *retVal = 0;

	DOMNodeList *nodeList = parent->getElementsByTagName(this->toXMLText(name));

	if(nodeList->getLength() == 1) {
		const XMLCh *content = nodeList->item(0)->getTextContent();
		XMLSize_t outputSize;
		XMLByte *binaryContent = Base64::decodeToXMLByte(content, &outputSize);
		if(outputSize > 0) {
			retVal = reinterpret_cast<uint8_t *>(binaryContent);
			this->_listXmlByteData.push_back(retVal);
		}
	}

	return retVal;
}

/**
 * \brief Return a single element
 *
 * There must be only one element with the given name in the given parent.
 * Otherwise nothing is returned.
 *
 * \param parent Parent of the requested element
 * \param name Name of the requested element
 *
 * \return The requested element
 */
const DOMElement *XMLDocument::getElement(const DOMElement *parent,
		const char *name) {

	DOMElement *retVal = 0;

	DOMNodeList *nodeList = parent->getElementsByTagName(this->toXMLText(name));

	if(nodeList->getLength() == 1) {
		retVal = dynamic_cast<DOMElement *>(nodeList->item(0));
	}

	return retVal;
}

/**
 * \brief Returns zero or more elements
 *
 * \param parent Parent of the requested elements
 * \param name Name of the requested elements
 *
 * \return A list with the requested elements
 */
const DOMNodeList *XMLDocument::getElements(const DOMElement *parent,
		const char *name) {

	DOMNodeList *retVal = parent->getElementsByTagName(this->toXMLText(name));

	return retVal;
}

/**
 * \brief Class destructor.
 *
 * Releases the allocated memory.
 */
XMLDocument::~XMLDocument() {
	// Release the parse if any
	if(this->_parser != 0) {
		this->_parser->release();
	}

	//Release the transcoded data
	for (std::vector<XMLCh*>::iterator it = this->_listXmlchData.begin();
			it!=this->_listXmlchData.end(); ++it) {
		XMLString::release(&(*it));
	}
	this->_listXmlchData.clear();

	//Release the byte data
	for (std::vector<uint8_t*>::iterator it = this->_listXmlByteData.begin();
			it!=this->_listXmlByteData.end(); ++it) {
		delete *it;
	}
	this->_listXmlByteData.clear();

	//Release the XML4C2 system.
	XMLPlatformUtils::Terminate();
}

}
