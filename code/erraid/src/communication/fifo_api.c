#include "communication/fifo_api.h"

/* This function is called as the daemon is called for the 1st time.
 * It will check if the FIFOs already exist, if they do then there
 * is nothing to be done, else it tries to create them.
 *
 * Note: at no point should one of the two only be created,
 * their existence is mutually exclusive, either both exist or they don't.
 * (at least in theory)...
 * */
bool	create_fifos_safe(struct s_data *ctx)
{
	if (mkfifo(ctx->fifo_reply, 0644) < 0 && errno != EEXIST) {
		ERR_SYS("failed to mkfifo(%s, 0644)", ctx->fifo_reply);
		return false;
	}
	if (mkfifo(ctx->fifo_request, 0644) < 0 && errno != EEXIST) {
		ERR_SYS("failed to mkfifo(%s, 0644)", ctx->fifo_request);
		return false;
	}
	return true;
}

/**
 * @brief tries to open a fd to the path passed in argument
 *
 * @param fifo_path path to the fifo
 * @param to_read true if open in read, false if to open in write
 * @return the fd on success, -1 on failure
 */
static int	openfifo(const char *fifo_path, bool to_read)
{
	int	fd = -1;
	int	mode = -1;

	mode = to_read ? O_RDONLY : O_WRONLY;
	if ((fd = open(fifo_path, mode)) < 0) {
		ERR_SYS("open: can't open %s", fifo_path)
		return -1;
	}

	return fd;
}


/**
 * @brief write the buf to the opened fd to path
 *
 * @param path the path to the fifo
 * @param buf the payload
 * @param len the length of the payload
 *
 * @return 
 *  @retval true on success
 *  @retval false on failure, logs the error
 */
bool	writefifo(const char *path, const void *buf, size_t len)
{
	int		fd;
	size_t		write_qty = 0;
	const char	*p = buf;


	if ((fd = openfifo(path, 0)) < 0)
		return false;

	while (write_qty < len) {
		ssize_t ret = write(fd, p + write_qty, len - write_qty);

		if (ret < 0) {
			if (errno == EINTR)
				continue; // for signal interruption
			ERR_SYS("write: couldn't write to fd [%d]", fd);
			close(fd);
			return false;
		}
		if (ret == 0) {
			ERR_MSG("write: wrote 0 bytes to fd [%d]", fd);
			close(fd);
			return false;
		}
		write_qty += (size_t)ret;
	}
	close(fd);
	return true;
}

bool	readfifo(const char *path, struct s_reply *reply)
{
	int	fd = -1;
	int	read_qty = 0;
	int	read_interval = 1024;
	uint8_t	*tmp = NULL;

	if ((fd = openfifo(path, 1)) < 0)
		return false;
	
	if (!(reply->buf = calloc(read_interval, sizeof(uint8_t)))) {
		ERR_SYS("Failed to calloc");
		goto exit;
	}

	reply->buf_size = 0;
	while ((read_qty = read(fd, reply->buf + reply->buf_size, read_interval)) > 0) {
		reply->buf_size += read_qty;

		if (read_qty == read_interval)
			tmp = realloc(reply->buf, reply->buf_size + read_interval);
		else
			break;

		if (!tmp) {
			ERR_SYS("realloc failed");
			goto exit;
		}
		reply->buf = tmp;
	}
	if (read_qty < 0) {
		ERR_SYS("failed to read(%d, buf, %d)", fd, read_interval)
		goto exit;
	}
	close(fd);
	return true;

exit:
	if (fd >= 0) close(fd);
	if (reply->buf) free(reply->buf);
	return false;
}
