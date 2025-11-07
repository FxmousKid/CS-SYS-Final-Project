
#ifndef STRUCT_H
# define STRUCT_H

#include <limits.h>
#include <stdbool.h>

/* @brief struct representing the daemon data */
struct s_data {
	/** @brief exit code of the daemon. */
	short	exit_code;
	/** @brief provided path to the run directory. */
	char	run_directory[PATH_MAX];
	/** @brief flag to indiciate if tasks files are little endian. */
	bool	is_data_le;
};

/** @brief struct representing a command */
struct s_cmd {
	/** @brief absolute cmd path : "/usr/bin/echo" */
	char	cmd_path[PATH_MAX];
	/** @brief execv family compiant char ** : {"echo", "lol", NULL}. */
	char	**command;
};

#endif
