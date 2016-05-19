#include "desta.h"


void nsdigest(char *digest, char *data_buf, int data_len)
{
  int i; // r5@0
  int data_len_; // r8@1
  unsigned char *digest_ptr; // r11@1
  char *data_ptr; // r10@1
  char c; // r4@4
  char *d; // r1@6
  signed int j; // r2@6
  char next; // t1@7
  int ongoing; // r3@8
  MD5_CTX md5ctx; // [sp+0h] [bp-A0h]@1
  char tmp[32]; // [sp+58h] [bp-48h]@3

  data_len_ = data_len;
  digest_ptr = (unsigned char *)digest;
  data_ptr = data_buf;
  MD5_Init(&md5ctx);
  if ( data_len_ > 0 )
    i = 0;
  *(__int32_t *)&tmp[28] = 0;
  *(__int32_t *)tmp = 0;
  *(__int32_t *)&tmp[4] = 0;
  *(__int32_t *)&tmp[8] = 0;
  *(__int32_t *)&tmp[12] = 0;
  *(__int32_t *)&tmp[16] = 0;
  *(__int32_t *)&tmp[20] = 0;
  *(__int32_t *)&tmp[24] = 0;
  if ( data_len_ > 0 )
  {
    c = i;
    do
    {
      d = &data_ptr[i];
      j = 0;
      do
      {
        next = *d++;
        ++i;
        tmp[j++] = next + c;
        if ( j <= 31 )
          ongoing = data_len_ > i;
        else
          ongoing = 0;
        c += 19;
      }
      while ( ongoing );
      MD5_Update(&md5ctx, (unsigned char *)tmp, j);
    }
    while ( data_len_ > i );
  }
  MD5_Final(digest_ptr, &md5ctx);
}
