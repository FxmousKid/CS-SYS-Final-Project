#ifndef LIST_TASKS_H
# define LIST_TASKS_H 

# include <stdbool.h>
# include <string.h> // IWYU pragma: keep
# include <unistd.h>
# include <fcntl.h>
# include <inttypes.h> // Pour PRIu64

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "communication/fifo_api.h" // IWYU pragma: keep
# include "utils/buffer_utils.h"
# include "utils/cmd_utils.h"

/** @brief Executes the client's LIST command (-l). */
bool	list_tasks(struct s_data *ctx);

#endif
