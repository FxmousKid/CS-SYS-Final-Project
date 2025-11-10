#include "commands/dir_cmd_utils.h"

/**
 * @brief Opens a directory in dir->dir and copies in dir->path
 * the path of the dir
 *
 * @param dir the struct to use
 * @param path_cmd_dir the path to open
 *
 * @return 
 *  @retval true on success
 *  @retval false on failure
 */
bool	opendir_cmd(DIR **dir, const char *path_cmd_dir)
{
	if (!(*dir = opendir(path_cmd_dir))) {
		ERR_SYS("opendir");
		return false;
	}
	// if dir->path is empty (opening first cmd/ )
	// otherwise readdir_cmd will fill it already
	return true;
}

/**
 * @brief wraps over readdir(3) and updates path in s_dir
 *
 * @param dir the struct containing path and DIR *
 * @param ent the dirent pointer
 * 
 * @return 
 *  @retval true on success
 *  @retval false on failure
 */
bool	readdir_cmd(struct s_dir *dir, struct dirent **ent_ptr)
{
	errno = 0;
	struct dirent *ent = readdir(dir->dir);	

	if (!ent && errno) {
		ERR_SYS("readdir");
		return false;
	}
	if (!ent)
		return false;

	// if len(path) + len("/") + len(file) > PATH_MAX
	if (strlen(dir->path) + strlen(ent->d_name) + 1 > PATH_MAX) {
		ERR_MSG("path too long");
		return (false);
	}
	if (dir->path[strlen(dir->path) - 1] != '/')
		dir->path[strlen(dir->path)] = '/'; 
	strcat(dir->path, ent->d_name);

	*ent_ptr = ent;
	return true;
}


void	closedir_cmd(struct s_dir *dir)
{
	if (dir->dir)
		closedir(dir->dir);
	dir->dir = NULL;
	dir->path[0] = '\0';
}

bool	is_ent_sub_dir(struct dirent *ent, struct stat *st)
{
	// if ent is not a directory
	if (!(st->st_mode & S_IFDIR))
		return false;

	// if name == ".." or name == "."
	if (!strcmp(ent->d_name, "..") || !strcmp(ent->d_name, "."))
		return false;
	

	return true;
}

/**
 * @brief Counts the number of sub-commands in a given directory
 *
 * @param dir The directory structure containing the path and DIR *
 * @return int The number of sub-commands found
 */
int	count_sub_cmds(char *path)
{
	int		count;
	struct dirent	*ent = NULL;
	struct stat	st = {0};
	struct s_dir	dir = {0};


	strcpy(dir.path, path);
	if (!opendir_cmd(&dir.dir, path))
		return -1;

	count = 0;
	errno = 0;
	while ((readdir_cmd(&dir, &ent))) {
		if (errno)
			return -1;

		if (stat(dir.path, &st)) {
			ERR_SYS("stat");
			return -1;
		}
		
		if (!is_ent_sub_dir(ent, &st)) {
			remove_last_file_from_path(dir.path);
			continue;
		}
		count++;
		remove_last_file_from_path(dir.path);
	}
	closedir(dir.dir);
	return (count);
}


