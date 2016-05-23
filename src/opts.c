#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "desta.h"
//#include "util.h"

//#include "ops.h"
//#include "fortunes.h"

#define SZ(p) (sizeof(p)/sizeof(typeof(p)))

extern char *gIn, gV;

int setVerbosity(char **p) {
  ++gV;
  return 0;
}

int setForcefullness(char **p) {
  ++gF;
  return 0;
}

int setInputFile(char **p) {
  xstrdup(&gIn, *p);
  return 0;
}

int setOutputFile(char **p) {
  xstrdup(&gOut, *p);
  return 0;
}

int setDump(char **p) {
  gMode |= OPT_DUMP;
  return 0;
}

int setBuild(char **p) {
  gMode |= OPT_BUILD;
  return 0;
}


int setOverrideDefaults(char **p) {
  return 0;
}

