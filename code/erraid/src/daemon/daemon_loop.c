# include "daemon/daemon_loop.h"

/**
 * @brief Writes the timestamp <int_64> + exit_code <uint16> entry in times-exitcodes file.
 *
 * @param task			Pointer to the task structure.
 * @param exit_code		The collected exit code.
 * @return true on success, false on failure.
 */
static bool log_texit_entry(struct s_task *task, uint16_t exit_code)
{
	int		fd;
	int64_t		timestamp_be;
	uint16_t	exit_code_be;

	fd = open(task->texit_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0) {
		ERR_SYS("open times-exitcodes file");
		return false;
	}

	timestamp_be = htobe64(task->launch_time);
	exit_code_be = htobe16(exit_code);

	// Write timestamp <int 64>
	if (write(fd, &timestamp_be, sizeof(timestamp_be)) != sizeof(timestamp_be)) {
		ERR_SYS("write timestamp");
		close(fd);
		return false;
	}
	// Write exitcode <uint16>
	if (write(fd, &exit_code_be, sizeof(exit_code_be)) != sizeof(exit_code_be)) {
		ERR_SYS("write exit_code");
		close(fd);
		return false;
	}
	
	if (close(fd) < 0) {
		ERR_SYS("close times-exitcode");
		return false;
	}
	return true;
}

/**
 * @brief Harvests finished child processes (zombies) and logs times-exitcodes.
 */
static void handle_zombies(struct s_data *ctx)
{
	struct s_task 	*current_task;
	int		status;
	pid_t		pid;
	uint16_t	exit_code;

	current_task = ctx->tasks;
	while (current_task) {
		// Only check for launched tasks (PID > 0)
		if (current_task->cmd->pid > 0) {
			// Check child process status
			pid = waitpid(current_task->cmd->pid, &status, WNOHANG);
			
			if (pid > 0) {
				// Child process is finished
				if (WIFEXITED(status))
					exit_code = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					// Finished by a signal
					exit_code = 0xFF;
				else
					continue;

				// Log the time and the exit_code of the task
				if (log_texit_entry(current_task, exit_code)) {
					current_task->cmd->pid = 0;
					current_task->cmd->exit_code = exit_code;
					current_task->launch_time = 0;
				}
			}
		}
		current_task = current_task->next;
	}
}

static int	check_date(struct s_task *task)
{
	time_t		now = time(NULL);
	struct tm	*t = localtime(&now);

	return (((task->timing.minutes >> t->tm_min) & 1) &&
		((task->timing.hours >> t->tm_hour) & 1) &&
		((task->timing.days >> t->tm_wday) & 1));
}

static void	exec_tasks_loop_debug(struct s_data *ctx, bool instant)
{
	struct s_task 	*current_task;
	taskid_t	task_id;
	int		task_count;

	current_task = ctx->tasks;
	task_count = 0;
	printf("Execution\n");

	while (current_task){

		task_id = current_task->task_id;
		printf("\nExecuting Task %" PRId64 "\n", task_id);
		printf("Path: %s\n", current_task->path);
		// print_cmd_tree(current_task->cmd);
		// printf("Output files:\n");
		// printf("stdout: %s\n", current_task->stdout_path);
		// printf("stderr: %s\n", current_task->stderr_path);
                // printf("times-exitcodes: %s\n", current_task->texit_path);

		if (check_date(current_task) || instant) {
			current_task->launch_time = (int64_t)time(NULL);
			exec_task(current_task);
			printf("Task %" PRId64 " launched successfully\n", task_id);
		} else {
			printf("Task %" PRId64 " not yet executed\n", task_id);
		}
		
		task_count++;
		current_task = current_task->next;
	}
	printf("\n\tTotal: %d tasks\n", task_count);
}

static void     exec_tasks_loop(struct s_data *ctx, bool instant)
{
	struct s_task *current_task;

	current_task = ctx->tasks;
	while (current_task) {
		if (check_date(current_task) || instant) {
			current_task->launch_time = (int64_t)time(NULL);
			exec_task(current_task);
		}
		current_task = current_task->next;
	}
}

void    daemon_loop(struct s_data *ctx)
{
	struct pollfd	pfds[1] = {0};
	time_t		next_execution ;
	time_t		current_time;
	time_t		now;


	pfds[0].fd = open(ctx->fifo_request, O_RDONLY | O_NONBLOCK);
	pfds[0].events = POLLIN;
	if (pfds[0].fd < 0) {
		ERR_SYS("open fifo_request: %s", ctx->fifo_request);
		return ;
	}

	setup_sig_handlers();

	now = time(NULL);
	// Get rid of seconds (to start on 00 seconds)
	next_execution = ((now / 60) + 1) * 60;
	
	while (is_daemon_running()) {

		handle_zombies(ctx);
		handle_all_requests(ctx, pfds);

		current_time = time(NULL);
		// Check every minute if a task can be executed
		if (ctx->exec_instant || current_time >= next_execution) {
			ctx->debug_mode ? exec_tasks_loop_debug(ctx, true) : exec_tasks_loop(ctx, true);
			next_execution = ((current_time/60) + 1) * 60;
		}
		// Wait 1 sec, maybe change it to 30s or 60s.
		sleep(1);
	}
}
