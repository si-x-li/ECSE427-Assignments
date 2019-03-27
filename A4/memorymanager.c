/* ----------------------------------------------------------------------------
 * @file MEMORYMANAGER.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file contains implementation for the memory manager.
 * ----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "memorymanager.h"
#include "kernel.h"
#include "constant.h"

/* ----------------------------------------------------------------------------
 * @brief Returns the number of pages required for a file.
 * @param input  - file  - A file pointer
 * @return Number of pages required for a file. -1 if the file pointer is null
 * ----------------------------------------------------------------------------
 */
int count_total_pages(FILE *file) {
	// File pointer is null
	if (!file) {
		return -1;
	}

	char line[MAX_CMD_LENGTH];
	int count = 0;
	fseek(file, 0, SEEK_SET);

	while (fgets(line, MAX_CMD_LENGTH, file)) {
		count++;
	}

	fseek(file, 0, SEEK_SET);

	if (count % PAGE_SIZE == 0) {
		return count / PAGE_SIZE;
	} else {
		return (count / PAGE_SIZE) + 1;
	}
}

/* ----------------------------------------------------------------------------
 * @brief Finds the page at the specified page number.
 * @param input  - page_number - The page to be returned
 *        input  - file        - A pointer to a file (destroyed) 
 * @return A file pointer beginning at the specified page number
 *  ----------------------------------------------------------------------------
 */
FILE *find_page(int page_number, FILE *file) {
	if (!file) {
		return NULL;
	}

	if (page_number < 0) {
		return NULL;
	}

	int count = page_number * PAGE_SIZE;
	char line[MAX_CMD_LENGTH];
	fseek(file, 0, SEEK_SET);

	while (count != 0 && fgets(line, MAX_CMD_LENGTH, file)) {
		count--;
	}

	return file;
}

/* ----------------------------------------------------------------------------
 * @brief Finds if the page is stored inside of memory.
 * @param input  - A file pointer to a page
 * @return A positive integer indicating the position of the page in memory.
 *         -1 otherwise
 * ----------------------------------------------------------------------------
 */
int find_frame(FILE *page) {
	int i = 0;
	for (i = 0; i < RAM_SIZE; i++) {
		if (ram[i] != NULL && ram[i] == page) {
			return i;
		}
	}

	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Find a victim. If a victim cannot be found after checking each frame
 *        then the initial random frame will be selected.
 * @param input  - pcb - The process wanting to obtain a new frame
 * @return A number between 0 and RAM_SIZE;.
 * ----------------------------------------------------------------------------
 */
int find_victim(pcb_t *pcb) {
	int r, i;
	
	// Randomly select the victim
	r = rand() % RAM_SIZE;
	
	// Checks that the victim is not the process itself
	if (pcb->page_table[r] == 0) {
		return r;
	}

	// Iteratively increase until a spot is found to be occupied by another PCB
	for (i = r; i < r + RAM_SIZE; i++) {
		if (pcb->page_table[i % r] == 0) {
			return (i % r);
		}
	}

	// The entire RAM is filled with pages of the process
	return r;
}

/* ----------------------------------------------------------------------------
 * @brief Overwrites a frame with the content of page.
 * @param 
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - Frame is greater than RAM size
 *                 -2 - Page is null
 * ----------------------------------------------------------------------------
 */
int update_frame(int frame_number, int victim_frame, FILE *page) {
	// Checks that frame numbers are within bounds
	if (frame_number >= RAM_SIZE || victim_frame >= RAM_SIZE) {
		return -1;
	}
	
	// Checks that page is not null
	if (!page) {
		return -2;
	}

	if (frame_number != -1) {
		if (ram[frame_number]) {
			fclose(ram[frame_number]);
		}
		ram[frame_number] = page;
	} else {
		if (ram[victim_frame]) { 
			fclose(ram[victim_frame]);
		}
		ram[victim_frame] = page;
	}
	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Updates the PCB's page table.
 * @param input  - pcb          - A pointer to a PCB
 *        input  - page_number  - The page number that's being stored
 *        input  - frame_number - The number of the frame
 *        input  - victim_frame - The selected victim
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - PCB is null
 *                 -2 - Frame number is greater than RAM size
 * ----------------------------------------------------------------------------
 */
int update_page_table(pcb_t *pcb,
                      int page_number,
                      int frame_number,
                      int victim_frame) {
	if (!pcb) {
		return -1;
	}

	if (frame_number >= RAM_SIZE || victim_frame >= RAM_SIZE) {
		return -2;
	}

	if (frame_number != -1) {
		pcb->page_table[frame_number] = page_number;
	} else {
		pcb->page_table[victim_frame] = page_number;
	}

	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Find empty frames within RAM.
 * @return A number of the position where an empty spot was found.
 *         -1 otherwise.
 * ----------------------------------------------------------------------------
 */
int find_empty_frame() {
	int i = 0;
	for (i = 0; i < RAM_SIZE; i++) {
		if (!ram[i]) {
			return i;
		}
	}
	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Copies files to the backing store and loads it into RAM
 * @param input  - file - A file pointer
 * @return int - Status code
 *                  1 - No error
 *                  0 - Failed to launch program
 * ----------------------------------------------------------------------------
 */
int launcher(FILE *file) {
	char filename[MAX_FILENAME_LENGTH];
	char line[MAX_CMD_LENGTH];
	FILE *new_file;

	// Copy file to the backing store
	sprintf(filename, "BackingStore/%p.txt", file);
	new_file = fopen(filename, "w");
	while (fgets(line, MAX_CMD_LENGTH, file)) {
		fprintf(new_file, "%s", line);
	}

	// Close both files
	fclose(file);

	// Reopen file from backing store in read mode
	new_file = freopen(filename, "r", new_file);

	// Initialize PCB and store into RAM
	if (myinit(new_file) != 0) {
		return 0;
	}

	return 1;
}
