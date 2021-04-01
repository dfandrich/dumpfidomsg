#ifndef _RFCMSG_H
#define _RFCMSG_H

typedef struct _rfcmsg {
	struct	_rfcmsg *next;
	char	*key;
	char	*val;
} rfcmsg;

rfcmsg *parsrfc(FILE *);
void tidyrfc(rfcmsg *);
void dumpmsg(rfcmsg *,FILE *);
char *hdr(char *,rfcmsg *);
#endif
