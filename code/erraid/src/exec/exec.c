# include "exec/exec.h"

extern char **environ;

bool	exec_cmd(struct s_cmd *cmd, int fd_in, int fd_out,
	         enum cmd_type parent_type, struct s_cmd_pl *parent_pl)
{
	int			retval = 0;

	switch (cmd->cmd_type) {
	case CMD_SI:
		if (parent_type == CMD_PL) {
			retval = exec_si(cmd, fd_in, fd_out, parent_pl);
			if (!retval) ERR_MSG("exec_si(cmd, %d, %d)", fd_in, fd_out);
		}
		else {
			retval = exec_si_and_wait(cmd, fd_in, fd_out);
			if (!retval) ERR_MSG("exec_si_and_wait(cmd, %d, %d)", fd_in, fd_out);
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

	case CMD_IF:
		if (parent_type == CMD_PL)
			retval = exec_if_if_parent_pl(cmd, parent_pl, fd_in, fd_out);
		else
			retval = exec_if(cmd, fd_in, fd_out);
		break;


	default:
		ERR_MSG("Unknown cmd_type: %d", cmd->cmd_type);
		return false;
	}

	return retval;
}

// bool	exec_task(struct s_task *task)
// {
// 	return exec_cmd(task->cmd, NO_REDIRECT, NO_REDIRECT, 0, NULL);
// }

bool	exec_task(struct s_task *task)
{
	pid_t	pid = 0;

	pid = fork();
	switch (pid) {
	case -1:
		ERR_SYS("fork");
		return false;

	case 0:
		// Child : execute the task and wait
		if (task->stdout_path[0] != '\0') {
			int fd = open(task->stdout_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (fd >= 0) close(fd);
		}
		if (task->stderr_path[0] != '\0') {
			int fd = open(task->stderr_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (fd >= 0) close(fd);
		}
		if (!exec_cmd(task->cmd, NO_REDIRECT, NO_REDIRECT, 0, NULL))
			exit(EXIT_FAILURE);
		exit(task->cmd->exit_code);

	default:
		// Parent : store the child pid and return
		task->cmd->pid = pid;
		return true;
	}
}
