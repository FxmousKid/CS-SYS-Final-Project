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

/**
 * @brief Initializes the dynamic buffer structure.
 *
 * @param buf 			Pointer to the s_buffer structure.
 * @param initial_capacity	The starting capacity for the buffer.
 * @return true on success, false on memory allocation failure.
 */
bool	buffer_init(struct s_buffer *buf, size_t initial_capacity)
{
	if (initial_capacity == 0)
		initial_capacity = INITIAL_BUF_CAPACITY;
	
	buf->data = malloc(initial_capacity);
	if (!buf->data) {
		ERR_SYS("malloc");
		return false;
	}
	buf->size = 0;
	buf->capacity = initial_capacity;
	return true;
}
/**
 * @brief Frees the dynamically allocated memory inside the s_buffer structure.
 *
 * @param buf Pointer to the s_buffer structure.
 */
void	buffer_free(struct s_buffer *buf)
{
	if (buf && buf->data)
		free(buf->data);
	buf->data = NULL;
	buf->size = 0;
	buf->capacity = 0;
}

/**
 * @brief Appends raw bytes to the buffer, growing it if necessary.
 *
 * @param buf	Pointer to the s_buffer structure.
 * @param src	Source data to append.
 * @param len	Length of the source data in bytes.
 * @return true on success, false on memory reallocation failure.
 */
bool	buffer_append(struct s_buffer *buf, const void *src, size_t len)
{
	if (buf->size + len > buf->capacity) {
		size_t	new_capacity = buf->capacity;
		while (buf->size + len > new_capacity)
			new_capacity *= 2;
		
		uint8_t *tmp = realloc(buf->data, new_capacity);
		if (!tmp) {
			ERR_SYS("realloc");
			return false;
		}
		buf->data = tmp;
		buf->capacity = new_capacity;
	}
	
	memcpy(buf->data + buf->size, src, len);
	buf->size += len;
	return true;
}

/* BE Serialization Functions (Host To BE) */

/**
 * @brief Converts a uint16_t to BE and appends it to the buffer.
 *
 * @param buf 	Pointer to the s_buffer structure.
 * @param val 	The host-endian value to append.
 * @return true on success, false on failure.
 */
bool	buffer_append_uint16(struct s_buffer *buf, uint16_t val)
{
	uint16_t be_val = htobe16(val);
	return buffer_append(buf, &be_val, sizeof(uint16_t));
}

/**
 * @brief Converts a uint32_t to BE and appends it to the buffer.
 *
 * @param buf 	Pointer to the s_buffer structure.
 * @param val 	The host-endian value to append.
 * @return true on success, false on failure.
 */
bool	buffer_append_uint32(struct s_buffer *buf, uint32_t val)
{
	uint32_t be_val = htobe32(val);
	return buffer_append(buf, &be_val, sizeof(uint32_t));
}

/**
 * @brief Converts a uint64_t to BE and appends it to the buffer.
 *
 * @param buf 	Pointer to the s_buffer structure.
 * @param val 	The host-endian value to append.
 * @return true on success, false on failure.
 */
bool	buffer_append_uint64(struct s_buffer *buf, uint64_t val)
{
	uint64_t be_val = htobe64(val);
	return buffer_append(buf, &be_val, sizeof(uint64_t));
}

/**
 * @brief Appends a protocol-compliant string: LENGTH <uint32> + DATA [bytes].
 *
 * @param buf 	Pointer to the s_buffer structure.
 * @param str 	The string to append.
 * @return true on success, false on failure.
 */
bool	buffer_append_string(struct s_buffer *buf, const char *str)
{
	uint32_t len;

	if (!str)
		str = "";
	
	len = (uint32_t)strlen(str); 
	
	// Write the length (uint32_t)
	if (!buffer_append_uint32(buf, len))
		return false;

	// Write raw bytes of data
	if (!buffer_append(buf, str, len))
		return false;

	return true;
}

bool	buffer_append_argument(struct s_buffer *buf, char **arg)
{
	uint32_t len = 0;

	while (arg[len]) {
		len++;
	}
	if (len <= 0)
		return false;
	
	// Write the length (uint32_t)
	if (!buffer_append_uint32(buf, len))
		return false;

	for (uint32_t i = 0; i < len; i++) {
	// Write string in buffer
		if (!buffer_append_string(buf, arg[i]))
			return false;
	}
	return true;
}
