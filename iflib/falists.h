#ifndef _FALIST_H
#define _FALIST_H

#include "ftn.h"

extern void fill_list(fa_list **,char *,fa_list **);
extern void fill_rlist(fa_list **,char *);
extern void fill_path(fa_list **,char *);
extern void sort_list(fa_list **);
extern void uniq_list(fa_list **);
extern int in_list(faddr *,fa_list **);

#endif
