#include "parser/parse_cli.h"

static bool	opts_handle(struct s_data *ctx, int opt, char *argv[])
{
	char	*taskid = NULL;
	char	*minutes = NULL;
	char	*hours = NULL;
	char	*daysofweek = NULL;
	char	*pipes_dir = NULL;

	// for -Werror...
	(void)taskid;
	(void)minutes;
	(void)hours;
	(void)daysofweek;
	(void)pipes_dir;

	switch(opt) {
	
	// list tasks
	case 'l':
		break;

	// remove a task
	case 'r':
		taskid = optarg;
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
		pipes_dir = optarg;
		break;

	// if launched in debug mode
	case 'b':
		ctx->debug = true;
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

	// add cmd and tasks parsing
	// parse_arguments(ctx, argc, argv);
	return true;
}
