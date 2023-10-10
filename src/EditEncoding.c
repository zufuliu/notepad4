// Edit Encoding

#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include "SciCall.h"
#include "VectorISA.h"
#include "Helpers.h"
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
#include "resource.h"

extern bool bSkipUnicodeDetection;
extern int iDefaultEncoding;
extern int iDefaultCodePage;
extern bool bLoadANSIasUTF8;
extern bool bLoadASCIIasUTF8;
extern bool bLoadNFOasOEM;
extern int iDefaultCharSet;
extern int iSrcEncoding;
extern int iWeakSrcEncoding;
extern int iCurrentEncoding;

int g_DOSEncoding;

// Supported Encodings
static WCHAR wchANSI[16];
static WCHAR wchOEM [16];
static LPWSTR g_AllEncodingLabel = NULL;

typedef struct NP2EncodingGroup {
	bool bMapped;		// map code page to index in mEncoding array.
	const UINT idsName;	// resource id for group name
	// presorted encoding list, code pages before been mapped to index.
	int encodings[10];	// use fixed array to simplify code
} NP2EncodingGroup;

// https://docs.microsoft.com/en-us/windows/desktop/Intl/code-page-identifiers
// https://www.iana.org/assignments/character-sets/character-sets.xhtml
// https://en.wikipedia.org/wiki/Windows_code_page
// https://en.wikipedia.org/wiki/ISO/IEC_8859
// https://dev.mysql.com/doc/refman/8.0/en/charset-charsets.html
// https://docs.python.org/3/library/codecs.html#standard-encodings

// https://encoding.spec.whatwg.org/#names-and-labels
static inline UINT GetEncodingAlias(UINT codePage) {
	// prefer Windows ANSI code page to corresponding ISO-8859 encoding
	// as the former contains more graphic characters in [0x80, 0xff].
	switch (codePage) {
	case 28591:	return 1252; // ISO 8859-1
	case 28599: return 1254; // ISO 8859-9
	default: return codePage;
	}
}

// encoding list, check with tools/Misc.py after make changes
NP2ENCODING mEncoding[] = {
	{ NCP_DEFAULT | NCP_RECODE, CP_ACP, "ANSI,ansi,ascii,", IDS_ENCODING_ANSI, NULL },
	{ NCP_8BIT | NCP_RECODE, CP_OEMCP, "OEM,oem,", IDS_ENCODING_OEM, NULL },
	{ NCP_UNICODE | NCP_UNICODE_BOM, 0, "", IDS_ENCODING_UTF16LE_BOM, NULL },
	{ NCP_UNICODE | NCP_UNICODE_REVERSE | NCP_UNICODE_BOM, 0, "", IDS_ENCODING_UTF16BE_BOM, NULL },
	{ NCP_UNICODE | NCP_RECODE, 0, "UTF-16,utf16,unicode,ucs2,utf16le,", IDS_ENCODING_UTF16LE, NULL },
	{ NCP_UNICODE | NCP_UNICODE_REVERSE | NCP_RECODE, 0, "UTF-16BE,utf16be,unicodebe,", IDS_ENCODING_UTF16BE, NULL },
	{ NCP_UTF8 | NCP_RECODE, CP_UTF8, "UTF-8,utf8,utf8mb4,cp65001,", IDS_ENCODING_UTF8, NULL },
	{ NCP_UTF8 | NCP_UTF8_SIGN, CP_UTF8, "UTF-8,utf8,utf8mb4,cp65001,", IDS_ENCODING_UTF8BOM, NULL },
	{ NCP_7BIT | NCP_RECODE, CP_UTF7, "UTF-7,utf7,", IDS_ENCODING_UTF7, NULL },
	// Latin-1
	{ NCP_8BIT | NCP_RECODE, 28591, "ISO-8859-1,iso88591,cp819,latin1,ibm819,isoir100,l1,latin,", IDS_ENCODING_ISO8859_1, NULL },// ISO 8859-1 Latin-1; Western European (ISO)
	{ NCP_8BIT | NCP_RECODE, 1252, "Windows-1252,windows1252,cp367,ibm367,us,xansi,cp1252,", IDS_ENCODING_WINDOWS1252, NULL },// ANSI Latin-1; Western European (Windows)
	// Windows ANSI
	{ NCP_8BIT | NCP_RECODE, 1250, "Windows-1250,windows1250,xcp1250,cp1250,", IDS_ENCODING_WINDOWS1250, NULL },// ANSI Central European; Central European (Windows)
	{ NCP_8BIT | NCP_RECODE, 1251, "Windows-1251,windows1251,xcp1251,cp1251,", IDS_ENCODING_WINDOWS1251, NULL },// ANSI Cyrillic; Cyrillic (Windows)
	{ NCP_8BIT | NCP_RECODE, 1253, "Windows-1253,windows1253,cp1253,", IDS_ENCODING_WINDOWS1253, NULL },// ANSI Greek; Greek (Windows)
	{ NCP_8BIT | NCP_RECODE, 1254, "Windows-1254,windows1254,cp1254,", IDS_ENCODING_WINDOWS1254, NULL },// ANSI Turkish; Turkish (Windows)
	{ NCP_8BIT | NCP_RECODE, 1255, "Windows-1255,windows1255,cp1255,", IDS_ENCODING_WINDOWS1255, NULL },// ANSI Hebrew; Hebrew (Windows)
	{ NCP_8BIT | NCP_RECODE, 1256, "Windows-1256,windows1256,cp1256,", IDS_ENCODING_WINDOWS1256, NULL },// ANSI Arabic; Arabic (Windows)
	{ NCP_8BIT | NCP_RECODE, 1257, "Windows-1257,windows1257,cp1257,", IDS_ENCODING_WINDOWS1257, NULL },// ANSI Baltic; Baltic (Windows)
	{ NCP_8BIT | NCP_RECODE, 1258, "Windows-1258,windows1258,cp1258,", IDS_ENCODING_WINDOWS1258, NULL },// ANSI/OEM Vietnamese; Vietnamese (Windows)
	{ NCP_8BIT | NCP_RECODE, 874, "Windows-874,windows874,dos874,cp874,iso885911,tis620,isoir166,thai,", IDS_ENCODING_TIS620, NULL },// ANSI/OEM Thai (ISO 8859-11); Thai (Windows)
	{ NCP_8BIT | NCP_RECODE, 932, "Shift-JIS,shiftjis,csshiftjis,cswindows31j,mskanji,xmscp932,xsjis,cp932,sjis,ms932,", IDS_ENCODING_SHIFT_JIS, NULL },// ANSI/OEM Japanese; Japanese (Shift-JIS)
	{ NCP_8BIT | NCP_RECODE, 936, "GBK,gbk,gb2312,chinese,cngb,csgb2312,csgb231280,gb231280,cp936,ms936,", IDS_ENCODING_GBK, NULL },// ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
	{ NCP_8BIT | NCP_RECODE, 949, "Windows-949,windows949,ksx1001,ksc56011987,csksc5601,isoir149,korean,ksc56011989,cp949,ms949,uhc,", IDS_ENCODING_UHC, NULL },// ANSI/OEM Korean (Unified Hangul Code)
	{ NCP_8BIT | NCP_RECODE, 950, "BIG5,big5,cnbig5,csbig5,xxbig5,cp950,ms950,", IDS_ENCODING_BIG5, NULL },// ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
	// ISO-8859
	{ NCP_8BIT | NCP_RECODE, 28592, "ISO-8859-2,iso88592,csisolatin2,isoir101,latin2,l2,", IDS_ENCODING_ISO8859_2, NULL },// Central European (ISO 8859-2); Central European (ISO)
	{ NCP_8BIT | NCP_RECODE, 28593, "ISO-8859-3,iso88593,latin3,isoir109,l3,", IDS_ENCODING_ISO8859_3, NULL },// Latin-3 (ISO 8859-3)
	{ NCP_8BIT | NCP_RECODE, 28594, "ISO-8859-4,iso88594,csisolatin4,isoir110,latin4,l4,", IDS_ENCODING_ISO8859_4, NULL },// Baltic (ISO 8859-4); North European
	{ NCP_8BIT | NCP_RECODE, 28595, "ISO-8859-5,iso88595,csisolatincyrillic,cyrillic,isoir144,", IDS_ENCODING_ISO8859_5, NULL },// Cyrillic (ISO 8859-5)
	{ NCP_8BIT | NCP_RECODE, 28596, "ISO-8859-6,iso88596,arabic,csisolatinarabic,ecma114,isoir127,", IDS_ENCODING_ISO8859_6, NULL },// Arabic (ISO 8859-6 Visual)
	{ NCP_8BIT | NCP_RECODE, 28597, "ISO-8859-7,iso88597,csisolatingreek,ecma118,elot928,greek,greek8,isoir126,", IDS_ENCODING_ISO8859_7, NULL },// Greek (ISO 8859-7)
	{ NCP_8BIT | NCP_RECODE, 28598, "ISO-8859-8,iso88598,csisolatinhebrew,hebrew,isoir138,visual,", IDS_ENCODING_ISO8859_8, NULL },// Hebrew (ISO 8859-8); Hebrew (ISO-Visual)
	{ NCP_8BIT | NCP_RECODE, 28599, "ISO-8859-9,iso88599,csisolatin5,latin5,isoir148,l5,", IDS_ENCODING_ISO8859_9, NULL },// Turkish (ISO 8859-9)
	{ NCP_8BIT | NCP_RECODE, 28600, "ISO-8859-10,iso885910,windows28600,latin6,l6,", IDS_ENCODING_ISO8859_10, NULL },// Nordic (ISO 8859-10)
	{ NCP_8BIT | NCP_RECODE, 28603, "ISO-8859-13,iso885913,latin7,l7,", IDS_ENCODING_ISO8859_13, NULL },// Estonian (ISO 8859-13)
	{ NCP_8BIT | NCP_RECODE, 28604, "ISO-8859-14,iso885914,windows28604,latin8,l8,", IDS_ENCODING_ISO8859_14, NULL },// Celtic (ISO 8859-14)
	{ NCP_8BIT | NCP_RECODE, 28605, "ISO-8859-15,iso885915,latin9,l9,", IDS_ENCODING_ISO8859_15, NULL },// Latin-9 (ISO 8859-15)
	{ NCP_8BIT | NCP_RECODE, 28606, "ISO-8859-16,iso885916,windows28606,latin10,l10,", IDS_ENCODING_ISO8859_16, NULL },// Latin-10 (ISO 8859-16)
	// Other
	{ NCP_8BIT | NCP_RECODE, 54936, "GB18030,gb18030,", IDS_ENCODING_GB18030, NULL },// Chinese GB18030
	{ NCP_8BIT | NCP_RECODE, 20866, "koi8-r,koi8r,cskoi8r,koi,koi8,", IDS_ENCODING_KOI8_R, NULL },// Russian (KOI8-R); Cyrillic (KOI8-R)
	{ NCP_8BIT | NCP_RECODE, 21866, "koi8-u,koi8u,koi8ru,", IDS_ENCODING_KOI8_U, NULL },// Ukrainian (KOI8-U); Cyrillic (KOI8-U)
	{ NCP_8BIT | NCP_RECODE, 20932, "euc-jp,eucjp,ujis,", IDS_ENCODING_EUC_JP, NULL },// Japanese (JIS X 0208-1990 & 0212-1990)
	{ NCP_8BIT | NCP_RECODE, 1361, "Johab,johab,cp1361,ms1361,", IDS_ENCODING_JOHAB, NULL },// Korean (Johab)
	{ NCP_8BIT | NCP_RECODE, 51949, "euc-kr,euckr,cseuckr,", IDS_ENCODING_EUC_KR, NULL },// Korean (EUC)
	{ NCP_8BIT | NCP_RECODE, 38596, "ISO-8859-6-I,iso88596i,", IDS_ENCODING_ISO8859_6I, NULL },// Arabic (ISO 8859-6-I Logical)
	{ NCP_8BIT | NCP_RECODE, 38598, "ISO-8859-8-I,iso88598i,logical,", IDS_ENCODING_ISO8859_8I, NULL },// Hebrew (ISO 8859-8); Hebrew (ISO-Logical)
	// OEM
	{ NCP_8BIT | NCP_RECODE, 437, "IBM437,ibm437,437,cp437,cspc8,codepage437,", IDS_ENCODING_DOS437, NULL },// OEM United States
	{ NCP_8BIT | NCP_RECODE, 850, "IBM850,ibm850,cp850,", IDS_ENCODING_DOS850, NULL },// OEM Multilingual Latin-1; Western European (DOS)
	{ NCP_8BIT | NCP_RECODE, 858, "IBM858,ibm858,ibm00858,cp858,", IDS_ENCODING_DOS858, NULL },// OEM Multilingual Latin-1 + Euro symbol
	{ NCP_8BIT | NCP_RECODE, 852, "IBM852,ibm852,cp852,", IDS_ENCODING_DOS852, NULL },// OEM Latin-2; Central European (DOS)
	{ NCP_8BIT | NCP_RECODE, 720, "DOS-720,dos720,cp720,", IDS_ENCODING_DOS720, NULL },// Arabic (Transparent ASMO); Arabic (DOS)
	{ NCP_8BIT | NCP_RECODE, 864, "IBM864,ibm864,cp864,", IDS_ENCODING_DOS864, NULL },// OEM Arabic; Arabic (864)
	{ NCP_8BIT | NCP_RECODE, 775, "IBM775,ibm775,cp775,", IDS_ENCODING_DOS775, NULL },// OEM Baltic; Baltic (DOS)
	{ NCP_8BIT | NCP_RECODE, 855, "IBM855,ibm855,cp855,", IDS_ENCODING_IBM855, NULL },// OEM Cyrillic (primarily Russian)
	{ NCP_8BIT | NCP_RECODE, 866, "CP866,cp866,ibm866,", IDS_ENCODING_DOS866, NULL },// OEM Russian; Cyrillic (DOS)
	{ NCP_8BIT | NCP_RECODE, 863, "IBM863,ibm863,cp863,", IDS_ENCODING_DOS863, NULL },// OEM French Canadian; French Canadian (DOS)
	{ NCP_8BIT | NCP_RECODE, 737, "IBM737,ibm737,cp737,", IDS_ENCODING_DOS737, NULL },// OEM Greek (formerly 437G); Greek (DOS)
	{ NCP_8BIT | NCP_RECODE, 869, "IBM869,ibm869,cp869,", IDS_ENCODING_DOS869, NULL },// OEM Modern Greek; Greek, Modern (DOS)
	{ NCP_8BIT | NCP_RECODE, 862, "DOS-862,dos862,cp862,", IDS_ENCODING_DOS862, NULL },// OEM Hebrew; Hebrew (DOS)
	{ NCP_8BIT | NCP_RECODE, 861, "IBM861,ibm861,cp861,", IDS_ENCODING_DOS861, NULL },// OEM Icelandic; Icelandic (DOS)
	{ NCP_8BIT | NCP_RECODE, 865, "IBM865,ibm865,cp865,", IDS_ENCODING_DOS865, NULL },// OEM Nordic; Nordic (DOS)
	{ NCP_8BIT | NCP_RECODE, 860, "IBM860,ibm860,cp860,", IDS_ENCODING_DOS860, NULL },// OEM Portuguese; Portuguese (DOS)
	{ NCP_8BIT | NCP_RECODE, 857, "IBM857,ibm857,cp857,", IDS_ENCODING_DOS857, NULL },// OEM Turkish; Turkish (DOS)
	// Mac
	{ NCP_8BIT | NCP_RECODE, 10000, "Macintosh,macintosh,macroman,", IDS_ENCODING_MAC_ROMAN, NULL },// MAC Roman; Western European (Mac)
	{ NCP_8BIT | NCP_RECODE, 10029, "x-mac-ce,xmacce,maclatin2,", IDS_ENCODING_MAC_LATIN2, NULL },// MAC Latin-2; Central European (Mac)
	{ NCP_8BIT | NCP_RECODE, 10004, "x-mac-arabic,xmacarabic,", IDS_ENCODING_MAC_ARABIC, NULL },// Arabic (Mac)
	{ NCP_8BIT | NCP_RECODE, 10008, "x-mac-chinesesimp,xmacchinesesimp,", IDS_ENCODING_MAC_GB2312, NULL },// MAC Simplified Chinese (GB 2312); Chinese Simplified (Mac)
	{ NCP_8BIT | NCP_RECODE, 10002, "x-mac-chinesetrad,xmacchinesetrad,", IDS_ENCODING_MAC_BIG5, NULL },// MAC Traditional Chinese (Big5); Chinese Traditional (Mac)
	{ NCP_8BIT | NCP_RECODE, 10082, "x-mac-croatian,xmaccroatian,", IDS_ENCODING_MAC_CROATIAN, NULL },// Croatian (Mac)
	{ NCP_8BIT | NCP_RECODE, 10007, "x-mac-cyrillic,xmaccyrillic,", IDS_ENCODING_MAC_CYRILLIC, NULL },// Cyrillic (Mac)
	{ NCP_8BIT | NCP_RECODE, 10006, "x-mac-greek,xmacgreek,", IDS_ENCODING_MAC_GREEK, NULL },// Greek (Mac)
	{ NCP_8BIT | NCP_RECODE, 10005, "x-mac-hebrew,xmachebrew,", IDS_ENCODING_MAC_HEBREW, NULL },// Hebrew (Mac)
	{ NCP_8BIT | NCP_RECODE, 10079, "x-mac-icelandic,xmacicelandic,", IDS_ENCODING_MAC_ICELANDIC, NULL },// Icelandic (Mac)
	{ NCP_8BIT | NCP_RECODE, 10001, "x-mac-japanese,xmacjapanese,", IDS_ENCODING_MAC_JAPANESE, NULL },// Japanese (Mac)
	{ NCP_8BIT | NCP_RECODE, 10003, "x-mac-korean,xmackorean,", IDS_ENCODING_MAC_KOREAN, NULL },// Korean (Mac)
	{ NCP_8BIT | NCP_RECODE, 10021, "x-mac-thai,xmacthai,", IDS_ENCODING_MAC_THAI, NULL },// Thai (Mac)
	{ NCP_8BIT | NCP_RECODE, 10081, "x-mac-turkish,xmacturkish,", IDS_ENCODING_MAC_TURKISH, NULL },// Turkish (Mac)
	{ NCP_8BIT | NCP_RECODE, 10010, "x-mac-romanian,xmacromanian,", IDS_ENCODING_MAC_ROMANIAN, NULL },// Romanian (Mac)
	{ NCP_8BIT | NCP_RECODE, 10017, "x-mac-ukrainian,xmacukrainian,", IDS_ENCODING_MAC_UKRAINIAN, NULL },// Ukrainian (Mac)
	//{ NCP_8BIT | NCP_RECODE, 708, "ASMO-708,asmo708,", 0, NULL },// Arabic (ASMO 708); ISO 8859-6
	//{ NCP_8BIT | NCP_RECODE, 709, "ASMO-709,asmo709,", 0, NULL },// Arabic (ASMO-449+, BCON V4)
	//{ NCP_8BIT | NCP_RECODE, 710, "ASMO-710,asmo710,", 0, NULL },// Arabic - Transparent Arabic
	//{ NCP_8BIT | NCP_RECODE, 29001, "x-Europa,xeuropa,", 0, NULL },// Europa 3
	{ NCP_8BIT | NCP_RECODE, 37, "ebcdic-cp-us,ebcdiccpus,ebcdiccpca,ebcdiccpwt,ebcdiccpnl,ibm037,cp037,", IDS_ENCODING_IBM037, NULL },// IBM EBCDIC US-Canada
	{ NCP_8BIT | NCP_RECODE, 1140, "x-ebcdic-cp-us-euro,xebcdiccpuseuro,ibm1140,", IDS_ENCODING_IBM1140, NULL },// IBM EBCDIC US-Canada (037 + Euro symbol); IBM EBCDIC (US-Canada-Euro)
	{ NCP_8BIT | NCP_RECODE, 500, "x-ebcdic-international,xebcdicinternational,ibm500,cp500,", IDS_ENCODING_IBM500, NULL },// IBM EBCDIC International
	{ NCP_8BIT | NCP_RECODE, 870, "CP870,cp870,ebcdiccproece,ebcdiccpyu,csibm870,ibm870,", IDS_ENCODING_IBM870, NULL },// IBM EBCDIC Multilingual/ROECE (Latin-2); IBM EBCDIC Multilingual Latin-2
	{ NCP_8BIT | NCP_RECODE, 875, "x-EBCDIC-GreekModern,xebcdicgreekmodern,cp875,", IDS_ENCODING_CP875, NULL },// IBM EBCDIC Greek Modern
	{ NCP_8BIT | NCP_RECODE, 1026, "CP1026,cp1026,csibm1026,ibm1026,", IDS_ENCODING_IBM1026, NULL },// IBM EBCDIC Turkish (Latin-5)
	//{ NCP_8BIT | NCP_RECODE, 1047, "IBM01047,ibm01047,", 00000, NULL },// IBM EBCDIC Latin-1/Open System
	//{ NCP_8BIT | NCP_RECODE, 1141, "x-ebcdic-germany-euro,xebcdicgermanyeuro,", 00000, NULL },// IBM EBCDIC Germany (20273 + Euro symbol); IBM EBCDIC (Germany-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1142, "x-ebcdic-denmarknorway-euro,xebcdicdenmarknorwayeuro,", 00000, NULL },// IBM EBCDIC Denmark-Norway (20277 + Euro symbol); IBM EBCDIC (Denmark-Norway-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1143, "x-ebcdic-finlandsweden-euro,xebcdicfinlandswedeneuro,", 00000, NULL },// IBM EBCDIC Finland-Sweden (20278 + Euro symbol); IBM EBCDIC (Finland-Sweden-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1144, "x-ebcdic-italy-euro,xebcdicitalyeuro,", 00000, NULL },// IBM EBCDIC Italy (20280 + Euro symbol); IBM EBCDIC (Italy-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1145, "x-ebcdic-spain-euro,xebcdicspaineuro,", 00000, NULL },// IBM EBCDIC Latin America-Spain (20284 + Euro symbol); IBM EBCDIC (Spain-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1146, "x-ebcdic-uk-euro,xebcdicukeuro,", 00000, NULL },// IBM EBCDIC United Kingdom (20285 + Euro symbol); IBM EBCDIC (UK-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1147, "x-ebcdic-france-euro,xebcdicfranceeuro,", 00000, NULL },// IBM EBCDIC France (20297 + Euro symbol); IBM EBCDIC (France-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1148, "x-ebcdic-international-euro,xebcdicinternationaleuro,", 00000, NULL },// IBM EBCDIC International (500 + Euro symbol); IBM EBCDIC (International-Euro)
	//{ NCP_8BIT | NCP_RECODE, 1149, "x-ebcdic-icelandic-euro,xebcdicicelandiceuro,", 00000, NULL },// IBM EBCDIC Icelandic (20871 + Euro symbol); IBM EBCDIC (Icelandic-Euro)
	//{ NCP_8BIT | NCP_RECODE, 20273, "x-EBCDIC-Germany,xebcdicgermany,", 00000, NULL },// IBM EBCDIC (Germany)
	//{ NCP_8BIT | NCP_RECODE, 20277, "x-EBCDIC-DenmarkNorway,xebcdicdenmarknorway,ebcdiccpdk,ebcdiccpno,", 00000, NULL },// IBM EBCDIC (Denmark-Norway)
	//{ NCP_8BIT | NCP_RECODE, 20278, "x-EBCDIC-FinlandSweden,xebcdicfinlandsweden,ebcdicpfi,ebcdiccpse,", 00000, NULL },// IBM EBCDIC (Finland-Sweden)
	//{ NCP_8BIT | NCP_RECODE, 20280, "x-EBCDIC-Italy,xebcdicitaly,", 00000, NULL },// IBM EBCDIC (Italy)
	//{ NCP_8BIT | NCP_RECODE, 20284, "x-EBCDIC-Spain,xebcdicspain,ebcdiccpes,", 00000, NULL },// IBM EBCDIC (Latin America-Spain)
	//{ NCP_8BIT | NCP_RECODE, 20285, "x-EBCDIC-UK,xebcdicuk,ebcdiccpgb,", 00000, NULL },// IBM EBCDIC (United Kingdom)
	//{ NCP_8BIT | NCP_RECODE, 20290, "x-EBCDIC-JapaneseKatakana,xebcdicjapanesekatakana,", 00000, NULL },// IBM EBCDIC (Japanese Katakana)
	//{ NCP_8BIT | NCP_RECODE, 20297, "x-EBCDIC-France,xebcdicfrance,ebcdiccpfr,", 00000, NULL },// IBM EBCDIC (France)
	//{ NCP_8BIT | NCP_RECODE, 20420, "x-EBCDIC-Arabic,xebcdicarabic,ebcdiccpar1,", 00000, NULL },// IBM EBCDIC (Arabic)
	//{ NCP_8BIT | NCP_RECODE, 20423, "x-EBCDIC-Greek,xebcdicgreek,ebcdiccpgr,", 00000, NULL },// IBM EBCDIC (Greek)
	//{ NCP_8BIT | NCP_RECODE, 20424, "x-EBCDIC-Hebrew,xebcdichebrew,ebcdiccphe,", 00000, NULL },// IBM EBCDIC (Hebrew)
	//{ NCP_8BIT | NCP_RECODE, 20833, "x-EBCDIC-KoreanExtended,xebcdickoreanextended,", 00000, NULL },// IBM EBCDIC (Korean Extended)
	//{ NCP_8BIT | NCP_RECODE, 20838, "x-EBCDIC-Thai,xebcdicthai,ibmthai,csibmthai,", 00000, NULL },// IBM EBCDIC (Thai)
	//{ NCP_8BIT | NCP_RECODE, 20871, "x-EBCDIC-Icelandic,xebcdicicelandic,ebcdiccpis,", 00000, NULL },// IBM EBCDIC (Icelandic)
	//{ NCP_8BIT | NCP_RECODE, 20880, "x-EBCDIC-CyrillicRussian,xebcdiccyrillicrussian,ebcdiccyrillic,", 00000, NULL },// IBM EBCDIC (Cyrillic Russian)
	//{ NCP_8BIT | NCP_RECODE, 20905, "x-EBCDIC-Turkish,xebcdicturkish,ebcdiccptr,", 00000, NULL },// IBM EBCDIC (Turkish)
	//{ NCP_8BIT | NCP_RECODE, 20924, "IBM00924,ibm00924,ebcdiclatin9euro,", 00000, NULL },// IBM EBCDIC Latin-1/Open System (1047 + Euro symbol)
	//{ NCP_8BIT | NCP_RECODE, 21025, "x-EBCDIC-CyrillicSerbianBulgarian,xebcdiccyrillicserbianbulgarian,", 00000, NULL },// IBM EBCDIC (Cyrillic Serbian-Bulgarian)
	//{ NCP_8BIT | NCP_RECODE, 50930, "x-EBCDIC-JapaneseAndKana,xebcdicjapaneseandkana,", 00000, NULL },// IBM EBCDIC Japanese (Katakana) Extended
	//{ NCP_8BIT | NCP_RECODE, 50931, "x-EBCDIC-JapaneseAndUSCanada,xebcdicjapaneseanduscanada,", 00000, NULL },// IBM EBCDIC (US-Canada and Japanese)
	//{ NCP_8BIT | NCP_RECODE, 50933, "x-EBCDIC-KoreanAndKoreanExtended,xebcdickoreanandkoreanextended,", 00000, NULL },// IBM EBCDIC (Korean and Korean Extended)
	//{ NCP_8BIT | NCP_RECODE, 50935, "x-EBCDIC-SimplifiedChinese,xebcdicsimplifiedchinese,", 00000, NULL },// IBM EBCDIC (Simplified Chinese Extended and Simplified Chinese)
	//{ NCP_8BIT | NCP_RECODE, 50936, "x-EBCDIC-SimplifiedChinese,xebcdicsimplifiedchinese,", 00000, NULL },// IBM EBCDIC (Chinese Simplified)
	//{ NCP_8BIT | NCP_RECODE, 50937, "x-EBCDIC-TraditionalChinese,xebcdictraditionalchinese,", 00000, NULL },// IBM EBCDIC (US-Canada and Traditional Chinese)
	//{ NCP_8BIT | NCP_RECODE, 50939, "x-EBCDIC-JapaneseAndJapaneseLatin,xebcdicjapaneseandjapaneselatin,", 00000, NULL },// IBM EBCDIC (Japanese (Latin) Extended and Japanese)
	//{ NCP_7BIT | NCP_RECODE, 20105, "x-IA5,xia5,", 00000, NULL },// IA5 (IRV International Alphabet No. 5, 7-bit); Western European (IA5)
	//{ NCP_7BIT | NCP_RECODE, 20106, "x-IA5-German,xia5german,", 00000, NULL },// German (IA5) (7-bit)
	//{ NCP_7BIT | NCP_RECODE, 20107, "x-IA5-Swedish,xia5swedish,", 00000, NULL },// Swedish (IA5) (7-bit)
	//{ NCP_7BIT | NCP_RECODE, 20108, "x-IA5-Norwegian,xia5norwegian,", 00000, NULL },// Norwegian (IA5) (7-bit)
	//{ NCP_8BIT | NCP_RECODE, 20936, "X-CP20936,xcp20936,", 00000, NULL },// Simplified Chinese (GB2312); Chinese Simplified (GB2312-80)
	//{ NCP_8BIT | NCP_RECODE, 20949, "x-cp20949,xcp20949,", 00000, NULL },// Korean Wansung
	{ NCP_7BIT | NCP_RECODE, 52936, "HZ-GB-2312,hzgb2312,hz,", IDS_ENCODING_HZ_GB2312, NULL },// HZ-GB2312 Simplified Chinese; Chinese Simplified (HZ)
	{ NCP_7BIT | NCP_RECODE, 50220, "ISO-2022-JP,iso2022jp,", IDS_ENCODING_ISO2022_JP, NULL },// ISO 2022 Japanese with no halfwidth Katakana; Japanese (JIS)
	{ NCP_7BIT | NCP_RECODE, 50221, "csISO2022JP,csiso2022jp,", IDS_ENCODING_CS_ISO2022_JP, NULL },// ISO 2022 Japanese with halfwidth Katakana; Japanese (JIS-Allow 1 byte Kana)
	{ NCP_7BIT | NCP_RECODE, 50222, "_iso-2022-jp$SIO,iso2022jpsio,", IDS_ENCODING_ISO2022_JP_SOSI, NULL },// ISO 2022 Japanese JIS X 0201-1989; Japanese (JIS-Allow 1 byte Kana - SO/SI)
	{ NCP_7BIT | NCP_RECODE, 50225, "ISO-2022-KR,iso2022kr,csiso2022kr,", IDS_ENCODING_ISO2022_KR, NULL },// Korean (ISO-2022-KR)
	{ NCP_7BIT | NCP_RECODE, 50227, "x-cp50227,xcp50227,", IDS_ENCODING_ISO2022_CNS, NULL },// Chinese Simplified (ISO-2022)
	{ NCP_7BIT | NCP_RECODE, 50229, "ISO-2022-CN,iso2022cn,", IDS_ENCODING_ISO2022_CNT, NULL },// Chinese Traditional (ISO-2022)
	{ NCP_8BIT | NCP_RECODE, 20000, "x-Chinese-CNS,xchinesecns,", IDS_ENCODING_CNS, NULL },// Chinese Traditional (CNS);  CNS Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20001, "x-cp20001,xcp20001,", 00000, NULL },// TCA Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20002, "x-Chinese-Eten,xchineseeten,", 00000, NULL },// Chinese Traditional (Eten); Eten Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20003, "x-cp20003,xcp20003,", 00000, NULL },// IBM5550 Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20004, "x-cp20004,xcp20004,", 00000, NULL },// TeleText Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20005, "x-cp20005,xcp20005,", 00000, NULL },// Wang Taiwan
	//{ NCP_7BIT | NCP_RECODE, 20127, "us-ascii,usascii,", 00000, NULL },// US-ASCII (7-bit)
	//{ NCP_8BIT | NCP_RECODE, 20261, "x-cp20261,xcp20261,", 00000, NULL },// T.61
	//{ NCP_8BIT | NCP_RECODE, 20269, "x-cp20269,xcp20269,", 00000, NULL },// ISO 6937 Non-Spacing Accent
	/*	20932 used by native API and 51932 used by .NET
		https://blogs.msdn.microsoft.com/shawnste/2006/07/18/encoding-getencodings-has-a-couple-duplicate-names/
		https://referencesource.microsoft.com/#mscorlib/system/text/eucjpencoding.cs
	*/
	//{ NCP_8BIT | NCP_RECODE, 51932, "euc-jp,eucjp,xeuc,xeucjp,", 0, NULL },// Japanese (EUC)
	//{ NCP_8BIT | NCP_RECODE, 51936, "euc-cn,euccn,xeuccn,", 00000, NULL },// Chinese Simplified (EUC)
	//{ NCP_8BIT | NCP_RECODE, 51950, "euc,euc,", 00000, NULL },// Chinese Traditional (EUC)
	//{ NCP_8BIT | NCP_RECODE, 57002, "x-iscii-de,xisciide,", 00000, NULL },// ISCII Devanagari
	//{ NCP_8BIT | NCP_RECODE, 57003, "x-iscii-be,xisciibe,", 00000, NULL },// ISCII Bangla
	//{ NCP_8BIT | NCP_RECODE, 57004, "x-iscii-ta,xisciita,", 00000, NULL },// ISCII Tamil
	//{ NCP_8BIT | NCP_RECODE, 57005, "x-iscii-te,xisciite,", 00000, NULL },// ISCII Telugu
	//{ NCP_8BIT | NCP_RECODE, 57006, "x-iscii-as,xisciias,", 00000, NULL },// ISCII Assamese
	//{ NCP_8BIT | NCP_RECODE, 57007, "x-iscii-or,xisciior,", 00000, NULL },// ISCII Odia
	//{ NCP_8BIT | NCP_RECODE, 57008, "x-iscii-ka,xisciika,", 00000, NULL },// ISCII Kannada
	//{ NCP_8BIT | NCP_RECODE, 57009, "x-iscii-ma,xisciima,", 00000, NULL },// ISCII Malayalam
	//{ NCP_8BIT | NCP_RECODE, 57010, "x-iscii-gu,xisciigu,", 00000, NULL },// ISCII Gujarati
	//{ NCP_8BIT | NCP_RECODE, 57011, "x-iscii-pa,xisciipa,", 00000, NULL },// ISCII Panjabi
};

static NP2EncodingGroup sEncodingGroupList[] = {
	// ANSI and OEM are root node
	{ true, IDS_ENCODINGGROUP_UNICODE, { // Unicode
		CPI_UNICODEBOM,
		CPI_UNICODEBEBOM,
		CPI_UNICODE,
		CPI_UNICODEBE,
		CPI_UTF8,
		CPI_UTF8SIGN,
		CPI_UTF7,
	}},
	{ false, IDS_ENCODINGGROUP_WESTERNEURO, { // Western European
		1252,		// Windows-1252
		28591,		// Latin-1, ISO 8859-1
		28605,		// Latin-9, ISO 8859-15
		28604,		// Celtic Latin-8, ISO 8859-14
		437,		// OEM United States
		850,		// OEM Latin-1
		858,		// OEM Latin-1 + Euro
		860,		// OEM Portuguese
		863,		// OEM French Canadian
		10000, 		// Mac Roman
	}},

	// dynamic sorted groups
	{ false, IDS_ENCODINGGROUP_ARABIC, { // Arabic
		1256,		// Windows-1256
		28596,		// ISO 8859-6 Visual
		38596,		// ISO 8859-6-I Logical
		720,		// ASMO DOS-720
		864,		// OEM Arabic
		10004,		// Mac (Arabic)
	}},
	{ false, IDS_ENCODINGGROUP_NORTHERNEURO, { // Baltic, Northern European
		1257,		// Baltic Windows-1257
		28594,		// Baltic Latin-4, ISO 8859-4
		28603,		// Estonian Latin-7, ISO 8859-13
		28600,		// Nordic Latin-6, ISO 8859-10
		775,		// OEM Baltic
		861,		// OEM Icelandic
		865,		// OEM Nordic
		10079,		// Mac (Icelandic)
	}},
	{ false, IDS_ENCODINGGROUP_CENTRALEURO, { // Central and Eastern European
		1250,		// Windows-1250
		28592,		// ISO 8859-2
		852,		// OEM Latin-2
		10029,		// Mac Latin-2
	}},
	{ false, IDS_ENCODINGGROUP_CHINESE, { // Chinese
		// Simplified Chinese
		936,		// GBK
		54936,		// GB18030
		10008,		// Mac (GB 2312)
		50227,		// ISO-2022-CN
		52936,		// HZ-GB2312
		// Traditional Chinese
		950,		// Big5
		20000,		// CNS
		10002,		// Mac (Big5)
		50229,		// ISO-2022-CN
	}},
	{ false, IDS_ENCODINGGROUP_CYRILLIC, { // Cyrillic
		1251,		// Windows-1251
		28595,		// ISO 8859-5
		20866,		// KOI8-R
		21866,		// KOI8-U
		866,		// OEM Russian
		855,		// OEM Cyrillic
		10007,		// Mac (Cyrillic)
		10017,		// Mac (Ukrainian)
	}},
	{ false, IDS_ENCODINGGROUP_GREEK, { // Greek
		1253,		// Windows-1253
		28597,		// ISO 8859-7
		737,		// OEM Greek
		869,		// OEM Modern Greek
		10006,		// Mac (Greek)
	}},
	{ false, IDS_ENCODINGGROUP_HEBREW, { // Hebrew
		1255,		// Windows-1255
		28598,		// ISO 8859-8 Visual
		38598,		// ISO 8859-8-I Logical
		862,		// OEM Hebrew
		10005,		// Mac (Hebrew)
	}},
	{ false, IDS_ENCODINGGROUP_JAPANESE, { // Japanese
		932,		// Shift-JIS
		20932,		// EUC-JP
		10001,		// Mac (Japanese)
		50220,		// ISO-2022-JP
		50221,		// CS ISO-2022-JP
		50222,		// ISO-2022-JP SI/SO
	}},
	{ false, IDS_ENCODINGGROUP_KOREAN, { // Korean
		949,		// UHC
		1361,		// Johab
		51949,		// EUC-KR
		10003,		// Mac (Korean)
		50225,		// ISO-2022-KR
	}},
	{ false, IDS_ENCODINGGROUP_SOUTHEURO, { // South European
		// Southern European
		28593,		// Latin-3, ISO 8859-3
		10010,		// Mac (Romanian)
		// Southeast European
		28606,		// Latin-10, ISO 8859-16
		10082,		// Mac (Croatian)
	}},
	{ false, IDS_ENCODINGGROUP_SOUTHASIA, { // Southeast Asia
		874,		// Thai TIS-620, ISO 8859-11
		10021,		// Mac (Thai)
		1258,		// Vietnamese Windows-1258
	}},
	{ false, IDS_ENCODINGGROUP_TURKISH, { // Turkish
		1254,		// Windows-1254
		28599,		// Latin-5, ISO 8859-9
		857,		// OEM Turkish
		10081,		// Mac (Turkish)
	}},

	// last group
	{ false, IDS_ENCODINGGROUP_EBCDIC, { // IBM EBCDIC
		37,			// US-Canada
		1140,		// US-Canada + Euro
		500,		// International
		870,		// Latin-2
		875,		// Greek Modern
		1026,		// Turkish, Latin-5
	}},
};

//++Autogenerated -- start of section automatically generated
// Created with Python 3.12.0, Unicode 15.0.0
static const uint8_t ANSICharClassifyTable[] = {
// Central European (Windows-1250)
0x22, 0xAA, 0xB8, 0xFF, 0xA8, 0xAA, 0xB8, 0xFF, 0xEC, 0xAE, 0xBA, 0xE2, 0xEA, 0xAE, 0xBE, 0xFB,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xBF,
// Cyrillic (Windows-1251)
0xEF, 0xAA, 0xBA, 0xFF, 0xAB, 0xAA, 0xB8, 0xFF, 0xFC, 0xAE, 0xBB, 0xE2, 0xFA, 0xAF, 0xBB, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
// Western European (Windows-1252)
0xE2, 0xAA, 0xBB, 0x33, 0xA8, 0xAA, 0xBA, 0xF3, 0xA8, 0xAA, 0xBA, 0xA2, 0xFA, 0xAE, 0xBE, 0xBF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF,
// Greek (Windows-1253)
0xE2, 0xAA, 0x88, 0x00, 0xA8, 0xAA, 0x88, 0x00, 0xB8, 0xAA, 0x8A, 0xA2, 0xFA, 0xAE, 0xBF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F,
// Turkish (Windows-1254)
0xE2, 0xAA, 0xBB, 0x03, 0xA8, 0xAA, 0xBA, 0xC3, 0xA8, 0xAA, 0xBA, 0xA2, 0xFA, 0xAE, 0xBE, 0xBF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF,
// Hebrew (Windows-1255)
0xE2, 0xAA, 0x8B, 0x00, 0xA8, 0xAA, 0x8A, 0x00, 0xA8, 0xAA, 0xAA, 0xA2, 0xFA, 0xAE, 0xAE, 0xBF,
0xFF, 0xFF, 0xCF, 0xEF, 0xBE, 0xBF, 0x02, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00,
// Arabic (Windows-1256)
0xEE, 0xAA, 0xBB, 0xFF, 0xAB, 0xAA, 0xBB, 0xC3, 0xA8, 0xAA, 0xBA, 0xA2, 0xFA, 0xAE, 0xAE, 0xBF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xC3,
// Baltic (Windows-1257)
0x22, 0xAA, 0x88, 0xB8, 0xA8, 0xAA, 0x88, 0x28, 0xA0, 0xA2, 0xBB, 0xE2, 0xFA, 0xAE, 0xBF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xBF,
// Vietnamese (Windows-1258)
0xE2, 0xAA, 0x8B, 0x03, 0xA8, 0xAA, 0x8A, 0xC3, 0xA8, 0xAA, 0xBA, 0xA2, 0xFA, 0xAE, 0xBE, 0xBF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xEF,
// Thai (Windows-874)
0x02, 0x08, 0x00, 0x00, 0xA8, 0xAA, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x80, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xAF, 0x00,
};

static inline const uint8_t* GetANSICharClassifyTable(UINT acp, int *length) {
	const UINT diff = acp - 1250;
	if (diff <= 1258 - 1250) {
		return ANSICharClassifyTable + diff*32;
	}
	if (acp == 874) {
		return ANSICharClassifyTable + 288;
	}
	*length = 0;
	return NULL;
}

const char* GetFoldDisplayEllipsis(UINT cpEdit, UINT acp) {
	switch (cpEdit) {
	case SC_CP_UTF8:
		return "\xC2\xB7\xC2\xB7\xC2\xB7";
	case 932: // Shift-JIS
		return "\xA5\xA5\xA5";
	case 936: // GBK
	case 949: // UHC
		return "\xA1\xA4\xA1\xA4\xA1\xA4";
	case 950: // Big5
		return "\xA1\x50\xA1\x50\xA1\x50";
	case 1361: // Johab
		return "\xD9\x34\xD9\x34\xD9\x34";
	}
	// SBCS
	cpEdit = acp - 1250;
	if (cpEdit <= 1258 - 1250) {
		return "\xB7\xB7\xB7";
	}
	return "...";
}
//--Autogenerated -- end of section automatically generated

void Encoding_ReleaseResources(void) {
	if (g_AllEncodingLabel) {
		NP2HeapFree(g_AllEncodingLabel);
	}
}

static inline bool IsValidEncoding(const NP2ENCODING *encoding) {
	return (encoding->uFlags & NCP_INTERNAL) || IsValidCodePage(encoding->uCodePage);
}

//=============================================================================
//
// EditSetNewEncoding()
//
bool EditSetNewEncoding(int iEncoding, int iNewEncoding, BOOL bNoUI, bool bSetSavePoint) {
	if (iEncoding != iNewEncoding) {
		if (iEncoding != CPI_DEFAULT && iNewEncoding != CPI_DEFAULT) {
			return true;
		}

		const UINT cpSrc = (mEncoding[iEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8;
		const UINT cpDest = (mEncoding[iNewEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8;

		if (SciCall_GetLength() == 0) {
			const bool bIsEmptyUndoHistory = !(SciCall_CanUndo() || SciCall_CanRedo());

			if (bNoUI || bIsEmptyUndoHistory || InfoBoxWarn(MB_YESNO, L"MsgConv2", IDS_ASK_ENCODING2) == IDYES) {
				EditConvertText(cpSrc, cpDest, bSetSavePoint);
				return true;
			}
		} else if (bNoUI || InfoBoxWarn(MB_YESNO, L"MsgConv1", IDS_ASK_ENCODING) == IDYES) {
			BeginWaitCursor();
			EditConvertText(cpSrc, cpDest, false);
			EndWaitCursor();
			return true;
		}
	}

	return false;
}

void EditOnCodePageChanged(UINT oldCodePage, bool showControlCharacter, LPEDITFINDREPLACE lpefr) {
	const UINT cpEdit = SciCall_GetCodePage();
	const UINT acp = GetACP();
	const bool lastFind = StrNotEmptyA(lpefr->szFind); // need to convert last find & replace string.

	if (oldCodePage == SC_CP_UTF8) {
		if (lastFind) {
			WCHAR wch[NP2_FIND_REPLACE_LIMIT];
			MultiByteToWideChar(CP_UTF8, 0, lpefr->szFindUTF8, -1, wch, COUNTOF(wch));
			WideCharToMultiByte(cpEdit, 0, wch, -1, lpefr->szFind, COUNTOF(lpefr->szFind), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, lpefr->szReplaceUTF8, -1, wch, COUNTOF(wch));
			WideCharToMultiByte(cpEdit, 0, wch, -1, lpefr->szReplace, COUNTOF(lpefr->szReplace), NULL, NULL);
		}
		if (cpEdit == 0) {
			// UTF-8 to SBCS
			int length = 32;
			const uint8_t *buf = GetANSICharClassifyTable(acp, &length);
			SciCall_SetCharClassesEx(length, buf);
		} else {
			// UTF-8 to DBCS
		}
	} else {
		if (lastFind) {
			strcpy(lpefr->szFind, lpefr->szFindUTF8);
			strcpy(lpefr->szReplace, lpefr->szReplaceUTF8);
		}
		if (showControlCharacter) {
			EditShowUnicodeControlCharacter(true);
		}
		if (oldCodePage == 0) {
			// SBCS to UTF-8
			SciCall_SetCharClassesEx(0, NULL);
		} else {
			// DBCS to UTF-8
		}
	}

	const char *text = GetFoldDisplayEllipsis(cpEdit, acp);
	SciCall_SetDefaultFoldDisplayText(text);
}

//=============================================================================
//
// Encoding Helper Functions
//
void Encoding_InitDefaults(void) {
	UINT acp = GetACP();
	if (acp == CP_UTF8) {
		GetLegacyACP(&acp);
		wsprintf(wchANSI, L" (UTF-8, %u)", acp);
	} else {
		wsprintf(wchANSI, L" (%u)", acp);
	}

	mEncoding[CPI_DEFAULT].uCodePage = acp;
	if (IsDBCSCodePage(acp) || acp == CP_UTF8) {
		iDefaultCodePage = acp;
	} else {
		iDefaultCodePage = CP_ACP;
	}

	CHARSETINFO ci;
	if (TranslateCharsetInfo((DWORD *)(UINT_PTR)iDefaultCodePage, &ci, TCI_SRCCODEPAGE)) {
		iDefaultCharSet = ci.ciCharset;
	} else {
		iDefaultCharSet = ANSI_CHARSET;
	}

	UINT oemcp = GetOEMCP();
	if (oemcp == CP_UTF8) {
		GetLegacyOEMCP(&oemcp);
		wsprintf(wchOEM, L" (UTF-8, %u)", oemcp);
	} else {
		wsprintf(wchOEM, L" (%u)", oemcp);
	}

	mEncoding[CPI_OEM].uCodePage = oemcp;
	g_DOSEncoding = CPI_OEM;
	// Try to set the DOS encoding to DOS-437 if the default OEMCP is not DOS-437
	if (oemcp != 437 && IsValidCodePage(437)) {
		g_DOSEncoding = Encoding_GetIndex(437);
	}
}

int Encoding_MapIniSetting(bool bLoad, UINT iSetting) {
	NP2_static_assert(CPI_UTF7 == 8);
	if (iSetting == CPI_UTF7) {
		return iSetting;
	}
	if (iSetting < 8) {
		const UINT maskLoad = (CPI_DEFAULT << 4*0)
			| (CPI_UNICODEBOM << 4*1)
			| (CPI_UNICODEBEBOM << 4*2)
			| (CPI_UTF8 << 4*3)
			| (CPI_UTF8SIGN << 4*4)
			| (CPI_OEM << 4*5)
			| (CPI_UNICODE << 4*6)
			| (CPI_UNICODEBE << 4*7);
		const UINT maskStore = (0 << CPI_DEFAULT*4)
			| (1 << CPI_UNICODEBOM*4)
			| (2 << CPI_UNICODEBEBOM*4)
			| (3 << CPI_UTF8*4)
			| (4 << CPI_UTF8SIGN*4)
			| (5 << CPI_OEM*4)
			| (6 << CPI_UNICODE*4)
			| (7 << CPI_UNICODEBE*4);
		const UINT mask = bLoad ? maskLoad : maskStore;
		return (mask >> (iSetting*4)) & 15;
	}

	if (bLoad) {
		if ((int)iSetting < 0) {
			return CPI_GLOBAL_DEFAULT; // default encoding
		}
		if (IsValidCodePage(iSetting)) {
			iSetting = Encoding_GetIndex(iSetting);
			return max_i(iSetting, CPI_FIRST);
		}
		return CPI_DEFAULT; // unknown encoding
	}
	return mEncoding[iSetting].uCodePage;
}

void Encoding_GetLabel(int iEncoding) {
	if (StrIsEmpty(mEncoding[iEncoding].wchLabel)) {
		WCHAR wch[256] = L"";
		GetString(mEncoding[iEncoding].idsName, wch, COUNTOF(wch));
		LPCWSTR pwsz = StrChr(wch, L';');
		if (pwsz != NULL) {
			++pwsz;
		}
		if (StrIsEmpty(pwsz)) {
			pwsz = wch;
		}
		if (g_AllEncodingLabel == NULL) {
			g_AllEncodingLabel = (LPWSTR)NP2HeapAlloc(COUNTOF(mEncoding) * MAX_ENCODING_LABEL_SIZE * sizeof(WCHAR));
		}
		mEncoding[iEncoding].wchLabel = g_AllEncodingLabel + iEncoding * MAX_ENCODING_LABEL_SIZE;
		lstrcpyn(mEncoding[iEncoding].wchLabel, pwsz, MAX_ENCODING_LABEL_SIZE);
	}
}

int Encoding_Match(LPCWSTR pwszTest) {
	char tchTest[256];
	WideCharToMultiByte(CP_UTF8, 0, pwszTest, -1, tchTest, COUNTOF(tchTest), NULL, NULL);
	return Encoding_MatchA(tchTest);
}

int Encoding_MatchA(LPCSTR pchTest) {
	char chTest[256];
	LPCSTR pchSrc = pchTest;
	char *pchDst = chTest;

	*pchDst++ = ',';
	char ch;
	while ((ch = *pchSrc) != '\0') {
		if (ch >= 'A' && ch <= 'Z') {
			*pchDst++ = UnsafeLower(ch);
		} else if (!(ch == '-' || ch == '_')) {
			// ignore hyphen and underscore
			*pchDst++ = ch;
		}
		pchSrc++;
	}

	*pchDst++ = ',';
	*pchDst = '\0';

	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		const NP2ENCODING *encoding = &mEncoding[i];
		if (strstr(encoding->pszParseNames, chTest)) {
			if (IsValidEncoding(encoding)) {
				return i;
			}
			break;
		}
	}

	return CPI_NONE;
}

bool Encoding_IsValid(int iEncoding) {
	return (UINT)iEncoding < COUNTOF(mEncoding)
		&& IsValidEncoding(&mEncoding[iEncoding]);
}

typedef struct ENCODINGENTRY {
	int id;
	WCHAR wch[256];
} ENCODINGENTRY, *PENCODINGENTRY;

static int __cdecl CmpEncoding(const void *s1, const void *s2) {
	return wcscmp(((PENCODINGENTRY)s1)->wch, ((PENCODINGENTRY)s2)->wch);
}

int Encoding_GetIndex(UINT codePage) {
	for (int i = CPI_UTF8; i < (int)COUNTOF(mEncoding); i++) {
		if (mEncoding[i].uCodePage == codePage) {
			return i;
		}
	}
	return CPI_NONE;
}

int Encoding_GetAnsiIndex(void) {
	int iEncoding = CPI_DEFAULT;
	const UINT acp = GetACP();
	if (acp == CP_UTF8) {
		iEncoding = CPI_UTF8;
	} else if (bLoadANSIasUTF8 || bLoadASCIIasUTF8) {
		iEncoding = Encoding_GetIndex(acp);
	}
	return iEncoding;
}

static inline int GetEncodingImageIndex(const NP2ENCODING *encoding) {
	return IsValidEncoding(encoding) ? 0 : 1;
}

void Encoding_AddToTreeView(HWND hwnd, int idSel, bool bRecodeOnly) {
	PENCODINGENTRY pEE = (PENCODINGENTRY)NP2HeapAlloc(COUNTOF(sEncodingGroupList) * sizeof(ENCODINGENTRY));
	for (int i = 0; i < (int)COUNTOF(sEncodingGroupList); i++) {
		NP2EncodingGroup *group = &sEncodingGroupList[i];
		pEE[i].id = i;
		GetString(group->idsName, pEE[i].wch, COUNTOF(pEE[i].wch));
		if (!group->bMapped) {
			// map code page to index
			for (UINT j = 0; j < COUNTOF(group->encodings); j++) {
				const UINT page = group->encodings[j];
				const int index = Encoding_GetIndex(page);
				group->encodings[j] = index;
				if (index < CPI_FIRST) {
					break;
				}
			}
			group->bMapped = true;
		}
	}

	// exclude Unicode, Western European and EBCDIC
	qsort(pEE + 2, COUNTOF(sEncodingGroupList) - 3, sizeof(ENCODINGENTRY), CmpEncoding);

#if 0	// verify sEncodingGroupList
	{
		int encodings[COUNTOF(mEncoding)] = {0};
		for (UINT i = 0; i < COUNTOF(sEncodingGroupList); i++) {
			const NP2EncodingGroup *group = &sEncodingGroupList[i];
			for (UINT j = 0; j < COUNTOF(group->encodings); j++) {
				const int index = group->encodings[j];
				if (index >= 0) {
					encodings[index]++;
				} else {
					break;
				}
			}
		}
		for (UINT i = 0; i < COUNTOF(encodings); i++) {
			const int count = encodings[i];
			const UINT page = mEncoding[i].uCodePage;
			if (count == 0) {
				DebugPrintf("%s missing code page: %u\n", __func__, page);
			} else if (count > 1) {
				DebugPrintf("%s duplicate code page: %u\n", __func__, page);
			}
		}
	}
#endif

	WCHAR wchBuf[256];
	TVINSERTSTRUCT tvis;
	memset(&tvis, 0, sizeof(TVINSERTSTRUCT));
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	HTREEITEM hParent = TVI_FIRST;
	HTREEITEM hSelNode = NULL;
	HTREEITEM hSelParent = NULL;

	// ANSI and OEM
	for (int id = CPI_DEFAULT; id <= CPI_OEM; id++) {
		const NP2ENCODING *encoding = &mEncoding[id];
		GetString(encoding->idsName, wchBuf, COUNTOF(wchBuf));
		LPWSTR pwsz = StrChr(wchBuf, L';');
		if (pwsz != NULL) {
			*pwsz = L'\0';
		}
		if (id == CPI_DEFAULT) {
			StrCatBuff(wchBuf, wchANSI, COUNTOF(wchBuf));
		} else if (id == CPI_OEM) {
			StrCatBuff(wchBuf, wchOEM, COUNTOF(wchBuf));
		}

		tvis.hInsertAfter = hParent;
		tvis.item.pszText = wchBuf;
		tvis.item.iImage = GetEncodingImageIndex(encoding);
		tvis.item.iSelectedImage = tvis.item.iImage;
		tvis.item.lParam = 1 + id;

		hParent = TreeView_InsertItem(hwnd, &tvis);
		if (idSel == id) {
			hSelNode = hParent;
		}
	}

	UINT legacyACP = GetACP();
	if (legacyACP == CP_UTF8) {
		legacyACP = mEncoding[CPI_DEFAULT].uCodePage;
	}
	for (UINT i = 0; i < COUNTOF(sEncodingGroupList); i++) {
		const NP2EncodingGroup *group = &sEncodingGroupList[pEE[i].id];
		tvis.hParent = NULL;
		tvis.hInsertAfter = hParent;
		tvis.item.pszText = pEE[i].wch;
		tvis.item.iImage = 2; // folder
		tvis.item.iSelectedImage = 2;
		tvis.item.lParam = 0; // group

		hParent = TreeView_InsertItem(hwnd, &tvis);
		tvis.hParent = hParent;

		HTREEITEM hTreeNode = TVI_FIRST;
		bool expand = i < 2; // Unicode, Western European

		for (UINT j = 0; j < COUNTOF(group->encodings); j++) {
			const int id = group->encodings[j];
			if (id <= 0) {
				break;
			}
			const NP2ENCODING *encoding = &mEncoding[id];
			if (!bRecodeOnly || (encoding->uFlags & NCP_RECODE)) {
				GetString(encoding->idsName, wchBuf, COUNTOF(wchBuf));
				LPWSTR pwsz = StrChr(wchBuf, L';');
				if (pwsz != NULL) {
					*pwsz = L'\0';
				}

				tvis.hInsertAfter = hTreeNode;
				tvis.item.pszText = wchBuf;
				tvis.item.iImage = GetEncodingImageIndex(encoding);
				tvis.item.iSelectedImage = tvis.item.iImage;
				tvis.item.lParam = 1 + id;

				hTreeNode = TreeView_InsertItem(hwnd, &tvis);
				if (idSel == id) {
					hSelNode = hTreeNode;
					hSelParent = hParent;
					expand = true;
				} else if (!expand && (id == iDefaultCodePage || id == iCurrentEncoding
					|| legacyACP == encoding->uCodePage)) {
					// group contains default code, current code page, ANSI code page.
					expand = true;
				}
			}
		}
		if (expand) {
			TreeView_Expand(hwnd, hParent, TVE_EXPAND);
		}
	}

	if (hSelParent != NULL) {
		TreeView_EnsureVisible(hwnd, hSelParent);
	}
	TreeView_Select(hwnd, hSelNode, TVGN_CARET);
	NP2HeapFree(pEE);
}

bool Encoding_GetFromTreeView(HWND hwnd, int *pidEncoding, bool bQuiet) {
	HTREEITEM hTreeNode = TreeView_GetSelection(hwnd);
	if (hTreeNode != NULL) {
		TVITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = TVIF_PARAM;
		item.hItem = hTreeNode;
		TreeView_GetItem(hwnd, &item);
		if (item.lParam > 0) {
			const int id = (int)(item.lParam - 1);
			if (Encoding_IsValid(id)) {
				*pidEncoding = id;
				return true;
			}
			if (!bQuiet) {
				MsgBoxWarn(MB_OK, IDS_ERR_ENCODINGNA);
			}
		}
	}
	return false;
}

#if 0
void Encoding_AddToListView(HWND hwnd, int idSel, bool bRecodeOnly) {
	PENCODINGENTRY pEE = (PENCODINGENTRY)NP2HeapAlloc(COUNTOF(mEncoding) * sizeof(ENCODINGENTRY));
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		pEE[i].id = i;
		GetString(mEncoding[i].idsName, pEE[i].wch, COUNTOF(pEE[i].wch));
	}
	qsort(pEE, COUNTOF(mEncoding), sizeof(ENCODINGENTRY), CmpEncoding);

	WCHAR wchBuf[256];
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = wchBuf;

	int iSelItem = -1;
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		const int id = pEE[i].id;
		const NP2ENCODING *encoding = &mEncoding[id];
		if (!bRecodeOnly || (encoding->uFlags & NCP_RECODE)) {
			lvi.iItem = ListView_GetItemCount(hwnd);
			LPWSTR pwsz = StrChr(pEE[i].wch, L';');
			if (pwsz != NULL) {
				lstrcpyn(wchBuf, pwsz + 1, COUNTOF(wchBuf));
				pwsz = StrChr(wchBuf, L';');
				if (pwsz != NULL) {
					*pwsz = L'\0';
				}
			} else {
				lstrcpyn(wchBuf, pEE[i].wch, COUNTOF(wchBuf));
			}

			if (id == CPI_DEFAULT) {
				StrCatBuff(wchBuf, wchANSI, COUNTOF(wchBuf));
			} else if (id == CPI_OEM) {
				StrCatBuff(wchBuf, wchOEM, COUNTOF(wchBuf));
			}

			lvi.iImage = GetEncodingImageIndex(encoding);
			lvi.lParam = (LPARAM)id;
			ListView_InsertItem(hwnd, &lvi);

			if (idSel == id) {
				iSelItem = lvi.iItem;
			}
		}
	}

	NP2HeapFree(pEE);

	if (iSelItem >= 0) {
		ListView_SetItemState(hwnd, iSelItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		ListView_EnsureVisible(hwnd, iSelItem, FALSE);
	} else {
		ListView_SetItemState(hwnd, 0, LVIS_FOCUSED, LVIS_FOCUSED);
		ListView_EnsureVisible(hwnd, 0, FALSE);
	}
}

bool Encoding_GetFromListView(HWND hwnd, int *pidEncoding) {
	LVITEM lvi;

	lvi.iItem = ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_SELECTED);
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;

	if (ListView_GetItem(hwnd, &lvi)) {
		if (Encoding_IsValid((int)lvi.lParam)) {
			*pidEncoding = (int)lvi.lParam;
			return true;
		}
		MsgBoxWarn(MB_OK, IDS_ERR_ENCODINGNA);
	}

	return false;
}

void Encoding_AddToComboboxEx(HWND hwnd, int idSel, bool bRecodeOnly) {
	PENCODINGENTRY pEE = (PENCODINGENTRY)NP2HeapAlloc(COUNTOF(mEncoding) * sizeof(ENCODINGENTRY));
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		pEE[i].id = i;
		GetString(mEncoding[i].idsName, pEE[i].wch, COUNTOF(pEE[i].wch));
	}
	qsort(pEE, COUNTOF(mEncoding), sizeof(ENCODINGENTRY), CmpEncoding);

	WCHAR wchBuf[256];
	COMBOBOXEXITEM cbei;
	memset(&cbei, 0, sizeof(COMBOBOXEXITEM));

	cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
	cbei.pszText = wchBuf;
	cbei.cchTextMax = COUNTOF(wchBuf);
	cbei.iImage = 0;
	cbei.iSelectedImage = 0;

	int iSelItem = -1;
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		const int id = pEE[i].id;
		const NP2ENCODING *encoding = &mEncoding[id];
		if (!bRecodeOnly || (encoding->uFlags & NCP_RECODE)) {
			cbei.iItem = ComboBox_GetCount(hwnd);
			LPWSTR pwsz = StrChr(pEE[i].wch, L';');
			if (pwsz != NULL) {
				lstrcpyn(wchBuf, pwsz + 1, COUNTOF(wchBuf));
				pwsz = StrChr(wchBuf, L';');
				if (pwsz != NULL) {
					*pwsz = L'\0';
				}
			} else {
				lstrcpyn(wchBuf, pEE[i].wch, COUNTOF(wchBuf));
			}

			if (id == CPI_DEFAULT) {
				StrCatBuff(wchBuf, wchANSI, COUNTOF(wchBuf));
			} else if (id == CPI_OEM) {
				StrCatBuff(wchBuf, wchOEM, COUNTOF(wchBuf));
			}

			cbei.iImage = GetEncodingImageIndex(encoding);
			cbei.lParam = (LPARAM)id;
			SendMessage(hwnd, CBEM_INSERTITEM, 0, (LPARAM)&cbei);

			if (idSel == id) {
				iSelItem = (int)cbei.iItem;
			}
		}
	}

	NP2HeapFree(pEE);

	if (iSelItem >= 0) {
		ComboBox_SetCurSel(hwnd, iSelItem);
	}
}

bool Encoding_GetFromComboboxEx(HWND hwnd, int *pidEncoding) {
	COMBOBOXEXITEM cbei;

	cbei.iItem = ComboBox_GetCurSel(hwnd);
	cbei.mask = CBEIF_LPARAM;

	if (SendMessage(hwnd, CBEM_GETITEM, 0, (LPARAM)&cbei)) {
		if (Encoding_IsValid((int)cbei.lParam)) {
			*pidEncoding = (int)cbei.lParam;
			return true;
		}
		MsgBoxWarn(MB_OK, IDS_ERR_ENCODINGNA);
	}

	return false;
}
#endif

//=============================================================================
//
// CodePageFromCharSet()
//
UINT CodePageFromCharSet(UINT uCharSet) {
	CHARSETINFO ci;
	if (TranslateCharsetInfo((DWORD *)(UINT_PTR)uCharSet, &ci, TCI_SRCCHARSET)) {
		return ci.ciACP;
	}
	return GetACP();
}


static inline bool IsC0ControlChar(uint8_t ch) {
#if 1
	return ch < 32 && ((uint8_t)(ch - 0x09)) > (0x0d - 0x09);
#else
	// exclude whitespace and separator
	return ch < 0x1c && ((uint8_t)(ch - 0x09)) > (0x0d - 0x09);
#endif
}

bool MaybeBinaryFile(const uint8_t *ptr, DWORD length) {
	/* Test C0 Control Character
	These characters are not reused in most text encodings, and do not appear in normal text files.
	Most binary files have reserved fields (mostly zeros) or small values in the header.
	Treat the file as binary when we find two adjacent C0 control characters
	(very common in file header) or some (currently set to 8) C0 control characters. */

	length = min_u(length, 1024);
	const uint8_t * const end = ptr + length;
	UINT count = 0;
	while (ptr < end) {
		uint8_t ch = *ptr++;
		if (IsC0ControlChar(ch)) {
			++count;
			ch = *ptr++;
			if ((count >= 8) || IsC0ControlChar(ch)) {
				return true;
			}
		}
	}
	return false;
}

static inline BOOL IsValidMultiByte(UINT codePage, const char *lpData, DWORD cbData) {
	return MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, lpData, cbData, NULL, 0);
}

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
static inline BOOL IsValidWideChar(LPCWSTR lpWide, DWORD cchWide) {
	return WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, lpWide, cchWide, NULL, 0, NULL, NULL);
}
#endif

static int DetectUnicode(char *pTest, DWORD nLength, bool ascii) {
	if (ascii) {
		// find ASCII inside first or last 4 KiB text
		const DWORD size = 4096;
		if (memchr(pTest, 0, min_u(nLength, size)) == NULL
			&& (nLength <= size || memchr(pTest + (nLength - size), 0, size) == NULL)) {
			return CPI_DEFAULT;
		}
	}

	int i = 0xFFFF;
	IsTextUnicode(pTest, nLength, &i);
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	if (i != 0xFFFF && (i & IS_TEXT_UNICODE_ILLEGAL_CHARS) == 0) {
		if (i & IS_TEXT_UNICODE_UNICODE_MASK) {
			if (IsValidWideChar((LPCWSTR)(pTest), nLength/2)) {
				return CPI_UNICODE;
			}
		}
		if (i & IS_TEXT_UNICODE_REVERSE_MASK) {
			_swab(pTest, pTest, nLength);
			if (IsValidWideChar((LPCWSTR)(pTest), nLength/2)) {
				return CPI_UNICODEBE;
			}
			_swab(pTest, pTest, nLength);
		}
	}

#else
	if (i != 0xFFFF && (i & IS_TEXT_UNICODE_ILLEGAL_CHARS) == 0) {
		UINT mask = ascii ? (IS_TEXT_UNICODE_UNICODE_MASK & ~IS_TEXT_UNICODE_STATISTICS) : IS_TEXT_UNICODE_UNICODE_MASK;
		if (i & mask) {
			return CPI_UNICODE;
		}
		mask = ascii ? (IS_TEXT_UNICODE_REVERSE_MASK & ~IS_TEXT_UNICODE_REVERSE_STATISTICS) : IS_TEXT_UNICODE_REVERSE_MASK;
		if (i & mask) {
			return CPI_UNICODEBE;
		}
	}
#endif
	return CPI_DEFAULT;
}

#if 0
static int DetectUTF16Latin1(const char *pTest, DWORD nLength) {
	const char *pt = pTest;
	const char * const end = pt + nLength;

#if NP2_USE_AVX2
	nLength &= sizeof(__m256i) - 1;
	nLength = UINT32_MAX << nLength;
	const __m256i zero = _mm256_setzero_si256();
	__m256i test = _mm256_set1_epi16(-0x0100); // 0xFF00
	uint32_t expected = 0xAAAAAAAA;
	do {
		const __m256i chunk = _mm256_loadu_si256((__m256i *)pt);
		pt += sizeof(__m256i);
		if (_mm256_testz_si256(chunk, test) == 0)
		//if (andn_u32(mask, expected) != 0)
		{
			uint32_t mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(chunk, zero));
			if (pt > end) {
				mask |= nLength;
				if (andn_u32(mask, expected) == 0) {
					break;
				}
			}
			if ((expected & 1) == 0) {
				expected >>= 1;
				if (andn_u32(mask, expected) == 0) {
					pt = pTest;
					test = _mm256_srli_si256(test, 1);
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	// end NP2_USE_AVX2
#elif NP2_USE_SSE2
	uint32_t padding = nLength & (sizeof(__m128i) - 1);
	const uint32_t high = UINT32_MAX << padding;
	const uint32_t lower = ~high; // (1 << padding) - 1
	padding = padding ? high : 0;
	nLength &= 2*sizeof(__m128i) - 1;
	const __m128i zero = _mm_setzero_si128();
	uint32_t expected = 0xAAAA;
	do {
		const __m128i chunk1 = _mm_loadu_si128((__m128i *)pt);
		const __m128i chunk2 = _mm_loadu_si128((__m128i *)(pt + sizeof(__m128i)));
		pt += 2*sizeof(__m128i);
		uint32_t mask = ~_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_or_si128(chunk1, chunk2), zero));
		if ((mask & expected) != 0) {
			if (pt > end) {
				mask = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk1, zero));
				uint32_t last = padding;
				if (nLength > sizeof(__m128i)) {
					last = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk2, zero));
					last &= lower;
				}
				mask |= last;
				mask = ~mask;
				if ((mask & expected) == 0) {
					break;
				}
			}
			if ((expected & 1) == 0) {
				expected >>= 1;
				if ((mask & expected) == 0) {
					pt = pTest;
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	// end NP2_USE_SSE2
#elif defined(_WIN64)
	nLength &= sizeof(uint64_t) - 1;
	nLength = (sizeof(uint64_t) - nLength)*8;
	uint64_t expected = UINT64_C(0xFF00FF00FF00FF00);
	do {
		uint64_t value = *((const uint64_t *)pt);
		pt += sizeof(uint64_t);
		if ((value & expected) != 0) {
			if (pt > end) {
				value <<= nLength;
				if ((value & expected) == 0) {
					break;
				}
			}
			if ((expected & 1) == 0) {
				expected >>= 8;
				if ((value & expected) == 0) {
					pt = pTest;
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	// end _WIN64
#else
	nLength &= sizeof(uint32_t) - 1;
	nLength = (sizeof(uint32_t) - nLength)*8;
	uint32_t expected = 0xFF00FF00U;
	do {
		uint32_t value = *((const uint32_t *)pt);
		pt += sizeof(uint32_t);
		if ((value & expected) != 0) {
			if (pt > end) {
				value <<= nLength;
				if ((value & expected) == 0) {
					break;
				}
			}
			if ((expected & 1) == 0) {
				expected >>= 8;
				if ((value & expected) == 0) {
					pt = pTest;
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	// end _WIN32
#endif

	return CPI_UNICODE + (expected & 1);
}
#endif

static int DetectUTF16LatinExt(const char *pTest, DWORD nLength) {
	const char *pt = pTest;
	const char * const end = pt + nLength;

#if NP2_USE_AVX2
	nLength &= sizeof(__m256i) - 1;
	nLength = UINT32_MAX << nLength;
	const __m256i zero = _mm256_setzero_si256();
	__m256i test = _mm256_set1_epi16(-0x0800); // 0xF800
	uint32_t expected = 0xAAAAAAAA;
	do {
		const __m256i chunk = _mm256_loadu_si256((__m256i *)pt);
		pt += sizeof(__m256i);
		if (_mm256_testz_si256(chunk, test) == 0)
		//if (andn_u32(mask, expected) != 0)
		{
			uint32_t mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_and_si256(chunk, test), zero));
			if (pt > end) {
				mask |= nLength;
				if (andn_u32(mask, expected) == 0) {
					break;
				}
			}
			if ((expected & 1) == 0) {
				expected >>= 1;
				if (andn_u32(mask, expected) == 0) {
					pt = pTest;
					test = _mm256_srli_si256(test, 1);
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	// end NP2_USE_AVX2
#elif NP2_USE_SSE2
	uint32_t padding = nLength & (sizeof(__m128i) - 1);
	const uint32_t high = UINT32_MAX << padding;
	const uint32_t lower = ~high; // (1 << padding) - 1
	padding = padding ? high : 0;
	nLength &= 2*sizeof(__m128i) - 1;
	__m128i test = _mm_set1_epi16(-0x0800); // 0xF800
	const __m128i zero = _mm_setzero_si128();
	uint32_t expected = 0xAAAA;
	do {
		const __m128i chunk1 = _mm_loadu_si128((__m128i *)pt);
		const __m128i chunk2 = _mm_loadu_si128((__m128i *)(pt + sizeof(__m128i)));
		pt += 2*sizeof(__m128i);
		uint32_t mask = ~_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_and_si128(_mm_or_si128(chunk1, chunk2), test), zero));
		if ((mask & expected) != 0) {
			if (pt > end) {
				mask = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_and_si128(chunk1, test), zero));
				uint32_t last = padding;
				if (nLength > sizeof(__m128i)) {
					last = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_and_si128(chunk2, test), zero));
					last &= lower;
				}
				mask |= last;
				mask = ~mask;
				if ((mask & expected) == 0) {
					break;
				}
			}
			if ((expected & 1) == 0) {
				expected >>= 1;
				if ((mask & expected) == 0) {
					pt = pTest;
					test = _mm_srli_si128(test, 1);
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	// end NP2_USE_SSE2
#elif defined(_WIN64)
	nLength &= sizeof(uint64_t) - 1;
	nLength = (sizeof(uint64_t) - nLength)*8;
	uint64_t expected = UINT64_C(0xF800F800F800F800);
	do {
		uint64_t value = *((const uint64_t *)pt);
		pt += sizeof(uint64_t);
		if ((value & expected) != 0) {
			if (pt > end) {
				value <<= nLength;
				if ((value & expected) == 0) {
					break;
				}
			}
			if ((expected & 8) == 0) {
				expected >>= 8;
				if ((value & expected) == 0) {
					pt = pTest;
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	expected >>= 3;
	// end _WIN64
#else
	nLength &= sizeof(uint32_t) - 1;
	nLength = (sizeof(uint32_t) - nLength)*8;
	uint32_t expected = 0xF800F800U;
	do {
		uint32_t value = *((const uint32_t *)pt);
		pt += sizeof(uint32_t);
		if ((value & expected) != 0) {
			if (pt > end) {
				value <<= nLength;
				if ((value & expected) == 0) {
					break;
				}
			}
			if ((expected & 8) == 0) {
				expected >>= 8;
				if ((value & expected) == 0) {
					pt = pTest;
					continue;
				}
			}
			return CPI_DEFAULT;
		}
	} while (pt < end);
	expected >>= 3;
	// end _WIN32
#endif

	return CPI_UNICODE + (expected & 1);
}

#if 0
bool IsUTF8(const char *pTest, DWORD nLength) {
	static const uint8_t byte_class_table[256] = {
		/* 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
		/* 00 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 10 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 20 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 30 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 40 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 50 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 60 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 70 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		/* 80 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		/* 90 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		/* A0 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		/* B0 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		/* C0 */ 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		/* D0 */ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		/* E0 */ 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7,
		/* F0 */ 9, 10, 10, 10, 11, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
		/* 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F*/
	};

	/* state table */
	typedef enum {
		kSTART = 0, kA, kB, kC, kD, kE, kF, kG, kERROR, kNumOfStates
	} utf8_state;

	static const uint8_t state_table[] = {
		/*							   kSTART, kA,     kB,     kC,     kD,     kE,     kF,     kG,     kERROR */
		/* 0x00-0x7F: 0				*/ kSTART, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0x80-0x8F: 1				*/ kERROR, kSTART, kA,     kERROR, kA,     kB,     kERROR, kB,     kERROR,
		/* 0x90-0x9f: 2				*/ kERROR, kSTART, kA,     kERROR, kA,     kB,     kB,     kERROR, kERROR,
		/* 0xa0-0xbf: 3				*/ kERROR, kSTART, kA,     kA,     kERROR, kB,     kB,     kERROR, kERROR,
		/* 0xc0-0xc1, 0xf5-0xff: 4  */ kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0xc2-0xdf: 5				*/ kA,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0xe0: 6					*/ kC,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0xe1-0xec, 0xee-0xef: 7  */ kB,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0xed: 8					*/ kD,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0xf0: 9					*/ kF,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0xf1-0xf3: 10			*/ kE,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR,
		/* 0xf4: 11					*/ kG,     kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR, kERROR
	};

#define BYTE_CLASS(b) (byte_class_table[b])
#define NEXT_STATE(b, cur) (state_table[(BYTE_CLASS(b) * kNumOfStates) + (cur)])

	utf8_state current = kSTART;

	const uint8_t *pt = (const uint8_t *)pTest;
	const uint8_t * const end = pt + nLength;

	while (pt < end) {
		current = NEXT_STATE(*pt, current);
		if (kERROR == current) {
			break;
		}
		++pt;
	}

	return current == kSTART;
}
#endif

// https://github.com/zwegner/faster-utf8-validator
// faster-utf8-validator
// Copyright (c) 2019 Zach Wegner
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// How this validator works:
//
//   [[[ UTF-8 refresher: UTF-8 encodes text in sequences of "code points",
//   each one from 1-4 bytes. For each code point that is longer than one byte,
//   the code point begins with a unique prefix that specifies how many bytes
//   follow. All bytes in the code point after this first have a continuation
//   marker. All code points in UTF-8 will thus look like one of the following
//   binary sequences, with x meaning "don't care":
//      1 byte:  0xxxxxxx
//      2 bytes: 110xxxxx  10xxxxxx
//      3 bytes: 1110xxxx  10xxxxxx  10xxxxxx
//      4 bytes: 11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
//   ]]]
//
// This validator works in two basic steps: checking continuation bytes, and
// handling special cases. Each step works on one vector's worth of input
// bytes at a time.
//
// The continuation bytes are handled in a fairly straightforward manner in
// the scalar domain. A mask is created from the input byte vector for each
// of the highest four bits of every byte. The first mask allows us to quickly
// skip pure ASCII input vectors, which have no bits set. The first and
// (inverted) second masks together give us every continuation byte (10xxxxxx).
// The other masks are used to find prefixes of multi-byte code points (110,
// 1110, 11110). For these, we keep a "required continuation" mask, by shifting
// these masks 1, 2, and 3 bits respectively forward in the byte stream. That
// is, we take a mask of all bytes that start with 11, and shift it left one
// bit forward to get the mask of all the first continuation bytes, then do the
// same for the second and third continuation bytes. Here's an example input
// sequence along with the corresponding masks:
//
//   bytes:        61 C3 80 62 E0 A0 80 63 F0 90 80 80 00
//   code points:  61|C3 80|62|E0 A0 80|63|F0 90 80 80|00
//   # of bytes:   1 |2  - |1 |3  -  - |1 |4  -  -  - |1
//   cont. mask 1: -  -  1  -  -  1  -  -  -  1  -  -  -
//   cont. mask 2: -  -  -  -  -  -  1  -  -  -  1  -  -
//   cont. mask 3: -  -  -  -  -  -  -  -  -  -  -  1  -
//   cont. mask *: 0  0  1  0  0  1  1  0  0  1  1  1  0
//
// The final required continuation mask is then compared with the mask of
// actual continuation bytes, and must match exactly in valid UTF-8. The only
// complication in this step is that the shifted masks can cross vector
// boundaries, so we need to keep a "carry" mask of the bits that were shifted
// past the boundary in the last loop iteration.
//
// Besides the basic prefix coding of UTF-8, there are several invalid byte
// sequences that need special handling. These are due to three factors:
// code points that could be described in fewer bytes, code points that are
// part of a surrogate pair (which are only valid in UTF-16), and code points
// that are past the highest valid code point U+10FFFF.
//
// All of the invalid sequences can be detected by independently observing
// the first three nibbles of each code point. Since AVX2 can do a 4-bit/16-byte
// lookup in parallel for all 32 bytes in a vector, we can create bit masks
// for all of these error conditions, look up the bit masks for the three
// nibbles for all input bytes, and AND them together to get a final error mask,
// that must be all zero for valid UTF-8. This is somewhat complicated by
// needing to shift the error masks from the first and second nibbles forward in
// the byte stream to line up with the third nibble.
//
// We have these possible values for valid UTF-8 sequences, broken down
// by the first three nibbles:
//
//   1st   2nd   3rd   comment
//   0..7  0..F        ASCII
//   8..B  0..F        continuation bytes
//   C     2..F  8..B  C0 xx and C1 xx can be encoded in 1 byte
//   D     0..F  8..B  D0..DF are valid with a continuation byte
//   E     0     A..B  E0 8x and E0 9x can be encoded with 2 bytes
//         1..C  8..B  E1..EC are valid with continuation bytes
//         D     8..9  ED Ax and ED Bx correspond to surrogate pairs
//         E..F  8..B  EE..EF are valid with continuation bytes
//   F     0     9..B  F0 8x can be encoded with 3 bytes
//         1..3  8..B  F1..F3 are valid with continuation bytes
//         4     8     F4 8F BF BF is the maximum valid code point
//
// That leaves us with these invalid sequences, which would otherwise fit
// into UTF-8's prefix encoding. Each of these invalid sequences needs to
// be detected separately, with their own bits in the error mask.
//
//   1st   2nd   3rd   error bit
//   C     0..1  0..F  0x01
//   E     0     8..9  0x02
//         D     A..B  0x04
//   F     0     0..8  0x08
//         4     9..F  0x10
//         5..F  0..F  0x20
//
// For every possible value of the first, second, and third nibbles, we keep
// a lookup table that contains the bitwise OR of all errors that that nibble
// value can cause. For example, the first nibble has zeroes in every entry
// except for C, E, and F, and the third nibble lookup has the 0x21 bits in
// every entry, since those errors don't depend on the third nibble. After
// doing a parallel lookup of the first/second/third nibble values for all
// bytes, we AND them together. Only when all three have an error bit in common
// do we fail validation.

#if NP2_USE_AVX2
#if defined(__GNUC__) || defined(__clang__)
__attribute__((__always_inline__)) static inline
#else
static __forceinline
#endif
bool z_validate_vec_avx2(__m256i bytes, __m256i shifted_bytes, uint32_t *last_cont) {
	// Error lookup tables for the first, second, and third nibbles
	// Simple macro to make a vector lookup table for use with vpshufb. Since
	// AVX2 is two 16-byte halves, we duplicate the input values.
#define V_TABLE_16(...)		_mm256_setr_epi8(__VA_ARGS__, __VA_ARGS__)
	const __m256i error_1 = V_TABLE_16(
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x06, 0x38
	);
	const __m256i error_2 = V_TABLE_16(
		0x0B, 0x01, 0x00, 0x00,
		0x10, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x24, 0x20, 0x20
	);
	const __m256i error_3 = V_TABLE_16(
		0x29, 0x29, 0x29, 0x29,
		0x29, 0x29, 0x29, 0x29,
		0x2B, 0x33, 0x35, 0x35,
		0x31, 0x31, 0x31, 0x31
	);
#undef V_TABLE_16

	// Quick skip for ascii-only input. If there are no bytes with the high bit
	// set, we don't need to do any more work. We return either valid or
	// invalid based on whether we expected any continuation bytes here.
	const uint32_t high = _mm256_movemask_epi8(bytes);
	if (!high) {
		return *last_cont == 0;
	}

	// Which bytes are required to be continuation bytes
	uint64_t req = *last_cont;

	// Compute the continuation byte mask by finding bytes that start with
	// 11x, 111x, and 1111. For each of these prefixes, we get a bitmask
	// and shift it forward by 1, 2, or 3. This loop should be unrolled by
	// the compiler, and the (n == 1) branch inside eliminated.
	uint32_t set = high;
	set &= _mm256_movemask_epi8(_mm256_slli_epi16(bytes, 1));
	// A bitmask of the actual continuation bytes in the input
	// Mark continuation bytes: those that have the high bit set but
	// not the next one
	const uint32_t cont = high ^ set;

	// We add the shifted mask here instead of ORing it, which would
	// be the more natural operation, so that this line can be done
	// with one lea. While adding could give a different result due
	// to carries, this will only happen for invalid UTF-8 sequences,
	// and in a way that won't cause it to pass validation. Reasoning:
	// Any bits for required continuation bytes come after the bits
	// for their leader bytes, and are all contiguous. For a carry to
	// happen, two of these bit sequences would have to overlap. If
	// this is the case, there is a leader byte before the second set
	// of required continuation bytes (and thus before the bit that
	// will be cleared by a carry). This leader byte will not be
	// in the continuation mask, despite being required. QEDish.
	req += (uint64_t)set << 1;
	set &= _mm256_movemask_epi8(_mm256_slli_epi16(bytes, 2));
	req += (uint64_t)set << 2;
	set &= _mm256_movemask_epi8(_mm256_slli_epi16(bytes, 3));
	req += (uint64_t)set << 3;

	// Check that continuation bytes match. We must cast req from uint64_t
	// (which holds the carry mask in the upper half) to uint32_t, which
	// zeroes out the upper bits
	if (cont != (uint32_t)req) {
		return false;
	}

	// Look up error masks for three consecutive nibbles.
	const __m256i nibbles = _mm256_set1_epi8(0x0F);
	const __m256i e_1 = _mm256_shuffle_epi8(error_1, _mm256_and_si256(_mm256_srli_epi16(shifted_bytes, 4), nibbles));
	const __m256i e_2 = _mm256_shuffle_epi8(error_2, _mm256_and_si256(shifted_bytes, nibbles));
	const __m256i e_3 = _mm256_shuffle_epi8(error_3, _mm256_and_si256(_mm256_srli_epi16(bytes, 4), nibbles));

	// Check if any bits are set in all three error masks
	if (!_mm256_testz_si256(_mm256_and_si256(e_1, e_2), e_3)) {
		return false;
	}

	// Save continuation bits and input bytes for the next round
	*last_cont = req >> sizeof(__m256i);
	return true;
}

static inline bool z_validate_utf8_avx2(const char *data, uint32_t len) {
	// Keep continuation bits from the previous iteration that carry over to
	// each input chunk vector
	uint32_t last_cont = 0;

	uint32_t offset = 0;
	// Deal with the input up until the last section of bytes
	if (len >= sizeof(__m256i)) {
		// We need a vector of the input byte stream shifted forward one byte.
		// Since we don't want to read the memory before the data pointer
		// (which might not even be mapped), for the first chunk of input just
		// use vector instructions.
		__m256i shifted_bytes = _mm256_loadu_si256((__m256i *)data);
		//__m256i shl_16 = _mm256_permute2x128_si256(shifted_bytes, _mm256_setzero_si256(), 0x03);
		//shifted_bytes = _mm256_alignr_epi8(shifted_bytes, shl_16, 15);
		shifted_bytes = _mm256_slli_si256(shifted_bytes, 1);

		// Loop over input in sizeof(__m256i)-byte chunks, as long as we can safely read
		// that far into memory
		for (; offset + sizeof(__m256i) < len; offset += sizeof(__m256i)) {
			const __m256i bytes = _mm256_loadu_si256((__m256i *)(data + offset));
			if (!z_validate_vec_avx2(bytes, shifted_bytes, &last_cont)) {
				return false;
			}
			shifted_bytes = _mm256_loadu_si256((__m256i *)(data + offset + sizeof(__m256i) - 1));
		}
	}

	// Deal with any bytes remaining. Rather than making a separate scalar path,
	// just fill in a buffer, reading bytes only up to len, and load from that.
	if (offset < len) {
		uint8_t buffer[sizeof(__m256i) + 1];
		_mm256_storeu_si256((__m256i *)buffer, _mm256_setzero_si256());
		buffer[sizeof(__m256i)] = 0;

		if (offset != 0) {
			buffer[0] = data[offset - 1];
		}
		__movsb(buffer + 1, (const uint8_t *)(data + offset), len - offset);

		const __m256i shifted_bytes = _mm256_loadu_si256((__m256i *)buffer);
		const __m256i bytes = _mm256_loadu_si256((__m256i *)(buffer + 1));
		if (!z_validate_vec_avx2(bytes, shifted_bytes, &last_cont)) {
			return false;
		}
	}

	// The input is valid if we don't have any more expected continuation bytes
	return last_cont == 0;
}

// end NP2_USE_AVX2
#elif NP2_USE_SSE2
#if defined(__clang__)
#include <tmmintrin.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
__attribute__((__target__("ssse3"), __always_inline__)) static inline
#else
static __forceinline
#endif
bool z_validate_vec_sse4(__m128i bytes, __m128i shifted_bytes, uint32_t *last_cont) {
	// Error lookup tables for the first, second, and third nibbles
	const __m128i error_1 = _mm_setr_epi8(
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x06, 0x38
	);
	const __m128i error_2 = _mm_setr_epi8(
		0x0B, 0x01, 0x00, 0x00,
		0x10, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x24, 0x20, 0x20
	);
	const __m128i error_3 = _mm_setr_epi8(
		0x29, 0x29, 0x29, 0x29,
		0x29, 0x29, 0x29, 0x29,
		0x2B, 0x33, 0x35, 0x35,
		0x31, 0x31, 0x31, 0x31
	);

	// Quick skip for ascii-only input. If there are no bytes with the high bit
	// set, we don't need to do any more work. We return either valid or
	// invalid based on whether we expected any continuation bytes here.
	const uint32_t high = _mm_movemask_epi8(bytes);
	if (!high) {
		return *last_cont == 0;
	}

	// Which bytes are required to be continuation bytes
	uint32_t req = *last_cont;

	// Compute the continuation byte mask by finding bytes that start with
	// 11x, 111x, and 1111. For each of these prefixes, we get a bitmask
	// and shift it forward by 1, 2, or 3. This loop should be unrolled by
	// the compiler, and the (n == 1) branch inside eliminated.
	uint32_t set = high;
	set &= _mm_movemask_epi8(_mm_slli_epi16(bytes, 1));
	// A bitmask of the actual continuation bytes in the input
	// Mark continuation bytes: those that have the high bit set but
	// not the next one
	const uint32_t cont = high ^ set;
	// We add the shifted mask here instead of ORing it, which would
	// be the more natural operation, so that this line can be done
	// with one lea. While adding could give a different result due
	// to carries, this will only happen for invalid UTF-8 sequences,
	// and in a way that won't cause it to pass validation. Reasoning:
	// Any bits for required continuation bytes come after the bits
	// for their leader bytes, and are all contiguous. For a carry to
	// happen, two of these bit sequences would have to overlap. If
	// this is the case, there is a leader byte before the second set
	// of required continuation bytes (and thus before the bit that
	// will be cleared by a carry). This leader byte will not be
	// in the continuation mask, despite being required. QEDish.
	req += set << 1;
	set &= _mm_movemask_epi8(_mm_slli_epi16(bytes, 2));
	req += set << 2;
	set &= _mm_movemask_epi8(_mm_slli_epi16(bytes, 3));
	req += set << 3;

	// Check that continuation bytes match. We must cast req from uint32_t
	// (which holds the carry mask in the upper half) to uint16_t, which
	// zeroes out the upper bits
	if (cont != (uint16_t)req) {
		return false;
	}

	// Look up error masks for three consecutive nibbles.
	const __m128i nibbles = _mm_set1_epi8(0x0F);
	const __m128i e_1 = _mm_shuffle_epi8(error_1, _mm_and_si128(_mm_srli_epi16(shifted_bytes, 4), nibbles));
	const __m128i e_2 = _mm_shuffle_epi8(error_2, _mm_and_si128(shifted_bytes, nibbles));
	__m128i e_3 = _mm_shuffle_epi8(error_3, _mm_and_si128(_mm_srli_epi16(bytes, 4), nibbles));

	// Check if any bits are set in all three error masks
#if defined(__SSE4_1__)
	if (!_mm_testz_si128(_mm_and_si128(e_1, e_2), e_3)) {
		return false;
	}
#else
	e_3 = _mm_and_si128(_mm_and_si128(e_1, e_2), e_3);
	const int mask = _mm_movemask_epi8(_mm_cmpeq_epi8(e_3, _mm_setzero_si128()));
	if (mask != 0xFFFF) {
		return false;
	}
#endif

	// Save continuation bits and input bytes for the next round
	*last_cont = req >> sizeof(__m128i);
	return true;
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__((__target__("ssse3")))
#endif
static inline bool z_validate_utf8_sse4(const char *data, uint32_t len) {
	// Keep continuation bits from the previous iteration that carry over to
	// each input chunk vector
	uint32_t last_cont = 0;

	uint32_t offset = 0;
	// Deal with the input up until the last section of bytes
	if (len >= sizeof(__m128i)) {
		// We need a vector of the input byte stream shifted forward one byte.
		// Since we don't want to read the memory before the data pointer
		// (which might not even be mapped), for the first chunk of input just
		// use vector instructions.
		__m128i shifted_bytes = _mm_loadu_si128((__m128i *)data);
		//shifted_bytes = _mm_alignr_epi8(shifted_bytes, _mm_setzero_si128(), 15);
		shifted_bytes = _mm_slli_si128(shifted_bytes, 1);

		// Loop over input in sizeof(__m128i)-byte chunks, as long as we can safely read
		// that far into memory
		for (; offset + sizeof(__m128i) < len; offset += sizeof(__m128i)) {
			const __m128i bytes = _mm_loadu_si128((__m128i *)(data + offset));
			if (!z_validate_vec_sse4(bytes, shifted_bytes, &last_cont)) {
				return false;
			}
			shifted_bytes = _mm_loadu_si128((__m128i *)(data + offset + sizeof(__m128i) - 1));
		}
	}

	// Deal with any bytes remaining. Rather than making a separate scalar path,
	// just fill in a buffer, reading bytes only up to len, and load from that.
	if (offset < len) {
		uint8_t buffer[sizeof(__m128i) + 1];
		_mm_storeu_ps((float *)buffer, _mm_setzero_ps());
		buffer[sizeof(__m128i)] = 0;

		if (offset != 0) {
			buffer[0] = data[offset - 1];
		}
		__movsb(buffer + 1, (const uint8_t *)(data + offset), len - offset);

		const __m128i shifted_bytes = _mm_loadu_si128((__m128i *)(buffer));
		const __m128i bytes = _mm_loadu_si128((__m128i *)(buffer + 1));
		if (!z_validate_vec_sse4(bytes, shifted_bytes, &last_cont)) {
			return false;
		}
	}

	// The input is valid if we don't have any more expected continuation bytes
	return last_cont == 0;
}

static inline int did_cpu_supports_ssse3(void) {
	int info[4] = {0};
	__cpuid(info, 0x00000001);
	return info[2] & 0x0000200;
}
// end NP2_USE_SSE2
#endif

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See https://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

bool IsUTF8(const char *pTest, DWORD nLength) {
#if 0
	StopWatch watch;
	StopWatch_Start(watch);
#endif

#if NP2_USE_AVX2
	const bool result = z_validate_utf8_avx2(pTest, nLength);
#if 0
	StopWatch_Stop(watch);
	StopWatch_ShowLog(&watch, "UTF8 time");
#endif
	return result;
	// end NP2_USE_AVX2
#else

#if NP2_USE_SSE2
	if (did_cpu_supports_ssse3()) {
		const bool result = z_validate_utf8_sse4(pTest, nLength);
#if 0
		StopWatch_Stop(watch);
		StopWatch_ShowLog(&watch, "UTF8 time");
#endif
		return result;
	}
#endif // NP2_USE_SSE2

	enum {
		UTF8_ACCEPT = 0,
		UTF8_REJECT = 12,
	};

	static const uint8_t utf8_dfa[] = {
		// The first part of the table maps bytes to character classes that
		// to reduce the size of the transition table and create bitmasks.
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		 8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

		// The second part is a transition table that maps a combination
		// of a state of the automaton and a character class to a state.
		 0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
		12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
		12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
		12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
		12,36,12,12,12,12,12,12,12,12,12,12,
	};

	const uint8_t *pt = (const uint8_t *)pTest;
	const uint8_t * const end = pt + nLength;
	UINT state = UTF8_ACCEPT;

#if 0 // NP2_USE_AVX2
	while (pt + sizeof(__m256i) <= end) {
		const __m256i chunk = _mm256_loadu_si256((__m256i *)pt);
		const uint32_t mask = _mm256_movemask_epi8(chunk);
		if (mask) {
			// skip leading and trailing ASCII
			const DWORD trailing = (state != UTF8_ACCEPT)? 0 : np2_ctz(mask);
			DWORD leading;
			_BitScanReverse(&leading, mask);

			const uint8_t *temp = pt + trailing;
			const uint8_t * const endPtr = pt + leading + 1;
			do {
				state = utf8_dfa[256 + state + utf8_dfa[*temp++]];
			} while (temp < endPtr);
			if (state == UTF8_REJECT || (leading != 31 && state != UTF8_ACCEPT)) {
				return false;
			}
		} else if (state != UTF8_ACCEPT) {
			return false;
		}
		pt += sizeof(__m256i);
	}
	// end NP2_USE_AVX2
#elif NP2_USE_SSE2
	while (pt + 2*sizeof(__m128i) <= end) {
		const __m128i chunk1 = _mm_loadu_si128((__m128i *)pt);
		const __m128i chunk2 = _mm_loadu_si128((__m128i *)(pt + sizeof(__m128i)));
		const uint32_t mask = _mm_movemask_epi8(chunk1)
			| (((uint32_t)_mm_movemask_epi8(chunk2)) << sizeof(__m128i));
		if (mask) {
			// skip leading and trailing ASCII
			const DWORD trailing = (state != UTF8_ACCEPT)? 0 : np2_ctz(mask);
			DWORD leading;
			_BitScanReverse(&leading, mask);

			const uint8_t *temp = pt + trailing;
			const uint8_t * const endPtr = pt + leading + 1;
			do {
				state = utf8_dfa[256 + state + utf8_dfa[*temp++]];
			} while (temp < endPtr);
			if (state == UTF8_REJECT || (leading != 31 && state != UTF8_ACCEPT)) {
				return false;
			}
		} else if (state != UTF8_ACCEPT) {
			return false;
		}
		pt += 2*sizeof(__m128i);
	}
	// end NP2_USE_SSE2
#elif defined(_WIN64)
	while (pt + sizeof(uint64_t) <= end) {
		const uint64_t val = *((const uint64_t *)pt);
		if (val & UINT64_C(0x8080808080808080)) {
#if 0
			state = utf8_dfa[256 + state + utf8_dfa[pt[0]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[1]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[2]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[3]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[4]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[5]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[6]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[7]]];
#else
			// same as above
			state = utf8_dfa[256 + state + utf8_dfa[val & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 8) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 16) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 24) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 32) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 40) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 48) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[val >> 56]];
#endif
			if (state == UTF8_REJECT) {
				return false;
			}
		} else if (state != UTF8_ACCEPT) {
			return false;
		}
		pt += sizeof(uint64_t);
	}
	// end _WIN64
#else
	while (pt + sizeof(uint32_t) <= end) {
		const uint32_t val = *((const uint32_t *)pt);
		if (val & 0x80808080U) {
#if 0
			state = utf8_dfa[256 + state + utf8_dfa[pt[0]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[1]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[2]]];
			state = utf8_dfa[256 + state + utf8_dfa[pt[3]]];
#else
			// same as above
			state = utf8_dfa[256 + state + utf8_dfa[val & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 8) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[(val >> 16) & 255]];
			state = utf8_dfa[256 + state + utf8_dfa[val >> 24]];
#endif
			if (state == UTF8_REJECT) {
				return false;
			}
		} else if (state != UTF8_ACCEPT) {
			return false;
		}
		pt += sizeof(uint32_t);
	}
	// end _WIN32
#endif

	while (pt < end) {
		state = utf8_dfa[256 + state + utf8_dfa[*pt++]];
	}

#if 0
	StopWatch_Stop(watch);
	StopWatch_ShowLog(&watch, "UTF8 time");
#endif

	return state == UTF8_ACCEPT;
#endif // !NP2_USE_AVX2
}

static const char* CheckUTF7(const char *pTest, DWORD nLength) {
	const char *pt = pTest;
#if NP2_USE_AVX2
	if (nLength >= 2*sizeof(__m256i)) {
		const char * const end = pt + nLength - 2*sizeof(__m256i);
		do {
			const __m256i chunk1 = _mm256_loadu_si256((__m256i *)pt);
			const __m256i chunk2 = _mm256_loadu_si256((__m256i *)(pt + sizeof(__m256i)));
			const uint32_t mask = _mm256_movemask_epi8(_mm256_or_si256(chunk1, chunk2));
			if (mask) {
				return pt;
			}
			pt += 2*sizeof(__m256i);
		} while (pt <= end);
	}

	nLength &= 2*sizeof(__m256i) - 1;
	if (nLength != 0) {
		uint32_t mask = 0;
		__m256i chunk = _mm256_loadu_si256((__m256i *)pt);
		uint32_t last = _mm256_movemask_epi8(chunk);
		if (nLength > sizeof(__m256i)) {
			nLength &= sizeof(__m256i) - 1;
			mask = last;
			chunk = _mm256_loadu_si256((__m256i *)(pt + sizeof(__m256i)));
			last = _mm256_movemask_epi8(chunk);
		}
		mask |= bit_zero_high_u32(last, nLength);
		return mask ? pt : NULL;
	}
	return NULL;
	// end NP2_USE_AVX2
#elif NP2_USE_SSE2
	if (nLength >= 4*sizeof(__m128i)) {
		const char * const end = pt + nLength - 4*sizeof(__m128i);
		do {
			const __m128i chunk1 = _mm_loadu_si128((__m128i *)pt);
			const __m128i chunk2 = _mm_loadu_si128((__m128i *)(pt + sizeof(__m128i)));
			const __m128i chunk3 = _mm_loadu_si128((__m128i *)(pt + 2*sizeof(__m128i)));
			const __m128i chunk4 = _mm_loadu_si128((__m128i *)(pt + 3*sizeof(__m128i)));
			const uint32_t mask = _mm_movemask_epi8(_mm_or_si128(_mm_or_si128(_mm_or_si128(chunk1, chunk2), chunk3), chunk4));
			if (mask) {
				return pt;
			}
			pt += 4*sizeof(__m128i);
		} while (pt <= end);
	}

	nLength &= 4*sizeof(__m128i) - 1;
	if (nLength != 0) {
		const char * const end = pt;
		__m128i chunk = _mm_setzero_si128();
#if defined(__clang__)
		#pragma clang loop unroll(disable)
#endif
		for (UINT i = 0; i < nLength / sizeof(__m128i); i++) {
			chunk = _mm_or_si128(chunk, _mm_loadu_si128((__m128i *)pt));
			pt += sizeof(__m128i);
		}

		uint32_t mask = _mm_movemask_epi8(chunk);
		nLength &= sizeof(__m128i) - 1;
		if (nLength != 0) {
			const uint32_t last = _mm_movemask_epi8(_mm_loadu_si128((__m128i *)pt));
			mask |= bit_zero_high_u32(last, nLength);
		}
		return mask ? end : NULL;
	}
	return NULL;
	// end NP2_USE_SSE2
#elif defined(_WIN64)
	const char * const end = pt + nLength;
	nLength &= sizeof(uint64_t) - 1;
	nLength = (sizeof(uint64_t) - nLength)*8;
	do {
		uint64_t value = *((const uint64_t *)pt);
		pt += sizeof(uint64_t);
		value &= UINT64_C(0x8080808080808080);
		if (value != 0) {
			if (pt > end) {
				value <<= nLength;
				if (value == 0) {
					return NULL;
				}
			}
			return pt - sizeof(uint64_t);
		}
	} while (pt < end);
	return NULL;
	// end _WIN64
#else
	const char * const end = pt + nLength;
	nLength &= sizeof(uint32_t) - 1;
	nLength = (sizeof(uint32_t) - nLength)*8;
	do {
		uint32_t value = *((const uint32_t *)pt);
		pt += sizeof(uint32_t);
		value &= 0x80808080U;
		if (value != 0) {
			if (pt > end) {
				value <<= nLength;
				if (value == 0) {
					return NULL;
				}
			}
			return pt - sizeof(uint32_t);
		}
	} while (pt < end);
	return NULL;
	// end _WIN32
#endif
}

#if 0
/* byte length of UTF-8 sequence based on value of first byte.
	 for UTF-16 (21-bit space), max. code length is 4, so we only need to look
	 at 4 upper bits.
 */
static const INT utf8_lengths[16] = {
	1, 1, 1, 1, 1, 1, 1, 1, 	/* 0000 to 0111 : 1 byte (plain ASCII) */
	0, 0, 0, 0, 			/* 1000 to 1011 : not valid */
	2, 2, 				/* 1100, 1101 : 2 bytes */
	3, 					/* 1110 : 3 bytes */
	4					/* 1111 :4 bytes */
};

/*++
Function :
	UTF8_mbslen_bytes [INTERNAL]
	Calculates the byte size of a NULL-terminated UTF-8 string.
Parameters :
	char *utf8_string : string to examine
Return value :
	size (in bytes) of a NULL-terminated UTF-8 string.
	1 if invalid NULL-terminated UTF-8 string
--*/
INT UTF8_mbslen_bytes(LPCSTR utf8_string) {
	INT length = 0;
	const uint8_t *pt = (const uint8_t *)utf8_string;

	while (*pt) {
		INT code_size;
		const BYTE ch = *pt;

		if ((ch <= 0xF7) && (0 != (code_size = utf8_lengths[ ch >> 4 ]))) {
			length += code_size;
			pt += code_size;
		} else {
			/* we got an invalid byte value but need to count it,
				 it will be later ignored during the string conversion */
			//WARN("invalid first byte value 0x%02X in UTF-8 sequence!\n", byte);
			length++;
			pt++;
		}
	}
	length++; /* include NULL terminator */

	return length;
}

/*++
Function :
	UTF8_mbslen [INTERNAL]
	Calculates the character size of a NULL-terminated UTF-8 string.
Parameters :
	char *utf8_string : string to examine
	int byte_length : byte size of string
Return value :
	size (in characters) of a UTF-8 string.
	-1 if invalid UTF-8 string
--*/
INT UTF8_mbslen(LPCSTR source, INT byte_length) {
	INT wchar_length = 0;
	const uint8_t *pt = (const uint8_t *)utf8_string;

	while (byte_length > 0) {
		INT code_size;
		const BYTE ch = *pt;
		/* UTF-16 can't encode 5-byte and 6-byte sequences, so maximum value
			 for first byte is 11110111. Use lookup table to determine sequence
			 length based on upper 4 bits of first byte */
		if ((ch <= 0xF7) && (0 != (code_size = utf8_lengths[ ch >> 4]))) {
			/* 1 sequence == 1 character */
			wchar_length++;

			if (code_size == 4) {
				wchar_length++;
			}

			pt += code_size;				/* increment pointer */
			byte_length -= code_size;		/* decrement counter*/
		} else {
			/*
				 unlike UTF8_mbslen_bytes, we ignore the invalid characters.
				 we only report the number of valid characters we have encountered
				 to match the Windows behavior.
			*/
			//WARN("invalid byte 0x%02X in UTF-8 sequence, skipping it!\n",
			//		 byte);
			pt++;
			byte_length--;
		}
	}

	return wchar_length;
}
#endif


LPSTR RecodeAsUTF8(LPSTR lpData, DWORD *cbData, UINT codePage, DWORD flags) {
	LPWSTR lpDataWide = (LPWSTR)NP2HeapAlloc(*cbData * sizeof(WCHAR) + 16);
	const int cbDataWide = MultiByteToWideChar(codePage, flags, lpData, *cbData, lpDataWide, (int)(NP2HeapSize(lpDataWide) / sizeof(WCHAR)));
	if (cbDataWide) {
		lpData = (char *)NP2HeapAlloc(cbDataWide * kMaxMultiByteCount + 16);
		*cbData = WideCharToMultiByte(CP_UTF8, 0, lpDataWide, cbDataWide, lpData, (int)NP2HeapSize(lpData), NULL, NULL);
	} else {
		lpData = NULL;
		*cbData = 0;
	}
	NP2HeapFree(lpDataWide);
	return lpData;
}

int EditDetermineEncoding(LPCWSTR pszFile, char *lpData, DWORD cbData, int *encodingFlag) {
	// TODO: scheme default encoding
	LPCWSTR const pszExt = PathFindExtension(pszFile);
	int preferedEncoding = CPI_NONE;
	if (bLoadNFOasOEM && (StrCaseEqual(pszExt, L".nfo") || StrCaseEqual(pszExt, L".diz"))) {
		preferedEncoding = g_DOSEncoding;
	} else if (StrCaseEqual(pszExt, L".bat") || StrCaseEqual(pszExt, L".cmd")) {
		preferedEncoding = CPI_DEFAULT;
	} else if (StrEqual(pszExt, L".sh") || StrStartsWith(lpData, "#!/")) {
		// shell script: #!/bin/sh[LF]
		preferedEncoding = CPI_UTF8;
	} else if (StrCaseEqual(pszExt, L".vhd") || StrCaseEqual(pszExt, L".vhdl")) {
		// VHDL should use ISO-8859-1 8-bit encoding
		preferedEncoding = Encoding_GetIndex(1252);
	}

	int iEncoding = CPI_DEFAULT;
	// default encoding for empty file.
	if (cbData == 0) {
		FileVars_Init(NULL, 0, &fvCurFile);
		*encodingFlag = EncodingFlag_UTF7;
		if (iSrcEncoding >= CPI_FIRST) {
			iEncoding = iSrcEncoding;
		} else if (iWeakSrcEncoding >= CPI_FIRST) {
			iEncoding = iWeakSrcEncoding;
		} else if (preferedEncoding >= CPI_FIRST) {
			iEncoding = preferedEncoding;
		} else if (bLoadASCIIasUTF8) {
			iEncoding = CPI_UTF8;
		} else {
			iEncoding = iDefaultEncoding;
		}
		return iEncoding;
	}

	// check Unicode / UTF-16 BOM
	const UINT bom = *((const uint16_t *)lpData);
	if (cbData < MAX_NON_UTF8_SIZE && (Encoding_IsUnicode(iSrcEncoding) // reload as UTF-16
		|| (iSrcEncoding < CPI_FIRST && (cbData & 1) == 0 && (bom == BOM_UTF16LE || bom == BOM_UTF16BE)))) {
		bool bBOM = iSrcEncoding < CPI_FIRST;
		bool bReverse = bom == BOM_UTF16BE;
		if (iSrcEncoding == CPI_UNICODE) {
			bBOM = bom == BOM_UTF16LE;
			bReverse = false;
		} else if (iSrcEncoding == CPI_UNICODEBE) {
			bBOM = bom == BOM_UTF16BE;
			bReverse = true;
		}

		NP2_static_assert(CPI_UNICODE + 1 == CPI_UNICODEBE);
		NP2_static_assert(CPI_UNICODEBOM + 2 == CPI_UNICODE);
		NP2_static_assert(CPI_UNICODEBEBOM + 2 == CPI_UNICODEBE);
		iEncoding = CPI_UNICODE + bReverse;
		iEncoding -= bBOM << 1;
		return iEncoding;
	}

	FileVars_Init(lpData, cbData, &fvCurFile);
	// check UTF-8 BOM
	const bool utf8Sig = IsUTF8Signature(lpData);
	if (Encoding_IsUTF8(iSrcEncoding) // reload as UTF-8
		|| (iSrcEncoding < CPI_FIRST && utf8Sig)) {
		NP2_static_assert(CPI_UTF8 + 1 == CPI_UTF8SIGN);
		iEncoding = CPI_UTF8 + utf8Sig;
		return iEncoding;
	}

	// file large than 2 GiB is loaded without encoding conversion, i.e. loaded as UTF-8 or ANSI only.
	if (cbData >= MAX_NON_UTF8_SIZE) {
		if (iSrcEncoding != CPI_DEFAULT && (utf8Sig || IsUTF8(lpData, cbData))) {
			iEncoding = CPI_UTF8 + utf8Sig;
		}
		return iEncoding;
	}

	// check UTF-16 without BOM for Latin
	if ((cbData & 1) == 0 && iSrcEncoding < CPI_FIRST
		// odd or even byte is lower C0 control character U+0000 to U+0007
		&& ((bom & 0xF800) == 0 || (bom & 0x00F8) == 0)
		&& fvCurFile.mask == 0) {
#if 0
		// Basic Latin and Latin-1: U+0000 to U+00FF
		iEncoding = DetectUTF16Latin1(lpData, cbData);
		if (iEncoding != CPI_DEFAULT) {
			return iEncoding;
		}
#endif
		// Latin Extended-A U+0100 to NKo U+07FF
		iEncoding = DetectUTF16LatinExt(lpData, cbData);
		if (iEncoding != CPI_DEFAULT) {
			return iEncoding;
		}
	}

	// treat as unreliable encoding declaration as we don't follow strict parse rules.
	const int sniffedEncoding = FileVars_GetEncoding(&fvCurFile);
	// check 7-bit ASCII
	const char * const multiData = CheckUTF7(lpData, cbData);
	if (multiData == NULL) {
		// 7-bit / any encoding, similar to empty file
		*encodingFlag = EncodingFlag_UTF7;
		if (iSrcEncoding >= CPI_FIRST) {
			iEncoding = iSrcEncoding;
		} else if (iWeakSrcEncoding >= CPI_FIRST && !Encoding_IsUnicode(iWeakSrcEncoding)) {
			iEncoding = iWeakSrcEncoding;
		} else if (preferedEncoding >= CPI_FIRST) {
			iEncoding = preferedEncoding;
		} else if (sniffedEncoding >= CPI_FIRST) {
			iEncoding = sniffedEncoding;
			const UINT codePage = mEncoding[iEncoding].uCodePage;
			const UINT page = GetEncodingAlias(codePage);
			if (codePage != page) {
				iEncoding = Encoding_GetIndex(page);
			}
		} else if (bLoadASCIIasUTF8) {
			iEncoding = CPI_UTF8;
		} else if (!Encoding_IsUnicode(iDefaultEncoding)) {
			iEncoding = iDefaultEncoding;
		}
		return (iEncoding == CPI_UTF8SIGN) ? CPI_UTF8 : iEncoding;
	}

	// reload with specific encoding
	if (iSrcEncoding >= CPI_FIRST && (mEncoding[iSrcEncoding].uFlags & (NCP_8BIT | NCP_DEFAULT)) != 0) {
		return iSrcEncoding;
	}

	// avoid validating initial ASCII for multi-byte encoding
	const DWORD multiLen = (DWORD)(lpData + cbData - multiData);
	//printf("%s initial ASCII: %u=%u - %u\n", __func__, (unsigned)(cbData - multiLen), (unsigned)cbData, (unsigned)multiLen);
	// prefer UTF-8 when no encoding specified
	if (IsUTF8(multiData, multiLen)) {
		return CPI_UTF8;
	}

	// brute force test other encoding
	const int encodings[4] = {
		// auto reload with current encoding
		iWeakSrcEncoding,
		// fallback encoding
		preferedEncoding,
		sniffedEncoding,
		iDefaultEncoding,
	};
	for (int i = 0; i < (int)COUNTOF(encodings); i++) {
		iEncoding = encodings[i];
		if (iEncoding > CPI_DEFAULT && (mEncoding[iEncoding].uFlags & NCP_8BIT) != 0) {
			const UINT codePage = mEncoding[iEncoding].uCodePage;
			if (IsValidMultiByte(codePage, multiData, multiLen)) {
				if (i == 2) {
					const UINT page = GetEncodingAlias(codePage);
					if (page != codePage && IsValidMultiByte(page, multiData, multiLen)) {
						iEncoding = Encoding_GetIndex(page);
					}
				}
				return iEncoding;
			}
		}
	}

	// test system ANSI code page
	iEncoding = CPI_DEFAULT;
	const UINT acp = GetACP();
	if (acp == CP_UTF8 || !IsValidMultiByte(acp, multiData, multiLen)) {
		*encodingFlag = EncodingFlag_Invalid;
		// check UTF-16 without BOM
		if ((cbData & 1) == 0 && fvCurFile.mask == 0) {
			iEncoding = DetectUnicode(lpData, cbData, bSkipUnicodeDetection);
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			if (iEncoding == CPI_UNICODEBE) {
				*encodingFlag = EncodingFlag_Reversed;
			}
#endif
		}
	}
	// detect binary file
	if (iEncoding == CPI_DEFAULT && MaybeBinaryFile((const uint8_t *)lpData, cbData)) {
		*encodingFlag = EncodingFlag_Binary;
	}

	// unknown encoding
	return iEncoding;
}

//case++Autogenerated -- start of section automatically generated
// Created with Python 3.12.0, Unicode 15.0.0
#define kUnicodeCaseSensitiveFirst	0x0600U
#define kUnicodeCaseSensitiveMax	0x1e943U

static const uint8_t UnicodeCaseSensitivityIndex[] = {
// block index
0, 0, 0, 0, 0, 0, 0, 0, 152, 153, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 0, 0,
0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 157, 0, 0, 0, 0, 0, 0, 0, 3, 4, 5, 6, 7, 8, 9,
0, 0, 10, 11, 0, 0, 0, 0, 0, 12, 0, 0, 80, 81, 82, 83, 0, 0, 255, 0, 0, 0, 84, 85, 13, 14, 15, 0, 190, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 118, 0, 0, 0, 0, 0, 0, 0, 119, 0,
// mask index
0, 8, 48, 49, 0, 8, 8, 50, 51, 49, 0, 0, 0, 0, 0, 52, 53, 0, 0, 0, 8, 8, 8, 8, 54, 8, 8, 8, 50, 8, 55, 56,
8, 57, 58, 59, 0, 60, 53, 8, 61, 0, 0, 0, 0, 62, 8, 63, 8, 8, 8, 64, 8, 8, 8, 65, 8, 66, 0, 0, 0, 0, 8, 67,
68, 0, 0, 0, 0, 69, 8, 70, 71, 72, 73, 74, 0, 0, 75, 76, 8, 8, 0, 0, 77, 0, 0, 0, 0, 2, 2, 0, 8, 8, 78, 0,
0, 76, 79, 68, 0, 0, 0, 80, 81, 82, 0, 0, 8, 83, 8, 83, 0, 8, 8, 0, 0, 0, 8, 8, 8, 8, 84, 0,
};

static const uint32_t UnicodeCaseSensitivityMask[] = {
0x00000000U, 0x00000000U, 0x07fffffeU, 0x07fffffeU, 0x00000000U, 0x00200000U, 0xff7fffffU, 0xff7fffffU,
0xffffffffU, 0xfeffffffU, 0xffffffffU, 0xffffffffU, 0xf7ffdfffU, 0xb3fff3ffU, 0xfffffff0U, 0xffffffffU,
0xffffffffU, 0xfc0ffffdU, 0x1adfffffU, 0x20269f6bU, 0x60041f8dU, 0x00000000U, 0x00000000U, 0x00000000U,
0x00000000U, 0x00000000U, 0x00000020U, 0xb8cf0000U, 0xffffd740U, 0xfffffffbU, 0xffe3ffffU, 0xefbfffffU,
0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU, 0xfffffc03U, 0xffffffffU, 0xffffffffU, 0xffffffffU,
0xffffffffU, 0xfffeffffU, 0x007fffffU, 0xfffffffeU, 0x000000ffU, 0x00000000U, 0x00000000U, 0x00000000U,
0xffff20bfU, 0xe7ffffffU, 0x3f3fffffU, 0xffff01ffU, 0x22000000U, 0x00004000U, 0x4fffffffU, 0xaaff3f3fU,
0x3fffffffU, 0x5fdfffffU, 0x0fcf1fdcU, 0x1fdc1fffU, 0x00040c40U, 0x00000018U, 0xffc00000U, 0x000003ffU,
0xc06dffffU, 0x000c780fU, 0x000020bfU, 0x00003fffU, 0x0fffffffU, 0xfffcfffcU, 0xfe00ffffU, 0xffdf38ffU,
0xffff7fffU, 0x03c307ffU, 0x00600000U, 0x00080000U, 0xffff0000U, 0x00f8007fU, 0x0000ffffU, 0xff0fffffU,
0xf7ff0000U, 0xffb7f7ffU, 0x1bfbfffbU, 0x0007ffffU, 0x0000000fU,
};

// case sensitivity for ch in [kUnicodeCaseSensitiveFirst, kUnicodeCaseSensitiveMax]
static inline bool IsCharacterCaseSensitiveSecond(uint32_t ch) {
	uint32_t block = ch >> 7;
	uint32_t index = UnicodeCaseSensitivityIndex[block & 0x7f];
	block = index ^ (block >> 2);
	index &= ((block - 0x20) >> 8) & 0x1f;
	if (index) {
		ch = ch & 0x7f;
		index = 124 + (index << 2);
		index = UnicodeCaseSensitivityIndex[index + (ch >> 5)];
		return bittest(UnicodeCaseSensitivityMask + index, ch & 31);
	}
	return false;
}
//case--Autogenerated -- end of section automatically generated

bool IsStringCaseSensitiveW(LPCWSTR pszTextW) {
	if (StrIsEmpty(pszTextW)) {
		return false;
	}

	LPCWSTR ptr = pszTextW;
	uint32_t ch;
	while ((ch = *ptr++) != 0) {
		if (ch < kUnicodeCaseSensitiveFirst) {
			if (BitTestEx(UnicodeCaseSensitivityMask, ch)) {
				return true;
			}
		} else {
			if (IS_SURROGATE_PAIR(ch, *ptr)) {
				ch = UTF16_TO_UTF32(ch, *ptr);
				++ptr;
				if (ch > kUnicodeCaseSensitiveMax) {
					continue;
				}
			}
			if (IsCharacterCaseSensitiveSecond(ch)) {
				return true;
			}
		}
	}
	return false;
}

bool IsStringCaseSensitiveA(LPCSTR pszText) {
	if (StrIsEmptyA(pszText)) {
		return false;
	}

	const uint8_t *ptr = (const uint8_t *)pszText;
	uint8_t ch;
	while ((ch = *ptr) != 0) {
		if (ch & 0x80) {
			// convert to Unicode
			const UINT cpEdit = SciCall_GetCodePage();
			LPCSTR start = (LPCSTR)ptr;
			const size_t length = strlen(start) + 1;
			LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc(length * sizeof(WCHAR));
			MultiByteToWideChar(cpEdit, 0, start, -1, pszTextW, (int)(length));
			const bool result = IsStringCaseSensitiveW(pszTextW);
			NP2HeapFree(pszTextW);
			return result;
		}

		// ASCII
		ch = UnsafeLower(ch);
		if (ch >= 'a' && ch <= 'z') {
			return true;
		}
		++ptr;
	}
	return false;
}
