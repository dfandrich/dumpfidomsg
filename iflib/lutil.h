#ifndef IFMAIL_LUTIL_H
#define IFMAIL_LUTIL_H

#define S(x) (x)?(x):"(null)"

#define DBG_LOCK	4
#define DBG_AKA		6
#define DBG_OUTBOUND	7
#define DBG_CALL	8
#define DBG_HANDSHAKE	10
#define DBG_PROTOCOL	11
#define DBG_FILELIST	12
#define DBG_NLFLAG	13
#define DBG_MODEM	18
#define DBG_NODELIST	20
#define DBG_HYDRA	26

extern unsigned long verbose;
extern char *myname;
extern int logfacility;
void setmyname(char*);
void setnamesfx(char*);
unsigned long setverbose(char*);
void debug(unsigned long,char*,...);
void loginf(char*,...);
void logerr(char*,...);
char *date(long);
char *printable(char*,int);
char *printablec(char);

#ifdef DONT_HAVE_GETOPT
extern int getopt();
extern char *optarg;
extern int optind;
#else
#include <getopt.h>
#endif

#if defined(HAS_TCP) || defined(HAS_TERM)
#define FIDOPORT  60179		/* my birthday */
#define BINKPPORT 24554
#endif

#define TCPMODE_IFC	0	/* ifcico native EMSI on raw TCP */
#define TCPMODE_TELNET	1	/* EMSI encapsulation through telnet */
#define TCPMODE_BINKP	2	/* Binkp protocol */

/* for the benefit of Hurd systems, which are too good to have static limits */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

#endif /* IFMAIL_LUTIL_H */
