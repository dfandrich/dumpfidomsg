/* ### Modified by P.Saratxaga on 5 Sep 1995 ###
 * - added unrar (support for RAR desarchiver) and for others (but as I's
 *   never heard of a unix version for them, i've let those commented out)
 * - added modemaftercall keyword (string to send to modem after calling)
 *   code from T. Tanaka
 * - deleted intab[] and outtab[] and getctab() (is now getmaptab in charconv.c)
 * - added maptabdir
 * - deleted ctoi() (used by getctab, I move it to kconv.c)
 * - added pktpwlist - code from T.Tanaka
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include "xutil.h"
#include "lutil.h"
#include "ftn.h"
#include "config.h"
#include "charset.h"

#ifndef PUBDIR
#define PUBDIR "/var/spool/uucppublic"
#endif

#ifndef CONFIGFILE
#define CONFIGFILE "config"
#endif

#ifndef MAPTABDIR
#define MAPTABDIR "/etc/ifmail/maptabs"
#endif

#ifndef LOGFILE
#define LOGFILE "iflog"
#endif

#ifndef DEBUGFILE
#define DEBUGFILE NULL
#endif

#ifndef BUFSIZ
#define BUFSIZ 512
#endif

#ifndef LOCAL_DOMAIN
#define LOCAL_DOMAIN	NULL
#endif
			/* Global configuration variables */

char *configname=CONFIGFILE;
char *nlbase=NULL;

dom_trans *domtrans=NULL;

#ifdef FBOX
 fa_list *InBox = NULL;
 fa_list *OutBox = NULL;
#endif

#ifdef HIDDEN
 fa_list *Override = NULL;
 fa_list *Hidden = NULL;
#endif

moderator_list *approve=NULL;

fa_list *whoami=NULL;
fa_list *pwlist=NULL;
fa_list *pktpwlist=NULL;
fa_list *nllist=NULL;
#ifdef DIRTY_CHRS
fa_list *dirtychrslist=NULL;
#endif

modem_string *modemport=NULL;
modem_string *phonetrans=NULL;
modem_string *modemreset=NULL;
modem_string *modemdial=NULL;
modem_string *modemhangup=NULL;
modem_string *modemaftercall=NULL;
modem_string *modemok=NULL;
modem_string *modemconnect=NULL;
modem_string *modemerror=NULL;
modem_string *options=NULL;

area_list *badgroups=NULL;
#ifdef GATEBAU_MSGID
area_list *gatebaugroups=NULL;
#endif

long maxfsize=65536L;
long maxpsize=0L;
long maxmsize=12300L;
long speed=0L;
long maxgroups=0L;
long timeoutreset=2L;
long timeoutconnect=75L;
long dialdelay=0L;
#ifdef RESTAMP_OLD_POSTINGS
long restampolder=RESTAMP_OLD_POSTINGS;
#endif
long paranoid=0L;
char *sentmode="^";

char *oldfilemap=NULL;
char maptab[] = {
"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
"\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
"\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
"\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
"\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
"\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
"\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"
"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"
"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"
"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"
"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"
"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"
"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377"
};

char *name="Unknown";
char *location="Somewhere";
char *sysop="Sysop";
char *phone="-Unpublished-";
char *flags="";
char *inbound="/tmp";
char *norminbound=NULL;
char *listinbound=NULL;
char *protinbound=NULL;
char *outbound=NULL;
char *database=NULL;
char *aliasfile=NULL;
char *myfqdn=NULL;
char *ftnmsgidstyle=NULL;
char *localdomain=LOCAL_DOMAIN;
char *sequence=NULL;
char *sendmail=NULL;
char *rnews=NULL;
char *iftoss=NULL;
char *packer=NULL;
char *unzip=NULL;
char *unarj=NULL;
char *unlzh=NULL;
char *unarc=NULL;
char *unzoo=NULL;
char *unrar=NULL;
char *unjar=NULL;
char *unsqz=NULL;
char *unhpk=NULL;
char *unuc2=NULL;
char *unhap=NULL;
char *unha=NULL;
char *areafile=NULL;
char *newslog=NULL;
char *msgidbm=NULL;
char *refdbm=NULL;
char *pubdir=PUBDIR;
char *reqmap=NULL;
char *magic=NULL;
char *debugfile=NULL;
char *routefile=NULL;
char *nonpacked=NULL;
char *magicname=NULL;
char *dosoutbound=NULL;
char *maptabdir=MAPTABDIR;
int defaultrfcchar=CHRS_DEFAULT_RFC;
int defaultftnchar=CHRS_DEFAULT_FTN;
int toftnchar=CHRS_NOTSET;
time_t configtime=0L;
char *pktaclfile=NULL;
char *msgaclfile=NULL;
char *orgaclfile=NULL;

			/* Local */

static int verbset=0;
static char *k,*v;
static int linecnt=0;
static int include_level=0;
static dom_trans defdomtrans = {NULL,"fidonet","fidonet.org"};

			/* External variables */

extern char *myname;
extern char *version;
extern char *reldate;
extern char *copyright;
extern char *logname;
extern char *errname;
extern unsigned long verbose;
extern unsigned long setverbose(char*);
extern void usage(void);

			/* parser prototypes */

static int getincl(char**);
static int getstr(char**);
static int getaddr(char**);
static int getadnl(char**);
static int getadpw(char**);
static int getarea(char**);
static int getmstr(char**);
static int getdtrn(char**);
static int getmdl(char**);
static int getlong(char**);
static int getverbose(char**);
static int getdefcode(char**);  /* check rfc charset names first */
static int getdefcode2(char**); /* checks FTN charset names first */

			/* keyword table */

static struct _keytab {
	char *key;
	int (*prc)(char**);
	char** dest;
} keytab[] = {
	{"include",	getincl,	NULL},

	{"address",	getaddr,	(char**)&whoami},
	{"password",	getadpw,	(char**)&pwlist},
	{"packetpasswd",getadpw,	(char**)&pktpwlist},
	{"nodelist",	getadnl,	(char**)&nllist},
#ifdef DIRTY_CHRS
	{"dirtychrs",	getadpw,	(char**)&dirtychrslist},
#endif

	{"domtrans",	getdtrn,	(char**)&domtrans},

	{"approve",	getmdl,		(char**)&approve},

	{"modemport",	getmstr,	(char**)&modemport},
	{"phonetrans",	getmstr,	(char**)&phonetrans},
	{"modemreset",	getmstr,	(char**)&modemreset},
	{"modemdial",	getmstr,	(char**)&modemdial},
	{"modemhangup",	getmstr,	(char**)&modemhangup},
	{"modemaftercall",getmstr,      (char**)&modemaftercall},
	{"modemok",	getmstr,	(char**)&modemok},
	{"modemconnect",getmstr,	(char**)&modemconnect},
	{"modemerror",	getmstr,	(char**)&modemerror},
	{"options",	getmstr,	(char**)&options},

	{"badgroup",	getarea,	(char**)&badgroups},
#ifdef GATEBAU_MSGID
	{"gatebaugroup",getarea,	(char**)&gatebaugroups},
#endif

	{"verbose",	getverbose,	(char**)&verbose},
	{"maxfsize",	getlong,	(char**)&maxfsize},
	{"maxpsize",	getlong,	(char**)&maxpsize},
	{"maxmsize",	getlong,	(char**)&maxmsize},
	{"speed",	getlong,	(char**)&speed},
	{"maxgroups",	getlong,	(char**)&maxgroups},
	{"timeoutreset",getlong,	(char**)&timeoutreset},
	{"timeoutconnect",getlong,	(char**)&timeoutconnect},
	{"dialdelay",	getlong,	(char**)&dialdelay},

	{"name",	getstr,		&name},
	{"location",	getstr,		&location},
	{"sysop",	getstr,		&sysop},
	{"phone",	getstr,		&phone},
	{"flags",	getstr,		&flags},
	{"inbound",	getstr,		&norminbound},
	{"protinbound",	getstr,		&protinbound},
	{"listinbound",	getstr,		&listinbound},
	{"outbound",	getstr,		&outbound},
	{"database",	getstr,		&database},
	{"sysalias",	getstr,		&aliasfile},
	{"myfqdn",	getstr,		&myfqdn},
	{"ftnmsgidstyle", getstr,	&ftnmsgidstyle},
	{"localdomain",	getstr,		&localdomain},
	{"sequencer",	getstr,		&sequence},
	{"sendmail",	getstr,		&sendmail},
	{"rnews",	getstr,		&rnews},
	{"iftoss",	getstr,		&iftoss},
	{"packer",	getstr,		&packer},
	{"unzip",	getstr,		&unzip},
	{"unarj",	getstr,		&unarj},
	{"unlzh",	getstr,		&unlzh},
	{"unarc",	getstr,		&unarc},
	{"unzoo",	getstr,		&unzoo},
	{"unrar",	getstr,		&unrar},
	{"unjar",	getstr,		&unjar},
	{"unsqz",	getstr,		&unsqz},
	{"unhpk",	getstr,		&unhpk},
	{"unuc2",	getstr,		&unuc2},
	{"unhap",	getstr,		&unhap},
	{"unha",	getstr,		&unha },
	{"areas",	getstr,		&areafile},
	{"newslog",	getstr,		&newslog},
	{"msgidbm",	getstr,		&msgidbm},
	{"refdbm",	getstr,		&refdbm},
	{"public",	getstr,		&pubdir},
	{"reqmap",	getstr,		&reqmap},
	{"magic",	getstr,		&magic},
	{"logfile",	getstr,		&logname},
	{"errfile",	getstr,		&errname},
	{"debugfile",	getstr,		&debugfile},
	{"routefile",	getstr,		&routefile},
	{"nonpacked",	getstr,		&nonpacked},
	{"magicname",	getstr,		&magicname},
	{"dosoutbound",	getstr,		&dosoutbound},
	{"maptabdir",	getstr,		&maptabdir},
	{"defaultrfcchar", getdefcode,	(char**)&defaultrfcchar},
	{"defaultftnchar", getdefcode2,	(char**)&defaultftnchar},
	{"toftnchar",	getdefcode2,	(char**)&toftnchar}, 
#ifdef RESTAMP_OLD_POSTINGS
	{"restampolder", getlong,	(char**)&restampolder},
#endif
	{"paranoid",	getlong,	(char**)&paranoid},
	{"sentmode",	getstr,		&sentmode},
	{"packetacl",	getstr,		&pktaclfile},
	{"messageacl",	getstr,		&msgaclfile},
	{"originacl",	getstr,		&orgaclfile},
#ifdef HIDDEN
	{"override",	getadpw,	(char **)&Override},
	{"hidden",	getadpw,	(char **)&Hidden},
#endif
	{NULL,		NULL,		NULL}
};

			/* public entry point */
int readconfig(void);
int readconfig(void)
{
	int maxrc=0,rc,i;
	FILE *fp;
	char buf[BUFSIZ],*p;
	struct stat st;

	if (stat(configname,&st) != 0)
	{
		fprintf(stderr,"readconfig: cannot stat file \"%s\" ",
			S(configname));
		perror("");
		return 1;
	}
	if (st.st_mtime > configtime) configtime=st.st_mtime;
	if ((fp=fopen(configname,"r")) == NULL)
	{
		fprintf(stderr,"readconfig: cannot open file \"%s\" ",
			S(configname));
		perror("");
		return 1;
	}
	if (include_level == 0)
	{
		logname=LOGFILE;
		errname=LOGFILE;
		debugfile=DEBUGFILE;
	}
	while (fgets(buf,sizeof(buf)-1,fp))
	{
		linecnt++;
		if (*(p=buf+strlen(buf)-1) != '\n')
		{
			fprintf(stderr,"%s(%d): %s - line too long\n",
				configname,linecnt,buf);
			while (fgets(buf,sizeof(buf)-1,fp) &&
				(*(p=buf+strlen(buf)-1) != '\n'));
			continue;
		}

		*p--='\0';
		while ((p >= buf) && isspace(*p)) *p--='\0';
		k=buf;
		while (*k && isspace(*k)) k++;
		p=k;
		while (*p && !isspace(*p)) p++;
		*p++='\0';
		v=p;
		while (*v && isspace(*v)) v++;

		if ((*k == '\0') || (*k == '#'))
		{
			if (verbose & (1<<15))
				fprintf(stderr,"%s(%d): \"%s\" \"%s\" - ignore\n",
					configname,linecnt,S(k),S(v));
			continue;
		}

		if (verbose & (1<<15))
			fprintf(stderr,"%s(%d): \"%s\" \"%s\" - parsed\n",
				configname,linecnt,S(k),S(v));

		for (i=0;keytab[i].key;i++)
			if (strcasecmp(k,keytab[i].key) == 0) break;
		if (keytab[i].key == NULL)
		{
			fprintf(stderr,"%s(%d): %s %s - unknown keyword\n",
				configname,linecnt,S(k),S(v));
		}
		else if ((rc=keytab[i].prc(keytab[i].dest)) > maxrc) maxrc=rc;
	}
	fclose(fp);

	if (include_level == 0)
	{
		if (whoami == NULL)
		{
			fprintf(stderr,"%s(EOF): address not specified\n",
					configname);
			if (maxrc < 1) maxrc=1;
		}
#if 0
		else if (whoami->addr->domain == NULL)
		{
			whoami->addr->domain=xstrcpy("fidonet");
		}
#endif

		if (whoami && (nllist == NULL))
		{
			nllist=(fa_list*)xmalloc(sizeof(fa_list));
			nllist->next=NULL;
			nllist->addr=whoami->addr;
		}

		if (whoami && (nllist->addr->domain == NULL))
			nllist->addr->domain=whoami->addr->domain;
	
		if (norminbound) inbound=norminbound;
		if (protinbound == NULL) protinbound=inbound;
		if (listinbound == NULL) listinbound=inbound;
		if (errname == NULL) errname=logname;

		if (domtrans == NULL) domtrans=&defdomtrans;

		if (verbose & (1<<15))
		{
			fprintf(stderr,"globals set in rdconfig:\n");
			fprintf(stderr,"verbose:    %lu\n",verbose);
			fprintf(stderr,"logname:    %s\n",S(logname));
			fprintf(stderr,"errname:    %s\n",S(errname));
			fprintf(stderr,"debugfile:  %s\n",S(debugfile));
			fprintf(stderr,"configname: %s\n",S(configname));
			fprintf(stderr,"configtime: %s",ctime(&configtime));
		}

		if (debugfile) freopen(debugfile,"a",stderr);
		debug(0,"start %s ver %s of %s, verbose 0x%08lx",
			myname,version,reldate,verbose);
	}

	return maxrc;
}

	/* check option + optarg for being config option */

int confopt(c,arg)
int c;
char *arg;
{
	switch (c)
	{

	case 'h':	usage();
			exit(0);

	case 'x':	verbset=1;
			verbose=setverbose(arg);
			break;

	case 'I':	configname=arg;
			break;

	default:	return 1; /* unrecognized option */

	}
	return 0; /* recognized option */
}

void confusage(extra)
char *extra;
{

#ifdef HAVE_LOCALE_H
	/* set up the correct locale */
	setlocale(LC_MESSAGES, "" );

	bindtextdomain(NLS_CAT_NAME, NLS_LOCALE_PATH);
	textdomain(NLS_CAT_NAME);
#endif

	fprintf(stderr,_("%s ver. %s of %s; (c) %s\n"),
		myname,version,reldate,copyright);
	fprintf(stderr,_("    This is free software. You can do what you wish \
with it\n    as long as this copyright notice is preserved.\n\n"));
	fprintf(stderr,_("usage: %s -h -x<N> -I<file> %s\n"),myname,S(extra));
	fprintf(stderr,_("-h\t\tget this help\n"));
	fprintf(stderr,_("-x<arg>\t\tset debug level <arg>\t[%08lx]\n"),
		verbose);
	fprintf(stderr,_("\t\t<arg> may be a number from 0 to 32 to set `on'\n"));
	fprintf(stderr,_("\t\tbits from 1 to number, or a string of letters\n"));
	fprintf(stderr,_("\t\t('a' - bit 1, 'b' - bit 2, e.t.c. up to bit 26)\n"));
	fprintf(stderr,_("-I<file>\tuse config file\t<file>\t[%s]\n"),
		configname);
}

			/* parsers */

static int getincl(dest)
char **dest;
{
	int rc=0;
	char *saveconfigname=configname;
	int savelinecnt=linecnt;

	if (verbose & (1<<15)) fprintf(stderr,"getincl: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));

	configname=v;
	linecnt=0;
	include_level++;

	rc=readconfig();

	include_level--;
	configname=saveconfigname;
	linecnt=savelinecnt;
	return rc;
}

static int getstr(dest)
char **dest;
{
	if (verbose & (1<<15)) fprintf(stderr,"getstr: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));
	*dest=xstrcpy(v);
	return 0;
}

static int addalist(fa_list**,char*,char*);
static int addalist(lst,addr,nm)
fa_list **lst;
char *addr,*nm;
{
	faddr *tmp;
	fa_list **tmpl;

	if (verbose & (1<<15)) fprintf(stderr,"addalist: %s(%d): %s %s\n",
		configname,linecnt,S(addr),S(nm));
	if (nm && (*nm == '\0'))
	{
		fprintf(stderr,"%s(%d): no value for address %s\n",
			configname,linecnt,S(addr));
		return 0;
	}
	if (addr)
	{
		if ((tmp=parsefnode(addr)) == NULL)
		{
			fprintf(stderr,"%s(%d): unparsable address %s\n",
				configname,linecnt,S(addr));
			return 0;
		}
	}
	else
	{
		tmp=(faddr*)xmalloc(sizeof(faddr));
		memset(tmp,0,sizeof(faddr));
	}

	/* check for "" (that is a NULL name, it is used to specify
	 * NULL passwords)
	 */
	if (nm && (*nm == '\"')) {
		if ((strlen(nm)==2) && (*(nm+1) == '\"'))
			*nm='\0';
	}

	for (tmpl=lst;*tmpl;tmpl=&((*tmpl)->next));
	*tmpl=(fa_list*)xmalloc(sizeof(fa_list));
	(*tmpl)->next=NULL;
	(*tmpl)->addr=tmp;
	(*tmpl)->addr->name=xstrcpy(nm);
	return 0;
}

static int getaddr(dest)
char **dest;
{
	if (verbose & (1<<15)) fprintf(stderr,"getaddr: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));
	return addalist((fa_list**)dest,v,NULL);
}

static int getadnl(dest)
char **dest;
{
	char *p,*q,*tmp;
	int rc;

	if (verbose & (1<<15)) fprintf(stderr,"getadnl: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));
	p=v;
	while (*p && !isspace(*p)) p++;
	if (*p == '\0') p=NULL;
	else
	{
		*p++='\0';
		while (*p && isspace(*p)) p++;
	}
	if (nlbase == NULL)
	{
		nlbase=xstrcpy(v);
		if (*nlbase != '/') nlbase[0]='\0';
		if ((q=strrchr(nlbase,'/'))) *++q='\0';
		if ((q=strrchr(v+1,'/'))) q++;
		else q=v;
	}
	else q=v;

	tmp=xstrcpy(nlbase);
	tmp=xstrcat(tmp,q);

	rc=addalist((fa_list**)dest,p,tmp);
	free(tmp);
	return rc;
}

static int getadpw(dest)
char **dest;
{
	char *p;

	if (verbose & (1<<15)) fprintf(stderr,"getadpw: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));
	p=v;
	while (*p && !isspace(*p)) p++;
	if (*p == '\0')
	{
		fprintf(stderr,"%s(%d): no password for address %s\n",
			configname,linecnt,S(v));
		return 0;
	}
	*p++='\0';
	while (*p && isspace(*p)) p++;
	return addalist((fa_list**)dest,v,p);
}

static int getarea(dest)
char **dest;
{
	area_list **tmpl;

	if (verbose & (1<<15)) fprintf(stderr,"getarea: %s(%d): %s %s\n",
		configname,linecnt,k,v);
	for (tmpl=(area_list**)dest;*tmpl;tmpl=&((*tmpl)->next));
	*tmpl=(area_list*)xmalloc(sizeof(area_list));
	(*tmpl)->next=NULL;
	(*tmpl)->name=xstrcpy(v);
	return 0;
}

static int getmstr(dest)
char **dest;
{
	modem_string **tmpm;
	int dep=0;
	char *p,*q;

	if (verbose & (1<<15)) fprintf(stderr,"getmstr: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));
	if (*v == '(')
	{
		p=v+1;
		q=v;
		do
		{
			if (*q == '(') dep++;
			if (*q == ')') dep--;
		}
		while (dep && *q++);
		if (*q == '\0')
		{
			fprintf(stderr,"%s(%d): %s - unbalanced brackets\n",
				configname,linecnt,S(v));
			return 0;
		}
		*q++='\0';
		while (*q && isspace(*q)) q++;
		if (*q == '\0')
		{
			fprintf(stderr,"%s(%d): %s - no value for expression\n",
				configname,linecnt,S(p));
			return 0;
		}
	}
	else
	{
		p=NULL;
		q=v;
	}

	for (tmpm=(modem_string**)dest;*tmpm;tmpm=&((*tmpm)->next));
	(*tmpm)=(modem_string *)
		xmalloc(sizeof(modem_string));
	(*tmpm)->next=NULL;
	(*tmpm)->expr=xstrcpy(p);
	(*tmpm)->line=xstrcpy(q);
	if (verbose & (1<<15)) fprintf(stderr,"getmstr: %s(%d): expr \"%s\" line \"%s\"\n",
		configname,linecnt,S(p),S(q));

	return 0;
}

static int getdtrn(dest)
char **dest;
{
	dom_trans **tmpm;
	char *p;

	if (verbose & (1<<15)) fprintf(stderr,"getdtrn: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));

	for (p=v;*p && !isspace(*p);p++);
	if (*p) *p++='\0';
	while (*p && isspace(*p)) p++;
	if (*p == '\0')
	{
		fprintf(stderr,"%s(%d): less than two tokens in domtrans",
			configname,linecnt);
	}
	else
	{
		for (tmpm=(dom_trans**)dest;*tmpm;tmpm=&((*tmpm)->next));
		(*tmpm)=(dom_trans *)
			xmalloc(sizeof(dom_trans));
		(*tmpm)->next=NULL;
		(*tmpm)->ftndom=xstrcpy(v);
		(*tmpm)->intdom=xstrcpy(p);
		if (verbose & (1<<15)) fprintf(stderr,"getdtrn: %s(%d): expr \"%s\" line \"%s\"\n",
			configname,linecnt,S(v),S(p));
	}

	return 0;
}

static int getmdl(dest)
char **dest;
{
	moderator_list **tmpm;
	char *p;

	if (verbose & (1<<15)) fprintf(stderr,"getmdl: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));

	for (p=v;*p && !isspace(*p);p++);
	if (*p) *p++='\0';
	while (*p && isspace(*p)) p++;
	if (*p == '\0')
	{
		fprintf(stderr,"%s(%d): less than two tokens in moderator list",
			configname,linecnt);
	}
	else
	{
		for (tmpm=(moderator_list**)dest;*tmpm;tmpm=&((*tmpm)->next));
		(*tmpm)=(moderator_list *)
			xmalloc(sizeof(moderator_list));
		(*tmpm)->next=NULL;
		(*tmpm)->prefix=xstrcpy(v);
		(*tmpm)->address=xstrcpy(p);
		if (verbose & (1<<15)) fprintf(stderr,"getmdl: %s(%d): expr \"%s\" line \"%s\"\n",
			configname,linecnt,S(v),S(p));
	}

	return 0;
}

static int getlong(dest)
char **dest;
{
	if (verbose & (1<<15)) fprintf(stderr,"getlong: %s(%d): %s %s\n",
		configname,linecnt,k,v);
	if (strspn(v,"0123456789") != strlen(v)) 
		fprintf(stderr,"%s(%d): %s %s - bad numeric\n",
			configname,linecnt,S(k),S(v));
	else *((long*)dest)=atol(v);
	return 0;
}

static int getverbose(dest)
char **dest;
{
	if (verbose & (1<<15)) fprintf(stderr,"getverbose: %s(%d): %s %s\n",
		configname,linecnt,S(k),S(v));
	if (verbset)
	{
		if (verbose & (1<<15))
			fprintf(stderr,"getverbose: already set, ignore\n");
		return 0;
	}
	*((unsigned long*)dest)=setverbose(v);
	if (verbose & (1<<15)) fprintf(stderr,"getverbose: %lu\n",
		*((unsigned long*)dest));
	return 0;
}

static int getdefcode(dest)
char **dest;
{
	int charset;

        if (verbose & (1<<15)) fprintf(stderr,"getdefcode: %s(%d): %s %s\n",
                configname,linecnt,k,v);

	charset=getcode(v);
	if ( charset == CHRS_NOTSET) charset=readchrs(v);
	if ( charset != CHRS_NOTSET) *((int*)dest)=charset;

        return 0;
}

static int getdefcode2(dest)
char **dest;
{
	int charset;

	if (verbose & (1<<15)) fprintf(stderr,"getdefcode2: %s(%d): %s %s\n",
		configname,linecnt,k,v);

	charset=readchrs(v);
	if ( charset == CHRS_NOTSET) charset=getcode(v);
	if ( charset != CHRS_NOTSET) *((int*)dest)=charset;

	return 0;
}

#ifdef TESTING

int main(argc,argv)
int argc;
char *argv[];
{
	verbose = 1<<15;

	if (argv[1]) configname=argv[1];
	return readconfig();
}

#endif
