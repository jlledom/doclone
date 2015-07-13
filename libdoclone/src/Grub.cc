/*
 *  libdoclone - library for cloning GNU/Linux systems
 *  Copyright (C) 2013-2015 Joan Lledó <joanlluislledo@gmail.com>
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

#include <doclone/Grub.h>

#include <string>
#include <fstream>
#include <map>

#include <doclone/Clone.h>
#include <doclone/Operation.h>
#include <doclone/Logger.h>
#include <doclone/Util.h>
#include <doclone/exception/Exception.h>
#include <doclone/exception/CancelException.h>
#include <doclone/exception/GrubException.h>

namespace Doclone {

/**
 * \brief Checks if a needed external tool is present and searches for the
 * partition where grub files are placed.
 *
 * \param disk
 * 		The disk on which we will work
 */
Grub::Grub(Disk *disk) throw(Exception) : _disk(disk) {
	// Formatting support
	if(Util::find_program_in_path(GRUB_COMMAND).empty()) {
		GrubException ex;
		throw ex;
	}

	this->searchPartition();
}

/**
 * \brief Clears _grubParts map
 */
Grub::~Grub() {
	this->_grubParts.clear();
}

/**
 * \brief Finds out the grub files in the disk
 */
void Grub::searchPartition() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Grub::searchPartition() start");

	std::vector<Partition*> partitions = this->_disk->getPartitions();

	for (unsigned int i = 0; i< partitions.size(); i++) {
		try {
			Partition *part = partitions[i];

			if(!part->isWritable()) {
				continue;
			}

			part->doMount();

			try {
				std::string relBoot = "/boot/";
				std::string relBootGrub = relBoot.c_str();
				relBootGrub.append(GRUB_DIR);
				std::string bootAbPath = part->getMountPoint();
				bootAbPath.append(relBootGrub);

				std::string relRoot = "/";
				std::string relRootGrub = relRoot.c_str();
				relRootGrub.append(GRUB_DIR);
				std::string rootAbPath = part->getMountPoint();
				rootAbPath.append(relRootGrub);

				std::ifstream bootGrubDir(bootAbPath.c_str());
				std::ifstream grubDir(rootAbPath.c_str());

				if(bootGrubDir) {
					this->_grubParts[i] = relBoot;
					bootGrubDir.close();
				}

				if(grubDir) {
					this->_grubParts[i] = relRoot;
					grubDir.close();
				}
			} catch (const CancelException &ex) {
				part->doUmount();
				throw;
			}

			part->doUmount();
		}catch(const WarningException &ex) {
			continue;
		}
	}

	log->debug("Grub::searchPartition() end");
}

/**
 * \brief Executes an external tool to install grub in the MBR
 */
void Grub::install() throw(Exception) {
	Logger *log = Logger::getInstance();
	log->debug("Grub::install() start");

	int exitValue=-1;

	if(this->_grubParts.empty()) {
		GrubException ex;
		throw ex;
	}

	// In the no-data mode, it is not possible to install GRUB
	Clone *dcl = Clone::getInstance();
	if(dcl->getEmpty() == true) {
		log->debug("Grub::install() end");
		return;
	}

	/*
	 * The operation is added here to make sure is only added if grub should
	 * really be installed
	 */
	Operation *installGrubOp = new Operation(
			Doclone::OP_GRUB_INSTALL, this->_disk->getPath());
	dcl->addOperation(installGrubOp);

	std::map<unsigned int, std::string>::iterator it;
	for (it = this->_grubParts.begin(); it != this->_grubParts.end(); ++it ) {
		try {
			Partition *part =
					this->_disk->getPartitions()[it->first];

			part->doMount();

			try {
				std::string grub_install_cmdline;

				grub_install_cmdline = GRUB_COMMAND;
				grub_install_cmdline.append(" --boot-directory ");
				grub_install_cmdline.append(part->getMountPoint());
				grub_install_cmdline.append(it->second);
				grub_install_cmdline.append(" ");
				grub_install_cmdline.append(this->_disk->getPath());
				grub_install_cmdline.append(" >/dev/null 2>&1");

				Util::spawn_command_line_sync(grub_install_cmdline, &exitValue, 0);
			} catch (const CancelException &ex) {
				part->doUmount();
				throw;
			}

			part->doUmount();

			if (exitValue == 0) {
				break;
			}
		}catch(const WarningException &ex) {
			continue;
		}
	}

	if (exitValue != 0) {
		GrubException ex;
		throw ex;
	}

	dcl->markCompleted(Doclone::OP_GRUB_INSTALL,
			this->_disk->getPath());

	log->debug("Grub::install() end");
}

}
