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

#ifndef DISKLABEL_H_
#define DISKLABEL_H_

#include <doclone/Disk.h>
#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \enum Doclone::diskLabelType
 * \brief The type of a disk label.
 *
 * A disk labels is a format to write the structure of partitions in the MBR of
 * a disk.
 *
 * \var Doclone::mountType::DISK_TYPE_NONE
 * \var Doclone::mountType::DISK_TYPE_AIX
 * \var Doclone::mountType::DISK_TYPE_AMIGA
 * \var Doclone::mountType::DISK_TYPE_BSD
 * \var Doclone::mountType::DISK_TYPE_DVH
 * \var Doclone::mountType::DISK_TYPE_GPT
 * \var Doclone::mountType::DISK_TYPE_MAC
 * \var Doclone::mountType::DISK_TYPE_MSDOS
 * \var Doclone::mountType::DISK_TYPE_PC98
 * \var Doclone::mountType::DISK_TYPE_SUN
 * \var Doclone::mountType::DISK_TYPE_LOOP
 */
enum diskLabelType {DISK_TYPE_NONE, DISK_TYPE_AIX, DISK_TYPE_BSD,
	DISK_TYPE_AMIGA, DISK_TYPE_DVH, DISK_TYPE_GPT, DISK_TYPE_MAC,
	DISK_TYPE_MSDOS, DISK_TYPE_PC98, DISK_TYPE_SUN, DISK_TYPE_LOOP};

/**
 * \class DiskLabel
 * \brief Virtual base class for the disk labels that doclone supports
 *
 * Has all the common attributes for all the disk labels supported.
 *
 * This class must not be instanced as is, but through any of its derived
 * classes.
 *
 * \date June, 2012
 */
class DiskLabel : public Disk {
public:
	DiskLabel(const std::string &path);
	virtual ~DiskLabel() {}

	void makeLabel() const throw(Exception);

	Doclone::diskLabelType getLabelType() const;
protected:
	/// The type of the partition table
	Doclone::diskLabelType _labelType;
	/// Name of this type of disk label in libparted
	std::string _partedName;
};

} /* namespace Doclone */
#endif /* DISKLABEL_H_ */
