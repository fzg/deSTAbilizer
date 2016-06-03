#ifndef _CFG_CDC_H_
#define _CFG_CDC_H_
uint8_t compute_crc(char *buffer, size_t length);
int verify_crc(char *buffer, size_t length);
int nvram_set(char *buffer, const char *key, const char *value);
char *encode(char *buf, int size);
char *decode(char *buf, int size);


#endif

