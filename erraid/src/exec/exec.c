# include "exec/exec.h"

extern char **environ;

bool execute_simple_command(struct s_cmd_si *cmd_si, short *exit_code, pid_t *pid)
{
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
	
	switch((*pid = fork())){
		case -1:
		ERR_SYS("fork");
		return false;
		case 0:
		execve(cmd_si->cmd_path, cmd_si->command, environ);
		ERR_SYS("execve");
		exit(EXIT_FAILURE);
		default:
		int status;
		waitpid(*pid, &status, 0);
		
		if (WIFEXITED(status)) {
			*exit_code = WEXITSTATUS(status);
		} else {
			*exit_code = 0xFFFF;
		}
		return true;
	}
}

bool	execute_command(struct s_cmd *cmd)
{
	if (!cmd) {
		ERR_MSG("Null command");
		return false;
	}
	
	bool success = true;
	
	switch (cmd->cmd_type) {
		case CMD_SI:
			success = execute_simple_command(&cmd->cmd.cmd_si, &cmd->exit_code, &cmd->pid);
			break;
			
		case CMD_SQ:
            // Debug lines
			//printf("Executing sequence of %d commands...\n", cmd->cmd.cmd_sq.nb_cmds);
			for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; i++) {
				///printf("Command %d/%d:\n", i + 1, cmd->cmd.cmd_sq.nb_cmds);
				success &= execute_command(&cmd->cmd.cmd_sq.cmds[i]);
				// if the command fails the loop continues
			}
			// command sequence exit code should be the last command's exit code
			if (cmd->cmd.cmd_sq.nb_cmds > 0) {
				cmd->exit_code = cmd->cmd.cmd_sq.cmds[cmd->cmd.cmd_sq.nb_cmds - 1].exit_code;
			}
			break;
			
		default:
			ERR_MSG("Unsupported command type PIPE or IF maybe");
			return false;
	}
	
	return success;
}

bool	execute_command_with_redirection(struct s_cmd *cmd, const char *stdout_file, const char *stderr_file)
{
	pid_t pid;
    	switch((pid = fork())){
		case -1:
			ERR_SYS("fork");
			return false;
		case 0:
			if (!setup_output_redirection(stdout_file, stderr_file)) {
				exit(EXIT_FAILURE);
			}
			bool success = execute_command(cmd);
			exit(success ? cmd->exit_code : EXIT_FAILURE);
		default:
			cmd->pid = pid;
			int status;
			waitpid(pid, &status, 0);
			
			if (WIFEXITED(status)) {
				cmd->exit_code = WEXITSTATUS(status);
			}
			else {
				cmd->exit_code = 0xFFFF;
			}
			return true;
    	}   
}

bool setup_output_redirection(const char *stdout_file, const char *stderr_file)
{
    if (stdout_file) {
        int stdout_fd = open(stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
        int stderr_fd = open(stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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