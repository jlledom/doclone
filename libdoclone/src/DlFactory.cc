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

DiskLabel *DlFactory::createDiskLabel() {
	Logger *log = Logger::getInstance();
	log->debug("FsFactory::createFilesystem() start");

	DiskLabel *dLabel = 0;

	PartedDevice *pedDev = PartedDevice::getInstance();

	pedDev->open();
	PedDisk *pDisk = pedDev->getDisk();

	std::string path = pedDev->getPath();
	std::string dlName;

	if(pDisk) {
		dlName = pDisk->type->name;
	}

	if(!dlName.compare("aix")) {
		dLabel = new Aix(path);
	} else if(!dlName.compare("amiga")) {
		dLabel = new Amiga(path);
	} else if(!dlName.compare("bsd")) {
		dLabel = new Bsd(path);
	} else if(!dlName.compare("dvh")) {
		dLabel = new Dvh(path);
	} else if(!dlName.compare("gpt")) {
		dLabel = new Gpt(path);
	} else if(!dlName.compare("mac")) {
		dLabel = new Mac(path);
	} else if(!dlName.compare("msdos")) {
		dLabel = new Msdos(path);
	} else if(!dlName.compare("pc98")) {
		dLabel = new Pc98(path);
	} else if(!dlName.compare("sun")) {
		dLabel = new Sun(path);
	} else if(!dlName.compare("loop")) {
		dLabel = new Loop(path);
	} else {
		dLabel = new Msdos(path);
	}

	pedDev->close();

	log->debug("FsFactory::createFilesystem(dLabel=>0x%x) end", dLabel);

	return dLabel;
}

DiskLabel *DlFactory::createDiskLabel(diskLabelType dlType,
		const std::string &path) {
	Logger *log = Logger::getInstance();
	log->debug("FsFactory::createFilesystem(dlType=>%d, path=>%s) start",
			dlType, path.c_str());

	DiskLabel *dLabel = 0;

	switch(dlType) {
	case DISK_TYPE_NONE: {
		/* If the disk label is not specified, probably the image had been
		 * created using some previous version of libdoclone. So we use the MSDOS
		 * label by default, the only one supported by previous versions.
		 */
		dLabel = new Msdos(path);
		break;
	}
	case DISK_TYPE_AIX: {
		dLabel = new Aix(path);
		break;
	}
	case DISK_TYPE_AMIGA: {
		dLabel = new Amiga(path);
		break;
	}
	case DISK_TYPE_BSD: {
		dLabel = new Bsd(path);
		break;
	}
	case DISK_TYPE_DVH: {
		dLabel = new Dvh(path);
		break;
	}
	case DISK_TYPE_GPT: {
		dLabel = new Gpt(path);
		break;
	}
	case DISK_TYPE_MAC: {
		dLabel = new Mac(path);
		break;
	}
	case DISK_TYPE_MSDOS: {
		dLabel = new Msdos(path);
		break;
	}
	case DISK_TYPE_PC98: {
		dLabel = new Pc98(path);
		break;
	}
	case DISK_TYPE_SUN: {
		dLabel = new Sun(path);
		break;
	}
	case DISK_TYPE_LOOP: {
		dLabel = new Loop(path);
		break;
	}
	default: {
		dLabel = new Msdos(path);
		break;
	}
	}

	log->debug("FsFactory::createFilesystem(dLabel=>0x%x) end", dLabel);

	return dLabel;
}

} /* namespace Doclone */
