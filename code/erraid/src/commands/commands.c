#include "commands/commands.h"

static bool handle_request(struct s_data *ctx, struct s_request *req)
{
	uint16_t	raw_opcode;
	uint16_t	opcode;
	uint64_t	taskid;
	memcpy(&raw_opcode, req->buf, sizeof(uint16_t));

	opcode = be16toh(raw_opcode);

	switch (opcode) {
	case OPCODE_LS:
		list_tasks(ctx);
		break;
	case OPCODE_TX:
		memcpy(&taskid, req->buf + 2, sizeof(uint64_t));
		taskid = htobe64(taskid);
		exitcode_tasks(ctx, taskid);
		break;
	case OPCODE_SO:
		memcpy(&taskid, req->buf + 2, sizeof(uint64_t));
		taskid = htobe64(taskid);
		std_tasks(ctx, taskid, "stdout");
		break;
	case OPCODE_SE:
		memcpy(&taskid, req->buf + 2, sizeof(uint64_t));
		taskid = htobe64(taskid);
		std_tasks(ctx, taskid, "stderr");
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

void handle_all_requests(struct s_data *ctx, struct pollfd *pfds)
{
	struct s_request req = {0};
	int		ready = 0;

	ready = poll(pfds, 1, 0);
	if (ready < 0) {
		if (errno == EINTR)
			return ; // for signal interruption
		ERR_SYS("poll");
		return ;
	}
	if (ready == 0)
		return ;

	if (!(pfds[0].revents & POLLIN))
		return ;


	if (!readfifo(ctx->fifo_request, &req)) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		ERR_SYS("read request pipe");
		return ;
	}
	if (!req.buf)
		return ;
	//printf("Received request of size %zu %s\n", req.buf_size, ctx->run_directory);
	handle_request(ctx, &req);
	return ;
}
