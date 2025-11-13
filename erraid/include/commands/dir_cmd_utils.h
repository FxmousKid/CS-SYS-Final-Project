#ifndef DIR_CMD_UTILS_H
# define DIR_CMD_UTILS_H

# include <stdlib.h> // IWYU pragma: keep
# include <stdint.h>
# include <fcntl.h>
# include <sys/stat.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep
# include "commands/cmd_utils.h" // IWYU pragma: keep


/** @brief opens a DIR *, copies path in dir->path.
 * return true on success, false on error*/
bool	opendir_cmd(DIR **dir, const char *path_cmd_dir);

/** @brief wraps over readdir(3) and updates path in s_dir.
 * returns true on success, false on error or end of dir */
bool	readdir_s_dir(struct s_dir *dir, struct dirent **ent_ptr);

/** @brief closes the DIR * inside s_dir */
void	closedir_s_dir(struct s_dir *dir);

/** @brief returns false entries like "..", ".", reg files
 * and true if it's a sub directory. */
bool	is_ent_sub_dir(struct dirent *ent, struct stat *st);

/** @brief wraps over readdir(3), returns the number of
 * sub-commands (valid sub-directories) for the given path */
int	count_sub_cmds(const char *path);

#endif
