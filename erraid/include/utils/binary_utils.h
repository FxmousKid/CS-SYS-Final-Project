#ifndef BINARY_UTILS_H
# define BINARY_UTILS_H

# include <string.h>
# include <stdbool.h>
# include <limits.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

bool find_binary_path(const char *command, char *full_path);

#endif
