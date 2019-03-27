/* ----------------------------------------------------------------------------
 * @file PCB.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief Contains functions for the PCB.
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "kernel.h"
#include "pcb.h"
#include "memorymanager.h"

/* ----------------------------------------------------------------------------
 * @brief Creates a PCB. To maintain compatibility with all other functions
 *        makePCB returns an int and not a pointer. The created PCB is returned
 *        as a reference instead.
 * @param input  - file  A pointer to a file
 *        output - pcb   A NULL pointer that will be overwritten
 * @return pcb_t - A PCB pointer
 * ----------------------------------------------------------------------------
 */
pcb_t *make_pcb(FILE *file) {
	pcb_t *pcb;
	int i, frame_number, victim_number;
	FILE *page[2];

	// Check if file pointer is null
	if (!file) {
		return NULL;
	}

	// Allocate memory space for a new PCB
	pcb = (pcb_t *) malloc(sizeof(pcb_t));

	if (pcb) {
		pcb->pc = file;
		pcb->pages_max = count_total_pages(file);
		pcb->pc_page = 1;
		pcb->pc_offset = 0;

		for (i = 0; i < RAM_SIZE; i++) {
			pcb->page_table[i] = 0;
		}

		// Load 2 frames into RAM
		for (i = 0; i < INITIAL_FRAME_NUMBER; i++) {
			// Duplicate opened file
			page[i] = fdopen(dup(fileno(pcb->pc)), "r");
			page[i] = find_page(i + 1, page[i]);
			frame_number = find_empty_frame();
			if (frame_number == -1) {
				// Select a victim and take its place in memory
				victim_number = find_victim(pcb);
				update_frame(frame_number, victim_number, page[i]);
				update_victim_page_table(frame_number, victim_number);
				update_page_table(pcb, i + 1, frame_number, victim_number);
			} else {
				// No victim needed
				update_frame(frame_number, -1, page[i]);
				update_page_table(pcb, i + 1, frame_number, -1);
			}
			fseek(pcb->pc, 0, SEEK_SET);
		}

		return pcb;
	} else {
		return NULL;
	}
}

/* ----------------------------------------------------------------------------
 * @brief Frees up a PCB and all of its resources.
 * @param input  - pcb  A PCB to be freed.
 * ----------------------------------------------------------------------------
 */
void free_pcb(pcb_t *pcb) {
	int i = 0;

	for (i = 0; i < RAM_SIZE; i++) {
		if (pcb->page_table[i] != 0) {
			fclose(ram[i]);
			ram[i] = NULL;
		}
	}

	free(pcb);
}
