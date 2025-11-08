#ifndef PARSE_TREE_H
# define PARSE_TREE_H

# include <stdbool.h>
# include <fcntl.h>
# include <unistd.h>
# include <stddef.h>
# include <stdlib.h>
# include <inttypes.h>

# include "utils/utils.h" // IWYU pragma: keep
# include "parser/parse_cmd.h" // IWYU pragma: keep
# include "structs.h"
# include "structs.h"

# define PATH_CMD "/home/fxmouskid/Desktop/SY5-poulet/sujet/exemples-arborescences/tmp-username-erraid/tasks/0/cmd/argv"

bool	parse_tree(struct s_data *ctx);

#endif
