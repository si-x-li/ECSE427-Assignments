/* ----------------------------------------------------------------------------
 * @file RAM.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief 
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "ram.h"

/*
 * RAM structure
 */
typedef struct ram ram_t;
struct ram {
	FILE *files[RAM_SIZE];
};
ram_t *memory;

/* ----------------------------------------------------------------------------
 * @brief Initializes the RAM.
 * ----------------------------------------------------------------------------
 */
void init_ram() {
	memory = (ram_t *) malloc(sizeof(ram_t));
}

/* ----------------------------------------------------------------------------
 * @brief Inserts the file pointer to the next available spot.
 * @param input  - file    A file pointer
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - Failed to find free spot in memory
 * ----------------------------------------------------------------------------
 */
int add_to_ram(FILE *file) {
	int i;
	for (i = 0; i < RAM_SIZE; i++) {
		if(!memory->files[i]) {
			memory->files[i] = file;
			return 0;
		}
	}

	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Removes a file pointer.
 * @param input  - file    A file pointer
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - Failed to find process in memory
 * ----------------------------------------------------------------------------
 */
int remove_from_ram(FILE *file) {
	int i;
	for (i = 0; i < RAM_SIZE; i++) {
		// Check if not null first, then compare
		if (memory->files[i] && memory->files[i] == file) {
			fclose(memory->files[i]);
			memory->files[i] = NULL;
			return 0;
		}
	}
	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Prints the content of the RAM.
 * ----------------------------------------------------------------------------
 */
void print_ram() {
	int i;
	for (i = 0; i < RAM_SIZE; i++) {
		printf("%d %p\n", i, memory->files[i]);
	}
}
