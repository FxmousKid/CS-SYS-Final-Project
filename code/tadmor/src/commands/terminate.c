# include "commands/terminate.h"

static bool     handle_request(struct s_data *ctx)
{
	uint16_t opcode;
	opcode = htobe16(OPCODE_TM);
	if (!writefifo(ctx->fifo_request, &opcode, sizeof(opcode)))
		return false;
	return true;
}

static bool     handle_reply(struct s_data *ctx)
{
	int 		fd_reply;
	uint16_t	anstype;

	if ((fd_reply = open(ctx->fifo_reply, O_RDONLY)) < 0) {
		ERR_SYS("open fifo_reply: %s", ctx->fifo_reply);
		return false;
	}
	if (!read_uint16(fd_reply, &anstype))
		goto error;

	if (close(fd_reply) < 0) {
		ERR_SYS("close fifo_reply");
		return false;
	}
	return true;
error :
	if (close(fd_reply) < 0) {
		ERR_SYS("close fifo_reply");
		return false;
	}
	return false;
}

bool	terminate(struct s_data *ctx)
{
	if (!handle_request(ctx))
		return false;
	if (!handle_reply(ctx))
		return false;
	return true;
}