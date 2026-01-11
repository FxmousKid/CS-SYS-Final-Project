#ifndef PARSE_CMD_ND_H
# define PARSE_CMD_ND_H

# include <stdbool.h>
# include <fcntl.h>
# include <unistd.h>
# include <stddef.h>
# include <fcntl.h>
# include <inttypes.h>

# include "utils/utils.h" // IWYU pragma: keep
# include "utils/cmd_utils.h" // IWYU pragma: keep
# include "utils/dir_cmd_utils.h" // IWYU pragma: keep
# include "structs.h"


bool	alloc_and_fill_nd_sub_dirs(struct s_cmd_nd *cmd_nd, const char *path);

#endif