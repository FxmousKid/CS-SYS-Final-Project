#ifndef CREATE_TASKS_H
# define CREATE_TASKS_H 

# include <stdbool.h>
# include <string.h> // IWYU pragma: keep
# include <unistd.h>
# include <fcntl.h>
# include <time.h>
# include <inttypes.h> // Pour PRIu64

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "communication/fifo_api.h" // IWYU pragma: keep
# include "utils/buffer_utils.h"
# include "utils/cmd_utils.h"

/** @brief Show the exitcodes of the command taskid (-x). */
bool	create_tasks(struct s_data *ctx);

#endif
