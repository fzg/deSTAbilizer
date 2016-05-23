#ifndef _DESTA_H_
#define _DESTA_H_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <openssl/md5.h>


#define VERB 2 //verbosity

extern char gV, *gIn, *gOut, gMode, gF;

#include "arg_parser.h"
#include "fmt.h"
#include "util.h"
#include "ops.h"

int usage();
int check_firmware(int);
void die(const char[]);
void nsdigest(unsigned char *digest, char *data_buf, int data_len);
int print_digest_x(unsigned char *digest, size_t len);
int print_digest(unsigned char *digest);
int whereami(int fd);
int prepare_output_dir();

#endif // _DESTA_H_
