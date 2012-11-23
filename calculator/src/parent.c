//
//  parent.c
//  osue
//
//  Created by Martin Prebio on 23.11.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "calculator.h"

FILE * reading_pipe, * writing_pipe;
int * pipes_saved;

void cleanup_parent()
{
	fclose(reading_pipe);
	fclose(writing_pipe);
	close(*(pipes_saved + 1));
	close(*(pipes_saved + 2));
	
	DEBUG ("WAIT PARENT\n");
	
	int wait_result= wait(NULL);
	
	if (wait_result == -1) {
		bail_out("Wait: No child process found");
	}
	
	DEBUG ("EXIT PARENT\n");
}

void bailout_parent(char * error)
{
	cleanup_parent();
	bail_out(error);
}

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
		
		fprintf(writing_pipe, "%s", input_buffer);
		fflush(writing_pipe);
		
		if (fgets(return_value, MAX_RESULT_LENGTH, reading_pipe) != NULL) {
			DEBUG("P pipe:\t%s", return_value);
			printf("%s", return_value);
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