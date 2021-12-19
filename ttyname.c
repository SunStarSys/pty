#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
  int fd = 0;
  char *rv;
  if (argc > 1)
    fd = atoi(argv[1]);
  rv = ttyname(fd);
  if (rv != NULL) {
    printf("%s\n", rv);
    return 0;
  }
  return 1;
}
