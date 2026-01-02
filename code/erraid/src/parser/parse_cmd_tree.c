#include "parser/parse_cmd_tree.h"

static bool	parse_sub_trees(struct s_cmd *cmd, int *cmd_count)
{
	struct s_cmd_sq	*cmd_sq = NULL;
	struct s_cmd_pl	*cmd_pl = NULL;
	struct s_cmd_if	*cmd_if = NULL;
	bool		ret_flag = true;

	if (!(cmd->cmd_type = get_cmd_type(cmd->path)))
		return false;

	if (!opendir_cmd(&cmd->cmd_dir, cmd->path))
		return false;
	

	switch (cmd->cmd_type) {
	case CMD_SI:
		cmd->cmd_id = *cmd_count;
		*cmd_count = *cmd_count + 1;
		ret_flag = parse_cmd_si(cmd->path, cmd);
		break;

	case CMD_SQ:
		cmd_sq = &cmd->cmd.cmd_sq;
		if ((cmd_sq->nb_cmds = count_sub_cmds(cmd->path)) < 0)
			goto exit;
		if (!alloc_and_fill_sq_sub_dirs(cmd_sq, cmd->path))
			goto exit;
		for (int i = 0; i < cmd_sq->nb_cmds; i++)
			ret_flag &= parse_sub_trees(&cmd_sq->cmds[i], cmd_count);
		break;

	case CMD_PL:
		cmd_pl = &cmd->cmd.cmd_pl;
		if ((cmd_pl->nb_cmds = count_sub_cmds(cmd->path)) < 0)
			goto exit;
		if (!alloc_and_fill_pl_sub_dirs(cmd_pl, cmd->path))
			goto exit;
		if (!alloc_pipe_fds(cmd_pl))
			goto exit;
		for (int i = 0; i < cmd_pl->nb_cmds; i++)
			ret_flag &= parse_sub_trees(&cmd_pl->cmds[i], cmd_count);
		break;
	
	case CMD_IF:
		cmd_if = &cmd->cmd.cmd_if;
		if (!alloc_and_fill_if_sub_dirs(cmd_if, cmd->path))
			goto exit;
		exit(12);
		break;


	default:
		ERR_MSG("Unknown command type at parsing")
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
 */
struct s_cmd	*parse_cmd_tree(const char *path_cmd_dir)
{
	struct s_cmd	*cmd;
	int		cmd_id = 0;

	if (!(cmd = get_cmd_struct()))
		return NULL;

	strcpy(cmd->path, path_cmd_dir);
	if (!parse_sub_trees(cmd, &cmd_id)) {
		free_command_rec(cmd);
		return NULL;
	}
	return cmd;
}
