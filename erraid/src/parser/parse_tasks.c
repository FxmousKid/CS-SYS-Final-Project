#include "parser/parse_tasks.h"

static bool	parse_sub_tasks_cmd(struct s_task *task)
{
	char	buf[PATH_MAX + 1] = {0};

	while (task) {
		strcpy(buf, task->path);
		strcat(buf, CMD_DIR);
		if (!(task->cmd = parse_cmd_tree(buf)))	
			return false;
		task = task->next;
		bzero(buf, sizeof(buf));
	}
	return true;
}

static bool	alloc_ll_sub_tasks(struct s_task **task, int subtasks_count)
{
	struct s_task	*tasks_allocated = NULL;

	tasks_allocated = calloc(subtasks_count, sizeof(struct s_task));
	if (!tasks_allocated) {
		ERR_SYS("calloc");
		return false;
	}

	while (subtasks_count--) {
		*task = tasks_allocated++;
		if (subtasks_count)
			(*task)->next = tasks_allocated;
		task = &(*task)->next;
	}

	return true;
}

/**
 * @brief Builds complete paths to stdout and stderr files of a given task
 */
static void	build_output_paths(struct s_task *task)
{
	if (!build_safe_path(task->stdout_path, PATH_MAX + 1, task->path, STDOUT_FILE)) 
		ERR_MSG("Failed to build stdout path");
	if (!build_safe_path(task->stderr_path, PATH_MAX + 1, task->path, STDERR_FILE))
		ERR_MSG("Failed to build stderr path");
}

/**
 * @brief Extract task's id from its path
 */
static int extract_task_id(const char *task_path)
{
	char		path_copy[PATH_MAX + 1];
	const char	*last_slash;
	const char	*task_id_ptr;	

	strcpy(path_copy, task_path);
	remove_trailing_slash(path_copy);
	
	last_slash = strrchr(path_copy, '/');
	if (!last_slash)
		return -1;
	
	task_id_ptr = last_slash + 1;
	if (!*task_id_ptr)
		return -1;

	return atoi(task_id_ptr);
}

static bool	parse_sub_tasks_path(struct s_task *task, const char *path)
{
	struct dirent	*ent = NULL;
	struct stat	st = {0};
	struct s_dir	dir = {0};


	strcpy(dir.path, path);
	if (!opendir_cmd(&dir.dir, path))
		return false;

	errno = 0;
	while ((readdir_s_dir(&dir, &ent))) {
		if (errno) {
			closedir_s_dir(&dir);
			return false;
		}

		if (stat(dir.path, &st)) {
			ERR_SYS("stat");
			closedir(dir.dir);
			return false;
		}

		if (!is_ent_sub_dir(ent, &st)) {
			remove_last_file_from_path(dir.path);
			continue;
		}

		strcpy(task->path, dir.path);
		strcat(task->path, "/");

		task->task_id = extract_task_id(dir.path);
		build_output_paths(task);
		remove_last_file_from_path(dir.path);
		task = task->next;
	}
	closedir_s_dir(&dir);
	return true;
}

/* This is the main function that will start the parsing starting from the 
 * root of the given directory, so "xxx/tasks/"
 * */
bool	parse_tasks(struct s_data *ctx)
{
	int		subtasks_count = 0;
	struct s_task	**task = NULL;
	char		tasks_path[PATH_MAX + 1] = {0};

	// little hack, tasks have the same criteria 
	// to be counted as a sub-command dir
	strcpy(tasks_path, ctx->run_directory);
	strcat(tasks_path, TASKS_DIR);
	subtasks_count = count_sub_cmds(tasks_path);
	if (subtasks_count < 0)
		return false;

	task = &ctx->tasks;
	if (!alloc_ll_sub_tasks(task, subtasks_count))
		return false;

	// parsing actual sub commands by reading through the dir
	task = &ctx->tasks;
	if (!parse_sub_tasks_path(*task, tasks_path))
		return false;

	if (!parse_sub_tasks_cmd(*task))
		return false;

	return true;
}

/* Frees the linked list of tasks */
void	free_tasks(struct s_task *tasks)
{
	if (!tasks)
		return;
	free_command_rec(tasks->cmd);
	free_tasks(tasks->next);
}
