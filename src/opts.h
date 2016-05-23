#ifndef _OPTS_H_
#define _OPTS_H_

#define OPT_CHECK		1
#define OPT_DUMP		1 << 1
#define OPT_BUILD		1 << 2
#define OPT_SPLIT_HDR		1 << 3

#define MUST_DUMP  (gMode & OPT_DUMP)
#define MUST_SPLIT_HDR (gMode & OPT_SPLIT_HDR)


#define CBK(x) int (x)(char **)


CBK(setForcefullness);
CBK(setVerbosity);
CBK(setInputFile);
CBK(setOutputFile);
CBK(setDump);
CBK(setBuild);
CBK(setSplitHdr);

#endif
