
#ifndef STRUCT_H
# define STRUCT_H

# include <stdlib.h>
# include <limits.h>
# include <stdbool.h>
# include <dirent.h>

/* @brief struct representing the daemon data */
struct s_data {
	/** @brief exit code of the daemon. */
	short	exit_code;
	/** @brief provided path to the run directory. */
	char	run_directory[PATH_MAX + 1];
	/** @brief flag to indiciate if tasks files are little endian. */
	bool	is_data_le;
};

struct s_dir {
	DIR	*dir;
	DIR	*old_dir;
	char	path[PATH_MAX + 1];

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
	short		exit_code;
	pid_t		pid;
};

#endif
