#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef TESTING
#include <stdio.h>
#endif
#include "xutil.h"
#include "rfcaddr.h"

int addrerror;

static char *errname[] = {
	"nested <>",
	"multiple <>",
	"unmatched <>""()",
	"badtoken",
	"badstructure",
};

char *addrerrstr(err)
int err;
{
	int i;
	static char buf[128];

	buf[0]='\0';
	for (i=0;i<ADDR_ERRMAX;i++)
	if (err & (1 << i))
	{
		if (buf[0]) strcat(buf,",");
		strcat(buf,errname[i]);
	}
	if (buf[0] == '\0') strcpy(buf,"none");
	return buf;
}

void tidyrfcaddr(addr)
parsedaddr addr;
{
	if (addr.target) free(addr.target);
	if (addr.remainder) free(addr.remainder);
	if (addr.comment) free(addr.comment);
}

parsedaddr parserfcaddr(s)
char *s;
{
	parsedaddr result;
	char *inbrackets,*outbrackets,*cleanbuf=NULL,*combuf=NULL;
	char *t,*r,*c,*p,*q,**x;
	int quotes,brackets,escaped,anglecomplete;
	char *firstat,*lastat,*percent,*colon,*comma,*exclam;

	result.target=NULL;
	result.remainder=NULL;
	result.comment=NULL;
	addrerror=0;

	if ((s == NULL) || (*s == '\0')) return result;

	/* First check if there is an "angled" portion */

	inbrackets=xmalloc(strlen(s)+1);
	outbrackets=xmalloc(strlen(s)+1);
	*inbrackets=*outbrackets='\0';
	brackets=quotes=escaped=anglecomplete=0;
	for (p=s,q=inbrackets,r=outbrackets,x=&r;
		*p;
		p++)
	{
		if (escaped) escaped=0;
		else /* process all special chars */
		switch (*p)
		{
		case '\\':	escaped=1; break;
		case '\"':	quotes=!quotes; break;
		case '<':	if (quotes) break;
				if (brackets) addrerror |= ADDR_NESTED;
				if (anglecomplete) addrerror |= ADDR_MULTIPLE;
				brackets++;
				x=&q;
				break;
		case '>':	if (quotes) break;
				if (brackets) brackets--;
				else addrerror |= ADDR_UNMATCHED;
				if (!brackets) anglecomplete=1;
				break;
		}
		*((*x)++)=*p;
		if (!brackets) x=&r;
	}
	*q='\0';
	*r='\0';
	if (brackets || quotes) addrerror |= ADDR_UNMATCHED;

#ifdef TESTING
	printf(" inbrackets: \"%s\"\n",inbrackets);
	printf("outbrackets: \"%s\"\n",outbrackets);
	printf("addrerror: 0x%04x\n",addrerror);
#endif

	if (addrerror) goto leave1;

	cleanbuf=xmalloc(strlen(s)+1);
	*cleanbuf='\0';
	combuf=xmalloc(strlen(s)+1);
	*combuf='\0';

	if (*inbrackets) /* there actually is an angled portion */
	{
		strcpy(combuf,outbrackets);
		c=combuf+strlen(combuf);
		p=inbrackets+1;
		*(p+strlen(p)-1)='\0';
	}
	else
	{
		c=combuf;
		p=outbrackets;
	}

#ifdef TESTING
	printf("    now parsing: \"%s\"\n",p);
	printf("current comment: \"%s\"\n",result.comment);
#endif

	/* OK, now we have result.comment filled with wat was outside
	   angle brackets, c pointing past the end of it,
	   p pointing to what is supposed to be address, with angle
	   brackets already removed */

	quotes=brackets=escaped=0;
	for (r=cleanbuf,x=&r;
		*p;
		p++)
	{
		if (escaped)
		{
			escaped=0;
			*((*x)++)=*p;
		}
		else /* process all special chars */
		if (isspace(*p))
		{
			if ((quotes) || (brackets)) *((*x)++)=*p;
		}
		else
		switch (*p)
		{
		case '\\':	escaped=1;
				/* pass backslash itself only inside quotes
				   and comments, or for the special cases
				   \" and \\  otherwise eat it away */
		if ((quotes) || (brackets)) *((*x)++)=*p;
		else if ((*(p+1)=='"') || (*(p+1)=='\\')) *((*x)++)=*p;
				break;
		case '\"':	quotes=!quotes;
				*((*x)++)=*p;
				break;
		case '(':
				brackets++;
				x=&c;
				break;
		case ')':
				if (brackets) brackets--;
				else addrerror |= ADDR_UNMATCHED;
				if (!brackets) x=&r;
				break;
		default:
				*((*x)++)=*p;
				break;
		}
	}
	*r='\0';
	*c='\0';
	if (brackets || quotes) addrerror |= ADDR_UNMATCHED;

#ifdef TESTING
	printf("     now parsing: \"%s\"\n",inbrackets);
	printf("complete comment: \"%s\"\n",result.comment);
	printf("       addrerror: 0x%04x\n",addrerror);
#endif

	if (addrerror) goto leave2;

	/* OK, now we have inangles buffer filled with the 'clean' address,
	   all comments removed, and result.comment is ready filled */

	/* seach for special chars that are outside quotes */

	firstat=lastat=percent=colon=comma=exclam=NULL;
	quotes=0; escaped=0;
	for (p=cleanbuf;
		*p;
		p++)
	if (*p == '\\') p++; 
	else if (*p == '\"') quotes = !quotes;
	else if (!quotes)
	switch (*p)
	{
	case '@':
			if (!firstat) firstat=p;
			lastat=p;
		break;
	case '%':
			percent=p;
		break;
	case ':':
			colon=p;
		break;
	case ',':
			comma=p;
		break;
	case '!':
			if (!exclam) exclam=p;
		break;
	}

	if ((firstat == cleanbuf) && colon)
	{
#ifdef TESTING
		printf("@aaa,@bbb:xxx@yyy construct\n");
#endif
		if (comma && (comma < colon))
		{
			*comma='\0';
			r=comma+1;
		}
		else
		{
			*colon='\0';
			r=colon+1;
		}
		t=firstat+1;
	}
	else if (lastat)
	{
#ifdef TESTING
		printf("anything@somewhere construct\n");
#endif
		*lastat='\0';
		r=cleanbuf;
		t=lastat+1;
	}
	else if (exclam)
	{
#ifdef TESTING
		printf("domain!something construct (without @'s)\n");
#endif
		*exclam='\0';
		r=exclam+1;
		t=cleanbuf;
	}
	else if (percent)
	{
#ifdef TESTING
		printf("anything%%somewhere construct (without !'s and @'s)\n");
#endif
		*percent='\0';
		r=cleanbuf;
		t=percent+1;
	}
	else
	{
#ifdef TESTING
		printf("remainder only present\n");
#endif
		/* unquote it if necessary */
		if ((*cleanbuf == '\"') &&
		    (*(p=(cleanbuf+strlen(cleanbuf)-1)) == '\"'))
		{
			*p='\0';
			r=cleanbuf+1;
		}
		else r=cleanbuf;
		t=NULL;
	}

	if (t && (*t != '\0')) result.target=xstrcpy(t);
	if (r && (*r != '\0')) result.remainder=xstrcpy(r);
	if (*combuf != '\0') result.comment=xstrcpy(combuf);

leave1: /* this is also normal exit */
	free(cleanbuf);
	free(combuf);
	free(inbrackets);
	free(outbrackets);
	return result;

leave2: /* if error found on second stage, free */
	free(cleanbuf);
	free(combuf);
	return result;
}

#ifdef TESTING

int main()
{
	parsedaddr tmp;
	char buffer[128];

	while(gets(buffer))
	{
		printf("  parsing: \"%s\"\n",buffer);
		tmp=parserfcaddr(buffer);
		printf("   target: \"%s\"\n",tmp.target);
		printf("remainder: \"%s\"\n",tmp.remainder);
		printf("  comment: \"%s\"\n",tmp.comment);
	}
	tidyrfcaddr(tmp);
	return 0;
}

#endif
