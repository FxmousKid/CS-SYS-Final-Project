#ifndef PARSE_CMD_H
# define PARSE_CMD_H

# include <stdbool.h>
# include <fcntl.h>
# include <unistd.h>
# include <stddef.h>
# include <stdlib.h>
# include <inttypes.h>

# include "utils/utils.h" // IWYU pragma: keep
# include "structs.h"
# include "structs.h"

bool	fparse_cmd(struct s_data *ctx, const char *path, struct s_cmd *cmd);

#endif
