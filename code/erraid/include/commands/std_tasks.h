#ifndef STD_TASKS_H
# define STD_TASKS_H

# include <stdbool.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include <endian.h>
# include "communication/fifo_api.h" // IWYU pragma: keep
# include "utils/buffer_utils.h"
# include "utils/cmd_utils.h"
# include "macros.h"

bool	std_tasks(struct s_data *ctx, uint64_t taskid, char *std);

#endif
