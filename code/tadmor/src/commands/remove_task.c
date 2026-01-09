#include "commands/remove_task.h"

static bool     handle_request(struct s_data *ctx)
{
	uint16_t opcode;
	opcode = OPCODE_RM;
	if (!request_opt_tasks(ctx->fifo_request, opcode, ctx->task_id))
        	return false;
	return true;
}

static bool     handle_reply(struct s_data *ctx)
{
	int 		fd_reply;
	uint16_t	anstype;
	uint16_t	errcode;

	if ((fd_reply = open(ctx->fifo_reply, O_RDONLY)) < 0) {
		ERR_SYS("open fifo_reply: %s", ctx->fifo_reply);
		return false;
	}
	if (!read_uint16(fd_reply, &anstype))
		goto error;
	if (anstype == OPCODE_ER)
		goto error;
	if (close(fd_reply) < 0) {
		ERR_SYS("close fifo_reply");
		return false;
	}
	return true;
error:
	// Maybe do something with the errcode ?
	if (!read_uint16(fd_reply, &errcode))
		return false;
	if (close(fd_reply) < 0)
		return false;
	return false;
}

bool            remove_task(struct s_data *ctx)
{
	if (!handle_request(ctx))
		return false;
	if (!handle_reply(ctx))
		return false;
	return true;
}