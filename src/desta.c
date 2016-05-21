/*
 * deSTAbilizer - WTFPL
 *
 */

#include "desta.h"

#define SWAP32(x) do { uint32_t tmp = (htonl((x))); (x) = tmp;} while(0)

void swap_mdul_endianness(mdul_hdr_t *it) {
  if (VERB) puts("swap_mdul_endianness");
  SWAP32(it->m_len);
  SWAP32(it->m_cksum);
  SWAP32(it->hdr_len);
}

void swap_endianness(frm_hdr_t *it) {
  if (VERB) puts("swap_endianness");
  SWAP32(it->fm_hdr_len);
  SWAP32(it->fm_len);
  SWAP32(it->mdul_hdr_len);
  SWAP32(it->num_mduls);
  SWAP32(it->flash_type);
  SWAP32(it->slic_type);
}

int  check_mdul_hdr(mdul_hdr_t *m) {
  int err = 0;
  unsigned char buf[16384];
  mdul_hdr_t bak;
  memcpy(&bak, m, 0x80);
  swap_mdul_endianness(&bak);

  return err;
}

int check_fmhdr(char *b, ssize_t len) {
  if (VERB) puts("check_fmhdr");


  MD5_CTX context;
  frm_t *buf = (frm_t *) b;

//  swap_endianness(&buf->h);

  unsigned char cmp_digest[0x10] = {0},    hdr_digest[0x10];
  unsigned char cmp_signature[0x20] = {0}, hdr_signature[0x20];
  unsigned char cmp_randseq[0x10] = {0},   hdr_randseq[0x10];

  memcpy(hdr_signature, buf->h.fm_signature, sizeof(hdr_signature));
  memset(buf->h.fm_signature, 0, 0x20);
  memcpy(hdr_digest,    buf->h.fm_digest,    sizeof(hdr_digest));
  memset(buf->h.fm_digest,    0, 0x10);

  MD5_Init(&context);							// Compute hash
  MD5_Update(&context, buf, len);
  MD5_Final(cmp_digest, &context);

  memcpy(hdr_randseq,   buf->h.fm_randseq,   sizeof(hdr_randseq));
  memset(buf->h.fm_randseq, 0, sizeof(buf->h.fm_randseq));
  nsdigest(cmp_randseq, (char *)buf, len);				// Compute digest
  memcpy(buf->h.fm_randseq,   cmp_randseq, sizeof(buf->h.fm_randseq));
  memcpy(buf->h.fm_digest,    cmp_digest, sizeof(buf->h.fm_digest));
  memcpy(buf->h.fm_signature, cmp_signature, sizeof(buf->h.fm_signature));

  if (memcmp(cmp_digest, hdr_digest, sizeof(hdr_digest))) {
    puts("Header digest error");
  print_digest(cmp_digest); print_digest(hdr_digest);
    return -1;
  }
  if (memcmp(cmp_randseq, hdr_randseq, sizeof(hdr_randseq))) {
    puts("Header randseq error");
  print_digest_x(cmp_randseq, 0x20); print_digest_x(hdr_randseq, 0x20);
    return -1;
  }
  puts ("Header OK!");
  return 0;
}

int check_firmware(int fd) {

   frm_hdr_t it, bkp;
   char     *filebuf;
   ssize_t   red, filesize, err;

   if (VERB) puts("check_firmware");

   memset((char*)&it, 0, sizeof(it));
   memset((char*)&bkp, 0, sizeof(bkp));

   if ((red = read(fd, &it, sizeof(it))) != sizeof(it))      die("Reading header:");

   if (strncmp(FIRM_MAGIC, (char *)it.fm_magic, strlen(FIRM_MAGIC))) die("Bad magic!");

   memcpy((char*)&bkp, (char*)&it, sizeof(it));
   swap_endianness(&it);

   filesize = 392; //it.fm_hdr_len + it.mdul_hdr_len * it.num_mduls;
   if (!(filebuf = malloc(filesize*sizeof(unsigned char)))) die ("Malloc failed:");
   memset(filebuf, 0, filesize);
   filesize -= sizeof(bkp);
   memcpy(filebuf, &bkp, sizeof(bkp));
   if (sizeof(bkp)!=136) die("oops");
//   if ((red = read(fd, filebuf, filesize)) != filesize) die("Reading headers:");

   if ((red = read(fd, filebuf+sizeof(bkp), filesize)) != filesize) die("Reading headers:");
   if (red + 136 != filesize+sizeof(bkp)) {
     die("oops2");
   }
   if (VERB)
    printf("We got %d modules weighing %d bytes and a %d bytes header. Total Header size: %ld bytes\n",
     it.num_mduls, it.mdul_hdr_len, it.fm_hdr_len, filesize+sizeof(it));
  if (it.num_mduls > 20 || it.mdul_hdr_len > 0x1000 || it.fm_hdr_len > 0x1000)
    return(printf("Incorrect header length\n"));

   if (!(err = check_fmhdr(filebuf, filesize+sizeof(it)))) {
     mdul_hdr_t * mdl;
     for (int i = 0; i < it.num_mduls && !err; ++i) {
       mdl = (mdul_hdr_t*) filebuf + it.fm_hdr_len + i * it.mdul_hdr_len;
       err = check_mdul_hdr(mdl);
     }
   }
   return err;
}
