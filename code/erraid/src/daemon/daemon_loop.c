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

	// Debug variables (remove them)
	int task_count;
	int task_id;
        task_count = 0;
        // Debug lines (remove them)
	printf("Execution\n");

	while (current_task){

		// Debug lines (remove them)
		task_id = current_task->task_id;
		printf("\n Executing Task %d\n", task_id);
		printf("Path: %s\n", current_task->path);
		print_cmd_tree(current_task->cmd);
		printf("Output files:\n");
		printf("  stdout: %s\n", current_task->stdout_path);
		printf("  stderr: %s\n", current_task->stderr_path);
                printf("  times-exitcodes: %s\n", current_task->texit_path);

                // Clean these if else later 
		if (check_date(current_task)) {
			if (exec_task(current_task)) {
				// Debug lines
				printf("Task %d executed successfully\n", task_id);
				printf("Exit code: %d\n", current_task->cmd->exit_code);
                                
                                // Append times-exitcodes file
                                if (append_texit(current_task))
                                        printf("append_texit succeeds\n");
                                else
                                        printf("append_texit failed\ttexit_path = %s", current_task->texit_path);
                        }
			else {
				// Debug lines
				printf("Task %d execution failed\n", task_id);
			}
		} else {
			printf("Task %d not yet executed", task_id);
		}
		
		// Debug line
		task_count++;
		
		current_task = current_task->next;
	}
	// Debug line
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


void    daemon_loop(struct s_data *ctx)
{
	time_t	last_execution ;
	time_t	current_time;

	last_execution = 0;

	setup_sig_handlers();

	while (is_daemon_running()) {

		current_time = time(NULL);

		// Check every minute if a task can be executed
		if (current_time - last_execution >= 60) {
			last_execution = current_time;
			ctx->debug_mode ? exec_tasks_loop_debug(ctx) : exec_tasks_loop(ctx);
		}
		// Wait 1 sec, maybe change it to 30s or 60s.
		sleep(1);
	}
}
