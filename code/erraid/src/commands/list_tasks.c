#include "commands/list_tasks.h"

bool	list_tasks(struct s_data *ctx)
{	
	writefifo(ctx->fifo_reply, "Yo mama", 7);
	return true;
}
