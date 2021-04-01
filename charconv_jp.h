#ifndef IFMAIL_CHARCONV_JP_H
#define IFMAIL_CHARCONV_JP_H

#define SJIS1(A)    ((A >= 129 && A <= 159) || (A >= 224 && A <= 239))
#define SJIS2(A)    (A >= 64 && A <= 252)
#define HANKATA(A)  (A >= 161 && A <= 223)
#define ISEUC(A)    (A >= 161 && A <= 254)
#define ISMARU(A)   (A >= 202 && A <= 206)
#define ISNIGORI(A) ((A >= 182 && A <= 196) || (A >= 202 && A <= 206))

void han2zen(FILE *in,int *p1,int *p2,int incode);
void sjis2jis(int *p1,int *p2);
void jis2sjis(int *p1,int *p2);

#endif /* IFMAIL_CHARCONV_JP_H */
