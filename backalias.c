#include <stdio.h>
#include <string.h>
#include "xutil.h"
#include "lutil.h"
#include "ftn.h"
#include "needed.h"

char *backalias(faddr *);
void readalias(char *);

static struct aliaslist {
	struct aliaslist *next;
	faddr *addr;
	char *alias;
} *alist = NULL;

char *backalias(fa)
faddr *fa;
{
	struct aliaslist *tmp;

	for (tmp=alist;tmp;tmp=tmp->next)
	if ((!fa->domain || !tmp->addr->domain ||
	     !strcasecmp(fa->domain,tmp->addr->domain)) &&
	    (!fa->zone   || (fa->zone == tmp->addr->zone)) &&
	    (fa->net == tmp->addr->net) &&
	    (fa->node == tmp->addr->node) &&
	    (fa->point == tmp->addr->point) &&
	    (fa->name) && (tmp->addr->name) &&
	    (strcasecmp(fa->name,tmp->addr->name) == 0))
	{
		debug(6,"Address \"%s\" has local alias \"%s\"",
			ascinode(fa,0x7f),S(tmp->alias));
		return tmp->alias;
	}
	return NULL;
}

void readalias(fn)
char *fn;
{
	FILE *fp;
	char buf[256],*k,*v;
	struct aliaslist *tmp=NULL;
	faddr *ta = NULL;

	if ((fp=fopen(fn,"r")) == NULL)
	{
		logerr("$cannot open system alias file %s",S(fn));
		return;
	}
	while (fgets(buf,sizeof(buf)-1,fp))
	{
		k=strtok(buf," \t:");
		v=strtok(NULL," \t:");
		if (k && v)
		if ((ta=parsefaddr(v)))
		{
			if (alist)
			{
				tmp->next=(struct aliaslist *)
					xmalloc(sizeof(struct aliaslist));
				tmp=tmp->next;
			}
			else
			{
				alist=(struct aliaslist *)
					xmalloc(sizeof(struct aliaslist));
				tmp=alist;
			}
			tmp->next=NULL;
			tmp->addr=ta;
			ta=NULL;
			tmp->alias=xstrcpy(k);
		}
	}
	fclose(fp);
}
