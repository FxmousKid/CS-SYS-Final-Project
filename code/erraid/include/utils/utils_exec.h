#ifndef UTILS_EXEC_H
# define UTILS_EXEC_H

# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>

# include "utils.h" // IWYU pragma: keep
# include "macros.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep


bool	close_all_pipes(struct s_cmd_pl *cmd_pl);
bool	close_pipes_except(struct s_cmd_pl *cmd_pl, int keep_fd1, int keep_fd2);
void	print_pipe_array(int fds[][2], int nb_pipes);
bool	setup_pipe(int fds[2]);
bool	setup_input_fd(int fd);
bool	setup_output_fd(int fd);
bool	wait_for_pipeline(struct s_cmd_pl *cmd_pl);
bool	setup_output_last_cmd(const char *stdout_file, const char *stderr_file);

#endif
