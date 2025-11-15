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

void	exec_test(struct s_task *tasks)
{
	char stdout_path[PATH_MAX + 1];
	char stderr_path[PATH_MAX + 1];

	printf("\nExecution\n");
	stdout_path[0] = '\0';
	strncat(stdout_path, tasks->path, PATH_MAX);
	strncat(stdout_path, STDOUT_FILE, PATH_MAX - strlen(stdout_path));

	stderr_path[0] = '\0';
	strncat(stderr_path, tasks->path, PATH_MAX);
	strncat(stderr_path, STDERR_FILE, PATH_MAX - strlen(stderr_path));

	if (exec_cmd_with_redir(tasks->cmd, stdout_path, stderr_path)) {
		printf("Command executed successfully\n");
		printf("Exit code: %d\n", tasks->cmd->exit_code);
	} else {
		printf("Command execution failed\n");
	}
}

int main(int argc, char *argv[])
{
	struct s_data	ctx = {0};
	// struct s_task	*task = NULL;

	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;

	print_cmd_tree(ctx.tasks->cmd);

	/* Exec tests */
	exec_test(ctx.tasks);

	/* End of exec tests */
	free_tasks(ctx.tasks);

	return EXIT_SUCCESS;
}
