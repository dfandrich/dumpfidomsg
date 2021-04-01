#include <stdio.h>
#include <time.h>

static char *wdays[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
static char *months[]={"Jan","Feb","Mar","Apr","May","Jun",
		"Jul","Aug","Sep","Oct","Nov","Dec"};
char *gmtoffset(time_t);


char *rfcdate(now)
time_t now;
{
	static char buf[40];
	struct tm ptm;

	if (!now) time(&now);
	ptm=*localtime(&now);

	sprintf(buf,"%s, %02d %s %02d %02d:%02d:%02d %s",
		wdays[ptm.tm_wday],ptm.tm_mday,months[ptm.tm_mon],
		ptm.tm_year%100,ptm.tm_hour,ptm.tm_min,ptm.tm_sec,
		gmtoffset(now));
	return(buf);
}

char *gmtoffset(now)
time_t now;
{
	static char buf[6]="+0000";
	struct tm ptm;
#if defined(DONT_HAVE_TM_GMTOFF)
	struct tm gtm;
#endif
	char sign;
	int hr,min;
	long offset;

	if (!now) time(&now);
	ptm=*localtime(&now);

#if defined(DONT_HAVE_TM_GMTOFF)
	/* To get the timezone, compare localtime with GMT. */
	gtm=*gmtime(&now);

	/* Assume we are never more than 24 hours away. */
	offset = gtm.tm_yday - ptm.tm_yday;
	if (offset > 1)
		offset = -24;
	else if (offset < -1)
		offset = 24;
	else
		offset *= 24;

	/* Scale in the hours and minutes; ignore seconds. */
	offset += gtm.tm_hour - ptm.tm_hour;
	offset *= 60;
	offset += gtm.tm_min - ptm.tm_min;
#else
	offset=-ptm.tm_gmtoff/60L;
#endif

	if (offset <= 0)
	{
		sign='+';
		offset=-offset;
	}
	else sign='-';
	hr=offset/60L;
	min=offset%60L;

	sprintf(buf,"%c%02d%02d",sign,hr,min);
	return(buf);
}
