/******************************************************************************
*
*
* matepath - The universal Explorer-like Plugin
*
* Dialogs.h
*   Definitions for matepath dialog boxes
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

#include "compiler.h"

extern bool bWindowLayoutRTL;
inline void InitWindowCommon(HWND hwnd) noexcept {
	if (bWindowLayoutRTL) {
		SetWindowLayoutRTL(hwnd, true);
	}
}

int MsgBox(UINT uType, UINT uIdMsg, ...) noexcept;
#define MsgBoxInfo(uType, uIdMsg, ...)		MsgBox(MB_ICONINFORMATION | (uType), (uIdMsg), ##__VA_ARGS__)
#define MsgBoxWarn(uType, uIdMsg, ...)		MsgBox(MB_ICONEXCLAMATION | (uType), (uIdMsg), ##__VA_ARGS__)
#define MsgBoxAsk(uType, uIdMsg, ...)		MsgBox(MB_ICONQUESTION | (uType), (uIdMsg), ##__VA_ARGS__)

bool GetDirectory(HWND hwndParent, int iTitle, LPWSTR pszFolder, LPCWSTR pszBase) noexcept;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
bool GetDirectory2(HWND hwndParent, int iTitle, LPWSTR pszFolder, REFKNOWNFOLDERID iBase) noexcept;
#else
bool GetDirectory2(HWND hwndParent, int iTitle, LPWSTR pszFolder, int iBase) noexcept;
#endif

void RunDlg(HWND hwnd) noexcept;
void GotoDlg(HWND hwnd) noexcept;

void OpenHelpLink(HWND hwnd, int cmd) noexcept;
INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) noexcept;
INT_PTR OptionsPropSheet(HWND hwnd, HINSTANCE hInstance) noexcept;

bool GetFilterDlg(HWND hwnd) noexcept;
bool RenameFileDlg(HWND hwnd);
bool CopyMoveDlg(HWND hwnd, UINT *wFunc);
bool OpenWithDlg(HWND hwnd, const DirListItem *lpdliParam);
bool NewDirDlg(HWND hwnd, LPWSTR pszNewDir) noexcept;

INT_PTR CALLBACK FindTargetDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) noexcept;
