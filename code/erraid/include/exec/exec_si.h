#ifndef EXEC_SI_H
# define EXEC_SI_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils_exec.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/binary_utils.h" // IWYU pragma: keep


bool	exec_si(struct s_cmd *cmd, int fd_in, int fd_out,
		  struct s_cmd_pl *parent_pl);
bool	exec_si_and_wait(struct s_cmd *cmd, int fd_in, int fd_out);


#endif
