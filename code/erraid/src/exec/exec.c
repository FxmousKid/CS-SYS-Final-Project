# include "exec/exec.h"

extern char **environ;

bool	exec_cmd(struct s_cmd *cmd, int fd_in, int fd_out,
	         enum cmd_type parent_type, struct s_cmd_pl *parent_pl)
{
	int			retval = 0;

	switch (cmd->cmd_type) {
	case CMD_SI:
		if (parent_type == CMD_PL) {
			retval = spawn_cmd(cmd, fd_in, fd_out, parent_pl);
			if (!retval) ERR_MSG("spawn_cmd(cmd, %d, %d)", fd_in, fd_out);
		}
		else {
			retval = run_cmd(cmd, fd_in, fd_out);
			if (!retval) ERR_MSG("run_cmd(cmd, %d, %d)", fd_in, fd_out);
		}
		break;

	case CMD_SQ:
		if (parent_type == CMD_PL)
			retval = exec_sq_if_parent_pl(cmd, parent_pl, fd_in, fd_out);
		else
			retval = exec_sq(cmd, fd_in, fd_out);
		break;
	
	case CMD_PL:
		if (parent_type == CMD_PL)
			retval = exec_pl_if_parent_pl(cmd, parent_pl, fd_in, fd_out);
		else
			retval = exec_pl(cmd, fd_in, fd_out);
		break;

	default:
		return false;
	}

	return retval;
}

bool	exec_task(struct s_task *task)
{
	return exec_cmd(task->cmd, NO_REDIRECT, NO_REDIRECT, 0, NULL);
}
