#include "exec/exec_pl.h"

// doesn't need to be included in any headers
// but is called by functions in this TU
bool	exec_cmd(struct s_cmd *cmd, int fd_in, int fd_out,
	       enum cmd_type parent_type, struct s_cmd_pl *parent_pl);

/* calls pipe(2) on all int[2] in the cmd_pl->fds array 
 * not the most correct, since we *will* eventually run out
 * of FDs, i.e. a pipeline with 10k commands, but at that points
 * my own shell (zsh 5.9) started leaking after more than 1100
 * consecutive ls | ls | ls ... so I'm not sure we're wrong
 * to allocate them all at first for each pipeline, altough
 * it certainly works, it just doesn't 'feel' the most correct
 * anyway as long as it works for our use case, we'll keep
 * the optimal solution for another discussion :/ 
 *
 * P.S. the argument could even be made for not even
 * trying to be evaluate 10k commands in the same pipeline,
 * for safety reasons ! perhaps as a natural defense against
 * pipe-bombs (though this belief should also be mirrored in 
 * the CMD_SQ part of the exection :)
 * */
bool	create_all_pipes(struct s_cmd_pl *cmd_pl)
{
	for (int i = 0; i < cmd_pl->nb_cmds - 1; i++) {
		if (!setup_pipe(cmd_pl->fds[i]))
			return false;
	}
	return true;
}

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

bool	exec_first_command(struct s_cmd_pl *cmd_pl, int fd_in)
{
	// fd_in is passed by exec_cmd, either NO_REDIRECT, or
	// even a pipe_fd (e.g. pipeline in pipeline)
	return exec_cmd(cmd_pl->cmds, fd_in, cmd_pl->fds[0][1], CMD_PL, cmd_pl);
}

bool	exec_middle_commands(struct s_cmd_pl *cmd_pl)
{
	int	(*fds)[2] = cmd_pl->fds;
	int	fd_in;
	int	fd_out;

	for (int i = 1; i < cmd_pl->nb_cmds - 1; i++) {
		fd_in = fds[i - 1][0];
		fd_out = fds[i][1];
		if (!exec_cmd(cmd_pl->cmds + i, fd_in, fd_out, CMD_PL, cmd_pl))
			return false;
	}
	return true;
}

bool	exec_last_command(struct s_cmd_pl *cmd_pl, int fd_out)
{
	struct s_cmd	*last_cmd = cmd_pl->cmds + (cmd_pl->nb_cmds - 1);
	int		*last_fds = cmd_pl->fds[cmd_pl->nb_cmds - 2];

	// like fd_in in exec_first_command, fd_out can either
	// be NO_REDIRECT, so output to task->stdout_path
	// or another pipeline...
	return exec_cmd(last_cmd, last_fds[0], fd_out, CMD_PL, cmd_pl);
}
