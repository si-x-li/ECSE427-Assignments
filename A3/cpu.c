/* ----------------------------------------------------------------------------
 * @file CPU.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the CPU. It executes a task for a specified quanta.
 * ----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cpu.h"
#include "kernel.h"
#include "interpreter.h"
#include "constant.h"
#include "memorymanager.h"

#define QUANTA 2

int page_fault();

/*
 * CPU structure
 */
typedef struct cpu cpu_t;
struct cpu {
	FILE *IP;
	char IR[MAX_CMD_LENGTH];
	int quanta;
	int offset;
};

cpu_t *cpu;
pcb_t *pcb_storage;

/* ----------------------------------------------------------------------------
 * @brief Initializes the CPU structure.
 * ----------------------------------------------------------------------------
 */
void init_cpu() {
	cpu = (cpu_t *) malloc(sizeof(cpu_t));
	pcb_storage = (pcb_t *) malloc(sizeof(pcb_t));
}

/* ----------------------------------------------------------------------------
 * @brief Loads the content of a PCB into the CPU and temporarily stores the
 *        PCB.
 * @param input  - pcb   A pointer to a PCB
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - PCB pointer is null
 * ----------------------------------------------------------------------------
 */
int context_switch(pcb_t *pcb) {
	if (!pcb) {
		return -1;
	}
	pcb_storage = pcb;
	cpu->IP = pcb->pc;
	cpu->offset = pcb->pc_offset;
	cpu->quanta = QUANTA;
	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Executes the file pointed loaded in the CPU.
 * @return int - Status code
 *                  0 - Ran out of quanta
 *                 -1 - EOF
 * ----------------------------------------------------------------------------
 */
int run() {
	while(cpu->quanta > 0 && cpu->offset < PAGE_SIZE) {
		cpu->quanta--;
		cpu->offset++;
		if (fgets(cpu->IR, MAX_CMD_LENGTH, cpu->IP)) {
			run_line_from_script(cpu->IR, 1);
		} else {
			// EOF
			return -1;
		}
	}

	return page_fault();
}

/* ----------------------------------------------------------------------------
 * @brief Checks if the process ran out of quanta or ran into a page fault. If
 *        it's the latter, first check if the
 * @return -1 if EOF was reached, 0 otherwise.
 * ----------------------------------------------------------------------------
 */
int page_fault() {
	int i, stored_in_frame, frame_number, victim_number;
	fpos_t pos;
	FILE *file;

	if (cpu->quanta == 0 && cpu->offset < PAGE_SIZE) {
		// Ran out of quanta
		pcb_storage->pc_offset = cpu->offset;
	} else {
		// Ran out of quanta and/or new page is required
		pcb_storage->pc_page++;
		
		if (pcb_storage->pc_page > pcb_storage->pages_max) {
			return -1;
		}

		pcb_storage->pc_offset = 0;
		stored_in_frame = 0;

		// Checks if the frame is already loaded into memory
		for (i = 0; i < RAM_SIZE; i++) {
			if (pcb_storage->page_table[i] == pcb_storage->pc_page) {
				stored_in_frame = 1;
				break;
			}
		}

		// Load page from disk
		if (stored_in_frame == 0) {
			// First make a copy of the position in the file stream
			fgetpos(cpu->IP, &pos);

			// Duplicate file pointer and find page
			file = fdopen(dup(fileno(cpu->IP)), "r");
			file = find_page(pcb_storage->pc_page, file);

			// Attempt to find an empty frame and a victim frame
			frame_number = find_empty_frame();

			if (frame_number == -1) {
				// Select a victim and overwrite its frame
				victim_number = find_victim(pcb_storage);

				// Overwrites the frame with new content
				update_frame(frame_number, victim_number, file);

				// Update page table for both victim and current pcb
				update_victim_page_table(frame_number, victim_number);
				update_page_table(pcb_storage, pcb_storage->pc_page,
			                    frame_number, victim_number);
			} else {
				// No victim needed
				update_frame(frame_number, -1, file);
				update_page_table(pcb_storage, pcb_storage->pc_page,
				                  frame_number, -1);
			}
			// Set the position back
			fsetpos(cpu->IP, &pos);
		}
	}

	return 0;
}
