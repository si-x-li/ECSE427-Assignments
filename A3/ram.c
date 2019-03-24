/* ----------------------------------------------------------------------------
 * @file RAM.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file contains the legacy RAM structure.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "ram.h"
#include "constant.h"

/*
 * RAM structure
 */
typedef struct ram ram_t;
struct ram {
	FILE *files[RAM_SIZE * RAM_SIZE];
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
	for (i = 0; i < (RAM_SIZE * RAM_SIZE); i++) {
		if(!memory->files[i]) {
			memory->files[i] = file;
			printf("Loaded file to memory\n");
			return 0;
		}
	}

	printf("No memory available to load file\n");
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
	for (i = 0; i < (RAM_SIZE * RAM_SIZE); i++) {
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
 * @brief Clear RAM due to recursive exec calls.
 * ----------------------------------------------------------------------------
 */
void clear_ram() {
	int i;
	for (i = 0; i < (RAM_SIZE * RAM_SIZE); i++) {
		if (memory->files[i]) {
			fclose(memory->files[i]);
			memory->files[i] = NULL;
		}
	}
}

/* ----------------------------------------------------------------------------
 * @brief Prints the content of the RAM.
 * ----------------------------------------------------------------------------
 */
void print_ram() {
	int i;
	for (i = 0; i < (RAM_SIZE * RAM_SIZE); i++) {
		printf("%d %p\n", i, memory->files[i]);
	}
}
