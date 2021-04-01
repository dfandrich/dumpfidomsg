/* ### Modified on Thu 4 Jan 1996 by P.Saratxaga ###
 * - added JE functions, for compatibility with JE-version config files
 * - added AREAS_HACKING
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lutil.h"
#include "xutil.h"
#include "areas.h"
#include "config.h"
#include "lhash.h"
#include "needed.h"
#ifdef JE
#include "charset.h"
#endif

typedef struct _areang {
	char *area;
	char *ng;
	char *dist;
#ifdef JE
	int incode;
	int outcode;
#endif
	char *moder;
} areang;

extern area_list *badgroups;
#ifdef GATEBAU_MSGID
extern area_list *gatebaugroups;
#endif
static LHASH *nglist_byarea=(LHASH*)0;
static LHASH *nglist_byng=(LHASH*)0;
char *defgroup=NULL,*defdist=NULL,*defarea=NULL;
int group_count;

static char *expand(p,q)
char *p,*q;
{
	char *r,*tmp,*ret;

	debug(9,"expanding \"%s\" + \"%s\"",S(p),S(q));

	tmp=xstrcpy(p);
	if ((r=strchr(tmp,'*'))) *r++='\0';
	ret=xstrcpy(tmp);
	if (r)
	{
		ret=xstrcat(ret,q);
		ret=xstrcat(ret,r);
	}
	free(tmp);
	debug(9,"expand result \"%s\"",S(ret));
	return ret;
}

static unsigned long hash_byarea(hash_entry)
areang *hash_entry;
{
	return lh_strhash(hash_entry->area);
}

static unsigned long hash_byng(hash_entry)
areang *hash_entry;
{
	return lh_strhash(hash_entry->ng);
}

static int cmp_byarea(e1,e2)
areang *e1,*e2;
{
	return strcmp(e1->area,e2->area);
}

static int cmp_byng(e1,e2)
areang *e1,*e2;
{
	return strcmp(e1->ng,e2->ng);
}

void tidy_arealist(al)
area_list *al;
{
	area_list *tmp;

	for (;al;al=tmp)
	{
		tmp=al->next;
		if (al->name) free(al->name);
		free(al);
	}
}

area_list *areas(ngroups,allowexpand)
char *ngroups;
int allowexpand;
{
	char *ng,*p,*q,*r=NULL;
	area_list *start=NULL,**cur,*bg;
	areang *ngl;
	areang dummy_entry;
	int forbid=0;

	if (ngroups == NULL) return NULL;
	ng=xstrcpy(ngroups);
	cur=&start;

	group_count=0;
	for (p=strtok(ng," ,\t\n");p;p=strtok(NULL," ,\t\n"))
	{
		group_count++;
		q=NULL;
		for (bg=badgroups;bg;bg=bg->next)
		if (!strncmp(bg->name,p,strlen(bg->name)))
		{
			loginf("Message not gated due to forbidden group %s",
				S(p));
			forbid=1;
			break;
		}
		dummy_entry.ng=p;
		if ((ngl=(areang*)lh_retrieve(nglist_byng,(char*)&dummy_entry)))
		{
			q=ngl->area;
		}
		else q=NULL;
		if (q || (defarea && allowexpand))
		{
			if (q == NULL)
			{
				for (r=p;*r;r++) *r=toupper(*r);
				q=expand(defarea,p);
			}
			debug(9,"newsgroup \"%s\" -> area \"%s\"",S(p),S(q));
			(*cur)=(area_list *)xmalloc(sizeof(area_list));
			(*cur)->next=NULL;
			(*cur)->name=xstrcpy(q);
			cur=&((*cur)->next);
		}
	}

	free(ng);
	if (forbid)
	{
		tidy_arealist(start);
		return NULL;
	}
	else return start;
}

#ifdef JE
void areacharset(area,incode,outcode)
char *area;
int *incode, *outcode;
{
	areang *ang;
	areang dummy_entry;
	char *p,*q;

	p=xstrcpy(area);
	if ((*p) && ((*(q=p+strlen(p)-1) == '\r') || (*q == '\n'))) *q='\0';

	dummy_entry.area=p;
	if ((ang=(areang*)lh_retrieve(nglist_byarea,(char*)&dummy_entry)))
	{
		if (*incode==CHRS_NOTSET) *incode = ang->incode;
		if (*outcode==CHRS_NOTSET) *outcode = ang->outcode;
		debug(9,"area \"%s\", incode %d, outcode %d",
			S(p), *incode, *outcode);
	}
	else
	{
		*incode = CHRS_NOTSET;
		*outcode = CHRS_NOTSET;
	}

	free(p);
}
#endif /* JE */

void ngdist(area,newsgroup,distribution,moderator,modtype)
char *area,**newsgroup,**distribution,**moderator;
int *modtype;
{
	areang *ang;
	areang dummy_entry;
	char *p,*q;

	*newsgroup=NULL;
	*distribution=NULL;
	*moderator=NULL;
	*modtype=0;
	p=xstrcpy(area);
	if ((*p) && ((*(q=p+strlen(p)-1) == '\r') || (*q == '\n'))) *q='\0';

	dummy_entry.area=p;
	if ((ang=(areang*)lh_retrieve(nglist_byarea,(char*)&dummy_entry)))
	{
		*newsgroup=ang->ng;
		*distribution=ang->dist;
		*moderator=ang->moder;
		if ((*moderator) && (q=strchr(*moderator,':')))
		{
			*q++='\0';
			if (strncasecmp(*moderator,"umod",4) == 0)
				*modtype=1; /* Usenet (Moderated) group */
			else if (strncasecmp(*moderator,"fro",3) == 0)
				*modtype=2; /* FTN read-only echoarea */
			else if (strncasecmp(*moderator,"fstd",4) == 0)
				*modtype=3; /* Fidonet moderated echoarea */
			else
			{
				loginf("Unknow moderation type %s for group %s",
							*moderator,*newsgroup);
				*modtype=0;
			}
			*moderator=q;
		}
		else if (*moderator) *modtype=2;

		debug(9,"area \"%s\" -> newsgroup \"%s\", dist \"%s\"",
			S(p),S(*newsgroup),S(*distribution));
		if(*moderator)
			debug(9,"modtype %d, moderator \"%s\"",
				*modtype,S(*moderator));
	}
	else
	{
		*newsgroup=NULL;
		*distribution=NULL;
		*moderator=NULL;
		*modtype=0;
	}

	if ((*newsgroup == NULL) && defgroup)
	{
		for (q=p;*q;q++) *q=tolower(*q);
		*newsgroup=expand(defgroup,p);
		*distribution=defdist;
		debug(9,"area \"%s\" -> default newsgroup \"%s\", dist \"%s\"",
			S(p),S(*newsgroup),S(*distribution));
	}

	if (*newsgroup == NULL)
	{
		loginf("No newsgroup for area tag %s",S(p));
	}

	free(p);
}

#ifdef AREAS_HACKING
void readareas(fn,site)
char *fn;
char *site;
#else
void readareas(fn)
char *fn;
#endif
{
	areang *cur;
	FILE *fp=NULL;
#ifdef JE
	char buf[128],*p,*q,*r,*s,*t;
	int incode, outcode;
#else
	char buf[128],*p,*q,*r;
#endif /* JE */
	char *u;
#ifdef AREAS_HACKING
#define MAXFILELEN 1024
	char namebuffer[MAXFILELEN];
#endif

	if (defgroup) free(defgroup);
	defgroup=NULL;
	if (defdist) free(defdist);
	defdist=NULL;
	if (defarea) free(defarea);
	defarea=NULL;

	if (!nglist_byarea) nglist_byarea=lh_new(hash_byarea,cmp_byarea);
	if (!nglist_byng) nglist_byng=lh_new(hash_byng,cmp_byng);

#ifdef AREAS_HACKING
	/* Tries to open "Areas.fidosite". Then defaults to the
	 * usual Areas.
	 */
	if (site) {
		sprintf(namebuffer, "%s.%s", fn, site);
		fp = fopen(namebuffer, "r");
		while ((!fp) && (site))
		{
			debug(5,"$unable to open area file %s", S(namebuffer));
			if ((p=strrchr(site,'.')))
			{
				*p='\0';
				sprintf(namebuffer, "%s.%s", fn, site);
				fp = fopen(namebuffer, "r");
			}
			else
				site='\0';
		}
			
	}
	if (!fp) {
#endif
		if ((fp=fopen(fn,"r")) == NULL)
		{
			logerr("$unable to open area file %s",S(fn));
			return;
		}
#ifdef AREAS_HACKING
	}
#endif
	while (fgets(buf,sizeof(buf)-1,fp))
	{
		if (isspace(buf[0])) continue;
#if defined(AREAS_NUMERAL_COMMENTS) || defined(AREAS_NUMERAL_COMPATIBILITY) 
		for (p=buf;(p=strchr(p, '#'));)
		if (p>buf && *(p-1)=='\\') {
			strcpy(p-1,p); /* discard the '\\' */
		/* this is illegal, p-1 and p overlap */
		} else { /* comment */
			*p='\0';
#ifdef AREAS_NUMERAL_COMPATIBILITY
			loginf("Warning! format of Areas files has changed;");
			loginf("usage of '#' as comment char will be dropped;");
			loginf("now comments are lines beginning whith a blank");
#endif
		}
#endif
		p=strtok(buf," \t\n");
		q=strtok(NULL," \t\n");
		r=strtok(NULL," \t\n");
#ifdef JE
		s=strtok(NULL," \t\n"); /* character conversion rule (in=inside=internal code) */
		t=strtok(NULL," \t\n"); /* character conversion rule (out=outside) */
		incode = CHRS_NOTSET;
		if(s) incode = getcode(s);
		outcode = CHRS_NOTSET;
		if(t) outcode = readchrs(t);
#endif /* JE */
		u=strtok(NULL," \t\n"); /* for Approved: line of read-only echos */

		if (p && q)
		{
			if (strcmp(q,"*") == 0)
			{
				debug(9,"adding default areatag \"%s\"",S(p));
				defarea=xstrcpy(p);
			}
			if (strcmp(p,"*") == 0)
			{
				debug(9,"adding default group \"%s\" (dist %s)",
					S(q),S(r));
				defgroup=xstrcpy(q);
				defdist=xstrcpy(r);
			}
			if ((strcmp(p,"*") != 0) && (strcmp(q,"*") != 0))
			{
				debug(9,"adding area \"%s\" for ng \"%s\" (dist %s)",
					S(p),S(q),S(r));
				cur=(areang*)malloc(sizeof(areang));
				cur->area=xstrcpy(p);
				cur->ng=xstrcpy(q);
				if ((r) && (strcasecmp(r,"world")))
					cur->dist=xstrcpy(r);
				else cur->dist=NULL;
#ifdef JE
				cur->incode = incode;
				cur->outcode = outcode;
#endif
				if (u) cur->moder=xstrcpy(u);
				else cur->moder=NULL;
				lh_insert(nglist_byarea,(char*)cur);
				lh_insert(nglist_byng,(char*)cur);
			}
		}
	}
	fclose(fp);
}

#ifdef GATEBAU_MSGID
int areagatebau(area)
char *area;
{
	area_list *gg;
	areang *ang;
	areang dummy_entry;
	char *p,*q;

	if (!area) return 0;

	p=xstrcpy(area);
	if ((*p) && ((*(q=p+strlen(p)-1) == '\r') || (*q == '\n'))) *q='\0';

	dummy_entry.area=p;
	if ((ang=(areang*)lh_retrieve(nglist_byarea,(char*)&dummy_entry)))
	{
		for (gg=gatebaugroups;gg;gg=gg->next)
		if (!strncmp(gg->name,ang->ng,strlen(gg->name)))
		{
			free(p);
			return 1;
		}
	}
	free(p);

	return 0;
}
#endif
