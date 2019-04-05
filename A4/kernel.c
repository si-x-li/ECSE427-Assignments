/* ----------------------------------------------------------------------------
 * @file KERNEL.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file contains the kernel and the scheduler.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kernel.h"
#include "disk_driver.h"
#include "cpu.h"
#include "ram.h"
#include "shellmemory.h"
#include "shell.h"

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
 * @brief Initializes data structures required for this simulator
 * ---------------------------------------------------------------------------- 
 */
void boot() {
	int i = 0;
	for (i = 0; i < RAM_SIZE; i++) {
		ram[i] = NULL;
	}

	// Recreate BackingStore if not done so already
	system("rm -rf BackingStore");
	system("mkdir BackingStore");
}

/* ----------------------------------------------------------------------------
 * @brief Loads the program to RAM, creates the PCB, PCB node and adds the
 *        program to the ready queue.
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
 * @brief Removes a PCB from the ready queue.
 * @return A PCB node if the ready queue is not empty. Null otherwise
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
 * @brief Adds a PCB node to the ready queue.
 * @param input  - pcb_node  A pointer to a PCB node
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

/* ----------------------------------------------------------------------------
 * @brief Finds the victim PCB and update its page table. This function is
 *        located here instead of memorymanager since it needs to look in the
 *        ready queue.
 * @param input  - frame_number   - An empty frame number
 *        input  - victim_number  - The victim frame
 * @return int - Status code
 *                  0 - No errors
 *                 -1 - An empty frame is available
 * ----------------------------------------------------------------------------
 */
int update_victim_page_table(int frame_number, int victim_number) {
	pcb_node_t *head, *temp;
	pcb_t *pcb;

	// An empty frame is available. No need to select the victim
	if (frame_number != -1) {
		return -1;
	}

	head = queue->first;
	temp = remove_from_ready();
	// Check each pcb if it is occupying the frame
	while(1) {
		add_to_ready(temp);
		pcb = temp->pcb;
		temp = remove_from_ready();

		// Found the victim PCB, update its page table
		if (pcb->page_table[victim_number] != 0) {
			pcb->page_table[victim_number] = 0;
		}

		// Circulated back
		if (temp == head) {
			break;
		}
	}
	add_to_ready(temp);
	return 0;
}


int main() {
	boot();

	// Initializes the ready queue
	queue = (ready_queue_t *) malloc(sizeof(ready_queue_t));

	// Initializes data structures used for this assignment
	init_cpu();
	init_shell_memory();

	// Initializes the RAM. This function still has to be called due to dependency
	init_ram();

	initIO();

	// Keep track of errors
	int err = 0;

	// Print welcome prompt
	printf("Kernel %s loaded!\n"
	       "Welcome to the %s shell!\n"
	       "Version %s Updated %s\n",
	       KERNEL_VERSION, SHELL_NAME, SHELL_VERSION, UPDATE_DATE);

	while(err == 0) {
		err = prompt_command();
		err = handle_error(err);

		// Clear line
		printf("\n");
	}
}

