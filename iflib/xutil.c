#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "xutil.h"
#include "lutil.h"

char *xmalloc(size)
size_t size;
{
	char *tmp;

	tmp=malloc(size);
	if (!tmp) abort();
	
	return tmp;
}

char *xstrcpy(src)
char *src;
{
	char	*tmp;

	if (src == NULL) return(NULL);
	tmp=xmalloc(strlen(src)+1);
	strcpy(tmp,src);
	return tmp;
}

char *xstrcat(src,add)
char *src,*add;
{
	char *tmp;
	size_t size=0;

	if ((add == NULL) || (strlen(add) == 0)) return src;
	if (src) size=strlen(src);
	size+=strlen(add);
	tmp=xmalloc(size+1);
	*tmp='\0';
	if (src) 
	{
		strcpy(tmp,src);
		free(src);
	}
	strcat(tmp,add);
	return tmp;
}
