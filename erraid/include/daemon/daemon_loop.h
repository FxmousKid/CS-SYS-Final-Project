#ifndef DAEMON_LOOP_H
# define DAEMON_LOOP_H

# include "utils/utils.h"
# include "structs.h"
# include "exec/exec.h"
# include "daemon/daemon.h"
# include "time.h"
# include "commands/cmd_utils.h"
# include "commands/dir_cmd_utils.h"

void    daemon_loop(struct s_data *ctx);

#endif