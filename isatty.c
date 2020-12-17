#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
  int fd = 0;
  if (argc > 1)
    fd = atoi(argv[1]);
  return !isatty(fd);
}
