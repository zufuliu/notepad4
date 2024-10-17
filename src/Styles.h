/******************************************************************************
*
*
* Notepad4
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

enum LineHighlightMode {
	LineHighlightMode_None = 0,
	LineHighlightMode_BackgroundColor,
	LineHighlightMode_OutlineFrame,
};

enum CaretStyle {
	CaretStyle_Block = 0,
	CaretStyle_LineWidth1,
	CaretStyle_LineWidth2,
	CaretStyle_LineWidth3,
};

enum StyleDefinitionMask {
	StyleDefinitionMask_None = 0,
	StyleDefinitionMask_FontFace = 1 << 0,
	StyleDefinitionMask_FontSize = 1 << 1,
	StyleDefinitionMask_ForeColor = 1 << 2,
	StyleDefinitionMask_BackColor = 1 << 3,
	StyleDefinitionMask_FontWeight = 1 << 4,
	StyleDefinitionMask_Charset = 1 << 5,
};

struct StyleDefinition {
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
};

extern PEDITLEXER pLexCurrent;
extern int np2LexLangIndex;
extern int np2StyleTheme;

void	Style_ReleaseResources() noexcept;
void	Style_Load() noexcept;
void	Style_Save() noexcept;
bool	Style_Import(HWND hwnd) noexcept;
bool	Style_Export(HWND hwnd) noexcept;
void	Style_LoadTabSettings(LPCEDITLEXER pLex) noexcept;
void	Style_SaveTabSettings(LPCEDITLEXER pLex) noexcept;
void	EditApplyDefaultEncoding(LPCEDITLEXER pLex, BOOL bLexerChanged) noexcept;
void	InitAutoCompletionCache(LPCEDITLEXER pLex) noexcept;

void	Style_DetectBaseFontSize(HMONITOR hMonitor) noexcept;
HFONT	Style_CreateCodeFont(UINT dpi) noexcept;
void	Style_OnDPIChanged(LPCEDITLEXER pLex) noexcept;
void	Style_OnStyleThemeChanged(int theme) noexcept;
void	Style_InitDefaultColor() noexcept;
void	Style_SetLexer(PEDITLEXER pLexNew, BOOL bLexerChanged) noexcept;
bool	Style_SetLexerFromFile(LPCWSTR lpszFile) noexcept;
void	Style_SetLexerFromName(LPCWSTR lpszFile, LPCWSTR lpszName) noexcept;
bool	Style_CanOpenFile(LPCWSTR lpszFile) noexcept;
void	Style_SetLexerFromID(int rid) noexcept;
int		Style_GetMatchLexerIndex(int rid) noexcept;

int		Style_GetDocTypeLanguage() noexcept;
LPCWSTR Style_GetCurrentLexerName(LPWSTR lpszName, int cchName) noexcept;
void	Style_SetLexerByLangIndex(int lang) noexcept;
void	Style_UpdateSchemeMenu(HMENU hmenu) noexcept;

void	Style_SetDefaultFont(HWND hwnd, bool bCode) noexcept;
void	Style_SetIndentGuides(bool bShow) noexcept;
void	Style_SetBookmark() noexcept;
void	Style_UpdateCaret() noexcept;
void	Style_SetLongLineColors() noexcept;
void	Style_HighlightCurrentLine() noexcept;
void	Style_ToggleUseDefaultCodeStyle() noexcept;
LPWSTR	Style_GetOpenDlgFilterStr(bool open, LPCWSTR lpszFile, int lexers[]) noexcept;

bool	Style_StrGetFontEx(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont, bool bDefaultStyle) noexcept;
bool	Style_StrGetCharSet(LPCWSTR lpszStyle, int *charset) noexcept;
BOOL	Style_StrGetLocale(LPCWSTR lpszStyle, LPWSTR lpszLocale, int cchLocale) noexcept;
bool	Style_StrGetFontSize(LPCWSTR lpszStyle, int *size) noexcept;
bool	Style_StrGetSize(LPCWSTR lpszStyle, int *size) noexcept;
bool	Style_StrGetFontWeight(LPCWSTR lpszStyle, int *weight) noexcept;
bool	Style_StrGetColor(bool bFore, LPCWSTR lpszStyle, COLORREF *rgb) noexcept;

inline bool Style_StrGetForeColor(LPCWSTR lpszStyle, COLORREF *rgb) noexcept {
	return Style_StrGetColor(true, lpszStyle, rgb);
}

inline bool Style_StrGetBackColor(LPCWSTR lpszStyle, COLORREF *rgb) noexcept {
	return Style_StrGetColor(false, lpszStyle, rgb);
}

bool	Style_StrGetAlphaEx(bool outline, LPCWSTR lpszStyle, int *alpha) noexcept;
inline bool Style_StrGetAlpha(LPCWSTR lpszStyle, int *alpha) noexcept {
	return Style_StrGetAlphaEx(false, lpszStyle, alpha);
}

inline bool Style_StrGetOutlineAlpha(LPCWSTR lpszStyle, int *alpha) noexcept {
	return Style_StrGetAlphaEx(true, lpszStyle, alpha);
}

bool	Style_SelectFont(HWND hwnd, LPWSTR lpszStyle, int cchStyle, bool bDefaultStyle) noexcept;
bool	Style_SelectColor(HWND hwnd, LPWSTR lpszStyle, int cchStyle, bool bFore) noexcept;
void	Style_SetStyles(int iStyle, LPCWSTR lpszStyle) noexcept;

int 	Style_GetLexerIconId(LPCEDITLEXER pLex, DWORD iconFlags) noexcept;
void	Style_ConfigDlg(HWND hwnd) noexcept;
void	Style_SelectLexerDlg(HWND hwnd, bool favorite) noexcept;
bool	SelectCSVOptionsDlg() noexcept;
