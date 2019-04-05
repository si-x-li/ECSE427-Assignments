/* ----------------------------------------------------------------------------
 * @file DISK_driver.c
 * @author Si Xun Li - 260674916
 * @version 1.0
 * @brief This file implements the disk driver. 
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include "interpreter.h"
#include "constant.h"
#include "disk_driver.h"

/*
 * Data structures used by this file
 */
struct PARTITION {
	int total_blocks;
	int block_size;
	char *partition_name;
} partition;

struct FAT {
	char *filename;
	int file_length;
	int block_ptrs[10];
	int current_location;
} fat[20];

char *block_buffer;
FILE *fp[5];

// Constants used to simplify calculations
const int SIZE_OF_FAT = sizeof(fat) / sizeof(struct FAT);
const int SIZE_OF_FP = sizeof(fp) / sizeof(FILE *);
const int SIZE_OF_BLOCK_PTRS = sizeof(fat[0].block_ptrs) / sizeof(int);

// Additional data strcutures
struct FAT_FP_MAP {
	int fat;
} fat_fp_map[5];

int find_empty_fp();
int find_empty_block(int file);
void skip_to_data(FILE *file);
void update_block_information(int fat_file);
void compute_length(int file);

/* ----------------------------------------------------------------------------
 * @brief Initializes all data structures and variables.
 * ----------------------------------------------------------------------------
 */
void initIO() {
	int i, j;
	partition.partition_name = NULL;
	partition.total_blocks = 0;
	partition.block_size = 0;
	for (i = 0; i < SIZE_OF_FAT; i++) {
		fat[i].filename = NULL;
		fat[i].file_length = 0;
		for (j = 0; j < SIZE_OF_BLOCK_PTRS; j++) {
			fat[i].block_ptrs[j] = 0;
		}
		fat[i].current_location = 0;
	}
	block_buffer = NULL;
	for (i = 0; i < 5; i++) {
		fp[i] = NULL;
	}
	for (i = 0; i < 5; i++) {
		fat_fp_map[i].fat = 0;
	}
}

/* ----------------------------------------------------------------------------
 * @brief Creates a new partition with specified number of blocks and block
 *        size.
 * @param input  - name         - Name of the partition
 *        input  - block_size   - Size of blocks
 *        input  - total_blocks - Total number of blocks
 * @return int - Status Code
 *                  0 - Failed to create partition
 *                 -1 - Successfully created partition
 * ----------------------------------------------------------------------------
 */
int partition_drive(char *name, int total_blocks, int block_size) {
	int i, j;
	FILE *file;
	char relative_path[MAX_CMD_LENGTH];
	DIR *dir = opendir(PARTITION_FOLDER_NAME);

	// Creates the directory if it doesn't exist
	if (dir) {
		closedir(dir);
	} else {
		system("mkdir PARTITION");
	}

	// Checks if inputs are valid
	if (!name ||
	    block_size <= 0 ||
	    total_blocks <= 0) {
		return 0;
	}

	// Create relative path
	strcpy(relative_path, PARTITION_FOLDER_NAME);
	strcat(relative_path, "/");
	strcat(relative_path, name);

	file = fopen(relative_path, "w");
	fprintf(file, "%010d\n", total_blocks);
	fprintf(file, "%010d\n", block_size);

	for (i = 0; i < SIZE_OF_FAT; i++) {
		if (fat[i].filename) {
			fprintf(file, "%s\n", "");
		} else {
			fprintf(file, "\n");
		}
		fprintf(file, "%010d\n", 0);
		for (j = 0; j < SIZE_OF_BLOCK_PTRS; j++) {
			fprintf(file, "%010d\n", -1);
		}
		fprintf(file, "%010d\n", -1);
	}

	for (i = 0; i < total_blocks * block_size; i++) {
		fprintf(file, "%d", 0);
	}
	fprintf(file, "%c", '\0');

	fclose(file);
	return 1;
}

/* ----------------------------------------------------------------------------
 * @brief Mounts a partition.
 * @param input  - name - Name of the partition
 * @return int - Status Code
 *                 -1 - Partition contains invalid data
 *                  0 - Failed to mount partition
 *                  1 - Successfully mounted partition
 * ----------------------------------------------------------------------------
 */
int mount(char *name) {
	FILE *file;
	char line[MAX_CMD_LENGTH], relative_path[MAX_CMD_LENGTH];
	int expected, lines_read, characters_read, lines_per_fat,
			total_blocks, block_size, count, i;
	DIR *dir = opendir(PARTITION_FOLDER_NAME);

	// Checks if the partition folder exists
	if (dir) {
		closedir(dir);
	} else {
		return 0;
	}

	// Checks that the input argument is not null
	if (!name) {
		return 0;
	}

	// Create relative path
	strcpy(relative_path, PARTITION_FOLDER_NAME);
	strcat(relative_path, "/");
	strcat(relative_path, name);

	// Checks that the file exists
	file = fopen(relative_path, "r");
	if (!file) {
		return 0;
	}

	lines_per_fat = 3 + SIZE_OF_BLOCK_PTRS;
	lines_read = 0;
	expected = 2 + lines_per_fat * SIZE_OF_FAT;

	// Validates the data before writing it to the FAT table
	while (lines_read < expected) {
		if (fgets(line, MAX_CMD_LENGTH, file)) {
			lines_read++;
			line[strlen(line) - 1] = '\0';
			// Check the partition data
			if ((lines_read < 2 &&
					 !is_number(line)) ||
					(lines_read >= 2 &&
					 ((lines_read - 3) % lines_per_fat) != 0 &&
					 !is_number(line))) {
				printf("%d", ((lines_read - 2) % lines_per_fat));
				printf(GENERIC_ERROR_MSG "%s in partition is invalid",
							 line);
				return -1;
			}
		} else {
			// Ran out of lines unexpectedly
			printf(GENERIC_ERROR_MSG "partition is missing structure data");
			return -1;
		}
	}

	// Validates that the data section is valid
	fgets(line, MAX_CMD_LENGTH, file);
	characters_read = strnlen(line, MAX_CMD_LENGTH);

	fseek(file, 0, SEEK_SET);
	lines_read = 0;
	expected = 2;
	
	// Read 2 lines to obtain total number of data bytes
	fgets(line, MAX_CMD_LENGTH, file);
	total_blocks = atoi(line);
	fgets(line, MAX_CMD_LENGTH, file);
	block_size = atoi(line);

	if (characters_read != (total_blocks * block_size)) {
		fclose(file);
		printf(GENERIC_ERROR_MSG "partition is missing data blocks");
		return -1;
	}

	// All checks have been completed, proceed with mount
	lines_per_fat = 3 + SIZE_OF_BLOCK_PTRS;
	lines_read = 0;
	count = -1;
	expected = 2 + lines_per_fat * SIZE_OF_FAT;

	// Reset head to the start
	fseek(file, 0, SEEK_SET);
	if (partition.partition_name) {
		free(partition.partition_name);
	}
	partition.partition_name = malloc(MAX_CMD_LENGTH);
	strcpy(partition.partition_name, relative_path);

	while (lines_read <= expected) {
		if (fgets(line, MAX_CMD_LENGTH, file)) {
			line[strlen(line) - 1] = '\0';
			if (lines_read == 0) {
				partition.total_blocks = atoi(line);
			} else if (lines_read == 1) {
				partition.block_size = atoi(line);
			} else {
				if (((lines_read - 2) % lines_per_fat) == 0) {
					// Copy filename
					count++;
					if (fat[count].filename) {
						free(fat[count].filename);
					}
					fat[count].filename = malloc(MAX_CMD_LENGTH);
					strcpy(fat[count].filename, line);
				} else if (((lines_read - 2) % lines_per_fat) == 1) {
					fat[count].file_length = atoi(line);
				} else if (((lines_read - 2) % lines_per_fat) >= 2 &&
									 ((lines_read - 2) % lines_per_fat) <= 11) {
					// Copy block pointers
					fat[count].block_ptrs[((lines_read - 2) % lines_per_fat) - 2] = atoi(line);
				} else {
					// Copy current location
					fat[count].current_location = atoi(line);
				}
			}
			lines_read++;
		}
	}

	// Free-up previous allocation of block_buffer and malloc a new entry
	if (block_buffer) {
		free(block_buffer);
	}
	block_buffer = malloc(partition.block_size);

	// fclose previous fp and set all to null and clean up the fat-fp map
	for (i = 0; i < (SIZE_OF_FP); i++) {
		if (fp[i]) {
			fclose(fp[i]);
		}
		fp[i] = NULL;
		fat_fp_map[i].fat = -1;
	}

	fclose(file);
	return 1;
}

/* ----------------------------------------------------------------------------
 * @brief Opens the file in the partition.
 * @param input  - name - The name of the file to open.
 * @return int - The index of the FAT where the file was found or created. -1
 *               if the FAT or fp is full.
 * ----------------------------------------------------------------------------
 */
int open_file(char *name) {
	int i, j, empty;
	
	// Checks that the input is not null
	if (!name) {
		return -1;
	}

	// Find if the file exists in the partition
	for (i = 0; i < SIZE_OF_FAT; i++) {
		// Check that the filename is not null
		if (!fat[i].filename) {
			continue;
		}

		if (strcmp(name, fat[i].filename) == 0) {
			// Checks if the file is already open
			for (j = 0; j < SIZE_OF_FP; j++) {
				if (fat_fp_map[j].fat == i) {
					skip_to_data(fp[j]);
					fat[i].current_location = 0;
					fseek(fp[empty],
								fat[i].block_ptrs[fat[i].current_location] * partition.block_size,
								SEEK_CUR);
					return i;
				}
			}

			// The file is not open, open the partition and fseek the first block
			empty = find_empty_fp();

			// Could not find an empty spot in fp
			if (empty == -1) {
				return -1;
			}

			fp[empty] = fopen(partition.partition_name, "r+");
			fat_fp_map[empty].fat = i;
			skip_to_data(fp[empty]);
			fat[i].current_location = 0;
			fseek(fp[empty],
						fat[i].block_ptrs[fat[i].current_location] * partition.block_size,
						SEEK_CUR);
			return i;
		}
	}

	// Did not find file. Create a new entry in the FAT
	for (i = 0; i < SIZE_OF_FAT; i++) {
		if (strcmp(fat[i].filename, "") == 0) {
			free(fat[i].filename);
			// Find an empty FAT entry and return that index
			fat[i].filename = malloc(MAX_CMD_LENGTH);
			strcpy(fat[i].filename, name);
			fat[i].file_length = 0;
			for (j = 0; j < SIZE_OF_BLOCK_PTRS; j++) {
				fat[i].block_ptrs[j] = -1;
			}
			fat[i].current_location = -1;
			return i;
		}
	}

	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Sets the pointer to the start of data section.
 * ----------------------------------------------------------------------------
 */
void skip_to_data(FILE *file) {
	int i, lines_read, expected;
	char line[MAX_CMD_LENGTH];

	lines_read = 0;
	expected = 2 + (3 + SIZE_OF_BLOCK_PTRS) * SIZE_OF_FAT;

	// Reset pointer to start of file
	fseek(file, 0, SEEK_SET);
	while (lines_read < expected) {
		if (fgets(line, MAX_CMD_LENGTH, file)) {
			lines_read++;
		} else {
			break;
		}
	}
}

/* ----------------------------------------------------------------------------
 * @brief Finds an empty spot in the filepointers
 * @return int - Index of an empty file pointer if an empty spot was found. -1
 *               otherwise.
 * ----------------------------------------------------------------------------
 */
int find_empty_fp() {
	int i;
	for (i = 0; i < (SIZE_OF_FP); i++) {
		if (!fp[i]) {
			return i;
		}
	}
	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Reads a block from the file.
 * @param input  - file - The index in the FAT
 * @return  int - Status Code
 *                   0 - Read is successful
 *                  -1 - Read failed
 * ----------------------------------------------------------------------------
 */
int read_block(int file) {
	FILE *f;
	int i;

	// New file, nothing to read
	if (fat[file].current_location == -1) {
		return 0;
	}

	// No more block
	if (fat[file].block_ptrs[fat[file].current_location] == -1) {
		return 0;
	}

	f = NULL;
	// Find the FP associated to the FAT
	for (i = 0; i < SIZE_OF_FP; i++) {
		if (fat_fp_map[i].fat == file) {
			f = fp[i];
		}
	}

	// The file does not exist
	if (f == NULL) {
		return 0;
	}

	if (block_buffer) {
		free(block_buffer);
	}
	block_buffer = malloc(partition.block_size);
	
	// Load the block into block_buffer
	skip_to_data(f);
	fseek(f,
				fat[file].block_ptrs[fat[file].current_location] * partition.block_size,
				SEEK_CUR);
	for (i = 0; i < partition.block_size; i++) {
		block_buffer[i] = fgetc(f);
	}

	fat[file].current_location++;
	return 1;
}

/* ----------------------------------------------------------------------------
 * @brief Returns the block buffer.
 * @return char *- The block buffer.
 * ---------------------------------------------------------------------------- 
 */
char *return_block() {
	return block_buffer;
}

/* ----------------------------------------------------------------------------
 * @brief Destructively writes to the block.
 * @param input  - file - The index in the FAT
 * @return int - Status Code
 *                  0 - Write failed
 *                 -1 - Write was successful
 * ----------------------------------------------------------------------------
 */
int write_block(int file, char *data) {
	int i, empty_block, empty_fp;
	FILE *f;
	char line[MAX_CMD_LENGTH];

	if (fat[file].current_location == -1) {
		fat[file].current_location = 0;
	}

	// Find an empty block
	empty_block = find_empty_block(file);
	if (empty_block == -1) {
		// Could not find an empty block to write
		return 0;
	}

	// Find the file pointer associated with the file
	f = NULL;
	for (i = 0; i < SIZE_OF_FP; i++) {
		if (fat_fp_map[i].fat == file) {
			f = fp[i];
		}
	}

	// Not found, open the file
	if (f == NULL) {
		empty_fp = find_empty_fp();
		// No more fp available
		if (empty_fp == -1) {
			return 0;
		} else {
			fp[empty_fp] = fopen(partition.partition_name, "r+");
			skip_to_data(fp[empty_fp]);
			fseek(fp[empty_fp], empty_block * partition.block_size, SEEK_CUR);
			fat_fp_map[empty_fp].fat = file;
			f = fp[empty_fp];
		}
	} else {
		skip_to_data(f);
		fseek(f, empty_block * partition.block_size, SEEK_CUR);
	}

	// Destructively overwrite block
	for (i = 0; i < partition.block_size; i++) {
		if (i < strnlen(data, partition.block_size)) {
			fprintf(f, "%c", data[i]);
		} else {
			fprintf(f, "%c", '0');
		}
	}

	// Flush to OS
	fflush(f);

	fat[file].block_ptrs[fat[file].current_location] = empty_block;
	fat[file].current_location++;
	compute_length(file);

	// Update the information in the FAT
	update_block_information(file);

	return 1;
}

/* ----------------------------------------------------------------------------
 * @brief Writes the content of FAT to disk.
 * ----------------------------------------------------------------------------
 */
void update_block_information(int fat_file) {
	int i, j;
	FILE *file;
	char buffer[partition.total_blocks * partition.block_size + 1];

	for (i = 0; i < SIZE_OF_FP; i++) {
		if (fat_fp_map[i].fat == fat_file) {
			file = fp[i];
		}
	}

	skip_to_data(file);
	// Copy data to buffer
	fgets(buffer, partition.total_blocks * partition.block_size + 1, file);

	// Truncate all content of the file. Rewrite
	fseek(file, 0, SEEK_SET);
	fprintf(file, "%010d\n", partition.total_blocks);
	fprintf(file, "%010d\n", partition.block_size);

	for (i = 0; i < SIZE_OF_FAT; i++) {
		if (fat[i].filename) {
			fprintf(file, "%s\n", fat[i].filename);
		} else {
			fprintf(file, "\n");
		}
		fprintf(file, "%010d\n", fat[i].file_length);
		for (j = 0; j < SIZE_OF_BLOCK_PTRS; j++) {
			fprintf(file, "%010d\n", fat[i].block_ptrs[j]);
		}
		fprintf(file, "%010d\n", -1);
	}

	for (i = 0; i < sizeof(buffer); i++) {
		fprintf(file, "%c", buffer[i]);
	}

	fflush(file);
}

/* ----------------------------------------------------------------------------
 * @brief Computes the length of the file in the FAT.
 * @return int - The length of the file.
 * ----------------------------------------------------------------------------
 */
void compute_length(int file) {
	int i, j, size;
	FILE *f;
	char c, line[MAX_CMD_LENGTH];

	fat[file].file_length = 0;

	for (i = 0; i < SIZE_OF_BLOCK_PTRS; i++) {
		if (fat[file].block_ptrs[i] == -1) {
			break;
		} else {
			fat[file].file_length += partition.block_size;
		}
	}

	// Obtain the file pointer
	for (j = 0; j < SIZE_OF_FP; j++) {
		if (fat_fp_map[j].fat == file) {
			f = fp[j];
		}
	}

	// Remove 0s from file length
	skip_to_data(f);
	fseek(f, (fat[file].block_ptrs[i - 1] + 1) * partition.block_size, SEEK_CUR);
	size = 0;
	fseek(f, -1, SEEK_CUR);
	for (i = partition.block_size - 1; i >= 0; i--) {
		c = fgetc(f);
		if (c == '0') {
			size++;
		} else {
			break;
		}
		fseek(f, -2, SEEK_CUR);
	}
	fat[file].file_length -= size;
}

/* ----------------------------------------------------------------------------
 * @brief Returns the position of an empty block
 * @return int - The index of an empty block. -1 if there all blocks are filled
 * ----------------------------------------------------------------------------
 */
int find_empty_block(int file) {
	int i, j;
	int available[partition.total_blocks];

	for (i = 0; i < partition.total_blocks; i++) {
		available[i] = 1;
	}

	for (i = 0; i < SIZE_OF_FAT; i++) {
		for (j = 0; j < SIZE_OF_BLOCK_PTRS; j++) {
			if (i == file && j >= fat[i].current_location) {
				fat[i].block_ptrs[j] = -1;
				continue;
			}
			// Mark blocks are unusable
			if (fat[i].block_ptrs[j] != -1) {
				available[fat[i].block_ptrs[j]] = 0;
			}
		}
	}

	for (i = 0; i < partition.total_blocks; i++) {
		if (available[i] == 1) {
			return i;
		}
	}
	return -1;
}

/* ----------------------------------------------------------------------------
 * @brief Returns the size of a block.
 * ----------------------------------------------------------------------------
 */
int get_block_size() {
	return partition.block_size;
}

/* ----------------------------------------------------------------------------
 * @brief Prints the content of the data structures. For debugging purposes
 *        only. Not to be called otherwise.
 * ----------------------------------------------------------------------------
 */
void debug_disk_driver() {
	int i, j;

	printf("PARTITION NAME: %s  TOTAL BLOCKS: %d  BLOCK_SIZE: %d\n",
									partition.partition_name,
									partition.total_blocks,
									partition.block_size);
	for (i = 0; i < SIZE_OF_FAT; i++) {
		printf("FILENAME: %s  FILE LENGTH: %d\n",
										fat[i].filename,
										fat[i].file_length);
		for (j = 0; j < SIZE_OF_BLOCK_PTRS; j++) {
			printf("BLOCK PTRS %d: %d  ", j, fat[i].block_ptrs[j]);
		}
		printf("\nCURRENT_LOCATION: %d\n", fat[i].current_location);
	}

	if (block_buffer) {
		printf("BLOCK BUFFER: %s\n", block_buffer);
	} else {
		printf("BLOCK BUFFER: \n");
	}

	for (i = 0; i < SIZE_OF_FP; i++) {
		printf("FP %d: %p ", i, fp[i]);
	}
	printf("\n");
	for (i = 0; i < SIZE_OF_FP; i++) {
		printf("FAT_FP_MAP %d FAT: %d\n",
										i,
										fat_fp_map[i].fat);
	}
}
