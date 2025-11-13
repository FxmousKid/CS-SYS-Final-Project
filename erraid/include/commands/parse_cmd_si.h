#ifndef PARSE_CMD_SI_H
# define PARSE_CMD_SI_H

# include <stdbool.h>
# include <fcntl.h>
# include <unistd.h>
# include <stddef.h>
# include <fcntl.h>
# include <inttypes.h>

# include "utils/utils.h" // IWYU pragma: keep
# include "commands/cmd_utils.h" // IWYU pragma: keep
# include "structs.h"

/** @brief parses the argv file living in the path into the passed
 * struct cmd, at the right union */
bool	parse_cmd_si(const char path[PATH_MAX + 1], struct s_cmd *cmd);

#endif
