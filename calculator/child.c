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

void child_main(int* pipes)
{
	/*
	 * First pipe is the writing for parent and reading for child
	 * The second is vice versa
	 */
	
	FILE * reading_pipe = fdopen(*(pipes + 0), "r");
	if (reading_pipe == NULL) {
		bail_out("child failed opening reading pipe");
	}
	
	FILE * writing_pipe = fdopen(*(pipes + 3), "w");
	if (writing_pipe == NULL) {
		bail_out("child failed opening writing pipe");
	}
	
	// Close the other pipes
	if (close(*(pipes + 1)) != 0) {
		bail_out("close pipes + 1 failed");
	}
	if (close(*(pipes + 2)) != 0) {
		bail_out("close pipes + 2 failed");
	}
	
	// LOOP UNIT EOF
	char read_buffer[MAX_INPUT_LENGTH + 2];
	char result[MAX_RESULT_LENGTH + 1] = "ERROR";
	while (fgets(read_buffer, MAX_INPUT_LENGTH + 1, reading_pipe) != NULL) {
		DEBUG("C: \t%s", read_buffer);
		
		/************ ZE REAL BUSINESS LOGIC *************/
		int operand1 = atoi(strtok(read_buffer, " "));
		int operand2 = atoi(strtok(NULL, " "));
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
					bail_out("Division through zero error");
				}
				iResult = operand1 / operand2;
				break;
			default:
				bail_out("Unexpected operand");
				break;
		}
		
		sprintf(result, "%d", iResult);
		
		fprintf(writing_pipe, "%s\n", result);
		fflush(writing_pipe);
	}
	
	if (feof(reading_pipe) == 0) {
		bail_out("fgets in child");
	}

	// CLEANUP
	fclose(writing_pipe);
	fclose(reading_pipe);
	close(*(pipes + 0));
	close(*(pipes + 3));
	
	DEBUG("EXIT CHILD\n");
	
	exit(EXIT_SUCCESS);
}