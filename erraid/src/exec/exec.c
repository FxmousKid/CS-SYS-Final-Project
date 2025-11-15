# include "exec/exec.h"

extern char **environ;

static bool execute_simple_command(struct s_cmd_si *cmd_si, uint8_t *exit_code, pid_t *pid)
{
	int	status;

	if (!cmd_si || !cmd_si->command || !cmd_si->command[0]) {
		ERR_MSG("Invalid simple command");
		return false;
	}
	
	if (cmd_si->cmd_path[0] == '\0') {
		if (!find_binary_path(cmd_si->command[0], cmd_si->cmd_path)) {
		ERR_MSG("binary not found");
		return false;
		}
	}
	
	status = 0;
	*pid = fork();
	switch((*pid)){
	case -1:
		ERR_SYS("fork");
		return false;
	case 0:
		execve(cmd_si->cmd_path, cmd_si->command, environ);
		ERR_SYS("execve");
		exit(EXIT_FAILURE);
	default:
		waitpid(*pid, &status, 0);
		*exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : 0xFF;
		return true;
	}
}

static bool	execute_command(struct s_cmd *cmd)
{
	bool		success = true;
	struct s_cmd_sq *cmd_sq = NULL;

	if (!cmd) {
		ERR_MSG("Null command");
		return false;
	}
	
	switch (cmd->cmd_type) {
	case CMD_SI:
		success = execute_simple_command(&cmd->cmd.cmd_si, &cmd->exit_code, &cmd->pid);
		break;
		
	case CMD_SQ:
		// Debug lines
		// printf("Executing sequence of %d commands...\n", cmd->cmd.cmd_sq.nb_cmds);
		cmd_sq = &cmd->cmd.cmd_sq;
		for (int i = 0; i < cmd_sq->nb_cmds; i++)
			///printf("Command %d/%d:\n", i + 1, cmd->cmd.cmd_sq.nb_cmds);
			// if the command fails the loop continues
			success &= execute_command(&cmd_sq->cmds[i]);
				
		// command sequence exit code should be the last command's exit code
		if (cmd_sq->nb_cmds > 0)
			cmd->exit_code = cmd_sq->cmds[cmd_sq->nb_cmds - 1].exit_code;
		break;
		
	default:
		ERR_MSG("Unsupported command type");
		return false;
	}
	
	return success;
}

static bool	setup_output_redir(const char *stdout_file, const char *stderr_file)
{
	int	stdout_fd;
	int	stderr_fd;

	if (stdout_file) {
		stdout_fd = open(stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (stdout_fd == -1) {
			ERR_SYS("open stdout");
			return false;
		}
		if (dup2(stdout_fd, STDOUT_FILENO) == -1) {
			ERR_SYS("dup2 stdout");
			close(stdout_fd);
			return false;
		}
		close(stdout_fd);
	}
    
	if (stderr_file) {
		stderr_fd = open(stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (stderr_fd == -1) {
			ERR_SYS("open stderr");
			return false;
		}
		if (dup2(stderr_fd, STDERR_FILENO) == -1) {
			ERR_SYS("dup2 stderr");
			close(stderr_fd);
			return false;
		}
		close(stderr_fd);
	}
	return true;
}


static bool	exec_cmd_with_redir(struct s_cmd *cmd, 
			    const char *stdout_path, 
			    const char *stderr_path)
{
	pid_t	pid;
	int	status;
	bool	success;

	pid = fork();
    	switch((pid)){
	case -1:
		ERR_SYS("fork");
		return false;
	case 0:
		if (!setup_output_redir(stdout_path, stderr_path))
			exit(EXIT_FAILURE);
		success = execute_command(cmd);
		exit(success ? cmd->exit_code : EXIT_FAILURE);
	default:
		cmd->pid = pid;
		waitpid(pid, &status, 0);
		cmd->exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : 0xFF;
		return true;
    	}
}

bool exec_task(struct s_task *task)
{
	return exec_cmd_with_redir(task->cmd,
		 		task->stdout_path, 
		 		task->stderr_path);
}