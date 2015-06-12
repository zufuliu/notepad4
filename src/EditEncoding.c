// Edit Encoding

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x501
#endif
#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
#include "Helpers.h"
#include "SciCall.h"
#include "Resource.h"

#pragma warning(push)
#pragma warning(disable: 4706)


extern BOOL bSkipUnicodeDetection;
extern int iDefaultCodePage;

int g_DOSEncoding;

// Supported Encodings
WCHAR wchANSI[8] = L"";
WCHAR wchOEM [8] = L"";


// Encoding
NP2ENCODING mEncoding[] = {
	{ NCP_DEFAULT | NCP_RECODE, 0, "ANSI,ANSI,ASCII,", 61000, L"" },
	{ NCP_8BIT | NCP_RECODE, 0, "OEM,OEM,", 61001, L"" },
	{ NCP_UNICODE | NCP_UNICODE_BOM, 0, "", 61002, L"" },
	{ NCP_UNICODE | NCP_UNICODE_REVERSE | NCP_UNICODE_BOM, 0, "", 61003, L"" },
	{ NCP_UNICODE | NCP_RECODE, 0, "UTF-16,UTF16,Unicode,", 61004, L"" },
	{ NCP_UNICODE | NCP_UNICODE_REVERSE | NCP_RECODE, 0, "UTF-16BE,UTF16BE,UnicodeBE,", 61005, L"" },
	{ NCP_UTF8 | NCP_RECODE, 0, "UTF-8,UTF8,", 61006, L"" },
	{ NCP_UTF8 | NCP_UTF8_SIGN, 0, "UTF-8,UTF8,", 61007, L"" },
	{ NCP_8BIT | NCP_RECODE, CP_UTF7, "UTF-7,UTF7,", 61008, L"" },
/* { NCP_8BIT|NCP_RECODE,720,"DOS-720,DOS720,",61009,L"" },
	{ NCP_8BIT|NCP_RECODE,28596,"ISO-8859-6,ISO88596,arabic,csisolatinarabic,ECMA114,isoir127,",61010,L"" },
	{ NCP_8BIT|NCP_RECODE,10004,"x-mac-arabic,xmacarabic,",61011,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1256, "Windows-1256,Windows1256,CP1256", 61012, L"" },
/*	{ NCP_8BIT|NCP_RECODE,775,"IBM775,IBM775,CP500,",61013,L"" },
	{ NCP_8BIT|NCP_RECODE,28594,"ISO-8859-4,ISO88594,csisolatin4,isoir110,l4,Latin4,",61014,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1257, "Windows-1257,Windows1257,", 61015, L"" },
/*	{ NCP_8BIT|NCP_RECODE,852,"ibm852,ibm852,cp852,",61016,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 28592, "ISO-8859-2,ISO88592,csisolatin2,isoir101,latin2,l2,", 61017, L"" },
/*	{ NCP_8BIT|NCP_RECODE,10029,"x-mac-ce,xmacce,",61018,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1250, "Windows-1250,Windows1250,xcp1250,", 61019, L"" },
	{ NCP_8BIT | NCP_RECODE, 936, "GB2312,GB2312,chinese,cngb,csgb2312,csgb231280,gb231280,gbk,", 61020, L"" },
/*	{ NCP_8BIT|NCP_RECODE,10008,"x-mac-chinesesimp,xmacchinesesimp,",61021,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 950, "BIG5,BIG5,cnbig5,csbig5,xxbig5,", 61022, L"" },
/*	{ NCP_8BIT|NCP_RECODE,10002,"x-mac-chinesetrad,xmacchinesetrad,",61023,L"" },
	{ NCP_8BIT|NCP_RECODE,10082,"x-mac-croatian,xmaccroatian,",61024,L"" },
	{ NCP_8BIT|NCP_RECODE,866,"CP866,CP866,IBM866,",61025,L"" },
	{ NCP_8BIT|NCP_RECODE,28595,"ISO-8859-5,ISO88595,csisolatin5,csisolatincyrillic,cyrillic,isoir144,",61026,L"" },
	{ NCP_8BIT|NCP_RECODE,20866,"koi8-r,koi8r,cskoi8r,koi,koi8,",61027,L"" },
	{ NCP_8BIT|NCP_RECODE,21866,"koi8-u,koi8u,koi8ru,",61028,L"" },
	{ NCP_8BIT|NCP_RECODE,10007,"x-mac-cyrillic,xmaccyrillic,",61029,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1251, "Windows-1251,Windows1251,XCP1251,", 61030, L"" },
/*	{ NCP_8BIT|NCP_RECODE,28603,"ISO-8859-13,ISO885913,",61031,L"" },
	{ NCP_8BIT|NCP_RECODE,863,"IBM863,IBM863,",61032,L"" },
	{ NCP_8BIT|NCP_RECODE,737,"IBM737,IBM737,",61033,L"" },
	{ NCP_8BIT|NCP_RECODE,28597,"ISO-8859-7,ISO88597,csisolatingreek,ecma118,elot928,greek,greek8,isoir126,",61034,L"" },
	{ NCP_8BIT|NCP_RECODE,10006,"x-mac-greek,xmacgreek,",61035,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1253, "Windows-1253,Windows1253,", 61036, L"" },
/*	{ NCP_8BIT|NCP_RECODE,869,"IBM869,IBM869,",61037,L"" },
	{ NCP_8BIT|NCP_RECODE,862,"dos-862,DOS862,",61038,L"" },
	{ NCP_8BIT|NCP_RECODE,38598,"iso-8859-8-i,iso88598i,logical,",61039,L"" },
	{ NCP_8BIT|NCP_RECODE,28598,"iso-8859-8,iso88598,csisolatinhebrew,hebrew,isoir138,visual,",61040,L"" },
	{ NCP_8BIT|NCP_RECODE,10005,"x-mac-hebrew,xmachebrew,",61041,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1255, "Windows-1255,Windows1255,", 61042, L"" },
/*	{ NCP_8BIT|NCP_RECODE,861,"ibm861,ibm861,",61043,L"" },
	{ NCP_8BIT|NCP_RECODE,10079,"x-mac-icelandic,xmacicelandic,",61044,L"" },
	{ NCP_8BIT|NCP_RECODE,10001,"x-mac-japanese,xmacjapanese,",61045,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 932, "Shift_JIS,ShiftJIS,Shiftjs,csshiftjis,cswindows31j,mskanji,xmscp932,xsjis,", 61046, L"" },
/*	{ NCP_8BIT|NCP_RECODE,10003,"x-mac-korean,xmackorean,",61047,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 949, "Windows-949,Windows949,ksc56011987,csksc5601,euckr,isoir149,korean,ksc56011989", 61048, L"" },
/*	{ NCP_8BIT|NCP_RECODE,28593,"iso-8859-3,iso88593,latin3,isoir109,l3,",61049,L"" },
	{ NCP_8BIT|NCP_RECODE,28605,"iso-8859-15,iso885915,latin9,l9,",61050,L"" },
	{ NCP_8BIT|NCP_RECODE,865,"ibm865,ibm865,",61051,L"" },
	{ NCP_8BIT|NCP_RECODE,437,"ibm437,ibm437,437,cp437,cspc8,codepage437,",61052,L"" },
	{ NCP_8BIT|NCP_RECODE,858,"ibm858,ibm858,ibm00858,",61053,L"" },
	{ NCP_8BIT|NCP_RECODE,860,"ibm860,ibm860,",61054,L"" },
	{ NCP_8BIT|NCP_RECODE,10010,"x-mac-romanian,xmacromanian,",61055,L"" },
	{ NCP_8BIT|NCP_RECODE,10021,"x-mac-thai,xmacthai,",61056,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 874, "Windows-874,Windows874,dos874,iso885911,tis620,", 61057, L"" },
/*	{ NCP_8BIT|NCP_RECODE,857,"ibm857,ibm857,",61058,L"" },
	{ NCP_8BIT|NCP_RECODE,28599,"iso-8859-9,iso88599,latin5,isoir148,l5,",61059,L"" },
	{ NCP_8BIT|NCP_RECODE,10081,"x-mac-turkish,xmacturkish,",61060,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1254, "Windows-1254,Windows1254,", 61061, L"" },
/*	{ NCP_8BIT|NCP_RECODE,10017,"x-mac-ukrainian,xmacukrainian,",61062,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1258, "Windows-1258,Windows-258,", 61063, L"" },
/*	{ NCP_8BIT|NCP_RECODE,850,"ibm850,ibm850,",61064,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 28591, "ISO-8859-1,ISO88591,cp819,latin1,ibm819,isoir100,latin1,l1,", 61065, L"" },
/*	{ NCP_8BIT|NCP_RECODE,10000,"Macintosh,Macintosh,",61066,L"" },*/
	{ NCP_8BIT | NCP_RECODE, 1252, "Windows-1252,Windows1252,cp367,cp819,ibm367,us,xansi,", 61067, L"" },
/*	{ NCP_8BIT|NCP_RECODE,37,"ebcdic-cp-us,ebcdiccpus,ebcdiccpca,ebcdiccpwt,ebcdiccpnl,ibm037,cp037,",61068,L"" },
	{ NCP_8BIT|NCP_RECODE,500,"x-ebcdic-international,xebcdicinternational,",61069,L"" },
	{ NCP_8BIT|NCP_RECODE,870,"CP870,cp870,ebcdiccproece,ebcdiccpyu,csibm870,ibm870,",00000,L"" },// IBM EBCDIC (Multilingual Latin-2)
	{ NCP_8BIT|NCP_RECODE,875,"x-EBCDIC-GreekModern,xebcdicgreekmodern,",61070,L"" },
	{ NCP_8BIT|NCP_RECODE,1026,"CP1026,cp1026,csibm1026,ibm1026,",61071,L"" },
	{ NCP_8BIT|NCP_RECODE,1047,"IBM01047,ibm01047,",00000,L"" },// IBM EBCDIC (Open System Latin-1)
	{ NCP_8BIT|NCP_RECODE,1140,"x-ebcdic-cp-us-euro,xebcdiccpuseuro,",00000,L"" },// IBM EBCDIC (US-Canada-Euro)
	{ NCP_8BIT|NCP_RECODE,1141,"x-ebcdic-germany-euro,xebcdicgermanyeuro,",00000,L"" },// IBM EBCDIC (Germany-Euro)
	{ NCP_8BIT|NCP_RECODE,1142,"x-ebcdic-denmarknorway-euro,xebcdicdenmarknorwayeuro,",00000,L"" },// IBM EBCDIC (Denmark-Norway-Euro)
	{ NCP_8BIT|NCP_RECODE,1143,"x-ebcdic-finlandsweden-euro,xebcdicfinlandswedeneuro,",00000,L"" },// IBM EBCDIC (Finland-Sweden-Euro)
	{ NCP_8BIT|NCP_RECODE,1144,"x-ebcdic-italy-euro,xebcdicitalyeuro,",00000,L"" },// IBM EBCDIC (Italy-Euro)
	{ NCP_8BIT|NCP_RECODE,1145,"x-ebcdic-spain-euro,xebcdicspaineuro,",00000,L"" },// IBM EBCDIC (Spain-Latin America-Euro)
	{ NCP_8BIT|NCP_RECODE,1146,"x-ebcdic-uk-euro,xebcdicukeuro,",00000,L"" },// IBM EBCDIC (UK-Euro)
	{ NCP_8BIT|NCP_RECODE,1147,"x-ebcdic-france-euro,xebcdicfranceeuro,",00000,L"" },// IBM EBCDIC (France-Euro)
	{ NCP_8BIT|NCP_RECODE,1148,"x-ebcdic-international-euro,xebcdicinternationaleuro,",00000,L"" },// IBM EBCDIC (International-Euro)
	{ NCP_8BIT|NCP_RECODE,1149,"x-ebcdic-icelandic-euro,xebcdicicelandiceuro,",00000,L"" },// IBM EBCDIC (Icelandic-Euro)
	{ NCP_8BIT|NCP_RECODE,20273,"x-EBCDIC-Germany,xebcdicgermany,",00000,L"" },// IBM EBCDIC (Germany)
	{ NCP_8BIT|NCP_RECODE,20277,"x-EBCDIC-DenmarkNorway,xebcdicdenmarknorway,ebcdiccpdk,ebcdiccpno,",00000,L"" },// IBM EBCDIC (Denmark-Norway)
	{ NCP_8BIT|NCP_RECODE,20278,"x-EBCDIC-FinlandSweden,xebcdicfinlandsweden,ebcdicpfi,ebcdiccpse,",00000,L"" },// IBM EBCDIC (Finland-Sweden)
	{ NCP_8BIT|NCP_RECODE,20280,"x-EBCDIC-Italy,xebcdicitaly,",00000,L"" },// IBM EBCDIC (Italy)
	{ NCP_8BIT|NCP_RECODE,20284,"x-EBCDIC-Spain,xebcdicspain,ebcdiccpes,",00000,L"" },// IBM EBCDIC (Spain-Latin America)
	{ NCP_8BIT|NCP_RECODE,20285,"x-EBCDIC-UK,xebcdicuk,ebcdiccpgb,",00000,L"" },// IBM EBCDIC (UK)
	{ NCP_8BIT|NCP_RECODE,20290,"x-EBCDIC-JapaneseKatakana,xebcdicjapanesekatakana,",00000,L"" },// IBM EBCDIC (Japanese Katakana)
	{ NCP_8BIT|NCP_RECODE,20297,"x-EBCDIC-France,xebcdicfrance,ebcdiccpfr,",00000,L"" },// IBM EBCDIC (France)
	{ NCP_8BIT|NCP_RECODE,20420,"x-EBCDIC-Arabic,xebcdicarabic,ebcdiccpar1,",00000,L"" },// IBM EBCDIC (Arabic)
	{ NCP_8BIT|NCP_RECODE,20423,"x-EBCDIC-Greek,xebcdicgreek,ebcdiccpgr,",00000,L"" },// IBM EBCDIC (Greek)
	{ NCP_8BIT|NCP_RECODE,20424,"x-EBCDIC-Hebrew,xebcdichebrew,ebcdiccphe,",00000,L"" },// IBM EBCDIC (Hebrew)
	{ NCP_8BIT|NCP_RECODE,20833,"x-EBCDIC-KoreanExtended,xebcdickoreanextended,",00000,L"" },// IBM EBCDIC (Korean Extended)
	{ NCP_8BIT|NCP_RECODE,20838,"x-EBCDIC-Thai,xebcdicthai,ibmthai,csibmthai,",00000,L"" },// IBM EBCDIC (Thai)
	{ NCP_8BIT|NCP_RECODE,20871,"x-EBCDIC-Icelandic,xebcdicicelandic,ebcdiccpis,",00000,L"" },// IBM EBCDIC (Icelandic)
	{ NCP_8BIT|NCP_RECODE,20880,"x-EBCDIC-CyrillicRussian,xebcdiccyrillicrussian,ebcdiccyrillic,",00000,L"" },// IBM EBCDIC (Cyrillic Russian)
	{ NCP_8BIT|NCP_RECODE,20905,"x-EBCDIC-Turkish,xebcdicturkish,ebcdiccptr,",00000,L"" },// IBM EBCDIC (Turkish)
	{ NCP_8BIT|NCP_RECODE,20924,"IBM00924,ibm00924,ebcdiclatin9euro,",00000,L"" },// IBM EBCDIC (Open System-Euro Latin-1)
	{ NCP_8BIT|NCP_RECODE,21025,"x-EBCDIC-CyrillicSerbianBulgarian,xebcdiccyrillicserbianbulgarian,",00000,L"" },// IBM EBCDIC (Cyrillic Serbian-Bulgarian)
	{ NCP_8BIT|NCP_RECODE,50930,"x-EBCDIC-JapaneseAndKana,xebcdicjapaneseandkana,",00000,L"" },// IBM EBCDIC (Japanese and Japanese Katakana)
	{ NCP_8BIT|NCP_RECODE,50931,"x-EBCDIC-JapaneseAndUSCanada,xebcdicjapaneseanduscanada,",00000,L"" },// IBM EBCDIC (Japanese and US-Canada)
	{ NCP_8BIT|NCP_RECODE,50933,"x-EBCDIC-KoreanAndKoreanExtended,xebcdickoreanandkoreanextended,",00000,L"" },// IBM EBCDIC (Korean and Korean Extended)
	{ NCP_8BIT|NCP_RECODE,50935,"x-EBCDIC-SimplifiedChinese,xebcdicsimplifiedchinese,",00000,L"" },// IBM EBCDIC (Chinese Simplified)
	{ NCP_8BIT|NCP_RECODE,50937,"x-EBCDIC-TraditionalChinese,xebcdictraditionalchinese,",00000,L"" },// IBM EBCDIC (Chinese Traditional)
	{ NCP_8BIT|NCP_RECODE,50939,"x-EBCDIC-JapaneseAndJapaneseLatin,xebcdicjapaneseandjapaneselatin,",00000,L"" },// IBM EBCDIC (Japanese and Japanese-Latin)
	{ NCP_8BIT|NCP_RECODE,20105,"x-IA5,xia5,",00000,L"" },// Western European (IA5)
	{ NCP_8BIT|NCP_RECODE,20106,"x-IA5-German,xia5german,",00000,L"" },// German (IA5)
	{ NCP_8BIT|NCP_RECODE,20107,"x-IA5-Swedish,xia5swedish,",00000,L"" },// Swedish (IA5)
	{ NCP_8BIT|NCP_RECODE,20108,"x-IA5-Norwegian,xia5norwegian,",00000,L"" },// Norwegian (IA5)*/
	{ NCP_8BIT | NCP_RECODE, 20936, "X-CP20936,XCP20936,", 61072, L"" }, // Chinese Simplified (GB2312)
	{ NCP_8BIT | NCP_RECODE, 52936, "HZ-GB-2312,HZGB2312,hz,", 61073, L"" }, // Chinese Simplified (HZ-GB2312)
	{ NCP_8BIT | NCP_RECODE, 54936, "GB18030,GB18030,", 61074, L"" }, // Chinese GB18030
/*	{ NCP_8BIT|NCP_RECODE,1361,	"johab,johab,",00000,L"" },// Korean (Johab)
	{ NCP_8BIT|NCP_RECODE,20932,"euc-jp,,",00000,L"" },// Japanese (JIS X 0208-1990 & 0212-1990)
	{ NCP_8BIT|NCP_RECODE,50220,"iso-2022-jp,iso2022jp,",00000,L"" },// Japanese (JIS)
	{ NCP_8BIT|NCP_RECODE,50221,"csISO2022JP,csiso2022jp,",00000,L"" },// Japanese (JIS-Allow 1 byte Kana)
	{ NCP_8BIT|NCP_RECODE,50222,"_iso-2022-jp$SIO,iso2022jpSIO,",00000,L"" },// Japanese (JIS-Allow 1 byte Kana - SO/SI)
	{ NCP_8BIT|NCP_RECODE,50225,"iso-2022-kr,iso2022kr,csiso2022kr,",00000,L"" },// Korean (ISO-2022-KR)
	{ NCP_8BIT|NCP_RECODE,50227,"x-cp50227,xcp50227,",00000,L"" },// Chinese Simplified (ISO-2022)
	{ NCP_8BIT|NCP_RECODE,50229,"iso-2022-cn,iso2022cn,",00000,L"" },// Chinese Traditional (ISO-2022)
	{ NCP_8BIT|NCP_RECODE,20000,"x-Chinese-CNS,xchinesecns,",00000,L"" },// Chinese Traditional (CNS)
	{ NCP_8BIT|NCP_RECODE,20002,"x-Chinese-Eten,xchineseeten,",00000,L"" },// Chinese Traditional (Eten)
	{ NCP_8BIT|NCP_RECODE,51932,"euc-jp,eucjp,xeuc,xeucjp,",00000,L"" },// Japanese (EUC)
	{ NCP_8BIT|NCP_RECODE,51936,"euc-cn,euccn,xeuccn,",00000,L"" },// Chinese Simplified (EUC)
	{ NCP_8BIT|NCP_RECODE,51949,"euc-kr,euckr,cseuckr,",00000,L"" },// Korean (EUC)
	{ NCP_8BIT|NCP_RECODE,57002,"x-iscii-de,xisciide,",00000,L"" },// ISCII Devanagari
	{ NCP_8BIT|NCP_RECODE,57003,"x-iscii-be,xisciibe,",00000,L"" },// ISCII Bengali
	{ NCP_8BIT|NCP_RECODE,57004,"x-iscii-ta,xisciita,",00000,L"" },// ISCII Tamil
	{ NCP_8BIT|NCP_RECODE,57005,"x-iscii-te,xisciite,",00000,L"" },// ISCII Telugu
	{ NCP_8BIT|NCP_RECODE,57006,"x-iscii-as,xisciias,",00000,L"" },// ISCII Assamese
	{ NCP_8BIT|NCP_RECODE,57007,"x-iscii-or,xisciior,",00000,L"" },// ISCII Oriya
	{ NCP_8BIT|NCP_RECODE,57008,"x-iscii-ka,xisciika,",00000,L"" },// ISCII Kannada
	{ NCP_8BIT|NCP_RECODE,57009,"x-iscii-ma,xisciima,",00000,L"" },// ISCII Malayalam
	{ NCP_8BIT|NCP_RECODE,57010,"x-iscii-gu,xisciigu,",00000,L"" },// ISCII Gujarathi
	{ NCP_8BIT|NCP_RECODE,57011,"x-iscii-pa,xisciipa,",00000,L"" },// ISCII Panjabi */
};


//=============================================================================
//
// EditSetNewEncoding()
//
BOOL EditSetNewEncoding(HWND hwnd, int iCurrentEncoding, int iNewEncoding, BOOL bNoUI, BOOL bSetSavePoint)
{
	if (iCurrentEncoding != iNewEncoding) {
		if ((iCurrentEncoding == CPI_DEFAULT && iNewEncoding == CPI_DEFAULT) ||
				(iCurrentEncoding != CPI_DEFAULT && iNewEncoding != CPI_DEFAULT)) {
			return TRUE;
		}

		if (SendMessage(hwnd, SCI_GETLENGTH, 0, 0) == 0) {
			BOOL bIsEmptyUndoHistory =
				(SendMessage(hwnd, SCI_CANUNDO, 0, 0) == 0 && SendMessage(hwnd, SCI_CANREDO, 0, 0) == 0);

			if ((iCurrentEncoding == CPI_DEFAULT || iNewEncoding == CPI_DEFAULT) &&
					(bNoUI || bIsEmptyUndoHistory || InfoBox(MBYESNO, L"MsgConv2", IDS_ASK_ENCODING2) == IDYES)) {
				EditConvertText(hwnd,
							(mEncoding[iCurrentEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8,
							(mEncoding[iNewEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8,
							bSetSavePoint);
				return TRUE;
			} else {
				return FALSE;
			}
		} else if ((iCurrentEncoding == CPI_DEFAULT || iNewEncoding == CPI_DEFAULT) &&
				   (bNoUI || InfoBox(MBYESNO, L"MsgConv1", IDS_ASK_ENCODING) == IDYES)) {
			BeginWaitCursor();
			EditConvertText(hwnd,
						(mEncoding[iCurrentEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8,
						(mEncoding[iNewEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8,
						FALSE);
			EndWaitCursor();
			return TRUE;
		} else {
			return FALSE;
		}
	}

	return FALSE;
}


//=============================================================================
//
// Encoding Helper Functions
//
void Encoding_InitDefaults()
{
	wsprintf(wchANSI, L" (%i)", GetACP());
	mEncoding[CPI_OEM].uCodePage = GetOEMCP();
	wsprintf(wchOEM, L" (%i)", mEncoding[CPI_OEM].uCodePage);

	g_DOSEncoding = CPI_OEM;

	// Try to set the DOS encoding to DOS-437 if the default OEMCP is not DOS-437
	if (mEncoding[g_DOSEncoding].uCodePage != 437) {
		unsigned int i;
		for (i = CPI_UTF7 + 1; i < COUNTOF(mEncoding); ++i) {
			if (mEncoding[i].uCodePage == 437 && Encoding_IsValid(i)) {
				g_DOSEncoding = i;
				break;
			}
		}
	}
}


int Encoding_MapIniSetting(BOOL bLoad, int iSetting)
{
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
			unsigned int i;
			for (i = CPI_UTF7 + 1; i < COUNTOF(mEncoding); i++) {
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
			return (mEncoding[iSetting].uCodePage);
		}
	}
}


void Encoding_GetLabel(int iEncoding)
{
	if (mEncoding[iEncoding].wchLabel[0] == 0) {
		WCHAR *pwsz;
		WCHAR wch[256] = L"";
		GetString(mEncoding[iEncoding].idsName, wch, COUNTOF(wch));
		if ((pwsz = StrChr(wch, L';'))) {
			if ((pwsz = StrChr(CharNext(pwsz), L';'))) {
				pwsz = CharNext(pwsz);
			}
		}
		if (!pwsz) {
			pwsz = wch;
		}
		StrCpyN(mEncoding[iEncoding].wchLabel, pwsz, COUNTOF(mEncoding[iEncoding].wchLabel));
	}
}


int Encoding_MatchW(LPCWSTR pwszTest)
{
	char tchTest[256];
	WideCharToMultiByte(CP_ACP, 0, pwszTest, -1, tchTest, COUNTOF(tchTest), NULL, NULL);
	return Encoding_MatchA(tchTest);
}


int Encoding_MatchA(char *pchTest)
{
	unsigned int i;
	char chTest[256];
	char *pchSrc = pchTest;
	char *pchDst = chTest;

	*pchDst++ = ',';
	while (*pchSrc) {
		if (IsCharAlphaNumericA(*pchSrc)) {
			*pchDst++ = *CharLowerA(pchSrc);
		}
		pchSrc++;
	}

	*pchDst++ = ',';
	*pchDst = 0;

	for (i = 0; i < COUNTOF(mEncoding); i++) {
		if (StrStrIA(mEncoding[i].pszParseNames, chTest)) {
			CPINFO cpi;
			if ((mEncoding[i].uFlags & NCP_INTERNAL) ||
				(IsValidCodePage(mEncoding[i].uCodePage) && GetCPInfo(mEncoding[i].uCodePage, &cpi))) {
				return i;
			} else {
				return -1;
			}
		}
	}

	return -1;
}


BOOL Encoding_IsValid(int iTestEncoding)
{
	CPINFO cpi;
	if (iTestEncoding >= 0 &&
			iTestEncoding < COUNTOF(mEncoding)) {
		if	((mEncoding[iTestEncoding].uFlags & NCP_INTERNAL) ||
			(IsValidCodePage(mEncoding[iTestEncoding].uCodePage) && GetCPInfo(mEncoding[iTestEncoding].uCodePage, &cpi))) {
			return TRUE;
		}
	}

	return FALSE;
}


typedef struct _ee {
	int		 id;
	WCHAR	 wch[256];
} ENCODINGENTRY, *PENCODINGENTRY;

int CmpEncoding(const void *s1, const void *s2)
{
	return StrCmp(((PENCODINGENTRY)s1)->wch, ((PENCODINGENTRY)s2)->wch);
}

void Encoding_AddToListView(HWND hwnd, int idSel, BOOL bRecodeOnly)
{
	unsigned int i;
	int iSelItem = -1;
	LVITEM lvi;
	WCHAR wchBuf[256];

	PENCODINGENTRY pEE = LocalAlloc(LPTR, COUNTOF(mEncoding) * sizeof(ENCODINGENTRY));
	for (i = 0; i < COUNTOF(mEncoding); i++) {
		pEE[i].id = i;
		GetString(mEncoding[i].idsName, pEE[i].wch, COUNTOF(pEE[i].wch));
	}
	qsort(pEE, COUNTOF(mEncoding), sizeof(ENCODINGENTRY), CmpEncoding);

	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = wchBuf;

	for (i = 0; i < COUNTOF(mEncoding); i++) {
		int id = pEE[i].id;
		if (!bRecodeOnly || (mEncoding[id].uFlags & NCP_RECODE)) {
			CPINFO cpi;
			WCHAR *pwsz;

			lvi.iItem = ListView_GetItemCount(hwnd);
			if ((pwsz = StrChr(pEE[i].wch, L';'))) {
				StrCpyN(wchBuf, CharNext(pwsz), COUNTOF(wchBuf));
				if ((pwsz = StrChr(wchBuf, L';'))) {
					*pwsz = 0;
				}
			} else {
				StrCpyN(wchBuf, pEE[i].wch, COUNTOF(wchBuf));
			}

			if (id == CPI_DEFAULT) {
				StrCatN(wchBuf, wchANSI, COUNTOF(wchBuf));
			} else if (id == CPI_OEM) {
				StrCatN(wchBuf, wchOEM, COUNTOF(wchBuf));
			}

			if ((mEncoding[id].uFlags & NCP_INTERNAL) ||
					(IsValidCodePage(mEncoding[id].uCodePage) &&
					 GetCPInfo(mEncoding[id].uCodePage, &cpi))) {
				lvi.iImage = 0;
			} else {
				lvi.iImage = 1;
			}

			lvi.lParam = (LPARAM)id;
			ListView_InsertItem(hwnd, &lvi);

			if (idSel == id) {
				iSelItem = lvi.iItem;
			}
		}
	}

	LocalFree(pEE);

	if (iSelItem != -1) {
		ListView_SetItemState(hwnd, iSelItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		ListView_EnsureVisible(hwnd, iSelItem, FALSE);
	} else {
		ListView_SetItemState(hwnd, 0, LVIS_FOCUSED, LVIS_FOCUSED);
		ListView_EnsureVisible(hwnd, 0, FALSE);
	}
}


BOOL Encoding_GetFromListView(HWND hwnd, int *pidEncoding)
{
	LVITEM lvi;

	lvi.iItem = ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_SELECTED);
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;

	if (ListView_GetItem(hwnd, &lvi)) {
		if (Encoding_IsValid((int)lvi.lParam)) {
			*pidEncoding = (int)lvi.lParam;
			return TRUE;
		} else {
			MsgBox(MBWARN, IDS_ERR_ENCODINGNA);
		}
	}

	return FALSE;
}


void Encoding_AddToComboboxEx(HWND hwnd, int idSel, BOOL bRecodeOnly)
{
	unsigned int i;
	int iSelItem = -1;
	COMBOBOXEXITEM cbei;
	WCHAR wchBuf[256];

	PENCODINGENTRY pEE = LocalAlloc(LPTR, COUNTOF(mEncoding) * sizeof(ENCODINGENTRY));
	for (i = 0; i < COUNTOF(mEncoding); i++) {
		pEE[i].id = i;
		GetString(mEncoding[i].idsName, pEE[i].wch, COUNTOF(pEE[i].wch));
	}
	qsort(pEE, COUNTOF(mEncoding), sizeof(ENCODINGENTRY), CmpEncoding);

	ZeroMemory(&cbei, sizeof(COMBOBOXEXITEM));

	cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
	cbei.pszText = wchBuf;
	cbei.cchTextMax = COUNTOF(wchBuf);
	cbei.iImage = 0;
	cbei.iSelectedImage = 0;

	for (i = 0; i < COUNTOF(mEncoding); i++) {
		int id = pEE[i].id;
		if (!bRecodeOnly || (mEncoding[id].uFlags & NCP_RECODE)) {
			CPINFO cpi;
			WCHAR *pwsz;

			cbei.iItem = SendMessage(hwnd, CB_GETCOUNT, 0, 0);
			if ((pwsz = StrChr(pEE[i].wch, L';'))) {
				StrCpyN(wchBuf, CharNext(pwsz), COUNTOF(wchBuf));
				if ((pwsz = StrChr(wchBuf, L';'))) {
					*pwsz = 0;
				}
			} else {
				StrCpyN(wchBuf, pEE[i].wch, COUNTOF(wchBuf));
			}

			if (id == CPI_DEFAULT) {
				StrCatN(wchBuf, wchANSI, COUNTOF(wchBuf));
			} else if (id == CPI_OEM) {
				StrCatN(wchBuf, wchOEM, COUNTOF(wchBuf));
			}

			if ((mEncoding[id].uFlags & NCP_INTERNAL) ||
					(IsValidCodePage(mEncoding[id].uCodePage) &&
					 GetCPInfo(mEncoding[id].uCodePage, &cpi))) {
				cbei.iImage = 0;
			} else {
				cbei.iImage = 1;
			}

			cbei.lParam = (LPARAM)id;
			SendMessage(hwnd, CBEM_INSERTITEM, 0, (LPARAM)&cbei);

			if (idSel == id) {
				iSelItem = (int)cbei.iItem;
			}
		}
	}

	LocalFree(pEE);

	if (iSelItem != -1) {
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)iSelItem, 0);
	}
}


BOOL Encoding_GetFromComboboxEx(HWND hwnd, int *pidEncoding)
{
	COMBOBOXEXITEM cbei;

	cbei.iItem = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	cbei.mask = CBEIF_LPARAM;

	if (SendMessage(hwnd, CBEM_GETITEM, 0, (LPARAM)&cbei)) {
		if (Encoding_IsValid((int)cbei.lParam)) {
			*pidEncoding = (int)cbei.lParam;
			return TRUE;
		} else {
			MsgBox(MBWARN, IDS_ERR_ENCODINGNA);
		}
	}

	return FALSE;
}


BOOL IsUnicode(const char *pBuffer, int cb, LPBOOL lpbBOM, LPBOOL lpbReverse)
{
	int i = 0xFFFF;

	BOOL bIsTextUnicode;

	BOOL bHasBOM;
	BOOL bHasRBOM;

	if (!pBuffer || cb < 2) {
		return FALSE;
	}

	if (!bSkipUnicodeDetection) {
		bIsTextUnicode = IsTextUnicode(pBuffer, cb, &i);
	} else {
		bIsTextUnicode = FALSE;
	}

	bHasBOM	 = (*((UNALIGNED PWCHAR)pBuffer) == 0xFEFF);
	bHasRBOM = (*((UNALIGNED PWCHAR)pBuffer) == 0xFFFE);

	if (i == 0xFFFF) { // i doesn't seem to have been modified ...
		i = 0;
	}

	if (bIsTextUnicode || bHasBOM || bHasRBOM ||
			((i & (IS_TEXT_UNICODE_UNICODE_MASK | IS_TEXT_UNICODE_REVERSE_MASK)) &&
			 !((i & IS_TEXT_UNICODE_UNICODE_MASK) && (i & IS_TEXT_UNICODE_REVERSE_MASK)) &&
			 !(i & IS_TEXT_UNICODE_ODD_LENGTH) &&
			 !(i & IS_TEXT_UNICODE_ILLEGAL_CHARS && !(i & IS_TEXT_UNICODE_REVERSE_SIGNATURE)) &&
			 !((i & IS_TEXT_UNICODE_REVERSE_MASK) == IS_TEXT_UNICODE_REVERSE_STATISTICS))) {
		if (lpbBOM)
			*lpbBOM = (bHasBOM || bHasRBOM ||
					   (i & (IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE)))
					  ? TRUE : FALSE;
		if (lpbReverse) {
			*lpbReverse = (bHasRBOM || (i & IS_TEXT_UNICODE_REVERSE_MASK)) ? TRUE : FALSE;
		}
		return TRUE;
	}

	return FALSE;
}


BOOL IsUTF8(const char *pTest, int nLength)
{
	static int byte_class_table[256] = {
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

	static utf8_state state_table[] = {
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
	int i;

	const char *pt = pTest;
	int len = nLength;

	for (i = 0; i < len ; i++, pt++) {
		current = NEXT_STATE(*pt, current);
		if (kERROR == current) {
			break;
		}
	}

	return (current == kSTART) ? TRUE : FALSE;
}


BOOL IsUTF7(const char *pTest, int nLength)
{
	int i;
	const char *pt = pTest;

	for (i = 0; i < nLength; i++) {
		if (*pt & 0x80 || !*pt) {
			return FALSE;
		}
		pt++;
	}

	return TRUE;
}


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
INT UTF8_mbslen_bytes(LPCSTR utf8_string)
{
	INT length = 0;

	while (*utf8_string) {
		INT code_size;
		BYTE byte = (BYTE)(*utf8_string);

		if ((byte <= 0xF7) && (0 != (code_size = utf8_lengths[ byte >> 4 ]))) {
			length += code_size;
			utf8_string += code_size;
		} else {
			/* we got an invalid byte value but need to count it,
				 it will be later ignored during the string conversion */
			//WARN("invalid first byte value 0x%02X in UTF-8 sequence!\n", byte);
			length++;
			utf8_string++;
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
INT UTF8_mbslen(LPCSTR source, INT byte_length)
{
	INT wchar_length = 0;

	while (byte_length > 0) {
		INT code_size;
		BYTE byte = (BYTE)(*source);
		/* UTF-16 can't encode 5-byte and 6-byte sequences, so maximum value
			 for first byte is 11110111. Use lookup table to determine sequence
			 length based on upper 4 bits of first byte */
		if ((byte <= 0xF7) && (0 != (code_size = utf8_lengths[ byte >> 4]))) {
			/* 1 sequence == 1 character */
			wchar_length++;

			if (code_size == 4) {
				wchar_length++;
			}

			source += code_size;				/* increment pointer */
			byte_length -= code_size;		/* decrement counter*/
		} else {
			/*
				 unlike UTF8_mbslen_bytes, we ignore the invalid characters.
				 we only report the number of valid characters we have encountered
				 to match the Windows behavior.
			*/
			//WARN("invalid byte 0x%02X in UTF-8 sequence, skipping it!\n",
			//		 byte);
			source++;
			byte_length--;
		}
	}

	return wchar_length;
}


//=============================================================================
//
// FileVars_IsUTF8()
//
BOOL FileVars_IsUTF8(LPFILEVARS lpfv)
{
	if (lpfv->mask & FV_ENCODING) {
		if (lstrcmpiA(lpfv->tchEncoding, "utf-8") == 0 || lstrcmpiA(lpfv->tchEncoding, "utf8") == 0) {
			return TRUE;
		}
	}

	return FALSE;
}


//=============================================================================
//
// FileVars_IsNonUTF8()
//
BOOL FileVars_IsNonUTF8(LPFILEVARS lpfv)
{
	if (lpfv->mask & FV_ENCODING) {
		if (lstrlenA(lpfv->tchEncoding) &&
			lstrcmpiA(lpfv->tchEncoding, "utf-8") != 0 && lstrcmpiA(lpfv->tchEncoding, "utf8") != 0) {
			return TRUE;
		}
	}

	return FALSE;
}


//=============================================================================
//
// FileVars_IsValidEncoding()
//
BOOL FileVars_IsValidEncoding(LPFILEVARS lpfv)
{
	CPINFO cpi;
	if (lpfv->mask & FV_ENCODING &&
			lpfv->iEncoding >= 0 &&
			lpfv->iEncoding < COUNTOF(mEncoding)) {
		if ((mEncoding[lpfv->iEncoding].uFlags & NCP_INTERNAL) ||
			(IsValidCodePage(mEncoding[lpfv->iEncoding].uCodePage) && GetCPInfo(mEncoding[lpfv->iEncoding].uCodePage, &cpi))) {
			return TRUE;
		}
	}

	return FALSE;
}


//=============================================================================
//
// FileVars_GetEncoding()
//
int FileVars_GetEncoding(LPFILEVARS lpfv)
{
	if (lpfv->mask & FV_ENCODING) {
		return lpfv->iEncoding;
	} else {
		return -1;
	}
}


#pragma warning(pop)

// End of EditEncoding.c
