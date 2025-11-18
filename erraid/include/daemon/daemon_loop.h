#ifndef DAEMON_LOOP_H
# define DAEMON_LOOP_H

# include <time.h>
# include "structs.h"
# include "utils/utils.h"
# include "exec/exec.h"
# include "daemon/daemon.h"
# include "commands/cmd_utils.h"
# include "commands/dir_cmd_utils.h"
# include <fcntl.h>
# include <unistd.h>


void    daemon_loop(struct s_data *ctx);
int	check_date(struct s_task *task);

#endif
