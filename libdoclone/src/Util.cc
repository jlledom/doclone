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

#include <doclone/Util.h>

#include <stdlib.h>
#include <stdio.h>
#include <mntent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <regex.h>

#include <sstream>
#include <string>
#include <fstream>

#include <blkid/blkid.h>

#include <doclone/Logger.h>
#include <doclone/Clone.h>
#include <doclone/exception/Exception.h>
#include <doclone/exception/WriteDataException.h>
#include <doclone/exception/NoAccessToDeviceException.h>
#include <doclone/exception/FileNotFoundException.h>
#include <doclone/exception/CreateFileException.h>
#include <doclone/exception/OpenFileException.h>
#include <doclone/exception/CloseFileException.h>
#include <doclone/exception/BrokenPipeException.h>
#include <doclone/exception/CancelException.h>
#include <doclone/exception/SignalCaughtException.h>
#include <doclone/exception/NoDeviceDriverRecognizedException.h>
#include <doclone/exception/SigAbrtException.h>
#include <doclone/exception/SpawnProcessException.h>

namespace Doclone {

/**
 * \brief Reads /proc/partitions and returns the path of a device whose
 * major/minor equals to the passed as parameters
 *
 * \param major
 * 		The major of the partition
 * \param minor
 * 		The minor of the partition
 * \param refresh
 * 		If the user wants to re-read /proc/partitions before performing the work
 *
 * \return The path of the device
 */
std::string Util::getPathByMajorMinor (unsigned char major, unsigned char minor, int refresh) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::getPathByMajorMinor(major=>%d, minor=>%d, refresh=>%d) start", major, minor, refresh);
	
	std::string retValue="";
	struct procpart {
		unsigned char major;
		unsigned char minor;
		std::string name;
	};
	static procpart parts[128];
	static int i = 0;
	// if (the array is empty || the caller wanted to refresh the part list)
	if (!i || refresh) {
		FILE *f;
		char line[100], nompt[100];
		int maj, min, tam;
		if (!(f = fopen ("/proc/partitions", "r"))) {
			FileNotFoundException ex("/proc/partitions");
			ex.logMsg();
			throw ex;
		}

		while (fgets (line, sizeof (line), f)) {
			if (sscanf (line, " %d %d %d %[^\n ]", &maj, &min, &tam, nompt) !=4) {
				continue;
			}
			parts[i].major = maj;
			parts[i].minor = min;
			parts[i].name = nompt;
			i++;
		}
		fclose(f);
	}
	int j;
	for (j = 0; j < i; j++) {
		if ((parts[j].major == major) && (parts[j].minor == minor)) {
			retValue="/dev/"+parts[j].name;
			break;
		}
	}
	
	log->debug("Util::getPathByMajorMinor(retValue=>%s) end", retValue.c_str());
	return retValue;
}

/**
 * \brief Gets the major of a device by its path
 *
 * \param dev
 * 		The path of the device
 * \return The major
 */
uint8_t Util::getMajor (const std::string &dev) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::getMajor(dev=>%s) start", dev.c_str());
	
	struct stat info_d;
	if (stat (dev.c_str(), &info_d) < 0) {
		NoAccessToDeviceException ex(dev);
		throw ex;
	}
	
	unsigned char retValue = info_d.st_rdev >> 8;
	
	log->debug("Util::getMajor(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Gets the minor of a device by its path
 *
 * \param dev
 * 		The path of the device
 * \return The minor
 */
uint8_t Util::getMinor (const std::string &dev) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::getMinor(dev=>%s) start", dev.c_str());
	
	struct stat info_d;
	if (stat (dev.c_str(), &info_d) < 0) {
		NoAccessToDeviceException ex(dev);
		throw ex;
	}
	
	unsigned char retValue = info_d.st_rdev & 0xff;
	
	log->debug("Util::getMinor(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Get the max number of minors allowed for the device passed as
 * parameter.
 *
 * Works either if the device is a disk or a partition.
 *
 * \return The number of minors.
 */
uint8_t Util::getNumberOfMinors(const std::string &dev) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::getNumberOfMinors(dev=>%s) start", dev.c_str());

	uint8_t retValue = 0;

	uint8_t major = Util::getMajor(dev);

	switch(major) {
	case IDE_MAJOR : {
		retValue = IDE_MINORS_BY_DEVICE;
		break;
	}
	case SCSI_MAJOR: {
		retValue = SCSI_MINORS_BY_DEVICE;
		break;
	}
	default: {
		NoDeviceDriverRecognizedException ex;
		ex.logMsg();
		break;
	}
	}

	log->debug("Util::getNumberOfMinors(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Gets the path of the disk that owns the specified partition
 *
 * \param path The path of the partition
 * \return String with the path of the disk
 */
std::string Util::getDiskPath(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::getDiskPath(path=>%s) start", path.c_str());
	
	int disk_major, disk_minor;
	disk_major = Util::getMajor(path.c_str());
	disk_minor = Util::getMinor(path.c_str());
	
	disk_minor -= disk_minor % (Util::getNumberOfMinors(path));
	std::string devPath = Util::getPathByMajorMinor (disk_major, disk_minor, 0);
	
	if(devPath.compare("") == 0) {
		NoAccessToDeviceException ex(path);
		throw ex;
	}
	
	log->debug("Util::getDiskPath(devPath=>%s) end", devPath.c_str());
	return devPath;
}

/**
 * \brief Gets the number of the partition in the disk.
 *
 *	For example: for /dev/sda3 => 3
 *
 * \param path The path of the partition
 * \return Number of the partition
 */
uint8_t Util::getPartNum(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::getPartNum(path=>%s) start", path.c_str());

	uint8_t retVal =
			Util::getMinor (path.c_str()) % (Util::getNumberOfMinors(path));

	log->debug("Util::getPartNum(retVal=>%d) end", retVal);
	return retVal;
}

/**
 * \brief Calculates the size in bytes of a file
 *
 * \param path
 * 		The file path.
 *
 * \return Size in bytes
 */
uint64_t Util::getFileSize(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::getFileSize(path=>%s) start", path.c_str());

	uint64_t retVal = 0;

	std::ifstream istr(path.c_str(), std::ios::in);

	istr.seekg(0, std::ios::end);

	retVal = istr.tellg();

	log->debug("Util::getFileSize(retVal=>%d) end", retVal);

	return retVal;
}

/**
 * \brief Writes [size] bytes of [data] in the [file] specified, starting at
 * [offset]
 *
 * \param file The path of the file
 * \param data Buffer of data to write
 * \param offset Byte where the data will be placed
 * \param size Number of bytes to write
 */
void Util::writeBinData(const std::string &file, const void *data, unsigned int offset, unsigned int size) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::writeBinData(file=>%s, data=>0x%x, offset=>%d, size=>%d) start", file.c_str(), data, offset, size);
	
	std::ofstream fstr(file.c_str(),
			std::fstream::in|std::fstream::out|std::fstream::binary);
	
	if(!fstr) {
		WriteDataException ex;
		throw ex;
	}

	try {
		fstr.seekp(offset, std::ios_base::beg);
		fstr.write (reinterpret_cast<const char*>(data), size);
	}catch(...) {
		fstr.close();

		WriteDataException ex;
		throw ex;
	}
	fstr.close();
	
	log->debug("Util::writeBinData() end");
}

/**
 * \brief Adds a new line in /etc/mtab, called when the program mounts a
 * partition
 *
 * \param partPath The path of the partition
 * \param mountPoint The directory where the partition is mounted
 * \param mountName The mounted filesystem
 * \param mountOptions The mount options
 */
void Util::addMtabEntry(const std::string &partPath, const std::string &mountPoint,
	const std::string &mountName, const std::string &mountOptions) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::addMtabEntry(partPath=>%s, mountPoint=>%s, mountName=>%s, mountOptions=>%s) start",
		partPath.c_str(), mountPoint.c_str(), mountName.c_str(), mountOptions.c_str());
	
	struct mntent filesys;
	FILE *f;
	f = setmntent ("/etc/mtab", "a+");
	if (!f) {
		FileNotFoundException ex("/etc/mtab");
		throw ex;
	}
	
	filesys.mnt_fsname = const_cast<char*>(partPath.c_str());
	filesys.mnt_dir = const_cast<char*>(mountPoint.c_str());
	filesys.mnt_type = const_cast<char*>(mountName.c_str());
	filesys.mnt_opts = const_cast<char*>(mountOptions.c_str());
	filesys.mnt_freq = 0;
	filesys.mnt_passno = 0;
	addmntent(f, &filesys);

	endmntent (f);
	
	log->debug("Util::addMtabEntry() end");
}

/**
 * \brief Updates /etc/mtab whan a partition is unmounted
 *
 * \param partPath
 * 	The path of the partition which will must be removed from /etc/mtab
 */
void Util::updateMtab(const std::string &partPath) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::updateMtab(partPath=>%s) start", partPath.c_str());
	
	// We need to remove the current /etc/mtab and create another one.

	FILE *fp,*fp2;
	struct mntent *tmp;
	
	fp = setmntent("/etc/mtab","r");
	fp2 = setmntent("/etc/mtab.temp","w");

	if(fp == 0) {
		FileNotFoundException ex("/etc/mtab");
		ex.logMsg();
	}

	if(fp2 == 0) {
		CreateFileException ex("/etc/mtab.temp");
		ex.logMsg();
	}

	// create a new mtab with all entries except the one passed as a parameter
	while( (tmp = getmntent(fp)) != 0) {
		// if doesn't match, add
		if(partPath.compare(tmp->mnt_fsname) != 0) {
			addmntent(fp2,tmp);
		}
	}

	endmntent(fp);
	endmntent(fp2);

	remove("/etc/mtab");
	rename("/etc/mtab.temp","/etc/mtab");
	
	log->debug("Util::updateMtab() end");
}

/**
 * \brief Gets a disk path and a partition number and returns the partition path
 *
 * \param path
 * 		The path of the disk
 * \param num
 * 		The number of the partition
 *
 * \return The path of the partition
 */
std::string Util::buildPartPath(const std::string &path, int num) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::buildPartPath(path=>%s, num=>%d) start", path.c_str(), num);
	
	std::stringstream ss;
	ss << num;
	std::string partPath = path+ss.str();
	
	log->debug("Util::buildPartPath(partPath=>%s) end", partPath.c_str());
	return partPath;
}

/**
 * \brief Creates a new file in disk
 *
 * \param name
 * 		The path of the file
 */
void Util::createFile(const std::string &name) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::createFile(name=>%s) start", name.c_str());

	std::ofstream fstr(name.c_str(), std::fstream::trunc|std::fstream::binary);

	if(!fstr) {
		CreateFileException ex(name);
		throw ex;
	}

	log->debug("Util::createFile() end");
}

/**
 * \brief Opens a new file in disk
 *
 * \param file
 * 		The path of the file
 *
 * \return The file descriptor
 */
int Util::openFile(const std::string &file) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::openFile(file=>%s) start", file.c_str());

	int fd = 0;

	if((fd = open(file.c_str(), O_RDWR|O_APPEND))<0) {
		OpenFileException ex(file);
		throw ex;
	}

	log->debug("Util::openFile(fd=>%d) end", fd);
	return fd;
}

/**
 * \brief Closes a file descriptor
 *
 * \param fd
 * 		The file descriptor
 */
void Util::closeFile(int fd) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::closeFile(fd=>%d) start", fd);

	if(fd) {
		if(close(fd)<0) {
			CloseFileException ex;
			ex.logMsg();
			throw ex;
		}
	}

	log->debug("Util::closeFile() end");
	return;
}

/**
 * \brief Gets the path of a file to determine if it is a block device
 *
 * \param path
 * 		The path of the file
 */
bool Util::isBlockDevice(const std::string &path) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::isBlockDevice() start");

	bool retValue = false;
	struct stat filestat;

	if (!stat (path.c_str(), &filestat)
		&& S_ISBLK (filestat.st_mode)) {
		retValue = true;
	}
	else {
		retValue = false;
	}

	log->debug("Util::isBlockDevice(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Checks if the device is disk or partition
 *
 * \return Boolean value
 */
bool Util::isDisk(const std::string &device) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::isDisk(device=>%s) start", device.c_str());

	bool retValue;

	if((Util::getMinor(device) % (Util::getNumberOfMinors(device))) == 0) {
		retValue = true;
	}
	else {
		retValue = false;
	}

	log->debug("Util::isDisk(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * Check if a string matches with the regular expression passed as parameter
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
	Logger *log = Logger::getInstance();
	log->debug("Util::match(str=>%s, regEx=>%s) start", str.c_str(), regEx.c_str());
	bool retVal = false;

	regex_t preg ;
	int nmatch = 1 ;
	regmatch_t pmatch[  nmatch ] ;
	int rc = regcomp( &preg, regEx.c_str(), REG_EXTENDED | REG_ICASE | REG_NEWLINE ) ;
	if(rc == 0) {
		if (regexec(&preg, str.c_str(), nmatch, pmatch, 0) == 0) {
			// If it matches
			retVal = true;
		}
	}

	regfree(&preg);

	log->debug("Util::match(retVal=>%d) end", retVal);
	return retVal;
}

/**
 * \brief Converts a number to a std::string
 *
 * \param num
 * 		The number of the partition
 *
 * \return The number in a string
 */
std::string Util::intToString(int num) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::intToString(num=>%d) start", num);

	std::stringstream ss;
	ss << num;
	std::string retVal = ss.str();

	log->debug("Util::intToString(retVal=>%s) end", retVal.c_str());
	return retVal;
}

/**
 * \brief Determines whether the given folder is virtual or not
 *
 * \param path
 * 		The path to the folder to check
 *
 * 	\return true or false
 */
bool Util::isVirtualDirectory(const char *path) {
	Logger *log = Logger::getInstance();
	log->debug("Util::isVirtualDirectory(path=>%s) start", path);

	bool retVal = !(strcmp ("/dev", path)
			&& strcmp ("/proc",path)
			&& strcmp ("/var/run", path)
			&& strcmp ("/tmp",path)
			&& strcmp ("/media",path)
			&& strcmp ("/mnt",path)
			&& strcmp ("/sys",path)
			&& strcmp ("/run",path));

	log->debug("Util::isVirtualDirectory(retVal=>%d) end", retVal);
	return retVal;
}

/**
 * \brief Determines whether the given file must not be cloned
 *
 * \param path
 * 		The path to the file to check
 *
 * 	\return true if the file have to be bypassed, false otherwise
 */
bool Util::isLiveFile(const char *path) {
	Logger *log = Logger::getInstance();
	log->debug("Util::isLiveFile(path=>%s) start", path);

	Clone *dcl = Clone::getInstance();

	//The image that is being created must not be cloned.
	bool retVal = !strcmp (dcl->getImage().c_str(), path);

	//FIXME: Add live folders as /proc, /dev...

	log->debug("Util::isLiveFile(retVal=>%d) end", retVal);
	return retVal;
}

/**
 * \brief Determines whether the given UUID is assigned to more than one device
 *
 * \param uuid
 * 		The UUID to check
 *
 * \return true if there are more than one device with the UUID, false otherwise
 */
bool Util::isUUIDRepeated(const char *uuid) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::isUUIDRepeated(uuid=>%s) start", uuid);
	bool retVal = false;

	blkid_dev_iterate iter;
	blkid_dev dev;
	blkid_cache cache = 0;
	int numDevRep = 0;

	blkid_get_cache(&cache, "/dev/null"); //Do not use blkid cache
	int blDev = blkid_probe_all(cache);
	if(blDev != 0) {
		log->debug("Util::isUUIDRepeated(retVal=>%d) end", retVal);
		// Returning true to enforce mounting/unmounting of the device, just in case.
		return true;
	}

	iter = blkid_dev_iterate_begin(cache);
	blkid_dev_set_search(iter, strdup("UUID"), strdup(uuid));
	while (blkid_dev_next(iter, &dev) == 0) {
		dev = blkid_verify(cache, dev);
		if (!dev) {
			continue;
		}
		numDevRep++;
	}
	blkid_dev_iterate_end(iter);

	retVal = numDevRep > 1;
	log->debug("Util::isUUIDRepeated(retVal=>%d) end", retVal);
	return retVal;
}

/**
 * \brief Inverts the byte order of a 32-bit integer
 */
uint32_t Util::swapEndian(uint32_t x) {
	Logger *log = Logger::getInstance();
	log->debug("Util::swapEndian(x=>%d) start", x);

	uint32_t retVal = (
			( x << 24 ) |
			(( x & 0x0000FF00 ) << 8) |
			(( x & 0x00FF0000 ) >> 8) |
			( x >> 24));

	log->debug("Util::swapEndian(retVal=>%d) end", retVal);
	return retVal;
}

/**
 * \brief Inverts the byte order of a 64-bit integer
 */
uint64_t Util::swapEndian(uint64_t x) {
	Logger *log = Logger::getInstance();
	log->debug("Util::swapEndian(x=>%d) start", x);

	uint64_t retVal = (
			( x << 56 ) |
			(( x & 0x000000000000FF00ULL ) << 40) |
			(( x & 0x0000000000FF0000ULL ) << 24) |
			(( x & 0x00000000FF000000ULL ) << 8) |
			(( x & 0x000000FF00000000ULL ) >> 8) |
			(( x & 0x0000FF0000000000ULL ) >> 24) |
			(( x & 0x00FF000000000000ULL ) >> 40) |
			( x >> 56));

	log->debug("Util::swapEndian(retVal=>%d) end", retVal);
	return retVal;
}

/**
 * \brief Binds the signals to their handler
 */
void Util::signalCapture() {
	Logger *log = Logger::getInstance();
	log->debug("Util::signalCapture() start");

	signal(SIGPIPE, Util::signalHandler);
	signal(SIGINT, Util::signalHandler);
	signal(SIGABRT, Util::signalHandler);

	log->debug("Util::signalCapture() end");
	return;
}

/**
 * \brief Captures the signals, preventing the interruption of the program.
 *
 * \param signal
 * 		The captured signal
 */
void Util::signalHandler(int sig) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::signalHandler() start");

	switch (sig) {
		case SIGPIPE: {
			BrokenPipeException ex;
			ex.logMsg();
			throw ex;
			break;
		}
		case SIGINT: {
			CancelException ex;
			throw ex;
			break;
		}
		case SIGABRT: {
			signal(sig, SIG_DFL);
			SigAbrtException ex;
			throw ex;
			break;
		}
		default: {
			SignalCaughtException ex(sig);
			throw ex;
			break;
		}
	}

	log->debug("Util::signalHandler() end");
	return;
}

/**
 * \brief Splits a string into an vector of strings
 *
 * \param string
 * 		String to be splitted
 * \param delim
 * 		Delimitator
 * \param elems
 * 		Address of the vector where the strings will be written
 */
void Util::split(const std::string &string, char delim, std::vector<std::string> &elems) {
	Logger *log = Logger::getInstance();
	log->debug("Util::split(string=>%s, delim=>%c) start", string.c_str(), delim);

	std::stringstream ss(string);
	std::string item;

	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}

	log->debug("Util::split() end");
	return;
}

/**
 * \brief Finds out whether a program is available through the path.
 *
 *
 * \param program
 * 		name of the program
 *
 * \return The path of the program, or an empty string if not found
 */
std::string Util::find_program_in_path(const std::string &program) {
	Logger *log = Logger::getInstance();
	log->debug("Util::find_program_in_path(program=>%s) start", program.c_str());

	std::string path = getenv("PATH");

	std::vector<std::string> elems;
	split(path, ':', elems);

	std::string retVal = "";

	for(unsigned int i=0; i< elems.size(); i++) {
		std::string abPath = elems.at(i)+"/"+program;
		if (access(abPath.c_str(), X_OK) == 0) {
			retVal = abPath;
		}
	}

	log->debug("Util::find_program_in_path(retVal=>%s) end", retVal.c_str());
	return retVal;
}

/**
 * \brief Spawns a new process
 *
 * \param command
 * 		The command line to be executed
 * \param exitValue
 * 		Pointer to an integer where the exit status will be written
 */
void Util::spawn_command_line_sync(std::string &command, int *exitValue) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Util::spawn_command_line_sync(command=>%s) start", command.c_str());

	pid_t pid;
	int status;

	pid = fork();
	if (pid < 0) {
		SpawnProcessException ex(command);
		throw ex;
	} else if (pid == 0) {
		execlp("/bin/sh", "sh", "-c", command.c_str(), (void *)0);
	} else if(waitpid(pid, &status, 0) != pid) {
		SpawnProcessException ex(command);
		throw ex;
	}

	if(WIFEXITED(status)) {
		*exitValue = WEXITSTATUS(status);
	} else {
		SpawnProcessException ex(command);
		throw ex;
	}

	log->debug("Util::spawn_command_line_sync() end");
	return;
}

}
