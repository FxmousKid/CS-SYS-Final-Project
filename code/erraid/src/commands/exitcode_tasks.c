#include "commands/exitcode_tasks.h"

static bool	serialize_exitcode_tasks(struct s_data *ctx, struct s_buffer *buf, uint64_t taskid)
{
	char		path[PATH_MAX] = {0};
	char		read_buf[10] = {0};
	int		read_size;
	uint32_t	nbruns = 0;
	uint16_t	exitcode = 0;
	uint64_t	time = 0;
	int		file;

	snprintf(path, PATH_MAX, "%s/tasks/%" PRId64 "/times-exitcodes", ctx->run_directory, taskid);
	if ((file = open(path, O_RDONLY)) < 0) {
		return false;
	}

	buffer_append_uint16(buf, 0x4f4b);
	buffer_append_uint32(buf, 0);

	while ((read_size = read(file, read_buf, 10)) > 0) {
		memcpy(&time, read_buf, 8);
		memcpy(&exitcode, read_buf + 8, 2);

		time = htobe64(time);
		exitcode = htobe16(exitcode);
		
		buffer_append_uint64(buf, time);
		buffer_append_uint16(buf, exitcode);
		nbruns++;
	}
	if (read_size < 0)
		return false;
	nbruns = htobe32(nbruns);
	memcpy(buf->data + 2, &nbruns, 4);
	close(file);
	return true;
}

static void	handle_err_exitcode(struct s_buffer *buf)
{
	buffer_append_uint16(buf, 0x4552);
	buffer_append_uint16(buf, 0x4e46);
}

bool	exitcode_tasks(struct s_data *ctx, uint64_t taskid)
{
	struct s_buffer	reply_buf = {0};

	// Init buffer
	if (!buffer_init(&reply_buf, INITIAL_BUF_CAPACITY))
		return false;
	if (!serialize_exitcode_tasks(ctx, &reply_buf, taskid)) {
		handle_err_exitcode(&reply_buf);
	}
	if (!writefifo(ctx->fifo_reply, reply_buf.data, reply_buf.size))
		return false;
	buffer_free(&reply_buf);
	return true;
}
