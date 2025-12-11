
#ifndef STRUCT_H
# define STRUCT_H

# include <stdlib.h>
# include <stdint.h>
# include <limits.h>
# include <stdbool.h>
# include <dirent.h>

# include "macros.h"

typedef uint64_t	taskid_t;

struct s_dir {
	DIR	*dir;
	DIR	*old_dir;
	char	path[PATH_MAX + 1];

};

struct s_reply {
	uint8_t	*buf;
	size_t	buf_size;
};

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

/** @brief union used to abstract the command type */
union u_cmd {
	/** @brief struct representing a simple command */
	struct s_cmd_si {
		/** @brief absolute cmd path : "/usr/bin/echo" */
		char	cmd_path[PATH_MAX + 1];
		/** @brief execve compliant char ** : {"echo", "lol", NULL}. */
		char	**command;
	} cmd_si;

	/** @brief struct representing a sequence of commands */
	struct s_cmd_sq {
		/** @brief cmd 0 ; ... ; cmd n - 1 <=> cmds[0], ... , cmds[n-1]. */
		struct s_cmd	*cmds;
		/** @brief number of commands (number of semi-colons + 1). */
		int		nb_cmds;
	} cmd_sq;
};

/** @brief struct representing a abstract command via a tagged union*/
struct s_cmd {
	DIR		*cmd_dir;
	char		path[PATH_MAX + 1];
	enum cmd_type	cmd_type;
	union u_cmd	cmd;
	pid_t		pid;
	uint16_t	exit_code;
};

struct s_timing {
	uint64_t	minutes;
	uint32_t	hours;
	uint8_t		days;
};

struct s_task {
	char		path[PATH_MAX + 1];
	char		stdout_path[PATH_MAX + 1];
	char		stderr_path[PATH_MAX + 1];
	char 		texit_path[PATH_MAX + 1];
	struct s_timing	timing;
	struct s_cmd	*cmd;
	struct s_task	*next;
	taskid_t	task_id;
};

/* @brief struct representing the daemon data */
struct s_data {
	/** @brief Ptr to first tasks, linked list like structure*/
	struct s_task	*tasks;
	/** @brief exit code of the daemon. */
	uint8_t		exit_code;
	/** @brief provided or default path to the run directory. *
	 * '- 100' to keep space for dedicated folders like tasks/ */
	char		run_directory[PATH_MAX + 1 - 100];
	/** @brief Full path for reply fifo.  */
	char		fifo_reply[PATH_MAX + 1];
	/** @brief Full path for request fifo.  */
	char		fifo_request[PATH_MAX + 1];
	/** @brief flag to indiciate if tasks files are little endian. */
	bool		is_data_le;
	/** @brief flag to indicate if debug mode is enabled. */
	bool		debug_mode;
};

#endif

