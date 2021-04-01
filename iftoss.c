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
#include "getopt.h"
#include "lutil.h"
#include "config.h"
#include "version.h"
#include "ftn.h"
#include "getheader.h"
#include "trap.h"
#include "acl.h"
#ifdef DIRTY_CHRS
#include "charset.h"
int dirtyoutcode;
#endif
#ifdef REFERENCES_MSC96
#include "ref_interface.h"
ref_private_t *ref_dbase;
#endif

#ifndef FAKEDIR
#define FAKEDIR "/tmp/ifmail/"
#endif

extern int getmessage(FILE *,faddr *,faddr *);
#ifdef AREAS_HACKING
extern void readareas(char *, char *);
#else
extern void readareas(char *);
#endif
extern void readalias(char *);
extern int exclose(FILE *);

extern int num_echo,num_mail;
extern long paranoid;

acl *packet_acl, *message_acl, *origin_acl;

int usetmp=1; /* to tell bgets that we do not use batch mode */
int notransports=0;

void usage(name)
char *name;
{
#ifdef RELAXED
	confusage("-N");
	fprintf(stderr,_("-N\t\tput messages to %s directory\n"),FAKEDIR);
#else
	confusage("-N -f");
	fprintf(stderr,_("-N\t\tput messages to %s directory\n"),FAKEDIR);
	fprintf(stderr,_("-f\t\tforce tossing of packets addressed to other nodes\n"));
#endif
}

FILE *nb = NULL;

int main(argc,argv)
int argc;
char *argv[];
{
	int c;
	int rc,maxrc;
#ifdef RELAXED
	int relaxed=1;
#else
	int relaxed=0;
#endif
	faddr from,to;
#ifdef DIRTY_CHRS
	fa_list *pa;
#endif

#if defined(HAS_SYSLOG) && defined(MAILLOG)
	logfacility=MAILLOG;
#endif

	setmyname(argv[0]);
	catch(myname);
#ifdef RELAXED
	while ((c=getopt(argc,argv,"Nhx:I:")) != -1)
#else
	while ((c=getopt(argc,argv,"Nfhx:I:")) != -1)
#endif
	if (confopt(c,optarg)) switch (c)
	{
		case 'N':	notransports=1; break;
#ifndef RELAXED
		case 'f':	relaxed=1; break;
#endif
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
	if (aliasfile) readalias(aliasfile);

	packet_acl=read_acl(pktaclfile);
	message_acl=read_acl(msgaclfile);
	origin_acl=read_acl(orgaclfile);

	if (notransports)
	{
		mkdir(FAKEDIR,0777);
		loginf("messages/newsbatches will go to %s",FAKEDIR);
	}

	switch ((rc=getheader(&from,&to,stdin))) {
	    case 0:
		break;
	    case 2:
		logerr("bad packet, aborting");
		exit(rc);
	    case 3:
		if (relaxed)
		    break;
		logerr("packet not to this node, aborting");
		exit(rc);
	    case 4:
		if (!paranoid)
		    break;
		logerr("bad password, aborting");
		exit(rc);
	    default:
		logerr("can't happen: getheader returned %d", rc);
		exit(rc);
	}
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
		if (notransports) rc=fclose(nb);
		else rc=exclose(nb);
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
