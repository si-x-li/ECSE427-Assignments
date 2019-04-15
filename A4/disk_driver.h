/* ----------------------------------------------------------------------------
 * @file disk_driver.h
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file is the disk driver's header file. 
 * ----------------------------------------------------------------------------
 */

void initIO();
int partition_drive(char *name, int total_blocks, int block_size);
int mount(char *name);
int open_file(char *name);
int read_block(int file);
char *return_block();
int write_block(int file, char *data);
int get_block_size();
void debug_disk_driver();
