#ifndef UTILS_H
# define UTILS_H

# include <stdio.h>
# include <unistd.h>
# include <stdint.h>
# include <errno.h> // IWYU pragma: keep
# include <string.h>
# include <stdlib.h>
# include <limits.h>

// basically using the compiler to check for apple vs linux 
# if defined(__has_include) && __has_include(<endian.h>)
#  include <endian.h>
# elif defined(__has_include) && __has_include(<sys/endian.h>)
#  include <sys/endian.h>
# endif

# include "structs.h" // IWYU pragma: keep

# define STRINGIFY(x) #x
# define TOSTRING(x) STRINGIFY(x)
# define DBG " [" __FILE__ " @ " TOSTRING(__LINE__) "]\n"
# define ERR_SYS(func) _write_perr(func, DBG)
# define ERR_MSG(msg) _write_err(msg, DBG)

/**
 * @brief calls read(2) and converts (if needed) data to host byte order
 *
 * @param fd file descriptor to read from
 * @param buf pointer to data buffer
 * @param nbytes number of bytes to read
 * @param is_data_le true if data is in little endian, false if in big endian
 *
 * @return the return of the read(2) call
 * */
ssize_t		read_endian(int fd, void *buf, size_t nbytes, bool is_data_le);

/** @brief prints to stdout the help message. */
void		print_help(void);

/** @brief prints to stdout the strings of cmd cleanly. */
void		print_darr(const char *tab_name, char **tab);

/** @brief takes NULL-terminatd **ptr,
 * then frees all elements of darr, then frees dar itself. */
void		free_darr(char **darr);

/** @brief prints the string representation of a cmd_type enum */
void		print_cmd_enum(enum cmd_type type, bool newline);

/** @brief appends the string representation of an int to a buffer */
void		append_int_to_buf(char *buf, int n);

/** @brief remove the last slash of a path if present */
void		remove_trailing_slash(char *path);

/** @brief builds a path in dest by concatening part1 and part2 */
bool		build_safe_path(char *dest, size_t dest_size, const char *part1, const char *part2);

/** @brief convert a relative path to an absolute path */
bool		convert_to_absolute_path(const char *relative_path, char *absolute_path);

void		_write_perr(const char *func, const char *location);
void		_write_err(const char *msg, const char *location);

#endif
