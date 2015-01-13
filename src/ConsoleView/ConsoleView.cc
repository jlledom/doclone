/*
 * doclone - a frontend for libdoclone
 * Copyright (C) 2013 Joan Lledó <joanlluislledo@gmail.com>
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

#include <ConsoleView/ConsoleView.h>

#include <unistd.h>
#include <getopt.h>
#include <libintl.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

#include <config.h>

#include <doclone/Clone.h>
#include <doclone/DataTransfer.h>

#define _(string) gettext(string)

ConsoleView::ConsoleView() : _totalSize(0), _percent(0) {
	// We subscribe to all subjects
	Doclone::Clone *dcl = Doclone::Clone::getInstance();
	Doclone::DataTransfer *trans = Doclone::DataTransfer::getInstance();
	Doclone::Logger *log = Doclone::Logger::getInstance();
	trans->addObserver(this);
	dcl->addObserver(this);
	log->addObserver(this);
}

void ConsoleView::notify(Doclone::dcTransferEvent event, const uint64_t numBytes) {
	switch (event) {
	case Doclone::TRANS_TOTAL_SIZE: {
		this->_totalSize = numBytes;
		break;
	}
	case Doclone::TRANS_TRANSFERRED_BYTES: {
		if(this->_totalSize>0) {
			// Calculate and print the percentage of data transferred
			float rate = numBytes/static_cast<float>(this->_totalSize);
			int percent = floor(rate*100);
			if(this->_percent != percent) {
				this->_percent = percent;
				// TO TRANSLATORS: looks like "Transferred: 55%"
				std::cout << _("Transferred:") << " " << this->_percent
						<< "%" << std::endl;
			}
		}

		break;
	}
	}
}

void ConsoleView::notify(Doclone::dcOperationEvent event,
		Doclone::dcOperationType type, const std::string &target) {

	// For this view, only print the completed tasks, except this three
	if(event != Doclone::OPER_MARK_COMPLETED) {
		switch(type) {
		case Doclone::OP_WAIT_SERVER: {
			std::cout << _("Waiting for server...") << std::endl;

			break;
		}
		case Doclone::OP_WAIT_CLIENTS: {
			std::cout << _("Waiting for client/s...") << std::endl;

			break;
		}
		case Doclone::OP_TRANSFER_DATA: {
			std::cout << _("Transferring data...") << " " << target << std::endl;

			break;
		}
		default: {
			return;
			break;
		}
		}
	}

	switch(type) {
	case Doclone::OP_READ_PARTITION_TABLE: {
		// TO TRANSLATORS: looks like Partition table read on /dev/sdb
		std::cout << _("Partition table read on") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_MAKE_DISKLABEL: {
		// TO TRANSLATORS: looks like Disk label created on /dev/sdb
		std::cout << _("Disk label created on") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_CREATE_PARTITION: {
		// TO TRANSLATORS: looks like New partition created: /dev/sdb1
		std::cout << _("New partition created:") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_FORMAT_PARTITION: {
		// TO TRANSLATORS: looks like Partition formated: /dev/sdb1
		std::cout << _("Partition formated:") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_WRITE_PARTITION_FLAGS: {
		// TO TRANSLATORS: looks like FS flags written: /dev/sdb1
		std::cout << _("FS flags written:") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_WRITE_FS_LABEL: {
		// TO TRANSLATORS: looks like FS label written: /dev/sdb1
		std::cout << _("FS label written:") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_WRITE_FS_UUID: {
		// TO TRANSLATORS: looks like FS UUID written: /dev/sdb1
		std::cout << _("FS UUID written:") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_READ_DATA: {
		// TO TRANSLATORS: looks like Data read from /dev/sdb1
		std::cout << _("Data read from") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_WRITE_DATA: {
		// TO TRANSLATORS: looks like Data written to /dev/sdb1
		std::cout << _("Data written to") << " " << target << std::endl;

		break;
	}
	case Doclone::OP_GRUB_INSTALL: {
		// TO TRANSLATORS: looks like Grub installed on /dev/sdb
		std::cout << _("Grub installed on") << " " << target << std::endl;

		break;
	}
	default: {
		return;
		break;
	}
	}

	return;
}

void ConsoleView::notify(Doclone::dcEvent event, const std::string &target) {
	switch(event) {
		case Doclone::EVT_NEW_CONNECION : {
			// TO TRANSLATORS: looks like	New connection from 192.168.1.10
			std::cout << _("New connection from") << " " << target << std::endl;
			break;
		}
		default: {
			break;
		}
	}

	return;
}

void ConsoleView::notify(const std::string &str) {
	std::cerr << str << std::endl;
}

/**
 * Gets the arguments of the command line and executes
 * the corresponding function
 *
 * \param argc
 * 		Number of command line parameters
 * \param argv
 * 		List of command line parameters
 */
void ConsoleView::initView(int argc, char **argv) const {
	const char *cmd = argv[0];
	dcConsoleFunction function = CONSOLE_NONE;

	char option;
	std::string image="";
	std::string device="";
	std::string address="";
	int nodesNumber = 0;

	const char options_c[] = "hvcrSRsld:f:a:n:eF";
	const struct option options_l[] = {
		{"help", 0, 0, 'h'},
		{"version", 0, 0, 'v'},
		{"create", 0, 0, 'c'},
		{"restore", 0, 0, 'r'},
		{"send", 0, 0, 'S'},
		{"receive", 0, 0, 'R'},
		{"link-send", 0, 0, 's'},
		{"link-receive", 0, 0, 'l'},
		{"device", 1, 0, 'd'},
		{"file", 1, 0, 'f'},
		{"address", 1, 0, 'a'},
		{"nodes", 1, 0, 'n'},
		{"empty", 0, 0, 'e'},
		{"force", 0, 0, 'F'},
		{0, 0, 0, 0}
	};

	Doclone::Clone *dcl = Doclone::Clone::getInstance();

	do {	// Process the options
		option = getopt_long (argc, argv, options_c, options_l, 0);
		switch (option) {
		case 'h': {
			usage (stdout, 0, cmd);
			break;
		}
		case 'v': {
			version();
			break;
		}
		case 'c': {
			if (function != CONSOLE_NONE)
				usage (stderr, 1, cmd);
			function = CONSOLE_CREATE;
			break;
		}
		case 'r': {
			if (function != CONSOLE_NONE)
				usage (stderr, 1, cmd);
			function = CONSOLE_RESTORE;
			break;
		}
		case 'S': {
			if (function != CONSOLE_NONE)
				usage (stderr, 1, cmd);
			function = CONSOLE_SEND;
			break;
		}
		case 'R': {
			if (function != CONSOLE_NONE)
				usage (stderr, 1, cmd);
			function = CONSOLE_RECEIVE;
			break;
		}
		case 's': {
			if (function != CONSOLE_NONE)
				usage (stderr, 1, cmd);
			function = CONSOLE_LINK_SEND;
			break;
		}
		case 'l': {
			if (function != CONSOLE_NONE)
				usage (stderr, 1, cmd);
			function = CONSOLE_LINK_RECEIVE;
			break;
		}
		case 'f': {
			if (!strrchr (optarg, '/'))	{ // If it is a relative path 
				char tmp[256];
				snprintf (tmp, sizeof(tmp), "./%s", optarg);
				image = tmp;
				dcl->setImage(image);
			}
			else {
				image = optarg;
				dcl->setImage(image);
			}
			break;
		}
		case 'd': {
			device = optarg;
			dcl->setDevice(device);
			break;
		}
		case 'a': {
			address = optarg;
			dcl->setAddress(address);
			break;
		}
		case 'n': {
			nodesNumber = atoi (optarg);
			dcl->setNodesNumber(nodesNumber);

			break;
		}
		case 'e': {
			dcl->setEmpty(true);
			break;
		}
		case 'F': {
			dcl->setForce(true);
			break;
		}
		case -1:
			break;
		case '?':
		default: {
			usage (stderr, 1, cmd);
			break;
		}
		}
	}
	while (option != -1);

	try {
		switch (function) {
		/* local working functions - create/restore */
		case CONSOLE_CREATE: {
			if(image.empty() || device.empty()) {
				usage(stderr, 1, cmd);
				break;
			}

			dcl->create();

			break;
		}
		case CONSOLE_RESTORE: {
			if(image.empty() || device.empty()) {
				usage(stderr, 1, cmd);
				break;
			}

			dcl->restore();

			break;
		}
		/* network functions - unicast/multicast */
		case CONSOLE_SEND: {
			if((image.empty() && device.empty())) {
				usage(stderr, 1, cmd);
				break;
			}

			dcl->send();

			break;
		}
		case CONSOLE_RECEIVE: {
			if(address.empty() || (image.empty() && device.empty())) {
				usage(stderr, 1, cmd);
				break;
			}

			dcl->receive();

			break;
		}
		/* network functions - link mode */
		case CONSOLE_LINK_SEND: {
			if(image.empty() && device.empty()) {
				usage(stderr, 1, cmd);
				break;
			}

			dcl->chainOrigin();

			break;
		}
		case CONSOLE_LINK_RECEIVE: {
			if(image.empty() && device.empty()) {
				usage(stderr, 1, cmd);
				break;
			}

			dcl->chainLink();

			break;
		}
		default: {
			usage (stderr, 1, cmd);
			break;
		}
		}
	} catch(const Doclone::Exception &ex) {
		ex.logMsg();
	}

	return;
}

/**
 * Shows the program help dialog
 *
 * \param stream
 * 		Output stream by which the message will be printed
 * \param code
 * 		Exit error code
 * \param cmd
 * 		Command used by the user for execute doclone
 */
void ConsoleView::usage (FILE * stream, int code, const char * cmd) const {
	fprintf (stream, _("Usage: %s FUNCTION "
			"[ -d, --device DEVICE ] [ -f, --file FILE ]\n"
			"\t[ -a, --address SERVER-IP-ADDRESS ]"
			" [ -n, --nodes NUMBER ]\n"
			"\t[ -e, --empty ] [ -F, --force]\n "), cmd);

	fprintf (stream,
			_("\nFUNCTION is made up of one of these specifications:\n"
					"\tFor local work: (All these options imply -d and -f)\n"
					"\t-c, --create\t\tCreates a doclone image.\n"
					"\t-r, --restore\t\tRestores a doclone image.\n"
					"\n\tFor work over the network: "
					"(All these options imply -d or -f)\n"
					"\tUnicast/Multicast:\n"
					"\t-S, --send\t\tSends server's data to receivers.\n"
					"\t\t\t\t(This function implies -n).\n"
					"\t-R, --receive\t\tReceives data from the server.\n"
					"\t\t\t\t(This option implies -a).\n"
					"\n"
					"\tLink mode:\n"
					"\t-s, --link-send\t\tSends data to the network.\n"
					"\t-l, --link-receive\tReceives data from the network.\n"));
	fprintf (stream,
			_("\n\tOthers:\n" "\t-h, --help\t\tShow this help.\n"
					"\t-v, --version\t\tShow doclone version.\n"));
	exit (code);
}

/**
 * Shows program version and credits
 */
void ConsoleView::version() const {
	printf (_("doclone %s\n"
			"Written by Joan Lledó\n"
			"Copyright (C) 2013 Joan Lledó\n"
			"doclone comes with ABSOLUTELY NO WARRANTY,\n"
			"to the extent permitted by law. \n"
			"You may redistribute copies of doclone\n"
			"under the terms of the GNU General Public License.\n"
			"For more information about these matters,\n"
			"see the file named COPYING.\n"), PACKAGE_VERSION);
	fflush (stdout);
	exit (0);
}
