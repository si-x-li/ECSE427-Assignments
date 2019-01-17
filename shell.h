/*
 *
 */
#define SHELL_NAME "Si Xun Li"
#define MAX_CMD_LENGTH 256

int prompt_command();
int trim(const char *str, int str_size, char *trimmed_str);
int parser(const char *str, char *words[MAX_CMD_LENGTH]);
