#ifndef TERMINATE
# define TERMINATE

# include <stdbool.h>

# include "structs.h"
# include "utils/buffer_utils.h"
# include "daemon/daemon.h"
# include "communication/fifo_api.h"

bool            terminate(struct s_data *ctx);

#endif
