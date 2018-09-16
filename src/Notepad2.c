/******************************************************************************
*
*
* Notepad2
*
* Notepad2.c
*   Main application window functionality
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

#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
#include "Helpers.h"
#include "SciCall.h"
#include "resource.h"

// show fold level
#define NP2_DEBUG_FOLD_LEVEL	0
// enable the .LOG feature
#define NP2_ENABLE_DOT_LOG_FEATURE	0
// enable call tips (currently not yet implemented)
#define NP2_ENABLE_SHOW_CALL_TIPS	0
// enable customize toolbar labels
#define NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS		0

/******************************************************************************
*
* Local and global Variables for Notepad2.c
*
*/
HWND	hwndStatus;
HWND	hwndToolbar;
HWND	hwndReBar;
HWND	hwndEdit;
HWND	hwndEditFrame;
HWND	hwndMain;
HWND	hwndNextCBChain = NULL;
HWND	hDlgFindReplace = NULL;

#define NUMTOOLBITMAPS		25
#define NUMINITIALTOOLS		24
#define MARGIN_LINE_NUMBER	0	// line number
#define MARGIN_SELECTION	1	// selection margin
#define MARGIN_FOLD_INDEX	2	// folding index

#define DefaultToolbarButtons	L"22 3 0 1 2 0 4 18 19 0 5 6 0 7 20 8 9 0 10 11 0 12 0 24 0 13 14 0 15 16 17 0"
static TBBUTTON  tbbMainWnd[] = {
	{0, 	IDT_FILE_NEW, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{1, 	IDT_FILE_OPEN, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{2, 	IDT_FILE_BROWSE, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{3, 	IDT_FILE_SAVE, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{4, 	IDT_EDIT_UNDO, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{5, 	IDT_EDIT_REDO, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{6, 	IDT_EDIT_CUT, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{7, 	IDT_EDIT_COPY, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{8, 	IDT_EDIT_PASTE, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{9, 	IDT_EDIT_FIND, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{10, 	IDT_EDIT_REPLACE, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{11, 	IDT_VIEW_WORDWRAP, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{12, 	IDT_VIEW_ZOOMIN, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{13, 	IDT_VIEW_ZOOMOUT, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{14, 	IDT_VIEW_SCHEME, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{15, 	IDT_VIEW_SCHEMECONFIG, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{16, 	IDT_FILE_EXIT, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{17, 	IDT_FILE_SAVEAS, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{18, 	IDT_FILE_SAVECOPY, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{19, 	IDT_EDIT_CLEAR, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{20, 	IDT_FILE_PRINT, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{21, 	IDT_FILE_OPENFAV, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{22, 	IDT_FILE_ADDTOFAV, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{23, 	IDT_VIEW_TOGGLEFOLDS, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{24, 	IDT_FILE_LAUNCH, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0}
};

WCHAR	szIniFile[MAX_PATH] = L"";
WCHAR	szIniFile2[MAX_PATH] = L"";
BOOL	bSaveSettings;
BOOL	bSaveRecentFiles;
BOOL	bSaveFindReplace;
WCHAR	tchLastSaveCopyDir[MAX_PATH] = L"";
WCHAR	tchOpenWithDir[MAX_PATH];
WCHAR	tchFavoritesDir[MAX_PATH];
WCHAR	tchDefaultDir[MAX_PATH];
WCHAR	tchDefaultExtension[64];
WCHAR	tchFileDlgFilters[5 * 1024];
WCHAR	tchToolbarButtons[512];
WCHAR	tchToolbarBitmap[MAX_PATH];
WCHAR	tchToolbarBitmapHot[MAX_PATH];
WCHAR	tchToolbarBitmapDisabled[MAX_PATH];
int		iPathNameFormat;
BOOL	fWordWrap;
BOOL	fWordWrapG;
int		iWordWrapMode;
int		iWordWrapIndent;
int		iWordWrapSymbols;
BOOL	bShowWordWrapSymbols;
BOOL	bMatchBraces;
BOOL	bAutoIndent;
BOOL	bAutoCloseTags;
BOOL	bShowIndentGuides;
BOOL	bHiliteCurrentLine;
BOOL	bTabsAsSpaces;
BOOL	bTabsAsSpacesG;
BOOL	bTabIndents;
BOOL	bTabIndentsG;
BOOL	bBackspaceUnindents;
int		iZoomLevel = 100;
int		iTabWidth;
int		iTabWidthG;
int		iIndentWidth;
int		iIndentWidthG;
BOOL	bMarkLongLines;
int		iLongLinesLimit;
int		iLongLinesLimitG;
int		iLongLineMode;
int		iWrapCol = 0;
BOOL	bShowSelectionMargin;
BOOL	bShowLineNumbers;
int		iMarkOccurrences;
BOOL	bMarkOccurrencesMatchCase;
BOOL	bMarkOccurrencesMatchWords;
BOOL 	bAutoCompleteWords;
int		iAutoCDefaultShowItemCount = 15;
int		iAutoCMinWordLength = 1;
int		iAutoCMinNumberLength = 3;
BOOL	bAutoCIncludeDocWord;
BOOL	bAutoCEnglishIMEModeOnly;
BOOL	bAutoCloseBracesQuotes;
BOOL	bShowCodeFolding;
#if NP2_ENABLE_SHOW_CALL_TIPS
BOOL	bShowCallTips = FALSE;
int		iCallTipsWaitTime = 500; // 500 ms
#endif
BOOL	bViewWhiteSpace;
BOOL	bViewEOLs;
int		iDefaultEncoding;
BOOL	bSkipUnicodeDetection;
BOOL	bLoadASCIIasUTF8;
BOOL	bLoadNFOasOEM;
BOOL	bNoEncodingTags;
int		iSrcEncoding = -1;
int		iWeakSrcEncoding = -1;
int		iDefaultEOLMode;
BOOL	bFixLineEndings;
BOOL	bAutoStripBlanks;
int		iPrintHeader;
int		iPrintFooter;
int		iPrintColor;
int		iPrintZoom = 100;
RECT	pagesetupMargin;
BOOL	bSaveBeforeRunningTools;
int		iFileWatchingMode;
BOOL	bResetFileWatching;
DWORD	dwFileCheckInverval;
DWORD	dwAutoReloadTimeout;
DWORD	dwFileLoadWarningMB;
int		iEscFunction;
BOOL	bAlwaysOnTop;
BOOL	bMinimizeToTray;
BOOL	bTransparentMode;
int		iRenderingTechnology;
BOOL	bUseInlineIME;
BOOL	bInlineIMEUseBlockCaret;
int		iBidirectional;
BOOL	bShowToolbar;
BOOL	bShowStatusbar;
BOOL	bInFullScreenMode;
BOOL	bFullScreenOnStartup;
BOOL	bFullScreenHideMenu;
BOOL	bFullScreenHideToolbar;
BOOL	bFullScreenHideStatusbar;

typedef struct _wi {
	int x;
	int y;
	int cx;
	int cy;
	int max;
} WININFO;

WININFO wi;
BOOL	bStickyWinPos;
BOOL	bIsAppThemed;

int		cyReBar;
int		cyReBarFrame;
int		cxEditFrame;
int		cyEditFrame;

int		cxEncodingDlg;
int		cyEncodingDlg;
int		cxRecodeDlg;
int		cyRecodeDlg;
int		cxFileMRUDlg;
int		cyFileMRUDlg;
int		cxOpenWithDlg;
int		cyOpenWithDlg;
int		cxFavoritesDlg;
int		cyFavoritesDlg;
int		xFindReplaceDlg;
int		yFindReplaceDlg;

LPWSTR	lpFileList[32];
int		cFileList = 0;
int		cchiFileList = 0;
LPWSTR	lpFileArg = NULL;
LPWSTR	lpSchemeArg = NULL;
LPWSTR	lpMatchArg = NULL;
LPWSTR	lpEncodingArg = NULL;
LPMRULIST	pFileMRU;
LPMRULIST	mruFind;
LPMRULIST	mruReplace;

DWORD	dwLastIOError;
WCHAR	szCurFile[MAX_PATH + 40];
FILEVARS	fvCurFile;
BOOL	bModified;
BOOL	bReadOnly = FALSE;
int		iOriginalEncoding;
int		iEOLMode;

int		iEncoding;
// DBCS code page
int		iDefaultCodePage;
int		iDefaultCharSet;

int		iInitialLine;
int		iInitialColumn;

int		iInitialLexer;

BOOL	bLastCopyFromMe = FALSE;
DWORD	dwLastCopyTime;

UINT	uidsAppTitle = IDS_APPTITLE;
WCHAR	szTitleExcerpt[128] = L"";
int		fKeepTitleExcerpt = 0;

HANDLE	hChangeHandle = NULL;
BOOL	bRunningWatch = FALSE;
DWORD	dwChangeNotifyTime = 0;

UINT	msgTaskbarCreated = 0;

HMODULE		hModUxTheme = NULL;

WIN32_FIND_DATA		fdCurFile;
static EDITFINDREPLACE efrData;
UINT	cpLastFind = 0;
BOOL	bReplaceInitialized = FALSE;

extern const NP2ENCODING mEncoding[];

const int iLineEndings[3] = {
	SC_EOL_CRLF,
	SC_EOL_LF,
	SC_EOL_CR
};

WCHAR	wchPrefixSelection[256] = L"";
WCHAR	wchAppendSelection[256] = L"";

WCHAR	wchPrefixLines[256] = L"";
WCHAR	wchAppendLines[256] = L"";

int		iSortOptions = 0;
int		iAlignMode	 = 0;
int		iMatchesCount = 0;
int		iAutoCItemCount = 0;
extern int iFontQuality;
extern int iCaretStyle;
extern int iCaretBlinkPeriod;

BOOL	fIsElevated = FALSE;
WCHAR	wchWndClass[16] = WC_NOTEPAD2;

// rarely changed statusbar items
struct CachedStatusItem {
	WCHAR tchZoom[16];

	LPCWSTR pszLexerName;
	LPCWSTR pszEOLMode;
	LPCWSTR pszOvrMode;

	BOOL lexerNameChanged;	// STATUS_LEXER
	BOOL encodingChanged;	// STATUS_CODEPAGE
	BOOL eolModeChanged;	// STATUS_EOLMODE
	BOOL ovrModeChanged;	// STATUS_OVRMODE
	BOOL zoomChanged;		// STATUS_DOCZOOM
} cachedStatusItem;

HINSTANCE	g_hInstance;
HANDLE		g_hDefaultHeap;
HANDLE		g_hScintilla;
UINT16		g_uWinVer;
UINT		g_uCurrentDPI = USER_DEFAULT_SCREEN_DPI;
UINT		g_uCurrentPPI = USER_DEFAULT_SCREEN_DPI;
WCHAR		g_wchAppUserModelID[38] = L"";
WCHAR		g_wchWorkingDirectory[MAX_PATH] = L"";

#define	NP2_BookmarkLineForeColor	(0xff << 8)
#define NP2_BookmarkLineColorAlpha	40

#ifdef BOOKMARK_EDITION
//Graphics for bookmark indicator
/* XPM */
static char *bookmark_pixmap[] = {
	"11 11 44 1",
	" 	c #EBE9ED",
	".	c #E5E3E7",
	"+	c #767C6D",
	"@	c #2A3120",
	"#	c #1B2312",
	"$	c #333B28",
	"%	c #E3E1E5",
	"&	c #D8D6DA",
	"*	c #444D38",
	"=	c #3F5C19",
	"-	c #63AD00",
	";	c #73C900",
	">	c #64AF00",
	", 	c #3D5718",
	"'	c #3E4634",
	")	c #7B8172",
	"!	c #42601A",
	"~	c #74CB00",
	"{	c #71C600",
	"]	c #3A5317",
	"^	c #707668",
	"/	c #3F4931",
	"(	c #262C1D",
	"_	c #2F3A1E",
	":	c #72C700",
	"<	c #74CA00",
	"[	c #0E1109",
	"}	c #3C462F",
	"|	c #62AC00",
	"1	c #21271A",
	"2	c #7A8071",
	"3	c #405D19",
	"4	c #3D5A18",
	"5	c #D9D7DB",
	"6	c #4E5841",
	"7	c #72C800",
	"8	c #63AC00",
	"9	c #3F5B19",
	"0	c #3D4533",
	"a	c #DFDDE0",
	"b	c #353E29",
	"c	c #29331B",
	"d	c #7B8272",
	"e	c #DDDBDF",
	"           ",
	"  .+@#$+%  ",
	" &*=-;>, '  ",
	")!~~~~{]^ ",
	" /-~~~~~>(",
	" _:~~~~~<[ ",
	" }|~~~~~|1 ",
	" 23~~~~;4+ ",
	" 56=|7890  ",
	"  a2bc}de  ",
	"           "
};
#endif

//=============================================================================
//
// Flags
//
int		flagNoReuseWindow		= 0;
int		flagReuseWindow			= 0;
BOOL	bSingleFileInstance		= TRUE;
BOOL	bReuseWindow			= FALSE;
int		flagMultiFileArg		= 0;
int		flagSingleFileInstance	= 1;
int		flagStartAsTrayIcon		= 0;
int		flagAlwaysOnTop			= 0;
int		flagRelativeFileMRU		= 0;
int		flagPortableMyDocs		= 0;
int		flagNoFadeHidden		= 0;
int		iOpacityLevel			= 75;
int		flagToolbarLook			= 0;
int		flagSimpleIndentGuides	= 0;
int 	fNoHTMLGuess			= 0;
int 	fNoCGIGuess				= 0;
int		fNoFileVariables		= 0;
int		flagPosParam			= 0;
int		flagDefaultPos			= 0;
int		flagNewFromClipboard	= 0;
int		flagPasteBoard			= 0;
int		flagSetEncoding			= 0;
int		flagSetEOLMode			= 0;
int		flagJumpTo				= 0;
int		flagMatchText			= 0;
int		flagChangeNotify		= 0;
int		flagLexerSpecified		= 0;
int		flagQuietCreate			= 0;
int		flagUseSystemMRU		= 0;
int		flagRelaunchElevated	= 0;
int		flagDisplayHelp			= 0;

//==============================================================================
//
// Folding Functions
//
//
#include "Edit_Fold.c"

//=============================================================================
//
// WinMain()
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HWND hwnd;
	HACCEL hAccMain;
	HACCEL hAccFindReplace;
	INITCOMMONCONTROLSEX icex;
	//HMODULE hSciLexer;
	WCHAR wchWorkingDirectory[MAX_PATH];

	// Set global variable g_hInstance
	g_hInstance = hInstance;

	// Set the Windows version global variable
	g_uWinVer = LOWORD(GetVersion());
	g_uWinVer = MAKEWORD(HIBYTE(g_uWinVer), LOBYTE(g_uWinVer));

	g_hDefaultHeap = GetProcessHeap();
	// https://docs.microsoft.com/en-us/windows/desktop/Memory/low-fragmentation-heap
#if 0 // default enabled since Vista
	if (IsWinXPAndAbove()) {
		// Enable the low-fragmenation heap (LFH).
		ULONG HeapInformation = 2;//HEAP_LFH;
		HeapSetInformation(g_hDefaultHeap, HeapCompatibilityInformation, &HeapInformation, sizeof(HeapInformation));
		// Enable heap terminate-on-corruption.
		HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	}
#endif

	// Don't keep working directory locked
	GetCurrentDirectory(COUNTOF(g_wchWorkingDirectory), g_wchWorkingDirectory);
	GetModuleFileName(NULL, wchWorkingDirectory, COUNTOF(wchWorkingDirectory));
	PathRemoveFileSpec(wchWorkingDirectory);
	SetCurrentDirectory(wchWorkingDirectory);

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

	// check if running at least on Windows 2000
	if (!IsWin2KAndAbove()) {
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			ERROR_OLD_WIN_VERSION,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPWSTR)&lpMsgBuf,
			0,
			NULL);
		MessageBox(NULL, (LPCWSTR)lpMsgBuf, L"Notepad2", MB_OK | MB_ICONEXCLAMATION);
		LocalFree(lpMsgBuf);
		return 0;
	}

	// Check if running with elevated privileges
	fIsElevated = IsElevated();

	// Default Encodings (may already be used for command line parsing)
	Encoding_InitDefaults();

	// Command Line, Ini File and Flags
	ParseCommandLine();
	FindIniFile();
	TestIniFile();
	CreateIniFile();
	LoadFlags();

	// set AppUserModelID
	PrivateSetCurrentProcessExplicitAppUserModelID(g_wchAppUserModelID);

	// Command Line Help Dialog
	if (flagDisplayHelp) {
		DisplayCmdLineHelp(NULL);
		return 0;
	}

	// Adapt window class name
	if (fIsElevated) {
		StrCat(wchWndClass, L"U");
	}
	if (flagPasteBoard) {
		StrCat(wchWndClass, L"B");
	}

	// Relaunch with elevated privileges
	if (RelaunchElevated()) {
		return 0;
	}

	// Try to run multiple instances
	if (RelaunchMultiInst()) {
		return 0;
	}

	// Try to activate another window
	if (ActivatePrevInst()) {
		return 0;
	}

	// Init OLE and Common Controls
	OleInitialize(NULL);

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC	= ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	msgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

	hModUxTheme = LoadLibrary(L"uxtheme.dll");

	Scintilla_RegisterClasses(hInstance);

	// Load Settings
	LoadSettings();

	if (!InitApplication(hInstance)) {
		OleUninitialize();
		return FALSE;
	}

	if ((hwnd = InitInstance(hInstance, nShowCmd)) == NULL) {
		OleUninitialize();
		return FALSE;
	}

	hAccMain = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	hAccFindReplace = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCFINDREPLACE));

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (IsWindow(hDlgFindReplace) && (msg.hwnd == hDlgFindReplace || IsChild(hDlgFindReplace, msg.hwnd))) {
			if (TranslateAccelerator(hDlgFindReplace, hAccFindReplace, &msg) || IsDialogMessage(hDlgFindReplace, &msg)) {
				continue;
			}
		}

		if (!TranslateAccelerator(hwnd, hAccMain, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Save Settings is done elsewhere

	Scintilla_ReleaseResources();
	UnregisterClass(wchWndClass, hInstance);

	if (hModUxTheme) {
		FreeLibrary(hModUxTheme);
	}

	OleUninitialize();

	return (int)(msg.wParam);
}

//=============================================================================
//
// InitApplication()
//
//
BOOL InitApplication(HINSTANCE hInstance) {
	WNDCLASS	 wc;

	wc.style		 = CS_BYTEALIGNWINDOW | CS_DBLCLKS;
	wc.lpfnWndProc	 = (WNDPROC)MainWndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wc.lpszMenuName	 = MAKEINTRESOURCE(IDR_MAINWND);
	wc.lpszClassName = wchWndClass;

	return RegisterClass(&wc);
}

//=============================================================================
//
// InitInstance()
//
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow) {
	RECT rc = { wi.x, wi.y, wi.x + wi.cx, wi.y + wi.cy };
	RECT rc2;
	MONITORINFO mi;
	BOOL bFileLoadCalled = FALSE;

	HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	if (flagDefaultPos == 1) {
		wi.x = wi.y = wi.cx = wi.cy = CW_USEDEFAULT;
		wi.max = 0;
	} else if (flagDefaultPos >= 4) {
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		if (flagDefaultPos & 8) {
			wi.x = (rc.right - rc.left) / 2;
		} else {
			wi.x = rc.left;
		}
		wi.cx = rc.right - rc.left;
		if (flagDefaultPos & (4 | 8)) {
			wi.cx /= 2;
		}
		if (flagDefaultPos & 32) {
			wi.y = (rc.bottom - rc.top) / 2;
		} else {
			wi.y = rc.top;
		}
		wi.cy = rc.bottom - rc.top;
		if (flagDefaultPos & (16 | 32)) {
			wi.cy /= 2;
		}
		if (flagDefaultPos & 64) {
			wi.x = rc.left;
			wi.y = rc.top;
			wi.cx = rc.right - rc.left;
			wi.cy = rc.bottom - rc.top;
		}
		if (flagDefaultPos & 128) {
			wi.x += (flagDefaultPos & 8) ? 4 : 8;
			wi.cx -= (flagDefaultPos & (4 | 8)) ? 12 : 16;
			wi.y += (flagDefaultPos & 32) ? 4 : 8;
			wi.cy -= (flagDefaultPos & (16 | 32)) ? 12 : 16;
		}
	} else if (flagDefaultPos == 2 || flagDefaultPos == 3 ||
			   wi.x == CW_USEDEFAULT || wi.y == CW_USEDEFAULT ||
			   wi.cx == CW_USEDEFAULT || wi.cy == CW_USEDEFAULT) {

		// default window position
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		wi.y = rc.top + 16;
		wi.cy = rc.bottom - rc.top - 32;
		wi.cx = min_i(rc.right - rc.left - 32, wi.cy);
		wi.x = (flagDefaultPos == 3) ? rc.left + 16 : rc.right - wi.cx - 16;
	} else {

		// fit window into working area of current monitor
		wi.x += (mi.rcWork.left - mi.rcMonitor.left);
		wi.y += (mi.rcWork.top - mi.rcMonitor.top);
		if (wi.x < mi.rcWork.left) {
			wi.x = mi.rcWork.left;
		}
		if (wi.y < mi.rcWork.top) {
			wi.y = mi.rcWork.top;
		}
		if (wi.x + wi.cx > mi.rcWork.right) {
			wi.x -= (wi.x + wi.cx - mi.rcWork.right);
			if (wi.x < mi.rcWork.left) {
				wi.x = mi.rcWork.left;
			}
			if (wi.x + wi.cx > mi.rcWork.right) {
				wi.cx = mi.rcWork.right - wi.x;
			}
		}
		if (wi.y + wi.cy > mi.rcWork.bottom) {
			wi.y -= (wi.y + wi.cy - mi.rcWork.bottom);
			if (wi.y < mi.rcWork.top) {
				wi.y = mi.rcWork.top;
			}
			if (wi.y + wi.cy > mi.rcWork.bottom) {
				wi.cy = mi.rcWork.bottom - wi.y;
			}
		}
		SetRect(&rc, wi.x, wi.y, wi.x + wi.cx, wi.y + wi.cy);
		if (!IntersectRect(&rc2, &rc, &mi.rcWork)) {
			wi.y = mi.rcWork.top + 16;
			wi.cy = mi.rcWork.bottom - mi.rcWork.top - 32;
			wi.cx = min_i(mi.rcWork.right - mi.rcWork.left - 32, wi.cy);
			wi.x = mi.rcWork.right - wi.cx - 16;
		}
	}

	hwndMain = CreateWindowEx(
				   0,
				   wchWndClass,
				   L"Notepad2",
				   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
				   wi.x,
				   wi.y,
				   wi.cx,
				   wi.cy,
				   NULL,
				   NULL,
				   hInstance,
				   NULL);

	if (wi.max) {
		nCmdShow = SW_SHOWMAXIMIZED;
	}

	if ((bAlwaysOnTop || flagAlwaysOnTop == 2) && flagAlwaysOnTop != 1) {
		SetWindowPos(hwndMain, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if (bTransparentMode) {
		SetWindowTransparentMode(hwndMain, TRUE);
	}

	// Current file information -- moved in front of ShowWindow()
	//FileLoad(TRUE, TRUE, FALSE, FALSE, L"");
	//
	if (!flagStartAsTrayIcon) {
		ShowWindow(hwndMain, nCmdShow);
		UpdateWindow(hwndMain);
	} else {
		ShowWindow(hwndMain, SW_HIDE); // trick ShowWindow()
		ShowNotifyIcon(hwndMain, TRUE);
	}

	// Source Encoding
	if (lpEncodingArg) {
		iSrcEncoding = Encoding_MatchW(lpEncodingArg);
	}

	BOOL bOpened = FALSE;
	// Pathname parameter
	if (lpFileArg /*&& !flagNewFromClipboard*/) {

		// Open from Directory
		if (PathIsDirectory(lpFileArg)) {
			WCHAR tchFile[MAX_PATH];
			if (OpenFileDlg(hwndMain, tchFile, COUNTOF(tchFile), lpFileArg)) {
				bOpened = FileLoad(FALSE, FALSE, FALSE, FALSE, tchFile);
				bFileLoadCalled = TRUE;
			}
		} else {
			if ((bOpened = FileLoad(FALSE, FALSE, FALSE, FALSE, lpFileArg)) != FALSE) {
				bFileLoadCalled = TRUE;
				if (flagJumpTo) { // Jump to position
					EditJumpTo(hwndEdit, iInitialLine, iInitialColumn);
					EditEnsureSelectionVisible(hwndEdit);
				}
			}
		}
		NP2HeapFree(lpFileArg);

		if (bOpened) {
			if (flagChangeNotify == 1) {
				iFileWatchingMode = 0;
				bResetFileWatching = TRUE;
				InstallFileWatching(szCurFile);
			} else if (flagChangeNotify == 2) {
				iFileWatchingMode = 2;
				bResetFileWatching = TRUE;
				InstallFileWatching(szCurFile);
			}
		}
	} else {
		if (iSrcEncoding != -1) {
			iEncoding = iSrcEncoding;
			iOriginalEncoding = iSrcEncoding;
			SendMessage(hwndEdit, SCI_SETCODEPAGE, (iEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8, 0);
		}
	}

	if (!bFileLoadCalled) {
		bOpened = FileLoad(TRUE, TRUE, FALSE, FALSE, L"");
	}

	// reset
	iSrcEncoding = -1;
	flagQuietCreate = 0;
	fKeepTitleExcerpt = 0;

	// Check for /c [if no file is specified] -- even if a file is specified
	/*else */
	if (flagNewFromClipboard) {
		if (SendMessage(hwndEdit, SCI_CANPASTE, 0, 0)) {
			BOOL bAutoIndent2 = bAutoIndent;
			bAutoIndent = FALSE;
			EditJumpTo(hwndEdit, -1, 0);
			SendMessage(hwndEdit, SCI_BEGINUNDOACTION, 0, 0);
			if (SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) > 0) {
				SendMessage(hwndEdit, SCI_NEWLINE, 0, 0);
			}
			SendMessage(hwndEdit, SCI_PASTE, 0, 0);
			SendMessage(hwndEdit, SCI_NEWLINE, 0, 0);
			SendMessage(hwndEdit, SCI_ENDUNDOACTION, 0, 0);
			bAutoIndent = bAutoIndent2;
			if (flagJumpTo) {
				EditJumpTo(hwndEdit, iInitialLine, iInitialColumn);
			}
			EditEnsureSelectionVisible(hwndEdit);
		}
	}

	// Encoding
	if (0 != flagSetEncoding) {
		SendMessage(
			hwndMain,
			WM_COMMAND,
			MAKELONG(IDM_ENCODING_ANSI + flagSetEncoding - 1, 1),
			0);
		flagSetEncoding = 0;
	}

	// EOL mode
	if (0 != flagSetEOLMode) {
		SendMessage(
			hwndMain,
			WM_COMMAND,
			MAKELONG(IDM_LINEENDINGS_CRLF + flagSetEOLMode - 1, 1),
			0);
		flagSetEOLMode = 0;
	}

	// Match Text
	if (flagMatchText && lpMatchArg) {
		if (StrNotEmpty(lpMatchArg) && SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0)) {

			UINT cp = (UINT)SendMessage(hwndEdit, SCI_GETCODEPAGE, 0, 0);
			WideCharToMultiByte(cp, 0, lpMatchArg, -1, efrData.szFind, COUNTOF(efrData.szFind), NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, lpMatchArg, -1, efrData.szFindUTF8, COUNTOF(efrData.szFindUTF8), NULL, NULL);
			cpLastFind = cp;

			if (flagMatchText & 4) {
				efrData.fuFlags |= SCFIND_REGEXP | SCFIND_POSIX;
			} else if (flagMatchText & 8) {
				efrData.bTransformBS = TRUE;
			}

			if (flagMatchText & 2) {
				if (!flagJumpTo) {
					EditJumpTo(hwndEdit, -1, 0);
				}
				EditFindPrev(hwndEdit, &efrData, FALSE);
				EditEnsureSelectionVisible(hwndEdit);
			} else {
				if (!flagJumpTo) {
					SendMessage(hwndEdit, SCI_DOCUMENTSTART, 0, 0);
				}
				EditFindNext(hwndEdit, &efrData, FALSE);
				EditEnsureSelectionVisible(hwndEdit);
			}
		}
		LocalFree(lpMatchArg);
	}

	// Check for Paste Board option -- after loading files
	if (flagPasteBoard) {
		bLastCopyFromMe = TRUE;
		hwndNextCBChain = SetClipboardViewer(hwndMain);
		uidsAppTitle = IDS_APPTITLE_PASTEBOARD;
		UpdateWindowTitle();
		bLastCopyFromMe = FALSE;
		dwLastCopyTime = 0;
		SetTimer(hwndMain, ID_PASTEBOARDTIMER, 100, PasteBoardTimer);
	}

	// check if a lexer was specified from the command line
	if (flagLexerSpecified) {
		if (lpSchemeArg) {
			Style_SetLexerFromName(hwndEdit, szCurFile, lpSchemeArg);
			LocalFree(lpSchemeArg);
		} else if (iInitialLexer >= 0 && iInitialLexer < NUMLEXERS) {
			Style_SetLexerFromID(hwndEdit, iInitialLexer);
		}
		flagLexerSpecified = 0;
	}

	// If start as tray icon, set current filename as tooltip
	if (flagStartAsTrayIcon) {
		SetNotifyIconTitle(hwndMain);
	}

	if (!bOpened) {
		UpdateStatusBarCache(STATUS_CODEPAGE);
		UpdateStatusBarCache(STATUS_EOLMODE);
	}
	UpdateStatusBarCache(STATUS_OVRMODE);
	UpdateStatusBarCache(STATUS_DOCZOOM);
	UpdateToolbar();
	UpdateStatusbar();

	return hwndMain;
}

static inline void NP2MinimizeWind(HWND hwnd) {
	MinimizeWndToTray(hwnd);
	ShowNotifyIcon(hwnd, TRUE);
	SetNotifyIconTitle(hwnd);
}

static inline void NP2RestoreWind(HWND hwnd) {
	ShowNotifyIcon(hwnd, FALSE);
	RestoreWndFromTray(hwnd);
	ShowOwnedPopups(hwnd, TRUE);
}

static inline void NP2ExitWind(HWND hwnd) {
	if (FileSave(FALSE, TRUE, FALSE, FALSE)) {
		DestroyWindow(hwnd);
	}
}

void OnDropOneFile(HWND hwnd, LPCWSTR szBuf) {
	// Reset Change Notify
	//bPendingChangeNotify = FALSE;
	if (IsIconic(hwnd)) {
		ShowWindow(hwnd, SW_RESTORE);
	}
	//SetForegroundWindow(hwnd);
	if (PathIsDirectory(szBuf)) {
		WCHAR tchFile[MAX_PATH];
		if (OpenFileDlg(hwndMain, tchFile, COUNTOF(tchFile), szBuf)) {
			FileLoad(FALSE, FALSE, FALSE, FALSE, tchFile);
		}
	} else {
		FileLoad(FALSE, FALSE, FALSE, FALSE, szBuf);
	}
}

//=============================================================================
//
// MainWndProc()
//
// Messages are distributed to the MsgXXX-handlers
//
//
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static BOOL bShutdownOK;

	switch (umsg) {
	// Quickly handle painting and sizing messages, found in ScintillaWin.cxx
	// Cool idea, don't know if this has any effect... ;-)
	case WM_MOVE:
	case WM_MOUSEACTIVATE:
	case WM_NCHITTEST:
	case WM_NCCALCSIZE:
	case WM_NCPAINT:
	case WM_PAINT:
	case WM_ERASEBKGND:
	case WM_NCMOUSEMOVE:
	case WM_NCLBUTTONDOWN:
	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
		return DefWindowProc(hwnd, umsg, wParam, lParam);

	case WM_CREATE:
		return MsgCreate(hwnd, wParam, lParam);

	case WM_DESTROY:
	case WM_ENDSESSION:
		if (!bShutdownOK) {
			WINDOWPLACEMENT wndpl;

			// Terminate file watching
			InstallFileWatching(NULL);

			// GetWindowPlacement
			wndpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwnd, &wndpl);

			wi.x = wndpl.rcNormalPosition.left;
			wi.y = wndpl.rcNormalPosition.top;
			wi.cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
			wi.cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
			wi.max = (IsZoomed(hwnd) || (wndpl.flags & WPF_RESTORETOMAXIMIZED));

			DragAcceptFiles(hwnd, FALSE);

			// Terminate clipboard watching
			if (flagPasteBoard) {
				KillTimer(hwnd, ID_PASTEBOARDTIMER);
				ChangeClipboardChain(hwnd, hwndNextCBChain);
			}

			// Destroy find / replace dialog
			if (IsWindow(hDlgFindReplace)) {
				DestroyWindow(hDlgFindReplace);
			}

			// call SaveSettings() when hwndToolbar is still valid
			SaveSettings(FALSE);

			if (StrNotEmpty(szIniFile)) {

				// Cleanup unwanted MRU's
				if (!bSaveRecentFiles) {
					MRU_Empty(pFileMRU);
					MRU_Save(pFileMRU);
				} else {
					MRU_MergeSave(pFileMRU, TRUE, flagRelativeFileMRU, flagPortableMyDocs);
				}
				MRU_Destroy(pFileMRU);

				if (!bSaveFindReplace) {
					MRU_Empty(mruFind);
					MRU_Empty(mruReplace);
					MRU_Save(mruFind);
					MRU_Save(mruReplace);
				} else {
					MRU_MergeSave(mruFind, FALSE, FALSE, FALSE);
					MRU_MergeSave(mruReplace, FALSE, FALSE, FALSE);
				}
				MRU_Destroy(mruFind);
				MRU_Destroy(mruReplace);
			}

			// Remove tray icon if necessary
			ShowNotifyIcon(hwnd, FALSE);

			bShutdownOK = TRUE;
		}
		if (umsg == WM_DESTROY) {
			PostQuitMessage(0);
		}
		break;

	case WM_CLOSE:
		if (bMinimizeToTray) {
			NP2MinimizeWind(hwnd);
		} else {
			NP2ExitWind(hwnd);
		}
		break;

	case WM_QUERYENDSESSION:
		if (FileSave(FALSE, TRUE, FALSE, FALSE)) {
			return TRUE;
		}
		return FALSE;

	// Reinitialize theme-dependent values and resize windows
	case WM_THEMECHANGED:
		MsgThemeChanged(hwnd, wParam, lParam);
		break;

	case WM_DPICHANGED:
		MsgDPIChanged(hwnd, wParam, lParam);
		break;

	// update Scintilla colors
	case WM_SYSCOLORCHANGE: {
		Style_SetLexer(hwndEdit, pLexCurrent);
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}

	//case WM_TIMER:
	//	break;

	case WM_SIZE:
		MsgSize(hwnd, wParam, lParam);
		break;

	case WM_SETFOCUS:
		SetFocus(hwndEdit);

		UpdateToolbar();
		UpdateStatusbar();

		//if (bPendingChangeNotify)
		//	PostMessage(hwnd, APPM_CHANGENOTIFY, 0, 0);
		break;

	case WM_DROPFILES: {
		WCHAR szBuf[MAX_PATH + 40];
		HDROP hDrop = (HDROP)wParam;

		DragQueryFile(hDrop, 0, szBuf, COUNTOF(szBuf));
		OnDropOneFile(hwnd, szBuf);

		//if (DragQueryFile(hDrop, (UINT)(-1), NULL, 0) > 1) {
		//	MsgBox(MBWARN, IDS_ERR_DROP);
		//}

		DragFinish(hDrop);
	}
	break;

	case WM_COPYDATA: {
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

		// Reset Change Notify
		//bPendingChangeNotify = FALSE;

		SetDlgItemInt(hwnd, IDC_REUSELOCK, GetTickCount(), FALSE);

		if (pcds->dwData == DATA_NOTEPAD2_PARAMS) {
			LPNP2PARAMS params = NP2HeapAlloc(pcds->cbData);
			CopyMemory(params, pcds->lpData, pcds->cbData);

			if (params->flagLexerSpecified) {
				flagLexerSpecified = 1;
			}

			if (params->flagQuietCreate) {
				flagQuietCreate = 1;
			}

			if (params->flagFileSpecified) {
				BOOL bOpened = FALSE;
				iSrcEncoding = params->iSrcEncoding;

				if (PathIsDirectory(&params->wchData)) {
					WCHAR tchFile[MAX_PATH];
					if (OpenFileDlg(hwndMain, tchFile, COUNTOF(tchFile), &params->wchData)) {
						bOpened = FileLoad(FALSE, FALSE, FALSE, FALSE, tchFile);
					}
				} else {
					bOpened = FileLoad(FALSE, FALSE, FALSE, FALSE, &params->wchData);
				}

				if (bOpened) {
					if (params->flagChangeNotify == 1) {
						iFileWatchingMode = 0;
						bResetFileWatching = TRUE;
						InstallFileWatching(szCurFile);
					} else if (params->flagChangeNotify == 2) {
						iFileWatchingMode = 2;
						bResetFileWatching = TRUE;
						InstallFileWatching(szCurFile);
					}

					if (0 != params->flagSetEncoding) {
						flagSetEncoding = params->flagSetEncoding;
						SendMessage(
							hwnd,
							WM_COMMAND,
							MAKELONG(IDM_ENCODING_ANSI + flagSetEncoding - 1, 1),
							0);
						flagSetEncoding = 0;
					}

					if (0 != params->flagSetEOLMode) {
						flagSetEOLMode = params->flagSetEOLMode;
						SendMessage(
							hwndMain,
							WM_COMMAND,
							MAKELONG(IDM_LINEENDINGS_CRLF + flagSetEOLMode - 1, 1),
							0);
						flagSetEOLMode = 0;
					}

					if (params->flagLexerSpecified) {
						if (params->iInitialLexer < 0) {
							WCHAR wchExt[32] = L".";
							lstrcpyn(CharNext(wchExt), StrEnd(&params->wchData) + 1, 30);
							Style_SetLexerFromName(hwndEdit, &params->wchData, wchExt);
						} else if (params->iInitialLexer >= 0 && params->iInitialLexer < NUMLEXERS) {
							Style_SetLexerFromID(hwndEdit, params->iInitialLexer);
						}
					}

					if (params->flagTitleExcerpt) {
						lstrcpyn(szTitleExcerpt, StrEnd(&params->wchData) + 1, COUNTOF(szTitleExcerpt));
						UpdateWindowTitle();
					}
				}
				// reset
				iSrcEncoding = -1;
			}

			if (params->flagJumpTo) {
				if (params->iInitialLine == 0) {
					params->iInitialLine = 1;
				}
				EditJumpTo(hwndEdit, params->iInitialLine, params->iInitialColumn);
				EditEnsureSelectionVisible(hwndEdit);
			}

			flagLexerSpecified = 0;
			flagQuietCreate = 0;

			NP2HeapFree(params);

			UpdateStatusbar();
		}
	}
	return TRUE;

	case WM_CONTEXTMENU: {
		HMENU hmenu;
		int imenu = 0;
		POINT pt;
		int nID = GetDlgCtrlID((HWND)wParam);

		if ((nID != IDC_EDIT) && (nID != IDC_STATUSBAR) &&
				(nID != IDC_REBAR) && (nID != IDC_TOOLBAR)) {
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}

		hmenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_POPUPMENU));
		//SetMenuDefaultItem(GetSubMenu(hmenu, 1), 0, FALSE);

		pt.x = (int)(short)LOWORD(lParam);
		pt.y = (int)(short)HIWORD(lParam);

		switch (nID) {
		case IDC_EDIT: {
			int iSelStart	= (int)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);
			int iSelEnd		= (int)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0);

			if (iSelStart == iSelEnd && pt.x != -1 && pt.y != -1) {
				int iNewPos;
				POINT ptc = { pt.x, pt.y };
				ScreenToClient(hwndEdit, &ptc);
				iNewPos = (int)SendMessage(hwndEdit, SCI_POSITIONFROMPOINT, (WPARAM)ptc.x, (LPARAM)ptc.y);
				SendMessage(hwndEdit, SCI_GOTOPOS, (WPARAM)iNewPos, 0);
			}

			if (pt.x == -1 && pt.y == -1) {
				int iCurrentPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
				pt.x = (int)SendMessage(hwndEdit, SCI_POINTXFROMPOSITION, 0, (LPARAM)iCurrentPos);
				pt.y = (int)SendMessage(hwndEdit, SCI_POINTYFROMPOSITION, 0, (LPARAM)iCurrentPos);
				ClientToScreen(hwndEdit, &pt);
			}
			imenu = 0;
		}
		break;

		case IDC_TOOLBAR:
		case IDC_STATUSBAR:
		case IDC_REBAR:
			if (pt.x == -1 && pt.y == -1) {
				GetCursorPos(&pt);
			}
			imenu = 1;
			break;
		}

		TrackPopupMenuEx(GetSubMenu(hmenu, imenu),
						 TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x + 1, pt.y + 1, hwnd, NULL);

		DestroyMenu(hmenu);
	}
	break;

	case WM_INITMENU:
		MsgInitMenu(hwnd, wParam, lParam);
		break;

	case WM_NOTIFY:
		return MsgNotify(hwnd, wParam, lParam);

	case WM_COMMAND:
		return MsgCommand(hwnd, wParam, lParam);

	case WM_SYSCOMMAND:
		switch (wParam) {
		case SC_MINIMIZE:
			ShowOwnedPopups(hwnd, FALSE);
			if (bMinimizeToTray) {
				NP2MinimizeWind(hwnd);
				return 0;
			}
			return DefWindowProc(hwnd, umsg, wParam, lParam);

		case SC_RESTORE: {
			LRESULT lrv = DefWindowProc(hwnd, umsg, wParam, lParam);
			ShowOwnedPopups(hwnd, TRUE);
			return (lrv);
		}
		}
		return DefWindowProc(hwnd, umsg, wParam, lParam);

	case APPM_CHANGENOTIFY:
		if (iFileWatchingMode == 1 || bModified || iEncoding != iOriginalEncoding) {
			SetForegroundWindow(hwnd);
		}

		if (PathFileExists(szCurFile)) {
			if ((iFileWatchingMode == 2 && !bModified && iEncoding == iOriginalEncoding) ||
					MsgBox(MBYESNO, IDS_FILECHANGENOTIFY) == IDYES) {

				int iCurPos		= (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
				int iAnchorPos	= (int)SendMessage(hwndEdit, SCI_GETANCHOR, 0, 0);
#if NP2_ENABLE_DOT_LOG_FEATURE
				int iVisTopLine = (int)SendMessage(hwndEdit, SCI_GETFIRSTVISIBLELINE, 0, 0);
				int iDocTopLine = (int)SendMessage(hwndEdit, SCI_DOCLINEFROMVISIBLE, (WPARAM)iVisTopLine, 0);
				int iXOffset	= (int)SendMessage(hwndEdit, SCI_GETXOFFSET, 0, 0);
#endif
				BOOL bIsTail	= (iCurPos == iAnchorPos) && (iCurPos == SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0));

				iWeakSrcEncoding = iEncoding;
				if (FileLoad(TRUE, FALSE, TRUE, FALSE, szCurFile)) {

					if (bIsTail && iFileWatchingMode == 2) {
						EditJumpTo(hwndEdit, -1, 0);
						EditEnsureSelectionVisible(hwndEdit);
					}
#if NP2_ENABLE_DOT_LOG_FEATURE
					 else if (SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) >= 4) {
						char tch[5] = "";
						SendMessage(hwndEdit, SCI_GETTEXT, 5, (LPARAM)tch);
						if (StrEqual(tch, ".LOG")) {
							int iNewTopLine;
							SendMessage(hwndEdit, SCI_SETSEL, iAnchorPos, iCurPos);
							SendMessage(hwndEdit, SCI_ENSUREVISIBLE, (WPARAM)iDocTopLine, 0);
							iNewTopLine = (int)SendMessage(hwndEdit, SCI_GETFIRSTVISIBLELINE, 0, 0);
							SendMessage(hwndEdit, SCI_LINESCROLL, 0, (LPARAM)iVisTopLine - iNewTopLine);
							SendMessage(hwndEdit, SCI_SETXOFFSET, (WPARAM)iXOffset, 0);
						}
					}
#endif
				}
			}
		} else {
			if (MsgBox(MBYESNO, IDS_FILECHANGENOTIFY2) == IDYES) {
				FileSave(TRUE, FALSE, FALSE, FALSE);
			}
		}

		if (!bRunningWatch) {
			InstallFileWatching(szCurFile);
		}
		break;

	//// This message is posted before Notepad2 reactivates itself
	//case APPM_CHANGENOTIFYCLEAR:
	//	bPendingChangeNotify = FALSE;
	//	break;

	case WM_DRAWCLIPBOARD:
		if (!bLastCopyFromMe) {
			dwLastCopyTime = GetTickCount();
		} else {
			bLastCopyFromMe = FALSE;
		}
		if (hwndNextCBChain) {
			SendMessage(hwndNextCBChain, WM_DRAWCLIPBOARD, wParam, lParam);
		}
		break;

	case WM_CHANGECBCHAIN:
		if ((HWND)wParam == hwndNextCBChain) {
			hwndNextCBChain = (HWND)lParam;
		}
		if (hwndNextCBChain) {
			SendMessage(hwndNextCBChain, WM_CHANGECBCHAIN, lParam, wParam);
		}
		break;

	case APPM_TRAYMESSAGE:
		switch (lParam) {
		case WM_RBUTTONUP: {
			HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_POPUPMENU));
			HMENU hMenuPopup = GetSubMenu(hMenu, 2);

			POINT pt;
			int iCmd;

			SetForegroundWindow(hwnd);

			GetCursorPos(&pt);
			SetMenuDefaultItem(hMenuPopup, IDM_TRAY_RESTORE, FALSE);
			iCmd = TrackPopupMenu(hMenuPopup,
								  TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
								  pt.x, pt.y, 0, hwnd, NULL);

			PostMessage(hwnd, WM_NULL, 0, 0);

			DestroyMenu(hMenu);

			if (iCmd == IDM_TRAY_RESTORE) {
				NP2RestoreWind(hwnd);
			} else if (iCmd == IDM_TRAY_EXIT) {
				NP2ExitWind(hwnd);
			}
		}
		return TRUE;

		case WM_LBUTTONUP:
			NP2RestoreWind(hwnd);
			return TRUE;
		}
		break;

	case APPM_CENTER_MESSAGE_BOX: {
		HWND box = FindWindow(L"#32770", NULL);
		HWND parent = GetParent(box);
		// MessageBox belongs to us.
		if (parent == (HWND)wParam || parent == hwndMain) {
			CenterDlgInParentEx(box, parent);
			SnapToDefaultButton(box);
		}
	}
	break;

	default:
		if (umsg == msgTaskbarCreated) {
			if (!IsWindowVisible(hwnd)) {
				ShowNotifyIcon(hwnd, TRUE);
			}
			SetNotifyIconTitle(hwnd);
			return 0;
		}

		return DefWindowProc(hwnd, umsg, wParam, lParam);

	}
	return 0;
}

void UpdateWindowTitle(void) {
	SetWindowTitle(hwndMain, uidsAppTitle, fIsElevated, IDS_UNTITLED, szCurFile,
				iPathNameFormat, bModified || iEncoding != iOriginalEncoding,
				IDS_READONLY, bReadOnly, szTitleExcerpt);
}

void UpdateSelectionMarginWidth(void) {
	// fixed width to put arrow cursor.
	int width = bShowSelectionMargin ? RoundToCurrentDPI(16) : 0;
	SciCall_SetMarginWidth(MARGIN_SELECTION, width);
}

void UpdateFoldMarginWidth(void) {
	if (bShowCodeFolding) {
		const int iLineMarginWidthNow = SciCall_GetMarginWidth(MARGIN_FOLD_INDEX);
		const int iLineMarginWidthFit = SciCall_TextWidth(STYLE_LINENUMBER, "+_");

		if (iLineMarginWidthNow != iLineMarginWidthFit) {
			SciCall_SetMarginWidth(MARGIN_FOLD_INDEX, iLineMarginWidthFit);
		}
	} else {
		SciCall_SetMarginWidth(MARGIN_FOLD_INDEX, 0);
	}
}

#if NP2_ENABLE_SHOW_CALL_TIPS
#define SetCallTipsWaitTime() \
	SendMessage(hwndEdit, SCI_SETMOUSEDWELLTIME, bShowCallTips? iCallTipsWaitTime : SC_TIME_FOREVER, 0);
#endif

void SetWrapStartIndent(void) {
	int i = 0;
	switch (iWordWrapIndent) {
	case 1:
		i = 1;
		break;
	case 2:
		i = 2;
		break;
	case 3:
		i = (iIndentWidth) ? 1 * iIndentWidth : 1 * iTabWidth;
		break;
	case 4:
		i = (iIndentWidth) ? 2 * iIndentWidth : 2 * iTabWidth;
		break;
	}
	SendMessage(hwndEdit, SCI_SETWRAPSTARTINDENT, i, 0);
}

void SetWrapIndentMode(void) {
	if (iWordWrapIndent == 5) {
		SendMessage(hwndEdit, SCI_SETWRAPINDENTMODE, SC_WRAPINDENT_SAME, 0);
	} else if (iWordWrapIndent == 6) {
		SendMessage(hwndEdit, SCI_SETWRAPINDENTMODE, SC_WRAPINDENT_INDENT, 0);
	} else if (iWordWrapIndent == 7) {
		SendMessage(hwndEdit, SCI_SETWRAPINDENTMODE, SC_WRAPINDENT_DEEPINDENT, 0);
	} else {
		SetWrapStartIndent();
		SendMessage(hwndEdit, SCI_SETWRAPINDENTMODE, SC_WRAPINDENT_FIXED, 0);
	}
}

void SetWrapVisualFlags(void) {
	if (bShowWordWrapSymbols) {
		int wrapVisualFlags = 0;
		int wrapVisualFlagsLocation = 0;
		if (iWordWrapSymbols == 0) {
			iWordWrapSymbols = 22;
		}
		switch (iWordWrapSymbols % 10) {
		case 1:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_END;
			wrapVisualFlagsLocation |= SC_WRAPVISUALFLAGLOC_END_BY_TEXT;
			break;
		case 2:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_END;
			break;
		}
		switch (((iWordWrapSymbols % 100) - (iWordWrapSymbols % 10)) / 10) {
		case 1:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_START;
			wrapVisualFlagsLocation |= SC_WRAPVISUALFLAGLOC_START_BY_TEXT;
			break;
		case 2:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_START;
			break;
		}
		SendMessage(hwndEdit, SCI_SETWRAPVISUALFLAGSLOCATION, wrapVisualFlagsLocation, 0);
		SendMessage(hwndEdit, SCI_SETWRAPVISUALFLAGS, wrapVisualFlags, 0);
	} else {
		SendMessage(hwndEdit, SCI_SETWRAPVISUALFLAGS, 0, 0);
	}
}

//=============================================================================
//
// MsgCreate() - Handles WM_CREATE
//
//
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
	g_uCurrentDPI = GetCurrentDPI(hwnd);
	g_uCurrentPPI = GetCurrentPPI(hwnd);

	// Setup edit control
	hwndEdit = EditCreate(hwnd);

	iRenderingTechnology = (int)SendMessage(hwndEdit, SCI_GETTECHNOLOGY, 0, 0);
	iBidirectional = (int)SendMessage(hwndEdit, SCI_GETBIDIRECTIONAL, 0, 0);

	SendMessage(hwndEdit, SCI_SETZOOM, iZoomLevel, 0);
	// Tabs
	SendMessage(hwndEdit, SCI_SETUSETABS, !bTabsAsSpaces, 0);
	SendMessage(hwndEdit, SCI_SETTABINDENTS, bTabIndents, 0);
	SendMessage(hwndEdit, SCI_SETBACKSPACEUNINDENTS, bBackspaceUnindents, 0);
	SendMessage(hwndEdit, SCI_SETTABWIDTH, iTabWidth, 0);
	SendMessage(hwndEdit, SCI_SETINDENT, iIndentWidth, 0);

	// Indent Guides
	Style_SetIndentGuides(hwndEdit, bShowIndentGuides);

	// Word wrap
	SendMessage(hwndEdit, SCI_SETWRAPMODE, (fWordWrap? iWordWrapMode : SC_WRAP_NONE), 0);
	SetWrapIndentMode();
	SetWrapVisualFlags();

	// Long Lines
	if (bMarkLongLines) {
		SendMessage(hwndEdit, SCI_SETEDGEMODE, (iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND, 0);
	} else {
		SendMessage(hwndEdit, SCI_SETEDGEMODE, EDGE_NONE, 0);
	}
	SendMessage(hwndEdit, SCI_SETEDGECOLUMN, iLongLinesLimit, 0);

	// Margins
	UpdateSelectionMarginWidth();

	// Margins
	SciCall_SetMarginType(MARGIN_FOLD_INDEX, SC_MARGIN_SYMBOL);
	SciCall_SetMarginMask(MARGIN_FOLD_INDEX, SC_MASK_FOLDERS);
	SciCall_SetMarginSensitive(MARGIN_FOLD_INDEX, TRUE);
	// Code folding, Box tree
	SciCall_MarkerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);
#if !NP2_DEBUG_FOLD_LEVEL
	// Draw below if not expanded
	SciCall_SetFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);
#else
	SciCall_SetFoldFlags(SC_FOLDFLAG_LEVELNUMBERS);
#endif
	// highlight for current folding block
	SciCall_MarkerEnableHighlight(TRUE);
#if NP2_ENABLE_SHOW_CALL_TIPS
	// CallTips
	SetCallTipsWaitTime();
#endif

	// Nonprinting characters
	SendMessage(hwndEdit, SCI_SETVIEWWS, (bViewWhiteSpace) ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE, 0);
	SendMessage(hwndEdit, SCI_SETVIEWEOL, bViewEOLs, 0);

	hwndEditFrame = CreateWindowEx(
						WS_EX_CLIENTEDGE,
						WC_LISTVIEW,
						NULL,
						WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
						0, 0, 100, 100,
						hwnd,
						(HMENU)IDC_EDITFRAME,
						hInstance,
						NULL);

	if (PrivateIsAppThemed()) {
		RECT rc, rc2;
		bIsAppThemed = TRUE;

		SetWindowLongPtr(hwndEdit, GWL_EXSTYLE, GetWindowLongPtr(hwndEdit, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwndEdit, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		if (IsVistaAndAbove()) {
			cxEditFrame = 0;
			cyEditFrame = 0;
		} else {
			GetClientRect(hwndEditFrame, &rc);
			GetWindowRect(hwndEditFrame, &rc2);

			cxEditFrame = ((rc2.right - rc2.left) - (rc.right - rc.left)) / 2;
			cyEditFrame = ((rc2.bottom - rc2.top) - (rc.bottom - rc.top)) / 2;
		}
	} else {
		bIsAppThemed = FALSE;

		cxEditFrame = 0;
		cyEditFrame = 0;
	}

	// Create Toolbar and Statusbar
	CreateBars(hwnd, hInstance);

	// Window Initialization

	CreateWindow(
		WC_STATIC,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 10, 10,
		hwnd,
		(HMENU)IDC_FILENAME,
		hInstance,
		NULL);

	SetDlgItemText(hwnd, IDC_FILENAME, szCurFile);

	CreateWindow(
		WC_STATIC,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		10, 10, 10, 10,
		hwnd,
		(HMENU)IDC_REUSELOCK,
		hInstance,
		NULL);

	SetDlgItemInt(hwnd, IDC_REUSELOCK, GetTickCount(), FALSE);

	// Menu
	//SetMenuDefaultItem(GetSubMenu(GetMenu(hwnd), 0), 0);

	// Drag & Drop
	DragAcceptFiles(hwnd, TRUE);

	// File MRU
	pFileMRU = MRU_Create(L"Recent Files", MRU_NOCASE, 32);
	MRU_Load(pFileMRU);

	mruFind = MRU_Create(L"Recent Find", (/*IsWindowsNT()*/1) ? MRU_UTF8 : 0, 16);
	MRU_Load(mruFind);

	mruReplace = MRU_Create(L"Recent Replace", (/*IsWindowsNT()*/1) ? MRU_UTF8 : 0, 16);
	MRU_Load(mruReplace);

	if (hwndEdit == NULL || hwndEditFrame == NULL ||
			hwndStatus == NULL || hwndToolbar == NULL || hwndReBar == NULL) {
		return -1;
	}

	if (bInFullScreenMode) {
		ToggleFullScreenMode();
	}
	return 0;
}

//=============================================================================
//
// CreateBars() - Create Toolbar and Statusbar
//
//
void CreateBars(HWND hwnd, HINSTANCE hInstance) {
	RECT rc;

	BITMAP bmp;
	HBITMAP hbmp, hbmpCopy = NULL;
	HIMAGELIST himl;
	WCHAR szTmp[MAX_PATH];
	BOOL bExternalBitmap = FALSE;

	bIsAppThemed = PrivateIsAppThemed();

	const DWORD dwToolbarStyle = WS_TOOLBAR;
	hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, dwToolbarStyle,
								 0, 0, 0, 0, hwnd, (HMENU)IDC_TOOLBAR, hInstance, NULL);

	SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	// Add normal Toolbar Bitmap
	hbmp = NULL;
	if (StrNotEmpty(tchToolbarBitmap)) {
		if (!SearchPath(NULL, tchToolbarBitmap, NULL, COUNTOF(szTmp), szTmp, NULL)) {
			lstrcpy(szTmp, tchToolbarBitmap);
		}
		hbmp = LoadImage(NULL, szTmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	}
	if (hbmp) {
		bExternalBitmap = TRUE;
	} else {
		hbmp = LoadImage(hInstance, MAKEINTRESOURCE(IDR_MAINWND), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	}
	hbmp = ResizeImageForCurrentDPI(hbmp);
	if (!bExternalBitmap) {
		hbmpCopy = CopyImage(hbmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	}
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	if (!IsWinXPAndAbove()) {
		BitmapMergeAlpha(hbmp, GetSysColor(COLOR_3DFACE));
	}
	himl = ImageList_Create(bmp.bmWidth / NUMTOOLBITMAPS, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
	ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
	DeleteObject(hbmp);
	SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)himl);

	// Optionally add hot Toolbar Bitmap
	hbmp = NULL;
	if (StrNotEmpty(tchToolbarBitmapHot)) {
		if (!SearchPath(NULL, tchToolbarBitmapHot, NULL, COUNTOF(szTmp), szTmp, NULL)) {
			lstrcpy(szTmp, tchToolbarBitmapHot);
		}
		if ((hbmp = LoadImage(NULL, szTmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE)) != NULL) {
			hbmp = ResizeImageForCurrentDPI(hbmp);
			GetObject(hbmp, sizeof(BITMAP), &bmp);
			himl = ImageList_Create(bmp.bmWidth / NUMTOOLBITMAPS, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
			ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
			DeleteObject(hbmp);
			SendMessage(hwndToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)himl);
		}
	}

	// Optionally add disabled Toolbar Bitmap
	hbmp = NULL;
	if (StrNotEmpty(tchToolbarBitmapDisabled)) {
		if (!SearchPath(NULL, tchToolbarBitmapDisabled, NULL, COUNTOF(szTmp), szTmp, NULL)) {
			lstrcpy(szTmp, tchToolbarBitmapDisabled);
		}
		if ((hbmp = LoadImage(NULL, szTmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE)) != NULL) {
			hbmp = ResizeImageForCurrentDPI(hbmp);
			GetObject(hbmp, sizeof(BITMAP), &bmp);
			himl = ImageList_Create(bmp.bmWidth / NUMTOOLBITMAPS, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
			ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
			DeleteObject(hbmp);
			SendMessage(hwndToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)himl);
			bExternalBitmap = TRUE;
		}
	}

	if (!bExternalBitmap) {
		BOOL fProcessed = FALSE;
		if (flagToolbarLook == 1) {
			fProcessed = BitmapAlphaBlend(hbmpCopy, GetSysColor(COLOR_3DFACE), 0x60);
		} else if (flagToolbarLook == 2 || (!IsWinXPAndAbove() && flagToolbarLook == 0)) {
			fProcessed = BitmapGrayScale(hbmpCopy);
		}
		if (fProcessed && !IsWinXPAndAbove()) {
			BitmapMergeAlpha(hbmpCopy, GetSysColor(COLOR_3DFACE));
		}
		if (fProcessed) {
			himl = ImageList_Create(bmp.bmWidth / NUMTOOLBITMAPS, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
			ImageList_AddMasked(himl, hbmpCopy, CLR_DEFAULT);
			SendMessage(hwndToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)himl);
		}
	}
	if (hbmpCopy) {
		DeleteObject(hbmpCopy);
	}

#if NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS
	// Load toolbar labels
	IniSection section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, COUNTOF(tbbMainWnd));
	LoadIniSection(L"Toolbar Labels", pIniSectionBuf, cchIniSection);
	IniSectionParseEx(pIniSection, pIniSectionBuf, IniSectionParseFlag_Array);

	for (int n = 0; n < pIniSection->count; n++) {
		const IniKeyValueNode *node = &pIniSection->nodeList[n];
		const UINT i = StrToInt(node->key) - 1;
		if (i >= COUNTOF(tbbMainWnd) || tbbMainWnd[i].fsStyle == TBSTYLE_SEP) {
			continue;
		}

		LPCWSTR tchDesc = node->value;
		if (StrNotEmpty(tchDesc)) {
			tbbMainWnd[i].iString = SendMessage(hwndToolbar, TB_ADDSTRING, 0, (LPARAM)tchDesc);
			tbbMainWnd[i].fsStyle |= BTNS_AUTOSIZE | BTNS_SHOWTEXT;
		} else {
			tbbMainWnd[i].fsStyle &= ~(BTNS_AUTOSIZE | BTNS_SHOWTEXT);
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
#endif

	SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0,
				SendMessage(hwndToolbar, TB_GETEXTENDEDSTYLE, 0, 0) | TBSTYLE_EX_MIXEDBUTTONS);

	SendMessage(hwndToolbar, TB_ADDBUTTONS, NUMINITIALTOOLS, (LPARAM)tbbMainWnd);
	if (Toolbar_SetButtons(hwndToolbar, IDT_FILE_NEW, tchToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd)) == 0) {
		SendMessage(hwndToolbar, TB_ADDBUTTONS, NUMINITIALTOOLS, (LPARAM)tbbMainWnd);
	}
	SendMessage(hwndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rc);
	//SendMessage(hwndToolbar, TB_SETINDENT, 2, 0);

	const DWORD dwStatusbarStyle = bShowStatusbar ? (WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE) : (WS_CHILD | WS_CLIPSIBLINGS);
	hwndStatus = CreateStatusWindow(dwStatusbarStyle, NULL, hwnd, IDC_STATUSBAR);

	// Create ReBar and add Toolbar
	const DWORD dwReBarStyle = bShowToolbar ? (WS_REBAR | WS_VISIBLE) : WS_REBAR;
	hwndReBar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, dwReBarStyle,
							   0, 0, 0, 0, hwnd, (HMENU)IDC_REBAR, hInstance, NULL);

	REBARINFO rbi;
	rbi.cbSize = sizeof(REBARINFO);
	rbi.fMask	 = 0;
	rbi.himl	 = (HIMAGELIST)NULL;
	SendMessage(hwndReBar, RB_SETBARINFO, 0, (LPARAM)&rbi);

	REBARBANDINFO rbBand;
	rbBand.cbSize	 = sizeof(REBARBANDINFO);
	rbBand.fMask	 = /*RBBIM_COLORS | RBBIM_TEXT | RBBIM_BACKGROUND | */
		RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE /*| RBBIM_SIZE*/;
	rbBand.fStyle	 = /*RBBS_CHILDEDGE |*//* RBBS_BREAK |*/ RBBS_FIXEDSIZE /*| RBBS_GRIPPERALWAYS*/;
	if (bIsAppThemed) {
		rbBand.fStyle |= RBBS_CHILDEDGE;
	}
	rbBand.hbmBack = NULL;
	rbBand.lpText		= L"Toolbar";
	rbBand.hwndChild	= hwndToolbar;
	rbBand.cxMinChild = (rc.right - rc.left) * COUNTOF(tbbMainWnd);
	rbBand.cyMinChild = (rc.bottom - rc.top) + 2 * rc.top;
	rbBand.cx		= 0;
	SendMessage(hwndReBar, RB_INSERTBAND, (WPARAM) - 1, (LPARAM)&rbBand);

	SetWindowPos(hwndReBar, NULL, 0, 0, 0, 0, SWP_NOZORDER);
	GetWindowRect(hwndReBar, &rc);
	cyReBar = rc.bottom - rc.top;

	cyReBarFrame = bIsAppThemed ? 0 : 2;
}

//=============================================================================
//
// MsgDPIChanged() - Handle WM_DPICHANGED
//
//
void MsgDPIChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	g_uCurrentDPI = HIWORD(wParam);
	RECT* const rc = (RECT *)lParam;
	const Sci_Position pos = SciCall_GetCurrentPos();
#if 0
	char buf[128];
	sprintf(buf, "WM_DPICHANGED: dpi=%u, %u\n", g_uCurrentDPI, g_uCurrentPPI);
	SendMessage(hwndEdit, SCI_INSERTTEXT, 0, (LPARAM)buf);
#endif

	Style_OnDPIChanged(hwndEdit);
	UpdateSelectionMarginWidth();
	SciCall_GotoPos(pos);

	// recreate toolbar and statusbar
	Toolbar_GetButtons(hwndToolbar, IDT_FILE_NEW, tchToolbarButtons, COUNTOF(tchToolbarButtons));

	DestroyWindow(hwndToolbar);
	DestroyWindow(hwndReBar);
	DestroyWindow(hwndStatus);
	CreateBars(hwnd, g_hInstance);
	UpdateToolbar();
	SetWindowPos(hwnd, NULL, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, SWP_NOZORDER | SWP_NOACTIVATE);
	UpdateStatusbar();
}

//=============================================================================
//
// MsgThemeChanged() - Handle WM_THEMECHANGED
//
//
void MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	RECT rc, rc2;
	HINSTANCE hInstance = (HINSTANCE)(INT_PTR)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

	// reinitialize edit frame

	if (PrivateIsAppThemed()) {
		bIsAppThemed = TRUE;

		SetWindowLongPtr(hwndEdit, GWL_EXSTYLE, GetWindowLongPtr(hwndEdit, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwndEdit, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

		if (IsVistaAndAbove()) {
			cxEditFrame = 0;
			cyEditFrame = 0;
		} else {
			SetWindowPos(hwndEditFrame, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			GetClientRect(hwndEditFrame, &rc);
			GetWindowRect(hwndEditFrame, &rc2);

			cxEditFrame = ((rc2.right - rc2.left) - (rc.right - rc.left)) / 2;
			cyEditFrame = ((rc2.bottom - rc2.top) - (rc.bottom - rc.top)) / 2;
		}
	} else {
		bIsAppThemed = FALSE;

		SetWindowLongPtr(hwndEdit, GWL_EXSTYLE, WS_EX_CLIENTEDGE | GetWindowLongPtr(hwndEdit, GWL_EXSTYLE));
		SetWindowPos(hwndEdit, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		cxEditFrame = 0;
		cyEditFrame = 0;
	}

	// recreate toolbar and statusbar
	Toolbar_GetButtons(hwndToolbar, IDT_FILE_NEW, tchToolbarButtons, COUNTOF(tchToolbarButtons));

	DestroyWindow(hwndToolbar);
	DestroyWindow(hwndReBar);
	DestroyWindow(hwndStatus);
	CreateBars(hwnd, hInstance);
	UpdateToolbar();

	GetClientRect(hwnd, &rc);
	SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELONG(rc.right, rc.bottom));
	UpdateStatusbar();
}

//=============================================================================
//
// MsgSize() - Handles WM_SIZE
//
//
void MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);

	RECT rc;
	int x, y, cx, cy;
	HDWP hdwp;

	if (wParam == SIZE_MINIMIZED) {
		return;
	}

	x = 0;
	y = 0;

	cx = LOWORD(lParam);
	cy = HIWORD(lParam);

	if (bShowToolbar) {
		/*
		SendMessage(hwndToolbar, WM_SIZE, 0, 0);
		GetWindowRect(hwndToolbar, &rc);
		y = (rc.bottom - rc.top);
		cy -= (rc.bottom - rc.top);
		*/

		//SendMessage(hwndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rc);
		SetWindowPos(hwndReBar, NULL, 0, 0, LOWORD(lParam), cyReBar, SWP_NOZORDER);
		// the ReBar automatically sets the correct height
		// calling SetWindowPos() with the height of one toolbar button
		// causes the control not to temporarily use the whole client area
		// and prevents flickering

		//GetWindowRect(hwndReBar, &rc);
		y = cyReBar + cyReBarFrame;		 // define
		cy -= cyReBar + cyReBarFrame;	 // border
	}

	if (bShowStatusbar) {
		SendMessage(hwndStatus, WM_SIZE, 0, 0);
		GetWindowRect(hwndStatus, &rc);
		cy -= (rc.bottom - rc.top);
	}

	hdwp = BeginDeferWindowPos(2);

	DeferWindowPos(hdwp, hwndEditFrame, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

	DeferWindowPos(hdwp, hwndEdit, NULL, x + cxEditFrame, y + cyEditFrame,
				   cx - 2 * cxEditFrame, cy - 2 * cyEditFrame, SWP_NOZORDER | SWP_NOACTIVATE);

	EndDeferWindowPos(hdwp);

	// Statusbar width
	UpdateStatusBarWidth();
}

void UpdateStatusBarCache(int item) {
	switch (item) {
	case STATUS_LEXER:
		cachedStatusItem.pszLexerName = Style_GetCurrentLexerName();
		cachedStatusItem.lexerNameChanged = TRUE;
		UpdateStatusBarWidth();
		break;

	case STATUS_CODEPAGE:
		Encoding_GetLabel(iEncoding);
		cachedStatusItem.encodingChanged = TRUE;
		UpdateStatusBarWidth();
		break;

	case STATUS_EOLMODE:
		cachedStatusItem.pszEOLMode = (iEOLMode == SC_EOL_LF) ? L"LF" : ((iEOLMode == SC_EOL_CR) ? L"CR" : L"CR+LF");
		cachedStatusItem.eolModeChanged = TRUE;
		break;

	case STATUS_OVRMODE:
		cachedStatusItem.pszOvrMode = SendMessage(hwndEdit, SCI_GETOVERTYPE, 0, 0) ? L"OVR" : L"INS";
		cachedStatusItem.ovrModeChanged = TRUE;
		break;

	case STATUS_DOCZOOM:
		wsprintf(cachedStatusItem.tchZoom, L" %i%%", iZoomLevel);
		cachedStatusItem.zoomChanged = TRUE;
		break;
	}
}

void UpdateStatusBarWidth(void) {
	RECT rc;
	int aWidth[7];

	GetClientRect(hwndMain, &rc);
	const int cx = rc.right - rc.left;
	const int iBytes = SciCall_GetLength();

	aWidth[1] = StatusCalcPaneWidth(hwndStatus, cachedStatusItem.pszLexerName) + 4;
	aWidth[2] = StatusCalcPaneWidth(hwndStatus, mEncoding[iEncoding].wchLabel) + 4;
	aWidth[3] = StatusCalcPaneWidth(hwndStatus, L"CR+LF");
	aWidth[4] = StatusCalcPaneWidth(hwndStatus, L"OVR");
	aWidth[5] = StatusCalcPaneWidth(hwndStatus, ((iBytes < 1024)? L"1,023 Bytes" : L"99.9 MiB"));
	aWidth[6] = StatusCalcPaneWidth(hwndStatus, L"500%") + 16;

	aWidth[0] = max_i(120, cx - (aWidth[1] + aWidth[2] + aWidth[3] + aWidth[4] + aWidth[5] + aWidth[6]));
	aWidth[1] += aWidth[0];
	aWidth[2] += aWidth[1];
	aWidth[3] += aWidth[2];
	aWidth[4] += aWidth[3];
	aWidth[5] += aWidth[4];
	aWidth[6] = -1;

	SendMessage(hwndStatus, SB_SETPARTS, COUNTOF(aWidth), (LPARAM)aWidth);
}

BOOL IsIMEInNativeMode(void) {
	BOOL result = FALSE;
	HIMC himc = ImmGetContext(hwndEdit);
	if (himc) {
		if (ImmGetOpenStatus(himc)) {
			DWORD dwConversion = IME_CMODE_ALPHANUMERIC, dwSentence = IME_SMODE_NONE;
			if (ImmGetConversionStatus(himc, &dwConversion, &dwSentence)) {
				result = dwConversion != IME_CMODE_ALPHANUMERIC;
			}
		}
		ImmReleaseContext(hwndEdit, himc);
	}
	return result;
}

void MsgNotifyZoom(void) {
	iZoomLevel = (int)SendMessage(hwndEdit, SCI_GETZOOM, 0, 0);

	UpdateStatusBarCache(STATUS_DOCZOOM);
	UpdateLineNumberWidth();
	UpdateFoldMarginWidth();
	UpdateStatusbar();
}

//=============================================================================
//
// MsgInitMenu() - Handles WM_INITMENU
//
//
void MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	int i, i2;
	HMENU hmenu = (HMENU)wParam;

	i = StrNotEmpty(szCurFile);
	EnableCmd(hmenu, IDM_FILE_REVERT, i);
	EnableCmd(hmenu, CMD_RELOADASCIIASUTF8, i);
	EnableCmd(hmenu, CMD_RELOADANSI, i);
	EnableCmd(hmenu, CMD_RELOADOEM, i);
	EnableCmd(hmenu, CMD_RELOADNOFILEVARS, i);
	EnableCmd(hmenu, CMD_RECODEDEFAULT, i);
	EnableCmd(hmenu, IDM_FILE_LAUNCH, i);
	EnableCmd(hmenu, IDM_FILE_PROPERTIES, i);
	EnableCmd(hmenu, IDM_FILE_CREATELINK, i);
	EnableCmd(hmenu, IDM_FILE_ADDTOFAV, i);

	EnableCmd(hmenu, IDM_FILE_RELAUNCH_ELEVATED, IsVistaAndAbove() && !fIsElevated);
	EnableCmd(hmenu, IDM_FILE_READONLY, i);
	CheckCmd(hmenu, IDM_FILE_READONLY, bReadOnly);

	//EnableCmd(hmenu, IDM_ENCODING_UNICODEREV, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_UNICODE, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_UTF8SIGN, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_UTF8, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_ANSI, !bReadOnly);
	//EnableCmd(hmenu, IDM_LINEENDINGS_CRLF, !bReadOnly);
	//EnableCmd(hmenu, IDM_LINEENDINGS_LF, !bReadOnly);
	//EnableCmd(hmenu, IDM_LINEENDINGS_CR, !bReadOnly);

	EnableCmd(hmenu, IDM_ENCODING_RECODE, i);

	if (mEncoding[iEncoding].uFlags & NCP_UNICODE_REVERSE) {
		i = IDM_ENCODING_UNICODEREV;
	} else if (mEncoding[iEncoding].uFlags & NCP_UNICODE) {
		i = IDM_ENCODING_UNICODE;
	} else if (mEncoding[iEncoding].uFlags & NCP_UTF8_SIGN) {
		i = IDM_ENCODING_UTF8SIGN;
	} else if (mEncoding[iEncoding].uFlags & NCP_UTF8) {
		i = IDM_ENCODING_UTF8;
	} else if (mEncoding[iEncoding].uFlags & NCP_DEFAULT) {
		i = IDM_ENCODING_ANSI;
	} else {
		i = -1;
	}
	CheckMenuRadioItem(hmenu, IDM_ENCODING_ANSI, IDM_ENCODING_UTF8SIGN, i, MF_BYCOMMAND);

	if (iEOLMode == SC_EOL_CRLF) {
		i = IDM_LINEENDINGS_CRLF;
	} else if (iEOLMode == SC_EOL_LF) {
		i = IDM_LINEENDINGS_LF;
	} else {
		i = IDM_LINEENDINGS_CR;
	}
	CheckMenuRadioItem(hmenu, IDM_LINEENDINGS_CRLF, IDM_LINEENDINGS_CR, i, MF_BYCOMMAND);

	EnableCmd(hmenu, IDM_FILE_RECENT, (MRU_GetCount(pFileMRU) > 0));

	EnableCmd(hmenu, IDM_EDIT_UNDO, SendMessage(hwndEdit, SCI_CANUNDO, 0, 0) /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_REDO, SendMessage(hwndEdit, SCI_CANREDO, 0, 0) /*&& !bReadOnly*/);

	i  = !EditIsEmptySelection();
	i2 = (int)SendMessage(hwndEdit, SCI_CANPASTE, 0, 0);

	EnableCmd(hmenu, IDM_EDIT_CUT, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_COPY, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_COPYALL, SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_COPYADD, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_PASTE, i2 /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_SWAP, i || i2 /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_CLEAR, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_COPYRTF, i /*&& !bReadOnly*/);

	OpenClipboard(hwnd);
	EnableCmd(hmenu, IDM_EDIT_CLEARCLIPBOARD, CountClipboardFormats());
	CloseClipboard();

	//EnableCmd(hmenu, IDM_EDIT_MOVELINEUP, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_MOVELINEDOWN, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_LINETRANSPOSE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_DUPLICATELINE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_CUTLINE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_COPYLINE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_DELETELINE, !bReadOnly);

	//EnableCmd(hmenu, IDM_EDIT_INDENT, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_UNINDENT, !bReadOnly);

	//EnableCmd(hmenu, IDM_EDIT_PADWITHSPACES, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_STRIP1STCHAR, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_STRIPLASTCHAR, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_TRIMLINES, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_TRIMLEAD, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_MERGEBLANKLINES, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_REMOVEBLANKLINES, !bReadOnly);

	EnableCmd(hmenu, IDM_EDIT_SORTLINES,
			  SendMessage(hwndEdit, SCI_LINEFROMPOSITION,
						  (WPARAM)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0), 0) -
			  SendMessage(hwndEdit, SCI_LINEFROMPOSITION,
						  (WPARAM)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0), 0) >= 1);

	EnableCmd(hmenu, IDM_EDIT_COLUMNWRAP, i /*&& IsWindowsNT()*/);
	EnableCmd(hmenu, IDM_EDIT_SPLITLINES, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_JOINLINES, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_JOINLINESEX, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_EDIT_CONVERTUPPERCASE, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_CONVERTLOWERCASE, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_INVERTCASE, i /*&& !bReadOnly*/ /*&& IsWindowsNT()*/);
	EnableCmd(hmenu, IDM_EDIT_TITLECASE, i /*&& !bReadOnly*/ /*&& IsWindowsNT()*/);
	EnableCmd(hmenu, IDM_EDIT_SENTENCECASE, i /*&& !bReadOnly*/ /*&& IsWindowsNT()*/);

	EnableCmd(hmenu, IDM_EDIT_CONVERTTABS, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_CONVERTSPACES, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_CONVERTTABS2, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_CONVERTSPACES2, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_EDIT_URLENCODE, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_URLDECODE, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_EDIT_XHTML_ESCAPE_CHAR, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_XHTML_UNESCAPE_CHAR, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_EDIT_ESCAPECCHARS, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_UNESCAPECCHARS, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_EDIT_CHAR2HEX, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_HEX2CHAR, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_SHOW_HEX, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_EDIT_NUM2HEX, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_NUM2DEC, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_NUM2BIN, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_NUM2OCT, i /*&& !bReadOnly*/);

	//EnableCmd(hmenu, IDM_EDIT_INCREASENUM, i /*&& !bReadOnly*/);
	//EnableCmd(hmenu, IDM_EDIT_DECREASENUM, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_VIEW_SHOWEXCERPT, i);

	i = (int)SendMessage(hwndEdit, SCI_GETLEXER, 0, 0);
	EnableCmd(hmenu, IDM_EDIT_LINECOMMENT,
			  !(i == SCLEX_NULL || i == SCLEX_CSS || i == SCLEX_DIFF));
	EnableCmd(hmenu, IDM_EDIT_STREAMCOMMENT,
			  !(i == SCLEX_NULL || i == SCLEX_VBSCRIPT || i == SCLEX_MAKEFILE || i == SCLEX_VB || i == SCLEX_ASM ||
				i == SCLEX_SQL || i == SCLEX_PERL || i == SCLEX_PYTHON || i == SCLEX_PROPERTIES || i == SCLEX_CONF ||
				i == SCLEX_POWERSHELL || i == SCLEX_BATCH || i == SCLEX_DIFF || i == SCLEX_BASH || i == SCLEX_TCL ||
				i == SCLEX_AU3 || i == SCLEX_RUBY || i == SCLEX_CMAKE));

	EnableCmd(hmenu, IDM_EDIT_INSERT_ENCODING, *mEncoding[iEncoding].pszParseNames);

	//EnableCmd(hmenu, IDM_EDIT_INSERT_SHORTDATE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_INSERT_LONGDATE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_INSERT_FILENAME, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_INSERT_PATHNAME, !bReadOnly);

	i = (int)SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) > 0;
	EnableCmd(hmenu, IDM_EDIT_FIND, i);
	EnableCmd(hmenu, IDM_EDIT_SAVEFIND, i);
	EnableCmd(hmenu, IDM_EDIT_FINDNEXT, i);
	EnableCmd(hmenu, IDM_EDIT_FINDPREV, i && StrNotEmptyA(efrData.szFind));
	EnableCmd(hmenu, IDM_EDIT_REPLACE, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_REPLACENEXT, i);
	//EnableCmd(hmenu, IDM_EDIT_SELECTWORD, i);
	//EnableCmd(hmenu, IDM_EDIT_SELECTLINE, i);
	EnableCmd(hmenu, IDM_EDIT_SELTOEND, i);
	EnableCmd(hmenu, IDM_EDIT_SELTOBEGIN, i);
	EnableCmd(hmenu, IDM_EDIT_SELTONEXT, i && StrNotEmptyA(efrData.szFind));
	EnableCmd(hmenu, IDM_EDIT_SELTOPREV, i && StrNotEmptyA(efrData.szFind));
	EnableCmd(hmenu, IDM_EDIT_FINDMATCHINGBRACE, i);
	EnableCmd(hmenu, IDM_EDIT_SELTOMATCHINGBRACE, i);
	EnableCmd(hmenu, CMD_JUMP2SELSTART, i);
	EnableCmd(hmenu, CMD_JUMP2SELEND, i);
#ifdef BOOKMARK_EDITION
	EnableCmd(hmenu, BME_EDIT_BOOKMARKPREV, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKNEXT, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKTOGGLE, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKCLEAR, i);
#endif
	EnableCmd(hmenu, IDM_EDIT_DELETELINELEFT, i);
	EnableCmd(hmenu, IDM_EDIT_DELETELINERIGHT, i);
	EnableCmd(hmenu, CMD_CTRLBACK, i);
	EnableCmd(hmenu, CMD_CTRLDEL, i);
	EnableCmd(hmenu, CMD_TIMESTAMPS, i);
	//EnableCmd(hmenu, IDM_VIEW_TOGGLEFOLDS, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_ALL, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL1, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL2, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL3, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL4, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL5, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL6, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL7, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL8, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL9, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_LEVEL10, i && bShowCodeFolding);
	CheckCmd(hmenu, IDM_VIEW_FOLDING, bShowCodeFolding);

	CheckCmd(hmenu, IDM_VIEW_USE2NDDEFAULT, Style_GetUse2ndDefault());

	CheckCmd(hmenu, IDM_VIEW_WORDWRAP, fWordWrap);
	i = IDM_VIEW_FONTQUALITY_DEFAULT + iFontQuality;
	CheckMenuRadioItem(hmenu, IDM_VIEW_FONTQUALITY_DEFAULT, IDM_VIEW_FONTQUALITY_CLEARTYPE, i, MF_BYCOMMAND);
	i = IDM_VIEW_CARET_STYLE_BLOCK + iCaretStyle;
	CheckMenuRadioItem(hmenu, IDM_VIEW_CARET_STYLE_BLOCK, IDM_VIEW_CARET_STYLE_WIDTH3, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_CARET_STYLE_NOBLINK, iCaretBlinkPeriod == 0);
	CheckCmd(hmenu, IDM_VIEW_LONGLINEMARKER, bMarkLongLines);
	CheckCmd(hmenu, IDM_VIEW_TABSASSPACES, bTabsAsSpaces);
	CheckCmd(hmenu, IDM_VIEW_SHOWINDENTGUIDES, bShowIndentGuides);
	CheckCmd(hmenu, IDM_VIEW_LINENUMBERS, bShowLineNumbers);
	CheckCmd(hmenu, IDM_VIEW_MARGIN, bShowSelectionMargin);
	EnableCmd(hmenu, IDM_EDIT_COMPLETEWORD, i);
	CheckCmd(hmenu, IDM_VIEW_AUTOINDENTTEXT, bAutoIndent);
	CheckCmd(hmenu, IDM_VIEW_AUTOCOMPLETEQUOTE, bAutoCloseBracesQuotes);
	CheckCmd(hmenu, IDM_VIEW_AUTOCOMPLETEWORDS, bAutoCompleteWords);
	CheckCmd(hmenu, IDM_VIEW_AUTOCWITHDOCWORDS, bAutoCIncludeDocWord);
	CheckCmd(hmenu, IDM_VIEW_AUTOC_ENGLISH_ONLY, bAutoCEnglishIMEModeOnly);

	switch (iMarkOccurrences) {
	case 0:
		i = IDM_VIEW_MARKOCCURRENCES_OFF;
		break;
	case 3:
		i = IDM_VIEW_MARKOCCURRENCES_BLUE;
		break;
	case 2:
		i = IDM_VIEW_MARKOCCURRENCES_GREEN;
		break;
	case 1:
		i = IDM_VIEW_MARKOCCURRENCES_RED;
		break;
	}
	CheckMenuRadioItem(hmenu, IDM_VIEW_MARKOCCURRENCES_OFF, IDM_VIEW_MARKOCCURRENCES_RED, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_CASE, bMarkOccurrencesMatchCase);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_WORD, bMarkOccurrencesMatchWords);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_CASE, iMarkOccurrences != 0);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_WORD, iMarkOccurrences != 0);

	CheckCmd(hmenu, IDM_VIEW_SHOWWHITESPACE, bViewWhiteSpace);
	CheckCmd(hmenu, IDM_VIEW_SHOWEOLS, bViewEOLs);
	CheckCmd(hmenu, IDM_VIEW_WORDWRAPSYMBOLS, bShowWordWrapSymbols);
#if NP2_ENABLE_SHOW_CALL_TIPS
	CheckCmd(hmenu, IDM_VIEW_SHOWCALLTIPS, bShowCallTips);
#endif
	CheckCmd(hmenu, IDM_VIEW_MATCHBRACES, bMatchBraces);
	CheckCmd(hmenu, IDM_VIEW_TOOLBAR, bShowToolbar);
	EnableCmd(hmenu, IDM_VIEW_CUSTOMIZETB, bShowToolbar);
	CheckCmd(hmenu, IDM_VIEW_STATUSBAR, bShowStatusbar);

	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_ON_START, bFullScreenOnStartup);
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_HIDE_MENU, bFullScreenHideMenu);
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_HIDE_TOOL, bFullScreenHideToolbar);
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_HIDE_STATUS, bFullScreenHideStatusbar);

	//i = (int)SendMessage(hwndEdit, SCI_GETLEXER, 0, 0);
	//EnableCmd(hmenu, IDM_VIEW_AUTOCLOSETAGS, (i == SCLEX_HTML || i == SCLEX_XML));
	CheckCmd(hmenu, IDM_VIEW_AUTOCLOSETAGS, bAutoCloseTags /*&& (i == SCLEX_HTML || i == SCLEX_XML)*/);
	CheckCmd(hmenu, IDM_VIEW_HILITECURRENTLINE, bHiliteCurrentLine);

	CheckCmd(hmenu, IDM_VIEW_REUSEWINDOW, bReuseWindow);
	CheckCmd(hmenu, IDM_VIEW_SINGLEFILEINSTANCE, bSingleFileInstance);
	CheckCmd(hmenu, IDM_VIEW_STICKYWINPOS, bStickyWinPos);
	CheckCmd(hmenu, IDM_VIEW_ALWAYSONTOP, ((bAlwaysOnTop || flagAlwaysOnTop == 2) && flagAlwaysOnTop != 1));
	CheckCmd(hmenu, IDM_VIEW_MINTOTRAY, bMinimizeToTray);
	i = IsWin2KAndAbove(); // bTransparentModeAvailable
	CheckCmd(hmenu, IDM_VIEW_TRANSPARENT, bTransparentMode && i);
	EnableCmd(hmenu, IDM_VIEW_TRANSPARENT, i);

	// Rendering Technology
	i = IsVistaAndAbove();
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2D, i);
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2DRETAIN, i);
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2DDC, i);
	i = IDM_SET_RENDER_TECH_GDI + iRenderingTechnology;
	CheckMenuRadioItem(hmenu, IDM_SET_RENDER_TECH_GDI, IDM_SET_RENDER_TECH_D2DDC, i, MF_BYCOMMAND);
	// Bidirectional
	i = iRenderingTechnology != SC_TECHNOLOGY_DEFAULT;
	EnableCmd(hmenu, IDM_SET_BIDIRECTIONAL_L2R, i);
	EnableCmd(hmenu, IDM_SET_BIDIRECTIONAL_R2L, i);
	i = IDM_SET_BIDIRECTIONAL_NONE + iBidirectional;
	CheckMenuRadioItem(hmenu, IDM_SET_BIDIRECTIONAL_NONE, IDM_SET_BIDIRECTIONAL_R2L, i, MF_BYCOMMAND);

	CheckCmd(hmenu, IDM_SET_USE_INLINE_IME, bUseInlineIME);
	CheckCmd(hmenu, IDM_SET_USE_BLOCK_CARET, bInlineIMEUseBlockCaret);

	CheckCmd(hmenu, IDM_VIEW_NOSAVEFINDREPL, bSaveFindReplace);
	CheckCmd(hmenu, IDM_VIEW_SAVEBEFORERUNNINGTOOLS, bSaveBeforeRunningTools);

	CheckCmd(hmenu, IDM_VIEW_CHANGENOTIFY, iFileWatchingMode);

	if (StrNotEmpty(szTitleExcerpt)) {
		i = IDM_VIEW_SHOWEXCERPT;
	} else if (iPathNameFormat == 0) {
		i = IDM_VIEW_SHOWFILENAMEONLY;
	} else if (iPathNameFormat == 1) {
		i = IDM_VIEW_SHOWFILENAMEFIRST;
	} else {
		i = IDM_VIEW_SHOWFULLPATH;
	}
	CheckMenuRadioItem(hmenu, IDM_VIEW_SHOWFILENAMEONLY, IDM_VIEW_SHOWEXCERPT, i, MF_BYCOMMAND);

	if (iEscFunction == 1) {
		i = IDM_VIEW_ESCMINIMIZE;
	} else if (iEscFunction == 2) {
		i = IDM_VIEW_ESCEXIT;
	} else {
		i = IDM_VIEW_NOESCFUNC;
	}
	CheckMenuRadioItem(hmenu, IDM_VIEW_NOESCFUNC, IDM_VIEW_ESCEXIT, i, MF_BYCOMMAND);

	i = StrNotEmpty(szIniFile);
	CheckCmd(hmenu, IDM_VIEW_SAVESETTINGS, bSaveSettings && i);
	EnableCmd(hmenu, CMD_OPENINIFILE, i);

	EnableCmd(hmenu, IDM_VIEW_REUSEWINDOW, i);
	EnableCmd(hmenu, IDM_VIEW_STICKYWINPOS, i);
	EnableCmd(hmenu, IDM_VIEW_SINGLEFILEINSTANCE, i);
	EnableCmd(hmenu, IDM_VIEW_NOSAVEFINDREPL, i);
	EnableCmd(hmenu, IDM_VIEW_SAVESETTINGS, i);

	i = i || StrNotEmpty(szIniFile2);
	EnableCmd(hmenu, IDM_VIEW_SAVESETTINGSNOW, i);

	Style_UpdateSchemeMenu(hmenu);
}

//=============================================================================
//
// MsgCommand() - Handles WM_COMMAND
//
//
LRESULT MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam)) {
	case IDM_FILE_NEW:
		FileLoad(FALSE, TRUE, FALSE, FALSE, L"");
		break;

	case IDM_FILE_OPEN:
		FileLoad(FALSE, FALSE, FALSE, FALSE, L"");
		break;

	case IDM_FILE_REVERT: {
		if (StrNotEmpty(szCurFile)) {
#if NP2_ENABLE_DOT_LOG_FEATURE
			int iCurPos		= (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
			int iAnchorPos	= (int)SendMessage(hwndEdit, SCI_GETANCHOR, 0, 0);
			int iVisTopLine	= (int)SendMessage(hwndEdit, SCI_GETFIRSTVISIBLELINE, 0, 0);
			int iDocTopLine	= (int)SendMessage(hwndEdit, SCI_DOCLINEFROMVISIBLE, (WPARAM)iVisTopLine, 0);
			int iXOffset	= (int)SendMessage(hwndEdit, SCI_GETXOFFSET, 0, 0);
#endif
			if ((bModified || iEncoding != iOriginalEncoding) && MsgBox(MBOKCANCEL, IDS_ASK_REVERT) != IDOK) {
				return 0;
			}

			iWeakSrcEncoding = iEncoding;
			if (FileLoad(TRUE, FALSE, TRUE, FALSE, szCurFile)) {
#if NP2_ENABLE_DOT_LOG_FEATURE
				if (SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) >= 4) {
					char tch[5] = "";
					SendMessage(hwndEdit, SCI_GETTEXT, 5, (LPARAM)tch);
					if (StrEqual(tch, ".LOG")) {
						int iNewTopLine;
						SendMessage(hwndEdit, SCI_SETSEL, iAnchorPos, iCurPos);
						SendMessage(hwndEdit, SCI_ENSUREVISIBLE, (WPARAM)iDocTopLine, 0);
						iNewTopLine = (int)SendMessage(hwndEdit, SCI_GETFIRSTVISIBLELINE, 0, 0);
						SendMessage(hwndEdit, SCI_LINESCROLL, 0, (LPARAM)iVisTopLine - iNewTopLine);
						SendMessage(hwndEdit, SCI_SETXOFFSET, (WPARAM)iXOffset, 0);
					}
				}
#endif
			}
		}
	}
	break;

	case IDM_FILE_SAVE:
		FileSave(TRUE, FALSE, FALSE, FALSE);
		break;

	case IDM_FILE_SAVEAS:
		FileSave(TRUE, FALSE, TRUE, FALSE);
		break;

	case IDM_FILE_SAVECOPY:
		FileSave(TRUE, FALSE, TRUE, TRUE);
		break;

	case IDM_FILE_READONLY:
		//bReadOnly = !bReadOnly;
		//SendMessage(hwndEdit, SCI_SETREADONLY, bReadOnly, 0);
		//UpdateToolbar();
		if (StrNotEmpty(szCurFile)) {
			DWORD dwFileAttributes = GetFileAttributes(szCurFile);
			if (dwFileAttributes != INVALID_FILE_ATTRIBUTES) {
				if (bReadOnly) {
					dwFileAttributes = (dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
				} else {
					dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
				}
				if (!SetFileAttributes(szCurFile, dwFileAttributes)) {
					MsgBox(MBWARN, IDS_READONLY_MODIFY, szCurFile);
				}
			} else {
				MsgBox(MBWARN, IDS_READONLY_MODIFY, szCurFile);
			}

			dwFileAttributes = GetFileAttributes(szCurFile);
			bReadOnly = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);
			UpdateWindowTitle();
		}
		break;

	case IDM_FILE_BROWSE: {
		SHELLEXECUTEINFO sei;
		WCHAR tchParam[MAX_PATH + 4] = L"";
		WCHAR tchExeFile[MAX_PATH + 4];
		WCHAR tchTemp[MAX_PATH + 4];

		if (!IniGetString(L"Settings2", L"filebrowser.exe", L"", tchTemp, COUNTOF(tchTemp))) {
			if (!SearchPath(NULL, L"metapath.exe", NULL, COUNTOF(tchExeFile), tchExeFile, NULL)) {
				GetModuleFileName(NULL, tchExeFile, COUNTOF(tchExeFile));
				PathRemoveFileSpec(tchExeFile);
				PathAppend(tchExeFile, L"metapath.exe");
			}
		} else {
			ExtractFirstArgument(tchTemp, tchExeFile, tchParam);
			if (PathIsRelative(tchExeFile)) {
				if (!SearchPath(NULL, tchExeFile, NULL, COUNTOF(tchTemp), tchTemp, NULL)) {
					GetModuleFileName(NULL, tchTemp, COUNTOF(tchTemp));
					PathRemoveFileSpec(tchTemp);
					PathAppend(tchTemp, tchExeFile);
					lstrcpy(tchExeFile, tchTemp);
				}
			}
		}

		if (StrNotEmpty(tchParam) && StrNotEmpty(szCurFile)) {
			StrCatBuff(tchParam, L" ", COUNTOF(tchParam));
		}

		if (StrNotEmpty(szCurFile)) {
			lstrcpy(tchTemp, szCurFile);
			PathQuoteSpaces(tchTemp);
			StrCatBuff(tchParam, tchTemp, COUNTOF(tchParam));
		}

		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_FLAG_NO_UI | /*SEE_MASK_NOZONECHECKS*/0x00800000;
		sei.hwnd = hwnd;
		sei.lpVerb = NULL;
		sei.lpFile = tchExeFile;
		sei.lpParameters = tchParam;
		sei.lpDirectory = NULL;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);

		if ((INT_PTR)sei.hInstApp < 32) {
			MsgBox(MBWARN, IDS_ERR_BROWSE);
		}
	}
	break;

	case IDM_FILE_NEWWINDOW:
	case IDM_FILE_NEWWINDOW2: {
		SHELLEXECUTEINFO sei;
		WCHAR szModuleName[MAX_PATH];
		WCHAR *szParameters;
		BOOL emptyWind = LOWORD(wParam) == IDM_FILE_NEWWINDOW2;

		if (!emptyWind && bSaveBeforeRunningTools && !FileSave(FALSE, TRUE, FALSE, FALSE)) {
			break;
		}

		GetModuleFileName(NULL, szModuleName, COUNTOF(szModuleName));
		szParameters = NP2HeapAlloc(sizeof(WCHAR) * 1024);
		GetRelaunchParameters(szParameters, szCurFile, TRUE, emptyWind);

		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = /*SEE_MASK_NOZONECHECKS*/0x00800000;
		sei.hwnd = hwnd;
		sei.lpVerb = NULL;
		sei.lpFile = szModuleName;
		sei.lpParameters = szParameters;
		sei.lpDirectory = g_wchWorkingDirectory;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);
		NP2HeapFree(szParameters);
	}
	break;

	case IDM_FILE_RELAUNCH_ELEVATED:
		flagRelaunchElevated = 2;
		if (RelaunchElevated()) {
			DestroyWindow(hwnd);
		}
		break;

	case IDM_FILE_LAUNCH: {
		SHELLEXECUTEINFO sei;
		WCHAR wchDirectory[MAX_PATH] = L"";

		if (StrIsEmpty(szCurFile)) {
			break;
		}

		if (bSaveBeforeRunningTools && !FileSave(FALSE, TRUE, FALSE, FALSE)) {
			break;
		}

		if (StrNotEmpty(szCurFile)) {
			lstrcpy(wchDirectory, szCurFile);
			PathRemoveFileSpec(wchDirectory);
		}

		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = NULL;
		sei.lpFile = szCurFile;
		sei.lpParameters = NULL;
		sei.lpDirectory = wchDirectory;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);
	}
	break;

	case IDM_FILE_RUN: {
		WCHAR tchCmdLine[MAX_PATH + 4];

		if (bSaveBeforeRunningTools && !FileSave(FALSE, TRUE, FALSE, FALSE)) {
			break;
		}

		lstrcpy(tchCmdLine, szCurFile);
		PathQuoteSpaces(tchCmdLine);

		RunDlg(hwnd, tchCmdLine);
	}
	break;

	case IDM_FILE_OPENWITH:
		if (bSaveBeforeRunningTools && !FileSave(FALSE, TRUE, FALSE, FALSE)) {
			break;
		}
		OpenWithDlg(hwnd, szCurFile);
		break;

	case IDM_FILE_PAGESETUP:
		EditPrintSetup(hwndEdit);
		break;

	case IDM_FILE_PRINT: {
		SHFILEINFO shfi;
		WCHAR *pszTitle;
		WCHAR tchUntitled[32];
		WCHAR tchPageFmt[32];

		if (StrNotEmpty(szCurFile)) {
			SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			pszTitle = shfi.szDisplayName;
		} else {
			GetString(IDS_UNTITLED, tchUntitled, COUNTOF(tchUntitled));
			pszTitle = tchUntitled;
		}

		GetString(IDS_PRINT_PAGENUM, tchPageFmt, COUNTOF(tchPageFmt));

		if (!EditPrint(hwndEdit, pszTitle, tchPageFmt)) {
			MsgBox(MBWARN, IDS_PRINT_ERROR, pszTitle);
		}
	}
	break;

	case IDM_FILE_PROPERTIES: {
		SHELLEXECUTEINFO sei;

		if (StrIsEmpty(szCurFile)) {
			break;
		}

		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_INVOKEIDLIST;
		sei.hwnd = hwnd;
		sei.lpVerb = L"properties";
		sei.lpFile = szCurFile;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);
	}
	break;

	case IDM_FILE_CREATELINK: {
		if (StrIsEmpty(szCurFile)) {
			break;
		}

		if (!PathCreateDeskLnk(szCurFile)) {
			MsgBox(MBWARN, IDS_ERR_CREATELINK);
		}
	}
	break;

	case IDM_FILE_OPENFAV:
		if (FileSave(FALSE, TRUE, FALSE, FALSE)) {
			WCHAR tchSelItem[MAX_PATH];

			if (FavoritesDlg(hwnd, tchSelItem)) {
				if (PathIsLnkToDirectory(tchSelItem, NULL, 0)) {
					PathGetLnkPath(tchSelItem, tchSelItem, COUNTOF(tchSelItem));
				}

				if (PathIsDirectory(tchSelItem)) {
					WCHAR tchFile[MAX_PATH];

					if (OpenFileDlg(hwndMain, tchFile, COUNTOF(tchFile), tchSelItem)) {
						FileLoad(TRUE, FALSE, FALSE, FALSE, tchFile);
					}
				} else {
					FileLoad(TRUE, FALSE, FALSE, FALSE, tchSelItem);
				}
			}
		}
		break;

	case IDM_FILE_ADDTOFAV:
		if (StrNotEmpty(szCurFile)) {
			SHFILEINFO shfi;
			SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			AddToFavDlg(hwnd, shfi.szDisplayName, szCurFile);
		}
		break;

	case IDM_FILE_MANAGEFAV: {
		SHELLEXECUTEINFO sei;
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = NULL;
		sei.lpFile = tchFavoritesDir;
		sei.lpParameters = NULL;
		sei.lpDirectory = NULL;
		sei.nShow = SW_SHOWNORMAL;

		// Run favorites directory
		ShellExecuteEx(&sei);
	}
	break;

	case IDM_FILE_RECENT:
		if (MRU_GetCount(pFileMRU) > 0) {
			if (FileSave(FALSE, TRUE, FALSE, FALSE)) {
				WCHAR tchFile[MAX_PATH];
				if (FileMRUDlg(hwnd, tchFile)) {
					FileLoad(TRUE, FALSE, FALSE, FALSE, tchFile);
				}
			}
		}
		break;

	case IDM_FILE_EXIT:
		NP2ExitWind(hwnd);
		break;

	case IDM_ENCODING_ANSI:
	case IDM_ENCODING_UNICODE:
	case IDM_ENCODING_UNICODEREV:
	case IDM_ENCODING_UTF8:
	case IDM_ENCODING_UTF8SIGN:
	case IDM_ENCODING_SELECT: {
		int iNewEncoding = iEncoding;
		if (LOWORD(wParam) == IDM_ENCODING_SELECT && !SelectEncodingDlg(hwnd, &iNewEncoding)) {
			break;
		}

		switch (LOWORD(wParam)) {
		case IDM_ENCODING_UNICODE:
			iNewEncoding = CPI_UNICODEBOM;
			break;
		case IDM_ENCODING_UNICODEREV:
			iNewEncoding = CPI_UNICODEBEBOM;
			break;
		case IDM_ENCODING_UTF8:
			iNewEncoding = CPI_UTF8;
			break;
		case IDM_ENCODING_UTF8SIGN:
			iNewEncoding = CPI_UTF8SIGN;
			break;
		case IDM_ENCODING_ANSI:
			iNewEncoding = CPI_DEFAULT;
			break;
		}

		if (EditSetNewEncoding(hwndEdit, iEncoding, iNewEncoding, (flagSetEncoding), StrIsEmpty(szCurFile))) {
			if (SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) == 0) {
				iEncoding = iNewEncoding;
				iOriginalEncoding = iNewEncoding;
			} else {
				if (iEncoding == CPI_DEFAULT || iNewEncoding == CPI_DEFAULT) {
					iOriginalEncoding = -1;
				}
				iEncoding = iNewEncoding;
			}

			UpdateStatusBarCache(STATUS_CODEPAGE);
			UpdateToolbar();
			UpdateStatusbar();
			UpdateWindowTitle();
		}
	}
	break;

	case IDM_ENCODING_RECODE: {
		if (StrNotEmpty(szCurFile)) {
			int iNewEncoding = -1;
			if (iEncoding != CPI_DEFAULT) {
				iNewEncoding = iEncoding;
			}
			if (iEncoding == CPI_UTF8SIGN) {
				iNewEncoding = CPI_UTF8;
			}
			if (iEncoding == CPI_UNICODEBOM) {
				iNewEncoding = CPI_UNICODE;
			}
			if (iEncoding == CPI_UNICODEBEBOM) {
				iNewEncoding = CPI_UNICODEBE;
			}

			if ((bModified || iEncoding != iOriginalEncoding) && MsgBox(MBOKCANCEL, IDS_ASK_RECODE) != IDOK) {
				return 0;
			}

			if (RecodeDlg(hwnd, &iNewEncoding)) {
				iSrcEncoding = iNewEncoding;
				FileLoad(TRUE, FALSE, TRUE, FALSE, szCurFile);
			}
		}
	}
	break;

	case IDM_ENCODING_SETDEFAULT:
		SelectDefEncodingDlg(hwnd, &iDefaultEncoding);
		break;

	case IDM_LINEENDINGS_CRLF:
	case IDM_LINEENDINGS_LF:
	case IDM_LINEENDINGS_CR: {
		int iNewEOLMode = iLineEndings[LOWORD(wParam) - IDM_LINEENDINGS_CRLF];
		iEOLMode = iNewEOLMode;
		SendMessage(hwndEdit, SCI_SETEOLMODE, iEOLMode, 0);
		SendMessage(hwndEdit, SCI_CONVERTEOLS, iEOLMode, 0);
		EditFixPositions(hwndEdit);
		UpdateStatusBarCache(STATUS_EOLMODE);
		UpdateToolbar();
		UpdateStatusbar();
		UpdateWindowTitle();
	}
	break;

	case IDM_LINEENDINGS_SETDEFAULT:
		SelectDefLineEndingDlg(hwnd, &iDefaultEOLMode);
		break;

	case IDM_EDIT_UNDO:
		SendMessage(hwndEdit, SCI_UNDO, 0, 0);
		break;

	case IDM_EDIT_REDO:
		SendMessage(hwndEdit, SCI_REDO, 0, 0);
		break;

	case IDM_EDIT_CUT:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SendMessage(hwndEdit, SCI_CUT, 0, 0);
		break;

	case IDM_EDIT_COPY:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SendMessage(hwndEdit, SCI_COPY, 0, 0);
		UpdateToolbar();
		break;

	case IDM_EDIT_COPYALL:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SendMessage(hwndEdit, SCI_COPYRANGE, 0, SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0));
		UpdateToolbar();
		break;

	case IDM_EDIT_COPYADD:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		EditCopyAppend(hwndEdit);
		UpdateToolbar();
		break;

	case IDM_EDIT_PASTE:
		SendMessage(hwndEdit, SCI_PASTE, 0, 0);
		break;

	case IDM_EDIT_SWAP:
		if (SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0) -
				SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0) == 0) {
			int iNewPos;
			int iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
			SendMessage(hwndEdit, SCI_PASTE, 0, 0);
			iNewPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
			SendMessage(hwndEdit, SCI_SETSEL, iPos, iNewPos);
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_CLEARCLIPBOARD, 1), 0);
		} else {
			int iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
			int iAnchor = (int)SendMessage(hwndEdit, SCI_GETANCHOR, 0, 0);
			char *pClip = EditGetClipboardText(hwndEdit);
			if (flagPasteBoard) {
				bLastCopyFromMe = TRUE;
			}
			SendMessage(hwndEdit, SCI_BEGINUNDOACTION, 0, 0);
			SendMessage(hwndEdit, SCI_CUT, 0, 0);
			SendMessage(hwndEdit, SCI_REPLACESEL, (WPARAM)0, (LPARAM)pClip);
			if (iPos > iAnchor) {
				SendMessage(hwndEdit, SCI_SETSEL, iAnchor, iAnchor + lstrlenA(pClip));
			} else {
				SendMessage(hwndEdit, SCI_SETSEL, iPos + lstrlenA(pClip), iPos);
			}
			SendMessage(hwndEdit, SCI_ENDUNDOACTION, 0, 0);
			LocalFree(pClip);
		}
		break;

	case IDM_EDIT_CLEAR:
		SendMessage(hwndEdit, SCI_CLEAR, 0, 0);
		break;

	case IDM_EDIT_CLEARCLIPBOARD:
		if (OpenClipboard(hwnd)) {
			if (CountClipboardFormats() > 0) {
				EmptyClipboard();
				UpdateToolbar();
				UpdateStatusbar();
			}
			CloseClipboard();
		}
		break;

	case IDM_EDIT_SELECTALL:
		SendMessage(hwndEdit, SCI_SELECTALL, 0, 0);
		//SendMessage(hwndEdit, SCI_SETSEL, 0, (LPARAM)-1);
		break;

	case IDM_EDIT_SELECTWORD:
		EditSelectWord(hwndEdit);
		break;

	case IDM_EDIT_SELECTLINE:
		EditSelectLine(hwndEdit);
		break;

	case IDM_EDIT_MOVELINEUP:
		EditMoveUp(hwndEdit);
		break;

	case IDM_EDIT_MOVELINEDOWN:
		EditMoveDown(hwndEdit);
		break;

	case IDM_EDIT_LINETRANSPOSE:
		SendMessage(hwndEdit, SCI_LINETRANSPOSE, 0, 0);
		break;

	case IDM_EDIT_DUPLICATELINE:
		SendMessage(hwndEdit, SCI_LINEDUPLICATE, 0, 0);
		break;

	case IDM_EDIT_CUTLINE:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SendMessage(hwndEdit, SCI_LINECUT, 0, 0);
		break;

	case IDM_EDIT_COPYLINE:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SendMessage(hwndEdit, SCI_LINECOPY, 0, 0);
		UpdateToolbar();
		break;

	case IDM_EDIT_DELETELINE:
		SendMessage(hwndEdit, SCI_LINEDELETE, 0, 0);
		break;

	case IDM_EDIT_DELETELINELEFT:
		SendMessage(hwndEdit, SCI_DELLINELEFT, 0, 0);
		break;

	case IDM_EDIT_DELETELINERIGHT:
		SendMessage(hwndEdit, SCI_DELLINERIGHT, 0, 0);
		break;

	case IDM_EDIT_INDENT: {
		int iLineSelStart	= (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION,
											   (int)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0), 0);
		int iLineSelEnd		= (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION,
											   (int)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0), 0);

		SendMessage(hwndEdit, SCI_SETTABINDENTS, TRUE, 0);
		if (iLineSelStart == iLineSelEnd) {
			SendMessage(hwndEdit, SCI_VCHOME, 0, 0);
			SendMessage(hwndEdit, SCI_TAB, 0, 0);
		} else {
			SendMessage(hwndEdit, SCI_TAB, 0, 0);
		}
		SendMessage(hwndEdit, SCI_SETTABINDENTS, bTabIndents, 0);
	}
	break;

	case IDM_EDIT_UNINDENT: {
		int iLineSelStart	= (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION,
											   (int)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0), 0);
		int iLineSelEnd		= (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION,
											   (int)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0), 0);

		SendMessage(hwndEdit, SCI_SETTABINDENTS, TRUE, 0);
		if (iLineSelStart == iLineSelEnd) {
			SendMessage(hwndEdit, SCI_VCHOME, 0, 0);
			SendMessage(hwndEdit, SCI_BACKTAB, 0, 0);
		} else {
			SendMessage(hwndEdit, SCI_BACKTAB, 0, 0);
		}
		SendMessage(hwndEdit, SCI_SETTABINDENTS, bTabIndents, 0);
	}
	break;

	case IDM_EDIT_ENCLOSESELECTION:
		if (EditEncloseSelectionDlg(hwnd, wchPrefixSelection, wchAppendSelection)) {
			BeginWaitCursor();
			EditEncloseSelection(hwndEdit, wchPrefixSelection, wchAppendSelection);
			EndWaitCursor();
		}
		break;

	case IDM_EDIT_SELECTIONDUPLICATE:
		SendMessage(hwndEdit, SCI_BEGINUNDOACTION, 0, 0);
		SendMessage(hwndEdit, SCI_SELECTIONDUPLICATE, 0, 0);
		SendMessage(hwndEdit, SCI_ENDUNDOACTION, 0, 0);
		break;

	case IDM_EDIT_PADWITHSPACES:
		BeginWaitCursor();
		EditPadWithSpaces(hwndEdit, FALSE, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_STRIP1STCHAR:
		BeginWaitCursor();
		EditStripFirstCharacter(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_STRIPLASTCHAR:
		BeginWaitCursor();
		EditStripLastCharacter(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_TRIMLINES:
		BeginWaitCursor();
		EditStripTrailingBlanks(hwndEdit, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_TRIMLEAD:
		BeginWaitCursor();
		EditStripLeadingBlanks(hwndEdit, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_COMPRESSWS:
		BeginWaitCursor();
		EditCompressSpaces(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_MERGEBLANKLINES:
		BeginWaitCursor();
		EditRemoveBlankLines(hwndEdit, TRUE);
		EndWaitCursor();
		break;

	case IDM_EDIT_REMOVEBLANKLINES:
		BeginWaitCursor();
		EditRemoveBlankLines(hwndEdit, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_MODIFYLINES:
		if (EditModifyLinesDlg(hwnd, wchPrefixLines, wchAppendLines)) {
			BeginWaitCursor();
			EditModifyLines(hwndEdit, wchPrefixLines, wchAppendLines);
			EndWaitCursor();
		}
		break;

	case IDM_EDIT_ALIGN:
		if (EditAlignDlg(hwnd, &iAlignMode)) {
			BeginWaitCursor();
			EditAlignText(hwndEdit, iAlignMode);
			EndWaitCursor();
		}
		break;

	case IDM_EDIT_SORTLINES:
		if (EditSortDlg(hwnd, &iSortOptions)) {
			BeginWaitCursor();
			StatusSetText(hwndStatus, 255, L"...");
			StatusSetSimple(hwndStatus, TRUE);
			InvalidateRect(hwndStatus, NULL, TRUE);
			UpdateWindow(hwndStatus);
			EditSortLines(hwndEdit, iSortOptions);
			StatusSetSimple(hwndStatus, FALSE);
			EndWaitCursor();
		}
		break;

	case IDM_EDIT_COLUMNWRAP: {
		if (iWrapCol == 0) {
			iWrapCol = iLongLinesLimit;
		}

		if (ColumnWrapDlg(hwnd, IDD_COLUMNWRAP, &iWrapCol)) {
			iWrapCol = clamp_i(iWrapCol, 1, 512);
			BeginWaitCursor();
			EditWrapToColumn(hwndEdit, iWrapCol);
			EndWaitCursor();
		}
	}
	break;

	case IDM_EDIT_SPLITLINES:
		BeginWaitCursor();
		SendMessage(hwndEdit, SCI_TARGETFROMSELECTION, 0, 0);
		SendMessage(hwndEdit, SCI_LINESSPLIT, 0, 0);
		EndWaitCursor();
		break;

	case IDM_EDIT_JOINLINES:
		BeginWaitCursor();
		SendMessage(hwndEdit, SCI_TARGETFROMSELECTION, 0, 0);
		SendMessage(hwndEdit, SCI_LINESJOIN, 0, 0);
		EditJoinLinesEx(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_JOINLINESEX:
		BeginWaitCursor();
		EditJoinLinesEx(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTUPPERCASE:
		BeginWaitCursor();
		SendMessage(hwndEdit, SCI_UPPERCASE, 0, 0);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTLOWERCASE:
		BeginWaitCursor();
		SendMessage(hwndEdit, SCI_LOWERCASE, 0, 0);
		EndWaitCursor();
		break;

	case IDM_EDIT_INVERTCASE:
		BeginWaitCursor();
		EditInvertCase(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_TITLECASE:
		BeginWaitCursor();
		EditTitleCase(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_SENTENCECASE:
		BeginWaitCursor();
		EditSentenceCase(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTTABS:
		BeginWaitCursor();
		EditTabsToSpaces(hwndEdit, iTabWidth, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTSPACES:
		BeginWaitCursor();
		EditSpacesToTabs(hwndEdit, iTabWidth, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTTABS2:
		BeginWaitCursor();
		EditTabsToSpaces(hwndEdit, iTabWidth, TRUE);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTSPACES2:
		BeginWaitCursor();
		EditSpacesToTabs(hwndEdit, iTabWidth, TRUE);
		EndWaitCursor();
		break;

	case IDM_EDIT_INSERT_XMLTAG: {
		WCHAR wszOpen[256] = L"";
		WCHAR wszClose[256] = L"";
		if (EditInsertTagDlg(hwnd, wszOpen, wszClose)) {
			EditEncloseSelection(hwndEdit, wszOpen, wszClose);
		}
	}
	break;

	case IDM_EDIT_INSERT_GUID: {
		GUID guid;
		if (S_OK == CoCreateGuid(&guid)) {
			char guidBuf[37] = {0};
			sprintf(guidBuf, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
					(unsigned int)(guid.Data1), guid.Data2, guid.Data3,
					guid.Data4[0], guid.Data4[1],
					guid.Data4[2], guid.Data4[3], guid.Data4[4],
					guid.Data4[5], guid.Data4[6], guid.Data4[7]
				   );
			SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)guidBuf);
		}
	}
	break;

	case IDM_EDIT_INSERT_ENCODING: {
		if (*mEncoding[iEncoding].pszParseNames) {
			char msz[64];
			char *p;
			BOOL done = FALSE;
			lstrcpynA(msz, mEncoding[iEncoding].pszParseNames, COUNTOF(msz));
			if ((p = StrChrA(msz, ',')) != NULL) {
				*p = 0;
			}
			if (pLexCurrent->iLexer == SCLEX_PYTHON) {
				const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
				const int iCurLine = SciCall_LineFromPosition(iCurrentPos);
				const Sci_Position iCurrentLinePos = iCurrentPos - SciCall_PositionFromLine(iCurLine);
				if (iCurLine < 2 && iCurrentLinePos == 0) {
					char cmsz[128];
					wsprintfA(cmsz, "#-*- coding: %s -*-", msz);
					SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)cmsz);
					done = TRUE;
				}
			}
			if (!done) {
				//int iSelStart;
				//iSelStart = SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);
				SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)msz);
				//SendMessage(hwndEdit, SCI_SETANCHOR, (WPARAM)iSelStart, 0);
			}
		}
	}
	break;

	case IDM_EDIT_INSERT_SHORTDATE:
	case IDM_EDIT_INSERT_LONGDATE: {
		WCHAR tchDateTime[256];
		WCHAR tchTemplate[256];
		SYSTEMTIME st;
		char	mszBuf[256 * kMaxMultiByteCount];
		//int		iSelStart;

		GetLocalTime(&st);

		if (IniGetString(L"Settings2",
						 (LOWORD(wParam) == IDM_EDIT_INSERT_SHORTDATE) ? L"DateTimeShort" : L"DateTimeLong",
						 L"", tchTemplate, COUNTOF(tchTemplate))) {
			struct tm sst;
			sst.tm_isdst	= -1;
			sst.tm_sec		= (int)st.wSecond;
			sst.tm_min		= (int)st.wMinute;
			sst.tm_hour		= (int)st.wHour;
			sst.tm_mday		= (int)st.wDay;
			sst.tm_mon		= (int)st.wMonth - 1;
			sst.tm_year		= (int)st.wYear - 1900;
			sst.tm_wday		= (int)st.wDayOfWeek;
			mktime(&sst);
			wcsftime(tchDateTime, COUNTOF(tchDateTime), tchTemplate, &sst);
		} else {
			WCHAR tchDate[128];
			WCHAR tchTime[128];
			GetDateFormat(LOCALE_USER_DEFAULT, (
							  LOWORD(wParam) == IDM_EDIT_INSERT_SHORTDATE) ? DATE_SHORTDATE : DATE_LONGDATE,
						  &st, NULL, tchDate, COUNTOF(tchDate));
			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, tchTime, COUNTOF(tchTime));

			wsprintf(tchDateTime, L"%s %s", tchTime, tchDate);
		}

		const UINT uCP = (SendMessage(hwndEdit, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) ? CP_UTF8 : CP_ACP;
		WideCharToMultiByte(uCP, 0, tchDateTime, -1, mszBuf, COUNTOF(mszBuf), NULL, NULL);
		//iSelStart = SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);
		SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)mszBuf);
		//SendMessage(hwndEdit, SCI_SETANCHOR, (WPARAM)iSelStart, 0);
	}
	break;

	case IDM_EDIT_INSERT_LOC_DATE:
	case IDM_EDIT_INSERT_LOC_DATETIME: {
		SYSTEMTIME lt;
		char	mszBuf[38];
		// Local
		GetLocalTime(&lt);
		if (LOWORD(wParam) == IDM_EDIT_INSERT_LOC_DATE) {
			sprintf(mszBuf, "%04u-%02u-%02u", lt.wYear, lt.wMonth, lt.wDay);
		} else {
			sprintf(mszBuf, "%04u-%02u-%02u %02u:%02u:%02u", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
		}
		SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)mszBuf);
	}
	break;

	case IDM_EDIT_INSERT_UTC_DATETIME: {
		SYSTEMTIME lt;
		char	mszBuf[38];
		// UTC
		GetSystemTime(&lt);
		sprintf(mszBuf, "%04u-%02u-%02uT%02u:%02u:%02uZ", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
		SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)mszBuf);
	}
	break;

	// http://www.frenk.com/2009/12/convert-filetime-to-unix-timestamp/
	case IDM_EDIT_INSERT_TIMESTAMP:		// second	1000 milli
	case IDM_EDIT_INSERT_TIMESTAMP_MS:	// milli	1000 micro
	case IDM_EDIT_INSERT_TIMESTAMP_US:	// micro 	1000 nano
	case IDM_EDIT_INSERT_TIMESTAMP_NS: {// nano
		char mszBuf[32];
		FILETIME ft;
		ULARGE_INTEGER date, adjust;
		// Windows timestamp in 100-nanosecond
		GetSystemTimeAsFileTime(&ft);
		//GetSystemTimePreciseAsFileTime(&ft);
		date.HighPart = ft.dwHighDateTime;
		date.LowPart = ft.dwLowDateTime;
		// Between Jan 1, 1601 and Jan 1, 1970 there are 11644473600 seconds
		adjust.QuadPart = 11644473600U * 1000 * 1000 * 10;
		date.QuadPart -= adjust.QuadPart;
		switch (LOWORD(wParam)) {
		case IDM_EDIT_INSERT_TIMESTAMP:		// second	1000 milli
			date.QuadPart /= 1000U * 1000 * 10;
			break;
		case IDM_EDIT_INSERT_TIMESTAMP_MS:	// milli	1000 micro
			date.QuadPart /= 1000U * 10;
			break;
		case IDM_EDIT_INSERT_TIMESTAMP_US:	// micro 	1000 nano
			date.QuadPart /= 10U;
			break;
		case IDM_EDIT_INSERT_TIMESTAMP_NS:	// nano
			date.QuadPart *= 100U;
			break;
		}
		sprintf(mszBuf, "%" PRIu64, date.QuadPart);
		SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)mszBuf);
	}
	break;

	case CMD_INSERTFILENAME_NOEXT:
	case IDM_EDIT_INSERT_FILENAME:
	case IDM_EDIT_INSERT_PATHNAME:
	case CMD_COPYFILENAME_NOEXT:
	case CMD_COPYFILENAME:
	case CMD_COPYPATHNAME: {
		SHFILEINFO shfi;
		WCHAR *pszInsert;
		WCHAR tchUntitled[MAX_PATH];

		if (StrNotEmpty(szCurFile)) {
			if (LOWORD(wParam) == IDM_EDIT_INSERT_FILENAME || LOWORD(wParam) == CMD_COPYFILENAME ||
					LOWORD(wParam) == CMD_INSERTFILENAME_NOEXT || LOWORD(wParam) == CMD_COPYFILENAME_NOEXT) {
				SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
				pszInsert = shfi.szDisplayName;
			} else {
				pszInsert = szCurFile;
			}
		} else {
			GetString(IDS_UNTITLED, tchUntitled, COUNTOF(tchUntitled));
			pszInsert = tchUntitled;
		}
		if (LOWORD(wParam) == CMD_INSERTFILENAME_NOEXT || LOWORD(wParam) == CMD_COPYFILENAME_NOEXT) {
			PathRemoveExtension(pszInsert);
		}

		if (LOWORD(wParam) == CMD_COPYFILENAME || LOWORD(wParam) == CMD_COPYFILENAME_NOEXT
				|| LOWORD(wParam) == CMD_COPYPATHNAME) {
			SetClipDataW(hwnd, pszInsert);
		} else {
			//int iSelStart;
			char mszBuf[MAX_PATH * kMaxMultiByteCount];
			const UINT uCP = (SendMessage(hwndEdit, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) ? CP_UTF8 : CP_ACP;
			WideCharToMultiByte(uCP, 0, pszInsert, -1, mszBuf, COUNTOF(mszBuf), NULL, NULL);
			//iSelStart = SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);
			SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)mszBuf);
			//SendMessage(hwndEdit, SCI_SETANCHOR, (WPARAM)iSelStart, 0);
		}
	}
	break;

	case IDM_EDIT_LINECOMMENT:
		EditToggleCommentLine(hwndEdit);
		break;

	case IDM_EDIT_STREAMCOMMENT:
		EditToggleCommentBlock(hwndEdit);
		break;

	case IDM_EDIT_URLENCODE:
		BeginWaitCursor();
		EditURLEncode(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_URLDECODE:
		BeginWaitCursor();
		EditURLDecode(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_ESCAPECCHARS:
		BeginWaitCursor();
		EditEscapeCChars(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_UNESCAPECCHARS:
		BeginWaitCursor();
		EditUnescapeCChars(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_XHTML_ESCAPE_CHAR:
		BeginWaitCursor();
		EditEscapeXHTMLChars(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_XHTML_UNESCAPE_CHAR:
		BeginWaitCursor();
		EditUnescapeXHTMLChars(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_CHAR2HEX:
		BeginWaitCursor();
		EditChar2Hex(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_HEX2CHAR:
		BeginWaitCursor();
		EditHex2Char(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_SHOW_HEX:
		BeginWaitCursor();
		EditShowHex(hwndEdit);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2HEX:
		BeginWaitCursor();
		EditConvertNumRadix(hwndEdit, 16);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2DEC:
		BeginWaitCursor();
		EditConvertNumRadix(hwndEdit, 10);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2BIN:
		BeginWaitCursor();
		EditConvertNumRadix(hwndEdit, 2);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2OCT:
		BeginWaitCursor();
		EditConvertNumRadix(hwndEdit, 8);
		EndWaitCursor();
		break;

	case IDM_EDIT_FINDMATCHINGBRACE: {
		int iBrace2 = -1;
		int iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
		char c = (char)SendMessage(hwndEdit, SCI_GETCHARAT, iPos, 0);
		if (StrChrA("()[]{}<>", c)) {
			iBrace2 = (int)SendMessage(hwndEdit, SCI_BRACEMATCH, iPos, 0);
		} else { // Try one before
			iPos = (int)SendMessage(hwndEdit, SCI_POSITIONBEFORE, iPos, 0);
			c = (char)SendMessage(hwndEdit, SCI_GETCHARAT, iPos, 0);
			if (StrChrA("()[]{}<>", c)) {
				iBrace2 = (int)SendMessage(hwndEdit, SCI_BRACEMATCH, iPos, 0);
			}
		}
		if (iBrace2 != -1) {
			SendMessage(hwndEdit, SCI_GOTOPOS, (WPARAM)iBrace2, 0);
		}
	}
	break;

	case IDM_EDIT_SELTOMATCHINGBRACE: {
		int iBrace2 = -1;
		int iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
		char c = (char)SendMessage(hwndEdit, SCI_GETCHARAT, iPos, 0);
		if (StrChrA("()[]{}<>", c)) {
			iBrace2 = (int)SendMessage(hwndEdit, SCI_BRACEMATCH, iPos, 0);
		} else { // Try one before
			iPos = (int)SendMessage(hwndEdit, SCI_POSITIONBEFORE, iPos, 0);
			c = (char)SendMessage(hwndEdit, SCI_GETCHARAT, iPos, 0);
			if (StrChrA("()[]{}<>", c)) {
				iBrace2 = (int)SendMessage(hwndEdit, SCI_BRACEMATCH, iPos, 0);
			}
		}
		if (iBrace2 != -1) {
			if (iBrace2 > iPos) {
				SendMessage(hwndEdit, SCI_SETSEL, (WPARAM)iPos, (LPARAM)iBrace2 + 1);
			} else {
				SendMessage(hwndEdit, SCI_SETSEL, (WPARAM)iPos + 1, (LPARAM)iBrace2);
			}
		}
	}
	break;

	case IDM_EDIT_FIND:
		if (!IsWindow(hDlgFindReplace)) {
			hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, FALSE);
		} else {
			if (GetDlgItem(hDlgFindReplace, IDC_REPLACE)) {
				SendMessage(hDlgFindReplace, WM_COMMAND, MAKELONG(IDMSG_SWITCHTOFIND, 1), 0);
				DestroyWindow(hDlgFindReplace);
				hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, FALSE);
			} else {
				SetForegroundWindow(hDlgFindReplace);
				PostMessage(hDlgFindReplace, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hDlgFindReplace, IDC_FINDTEXT)), 1);
			}
		}
		break;

#ifdef BOOKMARK_EDITION
	// Main Bookmark Functions
	case BME_EDIT_BOOKMARKNEXT: {
		int iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
		int iLine = (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION, iPos, 0);

		int bitmask = 1;
		int iNextLine = (int)SendMessage(hwndEdit, SCI_MARKERNEXT, iLine + 1, bitmask);
		if (iNextLine == -1) {
			iNextLine = (int)SendMessage(hwndEdit, SCI_MARKERNEXT, 0, bitmask);
		}

		if (iNextLine != -1) {
			SciCall_EnsureVisible(iNextLine);
			SendMessage(hwndEdit, SCI_GOTOLINE, iNextLine, 0);
			SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 10);
			SciCall_ScrollCaret();
			SciCall_SetYCaretPolicy(CARET_EVEN, 0);
		}
	}
	break;

	case BME_EDIT_BOOKMARKPREV: {
		int iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
		int iLine = (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION, iPos, 0);

		int bitmask = 1;
		int iNextLine = (int)SendMessage(hwndEdit, SCI_MARKERPREVIOUS, iLine - 1, bitmask);
		if (iNextLine == -1) {
			int nLines = (int)SendMessage(hwndEdit, SCI_GETLINECOUNT, 0, 0);
			iNextLine = (int)SendMessage(hwndEdit, SCI_MARKERPREVIOUS, nLines, bitmask);
		}

		if (iNextLine != -1) {
			SciCall_EnsureVisible(iNextLine);
			SendMessage(hwndEdit, SCI_GOTOLINE, iNextLine, 0);
			SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 10);
			SciCall_ScrollCaret();
			SciCall_SetYCaretPolicy(CARET_EVEN, 0);
		}
	}
	break;

	case BME_EDIT_BOOKMARKTOGGLE: {
		int iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
		int iLine = (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION, iPos, 0);

		int bitmask = (int)SendMessage(hwndEdit, SCI_MARKERGET, iLine, 0);
		if (bitmask & 1) {
			// unset
			SendMessage(hwndEdit, SCI_MARKERDELETE, iLine, 0);
		} else {
			if (bShowSelectionMargin) {
				SendMessage(hwndEdit, SCI_MARKERDEFINEPIXMAP, 0, (LPARAM)bookmark_pixmap);
			} else {
				SendMessage(hwndEdit, SCI_MARKERSETBACK, 0, NP2_BookmarkLineForeColor);
				SendMessage(hwndEdit, SCI_MARKERSETALPHA, 0, NP2_BookmarkLineColorAlpha);
				SendMessage(hwndEdit, SCI_MARKERDEFINE, 0, SC_MARK_BACKGROUND);
			}

			// set
			SendMessage(hwndEdit, SCI_MARKERADD, iLine, 0);
		}
	}
	break;

	case BME_EDIT_BOOKMARKCLEAR:
		SendMessage(hwndEdit, SCI_MARKERDELETEALL, (WPARAM) - 1, 0);
		break;
#endif

	case IDM_EDIT_FINDNEXT:
	case IDM_EDIT_FINDPREV:
	case IDM_EDIT_REPLACENEXT:
	case IDM_EDIT_SELTONEXT:
	case IDM_EDIT_SELTOPREV:
		if (SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) == 0) {
			break;
		}

		if (StrIsEmptyA(efrData.szFind)) {
			if (LOWORD(wParam) != IDM_EDIT_REPLACENEXT) {
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_FIND, 1), 0);
			} else {
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_REPLACE, 1), 0);
			}
		} else {
			UINT cp = (UINT)SendMessage(hwndEdit, SCI_GETCODEPAGE, 0, 0);
			if (cpLastFind != cp) {
				if (cp != SC_CP_UTF8) {
					WCHAR wch[NP2_FIND_REPLACE_LIMIT];

					MultiByteToWideChar(CP_UTF8, 0, efrData.szFindUTF8, -1, wch, COUNTOF(wch));
					WideCharToMultiByte(cp, 0, wch, -1, efrData.szFind, COUNTOF(efrData.szFind), NULL, NULL);

					MultiByteToWideChar(CP_UTF8, 0, efrData.szReplaceUTF8, -1, wch, COUNTOF(wch));
					WideCharToMultiByte(cp, 0, wch, -1, efrData.szReplace, COUNTOF(efrData.szReplace), NULL, NULL);
				} else {
					lstrcpyA(efrData.szFind, efrData.szFindUTF8);
					lstrcpyA(efrData.szReplace, efrData.szReplaceUTF8);
				}
			}

			cpLastFind = cp;
			switch (LOWORD(wParam)) {
			case IDM_EDIT_FINDNEXT:
				EditFindNext(hwndEdit, &efrData, FALSE);
				break;

			case IDM_EDIT_FINDPREV:
				EditFindPrev(hwndEdit, &efrData, FALSE);
				break;

			case IDM_EDIT_REPLACENEXT:
				if (bReplaceInitialized) {
					EditReplace(hwndEdit, &efrData);
				} else {
					SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_REPLACE, 1), 0);
				}
				break;

			case IDM_EDIT_SELTONEXT:
				EditFindNext(hwndEdit, &efrData, TRUE);
				break;

			case IDM_EDIT_SELTOPREV:
				EditFindPrev(hwndEdit, &efrData, TRUE);
				break;
			}
		}
		break;

	case IDM_EDIT_SELTOEND:
	case IDM_EDIT_SELTOBEGIN: {
		int selStart, selEnd;
		if (LOWORD(wParam) == IDM_EDIT_SELTOEND) {
			selStart = (int)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);
			selEnd = (int)SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0);
		} else {
			selStart = 0;
			selEnd = (int)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0);
		}
		SendMessage(hwndEdit, SCI_SETSELECTIONSTART, selStart, 0);
		SendMessage(hwndEdit, SCI_SETSELECTIONEND, selEnd, 0);
	}
	break;

	case IDM_EDIT_COMPLETEWORD:
		EditCompleteWord(hwndEdit, TRUE);
		break;

	case IDM_EDIT_REPLACE:
		if (!IsWindow(hDlgFindReplace)) {
			hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, TRUE);
		} else {
			if (!GetDlgItem(hDlgFindReplace, IDC_REPLACE)) {
				SendMessage(hDlgFindReplace, WM_COMMAND, MAKELONG(IDMSG_SWITCHTOREPLACE, 1), 0);
				DestroyWindow(hDlgFindReplace);
				hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, TRUE);
			} else {
				SetForegroundWindow(hDlgFindReplace);
				PostMessage(hDlgFindReplace, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hDlgFindReplace, IDC_FINDTEXT)), 1);
			}
		}
		break;

	case IDM_EDIT_GOTOLINE:
		EditLineNumDlg(hwndEdit);
		break;

	case IDM_VIEW_SCHEME:
		Style_SelectLexerDlg(hwndEdit);
		break;

	case IDM_VIEW_USE2NDDEFAULT:
		Style_ToggleUse2ndDefault(hwndEdit);
		break;

	case IDM_VIEW_SCHEMECONFIG:
		Style_ConfigDlg(hwndEdit);
		break;

	case IDM_VIEW_FONT:
		Style_SetDefaultFont(hwndEdit);
		break;

	case IDM_VIEW_WORDWRAP:
		fWordWrap = !fWordWrap;
		SendMessage(hwndEdit, SCI_SETWRAPMODE, (fWordWrap? iWordWrapMode : SC_WRAP_NONE), 0);
		fWordWrapG = fWordWrap;
		UpdateToolbar();
		break;

	case IDM_VIEW_WORDWRAPSETTINGS:
		if (WordWrapSettingsDlg(hwnd, IDD_WORDWRAP, &iWordWrapIndent)) {
			SendMessage(hwndEdit, SCI_SETWRAPMODE, (fWordWrap? iWordWrapMode : SC_WRAP_NONE), 0);
			fWordWrapG = fWordWrap;
			UpdateToolbar();
			SetWrapIndentMode();
			SetWrapVisualFlags();
		}
		break;

	case IDM_VIEW_LONGLINEMARKER:
		bMarkLongLines = !bMarkLongLines;
		if (bMarkLongLines) {
			SendMessage(hwndEdit, SCI_SETEDGEMODE, (iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND, 0);
			Style_SetLongLineColors(hwndEdit);
		} else {
			SendMessage(hwndEdit, SCI_SETEDGEMODE, EDGE_NONE, 0);
		}
		break;

	case IDM_VIEW_LONGLINESETTINGS:
		if (LongLineSettingsDlg(hwnd, IDD_LONGLINES, &iLongLinesLimit)) {
			bMarkLongLines = TRUE;
			SendMessage(hwndEdit, SCI_SETEDGEMODE, (iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND, 0);
			Style_SetLongLineColors(hwndEdit);
			iLongLinesLimit = clamp_i(iLongLinesLimit, 0, NP2_LONG_LINE_LIMIT);
			SendMessage(hwndEdit, SCI_SETEDGECOLUMN, iLongLinesLimit, 0);
			iLongLinesLimitG = iLongLinesLimit;
		}
		break;

	case IDM_VIEW_TABSASSPACES:
		bTabsAsSpaces = !bTabsAsSpaces;
		SendMessage(hwndEdit, SCI_SETUSETABS, !bTabsAsSpaces, 0);
		bTabsAsSpacesG = bTabsAsSpaces;
		break;

	case IDM_VIEW_TABSETTINGS:
		if (TabSettingsDlg(hwnd, IDD_TABSETTINGS, NULL)) {
			SendMessage(hwndEdit, SCI_SETUSETABS, !bTabsAsSpaces, 0);
			SendMessage(hwndEdit, SCI_SETTABINDENTS, bTabIndents, 0);
			SendMessage(hwndEdit, SCI_SETBACKSPACEUNINDENTS, bBackspaceUnindents, 0);
			iTabWidth = clamp_i(iTabWidth, 1, 256);
			iIndentWidth = clamp_i(iIndentWidth, 0, 256);
			SendMessage(hwndEdit, SCI_SETTABWIDTH, iTabWidth, 0);
			SendMessage(hwndEdit, SCI_SETINDENT, iIndentWidth, 0);
			bTabsAsSpacesG = bTabsAsSpaces;
			bTabIndentsG	 = bTabIndents;
			iTabWidthG		 = iTabWidth;
			iIndentWidthG	 = iIndentWidth;
			if (SendMessage(hwndEdit, SCI_GETWRAPINDENTMODE, 0, 0) == SC_WRAPINDENT_FIXED) {
				SetWrapStartIndent();
			}
		}
		break;

	case IDM_VIEW_SHOWINDENTGUIDES:
		bShowIndentGuides = !bShowIndentGuides;
		Style_SetIndentGuides(hwndEdit, bShowIndentGuides);
		break;

	case IDM_VIEW_AUTOINDENTTEXT:
		bAutoIndent = !bAutoIndent;
		break;

	case IDM_VIEW_LINENUMBERS:
		bShowLineNumbers = !bShowLineNumbers;
		UpdateLineNumberWidth();
		break;

	case IDM_VIEW_MARGIN:
		bShowSelectionMargin = !bShowSelectionMargin;
		UpdateSelectionMarginWidth();

#ifdef BOOKMARK_EDITION
		//Depending on if the margin is visible or not, choose different bookmark indication
		if (bShowSelectionMargin) {
			SendMessage(hwndEdit, SCI_MARKERDEFINEPIXMAP, 0, (LPARAM)bookmark_pixmap);
		} else {
			SendMessage(hwndEdit, SCI_MARKERSETBACK, 0, NP2_BookmarkLineForeColor);
			SendMessage(hwndEdit, SCI_MARKERSETALPHA, 0, NP2_BookmarkLineColorAlpha);
			SendMessage(hwndEdit, SCI_MARKERDEFINE, 0, SC_MARK_BACKGROUND);
		}
#endif

		break;

	case IDM_VIEW_AUTOCOMPLETEQUOTE:
		bAutoCloseBracesQuotes = !bAutoCloseBracesQuotes;
		break;

	case IDM_VIEW_AUTOCOMPLETEWORDS:
		bAutoCompleteWords = !bAutoCompleteWords;
		if (!bAutoCompleteWords) {
			// close the autocompletion list
			SendMessage(hwndEdit, SCI_AUTOCCANCEL, 0, 0);
		}
		break;

	case IDM_VIEW_AUTOCWITHDOCWORDS:
		bAutoCIncludeDocWord = !bAutoCIncludeDocWord;
		break;

	case IDM_VIEW_AUTOC_ENGLISH_ONLY:
		bAutoCEnglishIMEModeOnly = !bAutoCEnglishIMEModeOnly;
		break;

	case IDM_VIEW_MARKOCCURRENCES_OFF:
		iMarkOccurrences = 0;
		// clear all marks
		SendMessage(hwndEdit, SCI_SETINDICATORCURRENT, 1, 0);
		SendMessage(hwndEdit, SCI_INDICATORCLEARRANGE, 0, (int)SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0));
		break;

	case IDM_VIEW_MARKOCCURRENCES_RED:
		iMarkOccurrences = 1;
		EditMarkAll(hwndEdit, iMarkOccurrences, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords);
		UpdateStatusbar();
		break;

	case IDM_VIEW_MARKOCCURRENCES_GREEN:
		iMarkOccurrences = 2;
		EditMarkAll(hwndEdit, iMarkOccurrences, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords);
		UpdateStatusbar();
		break;

	case IDM_VIEW_MARKOCCURRENCES_BLUE:
		iMarkOccurrences = 3;
		EditMarkAll(hwndEdit, iMarkOccurrences, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords);
		UpdateStatusbar();
		break;

	case IDM_VIEW_MARKOCCURRENCES_CASE:
		bMarkOccurrencesMatchCase = !bMarkOccurrencesMatchCase;
		EditMarkAll(hwndEdit, iMarkOccurrences, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords);
		break;

	case IDM_VIEW_MARKOCCURRENCES_WORD:
		bMarkOccurrencesMatchWords = !bMarkOccurrencesMatchWords;
		EditMarkAll(hwndEdit, iMarkOccurrences, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords);
		UpdateStatusbar();
		break;

	case IDM_VIEW_FOLDING:
		bShowCodeFolding = !bShowCodeFolding;
		UpdateFoldMarginWidth();
		UpdateToolbar();
		if (!bShowCodeFolding) {
			FoldToggleAll(FOLD_ACTION_EXPAND);
		}
		break;

	case IDM_VIEW_TOGGLEFOLDS:
		if (bShowCodeFolding) {
			FoldToggleDefault(FOLD_ACTION_SNIFF);
		}
		break;

	case IDM_VIEW_FOLD_ALL:
		if (bShowCodeFolding) {
			FoldToggleAll(FOLD_ACTION_SNIFF);
		}
		break;

	case IDM_VIEW_FOLD_CURRENT:
		if (bShowCodeFolding) {
			FoldToggleCurrent(FOLD_ACTION_SNIFF);
		}
		break;

	case IDM_VIEW_FOLD_LEVEL1:
	case IDM_VIEW_FOLD_LEVEL2:
	case IDM_VIEW_FOLD_LEVEL3:
	case IDM_VIEW_FOLD_LEVEL4:
	case IDM_VIEW_FOLD_LEVEL5:
	case IDM_VIEW_FOLD_LEVEL6:
	case IDM_VIEW_FOLD_LEVEL7:
	case IDM_VIEW_FOLD_LEVEL8:
	case IDM_VIEW_FOLD_LEVEL9:
	case IDM_VIEW_FOLD_LEVEL10:
		if (bShowCodeFolding) {
			FoldToggleLevel(LOWORD(wParam) - IDM_VIEW_FOLD_LEVEL1, FOLD_ACTION_SNIFF);
		}
		break;

	case IDM_VIEW_SHOWWHITESPACE:
		bViewWhiteSpace = !bViewWhiteSpace;
		SendMessage(hwndEdit, SCI_SETVIEWWS, (bViewWhiteSpace) ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE, 0);
		break;

	case IDM_VIEW_SHOWEOLS:
		bViewEOLs = !bViewEOLs;
		SendMessage(hwndEdit, SCI_SETVIEWEOL, bViewEOLs, 0);
		break;

	case IDM_VIEW_WORDWRAPSYMBOLS:
		bShowWordWrapSymbols = !bShowWordWrapSymbols;
		SetWrapVisualFlags();
		break;

#if NP2_ENABLE_SHOW_CALL_TIPS
	case IDM_VIEW_SHOWCALLTIPS:
		bShowCallTips = !bShowCallTips;
		SetCallTipsWaitTime();
		break;
#endif

	case IDM_VIEW_MATCHBRACES:
		bMatchBraces = !bMatchBraces;
		if (bMatchBraces) {
			struct SCNotification scn;
			scn.nmhdr.hwndFrom = hwndEdit;
			scn.nmhdr.idFrom = IDC_EDIT;
			scn.nmhdr.code = SCN_UPDATEUI;
			scn.updated = SC_UPDATE_CONTENT;
			SendMessage(hwnd, WM_NOTIFY, IDC_EDIT, (LPARAM)&scn);
		} else {
			SendMessage(hwndEdit, SCI_BRACEHIGHLIGHT, (WPARAM) - 1, (LPARAM) - 1);
		}
		break;

	case IDM_VIEW_AUTOCLOSETAGS:
		bAutoCloseTags = !bAutoCloseTags;
		break;

	case IDM_VIEW_HILITECURRENTLINE:
		bHiliteCurrentLine = !bHiliteCurrentLine;
		Style_SetCurrentLineBackground(hwndEdit);
		break;

	case IDM_VIEW_ZOOMIN:
		SendMessage(hwndEdit, SCI_ZOOMIN, 0, 0);
		break;

	case IDM_VIEW_ZOOMOUT:
		SendMessage(hwndEdit, SCI_ZOOMOUT, 0, 0);
		break;

	case IDM_VIEW_RESETZOOM:
		SendMessage(hwndEdit, SCI_SETZOOM, 100, 0);
		break;

	case IDM_VIEW_TOOLBAR:
		if (bShowToolbar) {
			bShowToolbar = 0;
			ShowWindow(hwndReBar, SW_HIDE);
		} else {
			bShowToolbar = 1;
			UpdateToolbar();
			ShowWindow(hwndReBar, SW_SHOW);
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_CUSTOMIZETB:
		SendMessage(hwndToolbar, TB_CUSTOMIZE, 0, 0);
		break;

	case IDM_VIEW_STATUSBAR:
		if (bShowStatusbar) {
			bShowStatusbar = 0;
			ShowWindow(hwndStatus, SW_HIDE);
		} else {
			bShowStatusbar = 1;
			UpdateStatusbar();
			ShowWindow(hwndStatus, SW_SHOW);
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_STICKYWINPOS:
		bStickyWinPos = !bStickyWinPos;
		IniSetBool(L"Settings2", L"StickyWindowPosition", bStickyWinPos);
		if (bStickyWinPos) {
			WINDOWPLACEMENT wndpl;
			WCHAR tchPosX[32], tchPosY[32], tchSizeX[32], tchSizeY[32], tchMaximized[32];

			int ResX = GetSystemMetrics(SM_CXSCREEN);
			int ResY = GetSystemMetrics(SM_CYSCREEN);

			// GetWindowPlacement
			wndpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwndMain, &wndpl);

			wi.x = wndpl.rcNormalPosition.left;
			wi.y = wndpl.rcNormalPosition.top;
			wi.cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
			wi.cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
			wi.max = (IsZoomed(hwndMain) || (wndpl.flags & WPF_RESTORETOMAXIMIZED));

			wsprintf(tchPosX, L"%ix%i PosX", ResX, ResY);
			wsprintf(tchPosY, L"%ix%i PosY", ResX, ResY);
			wsprintf(tchSizeX, L"%ix%i SizeX", ResX, ResY);
			wsprintf(tchSizeY, L"%ix%i SizeY", ResX, ResY);
			wsprintf(tchMaximized, L"%ix%i Maximized", ResX, ResY);

			IniSetInt(L"Window", tchPosX, wi.x);
			IniSetInt(L"Window", tchPosY, wi.y);
			IniSetInt(L"Window", tchSizeX, wi.cx);
			IniSetInt(L"Window", tchSizeY, wi.cy);
			IniSetInt(L"Window", tchMaximized, wi.max);

			InfoBox(0, L"MsgStickyWinPos", IDS_STICKYWINPOS);
		}
		break;

	case IDM_VIEW_REUSEWINDOW:
		bReuseWindow = !bReuseWindow;
		IniSetBool(L"Settings2", L"ReuseWindow", bReuseWindow);
		break;

	case IDM_VIEW_SINGLEFILEINSTANCE:
		bSingleFileInstance = !bSingleFileInstance;
		IniSetBool(L"Settings2", L"SingleFileInstance", bSingleFileInstance);
		break;

	case IDM_VIEW_ALWAYSONTOP:
		if (bInFullScreenMode) {
			return 0;
		}
		if ((bAlwaysOnTop || flagAlwaysOnTop == 2) && flagAlwaysOnTop != 1) {
			bAlwaysOnTop = 0;
			flagAlwaysOnTop = 0;
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		} else {
			bAlwaysOnTop = 1;
			flagAlwaysOnTop = 0;
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		break;

	case IDM_VIEW_MINTOTRAY:
		bMinimizeToTray = !bMinimizeToTray;
		break;

	case IDM_VIEW_TRANSPARENT:
		bTransparentMode = !bTransparentMode;
		SetWindowTransparentMode(hwnd, bTransparentMode);
		break;

	case IDM_SET_RENDER_TECH_GDI:
	case IDM_SET_RENDER_TECH_D2D:
	case IDM_SET_RENDER_TECH_D2DRETAIN:
	case IDM_SET_RENDER_TECH_D2DDC:
		iRenderingTechnology = LOWORD(wParam) - IDM_SET_RENDER_TECH_GDI;
		SendMessage(hwndEdit, SCI_SETBUFFEREDDRAW, (iRenderingTechnology == SC_TECHNOLOGY_DEFAULT), 0);
		SendMessage(hwndEdit, SCI_SETTECHNOLOGY, iRenderingTechnology, 0);
		iRenderingTechnology = (int)SendMessage(hwndEdit, SCI_GETTECHNOLOGY, 0, 0);
		iBidirectional = (int)SendMessage(hwndEdit, SCI_GETBIDIRECTIONAL, 0, 0);
		break;

	case IDM_SET_BIDIRECTIONAL_NONE:
	case IDM_SET_BIDIRECTIONAL_L2R:
	case IDM_SET_BIDIRECTIONAL_R2L:
		SendMessage(hwndEdit, SCI_SETBIDIRECTIONAL, LOWORD(wParam) - IDM_SET_BIDIRECTIONAL_NONE, 0);
		iBidirectional = (int)SendMessage(hwndEdit, SCI_GETBIDIRECTIONAL, 0, 0);
		break;

	case IDM_SET_USE_INLINE_IME:
		bUseInlineIME = bUseInlineIME? SC_IME_WINDOWED : SC_IME_INLINE;
		SendMessage(hwndEdit, SCI_SETIMEINTERACTION, bUseInlineIME, 0);
		break;

	case IDM_SET_USE_BLOCK_CARET:
		bInlineIMEUseBlockCaret = !bInlineIMEUseBlockCaret;
		SendMessage(hwndEdit, SCI_SETINLINEIMEUSEBLOCKCARET, bInlineIMEUseBlockCaret, 0);
		break;

	case IDM_VIEW_FONTQUALITY_DEFAULT:
	case IDM_VIEW_FONTQUALITY_NONE:
	case IDM_VIEW_FONTQUALITY_STANDARD:
	case IDM_VIEW_FONTQUALITY_CLEARTYPE:
		iFontQuality = LOWORD(wParam) - IDM_VIEW_FONTQUALITY_DEFAULT;
		SendMessage(hwndEdit, SCI_SETFONTQUALITY, iFontQuality, 0);
		break;

	case IDM_VIEW_CARET_STYLE_BLOCK:
	case IDM_VIEW_CARET_STYLE_WIDTH1:
	case IDM_VIEW_CARET_STYLE_WIDTH2:
	case IDM_VIEW_CARET_STYLE_WIDTH3:
		iCaretStyle = LOWORD(wParam) -  IDM_VIEW_CARET_STYLE_BLOCK;
		Style_UpdateCaret(hwndEdit);
		break;

	case IDM_VIEW_CARET_STYLE_NOBLINK:
		iCaretBlinkPeriod = (iCaretBlinkPeriod == 0)? -1 : 0;
		Style_UpdateCaret(hwndEdit);
		break;

	case IDM_VIEW_SHOWFILENAMEONLY:
	case IDM_VIEW_SHOWFILENAMEFIRST:
	case IDM_VIEW_SHOWFULLPATH:
		iPathNameFormat = LOWORD(wParam) - IDM_VIEW_SHOWFILENAMEONLY;
		lstrcpy(szTitleExcerpt, L"");
		UpdateWindowTitle();
		break;

	case IDM_VIEW_SHOWEXCERPT:
		EditGetExcerpt(hwndEdit, szTitleExcerpt, COUNTOF(szTitleExcerpt));
		UpdateWindowTitle();
		break;

	case IDM_VIEW_NOSAVEFINDREPL:
		bSaveFindReplace = !bSaveFindReplace;
		break;

	case IDM_VIEW_SAVEBEFORERUNNINGTOOLS:
		bSaveBeforeRunningTools = !bSaveBeforeRunningTools;
		break;

	case IDM_VIEW_CHANGENOTIFY:
		if (ChangeNotifyDlg(hwnd)) {
			InstallFileWatching(szCurFile);
		}
		break;

	case IDM_VIEW_NOESCFUNC:
		iEscFunction = 0;
		break;

	case IDM_VIEW_ESCMINIMIZE:
		iEscFunction = 1;
		break;

	case IDM_VIEW_ESCEXIT:
		iEscFunction = 2;
		break;

	case IDM_VIEW_SAVESETTINGS:
		bSaveSettings = !bSaveSettings;
		break;

	case IDM_VIEW_SAVESETTINGSNOW: {
		BOOL bCreateFailure = FALSE;

		if (StrIsEmpty(szIniFile)) {
			if (StrNotEmpty(szIniFile2)) {
				if (CreateIniFileEx(szIniFile2)) {
					lstrcpy(szIniFile, szIniFile2);
					lstrcpy(szIniFile2, L"");
				} else {
					bCreateFailure = TRUE;
				}
			} else {
				break;
			}
		}

		if (!bCreateFailure) {
			if (WritePrivateProfileString(L"Settings", L"WriteTest", L"ok", szIniFile)) {
				BeginWaitCursor();
				StatusSetTextID(hwndStatus, STATUS_HELP, IDS_SAVINGSETTINGS);
				StatusSetSimple(hwndStatus, TRUE);
				InvalidateRect(hwndStatus, NULL, TRUE);
				UpdateWindow(hwndStatus);
				SaveSettings(TRUE);
				StatusSetSimple(hwndStatus, FALSE);
				EndWaitCursor();
				MsgBox(MBINFO, IDS_SAVEDSETTINGS);
			} else {
				dwLastIOError = GetLastError();
				MsgBox(MBWARN, IDS_WRITEINI_FAIL);
			}
		} else {
			MsgBox(MBWARN, IDS_CREATEINI_FAIL);
		}
	}
	break;

	case IDM_HELP_ABOUT:
		ThemedDialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
		break;

	case IDM_CMDLINE_HELP:
		DisplayCmdLineHelp(hwnd);
		break;

	case IDM_HELP_PROJECT_HOME:
	case IDM_HELP_LATEST_RELEASE:
	case IDM_HELP_REPORT_ISSUE:
	case IDM_HELP_FEATURE_REQUEST:
	case IDM_HELP_ONLINE_WIKI:
		OpenHelpLink(hwnd, LOWORD(wParam));
		break;

	case IDM_VIEW_TOGGLE_FULLSCREEN:
		bInFullScreenMode = !bInFullScreenMode;
		ToggleFullScreenMode();
		break;

	case IDM_VIEW_FULLSCREEN_ON_START:
		bFullScreenOnStartup = !bFullScreenOnStartup;
		break;

	case IDM_VIEW_FULLSCREEN_HIDE_MENU:
		bFullScreenHideMenu = !bFullScreenHideMenu;
		if (bInFullScreenMode) {
			ToggleFullScreenMode();
		}
		break;

	case IDM_VIEW_FULLSCREEN_HIDE_TOOL:
		bFullScreenHideToolbar = !bFullScreenHideToolbar;
		if (bInFullScreenMode) {
			ToggleFullScreenMode();
		}
		break;

	case IDM_VIEW_FULLSCREEN_HIDE_STATUS:
		bFullScreenHideStatusbar = !bFullScreenHideStatusbar;
		if (bInFullScreenMode) {
			ToggleFullScreenMode();
		}
		break;

	case CMD_ESCAPE:
		if (SendMessage(hwndEdit, SCI_AUTOCACTIVE, 0, 0)) {
			//close the AutoComplete box
			SendMessage(hwndEdit, SCI_AUTOCCANCEL, 0, 0);
		} else if (bInFullScreenMode) {
			bInFullScreenMode = FALSE;
			ToggleFullScreenMode();
		} else if (iEscFunction == 1) {
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		} else if (iEscFunction == 2) {
			NP2ExitWind(hwnd);
		}
		break;

	case CMD_SHIFTESC:
		NP2ExitWind(hwnd);
		break;

	// Newline with toggled auto indent setting
	case CMD_CTRLENTER:
		bAutoIndent = !bAutoIndent;
		SendMessage(hwndEdit, SCI_NEWLINE, 0, 0);
		bAutoIndent = !bAutoIndent;
		break;

	case CMD_CTRLBACK: {
		int iPos		= (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
		int iAnchor		= (int)SendMessage(hwndEdit, SCI_GETANCHOR, 0, 0);
		int iLine		= (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION, (WPARAM)iPos, 0);
		int iStartPos	= (int)SendMessage(hwndEdit, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
		int iIndentPos	= (int)SendMessage(hwndEdit, SCI_GETLINEINDENTPOSITION, (WPARAM)iLine, 0);

		if (iPos != iAnchor) {
			SendMessage(hwndEdit, SCI_SETSEL, (WPARAM)iPos, (LPARAM)iPos);
		} else {
			if (iPos == iStartPos) {
				SendMessage(hwndEdit, SCI_DELETEBACK, 0, 0);
			} else if (iPos <= iIndentPos) {
				SendMessage(hwndEdit, SCI_DELLINELEFT, 0, 0);
			} else {
				SendMessage(hwndEdit, SCI_DELWORDLEFT, 0, 0);
			}
		}
	}
	break;

	case CMD_CTRLDEL: {
		int iPos		= (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
		int iAnchor		= (int)SendMessage(hwndEdit, SCI_GETANCHOR, 0, 0);
		int iLine		= (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION, (WPARAM)iPos, 0);
		int iStartPos	= (int)SendMessage(hwndEdit, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
		int iEndPos		= (int)SendMessage(hwndEdit, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);

		if (iPos != iAnchor) {
			SendMessage(hwndEdit, SCI_SETSEL, (WPARAM)iPos, (LPARAM)iPos);
		} else {
			if (iStartPos != iEndPos) {
				SendMessage(hwndEdit, SCI_DELWORDRIGHT, 0, 0);
			} else { // iStartPos == iEndPos
				SendMessage(hwndEdit, SCI_LINEDELETE, 0, 0);
			}
		}
	}
	break;

	case CMD_CTRLTAB:
		SendMessage(hwndEdit, SCI_SETTABINDENTS, FALSE, 0);
		SendMessage(hwndEdit, SCI_SETUSETABS, TRUE, 0);
		SendMessage(hwndEdit, SCI_TAB, 0, 0);
		SendMessage(hwndEdit, SCI_SETUSETABS, !bTabsAsSpaces, 0);
		SendMessage(hwndEdit, SCI_SETTABINDENTS, bTabIndents, 0);
		break;

	case CMD_RECODEDEFAULT: {
		if (StrNotEmpty(szCurFile)) {
			if (iDefaultEncoding == CPI_UNICODEBOM) {
				iSrcEncoding = CPI_UNICODE;
			} else if (iDefaultEncoding == CPI_UNICODEBEBOM) {
				iSrcEncoding = CPI_UNICODEBE;
			} else if (iDefaultEncoding == CPI_UTF8SIGN) {
				iSrcEncoding = CPI_UTF8;
			} else {
				iSrcEncoding = iDefaultEncoding;
			}
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
		}
	}
	break;

	case CMD_RELOADANSI: {
		if (StrNotEmpty(szCurFile)) {
			iSrcEncoding = CPI_DEFAULT;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
		}
	}
	break;

	case CMD_RELOADOEM: {
		if (StrNotEmpty(szCurFile)) {
			iSrcEncoding = CPI_OEM;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
		}
	}
	break;

	case CMD_RELOADASCIIASUTF8: {
		if (StrNotEmpty(szCurFile)) {
			BOOL _bLoadASCIIasUTF8 = bLoadASCIIasUTF8;
			bLoadASCIIasUTF8 = 1;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
			bLoadASCIIasUTF8 = _bLoadASCIIasUTF8;
		}
	}
	break;

	case CMD_RELOADNOFILEVARS: {
		if (StrNotEmpty(szCurFile)) {
			int _fNoFileVariables = fNoFileVariables;
			BOOL _bNoEncodingTags = bNoEncodingTags;
			fNoFileVariables = 1;
			bNoEncodingTags = 1;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
			fNoFileVariables = _fNoFileVariables;
			bNoEncodingTags = _bNoEncodingTags;
		}
	}
	break;

	case IDM_LANG_DEFAULT:
	case IDM_LANG_APACHE:
	case IDM_LANG_WEB:
	case IDM_LANG_PHP:
	case IDM_LANG_JSP:
	case IDM_LANG_ASPX_CS:
	case IDM_LANG_ASPX_VB:
	case IDM_LANG_ASP_VBS:
	case IDM_LANG_ASP_JS:
	case IDM_LANG_XML:
	case IDM_LANG_XSD:
	case IDM_LANG_XSLT:
	case IDM_LANG_DTD:

	case IDM_LANG_ANT_BUILD:
	case IDM_LANG_MAVEN_POM:
	case IDM_LANG_MAVEN_SETTINGS:
	case IDM_LANG_IVY_MODULE:
	case IDM_LANG_IVY_SETTINGS:
	case IDM_LANG_PMD_RULESET:
	case IDM_LANG_CHECKSTYLE:

	case IDM_LANG_TOMCAT:
	case IDM_LANG_WEB_JAVA:
	case IDM_LANG_STRUTS:
	case IDM_LANG_HIB_CFG:
	case IDM_LANG_HIB_MAP:
	case IDM_LANG_SPRING_BEANS:
	case IDM_LANG_JBOSS:

	case IDM_LANG_WEB_NET:
	case IDM_LANG_RESX:
	case IDM_LANG_XAML:

	case IDM_LANG_PROPERTY_LIST:
	case IDM_LANG_ANDROID_MANIFEST:
	case IDM_LANG_ANDROID_LAYOUT:
	case IDM_LANG_SVG:

	case IDM_LANG_BASH:
	case IDM_LANG_CSHELL:
	case IDM_LANG_M4:

	case IDM_LANG_MATLAB:
	case IDM_LANG_OCTAVE:
	case IDM_LANG_SCILAB:

	case IDM_LANG_CSS:
	case IDM_LANG_SCSS:
	case IDM_LANG_LESS:
	case IDM_LANG_HSS:
		Style_SetLexerByLangIndex(hwndEdit, LOWORD(wParam));
		break;

	case CMD_TIMESTAMPS: {
		WCHAR wchFind[256] = {0};
		WCHAR wchTemplate[256] = {0};
		WCHAR *pwchSep;
		WCHAR wchReplace[256];

		SYSTEMTIME st;
		struct tm sst;

		UINT cp;
		EDITFINDREPLACE efrTS = {
			.fuFlags = SCFIND_REGEXP,
			.hwnd = hwndEdit,
		};

		IniGetString(L"Settings2", L"TimeStamp", L"\\$Date:[^\\$]+\\$ | $Date: %Y/%m/%d %H:%M:%S $", wchFind, COUNTOF(wchFind));

		if ((pwchSep = StrChr(wchFind, L'|')) != NULL) {
			lstrcpy(wchTemplate, pwchSep + 1);
			*pwchSep = 0;
		}

		StrTrim(wchFind, L" ");
		StrTrim(wchTemplate, L" ");

		if (StrIsEmpty(wchFind) || StrIsEmpty(wchTemplate)) {
			break;
		}

		GetLocalTime(&st);
		sst.tm_isdst = -1;
		sst.tm_sec	 = (int)st.wSecond;
		sst.tm_min	 = (int)st.wMinute;
		sst.tm_hour	 = (int)st.wHour;
		sst.tm_mday	 = (int)st.wDay;
		sst.tm_mon	 = (int)st.wMonth - 1;
		sst.tm_year	 = (int)st.wYear - 1900;
		sst.tm_wday	 = (int)st.wDayOfWeek;
		mktime(&sst);
		wcsftime(wchReplace, COUNTOF(wchReplace), wchTemplate, &sst);

		cp = (UINT)SendMessage(hwndEdit, SCI_GETCODEPAGE, 0, 0);
		WideCharToMultiByte(cp, 0, wchFind, -1, efrTS.szFind, COUNTOF(efrTS.szFind), NULL, NULL);
		WideCharToMultiByte(cp, 0, wchReplace, -1, efrTS.szReplace, COUNTOF(efrTS.szReplace), NULL, NULL);

		if (!EditIsEmptySelection()) {
			EditReplaceAllInSelection(hwndEdit, &efrTS, TRUE);
		} else {
			EditReplaceAll(hwndEdit, &efrTS, TRUE);
		}
	}
	break;

	case CMD_WEBACTION1:
	case CMD_WEBACTION2: {
		LPWSTR lpszTemplateName;
		WCHAR szCmdTemplate[256];

		lpszTemplateName = (LOWORD(wParam) == CMD_WEBACTION1) ? L"WebTemplate1" : L"WebTemplate2";
		const BOOL bCmdEnabled = IniGetString(L"Settings2", lpszTemplateName, L"", szCmdTemplate, COUNTOF(szCmdTemplate));

		if (bCmdEnabled) {
			DWORD cchSelection = (int)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0)
								 - (int)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);

			if (cchSelection > 0 && cchSelection <= 500 && SendMessage(hwndEdit, SCI_GETSELTEXT, 0, 0) < 512) {
				char mszSelection[512] = {0};
				char *lpsz;
				SendMessage(hwndEdit, SCI_GETSELTEXT, 0, (LPARAM)mszSelection);
				mszSelection[cchSelection] = 0; // zero terminate

				// Check lpszSelection and truncate bad WCHARs
				lpsz = StrPBrkA(mszSelection, "\r\n\t");
				if (lpsz) {
					*lpsz = '\0';
				}

				if (StrNotEmptyA(mszSelection)) {
					SHELLEXECUTEINFO sei;
					WCHAR wchDirectory[MAX_PATH] = L"";
					WCHAR wszSelection[512];

					const UINT uCP = (SendMessage(hwndEdit, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) ? CP_UTF8 : CP_ACP;
					MultiByteToWideChar(uCP, 0, mszSelection, -1, wszSelection, COUNTOF(wszSelection));

					LPWSTR lpszCommand = NP2HeapAlloc(sizeof(WCHAR) * (512 + COUNTOF(szCmdTemplate) + MAX_PATH + 32));
					const SIZE_T cbCommand = NP2HeapSize(lpszCommand);
					wsprintf(lpszCommand, szCmdTemplate, wszSelection);
					ExpandEnvironmentStringsEx(lpszCommand, (DWORD)(cbCommand / sizeof(WCHAR)));

					LPWSTR lpszArgs = NP2HeapAlloc(cbCommand);
					ExtractFirstArgument(lpszCommand, lpszCommand, lpszArgs);

					if (StrNotEmpty(szCurFile)) {
						lstrcpy(wchDirectory, szCurFile);
						PathRemoveFileSpec(wchDirectory);
					}

					ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

					sei.cbSize = sizeof(SHELLEXECUTEINFO);
					sei.fMask = /*SEE_MASK_NOZONECHECKS*/0x00800000;
					sei.hwnd = NULL;
					sei.lpVerb = NULL;
					sei.lpFile = lpszCommand;
					sei.lpParameters = lpszArgs;
					sei.lpDirectory = wchDirectory;
					sei.nShow = SW_SHOWNORMAL;

					ShellExecuteEx(&sei);

					NP2HeapFree(lpszCommand);
					NP2HeapFree(lpszArgs);
				}
			}
		}
	}
	break;

	case CMD_FINDNEXTSEL:
	case CMD_FINDPREVSEL:
	case IDM_EDIT_SAVEFIND: {
		int cchSelection = (int)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0) -
						   (int)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);

		if (cchSelection == 0) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_SELECTWORD, 1), 0);
			cchSelection = (int)SendMessage(hwndEdit, SCI_GETSELECTIONEND, 0, 0) -
						   (int)SendMessage(hwndEdit, SCI_GETSELECTIONSTART, 0, 0);
		}

		if (cchSelection > 0 && cchSelection < NP2_FIND_REPLACE_LIMIT && SendMessage(hwndEdit, SCI_GETSELTEXT, 0, 0) < NP2_FIND_REPLACE_LIMIT) {
			char	mszSelection[NP2_FIND_REPLACE_LIMIT];
			char	*lpsz;

			SendMessage(hwndEdit, SCI_GETSELTEXT, 0, (LPARAM)mszSelection);
			mszSelection[cchSelection] = 0; // zero terminate

			// Check lpszSelection and truncate newlines
			lpsz = StrPBrkA(mszSelection, "\r\n");
			if (lpsz) {
				*lpsz = '\0';
			}

			cpLastFind = (UINT)SendMessage(hwndEdit, SCI_GETCODEPAGE, 0, 0);
			lstrcpyA(efrData.szFind, mszSelection);

			if (cpLastFind != SC_CP_UTF8) {
				WCHAR wszBuf[NP2_FIND_REPLACE_LIMIT];
				MultiByteToWideChar(cpLastFind, 0, mszSelection, -1, wszBuf, COUNTOF(wszBuf));
				WideCharToMultiByte(CP_UTF8, 0, wszBuf, -1, efrData.szFindUTF8, COUNTOF(efrData.szFindUTF8), NULL, NULL);
			} else {
				lstrcpyA(efrData.szFindUTF8, mszSelection);
			}

			efrData.fuFlags &= (~(SCFIND_REGEXP | SCFIND_POSIX));
			efrData.bTransformBS = FALSE;

			switch (LOWORD(wParam)) {
			case IDM_EDIT_SAVEFIND:
				break;

			case CMD_FINDNEXTSEL:
				EditFindNext(hwndEdit, &efrData, FALSE);
				break;

			case CMD_FINDPREVSEL:
				EditFindPrev(hwndEdit, &efrData, FALSE);
				break;
			}
		}
	}
	break;

	case CMD_INCLINELIMIT:
	case CMD_DECLINELIMIT:
		if (!bMarkLongLines) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_LONGLINEMARKER, 1), 0);
		} else {
			if (LOWORD(wParam) == CMD_INCLINELIMIT) {
				iLongLinesLimit++;
			} else {
				iLongLinesLimit--;
			}
			iLongLinesLimit = clamp_i(iLongLinesLimit, 0, NP2_LONG_LINE_LIMIT);
			SendMessage(hwndEdit, SCI_SETEDGECOLUMN, iLongLinesLimit, 0);
			UpdateStatusbar();
			iLongLinesLimitG = iLongLinesLimit;
		}
		break;

	case CMD_EMBRACKETR:	// Ctrl+4
		EditEncloseSelection(hwndEdit, L"(", L")");
		break;
	case CMD_EMBRACKETC:	// Ctrl+5
		EditEncloseSelection(hwndEdit, L"{", L"}");
		break;
	case CMD_EMBRACKETA:	// Ctrl+6
		EditEncloseSelection(hwndEdit, L"<", L">");
		break;
	case CMD_EMBRACKETS:	// Ctrl+7
		EditEncloseSelection(hwndEdit, L"[", L"]");
		break;
	case CMD_STRINGIFYS:	// Ctrl+1
		EditEncloseSelection(hwndEdit, L"'", L"'");
		break;
	case CMD_STRINGIFYD:	// Ctrl+2
		EditEncloseSelection(hwndEdit, L"\"", L"\"");
		break;
	case CMD_STRINGIFYT2:	// Ctrl+3
		EditEncloseSelection(hwndEdit, L"\"\"\"", L"\"\"\"");
		break;
	case CMD_STRINGIFYT1:	// Ctrl+9
		EditEncloseSelection(hwndEdit, L"'''", L"'''");
		break;
	case CMD_STRINGIFYB:	// Ctrl+8
		EditEncloseSelection(hwndEdit, L"`", L"`");
		break;

	case CMD_INCREASENUM:	// Ctrl++
		EditModifyNumber(hwndEdit, TRUE);
		break;

	case CMD_DECREASENUM:	// Ctrl+-
		EditModifyNumber(hwndEdit, FALSE);
		break;

	case CMD_JUMP2SELSTART:	// Ctrl+'
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			int iAnchorPos = (int)SendMessage(hwndEdit, SCI_GETANCHOR, 0, 0);
			int iCursorPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
			if (iCursorPos > iAnchorPos) {
				SendMessage(hwndEdit, SCI_SETSEL, iCursorPos, iAnchorPos);
				SendMessage(hwndEdit, SCI_CHOOSECARETX, 0, 0);
			}
		}
		break;

	case CMD_JUMP2SELEND:	// Ctrl+.
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			int iAnchorPos = (int)SendMessage(hwndEdit, SCI_GETANCHOR, 0, 0);
			int iCursorPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
			if (iCursorPos < iAnchorPos) {
				SendMessage(hwndEdit, SCI_SETSEL, iCursorPos, iAnchorPos);
				SendMessage(hwndEdit, SCI_CHOOSECARETX, 0, 0);
			}
		}
		break;

	case CMD_COPYWINPOS: {
		WCHAR wszWinPos[256];
		WINDOWPLACEMENT wndpl;
		int x, y, cx, cy, max;

		wndpl.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwndMain, &wndpl);

		x = wndpl.rcNormalPosition.left;
		y = wndpl.rcNormalPosition.top;
		cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		max = (IsZoomed(hwndMain) || (wndpl.flags & WPF_RESTORETOMAXIMIZED));

		wsprintf(wszWinPos, L"/pos %i,%i,%i,%i,%i", x, y, cx, cy, max);

		SetClipDataW(hwnd, wszWinPos);
		UpdateToolbar();
	}
	break;

	case CMD_DEFAULTWINPOS:
		SnapToDefaultPos(hwnd);
		break;

	case CMD_OPENINIFILE:
		if (StrNotEmpty(szIniFile)) {
			CreateIniFile();
			FileLoad(FALSE, FALSE, FALSE, FALSE, szIniFile);
		}
		break;

	case IDT_FILE_NEW:
		if (IsCmdEnabled(hwnd, IDM_FILE_NEW)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_NEW, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_OPEN:
		if (IsCmdEnabled(hwnd, IDM_FILE_OPEN)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPEN, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_BROWSE:
		if (IsCmdEnabled(hwnd, IDM_FILE_BROWSE)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_BROWSE, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_SAVE:
		if (IsCmdEnabled(hwnd, IDM_FILE_SAVE)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_SAVE, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_EDIT_UNDO:
		if (IsCmdEnabled(hwnd, IDM_EDIT_UNDO)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_UNDO, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_EDIT_REDO:
		if (IsCmdEnabled(hwnd, IDM_EDIT_REDO)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_REDO, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_EDIT_CUT:
		if (IsCmdEnabled(hwnd, IDM_EDIT_CUT)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_CUT, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_EDIT_COPY:
		if (IsCmdEnabled(hwnd, IDM_EDIT_COPY)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_COPY, 1), 0);
		} else {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_COPYALL, 1), 0);
		}
		break;

	case IDT_EDIT_PASTE:
		if (IsCmdEnabled(hwnd, IDM_EDIT_PASTE)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_PASTE, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_EDIT_FIND:
		if (IsCmdEnabled(hwnd, IDM_EDIT_FIND)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_FIND, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_EDIT_REPLACE:
		if (IsCmdEnabled(hwnd, IDM_EDIT_REPLACE)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_REPLACE, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_VIEW_WORDWRAP:
		if (IsCmdEnabled(hwnd, IDM_VIEW_WORDWRAP)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_WORDWRAP, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_VIEW_ZOOMIN:
		if (IsCmdEnabled(hwnd, IDM_VIEW_ZOOMIN)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_ZOOMIN, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_VIEW_ZOOMOUT:
		if (IsCmdEnabled(hwnd, IDM_VIEW_ZOOMOUT)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_ZOOMOUT, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_VIEW_SCHEME:
		if (IsCmdEnabled(hwnd, IDM_VIEW_SCHEME)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_SCHEME, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_VIEW_SCHEMECONFIG:
		if (IsCmdEnabled(hwnd, IDM_VIEW_SCHEMECONFIG)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_SCHEMECONFIG, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_EXIT:
		NP2ExitWind(hwnd);
		break;

	case IDT_FILE_SAVEAS:
		if (IsCmdEnabled(hwnd, IDM_FILE_SAVEAS)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_SAVEAS, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_SAVECOPY:
		if (IsCmdEnabled(hwnd, IDM_FILE_SAVECOPY)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_SAVECOPY, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_EDIT_CLEAR:
		if (IsCmdEnabled(hwnd, IDM_EDIT_CLEAR)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_EDIT_CLEAR, 1), 0);
		} else {
			SendMessage(hwndEdit, SCI_CLEARALL, 0, 0);
		}
		break;

	case IDT_FILE_PRINT:
		if (IsCmdEnabled(hwnd, IDM_FILE_PRINT)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_PRINT, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_OPENFAV:
		if (IsCmdEnabled(hwnd, IDM_FILE_OPENFAV)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPENFAV, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_ADDTOFAV:
		if (IsCmdEnabled(hwnd, IDM_FILE_ADDTOFAV)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_ADDTOFAV, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_VIEW_TOGGLEFOLDS:
		if (IsCmdEnabled(hwnd, IDM_VIEW_TOGGLEFOLDS)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_TOGGLEFOLDS, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_LAUNCH:
		if (IsCmdEnabled(hwnd, IDM_FILE_LAUNCH)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_LAUNCH, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	}

	return 0;
}

//=============================================================================
//
// MsgNotify() - Handles WM_NOTIFY
//
//
LRESULT MsgNotify(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	LPNMHDR pnmh = (LPNMHDR)lParam;
	struct SCNotification *scn = (struct SCNotification *)lParam;

	switch (pnmh->idFrom) {
	case IDC_EDIT:
		switch (pnmh->code) {
		case SCN_UPDATEUI:
			if (scn->updated & ~(SC_UPDATE_V_SCROLL | SC_UPDATE_H_SCROLL)) {
				UpdateToolbar();
				UpdateStatusbar();

				// Invalidate invalid selections
				// TODO: Remove check for invalid selections once fixed in Scintilla
				if (SendMessage(hwndEdit, SCI_GETSELECTIONS, 0, 0) > 1 &&
						SendMessage(hwndEdit, SCI_GETSELECTIONMODE, 0, 0) != SC_SEL_RECTANGLE) {
					int iCurPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
					SendMessage(hwndEdit, WM_CANCELMODE, 0, 0);
					SendMessage(hwndEdit, SCI_CLEARSELECTIONS, 0, 0);
					SendMessage(hwndEdit, SCI_SETSELECTION, (WPARAM)iCurPos, (LPARAM)iCurPos);
				}

				// mark occurrences of text currently selected
				EditMarkAll(hwndEdit, iMarkOccurrences, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords);
				UpdateStatusbar();

				// Brace Match
				if (bMatchBraces) {
					int iPos;
					char c;

					int iEndStyled = (int)SendMessage(hwndEdit, SCI_GETENDSTYLED, 0, 0);
					if (iEndStyled < (int)SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0)) {
						int iLine = (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION, iEndStyled, 0);
						iEndStyled = (int)SendMessage(hwndEdit, SCI_POSITIONFROMLINE, iLine, 0);
						SendMessage(hwndEdit, SCI_COLOURISE, iEndStyled, -1);
					}

					iPos = (int)SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0);
					c = (char)SendMessage(hwndEdit, SCI_GETCHARAT, iPos, 0);
					if (StrChrA("()[]{}<>", c)) {
						int iBrace2 = (int)SendMessage(hwndEdit, SCI_BRACEMATCH, iPos, 0);
						if (iBrace2 != -1) {
							int col1 = (int)SendMessage(hwndEdit, SCI_GETCOLUMN, iPos, 0);
							int col2 = (int)SendMessage(hwndEdit, SCI_GETCOLUMN, iBrace2, 0);
							SendMessage(hwndEdit, SCI_BRACEHIGHLIGHT, iPos, iBrace2);
							SendMessage(hwndEdit, SCI_SETHIGHLIGHTGUIDE, min_i(col1, col2), 0);
						} else {
							SendMessage(hwndEdit, SCI_BRACEBADLIGHT, iPos, 0);
							SendMessage(hwndEdit, SCI_SETHIGHLIGHTGUIDE, 0, 0);
						}
					} else { // Try one before
						iPos = (int)SendMessage(hwndEdit, SCI_POSITIONBEFORE, iPos, 0);
						c = (char)SendMessage(hwndEdit, SCI_GETCHARAT, iPos, 0);
						if (StrChrA("()[]{}<>", c)) {
							int iBrace2 = (int)SendMessage(hwndEdit, SCI_BRACEMATCH, iPos, 0);
							if (iBrace2 != -1) {
								int col1 = (int)SendMessage(hwndEdit, SCI_GETCOLUMN, iPos, 0);
								int col2 = (int)SendMessage(hwndEdit, SCI_GETCOLUMN, iBrace2, 0);
								SendMessage(hwndEdit, SCI_BRACEHIGHLIGHT, iPos, iBrace2);
								SendMessage(hwndEdit, SCI_SETHIGHLIGHTGUIDE, min_i(col1, col2), 0);
							} else {
								SendMessage(hwndEdit, SCI_BRACEBADLIGHT, iPos, 0);
								SendMessage(hwndEdit, SCI_SETHIGHLIGHTGUIDE, 0, 0);
							}
						} else {
							SendMessage(hwndEdit, SCI_BRACEHIGHLIGHT, (WPARAM) - 1, (LPARAM) - 1);
							SendMessage(hwndEdit, SCI_SETHIGHLIGHTGUIDE, 0, 0);
						}
					}
				}
			}
			break;

		case SCN_CHARADDED:
			if (scn->ch > 0x7F) {
				return 0;
			}
			// Auto indent
			if (bAutoIndent && (scn->ch == '\r' || scn->ch == '\n')) {
				// in CRLF mode handle LF only...
				if ((SC_EOL_CRLF == iEOLMode && scn->ch != '\n') || SC_EOL_CRLF != iEOLMode) {
					EditAutoIndent(hwndEdit);
				}
			}
			// Auto close tags
			else if ((bAutoCloseTags || bAutoCompleteWords) && scn->ch == '>') {
				EditAutoCloseXMLTag(hwndEdit);
			}
			// Auto close braces/quotes
			else if (bAutoCloseBracesQuotes && StrChrA("([{<\"\'`,", (char)(scn->ch))) {
				EditAutoCloseBraceQuote(hwndEdit, scn->ch);
			} else if (bAutoCompleteWords/* && !SendMessage(hwndEdit, SCI_AUTOCACTIVE, 0, 0)*/) {
				// many items in auto-completion list (> iAutoCDefaultShowItemCount), recreate it
				if (!SendMessage(hwndEdit, SCI_AUTOCACTIVE, 0, 0) || iAutoCItemCount > iAutoCDefaultShowItemCount) {
					if (bAutoCEnglishIMEModeOnly && scn->modifiers) { // ignore IME input
						return 0;
					}
					EditCompleteWord(hwndEdit, FALSE);
				}
			}
			break;

		case SCN_AUTOCSELECTION:
		case SCN_USERLISTSELECTION: {
			Sci_Position iCurPos = SciCall_GetCurrentPos();
			SendMessage(hwndEdit, SCI_BEGINUNDOACTION, 0, 0);
			SendMessage(hwndEdit, SCI_SETSEL, scn->position, iCurPos);
			SendMessage(hwndEdit, SCI_REPLACESEL, 0, (LPARAM)scn->text);
			// function/array/template/generic
			if ((iCurPos = lstrlenA(StrPBrkA(scn->text, "([{<"))) != 0) {
				//if ((iCurPos = lstrlenA(StrChrA(scn->text, '(')))) {
				iCurPos = 1 - iCurPos;
			}
			iCurPos += scn->position + lstrlenA(scn->text);
			SendMessage(hwndEdit, SCI_SETSEL, iCurPos, iCurPos);
			SendMessage(hwndEdit, SCI_ENDUNDOACTION, 0, 0);
			SendMessage(hwndEdit, SCI_AUTOCCANCEL, 0, 0);
		}
		break;

#if NP2_ENABLE_SHOW_CALL_TIPS
		// CallTips
		case SCN_DWELLSTART:
			if (bShowCallTips && scn->position >= 0) {
				EditShowCallTips(hwndEdit, scn->position);
			}
			break;

		case SCN_DWELLEND:
			SendMessage(hwndEdit, SCI_CALLTIPCANCEL, 0, 0);
			break;
#endif

		case SCN_MODIFIED:
			if (scn->linesAdded) {
				UpdateLineNumberWidth();
			}
			break;

		case SCN_ZOOM:
			MsgNotifyZoom();
			break;

		case SCN_SAVEPOINTREACHED:
			bModified = FALSE;
			UpdateWindowTitle();
			break;

		case SCN_MARGINCLICK:
			if (scn->margin == MARGIN_FOLD_INDEX) {
				FoldClick(SciCall_LineFromPosition(scn->position), scn->modifiers);
			}
			break;

		case SCN_KEY:
			// Also see the corresponding patch in scintilla\src\Editor.cxx
			FoldAltArrow(scn->ch, scn->modifiers);
			break;

		case SCN_SAVEPOINTLEFT:
			bModified = TRUE;
			UpdateWindowTitle();
			break;

		case SCN_URIDROPPED: {
			WCHAR szBuf[MAX_PATH + 40];
			if (MultiByteToWideChar(CP_UTF8, 0, scn->text, -1, szBuf, COUNTOF(szBuf)) > 0) {
				OnDropOneFile(hwnd, szBuf);
			}
		}
		break;
		}
		break;

	case IDC_TOOLBAR:
		switch (pnmh->code) {
		case TBN_ENDADJUST:
			UpdateToolbar();
			break;

		case TBN_QUERYDELETE:
		case TBN_QUERYINSERT:
			return TRUE;

		case TBN_GETBUTTONINFO: {
			if (((LPTBNOTIFY)lParam)->iItem < (int)COUNTOF(tbbMainWnd)) {
				WCHAR tch[256];
				GetString(tbbMainWnd[((LPTBNOTIFY)lParam)->iItem].idCommand, tch, COUNTOF(tch));
				lstrcpyn(((LPTBNOTIFY)lParam)->pszText, /*StrChr(tch, L'\n')+1*/tch, ((LPTBNOTIFY)lParam)->cchText);
				CopyMemory(&((LPTBNOTIFY)lParam)->tbButton, &tbbMainWnd[((LPTBNOTIFY)lParam)->iItem], sizeof(TBBUTTON));
				return TRUE;
			}
		}
		return FALSE;

		case TBN_RESET: {
			int c = (int)SendMessage(hwndToolbar, TB_BUTTONCOUNT, 0, 0);
			for (int i = 0; i < c; i++) {
				SendMessage(hwndToolbar, TB_DELETEBUTTON, 0, 0);
			}
			SendMessage(hwndToolbar, TB_ADDBUTTONS, NUMINITIALTOOLS, (LPARAM)tbbMainWnd);
			return 0;
		}

		}
		break;

	case IDC_STATUSBAR:
		switch (pnmh->code) {
		case NM_CLICK: {
			LPNMMOUSE pnmm = (LPNMMOUSE)lParam;

			switch (pnmm->dwItemSpec) {
			case STATUS_EOLMODE:
				SendMessage(hwndEdit, SCI_CONVERTEOLS, SendMessage(hwndEdit, SCI_GETEOLMODE, 0, 0), 0);
				EditFixPositions(hwndEdit);
				return TRUE;

			default:
				return FALSE;
			}
		}

		case NM_DBLCLK: {
			int i;
			LPNMMOUSE pnmm = (LPNMMOUSE)lParam;

			switch (pnmm->dwItemSpec) {
			case STATUS_CODEPAGE:
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_ENCODING_SELECT, 1), 0);
				return TRUE;

			case STATUS_EOLMODE:
				if (iEOLMode == SC_EOL_CRLF) {
					i = IDM_LINEENDINGS_CRLF;
				} else if (iEOLMode == SC_EOL_LF) {
					i = IDM_LINEENDINGS_LF;
				} else {
					i = IDM_LINEENDINGS_CR;
				}
				i++;
				if (i > IDM_LINEENDINGS_CR) {
					i = IDM_LINEENDINGS_CRLF;
				}
				SendMessage(hwnd, WM_COMMAND, MAKELONG(i, 1), 0);
				return TRUE;

			case STATUS_LEXER:
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_SCHEME, 1), 0);
				return TRUE;

			case STATUS_OVRMODE:
				SendMessage(hwndEdit, SCI_EDITTOGGLEOVERTYPE, 0, 0);
				UpdateStatusBarCache(STATUS_OVRMODE);
				return TRUE;

			default:
				return FALSE;
			}
		}
		break;

		}
		break;

	default:
		switch (pnmh->code) {
		case TTN_NEEDTEXT: {
			if (((LPTOOLTIPTEXT)lParam)->uFlags & TTF_IDISHWND) {
				//nop;
			} else {
				WCHAR tch[256];
				GetString((UINT)pnmh->idFrom, tch, COUNTOF(tch));
				lstrcpyn(((LPTOOLTIPTEXT)lParam)->szText, /*StrChr(tch, L'\n')+1*/tch, 80);
			}
		}
		break;

		}
		break;
	}

	return 0;
}

extern BOOL fStylesModified;
//=============================================================================
//
// LoadSettings()
//
//
void LoadSettings(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);

	LoadIniSection(L"Settings", pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	const int iSettingsVersion = IniSectionGetInt(pIniSection, L"SettingsVersion", NP2SettingsVersion_None);
	fStylesModified = iSettingsVersion != NP2SettingsVersion_Current; // compress old ini file on save
	bSaveSettings = IniSectionGetBool(pIniSection, L"SaveSettings", 1);
	bSaveRecentFiles = IniSectionGetBool(pIniSection, L"SaveRecentFiles", 0);
	bSaveFindReplace = IniSectionGetBool(pIniSection, L"SaveFindReplace", 0);

	efrData.bFindClose = IniSectionGetBool(pIniSection, L"CloseFind", 0);
	efrData.bReplaceClose = IniSectionGetBool(pIniSection, L"CloseReplace", 0);
	efrData.bNoFindWrap = IniSectionGetBool(pIniSection, L"NoFindWrap", 0);

	if (!IniSectionGetString(pIniSection, L"OpenWithDir", L"", tchOpenWithDir, COUNTOF(tchOpenWithDir))) {
		SHGetSpecialFolderPath(NULL, tchOpenWithDir, CSIDL_DESKTOPDIRECTORY, TRUE);
	} else {
		PathAbsoluteFromApp(tchOpenWithDir, NULL, COUNTOF(tchOpenWithDir), TRUE);
	}

	if (!IniSectionGetString(pIniSection, L"Favorites", L"", tchFavoritesDir, COUNTOF(tchFavoritesDir))) {
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, tchFavoritesDir);
	} else {
		PathAbsoluteFromApp(tchFavoritesDir, NULL, COUNTOF(tchFavoritesDir), TRUE);
	}

	iPathNameFormat = IniSectionGetInt(pIniSection, L"PathNameFormat", 1);
	iPathNameFormat = clamp_i(iPathNameFormat, 0, 2);

	fWordWrap = IniSectionGetBool(pIniSection, L"WordWrap", 1);
	fWordWrapG = fWordWrap;

	iWordWrapMode = IniSectionGetInt(pIniSection, L"WordWrapMode", SC_WRAP_WORD);
	iWordWrapMode = clamp_i(iWordWrapMode, SC_WRAP_WORD, SC_WRAP_WHITESPACE);

	iWordWrapIndent = IniSectionGetInt(pIniSection, L"WordWrapIndent", 0);
	iWordWrapIndent = clamp_i(iWordWrapIndent, 0, 7);

	iWordWrapSymbols = IniSectionGetInt(pIniSection, L"WordWrapSymbols", 22);
	iWordWrapSymbols = clamp_i(iWordWrapSymbols % 10, 0, 2) + clamp_i((iWordWrapSymbols % 100 - iWordWrapSymbols % 10) / 10, 0, 2) * 10;

	bShowWordWrapSymbols = IniSectionGetBool(pIniSection, L"ShowWordWrapSymbols", 0);
	bMatchBraces = IniSectionGetBool(pIniSection, L"MatchBraces", 1);
	bHiliteCurrentLine = IniSectionGetBool(pIniSection, L"HighlightCurrentLine", 0);
	bShowIndentGuides = IniSectionGetBool(pIniSection, L"ShowIndentGuides", 0);

	bAutoIndent = IniSectionGetBool(pIniSection, L"AutoIndent", 1);
	bAutoCloseTags = IniSectionGetBool(pIniSection, L"AutoCloseTags", 1);
	bAutoCloseBracesQuotes = IniSectionGetBool(pIniSection, L"AutoCloseBracesQuotes", 1);
	bAutoCompleteWords = IniSectionGetBool(pIniSection, L"AutoCompleteWords", 1);
	bAutoCIncludeDocWord = IniSectionGetBool(pIniSection, L"AutoCIncludeDocWord", 1);
	bAutoCEnglishIMEModeOnly = IniSectionGetBool(pIniSection, L"AutoCEnglishIMEModeOnly", 0);

	iAutoCDefaultShowItemCount = IniSectionGetInt(pIniSection, L"AutoCDefaultShowItemCount", 16);
	iAutoCMinWordLength = IniSectionGetInt(pIniSection, L"AutoCMinWordLength", 1);
	iAutoCMinNumberLength = IniSectionGetInt(pIniSection, L"AutoCMinNumberLength", 3);

#if NP2_ENABLE_SHOW_CALL_TIPS
	bShowCallTips = IniSectionGetBool(pIniSection, L"ShowCallTips", 0);
	iCallTipsWaitTime = IniSectionGetInt(pIniSection, L"CallTipsWaitTime", 500);
#endif

	bTabsAsSpaces = IniSectionGetBool(pIniSection, L"TabsAsSpaces", 0);
	bTabsAsSpacesG = bTabsAsSpaces;
	bTabIndents = IniSectionGetBool(pIniSection, L"TabIndents", 1);
	bTabIndentsG = bTabIndents;

	bBackspaceUnindents = IniSectionGetBool(pIniSection, L"BackspaceUnindents", 0);

	iZoomLevel = IniSectionGetInt(pIniSection, L"ZoomLevel", 100);
	// Added in v4.2.25.1172, stored as a relative font size in point, in range [-10, 20].
	iZoomLevel = (iSettingsVersion < NP2SettingsVersion_V1)? 100 : iZoomLevel;
	iZoomLevel = clamp_i(iZoomLevel, SC_MIN_ZOOM_LEVEL, SC_MAX_ZOOM_LEVEL);

	iTabWidth = IniSectionGetInt(pIniSection, L"TabWidth", 4);
	iTabWidth = clamp_i(iTabWidth, 1, 256);
	iTabWidthG = iTabWidth;

	iIndentWidth = IniSectionGetInt(pIniSection, L"IndentWidth", 4);
	iIndentWidth = clamp_i(iIndentWidth, 0, 256);
	iIndentWidthG = iIndentWidth;

	bMarkLongLines = IniSectionGetBool(pIniSection, L"MarkLongLines", 1);

	iLongLinesLimit = IniSectionGetInt(pIniSection, L"LongLinesLimit", 80);
	iLongLinesLimit = clamp_i(iLongLinesLimit, 0, NP2_LONG_LINE_LIMIT);
	iLongLinesLimitG = iLongLinesLimit;

	iLongLineMode = IniSectionGetInt(pIniSection, L"LongLineMode", EDGE_LINE);
	iLongLineMode = clamp_i(iLongLineMode, EDGE_LINE, EDGE_BACKGROUND);

	bShowSelectionMargin = IniSectionGetBool(pIniSection, L"ShowSelectionMargin", 0);
	bShowLineNumbers = IniSectionGetBool(pIniSection, L"ShowLineNumbers", 1);
	bShowCodeFolding = IniSectionGetBool(pIniSection, L"ShowCodeFolding", 1);

	iMarkOccurrences = IniSectionGetInt(pIniSection, L"MarkOccurrences", 3);
	bMarkOccurrencesMatchCase = IniSectionGetBool(pIniSection, L"MarkOccurrencesMatchCase", 1);
	bMarkOccurrencesMatchWords = IniSectionGetBool(pIniSection, L"MarkOccurrencesMatchWholeWords", 0);

	bViewWhiteSpace = IniSectionGetBool(pIniSection, L"ViewWhiteSpace", 0);
	bViewEOLs = IniSectionGetBool(pIniSection, L"ViewEOLs", 0);

	iDefaultEncoding = Encoding_MapIniSetting(FALSE, CPI_UTF8);
	iDefaultEncoding = IniSectionGetInt(pIniSection, L"DefaultEncoding", iDefaultEncoding);
	iDefaultEncoding = Encoding_MapIniSetting(TRUE, iDefaultEncoding);
	if (!Encoding_IsValid(iDefaultEncoding)) {
		iDefaultEncoding = CPI_UTF8;
	}

	bSkipUnicodeDetection = IniSectionGetBool(pIniSection, L"SkipUnicodeDetection", 1);
	bLoadASCIIasUTF8 = IniSectionGetBool(pIniSection, L"LoadASCIIasUTF8", 0);
	bLoadNFOasOEM = IniSectionGetBool(pIniSection, L"LoadNFOasOEM", 1);
	bNoEncodingTags = IniSectionGetBool(pIniSection, L"NoEncodingTags", 0);

	iDefaultEOLMode = IniSectionGetInt(pIniSection, L"DefaultEOLMode", 0);
	iDefaultEOLMode = clamp_i(iDefaultEOLMode, SC_EOL_CRLF, SC_EOL_LF);

	bFixLineEndings = IniSectionGetBool(pIniSection, L"FixLineEndings", 1);
	bAutoStripBlanks = IniSectionGetBool(pIniSection, L"FixTrailingBlanks", 0);

	iPrintHeader = IniSectionGetInt(pIniSection, L"PrintHeader", 1);
	iPrintHeader = clamp_i(iPrintHeader, 0, 3);

	iPrintFooter = IniSectionGetInt(pIniSection, L"PrintFooter", 0);
	iPrintFooter = clamp_i(iPrintFooter, 0, 1);

	iPrintColor = IniSectionGetInt(pIniSection, L"PrintColorMode", SC_PRINT_COLOURONWHITE);
	iPrintColor = clamp_i(iPrintColor, SC_PRINT_NORMAL, SC_PRINT_SCREENCOLOURS);

	iPrintZoom = IniSectionGetInt(pIniSection, L"PrintZoom", 100);
	// previously stored as a relative font size in point plus 10, in range [-10, 20] + 10.
	iPrintZoom = (iSettingsVersion < NP2SettingsVersion_V1)? 100 : iPrintZoom;
	iPrintZoom = clamp_i(iPrintZoom, SC_MIN_ZOOM_LEVEL, SC_MAX_ZOOM_LEVEL);

	pagesetupMargin.left = IniSectionGetInt(pIniSection, L"PrintMarginLeft", -1);
	pagesetupMargin.left = max_i(pagesetupMargin.left, -1);

	pagesetupMargin.top = IniSectionGetInt(pIniSection, L"PrintMarginTop", -1);
	pagesetupMargin.top = max_i(pagesetupMargin.top, -1);

	pagesetupMargin.right = IniSectionGetInt(pIniSection, L"PrintMarginRight", -1);
	pagesetupMargin.right = max_i(pagesetupMargin.right, -1);

	pagesetupMargin.bottom = IniSectionGetInt(pIniSection, L"PrintMarginBottom", -1);
	pagesetupMargin.bottom = max_i(pagesetupMargin.bottom, -1);

	bSaveBeforeRunningTools = IniSectionGetBool(pIniSection, L"SaveBeforeRunningTools", 0);

	iFileWatchingMode = IniSectionGetInt(pIniSection, L"FileWatchingMode", 2);
	iFileWatchingMode = clamp_i(iFileWatchingMode, 0, 2);

	bResetFileWatching = IniSectionGetBool(pIniSection, L"ResetFileWatching", 0);

	iEscFunction = IniSectionGetInt(pIniSection, L"EscFunction", 0);
	iEscFunction = clamp_i(iEscFunction, 0, 2);

	bAlwaysOnTop = IniSectionGetBool(pIniSection, L"AlwaysOnTop", 0);
	bMinimizeToTray = IniSectionGetBool(pIniSection, L"MinimizeToTray", 0);
	bTransparentMode = IniSectionGetBool(pIniSection, L"TransparentMode", 0);

	iRenderingTechnology = IniSectionGetInt(pIniSection, L"RenderingTechnology", (IsVistaAndAbove()? SC_TECHNOLOGY_DIRECTWRITE : SC_TECHNOLOGY_DEFAULT));
	iRenderingTechnology = clamp_i(iRenderingTechnology, SC_TECHNOLOGY_DEFAULT, SC_TECHNOLOGY_DIRECTWRITEDC);

	iBidirectional = IniSectionGetInt(pIniSection, L"Bidirectional", SC_BIDIRECTIONAL_DISABLED);
	iBidirectional = clamp_i(iBidirectional, SC_BIDIRECTIONAL_DISABLED, SC_BIDIRECTIONAL_R2L);

	iFontQuality = IniSectionGetInt(pIniSection, L"FontQuality", SC_EFF_QUALITY_LCD_OPTIMIZED);
	iFontQuality = clamp_i(iFontQuality, SC_EFF_QUALITY_DEFAULT, SC_EFF_QUALITY_LCD_OPTIMIZED);

	iCaretStyle = IniSectionGetInt(pIniSection, L"CaretStyle", 1);
	iCaretStyle = clamp_i(iCaretStyle, 0, 3);
	iCaretBlinkPeriod = IniSectionGetInt(pIniSection, L"CaretBlinkPeriod", -1);

	// Korean IME use inline mode (and block caret in inline mode) by default
	bUseInlineIME = IniSectionGetBool(pIniSection, L"UseInlineIME", -1);
	bInlineIMEUseBlockCaret = IniSectionGetBool(pIniSection, L"InlineIMEUseBlockCaret", 0);
	if (bUseInlineIME == -1) { // auto detection once
		// ScintillaWin::KoreanIME()
		const int codePage = Scintilla_InputCodePage();
		if (codePage == 949 || codePage == 1361) {
			bUseInlineIME = TRUE;
			bInlineIMEUseBlockCaret = TRUE;
		} else {
			bUseInlineIME = FALSE;
		}
	}

	if (!IniSectionGetString(pIniSection, L"ToolbarButtons", L"", tchToolbarButtons, COUNTOF(tchToolbarButtons))) {
		lstrcpy(tchToolbarButtons, DefaultToolbarButtons);
	}

	bShowToolbar = IniSectionGetBool(pIniSection, L"ShowToolbar", 1);
	bShowStatusbar = IniSectionGetBool(pIniSection, L"ShowStatusbar", 1);

	bFullScreenOnStartup = IniSectionGetBool(pIniSection, L"FullScreenOnStartup", 0);
	bInFullScreenMode = bFullScreenOnStartup;
	bFullScreenHideMenu = IniSectionGetBool(pIniSection, L"FullScreenHideMenu", 0);
	bFullScreenHideToolbar = IniSectionGetBool(pIniSection, L"FullScreenHideToolbar", 0);
	bFullScreenHideStatusbar = IniSectionGetBool(pIniSection, L"FullScreenHideStatusbar", 0);

	cxEncodingDlg = IniSectionGetInt(pIniSection, L"EncodingDlgSizeX", 256);
	cxEncodingDlg = max_i(cxEncodingDlg, 0);

	cyEncodingDlg = IniSectionGetInt(pIniSection, L"EncodingDlgSizeY", 262);
	cyEncodingDlg = max_i(cyEncodingDlg, 0);

	cxRecodeDlg = IniSectionGetInt(pIniSection, L"RecodeDlgSizeX", 256);
	cxRecodeDlg = max_i(cxRecodeDlg, 0);

	cyRecodeDlg = IniSectionGetInt(pIniSection, L"RecodeDlgSizeY", 262);
	cyRecodeDlg = max_i(cyRecodeDlg, 0);

	cxFileMRUDlg = IniSectionGetInt(pIniSection, L"FileMRUDlgSizeX", 412);
	cxFileMRUDlg = max_i(cxFileMRUDlg, 0);

	cyFileMRUDlg = IniSectionGetInt(pIniSection, L"FileMRUDlgSizeY", 376);
	cyFileMRUDlg = max_i(cyFileMRUDlg, 0);

	cxOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeX", 384);
	cxOpenWithDlg = max_i(cxOpenWithDlg, 0);

	cyOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeY", 386);
	cyOpenWithDlg = max_i(cyOpenWithDlg, 0);

	cxFavoritesDlg = IniSectionGetInt(pIniSection, L"FavoritesDlgSizeX", 334);
	cxFavoritesDlg = max_i(cxFavoritesDlg, 0);

	cyFavoritesDlg = IniSectionGetInt(pIniSection, L"FavoritesDlgSizeY", 316);
	cyFavoritesDlg = max_i(cyFavoritesDlg, 0);

	xFindReplaceDlg = IniSectionGetInt(pIniSection, L"FindReplaceDlgPosX", 0);
	yFindReplaceDlg = IniSectionGetInt(pIniSection, L"FindReplaceDlgPosY", 0);

	if (bSaveFindReplace) {
		efrData.fuFlags = 0;
		if (IniSectionGetBool(pIniSection, L"FindReplaceMatchCase", 0)) {
			efrData.fuFlags |= SCFIND_MATCHCASE;
		}
		if (IniSectionGetBool(pIniSection, L"FindReplaceMatchWholeWorldOnly", 0)) {
			efrData.fuFlags |= SCFIND_WHOLEWORD;
		}
		if (IniSectionGetBool(pIniSection, L"FindReplaceMatchBeginingWordOnly", 0)) {
			efrData.fuFlags |= SCFIND_WORDSTART;
		}
		if (IniSectionGetBool(pIniSection, L"FindReplaceRegExpSearch", 0)) {
			efrData.fuFlags |= SCFIND_REGEXP | SCFIND_POSIX;
		}
		efrData.bTransformBS = IniSectionGetBool(pIniSection, L"FindReplaceTransformBackslash", 0);
		efrData.bNoFindWrap = IniSectionGetBool(pIniSection, L"FindReplaceDontWrapRound", 0);
		efrData.bFindClose = IniSectionGetBool(pIniSection, L"FindReplaceCloseAfterFind", 0);
		efrData.bReplaceClose = IniSectionGetBool(pIniSection, L"FindReplaceCloseAfterReplace", 0);
#ifdef BOOKMARK_EDITION
		efrData.bWildcardSearch = IniSectionGetBool(pIniSection, L"FindReplaceWildcardSearch", 0);
#endif
	}

	LoadIniSection(L"Toolbar Images", pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	IniSectionGetString(pIniSection, L"BitmapDefault", L"", tchToolbarBitmap, COUNTOF(tchToolbarBitmap));
	IniSectionGetString(pIniSection, L"BitmapHot", L"", tchToolbarBitmapHot, COUNTOF(tchToolbarBitmap));
	IniSectionGetString(pIniSection, L"BitmapDisabled", L"", tchToolbarBitmapDisabled, COUNTOF(tchToolbarBitmap));

	if (!flagPosParam /*|| bStickyWinPos*/) { // ignore window position if /p was specified
		WCHAR tchPosX[32], tchPosY[32], tchSizeX[32], tchSizeY[32], tchMaximized[32];
		int ResX = GetSystemMetrics(SM_CXSCREEN);
		int ResY = GetSystemMetrics(SM_CYSCREEN);

		wsprintf(tchPosX, L"%ix%i PosX", ResX, ResY);
		wsprintf(tchPosY, L"%ix%i PosY", ResX, ResY);
		wsprintf(tchSizeX, L"%ix%i SizeX", ResX, ResY);
		wsprintf(tchSizeY, L"%ix%i SizeY", ResX, ResY);
		wsprintf(tchMaximized, L"%ix%i Maximized", ResX, ResY);

		LoadIniSection(L"Window", pIniSectionBuf, cchIniSection);
		IniSectionParse(pIniSection, pIniSectionBuf);

		wi.x	= IniSectionGetIntEx(pIniSection, tchPosX, CW_USEDEFAULT);
		wi.y	= IniSectionGetIntEx(pIniSection, tchPosY, CW_USEDEFAULT);
		wi.cx	= IniSectionGetIntEx(pIniSection, tchSizeX, CW_USEDEFAULT);
		wi.cy	= IniSectionGetIntEx(pIniSection, tchSizeY, CW_USEDEFAULT);
		wi.max	= IniSectionGetIntEx(pIniSection, tchMaximized, 0);
		if (wi.max) {
			wi.max = 1;
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);

	iDefaultCodePage = 0;
	{
		const UINT acp = GetACP();
		if (IsDBCSCodePage(acp) || acp == CP_UTF8) {
			iDefaultCodePage = acp;
		}
	}

	{
		CHARSETINFO ci;
		if (TranslateCharsetInfo((DWORD *)(UINT_PTR)iDefaultCodePage, &ci, TCI_SRCCODEPAGE)) {
			iDefaultCharSet = ci.ciCharset;
		} else {
			iDefaultCharSet = ANSI_CHARSET;
		}
	}

	// Scintilla Styles
	Style_Load();
}

//=============================================================================
//
// SaveSettings()
//
//
void SaveSettings(BOOL bSaveSettingsNow) {
	if (StrIsEmpty(szIniFile)) {
		return;
	}

	CreateIniFile();

	if (!bSaveSettings && !bSaveSettingsNow) {
		IniSetBool(L"Settings", L"SaveSettings", bSaveSettings);
		return;
	}

	WCHAR wchTmp[MAX_PATH];
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	IniSectionSetInt(pIniSection, L"SettingsVersion", NP2SettingsVersion_Current);
	IniSectionSetBool(pIniSection, L"SaveSettings", bSaveSettings);
	IniSectionSetBoolEx(pIniSection, L"SaveRecentFiles", bSaveRecentFiles, 0);
	IniSectionSetBoolEx(pIniSection, L"SaveFindReplace", bSaveFindReplace, 0);
	IniSectionSetBoolEx(pIniSection, L"CloseFind", efrData.bFindClose, 0);
	IniSectionSetBoolEx(pIniSection, L"CloseReplace", efrData.bReplaceClose, 0);
	IniSectionSetBoolEx(pIniSection, L"NoFindWrap", efrData.bNoFindWrap, 0);
	PathRelativeToApp(tchOpenWithDir, wchTmp, COUNTOF(wchTmp), FALSE, TRUE, flagPortableMyDocs);
	IniSectionSetString(pIniSection, L"OpenWithDir", wchTmp);
	PathRelativeToApp(tchFavoritesDir, wchTmp, COUNTOF(wchTmp), FALSE, TRUE, flagPortableMyDocs);
	IniSectionSetString(pIniSection, L"Favorites", wchTmp);
	IniSectionSetIntEx(pIniSection, L"PathNameFormat", iPathNameFormat, 1);
	IniSectionSetBoolEx(pIniSection, L"WordWrap", fWordWrapG, 1);
	IniSectionSetIntEx(pIniSection, L"WordWrapMode", iWordWrapMode, SC_WRAP_WORD);
	IniSectionSetIntEx(pIniSection, L"WordWrapIndent", iWordWrapIndent, 0);
	IniSectionSetIntEx(pIniSection, L"WordWrapSymbols", iWordWrapSymbols, 22);
	IniSectionSetBoolEx(pIniSection, L"ShowWordWrapSymbols", bShowWordWrapSymbols, 0);
	IniSectionSetBoolEx(pIniSection, L"MatchBraces", bMatchBraces, 1);
	IniSectionSetBoolEx(pIniSection, L"HighlightCurrentLine", bHiliteCurrentLine, 0);
	IniSectionSetBoolEx(pIniSection, L"ShowIndentGuides", bShowIndentGuides, 0);
	IniSectionSetBoolEx(pIniSection, L"AutoIndent", bAutoIndent, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCloseTags", bAutoCloseTags, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCloseBracesQuotes", bAutoCloseBracesQuotes, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCompleteWords", bAutoCompleteWords, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCIncludeDocWord", bAutoCIncludeDocWord, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCEnglishIMEModeOnly", bAutoCEnglishIMEModeOnly, 0);
	IniSectionSetIntEx(pIniSection, L"AutoCDefaultShowItemCount", iAutoCDefaultShowItemCount, 16);
	IniSectionSetIntEx(pIniSection, L"AutoCMinWordLength", iAutoCMinWordLength, 1);
	IniSectionSetIntEx(pIniSection, L"AutoCMinNumberLength", iAutoCMinNumberLength, 3);
#if NP2_ENABLE_SHOW_CALL_TIPS
	IniSectionSetBoolEx(pIniSection, L"ShowCallTips", bShowCallTips, 1);
	IniSectionSetIntEx(pIniSection, L"CallTipsWaitTime", iCallTipsWaitTime, 500);
#endif
	IniSectionSetBoolEx(pIniSection, L"TabsAsSpaces", bTabsAsSpacesG, 0);
	IniSectionSetBoolEx(pIniSection, L"TabIndents", bTabIndentsG, 1);
	IniSectionSetBoolEx(pIniSection, L"BackspaceUnindents", bBackspaceUnindents, 0);
	IniSectionSetIntEx(pIniSection, L"ZoomLevel", iZoomLevel, 100);
	IniSectionSetIntEx(pIniSection, L"TabWidth", iTabWidthG, 4);
	IniSectionSetIntEx(pIniSection, L"IndentWidth", iIndentWidthG, 4);
	IniSectionSetBoolEx(pIniSection, L"MarkLongLines", bMarkLongLines, 1);
	IniSectionSetIntEx(pIniSection, L"LongLinesLimit", iLongLinesLimitG, 80);
	IniSectionSetIntEx(pIniSection, L"LongLineMode", iLongLineMode, EDGE_LINE);
	IniSectionSetBoolEx(pIniSection, L"ShowSelectionMargin", bShowSelectionMargin, 0);
	IniSectionSetBoolEx(pIniSection, L"ShowLineNumbers", bShowLineNumbers, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowCodeFolding", bShowCodeFolding, 1);
	IniSectionSetIntEx(pIniSection, L"MarkOccurrences", iMarkOccurrences, 3);
	IniSectionSetBoolEx(pIniSection, L"MarkOccurrencesMatchCase", bMarkOccurrencesMatchCase, 1);
	IniSectionSetBoolEx(pIniSection, L"MarkOccurrencesMatchWholeWords", bMarkOccurrencesMatchWords, 0);
	IniSectionSetBoolEx(pIniSection, L"ViewWhiteSpace", bViewWhiteSpace, 0);
	IniSectionSetBoolEx(pIniSection, L"ViewEOLs", bViewEOLs, 0);
	IniSectionSetIntEx(pIniSection, L"DefaultEncoding", Encoding_MapIniSetting(FALSE, iDefaultEncoding), Encoding_MapIniSetting(FALSE, CPI_UTF8));
	IniSectionSetBoolEx(pIniSection, L"SkipUnicodeDetection", bSkipUnicodeDetection, 1);
	IniSectionSetBoolEx(pIniSection, L"LoadASCIIasUTF8", bLoadASCIIasUTF8, 0);
	IniSectionSetBoolEx(pIniSection, L"LoadNFOasOEM", bLoadNFOasOEM, 1);
	IniSectionSetBoolEx(pIniSection, L"NoEncodingTags", bNoEncodingTags, 0);
	IniSectionSetIntEx(pIniSection, L"DefaultEOLMode", iDefaultEOLMode, 0);
	IniSectionSetBoolEx(pIniSection, L"FixLineEndings", bFixLineEndings, 1);
	IniSectionSetBoolEx(pIniSection, L"FixTrailingBlanks", bAutoStripBlanks, 0);

	IniSectionSetIntEx(pIniSection, L"PrintHeader", iPrintHeader, 1);
	IniSectionSetIntEx(pIniSection, L"PrintFooter", iPrintFooter, 0);
	IniSectionSetIntEx(pIniSection, L"PrintColorMode", iPrintColor, SC_PRINT_COLOURONWHITE);
	IniSectionSetIntEx(pIniSection, L"PrintZoom", iPrintZoom, 100);
	IniSectionSetInt(pIniSection, L"PrintMarginLeft", pagesetupMargin.left);
	IniSectionSetInt(pIniSection, L"PrintMarginTop", pagesetupMargin.top);
	IniSectionSetInt(pIniSection, L"PrintMarginRight", pagesetupMargin.right);
	IniSectionSetInt(pIniSection, L"PrintMarginBottom", pagesetupMargin.bottom);
	IniSectionSetBoolEx(pIniSection, L"SaveBeforeRunningTools", bSaveBeforeRunningTools, 0);
	IniSectionSetIntEx(pIniSection, L"FileWatchingMode", iFileWatchingMode, 2);
	IniSectionSetBoolEx(pIniSection, L"ResetFileWatching", bResetFileWatching, 0);
	IniSectionSetIntEx(pIniSection, L"EscFunction", iEscFunction, 0);
	IniSectionSetBoolEx(pIniSection, L"AlwaysOnTop", bAlwaysOnTop, 0);
	IniSectionSetBoolEx(pIniSection, L"MinimizeToTray", bMinimizeToTray, 0);
	IniSectionSetBoolEx(pIniSection, L"TransparentMode", bTransparentMode, 0);
	IniSectionSetIntEx(pIniSection, L"RenderingTechnology", iRenderingTechnology, (IsVistaAndAbove()? SC_TECHNOLOGY_DIRECTWRITE : SC_TECHNOLOGY_DEFAULT));
	IniSectionSetIntEx(pIniSection, L"Bidirectional", iBidirectional, SC_BIDIRECTIONAL_DISABLED);
	IniSectionSetIntEx(pIniSection, L"FontQuality", iFontQuality, SC_EFF_QUALITY_LCD_OPTIMIZED);
	IniSectionSetIntEx(pIniSection, L"CaretStyle", iCaretStyle, 1);
	IniSectionSetIntEx(pIniSection, L"CaretBlinkPeriod", iCaretBlinkPeriod, -1);
	IniSectionSetBool(pIniSection, L"UseInlineIME", bUseInlineIME); // keep result of auto detection
	IniSectionSetBoolEx(pIniSection, L"InlineIMEUseBlockCaret", bInlineIMEUseBlockCaret, 0);
	Toolbar_GetButtons(hwndToolbar, IDT_FILE_NEW, tchToolbarButtons, COUNTOF(tchToolbarButtons));
	IniSectionSetStringEx(pIniSection, L"ToolbarButtons", tchToolbarButtons, DefaultToolbarButtons);
	IniSectionSetBoolEx(pIniSection, L"ShowToolbar", bShowToolbar, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowStatusbar", bShowStatusbar, 1);
	IniSectionSetBoolEx(pIniSection, L"FullScreenOnStartup", bFullScreenOnStartup, 0);
	IniSectionSetBoolEx(pIniSection, L"FullScreenHideMenu", bFullScreenHideMenu, 0);
	IniSectionSetBoolEx(pIniSection, L"FullScreenHideToolbar", bFullScreenHideToolbar, 0);
	IniSectionSetBoolEx(pIniSection, L"FullScreenHideStatusbar", bFullScreenHideStatusbar, 0);
	IniSectionSetInt(pIniSection, L"EncodingDlgSizeX", cxEncodingDlg);
	IniSectionSetInt(pIniSection, L"EncodingDlgSizeY", cyEncodingDlg);
	IniSectionSetInt(pIniSection, L"RecodeDlgSizeX", cxRecodeDlg);
	IniSectionSetInt(pIniSection, L"RecodeDlgSizeY", cyRecodeDlg);
	IniSectionSetInt(pIniSection, L"FileMRUDlgSizeX", cxFileMRUDlg);
	IniSectionSetInt(pIniSection, L"FileMRUDlgSizeY", cyFileMRUDlg);
	IniSectionSetInt(pIniSection, L"OpenWithDlgSizeX", cxOpenWithDlg);
	IniSectionSetInt(pIniSection, L"OpenWithDlgSizeY", cyOpenWithDlg);
	IniSectionSetInt(pIniSection, L"FavoritesDlgSizeX", cxFavoritesDlg);
	IniSectionSetInt(pIniSection, L"FavoritesDlgSizeY", cyFavoritesDlg);
	IniSectionSetInt(pIniSection, L"FindReplaceDlgPosX", xFindReplaceDlg);
	IniSectionSetInt(pIniSection, L"FindReplaceDlgPosY", yFindReplaceDlg);

	if (bSaveFindReplace) {
		IniSectionSetBool(pIniSection, L"FindReplaceMatchCase", (efrData.fuFlags & SCFIND_MATCHCASE));
		IniSectionSetBool(pIniSection, L"FindReplaceMatchWholeWorldOnly", (efrData.fuFlags & SCFIND_WHOLEWORD));
		IniSectionSetBool(pIniSection, L"FindReplaceMatchBeginingWordOnly", (efrData.fuFlags & SCFIND_WORDSTART));
		IniSectionSetBool(pIniSection, L"FindReplaceRegExpSearch", (efrData.fuFlags & (SCFIND_REGEXP | SCFIND_POSIX)));
		IniSectionSetBool(pIniSection, L"FindReplaceTransformBackslash", efrData.bTransformBS);
		IniSectionSetBool(pIniSection, L"FindReplaceDontWrapRound", efrData.bNoFindWrap);
		IniSectionSetBool(pIniSection, L"FindReplaceCloseAfterFind", efrData.bFindClose);
		IniSectionSetBool(pIniSection, L"FindReplaceCloseAfterReplace", efrData.bReplaceClose);
#ifdef BOOKMARK_EDITION
		IniSectionSetBool(pIniSection, L"FindReplaceWildcardSearch", efrData.bWildcardSearch);
#endif
	}

	SaveIniSection(L"Settings", pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);

	/*
		SaveSettingsNow(): query Window Dimensions
	*/

	if (bSaveSettingsNow) {
		WINDOWPLACEMENT wndpl;

		// GetWindowPlacement
		wndpl.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwndMain, &wndpl);

		wi.x = wndpl.rcNormalPosition.left;
		wi.y = wndpl.rcNormalPosition.top;
		wi.cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		wi.cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		wi.max = (IsZoomed(hwndMain) || (wndpl.flags & WPF_RESTORETOMAXIMIZED));
	}

	if (!bStickyWinPos) {
		WCHAR tchPosX[32], tchPosY[32], tchSizeX[32], tchSizeY[32], tchMaximized[32];
		int ResX = GetSystemMetrics(SM_CXSCREEN);
		int ResY = GetSystemMetrics(SM_CYSCREEN);

		wsprintf(tchPosX, L"%ix%i PosX", ResX, ResY);
		wsprintf(tchPosY, L"%ix%i PosY", ResX, ResY);
		wsprintf(tchSizeX, L"%ix%i SizeX", ResX, ResY);
		wsprintf(tchSizeY, L"%ix%i SizeY", ResX, ResY);
		wsprintf(tchMaximized, L"%ix%i Maximized", ResX, ResY);

		IniSetInt(L"Window", tchPosX, wi.x);
		IniSetInt(L"Window", tchPosY, wi.y);
		IniSetInt(L"Window", tchSizeX, wi.cx);
		IniSetInt(L"Window", tchSizeY, wi.cy);
		IniSetInt(L"Window", tchMaximized, wi.max);
	}

	// Scintilla Styles
	Style_Save();
}

//=============================================================================
//
// ParseCommandLine()
//
//
int ParseCommandLineEncoding(LPCWSTR opt, int idmLE, int idmBE) {
	int flag = idmLE;
	if (*opt == '-') {
		++opt;
	}
	if (StrNCaseEqual(opt, L"LE", CSTRLEN(L"LE"))){
		flag = idmLE;
		opt += CSTRLEN(L"LE");
		if (*opt == '-') {
			++opt;
		}
	} else if (StrNCaseEqual(opt, L"BE", CSTRLEN(L"BE"))) {
		flag = idmBE;
		opt += CSTRLEN(L"BE");
		if (*opt == '-') {
			++opt;
		}
	}
	if (*opt == 0 || StrCaseEqual(opt, L"BOM") || StrCaseEqual(opt, L"SIG") || StrCaseEqual(opt, L"SIGNATURE")) {
		flagSetEncoding = flag - IDM_ENCODING_ANSI + 1;
		return 1;
	}
	return 0;
}

int ParseCommandLineOption(LPWSTR lp1, LPWSTR lp2, BOOL *bIsNotepadReplacement) {
	int state = 0;
	LPWSTR opt = lp1 + 1;
	// only accept /opt, -opt, --opt
	if (*opt == L'-') {
		++opt;
	}
	if (*opt == 0) {
		return 0;
	}

	if (opt[1] == 0) {
		switch (*CharUpper(opt)) {
		case L'A':
			flagSetEncoding = IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1;
			state = 1;
			break;

		case L'B':
			flagPasteBoard = 1;
			state = 1;
			break;

		case L'C':
			flagNewFromClipboard = 1;
			state = 1;
			break;

		case L'D':
			if (lpSchemeArg) {
				LocalFree(lpSchemeArg);
				lpSchemeArg = NULL;
			}
			iInitialLexer = Style_GetEditLexerId(EditLexer_Default);
			flagLexerSpecified = 1;
			state = 1;
			break;

		case L'E':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpEncodingArg) {
					LocalFree(lpEncodingArg);
				}
				lpEncodingArg = StrDup(lp1);
				state = 1;
			}
			break;

		case L'F':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				StrCpyN(szIniFile, lp1, COUNTOF(szIniFile));
				TrimString(szIniFile);
				PathUnquoteSpaces(szIniFile);
				state = 1;
			}
			break;

		case L'G':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				int x = 0, y = 0;
				int itok = swscanf(lp1, L"%i,%i", &x, &y);
				if (itok == 1 || itok == 2) { // scan successful
					flagJumpTo = 1;
					state = 1;
					iInitialLine = x;
					iInitialColumn = y;
				}
			}
			break;

		case L'H':
			if (lpSchemeArg) {
				LocalFree(lpSchemeArg);
				lpSchemeArg = NULL;
			}
			iInitialLexer = Style_GetEditLexerId(EditLexer_HTML);
			flagLexerSpecified = 1;
			state = 1;
			break;

		case L'I':
			flagStartAsTrayIcon = 1;
			state = 1;
			break;

		case L'L':
			flagChangeNotify = 2;
			state = 1;
			break;

		case L'N':
			flagReuseWindow = 0;
			flagNoReuseWindow = 1;
			flagSingleFileInstance = 0;
			state = 1;
			break;

		case L'O':
			flagAlwaysOnTop = 2;
			state = 1;
			break;

		case L'Q':
			flagQuietCreate = 1;
			state = 1;
			break;

		case L'R':
			flagReuseWindow = 1;
			flagNoReuseWindow = 0;
			flagSingleFileInstance = 0;
			state = 1;
			break;

		case L'S':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpSchemeArg) {
					LocalFree(lpSchemeArg);
				}
				lpSchemeArg = StrDup(lp1);
				flagLexerSpecified = 1;
				state = 1;
			}
			break;

		case L'T':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				StrCpyN(szTitleExcerpt, lp1, COUNTOF(szTitleExcerpt));
				fKeepTitleExcerpt = 1;
				state = 1;
			}
			break;

		case L'U':
			flagRelaunchElevated = 1;
			state = 1;
			break;

		case L'W':
			flagSetEncoding = IDM_ENCODING_UNICODE - IDM_ENCODING_ANSI + 1;
			state = 1;
			break;

		case L'X':
			if (lpSchemeArg) {
				LocalFree(lpSchemeArg);
				lpSchemeArg = NULL;
			}
			iInitialLexer = Style_GetEditLexerId(EditLexer_XML);
			flagLexerSpecified = 1;
			state = 1;
			break;

		case L'Z':
			ExtractFirstArgument(lp2, lp1, lp2);
			flagMultiFileArg = 1;
			*bIsNotepadReplacement = TRUE;
			state = 1;
			break;

		case L'?':
			flagDisplayHelp = 1;
			state = 1;
			break;

		default:
			state = 3;
			break;
		}
	} else if (opt[2] == 0) {
		switch (*CharUpper(opt)) {
		case L'C':
			if (opt[1] == L'R') {
				flagSetEOLMode = IDM_LINEENDINGS_CR - IDM_LINEENDINGS_CRLF + 1;
				state = 1;
			}
			break;

		case L'F':
			if (opt[1] == L'0' || *CharUpper(opt + 1) == L'O') {
				lstrcpy(szIniFile, L"*?");
				state = 1;
			}
			break;

		case L'L':
			if (opt[1] == L'F') {
				flagSetEOLMode = IDM_LINEENDINGS_LF - IDM_LINEENDINGS_CRLF + 1;
				state = 1;
			} else if (opt[1] == L'0' || opt[1] == L'-' || *CharUpper(opt + 1) == L'O') {
				flagChangeNotify = 1;
				state = 1;
			}
			break;

		case L'N':
			if (*CharUpper(opt + 1) == L'S') {
				flagReuseWindow = 0;
				flagNoReuseWindow = 1;
				flagSingleFileInstance = 1;
				state = 1;
			}
			break;

		case L'O':
			if (opt[1] == L'0' || opt[1] == L'-' || *CharUpper(opt + 1) == L'O') {
				flagAlwaysOnTop = 1;
				state = 1;
			}
			break;

		case L'R':
			if (*CharUpper(opt + 1) == L'S') {
				flagReuseWindow = 1;
				flagNoReuseWindow = 0;
				flagSingleFileInstance = 1;
				state = 1;
			}
			break;

		default:
			state = 3;
			break;
		}
	} else {
		state = 3;
	}

	if (state != 3) {
		return state;
	}

	state = 0;
	switch (*CharUpper(opt)) {
	case L'A':
		if (StrCaseEqual(opt, L"ANSI")) {
			flagSetEncoding = IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1;
			state = 1;
		} else if (StrNCaseEqual(opt, L"appid=", CSTRLEN(L"appid="))) {
			// Shell integration
			opt += CSTRLEN(L"appid=");
			StrCpyN(g_wchAppUserModelID, opt, COUNTOF(g_wchAppUserModelID));
			StrTrim(g_wchAppUserModelID, L"\" ");
			if (StrIsEmpty(g_wchAppUserModelID)) {
				lstrcpy(g_wchAppUserModelID, L"(default)");
			}
			state = 1;
		}
		break;

	case L'C':
		if (opt[1] == L'R') {
			opt += 2;
			if (*opt == L'-') {
				++opt;
			}
			if (*opt == L'L' && opt[1] == L'F' && opt[2] == 0) {
				flagSetEOLMode = IDM_LINEENDINGS_CRLF - IDM_LINEENDINGS_CRLF + 1;
				state = 1;
			}
		}
		break;

	case L'M':
		if (StrCaseEqual(opt, L"MBCS")) {
			flagSetEncoding = IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1;
			state = 1;
		} else {
			BOOL bFindUp = FALSE;
			BOOL bRegex = FALSE;
			BOOL bTransBS = FALSE;

			++opt;
			switch (*CharUpper(opt)) {
			case L'R':
				bRegex = TRUE;
				++opt;
				break;

			case L'B':
				bTransBS = TRUE;
				++opt;
				break;
			}

			if (*opt == L'-') {
				bFindUp = TRUE;
				++opt;
			}
			if (*opt != 0) {
				break;
			}

			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpMatchArg) {
					LocalFree(lpMatchArg);
				}

				lpMatchArg = StrDup(lp1);
				flagMatchText = 1;
				state = 1;

				if (bFindUp) {
					flagMatchText |= 2;
				}
				if (bRegex) {
					flagMatchText &= ~8;
					flagMatchText |= 4;
				}
				if (bTransBS) {
					flagMatchText &= ~4;
					flagMatchText |= 8;
				}
			}
		}
		break;

	case L'P': {
		if (*bIsNotepadReplacement) {
			if (*CharUpper(opt + 1) == L'T') {
				ExtractFirstArgument(lp2, lp1, lp2);
			}
			state = 1;
			break;
		}

		if (StrNCaseEqual(opt, L"POS", CSTRLEN(L"POS"))) {
			opt += CSTRLEN(L"POS");
		} else {
			++opt;
		}
		if (*opt == L':') {
			++opt;
		}

		switch (*CharUpper(opt)) {
		case L'0':
		case L'O':
			if (opt[1] == 0) {
				flagPosParam = 1;
				flagDefaultPos = 1;
				state = 1;
			}
			break;

		case L'D':
		case L'S':
			if (opt[1] == 0 || (opt[2] == 0 && *CharUpper(opt + 1) == L'L')) {
				flagPosParam = 1;
				flagDefaultPos = (opt[1] == 0)? 2 : 3;;
				state = 1;
			}
			break;

		case L'F':
		case L'L':
		case L'R':
		case L'T':
		case L'B':
		case L'M': {
			WCHAR *p = opt;
			flagPosParam = 1;
			flagDefaultPos = 0;
			state = 1;
			while (*p && state == 1) {
				switch (*CharUpper(p)) {
				case L'F':
					flagDefaultPos &= ~(4 | 8 | 16 | 32);
					flagDefaultPos |= 64;
					break;

				case L'L':
					flagDefaultPos &= ~(8 | 64);
					flagDefaultPos |= 4;
					break;

				case L'R':
					flagDefaultPos &= ~(4 | 64);
					flagDefaultPos |= 8;
					break;

				case L'T':
					flagDefaultPos &= ~(32 | 64);
					flagDefaultPos |= 16;
					break;

				case L'B':
					flagDefaultPos &= ~(16 | 64);
					flagDefaultPos |= 32;
					break;

				case L'M':
					if (flagDefaultPos == 0) {
						flagDefaultPos |= 64;
					}
					flagDefaultPos |= 128;
					break;

				default:
					state = 0;
					break;
				}
				p = CharNext(p);
			}
		}
		break;

		default:
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				int x = 0, y = 0, cx = 0, cy = 0, cm = 0;
				int itok = swscanf(lp1, L"%i,%i,%i,%i,%i", &x, &y, &cx, &cy, &cm);
				if (itok == 4 || itok == 5) { // scan successful
					flagPosParam = 1;
					flagDefaultPos = 0;
					state = 1;
					wi.x = x;
					wi.y = y;
					wi.cx = cx;
					wi.cy = cy;
					wi.max = cm;
					if (wi.cx < 1) {
						wi.cx = CW_USEDEFAULT;
					}
					if (wi.cy < 1) {
						wi.cy = CW_USEDEFAULT;
					}
					if (wi.max) {
						wi.max = 1;
					}
					if (itok == 4) {
						wi.max = 0;
					}
				}
			}
			break;
		}
	}
	break;

	case L'S':
		// Shell integration
		if (StrNCaseEqual(opt, L"sysmru=", CSTRLEN(L"sysmru="))) {
			opt += CSTRLEN(L"sysmru=");
			if (opt[1] == 0) {
				switch (*opt) {
				case L'0':
					flagUseSystemMRU = 1;
					state = 1;
					break;

				case L'1':
					flagUseSystemMRU = 2;
					state = 1;
					break;
				}
			}
		}
		break;

	case L'U':
		if (StrNCaseEqual(opt, L"UTF", CSTRLEN(L"UTF"))) {
			opt += CSTRLEN(L"UTF");
			if (*opt == '-') {
				++opt;
			}
			if (*opt == L'8') {
				++opt;
				if (*opt == '-') {
					++opt;
				}
				if (*opt == 0) {
					flagSetEncoding = IDM_ENCODING_UTF8 - IDM_ENCODING_ANSI + 1;
					state = 1;
				} else if (StrCaseEqual(opt, L"BOM") || StrCaseEqual(opt, L"SIG") || StrCaseEqual(opt, L"SIGNATURE")) {
					flagSetEncoding = IDM_ENCODING_UTF8SIGN - IDM_ENCODING_ANSI + 1;
					state = 1;
				}
			} else if (*opt == L'1' && opt[1] == L'6') {
				opt += 2;
				state = ParseCommandLineEncoding(opt, IDM_ENCODING_UNICODE, IDM_ENCODING_UNICODEREV);
			}
		} else if (StrNCaseEqual(opt, L"UNICODE", CSTRLEN(L"UNICODE"))) {
			opt += CSTRLEN(L"UNICODE");
			state = ParseCommandLineEncoding(opt, IDM_ENCODING_UNICODE, IDM_ENCODING_UNICODEREV);
		}
		break;
	}

	return state;
}

void ParseCommandLine(void) {
	LPWSTR lp1, lp2, lp3;
	BOOL bIsFileArg = FALSE;
	BOOL bIsNotepadReplacement = FALSE;

	LPWSTR lpCmdLine = GetCommandLine();
	size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLine) + 1);

	if (cmdSize == sizeof(WCHAR)) {
		return;
	}

	// Good old console can also send args separated by Tabs
	StrTab2Space(lpCmdLine);

	lp1 = NP2HeapAlloc(cmdSize);
	lp3 = NP2HeapAlloc(cmdSize);

	// Start with 2nd argument
	if (!(ExtractFirstArgument(lpCmdLine, lp1, lp3) && *lp3)) {
		NP2HeapFree(lp1);
		NP2HeapFree(lp3);
		return;
	}

	lp2 = NP2HeapAlloc(cmdSize);
	while (ExtractFirstArgument(lp3, lp1, lp2)) {
		// options
		if (!bIsFileArg) {
			int state = 0;
			if (lp1[1] == 0) {
				switch (*lp1) {
				case L'+':
					flagMultiFileArg = 2;
					bIsFileArg = TRUE;
					state = 1;
					break;

				case L'-':
					flagMultiFileArg = 1;
					bIsFileArg = TRUE;
					state = 1;
					break;
				}
			} else if (*lp1 == L'/' || *lp1 == L'-') {
				state = ParseCommandLineOption(lp1, lp2, &bIsNotepadReplacement);
			}

			if (state == 1) {
				lstrcpy(lp3, lp2);
				continue;
			}
			if (state == 2 && flagMultiFileArg == 2) {
				ExtractFirstArgument(lp3, lp1, lp2);
			}
		}

		// pathname
		{
			LPWSTR lpFileBuf = NP2HeapAlloc(cmdSize);

			if (lpFileArg) {
				NP2HeapFree(lpFileArg);
			}

			lpFileArg = NP2HeapAlloc(sizeof(WCHAR) * (MAX_PATH + 2)); // changed for ActivatePrevInst() needs
			if (flagMultiFileArg == 2) {
				// multiple file arguments with quoted spaces
				StrCpyN(lpFileArg, lp1, MAX_PATH);
			} else {
				StrCpyN(lpFileArg, lp3, MAX_PATH);
			}

			PathFixBackslashes(lpFileArg);
			StrTrim(lpFileArg, L" \"");

			if (!PathIsRelative(lpFileArg) && !PathIsUNC(lpFileArg) &&
					PathGetDriveNumber(lpFileArg) == -1 /*&& PathGetDriveNumber(g_wchWorkingDirectory) != -1*/) {

				WCHAR wchPath[MAX_PATH];
				lstrcpy(wchPath, g_wchWorkingDirectory);
				PathStripToRoot(wchPath);
				PathAppend(wchPath, lpFileArg);
				lstrcpy(lpFileArg, wchPath);
			}

			if (flagMultiFileArg == 2) {
				cchiFileList = lstrlen(lpCmdLine) - lstrlen(lp3);

				while (cFileList < 32 && ExtractFirstArgument(lp3, lpFileBuf, lp3)) {
					PathQuoteSpaces(lpFileBuf);
					lpFileList[cFileList++] = StrDup(lpFileBuf);
				}
			}

			NP2HeapFree(lpFileBuf);
			break;
		}
	}

	NP2HeapFree(lp2);
	NP2HeapFree(lp1);
	NP2HeapFree(lp3);
}

//=============================================================================
//
// LoadFlags()
//
//
void LoadFlags(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 64);

	LoadIniSection(L"Settings2", pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	bSingleFileInstance = IniSectionGetBool(pIniSection, L"SingleFileInstance", 1);
	bReuseWindow = IniSectionGetBool(pIniSection, L"ReuseWindow", 0);
	bStickyWinPos = IniSectionGetBool(pIniSection, L"StickyWindowPosition", 0);

	if (!flagReuseWindow && !flagNoReuseWindow) {
		flagNoReuseWindow = !bReuseWindow;
		flagSingleFileInstance = bSingleFileInstance;
	}

	if (flagMultiFileArg == 0) {
		if (IniSectionGetBool(pIniSection, L"MultiFileArg", 0)) {
			flagMultiFileArg = 2;
		}
	}

	flagRelativeFileMRU = IniSectionGetBool(pIniSection, L"RelativeFileMRU", 1);
	flagPortableMyDocs = IniSectionGetBool(pIniSection, L"PortableMyDocs", flagRelativeFileMRU);

	IniSectionGetString(pIniSection, L"DefaultExtension", L"txt", tchDefaultExtension, COUNTOF(tchDefaultExtension));
	StrTrim(tchDefaultExtension, L" \t.\"");

	IniSectionGetString(pIniSection, L"DefaultDirectory", L"", tchDefaultDir, COUNTOF(tchDefaultDir));

	ZeroMemory(tchFileDlgFilters, sizeof(tchFileDlgFilters));
	IniSectionGetString(pIniSection, L"FileDlgFilters", L"", tchFileDlgFilters, COUNTOF(tchFileDlgFilters) - 2);

	dwFileCheckInverval = IniSectionGetInt(pIniSection, L"FileCheckInverval", 1000);
	dwAutoReloadTimeout = IniSectionGetInt(pIniSection, L"AutoReloadTimeout", 1000);
	dwFileLoadWarningMB = IniSectionGetInt(pIniSection, L"FileLoadWarningMB", 64);

	flagNoFadeHidden = IniSectionGetBool(pIniSection, L"NoFadeHidden", 0);

	iOpacityLevel = IniSectionGetInt(pIniSection, L"OpacityLevel", 75);
	if (iOpacityLevel < 0 || iOpacityLevel > 100) {
		iOpacityLevel = 75;
	}

	flagToolbarLook = IniSectionGetInt(pIniSection, L"ToolbarLook", IsWinXPAndAbove() ? 1 : 2);
	flagToolbarLook = clamp_i(flagToolbarLook, 0, 2);

	flagSimpleIndentGuides = IniSectionGetBool(pIniSection, L"SimpleIndentGuides", 0);
	fNoHTMLGuess = IniSectionGetBool(pIniSection, L"NoHTMLGuess", 0);
	fNoFileVariables = IniSectionGetBool(pIniSection, L"NoFileVariables", 0);

	if (StrIsEmpty(g_wchAppUserModelID)) {
		IniSectionGetString(pIniSection, L"ShellAppUserModelID", L"(default)",
							g_wchAppUserModelID, COUNTOF(g_wchAppUserModelID));
	}

	if (flagUseSystemMRU == 0) {
		if (IniSectionGetBool(pIniSection, L"ShellUseSystemMRU", 0)) {
			flagUseSystemMRU = 2;
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
// FindIniFile()
//
//
int CheckIniFile(LPWSTR lpszFile, LPCWSTR lpszModule) {
	WCHAR tchFileExpanded[MAX_PATH];
	ExpandEnvironmentStrings(lpszFile, tchFileExpanded, COUNTOF(tchFileExpanded));

	if (PathIsRelative(tchFileExpanded)) {
		WCHAR tchBuild[MAX_PATH];
		// program directory
		lstrcpy(tchBuild, lpszModule);
		lstrcpy(PathFindFileName(tchBuild), tchFileExpanded);
		if (PathFileExists(tchBuild)) {
			lstrcpy(lpszFile, tchBuild);
			return 1;
		}
		// %appdata%
		if (S_OK == SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, tchBuild)) {
			PathAppend(tchBuild, tchFileExpanded);
			if (PathFileExists(tchBuild)) {
				lstrcpy(lpszFile, tchBuild);
				return 1;
			}
		}
		// general
		if (SearchPath(NULL, tchFileExpanded, NULL, COUNTOF(tchBuild), tchBuild, NULL)) {
			lstrcpy(lpszFile, tchBuild);
			return 1;
		}
	} else if (PathFileExists(tchFileExpanded)) {
		lstrcpy(lpszFile, tchFileExpanded);
		return 1;
	}

	return 0;
}

int CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule) {
	WCHAR tch[MAX_PATH];
	if (GetPrivateProfileString(L"Notepad2", L"Notepad2.ini", L"", tch, COUNTOF(tch), lpszFile)) {
		if (CheckIniFile(tch, lpszModule)) {
			lstrcpy(lpszFile, tch);
		} else {
			WCHAR tchFileExpanded[MAX_PATH];
			ExpandEnvironmentStrings(tch, tchFileExpanded, COUNTOF(tchFileExpanded));
			if (PathIsRelative(tchFileExpanded)) {
				lstrcpy(lpszFile, lpszModule);
				lstrcpy(PathFindFileName(lpszFile), tchFileExpanded);
			} else {
				lstrcpy(lpszFile, tchFileExpanded);
			}
		}
		return 1;
	}
	return 0;
}

int FindIniFile(void) {
	int bFound = 0;
	WCHAR tchTest[MAX_PATH];
	WCHAR tchModule[MAX_PATH];
	GetModuleFileName(NULL, tchModule, COUNTOF(tchModule));

	if (StrNotEmpty(szIniFile)) {
		if (StrEqual(szIniFile, L"*?")) {
			return 0;
		}
		if (!CheckIniFile(szIniFile, tchModule)) {
			ExpandEnvironmentStringsEx(szIniFile, COUNTOF(szIniFile));
			if (PathIsRelative(szIniFile)) {
				lstrcpy(tchTest, tchModule);
				PathRemoveFileSpec(tchTest);
				PathAppend(tchTest, szIniFile);
				lstrcpy(szIniFile, tchTest);
			}
		}
		return 1;
	}

	lstrcpy(tchTest, PathFindFileName(tchModule));
	PathRenameExtension(tchTest, L".ini");
	bFound = CheckIniFile(tchTest, tchModule);

	if (!bFound) {
		lstrcpy(tchTest, L"Notepad2.ini");
		bFound = CheckIniFile(tchTest, tchModule);
	}

	if (bFound) {
		// allow two redirections: administrator -> user -> custom
		if (CheckIniFileRedirect(tchTest, tchModule)) {
			CheckIniFileRedirect(tchTest, tchModule);
		}
		lstrcpy(szIniFile, tchTest);
	} else {
		lstrcpy(szIniFile, tchModule);
		PathRenameExtension(szIniFile, L".ini");
	}

	return 1;
}

int TestIniFile(void) {
	if (StrEqual(szIniFile, L"*?")) {
		lstrcpy(szIniFile2, L"");
		lstrcpy(szIniFile, L"");
		return 0;
	}

	if (PathIsDirectory(szIniFile) || *CharPrev(szIniFile, StrEnd(szIniFile)) == L'\\') {
		WCHAR wchModule[MAX_PATH];
		GetModuleFileName(NULL, wchModule, COUNTOF(wchModule));
		PathAppend(szIniFile, PathFindFileName(wchModule));
		PathRenameExtension(szIniFile, L".ini");
		if (!PathFileExists(szIniFile)) {
			lstrcpy(PathFindFileName(szIniFile), L"Notepad2.ini");
			if (!PathFileExists(szIniFile)) {
				lstrcpy(PathFindFileName(szIniFile), PathFindFileName(wchModule));
				PathRenameExtension(szIniFile, L".ini");
			}
		}
	}

	if (!PathFileExists(szIniFile) || PathIsDirectory(szIniFile)) {
		lstrcpy(szIniFile2, szIniFile);
		lstrcpy(szIniFile, L"");
		return 0;
	}

	return 1;
}

int CreateIniFile(void) {
	return CreateIniFileEx(szIniFile);
}

int CreateIniFileEx(LPCWSTR lpszIniFile) {
	if (StrNotEmpty(lpszIniFile)) {
		HANDLE hFile;
		WCHAR *pwchTail;

		if ((pwchTail = StrRChrW(lpszIniFile, NULL, L'\\')) != NULL) {
			*pwchTail = 0;
			SHCreateDirectoryEx(NULL, lpszIniFile, NULL);
			*pwchTail = L'\\';
		}

		hFile = CreateFile(lpszIniFile,
						   GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
						   NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		dwLastIOError = GetLastError();
		if (hFile != INVALID_HANDLE_VALUE) {
			if (GetFileSize(hFile, NULL) == 0) {
				DWORD dw;
				WriteFile(hFile, (LPCVOID)L"\xFEFF[Notepad2]\r\n", 26, &dw, NULL);
			}
			CloseHandle(hFile);
			return 1;
		}
	}
	return 0;
}

//=============================================================================
//
// UpdateToolbar()
//
//
#define EnableTool(id, b)		SendMessage(hwndToolbar, TB_ENABLEBUTTON, id, MAKELONG(((b) ? 1 : 0), 0))
#define CheckTool(id, b)		SendMessage(hwndToolbar, TB_CHECKBUTTON, id, MAKELONG(b, 0))

void UpdateToolbar(void) {
	int i;

	if (!bShowToolbar) {
		return;
	}

	EnableTool(IDT_FILE_ADDTOFAV, StrNotEmpty(szCurFile));

	EnableTool(IDT_EDIT_UNDO, SendMessage(hwndEdit, SCI_CANUNDO, 0, 0) /*&& !bReadOnly*/);
	EnableTool(IDT_EDIT_REDO, SendMessage(hwndEdit, SCI_CANREDO, 0, 0) /*&& !bReadOnly*/);

	i = !EditIsEmptySelection();
	EnableTool(IDT_EDIT_CUT, i /*&& !bReadOnly*/);
	i = (int)SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0);
	EnableTool(IDT_EDIT_COPY, i);
	EnableTool(IDT_EDIT_PASTE, SendMessage(hwndEdit, SCI_CANPASTE, 0, 0) /*&& !bReadOnly*/);

	EnableTool(IDT_EDIT_FIND, i);
	//EnableTool(IDT_EDIT_FINDNEXT, i);
	//EnableTool(IDT_EDIT_FINDPREV, i && StrNotEmptyA(efrData.szFind));
	EnableTool(IDT_EDIT_REPLACE, i /*&& !bReadOnly*/);
	EnableTool(IDT_EDIT_CLEAR, i /*&& !bReadOnly*/);

	EnableTool(IDT_VIEW_TOGGLEFOLDS, i && bShowCodeFolding);
	EnableTool(IDT_FILE_LAUNCH, i);

	CheckTool(IDT_VIEW_WORDWRAP, fWordWrap);
}

//=============================================================================
//
// UpdateStatusbar()
//
//
void UpdateStatusbar(void) {
	WCHAR tchLn[32];
	WCHAR tchLines[32];
	WCHAR tchCol[32];
	WCHAR tchCols[32];
	WCHAR tchCh[32];
	WCHAR tchChs[32];
	WCHAR tchSel[32];
	WCHAR tchSelCh[32];
	WCHAR tchDocPos[256];

	WCHAR tchBytes[32];
	WCHAR tchDocSize[32];

#ifdef BOOKMARK_EDITION
	WCHAR tchLinesSelected[32];
	WCHAR tchMatchesCount[32];
#endif

	if (!bShowStatusbar) {
		return;
	}

	int iPos =  SciCall_GetCurrentPos();

	const int iLn = SciCall_LineFromPosition(iPos) + 1;
	wsprintf(tchLn, L"%i", iLn);
	FormatNumberStr(tchLn);

	const int iLines = SciCall_GetLineCount();
	wsprintf(tchLines, L"%i", iLines);
	FormatNumberStr(tchLines);

	int iCol = SciCall_GetColumn(iPos) + 1;
	wsprintf(tchCol, L"%i", iCol);
	FormatNumberStr(tchCol);

	int iLineStart = SciCall_PositionFromLine(iLn - 1);
	int iLineEnd = SciCall_GetLineEndPosition(iLn - 1);
	iPos = SciCall_CountCharacters(iLineStart, iPos) + 1;
	iCol = SciCall_CountCharacters(iLineStart, iLineEnd);
	wsprintf(tchCh, L"%i", iPos);
	wsprintf(tchChs, L"%i", iCol);
	FormatNumberStr(tchCh);
	FormatNumberStr(tchChs);

	iCol = SciCall_GetColumn(iLineEnd);
	wsprintf(tchCols, L"%i", iCol);
	FormatNumberStr(tchCols);

	const int iSelStart = SciCall_GetSelectionStart();
	const int iSelEnd = SciCall_GetSelectionEnd();
	if (iSelStart == iSelEnd) {
		lstrcpy(tchSel, L"0");
		lstrcpy(tchSelCh, L"0");
	} else if (SC_SEL_RECTANGLE != SciCall_GetSelectionMode()) {
		int iSel = SciCall_GetSelText(NULL) - 1;
		wsprintf(tchSel, L"%i", iSel);
		FormatNumberStr(tchSel);
		iSel = SciCall_CountCharacters(iSelStart, iSelEnd);
		wsprintf(tchSelCh, L"%i", iSel);
		FormatNumberStr(tchSelCh);
	} else {
		lstrcpy(tchSel, L"--");
		lstrcpy(tchSelCh, L"--");
	}

#ifdef BOOKMARK_EDITION
	// Print number of lines selected lines in statusbar
	if (iSelStart == iSelEnd) {
		lstrcpy(tchLinesSelected, L"0");
		lstrcpy(tchMatchesCount, L"0");
	} else {
		iLineStart = SciCall_LineFromPosition(iSelStart);
		iLineEnd = SciCall_LineFromPosition(iSelEnd);
		int iStartOfLinePos = SciCall_PositionFromLine(iLineEnd);
		int iLinesSelected = iLineEnd - iLineStart;
		if (iSelStart != iSelEnd && iStartOfLinePos != iSelEnd) {
			iLinesSelected += 1;
		}
		wsprintf(tchLinesSelected, L"%i", iLinesSelected);
		FormatNumberStr(tchLinesSelected);
		wsprintf(tchMatchesCount, L"%i", iMatchesCount);
		FormatNumberStr(tchMatchesCount);
	}

	FormatString(tchDocPos, COUNTOF(tchDocPos), IDS_DOCPOS, tchLn, tchLines,
				 tchCol, tchCols, tchCh, tchChs, tchSelCh, tchSel, tchLinesSelected, tchMatchesCount);

#else

	FormatString(tchDocPos, COUNTOF(tchDocPos), IDS_DOCPOS, tchLn, tchLines, tchCol, tchCols, tchCh, tchChs, tchSelCh, tchSel);
#endif

	const int iBytes = SciCall_GetLength();
	StrFormatByteSize(iBytes, tchBytes, COUNTOF(tchBytes));
	FormatString(tchDocSize, COUNTOF(tchDocSize), IDS_DOCSIZE, tchBytes);

	StatusSetText(hwndStatus, STATUS_DOCPOS, tchDocPos);
	if (cachedStatusItem.lexerNameChanged) {
		StatusSetText(hwndStatus, STATUS_LEXER, cachedStatusItem.pszLexerName);
		cachedStatusItem.lexerNameChanged = FALSE;
	}
	if (cachedStatusItem.encodingChanged) {
		StatusSetText(hwndStatus, STATUS_CODEPAGE, mEncoding[iEncoding].wchLabel);
		cachedStatusItem.encodingChanged = FALSE;
	}
	if (cachedStatusItem.eolModeChanged) {
		StatusSetText(hwndStatus, STATUS_EOLMODE, cachedStatusItem.pszEOLMode);
		cachedStatusItem.eolModeChanged = FALSE;
	}
	if (cachedStatusItem.ovrModeChanged) {
		StatusSetText(hwndStatus, STATUS_OVRMODE, cachedStatusItem.pszOvrMode);
		cachedStatusItem.ovrModeChanged = FALSE;
	}
	StatusSetText(hwndStatus, STATUS_DOCSIZE, tchDocSize);
	if (cachedStatusItem.zoomChanged) {
		StatusSetText(hwndStatus, STATUS_DOCZOOM, cachedStatusItem.tchZoom);
		cachedStatusItem.zoomChanged = FALSE;
	}
}

//=============================================================================
//
// UpdateLineNumberWidth()
//
//
void UpdateLineNumberWidth(void) {
	if (bShowLineNumbers) {
		char tchLines[32];

		const int iLines = SciCall_GetLineCount();
		wsprintfA(tchLines, "_%i_", iLines);

		const int iLineMarginWidthNow = SciCall_GetMarginWidth(MARGIN_LINE_NUMBER);
		const int iLineMarginWidthFit = SciCall_TextWidth(STYLE_LINENUMBER, tchLines);

		if (iLineMarginWidthNow != iLineMarginWidthFit) {
#if !NP2_DEBUG_FOLD_LEVEL
			SciCall_SetMarginWidth(MARGIN_LINE_NUMBER, iLineMarginWidthFit);

#else
			SciCall_SetMarginWidth(MARGIN_LINE_NUMBER, RoundToCurrentDPI(100));
#endif
		}
	} else {
		SciCall_SetMarginWidth(MARGIN_LINE_NUMBER, 0);
	}
}

void ToggleFullScreenMode(void) {
	if (bInFullScreenMode) {
	} else {
	}
}

//=============================================================================
//
//	FileIO()
//
//
BOOL FileIO(BOOL fLoad, LPWSTR psz, BOOL bNoEncDetect, int *ienc, int *ieol,
			BOOL *pbUnicodeErr, BOOL *pbFileTooBig,
			BOOL *pbCancelDataLoss, BOOL bSaveCopy) {
	WCHAR tch[MAX_PATH + 40];
	BOOL fSuccess;
	DWORD dwFileAttributes;

	BeginWaitCursor();

	FormatString(tch, COUNTOF(tch), (fLoad) ? IDS_LOADFILE : IDS_SAVEFILE, PathFindFileName(psz));

	StatusSetText(hwndStatus, STATUS_HELP, tch);
	StatusSetSimple(hwndStatus, TRUE);

	InvalidateRect(hwndStatus, NULL, TRUE);
	UpdateWindow(hwndStatus);

	if (fLoad) {
		fSuccess = EditLoadFile(hwndEdit, psz, bNoEncDetect, ienc, ieol, pbUnicodeErr, pbFileTooBig);
	} else {
		fSuccess = EditSaveFile(hwndEdit, psz, *ienc, pbCancelDataLoss, bSaveCopy);
	}

	dwFileAttributes = GetFileAttributes(psz);
	bReadOnly = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);

	StatusSetSimple(hwndStatus, FALSE);

	EndWaitCursor();

	return fSuccess;
}

//=============================================================================
//
// FileLoad()
//
//
BOOL FileLoad(BOOL bDontSave, BOOL bNew, BOOL bReload, BOOL bNoEncDetect, LPCWSTR lpszFile) {
	WCHAR tch[MAX_PATH] = L"";
	WCHAR szFileName[MAX_PATH] = L"";
	BOOL fSuccess = FALSE;
	BOOL bUnicodeErr = FALSE;
	BOOL bFileTooBig = FALSE;
	int line = 0, col = 0;
	int keepTitleExcerpt = fKeepTitleExcerpt;
	int lexerSpecified = flagLexerSpecified;

	if (!bNew && StrNotEmpty(lpszFile)) {
		lstrcpy(tch, lpszFile);
		if (lpszFile == szCurFile || StrCaseEqual(lpszFile, szCurFile)) {
			Sci_Position pos = SciCall_GetCurrentPos();
			line = SciCall_LineFromPosition(pos) + 1;
			col = SciCall_GetColumn(pos) + 1;
			keepTitleExcerpt = 1;
			lexerSpecified = 1;
		}
		fSuccess = TRUE;
	}
	if (!bDontSave) {
		if (!FileSave(FALSE, TRUE, FALSE, FALSE)) {
			return FALSE;
		}
	}

	if (bNew) {
		lstrcpy(szCurFile, L"");
		SetDlgItemText(hwndMain, IDC_FILENAME, szCurFile);
		SetDlgItemInt(hwndMain, IDC_REUSELOCK, GetTickCount(), FALSE);
		if (!keepTitleExcerpt) {
			lstrcpy(szTitleExcerpt, L"");
		}
		FileVars_Init(NULL, 0, &fvCurFile);
		EditSetNewText(hwndEdit, "", 0);
		Style_SetLexer(hwndEdit, NULL);
		bModified = FALSE;
		bReadOnly = FALSE;
		iEOLMode = iLineEndings[iDefaultEOLMode];
		SendMessage(hwndEdit, SCI_SETEOLMODE, iLineEndings[iDefaultEOLMode], 0);
		iEncoding = iDefaultEncoding;
		iOriginalEncoding = iDefaultEncoding;
		SendMessage(hwndEdit, SCI_SETCODEPAGE, (iDefaultEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8, 0);
		UpdateStatusBarCache(STATUS_CODEPAGE);
		UpdateStatusBarCache(STATUS_EOLMODE);
		UpdateWindowTitle();

		// Terminate file watching
		if (bResetFileWatching) {
			iFileWatchingMode = 0;
		}
		InstallFileWatching(NULL);

		return TRUE;
	}

	if (!fSuccess) {
		if (!OpenFileDlg(hwndMain, tch, COUNTOF(tch), NULL)) {
			return FALSE;
		}
	}
	fSuccess = FALSE;

	ExpandEnvironmentStringsEx(tch, COUNTOF(tch));

	if (PathIsRelative(tch)) {
		StrCpyN(szFileName, g_wchWorkingDirectory, COUNTOF(szFileName));
		PathAppend(szFileName, tch);
		if (!PathFileExists(szFileName)) {
			WCHAR wchFullPath[MAX_PATH];
			if (SearchPath(NULL, tch, NULL, COUNTOF(wchFullPath), wchFullPath, NULL)) {
				lstrcpy(szFileName, wchFullPath);
			}
		}
	} else {
		lstrcpy(szFileName, tch);
	}

	PathCanonicalizeEx(szFileName);
	GetLongPathNameEx(szFileName, COUNTOF(szFileName));

	if (PathIsLnkFile(szFileName)) {
		PathGetLnkPath(szFileName, szFileName, COUNTOF(szFileName));
	}

	// Ask to create a new file...
	if (!bReload && !PathFileExists(szFileName)) {
		if (flagQuietCreate || MsgBox(MBYESNO, IDS_ASK_CREATE, szFileName) == IDYES) {
			HANDLE hFile = CreateFile(szFileName,
									  GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
									  NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			dwLastIOError = GetLastError();
			if (hFile != INVALID_HANDLE_VALUE) {
				fSuccess = TRUE;
				CloseHandle(hFile);
				FileVars_Init(NULL, 0, &fvCurFile);
				EditSetNewText(hwndEdit, "", 0);
				Style_SetLexer(hwndEdit, NULL);
				iEOLMode = iLineEndings[iDefaultEOLMode];
				SendMessage(hwndEdit, SCI_SETEOLMODE, iLineEndings[iDefaultEOLMode], 0);
				if (iSrcEncoding != -1) {
					iEncoding = iSrcEncoding;
					iOriginalEncoding = iSrcEncoding;
				} else {
					iEncoding = iDefaultEncoding;
					iOriginalEncoding = iDefaultEncoding;
				}
				SendMessage(hwndEdit, SCI_SETCODEPAGE, (iEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8, 0);
				bReadOnly = FALSE;
			}
		} else {
			return FALSE;
		}
	} else {
		fSuccess = FileIO(TRUE, szFileName, bNoEncDetect, &iEncoding, &iEOLMode, &bUnicodeErr, &bFileTooBig, NULL, FALSE);
	}

	if (fSuccess) {
		lstrcpy(szCurFile, szFileName);
		SetDlgItemText(hwndMain, IDC_FILENAME, szCurFile);
		SetDlgItemInt(hwndMain, IDC_REUSELOCK, GetTickCount(), FALSE);
		if (!keepTitleExcerpt) {
			lstrcpy(szTitleExcerpt, L"");
		}
		iOriginalEncoding = iEncoding;
		bModified = FALSE;
		UpdateStatusBarCache(STATUS_CODEPAGE);
		UpdateStatusBarCache(STATUS_EOLMODE);
		if (!lexerSpecified) { // flagLexerSpecified will be cleared
			np2LexLangIndex = 0;
			Style_SetLexerFromFile(hwndEdit, szCurFile);
		}
		if (!lexerSpecified) {
			UpdateLineNumberWidth();
		}
		//bReadOnly = FALSE;
		SendMessage(hwndEdit, SCI_SETEOLMODE, iEOLMode, 0);
		MRU_AddFile(pFileMRU, szFileName, flagRelativeFileMRU, flagPortableMyDocs);
		if (flagUseSystemMRU == 2) {
			SHAddToRecentDocs(SHARD_PATHW, szFileName);
		}

		// Install watching of the current file
		if (!bReload && bResetFileWatching) {
			iFileWatchingMode = 0;
		}
		InstallFileWatching(szCurFile);

		if (line > 1 || col > 1) {
			EditJumpTo(hwndEdit, line, col);
			EditEnsureSelectionVisible(hwndEdit);
		}
#if NP2_ENABLE_DOT_LOG_FEATURE
		// the .LOG feature ...
		if (SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) >= 4) {
			char tchLog[5] = "";
			SendMessage(hwndEdit, SCI_GETTEXT, 5, (LPARAM)tchLog);
			if (StrEqual(tchLog, ".LOG")) {
				EditJumpTo(hwndEdit, -1, 0);
				SendMessage(hwndEdit, SCI_BEGINUNDOACTION, 0, 0);
				SendMessage(hwndEdit, SCI_NEWLINE, 0, 0);
				SendMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_EDIT_INSERT_SHORTDATE, 1), 0);
				EditJumpTo(hwndEdit, -1, 0);
				SendMessage(hwndEdit, SCI_NEWLINE, 0, 0);
				SendMessage(hwndEdit, SCI_ENDUNDOACTION, 0, 0);
				EditJumpTo(hwndEdit, -1, 0);
				EditEnsureSelectionVisible(hwndEdit);
			}
		}
#endif

		UpdateStatusbar();
		UpdateWindowTitle();
		// Show warning: Unicode file loaded as ANSI
		if (bUnicodeErr) {
			MsgBox(MBWARN, IDS_ERR_UNICODE);
		}
	} else if (!bFileTooBig) {
		MsgBox(MBWARN, IDS_ERR_LOADFILE, szFileName);
	}

	return fSuccess;
}

//=============================================================================
//
// FileSave()
//
//
BOOL FileSave(BOOL bSaveAlways, BOOL bAsk, BOOL bSaveAs, BOOL bSaveCopy) {
	WCHAR tchFile[MAX_PATH];
	BOOL fSuccess = FALSE;
	BOOL bCancelDataLoss = FALSE;
	BOOL Untitled = StrIsEmpty(szCurFile);
	BOOL bIsEmptyNewFile = FALSE;

	if (Untitled) {
		int cchText = (int)SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0);
		if (cchText == 0) {
			bIsEmptyNewFile = TRUE;
		} else if (cchText < 1023) {
			char tchText[2048];
			SendMessage(hwndEdit, SCI_GETTEXT, (WPARAM)2047, (LPARAM)tchText);
			StrTrimA(tchText, " \t\n\r");
			if (StrIsEmptyA(tchText)) {
				bIsEmptyNewFile = TRUE;
			}
		}
	}

	if (!bSaveAlways && ((!bModified && iEncoding == iOriginalEncoding) || bIsEmptyNewFile) && !bSaveAs) {
		return TRUE;
	}

	if (bAsk) {
		// File or "Untitled" ...
		WCHAR tch[MAX_PATH];
		if (!Untitled) {
			lstrcpy(tch, szCurFile);
		} else {
			GetString(IDS_UNTITLED, tch, COUNTOF(tch));
		}

		switch (MsgBox(MBYESNOCANCEL, IDS_ASK_SAVE, tch)) {
		case IDCANCEL:
			return FALSE;
		case IDNO:
			return TRUE;
		}
	}

	// Read only...
	if (!bSaveAs && !bSaveCopy && !Untitled) {
		DWORD dwFileAttributes = GetFileAttributes(szCurFile);
		bReadOnly = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);
		if (bReadOnly) {
			UpdateWindowTitle();
			if (MsgBox(MBYESNOWARN, IDS_READONLY_SAVE, szCurFile) == IDYES) {
				bSaveAs = TRUE;
			} else {
				return FALSE;
			}
		}
		if (!bSaveAs) {
			fSuccess = FileIO(FALSE, szCurFile, FALSE, &iEncoding, &iEOLMode, NULL, NULL, &bCancelDataLoss, FALSE);
			if (!fSuccess) {
				bSaveAs = TRUE;
			}
		}
	}

	// Save As...
	if (bSaveAs || bSaveCopy || Untitled) {
		WCHAR tchInitialDir[MAX_PATH] = L"";
		if (bSaveCopy && StrNotEmpty(tchLastSaveCopyDir)) {
			lstrcpy(tchInitialDir, tchLastSaveCopyDir);
			lstrcpy(tchFile, tchLastSaveCopyDir);
			PathAppend(tchFile, PathFindFileName(szCurFile));
		} else {
			lstrcpy(tchFile, szCurFile);
		}

		if (SaveFileDlg(hwndMain, tchFile, COUNTOF(tchFile), tchInitialDir)) {
			fSuccess = FileIO(FALSE, tchFile, FALSE, &iEncoding, &iEOLMode, NULL, NULL, &bCancelDataLoss, bSaveCopy);
			if (fSuccess) {
				if (!bSaveCopy) {
					lstrcpy(szCurFile, tchFile);
					SetDlgItemText(hwndMain, IDC_FILENAME, szCurFile);
					SetDlgItemInt(hwndMain, IDC_REUSELOCK, GetTickCount(), FALSE);
					if (!fKeepTitleExcerpt) {
						lstrcpy(szTitleExcerpt, L"");
					}
					Style_SetLexerFromFile(hwndEdit, szCurFile);
				} else {
					lstrcpy(tchLastSaveCopyDir, tchFile);
					PathRemoveFileSpec(tchLastSaveCopyDir);
				}
			}
		} else {
			return FALSE;
		}
	} else if (!fSuccess) {
		fSuccess = FileIO(FALSE, szCurFile, FALSE, &iEncoding, &iEOLMode, NULL, NULL, &bCancelDataLoss, FALSE);
	}

	if (fSuccess) {
		if (!bSaveCopy) {
			bModified = FALSE;
			iOriginalEncoding = iEncoding;
			MRU_AddFile(pFileMRU, szCurFile, flagRelativeFileMRU, flagPortableMyDocs);
			if (flagUseSystemMRU == 2) {
				SHAddToRecentDocs(SHARD_PATHW, szCurFile);
			}
			if (flagRelaunchElevated == 2 && bSaveAs && iPathNameFormat == 0) {
				iPathNameFormat = 1;
			}
			UpdateWindowTitle();

			// Install watching of the current file
			if (bSaveAs && bResetFileWatching) {
				iFileWatchingMode = 0;
			}
			InstallFileWatching(szCurFile);
		}
	} else if (!bCancelDataLoss) {
		if (StrNotEmpty(szCurFile) != 0) {
			lstrcpy(tchFile, szCurFile);
		}

		UpdateWindowTitle();
		MsgBox(MBWARN, IDS_ERR_SAVEFILE, tchFile);
	}

	return fSuccess;
}

//=============================================================================
//
// OpenFileDlg()
//
//
BOOL OpenFileDlg(HWND hwnd, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) {
	OPENFILENAME ofn;
	WCHAR szFile[MAX_PATH];
	WCHAR szFilter[NUMLEXERS * 1024];
	WCHAR tchInitialDir[MAX_PATH] = L"";

	lstrcpy(szFile, L"");
	Style_GetOpenDlgFilterStr(szFilter, COUNTOF(szFilter));

	if (!lpstrInitialDir) {
		if (StrNotEmpty(szCurFile)) {
			lstrcpy(tchInitialDir, szCurFile);
			PathRemoveFileSpec(tchInitialDir);
		} else if (StrNotEmpty(tchDefaultDir)) {
			ExpandEnvironmentStrings(tchDefaultDir, tchInitialDir, COUNTOF(tchInitialDir));
			if (PathIsRelative(tchInitialDir)) {
				WCHAR tchModule[MAX_PATH];
				GetModuleFileName(NULL, tchModule, COUNTOF(tchModule));
				PathRemoveFileSpec(tchModule);
				PathAppend(tchModule, tchInitialDir);
				PathCanonicalize(tchInitialDir, tchModule);
			}
		} else {
			lstrcpy(tchInitialDir, g_wchWorkingDirectory);
		}
	}

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFile;
	ofn.lpstrInitialDir = (lpstrInitialDir) ? lpstrInitialDir : tchInitialDir;
	ofn.nMaxFile = COUNTOF(szFile);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | /* OFN_NOCHANGEDIR |*/
				OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST |
				OFN_SHAREAWARE /*| OFN_NODEREFERENCELINKS*/;
	ofn.lpstrDefExt = StrNotEmpty(tchDefaultExtension) ? tchDefaultExtension : NULL;

	if (GetOpenFileName(&ofn)) {
		lstrcpyn(lpstrFile, szFile, cchFile);
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// SaveFileDlg()
//
//
BOOL SaveFileDlg(HWND hwnd, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) {
	OPENFILENAME ofn;
	WCHAR szNewFile[MAX_PATH];
	WCHAR szFilter[NUMLEXERS * 1024];
	WCHAR tchInitialDir[MAX_PATH] = L"";

	lstrcpy(szNewFile, lpstrFile);
	Style_GetOpenDlgFilterStr(szFilter, COUNTOF(szFilter));

	if (StrNotEmpty(lpstrInitialDir)) {
		lstrcpy(tchInitialDir, lpstrInitialDir);
	} else if (StrNotEmpty(szCurFile)) {
		lstrcpy(tchInitialDir, szCurFile);
		PathRemoveFileSpec(tchInitialDir);
	} else if (StrNotEmpty(tchDefaultDir)) {
		ExpandEnvironmentStrings(tchDefaultDir, tchInitialDir, COUNTOF(tchInitialDir));
		if (PathIsRelative(tchInitialDir)) {
			WCHAR tchModule[MAX_PATH];
			GetModuleFileName(NULL, tchModule, COUNTOF(tchModule));
			PathRemoveFileSpec(tchModule);
			PathAppend(tchModule, tchInitialDir);
			PathCanonicalize(tchInitialDir, tchModule);
		}
	} else {
		lstrcpy(tchInitialDir, g_wchWorkingDirectory);
	}

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szNewFile;
	ofn.lpstrInitialDir = tchInitialDir;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY /*| OFN_NOCHANGEDIR*/ |
				/*OFN_NODEREFERENCELINKS |*/ OFN_OVERWRITEPROMPT |
				OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = StrNotEmpty(tchDefaultExtension) ? tchDefaultExtension : NULL;

	if (GetSaveFileName(&ofn)) {
		lstrcpyn(lpstrFile, szNewFile, cchFile);
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
*
* ActivatePrevInst()
*
* Tries to find and activate an already open Notepad2 Window
*
*
******************************************************************************/
BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam) {
	BOOL bContinue = TRUE;
	WCHAR szClassName[64];

	if (GetClassName(hwnd, szClassName, COUNTOF(szClassName))) {
		if (StrCaseEqual(szClassName, wchWndClass)) {
			DWORD dwReuseLock = GetDlgItemInt(hwnd, IDC_REUSELOCK, NULL, FALSE);
			if (GetTickCount() - dwReuseLock >= REUSEWINDOWLOCKTIMEOUT) {
				*(HWND *)lParam = hwnd;
				if (IsWindowEnabled(hwnd)) {
					bContinue = FALSE;
				}
			}
		}
	}
	return bContinue;
}

BOOL CALLBACK EnumWndProc2(HWND hwnd, LPARAM lParam) {
	BOOL bContinue = TRUE;
	WCHAR szClassName[64];

	if (GetClassName(hwnd, szClassName, COUNTOF(szClassName))) {
		if (StrCaseEqual(szClassName, wchWndClass)) {
			DWORD dwReuseLock = GetDlgItemInt(hwnd, IDC_REUSELOCK, NULL, FALSE);
			if (GetTickCount() - dwReuseLock >= REUSEWINDOWLOCKTIMEOUT) {
				WCHAR tchFileName[MAX_PATH] = L"";

				if (IsWindowEnabled(hwnd)) {
					bContinue = FALSE;
				}

				GetDlgItemText(hwnd, IDC_FILENAME, tchFileName, COUNTOF(tchFileName));
				if (StrCaseEqual(tchFileName, lpFileArg)) {
					*(HWND *)lParam = hwnd;
				} else {
					bContinue = TRUE;
				}
			}
		}
	}
	return bContinue;
}

BOOL ActivatePrevInst(void) {
	HWND hwnd = NULL;
	COPYDATASTRUCT cds;

	if ((flagNoReuseWindow && !flagSingleFileInstance) || flagStartAsTrayIcon || flagNewFromClipboard || flagPasteBoard) {
		return FALSE;
	}

	if (flagSingleFileInstance && lpFileArg) {
		// Search working directory from second instance, first!
		// lpFileArg is at least MAX_PATH+2 bytes
		WCHAR tchTmp[MAX_PATH];

		ExpandEnvironmentStringsEx(lpFileArg, (DWORD)NP2HeapSize(lpFileArg) / sizeof(WCHAR));

		if (PathIsRelative(lpFileArg)) {
			StrCpyN(tchTmp, g_wchWorkingDirectory, COUNTOF(tchTmp));
			PathAppend(tchTmp, lpFileArg);
			if (PathFileExists(tchTmp)) {
				lstrcpy(lpFileArg, tchTmp);
			} else {
				if (SearchPath(NULL, lpFileArg, NULL, COUNTOF(tchTmp), tchTmp, NULL)) {
					lstrcpy(lpFileArg, tchTmp);
				} else {
					StrCpyN(tchTmp, g_wchWorkingDirectory, COUNTOF(tchTmp));
					PathAppend(tchTmp, lpFileArg);
					lstrcpy(lpFileArg, tchTmp);
				}
			}
		} else if (SearchPath(NULL, lpFileArg, NULL, COUNTOF(tchTmp), tchTmp, NULL)) {
			lstrcpy(lpFileArg, tchTmp);
		}

		GetLongPathNameEx(lpFileArg, MAX_PATH);

		EnumWindows(EnumWndProc2, (LPARAM)&hwnd);

		if (hwnd != NULL) {
			// Enabled
			if (IsWindowEnabled(hwnd)) {
				LPNP2PARAMS params;
				DWORD cb = sizeof(NP2PARAMS);

				// Make sure the previous window won't pop up a change notification message
				//SendMessage(hwnd, APPM_CHANGENOTIFYCLEAR, 0, 0);

				if (IsIconic(hwnd)) {
					ShowWindowAsync(hwnd, SW_RESTORE);
				}

				if (!IsWindowVisible(hwnd)) {
					SendMessage(hwnd, APPM_TRAYMESSAGE, 0, WM_LBUTTONDBLCLK);
					SendMessage(hwnd, APPM_TRAYMESSAGE, 0, WM_LBUTTONUP);
				}

				SetForegroundWindow(hwnd);

				if (lpSchemeArg) {
					cb += (lstrlen(lpSchemeArg) + 1) * sizeof(WCHAR);
				}

				params = GlobalAlloc(GPTR, cb);
				params->flagFileSpecified = FALSE;
				params->flagChangeNotify = 0;
				params->flagQuietCreate = FALSE;
				params->flagLexerSpecified = flagLexerSpecified;
				if (flagLexerSpecified && lpSchemeArg) {
					lstrcpy(StrEnd(&params->wchData) + 1, lpSchemeArg);
					params->iInitialLexer = -1;
				} else {
					params->iInitialLexer = iInitialLexer;
				}
				params->flagJumpTo = flagJumpTo;
				params->iInitialLine = iInitialLine;
				params->iInitialColumn = iInitialColumn;

				params->iSrcEncoding = (lpEncodingArg) ? Encoding_MatchW(lpEncodingArg) : -1;
				params->flagSetEncoding = flagSetEncoding;
				params->flagSetEOLMode = flagSetEOLMode;
				params->flagTitleExcerpt = 0;

				cds.dwData = DATA_NOTEPAD2_PARAMS;
				cds.cbData = (DWORD)GlobalSize(params);
				cds.lpData = params;

				SendMessage(hwnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
				GlobalFree(params);

				return TRUE;
			}

			// Ask...
			if (IDYES == MsgBox(MBYESNO, IDS_ERR_PREVWINDISABLED)) {
				return FALSE;
			}
			return TRUE;
		}
	}

	if (flagNoReuseWindow) {
		return FALSE;
	}

	hwnd = NULL;
	EnumWindows(EnumWndProc, (LPARAM)&hwnd);

	// Found a window
	if (hwnd != NULL) {
		// Enabled
		if (IsWindowEnabled(hwnd)) {
			// Make sure the previous window won't pop up a change notification message
			//SendMessage(hwnd, APPM_CHANGENOTIFYCLEAR, 0, 0);

			if (IsIconic(hwnd)) {
				ShowWindowAsync(hwnd, SW_RESTORE);
			}

			if (!IsWindowVisible(hwnd)) {
				SendMessage(hwnd, APPM_TRAYMESSAGE, 0, WM_LBUTTONDBLCLK);
				SendMessage(hwnd, APPM_TRAYMESSAGE, 0, WM_LBUTTONUP);
			}

			SetForegroundWindow(hwnd);

			if (lpFileArg) {
				// Search working directory from second instance, first!
				// lpFileArg is at least MAX_PATH+2 bytes
				WCHAR tchTmp[MAX_PATH];
				LPNP2PARAMS params;
				DWORD cb = sizeof(NP2PARAMS);
				int cchTitleExcerpt;

				ExpandEnvironmentStringsEx(lpFileArg, (DWORD)NP2HeapSize(lpFileArg) / sizeof(WCHAR));

				if (PathIsRelative(lpFileArg)) {
					StrCpyN(tchTmp, g_wchWorkingDirectory, COUNTOF(tchTmp));
					PathAppend(tchTmp, lpFileArg);
					if (PathFileExists(tchTmp)) {
						lstrcpy(lpFileArg, tchTmp);
					} else {
						if (SearchPath(NULL, lpFileArg, NULL, COUNTOF(tchTmp), tchTmp, NULL)) {
							lstrcpy(lpFileArg, tchTmp);
						}
					}
				} else if (SearchPath(NULL, lpFileArg, NULL, COUNTOF(tchTmp), tchTmp, NULL)) {
					lstrcpy(lpFileArg, tchTmp);
				}

				cb += (lstrlen(lpFileArg) + 1) * sizeof(WCHAR);

				if (lpSchemeArg) {
					cb += (lstrlen(lpSchemeArg) + 1) * sizeof(WCHAR);
				}

				cchTitleExcerpt = lstrlen(szTitleExcerpt);
				if (cchTitleExcerpt) {
					cb += (cchTitleExcerpt + 1) * sizeof(WCHAR);
				}

				params = GlobalAlloc(GPTR, cb);
				params->flagFileSpecified = TRUE;
				lstrcpy(&params->wchData, lpFileArg);
				params->flagChangeNotify = flagChangeNotify;
				params->flagQuietCreate = flagQuietCreate;
				params->flagLexerSpecified = flagLexerSpecified;
				if (flagLexerSpecified && lpSchemeArg) {
					lstrcpy(StrEnd(&params->wchData) + 1, lpSchemeArg);
					params->iInitialLexer = -1;
				} else {
					params->iInitialLexer = iInitialLexer;
				}
				params->flagJumpTo = flagJumpTo;
				params->iInitialLine = iInitialLine;
				params->iInitialColumn = iInitialColumn;

				params->iSrcEncoding = (lpEncodingArg) ? Encoding_MatchW(lpEncodingArg) : -1;
				params->flagSetEncoding = flagSetEncoding;
				params->flagSetEOLMode = flagSetEOLMode;

				if (cchTitleExcerpt) {
					lstrcpy(StrEnd(&params->wchData) + 1, szTitleExcerpt);
					params->flagTitleExcerpt = 1;
				} else {
					params->flagTitleExcerpt = 0;
				}

				cds.dwData = DATA_NOTEPAD2_PARAMS;
				cds.cbData = (DWORD)GlobalSize(params);
				cds.lpData = params;

				SendMessage(hwnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
				GlobalFree(params);
				NP2HeapFree(lpFileArg);
			}
			return TRUE;
		}

		// Ask...
		if (IDYES == MsgBox(MBYESNO, IDS_ERR_PREVWINDISABLED)) {
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// RelaunchMultiInst()
//
//
BOOL RelaunchMultiInst(void) {
	if (flagMultiFileArg == 2 && cFileList > 1) {
		WCHAR *pwch;
		int i = 0;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		LPWSTR lpCmdLineNew = StrDup(GetCommandLine());
		size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLineNew) + 1);
		LPWSTR lp1 = NP2HeapAlloc(cmdSize);
		LPWSTR lp2 = NP2HeapAlloc(cmdSize);

		StrTab2Space(lpCmdLineNew);
		lstrcpy(lpCmdLineNew + cchiFileList, L"");

		pwch = CharPrev(lpCmdLineNew, StrEnd(lpCmdLineNew));
		while (*pwch == L' ' || *pwch == L'-' || *pwch == L'+') {
			*pwch = L' ';
			pwch = CharPrev(lpCmdLineNew, pwch);
			if (i++ > 1) {
				cchiFileList--;
			}
		}

		for (i = 0; i < cFileList; i++) {
			lstrcpy(lpCmdLineNew + cchiFileList, L" /n - ");
			lstrcat(lpCmdLineNew, lpFileList[i]);
			LocalFree(lpFileList[i]);

			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

			if (CreateProcess(NULL, lpCmdLineNew, NULL, NULL, FALSE, 0, NULL, g_wchWorkingDirectory, &si, &pi)) {
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		}

		LocalFree(lpCmdLineNew);
		NP2HeapFree(lp1);
		NP2HeapFree(lp2);
		NP2HeapFree(lpFileArg);

		return TRUE;
	}

	for (int i = 0; i < cFileList; i++) {
		LocalFree(lpFileList[i]);
	}
	return FALSE;
}

void GetRelaunchParameters(LPWSTR szParameters, LPCWSTR lpszFile, BOOL newWind, BOOL emptyWind) {
	MONITORINFO mi;
	HMONITOR hMonitor;
	WINDOWPLACEMENT wndpl;
	int x, y, cx, cy, imax;
	WCHAR tch[64];

	wsprintf(tch, L"-appid=\"%s\"", g_wchAppUserModelID);
	lstrcpy(szParameters, tch);

	wsprintf(tch, L" -sysmru=%i", (flagUseSystemMRU == 2));
	lstrcat(szParameters, tch);

	lstrcat(szParameters, L" -f");
	if (StrNotEmpty(szIniFile)) {
		lstrcat(szParameters, L" \"");
		lstrcat(szParameters, szIniFile);
		lstrcat(szParameters, L"\"");
	} else {
		lstrcat(szParameters, L"0");
	}

	if (newWind) {
		lstrcat(szParameters, L" -n");
	}

	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwndMain, &wndpl);

	hMonitor = MonitorFromRect(&wndpl.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	// offset new window position +10/+10
	x  = wndpl.rcNormalPosition.left + (newWind? 10 : 0);
	y  = wndpl.rcNormalPosition.top	+ (newWind? 10 : 0);
	cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
	cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

	// check if window fits monitor
	if ((x + cx) > mi.rcWork.right || (y + cy) > mi.rcWork.bottom) {
		x = mi.rcMonitor.left;
		y = mi.rcMonitor.top;
	}

	imax = IsZoomed(hwndMain);

	wsprintf(tch, L" -pos %i,%i,%i,%i,%i", x, y, cx, cy, imax);
	lstrcat(szParameters, tch);

	if (!emptyWind && StrNotEmpty(lpszFile)) {
		WCHAR szFileName[MAX_PATH + 4];
		Sci_Position pos = SciCall_GetCurrentPos();

		// file encoding
		switch (iEncoding) {
		case CPI_DEFAULT:
			lstrcat(szParameters, L" -ansi");
			break;

		case CPI_UNICODE:
		case CPI_UNICODEBOM:
			lstrcat(szParameters, L" -utf16le");
			break;

		case CPI_UNICODEBE:
		case CPI_UNICODEBEBOM:
			lstrcat(szParameters, L" -utf16be");
			break;

		case CPI_UTF8:
			lstrcat(szParameters, L" -utf8");
			break;

		case CPI_UTF8SIGN:
			lstrcat(szParameters, L" -utf8sig");
			break;

		default: {
			const char *enc = mEncoding[iEncoding].pszParseNames;
			char *sep = StrChrA(enc, ',');
			ZeroMemory(tch, sizeof(tch));
			MultiByteToWideChar(CP_UTF8, 0, enc, (int)(sep - enc), tch, COUNTOF(tch));
			lstrcat(szParameters, L" -e \"");
			lstrcat(szParameters, tch);
			lstrcat(szParameters, L"\"");
		}
		}

		// scheme
		switch (pLexCurrent->rid) {
		case NP2LEX_DEFAULT:
			lstrcat(szParameters, L" -d");
			break;

		case NP2LEX_HTML:
			lstrcat(szParameters, L" -h");
			break;

		case NP2LEX_XML:
			lstrcat(szParameters, L" -x");
			break;

		default:
			lstrcat(szParameters, L" -s \"");
			lstrcat(szParameters, pLexCurrent->pszName);
			lstrcat(szParameters, L"\"");
			break;
		}

		// position
		if (pos > 0) {
			x = SciCall_LineFromPosition(pos) + 1;
			y = SciCall_GetColumn(pos) + 1;
			wsprintf(tch, L" -g %i,%i", x, y);
			lstrcat(szParameters, tch);
		}

		lstrcpy(szFileName, lpszFile);
		PathQuoteSpaces(szFileName);
		lstrcat(szParameters, L" ");
		lstrcat(szParameters, szFileName);
	}
}

//=============================================================================
//
// RelaunchElevated()
//
//
BOOL RelaunchElevated(void) {
	if (!IsVistaAndAbove() || fIsElevated || !flagRelaunchElevated || flagDisplayHelp) {
		return FALSE;
	}
	{
		LPWSTR lpArg1, lpArg2;
		BOOL exit = TRUE;

		if (flagRelaunchElevated == 2) {
			WCHAR tchFile[MAX_PATH];
			lstrcpy(tchFile, szCurFile);
			if (!FileSave(FALSE, TRUE, FALSE, FALSE)) {
				return FALSE;
			}

			exit = StrCaseEqual(tchFile, szCurFile);
			lpArg1 = NP2HeapAlloc(sizeof(WCHAR) * MAX_PATH);
			GetModuleFileName(NULL, lpArg1, MAX_PATH);
			lpArg2 = NP2HeapAlloc(sizeof(WCHAR) * 1024);
			GetRelaunchParameters(lpArg2, tchFile, !exit, FALSE);
		} else {
			LPWSTR lpCmdLine = GetCommandLine();
			size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLine) + 1);
			lpArg1 = NP2HeapAlloc(cmdSize);
			lpArg2 = NP2HeapAlloc(cmdSize);
			ExtractFirstArgument(lpCmdLine, lpArg1, lpArg2);
		}

		if (StrNotEmpty(lpArg1)) {
			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_FLAG_NO_UI | /*SEE_MASK_NOASYNC*/0x00000100 | /*SEE_MASK_NOZONECHECKS*/0x00800000;
			sei.hwnd = GetForegroundWindow();
			sei.lpVerb = L"runas";
			sei.lpFile = lpArg1;
			sei.lpParameters = lpArg2;
			sei.lpDirectory = g_wchWorkingDirectory;
			sei.nShow = SW_SHOWNORMAL;

			ShellExecuteEx(&sei);
		} else {
			exit = FALSE;
		}

		NP2HeapFree(lpArg1);
		NP2HeapFree(lpArg2);
		return exit;
	}
}

//=============================================================================
//
// SnapToDefaultPos()
//
// Aligns Notepad2 to the default window position on the current screen
//
//
void SnapToDefaultPos(HWND hwnd) {
	WINDOWPLACEMENT wndpl;
	HMONITOR hMonitor;
	MONITORINFO mi;
	int x, y, cx, cy;
	RECT rcOld;

	GetWindowRect(hwnd, &rcOld);

	hMonitor = MonitorFromRect(&rcOld, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	y = mi.rcWork.top + 16;
	cy = mi.rcWork.bottom - mi.rcWork.top - 32;
	cx = min_i(mi.rcWork.right - mi.rcWork.left - 32, cy);
	x = mi.rcWork.right - cx - 16;

	wndpl.length = sizeof(WINDOWPLACEMENT);
	wndpl.flags = WPF_ASYNCWINDOWPLACEMENT;
	wndpl.showCmd = SW_RESTORE;

	wndpl.rcNormalPosition.left = x;
	wndpl.rcNormalPosition.top = y;
	wndpl.rcNormalPosition.right = x + cx;
	wndpl.rcNormalPosition.bottom = y + cy;

	if (EqualRect(&rcOld, &wndpl.rcNormalPosition)) {
		x = mi.rcWork.left + 16;
		wndpl.rcNormalPosition.left = x;
		wndpl.rcNormalPosition.right = x + cx;
	}

	if (GetDoAnimateMinimize()) {
		DrawAnimatedRects(hwnd, IDANI_CAPTION, &rcOld, &wndpl.rcNormalPosition);
		OffsetRect(&wndpl.rcNormalPosition, mi.rcMonitor.left - mi.rcWork.left, mi.rcMonitor.top - mi.rcWork.top);
	}

	SetWindowPlacement(hwnd, &wndpl);
}

//=============================================================================
//
// ShowNotifyIcon()
//
//
void ShowNotifyIcon(HWND hwnd, BOOL bAdd) {
	static HICON hIcon;
	NOTIFYICONDATA nid;

	if (!hIcon) {
		hIcon = LoadImage(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	}

	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = APPM_TRAYMESSAGE;
	nid.hIcon = hIcon;
	lstrcpy(nid.szTip, L"Notepad2");

	if (bAdd) {
		Shell_NotifyIcon(NIM_ADD, &nid);
	} else {
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}
}

//=============================================================================
//
// SetNotifyIconTitle()
//
//
void SetNotifyIconTitle(HWND hwnd) {
	NOTIFYICONDATA nid;
	SHFILEINFO shfi;
	WCHAR tchTitle[128];

	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 0;
	nid.uFlags = NIF_TIP;

	if (StrNotEmpty(szTitleExcerpt)) {
		WCHAR tchFormat[32];
		GetString(IDS_TITLEEXCERPT, tchFormat, COUNTOF(tchFormat));
		wsprintf(tchTitle, tchFormat, szTitleExcerpt);
	} else if (StrNotEmpty(szCurFile)) {
		SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
		PathCompactPathEx(tchTitle, shfi.szDisplayName, COUNTOF(tchTitle) - 4, 0);
	} else {
		GetString(IDS_UNTITLED, tchTitle, COUNTOF(tchTitle) - 4);
	}

	if (bModified || iEncoding != iOriginalEncoding) {
		lstrcpy(nid.szTip, L"* ");
	} else {
		lstrcpy(nid.szTip, L"");
	}
	lstrcat(nid.szTip, tchTitle);

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

//=============================================================================
//
// InstallFileWatching()
//
//
void InstallFileWatching(LPCWSTR lpszFile) {
	// Terminate
	if (!iFileWatchingMode || StrIsEmpty(lpszFile)) {
		if (bRunningWatch) {
			if (hChangeHandle) {
				FindCloseChangeNotification(hChangeHandle);
				hChangeHandle = NULL;
			}
			KillTimer(NULL, ID_WATCHTIMER);
			bRunningWatch = FALSE;
			dwChangeNotifyTime = 0;
		}
		return;
	}

	// Install
	{
		WCHAR tchDirectory[MAX_PATH];
		HANDLE hFind;
		// Terminate previous watching
		if (bRunningWatch) {
			if (hChangeHandle) {
				FindCloseChangeNotification(hChangeHandle);
				hChangeHandle = NULL;
			}
			dwChangeNotifyTime = 0;
		}

		// No previous watching installed, so launch the timer first
		else {
			SetTimer(NULL, ID_WATCHTIMER, dwFileCheckInverval, WatchTimerProc);
		}

		lstrcpy(tchDirectory, lpszFile);
		PathRemoveFileSpec(tchDirectory);

		// Save data of current file
		hFind = FindFirstFile(szCurFile, &fdCurFile);
		if (hFind != INVALID_HANDLE_VALUE) {
			FindClose(hFind);
		} else {
			ZeroMemory(&fdCurFile, sizeof(WIN32_FIND_DATA));
		}

		hChangeHandle = FindFirstChangeNotification(tchDirectory, FALSE,
						FILE_NOTIFY_CHANGE_FILE_NAME	| \
						FILE_NOTIFY_CHANGE_DIR_NAME		| \
						FILE_NOTIFY_CHANGE_ATTRIBUTES	| \
						FILE_NOTIFY_CHANGE_SIZE			| \
						FILE_NOTIFY_CHANGE_LAST_WRITE);

		bRunningWatch = TRUE;
		dwChangeNotifyTime = 0;
	}
}

//=============================================================================
//
// WatchTimerProc()
//
//
void CALLBACK WatchTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(idEvent);
	UNREFERENCED_PARAMETER(dwTime);

	if (bRunningWatch) {
		if (dwChangeNotifyTime > 0 && GetTickCount() - dwChangeNotifyTime > dwAutoReloadTimeout) {
			if (hChangeHandle) {
				FindCloseChangeNotification(hChangeHandle);
				hChangeHandle = NULL;
			}
			KillTimer(NULL, ID_WATCHTIMER);
			bRunningWatch = FALSE;
			dwChangeNotifyTime = 0;
			SendMessage(hwndMain, APPM_CHANGENOTIFY, 0, 0);
		}

		// Check Change Notification Handle
		else if (WAIT_OBJECT_0 == WaitForSingleObject(hChangeHandle, 0)) {
			// Check if the changes affect the current file
			WIN32_FIND_DATA fdUpdated;
			HANDLE hFind = FindFirstFile(szCurFile, &fdUpdated);
			if (INVALID_HANDLE_VALUE != hFind) {
				FindClose(hFind);
			} else { // The current file has been removed
				ZeroMemory(&fdUpdated, sizeof(WIN32_FIND_DATA));
			}

			// Check if the file has been changed
			if (CompareFileTime(&fdCurFile.ftLastWriteTime, &fdUpdated.ftLastWriteTime) != 0 ||
					fdCurFile.nFileSizeLow != fdUpdated.nFileSizeLow ||
					fdCurFile.nFileSizeHigh != fdUpdated.nFileSizeHigh) {
				// Shutdown current watching and give control to main window
				if (hChangeHandle) {
					FindCloseChangeNotification(hChangeHandle);
					hChangeHandle = NULL;
				}
				if (iFileWatchingMode == 2) {
					bRunningWatch = TRUE; /* ! */
					dwChangeNotifyTime = GetTickCount();
				} else {
					KillTimer(NULL, ID_WATCHTIMER);
					bRunningWatch = FALSE;
					dwChangeNotifyTime = 0;
					SendMessage(hwndMain, APPM_CHANGENOTIFY, 0, 0);
				}
			} else {
				FindNextChangeNotification(hChangeHandle);
			}
		}
	}
}

//=============================================================================
//
// PasteBoardTimer()
//
//
void CALLBACK PasteBoardTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(idEvent);
	UNREFERENCED_PARAMETER(dwTime);

	if (dwLastCopyTime > 0 && GetTickCount() - dwLastCopyTime > 200) {
		if (SendMessage(hwndEdit, SCI_CANPASTE, 0, 0)) {
			BOOL bAutoIndent2 = bAutoIndent;
			bAutoIndent = FALSE;
			EditJumpTo(hwndEdit, -1, 0);
			SendMessage(hwndEdit, SCI_BEGINUNDOACTION, 0, 0);
			if (SendMessage(hwndEdit, SCI_GETLENGTH, 0, 0) > 0) {
				SendMessage(hwndEdit, SCI_NEWLINE, 0, 0);
			}
			SendMessage(hwndEdit, SCI_PASTE, 0, 0);
			SendMessage(hwndEdit, SCI_NEWLINE, 0, 0);
			SendMessage(hwndEdit, SCI_ENDUNDOACTION, 0, 0);
			EditEnsureSelectionVisible(hwndEdit);
			bAutoIndent = bAutoIndent2;
		}

		dwLastCopyTime = 0;
	}
}

// End of Notepad2.c
