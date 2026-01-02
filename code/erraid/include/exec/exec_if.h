#ifndef EXEC_IF_H
# define EXEC_IF_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils_exec.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep


bool	exec_if(struct s_cmd *cmd, struct s_cmd *parent_pl,
	        int fd_in, int fd_out);
bool	exec_if_if_parent_pl(struct s_cmd *cmd, struct s_cmd *parent_pl,
			     int fd_in, int fd_out);

# include "exec/exec.h" // IWYU pragma: keep

#endif
