# include "commands/terminate.h"

static bool     handle_reply(struct s_data *ctx, struct s_buffer *reply_buf)
{
        if (!buffer_append_uint16(reply_buf, OPCODE_OK))
                return false;
        if (!writefifo(ctx->fifo_reply, reply_buf->data, reply_buf->size))
                return false;
        return true;
}

bool            terminate(struct s_data *ctx)
{
        struct s_buffer reply_buf = {0};
        if (!buffer_init(&reply_buf, INITIAL_BUF_CAPACITY))
                return false;
        if (!handle_reply(ctx, &reply_buf))
                return false;
        buffer_free(&reply_buf);

        stop_daemon();

        return true;
}