#ifndef _FTN_H
#define _FTN_H

#define MAXNAME 35

typedef struct _faddr {
	char *name;
	unsigned int point;
	unsigned int node;
	unsigned int net;
	unsigned int zone;
	char *domain;
} faddr;

typedef struct _fa_list {
		struct _fa_list *next;
		faddr *addr;
} fa_list;

faddr *parsefaddr(char *);
faddr *parsefnode(char *);
char *ascinode(faddr *,int);
char *ascfnode(faddr *,int);
int chkftnmsgid(char *);
void tidy_faddr(faddr *);
void tidy_falist(fa_list **);
faddr *bestaka_s(faddr *);
int is_local(faddr *);
int metric(faddr*,faddr*);
#define METRIC_EQUAL 0
#define METRIC_POINT 1
#define METRIC_NODE 2
#define METRIC_NET 3
#define METRIC_ZONE 4
#define METRIC_DOMAIN 5
#define METRIC_MAX METRIC_DOMAIN
#endif
