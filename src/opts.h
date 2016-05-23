#ifndef _OPTS_H_
#define _OPTS_H_


#define OPT_ATTEMPT_RECOVER	1
#define OPT_RETRY_ON_ERROR	1 << 1
#define OPT_IGNORE_ERRORS	1 << 2

#define CBK(x) int (x)(char **)


CBK(setVerbosity);
CBK(setInputFile);


#endif
