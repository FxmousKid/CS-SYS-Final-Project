#include <stdlib.h>

#include "structs.h"
#include "parser/parse_cli.h"
#include "communication/fifo_api.h"

void	test(struct s_data *ctx)
{
	struct s_reply reply = {0};
	readfifo("Makefile", &reply);
	(void)ctx;
	reply.buf[200] = '\0';
	printf("%s\n", reply.buf);
	free(reply.buf);
}

int main(int argc, char *argv[])
{
	struct s_data ctx = {0};

	parse_cli(&ctx, argc, argv);

	// Segfault if no option given
	return !ctx.communication_func(&ctx);
}
