#include "ourhdr.h"
#include "myhdr.h"

volatile sig_atomic_t pipecaught;
static void sig_pipe(int signo)
{
    pipecaught = 1;
    return;
}

ssize_t      /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
    size_t nleft, nwritten;
    const char *ptr;
    Sigfunc *oldsig = signal_intr(SIGPIPE, sig_pipe);
    if (oldsig == SIG_ERR)
      err_sys("signal_intr error for SIGPIPE");

    ptr = vptr; /* can't do pointer arithmetic on void* */
    nleft = n;
    while (nleft > 0) {
      if ((nwritten = write(fd, ptr, nleft)) == -1 || pipecaught == 1) {
        pipecaught = 0;
        n = nwritten;
        goto END;
      }
        nleft -= nwritten;
        ptr   += nwritten;
    }

 END:
      if (signal_intr(SIGPIPE, oldsig) == SIG_ERR)
        err_sys("signal_intr error for SIGPIPE");

    return(n);
}
