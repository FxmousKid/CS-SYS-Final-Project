#ifndef EXEC_H
# define EXEC_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>
# include "utils/utils.h"
# include "structs.h"
# include "utils/binary_utils.h"

bool	execute_simple_command(struct s_cmd_si *cmd_si, short *exit_code, pid_t *pid);

#endif