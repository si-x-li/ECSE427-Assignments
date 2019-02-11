/* ----------------------------------------------------------------------------
 * @file PCB.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief Contains functions for the PCB.
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"

/* ----------------------------------------------------------------------------
 * @brief Creates a PCB. To maintain compatibility with all other functions
 *        makePCB returns an int and not a pointer. The created PCB is returned
 *        as a reference instead.
 * @param input  - file  A pointer to a file
 *        output - pcb   A NULL pointer that will be overwritten
 * @return pcb_t - A PCB pointer
 * ----------------------------------------------------------------------------
 */
pcb_t *makePCB(FILE *file) {
	pcb_t *pcb;
	// Check if file pointer is null
	if (!file) {
		return NULL;
	}

	// Populate the program pointer of the PCB
	pcb = (pcb_t *) malloc(sizeof(pcb_t));
	if (pcb) {
		pcb->pc = file;
		return pcb;
	} else {
		return NULL;
	}
}

/* ----------------------------------------------------------------------------
 * @brief Frees up a PCB.
 * @param input  - pcb  A PCB to be freed.
 * ----------------------------------------------------------------------------
 */
void freePCB(pcb_t *pcb) {
	fclose(pcb->pc);
	free(pcb);
}
