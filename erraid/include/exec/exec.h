#ifndef EXEC_H
# define EXEC_H

# include <stdbool.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <unistd.h>

# include "utils/utils.h"
# include "structs.h"
# include "utils/binary_utils.h"


bool	exec_task(struct s_task *task);

#endif
