/* ----------------------------------------------------------------------------
 * @file SHELLMEMORY.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file contains all functions related to the shell memory.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shellmemory.h"

/* ----------------------------------------------------------------------------
 * @brief Inserts a node at the start of the linked list.
 * @param input  - A singly linked list to insert a node into
 *        input  - A key to insert
 *        input  - A value to insert
 * @return int - Status code
 *                  0 - No error
 *                 -1 - Key already exists
 *                 -2 - Linked list is null
 *                 -3 - Key is null
 *                 -6 - Could not allocate new memory space
 * ----------------------------------------------------------------------------
 */
int insert_first (linked_list_t *list, char *key, char *value) {
	// Checks if the linked list is null
	if (!list) {
		return -2;
	}

	// Checks if the key is null
	if (!key) {
		return -3;
	}

	// Proceed with insertion only if the key doesn't exist
	if (search(list, key) >= 0) {
		return -1;
	}

	// Standard insertion technique for singly linked list
	node_t *new_node = (node_t *) malloc(sizeof(node_t));
	char *new_key = (char *) malloc(strlen(key) + 1);
	char *new_value = (char *) malloc(strlen(value) + 1);

	// Check if memory is allocated
	if (new_node && new_key && new_value) {	
		// Copy keys and values over
		strncpy(new_key, key, strlen(key));
		new_key[strlen(key)] = '\0';
		strncpy(new_value, value, strlen(value));
		new_value[strlen(value)] = '\0';

		new_node->key = new_key;
		new_node->value = new_value;
		new_node->next = list->head;

		
		list->head = new_node;
		list->size = list->size + 1;

		return 0;
	}
	return -6;
}

/* ----------------------------------------------------------------------------
 * @brief Inserts a node at a specified position;
 * @param input  - list       A singly linked list
 *        input  - position   The position where to insert the key and value
 *        input  - key        A string key
 *        input  - value      A string value
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - Key already exist
 *                 -2 - Linked list is null
 *                 -3 - Position out of bounds
 *                 -4 - Key is null
 *                 -5 - Value is null
 *                 -6 - Could not allocate new memory space
 * ----------------------------------------------------------------------------
 */
int insert (linked_list_t *list, unsigned int position, char *key, char *value) {
	int i;
	int size;
	node_t *current_node;
	node_t *next_node;

	// Checks if list is null
	if (!list) {
		return -2;
	}

	size = list->size;

	// Checks if position is in permissible bounds
	if (position > size) {
		return -3;
	}

	// Checks if key is null
	if (!key) {
		return -4;
	}

	// Checks if value is null
	if (!value) {
		return -5;
	}

	// Proceed only with insertion if the key doesn't exist
	if (search(list, key) >= 0) {
		return -1;
	}

	if (position == 0) {
		return insert_first(list, key, value);
	}

	// Create a new node
	node_t *new_node = (node_t *) malloc(sizeof(node_t));
	char *new_key = (char *) malloc(strlen(key) + 1);
	char *new_value = (char *) malloc(strlen(value) + 1);

	// Check if malloc has completed properly
	if (new_node && new_key && new_value) {
		// Copy key and value over
		strncpy(new_key, key, strlen(key));
		new_key[strlen(key)] = '\0';
		strncpy(new_value, value, strlen(value));
		new_value[strlen(value)] = '\0';

		new_node->key = new_key;
		new_node->value = new_value;

		// Fetch the position where the new node is being inserted
		current_node = list->head;
		for (i = 0; i < position - 1; i++) {
			current_node = current_node->next;
		}

		// Insert the node
		next_node = current_node->next;
		new_node->next = next_node;
		current_node->next = new_node;
		list->size = size + 1;

		return 0;
	}

	// Failed to allocate memory
	return -6;
}

/* ----------------------------------------------------------------------------
 * @brief Traverses the singly linked list and prints the key<->value pair
 *        for each node.
 * @param input  - list     A singly linked list
 * ----------------------------------------------------------------------------
 */
void print_traversal (linked_list_t *list) {
	int i;
	node_t *current_node;
	int size;

	// Checks if list is null
	if (!list) {
		return;
	}

	size = list->size;

	current_node = list->head;
	for (i = 0; i < size; i++) {
		printf("<Index: %d Key: %s Value: %s>\n", i,
		       current_node->key, current_node->value);
		current_node = current_node->next; 
	}
}

/* ----------------------------------------------------------------------------
 * @brief Prints data contained within a node.
 * @param input  - node     A singly linked list node
 * ----------------------------------------------------------------------------
 */
void print_node (node_t *node) {
	// Check if node is null
	if (!node) {
		return;
	}
	printf("<Key: %s Value: %s>\n", node->key, node->value);
}


/* ----------------------------------------------------------------------------
 * @brief Removes a node from the linked list using a key.
 * @param input  - list    A singly linked list
 *        input  - key     A string key
 * @return int - Status code
 *                  0 - No error
 *                 -1 - Could not find key
 *                 -2 - list is null
 *                 -3 - key is null
 * ----------------------------------------------------------------------------
 */
int remove_node_by_key (linked_list_t *list, char *key) {
	int i;
	int index;
	node_t *previous_node;
	node_t *current_node;

	// Checks if list is null
	if (!list) {
		return -2;
	}

	// Checks if key is null
	if (!key) {
		return -3;
	}

	// Checks if the key is present
	index = search(list, key);

	if (index < 0) {
		return -1;
	}

	// Take care of special case
	if (index == 0) {
		current_node = list->head;
		list->head = current_node->next;
	} else {
		previous_node = list->head;
		for (i = 0; i < index - 1; i++) {
			previous_node = previous_node->next;
		}
		current_node = previous_node->next;
		previous_node->next = current_node->next;
	}

	// Free up allocated memory
	free(current_node->value);
	free(current_node->key);
	free(current_node);

	list->size = list->size - 1;
	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Updates a node using a key.
 * @param input  - list     A singly linked list
 *        input  - key      A string key
 *        input  - value    A string value
 * @return int - Status code
 *                  0 - No error
 *                 -1 - Could not find key
 *                 -2 - list is null
 *                 -3 - key is null
 *                 -4 - value is null
 *                 -6 - Failed to allocate resource
 * ----------------------------------------------------------------------------
 */
int update_value_by_key (linked_list_t *list, char *key, char *value) {
	int i;
	int index;
	node_t *current_node;
	// Checks if list is null
	if (!list) {
		return -2;
	}

	// Checks if key is null
	if (!key) {
		return -3;
	}

	// Checks if value is null
	if (!value) {
		return -4;
	}

	index = search(list, key); 
	if (index >= 0) {
		// Update value
		current_node = list->head;
		for (i = 0; i < index; i++) {
			current_node = current_node->next;
		}

		// Free up previous value
		free(current_node->value);

		// Allocate new value resource
		char *new_value = (char *) malloc(strlen(value) + 1);

		// Failed to allocate resource
		if (!new_value) {
			return -6;
		}

		// Copy over new value
		strncpy(new_value, value, strlen(value));
		new_value[strlen(value)] = '\0';
		current_node->value = new_value;
		return 0;
	} else {
		return -1;
	}
}

/*
 * ----------------------------------------------------------------------------
 * @brief Returns the value at a position in the linked list.
 * @brief input  - list          A singly linked list to check
 *        input  - position      The position of the value to return
 *        output - output_value  Returned value
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - list is null
 *                 -2 - Position is out of bounds
 *                 -3 - Output is null
 * ----------------------------------------------------------------------------
 */
int get_value (linked_list_t *list, unsigned int position, char *output_value) {
	int i;
	node_t *current_node;
	int size;

	// Check if list is null
	if (!list) {
		output_value = NULL;
		return -1;
	}
	
	size = list->size;

	// Check if position is contained in bounds
	if (position > size) {
		output_value = NULL;
		return -2;
	}

	// Check if output is null
	if (!output_value) {
		return -3;
	}

	current_node = list->head;
	for (i = 0; i < (int) (position - 1); i++) {
		current_node = current_node->next;
	}

	strcpy(output_value, current_node->value);
	output_value[strlen(current_node->value)] = '\0';
	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Searches the linked list for the value where the key was found. If
 *        the key was found, return the index and value.
 * @param input  - list          A linked list
 *        input  - key           A key to find
 *        output - output_value  A string value to return
 * @return int - Returns the index where the key was found or status code.
 *                 -1 - Key was not found
 *                 -2 - list is null
 *                 -3 - key is null
 *                 -4 - output_value is null
 * ----------------------------------------------------------------------------
 */
int get_value_by_key (linked_list_t *list, char *key, char *output_value) {
	int i;
	node_t *current_node;
	int size;

	// Checks if the list is null
	if (!list) {
		return -2;
	}

	// Checks if the key is null
	if (!key) {
		return -3;
	}

	// Checks if output is null
	if (!output_value) {
		return -4;
	}

	size = list->size;

	if (size != 0) {
		current_node = list->head;
		for (i = 0; i < size; i++) {
			if (strcmp(current_node->key, key) == 0) {
				strncpy(output_value, current_node->value, strlen(current_node->value));
				output_value[strlen(current_node->value)] = '\0';
				return i;
			}
			current_node = current_node->next;
		}
	}
	output_value[0] = '\0';
	return -1;
}

/*
 * ----------------------------------------------------------------------------
 * @brief Searches a linked list to see if a key exists. O(n) performance.
 *        Could be improved by using a self-sorting tree structure such as AVL
 *        trees. However, for the sake of this assignment, it is not needed.
 * @param input  - list   A linked list to check
 *        input  - key    A string to check if it exists in the linked list
 * @return int - Returns the index where the key was found or a status code.
 *                 -1 - Key was not found
 *                 -2 - list is null
 *                 -3 - key is null
 * ----------------------------------------------------------------------------
 */
int search (linked_list_t *list, char *key) {
	int i;
	node_t *current_node;
	int size;

	// Checks if list is null
	if (!list) {
		return -2;
	}

	// Checks if key is null
	if (!key) {
		return -3;
	}

	// Linear search
	size = list->size;
	if (size != 0) {
//		printf("Searching... Entered size: %d ", list->size); //
		current_node = list->head;
		for (i = 0; i < size; i++) {
//			printf("Current node: %s Key: %s\n", current_node->key, key); //
			if (strcmp(current_node->key, key) == 0) {
				return i;
			}
			current_node = current_node->next;
		}
	}
	return -1;
}
