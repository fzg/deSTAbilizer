#include "desta.h"

char gV = 0, gMode = OPT_CHECK;
char *gIn = 0, *gOut = 0;
const char *__progname = 0;
int usage() {
  return printf("usage: %s [-v] [--dump|--build] -i in [-o] out\n\n \
   --dump takes an input file and an output directory\n \
   --build takes an input directory and an output file\n", __progname);
}

void die(const char str[]) {
  perror(str);
  exit(errno);
}

int main(int c, const char *v[]) {
  int err = 0;

  __progname = v[0];
  err = parse_args(c, v);

  if (!gIn) return usage();

  if (gMode == OPT_CHECK) {  // neither --dump nor --build, just check
    err = opCheck();
  }
  if (gMode & OPT_DUMP) {
    err = opDump();
  }
 cleanup();
 return err;
}

