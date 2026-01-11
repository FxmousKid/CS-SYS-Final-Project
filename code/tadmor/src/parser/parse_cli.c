#include "parser/parse_cli.h"

static void	set_fifos_path_default(struct s_data *ctx)
{
	char    *user;
	char    part1[PATH_MAX + 1] = {0};
	char 	part2[PATH_MAX + 1] = {0};

	user = getenv("USER");
	if (!user) {
		ERR_SYS("getenv");
		return;
	}
	if (!build_safe_path(part1, sizeof(part1), TMP_PATH, user) ||
	!build_safe_path(part2, sizeof(part2), ERRAID_PATH, PIPES_DIR)) {
		ERR_MSG("Failed to build part1 or part2 of the default pipes path");
		return;
	}

	if (!build_safe_path(ctx->pipes_dir, sizeof(ctx->pipes_dir), part1, part2)) {
		ERR_MSG("Failed to build the default pipes path %s", ctx->pipes_dir);
		return;
	}
	if (!build_safe_path(ctx->fifo_request, sizeof(ctx->fifo_request), ctx->pipes_dir, REQUEST_FIFO_NAME)) {
		ERR_MSG("Failed to build request fifo path %s", ctx->fifo_request);
	}
	if (!build_safe_path(ctx->fifo_reply, sizeof(ctx->fifo_reply), ctx->pipes_dir, REPLY_FIFO_NAME)) {
		ERR_MSG("Failed to build reply fifo path %s", ctx->fifo_reply);
	}
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


static void	parse_timing_opt(struct s_data *ctx, char *argv[])
{
	const char *arg_cur = argv[optind];

	if (strcmp(arg_cur, "-n") == 0) {
		ctx->cmd.timing.days = 0;
		ctx->cmd.timing.hours = 0;
		ctx->cmd.timing.minutes = 0;
		optind++;
		arg_cur = argv[optind];
	}

	if (strcmp(arg_cur, "-m") == 0) {
		optind++;
		ctx->cmd.timing.minutes = parse_minutes(argv[optind]);
		optind++;
		arg_cur = argv[optind];
	}

	if (strcmp(arg_cur, "-H") == 0) {
		optind++;
		ctx->cmd.timing.hours = parse_hours(argv[optind]);
		optind++;
		arg_cur = argv[optind];
	}

	if (strcmp(arg_cur, "-d") == 0) {
		optind++;
		ctx->cmd.timing.days = parse_days(argv[optind]);
		optind++;
		arg_cur = argv[optind];
	}
}


/**
 * @brief handles parsing of variadic options, -i, -p, ..., then moves
 * optind onto the next option
 *
 * @param ctx 
 * @param argv 
 */
int	parse_variadic_opt(struct s_data *ctx, char *argv[])
{
	const char	*arg_cur = NULL;
	int		count = 0;


	(void)ctx;
	arg_cur = argv[optind];
	while (arg_cur && arg_cur[0] != '-') {
		count++;
		optind++;
		arg_cur = argv[optind];
	}
	return count;
}

static bool	opts_handle(struct s_data *ctx, int opt, char *argv[], int *current)
{
	char	*minutes = NULL;
	char	*hours = NULL;
	char	*daysofweek = NULL;
	// char	c = 0;

	// for -Werror...
	switch(opt) {
	
	// list tasks
	case 'l':
		ctx->communication_func = list_tasks;
		break;

	// remove a task
	case 'r':
		ctx->communication_func = remove_task;
		ctx->task_id = atol(optarg);
		break;

	// show exit code history of a task
	case 'x':
		ctx->communication_func = exitcode_tasks;
		ctx->task_id = atol(optarg);
		break;
	
	// show last stdout of a task
	case 'o':
		ctx->communication_func = stdout_tasks;
		ctx->task_id = atol(optarg);
		break;
	
	// show last stderr of a task
	case 'e':
		ctx->communication_func = stderr_tasks;
		ctx->task_id = atol(optarg);
		break;
	
	// create a simple command
	case 'c':
		parse_timing_opt(ctx, argv);
		argv = argv + optind - 1;
		ctx->argv = argv;
		ctx->current = optind ;
		ctx->communication_func = create_tasks;
		break;

	// creates a sequence command
	case 's':
		parse_timing_opt(ctx, argv);
		argv = argv + optind - 1;
		ctx->argv = argv;
		ctx->current = optind ;
		ctx->communication_func = sequence_tasks;
		break;

	// creates a if command
	case 'i':
		parse_timing_opt(ctx, argv);
		argv = argv + optind - 1;
		ctx->argv = argv;
		ctx->current = optind ;
		ctx->communication_func = if_tasks;
		break;

	// creates a pipeline command
	case 'p':
		parse_timing_opt(ctx, argv);
		argv = argv + optind - 1;
		ctx->argv = argv;
		ctx->current = optind ;
		ctx->communication_func = pipeline_tasks;
		break;
	case 'A':
		ctx->communication_func = and_tasks;
		(*current)++;
		break;
	case 'O':
		ctx->communication_func = or_tasks;
		(*current)++;
		break;
	case 'A':
		ctx->communication_func = and_tasks;
		(*current)++;
		break;
	case 'O':
		ctx->communication_func = or_tasks;
		(*current)++;
		break;
	// set the minutes of a task
	case 'm':
		minutes = optarg;
		ctx->cmd.timing.minutes = parse_minutes(minutes);
		*current += 2;
		//printf("%s %s\n", optarg, argv[*current + 1]);
		break;
	
	// set the hours of a task
	case 'H':
		hours = optarg;
		ctx->cmd.timing.hours = parse_hours(hours);
		*current += 2;
		//printf("%s %s\n", optarg, argv[*current + 1]);
		break;
	
	// set up the days of the week of a task
	case 'd':
		daysofweek = optarg;
		ctx->cmd.timing.days = parse_days(daysofweek);
		*current += 2;
		//printf("%s %s\n", optarg, argv[*current + 1]);
		break;

	// NEVER SUPPOSED TO REACH IT HERE
	// sets up a task that has no specified time to run at
	case 'n':
		ctx->cmd.timing.days = 0;
		ctx->cmd.timing.hours = 0;
		ctx->cmd.timing.minutes = 0;
		
		break;
		
	// stop the daemon
	case 'q':
		ctx -> communication_func = terminate;
		break;
	
	// the directory for storing named pipes
	case 'P':
		if (!parse_custom_fifo_dir(ctx, optarg))
			return false;
		*current += 2;
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
	int		current = 0;
	extern int	opterr;
	const char	*shortopts = "+x:o:e:r:R:lcspim:H:d:nP:qbh";
	struct option	longopts[] = {
		// Options to manipulate tasks
		{"show-exit-codes-history", required_argument, NULL, 'x'},
		{"show-last-stdout", required_argument, NULL, 'o'},
		{"show-last-stderr", required_argument, NULL, 'e'},
		{"remove-task", required_argument, NULL, 'r'},
		{"list-tasks", no_argument, NULL, 'l'},

		// Option to create task
		{"create-simple-command", no_argument, NULL, 'c'},
		{"combine-sequence-command", no_argument, NULL, 's'},
		{"combine-pipeline-command", no_argument, NULL, 'p'},
		{"combine-if-command", no_argument, NULL, 'i'},
		{"combine-and-command", no_argument, NULL, 'A'},
		{"combine-or-command", no_argument, NULL, 'O'},
		{"minutes", required_argument, NULL, 'm'},
		{"hours", required_argument, NULL, 'H'},
		{"daysofweek", required_argument, NULL, 'd'},
		{"no-time", no_argument, NULL, 'n'},

		// General options
		{"pipes-directory", required_argument, NULL, 'P'},
		{"stop-daemon", no_argument, NULL, 'q'},
		{"debug", no_argument, NULL, 'b'},
		{"help", no_argument, NULL, 'h'},
		{NULL, no_argument, NULL, 0}
	};

	opterr = 0;
	while ((opt = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
		if (!opts_handle(ctx, opt, argv, &current)) {
			exit(ctx->exit_code);
		}
	}
	ctx->argv = argv;
	//FUCK THIS LINE : ctx->current = current + 1;
}

bool	parse_cli(struct s_data *ctx, int argc, char *argv[])
{
	ctx->cmd.timing.days = 0x7F;
	ctx->cmd.timing.hours = 0x00FFFFFF;
	ctx->cmd.timing.minutes = 0x0FFFFFFFFFFFFFFF;
	parse_options(ctx, argc, argv);

	argc -= optind;
	argv += optind;
	if (ctx->pipes_dir[0] == '\0')
		set_fifos_path_default(ctx);

	// add cmd and tasks parsing
	// parse_arguments(ctx, argc, argv);
	return true;
}
