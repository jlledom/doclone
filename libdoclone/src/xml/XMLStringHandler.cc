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

#include <doclone/xml/XMLStringHandler.h>

#include <stdint.h>
#include <pthread.h>

#include <vector>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/TransService.hpp>

#include <doclone/Logger.h>
#include <doclone/exception/InitializationException.h>

namespace Doclone {

XERCES_CPP_NAMESPACE_USE

/**
 * Class constructor.
 *
 * Initializes the attributtes
 */
XMLStringHandler::XMLStringHandler() throw(Exception):
		_listXmlchData(), _listXmlByteData() {
	// Initialize the XML4C2 system.
	try {
		XMLPlatformUtils::Initialize();
	} catch(const XMLException& toCatch) {
		InitializationException ex;
		throw ex;
	}
}

/**
 * \brief Class destructor.
 *
 * Releases the allocated memory.
 */
XMLStringHandler::~XMLStringHandler() {
	//Release the transcoded data
	for (std::vector<const XMLCh*>::iterator it = this->_listXmlchData.begin();
			it!=this->_listXmlchData.end(); ++it) {
		XMLCh *ptr = const_cast<XMLCh*>(*it);
		XMLString::release(&ptr);
	}
	this->_listXmlchData.clear();

	//Release the byte data
	for (std::vector<const uint8_t*>::iterator it = this->_listXmlByteData.begin();
			it!=this->_listXmlByteData.end(); ++it) {
		delete *it;
	}
	this->_listXmlByteData.clear();

	//Release the XML4C2 system.
	XMLPlatformUtils::Terminate();
}

/**
 * \brief Singleton stuff
 *
 * \return Logger* object
 */
XMLStringHandler* XMLStringHandler::getInstance() {
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mutex);

	static XMLStringHandler instance;

	pthread_mutex_unlock(&mutex);

	return &instance;
}

/**
 * \brief Transforms a C string into a transcoded XML string
 *
 * \param c_str Pointer to the C string to be transcoded
 *
 * \return An transcoded string
 */
const XMLCh *XMLStringHandler::toXMLText(const char *c_str) {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::toXMLText(c_str=>%s) start", c_str);

	XMLCh *retVal = XMLString::transcode(c_str);
	this->_listXmlchData.push_back(retVal);

	log->debug("XMLDocument::toXMLText(retVal=>0x%x) end", retVal);
	return retVal;
}

/**
 * \brief Transforms a transcoded XML string into a C string
 *
 * \param xmlch Pointer to the transcoded XML string
 *
 * \return An old C string
 */
const char *XMLStringHandler::toCString(const XMLCh *xmlch) {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::toCString(xmlch=>0x%x) start", xmlch);

	TranscodeToStr trns(xmlch, "UTF-8");
	char *retVal = reinterpret_cast<char *>(trns.adopt());

	this->_listXmlByteData.push_back(reinterpret_cast<uint8_t *>(retVal));

	log->debug("XMLDocument::toCString(retVal=>%s) end", retVal);
	return retVal;
}

/**
 * \brief Transforms a C string into a XML byte
 *
 * \param c_str Pointer to the C string to be transcoded
 *
 * \return An XMLByte array
 */
const XMLByte *XMLStringHandler::toXMLByteArray(const uint8_t *data, bool adopt) {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::toXMLByteArray(data=>0x%x) start", data);

	const XMLByte *retVal = reinterpret_cast<const XMLByte *>(data);
	if(adopt){
		this->_listXmlByteData.push_back(data);
	}

	log->debug("XMLDocument::toXMLByteArray(retVal=>0x%x) end", retVal);
	return retVal;
}

/**
 * \brief Transforms a XML byte array into a C binary array
 *
 * \param xmlbyte Pointer to the XML byte
 *
 * \return Binary array
 */
const uint8_t *XMLStringHandler::toBinaryArray(const XMLByte *xmlbyte, bool adopt) {
	Logger *log = Logger::getInstance();
	log->debug("XMLDocument::toCString(xmlbyte=>0x%x) start", xmlbyte);

	const uint8_t *retVal = reinterpret_cast<const uint8_t *>(xmlbyte);
	if(adopt) {
		this->_listXmlByteData.push_back(retVal);
	}

	log->debug("XMLDocument::toCString(retVal=>%s) end", retVal);
	return retVal;
}
}
