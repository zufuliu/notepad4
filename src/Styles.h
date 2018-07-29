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
#define NUMLEXERS 57

/**
 * used in ParseCommandLine() for option /d, /h and /x.
 */
#define EditLexer_Default	0
#define EditLexer_HTML		1
#define EditLexer_XML		2

extern PEDITLEXER pLexCurrent;
extern int np2LexLangIndex;
extern UINT8 currentLexKeywordAttr[NUMKEYWORD];

BOOL IsDocWordChar(int ch);

void	Style_Load(void);
void	Style_Save(void);
BOOL	Style_Import(HWND hwnd);
BOOL	Style_Export(HWND hwnd);

void	Style_OnDPIChanged(HWND hwnd);
void	Style_SetLexer(HWND hwnd, PEDITLEXER pLexNew);
void	Style_SetLexerFromFile(HWND hwnd, LPCWSTR lpszFile);
void	Style_SetLexerFromName(HWND hwnd, LPCWSTR lpszFile, LPCWSTR lpszName);
void	Style_SetLexerFromID(HWND hwnd, int id);
int		Style_GetEditLexerId(int lexer);

int		Style_GetDocTypeLanguage();
void	Style_UpdateLexerKeywords(PEDITLEXER pLexNew);
void	Style_UpdateLexerKeywordAttr(PEDITLEXER pLexNew);
void	Style_GetCurrentLexerName(LPWSTR lpszName, int cchName);
void	Style_SetDefaultLexer(HWND hwnd);
void	Style_SetConfLexer(HWND hwnd);
void	Style_SetHTMLLexer(HWND hwnd);
void	Style_SetXMLLexer(HWND hwnd);

void	Style_SetDefaultFont(HWND hwnd);
void	Style_SetIndentGuides(HWND hwnd, BOOL bShow);
void	Style_SetLongLineColors(HWND hwnd);
void	Style_SetCurrentLineBackground(HWND hwnd);
void	Style_ToggleUse2ndDefault(HWND hwnd);
BOOL	Style_GetUse2ndDefault(HWND hwnd);
BOOL	Style_GetOpenDlgFilterStr(LPWSTR lpszFilter, int cchFilter);

BOOL	Style_StrGetFont(LPCWSTR lpszStyle, LPWSTR lpszFont, int cchFont);
BOOL	Style_StrGetFontQuality(LPCWSTR lpszStyle, LPWSTR lpszQuality, int cchQuality);
BOOL	Style_StrGetCharSet(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetSizeEx(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetSize(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetSizeStr(LPCWSTR lpszStyle, LPWSTR lpszSize, int cchSize);
BOOL	Style_StrGetColor(BOOL bFore, LPCWSTR lpszStyle, int *rgb);
BOOL	Style_StrGetCase(LPCWSTR lpszStyle, int *i);
BOOL	Style_StrGetAlpha(LPCWSTR lpszStyle, int *i);
BOOL	Style_SelectFont(HWND hwnd, LPWSTR lpszStyle, int cchStyle, BOOL bDefaultStyle);
BOOL	Style_SelectColor(HWND hwnd, BOOL bFore, LPWSTR lpszStyle, int cchStyle);
void	Style_SetStyles(HWND hwnd, int iStyle, LPCWSTR lpszStyle);
void	Style_SetFontQuality(HWND hwnd, LPCWSTR lpszStyle);

int 				Style_GetLexerIconId(PEDITLEXER pLex);
INT_PTR CALLBACK	Style_ConfigDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
void				Style_ConfigDlg(HWND hwnd);
INT_PTR CALLBACK	Style_SelectLexerDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
void				Style_SelectLexerDlg(HWND hwnd);

#endif // NOTEPAD2_STYLES_H_

// End of Style.h
