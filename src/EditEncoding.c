// Edit Encoding

#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Helpers.h"
#include "Dialogs.h"
#include "SciCall.h"
#include "resource.h"

extern BOOL bSkipUnicodeDetection;
extern int iDefaultCodePage;

int g_DOSEncoding;

// Supported Encodings
static WCHAR wchANSI[8];
static WCHAR wchOEM [8];
static LPWSTR g_AllEncodingLabel = NULL;

// https://docs.microsoft.com/en-us/windows/desktop/Intl/code-page-identifiers
// https://en.wikipedia.org/wiki/Windows_code_page
// https://en.wikipedia.org/wiki/ISO/IEC_8859
// https://encoding.spec.whatwg.org/

// Encoding
NP2ENCODING mEncoding[] = {
	{ NCP_DEFAULT | NCP_RECODE, 0, "ANSI,ANSI,ASCII,", 61000, NULL },
	{ NCP_8BIT | NCP_RECODE, 0, "OEM,OEM,", 61001, NULL },
	{ NCP_UNICODE | NCP_UNICODE_BOM, 0, "", 61002, NULL },
	{ NCP_UNICODE | NCP_UNICODE_REVERSE | NCP_UNICODE_BOM, 0, "", 61003, NULL },
	{ NCP_UNICODE | NCP_RECODE, 0, "UTF-16,UTF16,Unicode,", 61004, NULL },
	{ NCP_UNICODE | NCP_UNICODE_REVERSE | NCP_RECODE, 0, "UTF-16BE,UTF16BE,UnicodeBE,", 61005, NULL },
	{ NCP_UTF8 | NCP_RECODE, 0, "UTF-8,UTF8,", 61006, NULL },
	{ NCP_UTF8 | NCP_UTF8_SIGN, 0, "UTF-8,UTF8,", 61007, NULL },
	{ NCP_8BIT | NCP_RECODE, CP_UTF7, "UTF-7,UTF7,", 61008, NULL },
	//{ NCP_8BIT | NCP_RECODE, 708, ",ASMO-708,ASMO-708,", 0, NULL },// Arabic (ASMO 708); ISO 8859-6
	//{ NCP_8BIT | NCP_RECODE, 709, ",ASMO-709,ASMO-709,", 0, NULL },// Arabic (ASMO-449+, BCON V4)
	//{ NCP_8BIT | NCP_RECODE, 710, ",ASMO-710,ASMO-710,", 0, NULL },// Arabic - Transparent Arabic
	//{ NCP_8BIT | NCP_RECODE, 720, "DOS-720,DOS720,", 61009, NULL },// Arabic (Transparent ASMO); Arabic (DOS)
	{ NCP_8BIT | NCP_RECODE, 28596, "ISO-8859-6,ISO88596,arabic,csisolatinarabic,ECMA114,isoir127,", 61010, NULL },// Arabic (ISO 8859-6 Visual)
	{ NCP_8BIT | NCP_RECODE, 38596, "ISO-8859-6-I,ISO88596I,", 61011, NULL },// Arabic (ISO 8859-6-I Logical)
	//{ NCP_8BIT | NCP_RECODE, 864, "IBM864,IBM864,", 0, NULL },// OEM Arabic; Arabic (864)
	//{ NCP_8BIT | NCP_RECODE, 10004, "x-mac-arabic,xmacarabic,", 0, NULL },// Arabic (Mac)
	{ NCP_8BIT | NCP_RECODE, 1256, "Windows-1256,Windows1256,CP1256", 61012, NULL },// ANSI Arabic; Arabic (Windows)
	//{ NCP_8BIT | NCP_RECODE, 775, "IBM775,IBM775,CP500,", 61013, NULL },// OEM Baltic; Baltic (DOS)
	{ NCP_8BIT | NCP_RECODE, 28594, "ISO-8859-4,ISO88594,csisolatin4,isoir110,l4,Latin4,", 61014, NULL },// Baltic (ISO 8859-4); North European
	{ NCP_8BIT | NCP_RECODE, 1257, "Windows-1257,Windows1257,", 61015, NULL },// ANSI Baltic; Baltic (Windows)
	//{ NCP_8BIT | NCP_RECODE, 852, "ibm852,ibm852,cp852,", 61016, NULL },// OEM Latin-2; Central European (DOS)
	{ NCP_8BIT | NCP_RECODE, 28592, "ISO-8859-2,ISO88592,csisolatin2,isoir101,latin2,l2,", 61017, NULL },// Central European (ISO 8859-2); Central European (ISO)
	//{ NCP_8BIT | NCP_RECODE, 10029, "x-mac-ce,xmacce,", 61018, NULL },// MAC Latin-2; Central European (Mac)
	{ NCP_8BIT | NCP_RECODE, 1250, "Windows-1250,Windows1250,xcp1250,", 61019, NULL },// ANSI Central European; Central European (Windows)
	{ NCP_8BIT | NCP_RECODE, 28604, "ISO-8859-14,ISO885914,Windows-28604,Windows28604,", 61055, NULL },// Celtic (ISO 8859-14)
	{ NCP_8BIT | NCP_RECODE, 936, "GB2312,GB2312,chinese,cngb,csgb2312,csgb231280,gb231280,gbk,", 61020, NULL },// ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
	//{ NCP_8BIT | NCP_RECODE, 10008, "x-mac-chinesesimp,xmacchinesesimp,", 0, NULL },// MAC Simplified Chinese (GB 2312); Chinese Simplified (Mac)
	{ NCP_8BIT | NCP_RECODE, 950, "BIG5,BIG5,cnbig5,csbig5,xxbig5,", 61022, NULL },// ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
	//{ NCP_8BIT | NCP_RECODE, 10002, "x-mac-chinesetrad,xmacchinesetrad,", 0, NULL },// MAC Traditional Chinese (Big5); Chinese Traditional (Mac)
	//{ NCP_8BIT | NCP_RECODE, 10082, "x-mac-croatian,xmaccroatian,", 61024, NULL },// Croatian (Mac)
	//{ NCP_8BIT | NCP_RECODE, 866, "CP866,CP866,IBM866,", 61025, NULL },// OEM Russian; Cyrillic (DOS)
	{ NCP_8BIT | NCP_RECODE, 28595, "ISO-8859-5,ISO88595,csisolatin5,csisolatincyrillic,cyrillic,isoir144,", 61026, NULL },// Cyrillic (ISO 8859-5)
	{ NCP_8BIT | NCP_RECODE, 20866, "koi8-r,koi8r,cskoi8r,koi,koi8,", 61027, NULL },// Russian (KOI8-R); Cyrillic (KOI8-R)
	//{ NCP_8BIT | NCP_RECODE, 855, "ibm855,ibm855,", 0, NULL },// OEM Cyrillic (primarily Russian)
	{ NCP_8BIT | NCP_RECODE, 21866, "koi8-u,koi8u,koi8ru,", 61028, NULL },// Ukrainian (KOI8-U); Cyrillic (KOI8-U)
	//{ NCP_8BIT | NCP_RECODE, 10007, "x-mac-cyrillic,xmaccyrillic,", 61029, NULL },// Cyrillic (Mac)
	{ NCP_8BIT | NCP_RECODE, 1251, "Windows-1251,Windows1251,XCP1251,", 61030, NULL },// ANSI Cyrillic; Cyrillic (Windows)
	{ NCP_8BIT | NCP_RECODE, 28603, "ISO-8859-13,ISO885913,", 61031, NULL },// Estonian (ISO 8859-13)
	//{ NCP_8BIT | NCP_RECODE, 863, "IBM863,IBM863,", 61032, NULL },// OEM French Canadian; French Canadian (DOS)
	//{ NCP_8BIT | NCP_RECODE, 737, "IBM737,IBM737,", 61033, NULL },// OEM Greek (formerly 437G); Greek (DOS)
	{ NCP_8BIT | NCP_RECODE, 28597, "ISO-8859-7,ISO88597,csisolatingreek,ecma118,elot928,greek,greek8,isoir126,", 61034, NULL },// Greek (ISO 8859-7)
	//{ NCP_8BIT | NCP_RECODE, 10006, "x-mac-greek,xmacgreek,", 61035, NULL },// Greek (Mac)
	{ NCP_8BIT | NCP_RECODE, 1253, "Windows-1253,Windows1253,", 61036, NULL },// ANSI Greek; Greek (Windows)
	//{ NCP_8BIT | NCP_RECODE, 869, "IBM869,IBM869,", 61037, NULL },// OEM Modern Greek; Greek, Modern (DOS)
	//{ NCP_8BIT | NCP_RECODE, 862, "dos-862,DOS862,", 61038, NULL },// OEM Hebrew; Hebrew (DOS)
	{ NCP_8BIT | NCP_RECODE, 38598, "iso-8859-8-i,iso88598i,logical,", 61039, NULL },// Hebrew (ISO 8859-8); Hebrew (ISO-Logical)
	{ NCP_8BIT | NCP_RECODE, 28598, "iso-8859-8,iso88598,csisolatinhebrew,hebrew,isoir138,visual,", 61040, NULL },// Hebrew (ISO 8859-8); Hebrew (ISO-Visual)
	//{ NCP_8BIT | NCP_RECODE, 10005, "x-mac-hebrew,xmachebrew,", 61041, NULL },// Hebrew (Mac)
	{ NCP_8BIT | NCP_RECODE, 1255, "Windows-1255,Windows1255,", 61042, NULL },// ANSI Hebrew; Hebrew (Windows)
	//{ NCP_8BIT | NCP_RECODE, 861, "ibm861,ibm861,", 61043, NULL },// OEM Icelandic; Icelandic (DOS)
	//{ NCP_8BIT | NCP_RECODE, 10079, "x-mac-icelandic,xmacicelandic,", 61044, NULL },// Icelandic (Mac)
	//{ NCP_8BIT | NCP_RECODE, 10001, "x-mac-japanese,xmacjapanese,", 61045, NULL },// Japanese (Mac)
	{ NCP_8BIT | NCP_RECODE, 932, "Shift_JIS,ShiftJIS,Shiftjs,csshiftjis,cswindows31j,mskanji,xmscp932,xsjis,", 61046, NULL },// ANSI/OEM Japanese; Japanese (Shift-JIS)
	//{ NCP_8BIT | NCP_RECODE, 10003, "x-mac-korean,xmackorean,", 0, NULL },// Korean (Mac)
	{ NCP_8BIT | NCP_RECODE, 949, "Windows-949,Windows949,ksx1001,ksc56011987,csksc5601,euckr,isoir149,korean,ksc56011989", 61048, NULL },// ANSI/OEM Korean (Unified Hangul Code)
	{ NCP_8BIT | NCP_RECODE, 28593, "iso-8859-3,iso88593,latin3,isoir109,l3,", 61049, NULL },// Latin-3 (ISO 8859-3)
	{ NCP_8BIT | NCP_RECODE, 28605, "iso-8859-15,iso885915,latin9,l9,", 61050, NULL },// Latin-9 (ISO 8859-15)
	{ NCP_8BIT | NCP_RECODE, 28600, "ISO-8859-10,ISO885910,Windows-28600,Windows28600,", 61051, NULL },// Nordic (ISO 8859-10)
	//{ NCP_8BIT | NCP_RECODE, 865, "ibm865,ibm865,", 0, NULL },// OEM Nordic; Nordic (DOS)
	{ NCP_8BIT | NCP_RECODE, 437, "ibm437,ibm437,437,cp437,cspc8,codepage437,", 61072, NULL },// OEM United States
	//{ NCP_8BIT | NCP_RECODE, 858, "ibm858,ibm858,ibm00858,", 0, NULL },// OEM Multilingual Latin-1 + Euro symbol
	//{ NCP_8BIT | NCP_RECODE, 860, "ibm860,ibm860,", 0, NULL },// OEM Portuguese; Portuguese (DOS)
	{ NCP_8BIT | NCP_RECODE, 28606, "ISO-8859-16,ISO885916,Windows-28606,Windows28606", 61054, NULL },// Latin-10 (ISO 8859-16)
	//{ NCP_8BIT | NCP_RECODE, 10010, "x-mac-romanian,xmacromanian,", 0, NULL },// Romanian (Mac)
	//{ NCP_8BIT | NCP_RECODE, 10021, "x-mac-thai,xmacthai,", 61056, NULL },// Thai (Mac)
	{ NCP_8BIT | NCP_RECODE, 874, "Windows-874,Windows874,dos874,iso885911,tis-620,tis620,isoir166", 61057, NULL },// ANSI/OEM Thai (ISO 8859-11); Thai (Windows)
	//{ NCP_8BIT | NCP_RECODE, 857, "ibm857,ibm857,", 61058, NULL },// OEM Turkish; Turkish (DOS)
	{ NCP_8BIT | NCP_RECODE, 28599, "iso-8859-9,iso88599,latin5,isoir148,l5,", 61059, NULL },// Turkish (ISO 8859-9)
	//{ NCP_8BIT | NCP_RECODE, 10081, "x-mac-turkish,xmacturkish,", 61060, NULL },// Turkish (Mac)
	{ NCP_8BIT | NCP_RECODE, 1254, "Windows-1254,Windows1254,", 61061, NULL },// ANSI Turkish; Turkish (Windows)
	//{ NCP_8BIT | NCP_RECODE, 10017, "x-mac-ukrainian,xmacukrainian,", 61062, NULL },// Ukrainian (Mac)
	{ NCP_8BIT | NCP_RECODE, 1258, "Windows-1258,Windows-258,", 61063, NULL },// ANSI/OEM Vietnamese; Vietnamese (Windows)
	//{ NCP_8BIT | NCP_RECODE, 850, "ibm850,ibm850,", 61064, NULL },// OEM Multilingual Latin-1; Western European (DOS)
	{ NCP_8BIT | NCP_RECODE, 28591, "ISO-8859-1,ISO88591,cp819,latin1,ibm819,isoir100,latin1,l1,", 61065, NULL },// ISO 8859-1 Latin-1; Western European (ISO)
	//{ NCP_8BIT | NCP_RECODE, 29001, "x-Europa,x-Europa,", 0, NULL },// Europa 3
	//{ NCP_8BIT | NCP_RECODE, 10000, "Macintosh,Macintosh,", 61066, NULL },// MAC Roman; Western European (Mac)
	{ NCP_8BIT | NCP_RECODE, 1252, "Windows-1252,Windows1252,cp367,cp819,ibm367,us,xansi,", 61067, NULL },// ANSI Latin-1; Western European (Windows)
	//{ NCP_8BIT | NCP_RECODE, 37, "ebcdic-cp-us,ebcdiccpus,ebcdiccpca,ebcdiccpwt,ebcdiccpnl,ibm037,cp037,", 61068, NULL },// IBM EBCDIC US-Canada
	//{ NCP_8BIT | NCP_RECODE, 500, "x-ebcdic-international,xebcdicinternational,", 61069, NULL },// IBM EBCDIC International
	//{ NCP_8BIT | NCP_RECODE, 870, "CP870,cp870,ebcdiccproece,ebcdiccpyu,csibm870,ibm870,", 00000, NULL },// IBM EBCDIC Multilingual/ROECE (Latin-2); IBM EBCDIC Multilingual Latin-2
	//{ NCP_8BIT | NCP_RECODE, 875, "x-EBCDIC-GreekModern,xebcdicgreekmodern,", 61070, NULL },// IBM EBCDIC Greek Modern
	//{ NCP_8BIT | NCP_RECODE, 1026, "CP1026,cp1026,csibm1026,ibm1026,", 61071, NULL },// IBM EBCDIC Turkish (Latin-5)
	//{ NCP_8BIT | NCP_RECODE, 1047, "IBM01047,ibm01047,", 00000, NULL },// IBM EBCDIC Latin-1/Open System
	//{ NCP_8BIT | NCP_RECODE, 1140, "x-ebcdic-cp-us-euro,xebcdiccpuseuro,", 00000, NULL },// IBM EBCDIC US-Canada (037 + Euro symbol); IBM EBCDIC (US-Canada-Euro)
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
	//{ NCP_8BIT | NCP_RECODE, 20105, "x-IA5,xia5,", 00000, NULL },// IA5 (IRV International Alphabet No. 5, 7-bit); Western European (IA5)
	//{ NCP_8BIT | NCP_RECODE, 20106, "x-IA5-German,xia5german,", 00000, NULL },// German (IA5) (7-bit)
	//{ NCP_8BIT | NCP_RECODE, 20107, "x-IA5-Swedish,xia5swedish,", 00000, NULL },// Swedish (IA5) (7-bit)
	//{ NCP_8BIT | NCP_RECODE, 20108, "x-IA5-Norwegian,xia5norwegian,", 00000, NULL },// Norwegian (IA5) (7-bit)
	//{ NCP_8BIT | NCP_RECODE, 20936, "X-CP20936,XCP20936,", 00000, NULL },// Simplified Chinese (GB2312); Chinese Simplified (GB2312-80)
	//{ NCP_8BIT | NCP_RECODE, 20949, "x-cp20949,x-cp20949,", 00000, NULL },// Korean Wansung
	//{ NCP_8BIT | NCP_RECODE, 52936, "HZ-GB-2312,HZGB2312,hz,", 00000, NULL },// HZ-GB2312 Simplified Chinese; Chinese Simplified (HZ)
	{ NCP_8BIT | NCP_RECODE, 54936, "GB18030,GB18030,", 61021, NULL },// Chinese GB18030
	{ NCP_8BIT | NCP_RECODE, 1361, "johab,johab,", 61047, NULL },// Korean (Johab)
	{ NCP_8BIT | NCP_RECODE, 20932, "euc-jp,euc-jp,", 61052, NULL },// Japanese (JIS X 0208-1990 & 0212-1990)
	{ NCP_8BIT | NCP_RECODE, 50220, "iso-2022-jp,iso2022jp,", 61045, NULL },// ISO 2022 Japanese with no halfwidth Katakana; Japanese (JIS)
	//{ NCP_8BIT | NCP_RECODE, 50221, "csISO2022JP,csiso2022jp,", 00000, NULL },// ISO 2022 Japanese with halfwidth Katakana; Japanese (JIS-Allow 1 byte Kana)
	//{ NCP_8BIT | NCP_RECODE, 50222, "_iso-2022-jp$SIO,iso2022jpSIO,", 00000, NULL },// ISO 2022 Japanese JIS X 0201-1989; Japanese (JIS-Allow 1 byte Kana - SO/SI)
	//{ NCP_8BIT | NCP_RECODE, 50225, "iso-2022-kr,iso2022kr,csiso2022kr,", 00000, NULL },// Korean (ISO-2022-KR)
	//{ NCP_8BIT | NCP_RECODE, 50227, "x-cp50227,xcp50227,", 00000, NULL },// Chinese Simplified (ISO-2022)
	//{ NCP_8BIT | NCP_RECODE, 50229, "iso-2022-cn,iso2022cn,", 00000, NULL },// Chinese Traditional (ISO-2022)
	//{ NCP_8BIT | NCP_RECODE, 20000, "x-Chinese-CNS,xchinesecns,", 00000, NULL },// Chinese Traditional (CNS);  CNS Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20001, "x-cp20001,x-cp20001,", 00000, NULL },// TCA Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20002, "x-Chinese-Eten,xchineseeten,", 00000, NULL },// Chinese Traditional (Eten); Eten Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20003, "x-cp20003,x-cp20003,", 00000, NULL },// IBM5550 Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20004, "x-cp20004,x-cp20004,", 00000, NULL },// TeleText Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20005, "x-cp20004,x-cp20005,", 00000, NULL },// Wang Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20005, "x-cp20004,x-cp20005,", 00000, NULL },// Wang Taiwan
	//{ NCP_8BIT | NCP_RECODE, 20127, "us-ascii,us-ascii,", 00000, NULL },// US-ASCII (7-bit)
	//{ NCP_8BIT | NCP_RECODE, 20261, "x-cp20261,x-cp20261,", 00000, NULL },// T.61
	//{ NCP_8BIT | NCP_RECODE, 20269, "x-cp20269,x-cp20269,", 00000, NULL },// ISO 6937 Non-Spacing Accent
	//{ NCP_8BIT | NCP_RECODE, 51932, "euc-jp,eucjp,xeuc,xeucjp,", 61052, NULL },// Japanese (EUC)
	//{ NCP_8BIT | NCP_RECODE, 51936, "euc-cn,euccn,xeuccn,", 00000, NULL },// Chinese Simplified (EUC)
	{ NCP_8BIT | NCP_RECODE, 51949, "euc-kr,euckr,cseuckr,", 61053, NULL },// Korean (EUC)
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

//++Autogenerated -- start of section automatically generated
// Created with Python 3.7.3,  Unicode 11.0.0
static const UINT8 ANSICharClassifyTable[] = {
// Central European (Windows-1250)
0x33, 0xFF, 0xEC, 0xAA, 0xFC, 0xFF, 0xEC, 0xAA, 0xB8, 0xFB, 0xEF, 0xB3, 0xBF, 0xFB, 0xEB, 0xAE,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xEA,
// Cyrillic (Windows-1251)
0xBA, 0xFF, 0xEF, 0xAA, 0xFE, 0xFF, 0xEC, 0xAA, 0xA8, 0xFB, 0xEE, 0xB3, 0xAF, 0xFA, 0xEE, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
// Western European (Windows-1252)
0xB3, 0xFF, 0xEE, 0x22, 0xFC, 0xFF, 0xEF, 0xA2, 0xFC, 0xFF, 0xEF, 0xF3, 0xAF, 0xFB, 0xEB, 0xEA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA,
// Greek (Windows-1253)
0xB3, 0xFF, 0xCC, 0x00, 0xFC, 0xFF, 0xCC, 0x00, 0xEC, 0xFF, 0xCF, 0xF3, 0xAF, 0xFB, 0xEA, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0x8A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x2A,
// Turkish (Windows-1254)
0xB3, 0xFF, 0xEE, 0x02, 0xFC, 0xFF, 0xEF, 0x82, 0xFC, 0xFF, 0xEF, 0xF3, 0xAF, 0xFB, 0xEB, 0xEA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA,
// Hebrew (Windows-1255)
0xB3, 0xFF, 0xCE, 0x00, 0xFC, 0xFF, 0xCF, 0x00, 0xFC, 0xFF, 0xFF, 0xF3, 0xAF, 0xFB, 0xFB, 0xEA,
0xAA, 0xAA, 0x8A, 0xBA, 0xEB, 0xEA, 0x03, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x2A, 0x00,
// Arabic (Windows-1256)
0xBB, 0xFF, 0xEE, 0xAA, 0xFE, 0xFF, 0xEE, 0x82, 0xFC, 0xFF, 0xEF, 0xF3, 0xAF, 0xFB, 0xFB, 0xEA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0x82,
// Baltic (Windows-1257)
0x33, 0xFF, 0xCC, 0xEC, 0xFC, 0xFF, 0xCC, 0x3C, 0xF0, 0xF3, 0xEE, 0xB3, 0xAF, 0xFB, 0xEA, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xEA,
// Vietnamese (Windows-1258)
0xB3, 0xFF, 0xCE, 0x02, 0xFC, 0xFF, 0xCF, 0x82, 0xFC, 0xFF, 0xEF, 0xF3, 0xAF, 0xFB, 0xEB, 0xEA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xBA,
// Thai (Windows-874)
0x03, 0x0C, 0x00, 0x00, 0xFC, 0xFF, 0x00, 0x00, 0xA8, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x2A, 0xC0, 0xAA, 0xAA, 0xAA, 0xEA, 0xAA, 0xAA, 0xFA, 0x00,
};

static const UINT8* GetANSICharClassifyTable(UINT cp, int *length) {
	switch (cp) {
	case 1250: // Central European (Windows-1250)
		*length = 32;
		return ANSICharClassifyTable + 0;
	case 1251: // Cyrillic (Windows-1251)
		*length = 32;
		return ANSICharClassifyTable + 32;
	case 1252: // Western European (Windows-1252)
		*length = 32;
		return ANSICharClassifyTable + 64;
	case 1253: // Greek (Windows-1253)
		*length = 32;
		return ANSICharClassifyTable + 96;
	case 1254: // Turkish (Windows-1254)
		*length = 32;
		return ANSICharClassifyTable + 128;
	case 1255: // Hebrew (Windows-1255)
		*length = 32;
		return ANSICharClassifyTable + 160;
	case 1256: // Arabic (Windows-1256)
		*length = 32;
		return ANSICharClassifyTable + 192;
	case 1257: // Baltic (Windows-1257)
		*length = 32;
		return ANSICharClassifyTable + 224;
	case 1258: // Vietnamese (Windows-1258)
		*length = 32;
		return ANSICharClassifyTable + 256;
	case 874: // Thai (Windows-874)
		*length = 32;
		return ANSICharClassifyTable + 288;
	default:
		*length = 0;
		return NULL;
	}
}

const char* GetFoldDisplayEllipsis(UINT cpEdit, UINT acp) {
	switch (cpEdit) {
	case SC_CP_UTF8:
		return "\xC2\xB7\xC2\xB7\xC2\xB7";
	case 932: // Shift_JIS
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
	switch (acp) {
	case 1250: // Central European (Windows-1250)
	case 1251: // Cyrillic (Windows-1251)
	case 1252: // Western European (Windows-1252)
	case 1253: // Greek (Windows-1253)
	case 1254: // Turkish (Windows-1254)
	case 1255: // Hebrew (Windows-1255)
	case 1256: // Arabic (Windows-1256)
	case 1257: // Baltic (Windows-1257)
	case 1258: // Vietnamese (Windows-1258)
		return "\xB7\xB7\xB7";
	case 874: // Thai (Windows-874)
	default:
		return "...";
	}
}
//--Autogenerated -- end of section automatically generated

void Encoding_ReleaseResources(void) {
	if (g_AllEncodingLabel) {
		NP2HeapFree(g_AllEncodingLabel);
	}
}

//=============================================================================
//
// EditSetNewEncoding()
//
BOOL EditSetNewEncoding(HWND hwnd, int iCurrentEncoding, int iNewEncoding, BOOL bNoUI, BOOL bSetSavePoint) {
	if (iCurrentEncoding != iNewEncoding) {
		if (iCurrentEncoding != CPI_DEFAULT && iNewEncoding != CPI_DEFAULT) {
			return TRUE;
		}

		const UINT cpSrc = (mEncoding[iCurrentEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8;
		const UINT cpDest = (mEncoding[iNewEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8;

		if (SendMessage(hwnd, SCI_GETLENGTH, 0, 0) == 0) {
			const BOOL bIsEmptyUndoHistory =
				(SendMessage(hwnd, SCI_CANUNDO, 0, 0) == 0 && SendMessage(hwnd, SCI_CANREDO, 0, 0) == 0);

			if (bNoUI || bIsEmptyUndoHistory || InfoBox(MBYESNO, L"MsgConv2", IDS_ASK_ENCODING2) == IDYES) {
				EditConvertText(hwnd, cpSrc, cpDest, bSetSavePoint);
				return TRUE;
			}
		} else if (bNoUI || InfoBox(MBYESNO, L"MsgConv1", IDS_ASK_ENCODING) == IDYES) {
			BeginWaitCursor();
			EditConvertText(hwnd, cpSrc, cpDest, FALSE);
			EndWaitCursor();
			return TRUE;
		}
	}

	return FALSE;
}

void EditOnCodePageChanged(HWND hwnd, UINT oldCodePage) {
	const UINT cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
	const UINT acp = GetACP();
	if (oldCodePage == SC_CP_UTF8) {
		if (cpEdit == 0) {
			// UTF-8 to SBCS
			int length = 0;
			const UINT8 *buf = GetANSICharClassifyTable(acp, &length);
			SendMessage(hwnd, SCI_SETCHARCLASSESEX, length, (LPARAM)buf);
		} else {
			// UTF-8 to DBCS
		}
	} else {
		if (oldCodePage == 0) {
			// SBCS to UTF-8
			SendMessage(hwnd, SCI_SETCHARCLASSESEX, 0, 0);
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
	const UINT oemcp = GetOEMCP();
	wsprintf(wchANSI, L" (%u)", GetACP());
	mEncoding[CPI_OEM].uCodePage = oemcp;
	wsprintf(wchOEM, L" (%u)", oemcp);

	g_DOSEncoding = CPI_OEM;

	// Try to set the DOS encoding to DOS-437 if the default OEMCP is not DOS-437
	if (oemcp != 437) {
		for (int i = CPI_UTF7 + 1; i < (int)COUNTOF(mEncoding); ++i) {
			if (mEncoding[i].uCodePage == 437 && Encoding_IsValid(i)) {
				g_DOSEncoding = i;
				break;
			}
		}
	}
}

int Encoding_MapIniSetting(BOOL bLoad, int iSetting) {
	if (bLoad) {
		switch (iSetting) {
		case 0:
			return CPI_DEFAULT;
		case 1:
			return CPI_UNICODEBOM;
		case 2:
			return CPI_UNICODEBEBOM;
		case 3:
			return CPI_UTF8;
		case 4:
			return CPI_UTF8SIGN;
		case 5:
			return CPI_OEM;
		case 6:
			return CPI_UNICODE;
		case 7:
			return CPI_UNICODEBE;
		case 8:
			return CPI_UTF7;
		default: {
			for (int i = CPI_UTF7 + 1; i < (int)COUNTOF(mEncoding); i++) {
				if (mEncoding[i].uCodePage == (UINT)iSetting && Encoding_IsValid(i)) {
					return i;
				}
			}
			return CPI_DEFAULT;
		}
		}
	} else {
		switch (iSetting) {
		case CPI_DEFAULT:
			return 0;
		case CPI_UNICODEBOM:
			return 1;
		case CPI_UNICODEBEBOM:
			return 2;
		case CPI_UTF8:
			return 3;
		case CPI_UTF8SIGN:
			return 4;
		case CPI_OEM:
			return 5;
		case CPI_UNICODE:
			return 6;
		case CPI_UNICODEBE:
			return 7;
		case CPI_UTF7:
			return 8;
		default:
			return mEncoding[iSetting].uCodePage;
		}
	}
}

void Encoding_GetLabel(int iEncoding) {
	if (StrIsEmpty(mEncoding[iEncoding].wchLabel)) {
		WCHAR *pwsz;
		WCHAR wch[256] = L"";
		GetString(mEncoding[iEncoding].idsName, wch, COUNTOF(wch));
		if ((pwsz = StrChr(wch, L';')) != NULL) {
			if ((pwsz = StrChr(CharNext(pwsz), L';')) != NULL) {
				pwsz = CharNext(pwsz);
			}
		}
		if (!pwsz) {
			pwsz = wch;
		}
		if (g_AllEncodingLabel == NULL) {
			g_AllEncodingLabel = (LPWSTR)NP2HeapAlloc(COUNTOF(mEncoding) * MAX_ENCODING_LABEL_SIZE * sizeof(WCHAR));
		}
		mEncoding[iEncoding].wchLabel = g_AllEncodingLabel + iEncoding * MAX_ENCODING_LABEL_SIZE;
		lstrcpyn(mEncoding[iEncoding].wchLabel, pwsz, MAX_ENCODING_LABEL_SIZE);
	}
}

static inline BOOL IsValidEncoding(int iEncoding) {
	if (!(mEncoding[iEncoding].uFlags & NCP_INTERNAL)) {
		const UINT cp = mEncoding[iEncoding].uCodePage;
		CPINFO cpi;
		return IsValidCodePage(cp) && GetCPInfo(cp, &cpi);
	}
	return TRUE;
}

int Encoding_Match(LPCWSTR pwszTest) {
	char tchTest[256];
	WideCharToMultiByte(CP_ACP, 0, pwszTest, -1, tchTest, COUNTOF(tchTest), NULL, NULL);
	return Encoding_MatchA(tchTest);
}

int Encoding_MatchA(char *pchTest) {
	char chTest[256];
	char *pchSrc = pchTest;
	char *pchDst = chTest;

	*pchDst++ = ',';
	while (*pchSrc) {
		if (isalnum((unsigned char)(*pchSrc))) {
			if (*pchSrc >= 'A' && *pchSrc <= 'Z') {
				*pchDst++ = (char)(*pchSrc + 'a' - 'A');
			} else {
				*pchDst++ = *pchSrc;
			}
		}
		pchSrc++;
	}

	*pchDst++ = ',';
	*pchDst = 0;

	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		if (StrStrIA(mEncoding[i].pszParseNames, chTest)) {
			if (IsValidEncoding(i)) {
				return i;
			}
			break;
		}
	}

	return -1;
}

BOOL Encoding_IsValid(int iTestEncoding) {
	return iTestEncoding >= 0 && iTestEncoding < (int)COUNTOF(mEncoding) && IsValidEncoding(iTestEncoding);
}

typedef struct _ee {
	int id;
	WCHAR wch[256];
} ENCODINGENTRY, *PENCODINGENTRY;

int CmpEncoding(const void *s1, const void *s2) {
	return StrCmp(((PENCODINGENTRY)s1)->wch, ((PENCODINGENTRY)s2)->wch);
}

void Encoding_AddToListView(HWND hwnd, int idSel, BOOL bRecodeOnly) {
	PENCODINGENTRY pEE = (PENCODINGENTRY)NP2HeapAlloc(COUNTOF(mEncoding) * sizeof(ENCODINGENTRY));
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		pEE[i].id = i;
		GetString(mEncoding[i].idsName, pEE[i].wch, COUNTOF(pEE[i].wch));
	}
	qsort(pEE, COUNTOF(mEncoding), sizeof(ENCODINGENTRY), CmpEncoding);

	WCHAR wchBuf[256];
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = wchBuf;

	int iSelItem = -1;
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		const int id = pEE[i].id;
		if (!bRecodeOnly || (mEncoding[id].uFlags & NCP_RECODE)) {
			WCHAR *pwsz;

			lvi.iItem = ListView_GetItemCount(hwnd);
			if ((pwsz = StrChr(pEE[i].wch, L';')) != NULL) {
				lstrcpyn(wchBuf, CharNext(pwsz), COUNTOF(wchBuf));
				if ((pwsz = StrChr(wchBuf, L';')) != NULL) {
					*pwsz = 0;
				}
			} else {
				lstrcpyn(wchBuf, pEE[i].wch, COUNTOF(wchBuf));
			}

			if (id == CPI_DEFAULT) {
				StrCatBuff(wchBuf, wchANSI, COUNTOF(wchBuf));
			} else if (id == CPI_OEM) {
				StrCatBuff(wchBuf, wchOEM, COUNTOF(wchBuf));
			}

			lvi.iImage = IsValidEncoding(id) ? 0 : 1;
			lvi.lParam = (LPARAM)id;
			ListView_InsertItem(hwnd, &lvi);

			if (idSel == id) {
				iSelItem = lvi.iItem;
			}
		}
	}

	NP2HeapFree(pEE);

	if (iSelItem != -1) {
		ListView_SetItemState(hwnd, iSelItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		ListView_EnsureVisible(hwnd, iSelItem, FALSE);
	} else {
		ListView_SetItemState(hwnd, 0, LVIS_FOCUSED, LVIS_FOCUSED);
		ListView_EnsureVisible(hwnd, 0, FALSE);
	}
}

BOOL Encoding_GetFromListView(HWND hwnd, int *pidEncoding) {
	LVITEM lvi;

	lvi.iItem = ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_SELECTED);
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;

	if (ListView_GetItem(hwnd, &lvi)) {
		if (Encoding_IsValid((int)lvi.lParam)) {
			*pidEncoding = (int)lvi.lParam;
			return TRUE;
		}
		MsgBox(MBWARN, IDS_ERR_ENCODINGNA);
	}

	return FALSE;
}

void Encoding_AddToComboboxEx(HWND hwnd, int idSel, BOOL bRecodeOnly) {
	PENCODINGENTRY pEE = (PENCODINGENTRY)NP2HeapAlloc(COUNTOF(mEncoding) * sizeof(ENCODINGENTRY));
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		pEE[i].id = i;
		GetString(mEncoding[i].idsName, pEE[i].wch, COUNTOF(pEE[i].wch));
	}
	qsort(pEE, COUNTOF(mEncoding), sizeof(ENCODINGENTRY), CmpEncoding);

	WCHAR wchBuf[256];
	COMBOBOXEXITEM cbei;
	ZeroMemory(&cbei, sizeof(COMBOBOXEXITEM));

	cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
	cbei.pszText = wchBuf;
	cbei.cchTextMax = COUNTOF(wchBuf);
	cbei.iImage = 0;
	cbei.iSelectedImage = 0;

	int iSelItem = -1;
	for (int i = 0; i < (int)COUNTOF(mEncoding); i++) {
		const int id = pEE[i].id;
		if (!bRecodeOnly || (mEncoding[id].uFlags & NCP_RECODE)) {
			WCHAR *pwsz;

			cbei.iItem = SendMessage(hwnd, CB_GETCOUNT, 0, 0);
			if ((pwsz = StrChr(pEE[i].wch, L';')) != NULL) {
				lstrcpyn(wchBuf, CharNext(pwsz), COUNTOF(wchBuf));
				if ((pwsz = StrChr(wchBuf, L';')) != NULL) {
					*pwsz = 0;
				}
			} else {
				lstrcpyn(wchBuf, pEE[i].wch, COUNTOF(wchBuf));
			}

			if (id == CPI_DEFAULT) {
				StrCatBuff(wchBuf, wchANSI, COUNTOF(wchBuf));
			} else if (id == CPI_OEM) {
				StrCatBuff(wchBuf, wchOEM, COUNTOF(wchBuf));
			}

			cbei.iImage = IsValidEncoding(id) ? 0 : 1;
			cbei.lParam = (LPARAM)id;
			SendMessage(hwnd, CBEM_INSERTITEM, 0, (LPARAM)&cbei);

			if (idSel == id) {
				iSelItem = (int)cbei.iItem;
			}
		}
	}

	NP2HeapFree(pEE);

	if (iSelItem != -1) {
		SendMessage(hwnd, CB_SETCURSEL, iSelItem, 0);
	}
}

BOOL Encoding_GetFromComboboxEx(HWND hwnd, int *pidEncoding) {
	COMBOBOXEXITEM cbei;

	cbei.iItem = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	cbei.mask = CBEIF_LPARAM;

	if (SendMessage(hwnd, CBEM_GETITEM, 0, (LPARAM)&cbei)) {
		if (Encoding_IsValid((int)cbei.lParam)) {
			*pidEncoding = (int)cbei.lParam;
			return TRUE;
		}
		MsgBox(MBWARN, IDS_ERR_ENCODINGNA);
	}

	return FALSE;
}

BOOL IsUnicode(const char *pBuffer, DWORD cb, LPBOOL lpbBOM, LPBOOL lpbReverse) {
	if (pBuffer == NULL || cb < 2) {
		return FALSE;
	}

	int i = 0xFFFF;
	const BOOL bIsTextUnicode = bSkipUnicodeDetection ? FALSE : IsTextUnicode(pBuffer, cb, &i);
	const BOOL bHasBOM	 = (*((UNALIGNED PWCHAR)pBuffer) == 0xFEFF);
	const BOOL bHasRBOM = (*((UNALIGNED PWCHAR)pBuffer) == 0xFFFE);

	if (i == 0xFFFF) { // i doesn't seem to have been modified ...
		i = 0;
	}

	if (bIsTextUnicode || bHasBOM || bHasRBOM ||
			((i & (IS_TEXT_UNICODE_UNICODE_MASK | IS_TEXT_UNICODE_REVERSE_MASK)) &&
			 !((i & IS_TEXT_UNICODE_UNICODE_MASK) && (i & IS_TEXT_UNICODE_REVERSE_MASK)) &&
			 !(i & IS_TEXT_UNICODE_ODD_LENGTH) &&
			 !(i & IS_TEXT_UNICODE_ILLEGAL_CHARS && !(i & IS_TEXT_UNICODE_REVERSE_SIGNATURE)) &&
			 !((i & IS_TEXT_UNICODE_REVERSE_MASK) == IS_TEXT_UNICODE_REVERSE_STATISTICS))) {
		if (lpbBOM) {
			*lpbBOM = bHasBOM || bHasRBOM ||
					   (i & (IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE));
		}
		if (lpbReverse) {
			*lpbReverse = bHasRBOM || (i & IS_TEXT_UNICODE_REVERSE_MASK);
		}
		return TRUE;
	}

	return FALSE;
}

#if 0
BOOL IsUTF8(const char *pTest, DWORD nLength) {
	static const int byte_class_table[256] = {
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

	static const utf8_state state_table[] = {
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

#define BYTE_CLASS(b) (byte_class_table[(unsigned char)b])
#define NEXT_STATE(b, cur) (state_table[(BYTE_CLASS(b) * kNumOfStates) + (cur)])

	utf8_state current = kSTART;

	const unsigned char *pt = (const unsigned char *)pTest;
	const unsigned char * const end = pt + nLength;

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

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See https://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

BOOL IsUTF8(const char *pTest, DWORD nLength) {
	enum {
		UTF8_ACCEPT = 0,
		UTF8_REJECT = 12,
	};

#if 0
	StopWatch watch;
	StopWatch_Start(watch);
#endif

	static const unsigned char utf8_dfa[] = {
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

	const unsigned char *pt = (const unsigned char *)pTest;
	const unsigned char * const end = pt + nLength;

	UINT state = UTF8_ACCEPT;
	while (pt < end) {
		state = utf8_dfa[256 + state + utf8_dfa[*pt]];
		if (state == UTF8_REJECT) {
			return FALSE;
		}
		++pt;
	}

#if 0
	StopWatch_Stop(watch);
	StopWatch_Show(&watch, L"UTF8 time");
#endif

	return state == UTF8_ACCEPT;
}

BOOL IsUTF7(const char *pTest, DWORD nLength) {
	const unsigned char *pt = (const unsigned char *)pTest;
	const unsigned char * const end = pt + nLength;

	while (pt < end && (*pt & 0x80) == 0) {
		++pt;
	}

	return pt == end;
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
	const unsigned char *pt = (const unsigned char *)utf8_string;

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
	const unsigned char *pt = (const unsigned char *)utf8_string;

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

//=============================================================================
//
// FileVars_IsUTF8()
//
BOOL FileVars_IsUTF8(LPCFILEVARS lpfv) {
	if (lpfv->mask & FV_ENCODING) {
		if (_stricmp(lpfv->tchEncoding, "utf-8") == 0 || _stricmp(lpfv->tchEncoding, "utf8") == 0) {
			return TRUE;
		}
	}

	return FALSE;
}

//=============================================================================
//
// FileVars_IsNonUTF8()
//
BOOL FileVars_IsNonUTF8(LPCFILEVARS lpfv) {
	if (lpfv->mask & FV_ENCODING) {
		if (StrNotEmptyA(lpfv->tchEncoding) &&
				_stricmp(lpfv->tchEncoding, "utf-8") != 0 && _stricmp(lpfv->tchEncoding, "utf8") != 0) {
			return TRUE;
		}
	}

	return FALSE;
}

//=============================================================================
//
// FileVars_IsValidEncoding()
//
BOOL FileVars_IsValidEncoding(LPCFILEVARS lpfv) {
	return (lpfv->mask & FV_ENCODING) && Encoding_IsValid(lpfv->iEncoding);
}

//=============================================================================
//
// FileVars_GetEncoding()
//
int FileVars_GetEncoding(LPCFILEVARS lpfv) {
	if (lpfv->mask & FV_ENCODING) {
		return lpfv->iEncoding;
	}
	return -1;
}

// End of EditEncoding.c
