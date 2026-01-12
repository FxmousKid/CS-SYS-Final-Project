#include "exec/exec_if.h"

bool	exec_if(struct s_cmd *cmd, int fd_in, int fd_out)
{
	struct s_cmd_if *cmd_if = NULL;
	bool		conditional_is_true = false;
	bool		ret = 0;

	cmd_if = &cmd->cmd.cmd_if;

	ret = exec_cmd(cmd_if->conditional, NO_REDIRECT, NO_REDIRECT, CMD_IF, NULL);
	conditional_is_true = (cmd_if->conditional->exit_code == 0);
	if (!ret) {
		ERR_MSG("Failed to exec_cmd()")
		return false;
	}

	if (conditional_is_true) {
		ret = exec_cmd(cmd_if->cmd_if_true, fd_in, fd_out, CMD_IF, NULL);
		if (!ret) {
			ERR_MSG("Failed to exec_cmd()")
			return false;
		}
		cmd->exit_code = cmd_if->cmd_if_true->exit_code;
	}

	else if (cmd_if->cmd_if_false) {
		ret = exec_cmd(cmd_if->cmd_if_false, fd_in, fd_out, CMD_IF, NULL);
		if (!ret) {
			ERR_MSG("Failed to exec_cmd()")
			return false;
		}
		cmd->exit_code = cmd_if->cmd_if_false->exit_code;
	}
	else {
		cmd->exit_code = cmd_if->conditional->exit_code;
	}

	return ret;
}

bool	exec_if_if_parent_pl(struct s_cmd *cmd, struct s_cmd_pl *parent_pl, int fd_in, int fd_out)
{
	pid_t	pid = 0;

	pid = fork();
	switch (pid) {
	case -1:
		ERR_SYS("fork")
		return false;

	case 0:
		if (parent_pl)
			close_pipes_except(parent_pl, fd_in, fd_out);
		if (fd_in != NO_REDIRECT && !setup_input_fd(fd_in))
			exit(EXIT_FAILURE);
		if (fd_out != NO_REDIRECT && !setup_output_fd(fd_out))
			exit(EXIT_FAILURE);
		exec_if(cmd, NO_REDIRECT, NO_REDIRECT);
		exit(cmd->exit_code);
	
	default:
		cmd->pid = pid;
		return true;
	}
}
