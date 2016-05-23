#include "desta.h"


#include <string.h>
#include <unistd.h>

#define NUMARGS(...)  (sizeof((void*[]){__VA_ARGS__})/sizeof(void**))
#define XFREE_N(...)  (xfree_n(NUMARGS(__VA_ARGS__), __VA_ARGS__))

//extern char gV;

void xfree(char **b) {
  if (!*b) return;
  free(*b); *b = NULL;
}

void xfree_n(int n, ...) {
 va_list x;
 int i;
 if (gV > 1) printf("xfree_n: %d", n);
 va_start(x, n);
 for (i = 0; i < n; ++i)
  xfree(va_arg(x, char **));
 va_end(x);
}

void *xmalloc(void **b, size_t s) {
  if (*b) xfree((char**)b);
  if (!(*b = malloc(s))) {
   perror("xmalloc");
   exit(EXIT_FAILURE);
  } else return *b;
}

void xstrdup(char **d, const char *s) {
  if (*d) xfree((char**)d);
  if (!(*d = strdup(s))) {
   perror("xstrdup");
   exit(EXIT_FAILURE);
  }
}

void xsleep(int n) {
  const char x[] = "/|\\-";
  n *= 3;
  do {
   if (gV) {
    putchar(x[n%strlen(x)]);
    fflush(stdout);
   }
   usleep(1000000/3);
   if (gV) putchar('\b');
  } while (--n > 0);
  if (gV) {
   putchar('\b');
   fflush(stdout);
   puts(" \b");
  }
}

int max(int a, int b) {
        return (a > b)? a : b;
}

void cleanup() {
  if (gV) { printf("[cleanup]\t..."); fflush(stdout); }
    xfree(&gIn);
  if (gV) puts(" done.");
  return;
}




int whereami(int fd) {
  int x = lseek(fd, 0, SEEK_CUR);
  return printf("FD %d, Position: 0x%x\n", fd, x);
}

int print_digest_x(unsigned char *digest, size_t len) {
  if (gV > 1) printf("print_digest: ");
  for (int x = 0; x < len;x+=4) printf("%02x ", digest[x]);
  putchar('\n');
  return(len/4+1);
}

int print_digest(unsigned char *digest) {
  return (print_digest_x(digest, 0x10));
}

void nsdigest(unsigned char *digest, char *data_buf, int data_len) {
  signed int i, j, ongoing;
  char *ptr, offset, next;
  MD5_CTX md5ctx;
  unsigned char block[32] = {0};

  if (gV > 1) puts("nsdigest");

  if (gV > 1) printf("data_len, %d, data_buf, %s, digest %s\n", data_len, data_buf, digest);

  MD5_Init(&md5ctx);

  if ( data_len > 0 ) {
    i = offset = 0;
    ongoing = 1;
    while (data_len > i) {
      ptr = &data_buf[i];
      j = 0;
      do {
        next = *ptr++;
        ++i;
        block[j++] = next + offset;
        offset += 19;
      } while ((ongoing = (j <= 31)? (data_len > i) : 0));
      MD5_Update(&md5ctx, block, j);
    }
  }
  MD5_Final(digest, &md5ctx);
}
