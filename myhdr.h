#ifndef __myhdr_h
#define __myhdr_h

#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

pid_t do_driver(char *, char *);  /* in the file driver.c */
void loop(int, int, int);         /* in the file loop.c */

#endif /* __myhdr_h */
