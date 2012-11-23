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

void parent_main(int* pipes)
{
	/*
	 * First pipe is the writing for parent and reading for child
	 * The second is vice versa
	 */
		   
	FILE * writing_pipe = fdopen(*(pipes + 1), "w");
	if (writing_pipe == NULL) {
		bail_out("parent failed opening writing pipe");
	}
	
	FILE * reading_pipe = fdopen(*(pipes + 2), "r");
	if (reading_pipe == NULL) {
		bail_out("parent failed opening reading pipe");
	}
	
	// Close the other pipes
	if (close(*(pipes + 0)) != 0) {
		bail_out("close pipes + 1 failed");
	}
	if (close(*(pipes + 3)) != 0) {
		bail_out("close pipes + 2 failed");
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
		bail_out("fgets had an error");
	}
	
	if (feof(stdin) == 0) {
		bail_out("Parent: fgets failed");	
	}
	
	// CLEANUP
	fclose(reading_pipe);
	fclose(writing_pipe);
	close(*(pipes + 1));
	close(*(pipes + 2));
	
	DEBUG ("WAIT PARENT\n");
	
	int wait_result= wait(NULL);
	
	if (wait_result == -1) {
		bail_out("Wait: No child process found");
	}
	
	DEBUG ("EXIT PARENT\n");
	
	exit (EXIT_SUCCESS);
}