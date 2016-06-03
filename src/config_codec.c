#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
/* alg:
beg:
 consume until ':'
 if substr(beg, pos:) == key {
   value = str(pos:)
   if values match return
   else if value fit overwrite
        else copy block of old key value and overwrite, goto INSERTONEND
 else
   consume until NULL
   beg =  string(pos:)[strlen(string(pos:)+2)]
INSERTONEND:
  ptr = strchr(pos, ':');
   if pos < size
     if string(pos:)[strlen(string(pos:)+2)] != 0 continue
     else memcpy it there

*/

#include "desta.h"
#include "crc.c"

#define NVRAM_BASE 0xc0
#define NVRAM_KEYSTORE_OFF (20 + NVRAM_BASE)

/*
static void xputs(const char *str, const char sep) {
  static off_t pos;
  pos = strchr(str, sep) - str;
  for (off_t i = 0; i < pos;++i) putchar(str[i]);
  putchar('\n');
}
*/

int verify_crc(char *buffer, size_t length) {
  uint8_t file = *(uint8_t *)(buffer+8);
  uint8_t comp = compute_crc(buffer, length);
  return (!(comp == file));
}

uint8_t compute_crc(char *buffer, size_t length) {
  if (strncmp(buffer, "HSLF", 4)) {
    printf("Bad magic\n abort\n");
  }
  u_short last_entry = *(u_short *)(buffer+4);
  u_short rounded = (last_entry+3) & 0xFFFFFFFC;
  if (gV > 2) printf("Rounded %d to %d\n", *(u_short *)(buffer+4), rounded);
  uint8_t comp = *(uint8_t *)(buffer+8);
  unsigned char final = 0;
  uint8_t crc = hndcrc8((unsigned char *)buffer+9, 11, 255);
  uint8_t c2c = hndcrc8((unsigned char *)buffer+20, rounded-20, ((crc << 16) & 0xFF0000) >> 16);
  final |= c2c;
  if (gV > 1) printf("Computed crc: %d (value previously in file: %d)\n", final, comp);
  return final;
}

static char _nvram_set(char *buffer, size_t length, const char *key, const char *value) {
  size_t key_len, len;
  u_short last_used = *(u_short *)(buffer + 0xc4);
  last_used += 186;

//  last_used -= (0x10 + 0x10 - 7 - 0x1f);//FIXME triple-check
  unsigned char keyval_count = (unsigned char)buffer[0xc8];
  char *ptr = buffer + NVRAM_KEYSTORE_OFF, *tmp = NULL, *tmploc = NULL;

  uint8_t my_crc = compute_crc(buffer+NVRAM_BASE, 0x8000);
  uint8_t file_crc = *(uint8_t *)(buffer+NVRAM_BASE+8);
  if (my_crc != file_crc) die("CRC mismatch! Aborting.");

  if (gV > 2) printf("tail is %s(%d) with CRC=%d\n", buffer+last_used, last_used, keyval_count);
  if (gV > 2) printf("10 before tail is %s\n", buffer+last_used-10);

  if (!value) {
    printf("Warning: attempted to add key %s without value!\n Not doing anything.\n", key);
    return(1);
  }

  while ((ptr - buffer < length) && *ptr) {
//    while (!*ptr) ++ptr; // skip extra zeros
//    printf("ptr: %s\n", ptr);
    key_len = strlen(ptr) - strlen(strchr(ptr, '='));
  //  printf("key_len %d \n", key_len, ptr);
 //   xputs(ptr, '=');
    if (strncmp(ptr, key, strlen(key))) { // not that key
//      printf("key not matched! %s, %s -> %s\n", key, ptr, value);
      ptr += key_len + 1;
      len = strlen(ptr); // skip the value
      ptr += len + 1;
    } else { // got a key match :)
      if (gV > 3) printf("key matched! %s, %s -> %s\n", key, ptr, value);
      ptr += key_len + 1;
      if (strcmp(value, ptr)) { //
        if (strlen(ptr) != strlen(value)) {
          ptr -= (key_len +1); // ptr = where to move memory to, ptr+strlen(ptr)+1 = where to start move from
 /*
     we move from next kvpair to end of used zone to cur kvpair
     then insert at end and update end
  */
          memmove(ptr, ptr + strlen(ptr)+1, last_used - (NVRAM_KEYSTORE_OFF + (buffer - ptr)+strlen(ptr))); // compress
          last_used -= strlen(ptr); // update tail
          memset(ptr+last_used, 0, strlen(ptr));
          if (gV > 3) puts("goto INSERTONEND");
          goto INSERTONEND;
        } else {
          memset(ptr, 0, strlen(ptr));
          strncpy(ptr, value, strlen(value));
          return (0);
        }
      } return (0);
    }
  }
  if (gV > 2) printf("Note: couldn't find %s=%s inside NVRAM!\nAppening new value\n", key, value);
//  buffer[0xc8] += 1; Is not number of entries but crc8!
//    buffer[0xc8] = compute_crc(buffer+NVRAM_BASE, 0x8000);
INSERTONEND:
   ptr = buffer + last_used;

  while (ptr - buffer < length) {
    len = strlen(ptr); // a simplifier
    if (!len) {
      if (gV > 3) printf("At end we have %ld vs 0x8000\n", ptr - buffer + strlen(key) + strlen(value) + 2);
      if (ptr - buffer + strlen(key) + strlen(value) + 2 <= length) {
        strcpy(ptr, key); *(ptr+strlen(key)) = '='; strcpy(ptr+strlen(key)+1, value);
        last_used += ( strlen(key)+2+strlen(value));
        *(u_short *)(buffer + 0xc4) = last_used - 186;

        return(0);
      } else {
         printf("Error, couldn't fit moved %s=%s inside NVRAM! Copying back old value\n", key, value); // so copy back
         strcpy(tmploc, tmp); // actually we could abort since we're not ondevice but heh aftertought
         return (1);
      }
    } else ptr += strlen(ptr) + 1;  // skip entries
  }
  return (0);
}

int nvram_set(char *buffer, const char *key, const char *value) {
 if (!_nvram_set(buffer, 0x8000, key, value)) {
    buffer[0xc8] = compute_crc(buffer+NVRAM_BASE, 0x8000);
    return 0;
 }
 return -1;
}


char *encode(char *buf, int size) {
 int i = 0;
 do {
  buf[i] = ~(((buf[i]&3) << 6) | ((unsigned int)(unsigned char)buf[i] >> 2));
  ++i;
 } while (size != i);
 return buf;
}

char *decode(char *buf, int size) {
 int i = 0;
 do {
  buf[i] = ~(((unsigned int)(unsigned char)(buf[i]) >> 6) | 4 * buf[i]);
  ++i;
 } while (size != i);
 return buf;
}

#if 0
int main() {
 char buf[0x8000];
 int iif, oof;

 iif = open("in.cfg", O_RDONLY);
 read(iif, buf, 0x8000);
 decode(buf, 0x8000);
 nvram_set(buf, "nothing", 0);
// puts("adding new value");
// nvram_set(buf, "new_val", "1");
// puts("changing value with same size");
// nvram_set(buf, "locale", "FR");
// puts("changing value with bigger size");
// nvram_set(buf, "rip_flush", "123456");
 nvram_set(buf, "new_val", "1");
 nvram_set(buf, "nothing", 0);

 oof = open("out.cfg", O_WRONLY|O_CREAT);
 encode(buf, 0x8000);
 write(oof, buf, 0x8000);
 close(oof);
 close(iif);
 iif = open("out.cfg", O_RDONLY);
 read(iif, buf, 0x8000);
 decode(buf, 0x8000);
 nvram_set(buf, "nothing", 0);
// write(oof, buf, 0x8000);
}
#endif
