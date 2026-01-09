
#include "commands/create_tasks.h"

static taskid_t	get_taskid(struct s_task *tasks)
{
	taskid_t	id = 0;
	struct	s_task	*first = tasks;

	while (tasks) {
		if (tasks->task_id == id) {
			id++;
			tasks = first;
		} else {
			tasks = tasks->next;
		}
	}
	return id;
}

static void	create_directory(struct s_task *task)
{
	char		path[PATH_MAX + 1] = {0};
	struct stat	st = {0};
	int		fd;

	if (stat(task->path, &st) == -1)
    		mkdir(task->path, 0700);
	if (build_safe_path(path, PATH_MAX + 1, task->path, "times-exitcodes"))
		ERR_MSG("failed to build times-exitcodes path");

	if ((fd = open(path, O_CREAT, 0644)) < 0)
		ERR_SYS("open");
	close(fd);
}

static bool	create_timing(struct s_task *task, struct s_request *req)
{
	char		path_timing[PATH_MAX + 1] = {0};
	int		fd;

	if (build_safe_path(path_timing, PATH_MAX, task->path, "timing"))
		ERR_MSG("Failed to build stdout path");
	if ((fd = open(path_timing, O_CREAT | O_WRONLY, 0644)) < 0) {
		ERR_SYS("open");
		return false;
	}
	memcpy(&task->timing.minutes, req->buf + 2, 8);
	memcpy(&task->timing.hours, req->buf + 10, 4);
	// task->timing.minutes = htobe64(task->timing.minutes);
	// task->timing.hours = htobe64(task->timing.minutes);
	task->timing.days = (uint8_t)req->buf[14];

	write(fd, req->buf + 2, 13);
	close(fd);

	// print_timing(task->timing);
	return true;
}

static bool	parse_argv(int fd, uint8_t *req)
{
	uint32_t	nb_arg = 0;
	uint32_t	len_str = 0;
	size_t		len = 0;


	memcpy(&nb_arg, req, 4);
	len += 4;
	nb_arg = htobe32(nb_arg);
	for (uint32_t i = 0; i < nb_arg; i++) {
		memcpy(&len_str, req + len, 4);
		len_str = htobe32(len_str);
		len += 4 + len_str;
	}
	write(fd, req, len);
	return true;
}

static bool	create_cmd(struct s_task *task, struct s_request *req)
{
	char	path_argv[PATH_MAX + 1] = {0};
	char	path_type[PATH_MAX + 1] = {0};
	int	fd1;
	int	fd2;


	task->cmd = get_cmd_struct();

	build_safe_path(task->cmd->path, PATH_MAX, task->path, "cmd");
	build_safe_path(path_argv, PATH_MAX, task->cmd->path, "argv");
	build_safe_path(path_type, PATH_MAX, task->cmd->path, "type");

	mkdir(task->cmd->path, 0700);

	if ((fd1 = open(path_argv, O_CREAT | O_WRONLY | O_APPEND, 0644)) < 0
	||  (fd2 = open(path_type, O_CREAT | O_WRONLY, 0644)) < 0) {
		ERR_SYS("open");
		return false;
	}
	write(fd2, "SI", 2);
	task->cmd->cmd_type = CMD_SI;
	close(fd2);
	parse_argv(fd1, req->buf + 15);
	return true;
}

void	add_tasks(struct s_task *tasks, struct s_task *new)
{
	while (tasks->next) {
		tasks = tasks->next;
	}
	tasks->next = new;
}

static void	reply_to_client_create(struct s_data *ctx, taskid_t id)
{
	struct s_buffer	reply_buf = {0};

	// Init buffer
	if (!buffer_init(&reply_buf, INITIAL_BUF_CAPACITY))
		return ;
	buffer_append_uint16(&reply_buf, 0x4f4b);
	buffer_append_uint64(&reply_buf, id);
	writefifo(ctx->fifo_reply, reply_buf.data, reply_buf.size);
	buffer_free(&reply_buf);
}

bool	create_tasks(struct s_data *ctx, struct s_request *req)
{
	
	struct s_task	*task;
	taskid_t	id = get_taskid(ctx->tasks);
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
	create_timing(task, req);
	create_cmd(task, req);
	parse_cmd_si(task->cmd->path, task->cmd);
	task->cmd->cmd.cmd_si.stdout_path = task->stdout_path;
	task->cmd->cmd.cmd_si.stderr_path = task->stderr_path;
	task->next = NULL;
	add_tasks(ctx->tasks, task);
	reply_to_client_create(ctx, id);
	return true;
}