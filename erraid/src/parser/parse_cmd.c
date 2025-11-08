#include <parser/parse_cmd.h>

static bool	_read_and_alloc_command(struct s_cmd cmd, int fd,
					bool isdle, uint32_t argc)
{
	ssize_t		n;
	uint32_t	arg_len;

	for (size_t i = 0; i < argc; i++) {
		n = read_endian(fd, &arg_len, sizeof(arg_len), isdle);
		if (n < 0) {
			SYS_ERR("calloc");
			return (false);
		}
		if (n == 0) {
			MSG_ERR("unexpected EOF when reading");
			return false;
		}
		cmd.command[i] = calloc(arg_len, sizeof(char));
		if (!cmd.command[i]) {
			SYS_ERR("calloc");
			return (false);
		}

		// do not use read_endian here
		n = read(fd, cmd.command[i], arg_len);
		if (n < 0) {
			SYS_ERR("read");
			return false;
		}
		if (n == 0) {
			MSG_ERR("unexpected EOF when reading");
			return false;
		}
	}
	return true;
}

static bool	_fparse_cmd_si(struct s_data *ctx, int fd, struct s_cmd *cmd)
{
	uint32_t	argc = 0;
	ssize_t		n;
	bool		isdle = ctx->is_data_le;

	n = read_endian(fd, &argc, sizeof(argc), isdle);
	if (n < 0) {
		SYS_ERR("calloc");
		return (false);
	}
	if (n <= 0) {
		MSG_ERR("unexpected EOF when reading");
		return false;
	}
	cmd->command = calloc(argc + 1, sizeof(char *));
	if (!cmd->command) {
		SYS_ERR("calloc");
		return false;
	}
	if (!_read_and_alloc_command(*cmd, fd, isdle, argc)) {
		free_darr(cmd->command);
		return false;
	}
	cmd->command[argc] = NULL;
	print_darr(cmd->command);
	return true;
}

bool	parse_cmd(struct s_data *ctx, const char *path, struct s_cmd *cmd)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		SYS_ERR("open");
		return false;
	}

	int	n;
	// if (type == SI)
	n = _fparse_cmd_si(ctx, fd, cmd);

	close(fd);
	return n;
}
