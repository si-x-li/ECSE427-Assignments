/* ----------------------------------------------------------------------------
 * @file CPU.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This is the header file for the CPU.
 * ----------------------------------------------------------------------------
 */

#include "pcb.h"

/*
 * Public functions
 */
void init_cpu();
int context_switch(pcb_t *pcb);
int run();
