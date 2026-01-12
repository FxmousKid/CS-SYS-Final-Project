#ifndef TASK_CREATE_UTILS_H
# define TASK_CREATE_UTILS_H

# include <stdbool.h>
# include <inttypes.h>

// basically using the compiler to check for apple vs linux 
# if defined(__has_include) && __has_include(<endian.h>)
#  include <endian.h>
# elif defined(__has_include) && __has_include(<sys/endian.h>)
#  include <sys/endian.h>
# endif

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "communication/fifo_api.h" // IWYU pragma: keep
# include "utils/buffer_utils.h" // IWYU pragma: keep
# include "utils/cmd_utils.h" // IWYU pragma: keep
# include "utils/combine_utils.h"
# include "macros.h" // IWYU pragma: keep
# include "parser/parse_tasks.h"
# include "parser/parse_cmd_si.h"


void		add_tasks(struct s_task *tasks, struct s_task *new);
bool		create_timing(struct s_task *task, uint8_t *req);
taskid_t	get_taskid(struct s_data *ctx);
void		create_directory(struct s_task *task);

#endif