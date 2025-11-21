#ifndef DAEMON_LOOP_H
# define DAEMON_LOOP_H

# include <time.h>
# include <fcntl.h>
# include <unistd.h>

# include "structs.h"
# include "utils/utils.h" // IWYU pragma: keep
# include "exec/exec.h" // IWYU pragma: keep
# include "daemon/daemon.h" // IWYU pragma: keep
# include "commands/cmd_utils.h" // IWYU pragma: keep
# include "commands/dir_cmd_utils.h" // IWYU pragma: keep


void    daemon_loop(struct s_data *ctx);

#endif
