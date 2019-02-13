/* ----------------------------------------------------------------------------
 * @file INTERPRETER.C
 * @author Si Xun Li - 260674916
 * @version 2.0
 * @brief This file contains the interpreter and all helper functions. The 
 *        majority of functions return status codes. Any negative numbers
 *        indicate an error.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "kernel.h"
#include "interpreter.h"

/*
 * Local functions
 */
void print_debug(char *parsed_words[MAX_CMD_LENGTH], int num_of_words);
void print_help();
int print_var(char *parsed_words[MAX_CMD_LENGTH], int num_of_words);
int run(char *parsed_words[MAX_CMD_LENGTH], int num_of_words);
int set_var(char *parsed_words[MAX_CMD_LENGTH], int num_of_words);
int read_and_exec_file(char *file);
int exec(char *parsed_words[MAX_CMD_LENGTH], int num_of_words);

FILE *files[MAX_FILE_NUMBER];

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
 * @return int - Status code
 *                  0  - No error
 *                 -1  - Input array of strings is null
 *                 -2  - Invalid number of words
 *                 -3  - Exit
 *                 -4  - Variable could not be printed
 *                 -5  - Script could not be executed
 *                 -6  - Variable could not be set
 *                 -7  - Undefined command
 * ----------------------------------------------------------------------------
 */
int interpret(char *parsed_words[MAX_CMD_LENGTH], int num_of_words) {
	for (int i = 0; i < num_of_words; i++) {
		printf("%s \n", parsed_words[i]);
	}
	int err;
	// Catch if input is null
	if (!parsed_words) {
		err = -1;
		return err;
	}

	// Check if there are words
	if (num_of_words < 1) {
		err = -2;
		return err;
	}

	// Handle user inputs
	if (strcmp(parsed_words[0], "quit") == 0) {
		/* ------------------------------------------------------------
		 * Handles quit command
		 * ------------------------------------------------------------
		 */
		err = -3;
		return err;
	} else if (strcmp(parsed_words[0], "help") == 0) {
		/* ------------------------------------------------------------
		 * Handles help command
		 * ------------------------------------------------------------
		 */
		print_help();
		err = 0;
		return 0;
	} else if (strcmp(parsed_words[0], "print") == 0) {
		/* ------------------------------------------------------------
		 * Handles print command
		 * ------------------------------------------------------------
		 */
		err = print_var(parsed_words, num_of_words);
		return err;
	} else if (strcmp(parsed_words[0], "run") == 0) {
		/* ------------------------------------------------------------
		 * Handles run command
		 * ------------------------------------------------------------
		 */
		err = run(parsed_words, num_of_words);
		return err;
	} else if (strcmp(parsed_words[0], "set") == 0) {
		/* ------------------------------------------------------------
		 * Handles set command
		 * ------------------------------------------------------------
		 */
		err = set_var(parsed_words, num_of_words);
		return err;
	} else if (strcmp(parsed_words[0], "exec") == 0) {
		/* ------------------------------------------------------------
		 * Handles exec command
		 * ------------------------------------------------------------
		 */
		for (int i = 0; i < num_of_words; i++) {
			printf("%d %s\n", i, parsed_words[i]);
		}
		err = exec(parsed_words, num_of_words);
		return err;
	} else {
		/* ------------------------------------------------------------
		 * Handles unknown inputs
		 * ------------------------------------------------------------
		 */
		printf("Undefined command\n");
		err = -7;
		return err;
	}
}

/* ----------------------------------------------------------------------------
 * @brief For debugging purposes. Prints out the content of an array of strings
 *        up to a specified number of words.
 * @param input  - parsed_words  An array of strings
 *        input  - num_of_words  Number of strings
 * ----------------------------------------------------------------------------
 */
void print_debug (char *parsed_words[MAX_CMD_LENGTH], int num_of_words) {
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
	printf("%s shell - Version %s\n"
	       "Currently supported commands:\n"
	       TAB "help                     - Displays available commands\n"
	       TAB "quit                     - Exits this shell\n"
	       TAB "set <varname> <value>    - Sets a variable to a value\n"
	       TAB "print <varname>          - Prints the value of a set variable\n"
	       TAB "run <script_name>        - Execute a script\n"
	       TAB "exec <s1> [<s2>] [<s3>]  - Execute scripts in parallel\n",
	       SHELL_NAME,
	       SHELL_VERSION);
}

/* ----------------------------------------------------------------------------
 * @brief Prints out variables.
 * @param input  - parsed_words  An array of strings
 *        input  - num_of_words  The number of element in the array
 * @return int - Status code
 *                  0 - No errors
 *                 -4 - Variable could not be found
 *                 -7 - Number of arguments is not as expected
 * ----------------------------------------------------------------------------
 */
int print_var (char *parsed_words[MAX_CMD_LENGTH], int num_of_words) {
	int err;
	if (num_of_words != 2) {
		printf(GENERIC_EXPECTED_MSG "print <varname>\n");
		return -7;
	}

	char output_value[MAX_CMD_LENGTH];
	char *key = parsed_words[1];

	// Search for the key and fetch the value by key if found
	err = get_value_by_key(key, output_value);

	if (err < 0) {
		// Failed to find the variable or there are invalid arguments
		return -4;
	} else {
		printf("%s\n", output_value);
		return 0;
	}
}

/* ----------------------------------------------------------------------------
 * @brief Runs a script. Assumes null values were handled by the interpreter.
 * @param input  - parsed_words  An array of strings
 *        input  - num_of_words  The number of element in the array
 * @return int - Status code
 *                  0 - No errors
 *                 -5 - Script could not be executed
 *                 -7 - Number of arguments is not as expected          
 * ----------------------------------------------------------------------------
 */
int run (char *parsed_words[MAX_CMD_LENGTH], int num_of_words) {
	int err;
	if (num_of_words != 2) {
		printf(GENERIC_EXPECTED_MSG "run <filename>\n");
		return -7;
	}

	char *filename = parsed_words[1];
	err = read_and_exec_file(filename);

	// If file could not be read or directory issues
	if (err != 0) {
		return -5;
	}

	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Sets a variable. Assumes null values were handled by the interpreter.
 * @param input  - parsed_words  An array of strings
 *        input  - num_of_words  The number of element in the array
 * @return int - Status code
 *                  0 - No errors
 *                 -6 - Variable not set properly
 *                 -7 - Number of arguments is not as expected
 * ----------------------------------------------------------------------------
 */
int set_var (char *parsed_words[MAX_CMD_LENGTH], int num_of_words) {
	int i, j, err;
	char *key = parsed_words[1];
	char value[MAX_CMD_LENGTH];
	int count = 0;
	// Checks if the right number of arguments is obtained
	if (num_of_words < 3) {
		printf(GENERIC_EXPECTED_MSG "set <varname> <value>\n"); 
		return -7;
	}

	// Used to concatenate multiple string arguments together 
	for (i = 2; i < num_of_words; i++) {
		for (j = 0; j < strlen(parsed_words[i]); j++) {
			value[count++] = parsed_words[i][j];
		}
		// Insert a space
		value[count++] = ' ';
	}
	// Null-terminate the string
	value[count] = '\0';

	// Attempt to insert the node
	err = insert(0, key, value);
	if (err == -1) {
		// Key was found so update value
		err = update_value_by_key(key, value);
	}

	// Variable was not set properly
	if (err != 0) {
		return -6;
	}

	printf("Variable %s set\n", key);
	return err;
}

/* ----------------------------------------------------------------------------
 * @brief Reads the content of a file and executes every line.
 * @param input  - filename   A filename
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - Filename is null
 *                 -2 - Destination does not exist
 *                 -3 - Failed to read file
 * ----------------------------------------------------------------------------
 */
int read_and_exec_file (char *filename) {
	char line[MAX_LINE_LENGTH];

	// Checks if filename is null
	if (!filename) {
		return -1;
	}

	FILE *file = fopen(filename, "r");

	// If the file exists, read each line
	if (file) {
		while (fgets(line, MAX_LINE_LENGTH, file)) {
			execute_line_from_script(line);
		}
		fclose(file);
		return 0;
	}
	return -2;
}

/* ----------------------------------------------------------------------------
 * @brief Executes a line from a script.
 * @param input  - cmd    - A string of command
 * ----------------------------------------------------------------------------
 */
void execute_line_from_script(char *line) {
	int err;
	char *words[MAX_CMD_LENGTH];
	char trimmed_cmd[MAX_CMD_LENGTH];
	int trimmed_cmd_len;
	int num_of_words;

	printf("$%s", line);
	trimmed_cmd_len = trim(line, strlen(line), trimmed_cmd);

	// Parse through the user input
	if (trimmed_cmd_len > 0) {
		num_of_words = parser(trimmed_cmd, words);
	} else {
		num_of_words = 0;
	}

	err = interpret(words, num_of_words);
	handle_error(err);

	// Clear line
	printf("\n");
}

/* ----------------------------------------------------------------------------
 * @brief Executes scripts concurrently.
 * @param input  - parsed_words  - An array of strings
 *        input  - num_of_words  - Number of elements in the array
 * @return int - Status code
 *                  0 - No errors
 *                 -7 - Number of arguments is not as expected
 *                 -8 - Could no execute script due to lack of memory
 * ----------------------------------------------------------------------------
 */
int exec(char *parsed_words[MAX_CMD_LENGTH], int num_of_words) {
	FILE *files[3];
	char word[3][MAX_CMD_LENGTH];
	int i = 0;
	if (num_of_words > 4 || num_of_words < 2) {
		printf(GENERIC_EXPECTED_MSG "exec <script1> [<script2>] [<script3>]\n"); 
		return -7;
	}

	// Load into memory
	for (i = 1; i < num_of_words; i++) {
		strncpy(word[i - 1], parsed_words[i], MAX_CMD_LENGTH);
		files[i - 1] = fopen(word[i - 1], "r");

		// Cannot find file
		if (!files[i - 1]) {
			printf("%s cannot be found\n", word[i - 1]);
			continue;
		} else {
			myinit(files[i - 1]);
		}
	}

	for (i = 0; i < MAX_FILE_NUMBER; i++) {
		printf("%p\n", files[i]);
	}

	scheduler();

	return 0;
}
