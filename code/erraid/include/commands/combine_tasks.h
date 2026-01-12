#ifndef COMBINE_TASKS_H
# define COMBINE_TASKS_H

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
# include "utils/task_create_utils.h"
# include "macros.h" // IWYU pragma: keep
# include "parser/parse_tasks.h"
# include "parser/parse_cmd_si.h"
# include "utils/dir_cmd_utils.h"

bool	combine_tasks(struct s_data *ctx, uint8_t *req);

#endif
