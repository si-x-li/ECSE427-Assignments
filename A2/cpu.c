/* ----------------------------------------------------------------------------
 * @file CPU.C
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the CPU. It executes a task for a specified quanta.
 * ----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "interpreter.h"
#include "constant.h"

#define QUANTA 2

/*
 * CPU structure
 */
typedef struct cpu cpu_t;
struct cpu {
	FILE *IP;
	char IR[MAX_CMD_LENGTH];
	int quanta;
};

cpu_t *cpu;

/* ----------------------------------------------------------------------------
 * @brief Initializes the CPU structure.
 * ----------------------------------------------------------------------------
 */
void init_cpu() {
	cpu = (cpu_t *) malloc(sizeof(cpu_t));
}

/* ----------------------------------------------------------------------------
 * @brief Loads the content of a PCB into the CPU.
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
	cpu->IP = pcb->pc;
	cpu->quanta = QUANTA;
	return 0;
}

/* ----------------------------------------------------------------------------
 * @brief Executes the file pointed loaded in the CPU.
 * @return int - Status code
 *                  0 - Ran out of quanta
 *                 -1 - Finished executing
 * ----------------------------------------------------------------------------
 */
int execute() {
	while(cpu->quanta > 0) {
		cpu->quanta--;
		if (fgets(cpu->IR, MAX_CMD_LENGTH, cpu->IP)) {
			execute_line_from_script(cpu->IR);
		} else {
			return -1;
		}
	}
	return 0;
}
