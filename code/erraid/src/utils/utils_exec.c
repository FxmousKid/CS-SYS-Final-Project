#include "utils/utils_exec.h"

/* Closes all pipes in the pipe_fds array of the passed pipeline */
bool	close_all_pipes(struct s_cmd_pl *cmd_pl)
{
	for (int i = 0; i < cmd_pl->nb_cmds - 1; i++) {
		if (cmd_pl->fds[i][0] >= 0 && close(cmd_pl->fds[i][0]) < 0)
			ERR_SYS("close fd = %d", cmd_pl->fds[i][0]);
		cmd_pl->fds[i][0] = -1;
		if (cmd_pl->fds[i][1] >= 0 && close(cmd_pl->fds[i][1]) < 0)
			ERR_SYS("close fd = %d", cmd_pl->fds[i][1]);
		cmd_pl->fds[i][1] = -1;
	}
	return true;
}

/* Like close_all_pipes() execpt we ignore keep_fd1 and keep_fd2 */
bool	close_pipes_except(struct s_cmd_pl *cmd_pl, int keep_fd1, int keep_fd2)
{
	int	fd;

	for (int i = 0; i < cmd_pl->nb_cmds - 1; i++) {
		fd = cmd_pl->fds[i][0];
		if (fd >= 0 && fd != keep_fd1 && fd != keep_fd2) {
			if (close(fd) < 0)
				ERR_SYS("close fd = %d", fd);
		}
		fd = cmd_pl->fds[i][1];
		if (fd >= 0 && fd != keep_fd1 && fd != keep_fd2) {
			if (close(fd) < 0)
				ERR_SYS("close fd = %d", fd);
		}
	}
	return true;
}

void	print_pipe_array(int fds[][2], int nb_pipes)
{
	printf("pipes: {");
	for (int i = 0; i < nb_pipes; i++) {
		printf(" [%d, %d]", fds[i][0], fds[i][1]);
		if (i < nb_pipes - 1)
			printf(",");
	}
	printf("}\n");
}

bool	setup_pipe(int fds[2])
{
	if (pipe(fds) < 0) {
		ERR_SYS("pipe");
		return false;
	}
	return true;
}

bool	setup_input_fd(int fd)
{
	if (fd == NO_REDIRECT)
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
	if (fd == NO_REDIRECT)
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
