/* ----------------------------------------------------------------------------
 * @file MEMORYMANAGER.H
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the header file for the memory manager.
 * ----------------------------------------------------------------------------
 */
#include "pcb.h"

int count_total_pages(FILE *file);
FILE *find_page(int page_number, FILE *file);
int find_frame(FILE *page);
int find_victim(pcb_t *pcb);
int update_frame(int frame_number, int victim_frame, FILE *page);
int update_page_table(pcb_t *pcb,
                      int page_number,
                      int frame_number,
                      int victim_frame);
int find_empty_frame();
int launcher(FILE *file);
