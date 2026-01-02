#ifndef EXEC_H
# define EXEC_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h"
# include "exec/exec_si.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/utils_exec.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep

# define NEED_REDIR_BEFORE_FIRST_CMD(cmd) ((cmd)->cmd_type == CMD_SI || (cmd)->cmd_type == CMD_SQ)
# define NEED_REDIR_BEFORE_LAST_CMD(cmd)  ((cmd)->cmd_type == CMD_PL || (cmd)->cmd_type == CMD_IF)

bool	exec_cmd(struct s_cmd *cmd, int fd_in, int fd_out,
	         enum cmd_type parent_type, struct s_cmd_pl *parent_pl);
bool	exec_task(struct s_task *task);


// includes are here to avoid circular depedency
// but if we have to resort to that, maybe here
// is not the best place to put...
# include "exec/exec_if.h" // IWYU pragma: keep
# include "exec/exec_pl.h" // IWYU pragma: keep
# include "exec/exec_sq.h" // IWYU pragma: keep

#endif
