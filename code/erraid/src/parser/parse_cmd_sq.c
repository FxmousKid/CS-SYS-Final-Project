#include "parser/parse_cmd_sq.h"

bool	alloc_and_fill_sq_sub_dirs(struct s_cmd_sq *cmd_sq, const char *path)
{
	int	idx = 0;
	char	buf[PATH_MAX + 1] = {0};

	if (!(cmd_sq->cmds = calloc(cmd_sq->nb_cmds, sizeof(struct s_cmd)))) {
		ERR_SYS("calloc");
		return false;
	}

	strcpy(buf, path);
	while (idx < cmd_sq->nb_cmds) {
		if (buf[strlen(buf) - 1] != '/')
			buf[strlen(buf)] = '/'; 
		append_int_to_buf(buf, idx);
		strcpy(cmd_sq->cmds[idx++].path, buf);
		remove_last_file_from_path(buf);
	}
	return true;
}
