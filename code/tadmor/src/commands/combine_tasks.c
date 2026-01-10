
#include "commands/combine_tasks.h"

static bool	request_combine_tasks(struct s_data *ctx, enum cmd_type type)
{
	struct s_buffer	buf;

	buffer_init(&buf, INITIAL_BUF_CAPACITY);
	buffer_append_uint16(&buf, OPCODE_CB);
	// buffer_append_uint already converts to big-endian
	buffer_append_uint64(&buf, ctx->cmd.timing.minutes);
	buffer_append_uint32(&buf, ctx->cmd.timing.hours);
	buffer_append(&buf, &ctx->cmd.timing.days, 1);
	buffer_append_uint16(&buf, type);

	if (get_arg_len(ctx->argv + ctx->current) < 2
	|| (type == CMD_IF && get_arg_len(ctx->argv + ctx->current) > 3)) {
		buffer_free(&buf);
		return false;
	}
	buffer_append_taskids(&buf, ctx->argv + ctx->current);
	if (!writefifo(ctx->fifo_request, buf.data, buf.size)) {
		return false;
	}
	buffer_free(&buf);
	return true;
}

static bool	handle_combine_reply(struct s_data *ctx)
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


bool	combine_tasks(struct s_data *ctx, enum cmd_type type)
{
	if (!request_combine_tasks(ctx, type))
		return false;
	if (!handle_combine_reply(ctx))
		return false;
	return true;
}

bool	sequence_tasks(struct s_data *ctx)
{
	return combine_tasks(ctx, CMD_SQ);
}

bool	pipeline_tasks(struct s_data *ctx)
{
	return combine_tasks(ctx, CMD_PL);
}

bool	if_tasks(struct s_data *ctx)
{
	return combine_tasks(ctx, CMD_IF);
}
