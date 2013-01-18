#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "svctl.h"

char *program_name;

void usage()
{
	printf("USAGE: %s [-c <size>|-e|-d] <secvault id>\n", program_name);
	exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
	program_name = argv[0];
	int opt;

	int whattodo = 0;
	int size = 0;
	char buffer[KEY_LENGTH];
	memset(buffer, 0, KEY_LENGTH);

	while ((opt = getopt(argc, argv, "c:ed")) != -1) {
		switch (opt) {
		case 'c':
			size = strtol(optarg, (char**)NULL, 10);
			if (size < 0 || size > 1048576) usage();
			/* Read the key */
			for(int i = 0; i < KEY_LENGTH; i++) {
				int c = fgetc(stdin);
				if (c == EOF) {
					// XXX Handle ferror
					break;
				}
				buffer[i] = c;
			}
		case 'e':
		case 'd':
			if (whattodo > 0) usage();
			whattodo = opt;
			break;
		default: usage();
		}
	}

	if (argc - optind != 1 ) usage(); /* Need a secvault id */
	int sv_id = strtol(argv[optind], (char**)NULL, 10);

	/* Input validated ... */

	int ctl = open("/dev/sv_ctl", 0);
	if (ctl < 0) {
		perror("open failed");
		exit(EXIT_FAILURE);
	}

	char command;

	switch (whattodo) {
	case 'c':
		command = 'C';
		break;
	case 'e':
		command = 'I';
		break;
	case 'd':
		command = 'D';
		break;
	}

	struct sv_ioctl_message message;
	message.size = size;
	message.sv_id = sv_id;
	memcpy(message.key, buffer, KEY_LENGTH);

	if (ioctl(ctl, command, &message) < 0) {
		perror("ioctl failed"); return -1;
	}
	
	exit(EXIT_SUCCESS);
}
