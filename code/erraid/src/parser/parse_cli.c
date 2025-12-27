#include "parser/parse_cli.h"

static void	set_fifos_path_default(struct s_data *ctx)
{
	if(!build_safe_path(ctx->pipes_dir, sizeof(ctx->pipes_dir), ctx->run_directory, PIPES_DIR)) {
		ERR_MSG("Failed to build pipes directory path");
		return;
	}
	if (!build_safe_path(ctx->fifo_request, sizeof(ctx->fifo_request), ctx->pipes_dir, REQUEST_FIFO_NAME))
		ERR_MSG("Failed to build request fifo path");
	if (!build_safe_path(ctx->fifo_reply, sizeof(ctx->fifo_reply), ctx->pipes_dir, REPLY_FIFO_NAME))
		ERR_MSG("Failed to build reply fifo path");
}

static bool	parse_custom_fifo_dir(struct s_data *ctx, const char *path)
{
	char	abs_path[PATH_MAX + 1] = {0};

	if (!path || !*path) {
		ERR_MSG("Invalid pipes dir path\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}

	if (!convert_to_absolute_path(path, abs_path)) {
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}
	if (!build_safe_path(ctx->pipes_dir, sizeof(ctx->pipes_dir), "", abs_path)) {
		ERR_MSG("Failed to build pipes directory path");
		return false;
	}
	if (!build_safe_path(ctx-> fifo_reply, sizeof(ctx->fifo_reply), abs_path, REPLY_FIFO_NAME)) {
		ERR_MSG("Failed to build fifo reply path");
		return false;
	}
		
	if (!build_safe_path(ctx->fifo_request, sizeof(ctx->fifo_request), abs_path, REQUEST_FIFO_NAME)) {
		ERR_MSG("Failed to build fifo request path");
		return false;
	}

	return true;
}

static void	set_run_dir_default(struct s_data *ctx)
{
	char *user;
	char buf[PATH_MAX + 1];

	user = getenv("USER");
	if (!user)
		return;
	if (!build_safe_path(buf, sizeof(buf), TMP_PATH, user))
		ERR_MSG("Failed to build /tmp/$USER path %s", buf);
	if (!build_safe_path(ctx->run_directory, sizeof(ctx->run_directory), buf, ERRAID_PATH))
		ERR_MSG("Failed to build run_directory %s", ctx->run_directory);
}

static bool	parse_custom_run_directory(struct s_data *ctx, const char *path)
{
	char	abs_path[PATH_MAX + 1] = {0};

	if (!path || !*path) {
		ERR_MSG("Invalid run directory path\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}

	if (!convert_to_absolute_path(path, abs_path)) {
		ERR_MSG("Failed to convert to absolute path: %s\n", path);
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}
	if (!build_safe_path(ctx->run_directory, sizeof(ctx->run_directory), "", abs_path)) {
		ERR_MSG("Failed to build run directory path");
		return false;
	}
	return true;
}

static bool	opts_handle(struct s_data *ctx, int opt)
{
	switch (opt) {
	
	// specify dir of namedpath creation : -r PATH
	case 'R':
		if (!parse_custom_run_directory(ctx, optarg))
			return false;
		break;
	
	// specify dir of named pipes : -p PATH
	case 'P':
		if (!parse_custom_fifo_dir(ctx, optarg))
			return false;
		break;

	// help option : -h
	case 'h':
		print_help();
		return false;

	// debug mode : daemon writes to stdout;
	case 'd':
		ctx->debug_mode = true;
		break;

	// data is little-endian : -l
	case 'l':
		ctx->is_data_le = true;
		break;

	// Unknown option
	case '?':
		printf("Check usage with -h, --help\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}
	return true;
}


bool	parser_cli(struct s_data *ctx, int argc, char *argv[])
{
	char		*shortopts = "hdlR:P:";
	int		opt;
	extern int	opterr;

	struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"debug", no_argument, NULL, 'd'},
		{"little-endian", no_argument, NULL, 'l'},
		{"run-directory", required_argument, NULL, 'R'},
		{"pipes-directory", required_argument, NULL, 'P'},
		{NULL, 0, NULL, 0}
	};
	opterr = 0;
	while ((opt = getopt_long(argc, argv, shortopts, longopts, 0)) != -1) {
		// if opts_handle() returns false, no need to parse anymore
		if (!opts_handle(ctx, opt)) {
			exit(ctx->exit_code);
		}
	}
	// if -r not used, then used default
	if (ctx->run_directory[0] == '\0')
		set_run_dir_default(ctx);
	// if -p not used, then used default
	if (ctx->pipes_dir[0] == '\0')
		set_fifos_path_default(ctx);
	argc -= optind;
	argv += optind;
	return true;
}
