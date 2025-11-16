# include "daemon/daemon.h"


/**
 * @brief daemonize the process (see man 7 daemon for every steps)
 */
bool daemonize()
{
        pid_t pid;
        int fd;
        switch((pid = fork())){
        case -1:
                ERR_SYS("fork");
                return false;
        case 0:
                break;
        default:
                exit(EXIT_SUCCESS);
        }

        if (setsid() < 0){
                ERR_SYS("setsid");
                return false;
        }

        switch((pid = fork())){
        case -1:
                ERR_SYS("fork");
                return false;
        case 0:
                break;
        default:
                exit(EXIT_SUCCESS);
        }

        if (chdir("/") < 0){
                ERR_SYS("chdir");
                return false;
        }
        umask(0); // Always succeds (see man 2 umask)

        if (close(STDIN_FILENO) < 0){
                ERR_SYS("close stdin");
                return false;
        }
        if (close(STDOUT_FILENO) < 0){
                ERR_SYS("close stdout");
                return false;
        }
        if (close(STDERR_FILENO) < 0){
                ERR_SYS("close stderr");
                return false;
        }

        /* Replace below to redirect stdout and stderr to log files */
        fd = open("/dev/null", O_RDWR);
        if(dup2(fd, STDIN_FILENO) < 0){
                ERR_SYS("dup2");
                return false;
        }
        if (dup2(fd, STDOUT_FILENO)){
                ERR_SYS("dup2");
                return false;
        }
        if (dup2(fd, STDERR_FILENO)){
                ERR_SYS("dup2");
                return false;
        }
        if (fd > 2 && (close(fd) < 0)){
                ERR_SYS("close");
                return false;
        }
        return true;
}