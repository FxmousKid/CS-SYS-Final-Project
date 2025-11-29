#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "parser/parse_cli.h"

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;


	struct s_data ctx = {0};

	parse_cli(&ctx, argc, argv);
	
	return EXIT_SUCCESS;
}
