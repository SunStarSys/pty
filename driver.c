#include <sys/socket.h>
#include "ourhdr.h"
#include "myhdr.h"

pid_t
do_driver(char *driver)
{
    pid_t child;
    int pipe[2];

    /* create a stream pipe to communicate with the driver */
    if (socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, pipe) < 0)
        err_sys("can't create socketpair");

    if ( (child = fork()) < 0)
        err_sys("fork error");

    else if (child == 0) {   /* child */
        close(pipe[1]);

        /* stderr for driver is the controlling master tty on orig STDIN */
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            err_sys("dup2 error to sterr");
        /* stdin for driver */
        if (dup2(pipe[0], STDIN_FILENO) != STDIN_FILENO)
            err_sys("dup2 error to stdin");
        /* stdout for driver */
        if (dup2(pipe[0], STDOUT_FILENO) != STDOUT_FILENO)
            err_sys("dup2 error to stdout");
        close(pipe[0]);

        execlp(driver, driver, (char *) 0);
        err_sys("execlp error for: %s", driver);
    }

    close(pipe[0]);  /* parent */

    if (dup2(pipe[1], STDIN_FILENO) != STDIN_FILENO)
        err_sys("dup2 error to stdin");
    if (dup2(pipe[1], STDOUT_FILENO) != STDOUT_FILENO)
        err_sys("dup2 error to stdout");
    close(pipe[1]);

    /* Parent returns, but with stdin and stdout connected
       to the driver. */
    return child;
}
