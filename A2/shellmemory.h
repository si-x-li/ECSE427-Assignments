/* ----------------------------------------------------------------------------
 * @file SHELLMEMORY.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the header for shell memory. It also declares the
 *        linked list data structure.
 * ----------------------------------------------------------------------------
 */

/*
 * Declare more system-wide constants
 */
#define MAX_CMD_LENGTH          1024
#define MAX_KEY_LENGTH          1024
#define MAX_VALUE_LENGTH        1024
#define GENERIC_EXPECTED_MSG    "Expected: "
#define GENERIC_ERROR_MSG       "Error: "

/*
 * Declare maximum length of line that can be read
 */
#define MAX_LINE_LENGTH         1024

/*
 * Supported functions for singly linked list
 */
int init_linked_list();
int insert_first(char *key, char *value);
int insert(unsigned int position, char *key, char *value);
void print_traversal();
int remove_node_by_key(char *key);
int update_value_by_key(char *key, char *value);
int get_value(unsigned int position, char *output_value);
int get_value_by_key(char *key, char *output_value);
int search(char *key);
