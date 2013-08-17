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

#ifndef DLFACTORY_H_
#define DLFACTORY_H_

#include <doclone/DiskLabel.h>

namespace Doclone {

/**
 * \class DlFactory
 * \brief Factory class to create DiskLabel objects.
 *
 * Has a factory functions that receive the parted name for a partition table
 * type and return a pointer to an object with the right type.
 *
 * \date June, 2012
 */
class DlFactory {
public:
	static DiskLabel *createDiskLabel();
	static DiskLabel *createDiskLabel(diskLabelType dlType,
			const std::string &path);
};

} /* namespace Doclone */
#endif /* DLFACTORY_H_ */
