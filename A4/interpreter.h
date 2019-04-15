/* ----------------------------------------------------------------------------
 * @file INTERPRETER.H
 * @author Si Xun Li - 260674916
 * @version 4.0
 * @brief This file is the header file for the interpreter.
 * ----------------------------------------------------------------------------
 */

#include "constant.h"
#include "pcb.h"

/*
 * Available functions from the interpreter
 */
int interpret(char **parsed_words, int num_of_words, pcb_t *pcb, int is_cpu);
int is_number(char *word);
void run_line_from_script(char *line, pcb_t *pcb, int is_cpu);
