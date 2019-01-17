#include <stdio.h>
#include <string.h>
#include "interpreter.h"

/**
 * @brief Interprets an array of strings and calls the appropriate function
 *        to handle the user input. Supports the following commands
 *            - exit
 *            - help
 *            - print
 *            - run
 *            - set
 * @param input  - parsed_words An array of strings
 *        input  - num_of_words An integer representing the number of strings
 * @return int - status code
 *                  0  - No error
 *                 -1  - Input array of strings is null
 *                 -2  - Invalid number of words
 *                 -3  - Exit
 *                 -4  - Variable could not be printed
 *                 -5  - Script could not be executed
 *                 -6  - Variable could not be set
 */
int interpret(char **parsed_words, int num_of_words) {
	// Catch if input is null
	if (!parsed_words) {
		return -1;
	}

	// Check if there are words
	if (num_of_words < 1) {
		return -2;
	}

	if (strcmp(parsed_words[0], "quit") == 0) {
		return -3;
	} else if (strcmp(parsed_words[0], "help") == 0) {
		print_help();
	} else if (strcmp(parsed_words[0], "print") == 0) {
		printf("printing\n");
		return -4;
	} else if (strcmp(parsed_words[0], "run") == 0) {
		printf("running\n");
		return -5;
	} else if (strcmp(parsed_words[0], "set") == 0) {
		printf("setting \n");
		return -6;
	} else {
		printf("Undefined command\n");
	}

	return 0;
}

int print (char **parsed_words, int num_of_words) {
	int i;
	for (i = 1; i < num_of_words; i++) {
		printf("%s\n", parsed_words[i]);
	}
}

void print_help () {
	printf("help                   - Displays available commands\n"
	       "quit                   - Exits this shell\n"
	       "set <varname> <value>  - Sets a variable to a value\n"
	       "print <varname>        - Prints the value of a set variable\n"
	       "run <script_name>      - Execute a script\n");
}
