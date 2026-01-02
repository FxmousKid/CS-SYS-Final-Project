#ifndef EXEC_SQ_H
# define EXEC_SQ_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils_exec.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep


bool	exec_sq(struct s_cmd *cmd, int fd_in, int fd_out);
bool	exec_sq_if_parent_pl(struct s_cmd *cmd, struct s_cmd_pl *parent_pl,
			     int fd_in, int fd_out);


// includes are here to avoid circular depedency
// but if we have to resort to that, maybe here
// is not the best place to put...
# include "exec/exec.h" // IWYU pragma: keep

#endif
