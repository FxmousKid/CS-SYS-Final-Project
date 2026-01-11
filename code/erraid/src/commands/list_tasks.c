#include "commands/list_tasks.h"


/**
 * @brief Serializes the <arguments> structure for a simple command (CMD_SI).
 * Format: ARGC <uint32>, ARGV[0] <string>, ..., ARGV[ARGC-1] <string>
 *
 * @param buf 		Pointer to the serialization buffer.
 * @param command	The array of string arguments (char **command).
 * @return true on success, false otherwise.
 */
static bool serialize_arguments(struct s_buffer *buf, char **command)
{
    // Find ARGC
    uint32_t argc = 0;
    for (char **p = command; *p != NULL; p++) {
        argc++;
    }

    // ARGC <uint32>
    if (!buffer_append_uint32(buf, argc))
        return false;

    // Write each argument as a <string>
    for (uint32_t i = 0; i < argc; i++) {
        // buffer_append_string handles LENGTH <uint32> + DATA <string>
        if (!buffer_append_string(buf, command[i]))
            return false;
    }

    return true;
}

/**
 * @brief Recursively serializes the <command> structure for a task.
 *
 * @param buf 		Pointer to the serialization buffer.
 * @param cmd 		Pointer to the s_cmd structure.
 * @return true on success, false otherwise.
 */
static bool serialize_command(struct s_buffer *buf, const struct s_cmd *cmd)
{
	if (!cmd)
		return false;

	// TYPE <uint16>
	if (!buffer_append_uint16(buf, cmd->cmd_type))
		return false;
	// Simple Command: ARGS <arguments>
	if (cmd->cmd_type == CMD_SI) {
		return serialize_arguments(buf, cmd->cmd.cmd_si.command);
	}
	// Sequence Command: NBCMDS <uint32>, CMD[0] <command>, ...
	else if (cmd->cmd_type == CMD_SQ) {
		// NBCMDS <uint32>
		if (!buffer_append_uint32(buf, cmd->cmd.cmd_sq.nb_cmds))
			return false;

		// CMD[0] <command>, ..., CMD[N-1] <command>
		for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; i++) {
			// Recursive call for the sub-command
			if (!serialize_command(buf, &(cmd->cmd.cmd_sq.cmds[i])))
				return false;
		}
		return true;
	}
	else if(cmd->cmd_type == CMD_PL) {
		// NBCMDS <uint32>
		if (!buffer_append_uint32(buf, cmd->cmd.cmd_pl.nb_cmds))
			return false;

		// CMD[0] <command>, ..., CMD[N-1] <command>
		for (int i = 0; i < cmd->cmd.cmd_pl.nb_cmds; i++) {
			// Recursive call for the sub-command
			if (!serialize_command(buf, &(cmd->cmd.cmd_pl.cmds[i])))
				return false;
		}
		return true;
	}
	
	return true;
}

/**
 * @brief Serializes all tasks data into a binary buffer.
 *
 * @param ctx		Pointer to the daemon context structure.
 * @param buf		Pointer to the s_buffer structure..
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
		
		/* COMMANDLINE <command> */
		if (!serialize_command(buf, current_task->cmd))
			return false;

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