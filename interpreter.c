#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "interpreter.h"

/*
 * Local functions
 */
void print_debug(char **parsed_words, int num_of_words);
void print_help();

/* ----------------------------------------------------------------------------
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
 * ----------------------------------------------------------------------------
 */
int interpret(linked_list *list, char **parsed_words, int num_of_words) {
	int err;
	// Catch if input is null
	if (!parsed_words) {
		return -1;
	}

	// Check if there are words
	if (num_of_words < 1) {
		return -2;
	}

	// Handle user inputs
	if (strcmp(parsed_words[0], "quit") == 0) {
		return -3;
	} else if (strcmp(parsed_words[0], "help") == 0) {
		print_help();
	} else if (strcmp(parsed_words[0], "print") == 0) {
		if (num_of_words != 2) {
			printf("Expected: print <varname>\n");
			return -7;
		}
		char output_value[MAX_CMD_LENGTH];
		char *key = parsed_words[1];

		// Search for the key and fetch the value by key if found
		err = get_value_by_key(list, key, output_value);

		// Failed to find the variable or there are invalid arguments
		if (err < 0) {
			return -4;
		}

		printf("%s\n", output_value);
	} else if (strcmp(parsed_words[0], "run") == 0) {
		printf("running\n");
		return -5;
	} else if (strcmp(parsed_words[0], "set") == 0) {
		// Checks if the right number of arguments is obtained
		if (num_of_words != 3) {
			printf("Expected : set <varname> <value>\n"); 
			return -7;
		}

		char *key = parsed_words[1];
		char *value = parsed_words[2];

		// Attempt to insert the node
		err = insert(list, 0, key, value);

		if (err == -1) {
			// Key was found so update value
			err = update_value_by_key(list, key, value);
		} else if (err == 0) {
			// Key was not found and node is inserted
			return 0;
		} else {
			// Node could not be inserted
			return -6;
		}

		// Variable was not set properly
		if (err != 0) {
			return -6;
		}
	}	else {
		printf("Undefined command\n");
		return -7;
	}

	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief For debugging purposes. Prints out the content of an array of strings
 *        up to a specified number of words.
 * @param input  - parsed_words  An array of strings
 *        input  - num_of_words  Number of strings
 * ----------------------------------------------------------------------------
 */
void print_debug (char **parsed_words, int num_of_words) {
	// Checks if parsed_words is null
	if (!parsed_words) {
		return;
	}

	int i;
	for (i = 1; i < num_of_words; i++) {
		printf("%s\n", parsed_words[i]);
	}
}

/* ----------------------------------------------------------------------------
 * @brief Prints out available commands.
 * ----------------------------------------------------------------------------
 */
void print_help () {
	printf("help                   - Displays available commands\n"
	       "quit                   - Exits this shell\n"
	       "set <varname> <value>  - Sets a variable to a value\n"
	       "print <varname>        - Prints the value of a set variable\n"
	       "run <script_name>      - Execute a script\n");
}
