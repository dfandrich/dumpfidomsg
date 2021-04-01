#include <stdlib.h>
#include <string.h>
#include "xutil.h"
#include "lutil.h"
#include "ftn.h"
#include "falists.h"

int in_list(addr,fap)
faddr *addr;
fa_list **fap;
{
	fa_list *tmp;

	debug(8,"Seeking seen-by match for %s",ascinode(addr,0x06));

	if (addr->point)
	{
		debug(8,"No seen-by check for point address");
		return 0;
	}

	for (tmp=*fap;tmp;tmp=tmp->next)
		if ((tmp->addr->net == addr->net) &&
		    (tmp->addr->node == addr->node))
		{
			debug(8,"Match found");
			return 1;
		}
	debug(8,"Match not found");
	return 0;
}

void fill_list(fap,str,omit)
fa_list **fap;
char *str;
fa_list **omit;
{
	fa_list *tmp;
	faddr *ta;
	static unsigned int oldzone=0, oldnet=0, oldnode=0;
	char *buf,*p,*q,*r;
	int allowskip=1;

	if ((str == NULL) || (*str == '\0'))return;

	debug(8,"fill_list %s",str);
	buf=xstrcpy(str);
	r=buf+strlen(buf);
	for (p=strtok(buf," \t\n"),q=p+strlen(p)+1;
	     p;
	     p=(q<r)?strtok(q," \t\n"):NULL,q=p?p+strlen(p)+1:r)
	if ((ta=parsefnode(p)))
	{
		if (ta->zone == 0) ta->zone=oldzone;
		else oldzone=ta->zone;
		if (ta->net == 0) ta->net=oldnet;
		else oldnet=ta->net;
		if ((ta->point != 0) && (ta->node == 0)) ta->node=oldnode;
		else oldnode=ta->node;
		if (allowskip && omit && *omit && (metric(ta,(*omit)->addr) == 0))
		{
			debug(8,"omit %s",ascfnode(ta,0x1f));
			tmp=*omit;
			*omit=(*omit)->next;
			tmp->next=NULL;
			tidy_falist(&tmp);
		}
		else
		{
			debug(8,"add %s",ascfnode(ta,0x1f));
			allowskip=0;
			tmp=(fa_list *)xmalloc(sizeof(fa_list));
			tmp->next=*fap;
			tmp->addr=ta;
			*fap=tmp;
		}
	}
	free(buf);
	oldzone=0;
	for (tmp=*fap;tmp;tmp=tmp->next)
		debug(8,"fill_list returns: %s",ascfnode(tmp->addr,0x06));
	return;
}

void fill_rlist(fap,str)
fa_list **fap;
char *str;
{
	fa_list *tmp;
	faddr *ta;
	static unsigned int oldnet;
	char *buf,*p,*q;

	if ((str == NULL) || (*str == '\0')) return;

	debug(8,"fill_rlist %s",str);
	buf=xstrcpy(str);
	for (p=buf,q=strchr(p,'!');*p;p=q,q=strchr(p,'!'))
	{
		if (q) *q++='\0';
		else q=p+strlen(p);
		if ((ta=parsefaddr(p)))
		{
			if (ta->net == 0) ta->net=oldnet;
			else oldnet=ta->net;
			tmp=(fa_list *)xmalloc(sizeof(fa_list));
			tmp->next=*fap;
			tmp->addr=ta;
			*fap=tmp;
		}
	}
	free(buf);
	for (tmp=*fap;tmp;tmp=tmp->next)
		debug(8,"fill_rlist returns: %s",ascfnode(tmp->addr,0x06));
	return;
}

void fill_path(fap,str)
fa_list **fap;
char *str;
{
	fa_list **tmp;
	faddr *ta;
	static unsigned int oldnet;
	char *buf,*p,*q,*r;

	if ((str == NULL) || (*str == '\0')) return;

	debug(8,"fill_path %s",str);
	buf=xstrcpy(str);
	for (tmp=fap;*tmp;tmp=&((*tmp)->next)) /*nothing*/;
	r=buf+strlen(buf);
	for (p=strtok(buf," \t\n"),q=p+strlen(p)+1;
	     p;
	     p=(q<r)?strtok(q," \t\n"):NULL,q=p?p+strlen(p)+1:r)
	if ((ta=parsefnode(p)))
	{
		if (ta->net == 0) ta->net=oldnet;
		else oldnet=ta->net;
		*tmp=(fa_list *)xmalloc(sizeof(fa_list));
		(*tmp)->next=NULL;
		(*tmp)->addr=ta;
		tmp=&((*tmp)->next);
	}
	free(buf);
	for (tmp=fap;*tmp;tmp=&((*tmp)->next))
		debug(8,"fill_path returns: %s",ascfnode((*tmp)->addr,0x06));
	return;
}

int compaddr(fa_list **,fa_list **);

void uniq_list(fap)
fa_list **fap;
{
	fa_list *ta,*tan;

	debug(8,"uniq_list beginning from %s",
		*fap?ascinode((*fap)->addr,0x06):"<null>");
	for (ta=*fap;ta;ta=ta->next)
	{
		while ((tan=ta->next) && (compaddr(&ta,&tan) == 0))
		{
			ta->next=tan->next;
			tidy_faddr(tan->addr);
			free(tan);
		}
		ta->next=tan;
		debug(8,"path addr after unique: %s",
			ascfnode(ta->addr,0x06));
	}
}

void sort_list(fap)
fa_list **fap;
{
	fa_list *ta,**vector;
	size_t n=0,i;

	debug(8,"sort_list beginning from %s",
		*fap?ascinode((*fap)->addr,0x06):"<null>");
	for (ta=*fap;ta;ta=ta->next) n++;
	vector=(fa_list **)xmalloc(n * sizeof(fa_list *));
	debug(8,"Sorting %d seen-by entries",n);
	i=0;
	for (ta=*fap;ta;ta=ta->next)
	{
		vector[i++]=ta;
		debug(8,"seen-by addr before sort: %s",
			ascfnode(ta->addr,0x06));
	}
	qsort(vector,n,sizeof(faddr*),
		(int(*)(const void*,const void*))compaddr);
	for (i=0;i<n;i++)
		debug(8,"seen-by addr after sort: %s",
			ascfnode(vector[i]->addr,0x06));
	(*fap)=vector[0];
	i=1;
	for (ta=*fap;ta;ta=ta->next)
	{
		while ((i < n) && (compaddr(&ta,&(vector[i])) == 0))
		{
			tidy_faddr((vector[i])->addr);
			free(vector[i]);
			i++;
		}
		if (i < n) ta->next=vector[i++];
		else ta->next=NULL;
		debug(8,"seen-by addr after unique: %s",
			ascfnode(ta->addr,0x06));
	}
	free(vector);
	return;
}

int compaddr(fap1,fap2)
fa_list **fap1,**fap2;
{
	if ((*fap1)->addr->net != (*fap2)->addr->net)
		return ((*fap1)->addr->net - (*fap2)->addr->net);
	else
		return ((*fap1)->addr->node - (*fap2)->addr->node);
}
