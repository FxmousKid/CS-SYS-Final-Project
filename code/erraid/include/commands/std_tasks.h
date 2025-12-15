#ifndef STD_TASKS_H
# define STD_TASKS_H

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

bool	std_tasks(struct s_data *ctx, uint64_t taskid, char *std);

#endif
