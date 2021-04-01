#ifndef IFMAIL_CHARCONV_HZ_H
#define IFMAIL_CHARCONV_HZ_H

#define DOS
#define SPACE           0xA1A1          /* GB "space" symbol */
#define BOX             0xA1F5          /* GB "blank box" symbol */
#define isGB1(c)        ((c)>=0x21 && (c)<=0x77)        /* GB 1st byte */
#define isGB1U(c)       ((c)>=0x78 && (c)<=0x7D)        /* GB 1st byte unused*/
#define isGB2(c)        ((c)>=0x21 && (c)<=0x7E)        /* GB 2nd byte */
#define HI(code)        (((code) & 0xFF00)>>8)
#define LO(code)        ((code) & 0x00FF)
#define DB(hi,lo)       ((((hi)&0xFF) << 8) | ((lo)&0xFF))
#define CLEAN7(c)       ((c) & 0x7F)                    /* strip MSB */
#define notAscii(c)     ((c)&0x80)

int LF2CR = FALSE;      /* flag for converting ASCII <LF> to <CR> */
int CR2LF=FALSE;        /* flag for converting ASCII <CR> to <LF> */
int pass8 = FALSE;      /* flat for parsing all 8 bits of a character */
int termStyle = FALSE;  /* flag for ignoring line-continuation markers */
int MAXLEN = 77;        /* default maximum line length in the above style */
int MINLEN = 7;         /* minimum line length in the above style */
int errorCount = 0;     /* number of parsing errors detected */

/* internal functions */
void EOFerror(void);
void ESCerror(int c);
void GBerror(int c1,int c2);
void GBerror1(int c);
void GBtoSGB(int hi, int lo, int *hi1, int *lo1);
void mac2gb(int hi, int lo, int *hi1, int *lo1);
void dos2gb(int hi, int lo, int *hi1, int *lo1);

#endif /* IFMAIL_CHARCONV_HZ_H */
