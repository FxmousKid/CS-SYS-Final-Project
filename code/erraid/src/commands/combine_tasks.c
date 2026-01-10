
#include "commands/combine_tasks.h"

static bool	create_cmd(struct s_task *task, uint8_t *req)
{
	char		path_type[PATH_MAX + 1] = {0};
	enum cmd_type	type;
	int		fd;

	task->cmd = get_cmd_struct();
	build_safe_path(task->cmd->path, PATH_MAX, task->path, "cmd");
	build_safe_path(path_type, PATH_MAX, task->cmd->path, "type");
	mkdir(task->cmd->path, 0700);
	if ((fd = open(path_type, O_CREAT | O_WRONLY, 0644)) < 0) {
		ERR_SYS("open");
		return false;
	}
	memcpy(&type, req, 2);
	type = htobe16(type);
	write(fd, &type, 2);
	task->cmd->cmd_type = type;
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
	if (!snprintf(task->path, sizeof(task->path), "%s/%lu", tasks_path, id)) {
		ERR_MSG("Failed to build tasks path %s", tasks_path);
		return false;
	}
	build_output_paths(task);
	create_directory(task);
	create_timing(task, req + 2);
	create_cmd(task, req + 15);
	if (!move_all_cmd(ctx, task, req + 17)) {
		printf("removing : %s\n", task->path);
		recursive_rm(task->path);
		ctx->max_taskid--;
		reply_to_client_combine(ctx, id, OPCODE_ER);
		return false;
	}

	task->cmd->cmd.cmd_si.stdout_path = task->stdout_path;
	task->cmd->cmd.cmd_si.stderr_path = task->stderr_path;
	task->next = NULL;
	//add_tasks(ctx->tasks, task);
	reply_to_client_combine(ctx, id, OPCODE_OK);
	return true;
}