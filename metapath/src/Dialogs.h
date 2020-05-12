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

#define MBINFO			0
#define MBWARN			1
#define MBYESNO			2
#define MBYESNOWARN		3
#define MBYESNOCANCEL	4
#define MBOKCANCEL		8

extern BOOL bWindowLayoutRTL;
NP2_inline void InitWindowCommon(HWND hwnd) {
	if (bWindowLayoutRTL) {
		SetWindowLayoutRTL(hwnd, TRUE);
	}
}
int MsgBox(int iType, UINT uIdMsg, ...);
BOOL GetDirectory(HWND hwndParent, int iTitle, LPWSTR pszFolder, LPCWSTR pszBase);
BOOL GetDirectory2(HWND hwndParent, int iTitle, LPWSTR pszFolder, int iBase);

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
