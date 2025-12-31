# include "exec/exec.h"

extern char **environ;

/**
 * @brief executes a pipeline
 *
 * @param cmd the CMD_PL command to execute
 * @param fd_in input fd for first command, or NO_REDIRECT
 * @param fd_out output fd for last command, or NO_REDIRECT
 *
 * @return
 *  @retval true on success
 *  @retval false on failure
 */
static bool	exec_pl(struct s_cmd *cmd, int fd_in, int fd_out)
{
	struct s_cmd_pl	*cmd_pl = &cmd->cmd.cmd_pl;

	// create all pipes beforehand
	if (!create_all_pipes(cmd_pl))
		return false;

	// execute the pipeline 1; 1->n-1; n
	exec_first_command(cmd_pl, fd_in);
	exec_middle_commands(cmd_pl);
	exec_last_command(cmd_pl, fd_out);

	// closing all fds, much much easier that way
	close_all_pipes(cmd_pl);

	// wait for all commands that were launched parallely
	wait_for_pipeline(cmd_pl);
	cmd->exit_code = cmd_pl->cmds[cmd_pl->nb_cmds - 1].exit_code;

	return true;
}

static bool	exec_pl_if_parent_pl(struct s_cmd *cmd,
				     struct s_cmd_pl *parent_pl,
				     int fd_in, int fd_out)
{
	int pid = 0;

	pid = fork();
	switch ((pid)) {
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
		exec_pl(cmd, NO_REDIRECT, NO_REDIRECT);
		exit(cmd->exit_code);

	default:
		cmd->pid = pid;
	}
	return true;
}

bool	exec_cmd(struct s_cmd *cmd, int fd_in, int fd_out,
	         enum cmd_type parent_type, struct s_cmd_pl *parent_pl)
{
	int			retval = 0;
	struct s_cmd_sq 	*cmd_sq = NULL;

	switch (cmd->cmd_type) {
	case CMD_SI:
		if (parent_type == CMD_PL) {
			retval = spawn_cmd(cmd, fd_in, fd_out);
			if (!retval) ERR_MSG("spawn_cmd(cmd, %d, %d)", fd_in, fd_out);
		}
		else {
			retval = run_cmd(cmd, fd_in, fd_out);
			if (!retval) ERR_MSG("run_cmd(cmd, %d, %d)", fd_in, fd_out);
		}
		break;

	case CMD_SQ:
		cmd_sq = &cmd->cmd.cmd_sq;
		for (int i = 0; i < cmd_sq->nb_cmds; i++)
			retval = exec_cmd(&cmd_sq->cmds[i], fd_in, fd_out, CMD_SQ, NULL);

		// command sequence exit code should be the last command's exit code
		if (cmd_sq->nb_cmds > 0)
			cmd->exit_code = cmd_sq->cmds[cmd_sq->nb_cmds - 1].exit_code;
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
