/******************************************************************************
*
*
* Notepad2
*
* Notepad2.h
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

#ifndef NOTEPAD2_H_
#define NOTEPAD2_H_

//==== Main Window ============================================================
#define WC_NOTEPAD2 L"Notepad2"

//==== Data Type for WM_COPYDATA ==============================================
#define DATA_NOTEPAD2_PARAMS 0xFB10
typedef struct np2params {
	int		flagFileSpecified;
	int		flagChangeNotify;
	int		flagLexerSpecified;
	int		iInitialLexer;
	int		flagQuietCreate;
	int		flagJumpTo;
	int		iInitialLine;
	int		iInitialColumn;
	int		iSrcEncoding;
	int		flagSetEncoding;
	int		flagSetEOLMode;
	int		flagTitleExcerpt;
	WCHAR wchData;
} NP2PARAMS, *LPNP2PARAMS;

//==== Toolbar Style ==========================================================
#define WS_TOOLBAR (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |				\
					TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_ALTDRAG |		\
					TBSTYLE_LIST | CCS_NODIVIDER | CCS_NOPARENTALIGN |		\
					CCS_ADJUSTABLE)

//==== ReBar Style ============================================================
#define WS_REBAR (WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | RBS_VARHEIGHT |	\
				  RBS_BANDBORDERS | CCS_NODIVIDER | CCS_NOPARENTALIGN)

//==== Ids ====================================================================
#define IDC_STATUSBAR		0xFB00
#define IDC_TOOLBAR			0xFB01
#define IDC_REBAR			0xFB02
#define IDC_EDIT			0xFB03
#define IDC_EDITFRAME		0xFB04
#define IDC_FILENAME		0xFB05
#define IDC_REUSELOCK		0xFB06

// submenu in popup menu, IDR_POPUPMENU
#define IDP_POPUP_SUBMENU_EDIT	0
#define IDP_POPUP_SUBMENU_BAR	1
#define IDP_POPUP_SUBMENU_TRAY	2
#define IDP_POPUP_SUBMENU_FOLD	3

//==== Statusbar ==============================================================
#define STATUS_DOCPOS		0
#define STATUS_LEXER		1
#define STATUS_CODEPAGE		2
#define STATUS_EOLMODE		3
#define STATUS_OVRMODE		4
#define STATUS_DOCZOOM		5
#define STATUS_DOCSIZE		6
#define STATUS_HELP			(255 | SBT_NOBORDERS)

//==== Change Notifications ===================================================
#define ID_WATCHTIMER		0xA000
#define APPM_CHANGENOTIFY	(WM_APP + 2)
//#define APPM_CHANGENOTIFYCLEAR	(WM_APP + 3)

//==== Callback Message from System Tray ======================================
#define APPM_TRAYMESSAGE	(WM_APP + 4)

//==== Paste Board Timer ======================================================
#define ID_PASTEBOARDTIMER	0xA001

//==== Reuse Window Lock Timeout ==============================================
#define REUSEWINDOWLOCKTIMEOUT 1000

// Settings Version
enum {
// No version
NP2SettingsVersion_None = 0,
/*
1. `ZoomLevel` and `PrintZoom` changed from relative font size in point to absolute percentage.
2. `HighlightCurrentLine` changed to outline frame of subline, regardless of any previous settings.
*/
NP2SettingsVersion_V1 = 1,
NP2SettingsVersion_Current = NP2SettingsVersion_V1,
};

#define INI_SECTION_NAME_NOTEPAD2				L"Notepad2"
#define INI_SECTION_NAME_SETTINGS				L"Settings"
#define INI_SECTION_NAME_FLAGS					L"Settings2"
#define INI_SECTION_NAME_WINDOW_POSITION		L"Window Position"
#define INI_SECTION_NAME_TOOLBAR_LABELS			L"Toolbar Labels"
#define INI_SECTION_NAME_TOOLBAR_IMAGES			L"Toolbar Images"
#define INI_SECTION_NAME_SUPPRESSED_MESSAGES	L"Suppressed Messages"

#define MRU_KEY_RECENT_FILES					L"Recent Files"
#define MRU_KEY_RECENT_FIND						L"Recent Find"
#define MRU_KEY_RECENT_REPLACE					L"Recent Replace"
#define MRU_MAX_RECENT_FILES					32
#define MRU_MAX_RECENT_FIND						32
#define MRU_MAX_RECENT_REPLACE					32

#define MAX_INI_SECTION_SIZE_SETTINGS			(8 * 1024)
#define MAX_INI_SECTION_SIZE_FLAGS				(4 * 1024)
#define MAX_INI_SECTION_SIZE_TOOLBAR_LABELS		(2 * 1024)

//==== Function Declarations ==================================================
BOOL InitApplication(HINSTANCE hInstance);
HWND InitInstance(HINSTANCE hInstance, int nCmdShow);
BOOL ActivatePrevInst(void);
void GetRelaunchParameters(LPWSTR szParameters, LPCWSTR lpszFile, BOOL newWind, BOOL emptyWind);
BOOL RelaunchMultiInst(void);
BOOL RelaunchElevated(void);
void SnapToDefaultPos(HWND hwnd);
void ShowNotifyIcon(HWND hwnd, BOOL bAdd);
void SetNotifyIconTitle(HWND hwnd);
void InstallFileWatching(LPCWSTR lpszFile);
void CALLBACK WatchTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void CALLBACK PasteBoardTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

void LoadSettings(void);
void SaveSettingsNow(BOOL bOnlySaveStyle, BOOL bQuiet);
void SaveSettings(BOOL bSaveSettingsNow);
void SaveWindowPosition(BOOL bSaveSettingsNow, WCHAR *pIniSectionBuf);
void ClearWindowPositionHistory(void);
void ParseCommandLine(void);
void LoadFlags(void);

BOOL CheckIniFile(LPWSTR lpszFile, LPCWSTR lpszModule);
BOOL CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule, LPCWSTR redirectKey);
BOOL FindIniFile(void);
BOOL TestIniFile(void);
BOOL CreateIniFile(void);
BOOL CreateIniFileEx(LPCWSTR lpszIniFile);
void FindExtraIniFile(LPWSTR lpszIniFile, LPCWSTR defaultName, LPCWSTR redirectKey);

void UpdateWindowTitle(void);
void UpdateStatusbar(void);
void UpdateStatusBarCache(int item);
void UpdateStatusBarWidth(void);
void UpdateToolbar(void);
void UpdateFoldMarginWidth(void);
void UpdateLineNumberWidth(void);

enum {
	FullScreenMode_OnStartup = 1,
	FullScreenMode_HideCaption = 2,
	FullScreenMode_HideMenu = 4,

	FullScreenMode_Default = FullScreenMode_HideCaption,
};
void ToggleFullScreenMode(void);

typedef struct EditFileIOStatus {
	int iEncoding;		// load output, save input
	int iEOLMode;		// load output

	BOOL bFileTooBig;	// load output
	BOOL bUnicodeErr;	// load output

	// inconsistent line endings
	BOOL bLineEndingsDefaultNo; // set default button to "No"
	BOOL bInconsistent;	// load output
	UINT linesCount[3];	// load output: CR+LF, LF, CR

	BOOL bCancelDataLoss;// save output
} EditFileIOStatus;

BOOL FileIO(BOOL fLoad, LPWSTR pszFile, BOOL bFlag, EditFileIOStatus *status);
BOOL FileLoad(BOOL bDontSave, BOOL bNew, BOOL bReload, BOOL bNoEncDetect, LPCWSTR lpszFile);
BOOL FileSave(BOOL bSaveAlways, BOOL bAsk, BOOL bSaveAs, BOOL bSaveCopy);
BOOL OpenFileDlg(HWND hwnd, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir);
BOOL SaveFileDlg(HWND hwnd, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir);

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);
void	CreateBars(HWND hwnd, HINSTANCE hInstance);
void	MsgDPIChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
void	MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
void	MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam);
void	MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT MsgNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);

#endif // NOTEPAD2_H_

// End of Notepad2.h
