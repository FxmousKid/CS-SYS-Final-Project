#include "utils/buffer_utils.h"

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
