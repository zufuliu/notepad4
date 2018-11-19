/******************************************************************************
*
*
* Notepad2
*
* Styles.c
*   Scintilla Style Management
*
* See Readme.txt for more information about this source code.
* Please send me your comments to this work.
*
* See License.txt for details about distribution and modification.
*
*                                              (c) Florian Balmer 1996-2011
*                                                  florian.balmer@gmail.com
*                                               http://www.flos-freeware.ch
*
*
******************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <math.h>
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Helpers.h"
#include "Dialogs.h"
#include "SciCall.h"
#include "resource.h"

extern EDITLEXER lexDefault;

extern EDITLEXER lexCPP;
extern EDITLEXER lexCSharp;
extern EDITLEXER lexCSS;
extern EDITLEXER lexJava;
extern EDITLEXER lexJS;
extern EDITLEXER lexJSON;
extern EDITLEXER lexPHP;
extern EDITLEXER lexPython;
extern EDITLEXER lexRuby;
extern EDITLEXER lexSQL;
extern EDITLEXER lexHTML;
extern EDITLEXER lexXML;

extern EDITLEXER lexAS;
extern EDITLEXER lexSmali;
extern EDITLEXER lexASM;
extern EDITLEXER lexASY;
extern EDITLEXER lexAU3;
extern EDITLEXER lexAwk;

extern EDITLEXER lexBatch;

extern EDITLEXER lexCMake;
extern EDITLEXER lexCONF;

extern EDITLEXER lexD;
extern EDITLEXER lexDIFF;

extern EDITLEXER lexFSharp;
extern EDITLEXER lexFortran;

extern EDITLEXER lexGradle;
extern EDITLEXER lexDOT;
extern EDITLEXER lexGo;
extern EDITLEXER lexGroovy;

extern EDITLEXER lexHaXe;

//extern EDITLEXER lexIDL;
extern EDITLEXER lexINI;
extern EDITLEXER lexINNO;

extern EDITLEXER lexJAM;
extern EDITLEXER lexJulia;

extern EDITLEXER lexLaTeX;
extern EDITLEXER lexLisp;
extern EDITLEXER lexLLVM;
extern EDITLEXER lexLua;

extern EDITLEXER lexMake;
extern EDITLEXER lexMatlab;

extern EDITLEXER lexCIL;
extern EDITLEXER lexNsis;

extern EDITLEXER lexPascal;
extern EDITLEXER lexPerl;
extern EDITLEXER lexPS1;

extern EDITLEXER lexRC;

extern EDITLEXER lexScala;
extern EDITLEXER lexBash;

extern EDITLEXER lexTcl;
extern EDITLEXER lexTexinfo;

extern EDITLEXER lexVBS;
extern EDITLEXER lexVerilog;
extern EDITLEXER lexVHDL;
extern EDITLEXER lexVim;
extern EDITLEXER lexVB;

extern EDITLEXER lexANSI;

// This array holds all the lexers...
static const PEDITLEXER pLexArray[NUMLEXERS] = {
	&lexDefault,

	&lexCPP,
	&lexCSharp,
	&lexCSS,
	&lexJava,
	&lexJS,
	&lexJSON,
	&lexPHP,
	&lexPython,
	&lexRuby,
	&lexSQL,
	&lexHTML,
	&lexXML,

	&lexAS,
	&lexSmali,
	&lexASM,
	&lexASY,
	&lexAU3,
	&lexAwk,

	&lexBatch,

	&lexCMake,
	&lexCONF,

	&lexD,
	&lexDIFF,

	&lexFSharp,
	&lexFortran,

	&lexGradle,
	&lexDOT,
	&lexGo,
	&lexGroovy,

	&lexHaXe,

	//&lexIDL,
	&lexINI,
	&lexINNO,

	&lexJAM,
	&lexJulia,

	&lexLaTeX,
	&lexLisp,
	&lexLLVM,
	&lexLua,

	&lexMake,
	&lexMatlab,

	&lexCIL,
	&lexNsis,

	&lexPascal,
	&lexPerl,
	&lexPS1,

	&lexRC,

	&lexScala,
	&lexBash,

	&lexTcl,
	&lexTexinfo,

	&lexVBS,
	&lexVerilog,
	&lexVHDL,
	&lexVim,
	&lexVB,

	&lexANSI
};

#define	NUM_MONO_FONT	6
static LPCWSTR const SysMonoFontName[NUM_MONO_FONT + 1] = {
	L"DejaVu Sans Mono",
	//L"Bitstream Vera Sans Mono",
	L"Consolas",
	L"Source Code Pro",
	L"Monaco",
	L"Inconsolata",
	L"Lucida Console",
	L"Courier New",
};
static int np2MonoFontIndex = NUM_MONO_FONT; // Courier New

// Currently used lexer
PEDITLEXER pLexCurrent = &lexDefault;
int np2LexLangIndex = 0;
UINT8 currentLexKeywordAttr[NUMKEYWORD] = {0};

#define STYLESMODIFIED_NONE			0
#define STYLESMODIFIED_SOME_STYLE	1
#define STYLESMODIFIED_ALL_STYLE	2
#define STYLESMODIFIED_STYLE_MASK	3
#define STYLESMODIFIED_FILE_EXT		4
#define STYLESMODIFIED_WARN_MASK	7
#define STYLESMODIFIED_COLOR		8

#define MAX_CUSTOM_COLOR_COUNT		16
// run tools/CountColor.py on exported scheme file
static const COLORREF defaultCustomColor[MAX_CUSTOM_COLOR_COUNT] = {
	RGB(0xB0, 0x00, 0xB0),	// Constant, Macro, Operator
	RGB(0xFF, 0x80, 0x00),	// Indentation, Preprocessor, Keyword, Attribute, Enumeration, Annotation, Register
	RGB(0x00, 0x80, 0x00),	// String, Comment
	//RGB(0x00, 0x00, 0xFF),	// Keyword, Type Keyword
	//RGB(0xFF, 0x00, 0x00),	// Number
	RGB(0x60, 0x80, 0x60),	// Comment
	RGB(0xA4, 0x60, 0x00),	// Function, Here-doc, Now-doc
	RGB(0x00, 0x80, 0xFF),	// Class, Instruction, Struct
	RGB(0xFF, 0xF1, 0xAB),	// Regex, Backticks
	RGB(0xFF, 0xC0, 0x40),	// Label, Section
	RGB(0x1E, 0x90, 0xFF),	// Interface, Type Keyword
	RGB(0x64, 0x80, 0x00),	// Field, Here-doc
	RGB(0x00, 0x66, 0x33),	// Regex
	RGB(0x40, 0x80, 0x80),	// Doc Comment Tag
	RGB(0x00, 0x80, 0x80),	// String, Verbatim String
	RGB(0xFF, 0x00, 0x80),	// Backticks, Basic Function
	RGB(0x40, 0x80, 0x40),	// Doc Comment
	RGB(0x00, 0x80, 0xC0),	// Build-in Function
	//RGB(0x00, 0x3C, 0xE6),	// Variable
	//RGB(0x00, 0x7F, 0x7F),	// Class, Trait
};
static COLORREF customColor[MAX_CUSTOM_COLOR_COUNT];

static BOOL bCustomColorLoaded = FALSE;
static int iLexerLoadedCount = 0;

BOOL	bUse2ndDefaultStyle;
static UINT fStylesModified = STYLESMODIFIED_NONE;
static BOOL fWarnedNoIniFile = FALSE;
static int	iBaseFontSize = 11*SC_FONT_SIZE_MULTIPLIER; // 11 pt in lexDefault
int		iFontQuality = SC_EFF_QUALITY_LCD_OPTIMIZED;
int		iCaretStyle = 1; // width 1, 0 for block
int		iCaretBlinkPeriod = -1; // system default, 0 for noblink
static int	iDefaultLexer;
static BOOL bAutoSelect;
static int	cxStyleSelectDlg;
static int	cyStyleSelectDlg;
static int	cyStyleCustomizeDlg;
static int	cxStyleCustomizeDlg;

#define ALL_FILE_EXTENSIONS_BYTE_SIZE	((NUMLEXERS * MAX_EDITLEXER_EXT_SIZE) * sizeof(WCHAR))
static LPWSTR g_AllFileExtensions = NULL;

// Notepad2.c
extern int	iEncoding;
extern int	g_DOSEncoding;
extern int	iDefaultCodePage;
extern int	iDefaultCharSet;
extern BOOL	bHighlightCurrentLine;

#define STYLE_MASK_FONT_FACE	(1 << 0)
#define STYLE_MASK_FONT_SIZE	(1 << 1)
#define STYLE_MASK_FORE_COLOR	(1 << 2)
#define STYLE_MASK_BACK_COLOR	(1 << 3)
#define STYLE_MASK_UPPER_LOWER	(1 << 4)
#define STYLE_MASK_CHARSET		(1 << 5)

struct DetailStyle {
	UINT mask;
	int fontSize;
	int foreColor;
	int backColor;
	BOOL bold;
	BOOL italic;
	BOOL underline;
	BOOL strike;
	BOOL eolFilled;
	int upperLower;
	int charset;
	WCHAR fontWide[LF_FACESIZE];
	char fontFace[LF_FACESIZE * kMaxMultiByteCount];
};

//! keep same order as lexDefault
enum DefaultStyleIndex {
	Style_Default,
	Style_LineNumber,
	Style_MatchBrace,
	Style_MatchBraceError,
	Style_ControlCharacter,
	Style_IndentationGuide,
	Style_Selection,
	Style_Whitespace,
	Style_CurrentLine,
	Style_Caret,
	Style_LongLineMarker,
	Style_ExtraLineSpacing,
	Style_FoldingMarker,
	Style_MaxDefaultStyle,
};

static inline int GetDefaultStyleStartIndex(void) {
	return bUse2ndDefaultStyle ? Style_MaxDefaultStyle : Style_Default;
}

static inline int FindDefaultFontIndex(void) {
	for (int i = 0; i < NUM_MONO_FONT; i++) {
		if (IsFontAvailable(SysMonoFontName[i])) {
			return i;
		}
	}
	return NUM_MONO_FONT;
}

static void Style_Alloc(PEDITLEXER pLex) {
	int count = 0;
	while (pLex->Styles[count].rid != 0) {
		++count;
	}

	LPWSTR szStyleBuf = NP2HeapAlloc(count * MAX_EDITSTYLE_VALUE_SIZE * sizeof(WCHAR));
	pLex->iStyleCount = count;
	pLex->iStyleBufSize = count * MAX_EDITSTYLE_VALUE_SIZE * sizeof(WCHAR);
	pLex->szStyleBuf = szStyleBuf;
	for (int i = 0; i < count; i++) {
		pLex->Styles[i].szValue = szStyleBuf + (i * MAX_EDITSTYLE_VALUE_SIZE);
	}
}

void Style_ReleaseResources(void) {
	NP2HeapFree(g_AllFileExtensions);
	for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		if (pLex->szStyleBuf) {
			NP2HeapFree(pLex->szStyleBuf);
		}
	}
}

static void Style_LoadOneEx(PEDITLEXER pLex, IniSection *pIniSection, WCHAR *pIniSectionBuf, int cchIniSection) {
	LoadIniSection(pLex->pszName, pIniSectionBuf, cchIniSection);
	Style_Alloc(pLex);
	const int iStyleCount = pLex->iStyleCount;
	if (!IniSectionParse(pIniSection, pIniSectionBuf)) {
		for (int i = 0; i < iStyleCount; i++) {
			lstrcpy(pLex->Styles[i].szValue, pLex->Styles[i].pszDefault);
		}
	} else {
		for (int i = 0; i < iStyleCount; i++) {
			LPCWSTR value = IniSectionGetValueImpl(pIniSection, pLex->Styles[i].pszName, pLex->Styles[i].iNameLen);
			if (value != NULL) {
				lstrcpyn(pLex->Styles[i].szValue, value, MAX_EDITSTYLE_VALUE_SIZE);
			} else {
				lstrcpy(pLex->Styles[i].szValue, pLex->Styles[i].pszDefault);
			}
		}
	}
	++iLexerLoadedCount;
}

//=============================================================================
//
// Style_Load()
//
void Style_Load(void) {
	IniSection section;
	g_AllFileExtensions = NP2HeapAlloc(ALL_FILE_EXTENSIONS_BYTE_SIZE);
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);

	LoadIniSection(INI_SECTION_NAME_STYLES, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	// 2nd default
	bUse2ndDefaultStyle = IniSectionGetBool(pIniSection, L"Use2ndDefaultStyle", 0);

	// default scheme
	iDefaultLexer = IniSectionGetInt(pIniSection, L"DefaultScheme", 0);
	iDefaultLexer = clamp_i(iDefaultLexer, 0, NUMLEXERS - 1);

	// auto select
	bAutoSelect = IniSectionGetBool(pIniSection, L"AutoSelect", 1);

	// scheme select dlg dimensions
	cxStyleSelectDlg = IniSectionGetInt(pIniSection, L"SelectDlgSizeX", 0);
	cyStyleSelectDlg = IniSectionGetInt(pIniSection, L"SelectDlgSizeY", 0);
	cyStyleCustomizeDlg = IniSectionGetInt(pIniSection, L"CustomizeDlgSizeY", 0);
	cxStyleCustomizeDlg = IniSectionGetInt(pIniSection, L"CustomizeDlgSizeX", 0);

	LoadIniSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);
	for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		pLex->szExtensions = g_AllFileExtensions + (iLexer * MAX_EDITLEXER_EXT_SIZE);
		LPCWSTR value = IniSectionGetValueImpl(pIniSection, pLex->pszName, pLex->iNameLen);
		if (StrIsEmpty(value)) {
			lstrcpy(pLex->szExtensions, pLex->pszDefExt);
		} else {
			lstrcpyn(pLex->szExtensions, value, MAX_EDITLEXER_EXT_SIZE);
		}
	}

	Style_LoadOneEx(&lexDefault, pIniSection, pIniSectionBuf, cchIniSection);
	if (iDefaultLexer != 0) {
		Style_LoadOneEx(pLexArray[iDefaultLexer], pIniSection, pIniSectionBuf, cchIniSection);
	}
	np2MonoFontIndex = FindDefaultFontIndex();

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

static void Style_LoadOne(PEDITLEXER pLex) {
	IniSection section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);
	Style_LoadOneEx(pLex, pIniSection, pIniSectionBuf, cchIniSection);
	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

static void Style_LoadAll(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);

	// Custom colors
	if (!bCustomColorLoaded) {
		bCustomColorLoaded = TRUE;
		CopyMemory(customColor, defaultCustomColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));

		LoadIniSection(INI_SECTION_NAME_CUSTOM_COLORS, pIniSectionBuf, cchIniSection);
		IniSectionParseArray(pIniSection, pIniSectionBuf);

		const int count = min_i(pIniSection->count, MAX_CUSTOM_COLOR_COUNT);
		for (int i = 0; i < count; i++) {
			const IniKeyValueNode *node = &pIniSection->nodeList[i];
			const UINT n = (UINT)(wcstol(node->key, NULL, 10) - 1);
			LPCWSTR wch = node->value;
			if (n < MAX_CUSTOM_COLOR_COUNT && *wch == L'#') {
				int irgb;
				if (HexStrToInt(wch + 1, &irgb)) {
					customColor[n] = RGB((irgb & 0xFF0000) >> 16, (irgb & 0xFF00) >> 8, irgb & 0xFF);
				}
			}
		}
	}

	for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		if (!pLex->szStyleBuf) {
			Style_LoadOneEx(pLex, pIniSection, pIniSectionBuf, cchIniSection);
		}
	}

	iLexerLoadedCount = NUMLEXERS;
	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
//	Style_Save()
//
void Style_Save(void) {
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	// Custom colors
	if (fStylesModified & STYLESMODIFIED_COLOR) {
		for (unsigned int i = 0; i < MAX_CUSTOM_COLOR_COUNT; i++) {
			const COLORREF color = customColor[i];
			if (color != defaultCustomColor[i]) {
				WCHAR tch[4];
				WCHAR wch[16];
				wsprintf(tch, L"%02u", i + 1);
				wsprintf(wch, L"#%02X%02X%02X", (int)GetRValue(color), (int)GetGValue(color), (int)GetBValue(color));
				IniSectionSetString(pIniSection, tch, wch);
			}
		}

		SaveIniSection(INI_SECTION_NAME_CUSTOM_COLORS, pIniSectionBuf);
		ZeroMemory(pIniSectionBuf, cchIniSection);
		pIniSection->next = pIniSectionBuf;
	}

	// auto select
	IniSectionSetBoolEx(pIniSection, L"Use2ndDefaultStyle", bUse2ndDefaultStyle, 0);

	// default scheme
	IniSectionSetIntEx(pIniSection, L"DefaultScheme", iDefaultLexer, 0);

	// auto select
	IniSectionSetBoolEx(pIniSection, L"AutoSelect", bAutoSelect, 1);

	// scheme select dlg dimensions
	IniSectionSetInt(pIniSection, L"SelectDlgSizeX", cxStyleSelectDlg);
	IniSectionSetInt(pIniSection, L"SelectDlgSizeY", cyStyleSelectDlg);
	IniSectionSetInt(pIniSection, L"CustomizeDlgSizeY", cyStyleCustomizeDlg);
	IniSectionSetInt(pIniSection, L"CustomizeDlgSizeX", cxStyleCustomizeDlg);

	SaveIniSection(INI_SECTION_NAME_STYLES, pIniSectionBuf);

	if (fStylesModified & STYLESMODIFIED_FILE_EXT) {
		ZeroMemory(pIniSectionBuf, cchIniSection);
		pIniSection->next = pIniSectionBuf;
		for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			IniSectionSetStringEx(pIniSection, pLex->pszName, pLex->szExtensions, pLex->pszDefExt);
		}
		SaveIniSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf);
	}

	if (fStylesModified & STYLESMODIFIED_STYLE_MASK) {
		for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			if (!pLex->bStyleChanged) {
				continue;
			}

			ZeroMemory(pIniSectionBuf, cchIniSection);
			pIniSection->next = pIniSectionBuf;
			const int iStyleCount = pLex->iStyleCount;
			for (int i = 0; i < iStyleCount; i++) {
				IniSectionSetStringEx(pIniSection, pLex->Styles[i].pszName, pLex->Styles[i].szValue, pLex->Styles[i].pszDefault);
			}
			// delete this section if nothing changed
			SaveIniSection(pLex->pszName, StrIsEmpty(pIniSectionBuf) ? NULL : pIniSectionBuf);
			pLex->bStyleChanged = FALSE;
		}
	}

	fStylesModified = STYLESMODIFIED_NONE;
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
// Style_Import()
//
BOOL Style_Import(HWND hwnd) {
	WCHAR szFile[MAX_PATH * 2] = L"";
	WCHAR szFilter[256];

	GetString(IDS_FILTER_INI, szFilter, COUNTOF(szFilter));
	PrepareFilterStr(szFilter);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hwndOwner	= hwnd;
	ofn.lpstrFilter	= szFilter;
	ofn.lpstrFile	= szFile;
	ofn.lpstrDefExt	= L"ini";
	ofn.nMaxFile	= COUNTOF(szFile);
	ofn.Flags		= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT
					  | OFN_PATHMUSTEXIST | OFN_SHAREAWARE /*| OFN_NODEREFERENCELINKS*/;

	if (GetOpenFileName(&ofn)) {
		IniSection section;
		WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
		const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
		IniSection *pIniSection = &section;

		IniSectionInit(pIniSection, 128);
		if (GetPrivateProfileSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, cchIniSection, szFile)) {
			if (IniSectionParse(pIniSection, pIniSectionBuf)) {
				for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
					PEDITLEXER pLex = pLexArray[iLexer];
					LPCWSTR value = IniSectionGetValueImpl(pIniSection, pLex->pszName, pLex->iNameLen);
					if (StrNotEmpty(value)) {
						lstrcpyn(pLex->szExtensions, value, MAX_EDITLEXER_EXT_SIZE);
					}
					if (pIniSection->count == 0) {
						break;
					}
				}
			}
		}

		for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			if (GetPrivateProfileSection(pLex->pszName, pIniSectionBuf, cchIniSection, szFile)) {
				if (!IniSectionParse(pIniSection, pIniSectionBuf)) {
					continue;
				}
				const int iStyleCount = pLex->iStyleCount;
				for (int i = 0; i < iStyleCount; i++) {
					LPCWSTR value = IniSectionGetValueImpl(pIniSection, pLex->Styles[i].pszName, pLex->Styles[i].iNameLen);
					if (value != NULL) {
						lstrcpyn(pLex->Styles[i].szValue, value, MAX_EDITSTYLE_VALUE_SIZE);
					}
					if (pIniSection->count == 0) {
						break;
					}
				}
			}
		}

		IniSectionFree(pIniSection);
		NP2HeapFree(pIniSectionBuf);
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// Style_Export()
//
BOOL Style_Export(HWND hwnd) {
	WCHAR szFile[MAX_PATH * 2] = L"";
	WCHAR szFilter[256];

	GetString(IDS_FILTER_INI, szFilter, COUNTOF(szFilter));
	PrepareFilterStr(szFilter);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner	= hwnd;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile	= szFile;
	ofn.lpstrDefExt = L"ini";
	ofn.nMaxFile	= COUNTOF(szFile);
	ofn.Flags		= /*OFN_FILEMUSTEXIST |*/ OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT
			| OFN_PATHMUSTEXIST | OFN_SHAREAWARE /*| OFN_NODEREFERENCELINKS*/ | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn)) {
		DWORD dwError = ERROR_SUCCESS;
		IniSectionOnSave section;
		WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
		const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
		IniSectionOnSave *pIniSection = &section;

		pIniSection->next = pIniSectionBuf;
		for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			IniSectionSetString(pIniSection, pLex->pszName, pLex->szExtensions);
		}
		if (!WritePrivateProfileSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, szFile)) {
			dwError = GetLastError();
		}

		for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			ZeroMemory(pIniSectionBuf, cchIniSection);
			pIniSection->next = pIniSectionBuf;
			const int iStyleCount = pLex->iStyleCount;
			for (int i = 0; i < iStyleCount; i++) {
				IniSectionSetString(pIniSection, pLex->Styles[i].pszName, pLex->Styles[i].szValue);
			}
			if (!WritePrivateProfileSection(pLex->pszName, pIniSectionBuf, szFile)) {
				dwError = GetLastError();
			}
		}
		NP2HeapFree(pIniSectionBuf);

		if (dwError != ERROR_SUCCESS) {
			MsgBox(MBINFO, IDS_EXPORT_FAIL, szFile);
		}
		return TRUE;
	}
	return FALSE;
}

static void Style_ResetAll(void) {
	CopyMemory(customColor, defaultCustomColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));
	for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		lstrcpy(pLex->szExtensions, pLex->pszDefExt);
		pLex->bStyleChanged = TRUE;
		const int iStyleCount = pLex->iStyleCount;
		for (int i = 0; i < iStyleCount; i++) {
			lstrcpy(pLex->Styles[i].szValue, pLex->Styles[i].pszDefault);
		}
	}

	fStylesModified |= STYLESMODIFIED_ALL_STYLE | STYLESMODIFIED_FILE_EXT | STYLESMODIFIED_COLOR;
}

void Style_OnDPIChanged(HWND hwnd) {
	Style_SetLexer(hwnd, pLexCurrent);
}

void Style_UpdateCaret(HWND hwnd) {
	// caret style and width
	if (iCaretStyle == 0) {
		SendMessage(hwnd, SCI_SETCARETSTYLE, CARETSTYLE_BLOCK, 0);
	} else {
		SendMessage(hwnd, SCI_SETCARETSTYLE, CARETSTYLE_LINE, 0);
		SendMessage(hwnd, SCI_SETCARETWIDTH, iCaretStyle, 0);
	}

	const int iValue = (iCaretBlinkPeriod < 0)? (int)GetCaretBlinkTime() : iCaretBlinkPeriod;
	SendMessage(hwnd, SCI_SETCARETPERIOD, iValue, 0);
}

void Style_UpdateLexerKeywordAttr(PEDITLEXER pLexNew) {
	UINT8 *attr = currentLexKeywordAttr;
	memset(currentLexKeywordAttr, 0, sizeof(currentLexKeywordAttr));

	// Code Snippet
	attr[NUMKEYWORD - 1] = KeywordAttr_NoLexer;

	switch (pLexNew->rid) {
	case NP2LEX_CPP:
		attr[2] = KeywordAttr_NoAutoComp;	// Preprocessor
		attr[3] = KeywordAttr_NoAutoComp;	// Directive
		attr[11] = KeywordAttr_NoAutoComp;	// Assembler Intruction
		attr[12] = KeywordAttr_NoAutoComp;	// Assembler Register
		attr[13] = KeywordAttr_NoLexer;		// C Function
		attr[14] = KeywordAttr_NoLexer;		// C++ Function
		break;
	case NP2LEX_JAVA:
		attr[10] = KeywordAttr_NoLexer;		// Package
		break;
	case NP2LEX_CSHARP:
		attr[2] = KeywordAttr_NoAutoComp;	// Preprocessor
		break;
	case NP2LEX_JS:
		//attr[1] = KeywordAttr_NoAutoComp;	// Reserved Word
		attr[9] = KeywordAttr_NoLexer;		// Function
		attr[10] = KeywordAttr_NoLexer;		// Property
		attr[11] = KeywordAttr_NoLexer;		// Method
		break;
	case NP2LEX_RC:
		attr[2] = KeywordAttr_NoAutoComp;	// Preprocessor
		break;
	case NP2LEX_D:
		attr[2] = KeywordAttr_NoAutoComp;	// Preprocessor
		attr[11] = KeywordAttr_NoAutoComp;	// Assembler Intruction
		attr[12] = KeywordAttr_NoAutoComp;	// Assembler Register
		break;
	case NP2LEX_HAXE:
		attr[2] = KeywordAttr_NoAutoComp;	// Preprocessor
		break;
	case NP2LEX_VB:
	case NP2LEX_VBS:
		attr[0] = KeywordAttr_MakeLower;
		attr[1] = KeywordAttr_MakeLower;
		attr[2] = KeywordAttr_MakeLower;
		attr[3] = KeywordAttr_MakeLower | KeywordAttr_NoAutoComp; // Preprocessor
		attr[4] = KeywordAttr_MakeLower;
		attr[5] = KeywordAttr_MakeLower;
		attr[6] = KeywordAttr_MakeLower;
		break;
	case NP2LEX_HTML:
		attr[2] = KeywordAttr_MakeLower;	// VBScript
		break;
	case NP2LEX_SQL:
		attr[6] = KeywordAttr_NoLexer;		// Upper Case Keyword
		attr[7] = KeywordAttr_NoLexer;		// Upper Case Type
		attr[8] = KeywordAttr_NoLexer;		// Upper Case Function
		break;
	case NP2LEX_PHP:
		attr[9] = KeywordAttr_NoLexer;		// Function
		attr[10] = KeywordAttr_NoLexer;		// Field
		attr[11] = KeywordAttr_NoLexer;		// Method
		attr[12] = KeywordAttr_NoLexer;		// Tag
		attr[13] = KeywordAttr_NoLexer;		// String Constant
		break;
	case NP2LEX_PYTHON:
		attr[3] = KeywordAttr_NoAutoComp;	// Decorator
		attr[9] = KeywordAttr_NoLexer;		// Module
		attr[10] = KeywordAttr_NoLexer;		// Method
		attr[11] = KeywordAttr_NoLexer;		// Constant
		attr[12] = KeywordAttr_NoLexer;		// Attribute
		break;
	case NP2LEX_BATCH:
		attr[6] = KeywordAttr_NoLexer;		// Upper Case Keyword
		break;
	case NP2LEX_AU3:
		break;
	case NP2LEX_INNO:
		break;
	case NP2LEX_NSIS:
		attr[0] = KeywordAttr_MakeLower;
		break;
	default:
		break;
	}
}

static void Style_Parse(struct DetailStyle *style, LPCWSTR lpszStyle);
static void Style_SetParsed(HWND hwnd, const struct DetailStyle *style, int iStyle);
static inline void Style_SetDefaultStyle(HWND hwnd, int index) {
	Style_SetStyles(hwnd, lexDefault.Styles[index].iStyle, lexDefault.Styles[index].szValue);
}

//=============================================================================
// set current lexer
// Style_SetLexer()
//
void Style_SetLexer(HWND hwnd, PEDITLEXER pLexNew) {
	int iValue;
	char msg[10];

	// Select default if NULL is specified
	if (!pLexNew) {
		np2LexLangIndex = 0;
		pLexNew = pLexArray[iDefaultLexer];
	}
	if (!pLexNew->szStyleBuf) {
		Style_LoadOne(pLexNew);
	}

	// Lexer
	SendMessage(hwnd, SCI_SETLEXER, pLexNew->iLexer, 0);
	int rid = pLexNew->rid;
	if (rid == NP2LEX_MATLAB) {
		if (np2LexLangIndex == IDM_LANG_OCTAVE) {
			rid = NP2LEX_OCTAVE;
		} else if (np2LexLangIndex == IDM_LANG_SCILAB) {
			rid = NP2LEX_SCILAB;
		}
	}
	_itoa(rid - 63000, msg, 10);
	SciCall_SetProperty("lexer.lang.type", msg);

	// Code folding
	SciCall_SetProperty("fold", "1");
	SciCall_SetProperty("fold.foldsyntaxbased", "1");
	SciCall_SetProperty("fold.comment", "1");
	SciCall_SetProperty("fold.preprocessor", "1");
	SciCall_SetProperty("fold.compact", "0");

	switch (rid) {
	case NP2LEX_HTML:
	case NP2LEX_XML:
		SciCall_SetProperty("fold.html", "1");
		SciCall_SetProperty("fold.hypertext.comment", "1");
		SciCall_SetProperty("fold.hypertext.heredoc", "1");
		break;

	case NP2LEX_CSS:
		SciCall_SetProperty("lexer.css.scss.language", ((np2LexLangIndex == IDM_LANG_SCSS)? "1" : "0"));
		SciCall_SetProperty("lexer.css.less.language", ((np2LexLangIndex == IDM_LANG_LESS)? "1" : "0"));
		SciCall_SetProperty("lexer.css.hss.language", ((np2LexLangIndex == IDM_LANG_HSS)? "1" : "0"));
		break;

	case NP2LEX_BASH:
		SciCall_SetProperty("lexer.bash.csh.language", ((np2LexLangIndex == IDM_LANG_CSHELL)? "1" : "0"));
		break;
	}

	Style_UpdateLexerKeywords(pLexNew);
	Style_UpdateLexerKeywordAttr(pLexNew);
	// Add keyword lists
	for (int i = 0; i < NUMKEYWORD; i++) {
		const char *pKeywords = pLexNew->pKeyWords->pszKeyWords[i];
		if (StrNotEmptyA(pKeywords) && !(currentLexKeywordAttr[i] & KeywordAttr_NoLexer)) {
			if (currentLexKeywordAttr[i] & KeywordAttr_MakeLower) {
				const size_t len = strlen(pKeywords);
				char *lowerKeywords = NP2HeapAlloc(len + 1);
				char *p = lowerKeywords;
				CopyMemory(lowerKeywords, pKeywords, len);
				while (*p) {
					if (*p >= 'A' && *p <= 'Z') {
						*p += 'a' - 'A';
					}
					++p;
				}
				SendMessage(hwnd, SCI_SETKEYWORDS, i, (LPARAM)lowerKeywords);
				NP2HeapFree(lowerKeywords);
			} else {
				SendMessage(hwnd, SCI_SETKEYWORDS, i, (LPARAM)pKeywords);
			}
		} else {
			SendMessage(hwnd, SCI_SETKEYWORDS, i, (LPARAM)"");
		}
	}

	// Use 2nd default style
	const int iIdx = GetDefaultStyleStartIndex();
	// Font quality setup
	SendMessage(hwnd, SCI_SETFONTQUALITY, iFontQuality, 0);

	// Clear
	SendMessage(hwnd, SCI_CLEARDOCUMENTSTYLE, 0, 0);

	// Default Values are always set
	SendMessage(hwnd, SCI_STYLERESETDEFAULT, 0, 0);
	SendMessage(hwnd, SCI_STYLESETCHARACTERSET, STYLE_DEFAULT, DEFAULT_CHARSET);

	Style_StrGetSizeEx(lexDefault.Styles[Style_Default + iIdx].szValue, &iBaseFontSize);
	Style_SetDefaultStyle(hwnd, Style_Default + iIdx);

	// Auto-select codepage according to charset
	//Style_SetACPfromCharSet(hwnd);

	if (!Style_StrGetColor(TRUE, lexDefault.Styles[Style_Default + iIdx].szValue, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETFORE, STYLE_DEFAULT, GetSysColor(COLOR_WINDOWTEXT));
	}
	if (!Style_StrGetColor(FALSE, lexDefault.Styles[Style_Default + iIdx].szValue, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETBACK, STYLE_DEFAULT, GetSysColor(COLOR_WINDOW));
	}

	SendMessage(hwnd, SCI_STYLECLEARALL, 0, 0);

	Style_SetDefaultStyle(hwnd, Style_LineNumber + iIdx);
	Style_SetDefaultStyle(hwnd, Style_MatchBrace + iIdx);
	Style_SetDefaultStyle(hwnd, Style_MatchBraceError + iIdx);
	if (rid != NP2LEX_ANSI) {
		Style_SetDefaultStyle(hwnd, Style_ControlCharacter + iIdx);
	}
	Style_SetDefaultStyle(hwnd, Style_IndentationGuide + iIdx);

	if (Style_StrGetColor(TRUE, lexDefault.Styles[Style_Selection + iIdx].szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETSELFORE, TRUE, iValue);
		SendMessage(hwnd, SCI_SETADDITIONALSELFORE, iValue, 0);
	} else {
		SendMessage(hwnd, SCI_SETSELFORE, 0, 0);
		SendMessage(hwnd, SCI_SETADDITIONALSELFORE, 0, 0);
	}
	if (Style_StrGetColor(FALSE, lexDefault.Styles[Style_Selection + iIdx].szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETSELBACK, TRUE, iValue);
		SendMessage(hwnd, SCI_SETADDITIONALSELBACK, iValue, 0);
	} else {
		SendMessage(hwnd, SCI_SETSELBACK, TRUE, RGB(0xC0, 0xC0, 0xC0));
		SendMessage(hwnd, SCI_SETADDITIONALSELBACK, RGB(0xC0, 0xC0, 0xC0), 0);
	}

	if (Style_StrGetAlpha(lexDefault.Styles[Style_Selection + iIdx].szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETSELALPHA, iValue, 0);
		SendMessage(hwnd, SCI_SETADDITIONALSELALPHA, iValue, 0);
	} else {
		SendMessage(hwnd, SCI_SETSELALPHA, SC_ALPHA_NOALPHA, 0);
		SendMessage(hwnd, SCI_SETADDITIONALSELALPHA, SC_ALPHA_NOALPHA, 0);
	}

	if (StrStr(lexDefault.Styles[Style_Selection + iIdx].szValue, L"eolfilled")) {
		SendMessage(hwnd, SCI_SETSELEOLFILLED, 1, 0);
	} else {
		SendMessage(hwnd, SCI_SETSELEOLFILLED, 0, 0);
	}
	if (Style_StrGetColor(TRUE, lexDefault.Styles[Style_Whitespace + iIdx].szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETWHITESPACEFORE, TRUE, iValue);
	} else {
		SendMessage(hwnd, SCI_SETWHITESPACEFORE, 0, 0);
	}
	if (Style_StrGetColor(FALSE, lexDefault.Styles[Style_Whitespace + iIdx].szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETWHITESPACEBACK, TRUE, iValue);
	} else {
		SendMessage(hwnd, SCI_SETWHITESPACEBACK, 0, 0);
	}

	// whitespace dot size
	iValue = 1;
	Style_StrGetSize(lexDefault.Styles[Style_Whitespace + iIdx].szValue, &iValue);
	iValue = clamp_i(iValue, 0, 5);
	SendMessage(hwnd, SCI_SETWHITESPACESIZE, iValue, 0);

	Style_SetCurrentLineBackground(hwnd);
	Style_UpdateCaret(hwnd);
	// caret fore
	if (!Style_StrGetColor(TRUE, lexDefault.Styles[Style_Caret + iIdx].szValue, &iValue)) {
		iValue = GetSysColor(COLOR_WINDOWTEXT);
	}
	if (!VerifyContrast(iValue, (COLORREF)SendMessage(hwnd, SCI_STYLEGETBACK, 0, 0))) {
		iValue = (int)SendMessage(hwnd, SCI_STYLEGETFORE, 0, 0);
	}
	SendMessage(hwnd, SCI_SETCARETFORE, iValue, 0);
	SendMessage(hwnd, SCI_SETADDITIONALCARETFORE, iValue, 0);

	Style_SetLongLineColors(hwnd);

	// Extra Line Spacing
	if (rid != NP2LEX_ANSI && Style_StrGetSize(lexDefault.Styles[Style_ExtraLineSpacing + iIdx].szValue, &iValue)) {
		int iAscent = 0;
		int iDescent = 0;
		iValue = clamp_i(iValue, 0, 64);
		if ((iValue & 1)) { // iValue % 2
			iAscent++;
			iValue--;
		}
		iAscent += iValue / 2;
		iDescent += iValue / 2;
		SendMessage(hwnd, SCI_SETEXTRAASCENT, iAscent, 0);
		SendMessage(hwnd, SCI_SETEXTRADESCENT, iDescent, 0);
	} else {
		SendMessage(hwnd, SCI_SETEXTRAASCENT, 0, 0);
		SendMessage(hwnd, SCI_SETEXTRADESCENT, 0, 0);
	}

	// set folding style; braces are for scoping only
	{
		static const int iMarkerIDs[] = {
			SC_MARKNUM_FOLDEROPEN,
			SC_MARKNUM_FOLDER,
			SC_MARKNUM_FOLDERSUB,
			SC_MARKNUM_FOLDERTAIL,
			SC_MARKNUM_FOLDEREND,
			SC_MARKNUM_FOLDEROPENMID,
			SC_MARKNUM_FOLDERMIDTAIL
		};

		const COLORREF clrBack = SciCall_StyleGetBack(STYLE_DEFAULT);
		COLORREF clrFore, clrFill;

		if (Style_StrGetColor(TRUE, lexDefault.Styles[Style_FoldingMarker + iIdx].szValue, &iValue)) {
			clrFore = iValue;
		} else {
			clrFore = iIdx ? RGB(0x80, 0x80, 0xFF) : RGB(0x80, 0x80, 0x80);
		}
		if (Style_StrGetColor(FALSE, lexDefault.Styles[Style_FoldingMarker + iIdx].szValue, &iValue)) {
			clrFill = iValue;
		} else {
			clrFill = iIdx ? RGB(0xAD, 0xD8, 0xE6) : RGB(0xD3, 0xD3, 0xD3);
		}

		SciCall_SetFoldMarginColour(TRUE, clrBack);
		SciCall_SetFoldMarginHiColour(TRUE, clrBack);
#if 0	// use gray fold color
		COLORREF clrFore = SciCall_StyleGetFore(STYLE_DEFAULT);
		// Marker fore/back colors
		// Set marker color to the average of clrFore and clrBack
		clrFore =	(((clrFore & 0xFF0000) + (clrBack & 0xFF0000)) >> 1 & 0xFF0000) |
					(((clrFore & 0x00FF00) + (clrBack & 0x00FF00)) >> 1 & 0x00FF00) |
					(((clrFore & 0x0000FF) + (clrBack & 0x0000FF)) >> 1 & 0x0000FF);

		// Rounding hack for pure white against pure black
		if (clrFore == 0x7F7F7F) {
			clrFore = 0x808080;
		}
#endif

		for (unsigned int i = 0; i < COUNTOF(iMarkerIDs); ++i) {
			const int marker = iMarkerIDs[i];
			SciCall_MarkerSetBack(marker, clrFore);
			SciCall_MarkerSetFore(marker, clrBack);
		}
		SciCall_MarkerSetFore(SC_MARKNUM_FOLDER, clrFill);
		SciCall_MarkerSetFore(SC_MARKNUM_FOLDEREND, clrFill);
	} // end set folding style

	if (SendMessage(hwnd, SCI_GETINDENTATIONGUIDES, 0, 0) != SC_IV_NONE) {
		Style_SetIndentGuides(hwnd, TRUE);
	}

	if (rid != NP2LEX_DEFAULT) {
		struct DetailStyle style;
		const int iStyleCount = pLexNew->iStyleCount;
		for (int i = 0; i < iStyleCount; i++) {
			int iStyle = pLexNew->Styles[i].iStyle;
			LPCWSTR const szValue = pLexNew->Styles[i].szValue;
			if (iStyle > 0xFF) {
				Style_Parse(&style, szValue);
				do {
					Style_SetParsed(hwnd, &style, iStyle & 0xFF);
					iStyle >>= 8;
				} while (iStyle);
			} else {
				Style_SetStyles(hwnd, iStyle, szValue);
			}
		}
	}

	SendMessage(hwnd, SCI_COLOURISE, 0, - 1);

	// Save current lexer
	pLexCurrent = pLexNew;
	UpdateStatusBarCache(STATUS_LEXER);
	UpdateLineNumberWidth();
	UpdateFoldMarginWidth();
	UpdateStatusbar();
}

//=============================================================================
// find lexer from script interpreter
// Style_SniffShebang()
//
PEDITLEXER Style_SniffShebang(char *pchText) {
	if (pchText[0] == '#' && pchText[1] == '!') {
		size_t len = 0;
		char *pch = pchText + 2;
		char *name;
		while (*pch == ' ' || *pch == '\t') {
			pch++;
		}
		name = pch;
		while (*pch && *pch != ' ' && *pch != '\t' && *pch != '\r' && *pch != '\n') {
			len = *pch == '\\' || *pch == '/';
			pch++;
			name = len ? pch : name;
		}
		if ((pch - name) >= 3 && (_strnicmp(name, "env", 3) == 0 || _strnicmp(name, "winpty", 6) == 0)) {
			while (*pch == ' ' || *pch == '\t') {
				pch++;
			}
			name = pch;
			while (*pch && *pch != ' ' && *pch != '\t' && *pch != '\r' && *pch != '\n') {
				len = *pch == '\\' || *pch == '/';
				pch++;
				name = len ? pch : name;
			}
		}

		*pch = 0;
		len = pch - name;
		pch = name;
		while (*pch) {
			// to lower
			*pch |= 32;
			if (*pch < 'a' || *pch > 'z') {
				*pch = 0;
				len = pch - name;
				break;
			}
			pch++;
		}

		if (len >= 4) {
			if (len >= 5) {
				if (!strncmp(name, "python", 5)) {
					return &lexPython;
				}
				if (!strncmp(name, "groovy", 5)) {
					return &lexGroovy;
				}
				if (!strncmp(name, "scala", 5)) {
					return &lexScala;
				}
			}

			if (!strncmp(name, "bash", 4) || !strncmp(name, "dash", 4)) {
				return &lexBash;
			}
			if (!strncmp(name, "tcsh", 4)) {
				np2LexLangIndex = IDM_LANG_CSHELL;
				return &lexBash;
			}
			if (!strncmp(name, "perl", 4)) {
				return &lexPerl;
			}
			if (!strncmp(name, "ruby", 4)) {
				return &lexRuby;
			}
			if (!strncmp(name, "gawk", 4) || !strncmp(name, "nawk", 4)) {
				return &lexAwk;
			}
			if (!strncmp(name, "node", 4)) {
				return &lexJS;
			}
			if (!strncmp(name, "wish", 4)) {
				return &lexTcl;
			}
			if (!strncmp(name, "wlua", 4)) {
				return &lexLua;
			}
		}
		if (len >= 3) {
			if (!strncmp(name, "awk", 3)) {
				return &lexAwk;
			}
			if (!strncmp(name, "lua", 3)) {
				return &lexLua;
			}
			if (!strncmp(name, "php", 3)) {
				//np2LexLangIndex = IDM_LANG_PHP;
				return &lexPHP;
			}
			if (!strncmp(name, "tcl", 3)) {
				return &lexTcl;
			}
			if (!strncmp(name, "ash", 3) || !strncmp(name, "zsh", 3) || !strncmp(name, "ksh", 3)) {
				return &lexBash;
			}
			if (!strncmp(name, "csh", 3)) {
				np2LexLangIndex = IDM_LANG_CSHELL;
				return &lexBash;
			}
			if (!strncmp(name, "ipy", 3)) {
				return &lexPython;
			}
		}
		if (len >= 2) {
			if (!strncmp(name, "sh", 2)) {
				return &lexBash;
			}
			if (!strncmp(name, "py", 2)) {
				return &lexPython;
			}
			if (!strncmp(name, "go", 2)) {
				return &lexGo;
			}
		}
	}
	return NULL;
}

//=============================================================================
// find lexer from <!DOCTYPE type PUBLIC > or <%@ Page Language="lang" %>
// Style_GetDocTypeLanguage()
//
int Style_GetDocTypeLanguage(void) {
	char *p;
	char tchText[4096] = ""; // maybe contains header comments
	SendMessage(hwndEdit, SCI_GETTEXT, COUNTOF(tchText) - 1, (LPARAM)tchText);

	// check DOCTYPE
	if ((p = StrStrA(tchText, "!DOCTYPE")) != NULL) {
		p += 9;
		while (*p == ' ' || *p == '=' || *p == '\"') {
			p++;
		}
		//if (!_strnicmp(p, "html", 4))
		//	return IDM_LANG_WEB;
		if (!strncmp(p, "struts", 6) || !strncmp(p, "xwork", 5) || !strncmp(p, "validators", 10)) {
			return IDM_LANG_STRUTS;
		}
		if (!strncmp(p, "hibernate", 9)) {
			p += 9;
			if (*p == '-') {
				p++;
			}
			if (*p == 'm') {
				return IDM_LANG_HIB_MAP;
			}
			return IDM_LANG_HIB_CFG;
		}
		//if (!strncmp(p, "plist", 5))
		//	return IDM_LANG_PROPERTY_LIST;
		if (!strncmp(p, "schema", 6)) {
			return IDM_LANG_XSD;
		}
		if (!strncmp(p, "jboss", 5)) {
			return IDM_LANG_JBOSS;
		}
		if (!strncmp(p, "beans", 5)) {
			return IDM_LANG_SPRING_BEANS;
		}
		if (!_strnicmp(p, "module", 6)) {
			return IDM_LANG_CHECKSTYLE;
		}
	}

	if (StrStrA(tchText, "<?php")) {
		return IDM_LANG_PHP;
	}
	// check Language
	if ((p = StrStrA(tchText, "<%@")) != NULL && (p = StrStrIA(p + 3, "Language")) != NULL) {
		p += 9;
		while (*p == ' ' || *p == '=' || *p == '\"') {
			p++;
		}
		if (!_strnicmp(p, "C#", 2)) {
			return IDM_LANG_ASPX_CS;
		}
		if (!_strnicmp(p, "VBScript", 7)) {
			return IDM_LANG_ASP_VBS;
		}
		if (!_strnicmp(p, "VB", 2)) {
			return IDM_LANG_ASPX_VB;
		}
		if (!_strnicmp(p, "JScript", 7)) {
			return IDM_LANG_ASP_JS;
		}
		if (!_strnicmp(p, "Java", 4)) {
			return IDM_LANG_JSP;
		}
	}

	// find root tag
	p = tchText;
	while (p - tchText < (ptrdiff_t)COUNTOF(tchText)) {
		if ((p = strchr(p, '<')) == NULL) {
			return 0;
		}
		if (!strncmp(p, "<!--", 4)) {
			p += 4;
			if ((p = StrStrA(p, "-->")) != NULL) {
				p += 3;
			} else {
				return 0;
			}
		} else if (!strncmp(p, "<?", 2) || !strncmp(p, "<!", 2)) {
			p += 2;
			if ((p = strchr(p, '>')) != NULL) {
				p++;
			} else {
				return 0;
			}
		} else {
			break;
		}
	}
	if (*p == '<') {
		p++;
		if (!isalpha((unsigned char)(*p))) {
			return 0;
		}
	} else {
		return 0;
	}

	char *pb = p;
	//if (!_strnicmp(p, "html", 4))
	//	return IDM_LANG_WEB;
	if (!strncmp(p, "schema", 6)) {
		return IDM_LANG_XSD;
	}
	//if (!strncmp(p, "schema", 6) || !strncmp(p, "xsd:schema", 10) || !strncmp(p, "xs:schema", 9))
	//	return IDM_LANG_XSD;
	//if (!strncmp(p, "xsl:stylesheet", 14))
	//	return IDM_LANG_XSLT;

	if (!strncmp(p, "project", 7)) {
		return IDM_LANG_ANT_BUILD;
	}
	//if (!strncmp(p, "project", 7)) {
	//	p += 7;
	//	if (StrStrA(p, "maven") && StrStrA(p, "POM"))
	//		return IDM_LANG_MAVEN_POM;
	//	return IDM_LANG_ANT_BUILD;
	//}
	if (!strncmp(p, "settings", 8)) {
		return IDM_LANG_MAVEN_SETTINGS;
	}
	if (!strncmp(p, "ivy", 3)) {
		if (*(p + 3) == '-') {
			return IDM_LANG_IVY_MODULE;
		}
		return IDM_LANG_IVY_SETTINGS;
	}
	if (!strncmp(p, "ruleset", 7)) {
		return IDM_LANG_PMD_RULESET;
	}
	if (!strncmp(p, "module", 6)) {
		return IDM_LANG_CHECKSTYLE;
	}

	//if (!strncmp(p, "Server"))
	//	return IDM_LANG_TOMCAT;
	//if (!strncmp(p, "web-app"))
	//	return IDM_LANG_WEB_JAVA;
	if (!strncmp(p, "struts", 6) || !strncmp(p, "xwork", 5) || !strncmp(p, "validators", 10)) {
		return IDM_LANG_STRUTS;
	}
	if (!strncmp(p, "hibernate", 9)) {
		if (*(p + 10) == 'm') {
			return IDM_LANG_HIB_MAP;
		}
		return IDM_LANG_HIB_CFG;
	}
	if (!strncmp(p, "jboss", 5)) {
		return IDM_LANG_JBOSS;
	}
	if (!strncmp(p, "beans", 5)) {
		return IDM_LANG_SPRING_BEANS;
	}

	//if (!strncmp(p, "configuration", 10))
	//	return IDM_LANG_WEB_NET;
	//if (!strncmp(p, "root", 4))
	//	return IDM_LANG_RESX;
	//if (!strncmp(p, "Canvas", 6))
	//	return IDM_LANG_XAML;

	//if (!strncmp(p, "plist", 5))
	//	return IDM_LANG_PROPERTY_LIST;
	//if (!strncmp(p, "manifest", 8))
	//	return IDM_LANG_ANDROID_MANIFEST;
	//if (!strncmp(p, "svg", 3))
	//	return IDM_LANG_SVG;
	if (((p = StrStrA(pb, "Layout")) != NULL && StrStrA(p + 6, "xmlns:android")) ||
			((p = StrStrA(pb, "View")) != NULL && StrStrA(p + 4, "xmlns:android")) ||
			((p = StrStrA(pb, "menu")) != NULL && StrStrA(p + 4, "xmlns:android"))) {
		return IDM_LANG_ANDROID_LAYOUT;
	}

	return 0;
}

static inline BOOL IsASpace(int ch) {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

BOOL MatchCPPKeyword(char *p, int index) {
	if (*p < 'a' || *p > 'z') {
		return FALSE;
	}

	char word[32];
	word[0] = ' ';
	word[1] = *p++;
	int len = 2;
	while (len < 30 && (*p == '_' || (*p >= 'a' && *p <= 'z'))) {
		word[len++] = *p++;
	}
	if (len == 30 || isalnum((unsigned char)(*p))) {
		return FALSE;
	}
	word[len++] = ' ';
	word[len++] = 0;
	p = StrStrA(lexCPP.pKeyWords->pszKeyWords[index], word);
	return p != NULL;
}

PEDITLEXER Style_DetectObjCAndMatlab(void) {
	char tchText[4096] = ""; // maybe contains header comments
	SendMessage(hwndEdit, SCI_GETTEXT, COUNTOF(tchText) - 2, (LPARAM)tchText);

	char *p = tchText;
	np2LexLangIndex = 0;
	while (*p) {
		while (IsASpace(*p)) {
			++p;
		}
		switch (*p) {
		case '#':	// ObjC preprocessor or octave comment
			if (!(p == tchText && p[1] == '!')) {
				++p;
				while (*p == ' ' || *p == '\t') {
					++p;
				}
				if (MatchCPPKeyword(p, 2)) {
					return &lexCPP;
				}
				np2LexLangIndex = IDM_LANG_OCTAVE;
				return &lexMatlab;
			}
			break;
		case '(':
			++p;
			if (*p == '*') { // Mathematica comment
				return &lexFSharp;
			}
			break;
		case '@':	// ObjC keyword or Matlab command
			++p;
			if (MatchCPPKeyword(p, 3)) {
				return &lexCPP;
			}
			return &lexMatlab;
		case '/':	// C/C++ style comment
			++p;
			if (*p == '/' || *p == '*') {
				return &lexCPP;
			}
			break;
		case 'f':	// Matlab function
			if (strncmp(p, "function", 8) == 0 && (IsASpace(p[8]) || p[8] == '[')) {
				return &lexMatlab;
			}
			break;
		case 'c':	// Matlab classdef
			if (strncmp(p, "classdef", 8) == 0 && IsASpace(p[8])) {
				return &lexMatlab;
			}
			break;
		case '%':	// ObjC modular or Matlab comment
			return &lexMatlab;
		}
		// skip to next line
		while (*p && !(*p == '\r' || *p == '\n')) {
			++p;
		}
	}

	return NULL;
}

//=============================================================================
//
// Style_GetCurrentLexerName()
//
#if NP2_GET_LEXER_STYLE_NAME_FROM_RES
LPCWSTR Style_GetCurrentLexerDisplayName(LPWSTR lpszName, int cchName) {
	if (GetString(pLexCurrent->rid, lpszName, cchName)) {
		return lpszName;
	}
	return Style_GetCurrentLexerName();
}
#endif

LPCWSTR Style_GetCurrentLexerName(void) {
	if (np2LexLangIndex == 0) {
		return pLexCurrent->pszName;
	}
	switch (np2LexLangIndex) {
	case IDM_LANG_DEFAULT:
		return L"Default Text";

	case IDM_LANG_WEB:
		return L"Web Source Code";
	case IDM_LANG_PHP:
		return L"PHP Page";
	case IDM_LANG_JSP:
		return L"JSP Page";
	case IDM_LANG_ASPX_CS:
		return L"ASP.NET (C#)";
	case IDM_LANG_ASPX_VB:
		return L"ASP.NET (VB.NET)";
	case IDM_LANG_ASP_VBS:
		return L"ASP (VBScript)";
	case IDM_LANG_ASP_JS:
		return L"ASP (JScript)";

	case IDM_LANG_XML:
		return L"XML Document";
	case IDM_LANG_XSD:
		return L"XML Schema";
	case IDM_LANG_XSLT:
		return L"XSLT Stylesheet";
	case IDM_LANG_DTD:
		return L"XML DTD";

	case IDM_LANG_ANT_BUILD:
		return L"Ant Build";
	case IDM_LANG_MAVEN_POM:
		return L"Maven POM";
	case IDM_LANG_MAVEN_SETTINGS:
		return L"Maven Settings";
	case IDM_LANG_IVY_MODULE:
		return L"Ivy Module";
	case IDM_LANG_IVY_SETTINGS:
		return L"Ivy Settings";
	case IDM_LANG_PMD_RULESET:
		return L"PMD Ruleset";
	case IDM_LANG_CHECKSTYLE:
		return L"Checkstyle";

	case IDM_LANG_APACHE:
		return L"Apache Config";
	case IDM_LANG_TOMCAT:
		return L"Tomcat Config";
	case IDM_LANG_WEB_JAVA:
		return L"Java Web Config";
	case IDM_LANG_STRUTS:
		return L"Struts Config";
	case IDM_LANG_HIB_CFG:
		return L"Hibernate Config";
	case IDM_LANG_HIB_MAP:
		return L"Hibernate Mapping";
	case IDM_LANG_SPRING_BEANS:
		return L"Spring Beans";
	case IDM_LANG_JBOSS:
		return L"JBoss Config";

	case IDM_LANG_WEB_NET:
		return L"ASP.NET Web Config";
	case IDM_LANG_RESX:
		return L"ResX Schema";
	case IDM_LANG_XAML:
		return L"WPF XAML";

	case IDM_LANG_PROPERTY_LIST:
		return L"Property List";
	case IDM_LANG_ANDROID_MANIFEST:
		return L"Android Manifest";
	case IDM_LANG_ANDROID_LAYOUT:
		return L"Android Layout";
	case IDM_LANG_SVG:
		return L"SVG Document";

	case IDM_LANG_BASH:
		return L"Shell Script";
	case IDM_LANG_CSHELL:
		return L"C Shell";
	case IDM_LANG_M4:
		return L"M4 Macro";

	case IDM_LANG_MATLAB:
		return L"MATLAB Code";
	case IDM_LANG_OCTAVE:
		return L"Octave Code";
	case IDM_LANG_SCILAB:
		return L"Scilab Code";

	case IDM_LANG_CSS:
		return L"CSS Style Sheet";
	case IDM_LANG_SCSS:
		return L"Sassy CSS";
	case IDM_LANG_LESS:
		return L"Less CSS";
	case IDM_LANG_HSS:
		return L"HSS";

	default:
		return L"Error";
	}
}

//=============================================================================
// find lexer from file extension
// Style_MatchLexer()
//
PEDITLEXER Style_MatchLexer(LPCWSTR lpszMatch, BOOL bCheckNames) {
	if (!bCheckNames) {
		//if (StrNCaseEqual(L"php", lpszMatch, 3) || StrCaseEqual(L"phtml", lpszMatch))) {
		//	np2LexLangIndex = IDM_LANG_PHP;
		//	return &lexPHP;
		//}
		if (StrCaseEqual(L"jsp", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_JSP;
			return &lexHTML;
		}
		if (StrCaseEqual(L"aspx", lpszMatch)) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
			if (np2LexLangIndex == 0) {
				np2LexLangIndex = IDM_LANG_ASPX_CS;
			}
			return &lexHTML;
		}
		if (StrCaseEqual(L"asp", lpszMatch)) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
			if (np2LexLangIndex == 0) {
				np2LexLangIndex = IDM_LANG_ASP_VBS;
			}
			return &lexHTML;
		}

		if (StrCaseEqual(L"xsd", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_XSD;
			return &lexXML;
		}
		if (StrNCaseEqual(L"xsl", lpszMatch, 3)) {
			np2LexLangIndex = IDM_LANG_XSLT;
			return &lexXML;
		}
		if (StrCaseEqual(L"dtd", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_DTD;
			return &lexXML;
		}
		if (StrCaseEqual(L"pom", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_MAVEN_POM;
			return &lexXML;
		}
		if (StrCaseEqual(L"resx", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_RESX;
			return &lexXML;
		}
		if (StrCaseEqual(L"xaml", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_XAML;
			return &lexXML;
		}
		if (StrCaseEqual(L"plist", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_PROPERTY_LIST;
			return &lexXML;
		}
		if (StrCaseEqual(L"svg", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_SVG;
			return &lexXML;
		}
		if (StrCaseEqual(L"xml", lpszMatch)) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
			return &lexXML;
		}
		if (StrCaseEqual(L"sce", lpszMatch) || StrCaseEqual(L"sci", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_SCILAB;
			return &lexMatlab;
		}
		if (StrCaseEqual(L"m4", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_M4;
			return &lexBash;
		}
		if (StrCaseEqual(L"csh", lpszMatch) || StrCaseEqual(L"tcsh", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_CSHELL;
			return &lexBash;
		}
		if (StrCaseEqual(L"scss", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_SCSS;
			return &lexCSS;
		}
		if (StrCaseEqual(L"less", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_LESS;
			return &lexCSS;
		}
		if (StrCaseEqual(L"hss", lpszMatch)) {
			np2LexLangIndex = IDM_LANG_HSS;
			return &lexCSS;
		}
		if (bAutoSelect && StrCaseEqual(L"m", lpszMatch)) {
			PEDITLEXER lex = Style_DetectObjCAndMatlab();
			if (lex != NULL) {
				return lex;
			}
		}

		const int cch = lstrlen(lpszMatch);
		for (int i = 0; i < NUMLEXERS; i++) {
			LPCWSTR p1 = pLexArray[i]->szExtensions;
			do {
				LPCWSTR p2 = StrStrI(p1, lpszMatch);
				if (p2 == NULL) {
					break;
				}

				const WCHAR ch = (p2 == p1)? L'\0' : p2[-1];
				p2 += cch;
				if ((ch == L';' || ch == ' ' || ch == L'\0') && (*p2 == L';' || *p2 == L' ' || *p2 == L'\0')) {
					return pLexArray[i];
				}
				p1 = StrChr(p2, L';');
			} while (p1 != NULL);
		}
	} else {
		const int cch = lstrlen(lpszMatch);
		if (cch >= 3) {
			for (int i = 0; i < NUMLEXERS; i++) {
				if (StrNCaseEqual(pLexArray[i]->pszName, lpszMatch, cch)) {
					return pLexArray[i];
				}
			}
		}
	}
	return NULL;
}

//=============================================================================
// find lexer from file name
// Style_SetLexerFromFile()
//
extern int fNoHTMLGuess;
extern int fNoCGIGuess;
extern FILEVARS fvCurFile;

static PEDITLEXER Style_GetLexerFromFile(HWND hwnd, LPCWSTR lpszFile, BOOL bCGIGuess, LPWSTR *pszExt, BOOL *pDotFile) {
	LPWSTR lpszExt = PathFindExtension(lpszFile);
	LPWSTR lpszName = PathFindFileName(lpszFile);
	BOOL bFound = FALSE;
	PEDITLEXER pLexNew = NULL;

	if (StrNotEmpty(lpszExt)) {
		lpszExt++;

		if (StrCaseEqual(lpszExt, L"txt")) {
			bFound = TRUE;
			if (StrCaseEqual(lpszName, L"CMakeLists.txt") || StrCaseEqual(lpszName, L"CMakeCache.txt")) {
				pLexNew = &lexCMake;
			} else if (StrCaseEqual(lpszName, L"LLVMBuild.txt")) {
				pLexNew = &lexINI;
			} else {
				pLexNew = &lexDefault;
			}
		}

		if (!bFound && bCGIGuess && (StrCaseEqual(lpszExt, L"cgi") || StrCaseEqual(lpszExt, L"fcgi"))) {
			char tchText[256];
			SendMessage(hwnd, SCI_GETTEXT, COUNTOF(tchText) - 1, (LPARAM)tchText);
			StrTrimA(tchText, " \t\n\r");
			if ((pLexNew = Style_SniffShebang(tchText)) != NULL) {
				bFound = TRUE;
			}
		}

		if (!bFound && StrCaseEqual(lpszExt, L"xml")) {
			pLexNew = &lexXML;
			bFound = TRUE;
			if (StrCaseEqual(lpszName, L"build.xml") || StrCaseEqual(lpszName, L"javadoc.xml")) {
				np2LexLangIndex = IDM_LANG_ANT_BUILD;
			} else if (StrCaseEqual(lpszName, L"pom.xml")) {
				np2LexLangIndex = IDM_LANG_MAVEN_POM;
			} else if (StrCaseEqual(lpszName, L"settings.xml")) {
				np2LexLangIndex = IDM_LANG_MAVEN_SETTINGS;
			} else if (StrCaseEqual(lpszName, L"AndroidManifest.xml")) {
				np2LexLangIndex = IDM_LANG_ANDROID_MANIFEST;
			} else if (StrCaseEqual(lpszName, L"server.xml")) {
				np2LexLangIndex = IDM_LANG_TOMCAT;
			} else if (StrCaseEqual(lpszName, L"web.xml")) {
				np2LexLangIndex = IDM_LANG_WEB_JAVA;
			} else if (StrCaseEqual(lpszName, L"struts.xml") || StrCaseEqual(lpszName, L"struts-config.xml")) {
				np2LexLangIndex = IDM_LANG_STRUTS;
			} else if (StrCaseEqual(lpszName, L"hibernate.cfg.xml")) {
				np2LexLangIndex = IDM_LANG_HIB_CFG;
			} else if (StrCaseEqual(lpszName, L"ivy.xml")) {
				np2LexLangIndex = IDM_LANG_IVY_MODULE;
			} else if (StrCaseEqual(lpszName, L"ivysettings.xml")) {
				np2LexLangIndex = IDM_LANG_IVY_SETTINGS;
			} else if (StrCaseEqual(lpszName, L"pmd.xml")) {
				np2LexLangIndex = IDM_LANG_PMD_RULESET;
			}
		}

		if (!bFound && ((StrCaseEqual(lpszExt, L"conf") && StrNCaseEqual(lpszName, L"httpd", 5)) || StrCaseEqual(lpszExt, L"htaccess"))) {
			pLexNew = &lexCONF;
			bFound = TRUE;
			np2LexLangIndex = IDM_LANG_APACHE;
		}
		// MySQL ini/cnf
		if (!bFound && StrNCaseEqual(lpszName, L"my", 2) && (StrCaseEqual(lpszExt, L"ini") || StrCaseEqual(lpszExt, L"cnf"))) {
			pLexNew = &lexCONF;
			bFound = TRUE;
		}
		if (!bFound && StrCaseEqual(lpszName, L"web.config")) {
			pLexNew = &lexXML;
			bFound = TRUE;
			np2LexLangIndex = IDM_LANG_WEB_NET;
		}
		if (!bFound && StrRStrI(lpszFile, NULL, L".cmake.in")) {
			pLexNew = &lexCMake;
			bFound = TRUE;
		}

		// check associated extensions
		if (!bFound) {
			if ((pLexNew = Style_MatchLexer(lpszExt, FALSE)) != NULL) {
				bFound = TRUE;
			}
		}

		// dot file
		if (StrCaseEqual(lpszExt - 1, lpszName)) {
			*pDotFile = TRUE;
			if (StrNEqual(lpszExt, L"bash", 4) || StrEqual(lpszExt, L"profile")) { // .bash_history, .bash_logout, .bash_profile, .bashrc, .profile
				pLexNew = &lexBash;
				bFound = TRUE;
			}
		}
	}

	if (!bFound) {
		if (StrNCaseEqual(lpszName, L"Readme", 6)) {
			pLexNew = &lexDefault;
			bFound = TRUE;
		}
		if (!bFound && (StrNCaseEqual(lpszName, L"Makefile", 8) || StrNCaseEqual(lpszName, L"Kbuild", 6))) {
			pLexNew = &lexMake;
			bFound = TRUE;
		}
		if (!bFound && StrCaseEqual(lpszName, L"Rakefile")) {
			pLexNew = &lexRuby;
			bFound = TRUE;
		}
		if (!bFound && StrCaseEqual(lpszName, L"mozconfig")) {
			pLexNew = &lexBash;
			bFound = TRUE;
		}
		// Boost build
		if (!bFound && (StrCaseEqual(lpszName, L"Jamroot") || StrNCaseEqual(lpszName, L"Jamfile", 7))) {
			pLexNew = &lexJAM;
			bFound = TRUE;
		}
		if (!bFound && StrNCaseEqual(lpszName, L"Kconfig", 7)) {
			pLexNew = &lexCONF;
			bFound = TRUE;
		}
	}

	if (!bFound && pszExt) {
		*pszExt = lpszExt;
	}
	return pLexNew;
}

void Style_SetLexerFromFile(HWND hwnd, LPCWSTR lpszFile) {
	BOOL bFound = TRUE;
	BOOL bDotFile = FALSE;
	LPWSTR lpszExt = NULL;
	PEDITLEXER pLexNew = NULL;
	PEDITLEXER pLexSniffed;

	if (bAutoSelect) {
		pLexNew = Style_GetLexerFromFile(hwnd, lpszFile, !fNoCGIGuess, &lpszExt, &bDotFile);
	}
	if (pLexNew == NULL) {
		bFound = FALSE;
		pLexNew = pLexArray[iDefaultLexer];
	}

	// xml/html
	if ((!bFound && bAutoSelect && (!fNoHTMLGuess || !fNoCGIGuess)) || (bFound && pLexNew->rid == NP2LEX_PHP)) {
		char tchText[256];
		SendMessage(hwnd, SCI_GETTEXT, COUNTOF(tchText) - 1, (LPARAM)tchText);
		StrTrimA(tchText, " \t\n\r");
		if (pLexNew->rid == NP2LEX_PHP) {
			if (strncmp(tchText, "<?php", 5) != 0) {
				pLexNew = &lexHTML;
			}
		} else if (!fNoHTMLGuess && tchText[0] == '<') {
			if (StrStrIA(tchText, "<html")) {
				pLexNew = &lexHTML;
			} else {
				pLexNew = &lexXML;
			}
			np2LexLangIndex = Style_GetDocTypeLanguage();
			bFound = TRUE;
		} else if (!fNoCGIGuess && (pLexSniffed = Style_SniffShebang(tchText)) != NULL) {
			pLexNew = pLexSniffed;
			bFound = TRUE;
		}
	}

	// file mode
	if (!bFound && (fvCurFile.mask & FV_MODE) && fvCurFile.tchMode[0]) {
		WCHAR wchMode[32];
		const UINT cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
		MultiByteToWideChar(cpEdit, 0, fvCurFile.tchMode, -1, wchMode, COUNTOF(wchMode));

		if (!fNoCGIGuess && (StrCaseEqual(wchMode, L"cgi") || StrCaseEqual(wchMode, L"fcgi"))) {
			char tchText[256];
			SendMessage(hwnd, SCI_GETTEXT, COUNTOF(tchText) - 1, (LPARAM)tchText);
			StrTrimA(tchText, " \t\n\r");
			if ((pLexSniffed = Style_SniffShebang(tchText)) != NULL) {
				if (iEncoding != g_DOSEncoding || pLexSniffed != &lexDefault
						|| !(StrCaseEqual(lpszExt, L"nfo") || StrCaseEqual(lpszExt, L"diz"))) {
					// Although .nfo and .diz were removed from the default lexer's
					// default extensions list, they may still presist in the user's INI
					pLexNew = pLexSniffed;
					bFound = TRUE;
				}
			}
		}
		// file mode name/extension
		if (!bFound) {
			PEDITLEXER pLexMode;
			if ((pLexMode = Style_MatchLexer(wchMode, FALSE)) != NULL) {
				pLexNew = pLexMode;
				bFound = TRUE;
			} else if ((pLexMode = Style_MatchLexer(wchMode, TRUE)) != NULL) {
				pLexNew = pLexMode;
				bFound = TRUE;
			}
		}
	}

	if (!bFound && iEncoding == g_DOSEncoding) {
		pLexNew = &lexANSI;
		bFound = TRUE;
	}
	if (!bFound && bDotFile) {
		pLexNew = &lexCONF;
	}

	// Apply the new lexer
	Style_SetLexer(hwnd, pLexNew);
}

//=============================================================================
// find lexer from file name or extension
// Style_SetLexerFromName()
//
void Style_SetLexerFromName(HWND hwnd, LPCWSTR lpszFile, LPCWSTR lpszName) {
	PEDITLEXER pLexNew;
	if ((pLexNew = Style_MatchLexer(lpszName, FALSE)) != NULL) {
		Style_SetLexer(hwnd, pLexNew);
	} else if ((pLexNew = Style_MatchLexer(lpszName, TRUE)) != NULL) {
		Style_SetLexer(hwnd, pLexNew);
	} else {
		Style_SetLexerFromFile(hwnd, lpszFile);
	}
}

BOOL Style_CanOpenFile(LPCWSTR lpszFile) {
	const int lang = np2LexLangIndex;
	BOOL bDotFile = FALSE;
	LPWSTR lpszExt = NULL;
	PEDITLEXER pLexNew = Style_GetLexerFromFile(NULL, lpszFile, FALSE, &lpszExt, &bDotFile);
	np2LexLangIndex = lang;
	return pLexNew != NULL || StrIsEmpty(lpszExt) || bDotFile || StrCaseEqual(lpszExt, L"cgi") || StrCaseEqual(lpszExt, L"fcgi");
}

void Style_SetLexerByLangIndex(HWND hwnd, int lang) {
	np2LexLangIndex = lang;

	switch (lang) {
	case IDM_LANG_DEFAULT:
		Style_SetLexer(hwnd, &lexDefault);
		break;

	case IDM_LANG_APACHE:
		Style_SetLexer(hwnd, &lexCONF);
		break;

	case IDM_LANG_WEB:
	case IDM_LANG_PHP:
	case IDM_LANG_JSP:
	case IDM_LANG_ASPX_CS:
	case IDM_LANG_ASPX_VB:
	case IDM_LANG_ASP_VBS:
	case IDM_LANG_ASP_JS:
		if (lang == IDM_LANG_WEB) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
		}
		Style_SetLexer(hwnd, &lexHTML);
		break;

	case IDM_LANG_XML:
	case IDM_LANG_XSD:
	case IDM_LANG_XSLT:
	case IDM_LANG_DTD:

	case IDM_LANG_ANT_BUILD:
	case IDM_LANG_MAVEN_POM:
	case IDM_LANG_MAVEN_SETTINGS:
	case IDM_LANG_IVY_MODULE:
	case IDM_LANG_IVY_SETTINGS:
	case IDM_LANG_PMD_RULESET:
	case IDM_LANG_CHECKSTYLE:

	case IDM_LANG_TOMCAT:
	case IDM_LANG_WEB_JAVA:
	case IDM_LANG_STRUTS:
	case IDM_LANG_HIB_CFG:
	case IDM_LANG_HIB_MAP:
	case IDM_LANG_SPRING_BEANS:
	case IDM_LANG_JBOSS:

	case IDM_LANG_WEB_NET:
	case IDM_LANG_RESX:
	case IDM_LANG_XAML:

	case IDM_LANG_PROPERTY_LIST:
	case IDM_LANG_ANDROID_MANIFEST:
	case IDM_LANG_ANDROID_LAYOUT:
	case IDM_LANG_SVG:
		if (lang == IDM_LANG_XML) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
		}
		Style_SetLexer(hwnd, &lexXML);
		break;

	case IDM_LANG_BASH:
	case IDM_LANG_CSHELL:
	case IDM_LANG_M4:
		Style_SetLexer(hwnd, &lexBash);
		break;

	case IDM_LANG_MATLAB:
	case IDM_LANG_OCTAVE:
	case IDM_LANG_SCILAB:
		Style_SetLexer(hwnd, &lexMatlab);
		break;

	case IDM_LANG_CSS:
	case IDM_LANG_SCSS:
	case IDM_LANG_LESS:
	case IDM_LANG_HSS:
		Style_SetLexer(hwnd, &lexCSS);
		break;
	}
}

void Style_UpdateSchemeMenu(HMENU hmenu) {
	int lang = np2LexLangIndex;
	if (lang == 0) {
		switch (pLexCurrent->rid) {
		case NP2LEX_DEFAULT:
			lang = IDM_LANG_DEFAULT;
			break;
		case NP2LEX_HTML:
			lang = IDM_LANG_WEB;
			break;
		case NP2LEX_XML:
			lang = IDM_LANG_XML;
			break;
		case NP2LEX_BASH:
			lang = IDM_LANG_BASH;
			break;
		case NP2LEX_MATLAB:
			lang = IDM_LANG_MATLAB;
			break;
		case NP2LEX_OCTAVE:
			lang = IDM_LANG_OCTAVE;
			break;
		case NP2LEX_SCILAB:
			lang = IDM_LANG_SCILAB;
			break;
		case NP2LEX_CSS:
			lang = IDM_LANG_CSS;
			break;
		}
		np2LexLangIndex = lang;
	}
	for (int i = IDM_LANG_DEFAULT; i < IDM_LANG_NULL; i++) {
		CheckCmd(hmenu, i, FALSE);
	}
	if (lang >= IDM_LANG_DEFAULT) {
		CheckCmd(hmenu, lang, TRUE);
	}
}

//=============================================================================
//
// Style_SetLexerFromID()
//
void Style_SetLexerFromID(HWND hwnd, int id) {
	if (id >= 0 && id < NUMLEXERS) {
		np2LexLangIndex = Style_GetDocTypeLanguage();
		Style_SetLexer(hwnd, pLexArray[id]);
	}
}

int Style_GetEditLexerId(int lexer) {
	PEDITLEXER lex = NULL;

	switch (lexer) {
	case EditLexer_Default:
		lex = &lexDefault;
		break;

	case EditLexer_HTML:
		lex = &lexHTML;
		break;

	case EditLexer_XML:
		lex = &lexXML;
		break;

	default:
		return -1;
	}

	for (int i = 0; i < NUMLEXERS; i++) {
		if (pLexArray[i] == lex) {
			return i;
		}
	}
	return -1;
}

//=============================================================================
//
// Style_ToggleUse2ndDefault()
//
void Style_ToggleUse2ndDefault(HWND hwnd) {
	bUse2ndDefaultStyle = !bUse2ndDefaultStyle;
	Style_SetLexer(hwnd, pLexCurrent);
}

//=============================================================================
//
// Style_SetLongLineColors()
//
void Style_SetLongLineColors(HWND hwnd) {
	int rgb;

	// Use 2nd default style
	const int iIdx = GetDefaultStyleStartIndex();

	if (SendMessage(hwnd, SCI_GETEDGEMODE, 0, 0) == EDGE_LINE) {
		if (Style_StrGetColor(TRUE, lexDefault.Styles[Style_LongLineMarker + iIdx].szValue, &rgb)) { // edge fore
			SendMessage(hwnd, SCI_SETEDGECOLOUR, rgb, 0);
		} else {
			SendMessage(hwnd, SCI_SETEDGECOLOUR, GetSysColor(COLOR_3DLIGHT), 0);
		}
	} else {
		if (Style_StrGetColor(FALSE, lexDefault.Styles[Style_LongLineMarker + iIdx].szValue, &rgb)) { // edge back
			SendMessage(hwnd, SCI_SETEDGECOLOUR, rgb, 0);
		} else {
			SendMessage(hwnd, SCI_SETEDGECOLOUR, GetSysColor(COLOR_3DLIGHT), 0);
		}
	}
}

//=============================================================================
//
// Style_SetCurrentLineBackground()
//
void Style_SetCurrentLineBackground(HWND hwnd) {
	if (bHighlightCurrentLine) {
		// Use 2nd default style
		const int iIdx = GetDefaultStyleStartIndex();
		int iValue;
		if (Style_StrGetColor(FALSE, lexDefault.Styles[Style_CurrentLine + iIdx].szValue, &iValue)) { // caret line back
			SendMessage(hwnd, SCI_SETCARETLINEVISIBLE, TRUE, 0);
			SendMessage(hwnd, SCI_SETCARETLINEBACK, iValue, 0);

			if (Style_StrGetAlpha(lexDefault.Styles[Style_CurrentLine + iIdx].szValue, &iValue)) {
				SendMessage(hwnd, SCI_SETCARETLINEBACKALPHA, iValue, 0);
			} else {
				SendMessage(hwnd, SCI_SETCARETLINEBACKALPHA, SC_ALPHA_NOALPHA, 0);
			}
		} else {
			SendMessage(hwnd, SCI_SETCARETLINEVISIBLE, FALSE, 0);
		}
	} else {
		SendMessage(hwnd, SCI_SETCARETLINEVISIBLE, FALSE, 0);
	}
}

//=============================================================================
//
// Style_SetIndentGuides()
//
extern int flagSimpleIndentGuides;

void Style_SetIndentGuides(HWND hwnd, BOOL bShow) {
	int iIndentView = SC_IV_NONE;
	if (bShow) {
		if (!flagSimpleIndentGuides) {
			if (SendMessage(hwnd, SCI_GETLEXER, 0, 0) == SCLEX_PYTHON) {
				iIndentView = SC_IV_LOOKFORWARD;
			} else {
				iIndentView = SC_IV_LOOKBOTH;
			}
		} else {
			iIndentView = SC_IV_REAL;
		}
	}
	SendMessage(hwnd, SCI_SETINDENTATIONGUIDES, iIndentView, 0);
}

//=============================================================================
//
// Style_GetFileOpenDlgFilter()
//
extern LPWSTR tchFileDlgFilters;

BOOL Style_GetOpenDlgFilterStr(LPWSTR lpszFilter, int cchFilter) {
	if (StrIsEmpty(tchFileDlgFilters)) {
		GetString(IDS_FILTER_ALL, lpszFilter, cchFilter);
	} else {
		lstrcpyn(lpszFilter, tchFileDlgFilters, cchFilter - 2);
		lstrcat(lpszFilter, L"||");
	}
	PrepareFilterStr(lpszFilter);
	return TRUE;
}

//=============================================================================
//
// Style_StrGetFont()
//
BOOL Style_StrGetFont(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont) {
	WCHAR *p;

	if ((p = StrStr(lpszStyle, L"font:")) != NULL) {
		p += CSTRLEN(L"font:");
		while (*p == L' ') {
			++p;
		}
		lstrcpyn(lpszFont, p, cchFont);
		if ((p = StrChr(lpszFont, L';')) != NULL) {
			*p = L'\0';
		}
		TrimString(lpszFont);

		if (StrCaseEqual(lpszFont, L"Default") || !IsFontAvailable(lpszFont)) {
			lstrcpyn(lpszFont, SysMonoFontName[np2MonoFontIndex], cchFont);
		}
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetCharSet()
//
BOOL Style_StrGetCharSet(LPCWSTR lpszStyle, int *i) {
	WCHAR *p;

	if ((p = StrStr(lpszStyle, L"charset:")) != NULL) {
		p += CSTRLEN(L"charset:");
		return CRTStrToInt(p, i);
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetSize()
//
BOOL Style_StrGetSizeEx(LPCWSTR lpszStyle, int *i) {
	WCHAR *p;

	if ((p = StrStr(lpszStyle, L"size:")) != NULL) {
		p += CSTRLEN(L"size:");
		float value;
		if (StrToFloat(p, &value)) {
			int iValue = (int)lroundf(value * SC_FONT_SIZE_MULTIPLIER);
			iValue += (*p == L'+' || *p == '-')? iBaseFontSize : 0;
			// scintilla/src/ViewStyle.h GetFontSizeZoomed()
			iValue = max_i(iValue, 2 * SC_FONT_SIZE_MULTIPLIER);
			*i = iValue;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Style_StrGetSize(LPCWSTR lpszStyle, int *i) {
	if (Style_StrGetSizeEx(lpszStyle, i)) {
		*i /= SC_FONT_SIZE_MULTIPLIER;
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetSizeStr()
//
BOOL Style_StrGetSizeStr(LPCWSTR lpszStyle, LPWSTR lpszSize, int cchSize) {
	WCHAR *p;

	if ((p = StrStr(lpszStyle, L"size:")) != NULL) {
		p += CSTRLEN(L"size:");
		while (*p == L' ') {
			++p;
		}
		lstrcpyn(lpszSize, p, cchSize);
		if ((p = StrChr(lpszSize, L';')) != NULL) {
			*p = L'\0';
		}
		TrimString(lpszSize);
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetColor()
//
BOOL Style_StrGetColor(BOOL bFore, LPCWSTR lpszStyle, int *rgb) {
	WCHAR *p;

	if ((p = StrStr(lpszStyle, (bFore ? L"fore:" : L"back:"))) != NULL) {
		p += CSTRLEN("fore:");
		if (*p == L'#') {
			int iValue;
			if (HexStrToInt(p + 1, &iValue)) {
				*rgb = RGB((iValue & 0xFF0000) >> 16, (iValue & 0xFF00) >> 8, iValue & 0xFF);
				return TRUE;
			}
		}
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetCase()
//
BOOL Style_StrGetCase(LPCWSTR lpszStyle, int *i) {
	WCHAR *p;

	if ((p = StrStr(lpszStyle, L"case:")) != NULL) {
		p += CSTRLEN(L"case:");
		while (*p == L' ') {
			++p;
		}
		switch (*p) {
		case L'u':
		case L'U':
			*i = SC_CASE_UPPER;
			return TRUE;
		case L'l':
		case L'L':
			*i = SC_CASE_LOWER;
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetAlpha()
//
BOOL Style_StrGetAlpha(LPCWSTR lpszStyle, int *i) {
	WCHAR *p;

	if ((p = StrStr(lpszStyle, L"alpha:")) != NULL) {
		p += CSTRLEN(L"alpha:");
		if (CRTStrToInt(p, i)) {
			*i = clamp_i(*i, SC_ALPHA_TRANSPARENT, SC_ALPHA_OPAQUE);
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
// Style_SelectFont()
//
BOOL Style_SelectFont(HWND hwnd, LPWSTR lpszStyle, int cchStyle, BOOL bDefaultStyle) {
	CHOOSEFONT cf;
	LOGFONT lf;
	int iValue;
	WCHAR tch[32]; // LF_FACESIZE

	ZeroMemory(&cf, sizeof(CHOOSEFONT));
	ZeroMemory(&lf, sizeof(LOGFONT));

	// Map lpszStyle to LOGFONT
	if (Style_StrGetFont(lpszStyle, tch, COUNTOF(tch))) {
		lstrcpyn(lf.lfFaceName, tch, COUNTOF(lf.lfFaceName));
	}
	if (Style_StrGetCharSet(lpszStyle, &iValue)) {
		lf.lfCharSet = (BYTE)iValue;
	}
	if (Style_StrGetColor(TRUE, lpszStyle, &iValue)) {
		cf.rgbColors = iValue;
	}
	if (Style_StrGetSizeEx(lpszStyle, &iValue)) {
		HDC hdc = GetDC(hwnd);
		cf.iPointSize = iValue / (SC_FONT_SIZE_MULTIPLIER / 10);
		lf.lfHeight = -MulDiv(iValue, GetDeviceCaps(hdc, LOGPIXELSY), 72*SC_FONT_SIZE_MULTIPLIER);
		ReleaseDC(hwnd, hdc);
	}
	lf.lfWeight = (StrStr(lpszStyle, L"bold")) ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = StrStr(lpszStyle, L"italic") != NULL;
	lf.lfStrikeOut = StrStr(lpszStyle, L"strike") != NULL;

	// Init cf
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;
	cf.Flags = CF_INITTOLOGFONTSTRUCT /*| CF_NOSCRIPTSEL*/ | CF_SCREENFONTS | CF_EFFECTS;

	if (HIBYTE(GetKeyState(VK_SHIFT))) {
		cf.Flags |= CF_FIXEDPITCHONLY;
	}

	if (!ChooseFont(&cf) || StrIsEmpty(lf.lfFaceName)) {
		return FALSE;
	}

	// Map back to lpszStyle
	WCHAR szNewStyle[512];
	lstrcpy(szNewStyle, L"font:");
	lstrcat(szNewStyle, lf.lfFaceName);
	if (bDefaultStyle &&
			lf.lfCharSet != DEFAULT_CHARSET &&
			lf.lfCharSet != ANSI_CHARSET &&
			lf.lfCharSet != iDefaultCharSet) {
		lstrcat(szNewStyle, L"; charset:");
		wsprintf(tch, L"%u", lf.lfCharSet);
		lstrcat(szNewStyle, tch);
	}
	lstrcat(szNewStyle, L"; size:");
	wsprintf(tch, L"%i", cf.iPointSize / 10);
	lstrcat(szNewStyle, tch);
	iValue = cf.iPointSize % 10;
	if (iValue != 0) {
		tch[0] = '.';
		tch[1] = (WCHAR)(L'0' + iValue);
		tch[2] = 0;
		lstrcat(szNewStyle, tch);
	}
	if (cf.nFontType & BOLD_FONTTYPE) {
		lstrcat(szNewStyle, L"; bold");
	}
	if (cf.nFontType & ITALIC_FONTTYPE) {
		lstrcat(szNewStyle, L"; italic");
	}

	if (lf.lfUnderline /*StrStr(lpszStyle, L"underline")*/) {
		lstrcat(szNewStyle, L"; underline");
	}

	if (lf.lfStrikeOut /*StrStr(lpszStyle, L"strike")*/) {
		lstrcat(szNewStyle, L"; strike");
	}

	// save colors, ignore colors selected by user
	if (Style_StrGetColor(TRUE, lpszStyle, &iValue)) {
		wsprintf(tch, L"; fore:#%02X%02X%02X",
				 (int)GetRValue(iValue),
				 (int)GetGValue(iValue),
				 (int)GetBValue(iValue));
		lstrcat(szNewStyle, tch);
	}
	if (Style_StrGetColor(FALSE, lpszStyle, &iValue)) {
		wsprintf(tch, L"; back:#%02X%02X%02X",
				 (int)GetRValue(iValue),
				 (int)GetGValue(iValue),
				 (int)GetBValue(iValue));
		lstrcat(szNewStyle, tch);
	}

	if (StrStr(lpszStyle, L"eolfilled")) {
		lstrcat(szNewStyle, L"; eolfilled");
	}

	if (Style_StrGetCase(lpszStyle, &iValue)) {
		lstrcat(szNewStyle, L"; case:");
		lstrcat(szNewStyle, (iValue == SC_CASE_UPPER) ? L"u" : L"");
	}

	if (Style_StrGetAlpha(lpszStyle, &iValue)) {
		lstrcat(szNewStyle, L"; alpha:");
		wsprintf(tch, L"%i", iValue);
		lstrcat(szNewStyle, tch);
	}

	lstrcpyn(lpszStyle, szNewStyle, cchStyle);
	return TRUE;
}

//=============================================================================
//
// Style_SetDefaultFont()
//
void Style_SetDefaultFont(HWND hwnd) {
	const int iIdx = GetDefaultStyleStartIndex();
	if (Style_SelectFont(hwnd, lexDefault.Styles[Style_Default + iIdx].szValue, MAX_EDITSTYLE_VALUE_SIZE, TRUE)) {
		fStylesModified |= STYLESMODIFIED_SOME_STYLE;
		lexDefault.bStyleChanged = TRUE;
		Style_SetLexer(hwnd, pLexCurrent);
	}
}

//=============================================================================
//
// Style_SelectColor()
//
BOOL Style_SelectColor(HWND hwnd, BOOL bFore, LPWSTR lpszStyle, int cchStyle) {
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));

	int iRGBResult = bFore ? GetSysColor(COLOR_WINDOWTEXT) : GetSysColor(COLOR_WINDOW);
	Style_StrGetColor(bFore, lpszStyle, &iRGBResult);

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hwnd;
	cc.rgbResult = iRGBResult;
	cc.lpCustColors = customColor;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_SOLIDCOLOR;

	if (!ChooseColor(&cc)) {
		return FALSE;
	}

	iRGBResult = cc.rgbResult;

	// Rebuild style string
	WCHAR szNewStyle[512];
	int iValue;
	WCHAR tch[32]; // LF_FACESIZE

	lstrcpy(szNewStyle, L"");
	if (Style_StrGetFont(lpszStyle, tch, COUNTOF(tch))) {
		lstrcat(szNewStyle, L"font:");
		lstrcat(szNewStyle, tch);
	}
	if (Style_StrGetCharSet(lpszStyle, &iValue)) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		wsprintf(tch, L"charset:%i", iValue);
		lstrcat(szNewStyle, tch);
	}
	if (Style_StrGetSizeStr(lpszStyle, tch, COUNTOF(tch))) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		lstrcat(szNewStyle, L"size:");
		lstrcat(szNewStyle, tch);
	}

	if (StrStr(lpszStyle, L"bold")) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		lstrcat(szNewStyle, L"bold");
	}
	if (StrStr(lpszStyle, L"italic")) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		lstrcat(szNewStyle, L"italic");
	}
	if (StrStr(lpszStyle, L"underline")) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		lstrcat(szNewStyle, L"underline");
	}
	if (StrStr(lpszStyle, L"strike")) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		lstrcat(szNewStyle, L"strike");
	}

	if (bFore) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		wsprintf(tch, L"fore:#%02X%02X%02X",
				 (int)GetRValue(iRGBResult),
				 (int)GetGValue(iRGBResult),
				 (int)GetBValue(iRGBResult));
		lstrcat(szNewStyle, tch);
		if (Style_StrGetColor(FALSE, lpszStyle, &iValue)) 	{
			wsprintf(tch, L"; back:#%02X%02X%02X",
					 (int)GetRValue(iValue),
					 (int)GetGValue(iValue),
					 (int)GetBValue(iValue));
			lstrcat(szNewStyle, tch);
		}
	} else {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		if (Style_StrGetColor(TRUE, lpszStyle, &iValue)) {
			wsprintf(tch, L"fore:#%02X%02X%02X; ",
					 (int)GetRValue(iValue),
					 (int)GetGValue(iValue),
					 (int)GetBValue(iValue));
			lstrcat(szNewStyle, tch);
		}
		wsprintf(tch, L"back:#%02X%02X%02X",
				 (int)GetRValue(iRGBResult),
				 (int)GetGValue(iRGBResult),
				 (int)GetBValue(iRGBResult));
		lstrcat(szNewStyle, tch);
	}

	if (StrStr(lpszStyle, L"eolfilled")) {
		lstrcat(szNewStyle, L"; eolfilled");
	}

	if (Style_StrGetCase(lpszStyle, &iValue)) {
		lstrcat(szNewStyle, L"; case:");
		lstrcat(szNewStyle, (iValue == SC_CASE_UPPER) ? L"u" : L"");
	}

	if (Style_StrGetAlpha(lpszStyle, &iValue)) {
		lstrcat(szNewStyle, L"; alpha:");
		wsprintf(tch, L"%i", iValue);
		lstrcat(szNewStyle, tch);
	}

	lstrcpyn(lpszStyle, szNewStyle, cchStyle);
	return TRUE;
}

//=============================================================================
//
// Style_SetStyles()
//
void Style_SetStyles(HWND hwnd, int iStyle, LPCWSTR lpszStyle) {
	WCHAR tch[LF_FACESIZE];
	int iValue;

	// Font
	if (Style_StrGetFont(lpszStyle, tch, COUNTOF(tch))) {
		char mch[LF_FACESIZE * kMaxMultiByteCount];
		WideCharToMultiByte(CP_UTF8, 0, tch, -1, mch, COUNTOF(mch), NULL, NULL);
		SendMessage(hwnd, SCI_STYLESETFONT, iStyle, (LPARAM)mch);
	}

	// Size
	if (Style_StrGetSizeEx(lpszStyle, &iValue)) {
		iValue = ScaleFontSize(iValue);
		SendMessage(hwnd, SCI_STYLESETSIZEFRACTIONAL, iStyle, iValue);
	}

	// Fore
	if (Style_StrGetColor(TRUE, lpszStyle, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETFORE, iStyle, iValue);
	}

	// Back
	if (Style_StrGetColor(FALSE, lpszStyle, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETBACK, iStyle, iValue);
	}

	// Bold
	SendMessage(hwnd, SCI_STYLESETBOLD, iStyle, (StrStr(lpszStyle, L"bold") != NULL));
	// Italic
	SendMessage(hwnd, SCI_STYLESETITALIC, iStyle, (StrStr(lpszStyle, L"italic") != NULL));
	// Underline
	SendMessage(hwnd, SCI_STYLESETUNDERLINE, iStyle, (StrStr(lpszStyle, L"underline") != NULL));
	// Strike
	SendMessage(hwnd, SCI_STYLESETSTRIKE, iStyle, (StrStr(lpszStyle, L"strike") != NULL));
	// EOL Filled
	SendMessage(hwnd, SCI_STYLESETEOLFILLED, iStyle, (StrStr(lpszStyle, L"eolfilled") != NULL));

	// Case
	if (Style_StrGetCase(lpszStyle, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETCASE, iStyle, iValue);
	}

	// Character Set
	if (Style_StrGetCharSet(lpszStyle, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETCHARACTERSET, iStyle, iValue);
	}
}

static void Style_Parse(struct DetailStyle *style, LPCWSTR lpszStyle) {
	UINT mask = 0;
	int iValue;

	// Font
	if (Style_StrGetFont(lpszStyle, style->fontWide, COUNTOF(style->fontWide))) {
		WideCharToMultiByte(CP_UTF8, 0, style->fontWide, -1, style->fontFace, COUNTOF(style->fontFace), NULL, NULL);
		mask |= STYLE_MASK_FONT_FACE;
	}

	// Size
	if (Style_StrGetSizeEx(lpszStyle, &iValue)) {
		style->fontSize = ScaleFontSize(iValue);
		mask |= STYLE_MASK_FONT_SIZE;
	}

	// Fore
	if (Style_StrGetColor(TRUE, lpszStyle, &iValue)) {
		style->foreColor = iValue;
		mask |= STYLE_MASK_FORE_COLOR;
	}

	// Back
	if (Style_StrGetColor(FALSE, lpszStyle, &iValue)) {
		style->backColor = iValue;
		mask |= STYLE_MASK_BACK_COLOR;
	}

	// Bold
	style->bold = StrStr(lpszStyle, L"bold") != NULL;
	// Italic
	style->italic = StrStr(lpszStyle, L"italic") != NULL;
	// Underline
	style->underline = StrStr(lpszStyle, L"underline") != NULL;
	// Strike
	style->strike = StrStr(lpszStyle, L"strike") != NULL;
	// EOL Filled
	style->eolFilled = StrStr(lpszStyle, L"eolfilled") != NULL;

	// Case
	if (Style_StrGetCase(lpszStyle, &iValue)) {
		style->upperLower = iValue;
		mask |= STYLE_MASK_UPPER_LOWER;
	}

	// Character Set
	if (Style_StrGetCharSet(lpszStyle, &iValue)) {
		style->charset = iValue;
		mask |= STYLE_MASK_CHARSET;
	}

	style->mask = mask;
}

static void Style_SetParsed(HWND hwnd, const struct DetailStyle *style, int iStyle) {
	const UINT mask = style->mask;

	// Font
	if (mask & STYLE_MASK_FONT_FACE) {
		SendMessage(hwnd, SCI_STYLESETFONT, iStyle, (LPARAM)style->fontFace);
	}

	// Size
	if (mask & STYLE_MASK_FONT_SIZE) {
		SendMessage(hwnd, SCI_STYLESETSIZEFRACTIONAL, iStyle, style->fontSize);
	}

	// Fore
	if (mask & STYLE_MASK_FORE_COLOR) {
		SendMessage(hwnd, SCI_STYLESETFORE, iStyle, style->foreColor);
	}

	// Back
	if (mask & STYLE_MASK_BACK_COLOR) {
		SendMessage(hwnd, SCI_STYLESETBACK, iStyle, style->backColor);
	}

	// Bold
	SendMessage(hwnd, SCI_STYLESETBOLD, iStyle, style->bold);
	// Italic
	SendMessage(hwnd, SCI_STYLESETITALIC, iStyle, style->italic);
	// Underline
	SendMessage(hwnd, SCI_STYLESETUNDERLINE, iStyle, style->underline);
	// Strike
	SendMessage(hwnd, SCI_STYLESETSTRIKE, iStyle, style->strike);
	// EOL Filled
	SendMessage(hwnd, SCI_STYLESETEOLFILLED, iStyle, style->eolFilled);

	// Case
	if (mask & STYLE_MASK_UPPER_LOWER) {
		SendMessage(hwnd, SCI_STYLESETCASE, iStyle, style->upperLower);
	}

	// Character Set
	if (mask & STYLE_MASK_CHARSET) {
		SendMessage(hwnd, SCI_STYLESETCHARACTERSET, iStyle, style->charset);
	}
}

//=============================================================================
//
// Style_GetLexerIconId()
//
int Style_GetLexerIconId(PEDITLEXER pLex) {
	LPCWSTR pszExtensions;
	if (StrNotEmpty(pLex->szExtensions)) {
		pszExtensions = pLex->szExtensions;
	} else {
		pszExtensions = pLex->pszDefExt;
	}

	WCHAR *pszFile = NP2HeapAlloc(sizeof(WCHAR) * (lstrlen(pszExtensions) + CSTRLEN(L"*.txt") + 16));
	lstrcpy(pszFile, L"*.");
	lstrcat(pszFile, pszExtensions);
	WCHAR *p;
	if ((p = StrChr(pszFile, L';')) != NULL) {
		*p = L'\0';
	}

	// check for ; at beginning
	if (lstrlen(pszFile) < 3) {
		lstrcat(pszFile, L"txt");
	}

	SHFILEINFO shfi;
	SHGetFileInfo(pszFile, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
				  SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);

	NP2HeapFree(pszFile);

	return shfi.iIcon;
}

//=============================================================================
//
// Style_AddLexerToTreeView()
//
HTREEITEM Style_AddLexerToTreeView(HWND hwnd, PEDITLEXER pLex) {
#if NP2_GET_LEXER_STYLE_NAME_FROM_RES
	WCHAR tch[128];
#endif

	TVINSERTSTRUCT tvis;
	ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));

	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
#if NP2_GET_LEXER_STYLE_NAME_FROM_RES
	if (GetString(pLex->rid, tch, COUNTOF(tch))) {
		tvis.item.pszText = tch;
	} else {
		tvis.item.pszText = (WCHAR *)pLex->pszName;
	}
#else
	tvis.item.pszText = (WCHAR *)pLex->pszName;
#endif
	tvis.item.iImage = Style_GetLexerIconId(pLex);
	tvis.item.iSelectedImage = tvis.item.iImage;
	tvis.item.lParam = (LPARAM)pLex;

	HTREEITEM hTreeNode = (HTREEITEM)TreeView_InsertItem(hwnd, &tvis);

	tvis.hParent = hTreeNode;
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	//tvis.item.iImage = -1;
	//tvis.item.iSelectedImage = -1;

	const int iStyleCount = pLex->iStyleCount;
	for (int i = 0; i < iStyleCount; i++) {
#if NP2_GET_LEXER_STYLE_NAME_FROM_RES
		if (GetString(pLex->Styles[i].rid, tch, COUNTOF(tch))) {
			tvis.item.pszText = tch;
		} else {
			tvis.item.pszText = (WCHAR *)pLex->Styles[i].pszName;
		}
#else
		tvis.item.pszText = (WCHAR *)pLex->Styles[i].pszName;
#endif
		tvis.item.lParam = (LPARAM)(&pLex->Styles[i]);
		TreeView_InsertItem(hwnd, &tvis);
	}

	return hTreeNode;
}

//=============================================================================
//
// Style_AddLexerToListView()
//
void Style_AddLexerToListView(HWND hwnd, PEDITLEXER pLex) {
#if NP2_GET_LEXER_STYLE_NAME_FROM_RES
	WCHAR tch[MAX_EDITLEXER_NAME_SIZE];
#endif
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));

	lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
	lvi.iItem = ListView_GetItemCount(hwnd);
#if NP2_GET_LEXER_STYLE_NAME_FROM_RES
	if (GetString(pLex->rid, tch, COUNTOF(tch))) {
		lvi.pszText = tch;
	} else {
		lvi.pszText = (WCHAR *)pLex->pszName;
	}
#else
	lvi.pszText = (WCHAR *)pLex->pszName;
#endif
	lvi.iImage = Style_GetLexerIconId(pLex);
	lvi.lParam = (LPARAM)pLex;

	ListView_InsertItem(hwnd, &lvi);
}

//=============================================================================
//
// Style_ConfigDlgProc()
//
static INT_PTR CALLBACK Style_ConfigDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
#define NotifyStyleTextChanged()	NotifyEditTextChanged(hwnd, IDC_STYLEEDIT)

	static HWND hwndTV;
	static BOOL fDragging;
	static PEDITLEXER pCurrentLexer;
	static PEDITSTYLE pCurrentStyle;
	//static HBRUSH hbrFore;
	//static HBRUSH hbrBack;

	switch (umsg) {
	case WM_INITDIALOG: {
		ResizeDlg_InitY2(hwnd, cxStyleCustomizeDlg, cyStyleCustomizeDlg, IDC_RESIZEGRIP3, IDC_STYLEEDIT, IDC_STYLEVALUE_DEFAULT);

		hwndTV = GetDlgItem(hwnd, IDC_STYLELIST);
		fDragging = FALSE;

		SHFILEINFO shfi;
		TreeView_SetImageList(hwndTV,
							  (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO),
									  SHGFI_SMALLICON | SHGFI_SYSICONINDEX), TVSIL_NORMAL);

		// Add lexers
		BOOL found = FALSE;
		HTREEITEM currentLex = NULL;
		for (int i = 0; i < NUMLEXERS; i++) {
			if (!found && StrEqual(pLexArray[i]->pszName, pLexCurrent->pszName)) {
				found = TRUE;
				currentLex = Style_AddLexerToTreeView(hwndTV, pLexArray[i]);
			} else {
				Style_AddLexerToTreeView(hwndTV, pLexArray[i]);
			}
		}

		pCurrentStyle = NULL;

		//SetExplorerTheme(hwndTV);
		//TreeView_Expand(hwndTV, TreeView_GetRoot(hwndTV), TVE_EXPAND);
		TreeView_Select(hwndTV, currentLex, TVGN_CARET);

		MultilineEditSetup(hwnd, IDC_STYLEEDIT);
		MultilineEditSetup(hwnd, IDC_STYLEVALUE_DEFAULT);
		SendDlgItemMessage(hwnd, IDC_STYLEEDIT, EM_LIMITTEXT, MAX_LEXER_STYLE_EDIT_SIZE - 1, 0);

		MakeBitmapButton(hwnd, IDC_PREVSTYLE, g_hInstance, IDB_PREV);
		MakeBitmapButton(hwnd, IDC_NEXTSTYLE, g_hInstance, IDB_NEXT);

		// Setup title font
		HFONT hFontTitle = (HFONT)SendDlgItemMessage(hwnd, IDC_TITLE, WM_GETFONT, 0, 0);
		if (hFontTitle == NULL) {
			hFontTitle = GetStockObject(DEFAULT_GUI_FONT);
		}
		LOGFONT lf;
		GetObject(hFontTitle, sizeof(LOGFONT), &lf);
		lf.lfHeight += lf.lfHeight / 5;
		lf.lfWeight = FW_BOLD;
		hFontTitle = CreateFontIndirect(&lf);
		SendDlgItemMessage(hwnd, IDC_TITLE, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
		SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)(hFontTitle));

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY: {
		HFONT hFontTitle = (HFONT)GetWindowLongPtr(hwnd, DWLP_USER);
		DeleteObject(hFontTitle);
		DeleteBitmapButton(hwnd, IDC_STYLEFORE);
		DeleteBitmapButton(hwnd, IDC_STYLEBACK);
		DeleteBitmapButton(hwnd, IDC_PREVSTYLE);
		DeleteBitmapButton(hwnd, IDC_NEXTSTYLE);
		ResizeDlg_Destroy(hwnd, &cxStyleCustomizeDlg, &cyStyleCustomizeDlg);
	}
	return FALSE;

	case WM_SIZE: {
		int dx, dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);
		const int cy = ResizeDlg_CalcDeltaY2(hwnd, dy, 50, IDC_STYLEEDIT, IDC_STYLEVALUE_DEFAULT);
		HDWP hdwp = BeginDeferWindowPos(18);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP3, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLELIST, 0, dy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_INFO_GROUPBOX, dx, 0, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLEEDIT, dx, cy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLELABEL_DEFAULT, 0, cy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLEVALUE_DEFAULT, 0, cy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLEFORE, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLEBACK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLEFONT, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_PREVIEW, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLEDEFAULT, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_PREVSTYLE, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_NEXTSTYLE, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_IMPORT, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_EXPORT, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESETALL, 0, dy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);
		ResizeDlgCtl(hwnd, IDC_STYLEVALUE_DEFAULT, dx, dy - cy);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY:
		if (((LPNMHDR)(lParam))->idFrom == IDC_STYLELIST) {
			LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)lParam;
			switch (lpnmtv->hdr.code) {
			case TVN_SELCHANGED: {
				if (pCurrentStyle) {
					GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue, MAX_EDITSTYLE_VALUE_SIZE);
				} else if (pCurrentLexer) {
					if (!GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions, MAX_EDITLEXER_EXT_SIZE)) {
						lstrcpy(pCurrentLexer->szExtensions, pCurrentLexer->pszDefExt);
					}
				}

				// a lexer has been selected
				if (!TreeView_GetParent(hwndTV, lpnmtv->itemNew.hItem)) {
					WCHAR wch[MAX_EDITLEXER_EXT_SIZE];

					GetDlgItemText(hwnd, IDC_STYLELABELS, wch, COUNTOF(wch));
					if (StrChr(wch, L'|')) {
						*StrChr(wch, L'|') = 0;
					}

					pCurrentStyle = 0;
					if ((pCurrentLexer = (PEDITLEXER)lpnmtv->itemNew.lParam) != NULL) {
						SetDlgItemText(hwnd, IDC_STYLELABEL, wch);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFONT), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFORE), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEBACK), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), TRUE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEBOLD), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEITALIC), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEUNDERLINE), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEEOLFILLED), FALSE);
						//CheckDlgButton(hwnd, IDC_STYLEBOLD, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEITALIC, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, BST_UNCHECKED);
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions);
						SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, pCurrentLexer->pszDefExt);
					} else {
						SetDlgItemText(hwnd, IDC_STYLELABEL, L"");
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFONT), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFORE), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEBACK), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEBOLD), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEITALIC), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEUNDERLINE), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEEOLFILLED), FALSE);
						//CheckDlgButton(hwnd, IDC_STYLEBOLD, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEITALIC, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, BST_UNCHECKED);
						SetDlgItemText(hwnd, IDC_STYLEEDIT, L"");
						SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, L"");
					}
				}

				// a style has been selected
				else {
					WCHAR wch[MAX_EDITSTYLE_VALUE_SIZE];

					GetDlgItemText(hwnd, IDC_STYLELABELS, wch, COUNTOF(wch));
					if (StrChr(wch, L'|')) {
						*StrChr(wch, L'|') = 0;
					}

					pCurrentLexer = 0;
					if ((pCurrentStyle = (PEDITSTYLE)lpnmtv->itemNew.lParam) != NULL) {
						SetDlgItemText(hwnd, IDC_STYLELABEL, StrEnd(wch) + 1);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFONT), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFORE), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEBACK), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), TRUE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEBOLD), TRUE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEITALIC), TRUE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEUNDERLINE), TRUE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEEOLFILLED), TRUE);
						//CheckDlgButton(hwnd, IDC_STYLEBOLD, Style_StrGetAttribute(pCurrentStyle->szValue, L"bold") ? BST_CHECKED : BST_UNCHECKED));
						//CheckDlgButton(hwnd, IDC_STYLEITALIC, Style_StrGetAttribute(pCurrentStyle->szValue, L"italic") ? BST_CHECKED : BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, Style_StrGetAttribute(pCurrentStyle->szValue, L"underline") ? BST_CHECKED : BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, Style_StrGetAttribute(pCurrentStyle->szValue, L"eolfilled") ? BST_CHECKED : BST_UNCHECKED);
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue);
						SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, pCurrentStyle->pszDefault);
					} else {
						SetDlgItemText(hwnd, IDC_STYLELABEL, L"");
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFONT), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEFORE), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEBACK), FALSE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEBOLD), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEITALIC), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEUNDERLINE), FALSE);
						//EnableWindow(GetDlgItem(hwnd, IDC_STYLEEOLFILLED), FALSE);
						//CheckDlgButton(hwnd, IDC_STYLEBOLD, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEITALIC, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, BST_UNCHECKED);
						//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, BST_UNCHECKED);
						SetDlgItemText(hwnd, IDC_STYLEEDIT, L"");
						SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, L"");
					}
					NotifyStyleTextChanged();
				}
			}
			break;

			case TVN_BEGINDRAG: {
				//if (pCurrentStyle)
				//	GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue, MAX_EDITSTYLE_VALUE_SIZE);
				TreeView_Select(hwndTV, lpnmtv->itemNew.hItem, TVGN_CARET);

				//HIMAGELIST himl = TreeView_CreateDragImage(hwndTV, lpnmtv->itemNew.hItem);
				//ImageList_BeginDrag(himl, 0, 0, 0);
				//ImageList_DragEnter(hwndTV, lpnmtv->ptDrag.x, lpnmtv->ptDrag.y);
				if (pCurrentStyle) {
					DestroyCursor(SetCursor(LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_COPY))));
				} else {
					DestroyCursor(SetCursor(LoadCursor(NULL, IDC_NO)));
				}

				SetCapture(hwnd);
				fDragging = TRUE;
			}
			}
		}
		break;

	case WM_MOUSEMOVE: {
		if (fDragging && pCurrentStyle) {
			HTREEITEM htiTarget;
			TVHITTESTINFO tvht;
			LONG xCur = GET_X_LPARAM(lParam);
			LONG yCur = GET_Y_LPARAM(lParam);

			//ImageList_DragMove(xCur, yCur);
			//ImageList_DragShowNolock(FALSE);

			tvht.pt.x = xCur;
			tvht.pt.y = yCur;

			//ClientToScreen(hwnd, &tvht.pt);
			//ScreenToClient(hwndTV, &tvht.pt);
			MapWindowPoints(hwnd, hwndTV, &tvht.pt, 1);

			if ((htiTarget = TreeView_HitTest(hwndTV, &tvht)) != NULL &&
					TreeView_GetParent(hwndTV, htiTarget) != NULL) {
				TreeView_SelectDropTarget(hwndTV, htiTarget);
				//TreeView_Expand(hwndTV, htiTarget, TVE_EXPAND);
				TreeView_EnsureVisible(hwndTV, htiTarget);
			} else {
				TreeView_SelectDropTarget(hwndTV, NULL);
			}

			//ImageList_DragShowNolock(TRUE);
		}
	}
	break;

	case WM_LBUTTONUP: {
		if (fDragging) {
			HTREEITEM htiTarget;

			//ImageList_EndDrag();

			if ((htiTarget = TreeView_GetDropHilight(hwndTV)) != NULL) {
				WCHAR tchCopy[MAX_LEXER_STYLE_EDIT_SIZE];
				TreeView_SelectDropTarget(hwndTV, NULL);
				GetDlgItemText(hwnd, IDC_STYLEEDIT, tchCopy, COUNTOF(tchCopy));
				TreeView_Select(hwndTV, htiTarget, TVGN_CARET);

				// after select, this is new current item
				if (pCurrentStyle) {
					lstrcpy(pCurrentStyle->szValue, tchCopy);
					SetDlgItemText(hwnd, IDC_STYLEEDIT, tchCopy);
					//CheckDlgButton(hwnd, IDC_STYLEBOLD, Style_StrGetAttribute(tchCopy, L"bold") ? BST_CHECKED : BST_UNCHECKED);
					//CheckDlgButton(hwnd, IDC_STYLEITALIC, Style_StrGetAttribute(tchCopy, L"italic") ? BST_CHECKED : BST_UNCHECKED);
					//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, Style_StrGetAttribute(tchCopy, L"underline") ? BST_CHECKED : BST_UNCHECKED);
					//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, Style_StrGetAttribute(tchCopy, L"eolfilled") ? BST_CHECKED : BST_UNCHECKED);
					NotifyStyleTextChanged();
				}
			}
			ReleaseCapture();
			DestroyCursor(SetCursor(LoadCursor(NULL, IDC_ARROW)));
			fDragging = FALSE;
		}
	}
	break;

	case WM_CANCELMODE: {
		if (fDragging) {
			//ImageList_EndDrag();
			TreeView_SelectDropTarget(hwndTV, NULL);
			ReleaseCapture();
			DestroyCursor(SetCursor(LoadCursor(NULL, IDC_ARROW)));
			fDragging = FALSE;
		}
	}
	break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_PREVSTYLE:
			if (TreeView_GetSelection(hwndTV)) {
				TreeView_Select(hwndTV, TreeView_GetPrevVisible(hwndTV,
								TreeView_GetSelection(hwndTV)), TVGN_CARET);
			}
			NotifyStyleTextChanged();
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_NEXTSTYLE:
			if (TreeView_GetSelection(hwndTV)) {
				TreeView_Select(hwndTV, TreeView_GetNextVisible(hwndTV,
								TreeView_GetSelection(hwndTV)), TVGN_CARET);
			}
			NotifyStyleTextChanged();
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_STYLEFONT:
			if (pCurrentStyle) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
				if (Style_SelectFont(hwnd, tch, COUNTOF(tch),
									 StrEqual(pCurrentStyle->pszName, L"Default Style") ||
									 StrEqual(pCurrentStyle->pszName, L"2nd Default Style"))) {
					SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
					//CheckDlgButton(hwnd, IDC_STYLEBOLD, (Style_StrGetAttribute(tch, L"bold") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEITALIC, (Style_StrGetAttribute(tch, L"italic") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, (Style_StrGetAttribute(tch, L"underline") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, (Style_StrGetAttribute(tch, L"eolfilled") ? BST_CHECKED : BST_UNCHECKED));
					NotifyStyleTextChanged();
				}
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_STYLEFORE:
			if (pCurrentStyle) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
				if (Style_SelectColor(hwnd, TRUE, tch, COUNTOF(tch))) {
					SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
					//CheckDlgButton(hwnd, IDC_STYLEBOLD, (Style_StrGetAttribute(tch, L"bold") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEITALIC, (Style_StrGetAttribute(tch, L"italic") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, (Style_StrGetAttribute(tch, L"underline") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, (Style_StrGetAttribute(tch, L"eolfilled") ? BST_CHECKED : BST_UNCHECKED));
					NotifyStyleTextChanged();
				}
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_STYLEBACK:
			if (pCurrentStyle) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
				if (Style_SelectColor(hwnd, FALSE, tch, COUNTOF(tch))) {
					SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
					//CheckDlgButton(hwnd, IDC_STYLEBOLD, (Style_StrGetAttribute(tch, L"bold") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEITALIC, (Style_StrGetAttribute(tch, L"italic") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, (Style_StrGetAttribute(tch, L"underline") ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, (Style_StrGetAttribute(tch, L"eolfilled") ? BST_CHECKED : BST_UNCHECKED));
					NotifyStyleTextChanged();
				}
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_RESETALL:
			Style_ResetAll();
			// fall through
		case IDC_STYLEDEFAULT:
			if (pCurrentStyle) {
				lstrcpy(pCurrentStyle->szValue, pCurrentStyle->pszDefault);
				SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue);
				//CheckDlgButton(hwnd, IDC_STYLEBOLD, (Style_StrGetAttribute(pCurrentStyle->szValue, L"bold") ? BST_CHECKED : BST_UNCHECKED));
				//CheckDlgButton(hwnd, IDC_STYLEITALIC, (Style_StrGetAttribute(pCurrentStyle->szValue, L"italic") ? BST_CHECKED : BST_UNCHECKED));
				//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, (Style_StrGetAttribute(pCurrentStyle->szValue, L"underline") ? BST_CHECKED : BST_UNCHECKED));
				//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, (Style_StrGetAttribute(pCurrentStyle->szValue, L"eolfilled") ? BST_CHECKED : BST_UNCHECKED));
			} else if (pCurrentLexer) {
				lstrcpy(pCurrentLexer->szExtensions, pCurrentLexer->pszDefExt);
				SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions);
			}
			NotifyStyleTextChanged();
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		//case IDC_STYLEBOLD:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"bold", IsButtonChecked(hwnd, IDC_STYLEBOLD));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
		//		NotifyStyleTextChanged();
		//	}
		//	break;

		//case IDC_STYLEITALIC:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"italic", IsButtonChecked(hwnd, IDC_STYLEITALIC));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
		//		NotifyStyleTextChanged();
		//	}
		//	break;

		//case IDC_STYLEUNDERLINE:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"underline", IsButtonChecked(hwnd, IDC_STYLEUNDERLINE));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
		//		NotifyStyleTextChanged();
		//	}
		//	break;

		//case IDC_STYLEEOLFILLED:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"eolfilled", IsButtonChecked(hwnd, IDC_STYLEEOLFILLED));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
		//		NotifyStyleTextChanged();
		//	}
		//	break;

		case IDC_STYLEEDIT: {
			if (HIWORD(wParam) == EN_CHANGE && pCurrentStyle != NULL) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];

				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));

				int cr = -1;
				Style_StrGetColor(TRUE, tch, &cr);
				MakeColorPickButton(hwnd, IDC_STYLEFORE, g_hInstance, cr);

				cr = -1;
				Style_StrGetColor(FALSE, tch, &cr);
				MakeColorPickButton(hwnd, IDC_STYLEBACK, g_hInstance, cr);
			}
		}
		break;

		case IDC_IMPORT:
		case IDC_EXPORT:
		case IDC_PREVIEW:
		case IDOK: {
			if (pCurrentStyle) {
				GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue, MAX_EDITSTYLE_VALUE_SIZE);
			} else if (pCurrentLexer) {
				if (!GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions, MAX_EDITLEXER_EXT_SIZE)) {
					lstrcpy(pCurrentLexer->szExtensions, pCurrentLexer->pszDefExt);
				}
			}

			switch (LOWORD(wParam)) {
			case IDC_IMPORT:
				if (Style_Import(hwnd)) {
					if (pCurrentStyle) {
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue);
					} else if (pCurrentLexer) {
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions);
						NotifyStyleTextChanged();
					}

					TreeView_Select(hwndTV, TreeView_GetRoot(hwndTV), TVGN_CARET);
				}
				break;

			case IDC_EXPORT:
				Style_Export(hwnd);
				break;

			case IDC_PREVIEW:
				Style_SetLexer(hwndEdit, pLexCurrent);
				break;

			case IDOK:
				EndDialog(hwnd, IDOK);
				break;
			}
		}
		break;

		case IDCANCEL:
			if (fDragging) {
				SendMessage(hwnd, WM_CANCELMODE, 0, 0);
			} else {
				EndDialog(hwnd, IDCANCEL);
			}
			break;

		}
		return TRUE;
	}

#undef NotifyStyleTextChanged
	return FALSE;
}

//=============================================================================
//
// Style_ConfigDlg()
//
void Style_ConfigDlg(HWND hwnd) {
	LPWSTR extBackup = NP2HeapAlloc(ALL_FILE_EXTENSIONS_BYTE_SIZE);
	COLORREF colorBackup[MAX_CUSTOM_COLOR_COUNT];
	LPWSTR styleBackup[NUMLEXERS];
	BOOL apply = FALSE;

	if (!bCustomColorLoaded || iLexerLoadedCount != NUMLEXERS) {
		Style_LoadAll();
	}

	// Backup Styles
	CopyMemory(extBackup, g_AllFileExtensions, ALL_FILE_EXTENSIONS_BYTE_SIZE);
	CopyMemory(colorBackup, customColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));
	for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		LPWSTR szStyleBuf = NP2HeapAlloc(pLex->iStyleBufSize);
		CopyMemory(szStyleBuf, pLex->szStyleBuf, pLex->iStyleBufSize);
		styleBackup[iLexer] = szStyleBuf;
	}

	if (IDCANCEL == ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_STYLECONFIG), GetParent(hwnd),  Style_ConfigDlgProc, 0)) {
		// Restore Styles
		CopyMemory(g_AllFileExtensions, extBackup, ALL_FILE_EXTENSIONS_BYTE_SIZE);
		CopyMemory(customColor, colorBackup, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));
		for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			CopyMemory(pLex->szStyleBuf, styleBackup[iLexer], pLex->iStyleBufSize);
		}
	} else {
		apply = TRUE;
		if (!(fStylesModified & STYLESMODIFIED_FILE_EXT)) {
			if (memcmp(extBackup, g_AllFileExtensions, ALL_FILE_EXTENSIONS_BYTE_SIZE) != 0) {
				fStylesModified |= STYLESMODIFIED_FILE_EXT;
			}
		}
		if (!(fStylesModified & STYLESMODIFIED_COLOR)) {
			if (memcmp(colorBackup, customColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF)) != 0) {
				fStylesModified |= STYLESMODIFIED_COLOR;
			}
		}
		if (!(fStylesModified & STYLESMODIFIED_ALL_STYLE)) {
			int count = 0;
			for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
				PEDITLEXER pLex = pLexArray[iLexer];
				if (pLex->bStyleChanged || memcmp(styleBackup[iLexer], pLex->szStyleBuf, pLex->iStyleBufSize) != 0) {
					pLex->bStyleChanged = TRUE;
					++count;
				}
			}
			fStylesModified |= (count == 0) ? STYLESMODIFIED_NONE : ((count == NUMLEXERS) ? STYLESMODIFIED_ALL_STYLE : STYLESMODIFIED_SOME_STYLE);
		}
		if ((fStylesModified & STYLESMODIFIED_WARN_MASK) && StrIsEmpty(szIniFile) && !fWarnedNoIniFile) {
			MsgBox(MBWARN, IDS_SETTINGSNOTSAVED);
			fWarnedNoIniFile = TRUE;
		}
	}

	NP2HeapFree(extBackup);
	for (int iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		NP2HeapFree(styleBackup[iLexer]);
	}

	// Apply new (or previous) Styles
	if (apply) {
		Style_SetLexer(hwnd, pLexCurrent);
	}
}

//=============================================================================
//
// Style_SelectLexerDlgProc()
//
static INT_PTR CALLBACK Style_SelectLexerDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static int iInternalDefault;

	switch (umsg) {
	case WM_INITDIALOG: {
		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, L"", -1, 0, 0, 0 };
		ResizeDlg_Init(hwnd, cxStyleSelectDlg, cyStyleSelectDlg, IDC_RESIZEGRIP3);

		HWND hwndLV = GetDlgItem(hwnd, IDC_STYLELIST);
		InitWindowCommon(hwndLV);

		SHFILEINFO shfi;
		ListView_SetImageList(hwndLV, (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi,
							  sizeof(SHFILEINFO), SHGFI_SMALLICON | SHGFI_SYSICONINDEX), LVSIL_SMALL);

		ListView_SetImageList(hwndLV, (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi,
							  sizeof(SHFILEINFO), SHGFI_LARGEICON | SHGFI_SYSICONINDEX), LVSIL_NORMAL);

		//SetExplorerTheme(hwndLV);
		ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
		ListView_InsertColumn(hwndLV, 0, &lvc);

		// Add lexers
		for (int i = 0; i < NUMLEXERS; i++) {
			Style_AddLexerToListView(hwndLV, pLexArray[i]);
		}

		ListView_SetColumnWidth(hwndLV, 0, LVSCW_AUTOSIZE_USEHEADER);

		// Select current lexer
		const int lvItems = ListView_GetItemCount(hwndLV);
		LVITEM lvi;
		lvi.mask = LVIF_PARAM;
		for (int i = 0; i < lvItems; i++) {
			lvi.iItem = i;
			ListView_GetItem(hwndLV, &lvi);
			if (StrEqual(((PEDITLEXER)lvi.lParam)->pszName, pLexCurrent->pszName)) {
				ListView_SetItemState(hwndLV, i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EnsureVisible(hwndLV, i, FALSE);
				if (iDefaultLexer == i) {
					CheckDlgButton(hwnd, IDC_DEFAULTSCHEME, BST_CHECKED);
				}
			}
		}

		iInternalDefault = iDefaultLexer;

		if (bAutoSelect) {
			CheckDlgButton(hwnd, IDC_AUTOSELECT, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		ResizeDlg_Destroy(hwnd, &cxStyleSelectDlg, &cyStyleSelectDlg);
		return FALSE;

	case WM_SIZE: {
		int dx, dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);
		HDWP hdwp = BeginDeferWindowPos(6);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP3, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLELIST, dx, dy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_AUTOSELECT, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_DEFAULTSCHEME, 0, dy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);

		ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_STYLELIST), 0, LVSCW_AUTOSIZE_USEHEADER);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY:
		if (((LPNMHDR)(lParam))->idFrom == IDC_STYLELIST) {
			switch (((LPNMHDR)(lParam))->code) {
			case NM_DBLCLK:
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDOK, 1), 0);
				break;

			case LVN_ITEMCHANGED:
			case LVN_DELETEITEM: {
				const int i = ListView_GetNextItem(GetDlgItem(hwnd, IDC_STYLELIST), -1, LVNI_ALL | LVNI_SELECTED);
				if (iInternalDefault == i) {
					CheckDlgButton(hwnd, IDC_DEFAULTSCHEME, BST_CHECKED);
				} else {
					CheckDlgButton(hwnd, IDC_DEFAULTSCHEME, BST_UNCHECKED);
				}
				EnableWindow(GetDlgItem(hwnd, IDC_DEFAULTSCHEME), i != -1);
				EnableWindow(GetDlgItem(hwnd, IDOK), i != -1);
			}
			break;
			}
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_DEFAULTSCHEME:
			if (IsButtonChecked(hwnd, IDC_DEFAULTSCHEME)) {
				iInternalDefault = ListView_GetNextItem(GetDlgItem(hwnd, IDC_STYLELIST), -1, LVNI_ALL | LVNI_SELECTED);
			} else {
				iInternalDefault = 0;
			}
			break;

		case IDOK: {
			HWND hwndLV = GetDlgItem(hwnd, IDC_STYLELIST);
			LVITEM lvi;

			lvi.mask = LVIF_PARAM;
			lvi.iItem = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_SELECTED);
			if (ListView_GetItem(hwndLV, &lvi)) {
				pLexCurrent = (PEDITLEXER)lvi.lParam;
				np2LexLangIndex = 0;
				iDefaultLexer = iInternalDefault;
				bAutoSelect = IsButtonChecked(hwnd, IDC_AUTOSELECT);
				EndDialog(hwnd, IDOK);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// Style_SelectLexerDlg()
//
void Style_SelectLexerDlg(HWND hwnd) {
	if (IDOK == ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_STYLESELECT), GetParent(hwnd), Style_SelectLexerDlgProc, 0)) {
		Style_SetLexer(hwnd, pLexCurrent);
	}
}

// End of Styles.c
