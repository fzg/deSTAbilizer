#include "desta.h"
#include "ops.h"


int opCheck() {
  int fd, err;

  if ((fd = open(gIn, O_RDONLY)) != -1) {
    err = check_firmware(fd);
    close(fd);
    return err;
  }
  else return printf("Opening %s: %s", gIn, strerror(errno));
}

int opDump() {	// Dumping is just check_firmware with an extra flag
  if (gV >1) puts("opDump");
  if (!gOut) exit(usage()); // We need output dir

  int fd = prepare_output_dir();

  /*TODO:  Attempt to create outdir. Warn if already exists (implement -f to force overwrite)
           in check_firmware() for each module dump it (how to name it... by indice? by type?)
  */
  int err = opCheck();

  return err;
}

int opBuild() {
  int err;

  return err;
}

