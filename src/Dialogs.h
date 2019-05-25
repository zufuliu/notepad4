/******************************************************************************
*
*
* Notepad2
*
* Dialogs.h
*   Definitions for Notepad2 dialog boxes
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

#ifndef NOTEPAD2_DIALOGS_H_
#define NOTEPAD2_DIALOGS_H_

#define MBINFO			0
#define MBWARN			1
#define MBYESNO			2
#define MBYESNOWARN		3
#define MBYESNOCANCEL	4
#define MBOKCANCEL		8

/**
 * App message used to center MessageBox to the window of the program.
 */
#define APPM_CENTER_MESSAGE_BOX		(WM_APP + 1)

extern BOOL bWindowLayoutRTL;
NP2_inline void InitWindowCommon(HWND hwnd) {
	if (bWindowLayoutRTL) {
		SetWindowLayoutRTL(hwnd, TRUE);
	}
}

int 	MsgBox(int iType, UINT uIdMsg, ...);
void	DisplayCmdLineHelp(HWND hwnd);
void	OpenHelpLink(HWND hwnd, int cmd);
BOOL	GetDirectory(HWND hwndParent, int iTitle, LPWSTR pszFolder, LPCWSTR pszBase);
INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
void	RunDlg(HWND hwnd, LPCWSTR lpstrDefault);
BOOL	OpenWithDlg(HWND hwnd, LPCWSTR lpstrFile);
BOOL	FavoritesDlg(HWND hwnd, LPWSTR lpstrFile);
BOOL	AddToFavDlg(HWND hwnd, LPCWSTR lpszName, LPCWSTR lpszTarget);
BOOL	FileMRUDlg(HWND hwnd, LPWSTR lpstrFile);
BOOL	ChangeNotifyDlg(HWND hwnd);
BOOL	ColumnWrapDlg(HWND hwnd, int *iNumber);
BOOL	WordWrapSettingsDlg(HWND hwnd, int *iNumber);
BOOL	LongLineSettingsDlg(HWND hwnd, int *iNumber);
BOOL	TabSettingsDlg(HWND hwnd);
BOOL	SelectDefEncodingDlg(HWND hwnd, int *pidREncoding);
BOOL	SelectEncodingDlg(HWND hwnd, int *pidREncoding);
BOOL	RecodeDlg(HWND hwnd, int *pidREncoding);
BOOL	SelectDefLineEndingDlg(HWND hwnd, int *iOption);

struct EditFileIOStatus;
BOOL	WarnLineEndingDlg(HWND hwnd, struct EditFileIOStatus *status);
void	InitZoomLevelComboBox(HWND hwnd, int nCtlId, int zoomLevel);
BOOL	GetZoomLevelComboBoxValue(HWND hwnd, int nCtrId, int *zoomLevel);
void	ZoomLevelDlg(HWND hwnd, BOOL bBottom);
BOOL	AutoCompletionSettingsDlg(HWND hwnd);
INT_PTR InfoBox(int iType, LPCWSTR lpstrSetting, UINT uidMessage, ...);

#endif // NOTEPAD2_DIALOGS_H_

// End of Dialogs.h
