/* ### Modified by P.Saratxaga on 5 Sep 1995 ###
 * - added unrar (support for RAR desarchiver) and a bunch of others (but I
 *   never heard of a unix version for them,...)
 * - added modemaftercall keyword (string to send to modem after calling)
 *   code from T. Tanaka
 * - deleted intab[] and outtab[]
 * - added maptabdir
 * - added pktpwlist, passwords for packets. code from T.Tanaka
 */
#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef HAVE_LOCALE_H
#include <libintl.h>
#define _(String)	gettext((String))
#else
#define _(String)	(String)
#endif

#include <time.h>

#include "ftn.h"

#ifndef AREA_LIST_TYPE
#define AREA_LIST_TYPE
typedef struct _area_list {
	struct _area_list *next;
	char *name;
} area_list;
#endif

typedef struct _modem_string {
	struct _modem_string *next;
	char *line;
	char *expr;
} modem_string;

typedef struct _dom_trans {
	struct _dom_trans *next;
	char *ftndom;
	char *intdom;
} dom_trans;

typedef struct _moderator_list {
	struct _moderator_list *next;
	char *prefix;
	char *address;
} moderator_list;

extern char *configname;
extern char *nlbase;

extern fa_list *whoami;
extern fa_list *pwlist;
extern fa_list *pktpwlist;
extern fa_list *nllist;
#ifdef DIRTY_CHRS
extern fa_list *dirtychrslist;
#endif

extern dom_trans *domtrans;

extern moderator_list *approve;

extern modem_string *modemport;
extern modem_string *phonetrans;
extern modem_string *modemreset;
extern modem_string *modemdial;
extern modem_string *modemhangup;
extern modem_string *modemaftercall;
extern modem_string *modemok;
extern modem_string *modemconnect;
extern modem_string *modemerror;
extern modem_string *options;

extern area_list *badgroups;
#ifdef GATEBAU_MSGID
extern area_list *gatebaugroups;
#endif

extern long configverbose;
extern time_t configtime;
extern long maxfsize;
extern long maxpsize;
extern long maxmsize;
extern long speed;
extern long maxgroups;
extern long timeoutconnect;
extern long timeoutreset;
extern long dialdelay;

extern char *oldfilemap;
extern char maptab[];

extern char *name;
extern char *location;
extern char *sysop;
extern char *phone;
extern char *flags;
extern char *inbound;
extern char *norminbound;
extern char *listinbound;
extern char *protinbound;
extern char *outbound;
extern char *database;
extern char *aliasfile;
extern char *myfqdn;
extern char *ftnmsgidstyle;
extern char *localdomain;
extern char *sequence;
extern char *sendmail;
extern char *rnews;
extern char *iftoss;
extern char *packer;
extern char *unzip;
extern char *unarj;
extern char *unlzh;
extern char *unarc;
extern char *unzoo;
extern char *unrar;
extern char *unjar;
extern char *unsqz;
extern char *unhpk;
extern char *unuc2;
extern char *unhap;
extern char *unha;
extern char *areafile;
extern char *newslog;
extern char *msgidbm;
extern char *refdbm;
extern char *pubdir;
extern char *reqmap;
extern char *magic;
extern char *debugfile;
extern char *routefile;
extern char *nonpacked;
extern char *magicname;
extern char *dosoutbound;
extern char *maptabdir;
extern int  defaultrfcchar;
extern int  defaultftnchar;
extern int  toftnchar;

extern char *pktaclfile;
extern char *msgaclfile;
extern char *orgaclfile;

int readconfig(void);
int confopt(int,char*);
void confusage(char*);
#endif
