#ifndef MACROS_H
# define MACROS_H

# ifndef LOGFILE_PATH 
#  define LOGFILE_PATH	"/tmp/tadmor.log"
# endif
# define TMP_PATH               "/tmp/"
# define ERRAID_PATH            "erraid/"
# define PIPES_DIR              "pipes/"
# define REQUEST_FIFO_NAME	"erraid-request-pipe"
# define REQUEST_FIFO_NAME_LEN	20
# define REPLY_FIFO_NAME	"erraid-reply-pipe"
# define REPLY_FIFO_NAME_LEN	18
# define MAX_LINE_LEN           2048
# define MAX_TIMING_LEN         512
# define INITIAL_CMD_STR_SIZE   512



#endif
