#include <stdlib.h>

#include "parser/parse_cli.h"
#include "commands/parse_cmd_tree.h"
#include "utils/utils.h" // IWYU pragma: keep
#include "structs.h"

// if passed data is made on little-endian architecture
// set by parser()
bool	isdle;

int main(int argc, char *argv[])
{
	struct s_data ctx = {0};
	struct s_cmd *cmd = NULL;

	parser(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	cmd = parse_cmd_tree(ctx.run_directory);
	if (!cmd)
		return EXIT_FAILURE;
	
	print_cmd_tree(cmd);
	free_command_rec(cmd);

	return EXIT_SUCCESS;
}
