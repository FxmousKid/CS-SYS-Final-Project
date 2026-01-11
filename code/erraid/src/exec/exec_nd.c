#include "exec/exec_nd.h"

bool	exec_nd(struct s_cmd *cmd, int fd_in, int fd_out)
{
	struct s_cmd_nd 	*cmd_nd = NULL;
	bool			retval;

	cmd_nd = &cmd->cmd.cmd_nd;
	for (int i = 0; i < cmd_nd->nb_cmds; i++) {
		retval = exec_cmd(&cmd_nd->cmds[i], fd_in, fd_out, CMD_ND, NULL);
                if (retval != 0)
                        break;
        }

	// command and exit code should be the last command's exit code
        // since commands after are not executed
	if (cmd_nd->nb_cmds > 0)
		cmd->exit_code = cmd_nd->cmds[cmd_nd->nb_cmds - 1].exit_code;

	return retval;
}

bool	exec_nd_if_parent_pl(struct s_cmd *cmd, struct s_cmd_pl *parent_pl,
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
		exec_nd(cmd, NO_REDIRECT, NO_REDIRECT);
		exit(cmd->exit_code);

	default:
		cmd->pid = pid;
		return true;
	}
}
