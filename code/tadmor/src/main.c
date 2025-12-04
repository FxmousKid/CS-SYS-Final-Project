#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "parser/parse_cli.h"


#include <sys/types.h>
#include <sys/stat.h>
void	communicate(struct s_data *ctx)
{
	char buf[10000] = {0};
	snprintf(buf, 10000, "%s%s", ctx->pipes_dir, REQUEST_FIFO_NAME);
	if (mkfifo(buf, 0644) < 0) {
		ERR_SYS("mkfifo: couldn't mkfifo at %s", buf)
		return;
	}
}

int main(int argc, char *argv[])
{
	struct s_data ctx = {0};

	parse_cli(&ctx, argc, argv);

	// communicate(&ctx);
	
	return EXIT_SUCCESS;
}
