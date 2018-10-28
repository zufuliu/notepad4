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

#ifndef METAPATH_DIALOGS_H_
#define METAPATH_DIALOGS_H_
#include "compiler.h"

/**
 * App message used to center MessageBox to the window of the program.
 */
#define APPM_CENTER_MESSAGE_BOX		(WM_APP + 1)

int  ErrorMessage(int iLevel, UINT uIdMsg, ...);
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
BOOL OpenWithDlg(HWND hwnd, LPDLITEM lpdliParam);
BOOL NewDirDlg(HWND hwnd, LPWSTR pszNewDir);

INT_PTR CALLBACK FindTargetDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

#endif // METAPATH_DIALOGS_H_

// End of Dialogs.h
