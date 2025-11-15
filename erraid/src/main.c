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

/**
 * @brief Builds complete paths to stdout and stderr files of a given task
 *
 * @note a deplacer plus tard
 */
static void build_output_paths(const char *task_path, char *stdout_path, char *stderr_path)
{
	char	clean_path[PATH_MAX + 1];
	size_t	current_len;

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

void	exec_test(struct s_data *ctx)
{
	char		stdout_path[PATH_MAX + 1] = {0};
	char		stderr_path[PATH_MAX + 1] = {0};
	int		task_id = 0;
	struct s_task	*current_task = NULL;

	current_task = ctx->tasks;
	while (current_task) {
		task_id = extract_task_id(current_task->path);
		build_output_paths(current_task->path, stdout_path, stderr_path);
		if (!exec_cmd_with_redir(current_task->cmd, stdout_path, stderr_path))
			printf("Task %d execution failed\n", task_id);
		current_task = current_task->next;
	}

	stdout_path[0] = '\0';
	strncat(stdout_path, ctx->tasks->path, PATH_MAX);
	strncat(stdout_path, STDOUT_FILE, PATH_MAX - strlen(stdout_path));

	stderr_path[0] = '\0';
	strncat(stderr_path, ctx->tasks->path, PATH_MAX);
	strncat(stderr_path, STDERR_FILE, PATH_MAX - strlen(stderr_path));

	if (!exec_cmd_with_redir(ctx->tasks->cmd, stdout_path, stderr_path)) 
		printf("Command execution failed\n");
}


int main(int argc, char *argv[])
{
	struct s_data	ctx = {0};

	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;

	exec_test(&ctx);

	/* End of exec tests */
	free_tasks(ctx.tasks);

	return EXIT_SUCCESS;
}
