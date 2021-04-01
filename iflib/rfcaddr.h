typedef struct _parsedaddr {
	char *target;
	char *remainder;
	char *comment;
} parsedaddr;

extern int addrerror;
#define ADDR_NESTED 1
#define ADDR_MULTIPLE 2
#define ADDR_UNMATCHED 4
#define ADDR_BADTOKEN 8
#define ADDR_BADSTRUCT 16
#define ADDR_ERRMAX 5

extern parsedaddr parserfcaddr(char *);
extern void tidyrfcaddr(parsedaddr);
extern char *addrerrstr(int);
