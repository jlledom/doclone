/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
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

#ifndef UTIL_H_
#define UTIL_H_

#include <doclone/exception/Exception.h>

#include <stdint.h>

#include <string>

#ifndef IDE_MAJOR
/**
 * \def IDE_MAJOR
 *
 * Major number of the IDE device driver in linux kernel. By default 3.
 */
#define IDE_MAJOR 3
#endif /* IDE_MAJOR */

#ifndef SCSI_MAJOR
/**
 * \def SCSI_MAJOR
 *
 * Major number of the SCSI device driver in linux kernel. By default 8.
 */
#define SCSI_MAJOR 8
#endif /* SCSI_MAJOR */

#ifndef IDE_MINORS_BY_DEVICE
/**
 * \def IDE_MINORS_BY_DEVICE
 *
 * The number of minors reserved for each device using the IDE linux driver.
 * By default 64.
 */
#define IDE_MINORS_BY_DEVICE 64
#endif /* IDE_MINORS_BY_DEVICE */

#ifndef SCSI_MINORS_BY_DEVICE
/**
 * \def SCSI_MINORS_BY_DEVICE
 *
 * The number of minors reserved for each device using the SCSI linux driver.
 * By default 16.
 */
#define SCSI_MINORS_BY_DEVICE 16
#endif /* IDE_MINORS_BY_DEVICE */

#ifndef DEV_NULL_PATH
/**
 * \def DEV_NULL_PATH
 *
 * The path of /dev/null
 */
#define DEV_NULL_PATH "/dev/null"
#endif /* DEV_NULL_PATH */

namespace Doclone {

/**
 * \class Util
 * \brief A set of useful functions
 * \date August, 2011
 */
class Util {
public:
	static std::string getPathByMajorMinor (unsigned char major, unsigned char minor, int refresh) throw(Exception);
	static uint8_t getMajor (const std::string &dev) throw(Exception);
	static uint8_t getMinor (const std::string &dev) throw(Exception);
	static uint8_t getNumberOfMinors(const std::string &dev) throw(Exception);
	
	static std::string getDiskPath(const std::string &path) throw(Exception);
	static uint8_t getPartNum(const std::string &path) throw(Exception);
	
	static uint64_t getFileSize(const std::string &path) throw(Exception);

	static void writeBinData(const std::string &file, const void *data, unsigned int offset, unsigned int size) throw(Exception);
	
	static void addMtabEntry(const std::string &partPath, const std::string &mountPoint, const std::string &mountName, const std::string &mountOptions) throw(Exception);
	static void updateMtab(const std::string &partPath) throw(Exception);
	
	static std::string buildPartPath(const std::string &path, int num) throw(Exception);

	static void createFile(const std::string &name) throw(Exception);
	static int openFile(const std::string &file) throw(Exception);
	static void closeFile(int fd) throw(Exception);

	static bool isBlockDevice(const std::string &path) throw(Exception);
	static bool isDisk(const std::string &device) throw(Exception);

	static bool match(const std::string &str, const std::string &regEx);

	static std::string intToString(int num) throw(Exception);

	static bool isVirtualDirectory(const char *path);
	static bool isLiveFile(const char *path);

	static bool isUUIDRepeated(const char *uuid) throw(Exception);

	static uint32_t swapEndian(uint32_t x);
	static uint64_t swapEndian(uint64_t x);

	static void signalCapture();
	static void signalHandler(int s) throw(Exception);
};

}

#endif /* UTIL_H_ */
