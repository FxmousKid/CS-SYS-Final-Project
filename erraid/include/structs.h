#ifndef STRUCT_H
# define STRUCT_H

#include <limits.h>
#include <stdbool.h>

struct s_data {
	short	exit_code;
	char	run_directory[PATH_MAX];
	bool	is_data_le;
};

#endif
