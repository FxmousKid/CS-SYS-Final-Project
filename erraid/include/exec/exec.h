#ifndef EXEC_H
# define EXEC_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>
# include "utils/utils.h"
# include "structs.h"
# include "utils/binary_utils.h"

bool	execute_simple_command(struct s_cmd_si *cmd_si, uint8_t *exit_code, pid_t *pid);
bool	execute_command(struct s_cmd *cmd);
bool	exec_cmd_with_redir(struct s_cmd *cmd, const char *stdout_file, const char *stderr_file);
bool	setup_output_redir(const char *stdout_file, const char *stderr_file);

#endif
