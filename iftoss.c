/* ### Modified on 15 Jun 96 by P.Saratxaga 
 * - added REFERENCES_MSC96 from Marc Schaeffer
 * - added AREAS_HACKING from Marc Schaeffer
 */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#ifdef HAS_SYSLOG
#include <syslog.h>
#endif
#include <getopt.h>
#include "lutil.h"
#include "config.h"
#include "version.h"
#include "ftn.h"
#include "getheader.h"
#include "trap.h"
#ifdef DIRTY_CHRS
#include "charset.h"
int dirtyoutcode;
#endif
#ifdef REFERENCES_MSC96
  #include "ref_interface.h"
ref_private_t *ref_dbase;
#endif

extern int getmessage(FILE *,faddr *,faddr *);
#ifdef AREAS_HACKING
extern void readareas(char *, char *);
#else
extern void readareas(char *);
#endif
extern void readalias(char *);

extern int num_echo,num_mail;

int usetmp=1; /* to tell bgets that we do not use batch mode */

void usage(name)
char *name;
{
    confusage("");
}

FILE *nb = NULL;

int main(argc,argv)
int argc;
char *argv[];
{
	int c;
	int rc,maxrc;
	int relaxed=1;
	faddr from={0},to={0};
#ifdef DIRTY_CHRS
	fa_list *pa;
#endif

#if defined(HAS_SYSLOG) && defined(MAILLOG)
	logfacility=MAILLOG;
#endif

	setmyname(argv[0]);
	catch(myname);
	while ((c=getopt(argc,argv,"hx:I:")) != -1)
	if (confopt(c,optarg)) switch (c)
	{
		default:	usage(argv[0]); exit(1);
	}

	if (readconfig())
	{
		logerr("Error getting configuration, aborting");
		exit(1);
	}

#ifdef REFERENCES_MSC96
	/* even if not newsmode */
	if (refdbm)
		ref_dbase = ref_init(refdbm);
#endif

#ifndef AREAS_HACKING
	readareas(areafile);
#endif
	//if (aliasfile) readalias(aliasfile);

	//packet_acl=read_acl(pktaclfile);
	//message_acl=read_acl(msgaclfile);
	//origin_acl=read_acl(orgaclfile);

#ifdef AREAS_HACKING
	readareas(areafile, ascinode(&from, 0x3f));
#endif


#ifdef DIRTY_CHRS
	dirtyoutcode=CHRS_NOTSET;
	for (pa=dirtychrslist;pa;pa=pa->next)
	{
		if (metric(pa->addr,&from) == 0)
		{	
			dirtyoutcode=readchrs(pa->addr->name);
			if (dirtyoutcode == CHRS_NOTSET)
			dirtyoutcode=getcode(pa->addr->name);
		}
	}
#endif

	while ((rc=getmessage(stdin,&from,&to)) == 1);

	maxrc=rc;

	if (nb)
	{
		if (rc < 0) rc=10-rc;
		if (rc > maxrc) maxrc=rc;
	}

	loginf("end %d echomail, %d netmail messages processed, rc=%d",
		num_echo,num_mail,maxrc);

#ifdef REFERENCES_MSC96
	/* even if not newsmode  */
	if (refdbm)
		ref_deinit(ref_dbase);
#endif

	return maxrc;
}
