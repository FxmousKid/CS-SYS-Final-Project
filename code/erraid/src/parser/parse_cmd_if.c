#include "parser/parse_cmd_if.h"

static void	copy_next_sub_cmd_dir(char tmp[PATH_MAX + 1], int idx, char *dst)
{
	if (strlen(tmp) && tmp[strlen(tmp) - 1] != '/')
		tmp[strlen(tmp)] = '/'; 
	append_int_to_buf(tmp, idx);
	if (tmp == dst)
		return;
	strcpy(dst, tmp);
	remove_last_file_from_path(tmp);
}

bool	alloc_and_fill_if_sub_dirs(struct s_cmd_if *cmd_if, const char *path)
{
	int	idx = 0;
	char	buf[PATH_MAX + 1] = {0};
	DIR	*else_dir = NULL;

	if (!(cmd_if->conditional = calloc(1, sizeof(*cmd_if->conditional)))) {
		ERR_SYS("calloc")
		return false;
	}
	if (!(cmd_if->cmd_if_true = calloc(1, sizeof(*cmd_if->cmd_if_true)))) {
		ERR_SYS("calloc")
		return false;
	}

	bzero(buf, PATH_MAX + 1);
	strcpy(buf, path);
	copy_next_sub_cmd_dir(buf, idx++, cmd_if->conditional->path);
	copy_next_sub_cmd_dir(buf, idx++, cmd_if->cmd_if_true->path);
	
	// checking if else exist
	copy_next_sub_cmd_dir(buf, idx, buf);
	if (!opendir_cmd(&else_dir, buf)) {
		if (errno != ENOENT) {
			ERR_SYS("error checking if else command is set by opending dir")
			return false;
		};
		return true;
	}
	if (closedir(else_dir) < 0) {
		ERR_SYS("failed to close dir : %p", else_dir);
		return false;
	}

	if (!(cmd_if->cmd_if_false = calloc(1, sizeof(*cmd_if->cmd_if_false)))) {
		ERR_SYS("calloc")
		return false;
	}
	remove_last_file_from_path(buf);
	copy_next_sub_cmd_dir(buf, idx, cmd_if->cmd_if_false->path);
	
	return true;
}
