#ifndef COMBINE_UTILS_H
# define COMBINE_UTILS_H

# include <stdlib.h> // IWYU pragma: keep
# include <stdint.h>
# include <fcntl.h>
# include <sys/stat.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/cmd_utils.h" // IWYU pragma: keep
# include "commands/remove_task.h"

bool	move_all_cmd(struct s_data *ctx, struct s_task *task, uint8_t *req);

#endif