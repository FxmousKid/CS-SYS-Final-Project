#ifndef UTILS_H
# define UTILS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

// basically using the compiler to check for apple vs linux 
# if defined(__has_include) && __has_include(<endian.h>)
#  include <endian.h>
# elif defined(__has_include) && __has_include(<sys/endian.h>)
#  include <sys/endian.h>
# endif
# include <errno.h>
# include <string.h>

# include "structs.h" // IWYU pragma: keep

# define STRINGIFY(x) #x
# define TOSTRING(x) STRINGIFY(x)
# define DBG " [" __FILE__ " @ " TOSTRING(__LINE__) "]\n"
# define SYS_ERR(func) _write_perr(func, DBG)
# define MSG_ERR(msg) _write_err(msg, DBG)

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
void		print_darr(char **tab);

/** @brief takes NULL-terminatd **ptr,
 * then frees all elements of darr, then frees dar itself. */
void		free_darr(char **darr);

void		_write_perr(const char *func, const char *location);
void		_write_err(const char *msg, const char *location);

#endif
