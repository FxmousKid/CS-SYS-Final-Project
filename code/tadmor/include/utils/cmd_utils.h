#ifndef CMD_UTILS_H
# define CMD_UTILS_H

# include <stdbool.h>
# include <string.h>
# include <stdio.h>

# include "utils.h"
# include "buffer_utils.h"
# include "communication/fifo_api.h" // IWYU pragma: keep
# include "macros.h"

/**
 * @brief Reads the COMMAND structure from the FIFO and reconstructs the command string.
 *
 * @param fd            The file descriptor to read from.
 * @param cmd_str       Pointer to char* where the resulting string will be stored.
 */
bool	cmd_to_str(int fd, char **cmd_str);
bool	request_opt_tasks(char *fifo_request, uint16_t opcode, uint64_t opt);
bool    assemble_sq_pl_string(int fd, uint16_t type, uint32_t nb_cmds, char **cmd_str, bool parentheses);
bool    assemble_if_string(int fd, char **cmd_str, bool parentheses);

#endif