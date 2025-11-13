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
bool	execute_command(struct s_cmd *cmd);
bool	execute_command_with_redirection(struct s_cmd *cmd, const char *stdout_file, const char *stderr_file);
bool	setup_output_redirection(const char *stdout_file, const char *stderr_file);

#endif