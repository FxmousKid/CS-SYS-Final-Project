#include "utils/dir_cmd_utils.h"

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
		ERR_SYS("opendir: can't open %s", path_cmd_dir);
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
bool	readdir_s_dir(struct s_dir *dir, struct dirent **ent_ptr)
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


void	closedir_s_dir(struct s_dir *dir)
{
	if (dir->dir && closedir(dir->dir) == -1)
		ERR_SYS("closedir");
	dir->dir = NULL;
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
int	count_sub_cmds(const char *path)
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
	while ((readdir_s_dir(&dir, &ent))) {
		if (errno) {
			closedir_s_dir(&dir);
			return -1;
		}

		if (stat(dir.path, &st)) {
			ERR_SYS("stat");
			closedir(dir.dir);
			return -1;
		}

		remove_last_file_from_path(dir.path);
		count += is_ent_sub_dir(ent, &st);
	}
	closedir_s_dir(&dir);
	return (count);
}

bool	create_initial_dirs(const char *run_dir)
{
	char buf[PATH_MAX + 1];
	if (!create_dirs_recursive(run_dir, 0755)) {
		return false;
	}

	if (!build_safe_path(buf, PATH_MAX + 1, run_dir, TASKS_DIR))
		ERR_MSG("Failed to build tasks path");
	if(mkdir(buf, 0755) < 0 && errno != EEXIST) {
		ERR_SYS("mkdir(%s, 0755)", buf);
		return false;
	}
	// Always create pipes dir in run_dir even if -p specified
	if (!build_safe_path(buf, PATH_MAX + 1, run_dir, PIPES_DIR))
		ERR_MSG("Failed to build pipes path");
	if(mkdir(buf, 0755) < 0 && errno != EEXIST) {
		ERR_SYS("mkdir(%s, 0755)", buf);
		return false;
	}
	return true;
}

bool create_dirs_recursive(const char *path, mode_t mode)
{
	char *path_copy;
	char *p;
	struct stat st;

	// Check if path already exists
	if (stat(path, &st) == 0) {
		if (S_ISDIR(st.st_mode))
		return true;
		return false; // If it's a file
	}

	path_copy = strdup(path);
	if (!path_copy)
		return false;

	for (p = path_copy + 1; *p; p++)
	{
		if (*p == '/') {
		*p = '\0';

		// Create intermediate dir if it does not exist
		if (stat(path_copy, &st) != 0 &&
			mkdir(path_copy, mode) != 0 && errno != EEXIST) {

			free(path_copy);
			return false;

		}else if (!S_ISDIR(st.st_mode)) {
			// File exists with this name
			free(path_copy);
			return false;
		}
		*p = '/';
		}
	}

	// Create final directory
	if (mkdir(path_copy, mode) != 0 && errno != EEXIST) {
		free(path_copy);
		return false;
	}

	free(path_copy);
	return true;
}

bool recursive_rm(const char *path)
{
	DIR *dir;
	struct dirent *p;
	size_t path_len;
	struct stat statbuf;
	bool success = true;

	dir = opendir(path);
	path_len = strlen(path);

	if (!dir)
		return false;

	while (success && (p = readdir(dir))) {
		// Ignore '.' and '..'
		if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
			continue;

		char *buf = malloc(path_len + strlen(p->d_name) + 2);
		if (buf) {
			sprintf(buf, "%s/%s", path, p->d_name);
			if (!stat(buf, &statbuf)) {
				if (S_ISDIR(statbuf.st_mode))
					// Need to delete entries recursively from this dir
					success = recursive_rm(buf);
				else
					// Not a dir, just unlink it
					success = (unlink(buf) == 0);
			}
			free(buf);
		}
	}
	closedir(dir);

	if (success)
		// Finally delete directory
		success = (rmdir(path) == 0);

	return success;
}