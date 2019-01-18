#include "shellmemory.h"

/*
 * Define constants available
 */
#define SHELL_NAME "Si Xun Li"

/*
 * Available functions from shell
 */
int prompt_command(linked_list *list);
int trim(const char *str, int str_size, char *trimmed_str);
int parser(const char *str, char *words[MAX_CMD_LENGTH]);
int handle_error(int err);
