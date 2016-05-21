#include "desta.h"

int print_digest_x(unsigned char *digest, size_t len) {
  if (VERB) printf("print_digest: ");
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
  char *digest_ptr;
  MD5_CTX md5ctx;
  unsigned char block[32] = {0};

  if (VERB) puts("nsdigest");

  if (VERB) printf("data_len, %d, data_buf, %s, digest %s\n", data_len, data_buf, digest);

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
      printf("outOfOngoing %d\n", i);
      MD5_Update(&md5ctx, block, j);
    }
  }
  MD5_Final(digest, &md5ctx);
}
