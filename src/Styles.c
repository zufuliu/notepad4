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

extern EDITLEXER lexGlobal;
extern EDITLEXER lex2ndGlobal;
extern EDITLEXER lexTextFile;
extern EDITLEXER lex2ndTextFile;

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

// the two global lexers at the end of the array not visible in "Select Scheme" list, doesn't participate in file extension match
#define ALL_LEXER_COUNT		(NUMLEXERS + 2)
// This array holds all the lexers...
static const PEDITLEXER pLexArray[ALL_LEXER_COUNT] = {
	&lexTextFile,
	&lex2ndTextFile,

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

	&lexANSI,

	&lexGlobal,
	&lex2ndGlobal,
};

// system available default monospaced font and proportional font
static WCHAR systemCodeFontName[LF_FACESIZE];
static WCHAR systemTextFontName[LF_FACESIZE];
// global default monospaced font and proportional font
static WCHAR defaultCodeFontName[LF_FACESIZE];
static WCHAR defaultTextFontName[LF_FACESIZE];

static WCHAR darkStyleThemeFilePath[MAX_PATH];

// Currently used lexer
static PEDITLEXER pLexGlobal = &lexGlobal;
PEDITLEXER pLexCurrent = &lexTextFile;
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

static BOOL iCustomColorLoaded = FALSE;

BOOL	bUse2ndGlobalStyle;
int		np2StyleTheme;
BOOL	bCurrentLexerHasLineComment;
BOOL	bCurrentLexerHasBlockComment;
static UINT fStylesModified = STYLESMODIFIED_NONE;
static BOOL fWarnedNoIniFile = FALSE;
static int	iBaseFontSize = 11*SC_FONT_SIZE_MULTIPLIER; // 11 pt
int		iFontQuality = SC_EFF_QUALITY_LCD_OPTIMIZED;
int		iCaretStyle = 1; // width 1, 0 for block
int		iOvrCaretStyle = 0; // 0 for bar, 1 for block
int		iCaretBlinkPeriod = -1; // system default, 0 for noblink
static BOOL bBookmarkColorUpdated;
static int	iDefaultLexer;
static BOOL bAutoSelect;
int		cxStyleSelectDlg;
int		cyStyleSelectDlg;
int		cxStyleCustomizeDlg;
int		cyStyleCustomizeDlg;

#define ALL_FILE_EXTENSIONS_BYTE_SIZE	((NUMLEXERS * MAX_EDITLEXER_EXT_SIZE) * sizeof(WCHAR))
static LPWSTR g_AllFileExtensions = NULL;

// Notepad2.c
extern int	iEncoding;
extern int	g_DOSEncoding;
extern int	iDefaultCodePage;
extern int	iDefaultCharSet;
extern INT	iHighlightCurrentLine;
extern BOOL	bShowSelectionMargin;

#define STYLE_MASK_FONT_FACE	(1 << 0)
#define STYLE_MASK_FONT_SIZE	(1 << 1)
#define STYLE_MASK_FORE_COLOR	(1 << 2)
#define STYLE_MASK_BACK_COLOR	(1 << 3)
#define STYLE_MASK_FONT_WEIGHT	(1 << 4)
#define STYLE_MASK_FORCE_CASE	(1 << 5)
#define STYLE_MASK_CHARSET		(1 << 6)

#ifndef LOCALE_NAME_MAX_LENGTH
#define LOCALE_NAME_MAX_LENGTH	85
#endif

#if LF_FACESIZE > LOCALE_NAME_MAX_LENGTH
#define MAX_STYLE_VALUE_LENGTH	LF_FACESIZE
#else
#define MAX_STYLE_VALUE_LENGTH	LOCALE_NAME_MAX_LENGTH
#endif

struct DetailStyle {
	UINT mask;
	int fontSize;
	int foreColor;
	int backColor;
	int weight;
	BOOL italic;
	BOOL underline;
	BOOL strike;
	BOOL eolFilled;
	int forceCase;
	int charset;
	WCHAR fontWide[LF_FACESIZE];
	char fontFace[LF_FACESIZE * kMaxMultiByteCount];
};

/*
style in other lexers is inherited from it's lexer default (first) style and global default style.
	This also means other "Default" styles in lexHTML don't work as expected (bug by b7e7585f869897276e27a3b83b5b91a7196ca4da).
	Maybe it's better to remove them instead of confusing users.

font quality, caret style, caret width, caret blink period are moved to "Settings" section,
see above variables and the "View" menu.
*/
//! keep same order as lexGlobal
enum DefaultStyleIndex {
	Style_DefaultCode,		// global default code style.
	Style_DefaultText,		// global default text style.
	Style_LineNumber,		// inherited style, except for background color (default to COLOR_3DFACE).
	Style_MatchBrace,		// inherited style.
	Style_MatchBraceError,	// inherited style.
	Style_ControlCharacter,	// inherited style. font attributes (white on black)
	Style_IndentationGuide,	// inherited style. `fore`, `back`
	Style_Selection,		// standalone style. `fore`, `back`, `alpha`, `eolfilled`
	Style_Whitespace,		// standalone style. `fore`, `back`, `size`: dot size
	Style_CurrentLine,		// standalone style. frame (`fore`, `size`, `alpha`), background (`back`, `alpha`)
	Style_Caret,			// standalone style. `fore`: caret color
	Style_IMEIndicator,		// standalone style. `fore`: IME indicator color
	Style_LongLineMarker,	// standalone style. `fore`: edge line color, `back`: edge background color
	Style_ExtraLineSpacing,	// standalone style. descent = `size`/2, ascent = `size` - descent
	Style_FoldingMarker,	// standalone style. `fore`: folder line color, `back`: folder box fill color
	Style_FoldDispalyText,	// inherited style.
	Style_MarkOccurrences,	// standalone style. `fore`, `alpha`
	Style_Bookmark,			// standalone style. `fore`, `back`, `alpha`
};

// folding marker
#define FoldingMarkerLineColorDefault	RGB(0x80, 0x80, 0xFF)
#define FoldingMarkerFillColorDefault	RGB(0xAD, 0xD8, 0xE6)
#define FoldingMarkerLineColorDark		RGB(0x80, 0x80, 0x80)
#define FoldingMarkerFillColorDark		RGB(0x60, 0x60, 0x60)

// from ScintillaWin.cxx
#define SC_INDICATOR_INPUT		INDIC_IME
#define SC_INDICATOR_TARGET		(INDIC_IME + 1)
#define SC_INDICATOR_CONVERTED	(INDIC_IME + 2)
#define SC_INDICATOR_UNKNOWN	INDIC_IME_MAX

#define IMEIndicatorDefaultColor	RGB(0x10, 0x80, 0x10)
#define MarkOccurrencesDefaultAlpha	100

#define	BookmarkImageDefaultColor	RGB(0x40, 0x80, 0x40)
#define	BookmarkLineDefaultColor	RGB(0, 0xff, 0)
#define BookmarkLineDefaultAlpha	40

#define BookmarkUsingPixmapImage		0
#if BookmarkUsingPixmapImage
// XPM Graphics for bookmark on selection margin.
/* GIMP export Bookmark2_16x.png with Alpha threshold 127 */
static char bookmark_pixmap_color[16];
#define bookmark_pixmap_color_fmt	".	c #%06X"
static const char* const bookmark_pixmap[] = {
"16 16 2 1",
" 	c None",
//".	c #408040",
bookmark_pixmap_color,
"                ",
"  ............  ",
"  ............  ",
"  ............  ",
"  ............  ",
"  ............  ",
"  ............  ",
"  ............  ",
"  ............  ",
"  ............  ",
"  .....  .....  ",
"  ....    ....  ",
"  ...      ...  ",
"  ..        ..  ",
"  .          .  ",
"                "
};
#endif

// style UI controls on Customize Schemes dialog
enum {
	StyleControl_None = 0,
	StyleControl_Font = 1,
	StyleControl_Fore = 2,
	StyleControl_Back = 4,
	StyleControl_EOLFilled = 8,
	StyleControl_All = StyleControl_Font | StyleControl_Fore | StyleControl_Back | StyleControl_EOLFilled,
};

static inline BOOL IsGlobalBaseStyleIndex(int index) {
	return index == Style_DefaultCode || index == Style_DefaultText;
}

static inline UINT GetDefaultStyleControlMask(int index) {
	switch (index) {
	case Style_ControlCharacter:
		return StyleControl_Font;
	case Style_IndentationGuide:
	case Style_Whitespace:
	case Style_CurrentLine:
	case Style_LongLineMarker:
	case Style_FoldingMarker:
	case Style_Bookmark:
		return StyleControl_Fore | StyleControl_Back;
	case Style_Selection:
		return StyleControl_Fore | StyleControl_Back | StyleControl_EOLFilled;
	case Style_Caret:
	case Style_IMEIndicator:
	case Style_MarkOccurrences:
		return StyleControl_Fore;
	case Style_ExtraLineSpacing:
		return StyleControl_None;
	default:
		return StyleControl_All;
	}
}

static inline void FindSystemDefaultCodeFont(void) {
	LPCWSTR const commonCodeFontName[] = {
		L"DejaVu Sans Mono",
		L"Consolas",			// Vista and above
		//L"Source Code Pro",
		//L"Liberation Mono",
		//L"Droid Sans Mono",
		//L"Inconsolata",		// alternative to Consolas
	};

	for (UINT i = 0; i < (UINT)COUNTOF(commonCodeFontName); i++) {
		LPCWSTR fontName = commonCodeFontName[i];
		if (IsFontAvailable(fontName)) {
			lstrcpy(systemCodeFontName, fontName);
			return;
		}
	}
	lstrcpy(systemCodeFontName, L"Courier New");
}

static inline void FindSystemDefaultTextFont(void) {
	if (IsVistaAndAbove()) {
		NONCLIENTMETRICS ncm;
		ZeroMemory(&ncm, sizeof(ncm));
		ncm.cbSize = sizeof(ncm);
#if (WINVER >= _WIN32_WINNT_VISTA)
		if (!IsVistaAndAbove()) {
			ncm.cbSize -= sizeof(ncm.iPaddedBorderWidth);
		}
#endif
		if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0)) {
			lstrcpyn(systemTextFontName, ncm.lfMessageFont.lfFaceName, COUNTOF(systemTextFontName));
			return;
		}
	}

	// Windows 2000, XP, 2003
	lstrcpy(systemTextFontName, L"Tahoma");
}

void Style_DetectBaseFontSize(HWND hwnd) {
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	const int cxScreen = mi.rcMonitor.right - mi.rcMonitor.left;
	const int cyScreen = mi.rcMonitor.bottom - mi.rcMonitor.top;

	int size = 11*SC_FONT_SIZE_MULTIPLIER; // 11 pt
	// https://en.wikipedia.org/wiki/Display_resolution
	if (cxScreen < 1920 && cyScreen < 1080) {
		// SVGA		800 x 600
		// XGA		1024 x 768
		// WXGA		1280 x 720, 1280 x 800
		// SXGA		1280 x 1024
		// HD		1360 x 768, 1366 x 768
		// WXGA+	1440 x 900
		// Other	1536 x 864
		// HD+		1600 x 900
		// WSXGA+	1680 x 1050
		size = 10*SC_FONT_SIZE_MULTIPLIER + SC_FONT_SIZE_MULTIPLIER/2; // 10.5 pt
	}
#if 0
	else if (cxScreen < 2560 && cyScreen < 1440) {
		// FHD		1920 x 1080
		// WUXGA	1920 x 1200
		// QWXGA	2040 x 1152
		// Other	2560 x 1080
	} else if (cxScreen < 3840 && cyScreen < 2160) {
		// QHD		2560 x 1440
		// Other	3440 x 1440
	} else if (cxScreen >= 3840 && cyScreen >= 2160) {
		// 4K UHD	3840 x 2160
	} else {
		// Other
	}
#endif
	iBaseFontSize = size;
}

void Style_ReleaseResources(void) {
	NP2HeapFree(g_AllFileExtensions);
	for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		if (pLex->szStyleBuf) {
			NP2HeapFree(pLex->szStyleBuf);
		}
	}
}

static inline BOOL IsStyleLoaded(PEDITLEXER pLex) {
	return pLex->bStyleTheme == np2StyleTheme && pLex->szStyleBuf != NULL;
}

static inline LPCWSTR GetStyleThemeFilePath(void) {
	return (np2StyleTheme == StyleTheme_Dark) ? darkStyleThemeFilePath : szIniFile;
}

static inline LPCWSTR GetStyleThemeTitle(void) {
	return (np2StyleTheme == StyleTheme_Dark) ? L"Dark Theme" : L"Default Theme";
}

static inline void FindDarkThemeFile(void) {
	FindExtraIniFile(darkStyleThemeFilePath, L"Notepad2 DarkTheme.ini", L"DarkTheme.ini");
}

static void Style_LoadOneEx(PEDITLEXER pLex, IniSection *pIniSection, WCHAR *pIniSectionBuf, int cchIniSection) {
	LPCWSTR themePath = GetStyleThemeFilePath();
	GetPrivateProfileSection(pLex->pszName, pIniSectionBuf, cchIniSection, themePath);

	const UINT iStyleCount = pLex->iStyleCount;
	LPWSTR szStyleBuf = pLex->szStyleBuf;
	if (szStyleBuf == NULL) {
		szStyleBuf = (LPWSTR)NP2HeapAlloc(EDITSTYLE_BufferSize(iStyleCount));
		pLex->szStyleBuf = szStyleBuf;
	}
	if (!IniSectionParse(pIniSection, pIniSectionBuf)) {
		pLex->bUseDefaultCodeStyle = pLex->bUseDefaultCodeStyle_Default;
		for (UINT i = 0; i < iStyleCount; i++) {
			LPWSTR szValue = szStyleBuf + (i * MAX_EDITSTYLE_VALUE_SIZE);
			pLex->Styles[i].szValue = szValue;
			lstrcpy(szValue, pLex->Styles[i].pszDefault);
		}
	} else {
		pLex->bUseDefaultCodeStyle = (BYTE)IniSectionGetBool(pIniSection, L"UseDefaultCodeStyle", pLex->bUseDefaultCodeStyle_Default);
		for (UINT i = 0; i < iStyleCount; i++) {
			LPWSTR szValue = szStyleBuf + (i * MAX_EDITSTYLE_VALUE_SIZE);
			pLex->Styles[i].szValue = szValue;
			LPCWSTR value = IniSectionGetValueImpl(pIniSection, pLex->Styles[i].pszName, pLex->Styles[i].iNameLen);
			if (value != NULL) {
				lstrcpyn(szValue, value, MAX_EDITSTYLE_VALUE_SIZE);
			} else {
				lstrcpy(szValue, pLex->Styles[i].pszDefault);
			}
		}
	}

	pLex->bStyleTheme = (BYTE)np2StyleTheme;
}

//=============================================================================
//
// Style_Load()
//
void Style_Load(void) {
	IniSection section;
	g_AllFileExtensions = (LPWSTR)NP2HeapAlloc(ALL_FILE_EXTENSIONS_BYTE_SIZE);
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);

	LoadIniSection(INI_SECTION_NAME_STYLES, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	// 2nd default
	bUse2ndGlobalStyle = IniSectionGetBool(pIniSection, L"Use2ndGlobalStyle", 0);
	pLexGlobal = bUse2ndGlobalStyle ? &lex2ndGlobal : &lexGlobal;

	// default scheme
	int iValue = IniSectionGetInt(pIniSection, L"DefaultScheme", 0);
	iDefaultLexer = clamp_i(iValue, 0, NUMLEXERS - 1);

	iValue = IniSectionGetInt(pIniSection, L"StyleTheme", StyleTheme_Default);
	np2StyleTheme = clamp_i(iValue, StyleTheme_Default, StyleTheme_Max);

	// auto select
	bAutoSelect = IniSectionGetBool(pIniSection, L"AutoSelect", 1);

	LPCWSTR strValue = IniSectionGetValue(pIniSection, L"DarkTheme.ini");
	if (StrNotEmpty(strValue)) {
		lstrcpyn(darkStyleThemeFilePath, strValue, COUNTOF(darkStyleThemeFilePath));
	}

	LoadIniSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);
	for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		pLex->szExtensions = g_AllFileExtensions + (iLexer * MAX_EDITLEXER_EXT_SIZE);
		LPCWSTR value = IniSectionGetValueImpl(pIniSection, pLex->pszName, pLex->iNameLen);
		if (StrIsEmpty(value)) {
			lstrcpy(pLex->szExtensions, pLex->pszDefExt);
		} else {
			lstrcpyn(pLex->szExtensions, value, MAX_EDITLEXER_EXT_SIZE);
		}
	}

	if (np2StyleTheme == StyleTheme_Dark) {
		FindDarkThemeFile();
	}
	Style_LoadOneEx(pLexGlobal, pIniSection, pIniSectionBuf, cchIniSection);
	Style_LoadOneEx(pLexArray[iDefaultLexer], pIniSection, pIniSectionBuf, cchIniSection);

	FindSystemDefaultCodeFont();
	FindSystemDefaultTextFont();

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

static void Style_LoadOne(PEDITLEXER pLex) {
	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);
	Style_LoadOneEx(pLex, pIniSection, pIniSectionBuf, cchIniSection);
	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

static void Style_LoadAll(BOOL bFore) {
	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);

	// Custom colors
	const int value = (np2StyleTheme << 1) | 1;
	if (bFore || iCustomColorLoaded != value) {
		LPCWSTR themePath = GetStyleThemeFilePath();
		iCustomColorLoaded = value;
		CopyMemory(customColor, defaultCustomColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));

		GetPrivateProfileSection(INI_SECTION_NAME_CUSTOM_COLORS, pIniSectionBuf, cchIniSection, themePath);
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

	for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		if (bFore || !IsStyleLoaded(pLex)) {
			Style_LoadOneEx(pLex, pIniSection, pIniSectionBuf, cchIniSection);
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
//	Style_Save()
//
void Style_Save(void) {
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const SIZE_T cbIniSection = NP2HeapSize(pIniSectionBuf);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	// 2nd default
	IniSectionSetBoolEx(pIniSection, L"Use2ndGlobalStyle", bUse2ndGlobalStyle, 0);

	// default scheme
	IniSectionSetIntEx(pIniSection, L"DefaultScheme", iDefaultLexer, 0);
	IniSectionSetIntEx(pIniSection, L"StyleTheme", np2StyleTheme, StyleTheme_Default);

	// auto select
	IniSectionSetBoolEx(pIniSection, L"AutoSelect", bAutoSelect, 1);

	SaveIniSection(INI_SECTION_NAME_STYLES, pIniSectionBuf);

	if (fStylesModified & STYLESMODIFIED_FILE_EXT) {
		ZeroMemory(pIniSectionBuf, cbIniSection);
		pIniSection->next = pIniSectionBuf;
		for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			const LPCEDITLEXER pLex = pLexArray[iLexer];
			IniSectionSetStringEx(pIniSection, pLex->pszName, pLex->szExtensions, pLex->pszDefExt);
		}
		SaveIniSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf);
		fStylesModified &= ~STYLESMODIFIED_FILE_EXT;
	}

	if (!(fStylesModified & (STYLESMODIFIED_COLOR | STYLESMODIFIED_STYLE_MASK))) {
		fStylesModified = STYLESMODIFIED_NONE;
		NP2HeapFree(pIniSectionBuf);
		return;
	}

	// save changes to each theme
	LPCWSTR themePath = GetStyleThemeFilePath();
	if (np2StyleTheme != StyleTheme_Default) {
		if (!CreateIniFileEx(themePath)) {
			NP2HeapFree(pIniSectionBuf);
			MsgBox(MBWARN, IDS_CREATEINI_FAIL);
			return;
		}
	}

	// Custom colors
	if (fStylesModified & STYLESMODIFIED_COLOR) {
		ZeroMemory(pIniSectionBuf, cbIniSection);
		pIniSection->next = pIniSectionBuf;
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
		WritePrivateProfileSection(INI_SECTION_NAME_CUSTOM_COLORS, pIniSectionBuf, themePath);
	}

	if (fStylesModified & STYLESMODIFIED_STYLE_MASK) {
		for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			if (!pLex->bStyleChanged) {
				continue;
			}

			ZeroMemory(pIniSectionBuf, cbIniSection);
			pIniSection->next = pIniSectionBuf;
			IniSectionSetBoolEx(pIniSection, L"UseDefaultCodeStyle", pLex->bUseDefaultCodeStyle, pLex->bUseDefaultCodeStyle_Default);
			const UINT iStyleCount = pLex->iStyleCount;
			for (UINT i = 0; i < iStyleCount; i++) {
				IniSectionSetStringEx(pIniSection, pLex->Styles[i].pszName, pLex->Styles[i].szValue, pLex->Styles[i].pszDefault);
			}
			// delete this section if nothing changed
			WritePrivateProfileSection(pLex->pszName, StrIsEmpty(pIniSectionBuf) ? NULL : pIniSectionBuf, themePath);
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
					  | OFN_PATHMUSTEXIST | OFN_SHAREAWARE /*| OFN_NODEREFERENCELINKS*/ | OFN_NOVALIDATE;

	if (GetOpenFileName(&ofn)) {
		IniSection section;
		WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
		const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
		IniSection *pIniSection = &section;

		IniSectionInit(pIniSection, 128);
		if (GetPrivateProfileSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, cchIniSection, szFile)) {
			if (IniSectionParse(pIniSection, pIniSectionBuf)) {
				for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
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

		for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			if (GetPrivateProfileSection(pLex->pszName, pIniSectionBuf, cchIniSection, szFile)) {
				if (!IniSectionParse(pIniSection, pIniSectionBuf)) {
					continue;
				}
				pLex->bUseDefaultCodeStyle = (BYTE)IniSectionGetBool(pIniSection, L"UseDefaultCodeStyle", pLex->bUseDefaultCodeStyle_Default);
				const UINT iStyleCount = pLex->iStyleCount;
				for (UINT i = 0; i < iStyleCount; i++) {
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
		WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
		const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
		IniSectionOnSave *pIniSection = &section;

		pIniSection->next = pIniSectionBuf;
		for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			const LPCEDITLEXER pLex = pLexArray[iLexer];
			IniSectionSetString(pIniSection, pLex->pszName, pLex->szExtensions);
		}
		if (!WritePrivateProfileSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, szFile)) {
			dwError = GetLastError();
		}

		for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
			const LPCEDITLEXER pLex = pLexArray[iLexer];
			ZeroMemory(pIniSectionBuf, cchIniSection);
			pIniSection->next = pIniSectionBuf;
			IniSectionSetBool(pIniSection, L"UseDefaultCodeStyle", pLex->bUseDefaultCodeStyle);
			const UINT iStyleCount = pLex->iStyleCount;
			for (UINT i = 0; i < iStyleCount; i++) {
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

static void Style_ResetAll(BOOL resetColor) {
	if (resetColor) {
		CopyMemory(customColor, defaultCustomColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));
	}
	for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer];
		if (pLex->szExtensions) {
			lstrcpy(pLex->szExtensions, pLex->pszDefExt);
		}
		pLex->bStyleChanged = TRUE;
		pLex->bUseDefaultCodeStyle = pLex->bUseDefaultCodeStyle_Default;
		if (resetColor) {
			const UINT iStyleCount = pLex->iStyleCount;
			for (UINT i = 0; i < iStyleCount; i++) {
				lstrcpy(pLex->Styles[i].szValue, pLex->Styles[i].pszDefault);
			}
		}
	}

	fStylesModified |= STYLESMODIFIED_ALL_STYLE | STYLESMODIFIED_FILE_EXT | STYLESMODIFIED_COLOR;
}

void Style_OnDPIChanged(HWND hwnd) {
	Style_SetLexer(hwnd, pLexCurrent);
}

void Style_OnStyleThemeChanged(HWND hwnd, int theme) {
	if (theme != StyleTheme_Default) {
		if (!PathFileExists(darkStyleThemeFilePath)) {
			FindDarkThemeFile();
		}
	}

	if (fStylesModified) {
		SaveSettingsNow(TRUE, TRUE);
	}
	np2StyleTheme = theme;
	Style_SetLexer(hwnd, pLexCurrent);
}

void Style_UpdateCaret(HWND hwnd) {
	// caret style and width
	const int style = (iCaretStyle ? CARETSTYLE_LINE : CARETSTYLE_BLOCK)
		| (iOvrCaretStyle ? CARETSTYLE_OVERSTRIKE_BLOCK : CARETSTYLE_OVERSTRIKE_BAR);
	SendMessage(hwnd, SCI_SETCARETSTYLE, style, 0);
	if (iCaretStyle != 0) {
		SendMessage(hwnd, SCI_SETCARETWIDTH, iCaretStyle, 0);
	}

	const int iValue = (iCaretBlinkPeriod < 0)? (int)GetCaretBlinkTime() : iCaretBlinkPeriod;
	SendMessage(hwnd, SCI_SETCARETPERIOD, iValue, 0);
}

void Style_UpdateLexerKeywordAttr(LPCEDITLEXER pLexNew) {
	ZeroMemory(currentLexKeywordAttr, sizeof(currentLexKeywordAttr));
	UINT8 *attr = currentLexKeywordAttr;

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

static inline BOOL DidLexerHasLineComment(int iLexer) {
	return !(iLexer == SCLEX_NULL
		|| iLexer == SCLEX_DIFF
	);
}

static inline BOOL DidLexerHasBlockComment(int iLexer, int rid) {
	return !(iLexer == SCLEX_NULL
		|| iLexer == SCLEX_BASH
		|| iLexer == SCLEX_BATCH
		|| iLexer == SCLEX_CONF
		|| iLexer == SCLEX_DIFF
		|| iLexer == SCLEX_LLVM
		|| iLexer == SCLEX_MAKEFILE
		|| iLexer == SCLEX_PERL
		|| iLexer == SCLEX_PROPERTIES
		|| iLexer == SCLEX_PYTHON
		|| iLexer == SCLEX_RUBY
		|| iLexer == SCLEX_SMALI
		|| iLexer == SCLEX_TEXINFO
		|| iLexer == SCLEX_VB
		|| iLexer == SCLEX_VBSCRIPT
		|| iLexer == SCLEX_VIM
		|| rid == NP2LEX_AWK
		|| rid == NP2LEX_JAM
	);
}

static void Style_Parse(struct DetailStyle *style, LPCWSTR lpszStyle);
static void Style_SetParsed(HWND hwnd, const struct DetailStyle *style, int iStyle);
static inline void Style_SetDefaultStyle(HWND hwnd, int index) {
	Style_SetStyles(hwnd, pLexGlobal->Styles[index].iStyle, pLexGlobal->Styles[index].szValue);
}

static inline BOOL Style_StrGetAttributeEx(LPCWSTR lpszStyle, LPCWSTR key, int keyLen) {
	LPCWSTR p = StrStr(lpszStyle, key);
	if (p != NULL) {
		const WCHAR ch1 = (p == lpszStyle) ? L' ' : p[-1];
		const WCHAR ch2 = p[keyLen];
		return (ch1 == L' ' || ch1 == L';') && (ch2 == L'\0' || ch2 == L' ' || ch2 == L';');
	}
	return FALSE;
}

#define Style_StrGetAttribute(lpszStyle, name)	Style_StrGetAttributeEx((lpszStyle), (name), CSTRLEN(name))
#define Style_StrGetBold(lpszStyle)				Style_StrGetAttribute((lpszStyle), L"bold")
#define Style_StrGetItalic(lpszStyle)			Style_StrGetAttribute((lpszStyle), L"italic")
#define Style_StrGetUnderline(lpszStyle)		Style_StrGetAttribute((lpszStyle), L"underline")
#define Style_StrGetStrike(lpszStyle)			Style_StrGetAttribute((lpszStyle), L"strike")
#define Style_StrGetEOLFilled(lpszStyle)		Style_StrGetAttribute((lpszStyle), L"eolfilled")

void Style_InitDefaultColor(HWND hwnd) {
	PEDITLEXER pLexNew = pLexArray[iDefaultLexer];
	int iValue = pLexNew->bUseDefaultCodeStyle ? Style_DefaultCode : Style_DefaultText;
	LPCWSTR szValue = pLexGlobal->Styles[iValue].szValue;
	if (!Style_StrGetColor(TRUE, szValue, &iValue)) {
		if (!Style_StrGetColor(TRUE, pLexNew->Styles[0].szValue, &iValue)) {
			iValue = GetSysColor(COLOR_WINDOWTEXT);
		}
	}
	SendMessage(hwnd, SCI_STYLESETFORE, STYLE_DEFAULT, iValue);
	if (!Style_StrGetColor(FALSE, szValue, &iValue)) {
		if (!Style_StrGetColor(FALSE, pLexNew->Styles[0].szValue, &iValue)) {
			iValue = GetSysColor(COLOR_WINDOW);
		}
	}
	SendMessage(hwnd, SCI_STYLESETBACK, STYLE_DEFAULT, iValue);
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
	if (!IsStyleLoaded(pLexGlobal)) {
		Style_LoadOne(pLexGlobal);
	}
	if (!IsStyleLoaded(pLexNew)) {
		Style_LoadOne(pLexNew);
	}

	// Lexer
	const int iLexer = pLexNew->iLexer;
	SendMessage(hwnd, SCI_SETLEXER, iLexer, 0);
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
				char *lowerKeywords = (char *)NP2HeapAlloc(len + 1);
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

	// Font quality setup
	SendMessage(hwnd, SCI_SETFONTQUALITY, iFontQuality, 0);

	// Clear
	SendMessage(hwnd, SCI_CLEARDOCUMENTSTYLE, 0, 0);

	// Default Values are always set
	SendMessage(hwnd, SCI_STYLERESETDEFAULT, 0, 0);
	SendMessage(hwnd, SCI_STYLESETCHARACTERSET, STYLE_DEFAULT, DEFAULT_CHARSET);

	//! begin Style_Default
	Style_StrGetFontEx(pLexGlobal->Styles[Style_DefaultCode].szValue, defaultCodeFontName, COUNTOF(defaultCodeFontName), TRUE);
	Style_StrGetFontEx(pLexGlobal->Styles[Style_DefaultText].szValue, defaultTextFontName, COUNTOF(defaultTextFontName), TRUE);

	iValue = pLexNew->bUseDefaultCodeStyle ? Style_DefaultCode : Style_DefaultText;
	LPCWSTR szValue = pLexGlobal->Styles[iValue].szValue;
	// base font size
	if (!Style_StrGetFontSize(szValue, &iBaseFontSize)) {
		iValue = DefaultToCurrentDPI(iBaseFontSize);
		SendMessage(hwnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, iValue);
	}
	Style_SetStyles(hwnd, STYLE_DEFAULT, szValue);

	// Auto-select codepage according to charset
	//Style_SetACPfromCharSet(hwnd);

	// used in Direct2D for language dependent glyphs
	if (IsVistaAndAbove()) {
		// current user default locale (empty) to override "en-US" in Scintilla.
		WCHAR localeWide[LOCALE_NAME_MAX_LENGTH] = L"";
		char localeName[LOCALE_NAME_MAX_LENGTH] = "";
#if 0
		if (!Style_StrGetLocale(szValue, localeWide, COUNTOF(localeWide))) {
			 //GetUserDefaultLocaleName(localeWide, COUNTOF(localeWide));
			 GetLocaleInfo(0 /*LOCALE_NAME_USER_DEFAULT*/, 0x0000005c /*LOCALE_SNAME*/, localeWide, COUNTOF(localeWide));
		}
		WideCharToMultiByte(CP_UTF8, 0, localeWide, -1, localeName, COUNTOF(localeName), NULL, NULL);
#else
		if (Style_StrGetLocale(szValue, localeWide, COUNTOF(localeWide))) {
			WideCharToMultiByte(CP_UTF8, 0, localeWide, -1, localeName, COUNTOF(localeName), NULL, NULL);
		}
#endif
		SendMessage(hwnd, SCI_SETFONTLOCALE, 0, (LPARAM)localeName);
	}

	if (!Style_StrGetColor(TRUE, szValue, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETFORE, STYLE_DEFAULT, GetSysColor(COLOR_WINDOWTEXT));
	}
	if (!Style_StrGetColor(FALSE, szValue, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETBACK, STYLE_DEFAULT, GetSysColor(COLOR_WINDOW));
	}
	// lexer default (base style), i.e.: EDITSTYLE_DEFAULT
	Style_SetStyles(hwnd, STYLE_DEFAULT, pLexNew->Styles[0].szValue);
	// set all styles to have the same attributes as STYLE_DEFAULT.
	SendMessage(hwnd, SCI_STYLECLEARALL, 0, 0);
	//! end Style_Default

	Style_SetDefaultStyle(hwnd, Style_LineNumber);
	Style_SetDefaultStyle(hwnd, Style_MatchBrace);
	Style_SetDefaultStyle(hwnd, Style_MatchBraceError);
	if (rid != NP2LEX_ANSI) {
		Style_SetDefaultStyle(hwnd, Style_ControlCharacter);
	}
	Style_SetDefaultStyle(hwnd, Style_IndentationGuide);

	//! begin Style_Selection
	szValue = pLexGlobal->Styles[Style_Selection].szValue;
	if (Style_StrGetColor(TRUE, szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETSELFORE, TRUE, iValue);
		SendMessage(hwnd, SCI_SETADDITIONALSELFORE, iValue, 0);
	} else {
		SendMessage(hwnd, SCI_SETSELFORE, FALSE, 0);
		SendMessage(hwnd, SCI_SETADDITIONALSELFORE, 0, 0);
	}
	// always set background color
	if (!Style_StrGetColor(FALSE, szValue, &iValue)) {
		iValue = GetSysColor(COLOR_HIGHLIGHT);
	}
	SendMessage(hwnd, SCI_SETSELBACK, TRUE, iValue);
	SendMessage(hwnd, SCI_SETADDITIONALSELBACK, iValue, 0);

	if (!Style_StrGetAlpha(szValue, &iValue)) {
		iValue = SC_ALPHA_NOALPHA;
	}
	SendMessage(hwnd, SCI_SETSELALPHA, iValue, 0);
	SendMessage(hwnd, SCI_SETADDITIONALSELALPHA, iValue, 0);

	SendMessage(hwnd, SCI_SETSELEOLFILLED, Style_StrGetEOLFilled(szValue), 0);
	//! end Style_Selection

	//! begin Style_Whitespace
	szValue = pLexGlobal->Styles[Style_Whitespace].szValue;
	if (Style_StrGetColor(TRUE, szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETWHITESPACEFORE, TRUE, iValue);
	} else {
		SendMessage(hwnd, SCI_SETWHITESPACEFORE, FALSE, 0);
	}
	if (Style_StrGetColor(FALSE, szValue, &iValue)) {
		SendMessage(hwnd, SCI_SETWHITESPACEBACK, TRUE, iValue);
	} else {
		SendMessage(hwnd, SCI_SETWHITESPACEBACK, FALSE, 0);
	}

	// whitespace dot size
	iValue = 1;
	Style_StrGetRawSize(szValue, &iValue);
	iValue = max_i(1, RoundToCurrentDPI(iValue));
	SendMessage(hwnd, SCI_SETWHITESPACESIZE, iValue, 0);
	//! end Style_Whitespace

	Style_HighlightCurrentLine(hwnd);
	Style_UpdateCaret(hwnd);
	// caret fore
	if (!Style_StrGetColor(TRUE, pLexGlobal->Styles[Style_Caret].szValue, &iValue)) {
		iValue = GetSysColor(COLOR_WINDOWTEXT);
	}
	if (!VerifyContrast(iValue, (COLORREF)SendMessage(hwnd, SCI_STYLEGETBACK, 0, 0))) {
		iValue = (int)SendMessage(hwnd, SCI_STYLEGETFORE, STYLE_DEFAULT, 0);
	}
	SendMessage(hwnd, SCI_SETCARETFORE, iValue, 0);
	SendMessage(hwnd, SCI_SETADDITIONALCARETFORE, iValue, 0);
	// IME indicator
	szValue = pLexGlobal->Styles[Style_IMEIndicator].szValue;
	if (!Style_StrGetColor(TRUE, szValue, &iValue)) {
		iValue = IMEIndicatorDefaultColor;
	}
	SendMessage(hwnd, SCI_INDICSETFORE, SC_INDICATOR_INPUT, iValue);
	SendMessage(hwnd, SCI_INDICSETFORE, SC_INDICATOR_TARGET, iValue);
	SendMessage(hwnd, SCI_INDICSETFORE, SC_INDICATOR_CONVERTED, iValue);
	SendMessage(hwnd, SCI_INDICSETFORE, SC_INDICATOR_UNKNOWN, iValue);

	Style_SetLongLineColors(hwnd);
	// Extra Line Spacing
	if (rid != NP2LEX_ANSI && Style_StrGetRawSize(pLexGlobal->Styles[Style_ExtraLineSpacing].szValue, &iValue)) {
		int iAscent = 0;
		int iDescent = 0;
		iValue = max_i(0, RoundToCurrentDPI(iValue));
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
		COLORREF clrFore;
		COLORREF clrFill;

		szValue = pLexGlobal->Styles[Style_FoldingMarker].szValue;
		if (Style_StrGetColor(TRUE, szValue, &iValue)) {
			clrFore = iValue;
		} else {
			clrFore = (bUse2ndGlobalStyle || np2StyleTheme == StyleTheme_Dark) ? FoldingMarkerLineColorDark : FoldingMarkerLineColorDefault;
		}
		if (Style_StrGetColor(FALSE, szValue, &iValue)) {
			clrFill = iValue;
		} else {
			clrFill = (bUse2ndGlobalStyle || np2StyleTheme == StyleTheme_Dark) ? FoldingMarkerFillColorDark : FoldingMarkerFillColorDefault;
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

		for (UINT i = 0; i < (UINT)COUNTOF(iMarkerIDs); ++i) {
			const int marker = iMarkerIDs[i];
			SciCall_MarkerSetBack(marker, clrFore);
			SciCall_MarkerSetFore(marker, clrBack);
		}
		SciCall_MarkerSetFore(SC_MARKNUM_FOLDER, clrFill);
		SciCall_MarkerSetFore(SC_MARKNUM_FOLDEREND, clrFill);

		Style_SetDefaultStyle(hwnd, Style_FoldDispalyText);
	} // end set folding style

	if (SendMessage(hwnd, SCI_GETINDENTATIONGUIDES, 0, 0) != SC_IV_NONE) {
		Style_SetIndentGuides(hwnd, TRUE);
	}

	// Mark Occurrence
	szValue = pLexGlobal->Styles[Style_MarkOccurrences].szValue;
	if (!Style_StrGetColor(TRUE, szValue, &iValue)) {
		iValue = GetSysColor(COLOR_HIGHLIGHT);
	}
	SendMessage(hwnd, SCI_INDICSETFORE, IndicatorNumber_MarkOccurrences, iValue);
	if (!Style_StrGetAlpha(szValue, &iValue)) {
		iValue = MarkOccurrencesDefaultAlpha;
	}
	SendMessage(hwnd, SCI_INDICSETALPHA, IndicatorNumber_MarkOccurrences, iValue);
	SendMessage(hwnd, SCI_INDICSETSTYLE, IndicatorNumber_MarkOccurrences, INDIC_ROUNDBOX);

	// Bookmark
	bBookmarkColorUpdated = TRUE;
	// SC_MARK_CIRCLE is the default marker type.
	if (SendMessage(hwnd, SCI_MARKERSYMBOLDEFINED, MarkerNumber_Bookmark, 0) != SC_MARK_CIRCLE) {
		Style_SetBookmark(hwnd);
	}

	{
		struct DetailStyle style;
		const UINT iStyleCount = pLexNew->iStyleCount;
		// first style is the default style.
		for (UINT i = 1; i < iStyleCount; i++) {
			int iStyle = pLexNew->Styles[i].iStyle;
			szValue = pLexNew->Styles[i].szValue;
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
	bCurrentLexerHasLineComment = DidLexerHasLineComment(iLexer);
	bCurrentLexerHasBlockComment = DidLexerHasBlockComment(iLexer, rid);
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
	SendMessage(hwndEdit, SCI_GETTEXT, COUNTOF(tchText), (LPARAM)tchText);

	// check DOCTYPE
	if ((p = strstr(tchText, "!DOCTYPE")) != NULL) {
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

	if (strstr(tchText, "<?php")) {
		return IDM_LANG_PHP;
	}
	// check Language
	if ((p = strstr(tchText, "<%@")) != NULL && (p = StrStrIA(p + 3, "Language")) != NULL) {
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
			if ((p = strstr(p, "-->")) != NULL) {
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
	//	if (strstr(p, "maven") && strstr(p, "POM"))
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
	char * const pb = p;
	if (((p = strstr(pb, "Layout")) != NULL && strstr(p + 6, "xmlns:android")) ||
			((p = strstr(pb, "View")) != NULL && strstr(p + 4, "xmlns:android")) ||
			((p = strstr(pb, "menu")) != NULL && strstr(p + 4, "xmlns:android"))) {
		return IDM_LANG_ANDROID_LAYOUT;
	}

	return 0;
}

static inline BOOL IsASpace(int ch) {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

BOOL MatchCPPKeyword(const char *p, int index) {
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
	p = strstr(lexCPP.pKeyWords->pszKeyWords[index], word);
	return p != NULL;
}

PEDITLEXER Style_DetectObjCAndMatlab(void) {
	char tchText[4096] = ""; // maybe contains header comments
	SendMessage(hwndEdit, SCI_GETTEXT, COUNTOF(tchText), (LPARAM)tchText);

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

PEDITLEXER Style_AutoDetect(PEDITLEXER pLexNew, BOOL bDotFile) {
	char tchText[4096] = ""; // maybe contains header comments
	SendMessage(hwndEdit, SCI_GETTEXT, COUNTOF(tchText), (LPARAM)tchText);

	char *p = tchText;
	const BOOL shebang = *p == '#' && p[1] == '!';
	int cppCount = 0;
	int sharpCount = 0;
	BOOL maybeIni = FALSE;

	while (*p) {
		if (*p == '[') {
			maybeIni = TRUE;
		} else {
			while (IsASpace(*p)) {
				++p;
			}
		}
		switch (*p) {
		case '#': // C/C++ preprocessor, comment
			if (!(p == tchText && shebang)) {
				++p;
				while (*p == ' ' || *p == '\t') {
					++p;
				}
				if (MatchCPPKeyword(p, 2)) {
					++cppCount;
				} else {
					++sharpCount;
				}
			}
			break;
		case '/': // C/C++ style comment
			++p;
			if (*p == '/' || *p == '*') {
				return &lexCPP;
			}
			break;
		}
		// skip to next line
		while (*p && !(*p == '\r' || *p == '\n')) {
			++p;
		}
	}

	if (cppCount > sharpCount && !(shebang || bDotFile)) {
		return &lexCPP;
	}
	if (sharpCount) {
		return shebang ? &lexBash : &lexCONF;
	}
	if (maybeIni) {
		return &lexINI;
	}
	if (bDotFile) {
		return &lexCONF;
	}
	return pLexNew;
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
	case IDM_LANG_TEXTFILE:
	case IDM_LANG_2NDTEXTFILE:
		return pLexCurrent->pszName;
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
		for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			LPCWSTR p1 = pLex->szExtensions;
			do {
				LPCWSTR p2 = StrStrI(p1, lpszMatch);
				if (p2 == NULL) {
					break;
				}

				const WCHAR ch = (p2 == p1)? L'\0' : p2[-1];
				p2 += cch;
				if ((ch == L';' || ch == ' ' || ch == L'\0') && (*p2 == L';' || *p2 == L' ' || *p2 == L'\0')) {
					return pLex;
				}
				p1 = StrChr(p2, L';');
			} while (p1 != NULL);
		}
	} else {
		const int cch = lstrlen(lpszMatch);
		if (cch >= 3) {
			for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
				PEDITLEXER pLex = pLexArray[iLexer];
				if (StrNCaseEqual(pLex->pszName, lpszMatch, cch)) {
					return pLex;
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
extern BOOL fNoHTMLGuess;
extern BOOL fNoCGIGuess;
extern BOOL fNoAutoDetection;
extern FILEVARS fvCurFile;

static PEDITLEXER Style_GetLexerFromFile(HWND hwnd, LPCWSTR lpszFile, BOOL bCGIGuess, LPWSTR *pszExt, BOOL *pDotFile) {
	LPWSTR lpszExt = PathFindExtension(lpszFile);
	const LPCWSTR lpszName = PathFindFileName(lpszFile);
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
				pLexNew = &lexTextFile;
			}
		}

		if (!bFound && bCGIGuess && (StrCaseEqual(lpszExt, L"cgi") || StrCaseEqual(lpszExt, L"fcgi"))) {
			char tchText[256] = "";
			SendMessage(hwnd, SCI_GETTEXT, COUNTOF(tchText), (LPARAM)tchText);
			StrTrimA(tchText, " \t\n\r");
			pLexNew = Style_SniffShebang(tchText);
			bFound = pLexNew != NULL;
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

		// autoconf / automake
		if (!bFound && StrCaseEqual(lpszExt, L"in") && pDotFile != NULL) {
			WCHAR tchCopy[MAX_PATH];
			lstrcpyn(tchCopy, lpszFile, COUNTOF(tchCopy));
			PathRemoveExtension(tchCopy);
			pLexNew = Style_GetLexerFromFile(hwnd, tchCopy, FALSE, NULL, NULL);
			bFound = pLexNew != NULL;
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

		// check associated extensions
		if (!bFound) {
			pLexNew = Style_MatchLexer(lpszExt, FALSE);
			bFound = pLexNew != NULL;
		}

		// dot file
		if (StrCaseEqual(lpszExt - 1, lpszName)) {
			if (pDotFile) {
				*pDotFile = TRUE;
			}
			if (StrNEqual(lpszExt, L"bash", 4) || StrEqual(lpszExt, L"profile")) { // .bash_history, .bash_logout, .bash_profile, .bashrc, .profile
				pLexNew = &lexBash;
				bFound = TRUE;
			}
		}
	}

	if (!bFound) {
		if (StrNCaseEqual(lpszName, L"Readme", 6)) {
			pLexNew = &lexTextFile;
			bFound = TRUE;
		}
		if (!bFound && (StrNCaseEqual(lpszName, L"Makefile", 8) || StrNCaseEqual(lpszName, L"Kbuild", 6))) {
			pLexNew = &lexMake;
			bFound = TRUE;
		}
		if (!bFound && (StrCaseEqual(lpszName, L"Rakefile") || StrCaseEqual(lpszName, L"Podfile"))) {
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
		if (!bFound && (StrNCaseEqual(lpszName, L"Kconfig", 7) || StrNCaseEqual(lpszName, L"Doxyfile", 8))) {
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
		char tchText[256] = "";
		SendMessage(hwnd, SCI_GETTEXT, COUNTOF(tchText), (LPARAM)tchText);
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
			char tchText[256] = "";
			SendMessage(hwnd, SCI_GETTEXT, COUNTOF(tchText), (LPARAM)tchText);
			StrTrimA(tchText, " \t\n\r");
			if ((pLexSniffed = Style_SniffShebang(tchText)) != NULL) {
				if (iEncoding != g_DOSEncoding || pLexSniffed != &lexTextFile
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

	if (!bFound && (!fNoAutoDetection || bDotFile)) {
		if (!fNoAutoDetection) {
			pLexNew = Style_AutoDetect(pLexNew, bDotFile);
		} else {
			pLexNew = &lexCONF;
		}
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
	const LPCEDITLEXER pLexNew = Style_GetLexerFromFile(NULL, lpszFile, FALSE, &lpszExt, &bDotFile);
	np2LexLangIndex = lang;
	return pLexNew != NULL || StrIsEmpty(lpszExt) || bDotFile || StrCaseEqual(lpszExt, L"cgi") || StrCaseEqual(lpszExt, L"fcgi");
}

void Style_SetLexerByLangIndex(HWND hwnd, int lang) {
	np2LexLangIndex = lang;

	switch (lang) {
	case IDM_LANG_TEXTFILE:
		np2LexLangIndex = 0;
		Style_SetLexer(hwnd, &lexTextFile);
		break;

	case IDM_LANG_2NDTEXTFILE:
		np2LexLangIndex = 0;
		Style_SetLexer(hwnd, &lex2ndTextFile);
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
		case NP2LEX_TEXTFILE:
			lang = IDM_LANG_TEXTFILE;
			break;
		case NP2LEX_2NDTEXTFILE:
			lang = IDM_LANG_2NDTEXTFILE;
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
	if (lang == IDM_LANG_TEXTFILE || lang == NP2LEX_2NDTEXTFILE) {
		np2LexLangIndex = 0;
	}
	for (int i = IDM_LANG_TEXTFILE; i < IDM_LANG_NULL; i++) {
		CheckCmd(hmenu, i, FALSE);
	}
	if (lang >= IDM_LANG_TEXTFILE) {
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
		lex = &lexTextFile;
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

	for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		if (pLexArray[iLexer] == lex) {
			return iLexer;
		}
	}
	return -1;
}

//=============================================================================
//
// Style_ToggleUse2ndDefault()
//
void Style_ToggleUse2ndGlobalStyle(HWND hwnd) {
	bUse2ndGlobalStyle = !bUse2ndGlobalStyle;
	pLexGlobal = bUse2ndGlobalStyle ? &lex2ndGlobal : &lexGlobal;
	Style_SetLexer(hwnd, pLexCurrent);
}

void Style_ToggleUseDefaultCodeStyle(HWND hwnd) {
	pLexCurrent->bStyleChanged = TRUE;
	pLexCurrent->bUseDefaultCodeStyle = !pLexCurrent->bUseDefaultCodeStyle;
	fStylesModified |= STYLESMODIFIED_SOME_STYLE;
	Style_SetLexer(hwnd, pLexCurrent);
}

//=============================================================================
//
// Style_SetLongLineColors()
//
void Style_SetLongLineColors(HWND hwnd) {
	LPCWSTR szValue = pLexGlobal->Styles[Style_LongLineMarker].szValue;

	const BOOL foreColor = SendMessage(hwnd, SCI_GETEDGEMODE, 0, 0) == EDGE_LINE;
	int rgb;
	if (!Style_StrGetColor(foreColor, szValue, &rgb)) {
		rgb = GetSysColor(COLOR_3DLIGHT);
	}

	SendMessage(hwnd, SCI_SETEDGECOLOUR, rgb, 0);
}

//=============================================================================
//
// Style_HighlightCurrentLine()
//
void Style_HighlightCurrentLine(HWND hwnd) {
	SendMessage(hwnd, SCI_SETCARETLINEVISIBLE, FALSE, 0);
	if (iHighlightCurrentLine != 0) {
		LPCWSTR szValue = pLexGlobal->Styles[Style_CurrentLine].szValue;
		const BOOL backColor = iHighlightCurrentLine == 1;
		int iValue;
		if (Style_StrGetColor(!backColor, szValue, &iValue)) {
			int size = 0;
			if (!backColor) {
				Style_StrGetRawSize(szValue, &size);
				size = max_i(1, RoundToCurrentDPI(size));
			}

			SendMessage(hwnd, SCI_SETCARETLINEFRAME, size, 0);
			SendMessage(hwnd, SCI_SETCARETLINEBACK, iValue, 0);

			if (!Style_StrGetAlpha(szValue, &iValue)) {
				iValue = SC_ALPHA_NOALPHA;
			}
			SendMessage(hwnd, SCI_SETCARETLINEBACKALPHA, iValue, 0);
			SendMessage(hwnd, SCI_SETCARETLINEVISIBLE, TRUE, 0);
		}
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
			if (pLexCurrent->iLexer == SCLEX_PYTHON) {
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

void Style_SetBookmark(HWND hwnd) {
	if (!bBookmarkColorUpdated) {
#if BookmarkUsingPixmapImage
		const int marker = bShowSelectionMargin ? SC_MARK_PIXMAP : SC_MARK_BACKGROUND;
#else
		const int marker = bShowSelectionMargin ? SC_MARK_VERTICALBOOKMARK : SC_MARK_BACKGROUND;
#endif
		const int markType = (int)SendMessage(hwnd, SCI_MARKERSYMBOLDEFINED, MarkerNumber_Bookmark, 0);
		if (marker == markType) {
			return;
		}
	}

	LPCWSTR szValue = pLexGlobal->Styles[Style_Bookmark].szValue;
	if (bShowSelectionMargin) {
		int iBookmarkImageColor;
		if (!Style_StrGetColor(TRUE, szValue, &iBookmarkImageColor)) {
			iBookmarkImageColor = BookmarkImageDefaultColor;
		}
#if BookmarkUsingPixmapImage
		sprintf(bookmark_pixmap_color, bookmark_pixmap_color_fmt, iBookmarkImageColor);
		SendMessage(hwnd, SCI_MARKERDEFINEPIXMAP, MarkerNumber_Bookmark, (LPARAM)bookmark_pixmap);
#else
		SendMessage(hwnd, SCI_MARKERSETBACK, MarkerNumber_Bookmark, iBookmarkImageColor);
		// set same color to avoid showing edge.
		SendMessage(hwnd, SCI_MARKERSETFORE, MarkerNumber_Bookmark, iBookmarkImageColor);
		SendMessage(hwnd, SCI_MARKERDEFINE, MarkerNumber_Bookmark, SC_MARK_VERTICALBOOKMARK);
#endif
	} else {
		int iBookmarkLineColor;
		int iBookmarkLineAlpha;
		if (!Style_StrGetColor(FALSE, szValue, &iBookmarkLineColor)) {
			iBookmarkLineColor = BookmarkLineDefaultColor;
		}
		if (!Style_StrGetAlpha(szValue, &iBookmarkLineAlpha)) {
			iBookmarkLineAlpha = BookmarkLineDefaultAlpha;
		}
		SendMessage(hwnd, SCI_MARKERSETBACK, MarkerNumber_Bookmark, iBookmarkLineColor);
		SendMessage(hwnd, SCI_MARKERSETALPHA, MarkerNumber_Bookmark, iBookmarkLineAlpha);
		SendMessage(hwnd, SCI_MARKERDEFINE, MarkerNumber_Bookmark, SC_MARK_BACKGROUND);
	}
	bBookmarkColorUpdated = FALSE;
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
BOOL Style_StrGetFontEx(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont, BOOL bDefaultStyle) {
	LPWSTR p;

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

		if (bDefaultStyle) {
			if (StrEqual(lpszFont, L"$(Text)")) {
				lstrcpyn(lpszFont, systemTextFontName, cchFont);
			} else if (StrCaseEqual(lpszFont, L"$(Code)") || !IsFontAvailable(lpszFont)) {
				lstrcpyn(lpszFont, systemCodeFontName, cchFont);
			}
		} else {
			if (StrEqual(lpszFont, L"$(Text)")) {
				lstrcpyn(lpszFont, defaultTextFontName, cchFont);
			} else if (StrCaseEqual(lpszFont, L"$(Code)") || !IsFontAvailable(lpszFont)) {
				lstrcpyn(lpszFont, defaultCodeFontName, cchFont);
			}
		}
		return TRUE;
	}
	return FALSE;
}

static inline BOOL Style_StrGetFont(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont) {
	return Style_StrGetFontEx(lpszStyle, lpszFont, cchFont, FALSE);
}

//=============================================================================
//
// Style_StrGetCharSet()
//
BOOL Style_StrGetCharSet(LPCWSTR lpszStyle, int *i) {
	LPCWSTR p;

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
BOOL Style_StrGetFontSize(LPCWSTR lpszStyle, int *i) {
	LPCWSTR p;

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

BOOL Style_StrGetRawSize(LPCWSTR lpszStyle, int *i) {
	LPCWSTR p;

	if ((p = StrStr(lpszStyle, L"size:")) != NULL) {
		p += CSTRLEN(L"size:");
		return CRTStrToInt(p, i);
	}
	return FALSE;
}

// https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/ns-wingdi-taglogfontw
// https://docs.microsoft.com/en-us/windows/desktop/api/dwrite/ne-dwrite-dwrite_font_weight
#define MIN_FONT_WEIGHT		0
#define MAX_FONT_WEIGHT		1000
BOOL Style_StrGetFontWeight(LPCWSTR lpszStyle, int *i) {
	LPCWSTR p;

	if (Style_StrGetBold(lpszStyle)) {
		*i = FW_BOLD;
		return TRUE;
	}
	if ((p = StrStr(lpszStyle, L"weight:")) != NULL) {
		p += CSTRLEN(L"weight:");
		return CRTStrToInt(p, i) && (*i > MIN_FONT_WEIGHT && *i < MAX_FONT_WEIGHT);
	}

	return FALSE;
}

//=============================================================================
//
// Style_StrGetValueStr()
//
static BOOL Style_StrGetValueStr(LPCWSTR lpszStyle, LPCWSTR key, int keyLen, LPWSTR lpszValue, int cchValue) {
	LPWSTR p;

	if ((p = StrStr(lpszStyle, key)) != NULL) {
		p += keyLen;
		while (*p == L' ') {
			++p;
		}
		lstrcpyn(lpszValue, p, cchValue);
		if ((p = StrChr(lpszValue, L';')) != NULL) {
			*p = L'\0';
		}
		TrimString(lpszValue);
		return TRUE;
	}
	return FALSE;
}

static void Style_StrCopyValueStrEx(LPWSTR szNewStyle, LPCWSTR lpszStyle, LPCWSTR key, int keyLen, LPWSTR lpszValue, int cchValue) {
	if (Style_StrGetValueStr(lpszStyle, key, keyLen, lpszValue, cchValue)) {
		if (*szNewStyle) {
			lstrcat(szNewStyle, L"; ");
		}
		lstrcat(szNewStyle, key);
		lstrcat(szNewStyle, lpszValue);
	}
}

static void Style_StrCopyAttributeEx(LPWSTR szNewStyle, LPCWSTR lpszStyle, LPCWSTR key, int keyLen) {
	if (Style_StrGetAttributeEx(lpszStyle, key, keyLen)) {
		if (*szNewStyle) {
			lstrcat(szNewStyle, L"; ");
		}
		lstrcat(szNewStyle, key);
	}
}

BOOL Style_StrGetLocale(LPCWSTR lpszStyle, LPWSTR lpszLocale, int cchLocale) {
	return Style_StrGetValueStr(lpszStyle, L"locale:", CSTRLEN(L"locale:"), lpszLocale, cchLocale);
}

#define Style_StrCopyValueStr(szNewStyle, lpszStyle, name, tch)	Style_StrCopyValueStrEx((szNewStyle), (lpszStyle), (name), CSTRLEN(name), (tch), COUNTOF(tch))
#define Style_StrCopyFont(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"font:", (tch));
#define Style_StrCopyChatset(szNewStyle, lpszStyle, tch)	Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"charset:", (tch));
#define Style_StrCopyLocale(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"locale:", (tch));
#define Style_StrCopySize(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"size:", (tch));
#define Style_StrCopyWeight(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"weight:", (tch));
#define Style_StrCopyCase(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"case:", (tch));
#define Style_StrCopyFore(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"fore:", (tch));
#define Style_StrCopyBack(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"back:", (tch));
#define Style_StrCopyAlpha(szNewStyle, lpszStyle, tch)		Style_StrCopyValueStr((szNewStyle), (lpszStyle), L"alpha:", (tch));

#define Style_StrCopyAttribute(szNewStyle, lpszStyle, name)	Style_StrCopyAttributeEx((szNewStyle), (lpszStyle), (name), CSTRLEN(name))
#define Style_StrCopyBold(szNewStyle, lpszStyle)			Style_StrCopyAttribute((szNewStyle), (lpszStyle), L"bold")
#define Style_StrCopyItalic(szNewStyle, lpszStyle)			Style_StrCopyAttribute((szNewStyle), (lpszStyle), L"italic")
#define Style_StrCopyUnderline(szNewStyle, lpszStyle)		Style_StrCopyAttribute((szNewStyle), (lpszStyle), L"underline")
#define Style_StrCopyStrike(szNewStyle, lpszStyle)			Style_StrCopyAttribute((szNewStyle), (lpszStyle), L"strike")
#define Style_StrCopyEOLFilled(szNewStyle, lpszStyle)		Style_StrCopyAttribute((szNewStyle), (lpszStyle), L"eolfilled")

//=============================================================================
//
// Style_StrGetColor()
//
BOOL Style_StrGetColor(BOOL bFore, LPCWSTR lpszStyle, int *rgb) {
	LPCWSTR p;

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
	LPCWSTR p;

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
		case L'c':
		case L'C':
			*i = SC_CASE_CAMEL;
			return TRUE;
		//case L'm':
		//case L'M':
		//	*i = SC_CASE_MIXED; // normal case
		//	return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetAlpha()
//
BOOL Style_StrGetAlpha(LPCWSTR lpszStyle, int *i) {
	LPCWSTR p;

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
	WCHAR tch[MAX_STYLE_VALUE_LENGTH];

	ZeroMemory(&cf, sizeof(CHOOSEFONT));
	ZeroMemory(&lf, sizeof(LOGFONT));

	// Map lpszStyle to LOGFONT
	if (Style_StrGetFontEx(lpszStyle, tch, COUNTOF(tch), bDefaultStyle)) {
		lstrcpyn(lf.lfFaceName, tch, COUNTOF(lf.lfFaceName));
	}
	if (Style_StrGetCharSet(lpszStyle, &iValue)) {
		lf.lfCharSet = (BYTE)iValue;
	}
	if (Style_StrGetColor(TRUE, lpszStyle, &iValue)) {
		cf.rgbColors = iValue;
	}
	if (!Style_StrGetFontSize(lpszStyle, &iValue)) {
		iValue = iBaseFontSize;
	}
	{
		HDC hdc = GetDC(hwnd);
		cf.iPointSize = iValue / (SC_FONT_SIZE_MULTIPLIER / 10);
		lf.lfHeight = -MulDiv(iValue, GetDeviceCaps(hdc, LOGPIXELSY), 72*SC_FONT_SIZE_MULTIPLIER);
		ReleaseDC(hwnd, hdc);
	}
	if (!Style_StrGetFontWeight(lpszStyle, &iValue)) {
		iValue = FW_NORMAL;
	}
	lf.lfWeight = iValue;
	lf.lfItalic = Style_StrGetItalic(lpszStyle) ? TRUE : FALSE;
	lf.lfUnderline = Style_StrGetUnderline(lpszStyle) ? TRUE : FALSE;
	lf.lfStrikeOut = Style_StrGetStrike(lpszStyle) ? TRUE : FALSE;

	// Init cf
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;
	cf.Flags = CF_INITTOLOGFONTSTRUCT /*| CF_NOSCRIPTSEL*/ | CF_SCREENFONTS | CF_EFFECTS;

	if (KeyboardIsKeyDown(VK_SHIFT)) {
		cf.Flags |= CF_FIXEDPITCHONLY;
	}

	if (!ChooseFont(&cf) || StrIsEmpty(lf.lfFaceName)) {
		return FALSE;
	}

	// Map back to lpszStyle
	WCHAR szNewStyle[MAX_LEXER_STYLE_EDIT_SIZE];
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
	Style_StrCopyLocale(szNewStyle, lpszStyle, tch);
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
	if (lf.lfWeight != FW_NORMAL) {
		if (lf.lfWeight == FW_BOLD) {
			lstrcat(szNewStyle, L"; bold");
		} else {
			lstrcat(szNewStyle, L"; weight:");
			wsprintf(tch, L"%i", (int)lf.lfWeight);
			lstrcat(szNewStyle, tch);
		}
	}
	if (cf.nFontType & ITALIC_FONTTYPE) {
		lstrcat(szNewStyle, L"; italic");
	}

	if (lf.lfUnderline) {
		lstrcat(szNewStyle, L"; underline");
	}

	if (lf.lfStrikeOut) {
		lstrcat(szNewStyle, L"; strike");
	}

	Style_StrCopyCase(szNewStyle, lpszStyle, tch);
	Style_StrCopyFore(szNewStyle, lpszStyle, tch);
	Style_StrCopyBack(szNewStyle, lpszStyle, tch);
	Style_StrCopyAlpha(szNewStyle, lpszStyle, tch);
	Style_StrCopyEOLFilled(szNewStyle, lpszStyle);

	lstrcpyn(lpszStyle, szNewStyle, cchStyle);
	return TRUE;
}

//=============================================================================
//
// Style_SetDefaultFont()
//
void Style_SetDefaultFont(HWND hwnd, BOOL bCode) {
	const int iIdx = bCode ? Style_DefaultCode : Style_DefaultText;
	if (Style_SelectFont(hwnd, pLexGlobal->Styles[iIdx].szValue, MAX_EDITSTYLE_VALUE_SIZE, TRUE)) {
		fStylesModified |= STYLESMODIFIED_SOME_STYLE;
		pLexGlobal->bStyleChanged = TRUE;
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

	int iRGBResult;
	if (!Style_StrGetColor(bFore, lpszStyle, &iRGBResult)) {
		iRGBResult = (int)(bFore ? GetSysColor(COLOR_WINDOWTEXT) : GetSysColor(COLOR_WINDOW));
	}

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
	WCHAR szNewStyle[MAX_LEXER_STYLE_EDIT_SIZE];
	WCHAR tch[MAX_STYLE_VALUE_LENGTH];

	lstrcpy(szNewStyle, L"");
	Style_StrCopyFont(szNewStyle, lpszStyle, tch);
	Style_StrCopyChatset(szNewStyle, lpszStyle, tch);
	Style_StrCopyLocale(szNewStyle, lpszStyle, tch);
	Style_StrCopySize(szNewStyle, lpszStyle, tch);
	Style_StrCopyWeight(szNewStyle, lpszStyle, tch);

	Style_StrCopyBold(szNewStyle, lpszStyle);
	Style_StrCopyItalic(szNewStyle, lpszStyle);
	Style_StrCopyUnderline(szNewStyle, lpszStyle);
	Style_StrCopyStrike(szNewStyle, lpszStyle);
	Style_StrCopyCase(szNewStyle, lpszStyle, tch);

	if (bFore) {
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		wsprintf(tch, L"fore:#%02X%02X%02X",
				 (int)GetRValue(iRGBResult),
				 (int)GetGValue(iRGBResult),
				 (int)GetBValue(iRGBResult));
		lstrcat(szNewStyle, tch);
		Style_StrCopyBack(szNewStyle, lpszStyle, tch);
	} else {
		Style_StrCopyFore(szNewStyle, lpszStyle, tch);
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		wsprintf(tch, L"back:#%02X%02X%02X",
				 (int)GetRValue(iRGBResult),
				 (int)GetGValue(iRGBResult),
				 (int)GetBValue(iRGBResult));
		lstrcat(szNewStyle, tch);
	}

	Style_StrCopyAlpha(szNewStyle, lpszStyle, tch);
	Style_StrCopyEOLFilled(szNewStyle, lpszStyle);

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
	if (Style_StrGetFontSize(lpszStyle, &iValue)) {
		iValue = DefaultToCurrentDPI(iValue);
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

	// Weight
	if (Style_StrGetFontWeight(lpszStyle, &iValue)) {
		SendMessage(hwnd, SCI_STYLESETWEIGHT, iStyle, iValue);
	}

	// Italic
	if (Style_StrGetItalic(lpszStyle)) {
		SendMessage(hwnd, SCI_STYLESETITALIC, iStyle, TRUE);
	}
	// Underline
	if (Style_StrGetUnderline(lpszStyle)) {
		SendMessage(hwnd, SCI_STYLESETUNDERLINE, iStyle, TRUE);
	}
	// Strike
	if (Style_StrGetStrike(lpszStyle)) {
		SendMessage(hwnd, SCI_STYLESETSTRIKE, iStyle, TRUE);
	}
	// EOL Filled
	if (Style_StrGetEOLFilled(lpszStyle)) {
		SendMessage(hwnd, SCI_STYLESETEOLFILLED, iStyle, TRUE);
	}

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
	if (Style_StrGetFontSize(lpszStyle, &iValue)) {
		style->fontSize = DefaultToCurrentDPI(iValue);
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

	// Weight
	if (Style_StrGetFontWeight(lpszStyle, &iValue)) {
		style->weight = iValue;
		mask |= STYLE_MASK_FONT_WEIGHT;
	}

	// Italic
	style->italic = Style_StrGetItalic(lpszStyle);
	// Underline
	style->underline = Style_StrGetUnderline(lpszStyle);
	// Strike
	style->strike = Style_StrGetStrike(lpszStyle);
	// EOL Filled
	style->eolFilled = Style_StrGetEOLFilled(lpszStyle);

	// Case
	if (Style_StrGetCase(lpszStyle, &iValue)) {
		style->forceCase = iValue;
		mask |= STYLE_MASK_FORCE_CASE;
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

	// Weight
	if (mask & STYLE_MASK_FONT_WEIGHT) {
		SendMessage(hwnd, SCI_STYLESETWEIGHT, iStyle, style->weight);
	}

	// Italic
	if (style->italic) {
		SendMessage(hwnd, SCI_STYLESETITALIC, iStyle, TRUE);
	}
	// Underline
	if (style->underline) {
		SendMessage(hwnd, SCI_STYLESETUNDERLINE, iStyle, TRUE);
	}
	// Strike
	if (style->strike) {
		SendMessage(hwnd, SCI_STYLESETSTRIKE, iStyle, TRUE);
	}
	// EOL Filled
	if (style->eolFilled) {
		SendMessage(hwnd, SCI_STYLESETEOLFILLED, iStyle, TRUE);
	}

	// Case
	if (mask & STYLE_MASK_FORCE_CASE) {
		SendMessage(hwnd, SCI_STYLESETCASE, iStyle, style->forceCase);
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
int Style_GetLexerIconId(LPCEDITLEXER pLex) {
	LPCWSTR pszExtensions;
	if (StrNotEmpty(pLex->szExtensions)) {
		pszExtensions = pLex->szExtensions;
	} else {
		pszExtensions = pLex->pszDefExt;
	}

	WCHAR *pszFile = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * (lstrlen(pszExtensions) + CSTRLEN(L"*.txt") + 16));
	lstrcpy(pszFile, L"*.");
	if (StrNotEmpty(pszExtensions)) {
		lstrcat(pszFile, pszExtensions);
	}
	WCHAR *p = StrChr(pszFile, L';');
	if (p != NULL) {
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

	const UINT iStyleCount = pLex->iStyleCount;
	for (UINT i = 0; i < iStyleCount; i++) {
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

struct StyleConfigDlgParam {
	HFONT hFontTitle;
	LPWSTR extBackup;
	BOOL bApply;
	COLORREF colorBackup[MAX_CUSTOM_COLOR_COUNT];
	LPWSTR styleBackup[ALL_LEXER_COUNT];
};

static void Style_ResetStyle(PEDITLEXER pLex, PEDITSTYLE pStyle) {
	if (np2StyleTheme != StyleTheme_Default) {
		// reload style from external file
		LPCWSTR themePath = GetStyleThemeFilePath();
		WCHAR wch[MAX_EDITSTYLE_VALUE_SIZE] = L"";
		// use "NULL" to distinguish between empty style value like: Keyword=
		GetPrivateProfileString(pLex->pszName, pStyle->pszName, L"NULL", wch, COUNTOF(wch), themePath);
		if (!StrEqual(wch, L"NULL")) {
			lstrcpy(pStyle->szValue, wch);
			return;
		}
	}

	// reset style to built-in default
	lstrcpy(pStyle->szValue, pStyle->pszDefault);
}

//=============================================================================
//
// Style_ConfigDlgProc()
//
static INT_PTR CALLBACK Style_ConfigDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static HWND hwndTV;
	static BOOL fDragging;
	static BOOL fLexerSelected;
	static int iCurrentStyleIndex;
	static PEDITLEXER pCurrentLexer;
	static PEDITSTYLE pCurrentStyle;
	//static HBRUSH hbrFore;
	//static HBRUSH hbrBack;

	switch (umsg) {
	case WM_INITDIALOG: {
		ResizeDlg_InitY2(hwnd, cxStyleCustomizeDlg, cyStyleCustomizeDlg, IDC_RESIZEGRIP3, IDC_STYLEEDIT, IDC_STYLEVALUE_DEFAULT);

		WCHAR szTitle[512];
		GetWindowText(hwnd, szTitle, COUNTOF(szTitle));
		lstrcat(szTitle, L" - ");
		lstrcat(szTitle, GetStyleThemeTitle());
		SetWindowText(hwnd, szTitle);

		hwndTV = GetDlgItem(hwnd, IDC_STYLELIST);
		fDragging = FALSE;
		fLexerSelected = FALSE;
		iCurrentStyleIndex = -1;

		SHFILEINFO shfi;
		TreeView_SetImageList(hwndTV,
							  (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO),
									  SHGFI_SMALLICON | SHGFI_SYSICONINDEX), TVSIL_NORMAL);

		// Add lexers
		BOOL found = FALSE;
		HTREEITEM currentLex = NULL;
		Style_AddLexerToTreeView(hwndTV, &lexGlobal);
		Style_AddLexerToTreeView(hwndTV, &lex2ndGlobal);
		for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			if (!found && pLex->rid == pLexCurrent->rid) {
				found = TRUE;
				currentLex = Style_AddLexerToTreeView(hwndTV, pLex);
			} else {
				Style_AddLexerToTreeView(hwndTV, pLex);
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
			hFontTitle = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		}
		LOGFONT lf;
		GetObject(hFontTitle, sizeof(LOGFONT), &lf);
		lf.lfHeight += lf.lfHeight / 5;
		lf.lfWeight = FW_BOLD;
		hFontTitle = CreateFontIndirect(&lf);
		SendDlgItemMessage(hwnd, IDC_TITLE, WM_SETFONT, (WPARAM)hFontTitle, TRUE);

		struct StyleConfigDlgParam *param = (struct StyleConfigDlgParam *)lParam;
		param->hFontTitle = hFontTitle;
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY: {
		DeleteBitmapButton(hwnd, IDC_STYLEFORE);
		DeleteBitmapButton(hwnd, IDC_STYLEBACK);
		DeleteBitmapButton(hwnd, IDC_PREVSTYLE);
		DeleteBitmapButton(hwnd, IDC_NEXTSTYLE);
		ResizeDlg_Destroy(hwnd, &cxStyleCustomizeDlg, &cyStyleCustomizeDlg);
	}
	return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

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
				} else if (fLexerSelected && pCurrentLexer && pCurrentLexer->szExtensions) {
					if (!GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions, MAX_EDITLEXER_EXT_SIZE)) {
						lstrcpy(pCurrentLexer->szExtensions, pCurrentLexer->pszDefExt);
					}
				}

				HTREEITEM hParent = TreeView_GetParent(hwndTV, lpnmtv->itemNew.hItem);
				fLexerSelected = hParent == NULL;
				iCurrentStyleIndex = -1;
				UINT enableMask = StyleControl_None;
				// a lexer has been selected
				if (hParent == NULL) {
					WCHAR wch[MAX_EDITLEXER_EXT_SIZE];

					GetDlgItemText(hwnd, IDC_STYLELABELS, wch, COUNTOF(wch));
					if (StrChr(wch, L'|')) {
						*StrChr(wch, L'|') = 0;
					}

					pCurrentLexer = (PEDITLEXER)lpnmtv->itemNew.lParam;
					pCurrentStyle = NULL;
					if (pCurrentLexer != NULL) {
						SetDlgItemText(hwnd, IDC_STYLELABEL, wch);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), (pCurrentLexer->szExtensions != NULL));
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), TRUE);
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions);
						SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, pCurrentLexer->pszDefExt);
					}
				}

				// a style has been selected
				else {
					WCHAR wch[MAX_EDITSTYLE_VALUE_SIZE];

					GetDlgItemText(hwnd, IDC_STYLELABELS, wch, COUNTOF(wch));
					if (StrChr(wch, L'|')) {
						*StrChr(wch, L'|') = 0;
					}

					// TVS_SINGLEEXPAND: CTRL key.
					TVITEM item;
					ZeroMemory(&item, sizeof(item));
					item.mask = TVIF_PARAM;
					item.hItem = hParent;
					TreeView_GetItem(hwndTV, &item);
					pCurrentLexer = (PEDITLEXER)item.lParam;

					pCurrentStyle = (PEDITSTYLE)lpnmtv->itemNew.lParam;
					if (pCurrentStyle != NULL) {
						SetDlgItemText(hwnd, IDC_STYLELABEL, StrEnd(wch) + 1);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEBACK), TRUE);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), TRUE);
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue);
						SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, pCurrentStyle->pszDefault);

						for (UINT i = 0; i < pCurrentLexer->iStyleCount; i++) {
							if (pCurrentStyle == &pCurrentLexer->Styles[i]) {
								iCurrentStyleIndex = i;
								break;
							}
						}
						enableMask = (pCurrentLexer->rid == NP2LEX_GLOBAL || pCurrentLexer->rid == NP2LEX_2NDGLOBAL) ? GetDefaultStyleControlMask(iCurrentStyleIndex) : StyleControl_All;
					}
				}

				if ((fLexerSelected && pCurrentLexer == NULL) || (!fLexerSelected && pCurrentStyle == NULL)) {
					SetDlgItemText(hwnd, IDC_STYLELABEL, L"");
					EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), FALSE);
					SetDlgItemText(hwnd, IDC_STYLEEDIT, L"");
					SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, L"");
				}

				const BOOL changed = pCurrentStyle != NULL && (
					((enableMask & StyleControl_Fore) && !IsWindowEnabled(GetDlgItem(hwnd, IDC_STYLEFORE)))
					|| ((enableMask & StyleControl_Back) && !IsWindowEnabled(GetDlgItem(hwnd, IDC_STYLEBACK)))
				);

				EnableWindow(GetDlgItem(hwnd, IDC_STYLEFONT), (enableMask & StyleControl_Font) != 0);
				//if (enableMask & StyleControl_Font) {
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEFONT), TRUE);
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEBOLD), TRUE);
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEITALIC), TRUE);
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEUNDERLINE), TRUE);
				//	CheckDlgButton(hwnd, IDC_STYLEBOLD, Style_StrGetBold(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED));
				//	CheckDlgButton(hwnd, IDC_STYLEITALIC, Style_StrGetItalic(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED);
				//	CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, Style_StrGetUnderline(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED);
				//} else {
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEFONT), FALSE);
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEBOLD), FALSE);
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEITALIC), FALSE);
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEUNDERLINE), FALSE);
				//	CheckDlgButton(hwnd, IDC_STYLEBOLD, BST_UNCHECKED);
				//	CheckDlgButton(hwnd, IDC_STYLEITALIC, BST_UNCHECKED);
				//	CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, BST_UNCHECKED);
				//}
				EnableWindow(GetDlgItem(hwnd, IDC_STYLEFORE), (enableMask & StyleControl_Fore) != 0);
				EnableWindow(GetDlgItem(hwnd, IDC_STYLEBACK), (enableMask & StyleControl_Back) != 0);
				//if (enableMask & StyleControl_EOLFilled) {
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEEOLFILLED), TRUE);
				//	CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, Style_StrGetEOLFilled(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED);
				//} else {
				//	EnableWindow(GetDlgItem(hwnd, IDC_STYLEEOLFILLED), FALSE);
				//	CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, BST_UNCHECKED);
				//}
				if (changed) {
					NotifyEditTextChanged(hwnd, IDC_STYLEEDIT);
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
			const LONG xCur = GET_X_LPARAM(lParam);
			const LONG yCur = GET_Y_LPARAM(lParam);

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
					//CheckDlgButton(hwnd, IDC_STYLEBOLD, Style_StrGetBold(tchCopy) ? BST_CHECKED : BST_UNCHECKED);
					//CheckDlgButton(hwnd, IDC_STYLEITALIC, Style_StrGetItalic(tchCopy) ? BST_CHECKED : BST_UNCHECKED);
					//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, Style_StrGetUnderline(tchCopy) ? BST_CHECKED : BST_UNCHECKED);
					//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, Style_StrGetEOLFilled(tchCopy) ? BST_CHECKED : BST_UNCHECKED);
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
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_NEXTSTYLE:
			if (TreeView_GetSelection(hwndTV)) {
				TreeView_Select(hwndTV, TreeView_GetNextVisible(hwndTV,
								TreeView_GetSelection(hwndTV)), TVGN_CARET);
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_STYLEFONT:
			if (pCurrentStyle) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
				const BOOL bDefaultStyle = (pCurrentLexer->rid == NP2LEX_GLOBAL || pCurrentLexer->rid == NP2LEX_2NDGLOBAL) && IsGlobalBaseStyleIndex(iCurrentStyleIndex);

				if (Style_SelectFont(hwnd, tch, COUNTOF(tch), bDefaultStyle)) {
					SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
					//CheckDlgButton(hwnd, IDC_STYLEBOLD, (Style_StrGetBold(tch) ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEITALIC, (Style_StrGetItalic(tch) ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, (Style_StrGetUnderline(tch) ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, (Style_StrGetEOLFilled(tch) ? BST_CHECKED : BST_UNCHECKED));
				}
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_STYLEFORE:
		case IDC_STYLEBACK:
			if (pCurrentStyle) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
				const BOOL fore = LOWORD(wParam) == IDC_STYLEFORE;
				if (Style_SelectColor(hwnd, fore, tch, COUNTOF(tch))) {
					SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
					//CheckDlgButton(hwnd, IDC_STYLEBOLD, (Style_StrGetBold(tch) ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEITALIC, (Style_StrGetItalic(tch) ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, (Style_StrGetUnderline(tch) ? BST_CHECKED : BST_UNCHECKED));
					//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, (Style_StrGetEOLFilled(tch) ? BST_CHECKED : BST_UNCHECKED));
				}
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_RESETALL:
		case IDC_STYLEDEFAULT:
			if (LOWORD(wParam) == IDC_RESETALL) {
				if (np2StyleTheme == StyleTheme_Default) {
					// reset styles, extensions to built-in default
					Style_ResetAll(TRUE);
				} else {
					// reload styles from external file
					Style_LoadAll(TRUE);
					// reset file extensions to built-in default
					Style_ResetAll(FALSE);
				}
			} else if (pCurrentStyle) {
				Style_ResetStyle(pCurrentLexer, pCurrentStyle);
			}
			if (pCurrentStyle) {
				SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue);
				//CheckDlgButton(hwnd, IDC_STYLEBOLD, (Style_StrGetBold(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED));
				//CheckDlgButton(hwnd, IDC_STYLEITALIC, (Style_StrGetItalic(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED));
				//CheckDlgButton(hwnd, IDC_STYLEUNDERLINE, (Style_StrGetUnderline(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED));
				//CheckDlgButton(hwnd, IDC_STYLEEOLFILLED, (Style_StrGetEOLFilled(pCurrentStyle->szValue) ? BST_CHECKED : BST_UNCHECKED));
			} else if (fLexerSelected && pCurrentLexer && pCurrentLexer->szExtensions) {
				lstrcpy(pCurrentLexer->szExtensions, pCurrentLexer->pszDefExt);
				SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions);
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		//case IDC_STYLEBOLD:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"bold", IsButtonChecked(hwnd, IDC_STYLEBOLD));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
		//	}
		//	break;

		//case IDC_STYLEITALIC:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"italic", IsButtonChecked(hwnd, IDC_STYLEITALIC));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
		//	}
		//	break;

		//case IDC_STYLEUNDERLINE:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"underline", IsButtonChecked(hwnd, IDC_STYLEUNDERLINE));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
		//	}
		//	break;

		//case IDC_STYLEEOLFILLED:
		//	if (pCurrentStyle) {
		//		WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
		//		GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
		//		Style_StrSetAttribute(tch, COUNTOF(tch), L"eolfilled", IsButtonChecked(hwnd, IDC_STYLEEOLFILLED));
		//		SetDlgItemText(hwnd, IDC_STYLEEDIT, tch);
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
			} else if (fLexerSelected && pCurrentLexer && pCurrentLexer->szExtensions) {
				if (!GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions, MAX_EDITLEXER_EXT_SIZE)) {
					lstrcpy(pCurrentLexer->szExtensions, pCurrentLexer->pszDefExt);
				}
			}

			switch (LOWORD(wParam)) {
			case IDC_IMPORT:
				if (Style_Import(hwnd)) {
					if (pCurrentStyle) {
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue);
					} else if (fLexerSelected && pCurrentLexer) {
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions);
					}

					TreeView_Select(hwndTV, TreeView_GetRoot(hwndTV), TVGN_CARET);
				}
				break;

			case IDC_EXPORT:
				Style_Export(hwnd);
				break;

			case IDC_PREVIEW: {
				struct StyleConfigDlgParam *param = (struct StyleConfigDlgParam *)GetWindowLongPtr(hwnd, DWLP_USER);
				param->bApply = TRUE;
				Style_SetLexer(hwndEdit, pLexCurrent);
			} break;

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

	return FALSE;
}

//=============================================================================
//
// Style_ConfigDlg()
//
void Style_ConfigDlg(HWND hwnd) {
	struct StyleConfigDlgParam param;

	Style_LoadAll(FALSE);
	// Backup Styles
	param.hFontTitle = NULL;
	param.bApply = FALSE;
	LPWSTR extBackup = (LPWSTR)NP2HeapAlloc(ALL_FILE_EXTENSIONS_BYTE_SIZE);
	param.extBackup = extBackup;
	CopyMemory(extBackup, g_AllFileExtensions, ALL_FILE_EXTENSIONS_BYTE_SIZE);
	CopyMemory(param.colorBackup, customColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));
	LPCWSTR backupGlobal = NULL;
	LPCWSTR backupCurrent = NULL;
	for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
		const LPCEDITLEXER pLex = pLexArray[iLexer];
		const UINT iStyleBufSize = EDITSTYLE_BufferSize(pLex->iStyleCount);
		LPWSTR szStyleBuf = (LPWSTR)NP2HeapAlloc(iStyleBufSize);
		CopyMemory(szStyleBuf, pLex->szStyleBuf, iStyleBufSize);
		param.styleBackup[iLexer] = szStyleBuf;
		if (pLex == pLexGlobal) {
			backupGlobal = szStyleBuf;
		} else if (pLex == pLexCurrent) {
			backupCurrent = szStyleBuf;
		}
	}

	if (IDCANCEL == ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_STYLECONFIG), GetParent(hwnd), Style_ConfigDlgProc, (LPARAM)(&param))) {
		// Restore Styles
		CopyMemory(g_AllFileExtensions, param.extBackup, ALL_FILE_EXTENSIONS_BYTE_SIZE);
		CopyMemory(customColor, param.colorBackup, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));
		for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			CopyMemory(pLex->szStyleBuf, param.styleBackup[iLexer], EDITSTYLE_BufferSize(pLex->iStyleCount));
		}
	} else {
		if (!(fStylesModified & STYLESMODIFIED_FILE_EXT)) {
			if (memcmp(param.extBackup, g_AllFileExtensions, ALL_FILE_EXTENSIONS_BYTE_SIZE) != 0) {
				fStylesModified |= STYLESMODIFIED_FILE_EXT;
			}
		}
		if (!(fStylesModified & STYLESMODIFIED_COLOR)) {
			if (memcmp(param.colorBackup, customColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF)) != 0) {
				fStylesModified |= STYLESMODIFIED_COLOR;
			}
		}
		if (!(fStylesModified & STYLESMODIFIED_ALL_STYLE)) {
			int count = 0;
			for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
				PEDITLEXER pLex = pLexArray[iLexer];
				if (pLex->bStyleChanged || memcmp(param.styleBackup[iLexer], pLex->szStyleBuf, EDITSTYLE_BufferSize(pLex->iStyleCount)) != 0) {
					pLex->bStyleChanged = TRUE;
					++count;
				}
			}
			fStylesModified |= (count == 0) ? STYLESMODIFIED_NONE : ((count == ALL_LEXER_COUNT) ? STYLESMODIFIED_ALL_STYLE : STYLESMODIFIED_SOME_STYLE);
		}

		if (!param.bApply) {
			// global default or current styles changed
			// Note: bStyleChanged is retained until styles been saved.
			param.bApply = memcmp(backupGlobal, pLexGlobal->szStyleBuf, EDITSTYLE_BufferSize(pLexGlobal->iStyleCount)) != 0
				|| memcmp(backupCurrent, pLexCurrent->szStyleBuf, EDITSTYLE_BufferSize(pLexCurrent->iStyleCount)) != 0;
		}
		if ((fStylesModified & STYLESMODIFIED_WARN_MASK) && !fWarnedNoIniFile) {
			LPCWSTR themePath = GetStyleThemeFilePath();
			if (StrIsEmpty(themePath)) {
				MsgBox(MBWARN, IDS_SETTINGSNOTSAVED);
				fWarnedNoIniFile = TRUE;
			}
		}
	}

	NP2HeapFree(param.extBackup);
	for (UINT iLexer = 0; iLexer < ALL_LEXER_COUNT; iLexer++) {
		NP2HeapFree(param.styleBackup[iLexer]);
	}
	DeleteObject(param.hFontTitle);

	// Apply new (or previous) Styles
	if (param.bApply) {
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
		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, NULL, -1, 0, 0, 0
#if (NTDDI_VERSION >= NTDDI_VISTA)
			, 0, 0, 0
#endif
		};
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
		int iCurrent = -1;
		for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			Style_AddLexerToListView(hwndLV, pLex);
			if (iCurrent < 0 && pLex->rid == pLexCurrent->rid) {
				iCurrent = iLexer;
			}
		}

		ListView_SetColumnWidth(hwndLV, 0, LVSCW_AUTOSIZE_USEHEADER);

		// Select current lexer
		iCurrent = max_i(0, iCurrent);
		ListView_SetItemState(hwndLV, iCurrent, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		ListView_EnsureVisible(hwndLV, iCurrent, FALSE);
		if (iDefaultLexer == iCurrent) {
			CheckDlgButton(hwnd, IDC_DEFAULTSCHEME, BST_CHECKED);
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
		int dx;
		int dy;

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
				SendWMCommand(hwnd, IDOK);
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
