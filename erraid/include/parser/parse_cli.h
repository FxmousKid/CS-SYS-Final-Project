#ifndef PARSE_CLI_H
# define PARSE_CLI_H

# include <limits.h>
# include <stdbool.h>
# include <getopt.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "structs.h"


bool	parser_cli(struct s_data *ctx, int argc, char *argv[]);

#endif 
