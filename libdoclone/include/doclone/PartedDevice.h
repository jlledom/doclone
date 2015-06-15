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

#ifndef PARTEDDEVICE_H_
#define PARTEDDEVICE_H_

#include <string>

#include <parted/parted.h>

#include <doclone/exception/Exception.h>

namespace Doclone {

/**
 * \class PartedDevice
 * \brief Access to libparted functions. Singleton.
 *
 * This class is a singleton type that provides the access to the libparted
 * objects and functions from any place of the code.
 * \date August, 2011
 */
class PartedDevice {
public:
	~PartedDevice();

	static PartedDevice* getInstance();

	void initialize(const std::string &device);

	std::string getPath();
	PedDevice *getDevice() ;
	PedDisk *getDisk();
	void setDisk(PedDisk *pDisk);
	uint64_t getDevSize() const;
	uint64_t getPartitionSize(unsigned int numPartition) const;

	void open() throw(Exception);
	void commit() const throw(Exception);
	void close();

private:
	/// Private constructor to implement singleton pattern
	PartedDevice();

	/// Used to know if a new parted disk or device must be opened or closed.
	int _openings;
	/// The path of the disk what we work in
	std::string _path;
	/// Libparted device object
	PedDevice* _pDevice;
	/// Libparted disk object
	PedDisk* _pDisk;
};

}

#endif /* PARTEDDEVICE_H_ */
