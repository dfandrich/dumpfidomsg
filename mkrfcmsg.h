#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "ftn.h"
#include "rfcmsg.h"
/* from, to, subj, orig, mdate, flags, file, offset */
extern int mkrfcmsg(faddr *,faddr *,char *,char *,time_t,int,FILE *,off_t,rfcmsg *);
