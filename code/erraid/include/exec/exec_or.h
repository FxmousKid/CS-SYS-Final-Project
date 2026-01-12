#ifndef EXEC_OR_H
# define EXEC_OR_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils_exec.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep
# include "exec/exec.h"


bool	exec_or(struct s_cmd *cmd, int fd_in, int fd_out);
bool	exec_or_if_parent_pl(struct s_cmd *cmd, struct s_cmd_pl *parent_pl,
			     int fd_in, int fd_out);

#endif
