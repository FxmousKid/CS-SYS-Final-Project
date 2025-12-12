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
	print_cmd_enum(cmd->cmd_type, true);

	if (cmd->cmd_type != CMD_SQ || !cmd->cmd.cmd_sq.cmds)
		return;

	if (snprintf(next_prefix, sizeof(next_prefix), "%s%s", prefix,
		    is_last ? "    " : "│   ") < 0)
		next_prefix[0] = '\0';
	child_count = cmd->cmd.cmd_sq.nb_cmds;
	for (int i = 0; i < child_count; ++i)
		print_cmd_tree_rec(&cmd->cmd.cmd_sq.cmds[i], next_prefix,
				   i == child_count - 1);
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
	if (cmd->cmd_type != CMD_SQ || !cmd->cmd.cmd_sq.cmds)
		return;
	for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; ++i)
		print_cmd_tree_rec(&cmd->cmd.cmd_sq.cmds[i], "",
				   i == cmd->cmd.cmd_sq.nb_cmds - 1);
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
	case 0x5349: 
		return CMD_SI;
	case 0x5351: 
		return CMD_SQ;
	default:
		ERR_MSG("Unknown command type");
	}

	return false;
}

/**
 * @brief Recursively reconstructs the command line string from a structured command tree.

 * @param cmd	The pointer to the s_cmd structure to serialize.
 * @return A dynamically allocated command string (char*).
 */
char	*cmd_to_string(const struct s_cmd *cmd)
{
	if (!cmd)
		return strdup("");
	
	char	*str = NULL;
	size_t	len = 0;
	size_t	size = INITIAL_CMD_STR_SIZE;

	if (!(str = calloc(size, sizeof(char)))) {
		ERR_SYS("calloc");
		return NULL;
	}

	if (cmd->cmd_type == CMD_SI) {
		// Simple Command: Concat arguments separated by spaces (argv[0], argv[1], ...)
		for (int i = 0; cmd->cmd.cmd_si.command[i] != NULL; i++) {
			size_t arg_len = strlen(cmd->cmd.cmd_si.command[i]);
			
			/* Check if there is enough space */
			while (len + arg_len + 2 > size) { // +2 for space + '\0'
				size *= 2;
				char *tmp = realloc(str, size);
				if (!tmp) {
					ERR_SYS("realloc");
					free(str);
					return NULL;
				}
				str = tmp;
			}

			if (i > 0)
				str[len++] = ' '; /* Argument separator */
			
			memcpy(str + len, cmd->cmd.cmd_si.command[i], arg_len);
			len += arg_len;
		}
		str[len] = '\0';
		
	} else if (cmd->cmd_type == CMD_SQ) {
		/* Sequence Command: (cmd1 ; cmd2 ; ...) */
		
		str[len++] = '(';
		for (int i = 0; i < cmd->cmd.cmd_sq.nb_cmds; i++) {
			/* Recursive call to get the sub-command string */
			char *sub_cmd_str = cmd_to_string(&(cmd->cmd.cmd_sq.cmds[i]));
			if (!sub_cmd_str) {
				free(str);
				return NULL;
			}
			size_t sub_len = strlen(sub_cmd_str);

			/* Check if there is enough space */
			while (len + sub_len + 3 > size) { // +3 for space + ";" or ")" and '\0'
				size *= 2;
				char *tmp = realloc(str, size);
				if (!tmp) {
					ERR_SYS("realloc");
					free(sub_cmd_str);
					free(str);
					return NULL;
				}
				str = tmp;
			}

			memcpy(str + len, sub_cmd_str, sub_len);
			len += sub_len;
			free(sub_cmd_str);

			if (i < cmd->cmd.cmd_sq.nb_cmds - 1) {
				str[len++] = ';';
				str[len++] = ' ';
			}
		}
		str[len++] = ')';
		str[len] = '\0';
	}

	return str;
}