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