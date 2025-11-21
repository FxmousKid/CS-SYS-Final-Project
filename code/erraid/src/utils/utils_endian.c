#include "utils/utils.h"

/**
 * @brief checks if host byte order is little endian
 *
 * @return
 *  @retval true if host byte order is little endian 
 *  @retval false if host byte order is big endian
 */
static bool	is_little_endian(void)
{
	unsigned short x = 1;
	char *c = (char *)&x;
	return ((int)*c == 1);
}

/**
 * @brief convert data from little or big endian to host byte order
 *
 * @param buf pointer to data buffer
 * @param nbytes number of bytes to convert
 * @param is_data_le true if data is in little endian, false if in big endian
 */
static void	convert_from_all_endian(void *buf, size_t nbytes, bool is_data_le)
{
	if (nbytes == sizeof(uint16_t)) {
		uint16_t value;
		memcpy(&value, buf, sizeof(value));
		value = is_data_le ? le16toh(value) : be16toh(value);
		memcpy(buf, &value, sizeof(value));
		return;
	}
	if (nbytes == sizeof(uint32_t)) {
		uint32_t value;
		memcpy(&value, buf, sizeof(value));
		value = is_data_le ? le32toh(value) : be32toh(value);
		memcpy(buf, &value, sizeof(value));
		return;
	}
	if (nbytes == sizeof(uint64_t)) {
		uint64_t value;
		memcpy(&value, buf, sizeof(value));
		value = is_data_le ? le64toh(value) : be64toh(value);
		memcpy(buf, &value, sizeof(value));
		return;
	}

	// if nbytes is not power of 2
	uint8_t	*bytes = buf;
	size_t		i = 0;
	while (i + sizeof(uint32_t) <= nbytes) {
		uint32_t value;
		memcpy(&value, bytes + i, sizeof(value));
		value = is_data_le ? le32toh(value) : be32toh(value);
		memcpy(bytes + i, &value, sizeof(value));
		i += sizeof(uint32_t);
	}
	if (nbytes - i >= sizeof(uint16_t)) {
		uint16_t value;
		memcpy(&value, bytes + i, sizeof(value));
		value = is_data_le ? le16toh(value) : be16toh(value);
		memcpy(bytes + i, &value, sizeof(value));
		i += sizeof(uint16_t);
	}
}

/**
 * @brief calls read(2) and converts (if needed) data to host byte order
 *
 * k
 * @param fd file descriptor to read from
 * @param buf pointer to data buffer
 * @param nbytes number of bytes to read
 * @param is_data_le true if data is in little endian, false if in big endian
 *
 * @return the return of the read(2) call
 * */
ssize_t	read_endian(int fd, void *buf, size_t nbytes, bool is_data_le)
{
	ssize_t	n = read(fd, buf, nbytes);
	if (nbytes == 1)
		return (n);
	if (is_little_endian() && is_data_le)
		return (n);
	if (!is_little_endian() && !is_data_le)
		return (n);
	convert_from_all_endian(buf, nbytes, is_data_le);
	return (n);
}
