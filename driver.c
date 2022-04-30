#include <sys/socket.h>
#include "ourhdr.h"
#include "myhdr.h"

pid_t
do_driver(char *driver, char* slave_name)
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

        char *user=getenv("USER"), *path=getenv("PATH"), *home=getenv("HOME"),
          *term=getenv("TERM"), *moz=getenv("MOZILLA"),
          *euser=malloc(strlen(user)+6), *epath=malloc(strlen(path)+6),
          *ehome=malloc(strlen(home)+6), *eterm=malloc(strlen(term)+6),
          *emoz=NULL, *eslave_name=malloc(strlen(slave_name)+11);

        if (moz != NULL) {
          emoz=malloc(strlen(moz)+9);
          sprintf(emoz,"MOZILLA=%s", moz);
        }
        sprintf(euser,"USER=%s", user);
        sprintf(epath,"PATH=%s", path);
        sprintf(ehome,"HOME=%s", home);
        sprintf(eterm,"TERM=%s", term);
        sprintf(eslave_name,"STTY_NAME=%s", slave_name);

        char* envp[] = {euser, epath, ehome, eterm, eslave_name, emoz, NULL};
        extern char **environ;
        environ = envp;

        execlp(driver, driver, (char *)NULL);
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
