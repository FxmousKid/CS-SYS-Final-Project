#include <stdio.h>
#include <stdlib.h>

#include "parser/parse_cli.h"
#include "structs.h"

int main(int argc, char *argv[])
{
	struct s_data ctx = {0};

	parser(&ctx, argc, argv);
	return EXIT_SUCCESS;
}
