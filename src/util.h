#ifndef _UTIL_H_
#define _UTIL_H_

#ifndef WARN___D
#define WARN___D
#endif

#include <stdarg.h>
       #include <sys/types.h>
       #include <dirent.h>



void xfree(char **);
void *xmalloc(void **, size_t);
void xstrdup(char **, const char *);
void xsleep(int n);
int xclose(int *fd);
int max(int a, int b);

void cleanup();

#endif

