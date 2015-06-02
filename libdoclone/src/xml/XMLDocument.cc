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
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/Base64.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include <doclone/xml/XMLStringHandler.h>
#include <doclone/xml/XMLErrorHandler.h>
#include <doclone/exception/CreateImageException.h>
#include <doclone/exception/XMLParseException.h>

namespace Doclone {

XERCES_CPP_NAMESPACE_USE

/**
 * Class constructor.
 *
 * Initializes the Xerces library and the class attributes
 */
XMLDocument::XMLDocument() : _doc(), _parser() {
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
}

/**
 * \brief Creates a new XML document for writing and sotres it in this->_doc
 *
 * \param rootNode The root node of the new XML
 */
void XMLDocument::createNew() {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::createNew() start");

	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	DOMImplementation *impl =
			DOMImplementationRegistry::getDOMImplementation(
					xmlStr->toXMLText(Doclone::XML_FEATURE));

	DOMDocumentType *docType =
			impl->createDocumentType(xmlStr->toXMLText(Doclone::XML_ROOT_ELEMENT),
					xmlStr->toXMLText(Doclone::XML_PUBLIC_ID),
					xmlStr->toXMLText(Doclone::XML_SYSTEM_ID));

	this->_doc = impl->createDocument(0, xmlStr->toXMLText(Doclone::XML_ROOT_ELEMENT), docType);

	log->debug("XMLDocument::createNew() end");
}

/**
 * \brief Returns the root element of this document
 *
 * \return Root element of the XML
 */
DOMElement *XMLDocument::getRootElement() {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getRootElement() start");

	DOMElement *retVal = this->_doc->getDocumentElement();

	log->debug("XMLDocument::getRootElement(retVal=>0x%x) end", retVal);
	return retVal;
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::createElement(parent=>0x%x, name=>%s) start", parent, name);

	DOMElement *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	try {
		retVal = this->_doc->createElement(xmlStr->toXMLText(name));
		parent->appendChild(retVal);

	} catch(const OutOfMemoryException &e) {
		CreateImageException ex;
		throw ex;
	} catch(const DOMException &e) {
		CreateImageException ex;
		throw ex;
	}

	log->debug("XMLDocument::createElement(retVal=>0x%x) end", retVal);
	return retVal;
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::createElement(parent=>0x%x, name=>%s, value=>%s) start", parent, name, value);

	DOMElement *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	try {
		retVal = this->_doc->createElement(xmlStr->toXMLText(name));
		parent->appendChild(retVal);

		DOMText *prodDataVal = this->_doc->createTextNode(xmlStr->toXMLText(value));
		retVal->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &e) {
		CreateImageException ex;
		throw ex;
	} catch(const DOMException &e) {
		CreateImageException ex;
		throw ex;
	}

	log->debug("XMLDocument::createElement(retVal=>0x%x) end", retVal);
	return retVal;
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::createElement(parent=>0x%x, name=>%s, value=>%d) start", parent, name, value);

	DOMElement *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	try {
		retVal = this->_doc->createElement(xmlStr->toXMLText(name));
		parent->appendChild(retVal);

		char strValue[4];
		XMLString::binToText(static_cast<unsigned int>(value), strValue, 8, 10);
		DOMText *prodDataVal = this->_doc->createTextNode(xmlStr->toXMLText(strValue));
		retVal->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &e) {
		CreateImageException ex;
		throw ex;
	} catch(const DOMException &e) {
		CreateImageException ex;
		throw ex;
	}

	log->debug("XMLDocument::createElement(retVal=>0x%x) end", retVal);
	return retVal;
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::createElement(parent=>0x%x, name=>%s, value=>%d) start", parent, name, value);

	DOMElement *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	try {
		retVal = this->_doc->createElement(xmlStr->toXMLText(name));
		parent->appendChild(retVal);

		char strValue[8];
		XMLString::binToText(static_cast<unsigned int>(value), strValue, 16, 10);
		DOMText *prodDataVal =
				this->_doc->createTextNode(xmlStr->toXMLText(strValue));
		retVal->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &x) {
		CreateImageException ex;
		throw ex;
	} catch(const DOMException &e) {
		CreateImageException ex;
		throw ex;
	}

	log->debug("XMLDocument::createElement(retVal=>0x%x) end", retVal);
	return retVal;
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::createElement(parent=>0x%x, name=>%s, value=>%d) start", parent, name, value);

	DOMElement *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	try {
		retVal = this->_doc->createElement(xmlStr->toXMLText(name));
		parent->appendChild(retVal);

		char strValue[32];
		XMLString::binToText(static_cast<unsigned long>(value), strValue, 32, 10);
		DOMText *prodDataVal =
				this->_doc->createTextNode(xmlStr->toXMLText(strValue));
		retVal->appendChild(prodDataVal);

	} catch(const OutOfMemoryException &e) {
		CreateImageException ex;
		throw ex;
	} catch(const DOMException &e) {
		CreateImageException ex;
		throw ex;
	}

	log->debug("XMLDocument::createElement(retVal=>0x%x) end", retVal);
	return retVal;
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::createBinaryElement(parent=>0x%x, name=>%s, buf=>0x%x, len=>%d) start", parent, name, buf, len);

	DOMElement *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	try {
		retVal = this->_doc->createElement(xmlStr->toXMLText(name));
		parent->appendChild(retVal);

		XMLSize_t outputSize;
		XMLByte *base64str =
				Base64::encode(reinterpret_cast<const XMLByte *>(buf), len, &outputSize);
		DOMText *prodDataVal =
				this->_doc->createTextNode(xmlStr->toXMLText(
						reinterpret_cast<const uint8_t *>(base64str)));
		retVal->appendChild(prodDataVal);
		delete base64str;

		//If buffer have not been completely encoded
		if(outputSize != len) {
			retVal = 0;
		}
	} catch(const OutOfMemoryException &e) {
		CreateImageException ex;
		throw ex;
	} catch(const DOMException &e) {
		CreateImageException ex;
		throw ex;
	}

	log->debug("XMLDocument::createElement(retVal=>0x%x) end", retVal);
	return retVal;
}

/**
 * \brief Serializes the XML document into a std::string
 *
 * \param buf Target of the serialized string
 *
 * \return C string representation of the target
 */
const char *XMLDocument::serialize(std::string &buf) {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::serialize(buf=>0x%x) start", &buf);

	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();
	DOMImplementationLS *implLS =
			DOMImplementationRegistry::getDOMImplementation(
					xmlStr->toXMLText("LS"));

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

	log->debug("XMLDocument::serialize(buf.c_str()=>%s) end", buf.c_str());
	return buf.c_str();
}

/**
 * \brief Populates this->_doc with the XML file contained in the given buffer.
 *
 * \param buf Source XML text
 */
void XMLDocument::openFromMem(const char *buf) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::openFromMem(buf=>%s) start", buf);

	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();
	DOMImplementationLS *implLS =
			DOMImplementationRegistry::getDOMImplementation(
					xmlStr->toXMLText("LS"));

	this->_parser =
			implLS->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
	DOMConfiguration *config = this->_parser->getDomConfig();
	XMLErrorHandler errorHandler;

	config->setParameter(XMLUni::fgDOMNamespaces, true);
	config->setParameter(XMLUni::fgXercesSchema, true);
	config->setParameter(XMLUni::fgXercesHandleMultipleImports, true);
	config->setParameter(XMLUni::fgXercesSchemaFullChecking, true);
	config->setParameter(XMLUni::fgDOMErrorHandler, &errorHandler);

	this->_parser->resetDocumentPool();

	MemBufInputSource source(reinterpret_cast<const XMLByte*>(buf),
			strlen(buf), Doclone::XML_ROOT_ELEMENT);

	DOMLSInput *input = implLS->createLSInput();
	input->setEncoding(XMLUni::fgUTF8EncodingString);
	input->setByteStream(&source);
	this->_doc = this->_parser->parse(input);
	if(errorHandler.getErrors()) {
		XMLParseException ex;
		throw ex;
	}

	log->debug("XMLDocument::openFromMem() end");
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getElementValueCString(parent=>0x%x, name=>%s) start", parent, name);

	const uint8_t *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	DOMNodeList *nodeList = parent->getElementsByTagName(xmlStr->toXMLText(name));

	if(nodeList->getLength() == 1) {
		const XMLCh *content = nodeList->item(0)->getTextContent();
		if(XMLString::stringLen(content)!=0) {
			retVal = xmlStr->toCString(content);
		}
	}

	log->debug("XMLDocument::getElementValueCString(retVal=>%s) end", retVal);
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getElementValueU8(parent=>0x%x, name=>%s) start", parent, name);

	uint8_t retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	DOMNodeList *nodeList =
			parent->getElementsByTagName(xmlStr->toXMLText(name));

	if(nodeList->getLength() == 1) {
		unsigned int buf;
		XMLString::textToBin(nodeList->item(0)->getTextContent(), buf);
		retVal = static_cast<uint8_t>(buf);
	}

	log->debug("XMLDocument::getElementValueU8(retVal=>%d) end", retVal);
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getElementValueU16(parent=>0x%x, name=>%s) start", parent, name);

	uint16_t retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	DOMNodeList *nodeList =
			parent->getElementsByTagName(xmlStr->toXMLText(name));

	if(nodeList->getLength() == 1) {
		unsigned int buf;
		XMLString::textToBin(nodeList->item(0)->getTextContent(), buf);
		retVal = static_cast<uint16_t>(buf);
	}

	log->debug("XMLDocument::getElementValueU16(retVal=>%d) end", retVal);
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getElementValueU16(parent=>0x%x, name=>%s) start", parent, name);

	uint64_t retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	DOMNodeList *nodeList = parent->getElementsByTagName(xmlStr->toXMLText(name));

	if(nodeList->getLength() == 1) {
		const char *tmpVal = reinterpret_cast<const char *>(xmlStr->toCString(
				nodeList->item(0)->getTextContent()));
		if(tmpVal != 0) {
			retVal = atol(tmpVal);
		}
	}

	log->debug("XMLDocument::getElementValueU64(retVal=>%d) end", retVal);
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getElementValueBinary(parent=>0x%x, name=>%s) start", parent, name);

	const uint8_t *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	DOMNodeList *nodeList = parent->getElementsByTagName(xmlStr->toXMLText(name));

	if(nodeList->getLength() == 1) {
		const XMLCh *content = nodeList->item(0)->getTextContent();
		XMLSize_t outputSize;
		XMLByte *binaryContent = Base64::decodeToXMLByte(content, &outputSize);
		if(outputSize > 0) {
			retVal = xmlStr->toCString(binaryContent);
		}
	}

	log->debug("XMLDocument::getElementValueBinary(retVal=>%s) end", retVal);
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getElement(parent=>0x%x, name=>%s) start", parent, name);

	DOMElement *retVal = 0;
	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();

	DOMNodeList *nodeList = parent->getElementsByTagName(xmlStr->toXMLText(name));

	if(nodeList->getLength() == 1) {
		retVal = dynamic_cast<DOMElement *>(nodeList->item(0));
	}

	log->debug("XMLDocument::getElement(retVal=>0x%x) end", retVal);
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
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::getElements(parent=>0x%x, name=>%s) start", parent, name);

	XMLStringHandler *xmlStr = XMLStringHandler::getInstance();
	DOMNodeList *retVal = parent->getElementsByTagName(xmlStr->toXMLText(name));

	log->debug("XMLDocument::getElements(retVal=>0x%x) end", retVal);
	return retVal;
}

}
