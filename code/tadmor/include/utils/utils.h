#ifndef UTILS_H
# define UTILS_H

# include <stdio.h>
# include <unistd.h>
# include <stdint.h>
# include <errno.h> // IWYU pragma: keep
# include <string.h>
# include <stdlib.h>
# include <limits.h>
# include <fcntl.h>
# include <stdbool.h>
# include <stdarg.h>

# include "macros.h" // IWYU pragma: keep


// basically using the compiler to check for apple vs linux 
# if defined(__has_include) && __has_include(<endian.h>)
#  include <endian.h>
# elif defined(__has_include) && __has_include(<sys/endian.h>)
#  include <sys/endian.h>
# endif

# include "structs.h" // IWYU pragma: keep

# define STRINGIFY(x) #x
# define TOSTRING(x) STRINGIFY(x)
# define DBG __FILE__ " @ " TOSTRING(__LINE__)
// # define ERR_SYS(func) _write_perr(func, DBG) // <-- deprecated
// # define ERR_MSG(msg) _write_err(msg, DBG) // <-- deprecated
# define ERR_SYS(...) do { dprintf(get_logfd(), __VA_ARGS__); _write_perr(DBG); } while (0);
# define ERR_MSG(...) do { dprintf(get_logfd(), __VA_ARGS__); _write_err(DBG); } while (0);

/** @brief prints to stdout the help message. */
void		print_help(void);

/** @brief prints to stdout the strings of cmd cleanly. */
void		print_darr(const char *tab_name, char **tab);

/** @brief takes NULL-terminatd **ptr,
 * then frees all elements of darr, then frees dar itself. */
void		free_darr(char **darr);

/** @brief appends the string representation of an int to a buffer */
void		append_int_to_buf(char *buf, int n);

/** @brief remove the last slash of a path if present */
void		remove_trailing_slash(char *path);

/** @brief builds a path in dest by concatening part1 and part2 */
bool		build_safe_path(char *dest, size_t dest_size, const char *part1, const char *part2);

/** @brief convert a relative path to an absolute path */
bool		convert_to_absolute_path(const char *relative_path, char *absolute_path);

int		get_logfd(void);
void		_write_perr(const char *location);
void		_write_err(const char *location);

#endif
