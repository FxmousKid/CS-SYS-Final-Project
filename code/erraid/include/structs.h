
#ifndef STRUCT_H
# define STRUCT_H

# include <stdlib.h>
# include <stdint.h>
# include <limits.h>
# include <stdbool.h>
# include <dirent.h>

# include "macros.h" // IWYU pragma: keep

typedef uint64_t	taskid_t;

struct s_dir {
	DIR	*dir;
	DIR	*old_dir;
	char	path[PATH_MAX + 1];

};

struct s_request {
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

enum reply_opcode {
	/** @brief Request executed successfully */
	OPCODE_OK = 0x4f4b,
	/** @brief Request didn't executed successfully */
	OPCODE_ER = 0x4552,
};


enum	cmd_type {
	/** @brief Simple command "echo hello". */
	CMD_SI = 0x5349,
	/** @brief Sequence of commands "cmd_1 ; ... ; cmd_n". */
	CMD_SQ = 0x5351,
	/** @brief Pipeline "cmd_1 | ... | cmd_n". */
	CMD_PL = 0x504c,
	CMD_IF,
};

/** @brief union used to abstract the command type */
union u_cmd {
	/** @brief struct representing a simple command */
	struct s_cmd_si {
		int		fd_in;
		int		fd_out;
		int		fd_err;
		const char	*stdout_path;
		const char	*stderr_path;
		/** @brief absolute cmd path : "/usr/bin/echo" */
		char		cmd_path[PATH_MAX + 1];
		/** @brief execve compliant char ** : {"echo", "lol", NULL}. */
		char		**command;
	} cmd_si;

	/** @brief struct representing a sequence of commands */
	struct s_cmd_sq {
		/** @brief cmd 0 ; ... ; cmd n - 1 <=> cmds[0], ... , cmds[n-1]. */
		struct s_cmd	*cmds;
		/** @brief number of commands (number of semi-colons + 1). */
		int		nb_cmds;
	} cmd_sq;

	/** @brief struct representing a pipeline of commands. */
	struct s_cmd_pl {
		/** @brief cmd 0 | ... | cmd n - 1 <=> cmds[0], ... , cmds[n-1]. */
		struct s_cmd	*cmds;
		/** @brief allocated space of nb_cmds - 1 pairs of int, to store the pipes. */
		int		(*fds)[2];
		/** @brief number of commands (number of pipes + 1). */
		int		nb_cmds;
	} cmd_pl;
};


struct s_timing {
	uint64_t	minutes;
	uint32_t	hours;
	uint8_t		days;
};

/** @brief struct representing a abstract command via a tagged union*/
struct s_cmd {
	/** @brief The "index" of the command (0 or 1 indexed doesn't matter
	*   what matters is the fact that the order remains. */
	int		cmd_id;
	DIR		*cmd_dir;
	char		path[PATH_MAX + 1];
	enum cmd_type	cmd_type;
	union u_cmd	cmd;
	pid_t		pid;
	uint16_t	exit_code;
};

struct s_task {
	/** @brief The number of sub commands. */
	int		sub_cmds_count;
	char		path[PATH_MAX + 1];
	char		stdout_path[PATH_MAX + 1];
	char		stderr_path[PATH_MAX + 1];
	char 		texit_path[PATH_MAX + 1];
	struct s_timing	timing;
	struct s_cmd	*cmd;
	struct s_task	*next;
	taskid_t	task_id;
	int64_t		launch_time;
};

/* @brief struct representing the daemon data */
struct s_data {
	/** @brief wether to wait for the right timing or to execute
	*   all tasks instantly. */
	bool		exec_instant;
	/** @brief Ptr to first tasks, linked list like structure*/
	struct s_task	*tasks;
	/** @brief exit code of the daemon. */
	uint8_t		exit_code;
	/** @brief to execute in foreground (no daemonization). */
	bool		foreground;
	/** @brief provided or default path to the run directory. *
	 * '- 100' to keep space for dedicated folders like tasks/ */
	char		run_directory[PATH_MAX + 1 - 100];
	/** @brief provided or default path to the pipes directory */
	char		pipes_dir[PATH_MAX + 1 - 100];
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

