#include "commands/commands.h"

static bool handle_request(struct s_data *ctx, struct s_reply *req)
{
	uint16_t	opcode;

	memcpy(&opcode, req->buf, sizeof(uint16_t));

	switch (opcode) {
	case OPCODE_LS:
		list_tasks(ctx);
		break;
	case OPCODE_CR:
		break;
	case OPCODE_RM:
		break;
	default:
		break;
	}

	free(req->buf);
	return true;
}

void handle_all_requests(struct s_data *ctx)
{
	for (;;) {
		struct s_reply req = {0};

		if (!readfifo(ctx->fifo_request, &req)) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			ERR_SYS("read request pipe");
			break;
		}

		handle_request(ctx, &req);
		// if (req.buf) {
		// 	free(req.buf);
		// 	req.buf = NULL;
		// }
	}
}
