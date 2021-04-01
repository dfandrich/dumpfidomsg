/* ### Modified on Thu 4 Jan 1996 by P.Saratxaga ###
 * - added JE functions, for compatibility with JE-version config files
 */

#ifndef AREA_LIST_TYPE
#define AREA_LIST_TYPE
typedef struct _area_list {
	struct _area_list *next;
	char *name;
} area_list;
#endif

extern int group_count;
/* if int=1 return expanded areas from default, if int=0 does not */ 
extern area_list *areas(char *,int);
extern void ngdist(char*,char**,char**,char**,int*);
#ifdef JE
extern void areacharset(char*,int*,int*);
#endif
extern void tidy_arealist(area_list *);
#ifdef GATEBAU_MSGID
int areagatebau(char *);
#endif
