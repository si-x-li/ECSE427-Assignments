/* ----------------------------------------------------------------------------
 * @file io_scheduler.c
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file schedules IO operations.
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "io_scheduler.h"
#include "disk_driver.h"
#include "constant.h"

int current = 0;
struct WAIT_QUEUE {
	char *data;
	pcb_t *ptr;
	int cmd;
} wait_queue[10];

const int SIZE_OF_WAIT_QUEUE = sizeof(wait_queue) / sizeof(struct WAIT_QUEUE);
const int FAT_MASK = 0x3E;
const int CMD_MASK = 0x01;

int find_free_position();
void strip_irrelevant_values(char *input);
/* ----------------------------------------------------------------------------
 * @brief Since the program assumes there is no parallel processing,
 *        immediately process the request after it was scheduled.
 * @param input  - data - The data to be written to the file
 *        input  - pcb  - 
 *        input  - cmd  - The command to be processed masked with the FAT id
 * @return char * - Blocks read from the file
 * ----------------------------------------------------------------------------
 */
char *IO_scheduler(char *data, pcb_t *ptr, int cmd) {
	int i, count, skip, free_position, blocks_written;
	char *buffer;

	current = current % SIZE_OF_WAIT_QUEUE;

	free_position = find_free_position();
	if (free_position == -1) {
		return NULL;
	}

	// Add to the wait queue
	wait_queue[free_position].data = malloc(MAX_CMD_LENGTH);
	strcpy(wait_queue[free_position].data, data);
	wait_queue[free_position].ptr = ptr;
	wait_queue[free_position].cmd = cmd;

	// Read from file
	if ((cmd & CMD_MASK) == 0) {
		buffer = malloc(MAX_CMD_LENGTH);
		for (i = 0; i < MAX_CMD_LENGTH; i++) {
			buffer[i] = '\0';
		}

		blocks_written = 0;
		while(read_block((cmd & FAT_MASK) >> 1) == 1) {
			blocks_written++;
			strncat(buffer, return_block(), get_block_size());
			if (blocks_written >= 10) {
				break;
			}
		}
		strip_irrelevant_values(buffer);

		free(wait_queue[current].data);
		wait_queue[current].data = NULL;
		wait_queue[current].ptr = NULL;
		wait_queue[current].cmd = 0;
		current++;
		return buffer;
	}
	// Write to file
	else if ((cmd & CMD_MASK) == 1) {
		count = 0;   // Reset counter to keep track of the characters
		buffer = (char *) malloc(get_block_size() + 1);

		for (i = 0; i < get_block_size() + 1; i++) {
			buffer[i] = '\0';
		}

		// Copy characters into a buffer and send to be written to memory
		for (i = 0; i < get_block_size(); i++) {
			// Pad with NULL characters if there are no more entries
			if (count > (int) strlen(wait_queue[current].data)) {
				buffer[i] = '\0';
			}
			buffer[i] = wait_queue[current].data[count++];
		}

		// Checks if the initial input contains null
		skip = 0;
		for (i = 0; i < get_block_size(); i++) {
			if (buffer[i] == '\0') {
				skip = 1;
			}
		}

		blocks_written = 0;
		while(write_block((cmd & FAT_MASK) >> 1, buffer) == 1) {
			if (skip) {
				break;
			}
			for (i = 0; i < get_block_size(); i++) {
				buffer[i] = wait_queue[current].data[count++];
				if (wait_queue[current].data[count] == '\0') {
					skip = 1;
				}
			}
			blocks_written++;
			if (blocks_written >= 10) {
				break;
			}
		}
		free(buffer);
	}

	// Remove from wait queue
	free(wait_queue[current].data);
	wait_queue[current].data = NULL;
	wait_queue[current].ptr = NULL;
	wait_queue[current].cmd = 0;
	current++;

	return NULL;
}

/* ----------------------------------------------------------------------------
 * @brief Finds a free position in the circular wait queue.
 * @return int - A free position in the circular waiting queue. -1 if not found
 * ----------------------------------------------------------------------------
 */
int find_free_position() {
	int checked, currently_checking;
	checked = 0;
	currently_checking = current;
	
	while (checked < SIZE_OF_WAIT_QUEUE) {
		if (!wait_queue[currently_checking].data) {
			return currently_checking;
		}
		currently_checking = (currently_checking + 1) % SIZE_OF_WAIT_QUEUE;
		checked++;
	}

	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Removes all irrelevant data (0) padding
 * ----------------------------------------------------------------------------
 */
void strip_irrelevant_values(char *input) {
	int null_values, i;
	null_values = 1;
	for (i = MAX_CMD_LENGTH - 1; i >= 0; i--) {
		if (null_values == 1) {
			if (input[i] != '\0') {
				null_values = 0;
				if (input[i] == '0') {
					input[i] = '\0';
				} else {
					break;
				}
			}
		} else {
			if (input[i] == '0') {
				input[i] = '\0';
			} else {
				break;
			}
		}
	}
}
