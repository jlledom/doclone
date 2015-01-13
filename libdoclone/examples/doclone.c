/*
 * doclone - a frontend for libdoclone
 * Copyright (C) 2013, 2014, 2015 Joan Lledó <joanlluislledo@gmail.com>
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

#include <unistd.h>
#include <getopt.h>
#include <libintl.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <doclone/clone.h>

#define _(string) gettext(string)
#define PACKAGE_VERSION "0.7.1"

typedef enum dcConsoleFunction {
	CONSOLE_NONE,
	CONSOLE_CREATE,
	CONSOLE_RESTORE,
	CONSOLE_LINK_SEND,
	CONSOLE_LINK_RECEIVE,
	CONSOLE_SEND,
	CONSOLE_RECEIVE
} dcConsoleFunction;

uint64_t totalSize=0;
int percent=0;

void notifyTransfer(dcTransferEvent event, const uint64_t numBytes) {
	switch (event) {
	case TRANS_TOTAL_SIZE: {
		totalSize = numBytes;
		break;
	}
	case TRANS_TRANSFERRED_BYTES: {
		if(totalSize>0) {
			// Calculate and print the percentage of data transferred
			float rate = numBytes/(float)totalSize;
			int tmpPercent = floor(rate*100);
			if(percent != tmpPercent) {
				percent = tmpPercent;
				// TO TRANSLATORS: looks like "Transferred: 55%"
				printf (_("Transferred: %d%%\n"), percent);
			}
		}

		break;
	}
	}
}

void notifyOperation(dcOperationEvent event, dcOperationType type,
	const char *target) {

	// For this view, only print the completed tasks, except this three
	if(event != OPER_MARK_COMPLETED) {
		switch(type) {
		case OP_WAIT_SERVER: {
			printf(_("Waiting for server...\n"));

			break;
		}
		case OP_WAIT_CLIENTS: {
			printf(_("Waiting for client/s...\n"));

			break;
		}
		case OP_TRANSFER_DATA: {
			printf(_("Transferring data...\n"));

			break;
		}
		default: {
			return;
			break;
		}
		}
	}

	switch(type) {
	case OP_READ_PARTITION_TABLE: {
		// TO TRANSLATORS: looks like Partition table read on /dev/sdb
		printf(_("Partition table read on %s\n"), target);

		break;
	}
	case OP_MAKE_DISKLABEL: {
		// TO TRANSLATORS: looks like Disk label created on /dev/sdb
		printf(_("Disk label created on %s\n"), target);

		break;
	}
	case OP_CREATE_PARTITION: {
		// TO TRANSLATORS: looks like New partition created: /dev/sdb1
		printf(_("New partition created: %s\n"), target);

		break;
	}
	case OP_FORMAT_PARTITION: {
		// TO TRANSLATORS: looks like Partition formated: /dev/sdb1
		printf(_("Partition formated: %s\n"), target);

		break;
	}
	case OP_WRITE_PARTITION_FLAGS: {
		// TO TRANSLATORS: looks like Written the fs flags of /dev/sdb1
		printf(_("Written the fs flags of %s\n"), target);

		break;
	}
	case OP_WRITE_FS_LABEL: {
		// TO TRANSLATORS: looks like Written the fs label of /dev/sdb1
		printf(_("Written the fs label of %s\n"), target);

		break;
	}
	case OP_WRITE_FS_UUID: {
		// TO TRANSLATORS: looks like Written the fs uuid of /dev/sdb1
		printf(_("Written the fs uuid of %s\n"), target);

		break;
	}
	case OP_READ_DATA: {
		// TO TRANSLATORS: looks like Data readed from /dev/sdb1
		printf(_("Data readed from %s\n"), target);

		break;
	}
	case OP_WRITE_DATA: {
		// TO TRANSLATORS: looks like Data written to /dev/sdb1
		printf(_("Data written to %s\n"), target);

		break;
	}
	case OP_GRUB_INSTALL: {
		// TO TRANSLATORS: looks like Grub installed on /dev/sdb1
		printf(_("Grub installed on %s\n"), target);

		break;
	}
	default: {
		return;
		break;
	}
	}

	return;
}

void notifyGeneral(dcEvent event, const char *target) {
	switch(event) {
	case EVT_NEW_CONNECION : {
		// TO TRANSLATORS: looks like	New connection from 192.168.1.10
		printf(_("New connection from %s\n"), target);
		break;
	}
	}

	return;
}

void notify(const char *str) {
	dprintf(2, "%s\n", str);
}

/**
 * Shows the help dialog
 *
 * \param stream
 * 		Output stream by which the message will be printed
 * \param code
 * 		Exit error code
 * \param cmd
 * 		Command used by the user to execute doclone
 */
void usage (FILE * stream, int code, const char * cmd) {
	fprintf (stream, _("Usage: %s FUNCTION "
					   "[ -d, --device DEVICE ] [ -f, --file FILE ]\n"
					   "\t[ -a, --address SERVER-IP-ADDRESS ]"
					   " [ -n, --nodes NUMBER ]\n"
					   "\t[ -e, --empty ] [ -F, --force]\n "), cmd);

	fprintf (stream,
			 _("\nFUNCTION is made up of one of these specifications:\n"
			   "\tFor local work: (All these options imply -d and -f)\n"
			   "\t-c, --create\t\tCreates a DNA image.\n"
			   "\t-r, --restore\t\tRestores a DNA image.\n"
			   "\n\tFor work over the network: "
			   "(All these options imply -d or -f)\n"
			   "\tUnicast/Multicast:\n"
			   "\t-S, --send\t\tSends server's data to receivers.\n"
			   "\t(This function implies -n).\n"
			   "\t-R, --receive\t\tReceives data from the server.\n"
			   "\t(This option implies -a).\n"
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
void version() {
	printf (_("doclone %s\n"
			"Written by Joan Lledó\n\n"
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

/**
 * Gets the arguments of the command line and executes
 * the corresponding function
 *
 * \param argc
 * 		Number of command line parameters
 * \param argv
 * 		List of command line parameters
 */
int main(int argc, char **argv) {
	const char *cmd = argv[0];
	dcConsoleFunction function = CONSOLE_NONE;

	char option;
	char image[512]={};
	char device[512]={};
	char address[512]={};
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

	dc_doclone *dc_obj = doclone_new();
	doclone_subscribe_to_tranfer_events(dc_obj, &notifyTransfer);
	doclone_subscribe_to_operation_events(dc_obj, &notifyOperation);
	doclone_subscribe_to_general_events(dc_obj, &notifyGeneral);
	doclone_subscribe_to_notifications(dc_obj, &notify);

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
				snprintf (image, sizeof(image), "./%s", optarg);
				doclone_set_image(dc_obj, image);
			}
			else {
				snprintf(image, sizeof(image), "%s", optarg);
				doclone_set_image(dc_obj, image);
			}
			break;
		}
		case 'd': {
			snprintf(device, sizeof(device), "%s", optarg);
			doclone_set_device(dc_obj, device);
			break;
		}
		case 'a': {
			snprintf(address, sizeof(address), "%s", optarg);
			doclone_set_address(dc_obj, address);
			break;
		}
		case 'n': {
			nodesNumber = atoi (optarg);
			doclone_set_nodes_number(dc_obj, nodesNumber);

			break;
		}
		case 'e': {
			doclone_set_empty(dc_obj, 1);
			break;
		}
		case 'F': {
			doclone_set_force(dc_obj, 1);
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

	switch (function) {
	/* local work functions - create/restore */
	case CONSOLE_CREATE: {
		if(image[0] == '\0' || device[0] == '\0') {
			usage(stderr, 1, cmd);
			break;
		}

		doclone_create(dc_obj);

		break;
	}
	case CONSOLE_RESTORE: {
		if(image[0] == '\0' || device[0] == '\0') {
			usage(stderr, 1, cmd);
			break;
		}

		doclone_restore(dc_obj);

		break;
	}
	/* network functions - unicast/multicast */
	case CONSOLE_SEND: {
		if((image[0] == '\0' && device[0] == '\0')
				|| nodesNumber == 0) {
			usage(stderr, 1, cmd);
			break;
		}

		doclone_send(dc_obj);

		break;
	}
	case CONSOLE_RECEIVE: {
		if(address[0] == '\0' || (image[0] == '\0' && device[0] == '\0')) {
			usage(stderr, 1, cmd);
			break;
		}

		doclone_receive(dc_obj);

		break;
	}
	/* network functions - link mode */
	case CONSOLE_LINK_SEND: {
		if(image[0] == '\0' && device[0] == '\0') {
			usage(stderr, 1, cmd);
			break;
		}

		doclone_chain_origin(dc_obj);

		break;
	}
	case CONSOLE_LINK_RECEIVE: {
		if(image[0] == '\0' && device[0] == '\0') {
			usage(stderr, 1, cmd);
			break;
		}

		doclone_chain_link(dc_obj);

		break;
	}
	default: {
		usage (stderr, 1, cmd);
		break;
	}
	}

	doclone_destroy(dc_obj);

	return 0;
}

