#ifndef UTILS_H
# define UTILS_H

# include <stdio.h>
# include <stdbool.h>
# include <fcntl.h>
# include <unistd.h>
# include <stddef.h>
# include <inttypes.h>
# include <sys/endian.h>


void	print_help(void);
bool	is_little_endian(void);

#endif
