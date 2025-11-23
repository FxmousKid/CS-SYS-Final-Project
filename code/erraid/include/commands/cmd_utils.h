#ifndef CMD_UTILS_H
# define CMD_UTILS_H

# include <fcntl.h>
# include <sys/stat.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep

/** @brief extracts the type of cmd dir. */
enum cmd_type	get_cmd_type(const char *path_cmd_dir);

/** @brief allocates and returns a command, NULL on failure */
struct s_cmd	*get_cmd_struct(void);

/** @brief Frees a command */
void	free_command_rec(struct s_cmd *cmd);

/** @brief Pretty-prints a command tree */
void	print_cmd_tree(struct s_cmd *cmd);

/** @brief remove the last file from a path. */
void	remove_last_file_from_path(char *path);

#endif
