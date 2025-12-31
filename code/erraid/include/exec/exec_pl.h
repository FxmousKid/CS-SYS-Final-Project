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


bool	create_all_pipes(struct s_cmd_pl *cmd_pl);
bool	close_all_pipes(struct s_cmd_pl *cmd_pl);
bool	close_pipes_except(struct s_cmd_pl *cmd_pl, int keep_fd1, int keep_fd2);
bool	exec_first_command(struct s_cmd_pl *cmd_pl, int fd_in);
bool	exec_middle_commands(struct s_cmd_pl *cmd_pl);
bool	exec_last_command(struct s_cmd_pl *cmd_pl, int fd_out);

#endif
