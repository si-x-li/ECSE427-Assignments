/* ----------------------------------------------------------------------------
 * @file KERNEL.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the header file for the KERNEL.
 * ----------------------------------------------------------------------------
 */

#include "constant.h"
#include "pcb.h"

/*
 * Global variable for RAM
 */
#ifndef KERNEL_RAM_H
#define KERNEL_RAM_H
FILE *ram[RAM_SIZE];
#endif

/*
 * Available functions for the kernel
 */
void init_ready_queue();
int myinit(FILE *p);
void scheduler();
int update_victim_page_table(int frame_number, int victim_number);
