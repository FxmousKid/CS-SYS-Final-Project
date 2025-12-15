#ifndef BUFFER_UTILS_H
# define BUFFER_UTILS_H

# include <stdlib.h>
# include <stdint.h>
# include <stdbool.h>
# include <string.h>
# include <errno.h>

// basically using the compiler to check for apple vs linux 
# if defined(__has_include) && __has_include(<endian.h>)
#  include <endian.h>
# elif defined(__has_include) && __has_include(<sys/endian.h>)
#  include <sys/endian.h>
# endif

# include "macros.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep


/**
 * @brief Dynamic buffer structure used to build binary response messages.
 */
struct s_buffer {
	uint8_t	*data;
	size_t	size;    // Current size of used data
	size_t	capacity; // Total allocated capacity
};

/* Buffer Management */

/**
 * @brief Initializes the dynamic buffer structure. */
bool	buffer_init(struct s_buffer *buf, size_t initial_capacity);

/** @brief Frees the dynamically allocated memory inside the s_buffer structure. */
void	buffer_free(struct s_buffer *buf);

/**
 * @brief Appends raw bytes to the buffer, growing it if necessary. */
bool	buffer_append(struct s_buffer *buf, const void *src, size_t len);


/* Big-Endian Serialization Functions (Host To Big-Endian) */

/** @brief Converts a uint16_t to BE and appends it to the buffer. */
bool	buffer_append_uint16(struct s_buffer *buf, uint16_t val);

/** @brief Converts a uint32_t to BE and appends it to the buffer. */
bool	buffer_append_uint32(struct s_buffer *buf, uint32_t val);

/** @brief Converts a uint64_t to BE and appends it to the buffer. */
bool	buffer_append_uint64(struct s_buffer *buf, uint64_t val);

/** @brief Appends a protocol-compliant string: LENGTH <uint32> + DATA [bytes]. */
bool	buffer_append_string(struct s_buffer *buf, const char *str);

#endif
