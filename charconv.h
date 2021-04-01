#ifndef IFMAIL_CHARCONV_H
#define IFMAIL_CHARCONV_H

/* some special chars values */
#define NUL         0
#define NL          10
#define FF          12
#define CR          13
#define ESC         27
#define TRUE        1
#define FALSE       0
#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

/* ************ general functions ************* */
char *hdrconv(char *, int, int);
char *hdrnconv(char *, int, int, int);
char *strnkconv(const char *src, int incode, int outcode, int maxlen);
char *strkconv(const char *src, int incode, int outcode);
void kconv(char *src, char **dest, int incode, int outcode);

/* ************ 8 bit charsets **************** */
void noconv(char *in, char **out);
void eight2eight(char *in, char **out, char *filemap);
void utf7_to_eight(char *in, char **out,int *outcode);
void utf8_to_eight(char *in, char **out,int *outcode);

/* maptabs names */
#define CP424__CP862		"cp424__cp862"
#define CP424__ISO_8859_8	"cp424__iso-8859-8"
#define CP437__ISO_8859_1	"cp437__iso-8859-1"
#define CP437__MACINTOSH	"cp437__mac"
#define CP850__ISO_8859_1	"cp437__iso-8859-1"
#define CP850__MACINTOSH	"cp437__mac"
#define CP852__FIDOMAZOVIA	"cp852__fidomazovia"
#define CP852__ISO_8859_2	"cp852__iso-8859-2"
#define CP862__CP424		"cp862__cp424"
#define CP862__ISO_8859_8	"cp862__iso-8859-8"
#define CP866__ISO_8859_5	"mik__iso-8859-5"
#define CP866__KOI8		"cp866__koi8"
#define CP895__CP437            "cp895__cp437"
#define CP895__ISO_8859_2       "cp895__iso-8859-2"
#define FIDOMAZOVIA__CP852	"fidomazovia__cp852"
#define FIDOMAZOVIA__ISO_8859_2	"fidomazovia__iso-8859-2"
#define ISO_11__ISO_8859_1	"iso-11__iso-8859-1"
#define ISO_4__ISO_8859_1	"iso-4__iso-8859-1"
#define ISO_60__ISO_8859_1	"iso-60__iso-8859-1"
#define ISO_8859_1__CP437	"iso-8859-1__cp437"
#define ISO_8859_1__MACINTOSH	"iso-8859-1__mac"
#define ISO_8859_1__CP850	"iso-8859-1__cp437"
#define ISO_8859_2__CP852	"iso-8859-2__cp852"
#define ISO_8859_2__CP895       "iso-8859-2__cp895"
#define ISO_8859_2__FIDOMAZOVIA	"iso-8859-2__fidomazovia"
#define ISO_8859_5__CP866	"iso-8859-5__mik"
#define ISO_8859_5__KOI8	"iso-8859-5__koi8"
#define ISO_8859_5__MIK_CYR	"iso-8859-5__mik"
#define ISO_8859_8__CP424	"iso-8859-8__cp424"
#define ISO_8859_8__CP862	"iso-8859-8__cp862"
#define KOI8__CP866		"koi8__cp866"
#define KOI8__ISO_8859_5	"koi8__iso-8859-5"
#define KOI8__MIK_CYR		"koi8__mik"
#define MACINTOSH__CP437	"mac__cp437"
#define MACINTOSH__CP850	"mac__cp437"
#define MACINTOSH__ISO_8859_1	"mac__iso-8859-1"
#define MIK_CYR__ISO_8859_5	"mik__iso-8859-5"
#define MIK_CYR__KOI8		"mik__koi8"

/* ************ 16 bits charsets ************* */
/* japanese charsets */
void shift2seven(char *in,char **out,int incode,char ki[],char ko[]);
void shift2euc(char *in,char **out,int incode,int tofullsize);
void euc2seven(char *in,char **out,int incode,char ki[],char ko[]);
void euc2euc(char *in,char **out,int incode,int tofullsize);
void shift2shift(char *in,char **out,int incode,int tofullsize);
void euc2shift(char *in,char **out,int incode,int tofullsize);
void seven2shift(char *in,char **out);
void seven2euc(char *in,char **out);
void seven2seven(char *in,char **out,char ki[],char ko[]);

/* Chinese charsets */
void gb2hz(char *in,char **out);
void hz2gb(char *in,char **out);
void zw2hz(char *in,char **out);
void zw2gb(char *in,char **out);

/* ??? */
int toup(int data);
int SkipESCSeq(FILE *in,int temp,int *intwobyte);
int getkcode(int code,char ki[],char ko[]);
int iso2022_detectcode(char *in,int);

#endif /* IFMAIL_CHARCONV_H */
