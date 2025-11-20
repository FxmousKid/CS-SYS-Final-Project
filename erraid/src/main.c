#include <stdlib.h>

#include "parser/parse_cli.h"
#include "utils/utils.h" // IWYU pragma: keep
#include "parser/parse_tasks.h"
#include "structs.h"
#include "daemon/daemon_loop.h"
#include "daemon/daemon.h"


// if passed data is made on little-endian architecture
// set by parser()
bool	isdle;

int main(int argc, char *argv[])
{
	struct s_data	ctx = {0};

	parser_cli(&ctx, argc, argv);
	isdle = ctx.is_data_le;

	if (!parse_tasks(&ctx))
		return EXIT_FAILURE;

	if (!daemonize(ctx.debug_mode)) {
		ERR_MSG("deamonize");
		return EXIT_FAILURE;
	}

	daemon_loop(&ctx);

	free_tasks(ctx.tasks);
	return EXIT_SUCCESS;
}
