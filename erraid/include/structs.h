#ifndef STRUCT_H
# define STRUCT_H

#include <limits.h>

struct s_data {
	short	exit_code;
	char	run_directory[PATH_MAX];
};

#endif
