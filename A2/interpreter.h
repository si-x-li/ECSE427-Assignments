/* ----------------------------------------------------------------------------
 * @file INTERPRETER.H
 * @author Si Xun Li - 260674916
 * @version 2.0
 * @brief This file is the header file for the interpreter.
 * ----------------------------------------------------------------------------
 */

/*
 * Available functions from the interpreter
 */
int interpret(char **parsed_words, int num_of_words);
void execute_line_from_script(char *line);
