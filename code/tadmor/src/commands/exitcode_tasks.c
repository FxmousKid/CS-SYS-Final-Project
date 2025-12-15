
#include "commands/exitcode_tasks.h"

static bool	handle_exitcode_reply(struct s_data *ctx)
{
	int		fd_reply;
	int		len;
	char		buf[24];
	uint16_t	ans = 0;
	uint32_t	nbruns = 0;
	uint16_t	exitcode = 0;
	uint64_t	time = 0;
	time_t 		timestamp;
	struct tm * 	time_infos;

	if ((fd_reply = open(ctx->fifo_reply, O_RDONLY)) < 0) {
		ERR_SYS("open fifo_reply: %s", ctx->fifo_reply);
		return false;
	}

	if (!read_uint16(fd_reply, &ans))
		goto error;

	if (ans == OPCODE_ER)
		goto error;

	read_uint32(fd_reply, &nbruns);
	for (uint32_t i = 0; i < nbruns; i++) {
		read_uint64(fd_reply, &time);
		read_uint16(fd_reply, &exitcode);

		timestamp = time;
		time_infos = localtime(&timestamp);

		len = snprintf(buf, 24, "%04d-%02d-%02d %02d:%02d:%02d %u\n", time_infos->tm_year+1900, time_infos->tm_mon+1, time_infos->tm_mday,
        		time_infos->tm_hour, time_infos->tm_min, time_infos->tm_sec, exitcode);
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
	if (close(fd_reply) < 0)
		ERR_SYS("close fifo_reply");
	//write(STDOUT_FILENO, "identifiant de tâche non trouvé\n", 34);
	return false;
}


bool	exitcode_tasks(struct s_data *ctx)
{
	if (!request_opt_tasks(ctx->fifo_request, OPCODE_TX, ctx->task_id))
		return false;
	if (!handle_exitcode_reply(ctx))
		return false;
	return true;
}