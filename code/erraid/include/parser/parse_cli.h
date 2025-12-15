#ifndef PARSE_CLI_H
# define PARSE_CLI_H

# include <limits.h>
# include <stdbool.h>
# include <getopt.h>

# include "structs.h"
# include "utils/utils.h" // IWYU pragma: keep


bool	parser_cli(struct s_data *ctx, int argc, char *argv[]);

#endif 
