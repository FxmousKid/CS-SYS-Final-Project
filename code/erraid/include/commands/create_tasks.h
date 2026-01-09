#ifndef CREATE_TASKS_H
# define CREATE_TASKS_H

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
# include "macros.h" // IWYU pragma: keep
# include "parser/parse_tasks.h"
# include "parser/parse_cmd_si.h"


bool	create_tasks(struct s_data *ctx, struct s_request *req);

#endif
