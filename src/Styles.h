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
*                                               http://www.flos-freeware.ch
*
*
******************************************************************************/
#pragma once

#include "EditLexer.h"

// Number of Lexers in pLexArray
#define NUMLEXERS	64

// maximum favorite schemes count, DON'T change.
#define MAX_FAVORITE_SCHEMES_COUNT			31
// All Files, current scheme, Text File, 2nd Text File and favorite schemes
// see Style_GetOpenDlgFilterStr() for actually count.
#define OPENDLG_MAX_LEXER_COUNT				(MAX_FAVORITE_SCHEMES_COUNT + 2 + 2)

#define INI_SECTION_NAME_STYLES				L"Styles"
#define INI_SECTION_NAME_FILE_EXTENSIONS	L"File Extensions"
#define INI_SECTION_NAME_CUSTOM_COLORS		L"Custom Colors"

#define MAX_INI_SECTION_SIZE_STYLES			(8 * 1024)

extern PEDITLEXER pLexCurrent;
extern int np2LexLangIndex;
extern BOOL bUse2ndGlobalStyle;
extern int np2StyleTheme;
extern BOOL bCurrentLexerHasLineComment;
extern BOOL bCurrentLexerHasBlockComment;
extern uint8_t currentLexKeywordAttr[NUMKEYWORD];

void	Style_ReleaseResources(void);
void	Style_Load(void);
void	Style_Save(void);
BOOL	Style_Import(HWND hwnd);
BOOL	Style_Export(HWND hwnd);

void	Style_DetectBaseFontSize(HWND hwnd);
void	Style_OnDPIChanged(void);
void	Style_OnStyleThemeChanged(int theme);
void	Style_InitDefaultColor(void);
void	Style_SetLexer(PEDITLEXER pLexNew, BOOL bLexerChanged);
BOOL	Style_SetLexerFromFile(LPCWSTR lpszFile);
void	Style_SetLexerFromName(LPCWSTR lpszFile, LPCWSTR lpszName);
BOOL	Style_MaybeBinaryFile(LPCWSTR lpszFile);
BOOL	Style_CanOpenFile(LPCWSTR lpszFile);
void	Style_SetLexerFromID(int rid);
int		Style_GetMatchLexerIndex(int rid);

int		Style_GetDocTypeLanguage(void);
void	Style_UpdateLexerKeywords(LPCEDITLEXER pLexNew);
void	Style_UpdateLexerKeywordAttr(LPCEDITLEXER pLexNew);
LPCWSTR Style_GetCurrentLexerName(LPWSTR lpszName, int cchName);
void	Style_SetLexerByLangIndex(int lang);
void	Style_UpdateSchemeMenu(HMENU hmenu);

void	Style_SetDefaultFont(HWND hwnd, BOOL bCode);
void	Style_SetIndentGuides(BOOL bShow);
void	Style_SetBookmark(void);
void	Style_UpdateCaret(void);
void	Style_SetLongLineColors(void);
void	Style_HighlightCurrentLine(void);
void	Style_ToggleUse2ndGlobalStyle(void);
void	Style_ToggleUseDefaultCodeStyle(void);
LPWSTR	Style_GetOpenDlgFilterStr(BOOL open, LPCWSTR lpszFile, int lexers[]);

BOOL	Style_StrGetFontEx(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont, BOOL bDefaultStyle);
BOOL	Style_StrGetCharSet(LPCWSTR lpszStyle, int *charset);
BOOL	Style_StrGetLocale(LPCWSTR lpszStyle, LPWSTR lpszLocale, int cchLocale);
BOOL	Style_StrGetFontSize(LPCWSTR lpszStyle, int *size);
BOOL	Style_StrGetRawSize(LPCWSTR lpszStyle, int *size);
BOOL	Style_StrGetFontWeight(LPCWSTR lpszStyle, int *weight);
BOOL	Style_StrGetColor(BOOL bFore, LPCWSTR lpszStyle, COLORREF *rgb);

NP2_inline BOOL Style_StrGetForeColor(LPCWSTR lpszStyle, COLORREF *rgb) {
	return Style_StrGetColor(TRUE, lpszStyle, rgb);
}

NP2_inline BOOL Style_StrGetBackColor(LPCWSTR lpszStyle, COLORREF *rgb) {
	return Style_StrGetColor(FALSE, lpszStyle, rgb);
}

BOOL	Style_StrGetCase(LPCWSTR lpszStyle, int *forceCase);
BOOL	Style_StrGetAlphaEx(BOOL outline, LPCWSTR lpszStyle, int *alpha);

NP2_inline BOOL Style_StrGetAlpha(LPCWSTR lpszStyle, int *alpha) {
	return Style_StrGetAlphaEx(FALSE, lpszStyle, alpha);
}

NP2_inline BOOL Style_StrGetOutlineAlpha(LPCWSTR lpszStyle, int *alpha) {
	return Style_StrGetAlphaEx(TRUE, lpszStyle, alpha);
}

BOOL	Style_SelectFont(HWND hwnd, LPWSTR lpszStyle, int cchStyle, BOOL bDefaultStyle);
BOOL	Style_SelectColor(HWND hwnd, BOOL bFore, LPWSTR lpszStyle, int cchStyle);
void	Style_SetStyles(int iStyle, LPCWSTR lpszStyle);

int 	Style_GetLexerIconId(LPCEDITLEXER pLex, DWORD iconFlags);
void	Style_ConfigDlg(HWND hwnd);
void	Style_SelectLexerDlg(HWND hwnd, BOOL favorite);
