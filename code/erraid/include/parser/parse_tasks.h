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

#endif 
