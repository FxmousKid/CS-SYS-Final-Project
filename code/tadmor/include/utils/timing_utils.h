#ifndef TIMING_UTILS_H
# define TIMING_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

// basically using the compiler to check for apple vs linux 
# if defined(__has_include) && __has_include(<endian.h>)
#  include <endian.h>
# elif defined(__has_include) && __has_include(<sys/endian.h>)
#  include <sys/endian.h>
# endif

#include "utils/utils.h" // IWYU pragma: keep

minutes_t	parse_minutes(char *arg);
hours_t		parse_hours(char *arg);
dasyofweek_t	parse_days(char *arg);

#endif
