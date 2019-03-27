/* ----------------------------------------------------------------------------
 * @file RAM.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the header file for the RAM.
 * ----------------------------------------------------------------------------
 */

/*
 * Functions for RAM
 */
void init_ram();
int add_to_ram(FILE *file);
int remove_from_ram(FILE *file);
void clear_ram();
void print_ram();
