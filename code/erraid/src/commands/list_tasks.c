#include "commands/list_tasks.h"

/**
 * @brief Serializes all tasks data into a binary buffer.
 *
 * @param ctx	Pointer to the daemon context structure.
 * @param buf	Pointer to the s_buffer structure used for dynamic serialization.
 * @return	true on successful serialization of all tasks,
 * 		false on any buffer write or command string reconstruction failure.
 */
static bool	serialize_task_list(struct s_data *ctx, struct s_buffer *buf)
{
	struct s_task	*current_task;
	uint32_t	nb_tasks = 0;
	
	// Count tasks for NBTASKS
	current_task = ctx->tasks;
	while (current_task) {
		nb_tasks++;
		current_task = current_task->next;
	}

	// Write header (ANSTYPE and NBTASKS)
	if (!buffer_append_uint16(buf, OPCODE_OK))
		return false;
	if (!buffer_append_uint32(buf, nb_tasks))
		return false;

	// Iterate over all tasks to serialize each one data
	current_task = ctx->tasks;
	while (current_task) {
		char	*cmd_str;

		/* TASKID (uint64 BE) */
		if (!buffer_append_uint64(buf, current_task->task_id))
			return false;

		/* TIMING: MINUTES (uint64 BE), HOURS (uint32 BE), DAYSOFWEEK (uint8) */
		if (!buffer_append_uint64(buf, current_task->timing.minutes))
			return false;
		if (!buffer_append_uint32(buf, current_task->timing.hours))
			return false;
		if (!buffer_append(buf, &(current_task->timing.days), sizeof(uint8_t)))
			return false;
		
		/* COMMANDLINE (string: LENGTH <uint32 BE> + DATA [bytes]) */
		cmd_str = cmd_to_string(current_task->cmd);
		if (!cmd_str)
			return false;
		
		if (!buffer_append_string(buf, cmd_str)) {
			free(cmd_str);
			return false;
		}
		free(cmd_str);
		current_task = current_task->next;
	}

	return true;
}

bool	list_tasks(struct s_data *ctx)
{
	struct s_buffer	reply_buf = {0};
	bool		success = false;

	// Init buffer
	if (!buffer_init(&reply_buf, INITIAL_BUF_CAPACITY))
		return false;

	// Serialize data from all tasks
	if (serialize_task_list(ctx, &reply_buf)) {
		// Write in fifo the reply
		if (writefifo(ctx->fifo_reply, reply_buf.data, reply_buf.size))
			success = true;
	}
	
	buffer_free(&reply_buf);
	return success;
}