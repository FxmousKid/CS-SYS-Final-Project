#include <stdio.h>
#include <stdlib.h>

#include "parser/parse_cli.h"
#include "parser/parse_tree.h"
#include "utils/utils.h" // IWYU pragma: keep
#include "structs.h"


int main(int argc, char *argv[])
{
	struct s_data ctx = {0};

	parser(&ctx, argc, argv);
	parse_tree(&ctx);


	return EXIT_SUCCESS;
}
