#ifndef STD_TASKS_H
# define STD_TASKS_H 

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

/** @brief Show the stdout of the command taskid (-o). */
bool	std_tasks(struct s_data *ctx, enum req_opcode std);
bool	stdout_tasks(struct s_data *ctx);
bool	stderr_tasks(struct s_data *ctx);

#endif
