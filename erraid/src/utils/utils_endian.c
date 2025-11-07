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
	unsigned int *buf_int = buf;
	while (nbytes > 0) {
		switch (nbytes) {
		case 2:
			if (is_data_le)
				*buf_int = le16toh(*buf_int);
			else
				*buf_int = be16toh(*buf_int);
			nbytes -= 2;
			break;
		case 3:
			if (is_data_le)
				*buf_int = le16toh(*buf_int);
			else
				*buf_int = be16toh(*buf_int);
			nbytes -= 3;
			break;
		case 4:
			if (is_data_le)
				*buf_int = le32toh(*buf_int);
			else
				*buf_int = be32toh(*buf_int);
			nbytes -= 4;
			break;
		case 5:
			if (is_data_le)
				*buf_int = le32toh(*buf_int);
			else
				*buf_int = be32toh(*buf_int);
			nbytes -= 5;
			break;
		case 6:
			if (is_data_le)
				*buf_int = le32toh(*buf_int);
			else
				*buf_int = be32toh(*buf_int);
			nbytes -= 6;
			break;
		case 7:
			if (is_data_le)
				*buf_int = le32toh(*buf_int);
			else
				*buf_int = be32toh(*buf_int);
			nbytes -= 7;
			break;
		default:
			if (is_data_le)
				*buf_int = le64toh(*buf_int);
			else
				*buf_int = be64toh(*buf_int);
			nbytes -= 8;
			break;
		}
	}
}

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
ssize_t	read_endian(int fd, void *buf, size_t nbytes, bool is_data_le)
{
	ssize_t	n = read(fd, buf, nbytes);
	if (nbytes == 1)
		return (n);
	if (is_little_endian() && is_data_le)
		return (n);
	if (!is_little_endian() && !is_little_endian())
		return (n);
	convert_from_all_endian(buf, nbytes, is_data_le);
	return (n);
}
