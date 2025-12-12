#ifndef BUFFER_UTILS_H
# define BUFFER_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <byteswap.h>

#include "utils/utils.h"

/** @brief Reads an exact number of bytes from a file descriptor. */

bool	read_exact(int fd, void *buf, size_t len);

/* Functions for Big-Endian deserialization */

/** @brief Reads a uint16_t, converting it from BE to Host-Endian. */
bool	read_uint16(int fd, uint16_t *val);

/** @brief Reads a uint32_t, converting it from BE to Host-Endian. */
bool	read_uint32(int fd, uint32_t *val);

/** @brief Reads a uint64_t, converting it from BE to Host-Endian. */
bool	read_uint64(int fd, uint64_t *val);

#endif