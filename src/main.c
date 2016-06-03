#include "desta.h"

char gF = 0, gV = 0, gMode = OPT_CHECK;
char *gIn = 0, *gOut = 0, *gCfg = 0;
const char *__progname = 0;
int usage() {
  return printf("usage: %s [-v] [--dump|--build|--cfg key=value|--ckconf] -i in [-o] out\n\n \
   --dump takes an input file and an output directory\n \
   --build takes an input directory and an output file\n \
   --cfg takes a key=value argument, an input file and an\n \
     output file. Optionally --no[en|de]code\n \
   --ckconf takes an input file\n", __progname);
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
  if (gMode & OPT_CFG) {
    err = opCfg();
  }
  if (gMode & OPT_CKCONF) {
    err = opCkConf();
  }
  if (gV > 0) puts("[main]\t\tDone.");
 cleanup();
 return err;
}

