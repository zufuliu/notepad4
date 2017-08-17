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
} NP2PARAMS,  *LPNP2PARAMS;

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

//==== Statusbar ==============================================================
#define STATUS_DOCPOS		0
#define STATUS_DOCSIZE		1
#define STATUS_CODEPAGE		2
#define STATUS_EOLMODE		3
#define STATUS_OVRMODE		4
#define STATUS_LEXER		5
#define STATUS_HELP			255

//==== Change Notifications ===================================================
#define ID_WATCHTIMER		0xA000
#define WM_CHANGENOTIFY		WM_USER+1
//#define WM_CHANGENOTIFYCLEAR WM_USER+2

//==== Callback Message from System Tray ======================================
#define WM_TRAYMESSAGE		WM_USER

//==== Paste Board Timer ======================================================
#define ID_PASTEBOARDTIMER	0xA001

//==== Reuse Window Lock Timeout ==============================================
#define REUSEWINDOWLOCKTIMEOUT 1000

//==== Function Declarations ==================================================
BOOL InitApplication(HINSTANCE hInstance);
HWND InitInstance(HINSTANCE hInstance, LPSTR pszCmdLine, int nCmdShow);
BOOL ActivatePrevInst(void);
void GetRelaunchParameters(LPWSTR szParameters, BOOL newWind, BOOL emptyWind);
BOOL RelaunchMultiInst(void);
BOOL RelaunchElevated(void);
void SnapToDefaultPos(HWND hwnd);
void ShowNotifyIcon(HWND hwnd, BOOL bAdd);
void SetNotifyIconTitle(HWND hwnd);
void InstallFileWatching(LPCWSTR lpszFile);
void CALLBACK WatchTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void CALLBACK PasteBoardTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

void LoadSettings(void);
void SaveSettings(BOOL);
void ParseCommandLine(void);
void LoadFlags(void);
int  CheckIniFile(LPWSTR lpszFile, LPCWSTR lpszModule);
int  CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule);
int  FindIniFile(void);
int  TestIniFile(void);
int  CreateIniFile(void);
int  CreateIniFileEx(LPCWSTR lpszIniFile);

void UpdateStatusbar(void);
void UpdateToolbar(void);
void UpdateLineNumberWidth(void);

BOOL FileIO(BOOL fLoad, LPCWSTR psz, BOOL bNoEncDetect, int *ienc, int *ieol,
			BOOL *pbUnicodeErr, BOOL *pbFileTooBig,
			BOOL *pbCancelDataLoss, BOOL bSaveCopy);
BOOL FileLoad(BOOL bDontSave, BOOL bNew, BOOL bReload, BOOL bNoEncDetect, LPCWSTR lpszFile);
BOOL FileSave(BOOL bSaveAlways, BOOL bAsk, BOOL bSaveAs, BOOL bSaveCopy);
BOOL OpenFileDlg(HWND hwnd, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir);
BOOL SaveFileDlg(HWND hwnd, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir);

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam);
void	CreateBars(HWND hwnd, HINSTANCE hInstance);
void	MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);
void	MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam);
void	MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT MsgNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);

#endif // NOTEPAD2_H_

// End of Notepad2.h
