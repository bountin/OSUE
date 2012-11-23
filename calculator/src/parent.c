/**
 * @file parent.c
 * @author Martin Prebio (1025737) <martin.prebio@students.tuwien.ac.at>
 * @date 22.11.12
 *
 * @brief This part of the program does the i/o with the user.
 *
 * Calculator Parent
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "calculator.h"

/**
 * Pipes for communication with parent.
 */
FILE * reading_pipe, * writing_pipe;

/**
 * Pointer to pipe structure
 * @brief Saved global for cleanup
 */
int * pipes_saved;

/**
 * Cleanup parent process
 * @brief Close fds and files and wait for the end of the child process whcih is triggered by closing the pipe.
 * @details global variables: {writing,reading}_pipe, pipes_saved
 */
static void cleanup_parent()
{
	(void) fclose(reading_pipe);
	(void) fclose(writing_pipe);
	(void) close(*(pipes_saved + 1));
	(void) close(*(pipes_saved + 2));
	
	DEBUG ("WAIT PARENT\n");
	
	int wait_result= wait(NULL);
	
	if (wait_result == -1) {
		bail_out("Wait: No child process found");
	}
	
	DEBUG ("EXIT PARENT\n");
}

/**
 * Abort process properly
 * @param error Error message
 */
static void bailout_parent(char * error)
{
	(void) cleanup_parent();
	(void) bail_out(error);
}

/**
 * Entry point of forked parent
 * @brief Read calculation assignments from stdin, send them to the child and redirect it's response
 * to stdout unit a EOF is received (from stdin).
 * @param pipes 2x2 array of communication pipes
 * @return EXIT_SUCCESS if everything worked, EXIT_FAILURE otherwise
 */
void parent_main(int* pipes)
{
	pipes_saved = pipes;

	/*
	 * First pipe is the writing for parent and reading for child
	 * The second is vice versa
	 */
		   
	writing_pipe = fdopen(*(pipes + 1), "w");
	if (writing_pipe == NULL) {
		bailout_parent("parent failed opening writing pipe");
	}
	
	reading_pipe = fdopen(*(pipes + 2), "r");
	if (reading_pipe == NULL) {
		bailout_parent("parent failed opening reading pipe");
	}
	
	// Close the other pipes
	if (close(*(pipes + 0)) != 0) {
		bailout_parent("close pipes + 1 failed");
	}
	if (close(*(pipes + 3)) != 0) {
		bailout_parent("close pipes + 2 failed");
	}
	
	char input_buffer[MAX_INPUT_LENGTH + 2];
	char return_value[MAX_RESULT_LENGTH + 2];
	
	while (fgets(input_buffer, MAX_INPUT_LENGTH + 1, stdin) != NULL) {
		DEBUG("P in:\t%s", input_buffer);
		
		if (fprintf(writing_pipe, "%s", input_buffer) < 0) {
			bailout_parent("Parent: Writing to pipe failed");
		}
		if (fflush(writing_pipe) != 0) {
			bailout_parent("Parent: Writing flush failed");
		}
		
		if (fgets(return_value, MAX_RESULT_LENGTH, reading_pipe) != NULL) {
			DEBUG("P pipe:\t%s", return_value);
			(void) printf("%s", return_value);
			continue;
		}
		bailout_parent("Parent: fgets of readin_pipe got an error");
	}
	
	if (feof(stdin) == 0) {
		bailout_parent("Parent: fgets of stdin failed");
	}
	
	// CLEANUP
	cleanup_parent(pipes);
	
	exit (EXIT_SUCCESS);
}