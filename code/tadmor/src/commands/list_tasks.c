#include "commands/list_tasks.h"

static void	construct_list_tasks_payload(void *payload)
{
	enum req_opcode opcode = OPCODE_LS;
	memcpy(payload, &opcode, 2);
}

static bool	request_list_tasks(struct s_data *ctx)
{
	uint8_t	buf[2] = {0};

	construct_list_tasks_payload(buf);
	if (!writefifo(ctx->fifo_request, buf, sizeof(buf)))
		return false;
	return true;
}

static bool	handle_list_tasks_reply(struct s_data *ctx)
{
	struct s_reply reply = {0};


	if (!readfifo(ctx->fifo_reply, &reply))
		return false;
	write(STDOUT_FILENO, reply.buf, reply.buf_size);
	return true;
}

bool	list_tasks(struct s_data *ctx)
{
	if (!request_list_tasks(ctx))
		return false;

	if (!handle_list_tasks_reply(ctx))
		return false;

	return true;
}
