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

# include "structs.h"

bool	parse_timing(struct s_task *task);

#endif 
