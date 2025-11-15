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
	char path_to_stdout[PATH_MAX + 1];
	char path_to_stderr[PATH_MAX + 1];

	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;

	print_cmd_tree(ctx.tasks->cmd);
	

	/* Exec tests */
	printf("\nExecution\n");
	path_to_stdout[0] = '\0';
	strncat(path_to_stdout, ctx.tasks->path, PATH_MAX);
	strncat(path_to_stdout, "stdout", PATH_MAX - strlen(path_to_stdout));

	path_to_stderr[0] = '\0';
	strncat(path_to_stderr, ctx.tasks->path, PATH_MAX);
	strncat(path_to_stderr, "stderr", PATH_MAX - strlen(path_to_stderr));

	if (exec_cmd_with_redir(ctx.tasks->cmd, path_to_stdout, path_to_stderr)) {
		printf("Command executed successfully\n");
		printf("Exit code: %d\n", ctx.tasks->cmd->exit_code);
	} else {
		printf("Command execution failed\n");
	}

	/* End of exec tests */

	free_tasks(ctx.tasks);

	return EXIT_SUCCESS;
}
