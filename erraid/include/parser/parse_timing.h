#ifndef PARSE_TIMING_H
# define PARSE_TIMING_H

# include <limits.h>
# include <stdbool.h>
# include <getopt.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/stat.h>

// basically using the compiler to check for apple vs linux 
# if defined(__has_include) && __has_include(<endian.h>)
#  include <endian.h>
# elif defined(__has_include) && __has_include(<sys/endian.h>)
#  include <sys/endian.h>
# endif

# include "structs.h"
# include "utils/utils_timing.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "macros.h" // IWYU pragma: keep

bool	parse_timing(struct s_task *task);
void	test_timing_exemple();

#endif 
