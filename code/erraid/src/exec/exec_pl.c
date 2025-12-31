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
bool	exec_pl(struct s_cmd *cmd, int fd_in, int fd_out)
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

bool	exec_pl_if_parent_pl(struct s_cmd *cmd,
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
