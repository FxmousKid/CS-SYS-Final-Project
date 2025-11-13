# include "utils/binary_utils.h"
# include "utils/utils.h"

bool	find_binary_path(const char *command, char *full_path)
{
	// Checks if command is an absolute path
	if (command[0] == '/') {
		if (access(command, X_OK) == 0) {
			strcpy(full_path, command);
			return true;
		}
		return false;
	}
	
	// Else it checks if it is in PATH
	char *path_env = getenv("PATH");
	if (!path_env) {
		ERR_MSG("PATH environment variable not found");
		return false;
	}
	
	char *path_copy = strdup(path_env);
	if (!path_copy) {
		ERR_SYS("strdup");
		return false;
	}
	
    	// Parse the PATH
	char *dir = strtok(path_copy, ":");
	bool found = false;
	
	while (dir && !found) {
		if (snprintf(full_path, PATH_MAX, "%s/%s", dir, command) >= PATH_MAX) {
			dir = strtok(NULL, ":");
			continue;
		}
		
		if (access(full_path, X_OK) == 0) {
			found = true;
		}
		dir = strtok(NULL, ":");
	}
	
	free(path_copy);
	return found;
}