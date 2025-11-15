#include <stdlib.h>

#include "parser/parse_cli.h"
#include "utils/utils.h" // IWYU pragma: keep
#include "parser/parse_tasks.h"
#include "structs.h"
#include "exec/exec.h"


// if passed data is made on little-endian architecture
// set by parser()
bool	isdle;



// /**
//  * @brief Builds complete paths to stdout and stderr files of a given task
//  */
// static void build_output_paths(const char *task_path, char *stdout_path, char *stderr_path)
// {
// 	if (!build_safe_path(stdout_path, PATH_MAX+1, task_path, STDOUT_FILE))
// 		ERR_MSG("Failed to build stdout path");
// 	if (!build_safe_path(stderr_path, PATH_MAX+1, task_path, STDERR_FILE))
// 		ERR_MSG("Failed to build stderr path");
// }

void	exec_test(struct s_data *ctx)
{
	struct s_task *current_task;
	int task_count;
	int task_id;

	// Debug lines
	printf("Found %d tasks\n", count_sub_cmds(ctx->run_directory));
	printf("Execution\n");
	
	current_task = ctx->tasks;
	task_count = 0;
	
	while (current_task) {
		
		task_id = current_task->task_id;

		// Debug lines 
		printf("\n Executing Task %d\n", task_id);
		printf("Path: %s\n", current_task->path);
		print_cmd_tree(current_task->cmd);
		
		// build_output_paths(current_task->path, path_to_stdout, path_to_stderr);
		printf("Output files:\n");
		printf("  stdout: %s\n", current_task->stdout_path);
		printf("  stderr: %s\n", current_task->stderr_path);

		if (exec_cmd_with_redir(current_task->cmd, current_task->stdout_path, current_task->stderr_path)) {
			// Debug lines
			printf("Task %d executed successfully\n", task_id);
			printf("Exit code: %d\n", current_task->cmd->exit_code);
		} else {
			// Debug lines
			printf("Task %d execution failed\n", task_id);
		}
		
		current_task = current_task->next;
		task_count++;
	}
	// Debug lines
	printf("\nTotal: executed %d tasks\n", task_count);
}


int main(int argc, char *argv[])
{
	struct s_data	ctx = {0};
	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;

	exec_test(&ctx);

	free_tasks(ctx.tasks);
	return EXIT_SUCCESS;
}
