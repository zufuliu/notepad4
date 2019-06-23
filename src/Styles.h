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

#ifndef NOTEPAD2_STYLES_H_
#define NOTEPAD2_STYLES_H_

#include "EditLexer.h"
// Number of Lexers in pLexArray
#define NUMLEXERS 58

// all schemes with "All Files (*.*)"
#define MAX_OPEN_SAVE_FILE_DIALOG_FILTER_SIZE	((NUMLEXERS + 1) * 128)

#define INI_SECTION_NAME_STYLES				L"Styles"
#define INI_SECTION_NAME_FILE_EXTENSIONS	L"File Extensions"
#define INI_SECTION_NAME_CUSTOM_COLORS		L"Custom Colors"

#define MAX_INI_SECTION_SIZE_STYLES			(8 * 1024)

/**
 * used in ParseCommandLine() for option /d, /h and /x.
 */
#define EditLexer_Default	0
#define EditLexer_HTML		1
#define EditLexer_XML		2

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
void	Style_SetLexer(PEDITLEXER pLexNew);
BOOL	Style_SetLexerFromFile(LPCWSTR lpszFile);
void	Style_SetLexerFromName(LPCWSTR lpszFile, LPCWSTR lpszName);
BOOL	Style_MaybeBinaryFile(LPCWSTR lpszFile);
BOOL	Style_CanOpenFile(LPCWSTR lpszFile);
void	Style_SetLexerFromID(int id);
int		Style_GetEditLexerId(int lexer);

int		Style_GetDocTypeLanguage(void);
void	Style_UpdateLexerKeywords(LPCEDITLEXER pLexNew);
void	Style_UpdateLexerKeywordAttr(LPCEDITLEXER pLexNew);
#if NP2_GET_LEXER_STYLE_NAME_FROM_RES
LPCWSTR Style_GetCurrentLexerDisplayName(LPWSTR lpszName, int cchName);
#endif
LPCWSTR Style_GetCurrentLexerName(void);
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
BOOL	Style_GetOpenDlgFilterStr(LPWSTR lpszFilter, int cchFilter);

BOOL	Style_StrGetFontEx(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont, BOOL bDefaultStyle);
BOOL	Style_StrGetCharSet(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetLocale(LPCWSTR lpszStyle, LPWSTR lpszLocale, int cchLocale);
BOOL	Style_StrGetFontSize(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetRawSize(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetFontWeight(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetColor(BOOL bFore, LPCWSTR lpszStyle, int *rgb);
BOOL	Style_StrGetCase(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetAlpha(LPCWSTR lpszStyle, int *i);
BOOL	Style_SelectFont(HWND hwnd, LPWSTR lpszStyle, int cchStyle, BOOL bDefaultStyle);
BOOL	Style_SelectColor(HWND hwnd, BOOL bFore, LPWSTR lpszStyle, int cchStyle);
void	Style_SetStyles(int iStyle, LPCWSTR lpszStyle);

int 	Style_GetLexerIconId(LPCEDITLEXER pLex);
void	Style_ConfigDlg(HWND hwnd);
void	Style_SelectLexerDlg(HWND hwnd);

#endif // NOTEPAD2_STYLES_H_

// End of Style.h
