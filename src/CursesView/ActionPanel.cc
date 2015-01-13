/*
 * doclone - a frontend for libdoclone
 * Copyright (C) 2013, 2015 Joan Lledó <joanlluislledo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <CursesView/ActionPanel.h>

#include <string.h>
#include <libintl.h>

#include <sstream>

/**
 * \brief Initializes attributes
 */
ActionPanel::ActionPanel(): _operations() {
}

/**
 * Adds a new operation
 *
 * \param operation
 * 		The Operation to be added
 */
void ActionPanel::addOperation(Doclone::Operation &operation) {
	this->_operations.push_back(operation);

	this->refresh();
}

/**
 * Marks an operation as completed
 *
 * \param operation
 * 		The Operation to be marked as completed
 */
void ActionPanel::markOperationCompleted(Doclone::Operation &operation) {
	std::vector<Doclone::Operation>::iterator it;
	for(it = this->_operations.begin();it != this->_operations.end();++it) {
		if((*it).getType() == operation.getType()
				&& (*it).getTarget().compare(operation.getTarget()) == 0) {
			(*it).setCompleted(true);
		}
	}

	this->refresh();
}

/**
 * Removes an operation from the list
 *
 * \param operation
 * 		The Operation to be deleted
 */
void ActionPanel::deleteOperation(Doclone::Operation &operation) {
	std::vector<Doclone::Operation>::iterator it;
	for(it = this->_operations.begin();it != this->_operations.end();++it) {
		if((*it).getType() == operation.getType()
				&& (*it).getTarget().compare(operation.getTarget()) == 0) {
			it = this->_operations.erase(it);
		}
	}

	this->refresh();
}

/**
 * Clears all operations
 */
void ActionPanel::clearAll() {
	this->_operations.clear();

	this->refresh();
}

/**
 * Shows this widget
 */
void ActionPanel::post() {
	this->Menu::post();

	// Move the cursor to the last completed operation
	ITEM **items = menu_items(this->_menu);
	if(items != 0) {
		ITEM *curItem = 0;
		int nItems = this->_menu->nitems;

		for(int i = 0; i<nItems; i++) {
			Item_Options opts = item_opts(items[i]);
			if((opts & O_SELECTABLE) == false) {
				curItem = items[i];
			}
		}

		if(curItem != 0) {
			int nextItemIndex = item_index(curItem)+1;
			int newTopRow = nextItemIndex-((nextItemIndex)%this->_menu->arows)-1;
			if(newTopRow > this->_menu->rows - this->_menu->arows) {
				int difference=
						newTopRow-(this->_menu->rows - this->_menu->arows);
				newTopRow-=difference;
			}
			set_top_row(this->_menu, newTopRow);
		}
	}
}

/**
 * Re-draws the widget, displaying the current state of the operations
 */
void ActionPanel::refresh() {
	std::vector<std::string> labels;

	std::vector<Doclone::Operation>::iterator it;
	for(it = this->_operations.begin();it != this->_operations.end();++it) {
		std::stringstream actionStr;

		Doclone::dcOperationType type=(*it).getType();
		std::string target=(*it).getTarget();

		switch(type) {
		case Doclone::OP_READ_PARTITION_TABLE: {
			// TO TRANSLATORS: looks like Read the partition table of /dev/sdb
			actionStr << _("Read the partition table of") << " " << target;

			break;
		}
		case Doclone::OP_MAKE_DISKLABEL: {
			// TO TRANSLATORS: looks like Create disk label on /dev/sdb
			actionStr << _("Create disk label on") << " " << target;

			break;
		}
		case Doclone::OP_CREATE_PARTITION: {
			// TO TRANSLATORS: looks like Create partition: /dev/sdb1
			actionStr << _("Create partition:") << " " << target;

			break;
		}
		case Doclone::OP_FORMAT_PARTITION: {
			// TO TRANSLATORS: looks like Format partition: /dev/sdb1
			actionStr << _("Format partition:") << " " << target;

			break;
		}
		case Doclone::OP_WRITE_PARTITION_FLAGS: {
			// TO TRANSLATORS: looks like Write FS flags on /dev/sdb1
			actionStr << _("Write FS flags on") << " " << target;

			break;
		}
		case Doclone::OP_WRITE_FS_LABEL: {
			// TO TRANSLATORS: looks like Write FS label on /dev/sdb1
			actionStr << _("Write FS label on") << " " << target;

			break;
		}
		case Doclone::OP_WRITE_FS_UUID: {
			// TO TRANSLATORS: looks like Write fs uuid on /dev/sdb1
			actionStr << _("Write the FS uuid on") << " " << target;

			break;
		}
		case Doclone::OP_READ_DATA: {
			// TO TRANSLATORS: looks like Read data from /dev/sdb1
			actionStr << _("Read data from") << " " << target;

			break;
		}
		case Doclone::OP_WRITE_DATA: {
			// TO TRANSLATORS: looks like Write data to /dev/sdb1
			actionStr << _("Write data to") << " " << target;

			break;
		}
		case Doclone::OP_GRUB_INSTALL: {
			// TO TRANSLATORS: looks like Install Grub on /dev/sdb
			actionStr << _("Install Grub on") << " " << target;

			break;
		}
		case Doclone::OP_WAIT_SERVER: {
			actionStr << _("Waiting for server...");

			break;
		}
		case Doclone::OP_WAIT_CLIENTS: {
			actionStr << _("Waiting for client/s...");

			break;
		}
		case Doclone::OP_TRANSFER_DATA: {
			actionStr << _("Transferring data");

			break;
		}
		default: {
			break;
		}
		}

		labels.push_back(actionStr.str());
	}

	this->setContent(labels);
}

/**
 * Checks whether all operation are completed or not
 */
bool ActionPanel::allOperationsCompleted() {
	bool retVal = true;

	std::vector<Doclone::Operation>::iterator it;
	for(it = this->_operations.begin(); it != this->_operations.end(); ++it) {
		if((*it).getCompleted() == false) {
			retVal = false;
			break;
		}
	}

	return retVal;
}

/**
 * Sets new options of the menu and erases the old options if any
 *
 * This class works without descriptions.
 *
 * \param labels
 * 		Vector of options
 */
void ActionPanel::setContent(const std::vector<std::string> &labels){
	int size = labels.size();

	this->deleteContent();

	this->_options = new char*[size+1];
	ITEM **items = new ITEM* [size+1];

	int i;
	std::vector<std::string>tmpLabels = labels;
	std::vector<std::string>::iterator it = tmpLabels.begin();
	std::vector<Doclone::Operation>::iterator it2 = this->_operations.begin();
	for(i = 0; it != tmpLabels.end(); ++it, ++it2, i++) {
		int length=(*it).length()+1;
		this->_options[i]=new char[length];
		snprintf(this->_options[i], length, "%s", (*it).c_str());
		items[i] = new_item(this->_options[i], this->_options[i]);

		if((*it2).getCompleted() == true) {
			item_opts_off(items[i], O_SELECTABLE);
		}
	}

	this->_options[i]=0;
	items[i] = new_item(this->_options[i], this->_options[i]);

	this->deleteMenu();
	this->deleteWindows();

	this->_menu = new_menu(items);
}

const std::vector<Doclone::Operation> &ActionPanel::getOperations() const {
    return this->_operations;
}
