#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <varargs.h>
#include <errno.h>
#include <time.h>
#ifdef HAS_SYSLOG
#include <syslog.h>
#else
#define LOG_USER 0
#endif

#ifdef HAS_SYSLOG
static int syslog_opened=0;
#else
FILE *logfile=NULL;
FILE *errfile=NULL;
#endif
int logfacility=LOG_USER;
char *myname="unknown";
char *logname="/tmp/tmplog";
char *errname="/tmp/tmplog";
unsigned long verbose=0L;

void setmyname(arg)
char *arg;
{
	if ((myname=strrchr(arg,'/'))) myname++;
	else myname=arg;
	myname=xstrcpy(myname);
}

void setnamesfx(arg)
char *arg;
{
	char *p;

	if (arg == NULL) return;

	if ((p=strchr(myname,'.'))) *p='\0';
	myname=xstrcat(myname,".");
	myname=xstrcat(myname,arg);
#ifdef HAS_SYSLOG
	if (syslog_opened) closelog();
	openlog(myname,LOG_PID|LOG_CONS,logfacility);
#endif
}

static char *mon[] = {
"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

char *date(t)
time_t t;
{
	struct tm ptm;
	time_t now;
	static char buf[20];

	if (t) now=t; 
	else time(&now);
	ptm=*localtime(&now);
	sprintf(buf,"%s %02d %02d:%02d:%02d",
		mon[ptm.tm_mon],ptm.tm_mday,
		ptm.tm_hour,ptm.tm_min,ptm.tm_sec);
	return(buf);
}

static char *pbuff = NULL;

char *printable(s,l)
char *s;
int l;
{
	int len;
	char *p;

	if (pbuff) free(pbuff);
	pbuff=NULL;

	if (s == NULL) return "(null)";

	if (l > 0) len=l;
	else if (l == 0) len=strlen(s);
	else
	{
		len=strlen(s);
		if (len > -l) len=-l;
	}
	pbuff=(char*)malloc(len*4+1);
	p=pbuff;
	while (len--)
	{
		if (*(unsigned char*)s >= ' ') *p++=*s;
		else switch (*s)
		{
		case '\\': *p++='\\'; *p++='\\'; break;
		case '\r': *p++='\\'; *p++='r'; break;
		case '\n': *p++='\\'; *p++='n'; break;
		case '\t': *p++='\\'; *p++='t'; break;
		case '\b': *p++='\\'; *p++='b'; break;
		default:   sprintf(p,"\\%03o",*s); p+=4; break;
		}
		s++;
	}
	*p='\0';
	return pbuff;
}

char *printablec(c)
char c;
{
	return printable(&c,1);
}

		/* log and debug outputs */

#define PRINT_DEBUG(fmt,args) \
	vfprintf(stderr,*fmt == '$' ? fmt+1 : fmt,args);\
	if (*fmt == '$')\
		fprintf(stderr,"\n\terrno=%d : %s\n",\
			errno,strerror(errno));\
	else\
		fprintf(stderr,"\n");\
	fflush(stderr);

#define PRINT_LOGFILE(lfile,fmt,args) \
	fprintf(lfile,"%s local %s[%d]: ",date(0L),myname,getpid());\
	vfprintf(lfile,*fmt == '$' ? fmt+1 : fmt,args);\
	if (*fmt == '$')\
		fprintf(lfile,"\n\terrno=%d : %s\n",\
			errno,strerror(errno));\
	else\
		fprintf(lfile,"\n");\
	fflush(lfile);

#define PRINT_SYSLOG(level,fmt,args) \
	vsyslog(level,*fmt == '$' ? fmt+1 : fmt,args);\
	if (*fmt == '$')\
		syslog(level,"\terrno=%d : %s",\
                        errno,strerror(errno));

void loginf(va_alist)
va_dcl
{
	va_list	args;
	char	*fmt;
#ifndef HAS_SYSLOG
	int	oldmask;
#endif

	va_start(args);
	fmt=va_arg(args, char*);
	if (verbose)
	{
		PRINT_DEBUG(fmt,args);
	}
#ifdef HAS_SYSLOG
	if (!syslog_opened)
	{
		openlog(myname,LOG_PID|LOG_CONS,logfacility);
		syslog_opened=1;
	}
	PRINT_SYSLOG(LOG_INFO,fmt,args);
#else
	if (!logfile)
	{
		oldmask=umask(066);
		logfile=fopen(logname,"a");
		umask(oldmask);
		if (logfile == NULL)
		{
			perror("Cannot open log file");
			return;
		}
	}
	PRINT_LOGFILE(logfile,fmt,args);
#endif
	va_end(args);
	return;
}

void logerr(va_alist)
va_dcl
{
	va_list	args;
	char	*fmt;
#ifndef HAS_SYSLOG
	int	oldmask;
#endif

	va_start(args);
	fmt=va_arg(args, char*);
	if (verbose)
	{
		PRINT_DEBUG(fmt,args);
	}
#ifdef HAS_SYSLOG
	if (!syslog_opened)
	{
		openlog(myname,LOG_PID|LOG_CONS,logfacility);
		syslog_opened=1;
	}
	PRINT_SYSLOG(LOG_ERR,fmt,args);
#else
	if (!errfile)
	{
		oldmask=umask(066);
		errfile=fopen(errname,"a");
		umask(oldmask);
		if (errfile == NULL)
		{
			perror("Cannot open log file");
			return;
		}
	}
	PRINT_LOGFILE(errfile,fmt,args);
#endif
	va_end(args);
	return;
}

void debug(va_alist)
va_dcl
{
	va_list	args;
	unsigned long	level;
	char	*fmt;

	va_start(args);
	level=va_arg(args, unsigned long);
	fmt=va_arg(args, char*);

	if ((verbose && (level == 0)) || (verbose & (1 << (level-1))))
	{
		PRINT_DEBUG(fmt,args);
	}
	va_end(args);
	return;
}

unsigned long setverbose(s)
char *s;
{
	int shift;
	unsigned long tv=0L;
	int bad=0;

	if (strlen(s) == strspn(s,"0123456789"))
	{
		shift=atoi(s);
		if ((shift > 0) && (shift <= 32))
			while (--shift >= 0)
				tv |= 1<<shift;
		else if (shift != 0) bad=1;
	}
	else while ((shift=*s++))
	{
		if ((shift >= 'A') && (shift <= 'Z'))
			shift += ('a'-'A');
		if ((shift >= 'a') && (shift <= 'z'))
			tv |= 1<<(shift -'a');
		else bad=1;
	}

	if (tv && ((shift=getuid()) != 0) && (shift != geteuid()))
	{
		fprintf(stderr,"Debug request denied, run as user id %d\n",
			(int)geteuid());
		return 0L;
	}

	if (bad) fprintf(stderr,"invalid debug verbosity code\n");

	return tv;
}
