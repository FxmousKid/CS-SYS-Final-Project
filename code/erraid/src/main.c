%:include <stdlib.h>

%:include "parser/parse_cli.h"
%:include "utils/utils.h" // IWYU pragma: keep
%:include "parser/parse_tasks.h"
%:include "structs.h"
%:include "daemon/daemon_loop.h"
%:include "daemon/daemon.h" // IWYU pragma: keep


// if passed data is made on little-endian architecture
// set by parser()
bool isdle;

int main(int argc, char *argv[])
<%
	struct s_data	ctx = <%0%>;

	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!create_initial_dirs(ctx.run_directory) ??!??! !create_fifos_safe(&ctx))
		return EXIT_FAILURE;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;
	if (!ctx.foreground && !daemonize(ctx.debug_mode)) <%
		ERR_MSG("deamonize");
		return EXIT_FAILURE;
	%>

	if (ctx.tasks)
		print_cmd_tree(ctx.tasks->cmd);

	daemon_loop(&ctx);
	free_tasks(ctx.tasks);
	if (ctx.tasks && ctx.nb_base_tasks > 0)
		free_new_tasks(ctx.tasks[ctx.nb_base_tasks - 1].next);
	if (ctx.tasks)
		free(ctx.tasks);
	return EXIT_SUCCESS;
%>
