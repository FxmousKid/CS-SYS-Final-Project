#include <stdlib.h>

#include "structs.h"
#include "parser/parse_cli.h"

int main(int argc, char *argv[])
{
	struct s_data ctx = {0};

	parse_cli(&ctx, argc, argv);

	// If no option given, just return success
	if (!ctx.communication_func)
		return 0;

	return !ctx.communication_func(&ctx);
}
