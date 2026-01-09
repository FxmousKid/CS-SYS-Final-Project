#ifndef PARSE_CLI_H
# define PARSE_CLI_H

# include <limits.h>
# include <stdbool.h>
# include <getopt.h>

# include "structs.h"
# include "utils/utils.h" // IWYU pragma: keep
# include "utils/timing_utils.h"
# include "commands/list_tasks.h" // IWYU pragma: keep
# include "commands/exitcode_tasks.h"
# include "commands/std_tasks.h"
# include "commands/create_tasks.h"
# include "commands/remove_task.h"
# include "macros.h"

bool	parse_cli(struct s_data *ctx, int argc, char *argv[]);

#endif
