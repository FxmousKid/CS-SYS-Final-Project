#ifndef UTILS_EXEC_H
# define UTILS_EXEC_H

# include <stdio.h>
# include <unistd.h>

# include "utils.h" // IWYU pragma: keep
# include "macros.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep


void	print_pipe_array(int fds[][2], int nb_pipes);
bool	setup_pipe(int fds[2]);
bool	setup_input_fd(int fd);
bool	setup_output_fd(int fd);
bool	wait_for_pipeline(struct s_cmd_pl *cmd_pl);
bool	setup_output_last_cmd(const char *stdout_file, const char *stderr_file);
bool	spawn_cmd(struct s_cmd *cmd, int fd_in, int fd_out);
bool	run_cmd(struct s_cmd *cmd, int fd_in, int fd_out);

#endif
