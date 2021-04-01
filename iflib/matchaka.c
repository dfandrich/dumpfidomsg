#include <stdlib.h>
#include <string.h>
#include "ftn.h"
#include "lutil.h"
#include "config.h"

int metric(a1,a2)
faddr *a1,*a2;
{
	if ((a1->domain != NULL) &&
	    (a2->domain != NULL) &&
	    (strcasecmp(a1->domain,a2->domain) != 0))
		return METRIC_DOMAIN;
	if ((a1->zone != 0) && (a2->zone != 0) &&
	    (a1->zone != a2->zone))  return METRIC_ZONE;
	if (a1->net   != a2->net)   return METRIC_NET;
	if (a1->node  != a2->node)  return METRIC_NODE;
	if (a1->point != a2->point) return METRIC_POINT;
	return METRIC_EQUAL;
}

faddr *bestaka_s(addr)
faddr *addr;
{
	fa_list *tmp;
	faddr *best;
	int minmetric,wt;

	debug(6,"bestaka_s for %s",ascfnode(addr,0x1f));
	best=whoami->addr;
	if (addr == NULL) return best;
	minmetric=metric(addr,best);
	for (tmp=whoami;tmp;tmp=tmp->next)
	{
		wt=metric(addr,tmp->addr);
		debug(6,"addr %s metric %d",ascfnode(tmp->addr,0x1f),wt);
		if ((wt < minmetric) &&
		    ((best->point != 0) || (minmetric > METRIC_NODE)))
		/* In the same network, use node address even when routing
		   to the node where we have a point address */
		{
			debug(6,"setting best");
			minmetric=wt;
			best=tmp->addr;
		}
	}
	debug(6,"bestaka_s is  %s",ascfnode(best,0x1f));
	return best;
}

int is_local(addr)
faddr *addr;
{
	fa_list *tmp;

	for (tmp=whoami;tmp;tmp=tmp->next)
	{
		if (metric(tmp->addr,addr) == METRIC_EQUAL)
			return 1; /* yes */
	}
	return 0; /* no */
}
