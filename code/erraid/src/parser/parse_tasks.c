#include "parser/parse_tasks.h"

void	count_individual_cmds(struct s_cmd *cmd, int *count)
{
	switch (cmd->cmd_type) {
	case CMD_SI:
		*count = *count + 1;
		break;
	case CMD_SQ:
		// *count = *count + 1;
		for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; i++)
			count_individual_cmds(cmd->cmd.cmd_sq.cmds + i, count);
		break;
	case CMD_PL:
		// *count = *count + 1;
		for (int i = 0; i < cmd->cmd.cmd_pl.nb_cmds; i++)
			count_individual_cmds(cmd->cmd.cmd_pl.cmds + i, count);
		break;

	case CMD_IF:
		count_individual_cmds(cmd->cmd.cmd_if.conditional, count);
		count_individual_cmds(cmd->cmd.cmd_if.cmd_if_true, count);
		if (!cmd->cmd.cmd_if.cmd_if_false)
			return;
		count_individual_cmds(cmd->cmd.cmd_if.cmd_if_false, count);
		break;
	case CMD_ND:
		// *count = *count + 1;
		for (int i = 0; i < cmd->cmd.cmd_nd.nb_cmds; i++)
			count_individual_cmds(cmd->cmd.cmd_nd.cmds + i, count);
		break;
	case CMD_OR:
		// *count = *count + 1;
		for (int i = 0; i < cmd->cmd.cmd_or.nb_cmds; i++)
			count_individual_cmds(cmd->cmd.cmd_or.cmds + i, count);
		break;
	default:
		return;
	}
}

/**
 * @brief Recursively sets stdout/stderr paths for all commands in a tree.
 * 
 * For pipelines, only the last command gets the paths (others pipe their output).
 * For all other types (SQ, ND, OR, IF), all commands get the paths.
 * 
 * @param cmd The command to process
 * @param stdout_path Path to stdout file
 * @param stderr_path Path to stderr file  
 * @param is_last_in_pipeline True if this command is the last in its parent pipeline
 */
static void	set_output_paths_recursive(struct s_cmd *cmd,
					   const char *stdout_path,
					   const char *stderr_path,
					   bool is_last_in_pipeline)
{
	if (!cmd)
		return;

	if (cmd->cmd_type == CMD_SI) {
		// For CMD_SI, set paths only if we're the last in pipeline (or not in a pipeline)
		if (is_last_in_pipeline) {
			cmd->cmd.cmd_si.stdout_path = stdout_path;
			cmd->cmd.cmd_si.stderr_path = stderr_path;
		} else {
			cmd->cmd.cmd_si.stdout_path = NULL;
			cmd->cmd.cmd_si.stderr_path = NULL;
		}
		return;
	}

	switch (cmd->cmd_type) {
	case CMD_PL:
		// For pipelines: only the LAST command gets stdout, others get NULL
		for (int i = 0; i < cmd->cmd.cmd_pl.nb_cmds; i++) {
			bool is_last = (i == cmd->cmd.cmd_pl.nb_cmds - 1);
			// Pass is_last_in_pipeline only to the last command of the pipeline
			set_output_paths_recursive(cmd->cmd.cmd_pl.cmds + i, 
						   stdout_path, 
						   stderr_path,
						   is_last && is_last_in_pipeline);
		}
		break;

	case CMD_SQ:
		// For sequences: propagate is_last_in_pipeline to all children
		for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; i++) {
			set_output_paths_recursive(cmd->cmd.cmd_sq.cmds + i, 
						   stdout_path, 
						   stderr_path,
						   is_last_in_pipeline);
		}
		break;

	case CMD_ND:
		// For &&: propagate is_last_in_pipeline to all children
		for (int i = 0; i < cmd->cmd.cmd_nd.nb_cmds; i++) {
			set_output_paths_recursive(cmd->cmd.cmd_nd.cmds + i, 
						   stdout_path, 
						   stderr_path,
						   is_last_in_pipeline);
		}
		break;

	case CMD_OR:
		// For ||: propagate is_last_in_pipeline to all children
		for (int i = 0; i < cmd->cmd.cmd_or.nb_cmds; i++) {
			set_output_paths_recursive(cmd->cmd.cmd_or.cmds + i, 
						   stdout_path, 
						   stderr_path,
						   is_last_in_pipeline);
		}
		break;

	case CMD_IF:
		// For if: propagate is_last_in_pipeline to all branches
		set_output_paths_recursive(cmd->cmd.cmd_if.conditional,
						stdout_path, stderr_path, is_last_in_pipeline);
		set_output_paths_recursive(cmd->cmd.cmd_if.cmd_if_true,
					   	stdout_path, stderr_path, is_last_in_pipeline);
		if (cmd->cmd.cmd_if.cmd_if_false)
			set_output_paths_recursive(cmd->cmd.cmd_if.cmd_if_false,
						stdout_path, stderr_path, is_last_in_pipeline);
		break;

	default:
		break;
	}
}

// Wrapper for backward compatibility
void	set_output_paths_last_command(struct s_cmd *cmd,
				      int last_cmd_id,
				      const char *stdout_path,
				      const char *stderr_path,
				      bool is_inside_pipeline)
{
	(void)last_cmd_id;  // No longer used
	(void)is_inside_pipeline;  // No longer used
	set_output_paths_recursive(cmd, stdout_path, stderr_path, true);
}

static bool	parse_sub_tasks_cmd(struct s_task *task)
{
	char	buf[PATH_MAX + 1] = {0};

	while (task) {
		strcpy(buf, task->path);
		strcat(buf, CMD_DIR);
		if (!(task->cmd = parse_cmd_tree(buf)))
			return false;
		task->new_task = false;
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
void	build_output_paths(struct s_task *task)
{
	if (!build_safe_path(task->stdout_path, sizeof(task->stdout_path), task->path, STDOUT_FILE))
		ERR_MSG("Failed to build stdout path");
	if (!build_safe_path(task->stderr_path, sizeof(task->stderr_path), task->path, STDERR_FILE))
		ERR_MSG("Failed to build stderr path");
	if (!build_safe_path(task->texit_path, sizeof(task->texit_path), task->path, TEXIT_FILE))
		ERR_MSG("Failed to build times-exitcodes path");
}

/**
 * @brief Extract task's id from its path
 */
static taskid_t extract_task_id(const char *task_path)
{
	char		path_copy[PATH_MAX + 1];
	const char	*last_slash;
	const char	*task_id_ptr;

	strcpy(path_copy, task_path);
	remove_trailing_slash(path_copy);
	
	last_slash = strrchr(path_copy, '/');
	if (!last_slash)
		return 0;
	
	task_id_ptr = last_slash + 1;
	if (!*task_id_ptr)
		return 0;

	return (taskid_t)atol(task_id_ptr);
}

static bool	parse_sub_tasks_path(struct s_task *task, const char *path, bool debug, taskid_t *max_taskid)
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
		if (task->task_id > *max_taskid)
			*max_taskid = task->task_id;
		build_output_paths(task);
		remove_last_file_from_path(dir.path);
		if (!parse_timing(task, debug)) {
			ERR_MSG("parse_timing fail");
			return false;
		}
		task = task->next;
	}
	closedir_s_dir(&dir);
	return true;
}

static bool	set_task_dependencies(struct s_task *tasks)
{
	while (tasks) {
		count_individual_cmds(tasks->cmd, &tasks->sub_cmds_count);
		set_output_paths_last_command(tasks->cmd,
					      tasks->sub_cmds_count - 1,
					      tasks->stdout_path,
					      tasks->stderr_path,
					      false);
		tasks = tasks->next;
	}
	return true;
}

/* This is the main function that will start the parsing starting from the 
 * root of the given directory.
 * */
bool	parse_tasks(struct s_data *ctx)
{
	int		subtasks_count = 0;
	struct s_task	**task = NULL;
	char		tasks_path[PATH_MAX + 1] = {0};

	if (!build_safe_path(tasks_path, sizeof(tasks_path), ctx->run_directory, TASKS_DIR)) {
		ERR_MSG("Failed to build tasks path %s", tasks_path);
		return false;
	}

	// little hack, tasks have the same criteria
	// to be counted as a sub-command dir
	subtasks_count = count_sub_cmds(tasks_path);
	if (subtasks_count < 0)
		return false;

	ctx->nb_base_tasks = subtasks_count;

	// No tasks to parse, return early with success
	if (subtasks_count == 0)
		return true;

	task = &ctx->tasks;
	if (!alloc_ll_sub_tasks(task, subtasks_count))
		return false;

	// parsing actual sub commands by reading through the dir
	task = &ctx->tasks;
	ctx->max_taskid = 0;
	if (!parse_sub_tasks_path(*task, tasks_path, ctx->debug_mode, &ctx->max_taskid))
		return false;

	if (!parse_sub_tasks_cmd(*task))
		return false;

	if (!set_task_dependencies(*task))
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
	if (tasks->new_task)
		free(tasks);
}
