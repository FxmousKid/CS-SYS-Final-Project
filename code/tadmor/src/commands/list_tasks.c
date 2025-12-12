#include "commands/list_tasks.h"

/**
 * @brief Reads a protocol-compliant string from the FIFO.
 * The string format is LENGTH <uint32_t> followed by DATA <LENGTH bytes>.
 *
 * @param fd 	The file descriptor to read from.
 * @param str 	Pointer to a char* where the string will be stored.
 * @return true on success, false on read or allocation failure.
 */
static bool	read_protocol_string(int fd, char **str)
{
	uint32_t	len;
	
	// Read the length (uint32)
	if (!read_uint32(fd, &len))
		return false;
	
	// Allocate buffer (len + 1 for '\0')
	if (!(*str = malloc(len + 1))) {
		ERR_SYS("malloc");
		return false;
	}

	// Read data (len bytes)
	if (!read_exact(fd, *str, len)) {
		free(*str);
		*str = NULL;
		return false;
	}
	(*str)[len] = '\0'; // Null-terminate the string
	return true;
}

/**
 * @brief Sends the LIST request (OPCODE_LS) to the daemon via the request FIFO.
 * The OPCODE is sent in BE format as required by the protocol.
 *
 * @param fifo_request	The path to the fifo request.
 * @return true on successful write, false otherwise.
 */
static bool	request_list_tasks(char *fifo_request)
{
	// Send OPCODE_LS (0x4c53) in BE
	uint16_t opcode;
	opcode = htobe16(OPCODE_LS);
	if (!writefifo(fifo_request, &opcode, sizeof(opcode)))
		return false;
	return true;
}

/**
 * @brief Handles the response to the LIST command,
 * deserializes task data, sorts it, and prints the result.
 *
 * @param ctx	Pointer to the daemon context structure.
 * @return 	true on successful read and display,
 * 		false on error or if the daemon returns an error.
 */
static bool	handle_list_tasks_reply(struct s_data *ctx)
{
	int		fd_reply;
	uint16_t	ans_type;
	uint32_t	nb_tasks;
	uint64_t	task_id;
	struct s_timing	timing;
	char		*cmd_str;
	char		*timing_str;
	
	if ((fd_reply = open(ctx->fifo_reply, O_RDONLY)) < 0) {
		ERR_SYS("open fifo_reply: %s", ctx->fifo_reply);
		return false;
	}

	// Read ANSTYPE (uint16)
	if (!read_uint16(fd_reply, &ans_type))
		goto error;

	if (ans_type == OPCODE_ER)
		goto error;

	// Read NBTASKS (uint32)
	if (!read_uint32(fd_reply, &nb_tasks))
		goto error;
	
	// Iterate over all tasks
	for (uint32_t i = 0; i < nb_tasks; i++) {
		cmd_str = NULL;
		timing_str = NULL;

		// Read TASKID (uint64)
		if (!read_uint64(fd_reply, &task_id))
			goto error;
		
		/* Read TIMING */

		// MINUTES (uint64)
		if (!read_uint64(fd_reply, &timing.minutes))
			goto error;
		// HOURS (uint32)
		if (!read_uint32(fd_reply, &timing.hours))
			goto error;
		// DAYSOFWEEK (uint8)
		if (!read_exact(fd_reply, &timing.days, sizeof(uint8_t)))
			goto error;


		/* Read COMMANDLINE (string) */
		if (!read_protocol_string(fd_reply, &cmd_str))
			goto error;

		// Convert timing to a formatted string
		timing_str = timing_to_string(&timing);
		if (!timing_str)
			goto error;
		// Write the output on STDOUT
		if (!write_task_output(task_id, timing_str, cmd_str))
			goto error;

		free(cmd_str);
		free(timing_str);
	}

	if (close(fd_reply) < 0) {
		ERR_SYS("close fifo_reply");
		return false;
	}
	return true;

error:
	if (close(fd_reply) < 0)
		ERR_SYS("close fifo_reply");
	return false;
}

/**
 * @brief Main function to execute the LIST command.
 *
 * @param ctx 	Pointer to the daemon context structure.
 * @return true on overall success, false otherwise.
 */
bool	list_tasks(struct s_data *ctx)
{
	if (!request_list_tasks(ctx->fifo_request))
		return false;

	if (!handle_list_tasks_reply(ctx))
		return false;

	return true;
}