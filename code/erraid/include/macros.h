#ifndef MACROS_H
# define MACROS_H

# define CMD_DIR		"cmd/"
# define STDOUT_FILE		"stdout"
# define STDERR_FILE		"stderr"
# define TEXIT_FILE     	"times-exitcodes"
# define TASKS_DIR		"tasks/"
# define PIPES_DIR      	"pipes/"
# define TMP_PATH       	"/tmp/"
# define ERRAID_PATH    	"erraid/"
# define DEV_NULL_DIR   	"/dev/null"
# define TIMING_FILE    	"timing"
# define TIMING_SIZE    	13


# define REQUEST_FIFO_NAME	"erraid-request-pipe"
# define REQUEST_FIFO_NAME_LEN	20
# define REPLY_FIFO_NAME	"erraid-reply-pipe"
# define REPLY_FIFO_NAME_LEN	18
# define INITIAL_BUF_CAPACITY   512
# define INITIAL_CMD_STR_SIZE   1024
# define NO_REDIRECT		-1

# ifndef LOGFILE_PATH 
#  define LOGFILE_PATH	"/tmp/erraid.log"
# endif

#endif
