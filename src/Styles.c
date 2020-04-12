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
#include <uxtheme.h>
#include <stdio.h>
#include <math.h>
#include "SciCall.h"
#include "config.h"
#include "Helpers.h"
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
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
extern EDITLEXER lexANSI;
extern EDITLEXER lexASM;
extern EDITLEXER lexASY;
extern EDITLEXER lexAU3;
extern EDITLEXER lexAwk;

extern EDITLEXER lexBatch;

extern EDITLEXER lexCIL;
extern EDITLEXER lexCMake;
extern EDITLEXER lexCONF;

extern EDITLEXER lexD;
extern EDITLEXER lexDIFF;

extern EDITLEXER lexFSharp;
extern EDITLEXER lexFortran;

extern EDITLEXER lexGN;
extern EDITLEXER lexGo;
extern EDITLEXER lexGradle;
extern EDITLEXER lexDOT;
extern EDITLEXER lexGroovy;

extern EDITLEXER lexHaXe;

//extern EDITLEXER lexIDL;
extern EDITLEXER lexINI;
extern EDITLEXER lexINNO;

extern EDITLEXER lexJAM;
extern EDITLEXER lexJulia;

extern EDITLEXER lexKotlin;

extern EDITLEXER lexLaTeX;
extern EDITLEXER lexLisp;
extern EDITLEXER lexLLVM;
extern EDITLEXER lexLua;

extern EDITLEXER lexMake;
extern EDITLEXER lexMatlab;

extern EDITLEXER lexNsis;

extern EDITLEXER lexPascal;
extern EDITLEXER lexPerl;
extern EDITLEXER lexPS1;

extern EDITLEXER lexRC;
extern EDITLEXER lexRust;

extern EDITLEXER lexScala;
extern EDITLEXER lexBash;

extern EDITLEXER lexTcl;
extern EDITLEXER lexTexinfo;
extern EDITLEXER lexTOML;

extern EDITLEXER lexVBS;
extern EDITLEXER lexVerilog;
extern EDITLEXER lexVHDL;
extern EDITLEXER lexVim;
extern EDITLEXER lexVB;

extern EDITLEXER lexWASM;

extern EDITLEXER lexYAML;

// the two global styles at the beginning of the array not visible in
// "Select Scheme" list, don't participate in file extension match.
#define LEXER_INDEX_MATCH		2	// global styles
#define ALL_LEXER_COUNT			(NUMLEXERS + LEXER_INDEX_MATCH)
// the lexer array has three sections:
// 1. global styles and lexers for text file, not sortable, the order is hard-coded.
// 2. favorite lexers, sortable, the order is configured by FavoriteSchemes preference.
// 3. other lexers, grouped by first letter, sorted alphabetical (case insensitive).
#define LEXER_INDEX_GENERAL		(LEXER_INDEX_MATCH + 2)	// global styles and lexers for text file
#define GENERAL_LEXER_COUNT		(ALL_LEXER_COUNT - LEXER_INDEX_GENERAL)
#define MAX_FAVORITE_SCHEMES_COUNT			31
#define MAX_FAVORITE_SCHEMES_CONFIG_SIZE	96	// 1 + MAX_FAVORITE_SCHEMES_COUNT*3
// This array holds all the lexers...
static PEDITLEXER pLexArray[ALL_LEXER_COUNT] = {
	&lexGlobal,
	&lex2ndGlobal,

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
	&lexANSI,
	&lexASM,
	&lexASY,
	&lexAU3,
	&lexAwk,

	&lexBatch,

	&lexCIL,
	&lexCMake,
	&lexCONF,

	&lexD,
	&lexDIFF,

	&lexFSharp,
	&lexFortran,

	&lexGN,
	&lexGo,
	&lexGradle,
	&lexDOT,
	&lexGroovy,

	&lexHaXe,

	//&lexIDL,
	&lexINI,
	&lexINNO,

	&lexJAM,
	&lexJulia,

	&lexKotlin,

	&lexLaTeX,
	&lexLisp,
	&lexLLVM,
	&lexLua,

	&lexMake,
	&lexMatlab,

	&lexNsis,

	&lexPascal,
	&lexPerl,
	&lexPS1,

	&lexRC,
	&lexRust,

	&lexScala,
	&lexBash,

	&lexTcl,
	&lexTexinfo,
	&lexTOML,

	&lexVBS,
	&lexVerilog,
	&lexVHDL,
	&lexVim,
	&lexVB,

	&lexWASM,

	&lexYAML,
};

// system available default monospaced font and proportional font
static WCHAR systemCodeFontName[LF_FACESIZE];
static WCHAR systemTextFontName[LF_FACESIZE];
// global default monospaced font and proportional font
static WCHAR defaultCodeFontName[LF_FACESIZE];
static WCHAR defaultTextFontName[LF_FACESIZE];

static WCHAR darkStyleThemeFilePath[MAX_PATH];
static WCHAR favoriteSchemesConfig[MAX_FAVORITE_SCHEMES_CONFIG_SIZE];

// Currently used lexer
static PEDITLEXER pLexGlobal = &lexGlobal;
PEDITLEXER pLexCurrent = &lexTextFile;
int np2LexLangIndex = 0;
uint8_t currentLexKeywordAttr[NUMKEYWORD] = {0};

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
	RGB(0x00, 0x80, 0xC0),	// Built-in Function
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
static int	defaultBaseFontSize = 11*SC_FONT_SIZE_MULTIPLIER; // 11 pt
static int iBaseFontSize = 11*SC_FONT_SIZE_MULTIPLIER;
int		iFontQuality = SC_EFF_QUALITY_LCD_OPTIMIZED;
int		iCaretStyle = 1; // width 1, 0 for block
int		iOvrCaretStyle = 0; // 0 for bar, 1 for block
BOOL bBlockCaretOutSelection = 0;
int		iCaretBlinkPeriod = -1; // system default, 0 for noblink
static BOOL bBookmarkColorUpdated;
static int	iDefaultLexerIndex;
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
extern BOOL	bShowBookmarkMargin;

#define STYLE_MASK_FONT_FACE	(1 << 0)
#define STYLE_MASK_FONT_SIZE	(1 << 1)
#define STYLE_MASK_FORE_COLOR	(1 << 2)
#define STYLE_MASK_BACK_COLOR	(1 << 3)
#define STYLE_MASK_FONT_WEIGHT	(1 << 4)
#define STYLE_MASK_FORCE_CASE	(1 << 5)
#define STYLE_MASK_CHARSET		(1 << 6)

// LF_FACESIZE is 32, LOCALE_NAME_MAX_LENGTH is 85
#define MAX_STYLE_VALUE_LENGTH	LOCALE_NAME_MAX_LENGTH

struct DetailStyle {
	UINT mask;
	int fontSize;
	COLORREF foreColor;
	COLORREF backColor;
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
//! MUST keep all index in same order as lexGlobal
enum GlobalStyleIndex {
	GlobalStyleIndex_DefaultCode,		// global default code style.
	GlobalStyleIndex_DefaultText,		// global default text style.
	GlobalStyleIndex_LineNumber,		// inherited style, except for background color (default to COLOR_3DFACE).
	GlobalStyleIndex_MatchBrace,		// indicator style. `fore`, `alpha`, `outline`
	GlobalStyleIndex_MatchBraceError,	// indicator style. `fore`, `alpha`, `outline`
	GlobalStyleIndex_ControlCharacter,	// inherited style. font attributes (inverted text/background color, e.g. white on black or black on white based on theme)
	GlobalStyleIndex_IndentationGuide,	// inherited style. `fore`, `back`
	GlobalStyleIndex_Selection,			// standalone style. main selection (`back`, `alpha`), additional selection (`fore`, `outline`), `eolfilled`
	GlobalStyleIndex_Whitespace,		// standalone style. `fore`, `back`, `size`: dot size
	GlobalStyleIndex_CurrentBlock,		// standalone style. `fore`
	GlobalStyleIndex_CurrentLine,		// standalone style. frame (`fore`, `size`, `outline`), background (`back`, `alpha`)
	GlobalStyleIndex_Caret,				// standalone style. `fore`: main caret color, `back`: additional caret color
	GlobalStyleIndex_IMEIndicator,		// indicator style. `fore`: IME indicator color
	GlobalStyleIndex_LongLineMarker,	// standalone style. `fore`: edge line color, `back`: background color for text exceeds long line limit
	GlobalStyleIndex_ExtraLineSpacing,	// standalone style. descent = `size`/2, ascent = `size` - descent
	GlobalStyleIndex_FoldingMarker,		// standalone style. `fore`: folding line color, `back`: plus/minus box fill color
	GlobalStyleIndex_FoldDispalyText,	// inherited style.
	GlobalStyleIndex_MarkOccurrences,	// indicator style. `fore`, `alpha`, `outline`
	GlobalStyleIndex_Bookmark,			// indicator style. `fore`, `back`, `alpha`
	GlobalStyleIndex_CallTip,			// inherited style.
};

// styles in ANSI Art used to override global styles.
//! MUST keep all index in same order as lexANSI
enum ANSIArtStyleIndex {
	ANSIArtStyleIndex_Default,
	ANSIArtStyleIndex_LineNumber,
	ANSIArtStyleIndex_ExtraLineSpacing,
	ANSIArtStyleIndex_FoldDispalyText,
};

// folding marker
#define FoldingMarkerLineColorDefault	RGB(0x80, 0x80, 0xFF)
#define FoldingMarkerFillColorDefault	RGB(0xAD, 0xD8, 0xE6)
#define FoldingMarkerLineColorDark		RGB(0x80, 0x80, 0x80)
#define FoldingMarkerFillColorDark		RGB(0x60, 0x60, 0x60)

// from ScintillaWin.cxx
#define SC_INDICATOR_INPUT		INDICATOR_IME
#define SC_INDICATOR_TARGET		(INDICATOR_IME + 1)
#define SC_INDICATOR_CONVERTED	(INDICATOR_IME + 2)
#define SC_INDICATOR_UNKNOWN	INDICATOR_IME_MAX

#define IMEIndicatorDefaultColor	RGB(0x10, 0x80, 0x10)
#define MarkOccurrencesDefaultAlpha	100

#define	BookmarkImageDefaultColor	RGB(0x40, 0x80, 0x40)
#define	BookmarkLineDefaultColor	RGB(0, 0xff, 0)
#define BookmarkLineDefaultAlpha	40

#define BookmarkUsingPixmapImage		0
#if BookmarkUsingPixmapImage
// XPM Graphics for bookmark on selection margin, always 16px width.
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

static inline BOOL IsGlobalBaseStyleIndex(int rid, int index) {
	return (rid == NP2LEX_GLOBAL || rid == NP2LEX_2NDGLOBAL)
		&& (index == GlobalStyleIndex_DefaultCode || index == GlobalStyleIndex_DefaultText);
}

static inline UINT GetLexerStyleControlMask(int rid, int index) {
	switch (rid) {
	case NP2LEX_GLOBAL:
	case NP2LEX_2NDGLOBAL:
		switch (index) {
		case GlobalStyleIndex_ControlCharacter:
			return StyleControl_Font;
		case GlobalStyleIndex_IndentationGuide:
		case GlobalStyleIndex_Whitespace:
		case GlobalStyleIndex_CurrentLine:
		case GlobalStyleIndex_Caret:
		case GlobalStyleIndex_LongLineMarker:
		case GlobalStyleIndex_FoldingMarker:
		case GlobalStyleIndex_Bookmark:
			return StyleControl_Fore | StyleControl_Back;
		case GlobalStyleIndex_Selection:
			return StyleControl_Fore | StyleControl_Back | StyleControl_EOLFilled;
		case GlobalStyleIndex_MatchBrace:
		case GlobalStyleIndex_MatchBraceError:
		case GlobalStyleIndex_CurrentBlock:
		case GlobalStyleIndex_IMEIndicator:
		case GlobalStyleIndex_MarkOccurrences:
			return StyleControl_Fore;
		case GlobalStyleIndex_ExtraLineSpacing:
			return StyleControl_None;
		default:
			return StyleControl_All;
		}
		break;

	case NP2LEX_ANSI:
		return (index == ANSIArtStyleIndex_ExtraLineSpacing)? StyleControl_None : StyleControl_All;

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
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
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
	defaultBaseFontSize = size;
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
	return pLex->iStyleTheme == np2StyleTheme && pLex->szStyleBuf != NULL;
}

static inline LPCWSTR GetStyleThemeFilePath(void) {
	return (np2StyleTheme == StyleTheme_Dark) ? darkStyleThemeFilePath : szIniFile;
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
		for (UINT i = 0; i < iStyleCount; i++) {
			LPWSTR szValue = szStyleBuf + (i * MAX_EDITSTYLE_VALUE_SIZE);
			pLex->Styles[i].szValue = szValue;
			lstrcpy(szValue, pLex->Styles[i].pszDefault);
		}
	} else {
		pLex->bUseDefaultCodeStyle = IniSectionGetBool(pIniSection, L"UseDefaultCodeStyle", pLex->bUseDefaultCodeStyle);
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

	pLex->iStyleTheme = np2StyleTheme;
}

void Style_SetFavoriteSchemes(void) {
	int favorite[MAX_FAVORITE_SCHEMES_COUNT];
	const int count = ParseCommaList(favoriteSchemesConfig, favorite, MAX_FAVORITE_SCHEMES_COUNT);
	UINT index = LEXER_INDEX_GENERAL;

	for (int i = 0; i < count; i++) {
		const int rid = favorite[i] + NP2LEX_TEXTFILE;
		for (UINT iLexer = index; iLexer < ALL_LEXER_COUNT; iLexer++) {
			PEDITLEXER pLex = pLexArray[iLexer];
			if (pLex->rid == rid) {
				pLex->iFavoriteOrder = MAX_FAVORITE_SCHEMES_COUNT - i;
				// move pLex to the end of favorite schemes
				if (iLexer != index) {
					do {
						pLexArray[iLexer] = pLexArray[iLexer - 1];
						--iLexer;
					} while (iLexer != index);
					pLexArray[iLexer] = pLex;
				}
				++index;
				break;
			}
		}
	}
}

void Style_GetFavoriteSchemes(void) {
	const int maxCch = MAX_FAVORITE_SCHEMES_CONFIG_SIZE - 3; // two digits, one space and NULL
	WCHAR *wch = favoriteSchemesConfig;
	int len = 0;
	int count = 0;
	for (UINT iLexer = LEXER_INDEX_GENERAL; iLexer < ALL_LEXER_COUNT; iLexer++) {
		const LPCEDITLEXER pLex = pLexArray[iLexer];
		if (!pLex->iFavoriteOrder) {
			break;
		}

		len += wsprintf(wch + len, L"%i ", pLex->rid - NP2LEX_TEXTFILE);
		++count;
		if (count == MAX_FAVORITE_SCHEMES_COUNT || len > maxCch) {
			break;
		}
	}

	wch[len--] = L'\0';
	if (len >= 0) {
		wch[len] = L'\0';
	}
}

static int __cdecl CmpEditLexerByOrder(const void *p1, const void *p2) {
	LPCEDITLEXER pLex1 = *(LPCEDITLEXER *)(p1);
	LPCEDITLEXER pLex2 = *(LPCEDITLEXER *)(p2);
	int cmp = pLex2->iFavoriteOrder - pLex1->iFavoriteOrder;
	// TODO: sort by localized name
#if NP2_ENABLE_LEXER_NAME_LOCALIZATION
#endif
	cmp = cmp ? cmp : StrCmpIW(pLex1->pszName, pLex2->pszName);
	return cmp;
}

static int __cdecl CmpEditLexerByName(const void *p1, const void *p2) {
	LPCEDITLEXER pLex1 = *(LPCEDITLEXER *)(p1);
	LPCEDITLEXER pLex2 = *(LPCEDITLEXER *)(p2);
	// TODO: sort by localized name
#if NP2_ENABLE_LEXER_NAME_LOCALIZATION
#endif
	int cmp = StrCmpIW(pLex1->pszName, pLex2->pszName);
	return cmp;
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

	// favorite schemes
	LPCWSTR strValue = IniSectionGetValue(pIniSection, L"FavoriteSchemes");
	if (StrNotEmpty(strValue)) {
		lstrcpyn(favoriteSchemesConfig, strValue, MAX_FAVORITE_SCHEMES_CONFIG_SIZE);
		Style_SetFavoriteSchemes();
	}

	// default scheme
	int iValue = IniSectionGetInt(pIniSection, L"DefaultScheme", 0);
	iDefaultLexerIndex = Style_GetMatchLexerIndex(iValue + NP2LEX_TEXTFILE);

	iValue = IniSectionGetInt(pIniSection, L"StyleTheme", StyleTheme_Default);
	np2StyleTheme = clamp_i(iValue, StyleTheme_Default, StyleTheme_Max);

	// auto select
	bAutoSelect = IniSectionGetBool(pIniSection, L"AutoSelect", 1);

	strValue = IniSectionGetValue(pIniSection, L"DarkTheme.ini");
	if (StrNotEmpty(strValue)) {
		lstrcpyn(darkStyleThemeFilePath, strValue, COUNTOF(darkStyleThemeFilePath));
	}

	// file extensions
	LoadIniSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);
	for (UINT iLexer = 0; iLexer < NUMLEXERS; iLexer++) {
		PEDITLEXER pLex = pLexArray[iLexer + LEXER_INDEX_MATCH];
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
	Style_LoadOneEx(pLexArray[iDefaultLexerIndex], pIniSection, pIniSectionBuf, cchIniSection);

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

static void Style_LoadAll(BOOL bReload) {
	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_STYLES);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);

	// Custom colors
	const int value = (np2StyleTheme << 1) | 1;
	if (bReload || iCustomColorLoaded != value) {
		LPCWSTR themePath = GetStyleThemeFilePath();
		iCustomColorLoaded = value;
		CopyMemory(customColor, defaultCustomColor, MAX_CUSTOM_COLOR_COUNT * sizeof(COLORREF));

		GetPrivateProfileSection(INI_SECTION_NAME_CUSTOM_COLORS, pIniSectionBuf, cchIniSection, themePath);
		IniSectionParseArray(pIniSection, pIniSectionBuf, FALSE);

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
		if (bReload || !IsStyleLoaded(pLex)) {
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
	// favorite schemes
	IniSectionSetString(pIniSection, L"FavoriteSchemes", favoriteSchemesConfig);
	// default scheme
	IniSectionSetIntEx(pIniSection, L"DefaultScheme", pLexArray[iDefaultLexerIndex]->rid - NP2LEX_TEXTFILE, 0);
	IniSectionSetIntEx(pIniSection, L"StyleTheme", np2StyleTheme, StyleTheme_Default);

	// auto select
	IniSectionSetBoolEx(pIniSection, L"AutoSelect", bAutoSelect, 1);

	SaveIniSection(INI_SECTION_NAME_STYLES, pIniSectionBuf);

	// file extensions
	if (fStylesModified & STYLESMODIFIED_FILE_EXT) {
		ZeroMemory(pIniSectionBuf, cbIniSection);
		pIniSection->next = pIniSectionBuf;
		for (UINT iLexer = LEXER_INDEX_MATCH; iLexer < ALL_LEXER_COUNT; iLexer++) {
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
				wsprintf(wch, L"#%02X%02X%02X", GetRValue(color), GetGValue(color), GetBValue(color));
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
			IniSectionSetBoolEx(pIniSection, L"UseDefaultCodeStyle", pLex->bUseDefaultCodeStyle, pLex->rid != NP2LEX_TEXTFILE);
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
		// file extensions
		if (GetPrivateProfileSection(INI_SECTION_NAME_FILE_EXTENSIONS, pIniSectionBuf, cchIniSection, szFile)) {
			if (IniSectionParse(pIniSection, pIniSectionBuf)) {
				for (UINT iLexer = LEXER_INDEX_MATCH; iLexer < ALL_LEXER_COUNT; iLexer++) {
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
				pLex->bUseDefaultCodeStyle = IniSectionGetBool(pIniSection, L"UseDefaultCodeStyle", pLex->bUseDefaultCodeStyle);
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

		// file extensions
		pIniSection->next = pIniSectionBuf;
		for (UINT iLexer = LEXER_INDEX_MATCH; iLexer < ALL_LEXER_COUNT; iLexer++) {
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
		pLex->bUseDefaultCodeStyle = (pLex->rid == NP2LEX_TEXTFILE)? 0 : 1;
		if (resetColor) {
			const UINT iStyleCount = pLex->iStyleCount;
			for (UINT i = 0; i < iStyleCount; i++) {
				lstrcpy(pLex->Styles[i].szValue, pLex->Styles[i].pszDefault);
			}
		}
	}

	fStylesModified |= STYLESMODIFIED_ALL_STYLE | STYLESMODIFIED_FILE_EXT | STYLESMODIFIED_COLOR;
}

void Style_OnDPIChanged(void) {
	Style_SetLexer(pLexCurrent, FALSE);
}

void Style_OnStyleThemeChanged(int theme) {
	if (theme != StyleTheme_Default) {
		if (!PathFileExists(darkStyleThemeFilePath)) {
			FindDarkThemeFile();
		}
	}

	if (fStylesModified) {
		SaveSettingsNow(TRUE, TRUE);
	}
	np2StyleTheme = theme;
	Style_SetLexer(pLexCurrent, FALSE);
}

void Style_UpdateCaret(void) {
	// caret style and width
	const int style = (iCaretStyle ? CARETSTYLE_LINE : CARETSTYLE_BLOCK)
		| (iOvrCaretStyle ? CARETSTYLE_OVERSTRIKE_BLOCK : CARETSTYLE_OVERSTRIKE_BAR)
		| (bBlockCaretOutSelection ? CARETSTYLE_BLOCK_AFTER : 0);
	SciCall_SetCaretStyle(style);
	if (iCaretStyle != 0) {
		SciCall_SetCaretWidth(iCaretStyle);
	}

	const int iValue = (iCaretBlinkPeriod < 0)? (int)GetCaretBlinkTime() : iCaretBlinkPeriod;
	SciCall_SetCaretPeriod(iValue);
}

void Style_UpdateLexerKeywordAttr(LPCEDITLEXER pLexNew) {
	ZeroMemory(currentLexKeywordAttr, sizeof(currentLexKeywordAttr));
	uint8_t *attr = currentLexKeywordAttr;

	// Code Snippet
	attr[NUMKEYWORD - 1] = KeywordAttr_NoLexer;

	switch (pLexNew->rid) {
	case NP2LEX_AU3:
		break;
	case NP2LEX_BATCH:
		attr[6] = KeywordAttr_NoLexer;		// Upper Case Keyword
		break;
	case NP2LEX_CPP:
		attr[2] = KeywordAttr_NoAutoComp;	// Preprocessor
		attr[3] = KeywordAttr_NoAutoComp;	// Directive
		attr[11] = KeywordAttr_NoAutoComp;	// Assembler Intruction
		attr[12] = KeywordAttr_NoAutoComp;	// Assembler Register
		attr[13] = KeywordAttr_NoLexer;		// C Function
		attr[14] = KeywordAttr_NoLexer;		// C++ Function
		break;
	case NP2LEX_CSHARP:
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
	case NP2LEX_HTML:
		attr[1] = KeywordAttr_NoAutoComp;	// JavaScript
		attr[2] = KeywordAttr_MakeLower | KeywordAttr_NoAutoComp;	// VBScript
		attr[3] = KeywordAttr_NoAutoComp;	// Python
		attr[4] = KeywordAttr_NoAutoComp;	// PHP
		attr[7] = KeywordAttr_NoLexer;		// Value
		break;
	case NP2LEX_INNO:
		break;
	case NP2LEX_JAVA:
		attr[10] = KeywordAttr_NoLexer;		// Package
		break;
	case NP2LEX_JS:
		//attr[1] = KeywordAttr_NoAutoComp;	// Reserved Word
		attr[9] = KeywordAttr_NoLexer;		// Function
		attr[10] = KeywordAttr_NoLexer;		// Property
		attr[11] = KeywordAttr_NoLexer;		// Method
		break;
	case NP2LEX_NSIS:
		attr[0] = KeywordAttr_MakeLower;
		break;
	case NP2LEX_RC:
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
	case NP2LEX_SQL:
		attr[6] = KeywordAttr_NoLexer;		// Upper Case Keyword
		attr[7] = KeywordAttr_NoLexer;		// Upper Case Type
		attr[8] = KeywordAttr_NoLexer;		// Upper Case Function
		break;
	case NP2LEX_XML:
		attr[6] = KeywordAttr_NoLexer;		// Attribute
		attr[7] = KeywordAttr_NoLexer;		// Value
		break;
//++Autogenerated -- start of section automatically generated
	case NP2LEX_CMAKE:
		attr[6] = KeywordAttr_NoLexer;		// long properties
		attr[7] = KeywordAttr_NoLexer;		// long variables
		break;
	case NP2LEX_GN:
		attr[3] = KeywordAttr_NoLexer;		// target variables
		attr[4] = KeywordAttr_NoLexer;		// placeholders
		break;
	case NP2LEX_JULIA:
		attr[1] = KeywordAttr_NoAutoComp;	// code fold
		attr[5] = KeywordAttr_NoLexer;		// module
		attr[6] = KeywordAttr_NoLexer | KeywordAttr_NoAutoComp;	// macro
		attr[7] = KeywordAttr_NoLexer;		// function
		break;
	case NP2LEX_KOTLIN:
		attr[4] = KeywordAttr_NoLexer;		// annotation
		attr[5] = KeywordAttr_NoLexer;		// function
		attr[6] = KeywordAttr_NoLexer | KeywordAttr_NoAutoComp;	// KDoc
		break;
	case NP2LEX_RUBY:
		attr[1] = KeywordAttr_NoAutoComp;	// code fold
		attr[2] = KeywordAttr_NoAutoComp;	// re
		attr[3] = KeywordAttr_NoLexer;		// pre-defined variables
		break;
	case NP2LEX_RUST:
		attr[1] = KeywordAttr_NoAutoComp;	// reserved keywords
		attr[8] = KeywordAttr_NoLexer;		// attribute
		attr[9] = KeywordAttr_NoLexer;		// macro
		attr[10] = KeywordAttr_NoLexer;		// module
		attr[11] = KeywordAttr_NoLexer;		// function
		break;
	case NP2LEX_WASM:
		attr[3] = KeywordAttr_NoLexer;		// full instruction
		break;
//--Autogenerated -- end of section automatically generated
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
		|| iLexer == SCLEX_GN
		|| iLexer == SCLEX_LLVM
		|| iLexer == SCLEX_MAKEFILE
		|| iLexer == SCLEX_PERL
		|| iLexer == SCLEX_PROPERTIES
		|| iLexer == SCLEX_PYTHON
		|| iLexer == SCLEX_RUBY
		|| iLexer == SCLEX_SMALI
		|| iLexer == SCLEX_TEXINFO
		|| iLexer == SCLEX_TOML
		|| iLexer == SCLEX_VB
		|| iLexer == SCLEX_VBSCRIPT
		|| iLexer == SCLEX_VIM
		|| iLexer == SCLEX_YAML
		|| rid == NP2LEX_AWK
		|| rid == NP2LEX_JAM
	);
}

// Python like indentation based code folding that can use SC_IV_LOOKFORWARD
static inline BOOL IsPythonLikeFolding(int iLexer) {
	return iLexer == SCLEX_NULL
		|| iLexer == SCLEX_PYTHON
		|| iLexer == SCLEX_YAML;
}

static void Style_Parse(struct DetailStyle *style, LPCWSTR lpszStyle);
static void Style_SetParsed(const struct DetailStyle *style, int iStyle);
static inline void Style_SetDefaultStyle(int index) {
	Style_SetStyles(pLexGlobal->Styles[index].iStyle, pLexGlobal->Styles[index].szValue);
}

// parse a style attribute separated by ';'
// e.g.: 'bold', 'bold;', '; bold' and '; bold;'
static BOOL Style_StrGetAttributeEx(LPCWSTR lpszStyle, LPCWSTR key, int keyLen) {
	LPCWSTR p = StrStr(lpszStyle, key);
	while (p != NULL) {
		WCHAR chPrev = (p == lpszStyle) ? L';' : p[-1];
		if (chPrev == L' ') {
			LPCWSTR t = p - 2;
			while (t > lpszStyle && *t == L' ') {
				--t;
			}
			chPrev = (t <= lpszStyle) ? L';' : *t;
		}
		p += keyLen;
		if (chPrev == L';') {
			while (*p == L' ') {
				++p;
			}
			if (*p == L'\0' || *p == L';') {
				return TRUE;
			}
		}
		p = StrStr(p, key);
	}
	return FALSE;
}

#define Style_StrGetAttribute(lpszStyle, name)	Style_StrGetAttributeEx((lpszStyle), (name), CSTRLEN(name))
#define Style_StrGetBold(lpszStyle)				Style_StrGetAttribute((lpszStyle), L"bold")
#define Style_StrGetItalic(lpszStyle)			Style_StrGetAttribute((lpszStyle), L"italic")
#define Style_StrGetUnderline(lpszStyle)		Style_StrGetAttribute((lpszStyle), L"underline")
#define Style_StrGetStrike(lpszStyle)			Style_StrGetAttribute((lpszStyle), L"strike")
#define Style_StrGetEOLFilled(lpszStyle)		Style_StrGetAttribute((lpszStyle), L"eolfilled")

// set default colors to avoid showing white (COLOR_WINDOW or COLOR_3DFACE) window or margin while loading big file.
void Style_InitDefaultColor(void) {
	PEDITLEXER pLexNew = pLexArray[iDefaultLexerIndex];
	const int index = pLexNew->bUseDefaultCodeStyle ? GlobalStyleIndex_DefaultCode : GlobalStyleIndex_DefaultText;
	LPCWSTR szValue = pLexGlobal->Styles[index].szValue;
	COLORREF rgb;
	if (!Style_StrGetForeColor(szValue, &rgb)) {
		if (!Style_StrGetForeColor(pLexNew->Styles[0].szValue, &rgb)) {
			rgb = GetSysColor(COLOR_WINDOWTEXT);
		}
	}
	SciCall_StyleSetFore(STYLE_DEFAULT, rgb);
	if (!Style_StrGetBackColor(szValue, &rgb)) {
		if (!Style_StrGetBackColor(pLexNew->Styles[0].szValue, &rgb)) {
			rgb = GetSysColor(COLOR_WINDOW);
		}
	}
	SciCall_StyleSetBack(STYLE_DEFAULT, rgb);
	//SciCall_StyleClearAll();

	SciCall_SetFoldMarginColour(TRUE, rgb);
	SciCall_SetFoldMarginHiColour(TRUE, rgb);

	szValue = pLexGlobal->Styles[GlobalStyleIndex_LineNumber].szValue;
	if (Style_StrGetForeColor(szValue, &rgb)) {
		SciCall_StyleSetFore(STYLE_LINENUMBER, rgb);
	}
	if (Style_StrGetBackColor(szValue, &rgb)) {
		SciCall_StyleSetBack(STYLE_LINENUMBER, rgb);
	}
}

LPCWSTR Style_FindStyleValue(PEDITLEXER pLex, int style) {
	const UINT iStyleCount = pLex->iStyleCount;
	// first style is the default style.
	for (UINT i = 1; i < iStyleCount; i++) {
		int iStyle = pLex->Styles[i].iStyle;
		LPCWSTR szValue = pLex->Styles[i].szValue;
		do {
			if ((iStyle & 0xFF) == style) {
				return szValue;
			}
			iStyle >>= 8;
		} while (iStyle);
	}
	return NULL;
}

void Style_DefineIndicator(int index, int indicator, int indicatorStyle) {
	LPCWSTR szValue = pLexGlobal->Styles[index].szValue;
	COLORREF rgb;
	int iValue;

	SciCall_IndicSetStyle(indicator, indicatorStyle);
	if (Style_StrGetForeColor(szValue, &rgb)) {
		SciCall_IndicSetFore(indicator, rgb);
	}
	if (Style_StrGetAlpha(szValue, &iValue)) {
		SciCall_IndicSetAlpha(indicator, iValue);
	}
	if (Style_StrGetOutlineAlpha(szValue, &iValue)) {
		SciCall_IndicSetOutlineAlpha(indicator, iValue);
	}
}

//=============================================================================
// set current lexer
// Style_SetLexer()
//
void Style_SetLexer(PEDITLEXER pLexNew, BOOL bLexerChanged) {
	int iValue;

	// Select default if NULL is specified
	if (!pLexNew) {
		np2LexLangIndex = 0;
		pLexNew = pLexArray[iDefaultLexerIndex];
	}
	if (!IsStyleLoaded(pLexGlobal)) {
		Style_LoadOne(pLexGlobal);
	}
	if (!IsStyleLoaded(pLexNew)) {
		Style_LoadOne(pLexNew);
	}

	// Lexer
	const int iLexer = pLexNew->iLexer;
	int rid = pLexNew->rid;

	if (bLexerChanged) {
		SciCall_SetLexer(iLexer);

		if (iLexer == SCLEX_CPP || iLexer == SCLEX_MATLAB) {
			if (iLexer == NP2LEX_MATLAB) {
				if (np2LexLangIndex == IDM_LEXER_OCTAVE) {
					rid = NP2LEX_OCTAVE;
				} else if (np2LexLangIndex == IDM_LEXER_SCILAB) {
					rid = NP2LEX_SCILAB;
				}
			}

			char msg[10];
			_itoa(rid - NP2LEX_TEXTFILE, msg, 10);
			SciCall_SetProperty("lexer.lang.type", msg);
		}

		// Code folding
		SciCall_SetProperty("fold", "1");
		SciCall_SetProperty("fold.comment", "1");
		SciCall_SetProperty("fold.preprocessor", "1");
		SciCall_SetProperty("fold.compact", "0");

		switch (rid) {
		case NP2LEX_HTML:
		case NP2LEX_XML:
			SciCall_SetProperty("fold.html", "1");
			SciCall_SetProperty("fold.hypertext.comment", "1");
			SciCall_SetProperty("fold.hypertext.heredoc", "1");
			SciCall_SetProperty("lexer.lang.type", ((np2LexLangIndex == IDM_LEXER_PHP)? "1" : "0"));
			break;

		case NP2LEX_CSS:
			SciCall_SetProperty("lexer.css.scss.language", ((np2LexLangIndex == IDM_LEXER_SCSS)? "1" : "0"));
			SciCall_SetProperty("lexer.css.less.language", ((np2LexLangIndex == IDM_LEXER_LESS)? "1" : "0"));
			SciCall_SetProperty("lexer.css.hss.language", ((np2LexLangIndex == IDM_LEXER_HSS)? "1" : "0"));
			break;

		case NP2LEX_BASH:
			SciCall_SetProperty("lexer.bash.csh.language", ((np2LexLangIndex == IDM_LEXER_CSHELL)? "1" : "0"));
			break;
		}

		Style_UpdateLexerKeywords(pLexNew);
		Style_UpdateLexerKeywordAttr(pLexNew);
		// Add keyword lists
		for (int i = 0; i < NUMKEYWORD; i++) {
			const char *pKeywords = pLexNew->pKeyWords->pszKeyWords[i];
			const uint8_t attr = currentLexKeywordAttr[i];
			if (StrNotEmptyA(pKeywords) && !(attr & KeywordAttr_NoLexer)) {
				SciCall_SetKeywords(i | (attr & KeywordAttr_MakeLower), pKeywords);
			}
		}

		// Clear
		SciCall_ClearDocumentStyle();
	}

	// Font quality setup
	SciCall_SetFontQuality(iFontQuality);

	// Default Values are always set
	SciCall_StyleResetDefault();
	SciCall_StyleSetCharacterSet(STYLE_DEFAULT, DEFAULT_CHARSET);

	//! begin STYLE_DEFAULT
	LPCWSTR szValue = pLexGlobal->Styles[GlobalStyleIndex_DefaultCode].szValue;
	Style_StrGetFontEx(szValue, defaultCodeFontName, COUNTOF(defaultCodeFontName), TRUE);
	szValue = pLexGlobal->Styles[GlobalStyleIndex_DefaultText].szValue;
	Style_StrGetFontEx(szValue, defaultTextFontName, COUNTOF(defaultTextFontName), TRUE);

	iValue = pLexNew->bUseDefaultCodeStyle ? GlobalStyleIndex_DefaultCode : GlobalStyleIndex_DefaultText;
	szValue = pLexGlobal->Styles[iValue].szValue;
	// base font size
	if (!Style_StrGetFontSize(szValue, &iBaseFontSize)) {
		iBaseFontSize = defaultBaseFontSize;
		iValue = DefaultToCurrentDPI(iBaseFontSize);
		SciCall_StyleSetSizeFractional(STYLE_DEFAULT, iValue);
	}
	Style_SetStyles(STYLE_DEFAULT, szValue);

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
		SciCall_SetFontLocale(localeName);
	}

	COLORREF rgb;
	if (!Style_StrGetForeColor(szValue, &rgb)) {
		SciCall_StyleSetFore(STYLE_DEFAULT, GetSysColor(COLOR_WINDOWTEXT));
	}
	if (!Style_StrGetBackColor(szValue, &rgb)) {
		SciCall_StyleSetBack(STYLE_DEFAULT, GetSysColor(COLOR_WINDOW));
	}
	// lexer default (base style), i.e.: EDITSTYLE_DEFAULT
	Style_SetStyles(STYLE_DEFAULT, pLexNew->Styles[0].szValue);
	// set all styles to have the same attributes as STYLE_DEFAULT.
	SciCall_StyleClearAll();
	//! end STYLE_DEFAULT

	Style_SetDefaultStyle(GlobalStyleIndex_LineNumber);
	Style_DefineIndicator(GlobalStyleIndex_MatchBrace, IndicatorNumber_MatchBrace, INDIC_ROUNDBOX);
	Style_DefineIndicator(GlobalStyleIndex_MatchBraceError, IndicatorNumber_MatchBraceError, INDIC_ROUNDBOX);
	Style_SetDefaultStyle(GlobalStyleIndex_IndentationGuide);

	//! begin Selection
	szValue = pLexGlobal->Styles[GlobalStyleIndex_Selection].szValue;
	// never change text color on selecting.
	SciCall_SetSelFore(FALSE, 0);

	// always set background color
	if (!Style_StrGetBackColor(szValue, &rgb)) {
		rgb = GetSysColor(COLOR_HIGHLIGHT);
	}
	SciCall_SetSelBack(TRUE, rgb);
	if (Style_StrGetForeColor(szValue, &rgb)) {
		SciCall_SetAdditionalSelBack(rgb);
	}

	if (!Style_StrGetAlpha(szValue, &iValue)) {
		iValue = SC_ALPHA_NOALPHA;
	}
	SciCall_SetSelAlpha(iValue);
	if (Style_StrGetOutlineAlpha(szValue, &iValue)) {
		SciCall_SetAdditionalSelAlpha(iValue);
	}

	SciCall_SetSelEOLFilled(Style_StrGetEOLFilled(szValue));
	//! end Selection

	//! begin Whitespace
	szValue = pLexGlobal->Styles[GlobalStyleIndex_Whitespace].szValue;
	if (Style_StrGetForeColor(szValue, &rgb)) {
		SciCall_SetWhitespaceFore(TRUE, rgb);
	} else {
		SciCall_SetWhitespaceFore(FALSE, 0);
	}
	if (Style_StrGetBackColor(szValue, &rgb)) {
		SciCall_SetWhitespaceBack(TRUE, rgb);
	} else {
		SciCall_SetWhitespaceBack(FALSE, 0);
	}

	// whitespace dot size
	iValue = 1;
	Style_StrGetRawSize(szValue, &iValue);
	iValue = max_i(1, RoundToCurrentDPI(iValue));
	SciCall_SetWhitespaceSize(iValue);
	//! end Whitespace

	//! begin Caret
	Style_HighlightCurrentLine();
	Style_UpdateCaret();
	const COLORREF backColor = SciCall_StyleGetBack(STYLE_DEFAULT);
	// caret fore
	szValue = pLexGlobal->Styles[GlobalStyleIndex_Caret].szValue;
	if (!Style_StrGetForeColor(szValue, &rgb)) {
		rgb = GetSysColor(COLOR_WINDOWTEXT);
	}
	if (!VerifyContrast(rgb, backColor)) {
		rgb = SciCall_StyleGetFore(STYLE_DEFAULT);
	}
	SciCall_SetCaretFore(rgb);
	// additional caret fore
	if (Style_StrGetBackColor(szValue, &rgb) && VerifyContrast(rgb, backColor)) {
		SciCall_SetAdditionalCaretFore(rgb);
	}
	//! end Caret

	// IME indicator
	szValue = pLexGlobal->Styles[GlobalStyleIndex_IMEIndicator].szValue;
	if (!Style_StrGetForeColor(szValue, &rgb)) {
		rgb = IMEIndicatorDefaultColor;
	}
	SciCall_IndicSetFore(SC_INDICATOR_INPUT, rgb);
	SciCall_IndicSetFore(SC_INDICATOR_TARGET, rgb);
	SciCall_IndicSetFore(SC_INDICATOR_CONVERTED, rgb);
	SciCall_IndicSetFore(SC_INDICATOR_UNKNOWN, rgb);

	Style_SetLongLineColors();

	// Extra Line Spacing
	szValue = (rid != NP2LEX_ANSI)? pLexGlobal->Styles[GlobalStyleIndex_ExtraLineSpacing].szValue
		: pLexNew->Styles[ANSIArtStyleIndex_ExtraLineSpacing].szValue;
	if (Style_StrGetRawSize(szValue, &iValue) && iValue != 0) {
		int iAscent;
		int iDescent;
		if (iValue > 0) {
			// 5 => iAscent = 3, iDescent = 2
			iValue = max_i(0, RoundToCurrentDPI(iValue));
			iDescent = iValue/2 ;
			iAscent = iValue - iDescent;
		} else {
			// -5 => iAscent = -2, iDescent = -3
			iValue = -max_i(0, RoundToCurrentDPI(-iValue));
			iAscent = iValue/2 ;
			iDescent = iValue - iAscent;
		}

		SciCall_SetExtraAscent(iAscent);
		SciCall_SetExtraDescent(iDescent);
	} else {
		SciCall_SetExtraAscent(0);
		SciCall_SetExtraDescent(0);
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

		COLORREF foreColor;
		COLORREF fillColor;
		COLORREF highlightColor;

		szValue = pLexGlobal->Styles[GlobalStyleIndex_FoldingMarker].szValue;
		if (Style_StrGetForeColor(szValue, &rgb)) {
			foreColor = rgb;
		} else {
			foreColor = (bUse2ndGlobalStyle || np2StyleTheme == StyleTheme_Dark) ? FoldingMarkerLineColorDark : FoldingMarkerLineColorDefault;
		}
		if (Style_StrGetBackColor(szValue, &rgb)) {
			fillColor = rgb;
		} else {
			fillColor = (bUse2ndGlobalStyle || np2StyleTheme == StyleTheme_Dark) ? FoldingMarkerFillColorDark : FoldingMarkerFillColorDefault;
		}

		szValue = pLexGlobal->Styles[GlobalStyleIndex_CurrentBlock].szValue;
		if (!Style_StrGetForeColor(szValue, &highlightColor)) {
			highlightColor = RGB(0xFF, 0x00, 0x00); // Scintilla default red color
		}

		SciCall_SetFoldMarginColour(TRUE, backColor);
		SciCall_SetFoldMarginHiColour(TRUE, backColor);
#if 0	// use gray fold color
		COLORREF foreColor = SciCall_StyleGetFore(STYLE_DEFAULT);
		// Marker fore/back colors
		// Set marker color to the average of foreColor and backColor
		foreColor =	(((foreColor & 0xFF0000) + (backColor & 0xFF0000)) >> 1 & 0xFF0000) |
					(((foreColor & 0x00FF00) + (backColor & 0x00FF00)) >> 1 & 0x00FF00) |
					(((foreColor & 0x0000FF) + (backColor & 0x0000FF)) >> 1 & 0x0000FF);

		// Rounding hack for pure white against pure black
		if (foreColor == 0x7F7F7F) {
			foreColor = 0x808080;
		}
#endif

		for (UINT i = 0; i < (UINT)COUNTOF(iMarkerIDs); ++i) {
			const int marker = iMarkerIDs[i];
			SciCall_MarkerSetBack(marker, foreColor);
			SciCall_MarkerSetFore(marker, backColor);
			SciCall_MarkerSetBackSelected(marker, highlightColor);
		}

		SciCall_MarkerSetFore(SC_MARKNUM_FOLDER, fillColor);
		SciCall_MarkerSetFore(SC_MARKNUM_FOLDEREND, fillColor);

		Style_SetDefaultStyle(GlobalStyleIndex_FoldDispalyText);
	} // end set folding style

	// CallTip
	Style_SetDefaultStyle(GlobalStyleIndex_CallTip);

	if (SciCall_GetIndentationGuides() != SC_IV_NONE) {
		Style_SetIndentGuides(TRUE);
	}

	// Mark Occurrences
	szValue = pLexGlobal->Styles[GlobalStyleIndex_MarkOccurrences].szValue;
	SciCall_IndicSetStyle(IndicatorNumber_MarkOccurrence, INDIC_ROUNDBOX);
	if (!Style_StrGetForeColor(szValue, &rgb)) {
		rgb = GetSysColor(COLOR_HIGHLIGHT);
	}
	SciCall_IndicSetFore(IndicatorNumber_MarkOccurrence, rgb);
	if (!Style_StrGetAlpha(szValue, &iValue)) {
		iValue = MarkOccurrencesDefaultAlpha;
	}
	SciCall_IndicSetAlpha(IndicatorNumber_MarkOccurrence, iValue);
	if (Style_StrGetOutlineAlpha(szValue, &iValue)) {
		SciCall_IndicSetOutlineAlpha(IndicatorNumber_MarkOccurrence, iValue);
	}
	//! end Mark Occurrences

	// Bookmark
	bBookmarkColorUpdated = TRUE;
	// SC_MARK_CIRCLE is the default marker type.
	if (SciCall_MarkerSymbolDefined(MarkerNumber_Bookmark) != SC_MARK_CIRCLE) {
		Style_SetBookmark();
	}

	// other lexer styles
	if (rid != NP2LEX_ANSI) {
		Style_SetDefaultStyle(GlobalStyleIndex_ControlCharacter);

		struct DetailStyle style;
		const UINT iStyleCount = pLexNew->iStyleCount;
		// first style is the default style.
		for (UINT i = 1; i < iStyleCount; i++) {
			int iStyle = pLexNew->Styles[i].iStyle;
			szValue = pLexNew->Styles[i].szValue;
			if (iStyle > 0xFF) {
				Style_Parse(&style, szValue);
				do {
					Style_SetParsed(&style, iStyle & 0xFF);
					iStyle >>= 8;
				} while (iStyle);
			} else {
				Style_SetStyles(iStyle, szValue);
			}
		}
		if (iLexer == SCLEX_PERL) {
			szValue = Style_FindStyleValue(pLexNew, SCE_PL_SCALAR);
			if (szValue != NULL) {
				Style_Parse(&style, szValue);
				const int scalar[] = {
					SCE_PL_REGEX_VAR,
					SCE_PL_REGSUBST_VAR,
					SCE_PL_BACKTICKS_VAR,
					SCE_PL_HERE_QQ_VAR,
					SCE_PL_HERE_QX_VAR,
					SCE_PL_STRING_QQ_VAR,
					SCE_PL_STRING_QX_VAR,
					SCE_PL_STRING_QR_VAR,
				};
				for (UINT i = 0; i < COUNTOF(scalar); i++) {
					Style_SetParsed(&style, scalar[i]);
				}
			}
		}
	} else {
		szValue = pLexNew->Styles[ANSIArtStyleIndex_LineNumber].szValue;
		Style_SetStyles(STYLE_LINENUMBER, szValue);
		szValue = pLexNew->Styles[ANSIArtStyleIndex_FoldDispalyText].szValue;
		Style_SetStyles(STYLE_FOLDDISPLAYTEXT, szValue);
	}

	// update style font, color, etc. don't need colorizing (analyzing whole document) again,
	// thus we not call SciCall_ClearDocumentStyle() in previous block.
	if (bLexerChanged) {
#if 0
		// profile lexer performance
		SciCall_ColouriseAll();
#else
		// idle styling
		SciCall_StartStyling(0);
#endif

		// Save current lexer
		pLexCurrent = pLexNew;
		bCurrentLexerHasLineComment = DidLexerHasLineComment(iLexer);
		bCurrentLexerHasBlockComment = DidLexerHasBlockComment(iLexer, rid);
		UpdateStatusBarCache(STATUS_LEXER);
		UpdateStatusbar();
	}

	// font might changed
	UpdateLineNumberWidth();
	UpdateBookmarkMarginWidth();
	UpdateFoldMarginWidth();
}

static inline BOOL IsASpace(int ch) {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

//=============================================================================
// find lexer from script interpreter, which must be first line of the file.
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
		while (*pch && !IsASpace(*pch)) {
			len = *pch == '\\' || *pch == '/';
			pch++;
			name = len ? pch : name;
		}
		if ((pch - name) >= 3 && (_strnicmp(name, "env", 3) == 0 || _strnicmp(name, "winpty", 6) == 0)) {
			while (*pch == ' ' || *pch == '\t') {
				pch++;
			}
			name = pch;
			while (*pch && !IsASpace(*pch)) {
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
				np2LexLangIndex = IDM_LEXER_CSHELL;
				return &lexBash;
			}
			if (!strncmp(name, "perl", 4)) {
				return &lexPerl;
			}
			if (!strncmp(name, "ruby", 4)) {
				return &lexRuby;
			}
			//if (!strncmp(name, "rust", 4)) {
			//	return &lexRust;
			//}
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
				//np2LexLangIndex = IDM_LEXER_PHP;
				return &lexPHP;
			}
			if (!strncmp(name, "tcl", 3)) {
				return &lexTcl;
			}
			if (!strncmp(name, "ash", 3) || !strncmp(name, "zsh", 3) || !strncmp(name, "ksh", 3)) {
				return &lexBash;
			}
			if (!strncmp(name, "csh", 3)) {
				np2LexLangIndex = IDM_LEXER_CSHELL;
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
	char tchText[4096] = ""; // maybe contains header comments
	SciCall_GetText(COUNTOF(tchText), tchText);

	// check DOCTYPE
	const char *p = StrStrIA(tchText, "<!DOCTYPE");
	if (p != NULL) {
		p += 9;
		while (IsASpace(*p)) {
			++p;
		}
		if (!_strnicmp(p, "html", 4)) {
			return IDM_LEXER_WEB;
		}
		if (!strncmp(p, "struts", 6) || !strncmp(p, "xwork", 5) || !strncmp(p, "validators", 10)) {
			return IDM_LEXER_STRUTS;
		}
		if (!strncmp(p, "hibernate", 9)) {
			p += 9;
			if (*p == '-') {
				p++;
			}
			if (*p == 'm') {
				return IDM_LEXER_HIB_MAP;
			}
			return IDM_LEXER_HIB_CFG;
		}
		//if (!strncmp(p, "plist", 5)) {
		//	return IDM_LEXER_PROPERTY_LIST;
		//}
		if (!strncmp(p, "schema", 6)) {
			return IDM_LEXER_XSD;
		}
		if (!strncmp(p, "jboss", 5)) {
			return IDM_LEXER_JBOSS;
		}
		if (!strncmp(p, "beans", 5)) {
			return IDM_LEXER_SPRING_BEANS;
		}
		if (!_strnicmp(p, "module", 6)) {
			return IDM_LEXER_CHECKSTYLE;
		}
	}

	if (strstr(tchText, "<?php")) {
		return IDM_LEXER_PHP;
	}
	// check Language
	if ((p = strstr(tchText, "<%@")) != NULL && (p = StrStrIA(p + 3, "Language")) != NULL) {
		p += 9;
		while (*p == ' ' || *p == '=' || *p == '\"') {
			p++;
		}
		if (!_strnicmp(p, "C#", 2)) {
			return IDM_LEXER_ASPX_CS;
		}
		if (!_strnicmp(p, "VBScript", 7)) {
			return IDM_LEXER_ASP_VBS;
		}
		if (!_strnicmp(p, "VB", 2)) {
			return IDM_LEXER_ASPX_VB;
		}
		if (!_strnicmp(p, "JScript", 7)) {
			return IDM_LEXER_ASP_JS;
		}
		if (!_strnicmp(p, "Java", 4)) {
			return IDM_LEXER_JSP;
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
	//	return IDM_LEXER_WEB;
	if (!strncmp(p, "schema", 6)) {
		return IDM_LEXER_XSD;
	}
	//if (!strncmp(p, "schema", 6) || !strncmp(p, "xsd:schema", 10) || !strncmp(p, "xs:schema", 9))
	//	return IDM_LEXER_XSD;
	//if (!strncmp(p, "xsl:stylesheet", 14))
	//	return IDM_LEXER_XSLT;

	if (!strncmp(p, "project", 7)) {
		return IDM_LEXER_ANT_BUILD;
	}
	//if (!strncmp(p, "project", 7)) {
	//	p += 7;
	//	if (strstr(p, "maven") && strstr(p, "POM"))
	//		return IDM_LEXER_MAVEN_POM;
	//	return IDM_LEXER_ANT_BUILD;
	//}
	if (!strncmp(p, "settings", 8)) {
		return IDM_LEXER_MAVEN_SETTINGS;
	}
	if (!strncmp(p, "ivy", 3)) {
		if (*(p + 3) == '-') {
			return IDM_LEXER_IVY_MODULE;
		}
		return IDM_LEXER_IVY_SETTINGS;
	}
	if (!strncmp(p, "ruleset", 7)) {
		return IDM_LEXER_PMD_RULESET;
	}
	if (!strncmp(p, "module", 6)) {
		return IDM_LEXER_CHECKSTYLE;
	}

	//if (!strncmp(p, "Server"))
	//	return IDM_LEXER_TOMCAT;
	//if (!strncmp(p, "web-app"))
	//	return IDM_LEXER_WEB_JAVA;
	if (!strncmp(p, "struts", 6) || !strncmp(p, "xwork", 5) || !strncmp(p, "validators", 10)) {
		return IDM_LEXER_STRUTS;
	}
	if (!strncmp(p, "hibernate", 9)) {
		if (*(p + 10) == 'm') {
			return IDM_LEXER_HIB_MAP;
		}
		return IDM_LEXER_HIB_CFG;
	}
	if (!strncmp(p, "jboss", 5)) {
		return IDM_LEXER_JBOSS;
	}
	if (!strncmp(p, "beans", 5)) {
		return IDM_LEXER_SPRING_BEANS;
	}

	//if (!strncmp(p, "configuration", 10))
	//	return IDM_LEXER_WEB_NET;
	//if (!strncmp(p, "root", 4))
	//	return IDM_LEXER_RESX;
	//if (!strncmp(p, "Canvas", 6))
	//	return IDM_LEXER_XAML;

	//if (!strncmp(p, "plist", 5))
	//	return IDM_LEXER_PROPERTY_LIST;
	//if (!strncmp(p, "manifest", 8))
	//	return IDM_LEXER_ANDROID_MANIFEST;
	//if (!strncmp(p, "svg", 3))
	//	return IDM_LEXER_SVG;
	const char * const pb = p;
	if (((p = strstr(pb, "Layout")) != NULL && strstr(p + 6, "xmlns:android")) ||
			((p = strstr(pb, "View")) != NULL && strstr(p + 4, "xmlns:android")) ||
			((p = strstr(pb, "menu")) != NULL && strstr(p + 4, "xmlns:android"))) {
		return IDM_LEXER_ANDROID_LAYOUT;
	}

	return 0;
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
	SciCall_GetText(COUNTOF(tchText), tchText);

	const char *p = tchText;
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
				np2LexLangIndex = IDM_LEXER_OCTAVE;
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

// auto detect file type from content.
PEDITLEXER Style_AutoDetect(BOOL bDotFile) {
	char tchText[4096] = ""; // maybe contains header comments
	SciCall_GetText(COUNTOF(tchText), tchText);

	const char *p = tchText;
	const BOOL shebang = *p == '#' && p[1] == '!';
	int cppCount = 0;
	int sharpCount = 0;
	BOOL maybeIni = FALSE;
	BOOL maybeJson = FALSE;
	BOOL notJson = FALSE;

	while (*p) {
		if (*p == '[') {
			// bracket at line beginning
			maybeIni = TRUE;
		} else if (*p == '-' && p[1] == '-' && p[2] == '-' && IsASpace(p[3])) {
			// `---` at line beginning
			p += 3;
			while (*p == ' ' || *p == '\t') {
				++p;
			}
			if (IsASpace(*p) || *p == '#' || *p == '%') {
				return &lexYAML;
			}
		} else {
			// skip leading space
			while (*p == ' ' || *p == '\t') {
				++p;
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
					++cppCount;
					if (!sharpCount) {
						return &lexCPP;
					}
				}
				break;

			case '{':
			case '}':
			case ']':
				maybeJson |= TRUE;
				break;
			case '\"':
				maybeJson |= maybeIni;
				break;

			default:
				// not a normal JSON file: line not starts with any of `{}[]"`, possible JSON with unquoted keys, which is rare.
				notJson |= *p;
				break;
			}
		}

		// skip to next line
		while (*p && !(*p == '\r' || *p == '\n')) {
			++p;
		}
		while (*p == '\r' || *p == '\n') {
			++p;
		}
	}

	if (cppCount > sharpCount && !(shebang || bDotFile)) {
		return &lexCPP;
	}
	if (sharpCount) {
		return shebang ? &lexBash : &lexCONF;
	}
	if (maybeJson && !notJson) {
		return &lexJSON;
	}
	if (maybeIni) {
		return &lexINI;
	}
	if (bDotFile) {
		return &lexCONF;
	}
	if (maybeJson) {
		// for braces and brackets
		return &lexCPP;
	}
	return NULL;
}

//=============================================================================
//
// Style_GetCurrentLexerName()
//
LPCWSTR Style_GetCurrentLexerDisplayName(LPWSTR lpszName, int cchName) {
	if (GetString(pLexCurrent->rid, lpszName, cchName)) {
		return lpszName;
	}
	return Style_GetCurrentLexerName();
}

LPCWSTR Style_GetCurrentLexerName(void) {
	if (np2LexLangIndex == 0) {
		return pLexCurrent->pszName;
	}
	switch (np2LexLangIndex) {
	case IDM_LEXER_TEXTFILE:
	case IDM_LEXER_2NDTEXTFILE:
		return pLexCurrent->pszName;
	case IDM_LEXER_WEB:
		return L"Web Source Code";
	case IDM_LEXER_PHP:
		return L"PHP Page";
	case IDM_LEXER_JSP:
		return L"JSP Page";
	case IDM_LEXER_ASPX_CS:
		return L"ASP.NET (C#)";
	case IDM_LEXER_ASPX_VB:
		return L"ASP.NET (VB.NET)";
	case IDM_LEXER_ASP_VBS:
		return L"ASP (VBScript)";
	case IDM_LEXER_ASP_JS:
		return L"ASP (JScript)";

	case IDM_LEXER_XML:
		return L"XML Document";
	case IDM_LEXER_XSD:
		return L"XML Schema";
	case IDM_LEXER_XSLT:
		return L"XSLT Stylesheet";
	case IDM_LEXER_DTD:
		return L"XML DTD";

	case IDM_LEXER_ANT_BUILD:
		return L"Ant Build";
	case IDM_LEXER_MAVEN_POM:
		return L"Maven POM";
	case IDM_LEXER_MAVEN_SETTINGS:
		return L"Maven Settings";
	case IDM_LEXER_IVY_MODULE:
		return L"Ivy Module";
	case IDM_LEXER_IVY_SETTINGS:
		return L"Ivy Settings";
	case IDM_LEXER_PMD_RULESET:
		return L"PMD Ruleset";
	case IDM_LEXER_CHECKSTYLE:
		return L"CheckStyle";

	case IDM_LEXER_APACHE:
		return L"Apache Config";
	case IDM_LEXER_TOMCAT:
		return L"Tomcat Config";
	case IDM_LEXER_WEB_JAVA:
		return L"Java Web Config";
	case IDM_LEXER_STRUTS:
		return L"Struts Config";
	case IDM_LEXER_HIB_CFG:
		return L"Hibernate Config";
	case IDM_LEXER_HIB_MAP:
		return L"Hibernate Mapping";
	case IDM_LEXER_SPRING_BEANS:
		return L"Spring Beans";
	case IDM_LEXER_JBOSS:
		return L"JBoss Config";

	case IDM_LEXER_WEB_NET:
		return L"ASP.NET Web Config";
	case IDM_LEXER_RESX:
		return L"ResX Schema";
	case IDM_LEXER_XAML:
		return L"WPF XAML";

	case IDM_LEXER_PROPERTY_LIST:
		return L"Property List";
	case IDM_LEXER_ANDROID_MANIFEST:
		return L"Android Manifest";
	case IDM_LEXER_ANDROID_LAYOUT:
		return L"Android Layout";
	case IDM_LEXER_SVG:
		return L"SVG Document";

	case IDM_LEXER_BASH:
		return L"Shell Script";
	case IDM_LEXER_CSHELL:
		return L"C Shell";
	case IDM_LEXER_M4:
		return L"M4 Macro";

	case IDM_LEXER_MATLAB:
		return L"MATLAB Code";
	case IDM_LEXER_OCTAVE:
		return L"Octave Code";
	case IDM_LEXER_SCILAB:
		return L"Scilab Code";

	case IDM_LEXER_CSS:
		return L"CSS Style Sheet";
	case IDM_LEXER_SCSS:
		return L"Sassy CSS";
	case IDM_LEXER_LESS:
		return L"Less CSS";
	case IDM_LEXER_HSS:
		return L"HSS";

	default:
		return L"Error";
	}
}

static void Style_UpdateLexerLang(PEDITLEXER pLex, LPCWSTR lpszExt, LPCWSTR lpszName) {
	switch (pLex->rid) {
	case NP2LEX_BASH:
		if (StrCaseEqual(L"m4", lpszExt) || StrCaseEqual(L"ac", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_M4;
		} else if (StrCaseEqual(L"csh", lpszExt) || StrCaseEqual(L"tcsh", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_CSHELL;
		}
		break;

	case NP2LEX_CONF:
		if (StrNCaseEqual(lpszName, L"httpd", 5) || StrCaseEqual(lpszExt, L"htaccess")) {
			np2LexLangIndex = IDM_LEXER_APACHE;
		}
		break;

	case NP2LEX_CSS:
		if (StrCaseEqual(L"scss", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_SCSS;
		} else if (StrCaseEqual(L"less", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_LESS;
		} else if (StrCaseEqual(L"hss", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_HSS;
		}
		break;

	case NP2LEX_HTML:
		if (StrCaseEqual(L"jsp", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_JSP;
		} else if (StrCaseEqual(L"aspx", lpszExt)) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
			if (np2LexLangIndex == 0) {
				np2LexLangIndex = IDM_LEXER_ASPX_CS;
			}
		} else if (StrCaseEqual(L"asp", lpszExt)) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
			if (np2LexLangIndex == 0) {
				np2LexLangIndex = IDM_LEXER_ASP_VBS;
			}
		}
		break;

	case NP2LEX_MATLAB:
		if (StrCaseEqual(L"sce", lpszExt) || StrCaseEqual(L"sci", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_SCILAB;
		}
		break;

	case NP2LEX_XML:
		if (StrCaseEqual(L"xml", lpszExt)) {
			if (StrCaseEqual(lpszName, L"build.xml") || StrCaseEqual(lpszName, L"javadoc.xml")) {
				np2LexLangIndex = IDM_LEXER_ANT_BUILD;
			} else if (StrCaseEqual(lpszName, L"pom.xml")) {
				np2LexLangIndex = IDM_LEXER_MAVEN_POM;
			} else if (StrCaseEqual(lpszName, L"settings.xml")) {
				np2LexLangIndex = IDM_LEXER_MAVEN_SETTINGS;
			} else if (StrCaseEqual(lpszName, L"AndroidManifest.xml")) {
				np2LexLangIndex = IDM_LEXER_ANDROID_MANIFEST;
			} else if (StrCaseEqual(lpszName, L"server.xml")) {
				np2LexLangIndex = IDM_LEXER_TOMCAT;
			} else if (StrCaseEqual(lpszName, L"web.xml")) {
				np2LexLangIndex = IDM_LEXER_WEB_JAVA;
			} else if (StrCaseEqual(lpszName, L"struts.xml") || StrCaseEqual(lpszName, L"struts-config.xml")) {
				np2LexLangIndex = IDM_LEXER_STRUTS;
			} else if (StrCaseEqual(lpszName, L"hibernate.cfg.xml")) {
				np2LexLangIndex = IDM_LEXER_HIB_CFG;
			} else if (StrCaseEqual(lpszName, L"ivy.xml")) {
				np2LexLangIndex = IDM_LEXER_IVY_MODULE;
			} else if (StrCaseEqual(lpszName, L"ivysettings.xml")) {
				np2LexLangIndex = IDM_LEXER_IVY_SETTINGS;
			} else if (StrCaseEqual(lpszName, L"pmd.xml")) {
				np2LexLangIndex = IDM_LEXER_PMD_RULESET;
			} else {
				np2LexLangIndex = Style_GetDocTypeLanguage();
			}
		} else if (StrCaseEqual(L"xsd", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_XSD;
		} else if (StrNCaseEqual(L"xsl", lpszExt, 3)) {
			np2LexLangIndex = IDM_LEXER_XSLT;
		} else if (StrCaseEqual(L"dtd", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_DTD;
		} else if (StrCaseEqual(L"pom", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_MAVEN_POM;
		} else if (StrCaseEqual(L"resx", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_RESX;
		} else if (StrCaseEqual(L"xaml", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_XAML;
		} else if (StrCaseEqual(L"plist", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_PROPERTY_LIST;
		} else if (StrCaseEqual(L"svg", lpszExt)) {
			np2LexLangIndex = IDM_LEXER_SVG;
		}
		break;
	}
}

//=============================================================================
// find lexer from file extension
// Style_MatchLexer()
//
PEDITLEXER Style_MatchLexer(LPCWSTR lpszMatch, BOOL bCheckNames) {
	if (!bCheckNames) {
		if (bAutoSelect && StrCaseEqual(L"m", lpszMatch)) {
			PEDITLEXER lex = Style_DetectObjCAndMatlab();
			if (lex != NULL) {
				return lex;
			}
		}

		const int cch = lstrlen(lpszMatch);
		for (UINT iLexer = LEXER_INDEX_MATCH; iLexer < ALL_LEXER_COUNT; iLexer++) {
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
			for (UINT iLexer = LEXER_INDEX_MATCH; iLexer < ALL_LEXER_COUNT; iLexer++) {
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

static PEDITLEXER Style_GetLexerFromFile(LPCWSTR lpszFile, BOOL bCGIGuess, LPCWSTR *pszExt, BOOL *pDotFile) {
	LPCWSTR lpszExt = PathFindExtension(lpszFile);
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
			SciCall_GetText(COUNTOF(tchText), tchText);
			pLexNew = Style_SniffShebang(tchText);
			bFound = pLexNew != NULL;
		}

		// autoconf / automake
		if (!bFound && StrCaseEqual(lpszExt, L"in") && pDotFile != NULL) {
			WCHAR tchCopy[MAX_PATH];
			lstrcpyn(tchCopy, lpszFile, COUNTOF(tchCopy));
			PathRemoveExtension(tchCopy);
			pLexNew = Style_GetLexerFromFile(tchCopy, FALSE, NULL, NULL);
			bFound = pLexNew != NULL;
		}

		// MySQL ini/cnf
		if (!bFound && StrNCaseEqual(lpszName, L"my", 2) && (StrCaseEqual(lpszExt, L"ini") || StrCaseEqual(lpszExt, L"cnf"))) {
			pLexNew = &lexCONF;
			bFound = TRUE;
		}
		if (!bFound && StrCaseEqual(lpszName, L"web.config")) {
			pLexNew = &lexXML;
			bFound = TRUE;
			np2LexLangIndex = IDM_LEXER_WEB_NET;
		}

		// check associated extensions
		if (!bFound) {
			pLexNew = Style_MatchLexer(lpszExt, FALSE);
			bFound = pLexNew != NULL;
		}
		if (bFound) {
			Style_UpdateLexerLang(pLexNew, lpszExt, lpszName);
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

BOOL Style_SetLexerFromFile(LPCWSTR lpszFile) {
	BOOL bFound = TRUE;
	BOOL bDotFile = FALSE;
	LPCWSTR lpszExt = NULL;
	PEDITLEXER pLexNew = NULL;
	PEDITLEXER pLexSniffed;

	if (bAutoSelect) {
		pLexNew = Style_GetLexerFromFile(lpszFile, !fNoCGIGuess, &lpszExt, &bDotFile);
	}
	if (pLexNew == NULL) {
		bFound = FALSE;
		pLexNew = pLexArray[iDefaultLexerIndex];
	}

	// xml/html
	if ((!bFound && bAutoSelect) || (bFound && (pLexNew->rid == NP2LEX_PHP || pLexNew->rid == NP2LEX_CONF))) {
		char tchText[256] = "";
		SciCall_GetText(COUNTOF(tchText), tchText);
		const char *p = tchText;
		while (IsASpace(*p)) {
			++p;
		}
		const BOOL bPHP = strncmp(p, "<?php", 5) == 0;
		if ((pLexNew->rid == NP2LEX_PHP) ^ bPHP) {
			pLexNew = &lexHTML;
			np2LexLangIndex = IDM_LEXER_PHP;
			bFound = TRUE;
		} else if (*p == '<') {
			if (strncmp(p, "<?xml", 5) == 0) {
				// some conf/cfg file is xml
				pLexNew = &lexXML;
			} else if (!bFound) {
				if (_strnicmp(p, "<!DOCTYPE", 9) == 0) {
					p += 9;
					while (IsASpace(*p)) {
						++p;
					}
				}
				if (_strnicmp(p, "<html", 5) == 0) {
					pLexNew = &lexHTML;
				} else if (!fNoHTMLGuess) {
					if (StrStrIA(p, "<html")) {
						pLexNew = &lexHTML;
					} else {
						pLexNew = &lexXML;
					}
				}
			}
			if (pLexNew->rid == NP2LEX_HTML || pLexNew->rid == NP2LEX_XML) {
				bFound = TRUE;
				np2LexLangIndex = Style_GetDocTypeLanguage();
				if (pLexNew->rid == NP2LEX_XML && np2LexLangIndex == IDM_LEXER_WEB) {
					// xhtml: <?xml version="1.0" encoding="UTF-8"?><!DOCTYPE html>
					pLexNew = &lexHTML;
				}
			}
		} else if ((p == tchText) && !fNoCGIGuess && (pLexSniffed = Style_SniffShebang(tchText)) != NULL) {
			pLexNew = pLexSniffed;
			bFound = TRUE;
		}
	}

	// file mode
	if (!bFound && (fvCurFile.mask & FV_MODE) && fvCurFile.tchMode[0]) {
		WCHAR wchMode[32];
		const UINT cpEdit = SciCall_GetCodePage();
		MultiByteToWideChar(cpEdit, 0, fvCurFile.tchMode, -1, wchMode, COUNTOF(wchMode));

		if (!fNoCGIGuess && (StrCaseEqual(wchMode, L"cgi") || StrCaseEqual(wchMode, L"fcgi"))) {
			char tchText[256] = "";
			SciCall_GetText(COUNTOF(tchText), tchText);
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
			if ((pLexMode = Style_MatchLexer(wchMode, FALSE)) != NULL ||
				(pLexMode = Style_MatchLexer(wchMode, TRUE)) != NULL) {
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
			if ((pLexSniffed = Style_AutoDetect(bDotFile)) != NULL) {
				pLexNew = pLexSniffed;
				bFound = TRUE;
			}
		} else {
			pLexNew = &lexCONF;
			bFound = TRUE;
		}
	}

	// Apply the new lexer
	Style_SetLexer(pLexNew, TRUE);
	return bFound;
}

//=============================================================================
// find lexer from file name or extension
// Style_SetLexerFromName()
//
void Style_SetLexerFromName(LPCWSTR lpszFile, LPCWSTR lpszName) {
	PEDITLEXER pLexNew;
	if ((pLexNew = Style_MatchLexer(lpszName, FALSE)) != NULL ||
		(pLexNew = Style_MatchLexer(lpszName, TRUE)) != NULL) {
		Style_SetLexer(pLexNew, TRUE);
	} else {
		Style_SetLexerFromFile(lpszFile);
	}
}

BOOL Style_CanOpenFile(LPCWSTR lpszFile) {
	const int lang = np2LexLangIndex;
	BOOL bDotFile = FALSE;
	LPCWSTR lpszExt = NULL;
	const LPCEDITLEXER pLexNew = Style_GetLexerFromFile(lpszFile, FALSE, &lpszExt, &bDotFile);
	np2LexLangIndex = lang;
	return pLexNew != NULL || StrIsEmpty(lpszExt) || bDotFile || StrCaseEqual(lpszExt, L"cgi") || StrCaseEqual(lpszExt, L"fcgi");
}

BOOL Style_MaybeBinaryFile(LPCWSTR lpszFile) {
#if 1
	UNREFERENCED_PARAMETER(lpszFile);
	/* Test C0 Control Character
	These character is not reused in most text encoding, and doesn't appears in normal text file.
	Most binary file has reserved fields (most are NULL) or small values in it's header.
	Treat the file as binary when we find two adjacent C0 control characters
	(very common in file header) or some (currently set to 8) C0 control characters. */

	// see tools/GenerateTable.py for this mask.
	const UINT C0Mask = 0x0FFFC1FFU;
	const Sci_Position headerLen = min_pos(1023, SciCall_GetLength() - 1);
	const uint8_t *ptr = (const uint8_t *)SciCall_GetRangePointer(0, headerLen + 1);
	if (ptr == NULL || headerLen <= 0) {
		return FALSE; // empty file
	}

	const uint8_t * const end  = ptr + headerLen;
	UINT count = 0;
	while (ptr < end) {
		uint8_t ch = *ptr++;
		if (ch < 32 && (C0Mask & (1U << ch))) {
			++count;
			ch = *ptr++;
			if ((count >= 8) || (ch < 32 && (C0Mask & (1U << ch)))) {
				return TRUE;
			}
		}
	}
#else
	uint8_t buf[5] = {0}; // file magic
	SciCall_GetText(COUNTOF(buf), buf);
	const UINT magic2 = (buf[0] << 8) | buf[1];
	if (magic2 == 0x4D5AU ||	// PE (exe, dll, etc.): MZ
		magic2 == 0x504BU ||	// ZIP (zip, jar, docx, apk, etc.): PK
		magic2 == 0x377AU ||	// 7z: 7z
		magic2 == 0x1F8BU ||	// gzip (gz, gzip, svgz, etc.)
		magic2 == 0x424DU ||	// BMP: BM
		magic2 == 0xFFD8U		// JPEG (jpg, jpeg, etc.)
		) {
		return TRUE;
	}

	const UINT magic = (magic2 << 16) | (buf[2] << 8) | buf[3];
	if (magic == 0x89504E47U ||	// PNG: 0x89+PNG
		magic == 0x47494638U ||	// GIF: GIF89a
		magic == 0x25504446U ||	// PDF: %PDF-{version}
		magic == 0x52617221U ||	// RAR: Rar!
		magic == 0x7F454C46U ||	// ELF: 0x7F+ELF
		magic == 0x213C6172U ||	// .lib, .a: !<arch>\n
		magic == 0xFD377A58U ||	// xz: 0xFD+7zXZ
		magic == 0xCAFEBABEU	// Java class
		) {
		return TRUE;
	}

	LPCWSTR lpszExt = PathFindExtension(lpszFile);
	if (StrNotEmpty(lpszExt)) {
		++lpszExt;
		const int len = lstrlen(lpszExt);
		if (len < 3 || len > 5) {
			return FALSE;
		}
		// full match
		WCHAR wch[8] = L"";
		wch[0] = L' ';
		lstrcpy(wch + 1, lpszExt);
		wch[len + 1] = L' ';
		LPCWSTR lpszMatch = StrStrI(
			L" cur"		// Cursor
			L" ico"		// Icon

			L" obj"
			L" pdb"
			L" bin"
			L" pak"		// Chrome

			L" dmg"		// macOS
			L" img"
			L" iso"
			L" tar"
			L" ", wch);
		return lpszMatch != NULL;
	}
#endif
	return FALSE;
}

void Style_SetLexerByLangIndex(int lang) {
	const int langIndex = np2LexLangIndex;
	PEDITLEXER pLex = NULL;
	np2LexLangIndex = lang;

	switch (lang) {
	case IDM_LEXER_TEXTFILE:
		np2LexLangIndex = 0;
		pLex = &lexTextFile;
		break;

	case IDM_LEXER_2NDTEXTFILE:
		np2LexLangIndex = 0;
		pLex = &lex2ndTextFile;
		break;

	case IDM_LEXER_APACHE:
		pLex = &lexCONF;
		break;

	case IDM_LEXER_WEB:
	case IDM_LEXER_PHP:
	case IDM_LEXER_JSP:
	case IDM_LEXER_ASPX_CS:
	case IDM_LEXER_ASPX_VB:
	case IDM_LEXER_ASP_VBS:
	case IDM_LEXER_ASP_JS:
		if (lang == IDM_LEXER_WEB) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
		}
		pLex = &lexHTML;
		break;

	case IDM_LEXER_XML:
	case IDM_LEXER_XSD:
	case IDM_LEXER_XSLT:
	case IDM_LEXER_DTD:

	case IDM_LEXER_ANT_BUILD:
	case IDM_LEXER_MAVEN_POM:
	case IDM_LEXER_MAVEN_SETTINGS:
	case IDM_LEXER_IVY_MODULE:
	case IDM_LEXER_IVY_SETTINGS:
	case IDM_LEXER_PMD_RULESET:
	case IDM_LEXER_CHECKSTYLE:

	case IDM_LEXER_TOMCAT:
	case IDM_LEXER_WEB_JAVA:
	case IDM_LEXER_STRUTS:
	case IDM_LEXER_HIB_CFG:
	case IDM_LEXER_HIB_MAP:
	case IDM_LEXER_SPRING_BEANS:
	case IDM_LEXER_JBOSS:

	case IDM_LEXER_WEB_NET:
	case IDM_LEXER_RESX:
	case IDM_LEXER_XAML:

	case IDM_LEXER_PROPERTY_LIST:
	case IDM_LEXER_ANDROID_MANIFEST:
	case IDM_LEXER_ANDROID_LAYOUT:
	case IDM_LEXER_SVG:
		if (lang == IDM_LEXER_XML) {
			np2LexLangIndex = Style_GetDocTypeLanguage();
		}
		pLex = &lexXML;
		break;

	case IDM_LEXER_BASH:
	case IDM_LEXER_CSHELL:
	case IDM_LEXER_M4:
		pLex = &lexBash;
		break;

	case IDM_LEXER_MATLAB:
	case IDM_LEXER_OCTAVE:
	case IDM_LEXER_SCILAB:
		pLex = &lexMatlab;
		break;

	case IDM_LEXER_CSS:
	case IDM_LEXER_SCSS:
	case IDM_LEXER_LESS:
	case IDM_LEXER_HSS:
		pLex = &lexCSS;
		break;
	}
	if (pLex != NULL) {
		const BOOL bLexerChanged = pLex != pLexCurrent || langIndex != np2LexLangIndex;
		if (bLexerChanged) {
			Style_SetLexer(pLex, TRUE);
		}
	}
}

void Style_UpdateSchemeMenu(HMENU hmenu) {
	int lang = np2LexLangIndex;
	if (lang == 0) {
		switch (pLexCurrent->rid) {
		case NP2LEX_TEXTFILE:
			lang = IDM_LEXER_TEXTFILE;
			break;
		case NP2LEX_2NDTEXTFILE:
			lang = IDM_LEXER_2NDTEXTFILE;
			break;
		case NP2LEX_HTML:
			lang = IDM_LEXER_WEB;
			break;
		case NP2LEX_XML:
			lang = IDM_LEXER_XML;
			break;
		case NP2LEX_BASH:
			lang = IDM_LEXER_BASH;
			break;
		case NP2LEX_MATLAB:
			lang = IDM_LEXER_MATLAB;
			break;
		case NP2LEX_OCTAVE:
			lang = IDM_LEXER_OCTAVE;
			break;
		case NP2LEX_SCILAB:
			lang = IDM_LEXER_SCILAB;
			break;
		case NP2LEX_CSS:
			lang = IDM_LEXER_CSS;
			break;
		}
		np2LexLangIndex = lang;
	}
	if (lang == IDM_LEXER_TEXTFILE || lang == NP2LEX_2NDTEXTFILE) {
		np2LexLangIndex = 0;
	}
	for (int i = IDM_LEXER_TEXTFILE; i < IDM_LEXER_LAST_LEXER; i++) {
		CheckCmd(hmenu, i, FALSE);
	}
	if (lang >= IDM_LEXER_TEXTFILE) {
		CheckCmd(hmenu, lang, TRUE);
	}
}

//=============================================================================
//
// Style_SetLexerFromID()
//
void Style_SetLexerFromID(int rid) {
	const int iLexer = Style_GetMatchLexerIndex(rid);
	np2LexLangIndex = Style_GetDocTypeLanguage();
	Style_SetLexer(pLexArray[iLexer], TRUE);
}

int Style_GetMatchLexerIndex(int rid) {
	for (int iLexer = LEXER_INDEX_MATCH; iLexer < ALL_LEXER_COUNT; iLexer++) {
		if (pLexArray[iLexer]->rid == rid) {
			return iLexer;
		}
	}
	return LEXER_INDEX_MATCH; // Text File
}

//=============================================================================
//
// Style_ToggleUse2ndDefault()
//
void Style_ToggleUse2ndGlobalStyle(void) {
	bUse2ndGlobalStyle = !bUse2ndGlobalStyle;
	pLexGlobal = bUse2ndGlobalStyle ? &lex2ndGlobal : &lexGlobal;
	Style_SetLexer(pLexCurrent, FALSE);
}

void Style_ToggleUseDefaultCodeStyle(void) {
	pLexCurrent->bStyleChanged = TRUE;
	pLexCurrent->bUseDefaultCodeStyle = !pLexCurrent->bUseDefaultCodeStyle;
	fStylesModified |= STYLESMODIFIED_SOME_STYLE;
	Style_SetLexer(pLexCurrent, FALSE);
}

//=============================================================================
//
// Style_SetLongLineColors()
//
void Style_SetLongLineColors(void) {
	LPCWSTR szValue = pLexGlobal->Styles[GlobalStyleIndex_LongLineMarker].szValue;

	const BOOL foreColor = SciCall_GetEdgeMode() == EDGE_LINE;
	COLORREF rgb;
	if (!Style_StrGetColor(foreColor, szValue, &rgb)) {
		rgb = GetSysColor(COLOR_3DLIGHT);
	}

	SciCall_SetEdgeColour(rgb);
}

//=============================================================================
//
// Style_HighlightCurrentLine()
//
void Style_HighlightCurrentLine(void) {
	SciCall_SetCaretLineVisible(FALSE);
	if (iHighlightCurrentLine != 0) {
		LPCWSTR szValue = pLexGlobal->Styles[GlobalStyleIndex_CurrentLine].szValue;
		// 1: background color, 2: outline frame
		const BOOL outline = iHighlightCurrentLine != 1;
		COLORREF rgb;
		if (Style_StrGetColor(outline, szValue, &rgb)) {
			int size = 0;
			if (outline) {
				Style_StrGetRawSize(szValue, &size);
				size = max_i(1, RoundToCurrentDPI(size));
			}

			SciCall_SetCaretLineFrame(size);
			SciCall_SetCaretLineBack(rgb);

			int alpha;
			if (!Style_StrGetAlphaEx(outline, szValue, &alpha)) {
				alpha = SC_ALPHA_NOALPHA;
			}
			SciCall_SetCaretLineBackAlpha(alpha);
			SciCall_SetCaretLineVisible(TRUE);
		}
	}
}

//=============================================================================
//
// Style_SetIndentGuides()
//
extern int flagSimpleIndentGuides;

void Style_SetIndentGuides(BOOL bShow) {
	int iIndentView = SC_IV_NONE;
	if (bShow) {
		if (!flagSimpleIndentGuides) {
			if (IsPythonLikeFolding(pLexCurrent->iLexer)) {
				iIndentView = SC_IV_LOOKFORWARD;
			} else {
				iIndentView = SC_IV_LOOKBOTH;
			}
		} else {
			iIndentView = SC_IV_REAL;
		}
	}
	SciCall_SetIndentationGuides(iIndentView);
}

void Style_SetBookmark(void) {
	if (!bBookmarkColorUpdated) {
#if BookmarkUsingPixmapImage
		const int marker = bShowBookmarkMargin ? SC_MARK_PIXMAP : SC_MARK_BACKGROUND;
#else
		const int marker = bShowBookmarkMargin ? SC_MARK_VERTICALBOOKMARK : SC_MARK_BACKGROUND;
#endif
		const int markType = SciCall_MarkerSymbolDefined(MarkerNumber_Bookmark);
		if (marker == markType) {
			return;
		}
	}

	LPCWSTR szValue = pLexGlobal->Styles[GlobalStyleIndex_Bookmark].szValue;
	if (bShowBookmarkMargin) {
		COLORREF iBookmarkImageColor;
		if (!Style_StrGetForeColor(szValue, &iBookmarkImageColor)) {
			iBookmarkImageColor = BookmarkImageDefaultColor;
		}
#if BookmarkUsingPixmapImage
		sprintf(bookmark_pixmap_color, bookmark_pixmap_color_fmt, iBookmarkImageColor);
		SciCall_MarkerDefinePixmap(MarkerNumber_Bookmark, bookmark_pixmap);
#else
		SciCall_MarkerSetBack(MarkerNumber_Bookmark, iBookmarkImageColor);
		// set same color to avoid showing edge.
		SciCall_MarkerSetFore(MarkerNumber_Bookmark, iBookmarkImageColor);
		SciCall_MarkerDefine(MarkerNumber_Bookmark, SC_MARK_VERTICALBOOKMARK);
#endif
	} else {
		COLORREF iBookmarkLineColor;
		int iBookmarkLineAlpha;
		if (!Style_StrGetBackColor(szValue, &iBookmarkLineColor)) {
			iBookmarkLineColor = BookmarkLineDefaultColor;
		}
		if (!Style_StrGetAlpha(szValue, &iBookmarkLineAlpha)) {
			iBookmarkLineAlpha = BookmarkLineDefaultAlpha;
		}
		SciCall_MarkerSetBack(MarkerNumber_Bookmark, iBookmarkLineColor);
		SciCall_MarkerSetAlpha(MarkerNumber_Bookmark, iBookmarkLineAlpha);
		SciCall_MarkerDefine(MarkerNumber_Bookmark, SC_MARK_BACKGROUND);
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
	LPWSTR p = StrStr(lpszStyle, L"font:");

	if (p != NULL) {
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
BOOL Style_StrGetCharSet(LPCWSTR lpszStyle, int *charset) {
	LPCWSTR p = StrStr(lpszStyle, L"charset:");

	if (p != NULL) {
		p += CSTRLEN(L"charset:");
		return CRTStrToInt(p, charset);
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetSize()
//
BOOL Style_StrGetFontSize(LPCWSTR lpszStyle, int *size) {
	LPCWSTR p = StrStr(lpszStyle, L"size:");

	if (p != NULL) {
		p += CSTRLEN(L"size:");
		float value;
		if (StrToFloat(p, &value)) {
			int iValue = (int)lroundf(value * SC_FONT_SIZE_MULTIPLIER);
			iValue += (*p == L'+' || *p == '-')? iBaseFontSize : 0;
			// scintilla/src/ViewStyle.h GetFontSizeZoomed()
			iValue = max_i(iValue, 2 * SC_FONT_SIZE_MULTIPLIER);
			*size = iValue;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Style_StrGetRawSize(LPCWSTR lpszStyle, int *size) {
	LPCWSTR p = StrStr(lpszStyle, L"size:");

	if (p != NULL) {
		p += CSTRLEN(L"size:");
		return CRTStrToInt(p, size);
	}
	return FALSE;
}

// https://docs.microsoft.com/en-us/windows/desktop/api/wingdi/ns-wingdi-taglogfontw
// https://docs.microsoft.com/en-us/windows/desktop/api/dwrite/ne-dwrite-dwrite_font_weight
#define MIN_FONT_WEIGHT		0
#define MAX_FONT_WEIGHT		1000
BOOL Style_StrGetFontWeight(LPCWSTR lpszStyle, int *weight) {
	if (Style_StrGetBold(lpszStyle)) {
		*weight = FW_BOLD;
		return TRUE;
	}

	LPCWSTR p = StrStr(lpszStyle, L"weight:");
	if (p != NULL) {
		p += CSTRLEN(L"weight:");
		return CRTStrToInt(p, weight) && (*weight > MIN_FONT_WEIGHT && *weight < MAX_FONT_WEIGHT);
	}

	return FALSE;
}

//=============================================================================
//
// Style_StrGetValueEx()
//
static BOOL Style_StrGetValueEx(LPCWSTR lpszStyle, LPCWSTR key, int keyLen, LPWSTR lpszValue, int cchValue) {
	LPWSTR p = StrStr(lpszStyle, key);

	if (p != NULL) {
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

static void Style_StrCopyValueEx(LPWSTR szNewStyle, LPCWSTR lpszStyle, LPCWSTR key, int keyLen, LPWSTR lpszValue, int cchValue) {
	if (Style_StrGetValueEx(lpszStyle, key, keyLen, lpszValue, cchValue)) {
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
	return Style_StrGetValueEx(lpszStyle, L"locale:", CSTRLEN(L"locale:"), lpszLocale, cchLocale);
}

#define Style_StrCopyValue(szNewStyle, lpszStyle, name, tch)	Style_StrCopyValueEx((szNewStyle), (lpszStyle), (name), CSTRLEN(name), (tch), COUNTOF(tch))
#define Style_StrCopyFont(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"font:", (tch));
#define Style_StrCopyChatset(szNewStyle, lpszStyle, tch)	Style_StrCopyValue((szNewStyle), (lpszStyle), L"charset:", (tch));
#define Style_StrCopyLocale(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"locale:", (tch));
#define Style_StrCopySize(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"size:", (tch));
#define Style_StrCopyWeight(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"weight:", (tch));
#define Style_StrCopyCase(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"case:", (tch));
#define Style_StrCopyFore(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"fore:", (tch));
#define Style_StrCopyBack(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"back:", (tch));
#define Style_StrCopyAlpha(szNewStyle, lpszStyle, tch)		Style_StrCopyValue((szNewStyle), (lpszStyle), L"alpha:", (tch));
#define Style_StrCopyOutline(szNewStyle, lpszStyle, tch)	Style_StrCopyValue((szNewStyle), (lpszStyle), L"outline:", (tch));

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
BOOL Style_StrGetColor(BOOL bFore, LPCWSTR lpszStyle, COLORREF *rgb) {
	LPCWSTR p = StrStr(lpszStyle, (bFore ? L"fore:" : L"back:"));

	if (p != NULL) {
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
BOOL Style_StrGetCase(LPCWSTR lpszStyle, int *forceCase) {
	LPCWSTR p = StrStr(lpszStyle, L"case:");

	if (p != NULL) {
		p += CSTRLEN(L"case:");
		while (*p == L' ') {
			++p;
		}
		switch (*p) {
		case L'u':
		case L'U':
			*forceCase = SC_CASE_UPPER;
			return TRUE;
		case L'l':
		case L'L':
			*forceCase = SC_CASE_LOWER;
			return TRUE;
		case L'c':
		case L'C':
			*forceCase = SC_CASE_CAMEL;
			return TRUE;
		//case L'm':
		//case L'M':
		//	*forceCase = SC_CASE_MIXED; // default normal case
		//	return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
// Style_StrGetAlphaEx()
//
BOOL Style_StrGetAlphaEx(BOOL outline, LPCWSTR lpszStyle, int *alpha) {
	LPCWSTR p = StrStr(lpszStyle, (outline ? L"outline:" : L"alpha:"));

	if (p != NULL) {
		p += outline ? CSTRLEN(L"outline:") : CSTRLEN(L"alpha:");
		if (CRTStrToInt(p, alpha)) {
			*alpha = clamp_i(*alpha, SC_ALPHA_TRANSPARENT, SC_ALPHA_OPAQUE);
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
	COLORREF rgb;
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
	if (Style_StrGetForeColor(lpszStyle, &rgb)) {
		cf.rgbColors = rgb;
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
		_ltow(lf.lfCharSet, tch, 10);
		lstrcat(szNewStyle, tch);
	}
	Style_StrCopyLocale(szNewStyle, lpszStyle, tch);
	lstrcat(szNewStyle, L"; size:");
	_ltow(cf.iPointSize / 10, tch, 10);
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
			_ltow(lf.lfWeight, tch, 10);
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
	Style_StrCopyOutline(szNewStyle, lpszStyle, tch);
	Style_StrCopyEOLFilled(szNewStyle, lpszStyle);

	lstrcpyn(lpszStyle, szNewStyle, cchStyle);
	return TRUE;
}

//=============================================================================
//
// Style_SetDefaultFont()
//
void Style_SetDefaultFont(HWND hwnd, BOOL bCode) {
	const int iIdx = bCode ? GlobalStyleIndex_DefaultCode : GlobalStyleIndex_DefaultText;
	if (Style_SelectFont(hwnd, pLexGlobal->Styles[iIdx].szValue, MAX_EDITSTYLE_VALUE_SIZE, TRUE)) {
		fStylesModified |= STYLESMODIFIED_SOME_STYLE;
		pLexGlobal->bStyleChanged = TRUE;
		Style_SetLexer(pLexCurrent, FALSE);
	}
}

//=============================================================================
//
// Style_SelectColor()
//
BOOL Style_SelectColor(HWND hwnd, BOOL bFore, LPWSTR lpszStyle, int cchStyle) {
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(CHOOSECOLOR));

	COLORREF iRGBResult;
	if (!Style_StrGetColor(bFore, lpszStyle, &iRGBResult)) {
		iRGBResult = bFore ? GetSysColor(COLOR_WINDOWTEXT) : GetSysColor(COLOR_WINDOW);
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
		wsprintf(tch, L"fore:#%02X%02X%02X", GetRValue(iRGBResult), GetGValue(iRGBResult), GetBValue(iRGBResult));
		lstrcat(szNewStyle, tch);
		Style_StrCopyBack(szNewStyle, lpszStyle, tch);
	} else {
		Style_StrCopyFore(szNewStyle, lpszStyle, tch);
		if (StrNotEmpty(szNewStyle)) {
			lstrcat(szNewStyle, L"; ");
		}
		wsprintf(tch, L"back:#%02X%02X%02X", GetRValue(iRGBResult), GetGValue(iRGBResult), GetBValue(iRGBResult));
		lstrcat(szNewStyle, tch);
	}

	Style_StrCopyAlpha(szNewStyle, lpszStyle, tch);
	Style_StrCopyOutline(szNewStyle, lpszStyle, tch);
	Style_StrCopyEOLFilled(szNewStyle, lpszStyle);

	lstrcpyn(lpszStyle, szNewStyle, cchStyle);
	return TRUE;
}

//=============================================================================
//
// Style_SetStyles()
//
void Style_SetStyles(int iStyle, LPCWSTR lpszStyle) {
	WCHAR tch[LF_FACESIZE];
	int iValue;
	COLORREF rgb;

	// Font
	if (Style_StrGetFont(lpszStyle, tch, COUNTOF(tch))) {
		char mch[LF_FACESIZE * kMaxMultiByteCount];
		WideCharToMultiByte(CP_UTF8, 0, tch, -1, mch, COUNTOF(mch), NULL, NULL);
		SciCall_StyleSetFont(iStyle, mch);
	}

	// Size
	if (Style_StrGetFontSize(lpszStyle, &iValue)) {
		iValue = DefaultToCurrentDPI(iValue);
		SciCall_StyleSetSizeFractional(iStyle, iValue);
	}

	// Fore
	if (Style_StrGetForeColor(lpszStyle, &rgb)) {
		SciCall_StyleSetFore(iStyle, rgb);
	}

	// Back
	if (Style_StrGetBackColor(lpszStyle, &rgb)) {
		SciCall_StyleSetBack(iStyle, rgb);
	}

	// Weight
	if (Style_StrGetFontWeight(lpszStyle, &iValue)) {
		SciCall_StyleSetWeight(iStyle, iValue);
	}

	// Italic
	if (Style_StrGetItalic(lpszStyle)) {
		SciCall_StyleSetItalic(iStyle, TRUE);
	}
	// Underline
	if (Style_StrGetUnderline(lpszStyle)) {
		SciCall_StyleSetUnderline(iStyle, TRUE);
	}
	// Strike
	if (Style_StrGetStrike(lpszStyle)) {
		SciCall_StyleSetStrike(iStyle, TRUE);
	}
	// EOL Filled
	if (Style_StrGetEOLFilled(lpszStyle)) {
		SciCall_StyleSetEOLFilled(iStyle, TRUE);
	}

	// Case
	if (Style_StrGetCase(lpszStyle, &iValue)) {
		SciCall_StyleSetCase(iStyle, iValue);
	}

	// Character Set
	if (Style_StrGetCharSet(lpszStyle, &iValue)) {
		SciCall_StyleSetCharacterSet(iStyle, iValue);
	}
}

static void Style_Parse(struct DetailStyle *style, LPCWSTR lpszStyle) {
	UINT mask = 0;
	int iValue;
	COLORREF rgb;

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
	if (Style_StrGetForeColor(lpszStyle, &rgb)) {
		style->foreColor = rgb;
		mask |= STYLE_MASK_FORE_COLOR;
	}

	// Back
	if (Style_StrGetBackColor(lpszStyle, &rgb)) {
		style->backColor = rgb;
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

static void Style_SetParsed(const struct DetailStyle *style, int iStyle) {
	const UINT mask = style->mask;

	// Font
	if (mask & STYLE_MASK_FONT_FACE) {
		SciCall_StyleSetFont(iStyle, style->fontFace);
	}

	// Size
	if (mask & STYLE_MASK_FONT_SIZE) {
		SciCall_StyleSetSizeFractional(iStyle, style->fontSize);
	}

	// Fore
	if (mask & STYLE_MASK_FORE_COLOR) {
		SciCall_StyleSetFore(iStyle, style->foreColor);
	}

	// Back
	if (mask & STYLE_MASK_BACK_COLOR) {
		SciCall_StyleSetBack(iStyle, style->backColor);
	}

	// Weight
	if (mask & STYLE_MASK_FONT_WEIGHT) {
		SciCall_StyleSetWeight(iStyle, style->weight);
	}

	// Italic
	if (style->italic) {
		SciCall_StyleSetItalic(iStyle, TRUE);
	}
	// Underline
	if (style->underline) {
		SciCall_StyleSetUnderline(iStyle, TRUE);
	}
	// Strike
	if (style->strike) {
		SciCall_StyleSetStrike(iStyle, TRUE);
	}
	// EOL Filled
	if (style->eolFilled) {
		SciCall_StyleSetEOLFilled(iStyle, TRUE);
	}

	// Case
	if (mask & STYLE_MASK_FORCE_CASE) {
		SciCall_StyleSetCase(iStyle, style->forceCase);
	}

	// Character Set
	if (mask & STYLE_MASK_CHARSET) {
		SciCall_StyleSetCharacterSet(iStyle, style->charset);
	}
}

//=============================================================================
//
// Style_GetLexerIconId()
//
int Style_GetLexerIconId(LPCEDITLEXER pLex, DWORD iconFlags) {
	LPCWSTR pszExtensions;
	if (StrNotEmpty(pLex->szExtensions)) {
		pszExtensions = pLex->szExtensions;
	} else {
		pszExtensions = pLex->pszDefExt;
	}

	WCHAR pszFile[MAX_PATH];
	lstrcpy(pszFile, L"*.");

	// TODO: avoid copying all extensions then find separators.
	// we only need the first extension, it's usually very short.
	LPWSTR p = pszFile + 2;
	if (StrNotEmpty(pszExtensions)) {
		lstrcpyn(p, pszExtensions, MAX_PATH - 2);

		p = StrChr(p, L';');
		//p = StrPBrk(p, L"; ");
		if (p != NULL) {
			*p = L'\0';
		}
	}

	// check for ; at beginning
	if (p == pszFile + 2) {
		lstrcpy(p, L"txt");
	}

	SHFILEINFO shfi;
	SHGetFileInfo(pszFile, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), iconFlags);

	return shfi.iIcon;
}

//=============================================================================
//
// Style_AddLexerToTreeView()
//
HTREEITEM Style_AddLexerToTreeView(HWND hwnd, PEDITLEXER pLex, DWORD iconFlags, HTREEITEM hParent, HTREEITEM hInsertAfter, BOOL withStyles) {
#if NP2_ENABLE_LEXER_NAME_LOCALIZATION || NP2_ENABLE_STYLE_NAME_LOCALIZATION
	WCHAR tch[MAX_EDITLEXER_NAME_SIZE];
#endif

	TVINSERTSTRUCT tvis;
	ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));

	tvis.hParent = hParent;
	tvis.hInsertAfter = hInsertAfter;
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
#if NP2_ENABLE_LEXER_NAME_LOCALIZATION
	if (GetString(pLex->rid, tch, COUNTOF(tch))) {
		tvis.item.pszText = tch;
	} else {
		tvis.item.pszText = (WCHAR *)pLex->pszName;
	}
#else
	tvis.item.pszText = (WCHAR *)pLex->pszName;
#endif
	tvis.item.iImage = Style_GetLexerIconId(pLex, iconFlags);
	tvis.item.iSelectedImage = tvis.item.iImage;
	tvis.item.lParam = (LPARAM)pLex;

	hParent = TreeView_InsertItem(hwnd, &tvis);
	if (!withStyles) {
		return hParent;
	}

	tvis.hParent = hParent;
	//tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	//tvis.item.iImage = -1;
	//tvis.item.iSelectedImage = -1;

	hInsertAfter = TVI_FIRST;
	const UINT iStyleCount = pLex->iStyleCount;

	for (UINT i = 0; i < iStyleCount; i++) {
		tvis.hInsertAfter = hInsertAfter;
#if NP2_ENABLE_STYLE_NAME_LOCALIZATION
		if (GetString(pLex->Styles[i].rid, tch, COUNTOF(tch))) {
			tvis.item.pszText = tch;
		} else {
			tvis.item.pszText = (WCHAR *)pLex->Styles[i].pszName;
		}
#else
		tvis.item.pszText = (WCHAR *)pLex->Styles[i].pszName;
#endif
		tvis.item.lParam = (LPARAM)(&pLex->Styles[i]);
		hInsertAfter = TreeView_InsertItem(hwnd, &tvis);
	}

	return hParent;
}

//=============================================================================
//
// Style_AddLexerToListView()
//
void Style_AddLexerToListView(HWND hwnd, PEDITLEXER pLex, DWORD iconFlags) {
#if NP2_ENABLE_LEXER_NAME_LOCALIZATION
	WCHAR tch[MAX_EDITLEXER_NAME_SIZE];
#endif
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));

	lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
	lvi.iItem = ListView_GetItemCount(hwnd);
#if NP2_ENABLE_LEXER_NAME_LOCALIZATION
	if (GetString(pLex->rid, tch, COUNTOF(tch))) {
		lvi.pszText = tch;
	} else {
		lvi.pszText = (WCHAR *)pLex->pszName;
	}
#else
	lvi.pszText = (WCHAR *)pLex->pszName;
#endif
	lvi.iImage = Style_GetLexerIconId(pLex, iconFlags);
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

struct SchemeGroupInfo {
	int group;
	int count;
};

enum {
	SchemeGroup_Global = 0,
	SchemeGroup_Favorite = 1,
};

static inline int Lexer_GetSchemeGroup(LPCEDITLEXER pLex) {
	const int ch = pLex->pszName[0];
	// assume all lexer name start with A-Z a-z
	return ToUpperA(ch);
}

static HTREEITEM Style_AddAllLexerToTreeView(HWND hwndTV, BOOL withStyles, BOOL withCheckBox) {
	struct SchemeGroupInfo groupList[26 + 2];
	int groupCount = 2;
	groupList[0].group = SchemeGroup_Global;
	groupList[0].count = withStyles ? LEXER_INDEX_GENERAL : LEXER_INDEX_MATCH;
	groupList[1].group = SchemeGroup_Favorite;
	groupList[1].count = 0;

	// favorite schemes
	int iLexer = LEXER_INDEX_GENERAL;
	while (iLexer < ALL_LEXER_COUNT && pLexArray[iLexer]->iFavoriteOrder) {
		++iLexer;
		++groupList[1].count;
	}

	// all general schemes
	PEDITLEXER generalLex[GENERAL_LEXER_COUNT];
	CopyMemory(generalLex, pLexArray + LEXER_INDEX_GENERAL, sizeof(generalLex));
	qsort(generalLex, GENERAL_LEXER_COUNT, sizeof(PEDITLEXER), CmpEditLexerByName);

	iLexer = 0;
	while (iLexer < GENERAL_LEXER_COUNT) {
		PEDITLEXER pLex = generalLex[iLexer++];
		int count = 1;
		const int group = Lexer_GetSchemeGroup(pLex);
		while (iLexer < GENERAL_LEXER_COUNT && group == Lexer_GetSchemeGroup(generalLex[iLexer])) {
			++iLexer;
			++count;
		}

		groupList[groupCount].group = group;
		groupList[groupCount].count = count;
		++groupCount;
	}

	InitWindowCommon(hwndTV);
	TreeView_SetExtendedStyle(hwndTV, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);
	SetExplorerTheme(hwndTV);

	const DWORD iconFlags = SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX;
	SHFILEINFO shfi;
	HIMAGELIST himl = (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO), iconFlags);
	TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL);
	// folder icon
	SHGetFileInfo(L"Icon", FILE_ATTRIBUTE_DIRECTORY, &shfi, sizeof(SHFILEINFO), iconFlags);

	WCHAR szTitle[128];

	TVINSERTSTRUCT tvis;
	ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE;
	tvis.item.pszText = szTitle;
	tvis.item.iImage = shfi.iIcon;
	tvis.item.iSelectedImage = shfi.iIcon;
	tvis.item.lParam = 0; // group
	// expand all group
	tvis.item.state = TVIS_EXPANDED;
	tvis.item.stateMask = TVIS_EXPANDED;

	// remove checkbox for group folder and Text File
	TVITEM item;
	ZeroMemory(&item, sizeof(item));
	item.mask = TVIF_STATE;
	item.state = 0;
	item.stateMask = TVIS_STATEIMAGEMASK;

	HTREEITEM hSelNode = NULL;
	HTREEITEM hSelParent = NULL;
	HTREEITEM hFavoriteNode = NULL;
	HTREEITEM hInsertAfter = TVI_FIRST;

	iLexer = LEXER_INDEX_GENERAL - groupList[0].count;
	for (int i = 0; i < groupCount; i++) {
		const struct SchemeGroupInfo info = groupList[i];
		const int group = info.group;
		const int count = info.count;

		HTREEITEM hParent = NULL;
		if (group != SchemeGroup_Global) {
			if (group == SchemeGroup_Favorite) {
				GetString(IDS_FAVORITE_SCHEMES_TITLE, szTitle, COUNTOF(szTitle));
			} else {
				szTitle[0] = (WCHAR)group;
				szTitle[1] = L'\0';
			}

			tvis.hInsertAfter = hInsertAfter;
			hParent = TreeView_InsertItem(hwndTV, &tvis);
			hInsertAfter = hParent;

			if (group == SchemeGroup_Favorite) {
				hFavoriteNode = hParent;
			}
			if (withCheckBox) {
				// remove checkbox for group folder
				item.hItem = hParent;
				TreeView_SetItem(hwndTV, &item);
			}
		}

		if (group <= SchemeGroup_Favorite) {
			HTREEITEM hTreeNode = TVI_FIRST;
			for (int j = 0; j < count; j++) {
				PEDITLEXER pLex = pLexArray[iLexer++];
				hTreeNode = Style_AddLexerToTreeView(hwndTV, pLex, iconFlags, hParent, hTreeNode, withStyles);
				if (hSelNode == NULL && pLex == pLexCurrent) {
					hSelNode = hTreeNode;
					hSelParent = hParent;
				}
				if (withCheckBox) {
					if (group == SchemeGroup_Favorite) {
						TreeView_SetCheckState(hwndTV, hTreeNode, TRUE);
					} else {
						// remove checkbox for Text File
						item.hItem = hTreeNode;
						TreeView_SetItem(hwndTV, &item);
					}
				}
			}

			if (group == SchemeGroup_Global) {
				hInsertAfter = hTreeNode;
			} else {
				iLexer = 0;
			}
		} else {
			HTREEITEM hTreeNode = TVI_FIRST;
			for (int j = 0; j < count; j++) {
				PEDITLEXER pLex = generalLex[iLexer++];
				hTreeNode = Style_AddLexerToTreeView(hwndTV, pLex, iconFlags, hParent, hTreeNode, withStyles);
				if (hSelNode == NULL && pLex == pLexCurrent) {
					hSelNode = hTreeNode;
					hSelParent = hParent;
				}
				if (withCheckBox) {
					if (pLex->iFavoriteOrder) {
						TreeView_SetCheckState(hwndTV, hTreeNode, TRUE);
					}
				}
			}
		}
	}

	if (hSelParent != NULL) {
		TreeView_EnsureVisible(hwndTV, hSelParent);
	}
	TreeView_Select(hwndTV, hSelNode, TVGN_CARET);
	return hFavoriteNode;
}

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

		WCHAR szTitle[1024];
		const UINT idsTitle = (np2StyleTheme == StyleTheme_Dark) ? IDS_CONFIG_THEME_TITLE_DARK : IDS_CONFIG_THEME_TITLE_DEFAULT;
		GetString(idsTitle, szTitle, COUNTOF(szTitle));
		SetWindowText(hwnd, szTitle);

		GetString(IDS_CUSTOMIZE_SCHEMES_HELP, szTitle, COUNTOF(szTitle));
		SetDlgItemText(hwnd, IDC_STYLEEDIT_HELP, szTitle);

		fDragging = FALSE;
		fLexerSelected = FALSE;
		iCurrentStyleIndex = -1;
		pCurrentLexer = NULL;
		pCurrentStyle = NULL;

		hwndTV = GetDlgItem(hwnd, IDC_STYLELIST);
		Style_AddAllLexerToTreeView(hwndTV, TRUE, FALSE);

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
		HDWP hdwp = BeginDeferWindowPos(19);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP3, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLELIST, 0, dy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_INFO_GROUPBOX, dx, 0, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_STYLEEDIT_HELP, dx, 0, SWP_NOMOVE);
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

				fLexerSelected = FALSE;
				iCurrentStyleIndex = -1;
				pCurrentStyle = NULL;

				HTREEITEM hParent = TreeView_GetParent(hwndTV, lpnmtv->itemNew.hItem);
				if (hParent == NULL) {
					if (lpnmtv->itemNew.lParam == 0) {
						pCurrentLexer = NULL;
					} else {
						fLexerSelected = TRUE;
						pCurrentLexer = (PEDITLEXER)lpnmtv->itemNew.lParam;
					}
				} else {
					TVITEM item;
					ZeroMemory(&item, sizeof(item));
					item.mask = TVIF_PARAM;
					item.hItem = hParent;
					TreeView_GetItem(hwndTV, &item);

					if (item.lParam == 0) {
						fLexerSelected = TRUE;
						pCurrentLexer = (PEDITLEXER)lpnmtv->itemNew.lParam;
					} else {
						pCurrentLexer = (PEDITLEXER)item.lParam;
						pCurrentStyle = (PEDITSTYLE)lpnmtv->itemNew.lParam;
					}
				}
				if (hParent == NULL || fLexerSelected) {
					TreeView_Expand(hwndTV, lpnmtv->itemNew.hItem, TVE_EXPAND);
				}

				UINT enableMask = StyleControl_None;
				// a lexer has been selected
				if (fLexerSelected) {
					if (pCurrentLexer != NULL) {
						WCHAR wch[MAX_EDITLEXER_EXT_SIZE];

						GetDlgItemText(hwnd, IDC_STYLELABELS, wch, COUNTOF(wch));
						if (StrChr(wch, L'|')) {
							*StrChr(wch, L'|') = 0;
						}

						SetDlgItemText(hwnd, IDC_STYLELABEL, wch);
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEEDIT), (pCurrentLexer->szExtensions != NULL));
						EnableWindow(GetDlgItem(hwnd, IDC_STYLEDEFAULT), TRUE);
						SetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentLexer->szExtensions);
						SetDlgItemText(hwnd, IDC_STYLEVALUE_DEFAULT, pCurrentLexer->pszDefExt);
					}
				}

				// a style or group has been selected
				else {
					if (pCurrentStyle != NULL) {
						WCHAR wch[MAX_EDITSTYLE_VALUE_SIZE];

						GetDlgItemText(hwnd, IDC_STYLELABELS, wch, COUNTOF(wch));
						if (StrChr(wch, L'|')) {
							*StrChr(wch, L'|') = 0;
						}

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
						enableMask = GetLexerStyleControlMask(pCurrentLexer->rid, iCurrentStyleIndex);
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
				//if (pCurrentStyle) {
				//	GetDlgItemText(hwnd, IDC_STYLEEDIT, pCurrentStyle->szValue, MAX_EDITSTYLE_VALUE_SIZE);
				//}
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
			break;
			}
		}
		break;

	case WM_MOUSEMOVE:
		if (fDragging && pCurrentStyle) {
			TVHITTESTINFO tvht = EMPTY_BRACE_INITIALIZER;

			//ImageList_DragMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			//ImageList_DragShowNolock(FALSE);

			tvht.pt.x = GET_X_LPARAM(lParam);
			tvht.pt.y = GET_Y_LPARAM(lParam);

			//ClientToScreen(hwnd, &tvht.pt);
			//ScreenToClient(hwndTV, &tvht.pt);
			MapWindowPoints(hwnd, hwndTV, &tvht.pt, 1);
			TreeView_HitTest(hwndTV, &tvht);

			HTREEITEM htiTarget = tvht.hItem;
			if (htiTarget != NULL) {
				TreeView_EnsureVisible(hwndTV, htiTarget);
				HTREEITEM hParent = TreeView_GetParent(hwndTV, htiTarget);
				HTREEITEM hChildNode = TreeView_GetChild(hwndTV, htiTarget);
				if (!(hParent != NULL && hChildNode == NULL)) {
					// not on style node
					//TreeView_Expand(hwndTV, htiTarget, TVE_EXPAND);
					htiTarget = NULL;
				}
			}

			TreeView_SelectDropTarget(hwndTV, htiTarget);
			//ImageList_DragShowNolock(TRUE);
		}
		break;

	case WM_LBUTTONUP:
		if (fDragging) {
			//ImageList_EndDrag();
			HTREEITEM htiTarget = TreeView_GetDropHilight(hwndTV);
			TreeView_SelectDropTarget(hwndTV, NULL);

			if (htiTarget != NULL) {
				WCHAR tchCopy[MAX_LEXER_STYLE_EDIT_SIZE];
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
		break;

	case WM_CANCELMODE:
		if (fDragging) {
			//ImageList_EndDrag();
			TreeView_SelectDropTarget(hwndTV, NULL);
			ReleaseCapture();
			DestroyCursor(SetCursor(LoadCursor(NULL, IDC_ARROW)));
			fDragging = FALSE;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_PREVSTYLE:
			if (TreeView_GetSelection(hwndTV)) {
				TreeView_Select(hwndTV, TreeView_GetPrevVisible(hwndTV, TreeView_GetSelection(hwndTV)), TVGN_CARET);
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_NEXTSTYLE:
			if (TreeView_GetSelection(hwndTV)) {
				TreeView_Select(hwndTV, TreeView_GetNextVisible(hwndTV, TreeView_GetSelection(hwndTV)), TVGN_CARET);
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_STYLEEDIT)), 1);
			break;

		case IDC_STYLEFONT:
			if (pCurrentStyle) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];
				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));
				const BOOL bDefaultStyle = IsGlobalBaseStyleIndex(pCurrentLexer->rid, iCurrentStyleIndex);

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

		case IDC_STYLEEDIT:
			if (HIWORD(wParam) == EN_CHANGE && pCurrentStyle != NULL) {
				WCHAR tch[MAX_LEXER_STYLE_EDIT_SIZE];

				GetDlgItemText(hwnd, IDC_STYLEEDIT, tch, COUNTOF(tch));

				COLORREF cr = (COLORREF)(-1);
				Style_StrGetForeColor(tch, &cr);
				MakeColorPickButton(hwnd, IDC_STYLEFORE, g_hInstance, cr);

				cr = (COLORREF)(-1);
				Style_StrGetBackColor(tch, &cr);
				MakeColorPickButton(hwnd, IDC_STYLEBACK, g_hInstance, cr);
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
				Style_SetLexer(pLexCurrent, FALSE);
			}
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
		Style_SetLexer(pLexCurrent, FALSE);
	}
}

static PEDITLEXER Lexer_GetFromTreeView(HWND hwndTV) {
	HTREEITEM hTreeNode = TreeView_GetSelection(hwndTV);
	if (hTreeNode != NULL) {
		TVITEM item;
		ZeroMemory(&item, sizeof(item));
		item.mask = TVIF_PARAM;
		item.hItem = hTreeNode;
		TreeView_GetItem(hwndTV, &item);
		return (PEDITLEXER)item.lParam;
	}
	return NULL;
}

static void Lexer_OnCheckStateChanged(HWND hwndTV, HTREEITEM hFavoriteNode, HTREEITEM hTreeNode) {
	TVITEM item;
	ZeroMemory(&item, sizeof(item));
	item.mask = TVIF_PARAM;
	item.hItem = hTreeNode;
	TreeView_GetItem(hwndTV, &item);

	const LPARAM lParam = item.lParam;
	if (lParam == 0) {
		return;
	}

	// toggle check state on clicking
	const BOOL checked = !TreeView_GetCheckState(hwndTV, hTreeNode);

	HTREEITEM hParent = TreeView_GetParent(hwndTV, hTreeNode);
	BOOL found = hParent == hFavoriteNode;
	HTREEITEM hInsertAfter = TVI_FIRST;

	if (!found) {
		HTREEITEM hChildNode = TreeView_GetChild(hwndTV, hFavoriteNode);
		while (hChildNode != NULL) {
			item.hItem = hChildNode;
			TreeView_GetItem(hwndTV, &item);
			if (item.lParam == lParam) {
				found = TRUE;
				hTreeNode = hChildNode;
				break;
			}

			hInsertAfter = hChildNode;
			hChildNode = TreeView_GetNextSibling(hwndTV, hChildNode);
		}
	}

	PEDITLEXER pLex = (PEDITLEXER)lParam;
	if (checked) {
		// append node into Favorite Schemes
		if (!found) {
			const DWORD iconFlags = SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX;
			hTreeNode = TreeView_GetLastVisible(hwndTV);
			hParent = Style_AddLexerToTreeView(hwndTV, pLex, iconFlags, hFavoriteNode, hInsertAfter, FALSE);
			TreeView_SetCheckState(hwndTV, hParent, TRUE);
			// prevent auto scroll
			TreeView_EnsureVisible(hwndTV, hTreeNode);
		}
	} else {
		// remove node from Favorite Schemes
		pLex->iFavoriteOrder = 0;

		if (found) {
			TreeView_DeleteItem(hwndTV, hTreeNode);
		}

		// update check state in general schemes
		if (hParent == hFavoriteNode) {
			const int group = Lexer_GetSchemeGroup(pLex);
			WCHAR szTitle[4] = {0};
			//item.mask = TVIF_TEXT;
			item.pszText = szTitle;
			item.cchTextMax  = COUNTOF(szTitle);

			hParent = TreeView_GetNextSibling(hwndTV, hParent);
			found = FALSE;
			while (!found && hParent != NULL) {
				item.mask = TVIF_TEXT;
				item.hItem = hParent;
				TreeView_GetItem(hwndTV, &item);
				// test group header
				if (group == szTitle[0]) {
					item.mask = TVIF_PARAM;
					hTreeNode = TreeView_GetChild(hwndTV, hParent);
					while (hTreeNode != NULL) {
						item.hItem = hTreeNode;
						TreeView_GetItem(hwndTV, &item);
						if (item.lParam == lParam) {
							found = TRUE;
							TreeView_SetCheckState(hwndTV, hTreeNode, FALSE);
							break;
						}

						hTreeNode = TreeView_GetNextSibling(hwndTV, hTreeNode);
					}
				}

				hParent = TreeView_GetNextSibling(hwndTV, hParent);
			}
		}
	}
}

static void Lexer_OnDragDrop(HWND hwndTV, HTREEITEM hFavoriteNode, HTREEITEM hDraggingNode, HTREEITEM htiTarget) {
	TVITEM item;
	ZeroMemory(&item, sizeof(item));
	item.mask = TVIF_PARAM;
	item.hItem = hDraggingNode;
	TreeView_GetItem(hwndTV, &item);

	if (item.lParam == 0) {
		return;
	}

	HTREEITEM hTreeNode = TreeView_GetParent(hwndTV, htiTarget);
	if (!(htiTarget == hFavoriteNode || hTreeNode == hFavoriteNode)) {
		return;
	}

	HTREEITEM hParent = TreeView_GetParent(hwndTV, hDraggingNode);
	HTREEITEM hLastChild = TVI_FIRST;
	const LPARAM lParam = item.lParam;

	hTreeNode = TreeView_GetChild(hwndTV, hFavoriteNode);
	if (hParent == hFavoriteNode) {
		// dragging within Favorite Schemes
		if (htiTarget == hFavoriteNode && hDraggingNode == hTreeNode) {
			// position not changed for first child
			return;
		}

		hTreeNode = TreeView_GetNextSibling(hwndTV, htiTarget);
		if (hTreeNode == hDraggingNode) {
			// position not changed for next sibling node
			return;
		}

		TreeView_DeleteItem(hwndTV, hDraggingNode);
	} else {
		// dragging into Favorite Schemes
		TreeView_SetCheckState(hwndTV, hDraggingNode, TRUE);

		BOOL found = FALSE;
		while (hTreeNode != NULL) {
			item.hItem = hTreeNode;
			TreeView_GetItem(hwndTV, &item);
			if (item.lParam == lParam) {
				found = TRUE;
				break;
			}

			hLastChild = hTreeNode;
			hTreeNode = TreeView_GetNextSibling(hwndTV, hTreeNode);
		}

		if (found) {
			TreeView_SetCheckState(hwndTV, hTreeNode, TRUE);
			return;
		}
	}

	const UINT expanded = TreeView_GetItemState(hwndTV, hFavoriteNode, TVIS_EXPANDED) & TVIS_EXPANDED;
	HTREEITEM hInsertAfter = expanded ? ((htiTarget == hFavoriteNode)? TVI_FIRST : htiTarget) : hLastChild;
	PEDITLEXER pLex = (PEDITLEXER)lParam;

	const DWORD iconFlags = SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX;
	hTreeNode = Style_AddLexerToTreeView(hwndTV, pLex, iconFlags, hFavoriteNode, hInsertAfter, FALSE);
	TreeView_SetCheckState(hwndTV, hTreeNode, TRUE);
	if (expanded) {
		TreeView_Select(hwndTV, hTreeNode, TVGN_CARET);
	}
}

static void Style_GetFavoriteSchemesFromTreeView(HWND hwndTV, HTREEITEM hFavoriteNode) {
	const int maxCch = MAX_FAVORITE_SCHEMES_CONFIG_SIZE - 3; // two digits, one space and NULL
	WCHAR *wch = favoriteSchemesConfig;
	int len = 0;
	int count = 0;

	TVITEM item;
	ZeroMemory(&item, sizeof(item));
	item.mask = TVIF_PARAM;

	HTREEITEM hTreeNode = TreeView_GetChild(hwndTV, hFavoriteNode);
	while (hTreeNode != NULL) {
		item.hItem = hTreeNode;
		TreeView_GetItem(hwndTV, &item);
		if (item.lParam) {
			PEDITLEXER pLex = (PEDITLEXER)item.lParam;
			pLex->iFavoriteOrder = MAX_FAVORITE_SCHEMES_COUNT - count;

			len += wsprintf(wch + len, L"%i ", pLex->rid - NP2LEX_TEXTFILE);
			++count;
			if (count == MAX_FAVORITE_SCHEMES_COUNT || len > maxCch) {
				break;
			}
		}

		hTreeNode = TreeView_GetNextSibling(hwndTV, hTreeNode);
	}

	wch[len--] = L'\0';
	if (len >= 0) {
		wch[len] = L'\0';
	}

	qsort(pLexArray + LEXER_INDEX_GENERAL, GENERAL_LEXER_COUNT, sizeof(PEDITLEXER), CmpEditLexerByOrder);
}

//=============================================================================
//
// Style_SelectLexerDlgProc()
//
static INT_PTR CALLBACK Style_SelectLexerDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static HWND hwndTV;
	static HTREEITEM hFavoriteNode;
	static HTREEITEM hDraggingNode;
	static BOOL fDragging;
	static int iInternalDefault;

	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		ResizeDlg_Init(hwnd, cxStyleSelectDlg, cyStyleSelectDlg, IDC_RESIZEGRIP3);

		const BOOL favorite = lParam != 0;
		if (favorite) {
			WCHAR szTitle[128];
			GetString(IDS_FAVORITE_SCHEMES_TITLE, szTitle, COUNTOF(szTitle));
			SetWindowText(hwnd, szTitle);
		}

		hwndTV = GetDlgItem(hwnd, IDC_STYLELIST);
		SetWindowStyle(hwndTV, GetWindowStyle(hwndTV) | (favorite ? TVS_CHECKBOXES : TVS_DISABLEDRAGDROP));

		hFavoriteNode = Style_AddAllLexerToTreeView(hwndTV, FALSE, favorite);
		if (favorite) {
			TreeView_EnsureVisible(hwndTV, hFavoriteNode);
			TreeView_SetInsertMarkColor(hwndTV, GetSysColor(COLOR_HIGHLIGHT));
		}

		hDraggingNode = NULL;
		fDragging = FALSE;
		iInternalDefault = pLexArray[iDefaultLexerIndex]->rid;

		if (iInternalDefault == pLexCurrent->rid) {
			CheckDlgButton(hwnd, IDC_DEFAULTSCHEME, BST_CHECKED);
		}

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
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY:
		if (((LPNMHDR)(lParam))->idFrom == IDC_STYLELIST) {
			LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)lParam;
			switch (lpnmtv->hdr.code) {
			case NM_CLICK: {
				// https://support.microsoft.com/en-us/help/261289/how-to-know-when-the-user-clicks-a-check-box-in-a-treeview-control
				TVHITTESTINFO tvht = EMPTY_BRACE_INITIALIZER;
				DWORD dwpos = GetMessagePos();
				tvht.pt.x = GET_X_LPARAM(dwpos);
				tvht.pt.y = GET_Y_LPARAM(dwpos);
				MapWindowPoints(HWND_DESKTOP, hwndTV, &tvht.pt, 1);
				TreeView_HitTest(hwndTV, &tvht);

				if (tvht.hItem != NULL && (TVHT_ONITEMSTATEICON & tvht.flags)) {
					Lexer_OnCheckStateChanged(hwndTV, hFavoriteNode, tvht.hItem);
				}
			}
			break;

			case NM_DBLCLK:
				// disable double click when managing favorite schemes.
				if (Lexer_GetFromTreeView(hwndTV) != NULL && GetWindowLongPtr(hwnd, DWLP_USER) == 0) {
					SendWMCommand(hwnd, IDOK);
				}
				break;

			case TVN_SELCHANGED: {
				PEDITLEXER pLex = (PEDITLEXER)lpnmtv->itemNew.lParam;
				const BOOL selected = pLex != NULL;
				if (selected) {
					CheckDlgButton(hwnd, IDC_DEFAULTSCHEME, (iInternalDefault == pLex->rid)? BST_CHECKED : BST_UNCHECKED);
				} else {
					TreeView_Expand(hwndTV, lpnmtv->itemNew.hItem, TVE_EXPAND);
				}
				EnableWindow(GetDlgItem(hwnd, IDC_DEFAULTSCHEME), selected);
			}
			break;

			case TVN_BEGINDRAG: {
				TreeView_Select(hwndTV, lpnmtv->itemNew.hItem, TVGN_CARET);
				// prevent dragging group folder or Text File
				if (lpnmtv->itemNew.lParam != 0 && TreeView_GetParent(hwndTV, lpnmtv->itemNew.hItem) != NULL) {
					hDraggingNode = lpnmtv->itemNew.hItem;
					DestroyCursor(SetCursor(LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_COPY))));
				} else {
					hDraggingNode = NULL;
					DestroyCursor(SetCursor(LoadCursor(NULL, IDC_NO)));
				}
				SetCapture(hwnd);
				fDragging = TRUE;
			}
			break;
			}
		}
		return TRUE;

	case WM_MOUSEMOVE:
		if (fDragging && hDraggingNode != NULL) {
			TVHITTESTINFO tvht = EMPTY_BRACE_INITIALIZER;

			tvht.pt.x = GET_X_LPARAM(lParam);
			tvht.pt.y = GET_Y_LPARAM(lParam);

			MapWindowPoints(hwnd, hwndTV, &tvht.pt, 1);
			TreeView_HitTest(hwndTV, &tvht);

			HTREEITEM htiTarget = tvht.hItem;
			if (htiTarget != NULL) {
				TreeView_EnsureVisible(hwndTV, htiTarget);
				HTREEITEM hParent = TreeView_GetParent(hwndTV, htiTarget);
				if (htiTarget == hFavoriteNode || hParent == hFavoriteNode) {
					//TreeView_Expand(hwndTV, hFavoriteNode, TVE_EXPAND);
				} else {
					htiTarget = NULL;
				}
			}

			TreeView_SelectDropTarget(hwndTV, htiTarget);
			TreeView_SetInsertMark(hwndTV, htiTarget, TRUE);
		}
		break;

	case WM_LBUTTONUP:
		if (fDragging) {
			HTREEITEM htiTarget = TreeView_GetDropHilight(hwndTV);
			TreeView_SelectDropTarget(hwndTV, NULL);
			TreeView_SetInsertMark(hwndTV, NULL, TRUE);

			if (hDraggingNode != NULL && htiTarget != NULL && hDraggingNode != htiTarget) {
				Lexer_OnDragDrop(hwndTV, hFavoriteNode, hDraggingNode, htiTarget);
			}

			ReleaseCapture();
			DestroyCursor(SetCursor(LoadCursor(NULL, IDC_ARROW)));
			fDragging = FALSE;
		}
		break;

	case WM_CANCELMODE:
		if (fDragging) {
			TreeView_SelectDropTarget(hwndTV, NULL);
			TreeView_SetInsertMark(hwndTV, NULL, TRUE);
			ReleaseCapture();
			DestroyCursor(SetCursor(LoadCursor(NULL, IDC_ARROW)));
			fDragging = FALSE;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_DEFAULTSCHEME:
			iInternalDefault = NP2LEX_TEXTFILE;
			if (IsButtonChecked(hwnd, IDC_DEFAULTSCHEME)) {
				PEDITLEXER pLex = Lexer_GetFromTreeView(hwndTV);
				if (pLex != NULL) {
					iInternalDefault = pLex->rid;
				}
			}
			break;

		case IDOK: {
			if (GetWindowLongPtr(hwnd, DWLP_USER)) {
				Style_GetFavoriteSchemesFromTreeView(hwndTV, hFavoriteNode);
			} else {
				PEDITLEXER pLex = Lexer_GetFromTreeView(hwndTV);
				if (pLex != NULL) {
					pLexCurrent = pLex;
					np2LexLangIndex = 0;
				}
			}

			if (iInternalDefault != pLexArray[iDefaultLexerIndex]->rid) {
				// default lexer changed
				iDefaultLexerIndex = Style_GetMatchLexerIndex(iInternalDefault);
			}

			bAutoSelect = IsButtonChecked(hwnd, IDC_AUTOSELECT);
			EndDialog(hwnd, IDOK);
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
// Style_SelectLexerDlg()
//
void Style_SelectLexerDlg(HWND hwnd, BOOL favorite) {
	const LPCEDITLEXER pLex = pLexCurrent;
	const int langIndex = np2LexLangIndex;
	if (IDOK == ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_STYLESELECT), GetParent(hwnd), Style_SelectLexerDlgProc, favorite)) {
		const BOOL bLexerChanged = !favorite && (pLex != pLexCurrent || langIndex != np2LexLangIndex);
		if (bLexerChanged) {
			Style_SetLexer(pLexCurrent, TRUE);
		}
	} else {
		if (favorite) {
			Style_SetFavoriteSchemes();
		}
	}
}
