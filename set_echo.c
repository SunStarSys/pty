#include <getopt.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <util.h>
#else
#if defined(__linux__)
#include <pty.h>
#else
#include <termios.h>
#endif
#endif
#include <sys/wait.h>
#include "ourhdr.h"
#include "myhdr.h"

static void set_echo(int fd)  /* turn on echo (for slave pty) */
{
    struct termios stermios;

    if (tcgetattr(fd, &stermios) < 0)
        err_sys("tcgetattr error");

    stermios.c_lflag |= (ECHO | ECHOE | ECHOK | ECHONL);

    if (tcsetattr(fd, TCSANOW, &stermios) < 0)
        err_sys("tcsetattr error");
}

int main(int argc, char *argv[]) {
  set_echo(STDIN_FILENO);
  return 0;
}
