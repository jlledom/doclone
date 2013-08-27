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

#include <doclone/DiskLabel.h>

#include <doclone/PartedDevice.h>
#include <doclone/exception/MakeLabelException.h>

namespace Doclone {

DiskLabel::DiskLabel(const std::string &path) : Disk(path), _labelType(),
		_partedName() {
}

Doclone::diskLabelType DiskLabel::getLabelType() const {
	return this->_labelType;
}

/**
 * \brief Writes a new disk label in the current disk
 */
void DiskLabel::makeLabel() const throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("DiskLabel::makeLabel() start");

	PartedDevice *pedDev = PartedDevice::getInstance();
	pedDev->open();
	PedDevice *pDevice = pedDev->getDevice();

	PedDiskType *type = 0 ;
	type = ped_disk_type_get(this->_partedName.c_str()) ;

	if ( type ) {
		PedDisk *pDisk = ped_disk_new_fresh(pDevice, type );

		if(!pDisk) {
			MakeLabelException ex;
			throw ex;
		}

		pedDev->setDisk(pDisk);
		pedDev->commit();
	}

	pedDev->close();

	log->debug("DiskLabel::makeLabel() end");
}

} /* namespace Doclone */
