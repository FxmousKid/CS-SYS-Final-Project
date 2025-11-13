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
		// *task = tasks_allocated++;
		// (*task)->next = tasks_allocated;
		// task = &(*task)->next;
		*task = tasks_allocated++;
		if (subtasks_count)
			(*task)->next = tasks_allocated;
		task = &(*task)->next;
	}

	return true;
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
		task = task->next;
		remove_last_file_from_path(dir.path);
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

	// little hack, tasks have the same criteria 
	// to be counted as a sub-command dir
	subtasks_count = count_sub_cmds(ctx->run_directory);
	if (subtasks_count < 0)
		return false;

	task = &ctx->tasks;
	if (!alloc_ll_sub_tasks(task, subtasks_count))
		return false;

	// parsing actual sub commands by reading through the dir
	task = &ctx->tasks;
	if (!parse_sub_tasks_path(*task, ctx->run_directory))
		return false;

	if (!parse_sub_tasks_cmd(*task))
		return false;

	return true;
}

/* Frees the linked list of tasks */
void	free_tasks(struct s_task *tasks)
{
	while (tasks) {
		free_command_rec(tasks->cmd);
		tasks = tasks->next;
	}
	free(tasks);
}
