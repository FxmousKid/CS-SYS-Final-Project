#ifndef COMMANDS_H
# define COMMANDS_H

# include "commands/list_tasks.h" // IWYU pragma: keep
# include "structs.h" // IWYU pragma: keep
# include "communication/fifo_api.h" // IWYU pragma: keep

void handle_all_requests(struct s_data *ctx);

#endif
