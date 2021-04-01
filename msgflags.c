#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "xutil.h"
#include "lutil.h"
#include "ftnmsg.h"

static char *flnm[] = {
	"PVT","CRS","RCV","SNT","ATT","TRN","ORP","K/S",
	"LOC","HLD","RSV","FRQ","RRQ","RRC","ARQ","FUP"
};

int flag_on(char *,char *);

int flagset(s)
char *s;
{
	char *buf,*p;
	int i;
	int fl=0;

	debug(5,"setting flags from string \"%s\"",S(s));
	buf=xstrcpy(s);
	for (p=strtok(buf," ,\t\n");p;p=strtok(NULL," ,\t\n"))
	{
		for (i=0;i<16;i++)
			if (!strcasecmp(p,flnm[i]))
			{
				debug(5,"setting flag bit %d for \"%s\"",
					i,S(p));
				fl |= (1 << i);
			}
	}
	free(buf);
	debug(5,"set flags 0x%04x",fl);
	return fl;
}

char *compose_flags(flags,fkludge)
int flags;
char *fkludge;
{
	int i;
	char *buf=NULL,*p;

	debug(5,"composing flag string from binary 0x%04x and string \"%s\"",
		flags,S(fkludge));
	if (fkludge)
	{
		if (!isspace(fkludge[0])) buf=xstrcpy(" ");
		buf=xstrcat(buf,fkludge);
		p=buf+strlen(buf)-1;
		while (isspace(*p)) *p--='\0';
	}

	for (i=0;i<16;i++)
	if ((flags & (1 << i)) && (!flag_on(flnm[i],buf)))
	{
		buf=xstrcat(buf," ");
		buf=xstrcat(buf,flnm[i]);
		debug(5,"adding \"%s\"",flnm[i]);
	}
	debug(5,"resulting string is \"%s\"",buf);
	return buf;
}

char *strip_flags(flags)
char *flags;
{
	char *p,*q=NULL,*tok;
	int canonic,i;

	debug(5,"stripping official flags from \"%s\"",S(flags));
	if (flags == NULL) return NULL;
	p=xstrcpy(flags);
	for (tok=strtok(flags,", \t\n");tok;tok=strtok(NULL,", \t\n"))
	{
		canonic=0;
		for (i=0;i<16;i++)
			if (strcasecmp(tok,flnm[i]) == 0)
				canonic=1;
		if (!canonic)
		{
			q=xstrcat(q," ");
			q=xstrcat(q,tok);
		}
	}
	free(p);
	debug(5,"stripped string is \"%s\"",q);
	return q;
}

int flag_on(flag,flags)
char *flag,*flags;
{
	char *p,*tok;
	int up=0;

	debug(5,"checking flag \"%s\" in string \"%s\"",S(flag),S(flags));
	if (flags == NULL) return 0;
	p=xstrcpy(flags);
	for (tok=strtok(p,", \t\n");tok;tok=strtok(NULL,", \t\n"))
	{
		if (strcasecmp(flag,tok) == 0) up=1;
	}
	free(p);
	debug(5,"flag%s present",up?"":" not");
	return up;
}
