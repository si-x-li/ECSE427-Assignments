/* ----------------------------------------------------------------------------
 * @file RAM.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief 
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include "ram.h"

/*
 * Memory
 */
FILE *ram[RAM_SIZE];

/* ----------------------------------------------------------------------------
 * @brief Inserts the file pointer to the next available spot.
 * @param input  - file    A file pointer
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - Failed to find free spot in memory
 * ----------------------------------------------------------------------------
 */
int addToRAM(FILE *file) {
	int i;
	for (i < 0; i < RAM_SIZE; i++) {
		if(!ram[i]) {
			ram[i] = file;
			return 0;
		}
	}

	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Removes a file pointer.
 * @param input  - i    An index in RAM to set to NULL
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - Failed to find process in memory
 * ----------------------------------------------------------------------------
 */
int removeFromRAM(FILE *file) {
	int i;
	for (i < 0; i < RAM_SIZE; i++) {
		if (ram[i] == file) {
			fclose(ram[i]);
			ram[i] = NULL;
			return 0;
		}
	}
	return -1;
}
