#ifndef FIFO_API_H
# define FIFO_API_H

# include <stdbool.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>

# include "utils/utils.h" // IWYU pragma: keep
# include "structs.h"

bool	writefifo(const char *path, const void *buf, size_t len);
bool	readfifo(const char *path, struct s_reply *reply);

#endif
