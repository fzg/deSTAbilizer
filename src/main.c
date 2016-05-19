#include "desta.h"


int usage(const char *pn) {
  return printf("usage: %s firm.bin\n");
}

void die(const char str[]) {
  perror(str);
  exit(errno);
}

int main(int c, const char **v) {
  if (c < 2) return usage(v[0]);

  int fd, err;  fd = err = 0;

  if ((fd = open(v[1], O_RDONLY)) == -1) {
   die("Opening: ");
  }
  err = check_firmware(fd);
  return(close(fd));
}

