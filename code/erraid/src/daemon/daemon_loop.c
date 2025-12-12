# include "daemon/daemon_loop.h"

/**
 * @brief       appends the times-exitcodes file of the task with
 *              the current time and its exit code.
 */

static bool     append_texit(struct s_task *task)
{
        int		fd;
        uint16_t	exit_code;
        uint16_t	exit_code_be;
        int64_t		timestamp; // for serialization requirements
        int64_t		timestamp_be; // same here

        fd = open(task->texit_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) {
                ERR_SYS("open");
                return false;
        }

        timestamp = (int64_t)time(NULL);
        exit_code = task->cmd->exit_code;
        timestamp_be = htobe64(timestamp);
        exit_code_be = htobe16(exit_code);

        if (write(fd, &timestamp_be, 8) != 8) {
                ERR_SYS("write timestamp");
                close(fd);
                return false;
        }
        if (write(fd, &exit_code_be, 2) != 2) {
                ERR_SYS("write exit_code");
                close(fd);
                return false;
        }
        if (close(fd) < 0){
                ERR_SYS("close");
                return false;
        }
        return true;
}       

static int	check_date(struct s_task *task)
{
	time_t		now = time(NULL);
	struct tm	*t = localtime(&now);

	return (((task->timing.minutes >> t->tm_min) & 1) &&
		((task->timing.hours >> t->tm_hour) & 1) &&
		((task->timing.days >> t->tm_wday) & 1));
}

static void	exec_tasks_loop_debug(struct s_data *ctx)
{
	struct s_task *current_task;

	current_task = ctx->tasks;

	int		task_count = 0;
	taskid_t	task_id;
	printf("Execution\n");

	while (current_task){

		task_id = current_task->task_id;
		printf("\n Executing Task %" PRId64 "\n", task_id);
		printf("Path: %s\n", current_task->path);
		print_cmd_tree(current_task->cmd);
		printf("Output files:\n");
		printf("  stdout: %s\n", current_task->stdout_path);
		printf("  stderr: %s\n", current_task->stderr_path);
                printf("  times-exitcodes: %s\n", current_task->texit_path);

		if (check_date(current_task)) {
			if (exec_task(current_task)) {
				printf("Task %" PRId64 "executed successfully\n", task_id);
				printf("Exit code: %d\n", current_task->cmd->exit_code);
                                
                                // Append times-exitcodes file
                                if (append_texit(current_task))
                                        printf("append_texit succeeds\n");
                                else
                                        printf("append_texit failed\ttexit_path = %s", current_task->texit_path);
                        }
			else {
				printf("Task %" PRId64 "execution failed\n", task_id);
			}
		} else {
			printf("Task %" PRId64 "not yet executed", task_id);
		}
		
		task_count++;
		current_task = current_task->next;
	}
	printf("\nTotal: executed %d tasks\n", task_count);
}

static void     exec_tasks_loop(struct s_data *ctx)
{
	struct s_task *current_task;

	current_task = ctx->tasks;
	while (current_task) {
		if (check_date(current_task)) {
			exec_task(current_task);
                        append_texit(current_task);
		}
		current_task = current_task->next;
	}
}

#include <poll.h>

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

		handle_all_requests(ctx, pfds);
		// printf("no blocking\n");
		
		current_time = time(NULL);

		// Check every minute if a task can be executed
		if (current_time >= next_execution) {
			ctx->debug_mode ? exec_tasks_loop_debug(ctx) : exec_tasks_loop(ctx);
			next_execution = ((current_time/60) + 1) * 60;
		}
		// Wait 1 sec, maybe change it to 30s or 60s.
		sleep(1);
	}
}
