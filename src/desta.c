/*
 * deSTAbilizer - WTFPL
 *
 */

#include "desta.h"

#define SWAP32(x) do { uint32_t tmp = (htonl((x))); (x) = tmp;} while(0)

void swap_mdul_endianness(mdul_hdr_t *it) {
  if (gV > 1) puts("swap_mdul_endianness");
  SWAP32(it->m_len);
  SWAP32(it->m_cksum);
  SWAP32(it->hdr_len);
}

void swap_endianness(frm_hdr_t *it) {
  if (gV > 1) puts("swap_endianness");
  SWAP32(it->fm_hdr_len);
  SWAP32(it->fm_len);
  SWAP32(it->mdul_hdr_len);
  SWAP32(it->num_mduls);
  SWAP32(it->flash_type);
  SWAP32(it->slic_type);
}

int curDumpFd = 0, curHdrDumpFd = 0;

int currentDumpFd() {
  static int lastcurDumpFd = 0;
  int fd;

  if (!MUST_SPLIT_HDR) return curDumpFd;
  fd = (!curDumpFd || curDumpFd == lastcurDumpFd)? curHdrDumpFd: curDumpFd;
  if (curDumpFd != lastcurDumpFd) {
   puts("bim"); // just for dbg
  }
  lastcurDumpFd = curDumpFd;
  return fd;
}


void createDumpFd(mdul_hdr_t *m) {
  int fd;
  mode_t M =  O_CREAT | S_IRWXU;
  M |=  (gF) ? O_TRUNC : O_EXCL;
  char name[] = "x.out";
  name[0] = m->type+'0';
  if ((fd = creat(name, M)) == -1) die("creating output file");
  printf("curDumpFd: %d\n", fd);
  curDumpFd = fd;
  if (MUST_SPLIT_HDR) {
    name[1] = '-';
    if ((fd = creat(name, M)) == -1) die("creating output file");
    printf("curHdrDumpFd: %d\n", fd);
    curHdrDumpFd = fd;
  }
}


static void dump_header(char *buf, int len) {
  write(currentDumpFd(), buf, len);
}


static int mdul_hdr_md5(int fd, mdul_hdr_t *m) {
  if (gV > 1) puts("mdul_hdr_md5");

  unsigned char digest[0x10];
  unsigned char buf[16384];
  unsigned char *optr;
  MD5_CTX context;
  __uint32_t len, range, err = 0, towrite, skip=0;

  MD5_Init(&context);
  len = m->m_len;
  if (m->digest[12] <= 0) err = 1;
  while (!err && len > 0) {
    range = len >= 0x4000? 0x4000 : len;
    len -= range;
    if (range != read(fd, &buf, range)) {
      err = 1;
      printf("Error reading %d bytes\n", range);
      die("read");
    }
    else {
      MD5_Update(&context, buf, range);
      if (MUST_DUMP) {
        towrite = range - skip;
        optr = buf + skip;
        if ( range > skip ) {
          skip = 0;
          write(currentDumpFd(), optr, towrite);
        } else skip -= range;
        //
      }
    }
  }

  MD5_Final(digest, &context);
  if (memcmp(digest, m->digest, 0x10)) die("module digest mismatch!");
  puts("module digest is good");
  return 0;
}

int  check_mdul_hdr(int fd, mdul_hdr_t *m) {
  if (gV > 1) puts("check_mdul_hdr");
  if (MUST_DUMP) puts("WILL DUMP");

  int err = 0, red;
  mdul_hdr_t bak;
  char *it;

  memcpy(&bak, m, 0x80);
  swap_mdul_endianness(&bak);
  if (bak.hdr_len > 65536)
    die("invalid module header length");
  if (!(it = malloc(bak.hdr_len)))
    die("malloc failed");
  if ((red = read(fd, it, bak.hdr_len)) != bak.hdr_len)
    die("read error!");


  if (MUST_DUMP) dump_header(it, bak.hdr_len);

  __uint32_t cmp_cksum, hdr_cksum;
  unsigned char magic[4] = {0};

  memcpy(&magic, (void*)(&module_magics[0]) + bak.type -1, 4);

  hdr_cksum = ((mdul_hdr_t*)it)->hdr_cksum;
  ((mdul_hdr_t*)it)->hdr_cksum = 0;
  for (unsigned char inc = 0, cmp_cksum = 0; inc != 128;++inc) cmp_cksum += *(it + inc);
  ((mdul_hdr_t*)it)->hdr_cksum = hdr_cksum;

  if (bak.hdr_cksum == cmp_cksum || 1) {
    if (bak.type >= 9 || magic == 0) {
      err = mdul_hdr_md5(fd, &bak);
      puts("good module md5 digest");
    }
    else { // magic != 0 || type <= 9
      unsigned short x;
      for (x = 0; magic[x] && magic[x] == bak.magic[x] && x < 4; ++x);
        if (x != 4) {
          puts("module magic is wrong!");
          err = 1;
        }
        else {
          puts("good module magic!");
          err = mdul_hdr_md5(fd, &bak);}
    }
  }
  free(it);
  return err;
}


int check_fmhdr(char *b, ssize_t len) {
  if (gV > 1) puts("check_fmhdr");

  MD5_CTX context;
  frm_t *buf = (frm_t *) b;

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

   if (gV > 1) puts("check_firmware");

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
   if (gV > 1)
    printf("We got %d modules weighing %d bytes and a %d bytes header. Total Header size: %ld bytes\n",
     it.num_mduls, it.mdul_hdr_len, it.fm_hdr_len, filesize+sizeof(it));
  if (it.num_mduls > 20 || it.mdul_hdr_len > 0x1000 || it.fm_hdr_len > 0x1000)
    return(printf("Incorrect header length\n"));

   if (!(err = check_fmhdr(filebuf, filesize+sizeof(it)))) {
     mdul_hdr_t * mdl;
     for (int i = 0; i < it.num_mduls && !err; ++i) {
       if (gV > 1) printf("\n\n--- MODULE %d ---\n",i);
       if (MUST_DUMP) {
         if (curDumpFd) xclose(&curDumpFd);
         if (MUST_SPLIT_HDR && curHdrDumpFd) xclose(&curHdrDumpFd);
         currentDumpFd(); // to reset inner variable
         createDumpFd(mdl);
       }
       whereami(fd);
       mdl = (mdul_hdr_t*) (filebuf + it.fm_hdr_len + (i * it.mdul_hdr_len));
       err = check_mdul_hdr(fd, mdl);
	
     }
   }
   free(filebuf);
   return err;
}
