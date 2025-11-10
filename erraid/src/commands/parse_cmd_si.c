#include <commands/parse_cmd_si.h>

// defined in main.c
extern bool isdle;

// static bool	_read_and_alloc_sub_cmds(struct s_cmd_si cmd, int fd,
bool	_read_and_alloc_sub_cmds(struct s_cmd_si *cmd, int fd,
					bool isdle, uint32_t argc)
{
	ssize_t		n;
	uint32_t	arg_len;

	for (size_t i = 0; i < argc; i++) {
		n = read_endian(fd, &arg_len, sizeof(arg_len), isdle);
		if (n < 0) {
			ERR_SYS("calloc");
			return (false);
		}
		if (n == 0) {
			ERR_MSG("unexpected EOF when reading");
			return false;
		}
		cmd->command[i] = calloc(arg_len + 1, sizeof(char));
		if (!cmd->command[i]) {
			ERR_SYS("calloc");
			return (false);
		}

		// do not use read_endian here
		n = read(fd, cmd->command[i], arg_len);
		if (n < 0) {
			ERR_SYS("read");
			return false;
		}
		if (n == 0) {
			ERR_MSG("unexpected EOF when reading");
			return false;
		}
	}
	return true;
}

// static bool	_fparse_cmd_si(struct s_data *ctx, int fd, struct s_cmd_si *cmd)
bool	_fparse_cmd_si(int fd, struct s_cmd_si *cmd, int isdle)
{
	uint32_t	argc = 0;
	ssize_t		n;

	n = read_endian(fd, &argc, sizeof(argc), isdle);
	if (n < 0) {
		ERR_SYS("calloc");
		return (false);
	}
	if (n <= 0) {
		ERR_MSG("unexpected EOF when reading");
		return false;
	}
	cmd->command = calloc(argc + 1, sizeof(char *));
	if (!cmd->command) {
		ERR_SYS("calloc");
		return false;
	}
	if (!_read_and_alloc_sub_cmds(cmd, fd, isdle, argc)) {
		free_darr(cmd->command);
		return false;
	}
	cmd->command[argc] = NULL;
	return true;
}


/**
 * @brief takes the path to a cmd (or sub-cmd) tree, and parses the
 * argv file into the passed cmd
 *
 * @param path the path of the dir where the argv lives
 * @param cmd struct to parse argv in the right union member
 *
 * @return 
 *  @retval true on success
 *  @retval false on failure
 */
bool	parse_cmd_si(char path[PATH_MAX + 1], struct s_cmd *cmd)
{
	int		fd;

	if (path[strlen(path) - 1] != '/')
		path[strlen(path)] = '/'; 

	if (strlen(path) + 4 > PATH_MAX) {
		ERR_MSG("path too big");
		return false;
	}

	strcat(path, "argv");
	if ((fd = open(path, O_RDONLY)) < 0) {
		ERR_SYS("open");
		return false;
	}

	remove_last_file_from_path(path);

	if (!_fparse_cmd_si(fd, &cmd->cmd.cmd_si, isdle)) {
		close(fd);
		return false;
	}

	close(fd);
	return true;
}
