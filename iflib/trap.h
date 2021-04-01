#ifndef TRAP_H
#define TRAP_H
#ifdef NEED_TRAP
void catch(char*);
#else
#define catch(x)
#endif
#endif
