#include "utils/buffer_utils.h"

/**
 * @brief Reads an exact number of bytes from a file descriptor.
 *
 * @param fd	The file descriptor to read from.
 * @param buf	The buffer to fill with the read data.
 * @param len	The exact number of bytes expected to be read.
 * @return true on successful read of 'len' bytes, false on failure or unexpected EOF.
 */
bool	read_exact(int fd, void *buf, size_t len)
{
	size_t	bytes_read = 0;
	ssize_t	ret;
	char	*p = (char *)buf;

	while (bytes_read < len) {
		ret = read(fd, p + bytes_read, len - bytes_read);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			ERR_SYS("read_exact");
			return false;
		}
		if (ret == 0) {
			// Unexpected EOF
			return false;
		}
		bytes_read += (size_t)ret;
	}
	return true;
}

/**
 * @brief Reads a uint16_t from a file descriptor and converts it from BE to Host-Endian.
 *
 * @param fd 	The file descriptor to read from.
 * @param val 	Pointer to a uint16_t where the host-endian value will be stored.
 * @return true on success, false on read failure.
 */
bool	read_uint16(int fd, uint16_t *val)
{
	uint16_t	be_val;
	
	if (!read_exact(fd, &be_val, sizeof(uint16_t)))
		return false;
	*val = be16toh(be_val);
	return true;
}

/**
 * @brief Reads a uint32_t from a file descriptor and converts it from BE to Host-Endian.
 *
 * @param fd	The file descriptor to read from.
 * @param val	Pointer to a uint32_t where the host-endian value will be stored.
 * @return true on success, false on read failure.
 */
bool	read_uint32(int fd, uint32_t *val)
{
	uint32_t	be_val;
	
	if (!read_exact(fd, &be_val, sizeof(uint32_t)))
		return false;
	*val = be32toh(be_val);
	return true;
}

/**
 * @brief Reads a uint64_t from a file descriptor and converts it from BE to Host-Endian.
 *
 * @param fd	The file descriptor to read from.
 * @param val	Pointer to a uint64_t where the host-endian value will be stored.
 * @return true on success, false on read failure.
 */
bool	read_uint64(int fd, uint64_t *val)
{
	uint64_t	be_val;
	
	if (!read_exact(fd, &be_val, sizeof(uint64_t)))
		return false;
	*val = be64toh(be_val);
	return true;
}