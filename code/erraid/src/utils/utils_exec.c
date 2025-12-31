#include "utils/utils_exec.h"

extern char **environ;

bool	setup_pipe(int fds[2])
{
	ERR_MSG("XXXX")
	if (pipe(fds) < 0) {
		ERR_SYS("pipe");
		return false;
	}
	return true;
}

bool	setup_input_fd(int fd)
{
	if (fd <= 0)
		return true;

	if (dup2(fd, STDIN_FILENO) < 0) {
		ERR_SYS("dup2 fd = %d", fd);
		return false;
	}
	if (close(fd) < 0) {
		ERR_SYS("close fd = %d", fd);
		return false;
	}
	return true;
}

bool	setup_output_fd(int fd)
{	
	if (fd <= 0)
		return true;

	if (dup2(fd, STDOUT_FILENO) < 0) {
		ERR_SYS("dup2 fd = %d", fd);
		return false;
	}
	if (close(fd) < 0) {
		ERR_SYS("close fd = %d", fd);
		return false;
	}
	return true;
}

bool	wait_for_pipeline(struct s_cmd_pl *cmd_pl)
{
	int	status = 0;

	for (int i = 0; i < cmd_pl->nb_cmds; i++) {
		if (cmd_pl->cmds[i].pid <= 0)
			continue;
		waitpid(cmd_pl->cmds[i].pid, &status, 0);
		printf("Successfully waited for command %d!\n", i);
		fflush(stdout);
		if (WIFEXITED(status))
			cmd_pl->cmds[i].exit_code = WEXITSTATUS(status);
	}

	return true;
}

bool	setup_output_last_cmd(const char *stdout_file, const char *stderr_file)
{
	int	stdout_fd;
	int	stderr_fd;

	if (stdout_file) {
		stdout_fd = open(stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
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
		stderr_fd = open(stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
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

/**
 * @brief forks and execve the command
 *
 * @param cmd the the command to execute
 * @param fd_in the fd to use as stdin, ignores if <= 0
 * @param fd_out the fd to use as stdout, ignores if <= 0
 *
 * @return
 *  @retval true on success
 *  @retval false on failure
 */
bool	spawn_cmd(struct s_cmd *cmd, int fd_in, int fd_out)
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
	switch((cmd->pid)){
	case -1:
		ERR_SYS("fork");
		return false;

	case 0:
		if (cmd_si->stdout_path && \
		    !setup_output_last_cmd(cmd_si->stdout_path, cmd_si->stderr_path)) {
			ERR_MSG("setup_output_last_cmd failed in cmd id %d", cmd->cmd_id);
			exit(EXIT_FAILURE);
		}
		else if (fd_out && !setup_output_fd(fd_out)) {
			ERR_MSG("setup_output_fd failed in cmd id %d", cmd->cmd_id);
			exit(EXIT_FAILURE);
		}
		if (fd_in && !setup_input_fd(fd_in)) {
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
 * @brief calls spawn_cmd but also waitpids it
 *
 * @param cmd the command
 * @param fd_in the fd to use as stdin, ignores if <= 0
 * @param fd_out the fd to use as stdout, ignores if <= 0
 *
 * @return
 *  @retval true on success
 *  @retval false on failure
 */
bool	run_cmd(struct s_cmd *cmd, int fd_in, int fd_out)
{
	int	status = 0;

	if (!spawn_cmd(cmd, fd_in, fd_out))
		return false;

	waitpid(cmd->pid, &status, 0);
	cmd->exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : 0xFF;
	return true;
}
