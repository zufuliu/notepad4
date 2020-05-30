/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* Dialogs.h
*   Definitions for metapath dialog boxes
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

#include "compiler.h"

extern BOOL bWindowLayoutRTL;
NP2_inline void InitWindowCommon(HWND hwnd) {
	if (bWindowLayoutRTL) {
		SetWindowLayoutRTL(hwnd, TRUE);
	}
}

int MsgBox(UINT uType, UINT uIdMsg, ...);
#if defined(__GNUC__) || defined(__clang__)
#define MsgBoxInfo(uType, uIdMsg, ...)		MsgBox(MB_ICONINFORMATION | (uType), (uIdMsg), ##__VA_ARGS__)
#define MsgBoxWarn(uType, uIdMsg, ...)		MsgBox(MB_ICONEXCLAMATION | (uType), (uIdMsg), ##__VA_ARGS__)
#define MsgBoxAsk(uType, uIdMsg, ...)		MsgBox(MB_ICONQUESTION | (uType), (uIdMsg), ##__VA_ARGS__)
#else
#define MsgBoxInfo(uType, uIdMsg, ...)		MsgBox(MB_ICONINFORMATION | (uType), (uIdMsg), __VA_ARGS__)
#define MsgBoxWarn(uType, uIdMsg, ...)		MsgBox(MB_ICONEXCLAMATION | (uType), (uIdMsg), __VA_ARGS__)
#define MsgBoxAsk(uType, uIdMsg, ...)		MsgBox(MB_ICONQUESTION | (uType), (uIdMsg), __VA_ARGS__)
#endif

BOOL GetDirectory(HWND hwndParent, int iTitle, LPWSTR pszFolder, LPCWSTR pszBase);
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
BOOL GetDirectory2(HWND hwndParent, int iTitle, LPWSTR pszFolder, int iBase);
#else
BOOL GetDirectory2(HWND hwndParent, int iTitle, LPWSTR pszFolder, REFKNOWNFOLDERID iBase);
#endif

void RunDlg(HWND hwnd);
void GotoDlg(HWND hwnd);

void OpenHelpLink(HWND hwnd, int cmd);
INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
INT_PTR OptionsPropSheet(HWND hwnd, HINSTANCE hInstance);

BOOL GetFilterDlg(HWND hwnd);
BOOL RenameFileDlg(HWND hwnd);
BOOL CopyMoveDlg(HWND hwnd, UINT *wFunc);
BOOL OpenWithDlg(HWND hwnd, LPCDLITEM lpdliParam);
BOOL NewDirDlg(HWND hwnd, LPWSTR pszNewDir);

INT_PTR CALLBACK FindTargetDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
