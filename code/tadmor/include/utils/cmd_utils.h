#ifndef CMD_UTILS_H
# define CMD_UTILS_H

# include <stdbool.h>
# include <string.h>
# include <stdio.h>

# include "utils.h"
# include "buffer_utils.h"
# include "macros.h"

/**
 * @brief Reads the COMMAND structure from the FIFO and reconstructs the command string.
 *
 * @param fd            The file descriptor to read from.
 * @param cmd_str       Pointer to char* where the resulting string will be stored.
 */
bool read_cmd_reconstruct_str(int fd, char **cmd_str);

#endif