#include <stdlib.h>

#include "parser/parse_cli.h"
#include "commands/parse_cmd_tree.h"
#include "utils/utils.h" // IWYU pragma: keep
#include "parser/parse_tasks.h"
#include "structs.h"
#include "exec/exec.h"
#include "macros.h"


// if passed data is made on little-endian architecture
// set by parser()
bool	isdle;



/* 

	A déplacer c'est juste pour tester la liste chainée de tasks rapidos

*/
/**
 * @brief Builds complete paths to stdout and stderr files of a given task
 */
static void build_output_paths(const char *task_path, char *stdout_path, char *stderr_path)
{
	char clean_path[PATH_MAX + 1];
	size_t current_len;

	// Init a clean path as a copy of task_path
	strncpy(clean_path, task_path, PATH_MAX);
	clean_path[PATH_MAX] = '\0';
	// Avoid double slash path like ".../15//stdout"
	remove_trailing_slash(clean_path);
	
	// Stdout path
	strncpy(stdout_path, clean_path, PATH_MAX);
	stdout_path[PATH_MAX] = '\0';
	current_len = strlen(stdout_path);
	if (current_len < PATH_MAX - 1) 
		strncat(stdout_path, "/", PATH_MAX - current_len);
	
	current_len = strlen(stdout_path);
	if (current_len < PATH_MAX) 
		strncat(stdout_path, STDOUT_FILE, PATH_MAX - current_len);
	

	// Stderr path
	strncpy(stderr_path, clean_path, PATH_MAX);
	stderr_path[PATH_MAX] = '\0';
	
	current_len = strlen(stderr_path);
	if (current_len < PATH_MAX - 1) 
		strncat(stderr_path, "/", PATH_MAX - current_len);
	
	current_len = strlen(stderr_path);
	if (current_len < PATH_MAX) 
		strncat(stderr_path, STDERR_FILE, PATH_MAX - current_len);
	
}

/**
 * @brief Extract task's id from its path
 */
static int extract_task_id(const char *task_path)
{
	const char *last_slash;
	const char *id_str;	
	char path_copy[PATH_MAX + 1];

	strcpy(path_copy, task_path);
	remove_trailing_slash(path_copy);
	
	last_slash = strrchr(path_copy, '/');
	if (!last_slash)
		return -1;
	
	id_str = last_slash + 1;
	if (*id_str == '\0')
		return -1;
	
	return atoi(id_str);
}












int main(int argc, char *argv[])
{
	struct s_data	ctx = {0};
	struct s_task *current_task;
	int task_count;
	char path_to_stdout[PATH_MAX + 1];
	char path_to_stderr[PATH_MAX + 1];
	int task_id;

	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;


	// Debug lines
	printf("Found %d tasks\n", count_sub_cmds(ctx.run_directory));
	printf("Execution\n");
	
	current_task = ctx.tasks;
	task_count = 0;
	
	while (current_task) {
		
		task_id = extract_task_id(current_task->path);

		// Debug lines 
		printf("\n Executing Task %d\n", task_id);
		printf("Path: %s\n", current_task->path);
		print_cmd_tree(current_task->cmd);
		
		build_output_paths(current_task->path, path_to_stdout, path_to_stderr);
		printf("Output files:\n");
		printf("  stdout: %s\n", path_to_stdout);
		printf("  stderr: %s\n", path_to_stderr);

		if (exec_cmd_with_redir(current_task->cmd, path_to_stdout, path_to_stderr)) {
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

	free_tasks(ctx.tasks);

	return EXIT_SUCCESS;
}
