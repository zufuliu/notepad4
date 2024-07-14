/******************************************************************************
*
*
* Notepad4
*
* Dialogs.h
*   Definitions for Notepad4 dialog boxes
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

extern bool bWindowLayoutRTL;
inline void InitWindowCommon(HWND hwnd) noexcept {
	if (bWindowLayoutRTL) {
		SetWindowLayoutRTL(hwnd, true);
	}
}

int 	MsgBox(UINT uType, UINT uIdMsg, ...) noexcept;
#define MsgBoxInfo(uType, uIdMsg, ...)		MsgBox(MB_ICONINFORMATION | (uType), (uIdMsg), ##__VA_ARGS__)
#define MsgBoxWarn(uType, uIdMsg, ...)		MsgBox(MB_ICONEXCLAMATION | (uType), (uIdMsg), ##__VA_ARGS__)
#define MsgBoxAsk(uType, uIdMsg, ...)		MsgBox(MB_ICONQUESTION | (uType), (uIdMsg), ##__VA_ARGS__)
#define MsgBoxLastError(uType, uIdMsg, ...)	MsgBox(MB_ICONEXCLAMATION | MB_SERVICE_NOTIFICATION | (uType), (uIdMsg), ##__VA_ARGS__)

void	DisplayCmdLineHelp(HWND hwnd) noexcept;
void	OpenHelpLink(HWND hwnd, int cmd) noexcept;
bool	GetDirectory(HWND hwndParent, int iTitle, LPWSTR pszFolder, LPCWSTR pszBase) noexcept;
INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) noexcept;
void	RunDlg(HWND hwnd, LPCWSTR lpstrDefault) noexcept;
bool	OpenWithDlg(HWND hwnd, LPCWSTR lpstrFile);
bool	FavoritesDlg(HWND hwnd, LPWSTR lpstrFile) noexcept;
bool	AddToFavDlg(HWND hwnd, LPCWSTR lpszName, LPCWSTR lpszTarget);
bool	FileMRUDlg(HWND hwnd, LPWSTR lpstrFile) noexcept;
bool	ChangeNotifyDlg(HWND hwnd) noexcept;
bool	ColumnWrapDlg(HWND hwnd) noexcept;
bool	WordWrapSettingsDlg(HWND hwnd) noexcept;
bool	LongLineSettingsDlg(HWND hwnd) noexcept;
bool	TabSettingsDlg(HWND hwnd) noexcept;
bool	SelectDefEncodingDlg(HWND hwnd, int *pidREncoding) noexcept;
bool	SelectEncodingDlg(HWND hwnd, int *pidREncoding, UINT uidLabel) noexcept;
bool	SelectDefLineEndingDlg(HWND hwnd, int *iOption) noexcept;

struct EditFileIOStatus;
bool	WarnLineEndingDlg(HWND hwnd, EditFileIOStatus *status) noexcept;
void	InitZoomLevelComboBox(HWND hwnd, int nCtlId, int zoomLevel) noexcept;
bool	GetZoomLevelComboBoxValue(HWND hwnd, int nCtrId, int *zoomLevel) noexcept;
void	ZoomLevelDlg(HWND hwnd, bool bBottom) noexcept;
bool	AutoCompletionSettingsDlg(HWND hwnd) noexcept;
bool	AutoSaveSettingsDlg(HWND hwnd) noexcept;

INT_PTR InfoBox(UINT uType, LPCWSTR lpstrSetting, UINT uidMessage, ...) noexcept;
#define InfoBoxInfo(uType, lpstrSetting, uidMessage, ...)	InfoBox(MB_ICONINFORMATION | (uType), (lpstrSetting), (uidMessage), ##__VA_ARGS__)
#define InfoBoxWarn(uType, lpstrSetting, uidMessage, ...)	InfoBox(MB_ICONEXCLAMATION | (uType), (lpstrSetting), (uidMessage), ##__VA_ARGS__)
#define InfoBoxAsk(uType, lpstrSetting, uidMessage, ...)	InfoBox(MB_ICONQUESTION | (uType), (lpstrSetting), (uidMessage), ##__VA_ARGS__)

void	SystemIntegrationDlg(HWND hwnd) noexcept;
