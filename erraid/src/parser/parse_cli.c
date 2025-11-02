#include "parser/parse_cli.h"
#include "utils.h"

static void	check_and_set_run_dir_default(struct s_data *ctx)
{
	char *user;

	user = getenv("USER");
	strcpy(ctx->run_directory, "/tmp/");
	if (user)
		strcat(ctx->run_directory, user);
	strcat(ctx->run_directory, "/erraid");	

}

static bool	parse_run_directory(struct s_data *ctx, const char *path)
{
	if (!path || !*path) {
		fprintf(stderr, "Error: Invalid run directory path\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}
	if (strlen(path) >= PATH_MAX) {
		fprintf(stderr, "Error: Run directory path too long\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}
	strcpy(ctx->run_directory, path);
	return true;
}

static bool	opts_handle(struct s_data *ctx, int opt, char *argv[])
{
	(void)argv;
	(void)opt;
	switch (opt) {
	
	// specify dir of namedpath creation : -r PATH
	case 'r':
		parse_run_directory(ctx, optarg);
		break;

	// help option : -h
	case 'h':
		print_help();
		return false;

	// data is little-endian : -l
	case 'l':
		ctx->is_data_le = true;
		break;

	// Unknown option
	case '?':
		fprintf(stderr, "Check usage with -h, --help\n");
		ctx->exit_code = EXIT_FAILURE;
		return false;
	}
	return true;
}


bool	parser(struct s_data *ctx, int argc, char *argv[])
{
	char		*shortopts = "hr:";		
	int		opt;
	extern int	opterr;

	struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"little-endian", no_argument, NULL, 'l'},
		{"run-directory", required_argument, NULL, 'r'},
		{NULL, 0, NULL, 0}
	};
	opterr = 0;
	while ((opt = getopt_long(argc, argv, shortopts, longopts, 0)) != -1) {
		// if opts_handle() returns false, no need to parse anymore
		if (!opts_handle(ctx, opt, argv)) {
			exit(ctx->exit_code);
		}
	}
	check_and_set_run_dir_default(ctx);
	argc -= optind;
	argv += optind;
	return true;
}
