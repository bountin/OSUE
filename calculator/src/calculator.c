//
//  main.c
//  calculator
//
//  Created by Martin Prebio on 22.11.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "calculator.h"
#include "child.h"
#include "parent.h"

char* program_name;


void bail_out(char * error)
{
	(void) fprintf(stderr, "%s: ", program_name);
	(void) fprintf(stderr, "%s - (%s)\n", error, strerror(errno));
	exit(EXIT_FAILURE);
}

static void usage(void)
{
	(void) fprintf(stderr, "Usage: just `%s` without any arguments\nSupplies a shell for simple arithmetic operations\n", program_name);
	exit(EXIT_FAILURE);
}


int main(int argc, const char * argv[])
{
	program_name = (char *) argv[0];
	
	if (argc != 1) {
		usage();
	}

	/* 
	 * First pipe is the writing for parent and reading for child
	 * The second is vice versa
	 */
	
	int pipes[2][2];

	if (pipe((int *)&pipes[0]) != 0) {
		bail_out("Creation of pipe 1 failed");
	}
	if (pipe((int *)&pipes[1]) != 0) {
		bail_out("Creation of pipe 2 failed");			
	}
	
	pid_t pid = fork();
	
	if (pid < 0) {
		bail_out("Forking failed");
	} else if (pid == 0) {
		// Child
		child_main((int*)&pipes);
	} else {
		// Parent
		parent_main((int*)&pipes);
	}

    return 0;
}
