/**
 * @file child.c
 * @author Martin Prebio (1025737) <martin.prebio@students.tuwien.ac.at>
 * @date 22.11.12
 *
 * @brief This part of the program does the arithmetic part.
 *
 * Calculator Child
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
 * Cleanup child process
 * @brief Close fds and files
 * @details global variables: {writing,reading}_pipe, pipes_saved
 */
static void cleanup_child()
{
	(void) fclose(writing_pipe);
	(void) fclose(reading_pipe);
	(void) close(*(pipes_saved + 0));
	(void) close(*(pipes_saved + 3));
	
	DEBUG("EXIT CHILD\n");
}

/**
 * Abort process properly
 * @param error Error message
 */
static void bailout_child(char * error)
{
	(void) cleanup_child();
	(void) bail_out(error);
}

/**
 * Entry point of forked child
 * @brief Wait for messages of parent and return calculation result via pipes. Abort if an EOF is received.
 * @param pipes 2x2 array of communication pipes
 * @return EXIT_SUCCESS if everything worked, EXIT_FAILURE otherwise
 */
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

		// Invalid operands are silently ignored

		long operand1 = strtol(strtok(read_buffer, " "), NULL, 10);
		long operand2 = strtol(strtok(NULL, " "), NULL, 10);
		char *operator_p = strtok(NULL, " ");
		char operator = operator_p[0];
		
		DEBUG("C:\tOP1=%ld, OP2=%ld, OP=%c\n", operand1, operand2, operator);
		
		long iResult;
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
		
		sprintf(result, "%ld", iResult);
		
		if (fprintf(writing_pipe, "%s\n", result) < 0) {
			bailout_child("Child: Writing to pipe failed");
		}
		if (fflush(writing_pipe) != 0) {
			bailout_child("Child: Writing flush failed");
		}
	}
	
	if (feof(reading_pipe) == 0) {
		bailout_child("fgets in child");
	}

	// CLEANUP
	cleanup_child();
	
	exit(EXIT_SUCCESS);
}