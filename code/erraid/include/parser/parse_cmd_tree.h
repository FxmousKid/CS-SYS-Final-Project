#ifndef PARSE_CMD_TREE_H
# define PARSE_CMD_TREE_H

# include <stdbool.h>
# include <fcntl.h>
# include <unistd.h>
# include <dirent.h>
# include <fcntl.h>
# include <sys/stat.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "parser/parse_cmd_si.h" // IWYU pragma: keep
# include "parser/parse_cmd_sq.h" // IWYU pragma: keep
# include "parser/parse_cmd_pl.h" // IWYU pragma: keep
# include "utils/dir_cmd_utils.h" // IWYU pragma: keep


/** @brief parses the commands from the top-level cmd/ dir. */
struct s_cmd	*parse_cmd_tree(const char *path_cmd_dir);

#endif
