#ifndef EXEC_H
# define EXEC_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>
# include "utils/utils.h"
# include "structs.h"
# include "utils/binary_utils.h"

bool	exec_cmd_with_redir(struct s_cmd *cmd, const char *stdout_file, const char *stderr_file);

#endif
