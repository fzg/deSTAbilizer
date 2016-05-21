#ifndef _FMT_H_
#define _FMT_H_

#include <sys/types.h>


#define FIRM_MAGIC ((const char*)"PAYTOND FiRmWaRe")

typedef struct mdul_hdr_s {
  __int32_t zero;
  __int32_t magic;
  __int32_t type;
  __int32_t instance;
  __int32_t res;
  __int32_t hdr_len;
  __int32_t digest;
  __int32_t m_len;
  __int32_t m_cksum;
  __int32_t m_version;
  __int32_t res_2;
  __int32_t hdr_checksum;
} mdul_hdr_t;

typedef struct frm_hdr_s {
  unsigned char fm_magic[0x10];
  unsigned char fm_signature[0x20];
  unsigned char fm_digest[0x10];
  unsigned char fm_randseq[0x10];
  __int32_t fm_hdr_len;
  __int32_t mdul_hdr_len;
  __int32_t fm_len;
  unsigned char fm_version[0x20];
  __int32_t num_mduls;
  __int32_t flash_type;
  __int32_t slic_type;
} frm_hdr_t;

typedef struct frm_s {
 frm_hdr_t      h;
 char           b[];
} frm_t;

typedef struct mdul_spath_t {
  __int32_t mdul_type;
  __int32_t skip_bytes;
  unsigned char savepath[128];
} mdul_spath_s;

#endif

