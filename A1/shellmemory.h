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
 * Declare singly linked list data structure
 */
typedef struct node node_t;
typedef struct linked_list linked_list_t;

struct node {
	char *key;
	char *value;
	node_t *next;
};

struct linked_list {
	node_t *head;
	unsigned int size;
};

/*
 * Supported functions for singly linked list
 */
int insert_first(linked_list_t *list, char *key, char *value);
int insert(linked_list_t *list, unsigned int position, char *key, char *value);
void print_traversal(linked_list_t *list);
void print_node(node_t *node);
int remove_node_by_key(linked_list_t *list, char *key);
int update_value_by_key(linked_list_t *list, char *key, char *value);
int get_value(linked_list_t *list, unsigned int position, char *output_value);
int get_value_by_key(linked_list_t *list, char *key, char *output_value);
int search(linked_list_t *list, char *key);
