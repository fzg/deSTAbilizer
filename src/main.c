#include "desta.h"

char gV = 0;
char *gIn = 0;

int usage(const char *pn) {
  return printf("usage: %s [-v] in firm.bin\n", pn);
}

void die(const char str[]) {
  perror(str);
  exit(errno);
}

int main(int c, const char *v[]) {
  int fd, err;  fd = err = 0;

  err = parse_args(c, v);

  if (!gIn) return usage(v[0]);

  if ((fd = open(gIn, O_RDONLY)) == -1) {
   printf("Opening %s: %s", gIn, strerror(errno));
  }
  err = check_firmware(fd);
  cleanup();
  return(close(fd));
}

