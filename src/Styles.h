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

#ifndef _NOTEPAD2_STYLES_H_
#define _NOTEPAD2_STYLES_H_

#include "EditLexer.h"
// Number of Lexers in pLexArray
#define NUMLEXERS 55

extern PEDITLEXER pLexCurrent;
extern int np2LexLangIndex;

__inline BOOL IsDocLowerKeywords(PEDITLEXER pLex)
{
	switch (pLex->rid) {
	case NP2LEX_HTML:		// HTML
	case NP2LEX_PHP:		// PHP
	case NP2LEX_VB:			// VB
	case NP2LEX_VBS:		// VBS
	case NP2LEX_CONF:		// Apache Config
	case NP2LEX_NSIS:
		return TRUE;
	default:
		return FALSE;
	}
}

__inline BOOL IsDocWordChar(int ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '.') {
		return TRUE;
	}
	switch (pLexCurrent->rid) {
	case NP2LEX_CPP:
		return (ch == '#' || ch == '@' || ch == ':');
	case NP2LEX_CSHARP:
		return (ch == '#' || ch == '@');
	case NP2LEX_JAVA:
		return (ch == '$' || ch == '@' || ch == ':');
	case NP2LEX_GROOVY:
	case NP2LEX_SCALA:
	case NP2LEX_PYTHON:
	case NP2LEX_PERL:
	case NP2LEX_SQL:
		return (ch == '$' || ch == '@');
	case NP2LEX_RC:
	case NP2LEX_D:
	case NP2LEX_HAXE:
	case NP2LEX_VB:
		return (ch == '#');
	case NP2LEX_PHP:
	case NP2LEX_HTML:
	case NP2LEX_PS1:
	case NP2LEX_BATCH:
	case NP2LEX_BASH:
		return (ch == '-' || ch == '$');
	case NP2LEX_MAKE:
		return (ch == '-' || ch == '$' || ch == '!');
	case NP2LEX_XML:
		return (ch == '-' || ch == ':');
	case NP2LEX_CSS:
	case NP2LEX_SMALI:
	case NP2LEX_LISP:
	case NP2LEX_DOT:
	case NP2LEX_ANSI:
		return (ch == '-');
	}
	return FALSE;
}

__inline BOOL IsOperatorStyle(int style) {
	switch (pLexCurrent->iLexer) {
	case SCLEX_CPP:
	case SCLEX_JSON:
	case SCLEX_NSIS:
	case SCLEX_GRAPHVIZ:
	case SCLEX_LISP:
		return style == SCE_C_OPERATOR;
	case SCLEX_SMALI:
		return style == SCE_SMALI_OPERATOR;
	case SCLEX_PYTHON:
		return style == SCE_PY_OPERATOR;
	case SCLEX_RUBY:
		return style == SCE_RB_OPERATOR;
	case SCLEX_SQL:
		return style == SCE_SQL_OPERATOR;
	case SCLEX_TCL:
		return style == SCE_TCL_OPERATOR;
	case SCLEX_VB:
	case SCLEX_VBSCRIPT:
		return style == SCE_B_OPERATOR;
	case SCLEX_VERILOG:
		return style == SCE_V_OPERATOR;
	case SCLEX_VHDL:
		return style == SCE_VHDL_OPERATOR;
	case SCLEX_PERL:
		return style == SCE_PL_OPERATOR;
	case SCLEX_MATLAB:
		return style == SCE_MAT_OPERATOR;
	case SCLEX_LUA:
		return style == SCE_LUA_OPERATOR;
	case SCLEX_ASM:
		return style == SCE_ASM_OPERATOR;
	case SCLEX_AU3:
		return style == SCE_AU3_OPERATOR;
	case SCLEX_BASH:
		return style == SCE_SH_OPERATOR;
	case SCLEX_BATCH:
		return style == SCE_BAT_OPERATOR;
	case SCLEX_CMAKE:
		return style == SCE_CMAKE_OPERATOR;
	case SCLEX_CONF:
		return style == SCE_CONF_OPERATOR;
	case SCLEX_CSS:
		return style == SCE_CSS_OPERATOR;
	case SCLEX_FORTRAN:
		return style == SCE_F_OPERATOR;
	case SCLEX_FSHARP:
		return style == SCE_FSHARP_OPERATOR;
	case SCLEX_HTML:
		return style == SCE_HPHP_OPERATOR || style == SCE_HJ_SYMBOLS || style == SCE_HJA_SYMBOLS;
	case SCLEX_LATEX:
	case SCLEX_TEXINFO:
		return style == SCE_L_OPERATOR;
	case SCLEX_MAKEFILE:
		return style == SCE_MAKE_OPERATOR;
	case SCLEX_PASCAL:
		return style == SCE_PAS_OPERATOR;
	case SCLEX_POWERSHELL:
		return style == SCE_POWERSHELL_OPERATOR;
	}
	return FALSE;
}

void	Style_Load();
void	Style_Save();
BOOL	Style_Import(HWND hwnd);
BOOL	Style_Export(HWND hwnd);

void	Style_SetLexer(HWND hwnd, PEDITLEXER pLexNew);
void	Style_SetLexerFromFile(HWND hwnd, LPCWSTR lpszFile);
void	Style_SetLexerFromName(HWND hwnd, LPCWSTR lpszFile, LPCWSTR lpszName);
void	Style_SetLexerFromID(HWND hwnd, int id);

int		Style_GetDocTypeLanguage();
void	Style_UpdateLexKeywords(PEDITLEXER pLexNew);
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

#endif _NOTEPAD2_STYLES_H_

// End of Style.h

