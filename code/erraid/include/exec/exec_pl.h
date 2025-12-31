#ifndef EXEC_PL_H
# define EXEC_PL_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils_exec.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep


bool	exec_pl(struct s_cmd *cmd, int fd_in, int fd_out);
bool	exec_pl_if_parent_pl(struct s_cmd *cmd,
			     struct s_cmd_pl *parent_pl,
			     int fd_in, int fd_out);

#endif
