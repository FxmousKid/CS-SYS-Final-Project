#include "exec/exec_or.h"

bool	exec_or(struct s_cmd *cmd, int fd_in, int fd_out)
{
	struct s_cmd_or 	*cmd_or = NULL;
	bool			retval;
	int			last_executed = 0;

	cmd_or = &cmd->cmd.cmd_or;
	for (int i = 0; i < cmd_or->nb_cmds; i++) {
		retval = exec_cmd(&cmd_or->cmds[i], fd_in, fd_out, CMD_OR, NULL);
		last_executed = i;
		// For ||: stop if the command succeeded (exit_code == 0)
		if (cmd_or->cmds[i].exit_code == 0)
			break;
	}

	// command or exit code should be the last executed command's exit code
	if (cmd_or->nb_cmds > 0)
		cmd->exit_code = cmd_or->cmds[last_executed].exit_code;

	return retval;
}

bool	exec_or_if_parent_pl(struct s_cmd *cmd, struct s_cmd_pl *parent_pl,
			     int fd_in, int fd_out)
{
	pid_t	pid = 0;

	pid = fork();
	switch (pid) {
	case -1:
		ERR_SYS("fork");
		return false;
	
	case 0:
		if (parent_pl)
			close_pipes_except(parent_pl, fd_in, fd_out);
		if (fd_in != NO_REDIRECT && !setup_input_fd(fd_in))
			exit(EXIT_FAILURE);
		if (fd_out != NO_REDIRECT && !setup_output_fd(fd_out))
			exit(EXIT_FAILURE);
		exec_or(cmd, NO_REDIRECT, NO_REDIRECT);
		exit(cmd->exit_code);

	default:
		cmd->pid = pid;
		return true;
	}
}
