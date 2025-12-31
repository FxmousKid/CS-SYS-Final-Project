# include "exec/exec.h"

extern char **environ;

// static bool	exec_cmd_si(struct s_cmd *cmd,
// 			    int fd_in,
// 			    int fd_out)
// {
// 	int		status;
// 	uint16_t	*exit_code = &cmd->exit_code;
// 	struct s_cmd_si	*cmd_si = &cmd->cmd.cmd_si;

// 	if (!cmd_si || !cmd_si->command || !cmd_si->command[0]) {
// 		ERR_MSG("Invalid simple command");
// 		return false;
// 	}
// 	
// 	if (cmd_si->cmd_path[0] == '\0' &&
// 	    !find_binary_path(cmd_si->command[0], cmd_si->cmd_path)) {
// 		ERR_MSG("binary not found");
// 		return false;
// 	}
// 	
// 	status = 0;
// 	cmd->pid = fork();
// 	switch((cmd->pid)){
// 	case -1:
// 		ERR_SYS("fork");
// 		return false;

// 	case 0:
// 		if (cmd_si->stdout_path && \
// 		    !setup_output_last_cmd(cmd_si->stdout_path, cmd_si->stderr_path))
// 			exit(EXIT_FAILURE);
// 		else if (fd_out && !setup_output_fd(fd_out))
// 			exit(EXIT_FAILURE);
// 		if (fd_in && !setup_input_fd(fd_in))
// 			exit(EXIT_FAILURE);
// 		execve(cmd_si->cmd_path, cmd_si->command, environ);
// 		ERR_SYS("execve");
// 		exit(EXIT_FAILURE);

// 	default:
// 		waitpid(cmd->pid, &status, 0);
// 		*exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : 0xFF;
// 		return true;
// 	}
// }

// static bool	execute_command(struct s_cmd *cmd)
// {
// 	bool			success = true;
// 	struct s_cmd_sq 	*cmd_sq = NULL;

// 	if (!cmd) {
// 		ERR_MSG("Null command");
// 		return false;
// 	}
// 	
// 	switch (cmd->cmd_type) {
// 	case CMD_SI:
// 		success = exec_cmd_si(cmd, 0, 0);
// 		break;

// 	case CMD_SQ:
// 		cmd_sq = &cmd->cmd.cmd_sq;
// 		for (int i = 0; i < cmd_sq->nb_cmds; i++)
// 			success = execute_command(&cmd_sq->cmds[i]);
// 				
// 		// command sequence exit code should be the last command's exit code
// 		if (cmd_sq->nb_cmds > 0)
// 			cmd->exit_code = cmd_sq->cmds[cmd_sq->nb_cmds - 1].exit_code;
// 		break;

// 	case CMD_PL:
// 		break;
// 	
// 	default:
// 		ERR_MSG("Unsupported command type");
// 		return false;
// 	}
// 	return success;
// }


// // static bool	exec_cmd_with_redir(struct s_cmd *cmd,
// bool	exec_cmd_with_redir(struct s_cmd *cmd,
// 				    const char *stdout_path,
// 				    const char *stderr_path)
// {
// 	pid_t	pid;
// 	bool	success;

// 	pid = fork();
//     	switch((pid)){
// 	case -1:
// 		ERR_SYS("fork");
// 		return false;

// 	case 0:
// 		if (NEED_REDIR_BEFORE_FIRST_CMD(cmd) && \
// 		    !setup_output_last_cmd(stdout_path, stderr_path))
// 			exit(EXIT_FAILURE);
// 		success = execute_command(cmd);
// 		exit(success ? cmd->exit_code : EXIT_FAILURE);

// 	default:
// 		cmd->pid = pid;
// 		return true;
//     	}
// }

// static bool	exec_A(struct s_cmd *cmd)
bool	exec_A(struct s_cmd *cmd, int fd_in, int fd_out, enum cmd_type parent_type)
{
	int			retval = 0;
	struct s_cmd_sq 	*cmd_sq = NULL;
	struct s_cmd_pl 	*cmd_pl = NULL;

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
			retval = exec_A(&cmd_sq->cmds[i], fd_in, fd_out, CMD_SQ);

		// command sequence exit code should be the last command's exit code
		if (cmd_sq->nb_cmds > 0)
			cmd->exit_code = cmd_sq->cmds[cmd_sq->nb_cmds - 1].exit_code;
		break;
	
	case CMD_PL:
		cmd_pl = &cmd->cmd.cmd_pl;
		// first command
		// if (!exec_first_command(cmd_pl))
		// 	exit(EXIT_FAILURE);
		exec_first_command(cmd_pl, fd_in);

		// second to second-to-last
		exec_middle_commands(cmd_pl);

		// last command
		exec_last_command(cmd_pl, fd_out);
			
		// now we wait for all commands
		wait_for_pipeline(cmd_pl);
		cmd->exit_code = cmd_pl->cmds[cmd_pl->nb_cmds - 1].exit_code;

			
		break;

		

	default:
		return false;
	}

	return retval;
}

bool exec_task(struct s_task *task)
{

	return exec_A(task->cmd, 0, 0, 0);

	// return exec_cmd_with_redir(task->cmd,
	// 	 		   task->stdout_path,
	// 	 		   task->stderr_path);

}
