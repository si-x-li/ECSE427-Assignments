/* ----------------------------------------------------------------------------
 * @file CONSTANT.H
 * @author Si Xun Li - 260674916
 * @version 3.0
 * @brief This file contains system-wide macros.
 * ----------------------------------------------------------------------------
 */

/*
 * Declare more system-wide constants
 */
#define MAX_CMD_LENGTH          1024
#define MAX_KEY_LENGTH          1024
#define MAX_VALUE_LENGTH        1024
#define GENERIC_EXPECTED_MSG    "Expected: "
#define GENERIC_ERROR_MSG       "Error: "

/*
 * Declare maximum length of line that can be read
 */
#define MAX_LINE_LENGTH         1024

/*
 * Declare maximum number of concurrent files that can be opened per exec
 */
#define MAX_FILE_NUMBER         3

/*
 * Declare maximum length of a filename
 */
#define MAX_FILENAME_LENGTH     1024

/*
 * Declare the size of RAM
 */
#define RAM_SIZE                10

/*
 * Declare the size of a page
 */
#define PAGE_SIZE               4

/*
 * Declare initial frame count
 */
#define INITIAL_FRAME_NUMBER    2

/*
 * Declare quanta size
 */
#define QUANTA                  2

/*
 * Declare the name of the partion folder
 */
#define PARTITION_FOLDER_NAME   "PARTITION"

/*
 * Define system-wide constants
 */
#define SHELL_NAME              "Si Xun Li"
#define SHELL_VERSION           "4.0"
#define UPDATE_DATE             "April 2019"
#define KERNEL_VERSION          "3.0"
#define TAB                     "    "
