#ifndef MACROS_H
# define MACROS_H

# define CMD_DIR	"cmd/"
# define STDOUT_FILE	"stdout"
# define STDERR_FILE	"stderr"
# define TEXIT_FILE     "times-exitcodes"
# define TASKS_DIR	"tasks/"
# define DEV_NULL_DIR   "/dev/null"
# define TIMING_FILE    "timing"
# define TIMING_SIZE    13
# ifndef LOGFILE_PATH 
#  define LOGFILE_PATH	"/tmp/erraid.log"
# endif
# define REQUEST_FIFO_NAME "erraid-request-pipe"
# define REPLY_FIFO_NAME   "erraid-reply-pipe"

#endif
