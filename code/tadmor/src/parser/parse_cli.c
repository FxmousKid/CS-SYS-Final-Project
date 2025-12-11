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
	char	abs_path[sizeof(ctx->fifo_request) - REQUEST_FIFO_NAME_LEN] = {0};

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
	snprintf(ctx->fifo_reply, sizeof(ctx->fifo_reply) , "%s%s", abs_path, REPLY_FIFO_NAME);
	snprintf(ctx->fifo_request, sizeof(ctx->fifo_request), "%s%s", abs_path, REQUEST_FIFO_NAME);
	return true;
}

static bool	opts_handle(struct s_data *ctx, int opt, char *argv[])
{
	char	*taskid = NULL;
	char	*minutes = NULL;
	char	*hours = NULL;
	char	*daysofweek = NULL;

	// for -Werror...
	(void)taskid;
	(void)minutes;
	(void)hours;
	(void)daysofweek;

	switch(opt) {
	
	// list tasks
	case 'l':
		ctx->communication_func = list_tasks;
		break;

	// remove a task
	case 'r':
		ctx->task_id = atol(optarg);
		break;

	// show exit code history of a task
	case 'x':
		taskid = optarg;
		break;
	
	// show last stdout of a task
	case 'o':
		taskid = optarg;
		break;
	
	// show last stderr of a task
	case 'e':
		taskid = optarg;
		break;
	
	// create a simple command
	case 'c':
		break;

	// creates a sequence command
	case 's':
		break;
	
	// set the minutes of a task
	case 'm':
		minutes = optarg;
		break;
	
	// set the hours of a task
	case 'H':
		hours = optarg;
		break;
	
	// set up the days of the week of a task
	case 'd':
		daysofweek = optarg;
		break;

	// sets up a task that has no specified time to run at
	case 'n':
		break;
		
	// stop the daemon
	case 'q':
		break;
	
	// the directory for storing named pipes
	case 'p':
		if (!parse_custom_fifo_dir(ctx, optarg))
			return false;
		break;

	// if launched in debug mode
	case 'b':
		ctx->debug_mode = true;
		break;
	
	// display help dialog
	case 'h':
		print_help();
		return false;

	// Unknown opts
	case '?':
		ERR_MSG("Unkown option '%s'", argv[optind]);
		fprintf(stderr, "Check usage with -h, --help\n");
		ctx->exit_code = false;
		return false;

	// if no option provided
	default:
		return false;
	}

	return true;
}

static void	parse_options(struct s_data *ctx, int argc, char *argv[])
{
	int		opt;
	extern int	opterr;
	const char	*shortopts = "x:o:e:r:lcsm:H:d:np:qbh";
	struct option	longopts[] = {
		// Options to manipulate tasks
		{"show-exit-codes-history", required_argument, NULL, 'x'},
		{"show-last-stdout", required_argument, NULL, 'o'},
		{"show-last-stderr", required_argument, NULL, 'e'},
		{"remove-task", required_argument, NULL, 'r'},
		{"list-tasks", no_argument, NULL, 'l'},

		// Option to create task
		{"create-simple-command", no_argument, NULL, 'c'},
		{"create-sequence-command", no_argument, NULL, 's'},
		{"minutes", required_argument, NULL, 'm'},
		{"hours", required_argument, NULL, 'H'},
		{"daysofweek", required_argument, NULL, 'd'},
		{"no-time", no_argument, NULL, 'n'},

		// General options
		{"pipes-directory", required_argument, NULL, 'p'},
		{"stop-daemon", no_argument, NULL, 'q'},
		{"debug", no_argument, NULL, 'b'},
		{"help", no_argument, NULL, 'h'},
		{NULL, no_argument, NULL, 0}
	};

	opterr = 0;
	while ((opt = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
		if (!opts_handle(ctx, opt, argv)) {
			exit(ctx->exit_code);
		}
	}
}

bool	parse_cli(struct s_data *ctx, int argc, char *argv[])
{
	parse_options(ctx, argc, argv);
	argc -= optind;
	argv += optind;	
	if (ctx->fifo_request[0] == '\0')
		set_fifos_path_default(ctx);

	// add cmd and tasks parsing
	// parse_arguments(ctx, argc, argv);
	return true;
}
