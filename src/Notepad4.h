/******************************************************************************
*
*
* Notepad4
*
* Notepad4.h
*   Global definitions and declarations
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

//==== Main Window ============================================================
#define WC_NOTEPAD4 L"Notepad4"
#define MY_APPUSERMODELID	L"Notepad4 Text Editor"

enum TripleBoolean {
	TripleBoolean_False = 0,
	TripleBoolean_True,
	TripleBoolean_NotSet,
};

enum {
	ReadOnlyMode_None = 0,
	ReadOnlyMode_Current,
	ReadOnlyMode_AllFile,
};

enum FileWatchingMode {
	FileWatchingMode_None = 0,
	FileWatchingMode_ShowMessage,
	FileWatchingMode_AutoReload,
};

enum FileWatchingOption {
	FileWatchingOption_None = 0,
	FileWatchingOption_LogFile = 1,
	FileWatchingOption_KeepAtEnd = 2,
};

enum MatchTextFlag {
	MatchTextFlag_None = 0,
	MatchTextFlag_Default = 1,
	MatchTextFlag_FindUp = 2,
	MatchTextFlag_Regex = 4,
	MatchTextFlag_TransformBS = 8,
};

//==== Data Type for WM_COPYDATA ==============================================
#define DATA_NOTEPAD4_PARAMS 0xFB10
struct NP2PARAMS {
	bool	flagFileSpecified;
	bool	flagReadOnlyMode;
	bool	flagLexerSpecified;
	bool	flagQuietCreate;
	bool	flagTitleExcerpt;
	bool	flagJumpTo;
	TripleBoolean	flagChangeNotify;
	int		iInitialLexer;
	Sci_Line		iInitialLine;
	Sci_Position	iInitialColumn;
	int		iSrcEncoding;
	int		flagSetEncoding;
	int		flagSetEOLMode;
	MatchTextFlag flagMatchText;
	WCHAR wchData;
};

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
#define IDC_FILENAME		0xFB05

// submenu in popup menu, IDR_POPUPMENU
#define IDP_POPUP_SUBMENU_EDIT	0
#define IDP_POPUP_SUBMENU_BAR	1
#define IDP_POPUP_SUBMENU_TRAY	2
#define IDP_POPUP_SUBMENU_FOLD	3

//==== Statusbar ==============================================================
enum {
	StatusItem_Line,
	StatusItem_Column,
	StatusItem_Character,
	StatusItem_Selection,
	StatusItem_SelectedLine,
	StatusItem_Find,
	StatusItem_Empty,
	StatusItem_Lexer,
	StatusItem_Encoding,
	StatusItem_EolMode,
	StatusItem_OvrMode,
	StatusItem_Zoom,
	StatusItem_DocSize,
	StatusItem_ItemCount,
};
#define STATUS_HELP			(255 | SBT_NOBORDERS)

/**
 * App message used to center MessageBox to the window of the program.
 */
#define APPM_CENTER_MESSAGE_BOX		(WM_APP + 1)
#define APPM_CHANGENOTIFY			(WM_APP + 2)	// file change notifications
//#define APPM_CHANGENOTIFYCLEAR	(WM_APP + 3)
#define APPM_TRAYMESSAGE			(WM_APP + 4)	// callback message from system tray
#define APPM_POST_HOTSPOTCLICK		(WM_APP + 5)
// TODO: WM_COPYDATA is blocked by the User Interface Privilege Isolation
// https://www.codeproject.com/tips/1017834/how-to-send-data-from-one-process-to-another-in-cs
#define APPM_COPYDATA				(WM_APP + 6)
#define APPM_DROPFILES				(WM_APP + 7)	// ScintillaWin::Drop()

#define ID_WATCHTIMER				0xA000	// file watch timer
#define ID_PASTEBOARDTIMER			0xA001	// paste board timer
#define ID_AUTOSAVETIMER			0xA002	// AutoSave timer

enum EscFunction {
	EscFunction_None = 0,
	EscFunction_Minimize,
	EscFunction_Exit,
};

enum TitlePathNameFormat {
	TitlePathNameFormat_NameOnly = 0,
	TitlePathNameFormat_NameFirst,
	TitlePathNameFormat_FullPath,
};

enum PrintHeaderOption {
	PrintHeaderOption_FilenameAndDateTime = 0,
	PrintHeaderOption_FilenameAndDate,
	PrintHeaderOption_Filename,
	PrintHeaderOption_LeaveBlank,
};

enum PrintFooterOption {
	PrintFooterOption_PageNumber = 0,
	PrintFooterOption_LeaveBlank,
};

#define INI_SECTION_NAME_NOTEPAD4				L"Notepad4"
#define INI_SECTION_NAME_SETTINGS				L"Settings"
#define INI_SECTION_NAME_FLAGS					L"Settings2"
#define INI_SECTION_NAME_WINDOW_POSITION		L"Window Position"
#define INI_SECTION_NAME_TOOLBAR_LABELS			L"Toolbar Labels"
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

extern WCHAR szCurFile[MAX_PATH + 40];

//==== Function Declarations ==================================================
BOOL InitApplication(HINSTANCE hInstance) noexcept;
void InitInstance(HINSTANCE hInstance, int nCmdShow);
bool ActivatePrevInst() noexcept;
void GetRelaunchParameters(LPWSTR szParameters, LPCWSTR lpszFile, bool newWind, bool emptyWind) noexcept;
bool RelaunchMultiInst() noexcept;
bool RelaunchElevated() noexcept;
void SnapToDefaultPos(HWND hwnd) noexcept;
void ShowNotifyIcon(HWND hwnd, bool bAdd) noexcept;
void SetNotifyIconTitle(HWND hwnd) noexcept;

void ShowNotificationA(WPARAM notifyPos, LPCSTR lpszText) noexcept;
void ShowNotificationW(WPARAM notifyPos, LPCWSTR lpszText) noexcept;
void ShowNotificationMessage(WPARAM notifyPos, UINT uidMessage, ...) noexcept;

void InstallFileWatching(bool terminate) noexcept;
void CALLBACK WatchTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) noexcept;
void CALLBACK PasteBoardTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) noexcept;

void LoadSettings() noexcept;
void SaveSettingsNow(bool bOnlySaveStyle, bool bQuiet) noexcept;
void SaveSettings(bool bSaveSettingsNow) noexcept;
void SaveWindowPosition(WCHAR *pIniSectionBuf) noexcept;
void ClearWindowPositionHistory() noexcept;
void ParseCommandLine() noexcept;
void LoadFlags() noexcept;

bool CheckIniFile(LPWSTR lpszFile, LPCWSTR lpszModule) noexcept;
bool CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule, LPCWSTR redirectKey) noexcept;
bool FindIniFile() noexcept;
bool TestIniFile() noexcept;
bool CreateIniFile(LPCWSTR lpszIniFile) noexcept;
void FindExtraIniFile(LPWSTR lpszIniFile, LPCWSTR defaultName, LPCWSTR redirectKey) noexcept;

void UpdateWindowTitle() noexcept;
void UpdateStatusbar() noexcept;
void UpdateStatusBarCache(int item) noexcept;
void UpdateToolbar() noexcept;
void UpdateFoldMarginWidth() noexcept;
void UpdateLineNumberWidth() noexcept;
void UpdateBookmarkMarginWidth() noexcept;

enum {
	FullScreenMode_OnStartup = 1,
	FullScreenMode_HideCaption = 2,

	FullScreenMode_Default = FullScreenMode_HideCaption,
};

void ToggleFullScreenMode() noexcept;

struct EditFileIOStatus {
	int iEncoding;		// load output, save input
	int iEOLMode;		// load output

	bool bFileTooBig;	// load output
	bool bUnicodeErr;	// load output
	bool bBinaryFile;	// load output
	bool bCancelDataLoss;// save output

	// inconsistent line endings
	bool bLineEndingsDefaultNo; // set default button to "No"
	bool bInconsistent;	// load output
	Sci_Line totalLineCount; // load output, sum(linesCount) + 1
	Sci_Line linesCount[3];	// load output: CR+LF, LF, CR
};

enum FileLoadFlag {
	FileLoadFlag_Default = 0,
	FileLoadFlag_DontSave = 1,
	FileLoadFlag_New = 2,
	FileLoadFlag_Reload = 4,
};

enum FileSaveFlag {
	FileSaveFlag_Default = 0,
	FileSaveFlag_SaveAlways = 1,
	FileSaveFlag_Ask = 2,
	FileSaveFlag_SaveAs = 4,
	FileSaveFlag_SaveCopy = 8,
	FileSaveFlag_EndSession = 16,
};

bool FileIO(bool fLoad, LPWSTR pszFile, FileSaveFlag flag, EditFileIOStatus &status) noexcept;
bool FileLoad(FileLoadFlag loadFlag, LPCWSTR lpszFile);
bool FileSave(FileSaveFlag saveFlag) noexcept;
BOOL OpenFileDlg(LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) noexcept;
BOOL SaveFileDlg(bool Untitled, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) noexcept;

enum {
	AutoSaveOption_None = 0,
	AutoSaveOption_Periodic = 1,
	AutoSaveOption_Suspend = 2,
	AutoSaveOption_Shutdown = 4,
	AutoSaveOption_ManuallyDelete = 8,
	AutoSaveOption_OverwriteCurrent = 16,
	AutoSaveOption_Default = AutoSaveOption_Suspend | AutoSaveOption_Shutdown,
	AutoSaveDefaultPeriod = 5000,
};

void	AutoSave_Start(bool reset) noexcept;
void	AutoSave_Stop(BOOL keepBackup) noexcept;
void	AutoSave_DoWork(FileSaveFlag saveFlag) noexcept;
LPCWSTR AutoSave_GetDefaultFolder() noexcept;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept;
void	CreateBars(HWND hwnd, HINSTANCE hInstance) noexcept;
void	MsgDPIChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept;
void	MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept;
void	MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept;
void	MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept;
LRESULT MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT MsgNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);
