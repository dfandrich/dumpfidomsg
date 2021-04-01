#ifndef _FTNMSG_H
#define _FTNMSG_H
#include <sys/types.h>
#include <time.h>
#include "ftn.h"
#include "rfcmsg.h"

#define MAXSUBJ 71
#define MSGTYPE 2

typedef struct _ftnmsg {
	int flags;
	int ftnorigin;
	faddr *to;
	faddr *from;
	time_t date;
	char *subj;
	char *msgid_s;
	char *msgid_a;
	unsigned long msgid_n;
	char *reply_s;
	char *reply_a;
	unsigned long reply_n;
	char *origin;
	char *area;
} ftnmsg;

#define FLG_PVT 0x0001
#define FLG_CRS 0x0002
#define FLG_RCV 0x0004
#define FLG_SNT 0x0008
#define FLG_ATT 0x0010
#define FLG_TRN 0x0020
#define FLG_ORP 0x0040
#define FLG_K_S 0x0080
#define FLG_LOC 0x0100
#define FLG_HLD 0x0200
#define FLG_RSV 0x0400
#define FLG_FRQ 0x0800
#define FLG_RRQ 0x1000
#define FLG_RRC 0x2000
#define FLG_ARQ 0x4000
#define FLG_FUP 0x8000

extern char *ftndate(time_t);
extern void tidy_ftnmsg(ftnmsg *);
extern ftnmsg *mkftnhdr(rfcmsg *,int,int);
extern FILE *ftnmsghdr(ftnmsg *,FILE *,faddr *,char);
#endif
