#include "exec/exec_if.h"

bool	exec_if(struct s_cmd *cmd, struct s_cmd *parent_pl, int fd_in, int fd_out)
{
	(void)cmd;
	(void)parent_pl;
	(void)fd_in;
	(void)fd_out;


	return true;
}

bool	exec_if_if_parent_pl(struct s_cmd *cmd, struct s_cmd *parent_pl, int fd_in, int fd_out)
{
	(void)cmd;
	(void)parent_pl;
	(void)fd_in;
	(void)fd_out;

	return true;
}
