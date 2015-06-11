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

#include <doclone/Image.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <endian.h>
#include <time.h>
#include <dirent.h>

#include <sstream>
#include <string>
#include <vector>

#include <config.h>
#include <archive.h>
#include <archive_entry.h>

#include <xercesc/dom/DOM.hpp>

#include <doclone/Clone.h>
#include <doclone/Logger.h>
#include <doclone/Operation.h>
#include <doclone/DataTransfer.h>
#include <doclone/DlFactory.h>
#include <doclone/FsFactory.h>
#include <doclone/xml/XMLDocument.h>
#include <doclone/exception/Exception.h>
#include <doclone/exception/ErrorException.h>
#include <doclone/exception/WarningException.h>
#include <doclone/exception/InitializationException.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/InvalidImageException.h>
#include <doclone/exception/WrongImageTypeException.h>
#include <doclone/exception/NoFsToolFoundException.h>
#include <doclone/exception/NoUuidSupportException.h>
#include <doclone/exception/NoLabelSupportException.h>
#include <doclone/exception/NoMountSupportException.h>
#include <doclone/exception/NoSelinuxSupportException.h>
#include <doclone/exception/NoFitInDeviceException.h>
#include <doclone/exception/TooMuchPartitionsException.h>
#include <doclone/exception/FileNotFoundException.h>
#include <doclone/exception/ReadErrorsInDirectoryException.h>
#include <doclone/exception/WriteErrorsInDirectoryException.h>
#include <doclone/exception/CancelException.h>
#include <doclone/exception/ReadDataException.h>
#include <doclone/exception/SendDataException.h>
#include <doclone/exception/WriteDataException.h>
#include <doclone/exception/ReceiveDataException.h>

namespace Doclone {

/**
 * \brief Initializes attributes
 */
Image::Image(): _size(), _type(), _disk(), _archiveIn(), _archivesOut() {
	Clone *dcl = Clone::getInstance();
	this->_noData = dcl->getEmpty();
}

/**
 * \brief Checks if the image entered by the user is really a doclone image.
 *
 * To determine if the passed file is a valid doclone image, this function amounts
 * every min_size of each partition and compare it with the header image_size.
 * If the amount of min_size's is equal to image_size, the passed file is a
 * valid doclone image.
 */
bool Image::isValid() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::isValid() start");

	uint64_t amount_bytes = 0;
	bool retValue;

	std::vector<Partition *> partitions = this->_disk->getPartitions();
	for (unsigned int i = 0; i<partitions.size(); i++) {
		amount_bytes += partitions.at(i)->getMinSize();
	}

	if (amount_bytes != this->_size) {
		retValue = false;
	}
	else {
		retValue = true;
	}

	log->debug("Image::isValid(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Makes this->_archiveIn be a disk read archive
 */
void Image::initDiskReadArchive() {
	Logger *log = Logger::getInstance();
	log->debug("Image::initDiskRead() start");

	this->_archiveIn = archive_read_disk_new();
	archive_read_disk_set_symlink_physical(this->_archiveIn);

	log->debug("Image::initDiskRead() end");
}

/**
 * \brief Makes this->_archiveIn be a descriptor read archive
 */
void Image::initFdReadArchive(const int fdin) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::initFdRead(fdin=>%d) start", fdin);

	this->_archiveIn = archive_read_new();
	archive_read_support_format_tar(this->_archiveIn);
	archive_read_support_compression_gzip(this->_archiveIn);

	if(archive_read_open_fd(this->_archiveIn,
			fdin, Doclone::BUFFER_SIZE) != ARCHIVE_OK) {
		InitializationException ex;
		throw ex;
	}

	log->debug("Image::initFdRead() end");
}

/**
 * \brief Makes this->_archivesOut[0] be a disk write archive
 */
void  Image::initDiskWriteArchive() {
	Logger *log = Logger::getInstance();
	log->debug("Image::initDiskWrite() start");

	 struct archive *arch = archive_write_disk_new();

	int flags = ARCHIVE_EXTRACT_OWNER;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;
	flags |= ARCHIVE_EXTRACT_XATTR;
	flags |= ARCHIVE_EXTRACT_UNLINK;

	archive_write_disk_set_options(arch, flags);
	this->_archivesOut.push_back(arch);

	log->debug("Image::initDiskWrite() end");
}

/**
 * \brief Makes this->_archivesOut[0] be a write archive for [fdout]
 *
 * \param fdout
 * 		Descriptor where archive will write
 */
void Image::initFdWriteArchive(const int fdout) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::initFdWrite(fdout=>%d) start", fdout);

	struct archive *arch = archive_write_new();
	archive_write_add_filter_gzip(arch);
	archive_write_set_format_pax(arch);

	if(archive_write_open_fd(arch, fdout) != ARCHIVE_OK) {
		InitializationException ex;
		throw ex;
	}

	this->_archivesOut.push_back(arch);

	log->debug("Image::initFdWrite() end");
}

/**
 * \brief For each descriptor in [fds], create an archive for writing to it
 *
 * \param fds
 * 		Vector of descriptors
 */
void Image::initFdWriteArchive(std::vector<int> &fds) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::initFdWrite(fds=>0x%x) start", &fds);

	std::vector<int>::iterator it;
	for(it = fds.begin(); it != fds.end(); ++it) {
		struct archive *arch = archive_write_new();
		archive_write_add_filter_gzip(arch);
		archive_write_set_format_pax(arch);

		if(archive_write_open_fd(arch, *it) != ARCHIVE_OK) {
			InitializationException ex;
			throw ex;
		}

		this->_archivesOut.push_back(arch);
	}

	log->debug("Image::initFdWrite() end");
}

/**
 * \brief Free allocated memory for read archive
 */
void Image::freeReadArchive() {
	Logger *log = Logger::getInstance();
	log->debug("Image::freeReadArchive() start");

	archive_read_close(this->_archiveIn);
	archive_read_free(this->_archiveIn);

	log->debug("Image::freeReadArchive() end");
}

/**
 * \brief Free allocated memory for write archive
 */
void Image::freeWriteArchive() {
	Logger *log = Logger::getInstance();
	log->debug("Image::freeReadArchive() start");

	std::vector<struct archive*>::iterator it;
	for(it = this->_archivesOut.begin(); it != this->_archivesOut.end(); ++it) {
		archive_write_close(*it);
		archive_write_free(*it);
	}

	log->debug("Image::freeReadArchive() end");
}

/**
 * \brief Checks if image fits in the specified device
 *
 * \param dcDisk
 * 		For get the size of the disk.
 */
bool Image::fitInDisk() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::fitInDisk() start");

	bool retValue = this->_size < this->_disk->getSize();

	log->debug("Image::fitInDisk(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Reads the all the necessary metadata from the image header.
 *
 * \return Number of bytes read
 */
void Image::loadImageHeader() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::loadImageHeader() start");

	DataTransfer *trns = DataTransfer::getInstance();
	struct archive_entry *entry;
	std::string xmlText;

	if(archive_read_next_header(this->_archiveIn, &entry) != ARCHIVE_OK) {
		ReadDataException ex;
		throw ex;
	}

	trns->archiveToBuf(this->_archiveIn, xmlText);
	XMLDocument doc;
	doc.openFromMem(xmlText.c_str());

	DOMElement *rootElement=doc.getRootElement();

	uint8_t numPartitions = doc.getElementValueU8(
			rootElement, "numPartitions");

	this->_size = doc.getElementValueU64(rootElement, "imageSize");
	this->_type = static_cast<Doclone::imageType>(
			doc.getElementValueU8(rootElement, "imageType"));

	this->_disk = DlFactory::createDiskLabel();
	this->_disk->setLabelType(
			static_cast<Doclone::diskLabelType>(
					doc.getElementValueU8(rootElement, "diskType")));

	if(this->_type == Doclone::IMAGE_DISK) {
		const uint8_t *bootCode =
				doc.getElementValueBinary(rootElement, "bootCode");
		uint8_t buffBootCode[Doclone::MBR_SIZE] = {};
		for(int i=0; i<Doclone::MBR_SIZE; i++) {
			buffBootCode[i] = bootCode[i];
		}
		this->_disk->setBootCode(reinterpret_cast<const char *>(buffBootCode));
	}

	const DOMElement *partitions = doc.getElement(rootElement, "partitions");
	const DOMNodeList *partitionList = doc.getElements(partitions, "partition");

	for(int i = 0; i<numPartitions; i++) {
		DOMElement *xmlPartition =
				dynamic_cast<DOMElement *>(partitionList->item(i));

		Partition *part = new Partition();
		if(this->_type == Doclone::IMAGE_PARTITION) {
			Clone *dcl = Clone::getInstance();
			part->setPath(dcl->getDevice());
			part->setPartNum(Util::getPartNum(dcl->getDevice()));
		}

		part->setType(
				(Doclone::partType)doc.getElementValueU8(xmlPartition, "type"));

		Filesystem *fs = 0;
		try {
			const char *fsName =doc.getElementValueCString(xmlPartition, "fsName");

			fs = FsFactory::createFilesystem(fsName?fsName:"");
		} catch (const Exception &ex) {
			fs = FsFactory::createFilesystem("nofs");
		}
		part->setFileSystem(fs);

		part->setMinSize(doc.getElementValueU64(xmlPartition, "minSize"));

		double startPos = Util::stringToDouble(
						doc.getElementValueCString(xmlPartition, "startPos"));
		if(startPos > 1) {
			InvalidImageException ex;
			throw ex;
		}
		part->setStartPos(startPos);

		double usedPart = Util::stringToDouble(
				doc.getElementValueCString(xmlPartition, "usedPart"));
		if(usedPart>1) {
			InvalidImageException ex;
			throw ex;
		}
		part->setUsedPart(usedPart);

		// Flags
		part->setFlags(
				doc.getElementValueU8(xmlPartition, "flags"));

		// Label
		const char *label = doc.getElementValueCString(xmlPartition, "label");
		part->getFileSystem()->setLabel(label?label:"");

		// UUID
		const char *uuid = doc.getElementValueCString(xmlPartition, "uuid");
		part->getFileSystem()->setUUID(uuid?uuid:"");

		//Root folder on header
		const char *rootDir = doc.getElementValueCString(xmlPartition, "rootDir");
		part->setRootDir(rootDir?rootDir:"");

		this->_disk->getPartitions().push_back(part);
	}

	if(!this->isValid()) {
		InvalidImageException ex;
		throw ex;
	}

	log->debug("Image::loadImageHeader() end");
}

/**
 * \brief Writes the necessary metadata in the image header.
 *
 * \return Number of bytes written
 */
void Image::saveImageHeader() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::saveImageHeader() start");

	XMLDocument doc;
	doc.createNew();

	uint8_t numPartitions = this->_disk->getPartitions().size();

	uint64_t imageSize = 0;
	for(int i = 0;i<numPartitions;i++) {
		imageSize += this->_disk->getPartitions().at(i)->getMinSize();
	}

	DOMElement *rootElem = doc.getRootElement();
	doc.createElement(rootElem, "numPartitions", numPartitions);
	doc.createElement(rootElem, "imageSize", imageSize);
	doc.createElement(rootElem, "imageType", static_cast<uint8_t>(this->_type));

	doc.createBinaryElement(rootElem, "bootCode",
			reinterpret_cast<const uint8_t*>(this->_disk->getBootCode()), Doclone::MBR_SIZE);

	doc.createElement(rootElem, "diskType",
			static_cast<uint8_t>(this->_disk->getLabelType()));

	DOMElement *partsXML = doc.createElement(rootElem, "partitions");
	for(int i=0; i<numPartitions; i++) {
		Partition *part = this->_disk->getPartitions().at(i);

		//Set the root of this partition inside the image
		std::string rootDir = "_part";
		char partNum[2] = {};
		sprintf(partNum,"%d", part->getPartNum());
		rootDir.append(partNum);
		part->setRootDir(rootDir);

		char startPos[32];
		char usedPart[32];
		DOMElement *partitionXML = doc.createElement(partsXML, "partition");
		doc.createElement(partitionXML, "flags", part->getFlags());
		doc.createElement(partitionXML, "startPos",
				Util::doubletoString(part->getStartPos(), startPos));
		doc.createElement(partitionXML, "usedPart",
				Util::doubletoString(part->getUsedPart(), usedPart));
		doc.createElement(partitionXML, "type",
				static_cast<uint8_t>(part->getType()));
		doc.createElement(partitionXML, "minSize", part->getMinSize());
		doc.createElement(partitionXML, "fsName",
						part->getFileSystem()->getdocloneName().c_str());
		doc.createElement(partitionXML, "label",
						part->getFileSystem()->getLabel().c_str());
		doc.createElement(partitionXML, "uuid",
						part->getFileSystem()->getUUID().c_str());
		doc.createElement(partitionXML, "rootDir",rootDir.c_str());
	}

	std::string xmlSer;
	doc.serialize(xmlSer);

	time_t timeNow = time(0);

	struct archive_entry *headerEntry = archive_entry_new();
	archive_entry_set_pathname(headerEntry, "_header.xml");
	archive_entry_set_filetype(headerEntry, AE_IFREG);
	archive_entry_set_size(headerEntry, xmlSer.length());
	archive_entry_set_perm(headerEntry, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	archive_entry_set_atime(headerEntry, timeNow, 0);
	archive_entry_set_birthtime(headerEntry, timeNow, 0);
	archive_entry_set_ctime(headerEntry, timeNow, 0);
	archive_entry_set_mtime(headerEntry, timeNow, 0);

	DataTransfer *trns = DataTransfer::getInstance();
	trns->copyHeader(headerEntry, this->_archivesOut);
	trns->bufToArchive(xmlSer, this->_archivesOut);

	archive_entry_free(headerEntry);

	log->debug("Image::saveImageHeader() end");
}

/**
 * \brief Reads the data of a directory and stores it in the out archive
 * or vector of archives
 *
 * This function is called for first time on the root path of the partition and
 * is recursively called to read all the data in the directory tree.
 *
 * \param lResolv
 * 		Libarchive link resolver for handling hard links logic
* \param path
* 		Path in the FS of the folder to be read
* \param imgRootDir
* 		Path into the image file where the data of the current partition
* 		is being written
* \param mPointLength
* 		Size of the path to the current mount point of the partition
 */
void Image::readDataFromDisk(struct archive_entry_linkresolver *lResolv,
		const std::string &path, const std::string &imgRootDir,
		size_t mPointLength) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("Image::readDataFromDisk(lResolv=>0x%x, path=>%s, imgRootDir=>%s, mPointLength=>%d) start",
			lResolv, path.c_str(), imgRootDir.c_str(), mPointLength);

	DIR *directory;
	struct dirent *d_file; // a file in *directory
	struct archive_entry *entry;
	bool errors = false;
	DataTransfer *trns = DataTransfer::getInstance();

	if ((directory = opendir (path.c_str())) == 0) {
		FileNotFoundException ex(path);
		throw ex;
	}

	while ((d_file = readdir (directory)) != 0) {
		struct stat filestat;
		std::string abPath;

		abPath = path;
		abPath.append(d_file->d_name);

		//Path of the file in the image
		std::string relPath = imgRootDir + "/" + abPath.substr(mPointLength);

		if (lstat (abPath.c_str(), &filestat) < 0) {
			FileNotFoundException ex(abPath);
			throw ex;
		}

		try {
			int fdin = open (abPath.c_str(), O_RDONLY);

			entry = archive_entry_new();
			archive_entry_update_pathname_utf8(entry, relPath.c_str());
			archive_read_disk_entry_from_file(this->_archiveIn, entry, fdin, &filestat);

			switch (archive_entry_filetype(entry)) {
			case AE_IFDIR: {
				if (strcmp (".", d_file->d_name) && strcmp ("..", d_file->d_name)) {

					trns->copyHeader(entry, this->_archivesOut);

					/*
					 * If the current folder is a virtual one or is the mount
					 * point of another partition, bypass it.
					 */
					if(Util::isVirtualDirectory(abPath.c_str())
						|| Util::isMountPoint(abPath)) {

						continue;
					}

					abPath.push_back('/');
					this->readDataFromDisk(lResolv, abPath.c_str(), imgRootDir,
							mPointLength);
				}
				break;
			}
			case AE_IFLNK: {
				char linkPath[4096] = {};
				ssize_t size = 0;

				// Read link
				if ((size = readlink (abPath.c_str(), linkPath, sizeof(linkPath))) < 0) {
					FileNotFoundException ex(path);
					throw ex;
				} else {
						archive_entry_update_symlink_utf8(entry, linkPath);
						trns->copyHeader(entry, this->_archivesOut);
				}

				break;
			}
			case AE_IFIFO:
			case AE_IFSOCK:
			case AE_IFCHR:
			case AE_IFBLK:
			case AE_IFREG: {
				struct archive_entry *sparse;
				if(archive_entry_nlink(entry) > 1) {
					archive_entry_linkify(lResolv, &entry, &sparse);
				}
				if(entry != 0) {
					trns->copyHeader(entry, this->_archivesOut);
				}

				/*
				 * If the current file is a virtual one, bypass
				 */
				if(Util::isLiveFile(abPath.c_str())) {
					continue;
				}

				// else
				if(archive_entry_size(entry) > 0) {
					trns->fdToArchive(fdin, this->_archivesOut);
				}

				break;
			}
			default:
				ReadDataException ex;
				throw ex;
			}

			archive_entry_free(entry);
			close(fdin);
		}catch(const WarningException &ex) {
			errors = true;
		}
	}

	closedir (directory);

	if(errors) {
		ReadErrorsInDirectoryException ex(path);
		ex.logMsg();
	}

	log->loopDebug("Image::readDataToDisk() end");
}

/**
 * \brief Writes all the data in the image file.
 *
 * This method is not called for each partition but just once for the whole
 * image file. All the partitions have been mounted before calling it and the
 * files in the archive are written in the corresponding mount point. This way
 * of restoring let us restore a Doclone image that has been modified by other
 * tools.
 */
void Image::writeDataToDisk() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->loopDebug("Image::writeDataToDisk() start");

	struct archive_entry *entry;
	DataTransfer *trns = DataTransfer::getInstance();

	//Won't keep restoring partitions with errors
	uint8_t numPartitions = this->_disk->getPartitions().size();
	bool errorPartitions[numPartitions];
	memset(errorPartitions, false, numPartitions);

	while(archive_read_next_header(this->_archiveIn, &entry) == ARCHIVE_OK) {
		std::string abPath = archive_entry_pathname(entry);

		for(int i = 0;i<numPartitions
			&& this->_disk->getPartitions().at(i)->getUsedPart() != 0; i++) {

			try {
				Partition *part = this->_disk->getPartitions().at(i);

				if(part->isMounted()
						&& abPath.find(part->getRootDir().c_str()) == 0
						&& !errorPartitions[i]) {

					abPath.replace(0,part->getRootDir().length(),
									part->getMountPoint().c_str());

					archive_entry_update_pathname_utf8(entry, abPath.c_str());

					if(archive_entry_hardlink(entry)!=0
							&& archive_entry_size(entry)==0) {

							std::string hardLinkPath =
									archive_entry_hardlink(entry);
							hardLinkPath.replace(0, part->getRootDir().length(),
									part->getMountPoint().c_str());

							archive_entry_update_hardlink_utf8(entry,
									hardLinkPath.c_str());
					}

					trns->copyHeader(entry, this->_archivesOut);
					trns->copyData(this->_archiveIn, this->_archivesOut);
				}
			} catch(const WarningException &e) {
				errorPartitions[i] = true;
				WriteErrorsInDirectoryException ex(archive_entry_pathname(entry));
				ex.logMsg();
			}
		}
	}

	log->loopDebug("Image::writeDataToDisk() end");
}

/**
 * \brief Reads and transfers all the data of a partition
 *
 * \param index
 * 		The order of the partition (in the vector of Partition*)
 */
void Image::readPartition(int index) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::readPartition(numPart=>%d) start", index);

	Partition *part = this->_disk->getPartitions().at(index);
	Clone *dcl = Clone::getInstance();

	if(!this->_noData) {
		struct archive_entry_linkresolver *lResolv =
				archive_entry_linkresolver_new();
		archive_entry_linkresolver_set_strategy(lResolv, ARCHIVE_FORMAT_TAR);

		part->doMount();
		try {
			std::string mountPoint = part->getMountPoint();
			if(mountPoint[mountPoint.length()-1]!='/') {
				mountPoint.push_back('/');
			}

			this->readDataFromDisk(lResolv, mountPoint, part->getRootDir(),
					mountPoint.length());
		} catch (const CancelException &ex) {
			part->doUmount();
			throw;
		} catch (const ReadDataException &ex) {
			part->doUmount();
			throw;
		} catch (const SendDataException &ex) {
			part->doUmount();
			throw;
		}
		part->doUmount();

		struct archive_entry *sparse;
		struct archive_entry *entryLink = 0;

		archive_entry_linkify(lResolv, &entryLink, &sparse);
		while(entryLink != 0) {
				archive_entry_linkify(lResolv, &entryLink, &sparse);
		}

		archive_entry_linkresolver_free(lResolv);

		dcl->markCompleted(Doclone::OP_READ_DATA, part->getPath());
	}

	log->debug("Image::readPartition() end");
}

/**
 * \brief Reads the data of all partitions in the vector
 */
void Image::readPartitionsData() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::readPartitionsData() start");

	// Initialize the counter of progress
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(this->_size);

	for(unsigned int i = 0;i<this->_disk->getPartitions().size(); i++) {
		try {
			this->readPartition(i);
		}catch(const WarningException &ex) {
			if(this->_disk->getPartitions().size() == 1) {
				throw;
			}

			continue;
		}
	}

	log->debug("Image::readPartitionsData() end");
}

/**
 * \brief Writes the data of all partitions in the vector
 */
void Image::writePartitionsData(const std::string &device) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::writePartitionsData() start");

	// Initialize the counter of progress
	DataTransfer *trns = DataTransfer::getInstance();
	trns->setTotalSize(this->_size);

	if(!this->_noData) {
		try {
			for(unsigned int i = 0;i<this->_disk->getPartitions().size(); i++) {
				if(this->_disk->getPartitions().at(i)->getMinSize() != 0) {
					try {
						this->_disk->getPartitions().at(i)->doMount();
					} catch(WarningException &ex) {
						//Ignore it, this partition just won't be restored
					} catch (const Exception &ex) {
						throw;
					}
				}
			}

			this->writeDataToDisk();

			Clone *dcl = Clone::getInstance();
			dcl->markCompleted(Doclone::OP_WRITE_DATA, device);

			//Restore Grub if this is a disk restoring
			if(this->_type == Doclone::IMAGE_DISK) {
				this->_disk->restoreGrub();
			}
		} catch (const Exception &ex) {
			for(unsigned int i = 0;i<this->_disk->getPartitions().size(); i++) {
				if(this->_disk->getPartitions().at(i)->getMinSize() != 0) {
					this->_disk->getPartitions().at(i)->doUmount();
				}
			}
			throw;
		}

		for(unsigned int i = 0;i<this->_disk->getPartitions().size(); i++) {
			if(this->_disk->getPartitions().at(i)->getMinSize() != 0) {
				this->_disk->getPartitions().at(i)->doUmount();
			}
		}
	}

	log->debug("Image::writePartitionsData() end");
}

/**
 * \brief Reads the partition table and fills the vector of Partition*
 *
 * \param device
 * 		The device which we must read.
 */
void Image::readPartitionTable(const std::string &device) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::readPartitionTable(device=>%s) start", device.c_str());

	this->_disk = DlFactory::createDiskLabel();
	if(this->_type == Doclone::IMAGE_DISK) {
		this->_disk->readBootCode();
		this->_disk->readPartitions();
	} else {
		Partition *part = new Partition();
		part->initFromPath(device);
		this->_disk->getPartitions().push_back(part);
	}

	log->debug("Image::readPartitionTable() end");
}

/**
 * \brief Reads the vector of partitions to create the real partitions in disk.
 *
 * \param device
 * 		The device on which write.
 */
void Image::writePartitionTable(const std::string &device) throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::writePartitionTable(device=>%s) start", device.c_str());

	Clone *dcl = Clone::getInstance();

	if(this->_type == Doclone::IMAGE_DISK) {

		this->_disk->writePartitions();

		for (unsigned int i = 0;i<this->_disk->getPartitions().size() &&
		this->_disk->getPartitions()[i]->getUsedPart() != 0; i++) {
			Partition *part = this->_disk->getPartitions()[i];

			std::stringstream target;
			target << device << ", #" << (i+1);

			try {
				part->format();
				dcl->markCompleted(Doclone::OP_FORMAT_PARTITION,
						target.str());
			} catch (const WarningException &ex) {
				/*
				 * This partition won't be restored.
				 * Show the message and skip to the next partition.
				 */
				ex.logMsg();
				continue;
			}

			try {
				part->writeFlags();
				dcl->markCompleted(Doclone::OP_WRITE_PARTITION_FLAGS,
						target.str());
			} catch (const WarningException &ex) {
				/*
				 * This error doesn't prevent the partition to be restored.
				 * Show the message and continue.
				 */
				ex.logMsg();
			}


			try {
				part->writeLabel();
				dcl->markCompleted(Doclone::OP_WRITE_FS_LABEL,
						target.str());
			} catch (const WarningException &ex) {
				/*
				 * This error doesn't prevent the partition to be restored.
				 * Show the message and continue.
				 */
				ex.logMsg();
			}

			try {
				part->writeUUID();
				dcl->markCompleted(Doclone::OP_WRITE_FS_UUID,
						target.str());
			} catch (const WarningException &ex) {
				/*
				 * This error doesn't prevent the partition to be restored.
				 * Show the message and continue.
				 */
				ex.logMsg();
			}
		}

	} else {
		this->_disk->getPartitions()[0]->setPath(device);
		this->_disk->getPartitions()[0]->setPartNum(Util::getPartNum(device));

		std::stringstream target;
		target << device;

		try {
			this->_disk->getPartitions()[0]->format();
			dcl->markCompleted(Doclone::OP_FORMAT_PARTITION, target.str());
		} catch (const WarningException &ex) {
			/*
			 * This partition won't be restored. Since this is the
			 * only partition, execution must stop.
			 */
			ex.logMsg();
			throw;
		}

		try {
			this->_disk->getPartitions()[0]->writeFlags();
			dcl->markCompleted(Doclone::OP_WRITE_PARTITION_FLAGS,
					target.str());
		} catch (const WarningException &ex) {
			/*
			 * This error doesn't prevent the partition to be restored.
			 * Show the message and continue.
			 */
			ex.logMsg();
		}

		try {
			this->_disk->getPartitions()[0]->writeLabel();
			dcl->markCompleted(Doclone::OP_WRITE_FS_LABEL, target.str());
		} catch (const WarningException &ex) {
			/*
			 * This error doesn't prevent the partition to be restored.
			 * Show the message and continue.
			 */
			ex.logMsg();
		}

		try {
			this->_disk->getPartitions()[0]->writeUUID();
			dcl->markCompleted(Doclone::OP_WRITE_FS_UUID, target.str());
		} catch (const WarningException &ex) {
			/*
			 * This error doesn't prevent the partition to be restored.
			 * Show the message and continue.
			 */
			ex.logMsg();
		}
	}

	log->debug("Image::writePartitionTable() end");
}

/**
 * \brief Checks if the system has installed all the necessary external tools to
 * create an image of a disk or a partition.
 *
 * \return True or False
 */
bool Image::canCreateCheck() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::canCreateCheck() start");

	bool retValue = true;

	try {
		// For each partition
		for(unsigned int i = 0;i<this->_disk->getPartitions().size()
				&& this->_disk->getPartitions()[i]->getUsedPart()!= 0; i++) {
			Partition *part = this->_disk->getPartitions()[i];

			Filesystem *fs = part->getFileSystem();
			if(fs->getCode() != Doclone::FS_NOFS) {
				bool formatSupport = fs->getFormatSupport();
				bool labelSupport = fs->getLabelSupport();
				bool uuidSupport = fs->getUUIDSupport();
				bool mountSupport = fs->getMountSupport();

				if(mountSupport == false) {
					if(fs->getCode() != Doclone::FS_LINUXSWAP) {
						NoMountSupportException ex(fs->getdocloneName());
						ex.logMsg();
					}
				}

				if(formatSupport == false) {
					NoFsToolFoundException ex(fs->getCommand());
					ex.logMsg();
					throw ex;
				}

				if(labelSupport == false) {
					NoLabelSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}

				if(uuidSupport == false) {
					NoUuidSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}
			}
		}
	} catch (const ErrorException &ex) {
		retValue = false;
	}

	log->debug("Image::canCreateCheck(retValue=>%d) end", retValue);

	return retValue;
}

/**
 * \brief Checks if the system has installed all the necessary external tools to
 * restore an image in a disk or a partition.
 *
 * \para dcDisk
 * 		Disk on which work
 * \return True or False
 */
bool Image::canRestoreCheck(const std::string &device) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::canRestoreCheck(device=>%s) start", device.c_str());

	bool retValue = true;

	try {
		switch(this->_type) {
			case Doclone::IMAGE_DISK: {
				if(!Util::isDisk(device)) {
					WrongImageTypeException ex;
					ex.logMsg();
					throw ex;
				}

				break;
			}
			case Doclone::IMAGE_PARTITION: {
				if(Util::isDisk(device)) {
					WrongImageTypeException ex;
					ex.logMsg();
					throw ex;
				}

				break;
			}
			default: {
				InvalidImageException ex;
				ex.logMsg();
				throw ex;
			}
		}

		/*
		 * Check if the amount of partitions in the image is supported
		 * by the kernel driver.
		 */
		uint8_t numPartitions = this->_disk->getPartitions().size();
		if (numPartitions > Util::getNumberOfMinors(device)) {
			TooMuchPartitionsException ex(numPartitions,
					Util::getNumberOfMinors(device));
			ex.logMsg();
			throw ex;
		}

		// Check if it fills in the entire device
		if(!this->fitInDisk()) {
			NoFitInDeviceException ex;
			ex.logMsg();
			throw ex;
		}

		for(int i = 0;i<numPartitions
				&& this->_disk->getPartitions()[i]->getUsedPart() != 0; i++) {
			Partition *part = this->_disk->getPartitions()[i];

			// Check if every partition fills in its assigned space in this disk
			if(!part->fitInDevice(this->_type == Doclone::IMAGE_DISK)) {
				Clone *dcl = Clone::getInstance();
				if(dcl->getForce() == false) {
					NoFitInDeviceException ex;
					ex.logMsg();
					throw ex;
				}
			}

			Filesystem *fs = part->getFileSystem();
			if(fs->getCode() != Doclone::FS_NOFS) {
				bool formatSupport = fs->getFormatSupport();
				bool labelSupport = fs->getLabelSupport();
				bool uuidSupport = fs->getUUIDSupport();
				bool mountSupport = fs->getMountSupport();

				if(mountSupport == false) {
					if(fs->getCode() != Doclone::FS_LINUXSWAP) {
						NoMountSupportException ex(fs->getdocloneName());
						ex.logMsg();
						throw ex;
					}
				}

				if(formatSupport == false) {
					NoFsToolFoundException ex(fs->getCommand());
					ex.logMsg();
					throw ex;
				}

				if(labelSupport == false) {
					NoLabelSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}

				if(uuidSupport == false) {
					NoUuidSupportException ex(fs->getdocloneName());
					ex.logMsg();
				}
			}
		}
	} catch (const NoMountSupportException &ex) {
		retValue = false;
	} catch (const ErrorException &ex) {
		retValue = false;
	}

	log->debug("Image::canRestoreCheck(retValue=>%d) end", retValue);
	return retValue;
}

/**
 * \brief Makes a list of all the operations to be performed in the process of
 * image creation.
 */
void Image::initCreateOperations() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::initCreateOperations() start");

	Clone *dcl = Clone::getInstance();

	if(this->_noData == false) {
		std::vector<Partition *>::iterator it;
		for (it =this->_disk->getPartitions().begin();
				it != this->_disk->getPartitions().end(); ++it) {
			Partition *part=(*it);

			if(part->isWritable() == true) {
				std::string dataTarget = part->getPath();

				// All these Operation objects are deleted in doclone::~doclone()
				Operation *dataOp= new Operation(Doclone::OP_READ_DATA,
						dataTarget);

				dcl->addOperation(dataOp);
			}
		}
	}

	log->debug("Image::initCreateOperations() end");
}

/**
 * \brief Makes a list of all the operations to be performed in the process of
 * image restoration.
 *
 * \param device
 * 		device on which restore the data
 */
void Image::initRestoreOperations(const std::string &device) const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Image::initRestoreOperations(device=>%s) start", device.c_str());

	Clone *dcl = Clone::getInstance();

	uint8_t numPartitions = this->_disk->getPartitions().size();
	std::string target = device;

	if(this->_type == Doclone::IMAGE_DISK) {
		Operation *diskLabelOp = new Operation(Doclone::OP_MAKE_DISKLABEL,
				target);
		dcl->addOperation(diskLabelOp);

		for (int i = 0;i<numPartitions &&
		this->_disk->getPartitions()[i]->getUsedPart() != 0; i++) {
			std::stringstream partTarget;
			partTarget << target << ", #" << (i+1);

			Operation *createPartOp = new Operation(Doclone::OP_CREATE_PARTITION,
					partTarget.str());
			dcl->addOperation(createPartOp);
		}
	}

	for (int i = 0;i<numPartitions &&
	this->_disk->getPartitions()[i]->getUsedPart() != 0; i++) {
		Partition *part = this->_disk->getPartitions()[i];
		Filesystem *fs = part->getFileSystem();

		std::stringstream partTarget;

		if(this->_type == Doclone::IMAGE_DISK) {
			partTarget << target << ", #" << (i+1);
		} else {
			partTarget << target;
		}

		// All these Operation objects are deleted in doclone::~doclone()
		Operation *formatPartOp = new Operation(Doclone::OP_FORMAT_PARTITION,
				partTarget.str());
		dcl->addOperation(formatPartOp);

		Operation *writeFSFlags = new Operation(Doclone::OP_WRITE_PARTITION_FLAGS,
				partTarget.str());
		dcl->addOperation(writeFSFlags);

		if(fs->getLabelSupport() == true) {
			Operation *writeFSLabelOp = new Operation(Doclone::OP_WRITE_FS_LABEL,
							partTarget.str());
					dcl->addOperation(writeFSLabelOp);
		}

		if(fs->getUUIDSupport() == true) {
			Operation *writeFSUuid = new Operation(Doclone::OP_WRITE_FS_UUID,
					partTarget.str());
			dcl->addOperation(writeFSUuid);
		}
	}

	Operation *dataOp = new Operation(Doclone::OP_WRITE_DATA,target);
	dcl->addOperation(dataOp);

	log->debug("Image::initRestoreOperations() end");
}

uint64_t Image::getSize() const {
	return this->_size;
}

Doclone::imageType Image::getType() const {
	return this->_type;
}

void Image::setType(Doclone::imageType type) {
	this->_type = type;
}

Disk *Image::getDisk() {
	return this->_disk;
}

struct archive *Image::getArchiveIn() const {
	return this->_archiveIn;
}
const std::vector<struct archive *> &Image::getArchivesOut() const {
	return this->_archivesOut;
}

}
