/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013, 2015 Joan Lledó <joanlluislledo@gmail.com>
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

#include <doclone/DlFactory.h>

#include <doclone/PartedDevice.h>
#include <doclone/dl/Aix.h>
#include <doclone/dl/Amiga.h>
#include <doclone/dl/Bsd.h>
#include <doclone/dl/Dvh.h>
#include <doclone/dl/Gpt.h>
#include <doclone/dl/Mac.h>
#include <doclone/dl/Msdos.h>
#include <doclone/dl/Pc98.h>
#include <doclone/dl/Sun.h>
#include <doclone/dl/Loop.h>

namespace Doclone {

/**
 * \brief Creates a disk label from a PartedDevice instance
 *
 */
DiskLabel *DlFactory::createDiskLabel() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("FsFactory::createFilesystem() start");

	DiskLabel *dLabel = 0;

	PartedDevice *pedDev = PartedDevice::getInstance();

	if(!pedDev->getPath().empty()) {
		pedDev->open();
		PedDisk *pDisk = pedDev->getDisk();

		std::string dlName;

		if(pDisk) {
			dlName = pDisk->type->name;
		}

		if(!dlName.compare("aix")) {
			dLabel = new Aix();
		} else if(!dlName.compare("amiga")) {
			dLabel = new Amiga();
		} else if(!dlName.compare("bsd")) {
			dLabel = new Bsd();
		} else if(!dlName.compare("dvh")) {
			dLabel = new Dvh();
		} else if(!dlName.compare("gpt")) {
			dLabel = new Gpt();
		} else if(!dlName.compare("mac")) {
			dLabel = new Mac();
		} else if(!dlName.compare("msdos")) {
			dLabel = new Msdos();
		} else if(!dlName.compare("pc98")) {
			dLabel = new Pc98();
		} else if(!dlName.compare("sun")) {
			dLabel = new Sun();
		} else if(!dlName.compare("loop")) {
			dLabel = new Loop();
		} else {
			dLabel = new Msdos();
		}

		dLabel->setPath(pedDev->getPath());
		dLabel->setSize(pedDev->getDevSize());

		pedDev->close();
	} else {
		// No device path given, return empty Msdos disk
		dLabel = new Msdos();
	}

	log->debug("FsFactory::createFilesystem(dLabel=>0x%x) end", dLabel);

	return dLabel;
}

/**
 * \brief Creates a disk label from a Doclone::diskLabelType
 *
 * \param dlType
 * 		The type of the disk label to be created
 */
DiskLabel *DlFactory::createDiskLabel(diskLabelType dlType) {
	Logger *log = Logger::getInstance();
	log->debug("FsFactory::createFilesystem() start");

	DiskLabel *dLabel = 0;

	switch(dlType) {
	case DISK_TYPE_AIX: {
		dLabel = new Aix();
		break;
	}
	case DISK_TYPE_AMIGA: {
		dLabel = new Amiga();
		break;
	}
	case DISK_TYPE_BSD: {
		dLabel = new Bsd();
		break;
	}
	case DISK_TYPE_DVH: {
		dLabel = new Dvh();
		break;
	}
	case DISK_TYPE_GPT: {
		dLabel = new Gpt();
		break;
	}
	case DISK_TYPE_MAC: {
		dLabel = new Mac();
		break;
	}
	case DISK_TYPE_MSDOS: {
		dLabel = new Msdos();
		break;
	}
	case DISK_TYPE_PC98: {
		dLabel = new Pc98();
		break;
	}
	case DISK_TYPE_SUN: {
		dLabel = new Sun();
		break;
	}
	case DISK_TYPE_LOOP: {
		dLabel = new Loop();
		break;
	}
	default: {
		dLabel = new Msdos();
		break;
	}
	}

	log->debug("FsFactory::createFilesystem(dLabel=>0x%x) end", dLabel);

	return dLabel;
}

} /* namespace Doclone */
