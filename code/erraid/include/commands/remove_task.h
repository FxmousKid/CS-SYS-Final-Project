#ifndef REMOVE_TASK
# define REMOVE_TASK

# include <stdbool.h>
# include <inttypes.h>

# include "structs.h"
# include "utils/utils.h"
# include "utils/buffer_utils.h"
# include "utils/dir_cmd_utils.h"
# include "utils/cmd_utils.h"
# include "communication/fifo_api.h"
# include "macros.h"


bool    remove_task(struct s_data *ctx, uint64_t taskid);
bool	del_task_node(struct s_data *ctx, uint64_t taskid);

#endif