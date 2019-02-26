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
#include "pcb.h"
#include "ram.h"
#include "cpu.h"

/*
 * Ready queue and pcb_node implemented as FIFO and RR
 */
typedef struct pcb_node pcb_node_t;
struct pcb_node {
	pcb_t *pcb;
	pcb_node_t *next;
};

typedef struct ready_queue ready_queue_t;
struct ready_queue {
	pcb_node_t *first;
	pcb_node_t *last;
};
ready_queue_t *queue;

/*
 * Functions specific to kernel.c
 */
void add_to_ready(pcb_node_t *pcb);
pcb_node_t *remove_from_ready();

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
	pcb_node_t *pcb_node;
	pcb_t *pcb;
	// Add the file to RAM and create the PCB
	if (add_to_ram(file) == 0) {
		pcb = make_pcb(file);
		pcb_node = (pcb_node_t *) malloc(sizeof(pcb_node_t));
		if (!pcb) {
			// If PCB was not created successfully, free RAM and close file
			remove_from_ram(file);
			fclose(file);
			return -1;
		}
		if (!pcb_node) {
			remove_from_ram(file);
			fclose(file);
			free_pcb(pcb);
			return -1;
		}
		pcb_node->pcb = pcb;
		// If PCB was successfully created, add to ready queue
		add_to_ready(pcb_node);
	} else {
		// If the file was not added to RAM, close file
		fclose(file);
		return -1;
	}
	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Mimics the scheduler.
 * ----------------------------------------------------------------------------
 */
void scheduler() {
	pcb_node_t *pcb_node;
	// Check if there are any tasks scheduled
	if (!queue->first) {
		printf("No tasks scheduled on the ready queue\n");
		return;
	}

	// Indicate start of execution
	printf("Start execution...\n\n");
	pcb_node = remove_from_ready();
	while(pcb_node) {
		// Switch the pcb into the cpu
		context_switch(pcb_node->pcb);
		// Execute
		if (run() == 0) {
			// Process still has lines add back to ready queue
			add_to_ready(pcb_node);
		} else {
			// Free up RAM and PCB
			remove_from_ram(pcb_node->pcb->pc);
			free_pcb(pcb_node->pcb);
			free(pcb_node);
		}

		// Obtain the next PCB node from the ready_queue
		pcb_node = remove_from_ready();
	}
	printf("Execution completed...\n");
}

/* ----------------------------------------------------------------------------
 * @brief Removes a PCB from the ready queue
 * ----------------------------------------------------------------------------
 */
pcb_node_t *remove_from_ready() {
	pcb_node_t *temp;
	if (!queue->first) {
		// Queue is empty
		return NULL;
	} else if (queue->first == queue->last) {
		// Ready queue has 1 PCB
		temp = queue->first;
		queue->first = NULL;
		queue->last = NULL;
		return temp;
	} else {
		// Ready queue has 2 or more PCBs
		temp = queue->first;
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
void add_to_ready(pcb_node_t *pcb_node) {
	if (!queue->first || !queue->last) {
		// First PCB in queue
		queue->first = pcb_node;
		queue->last = pcb_node;
	} else {
		// Other PCBs are in queue
		queue->last->next = pcb_node;
		queue->last = pcb_node;
	}
}
