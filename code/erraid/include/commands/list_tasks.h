#ifndef LIST_TASKS_H
# define LIST_TASKS_H

# include <stdbool.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "communication/fifo_api.h" // IWYU pragma: keep
# include "utils/buffer_utils.h"
# include "utils/cmd_utils.h"
# include "macros.h"

bool	list_tasks(struct s_data *ctx);

#endif
