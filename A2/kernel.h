/* ----------------------------------------------------------------------------
 * @file KERNEL.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the header file for the KERNEL.
 * ----------------------------------------------------------------------------
 */

/*
 * Available functions for the kernel
 */
void init_ready_queue();
int myinit(FILE *p);
void scheduler();
