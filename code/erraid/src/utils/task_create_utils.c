
#include "utils/task_create_utils.h"

void	add_tasks(struct s_task *tasks, struct s_task *new)
{
	while (tasks->next) {
		tasks = tasks->next;
	}
	tasks->next = new;
}

bool	create_timing(struct s_task *task, uint8_t *req)
{
	char		path_timing[PATH_MAX + 1] = {0};
	int		fd;
	uint64_t	minutes_be;
	uint32_t	hours_be;

	if (build_safe_path(path_timing, PATH_MAX, task->path, "timing"))
		ERR_MSG("Failed to build stdout path");
	if ((fd = open(path_timing, O_CREAT | O_WRONLY, 0644)) < 0) {
		ERR_SYS("open");
		return false;
	}

	// Data from client is in Big Endian, convert to host endian for in-memory use
	memcpy(&minutes_be, req, 8);
	memcpy(&hours_be, req + 8, 4);
	task->timing.minutes = be64toh(minutes_be);
	task->timing.hours = be32toh(hours_be);
	task->timing.days = (uint8_t)req[12];

	// Write the raw BE data to disk
	write(fd, req, 13);
	close(fd);

	return true;
}

taskid_t	get_taskid(struct s_data *ctx)
{
	taskid_t	id = ctx->max_taskid + 1;
	ctx->max_taskid = id;
	return id;
}

void	create_directory(struct s_task *task)
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