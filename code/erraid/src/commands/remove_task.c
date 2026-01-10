#include "commands/remove_task.h"

static bool	del_task_dir(struct s_data *ctx, uint64_t taskid)
{

	char taskid_dir [PATH_MAX + 1] = {0};
	char taskid_str [PATH_MAX + 1] = {0};

	// Build run_dir/tasks directory path
	if (!build_safe_path(taskid_dir, sizeof(taskid_dir), ctx->run_directory, TASKS_DIR)) {
		ERR_MSG("Failed to build tasks directory %s", taskid_dir);
		return false;
	}
	// Convert taskid (uint64) to a string
	snprintf(taskid_str, sizeof(taskid_str), "%" PRIu64, taskid);

	// Build run_dir/tasks/taskid directory path
	if (!build_safe_path(taskid_dir, sizeof(taskid_dir), taskid_dir, taskid_str)) {
		ERR_MSG("Failed to build taskid_dir directory %s", taskid_dir);
		return false;
	}
	// Delete taskid dir and all its files/repertory inside
	if (!recursive_rm(taskid_dir)) {
		ERR_MSG("Failed to delete directory %s", taskid_dir);
		return false;
	}
	return true;
}

bool	del_task_node(struct s_data *ctx, uint64_t taskid)
{
	struct s_task **indirect = &ctx->tasks;
	struct s_task *task;

	while (*indirect) {
		task = *indirect;

		if (task->task_id == taskid) {
			*indirect = task->next;
			free_command_rec(task->cmd);
			if (task->new_task)
				free(task);
			return true;
		}
		indirect = &task->next;
	}
	return false;
}

static bool	handle_reply(struct s_data *ctx, uint64_t taskid, struct s_buffer *buf)
{
	if (del_task_node(ctx, taskid) && del_task_dir(ctx, taskid)) {
		// Write ANSTYPE = 'OK' in fifo
		if (!buffer_append_uint16(buf, OPCODE_OK))
			return false;
		if (!writefifo(ctx->fifo_reply, buf->data, buf->size))
			return false;
	} else {
		// Write ANSTYPE = 'ER' and ERRCODE = 'NF' in fifo
		if (!buffer_append_uint16(buf, OPCODE_ER))
			return false;
		if (!buffer_append_uint16(buf, OPCODE_NF))
			return false;
		if (!writefifo(ctx->fifo_reply, buf->data, buf->size))
			return false;
	}
	return true;
}
bool	remove_task(struct s_data *ctx, uint64_t taskid)
{
	struct s_buffer reply_buf = {0};

	if (!buffer_init(&reply_buf, INITIAL_BUF_CAPACITY))
		return false;
	if (!handle_reply(ctx, taskid, &reply_buf))
		return false;
	buffer_free(&reply_buf);
	return true;
}