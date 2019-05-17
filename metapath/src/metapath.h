/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* metapath.h
*   Global definitions and declarations
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

#ifndef METAPATH_H_
#define METAPATH_H_

//==== Main Window ============================================================
#define WC_METAPATH L"metapath"
#define MY_APPUSERMODELID	L"metapath File Browser"

#define WS_METAPATH ((WS_OVERLAPPEDWINDOW ^ \
					  (WS_MINIMIZEBOX | WS_MAXIMIZEBOX)) | \
					 (WS_CLIPCHILDREN | WS_POPUP))

//==== Data Type for WM_COPYDATA ==============================================
#define DATA_METAPATH_PATHARG 0xFB30

//==== ComboBox Control =======================================================
//#define WC_COMBOBOX L"ComboBox"
#define WS_DRIVEBOX (WS_CHILD | \
					 /*WS_VISIBLE |*/ \
					 WS_CLIPSIBLINGS | \
					 WS_VSCROLL | \
					 CBS_DROPDOWNLIST)

//==== Listview Control =======================================================
#define WS_DIRLIST (WS_CHILD | \
					WS_VISIBLE | \
					WS_CLIPSIBLINGS | \
					WS_CLIPCHILDREN | \
					LVS_REPORT | \
					LVS_NOCOLUMNHEADER | \
					LVS_SHAREIMAGELISTS | \
					LVS_AUTOARRANGE | \
					LVS_SINGLESEL | \
					LVS_SHOWSELALWAYS)

//==== Toolbar Style ==========================================================
#define WS_TOOLBAR (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | \
					TBSTYLE_TOOLTIPS | TBSTYLE_ALTDRAG | TBSTYLE_LIST | \
					CCS_NODIVIDER | CCS_NOPARENTALIGN)

//==== ReBar Style ============================================================
#define WS_REBAR (WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPCHILDREN | WS_BORDER | \
				  RBS_VARHEIGHT | RBS_BANDBORDERS /*| RBS_FIXEDORDER */ | \
				  CCS_NODIVIDER |/*CCS_NORESIZE | */CCS_NOPARENTALIGN)

//==== Ids ====================================================================
#define IDC_STATUSBAR 0x00A0
#define IDC_TOOLBAR   0x00A1
#define IDC_REBAR     0x00A2
#define IDC_DRIVEBOX  0xA000
#define IDC_DIRLIST   0xA001

// submenu in popup menu, IDR_MAINWND
#define IDP_POPUP_SUBMENU_PATH	0
#define IDP_POPUP_SUBMENU_LIST	1
#define IDP_POPUP_SUBMENU_DRIVE	2
#define IDP_POPUP_SUBMENU_BAR	3
#define IDP_POPUP_SUBMENU_TRAY	4

//==== Statusbar ==============================================================
#define ID_FILEINFO   0
#define ID_MENUHELP (255 | SBT_NOBORDERS)

//==== Timer for Change Notifications =========================================
#define ID_TIMER 0xA000

//==== Callback Message from System Tray ======================================
#define APPM_TRAYMESSAGE	(WM_APP + 4)

#define INI_SECTION_NAME_METAPATH			L"metapath"
#define INI_SECTION_NAME_SETTINGS			L"Settings"
#define INI_SECTION_NAME_FLAGS				L"Settings2"
#define INI_SECTION_NAME_WINDOW_POSITION	L"Window Position"
#define INI_SECTION_NAME_TOOLBAR_LABELS		L"Toolbar Labels"
#define INI_SECTION_NAME_TOOLBAR_IMAGES		L"Toolbar Images"
#define INI_SECTION_NAME_FILTERS			L"Filters"
#define INI_SECTION_NAME_TARGET_APPLICATION	L"Target Application"

#define MRU_KEY_COPY_MOVE_HISTORY			L"Copy/Move MRU"

#define MAX_INI_SECTION_SIZE_SETTINGS			(4 * 1024)
#define MAX_INI_SECTION_SIZE_FLAGS				(4 * 1024)
#define MAX_INI_SECTION_SIZE_TOOLBAR_LABELS		(2 * 1024)
#define MAX_INI_SECTION_SIZE_FILTERS			(4 * 1024)
#define MAX_INI_SECTION_SIZE_TARGET_APPLICATION	(4 * 1024)

//==== Function Declarations ==================================================
BOOL InitApplication(HINSTANCE hInstance);
HWND InitInstance(HINSTANCE hInstance, int nCmdShow);
BOOL ActivatePrevInst(void);
void GetRelaunchParameters(LPWSTR szParameters);
void ShowNotifyIcon(HWND hwnd, BOOL bAdd);

BOOL ChangeDirectory(HWND hwnd, LPCWSTR lpszNewDir, BOOL bUpdateHistory);
void LoadSettings(void);
void SaveSettingsNow(void);
void SaveSettings(BOOL bSaveSettingsNow);
void SaveWindowPosition(BOOL bSaveSettingsNow, WCHAR *pIniSectionBuf);
void ClearWindowPositionHistory(void);
void ParseCommandLine(void);
void LoadFlags(void);
void LoadLaunchSetings(void);

BOOL CheckIniFile(LPWSTR lpszFile, LPCWSTR lpszModule);
BOOL CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule);
BOOL FindIniFile(void);
BOOL TestIniFile(void);
BOOL CreateIniFile(void);
BOOL CreateIniFileEx(LPCWSTR lpszIniFile);

BOOL DisplayPath(LPCWSTR lpPath, UINT uIdError);
BOOL DisplayLnkFile(LPCWSTR pszLnkFile);

void LaunchTarget(LPCWSTR lpFileName, BOOL bOpenNew);
void SnapToTarget(HWND hwnd);
void SnapToDefaultPos(HWND hwnd);

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);
void CreateBars(HWND hwnd, HINSTANCE hInstance);
void MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
void MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam);
void MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT MsgNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);

#endif // METAPATH_H_

///   End of metapath.h
