#ifndef EXEC_H
# define EXEC_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h"
# include "exec/exec_pl.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep

# define NEED_REDIR_BEFORE_FIRST_CMD(cmd) ((cmd)->cmd_type == CMD_SI || (cmd)->cmd_type == CMD_SQ)
# define NEED_REDIR_BEFORE_LAST_CMD(cmd)  ((cmd)->cmd_type == CMD_PL || (cmd)->cmd_type == CMD_IF)

bool	exec_task(struct s_task *task);

#endif
