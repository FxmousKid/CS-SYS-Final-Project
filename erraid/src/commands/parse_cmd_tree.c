#include "commands/parse_cmd_tree.h"

bool	parse_sub_trees(struct s_cmd *cmd)
{
	struct s_cmd_sq	*cmd_sq = NULL;
	bool		ret_flag = true;

	if (!opendir_cmd(&cmd->cmd_dir, cmd->path))
		return false;
	
	if (!(cmd->cmd_type = get_cmd_type(cmd->path))) {
		return false;
	}

	switch (cmd->cmd_type) {
	case CMD_SI:
		ret_flag |= parse_cmd_si(cmd->path, cmd);
		break;

	case CMD_SQ:
		cmd_sq = &cmd->cmd.cmd_sq;
		if ((cmd_sq->nb_cmds = count_sub_cmds(cmd->path)) < 0)
			goto exit;
		if (!alloc_and_fill_sq_sub_dirs(cmd_sq, cmd->path))
			goto exit;
		for (int i = 0; i < cmd_sq->nb_cmds; i++)
			ret_flag |= parse_sub_trees(&cmd_sq->cmds[i]);
		break;

	default:
		return false;
	}
	return ret_flag;

exit:
	return false;
}
/* Main function for Parsing the cmd/ folder
 *
 * takes the path to the initial cmd/ under TASK_ID/
 * 
 * */
struct s_cmd	*parse_cmd_tree(const char *path_cmd_dir)
{
	struct s_cmd	*cmd;

	if (!(cmd = get_cmd_struct()))
		return NULL;

	strcpy(cmd->path, path_cmd_dir);
	if (!parse_sub_trees(cmd)) {
		free_command_rec(cmd);
		return NULL;
	}
	return cmd;
}
