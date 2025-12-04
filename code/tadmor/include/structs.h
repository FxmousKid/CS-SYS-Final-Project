#ifndef STRUCTS_H
# define STRUCTS_H

# include <stdbool.h>
# include <stdint.h>
# include <limits.h>

typedef uint64_t	taskid_t;
typedef uint64_t	minutes_t;
typedef uint32_t	hours_t;
typedef uint8_t		dasyofweek_t;

struct s_data {
	char		pipes_dir[PATH_MAX + 1];
	unsigned char	exit_code;
	bool		debug;
};

#endif
