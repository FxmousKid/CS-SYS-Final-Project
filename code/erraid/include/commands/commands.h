#ifndef COMMANDS_H
# define COMMANDS_H

# include <poll.h> // IWYU pragma: keep

# include "commands/list_tasks.h" // IWYU pragma: keep
# include "commands/exitcode_tasks.h" // IWYU pragma: keep
# include "commands/std_tasks.h"
# include "commands/create_tasks.h"
# include "commands/combine_tasks.h"
# include "commands/remove_task.h"
# include "commands/terminate.h"
# include "structs.h" // IWYU pragma: keep
# include "communication/fifo_api.h" // IWYU pragma: keep

void handle_all_requests(struct s_data *ctx, struct pollfd *pfds);

#endif
