/* ### Modified by P.Saratxaga on 12 Aug 1995 ###
 * - little hack to allox \t and \n in kludge lines
 * - added #include "crc.h"
 * - added bNeedToGetAddressFromMsgid code from T.Tanaka
 * - No badkludge if ^aRFC-
 */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "xutil.h"
#include "lutil.h"
#include "bread.h"
#include "ftn.h"
#include "rfcmsg.h"
#include "mkrfcmsg.h"
#include "config.h"
#include "crc.h"
#include "charset.h"
//#include "acl.h"

#define KWDCHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
		"abcdefghijklmnopqrstuvwxyz" \
		"0123456789-_"

extern time_t parsedate(char *,void *);
extern char *rfcdate(time_t);

int pgpsigned;

//extern acl *packet_acl, *message_acl, *origin_acl;

static time_t parsefdate(char *,void *);
static time_t parsefdate(str,now)
char *str;
void *now;
{
	if (str == NULL) return (time_t)0;

	if ((strncasecmp(str,"Sun ",4) == 0) ||
	    (strncasecmp(str,"Mon ",4) == 0) ||
	    (strncasecmp(str,"Tue ",4) == 0) ||
	    (strncasecmp(str,"Wed ",4) == 0) ||
	    (strncasecmp(str,"Thu ",4) == 0) ||
	    (strncasecmp(str,"Fri ",4) == 0) ||
	    (strncasecmp(str,"Sat ",4) == 0))
		str+=4;
	return parsedate(str,now);
}

/* 0-no more messages, 1-more messages, 2-bad file */
int getmessage(pkt,p_from,p_to)
FILE *pkt;
faddr *p_from,*p_to;
{
	char buf[BUFSIZ];
	char *subj=NULL,*orig=NULL;
	char *p,*l,*r;
	rfcmsg *kmsg=NULL,**tmsg;
	int tmp,rc,maxrc=0;
	faddr f,t,*o;
	faddr m_from, m_to;
	int flags;
	int waskluge,badkludge;
	time_t mdate=0L;
	off_t endmsg_off,tear_off,orig_off,via_off;
	FILE *fp;
	int bNeedToGetAddressFromMsgid = (int)NULL;

	tmsg=&kmsg;

	pgpsigned=0;

	f.zone=0;
	t.zone=0;
	f.point=0;
	t.point=0;
	f.name=NULL;
	t.name=NULL;
	f.domain=NULL;
	t.domain=NULL;

    if (fread(buf, 1, 36, pkt) != 36) {
		loginf("Could not read message start, aborting");
		return 3;
    }
    buf[36] = 0;
	f.name=xstrcpy(buf);

    if (fread(buf, 1, 36, pkt) != 36) {
		loginf("Could not read message, aborting");
		return 3;
    }
    buf[36] = 0;
	t.name=xstrcpy(buf);

    if (fread(buf, 1, 72, pkt) != 72) {
		loginf("Could not read message, aborting");
		return 3;
    }
    buf[72] = 0;
	subj=xstrcpy(buf);

    if (fread(buf, 1, 20, pkt) != 20) {
		loginf("Could not read message, aborting");
		return 3;
    }
    buf[20] = 0;
	mdate=parsefdate(buf,NULL);

	tmp=iread(pkt); // times
	t.node=iread(pkt);
	f.node=iread(pkt);
	tmp=iread(pkt); // cost
	f.net=iread(pkt);
	t.net=iread(pkt);
	tmp=iread(pkt); // written date l
	tmp=iread(pkt); // written date h
	tmp=iread(pkt); // arrived date l
	tmp=iread(pkt); // arrived date h
	tmp=iread(pkt); // reply
	flags=iread(pkt);
	tmp=iread(pkt); // up

	if (feof(pkt) || ferror(pkt))
	{
		loginf("Could not read message header, aborting");
		return 3;
	}

	f.zone=p_from->zone;
	t.zone=p_to->zone;

	debug(5,"message from %s",ascfnode(&f,0x7f));
	debug(5,"message to   %s",ascfnode(&t,0x7f));
	debug(5,"message subj \"%s\"",S(subj));
	debug(5,"message date \"%s\"",rfcdate(mdate));

	/* We need only numeric fields */
	m_from=f;
	m_to=t;

	tear_off=0L;
	orig_off=0L;
	via_off=0L;
	waskluge=0;
	if ((fp=tmpfile()) == NULL)
	{
		logerr("$unable to open temporary file");
		return 4;
	}
	while (aread(buf,sizeof(buf)-1,pkt))
	if ((buf[0] == '\1') ||
	    !strncmp(buf,"AREA:",5) ||
	    !strncmp(buf,"SEEN-BY",7)) /* This is a kluge line */
	{
		waskluge=1;
		badkludge=0;
		if (buf[0] == '\1')
		{
			l=buf+1;
	    		if (!strncmp(l,"AREA:",5) ||
	    		    !strncmp(l,"SEEN-BY",7))
				badkludge=1;
		}
		else l=buf;
		if (*l == '\n') badkludge=1;
		else while (isspace(*l)) l++;
		if (strncmp(l,"RFC-",4))
		for (p=l;*p;p++)
			if ((*p != '\n') && (((*p)&0x7f) < ' ')) badkludge=1;
		p=strchr(l,':');
		r=strchr(l,' ');
		if (p && (!r || (r > p))) r=p;
		else p=r;
		if (r == NULL) badkludge=1;
		else if (!*(p+1) || (*(p+1)=='\n')) badkludge=1;
		else
		{
			char c=*r;
			*r='\0';
			if (strspn(l,KWDCHARS) != strlen(l)) badkludge=1;
			*r=c;
		}

		*tmsg=(rfcmsg *)xmalloc(sizeof(rfcmsg));
		(*tmsg)->next=NULL;
		if (badkludge)
		{
			(*tmsg)->key=xstrcpy("KLUDGE");
			p=printable(l,0);
			r=p+strlen(p)-2;
			if (strcmp(r,"\\n") == 0)
			{
				*r++='\n';
				*r='\0';
			}
			(*tmsg)->val=xstrcpy(p);
		}
		else
		{
			*r++='\0';
			while (isspace(*r)) r++;
			(*tmsg)->key=xstrcpy(l);
			(*tmsg)->val=xstrcpy(r);
		}
		tmsg=&((*tmsg)->next);

		if (!strcmp(l,"Via") && (via_off == 0L))
		{ 
			via_off=ftell(fp);
			debug(5,"^AVia \"%s\" at offset %ld",
				buf,(long)via_off);
		}
	}
	else /* this is not a kludge line */
	{
		if (waskluge && (isspace(buf[0])))
			fputs("\n",fp); /* first body line is not RFC hdr */
		waskluge=0;
		if (!strncmp(buf,PGP_SIGNED_BEGIN,strlen(PGP_SIGNED_BEGIN)))
			pgpsigned=1;
	    	else if ((!strncmp(buf,"---",3)) &&
			 ((buf[3] == '\r') || (buf[3] == ' ') ||
			  (buf[3] == '\n')))
		{
			tear_off=ftell(fp);
			if ((hdr("Tearline",kmsg) == NULL))
			{
				*tmsg=(rfcmsg *)xmalloc(sizeof(rfcmsg));
				(*tmsg)->next=NULL;
				(*tmsg)->key=xstrcpy("Tearline");
				if (strlen(buf+3) == strspn(buf+3," \t\r\n"))
					(*tmsg)->val=xstrcpy("(none)\n");
				else
					(*tmsg)->val=xstrcpy(buf+4);
				tmsg=&((*tmsg)->next);
			}
			debug(5,"tearline \"%s\" at offset %ld",
				buf,(long)tear_off);
		}
		else if (!strncmp(buf," * Origin:",10))
		{
			orig_off=ftell(fp);
			*tmsg=(rfcmsg *)xmalloc(sizeof(rfcmsg));
			(*tmsg)->next=NULL;
			(*tmsg)->key=xstrcpy("Origin");
			(*tmsg)->val=xstrcpy(buf+11);
			tmsg=&((*tmsg)->next);
			debug(5,"origin \"%s\" at offset %ld",
				buf,(long)orig_off);
			p=buf+10;
			while (*p == ' ') p++;
			if ((l=strrchr(p,'(')) && (r=strrchr(p,')')) &&
			    (l < r))
			{
				*l='\0';
				*r='\0';
				l++;
				if ((o=parsefnode(l)))
				{
					f.point=o->point;
					f.node=o->node;
					f.net=o->net;
					f.zone=o->zone;
					if (o->domain) f.domain=o->domain;
					o->domain=NULL;
					tidy_faddr(o);
					debug(5,"Origin from: %s",
						ascfnode(&f,0x7f));
				}
			}
			else {
				bNeedToGetAddressFromMsgid = !NULL;
				loginf("Couldn't find address in origin line (%s of %s, [%s])", f.name, ascfnode(&f, 0x1f), hdr("Origin", kmsg));
				if (*(l=p+strlen(p)-1) == '\n') *l='\0';
			}
			for (l=p+strlen(p)-1;*l == ' ';l--) *l='\0';
			orig=xstrcpy(p);
		}
		else if (!strncmp(buf," * Message split",16))
		{
			*tmsg=(rfcmsg *)xmalloc(sizeof(rfcmsg));
			(*tmsg)->next=NULL;
			(*tmsg)->key=xstrcpy("Split");
			(*tmsg)->val=xstrcpy("already\n");
			tmsg=&((*tmsg)->next);
			debug(5,"Split indicator found");
		}
		fputs(buf,fp);
	}

	if(bNeedToGetAddressFromMsgid && (p = hdr("MSGID", kmsg))) {
		l = p;
		while(isspace(*l) && *l)
			l++;
		r = strchr(l, ' ');
		if(r) {
			*r-- = '\0';
			while(isspace(*r) && *r)
				r--;
		}
		if (l && r && l > r) {
			if ((o = parsefnode(l))) {
				f.point = o->point;
				f.node = o->node;
				f.net = o->net;
				f.zone = o->zone;
				if (o->domain) f.domain = o->domain;
				o->domain = NULL;
				tidy_faddr(o);
				/*debug(5,"Origin from: %s (src MSGID)",
					ascfnode(&f,0x7f));*/
				loginf("Origin from: %s (src MSGID)",
					ascfnode(&f,0x7f));
			}
		}
	}

	endmsg_off=ftell(fp);
	if ((tear_off) && (tear_off < endmsg_off)) endmsg_off=tear_off;
	if ((orig_off) && (orig_off < endmsg_off)) endmsg_off=orig_off;
	if ((via_off) && (via_off < endmsg_off)) endmsg_off=via_off;
	debug(5,"end message offset %ld",(long)endmsg_off);
	
	rewind(fp);
	rc=mkrfcmsg(&f,&t,subj,orig,mdate,flags,fp,endmsg_off,kmsg);
	if (rc) rc+=10;
	if (rc > maxrc) maxrc=rc;

skip:
	fclose(fp);
	tidyrfc(kmsg);
	if(f.name) free(f.name); f.name=NULL;
	if(t.name) free(t.name); t.name=NULL;
	if(f.domain) free(f.domain); f.domain=NULL;
	if(t.domain) free(t.domain); t.domain=NULL;

	if (!feof(pkt) || ferror(pkt))
	{
		logerr("Unexpected error in packet");
		return 5;
	}
	return 0;
}
