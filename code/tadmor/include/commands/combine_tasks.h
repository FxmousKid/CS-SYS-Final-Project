#ifndef COMBINE_TASKS_H
# define COMBINE_TASKS_H 

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

bool	combine_tasks(struct s_data *ctx, enum cmd_type type);
bool	sequence_tasks(struct s_data *ctx);
bool	pipeline_tasks(struct s_data *ctx);
bool	if_tasks(struct s_data *ctx);

#endif
