/* ----------------------------------------------------------------------------
 * @file KERNEL.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file contains the kernel and the scheduler.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "kernel.h"
#include "ram.h"
#include "cpu.h"

/*
 * Ready queue implemented as FIFO and RR
 */
typedef struct ready_queue ready_queue_t;
struct ready_queue {
	pcb_t *first;
	pcb_t *last;
};

ready_queue_t *queue;

/*
 * Functions specific to kernel.c
 */
void add_to_ready(pcb_t *pcb);
pcb_t *remove_from_ready();

/* ----------------------------------------------------------------------------
 * @brief Initializes the ready queue.
 * ---------------------------------------------------------------------------- 
 */
void init_ready_queue() {
	queue = (ready_queue_t *) malloc(sizeof(ready_queue_t));
}

/* ----------------------------------------------------------------------------
 * @brief Loads the program to RAM, creates the PCB and add the program to the
 *        ready queue.
 * @param input  - file   A file pointer
 * @return int - Status Code
 *                  0 - No errors
 *                 -1 - Failed to create the PCB
 * ----------------------------------------------------------------------------
 */
int myinit(FILE *file) {
	// Allocate memory to PCB
	pcb_t *pcb;
	// Add the file to RAM and create the PCB
	if (add_to_ram(file) == 0) {
		pcb = make_pcb(file);
		if (!pcb) {
			// If PCB didn't create successfully or if the RAM is full, free memory 
		  // for pcb and remove file from RAM
			return -1;
		}
		// If PCB was successfully created, add to ready queue
		add_to_ready(pcb);
	} else {
		return -1;
	}
	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Mimics the scheduler.
 * ----------------------------------------------------------------------------
 */
void scheduler() {
	pcb_t *pcb;
	// Check if there are any tasks scheduled
	if (!queue->first) {
		printf("No tasks scheduled\n");
		return;
	}

	// Indicate start of execution
	printf("Executing\n");
	pcb = remove_from_ready();
	context_switch(pcb);
	while(pcb) {
		// Execute
		if (execute() == 0) {
			// Process still has lines
			add_to_ready(pcb);
		} else {
			// Free up RAM and PCB
			printf("TASK has finished\n");
			print_ram();
			remove_from_ram(pcb->pc);
			free_pcb(pcb);
			print_ram();
		}

		// Obtain the next 
		pcb = remove_from_ready();
		context_switch(pcb);
	}
}

/* ----------------------------------------------------------------------------
 * @brief Removes a PCB from the ready queue
 * ----------------------------------------------------------------------------
 */
pcb_t *remove_from_ready() {
	if (!queue->first) {
		// Queue is empty
		return NULL;
	} else if (queue->first == queue->last) {
		// Ready queue has 1 PCB
		pcb_t *temp = queue->first;
		queue->first = NULL;
		queue->last = NULL;
		return temp;
	} else {
		// Ready queue has 2 or more PCBs
		pcb_t *temp = queue->first;
		queue->first = temp->next;
		temp->next = NULL;
		return temp;
	}
}

/* ----------------------------------------------------------------------------
 * @brief Adds a PCB to the ready queue.
 * @param input  - pcb  A pointer to a PCB
 * ----------------------------------------------------------------------------
 */
void add_to_ready(pcb_t *pcb) {
	if (!queue->first || !queue->last) {
		// First PCB in queue
		queue->first = pcb;
		queue->last = pcb;
	} else {
		// Other PCBs are in queue
		queue->last->next = pcb;
		queue->last = pcb;
	}
}

