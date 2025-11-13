#include <stdlib.h>

#include "parser/parse_cli.h"
#include "commands/parse_cmd_tree.h"
#include "utils/utils.h" // IWYU pragma: keep
#include "parser/parse_tasks.h"
#include "structs.h"
#include "exec/exec.h"

// if passed data is made on little-endian architecture
// set by parser()
bool	isdle;

int main(int argc, char *argv[])
{
	struct s_data	ctx = {0};
	// struct s_task	*task = NULL;

	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;

	print_cmd_tree(ctx.tasks->cmd);

	printf("\nExecution\n");
	if (execute_command(ctx.tasks->cmd)) {
		printf("Command executed successfully\n");
		printf("Exit code: %d\n", ctx.tasks->cmd->exit_code);
	} else {
		printf("Command execution failed\n");
	}

	free_tasks(ctx.tasks);

	return EXIT_SUCCESS;
}
