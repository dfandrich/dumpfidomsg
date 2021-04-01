#ifndef IFMAIL_CHARSET_H
#define IFMAIL_CHARSET_H
#include "rfcmsg.h"

/* recognized charsets */
#define CHRS_AUTODETECT		-1
#define CHRS_NOTSET		 0
#define CHRS_ASCII		 1 /* us-ascii */
#define CHRS_BIG5		 2 /* Chinese Big5 charset */
#define CHRS_CP424		 3 /* hebrew EBCDIC */
#define CHRS_CP437		 4 /* Latin-1 MS codage (cp437) */
#define CHRS_CP850		 5 /* Latin-1 MS codage (cp850) */
#define CHRS_CP852		 6 /* Polish MS-DOS codage */
#define CHRS_CP862		 7 /* Hebrew PC */
#define CHRS_CP866		 8 /* Cyrillic Alt-PC (cp866) */
#define CHRS_CP895		 9 /* Kamenicky (DOS charset in CZ & SK) */
#define CHRS_EUC_JP		10 /* Japanese EUC */
#define CHRS_EUC_KR		11 /* Korean EUC */
#define CHRS_FIDOMAZOVIA	12 /* Polish "FIDOMAZOVIA" charset */
#define CHRS_GB			13 /* Chinese GB 2312 8 bits */
#define CHRS_HZ			14 /* Chinese HZ coding */
#define CHRS_ISO_2022_CN	15 /* Chinese GB 2312 7 bits */
#define CHRS_ISO_2022_JP	16 /* Japanese iso-2022-jp */
#define CHRS_ISO_2022_KR        17 /* Korean iso-2022-kr */
#define CHRS_ISO_2022_TW	18 /* Taiwanese iso-2022-tw */
#define CHRS_ISO_8859_1		19 /* Latin-1, Western Europe, America */ 
#define CHRS_ISO_8859_1_QP	20
#define CHRS_ISO_8859_2		21 /* Latin-2, Eastern Europe */
#define CHRS_ISO_8859_3		22 /* Latin-3, Balkanics languages */
#define CHRS_ISO_8859_4		23 /* Latin-4, Scandinavian, Baltic */
#define CHRS_ISO_8859_5		24 /* Cyrillic (iso-8859-5) */
#define CHRS_ISO_8859_6		25 /* Arabic (iso-8859-6) */
#define CHRS_ISO_8859_7		26 /* Greek (iso-8859-7) */
#define CHRS_ISO_8859_8		27 /* Hebrew (iso-8859-8) */
#define CHRS_ISO_8859_9		28 /* Latin-5, Turkish */
#define CHRS_ISO_8859_10	29 /* Latin-6, Lappish/Nordic/Eskimo */
#define CHRS_ISO_8859_11	30 /* Thai (iso-8859-11, aka TIS620) */
#define CHRS_ISO_8859_15	31 /* Latin-0 (Latin-1 + a few letters) */
#define CHRS_KOI8_R		32 /* Cyrillic Koi8 (Russian) */
#define CHRS_KOI8_U		33 /* Cyrillic Koi8 (Ukranian) */
#define CHRS_MACINTOSH		34 /* Macintosh */
#define CHRS_MIK_CYR		35 /* Bulgarian "Mik" cyrillic charset */
#define CHRS_NEC		36 /* Japanese NEC-JIS charset */
#define CHRS_SJIS		37 /* Japanese Shift-JIS (MS codage) */
#define CHRS_UTF_7		38 /* Unicode in UTF-7 encoding */
#define CHRS_UTF_8		39 /* Unicode in UTF-8 encoding */
#define CHRS_VISCII_10		40 /* VISCII 1.0 */
#define CHRS_VISCII_11		41 /* VISCII 1.1 */
#define CHRS_ZW			42 /* Chinese Zw encoding */

#define CHRS_ISO_11             91
#define CHRS_ISO_4              92
#define CHRS_ISO_60             93

/* languages (used for LANG_DEFAULT definition) */
#define LANG_WEST		1 /* West-European languages */	
#define LANG_EAST		2 /* East-Eurpean languages */
#define LANG_JAPAN		3 /* japanese */
#define LANG_KOREA		4 /* korean */
#define LANG_CHINA		5 /* chinese */
#define LANG_CYRILLIC		6 /* Cyrillic based languages */

/* Define these according to the values used in your country */
#define CHRS_DEFAULT_FTN	CHRS_CP437
#define CHRS_DEFAULT_RFC	CHRS_ISO_8859_1
#define LANG_DEFAULT		LANG_WEST

#if (LANG_DEFAULT==LANG_JAPAN || LANG_DEFAULT==LANG_KOREA || LANG_DEFAULT==LANG_CHINA)
#define LANG_BITS	16
#else
#define LANG_BITS	8
#endif

/* used to recognize pgpsigned messages */
#define PGP_SIGNED_BEGIN	"-----BEGIN PGP SIGNED MESSAGE-----"
#define PGP_SIG_BEGIN		"-----BEGIN PGP SIGNATURE-----"
#define PGP_SIG_END		"-----END PGP SIGNATURE-----"

/* charset reading functions */
int getoutcode(int);
int getincode(int);
char *getcharset(int);
char *getchrs(int);
int getcode(char *);
int readchrs(char *);
int readcharset(char *);
void writechrs(int,FILE *,int);
void writecharset(int,FILE *,rfcmsg *,rfcmsg *);

#endif /* IFMAIL_CHARSET_H */
