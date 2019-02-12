/* ----------------------------------------------------------------------------
 * @file PCB.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the header file for the PCB.
 * ----------------------------------------------------------------------------
 */

/*
 * PCB structure
 */
typedef struct pcb pcb_t;

struct pcb {
	FILE *pc;
	pcb_t *next;
};

/*
 * PCB interfaces
 */
pcb_t *make_pcb(FILE *file);
void free_pcb(pcb_t *pcb);
