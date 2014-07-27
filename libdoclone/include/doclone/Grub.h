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

#ifndef GRUB_H_
#define GRUB_H_

#include <map>

#include <doclone/Disk.h>
#include <doclone/exception/Exception.h>

#ifndef GRUB_COMMAND
/**
 * \def GRUB_COMMAND
 *
 * External tool to restore grub.
 */
#define GRUB_COMMAND "grub-setup"
#endif

namespace Doclone {

/**
 * \class Grub
 * \brief Useful methods to restore the grub boot loader
 * \date August, 2011
 */
class Grub {
public:
	Grub(const Disk *disk) throw(Exception);
	~Grub();
	void install() throw(Exception);
	
private:
	/// The device on which work
	const Disk *_disk;
	/// Vector of partitions where maybe grub is installed
	std::map<unsigned int, std::string> _grubParts;
	
	void searchPartition() throw(Exception);
};

}

#endif /* GRUB_H_ */
