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
void addToReady(pcb_t *pcb);
pcb_t *removeFromReady();

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
	if (addToRAM(file) != 0) {
		pcb = makePCB(file);
		if (!pcb) {
			// If PCB didn't create successfully or if the RAM is full, free memory 
		  // for pcb and remove file from RAM
			return -1;
		}
		// If PCB was successfully created, add to ready queue
		addToReady(pcb);
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
	pcb = removeFromReady();
	context_switch(pcb);
	while(pcb) {
		// Execute
		if (execute() == 0) {
			// Process still has lines
			addToReady(pcb);
		} else {
			printf("TASK has finished\n");
			// TODO: process finished executing
			// FREE-UP RAM
			// FREE-UP PCB
		}

		// Obtain the next 
		pcb = removeFromReady();
		context_switch(pcb);
	}
}

/* ----------------------------------------------------------------------------
 * @brief Removes a PCB from the ready queue
 * ----------------------------------------------------------------------------
 */
pcb_t *removeFromReady() {
	if (!queue->first) {
//		printf("TEST No PCB in queue \n");
		// Queue is empty
		return NULL;
	} else if (queue->first == queue->last) {
//		printf("TEST 1 PCB in queue \n");
		// Ready queue has 1 PCB
		pcb_t *temp = queue->first;
		queue->first = NULL;
		queue->last = NULL;
		return temp;
	} else {
//		printf("TEST 2 PCBs in queue \n");
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
void addToReady(pcb_t *pcb) {
//	printf("TEST adding to ready queue\n");
	if (!queue->first || !queue->last) {
//		printf("No PCB in queue\n");
		// First PCB in queue
		queue->first = pcb;
		queue->last = pcb;
//		printf("TEST queue->first %d \n", queue->first);
//		printf("TEST queue->last %d \n", queue->last);
	} else {
//		printf("Other PCB in queue\n");
		// Other PCBs are in queue
//		printf("TEST queue->last %d \n", queue->last);
//		printf("TEST queue->last->next %d \n", queue->last->next);
		queue->last->next = pcb;
		queue->last = pcb;
	}
//	printf("TEST finished adding to ready queue\n");
}

