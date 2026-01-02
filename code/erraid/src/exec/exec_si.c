#include "exec/exec_si.h"

extern char **environ;

/**
 * @brief forks and execve the command
 *
 * @param cmd the the command to execute
 * @param fd_in input fd, or NO_REDIRECT
 * @param fd_out output fd, or NO_REDIRECT
 * @param parent_pl parent pipeline (to close inherited fds), or NULL
 *
 * @return
 *  @retval true on success
 *  @retval false on failure
 */
bool	exec_si(struct s_cmd *cmd, int fd_in, int fd_out,
		  struct s_cmd_pl *parent_pl)
{
	struct s_cmd_si	*cmd_si = &cmd->cmd.cmd_si;

	if (!cmd_si || !cmd_si->command || !cmd_si->command[0]) {
		ERR_MSG("Invalid simple command");
		return false;
	}
	
	if (cmd_si->cmd_path[0] == '\0' &&
	    !find_binary_path(cmd_si->command[0], cmd_si->cmd_path)) {
		ERR_MSG("binary not found");
		return false;
	}
	
	cmd->pid = fork();
	switch ((cmd->pid)) {
	case -1:
		ERR_SYS("fork");
		return false;

	case 0:
		// close inherited pipeline fds we don't need
		if (parent_pl)
			close_pipes_except(parent_pl, fd_in, fd_out);

		if (cmd_si->stdout_path && \
		    !setup_output_last_cmd(cmd_si->stdout_path, cmd_si->stderr_path)) {
			ERR_MSG("setup_output_last_cmd failed in cmd id %d", cmd->cmd_id);
			exit(EXIT_FAILURE);
		}
		else if (fd_out != NO_REDIRECT && !setup_output_fd(fd_out)) {
			ERR_MSG("setup_output_fd failed in cmd id %d", cmd->cmd_id);
			exit(EXIT_FAILURE);
		}
		if (fd_in != NO_REDIRECT && !setup_input_fd(fd_in)) {
			ERR_MSG("setup_input_fd failed in cmd id %d", cmd->cmd_id);
			exit(EXIT_FAILURE);
		}
		execve(cmd_si->cmd_path, cmd_si->command, environ);
		ERR_SYS("execve");
		exit(EXIT_FAILURE);
	}

	return true;
}


/**
 * @brief calls exec_si but also waitpids it
 *
 * @param cmd the command
 * @param fd_in the fd to use as stdin, NO_REDIRECT (-1) for no redirect
 * @param fd_out the fd to use as stdout, NO_REDIRECT (-1) for no redirect
 *
 * @return
 *  @retval true on success
 *  @retval false on failure
 */
bool	exec_si_and_wait(struct s_cmd *cmd, int fd_in, int fd_out)
{
	int	status = 0;

	if (!exec_si(cmd, fd_in, fd_out, NULL))
		return false;

	waitpid(cmd->pid, &status, 0);
	cmd->exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : 0xFF;
	return true;
}
