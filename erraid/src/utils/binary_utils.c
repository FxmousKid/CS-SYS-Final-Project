# include "utils/binary_utils.h"
# include "utils/utils.h"

bool	find_binary_path(const char* restrict bin_name, char* restrict bin_path)
{
	bool	found;
	char	*path_tmp = NULL;
	char	path_copy[PATH_MAX + 1] = {0};

	// if binary is relative or absolute
	if (access(bin_name, X_OK) == 0) {
		strcpy(bin_path, bin_name);
		return true;
	}
	
	// Else it checks if it is in PATH
	char *path_env = getenv("PATH");
	if (!path_env) {
		ERR_MSG("PATH environment variable not found");
		return false;
	}
	
	strcpy(path_copy, path_env);
	path_tmp = strtok(path_copy, ":");
	found = false;
	while (path_tmp && !found) {
		if (snprintf(bin_path, PATH_MAX, "%s/%s", path_tmp, bin_name) >= PATH_MAX) {
			path_tmp = strtok(NULL, ":");
			continue;
		}
		
		if (access(bin_path, X_OK) == 0)
			found = true;

		path_tmp = strtok(NULL, ":");
	}
	return found;
}
