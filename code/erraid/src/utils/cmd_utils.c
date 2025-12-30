#include "utils/cmd_utils.h"

// defined in main.c
extern bool isdle;

/**
 * @brief Allocates and initializes a command structure
 *
 * @return struct s_cmd* Pointer to the allocated command structure, or NULL on failure
 */
struct s_cmd	*get_cmd_struct(void)
{
	struct s_cmd	*cmd = NULL;

	if (!(cmd = calloc(1, sizeof(struct s_cmd)))) {
		ERR_SYS("calloc");
		return NULL;
	}
	return (cmd);
}

static void	free_cmd_node(struct s_cmd *cmd, bool free_self)
{
	if (!cmd)
		return;

	switch (cmd->cmd_type) {
	case CMD_SI:
		if (cmd->cmd.cmd_si.command) {
			free_darr(cmd->cmd.cmd_si.command);
			cmd->cmd.cmd_si.command = NULL;
		}
		break;
	case CMD_SQ:
		if (cmd->cmd.cmd_sq.cmds) {
			for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; ++i)
				free_cmd_node(&cmd->cmd.cmd_sq.cmds[i], false);
			free(cmd->cmd.cmd_sq.cmds);
			cmd->cmd.cmd_sq.cmds = NULL;
			cmd->cmd.cmd_sq.nb_cmds = 0;
		}
		break;

	case CMD_PL:
		if (cmd->cmd.cmd_pl.cmds) {
			for (int i = 0; i < cmd->cmd.cmd_pl.nb_cmds; ++i)
				free_cmd_node(&cmd->cmd.cmd_pl.cmds[i], false);
			free(cmd->cmd.cmd_pl.cmds);
			free(cmd->cmd.cmd_pl.fds);
			cmd->cmd.cmd_pl.cmds = NULL;
			cmd->cmd.cmd_pl.nb_cmds = 0;
		}
		break;

	default:
		break;
	}

	if (free_self)
		free(cmd);
}

static void	print_cmd_tree_rec(const struct s_cmd *cmd, const char *prefix,
				bool is_last)
{
	char	next_prefix[PATH_MAX + 1] = {0};
	int	child_count;

	if (!cmd)
		return;

	printf("%s%s ", prefix, is_last ? "└──" : "├──");
	print_cmd_enum(cmd->cmd_type, false);
	printf(" - [%d]", cmd->cmd_id);
	if (cmd->cmd_type == CMD_SI && cmd->cmd.cmd_si.stdout_path)
		printf(" -- [out : %s]", cmd->cmd.cmd_si.stdout_path);
	if (cmd->cmd_type == CMD_SI && cmd->cmd.cmd_si.stderr_path)
		printf(" -- [err : %s]", cmd->cmd.cmd_si.stderr_path);
	printf("\n");
	

	if (cmd->cmd_type != CMD_SQ && cmd->cmd_type != CMD_PL)
		return;

	if (snprintf(next_prefix, sizeof(next_prefix), "%s%s", prefix,
		    is_last ? "    " : "│   ") < 0)
		next_prefix[0] = '\0';

	if (cmd->cmd_type == CMD_SQ) {
		child_count = cmd->cmd.cmd_sq.nb_cmds;
		for (int i = 0; i < child_count; ++i)
			print_cmd_tree_rec(&cmd->cmd.cmd_sq.cmds[i], next_prefix,
					   i == child_count - 1);
	}

	if (cmd->cmd_type == CMD_PL) {
		child_count = cmd->cmd.cmd_pl.nb_cmds;
		for (int i = 0; i < child_count; ++i)
			print_cmd_tree_rec(&cmd->cmd.cmd_pl.cmds[i], next_prefix,
					   i == child_count - 1);
	}
}

static void	closedir_cmd_rec(struct s_cmd *cmd)
{
	if (cmd && cmd->cmd_dir) {
		closedir(cmd->cmd_dir);
		cmd->cmd_dir = NULL;
	}

	switch (cmd->cmd_type) {
	case CMD_SQ:
		for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; ++i)
			closedir_cmd_rec(&cmd->cmd.cmd_sq.cmds[i]);
		break;

	case CMD_PL:
		for (int i = 0; i < cmd->cmd.cmd_pl.nb_cmds; ++i)
			closedir_cmd_rec(&cmd->cmd.cmd_pl.cmds[i]);
		break;

	
	default:
		return ;
	}
}

/**
 * @brief Frees the full command structure
 *
 * @param cmd The command structure to free
 */
void	free_command_rec(struct s_cmd *cmd)
{
	closedir_cmd_rec(cmd);
	free_cmd_node(cmd, true);
}

/**
 * @brief Prints the in-memory representation of a command tree
 *
 * @param cmd The command tree to display
 */
void	print_cmd_tree(struct s_cmd *cmd)
{
	if (!cmd) {
		printf("(null cmd)\n");
		return;
	}
	print_cmd_enum(cmd->cmd_type, true);
	if (cmd->cmd_type != CMD_SQ && cmd->cmd_type != CMD_PL)
		return;

	if (cmd->cmd_type == CMD_SQ)
		for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; ++i)
			print_cmd_tree_rec(&cmd->cmd.cmd_sq.cmds[i], "",
					   i == cmd->cmd.cmd_sq.nb_cmds - 1);
	if (cmd->cmd_type == CMD_PL)
		for (int i = 0; i < cmd->cmd.cmd_pl.nb_cmds; ++i)
			print_cmd_tree_rec(&cmd->cmd.cmd_pl.cmds[i], "",
					   i == cmd->cmd.cmd_pl.nb_cmds - 1);
}

/**
 * @brief Removes the last file from a given path
 *
 * @param path The path from which to remove the last file
 */
void	remove_last_file_from_path(char *path)
{
	int	n;

	n = strlen(path);
	while (path[--n] != '/')
		path[n] = '\0';
}

/* cmd_path is "...../cmd" or "...../cmd_id", somewhere
 * we are guaranteed to have a "type" file in
 * */
enum cmd_type	get_cmd_type(const char *path_cmd_dir)
{
	int		type_fd;
	char		buf[PATH_MAX + 1] = {0};
	uint16_t	type = 0;

	strcpy(buf, path_cmd_dir);
	// 5 = len("/") + len("type") 
	if (strlen(buf) + 5 > PATH_MAX) {
		ERR_MSG("filename is too big");
		return false;
	}

	// if path_cmd_dir does not end with '/', add it
	if (buf[strlen(buf) - 1] != '/')
		buf[strlen(buf)] = '/'; 
	strcat(buf, "type");

	if ((type_fd = open(buf, O_RDONLY)) < 0) {
		ERR_SYS("type file open");
		return false;
	}

	if (read_endian(type_fd, &type, sizeof(type), isdle) != 2) {
		ERR_SYS("read");
		close(type_fd);
		return (false);
	}
	close(type_fd);

	switch(type) {
	case CMD_SI:
		return CMD_SI;
	case CMD_SQ:
		return CMD_SQ;
	case CMD_PL:
		return CMD_PL;
	default:
		ERR_MSG("Unknown command type");
	}

	return false;
}
