#include "exec/exec_pl.h"

// doesn't need to be included in any headers
// but is called by functions in this TU
bool	exec_A(struct s_cmd *cmd, int fd_in, int fd_out, enum cmd_type parent_type);

bool	exec_first_command(struct s_cmd_pl *cmd_pl, int fd_in)
{
	bool	retval = true;

	if (!setup_pipe(cmd_pl->fds[0]))
		return false;


	retval = exec_A(cmd_pl->cmds, fd_in, cmd_pl->fds[0][1], CMD_PL);

	if (close(cmd_pl->fds[0][1]) < 0)
		ERR_SYS("close fd = %d", cmd_pl->fds[0][1]);
	cmd_pl->fds[0][1] = -1;

	return retval;
}

bool	exec_middle_commands(struct s_cmd_pl *cmd_pl)
{
	bool	retval = true;
	int	(*fds)[2] = cmd_pl->fds;
	int	fd_in = 0;
	int	fd_out = 0;

	for (int i = 1; i < cmd_pl->nb_cmds - 1; i++) {
		if (!setup_pipe(fds[i]))
			return false;

		fd_in = fds[i - 1][0];
		fd_out = fds[i][1];

		if (i != 1 && close(cmd_pl->fds[i - 1][1]) < 0)
		// if (close(cmd_pl->fds[i - 1][1]) < 0)
			ERR_SYS("close fd = %d", cmd_pl->fds[i - 1][1]);
		// cmd_pl->fds[i - 1][1] = -1;

		retval = exec_A(cmd_pl->cmds + i, fd_in, fd_out, CMD_PL);
		if (!retval) return false;

		if (i != cmd_pl->nb_cmds - 2 && close(cmd_pl->fds[i - 1][0]) < 0)
			ERR_SYS("close fd = %d", cmd_pl->fds[i - 1][0]);
		// cmd_pl->fds[i - 1][0] = -1;
	}
	return true;
}

bool	exec_last_command(struct s_cmd_pl *cmd_pl, int fd_out)
{
	struct s_cmd	*last_cmd = cmd_pl->cmds + (cmd_pl->nb_cmds - 1);
	int		*last_fds = cmd_pl->fds[cmd_pl->nb_cmds - 2];
	bool		retval = true;

	if (close(last_fds[1]) < 0)
		ERR_SYS("close fd = %d", last_fds[1]);
	last_fds[1] = -1;

	retval = exec_A(last_cmd, last_fds[0], fd_out, CMD_PL);

	if (close(last_fds[0]) < 0)
		ERR_SYS("close fd = %d", last_fds[0]);
	last_fds[0] = -1;

	return retval;
}
