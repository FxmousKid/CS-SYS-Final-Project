# include "daemon/daemon_loop.h"

static void     exec_tasks_loop(struct s_data *ctx)
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

		if (exec_task(current_task)) {
			// Debug lines
			printf("Task %d executed successfully\n", task_id);
			printf("Exit code: %d\n", current_task->cmd->exit_code);
		} else {
			// Debug lines
			printf("Task %d execution failed\n", task_id);
		}
                // Debug line
                task_count++;
                
                current_task = current_task->next;
        }
        // Debug line
	printf("\nTotal: executed %d tasks\n", task_count);
}


void    daemon_loop(struct s_data *ctx)
{
        time_t	last_execution ;
	time_t	current_time;

        last_execution = 0;

        setup_sig_handlers();

        while (is_daemon_running()){

                current_time = time(NULL);

                // Check every minute if a task can be executed
                if (current_time - last_execution >= 60) {
			last_execution = current_time;

                        exec_tasks_loop(ctx);
                }
                // Wait 1 sec, maybe change it to 30s or 60s.
                sleep(1);
        }
}
