#ifndef FIFO_API_H
# define FIFO_API_H

# include <stdbool.h>
# include <sys/types.h>
# include <sys/stat.h>

# include "structs.h" // IWYU pragma: keep
# include "utils/utils.h" // IWYU pragma: keep

/**
 * @brief Creates the fifo for the 1st time if they dont exist
 *
 * @param ctx the structure that contains the path of both FIFOs
 *
 * @return 
 *  @retval true on success
 *  @retval false on failure and logs error
 */
bool	create_fifos_safe(struct s_data *ctx);
bool	writefifo(const char *path, const void *buf, size_t len);
bool	readfifo(const char *path, struct s_reply *reply);


#endif
