#include "parser/parse_tree.h"

bool	parse_tree(struct s_data *ctx)
{
	struct s_cmd	cmd = {0};
	if (!parse_cmd(ctx, PATH_CMD, &cmd))
		return false;	
	free_darr(cmd.command);

	return true;
}
