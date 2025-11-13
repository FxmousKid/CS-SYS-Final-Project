# include "exec/exec.h"

extern char **environ;

bool execute_simple_command(struct s_cmd_si *cmd_si, short *exit_code, pid_t *pid)
{
    if (!cmd_si || !cmd_si->command || !cmd_si->command[0]) {
        ERR_MSG("Invalid simple command");
        return false;
    }
    
    // Vérifier que le chemin du binaire est défini
    if (cmd_si->cmd_path[0] == '\0') {
        if (!find_binary_path(cmd_si->command[0], cmd_si->cmd_path)) {
            ERR_MSG("binary not found");
            return false;
        }
    }
    
    *pid = fork();
    
    if (*pid == -1) {
        ERR_SYS("fork");
        return false;
    }
    
    if (*pid == 0) {
        // Processus enfant
        execve(cmd_si->cmd_path, cmd_si->command, environ);
        ERR_SYS("execve");
        exit(EXIT_FAILURE);
    } else {
        // Processus parent
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