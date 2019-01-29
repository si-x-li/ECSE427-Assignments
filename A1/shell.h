/* ----------------------------------------------------------------------------
 * @file SHELL.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the header for the shell.
 * ----------------------------------------------------------------------------
 */

#include "shellmemory.h"

/*
 * Define system-wide constants
 */
#define SHELL_NAME "Si Xun Li"
#define SHELL_VERSION "1.0"
#define TAB "    "

/*
 * Available functions from shell
 */
int prompt_command(linked_list_t *list);
int trim(const char *str, int str_size, char *trimmed_str);
int parser(const char *str, char *words[MAX_CMD_LENGTH]);
int handle_error(int err);
