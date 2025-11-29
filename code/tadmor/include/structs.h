#ifndef STRUCTS_H
# define STRUCTS_H

# include <stdbool.h>
# include <stdint.h>

typedef uint64_t	taskid_t;
typedef uint64_t	minutes_t;
typedef uint32_t	hours_t;
typedef uint8_t		dasyofweek_t;

struct s_data {
	unsigned char	exit_code;
	bool		debug;
};

#endif
