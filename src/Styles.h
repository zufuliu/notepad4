/******************************************************************************
*
*
* Notepad2
*
* Styles.h
*   Scintilla Style Management
*
* See Readme.txt for more information about this source code.
* Please send me your comments to this work.
*
* See License.txt for details about distribution and modification.
*
*                                              (c) Florian Balmer 1996-2011
*                                                  florian.balmer@gmail.com
*                                              https://www.flos-freeware.ch
*
*
******************************************************************************/
#pragma once

#include "EditLexer.h"

// maximum favorite schemes count, DON'T change.
#define MAX_FAVORITE_SCHEMES_COUNT			31
// All Files, current scheme, Text File, 2nd Text File and favorite schemes
// see Style_GetOpenDlgFilterStr() for actually count.
#define OPENDLG_MAX_LEXER_COUNT				(MAX_FAVORITE_SCHEMES_COUNT + 2 + 2)

#define INI_SECTION_NAME_STYLES				L"Styles"
#define INI_SECTION_NAME_FILE_EXTENSIONS	L"File Extensions"
#define INI_SECTION_NAME_CUSTOM_COLORS		L"Custom Colors"

#define MAX_INI_SECTION_SIZE_STYLES			(8 * 1024)

typedef enum LineHighlightMode {
	LineHighlightMode_None = 0,
	LineHighlightMode_BackgroundColor,
	LineHighlightMode_OutlineFrame,
} LineHighlightMode;

typedef enum CaretStyle {
	CaretStyle_Block = 0,
	CaretStyle_LineWidth1,
	CaretStyle_LineWidth2,
	CaretStyle_LineWidth3,
} CaretStyle;

typedef enum StyleDefinitionMask {
	StyleDefinitionMask_None = 0,
	StyleDefinitionMask_FontFace = 1 << 0,
	StyleDefinitionMask_FontSize = 1 << 1,
	StyleDefinitionMask_ForeColor = 1 << 2,
	StyleDefinitionMask_BackColor = 1 << 3,
	StyleDefinitionMask_FontWeight = 1 << 4,
	StyleDefinitionMask_Charset = 1 << 5,
} StyleDefinitionMask;

typedef struct StyleDefinition {
	UINT mask;
	int fontSize;
	COLORREF foreColor;
	COLORREF backColor;
	int weight;
	bool italic;
	bool underline;
	bool strike;
	bool overline;
	bool eolFilled;
	uint8_t unused;
	uint16_t backIndex;
	int charset;
	WCHAR fontWide[LF_FACESIZE];
	char fontFace[LF_FACESIZE * kMaxMultiByteCount];
} StyleDefinition;

extern PEDITLEXER pLexCurrent;
extern int np2LexLangIndex;
extern bool bUse2ndGlobalStyle;
extern int np2StyleTheme;

void	Style_ReleaseResources(void);
void	Style_Load(void);
void	Style_Save(void);
bool	Style_Import(HWND hwnd);
bool	Style_Export(HWND hwnd);
void	Style_LoadTabSettings(LPCEDITLEXER pLex);
void	Style_SaveTabSettings(LPCEDITLEXER pLex);
void	EditApplyDefaultEncoding(LPCEDITLEXER pLex, BOOL bLexerChanged);
void	InitAutoCompletionCache(LPCEDITLEXER pLex);

void	Style_DetectBaseFontSize(HMONITOR hMonitor);
HFONT	Style_CreateCodeFont(UINT dpi);
void	Style_OnDPIChanged(LPCEDITLEXER pLex);
void	Style_OnStyleThemeChanged(int theme);
void	Style_InitDefaultColor(void);
void	Style_SetLexer(PEDITLEXER pLexNew, BOOL bLexerChanged);
bool	Style_SetLexerFromFile(LPCWSTR lpszFile);
void	Style_SetLexerFromName(LPCWSTR lpszFile, LPCWSTR lpszName);
bool	Style_CanOpenFile(LPCWSTR lpszFile);
void	Style_SetLexerFromID(int rid);
int		Style_GetMatchLexerIndex(int rid);

int		Style_GetDocTypeLanguage(void);
LPCWSTR Style_GetCurrentLexerName(LPWSTR lpszName, int cchName);
void	Style_SetLexerByLangIndex(int lang);
void	Style_UpdateSchemeMenu(HMENU hmenu);

void	Style_SetDefaultFont(HWND hwnd, bool bCode);
void	Style_SetIndentGuides(bool bShow);
void	Style_SetBookmark(void);
void	Style_UpdateCaret(void);
void	Style_SetLongLineColors(void);
void	Style_HighlightCurrentLine(void);
void	Style_ToggleUse2ndGlobalStyle(void);
void	Style_ToggleUseDefaultCodeStyle(void);
LPWSTR	Style_GetOpenDlgFilterStr(bool open, LPCWSTR lpszFile, int lexers[]);

bool	Style_StrGetFontEx(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont, bool bDefaultStyle);
bool	Style_StrGetCharSet(LPCWSTR lpszStyle, int *charset);
BOOL	Style_StrGetLocale(LPCWSTR lpszStyle, LPWSTR lpszLocale, int cchLocale);
bool	Style_StrGetFontSize(LPCWSTR lpszStyle, int *size);
bool	Style_StrGetSize(LPCWSTR lpszStyle, int *size);
bool	Style_StrGetFontWeight(LPCWSTR lpszStyle, int *weight);
bool	Style_StrGetColor(bool bFore, LPCWSTR lpszStyle, COLORREF *rgb);

NP2_inline bool Style_StrGetForeColor(LPCWSTR lpszStyle, COLORREF *rgb) {
	return Style_StrGetColor(true, lpszStyle, rgb);
}

NP2_inline bool Style_StrGetBackColor(LPCWSTR lpszStyle, COLORREF *rgb) {
	return Style_StrGetColor(false, lpszStyle, rgb);
}

bool	Style_StrGetAlphaEx(bool outline, LPCWSTR lpszStyle, int *alpha);
NP2_inline bool Style_StrGetAlpha(LPCWSTR lpszStyle, int *alpha) {
	return Style_StrGetAlphaEx(false, lpszStyle, alpha);
}

NP2_inline bool Style_StrGetOutlineAlpha(LPCWSTR lpszStyle, int *alpha) {
	return Style_StrGetAlphaEx(true, lpszStyle, alpha);
}

bool	Style_SelectFont(HWND hwnd, LPWSTR lpszStyle, int cchStyle, bool bDefaultStyle);
bool	Style_SelectColor(HWND hwnd, LPWSTR lpszStyle, int cchStyle, bool bFore);
void	Style_SetStyles(int iStyle, LPCWSTR lpszStyle);

int 	Style_GetLexerIconId(LPCEDITLEXER pLex, DWORD iconFlags);
void	Style_ConfigDlg(HWND hwnd);
void	Style_SelectLexerDlg(HWND hwnd, bool favorite);
bool	SelectCSVOptionsDlg(void);
