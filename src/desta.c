/*
 deSTAbilizer - WTFPL

*/

#include "desta.h"

#define SWAP32(x) do { uint32_t tmp = (ntohl((x))); (x) = tmp;} while(0)

void swap_endianness(frm_hdr_t *it) {
  SWAP32(it->fm_hdr_len);
  SWAP32(it->fm_len);
  SWAP32(it->mdul_hdr_len);
  SWAP32(it->num_mduls);
  SWAP32(it->flash_type);
  SWAP32(it->slic_type);
}

int check_fmhdr(frm_hdr_t *it, frm_hdr_t *orig) {

  if (it->num_mduls > 20 || it->mdul_hdr_len > 0x1000
      || it->fm_hdr_len > 0x1000)
    return(printf("Incorrect header length\n"));

  MD5_CTX context;

  char fm_signature[0x20];  memcpy(fm_signature, orig->fm_signature, sizeof(fm_signature));
  char fm_digest[0x10];     memcpy(fm_digest,    orig->fm_digest,    sizeof(fm_digest));
  char fm_randseq[0x10];    memcpy(fm_randseq,   orig->fm_randseq,   sizeof(fm_randseq));
  char digest[0x10] = {0};  memset(&digest,      0,                  sizeof(digest));
  char randseq[0x10] = {0}; memset(&randseq,     0,                  sizeof(randseq));

  int len = it->mdul_hdr_len * it->num_mduls + it->fm_hdr_len;

  memset(orig->fm_signature, 0, sizeof(orig->fm_signature));
  memset(orig->fm_digest, 0, sizeof(orig->fm_digest));

  for (ssize_t i = 0; i < sizeof(digest); ++i) digest[i] = 0;

  MD5_Init(&context);							// Compute hash
  MD5_Update(&context, (unsigned int *)orig, len);
  MD5_Final(&digest[0], &context);

  memset(it->fm_randseq, 0, sizeof(it->fm_randseq));
  nsdigest(randseq, (char *)orig, len);					// Compute digest
  memcpy(it->fm_randseq, fm_randseq, sizeof(it->fm_randseq));
  memcpy(it->fm_digest, fm_digest, sizeof(it->fm_digest));
  memcpy(it->fm_signature, fm_signature, sizeof(it->fm_signature));

  if (!memcmp(digest, fm_digest, sizeof(digest))) {
   if (!memcmp(randseq, fm_randseq, sizeof(randseq))) {
      puts("Header OK!");
      return 0;
   } else {
      puts ("Header randseq error");
   }
  } else { puts("Header digest error");
       printf("digest sizes: %d %d\n", sizeof(digest), sizeof(fm_digest));
     }
  return -1;
}

int check_firmware(int fd) {

   frm_hdr_t it2;
   frm_hdr_t it;
   ssize_t red, filesize, err;
   red = filesize = err = 0;

   memset((char*)&it, 0, sizeof(it));
   memset((char*)&it2, 0, sizeof(it2));
   if ((red = read(fd, &it, sizeof(it))) != sizeof(it)) {
     die("Reading header:");
   }

   if (strncmp(FIRM_MAGIC, it.fm_magic, strlen(FIRM_MAGIC))) {
     die("Bad magic!");
   }
   memcpy((char*)&it2, (char*)&it, sizeof(it));
   swap_endianness(&it);
   filesize = it.fm_hdr_len + it.mdul_hdr_len * it.num_mduls;
   if (VERB)
    printf("We got %d modules weighing %d bytes and a %d bytes header. Total Header size: %d bytes\n",
     it.num_mduls, it.mdul_hdr_len, it.fm_hdr_len, filesize);
   err = check_fmhdr(&it, &it2);
}
