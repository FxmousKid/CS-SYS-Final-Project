#ifndef PARSE_CMD_PL_H
# define PARSE_CMD_PL_H

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


bool	alloc_and_fill_pl_sub_dirs(struct s_cmd_pl *cmd_pl, const char *path);
bool	alloc_pipe_fds(struct s_cmd_pl *cmd_pl);

#endif
