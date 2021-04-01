/* ### Modified by P.Saratxaga on 4 Jan 1996 ###
 * - added pktpwlist from T.Tanaka
 * - added pkt 2.2 support
 * - added fsc-0048 support
 */

#include <stdio.h>
#include <string.h>
#include "lutil.h"
#include "xutil.h"
#include "ftn.h"
#include "bread.h"
#include "config.h"

#define min(a,b)	((a) <= (b) ? (a) : (b))

faddr pktfrom;
char pktpwd[9];

int getheader(f,t,pkt)
faddr *f,*t;
FILE *pkt;
{
	int i,pwdok;
	int capword,capvalid;
	int tome,pointcheck;
	int pkt_sub;
	fa_list *al;
	char *p,*pktpassword;

	f->domain=NULL;
	f->name=NULL;
	t->domain=NULL;
	t->name=NULL;

	debug(5,"from node: %u",f->node=iread(pkt));
	debug(5,"to   node: %u",t->node=iread(pkt));
	debug(5,"year     : %u",capword=iread(pkt));
	debug(5,"month    : %u",capvalid=iread(pkt));
	debug(5,"day      : %u",iread(pkt));
	debug(5,"hour     : %u",iread(pkt));
	debug(5,"min      : %u",iread(pkt));
	debug(5,"sec      : %u",iread(pkt));
	debug(5,"rate     : %u",pkt_sub=iread(pkt));
	if (pkt_sub==2) {
		debug(5,"In fact it is a pkt 2.2 packet; so we have");
		debug(5,"from pnt : %u",f->point=capword);
		debug(5,"to   pnt : %u",t->point=capvalid);
		debug(5,"subver   : %u",pkt_sub);
		debug(5,"instead of year, month and rate respectively");
		debug(5,"(hour, min, sec are meaningless)");
	} else { pkt_sub=0; /* keep pkt sub version at 0 */ }
	debug(5,"ver      : %u",iread(pkt));
	f->net=iread(pkt);
	if ((f->net==-1) && (pkt_sub==0)) {
		 debug(5,"This is an fsc-0048 packet from a point");
	} else {
		debug(5,"from net : %u",f->net);
	}
	debug(5,"to   net : %u",t->net=iread(pkt));
	debug(5,"prodx    : 0x%04x",iread(pkt));
	for (i=0;i<8;i++) pktpwd[i]=getc(pkt);
	pktpwd[8]='\0';
	for (p=pktpwd+7;(p >= pktpwd) && (*p == ' ');p--) *p='\0';
	debug(5,"password : %s",pktpwd);
	if (pktpwd[0]) f->name=pktpwd;
	debug(5,"from zone: %u",f->zone=iread(pkt));
	debug(5,"to   zone: %u",t->zone=iread(pkt));
	if (pkt_sub==2) {
		for (i=0;i<8;i++) *(p+i)=getc(pkt);
		*(p+8)='\0';
		debug(5,"from dom : %s",p);
		if (*p) f->domain=xstrcpy(p);
		for (i=0;i<8;i++) *(p+i)=getc(pkt);
		*(p+8)='\0';
		debug(5,"to   dom : %s",p);
		if (*p) t->domain=xstrcpy(p);
	} else {
		if ((f->net==-1) && (pkt_sub==0)) {
			debug(5,"from net : %u",f->net=iread(pkt));
		} else {
			debug(5,"filler   : 0x%04x",iread(pkt));
		}
		debug(5,"capvalid : 0x%04x",capvalid=iread(pkt));
		debug(5,"prodcode : 0x%04x",iread(pkt));
		debug(5,"capword  : 0x%04x",capword=iread(pkt));
		debug(5,"from zone: %u",iread(pkt));
		debug(5,"to   zone: %u",iread(pkt));
		debug(5,"from pnt : %u",f->point=iread(pkt));
		debug(5,"to   pnt : %u",t->point=iread(pkt));
	}
	debug(5,"proddata : 0x%08lx",lread(pkt));

	if (feof(pkt) || ferror(pkt)) 
	{
		logerr("$Could not read packet header");
		return 2;
	}

	pointcheck=0;
	if (pkt_sub==2) {
		pointcheck=1;
	} else {
		if (((capword >> 8) == (capvalid & 0xff)) &&
		    ((capvalid >> 8) == (capword & 0xff)))
			pointcheck=(capword & 0x0001);
		else capword=0;
	}

	debug(5,"capword=%04x, pointcheck=%s",capword,pointcheck?"yes":"no");

	pktfrom.name=NULL;
	pktfrom.domain=f->domain;
	pktfrom.zone=f->zone;
	pktfrom.net=f->net;
	pktfrom.node=f->node;
	if (pointcheck) pktfrom.point=f->point;
	else pktfrom.point=0;

	tome=0;
	for (al=whoami;al;al=al->next)
	{
	if (((t->domain == NULL) ||
	     (strncasecmp(al->addr->domain,t->domain,
		min(strlen(al->addr->domain),strlen(t->domain))))) &&
	    ((t->zone == 0) || (t->zone == al->addr->zone)) &&
	    (t->net == al->addr->net) &&
	    (t->node == al->addr->node) &&
	    ((!pointcheck) || (t->point == al->addr->point)))
		tome=1;
	}

	pwdok=0;
	pktpassword=NULL;

	/* look between packetpasswd lines */
	for (al=pktpwlist;al;al=al->next)
	{
	if (((f->domain == NULL) ||
	     (strncasecmp(al->addr->domain,f->domain,
		min(strlen(al->addr->domain),strlen(f->domain))))) &&
	    ((f->zone == 0) || (f->zone == al->addr->zone)) &&
	    (f->net == al->addr->net) &&
	    (f->node == al->addr->node) &&
	    ((!pointcheck) || (f->point == al->addr->point)))
		{
			pktpassword=al->addr->name;
			pwdok=1;
		}
	}
	/* if not matching packetpasswd lines, search in password ones */
	if (!pwdok) for (al=pwlist;al;al=al->next)
	{
	if (((f->domain == NULL) ||
	     (strncasecmp(al->addr->domain,f->domain,
		min(strlen(al->addr->domain),strlen(f->domain))))) &&
	    ((f->zone == 0) || (f->zone == al->addr->zone)) &&
	    (f->net == al->addr->net) &&
	    (f->node == al->addr->node) &&
	    ((!pointcheck) || (f->point == al->addr->point)))
		{
			pktpassword=al->addr->name;
			pwdok=1;
		}
	}
	
	/* if we have a password (general or for packets) for that address */
	if ((pwdok) && (strcasecmp(pktpassword,pktpwd) != 0))
	{
			loginf("password got \"%s\", expected \"%s\"",
				pktpwd,pktpassword);
			pwdok=0;
	}

	loginf("packet from node %s",ascfnode(f,0x1f));
	loginf("         to node %s",ascfnode(t,0x1f));

	if (!tome) return 3;
	else if (!pwdok) return 4;
	else return 0;
}
