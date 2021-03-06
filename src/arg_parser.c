#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "arg_parser.h"

extern char gV;

t_arg args[] = {
{0, 0, &setVerbosity, 0, "-v"},
{1, 0, &setInputFile, 0, "-i"},
{1, 0, &setOutputFile, 0, "-o"},
{0, 0, &setDump, 0, "--dump"},
{0, 0, &setBuild, 0, "--build"},
{0, 0, &setForcefullness, 0, "-f"},
{0, 0, &setSplitHdr, 0, "-s"},
{1, 0, &setCfg, 0, "--cfg"},
{0, 0, &setNoDecode, 0, "--no-decode"},
{0, 0, &setNoEncode, 0, "--no-encode"},
{1, 0, &setCkConf, 0, "--ckconf"}
};

t_default_arg defaults[] = {
{ARG(1), 0, "firm.bin"},
{ARG(2), 0, "baked.bin"},
{ARG(7), 0, "phony"}
//{ARG(2), 1, "admin"},
//{ARG(12), 0, "127.0.0.1"},
//{ARG(12), 1, "8080"}
};

void override_default_arg(const t_arg *a, int i, char *val) {
  t_default_arg *p = &defaults[0];
  while (p && a != p->a && p->i != i) ++p;
  if (p && i == p->i) {
   p->val = val;
  }
  else {
   puts("[OverrideDefaultArg]\t PROBLEM");
   exit(1);
  }
}

const t_arg *match_arg(const char *a) {
  int i;
  if (gV > 2) printf("[Match arg]\t\"%s\"\n\t\t->", a);
  if (!a) {
    puts("match_arg called with null arg!");
    return (t_arg*) NULL;
  }
  for (i = 0; i < sizeof(args)/sizeof(t_arg); ++i) {
//   printf("%i %i strcmp %s %s\n", i, sizeof(args)/sizeof(t_arg), args[i].aname, a);
   if(!strcmp(args[i].aname, a)) {
     if (gV > 2) printf("Good arg\n");
     return &args[i];
   }
  }
  if (gV > 2) printf("Bad arg\n");
  return (t_arg*)NULL;
}

char isOp(const char **a) {
  char ret = (size_t)match_arg(*a);
  if (gV > 2) printf("[isOp]\t\t%s:\t%d\n", *a, ret);
  return (ret);
}

char *get_default(const t_arg *a, int i) {
  t_default_arg *p = &defaults[0];
  while (p && a != p->a /*&& p->i != i*/) ++p;
  puts("a");
  if (p && p->i == i) {
	return (p->val);
  }
  return (char *)NULL;
}

#define SZ_APTR (a->ops_n * sizeof(char*))

int parse_arg(const char **v, int i, int *err) {
 int   consumed = 1, n=1;
 const t_arg *a = (t_arg*)NULL;
 static char **aptr = NULL;
 if (!(a = match_arg(v[i]))) {
  if (1 || gV) printf("[ParseArg]\tInvalid arg %s\n", v[i]);
  *err = 1; return 1;
 } else {
// TODO: si il y a des operandes, check que les ops_f suivants
//       ne sont pas des operations. sinon erreur
//       si ca joue, mais que les facultatifs ne sont pas tous presents
//       chopper le default(overridable dans les appels precedents)[cmd][i]
//	 return le callback
  if (a->ops_n) {
   if (gV > 2) printf("[ParseArg]\tAllocated %ld bytes\n", SZ_APTR);
   xmalloc((void*)(&aptr), SZ_APTR);
   memset(aptr, 0, SZ_APTR);
  }
  char is_op;
  while (n-1 < a->ops_n) {
   ++ consumed;
   if ((!v[i+n] || (is_op = isOp(&v[i+n])) )&& n-1 < a->ops_m) {
    printf("[ParseArg]\tMissing mandatory argument for %s\n", a->aname);
    exit(1);
   }
   if ( is_op && (n > a->ops_m)) {
    puts("[ParseArg]\tOOPS GOT OP INSTEAD OF OPTIONAL OPERAND");
   }
   //xstrdup(&aptr[n-1], v[i+n]);
   if ( n -1 < a->ops_m && is_op ) {
    puts("[ParseArg]\tOOPS GOT OP INSTEAD OF OPERAND");
    *err = 2; return(1);
   }
   if (v[i+n] && !is_op) xstrdup(&aptr[n-1], v[i+n]); else {
     xstrdup(&(aptr[n-1]), get_default(a, n-1));
     --consumed;
   }
  ++n;
  }
 }
 if (gV > 2 && aptr) {
  for (i = 0; i < a->ops_n; ++i) {
  printf("args [%d]\t%s\n", i, aptr[i]);
  }
 }
 // WE DO THE CALLBACK HERE
  if (1 && !(a->handler)) puts("[ParseArg]\tFIXME: UNIMPLEMENTED HANDLER. PREPARE FOR CRASH");
  *err = a->handler(aptr);
  if (aptr) for (i = 0; i < a->ops_n; ++i) xfree(&(aptr[i]));
  xfree((void*)(&aptr));
 // DONNNNNNNNNNNNNNNNNNNNE

 if (gV > 2) printf("[ParseArg]\tAte %dw\n", consumed);
 return consumed;
}

int	parse_args(int c, const char **v) {
 int	i, err, n;

 i = err = 0;
 while((!err || err == 200) && i < c - 1) {
  i += (n = parse_arg(v, i+1, &err));
  if (gV > 1) printf("[ParseArgs]\t%s\t\terr=%d\targ=%d\n", v[i-n+1], err, i);
 }
 return err;
}
