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

#ifndef OPERATION_H_
#define OPERATION_H_

#include <string>

namespace Doclone {

/**
 * \enum dcOperationType
 * \brief Identify the type of operation
 *
 * \var OP_NONE
 * 	No operation
 * \var OP_READ_PARTITION_TABLE
 * 	Reading the partition table
 * \var OP_MAKE_DISKLABEL
 * 	Writing a new disklabel
 * \var OP_CREATE_PARTITION
 * 	Creating a new partition
 * \var OP_FORMAT_PARTITION
 * 	Formatting a partition
 * \var OP_WRITE_PARTITION_FLAGS
 * 	Writing the flags of a partition
 * \var OP_WRITE_FS_LABEL
 * 	Writing the label of a filesystem
 * \var OP_WRITE_FS_UUID
 * 	Writing the uuid of a filesystem
 * \var OP_READ_DATA
 * 	Reading the data of a partition
 * \var OP_WRITE_DATA
 * 	Writing the data of a partition
 * \var OP_GRUB_INSTALL
 * 	Installing GRUB
 * \var OP_TRANSFER_DATA
 * 	Sending or receiving data
 * \var OP_WAIT_SERVER
 * 	Waiting for connect to server
 * \var OP_WAIT_CLIENTS
 * 	Waiting for connect to client/s
 */
enum dcOperationType {
	OP_NONE,
	OP_READ_PARTITION_TABLE,
	OP_MAKE_DISKLABEL,
	OP_CREATE_PARTITION,
	OP_FORMAT_PARTITION,
	OP_WRITE_PARTITION_FLAGS,
	OP_WRITE_FS_LABEL,
	OP_WRITE_FS_UUID,
	OP_READ_DATA,
	OP_WRITE_DATA,
	OP_GRUB_INSTALL,
	OP_TRANSFER_DATA,
	OP_WAIT_SERVER,
	OP_WAIT_CLIENTS
};

/**
 * \class Operation
 * \brief Operation that the program must perform.
 *
 * This class is used to set the state of the execution. The class doclone
 * has a vector of objects of this type.
 * \date August, 2011
 */
class Operation {
public:
	Operation(dcOperationType type, const std::string &target);
	~Operation() {}

	void setType(dcOperationType type);
	dcOperationType getType() const;

	void setCompleted(bool completed);
	bool getCompleted()const;

	void setTarget(const std::string &target);
	const std::string &getTarget() const;

protected:
	/// What is what operation do
	dcOperationType _type;
	/// If is completed yet
	bool _completed;
	/// In which file/image/device should this operation work
	std::string _target;
};

}

#endif /* OPERATION_H_ */
