#ifndef LIST_TASKS_H
# define LIST_TASKS_H

# include <stdbool.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "communication/fifo_api.h" // IWYU pragma: keep

bool	list_tasks(struct s_data *ctx);

#endif
