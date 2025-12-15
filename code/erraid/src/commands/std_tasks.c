#include "commands/std_tasks.h"

static char	serialize_std_tasks(struct s_data *ctx, struct s_buffer *buf, uint64_t taskid, char *std)
{
	char		path[PATH_MAX] = {0};
	char		read_buf[1] = {0};
	int		read_size;
	uint32_t	nbchar = 0;
	struct stat	st;
	int		file;

	read_size = snprintf(path, PATH_MAX, "%s/tasks/%" PRId64 "/", ctx->run_directory, taskid);
	//printf("path : %s\n", path);
	if (!(stat(path, &st) == 0 && S_ISDIR(st.st_mode)))
		return 'F';
	snprintf(path + read_size, 7, "%s", std);
	//printf("path : %s\n", path);
	if ((file = open(path, O_RDONLY)) < 0) {
		return 'R';
	}
	buffer_append_uint16(buf, 0x4f4b);
	buffer_append_uint32(buf, 0);
	read_size = 0;
	while ((read_size = read(file, read_buf, 1)) > 0) {
		//printf("buf : %s\n", read_buf);
		buffer_append(buf, read_buf, read_size);
		nbchar++;
	}
	if (read_size < 0)
		return 'E';

	nbchar = htobe32(nbchar);
	memcpy(buf->data + 2, &nbchar, 4);
	close(file);
	return 'S';
}

static void	handle_err_std(struct s_buffer *buf, char err)
{
	switch (err)
	{
	case 'F':
		buffer_append_uint16(buf, 0x4552);
		buffer_append_uint16(buf, 0x4e46);
		break;
	case 'R':
		buffer_append_uint16(buf, 0x4552);
		buffer_append_uint16(buf, 0x4e52);
		break;
	default:
		break;
	}
}


bool	std_tasks(struct s_data *ctx, uint64_t taskid, char *std)
{
	struct s_buffer	reply_buf = {0};
	char		success;

	// Init buffer
	if (!buffer_init(&reply_buf, INITIAL_BUF_CAPACITY))
		return false;
	if ((success = serialize_std_tasks(ctx, &reply_buf, taskid, std)) != 'S') {
		handle_err_std(&reply_buf, success);
	}
	if (!writefifo(ctx->fifo_reply, reply_buf.data, reply_buf.size))
		return false;
	buffer_free(&reply_buf);
	return true;
}
