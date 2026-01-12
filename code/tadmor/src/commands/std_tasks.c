#include "commands/std_tasks.h"

static bool	handle_std_reply(struct s_data *ctx)
{
	int		fd_reply;
	int		len;
	char		buf[24] = {0};
	uint16_t	ans = 0;
	uint16_t	errtype = 0;
	uint32_t	nbchar;


	if ((fd_reply = open(ctx->fifo_reply, O_RDONLY)) < 0) {
		ERR_SYS("open fifo_reply: %s", ctx->fifo_reply);
		return false;
	}

	if (!read_uint16(fd_reply, &ans))
		goto error;

	if (ans == OPCODE_ER)
		goto error;
	read_uint32(fd_reply, &nbchar);
	while ((len = read(fd_reply, buf, 24)) > 0) {
		if (!write(STDOUT_FILENO, buf, len)) {
			ERR_SYS("write");
			return false;
		}
	}

	if (close(fd_reply) < 0) {
		ERR_SYS("close fifo_reply");
		return false;
	}
	return true;
error:
	if (!read_uint16(fd_reply, &errtype))
		return false;
	if (close(fd_reply) < 0)
		return false;
	// If the task hasn't been executed yet (OPCODE_NR), return success
	// but write nothing to stdout (task will have empty stdout/stderr)
	if (errtype == 0x4e52)
		return true;
	return false;
	
}


bool	std_tasks(struct s_data *ctx, enum req_opcode std)
{
	if (!request_opt_tasks(ctx->fifo_request, std, ctx->task_id))
		return false;
	if (!handle_std_reply(ctx))
		return false;
	return true;
}

bool	stdout_tasks(struct s_data *ctx)
{
	return std_tasks(ctx, OPCODE_SO);
}
bool	stderr_tasks(struct s_data *ctx)
{
	return std_tasks(ctx, OPCODE_SE);
}