
#include "commands/combine_tasks.h"

static bool	create_cmd_dir_and_type(struct s_task *task, uint8_t *req)
{
	char		path_cmd[PATH_MAX + 1] = {0};
	char		path_type[PATH_MAX + 1] = {0};
	enum cmd_type	type;
	int		fd;

	build_safe_path(path_cmd, PATH_MAX, task->path, "cmd");
	build_safe_path(path_type, PATH_MAX, path_cmd, "type");
	mkdir(path_cmd, 0700);
	if ((fd = open(path_type, O_CREAT | O_WRONLY, 0644)) < 0) {
		ERR_SYS("open");
		return false;
	}
	memcpy(&type, req, 2);
	write(fd, &type, 2);
	close(fd);
	return true;
}

static void	reply_to_client_combine(struct s_data *ctx, taskid_t id, enum reply_opcode rep)
{
	struct s_buffer	reply_buf = {0};

	// Init buffer
	if (!buffer_init(&reply_buf, INITIAL_BUF_CAPACITY))
		return ;
	buffer_append_uint16(&reply_buf, rep);
	if (rep == OPCODE_ER) {
		buffer_append_uint16(&reply_buf, 0x4e46);
	} else {
		buffer_append_uint64(&reply_buf, id);
	}
	writefifo(ctx->fifo_reply, reply_buf.data, reply_buf.size);
	buffer_free(&reply_buf);
}

bool	combine_tasks(struct s_data *ctx, uint8_t *req)
{
	
	struct s_task	*task;
	taskid_t	id = get_taskid(ctx);
	char		tasks_path[PATH_MAX + 1] = {0};
	char		cmd_path[PATH_MAX + 1] = {0};

	if (!build_safe_path(tasks_path, sizeof(tasks_path), ctx->run_directory, TASKS_DIR)) {
		ERR_MSG("Failed to build tasks path %s", tasks_path);
		return false;
	}

	task = calloc(1, sizeof(struct s_task));
	if (task == NULL) {
		ERR_SYS("calloc");
		return false;
	}

	task->task_id = id;
	task->sub_cmds_count = 0;
	if (!snprintf(task->path, sizeof(task->path), "%s/%" PRId64, tasks_path, id)) {
		ERR_MSG("Failed to build tasks path %s", tasks_path);
		return false;
	}
	build_output_paths(task);
	create_directory(task);
	create_timing(task, req + 2);
	
	// Create cmd directory and type file
	create_cmd_dir_and_type(task, req + 15);
	
	// Move sub-commands from source tasks
	if (!move_all_cmd(ctx, task, req + 17)) {
		printf("removing : %s\n", task->path);
		recursive_rm(task->path);
		ctx->max_taskid--;
		reply_to_client_combine(ctx, id, OPCODE_ER);
		free(task);
		return false;
	}

	// Parse the complete cmd tree from disk
	build_safe_path(cmd_path, sizeof(cmd_path), task->path, "cmd");
	task->cmd = parse_cmd_tree(cmd_path);
	if (!task->cmd) {
		ERR_MSG("Failed to parse cmd tree");
		recursive_rm(task->path);
		ctx->max_taskid--;
		reply_to_client_combine(ctx, id, OPCODE_ER);
		free(task);
		return false;
	}

	// Set up stdout/stderr paths for the last command in the tree
	count_individual_cmds(task->cmd, &task->sub_cmds_count);
	set_output_paths_last_command(task->cmd,
				      task->sub_cmds_count - 1,
				      task->stdout_path,
				      task->stderr_path,
				      false);

	task->new_task = true;
	task->next = NULL;
	if (ctx->tasks)
		add_tasks(ctx->tasks, task);
	else
		ctx->tasks = task;
	reply_to_client_combine(ctx, id, OPCODE_OK);
	return true;
}
