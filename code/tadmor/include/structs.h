#ifndef STRUCTS_H
# define STRUCTS_H

# include <stdbool.h>
# include <stdint.h>
# include <limits.h>
# include <stdio.h>

typedef uint64_t	taskid_t;
typedef uint64_t	minutes_t;
typedef uint32_t	hours_t;
typedef uint8_t		dasyofweek_t;

enum req_opcode {
	/** @brief List all tasks. */
	OPCODE_LS = 0x4c53,
	/** @brief Create a new simple task. */
	OPCODE_CR = 0x4352,
	/** @brief Create a new task by combining existing tasks. */
	OPCODE_CB = 0x4342,
	/** @brief Remove a task. */
	OPCODE_RM = 0x524d,
	/** @brief List execution times and exit codes of all previous runs of a task. */
	OPCODE_TX = 0x5458,
	/** @brief Display the standard output of the last complete execution of a task. */
	OPCODE_SO = 0x534f,
	/** @brief Display the standard error output of the last complete execution of a task. */
	OPCODE_SE = 0x5345,
	/** @brief Terminate the daemon. */
	OPCODE_TM = 0x4b49,
};

enum	cmd_type {
	/** @brief Simple command "echo hello". */
	CMD_SI = 0x5349,
	/** @brief Sequence of commands "cmd1 ; cmd2 ; cmd3". */
	CMD_SQ = 0x5351,
	CMD_IF,
	/** @brief Pipeline "cmd1 | cmd2 | cmd3". */
	CMD_PL,
};

struct s_timing {
	minutes_t	minutes;
	hours_t		hours;
	dasyofweek_t	days;
};

struct s_data;

typedef bool	(*t_command_function)(struct s_data *ctx);

/* structure to fill as we parse, so that we 
 * serialize into the right types, then send over the
 * pipe to the daemon, so that he deserializes them */
struct s_cmd {
	char		*time_raw; // raw string for now -- to remove 
	struct s_timing timing; // -m
	bool		is_abstraite; // -n or not
};


struct s_data {
	/** @brief Full path for reply fifo.  */
	char			fifo_reply[PATH_MAX + 1];
	/** @brief Full path for request fifo.  */
	char			fifo_request[PATH_MAX + 1];
	/** @brief exit code of the daemon. */
	uint8_t			exit_code;
	/** @brief the constructed command object to send. */
	struct s_cmd		cmd;
	/** @brief ptr to the right function to call after parsing. */
	t_command_function	communication_func;
	/** @brief task id to use with right options. */
	uint64_t		task_id;
	/** @brief flag to indicate if debug mode is enabled. */
	bool			debug_mode;
};

struct s_reply {
	uint8_t	*buf;
	size_t	buf_size;
};

#endif
