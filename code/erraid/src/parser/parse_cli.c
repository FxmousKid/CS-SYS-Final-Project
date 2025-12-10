#include "parser/parse_cli.h"

static void	set_fifos_path_default(struct s_data *ctx)
{
	char	*user;
	// should be more than enough for /tmp/$USER/erraid/pipes/
	char	buf[512] = {0};

	user = getenv("USER");
	strcpy(buf, "/tmp/");
	if (user)
		strcat(buf, user);
	strcat(buf, "/erraid/pipes");
	snprintf(ctx->fifo_request, sizeof(ctx->fifo_request), "%s/%s", buf, REQUEST_FIFO_NAME);
	snprintf(ctx->fifo_reply, sizeof(ctx->fifo_reply), "%s/%s", buf, REPLY_FIFO_NAME);
}

static bool	parse_custom_fifo_dir(struct s_data *ctx, const char *path)
{
	// 'PATH_MAX + 1' for the biggest path, 
	// '- sizeof(REQUEST_FIFO_NAME)' to make sure it fits
	char	abs_path[PATH_MAX + 1 - sizeof(REQUEST_FIFO_NAME)] = {0};

	if (!path || !*path) {
		ERR_MSG("Invalid pipes dir path\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}

	if (!convert_to_absolute_path(path, abs_path)) {
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}

	if (abs_path[strlen(abs_path) - 1] != '/' && strlen(abs_path) < PATH_MAX - 1)
		abs_path[strlen(abs_path)] = '/';
	if (strlen(abs_path) + strlen(REQUEST_FIFO_NAME) + 1 >= PATH_MAX) {
		ERR_MSG("Full fifo path too long : %s\n", abs_path);
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}
	snprintf(ctx->fifo_reply, PATH_MAX + 1 , "%s%s", abs_path, REPLY_FIFO_NAME);
	snprintf(ctx->fifo_request, PATH_MAX + 1, "%s%s", abs_path, REQUEST_FIFO_NAME);
	return true;
}

static void	set_run_dir_default(struct s_data *ctx)
{
	char *user;

	user = getenv("USER");
	strcpy(ctx->run_directory, "/tmp/");
	if (user)
		strcat(ctx->run_directory, user);
	strcat(ctx->run_directory, "/erraid/");	
}

static bool	parse_custom_run_directory(struct s_data *ctx, const char *path)
{
	char	*ctx_rd;
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

	if (strlen(abs_path) > PATH_MAX) {
		ERR_MSG("Run directory path too long\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}

	ctx_rd = ctx->run_directory;
	strcpy(ctx_rd, abs_path);
	if (ctx_rd[strlen(ctx_rd) - 1] != '/' && strlen(ctx_rd) < PATH_MAX - 1)
		ctx_rd[strlen(ctx_rd)] = '/';
	return true;
}

static bool	opts_handle(struct s_data *ctx, int opt)
{
	switch (opt) {
	
	// specify dir of namedpath creation : -r PATH
	case 'r':
		if (!parse_custom_run_directory(ctx, optarg))
			return false;
		break;
	
	// specify dir of named pipes : -p PATH
	case 'p':
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
	char		*shortopts = "hdlr:p:";		
	int		opt;
	extern int	opterr;

	struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"debug", no_argument, NULL, 'd'},
		{"little-endian", no_argument, NULL, 'l'},
		{"run-directory", required_argument, NULL, 'r'},
		{"pipes-directory", required_argument, NULL, 'p'},
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
	if (ctx->fifo_request[0] == '\0')
		set_fifos_path_default(ctx);
	argc -= optind;
	argv += optind;
	return true;
}
