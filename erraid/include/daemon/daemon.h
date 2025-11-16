#ifndef DAEMON_H
# define DAEMON_H

# include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <utils/utils.h>
#include <macros.h>
#include <signal.h>

bool daemonize();
bool is_daemon_running();
void setup_sig_handlers();

#endif