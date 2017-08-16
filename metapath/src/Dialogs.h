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

/**
 * App message used to center MessageBox to the window of the program.
 * https://stackoverflow.com/questions/6299797/c-how-to-center-messagebox
 */
#define APPM_CENTER_MESSAGE_BOX		(WM_APP + 1)

int  ErrorMessage(int iLevel, UINT uIdMsg, ...);
BOOL GetDirectory(HWND hwndParent, int iTitle, LPWSTR pszFolder, LPCWSTR pszBase, BOOL bNewDialogStyle);
BOOL GetDirectory2(HWND hwndParent, int iTitle, LPWSTR pszFolder, int iBase);

INT_PTR CALLBACK RunDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
void RunDlg(HWND hwnd);

INT_PTR CALLBACK GotoDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
void GotoDlg(HWND hwnd);

INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
INT_PTR OptionsPropSheet(HWND, HINSTANCE);

INT_PTR CALLBACK GetFilterDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL GetFilterDlg(HWND hwnd);

INT_PTR CALLBACK RenameFileDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL RenameFileDlg(HWND hwnd);

INT_PTR CALLBACK CopyMoveDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL CopyMoveDlg(HWND hwnd, UINT *wFunc);

INT_PTR CALLBACK OpenWithDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL OpenWithDlg(HWND hwnd, LPDLITEM lpdliParam);

INT_PTR CALLBACK NewDirDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
BOOL NewDirDlg(HWND hwnd, LPWSTR pszNewDir);

INT_PTR CALLBACK FindWinDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FindTargetDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

#endif // METAPATH_DIALOGS_H_

// End of Dialogs.h
