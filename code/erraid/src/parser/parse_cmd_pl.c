#include "parser/parse_cmd_pl.h"

bool	alloc_pipe_fds(struct s_cmd_pl *cmd_pl)
{
	if (!(cmd_pl->fds = calloc(cmd_pl->nb_cmds - 1, sizeof(*cmd_pl->fds)))) {
		ERR_SYS("calloc");
		return false;
	}
	return true;
}

bool	alloc_and_fill_pl_sub_dirs(struct s_cmd_pl *cmd_pl, const char *path)
{
	int	idx = 0;
	char	buf[PATH_MAX + 1] = {0};

	if (!(cmd_pl->cmds = calloc(cmd_pl->nb_cmds, sizeof(struct s_cmd)))) {
		ERR_SYS("calloc");
		return false;
	}

	strcpy(buf, path);
	while (idx < cmd_pl->nb_cmds) {
		if (buf[strlen(buf) - 1] != '/')
			buf[strlen(buf)] = '/'; 
		append_int_to_buf(buf, idx);
		strcpy(cmd_pl->cmds[idx++].path, buf);
		remove_last_file_from_path(buf);
	}
	return true;
}
