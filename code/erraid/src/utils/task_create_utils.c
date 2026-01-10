
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

	if (build_safe_path(path_timing, PATH_MAX, task->path, "timing"))
		ERR_MSG("Failed to build stdout path");
	if ((fd = open(path_timing, O_CREAT | O_WRONLY, 0644)) < 0) {
		ERR_SYS("open");
		return false;
	}
	memcpy(&task->timing.minutes, req, 8);
	memcpy(&task->timing.hours, req + 8, 4);
	// task->timing.minutes = htobe64(task->timing.minutes);
	// task->timing.hours = htobe64(task->timing.minutes);
	task->timing.days = (uint8_t)req[12];

	write(fd, req, 13);
	close(fd);

	// print_timing(task->timing);
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