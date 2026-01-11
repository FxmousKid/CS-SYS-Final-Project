
#include "utils/combine_utils.h"

static bool	check_taskid_exist(struct s_data *ctx, taskid_t taskid)
{
	struct stat	st = {0};
	char		taskid_string[44] = {0};
	char		path[PATH_MAX] = {0};

	snprintf(taskid_string, 44, "tasks/%" PRId64, taskid);
	build_safe_path(path, PATH_MAX, ctx->run_directory, taskid_string);
	//printf("check : %s\n", path);
	return (stat(path, &st) != -1 && S_ISDIR(st.st_mode));
}

static bool	move_cmd(char *oldpath, char *newpath)
{
	char		*oldbuf;
	char		*newbuf;
	size_t 		old_path_len;
	size_t 		new_path_len;
	DIR 		*dir;
	struct dirent	*p;
	struct stat 	statbuf;
	bool 		success = true;

	dir = opendir(oldpath);
	if (stat(newpath, &statbuf) == -1)
		mkdir(newpath, 0755);

	old_path_len = strlen(oldpath);
	new_path_len = strlen(newpath);
	if (!dir)
		return false;

	while (success && (p = readdir(dir))) {
		// Ignore '.' and '..'
		if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
			continue;
		oldbuf = malloc(old_path_len + strlen(p->d_name) + 2);
		newbuf = malloc(new_path_len + strlen(p->d_name) + 2);
		if (oldbuf && newbuf) {
			sprintf(oldbuf, "%s/%s", oldpath, p->d_name);
			sprintf(newbuf, "%s/%s", newpath, p->d_name);
			//printf("\noldbuf : %s\nnewbuf : %s\n", oldbuf, newbuf);
			if (!stat(oldbuf, &statbuf)) {
				if (S_ISDIR(statbuf.st_mode)) {
					// Need to delete entries recursively from this dir
					if (!strcmp(p->d_name, "cmd"))
						success = move_cmd(oldbuf, newpath);
					else
						success = move_cmd(oldbuf, newbuf);
				} else {
					if (!strcmp(p->d_name, "argv") || !strcmp(p->d_name, "type")) {
						//printf("move %s\n", p->d_name);
						rename(oldbuf, newbuf);
					}
					unlink(oldbuf);
					// Not a dir, just unlink it
				}
			}
			free(oldbuf);
			free(newbuf);
		}
	}
	closedir(dir);

	if (success)
		// Finally delete directory
		success = (rmdir(oldpath) == 0);

	return success;
}

bool	move_all_cmd(struct s_data *ctx, struct s_task *task, uint8_t *req)
{
	char		oldpath[PATH_MAX] = {0};
	char		newpath[PATH_MAX] = {0};
	char		taskid_string[44] = {0};
	uint64_t	taskid;
	uint32_t	nb_arg = 0;
	size_t		len = 4;


	memcpy(&nb_arg, req, 4);
	nb_arg = htobe32(nb_arg);
	//printf("nbarg : %u\n", nb_arg);

	for (uint32_t current = 0; current < nb_arg; current++) {
		memcpy(&taskid, req + len, 8);
		taskid = htobe64(taskid);
		if (!check_taskid_exist(ctx, taskid)) {
			printf("existe pas : %" PRId64 "\n", taskid);
			return false;
		}
		taskid = 0;
		len += 8;
	}

	len = 4;
	for (uint32_t current = 0; current < nb_arg; current++) {
		memcpy(&taskid, req + len, 8);
		taskid = htobe64(taskid);
		snprintf(taskid_string, 44, "tasks/%" PRId64, taskid);
		build_safe_path(oldpath, PATH_MAX, ctx->run_directory, taskid_string);

		memset(taskid_string, 0, 44);
		snprintf(taskid_string, 44, "cmd/%u", current);
		build_safe_path(newpath, PATH_MAX, task->path, taskid_string);

		//printf("oldpath : %s\nnewpath : %s\n", oldpath, newpath);
		move_cmd(oldpath, newpath);
		
		del_task_node(ctx, taskid);
		len += 8;
	}
	return true;
}
