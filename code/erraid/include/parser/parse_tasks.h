#ifndef PARSE_TASKS_H
# define PARSE_TASKS_H

# include <stdbool.h>

# include "structs.h"
# include "parser/parse_cmd_tree.h" // IWYU pragma: keep
# include "parser/parse_timing.h" // IWYU pragma: keep

/**
 * @brief Parses all tasks in the given run_directory and fills
 * the ctx->tasks linked list
 *
 * @param ctx the context containing run_directory and tasks head
 *
 * @return 
 *  @retval true on success
 *  @retval false on failure
 */
bool	parse_tasks(struct s_data *ctx);

/**
 * @brief Frees the linked list of tasks
 *
 * @param tasks the head of the linked list of tasks
 */
void	free_tasks(struct s_task *tasks);

void	build_output_paths(struct s_task *task);

void	count_individual_cmds(struct s_cmd *cmd, int *count);

void	set_output_paths_last_command(struct s_cmd *cmd,
				      int last_cmd_id,
				      const char *stdout_path,
				      const char *stderr_path,
				      bool is_inside_pipeline);

#endif 
