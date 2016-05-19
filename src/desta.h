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

#include "fmt.h"

int check_firmware(int);
void die(const char[]);
void nsdigest(char *digest, char *data_buf, int data_len);


#endif

