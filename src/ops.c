#include <sys/stat.h>

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

int opCfg() {
  int err, in_fd, out_fd;
  char buf[0x8000], *value;

  if (gV > 0) puts("[cfg]\t\tSetting nvram variable");
  if ((in_fd = open(gCfg, O_RDONLY)) == -1) die("Opening input file");
  if ((err = read(in_fd, buf, 0x8000)) != 0x8000) die("Couldn't read enough bytes!");
  if (!(value = strchr(gCfg, '='))) die("Must provide key=value!");
  *value = 0;
  ++value;
  if (!(gMode & OPT_NODEC)) { if (gV) puts("Decoding"); decode(buf, 0x8000); }
  if ((err = nvram_set(buf, gCfg, value))) die("Something went wrong setting nvram variable!");
  if (!(gMode & OPT_NOENC)) { if (gV) puts("Encoding"); encode(buf, 0x8000); }
  if ((out_fd = open(gOut, O_CREAT | O_WRONLY, 0644)) == -1) die("Opening output file");
  if ((err = write(out_fd, buf, 0x8000)) != 0x8000) die("Couldn't write enough bytes!");
  return 0;
}

int opCkConf() {
  int in_fd, err;
  char *buf;

  struct stat s;

  if ((in_fd = open(gCfg, O_RDONLY)) == -1) die("Opening input file");
  if ((fstat(in_fd, &s))) die("Stat'ing config file");


  if (!(buf = malloc(s.st_size))) die("Allocating for config file failed;");

  if (gV > 0) printf("[cfg]\t\tChecking NVRAM CRC %s\n", gCfg);

  if ((err = read(in_fd, buf, s.st_size)) != s.st_size) die("Couldn't read enough bytes!");
  if ((err = fixfile(buf, s.st_size))) die ("File CRC mismatch! Router will reject your file.");

  if (!(gMode & OPT_NODEC)) { if (gV) puts("Decoding"); decode(buf, 0x8000); } else {
   if (gV) puts("Warning: no decode!\n");
  }
  if ((err = verify_crc(buf, 0x8000))) die("Config CRC mismatch! Flash this at your own risk!");
  if (gV > 0) puts("[cfg]\t\tAll good!");
  free(buf);
  return err;
}
