/**
 * @file calculator.c
 * @author Martin Prebio (1025737) <martin.prebio@students.tuwien.ac.at>
 * @date 22.11.12
 *
 * @brief The program is divided in a parent process which handles i/o and a child process which does the
 * arithmetic stuff. 
 *
 * Calculator: Supplies a shell for simple arithmetic operations
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "calculator.h"
#include "child.h"
#include "parent.h"

/**
 * Program name for usage and error messages
 */
char* program_name;

/**
 * Shutdown function
 * @brief Abort program in a non chaotic way by calling cleanup and exit.
 * @param error Error message
 * @details global variables: program_name
 */
void bail_out(char * error)
{
	(void) fprintf(stderr, "%s: ", program_name);
	(void) fprintf(stderr, "%s - (%s)\n", error, strerror(errno));
	exit(EXIT_FAILURE);
}

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: program_name
 */
static void usage(void)
{
	(void) fprintf(stderr, "Usage: just `%s` without any arguments\nSupplies a shell for simple arithmetic operations\n", program_name);
	exit(EXIT_FAILURE);
}

/**
 * Program entry point
 * @brief The program is divided in a parent process which handles i/o and a child process which does the
 * arithmetic stuff.
 * @param argc The argument counter
 * @param argv The argument vector
 * @return EXIT_SUCCESS on success, EXIT_FAILURE in case of an error
 * @details global variables: program_name
 */
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
