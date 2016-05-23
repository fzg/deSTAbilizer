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
  int in_fd, out_fd, err;

  if (gV >1) puts("opDump");
  if (!gOut) exit(usage()); // We need output dir

  if ((in_fd = open(gIn, O_RDONLY)) == -1) die("Opening");

  if ((out_fd = prepare_output_dir()) < 0) die ("prepare_output_dir__outer"); // Not supposed to happen, but would be nicer to let errors flow up

  /*TODO:  Attempt to create outdir. Warn if already exists (implement -f to force overwrite)
           in check_firmware() for each module dump it (how to name it... by indice? by type?)
  */
  err = check_firmware(in_fd);

  return err;
}

int opBuild() {
  int err;

  return err = printf("Not implemented yet\n");
}

