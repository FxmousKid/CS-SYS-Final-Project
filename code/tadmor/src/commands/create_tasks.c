
#include "commands/create_tasks.h"

static bool	request_create_tasks(struct s_data *ctx)
{
	struct s_buffer	buf;

	buffer_init(&buf, INITIAL_BUF_CAPACITY);
	buffer_append_uint16(&buf, OPCODE_CR);
<<<<<<< HEAD
	// buffer_append_uint* already converts to big-endian
=======
	// buffer_append_uint already converts to big-endian
>>>>>>> eb8fe82 (cancelled the combine changes, but fixed the endianness for timing)
	buffer_append_uint64(&buf, ctx->cmd.timing.minutes);
	buffer_append_uint32(&buf, ctx->cmd.timing.hours);
	buffer_append(&buf, &ctx->cmd.timing.days, 1);
	buffer_append_argument(&buf, ctx->argv + ctx->current);
	if (!writefifo(ctx->fifo_request, buf.data, buf.size)) {
		return false;
	}
	buffer_free(&buf);
	return true;
}

static bool	handle_create_reply(struct s_data *ctx)
{
	char		buf[21];
	int		fd_reply;
	int		len;
	uint16_t	ans = 0;
	uint64_t	id;

	if ((fd_reply = open(ctx->fifo_reply, O_RDONLY)) < 0) {
		ERR_SYS("open fifo_reply: %s", ctx->fifo_reply);
		return false;
	}

	if (!read_uint16(fd_reply, &ans))
		return false;

	if (ans == OPCODE_ER)
		return false;
	read_uint64(fd_reply, &id);
	len = snprintf(buf, 21, "%lu\n", id);
	write(STDOUT_FILENO, buf, len);
	return true;
}

bool	create_tasks(struct s_data *ctx)
{
	if (!request_create_tasks(ctx))
		return false;
	if (!handle_create_reply(ctx))
		return false;
	return true;
}
