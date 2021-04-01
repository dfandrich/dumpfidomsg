#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "xutil.h"
#include "lutil.h"
#include "rfcmsg.h"

char *hdr(key,msg)
char *key;
rfcmsg *msg;
{
	for (;msg;msg=msg->next)
		if (!strcasecmp(key,msg->key)) return(msg->val);
	return(NULL);
}
