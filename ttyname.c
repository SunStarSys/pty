#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
  int fd = 0;
  if (argc > 1)
    fd = atoi(argv[1]);
  printf("%s\n", ttyname(fd));
  return 0;
}
