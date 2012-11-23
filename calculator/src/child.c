//
//  child.c
//  osue
//
//  Created by Martin Prebio on 23.11.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "calculator.h"

FILE * reading_pipe, * writing_pipe;
int * pipes_saved;

void cleanup_child()
{
	fclose(writing_pipe);
	fclose(reading_pipe);
	close(*(pipes_saved + 0));
	close(*(pipes_saved + 3));
	
	DEBUG("EXIT CHILD\n");
}

void bailout_child(char * error)
{
	cleanup_child();
	bail_out(error);
}

void child_main(int* pipes)
{
	pipes_saved = pipes;
	/*
	 * First pipe is the writing for parent and reading for child
	 * The second is vice versa
	 */
	
	reading_pipe = fdopen(*(pipes + 0), "r");
	if (reading_pipe == NULL) {
		bailout_child("child failed opening reading pipe");
	}
	
	writing_pipe = fdopen(*(pipes + 3), "w");
	if (writing_pipe == NULL) {
		bailout_child("child failed opening writing pipe");
	}
	
	// Close the other pipes
	if (close(*(pipes + 1)) != 0) {
		bailout_child("close pipes + 1 failed");
	}
	if (close(*(pipes + 2)) != 0) {
		bailout_child("close pipes + 2 failed");
	}
	
	// LOOP UNIT EOF
	char read_buffer[MAX_INPUT_LENGTH + 2];
	char result[MAX_RESULT_LENGTH + 1] = "ERROR";
	while (fgets(read_buffer, MAX_INPUT_LENGTH + 1, reading_pipe) != NULL) {
		DEBUG("C: \t%s", read_buffer);
		
		/************ ZE REAL BUSINESS LOGIC *************/
		int operand1 = strtol(strtok(read_buffer, " "), NULL, 10);
		int operand2 = strtol(strtok(NULL, " "), NULL, 10);
		char *operator_p = strtok(NULL, " ");
		char operator = operator_p[0];
		
		DEBUG("C:\tOP1=%d, OP2=%d, OP=%c\n", operand1, operand2, operator);
		
		int iResult;
		switch (operator) {
			case '+':
				iResult = operand1 + operand2;
				break;
			case '-':
				iResult = operand1 - operand2;
				break;
			case '*':
				iResult = operand1 * operand2;
				break;
			case '/':
				if (operand2 == 0) {
					bailout_child("Division through zero error");
				}
				iResult = operand1 / operand2;
				break;
			default:
				bailout_child("Unexpected operand");
				break;
		}
		
		sprintf(result, "%d", iResult);
		
		fprintf(writing_pipe, "%s\n", result);
		fflush(writing_pipe);
	}
	
	if (feof(reading_pipe) == 0) {
		bailout_child("fgets in child");
	}

	// CLEANUP
	cleanup_child();
	
	exit(EXIT_SUCCESS);
}