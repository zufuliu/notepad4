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
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "SciCall.h"
#include "config.h"
#include "Helpers.h"
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
#include "resource.h"

//! show fold level
#define NP2_DEBUG_FOLD_LEVEL		0

//! Enable CallTips (not yet implemented)
#define NP2_ENABLE_SHOW_CALLTIPS	0

/******************************************************************************
*
* Local and global Variables for Notepad2.c
*
*/
HWND	hwndStatus;
static HWND hwndToolbar;
static HWND hwndReBar;
static HWND hwndEdit;
static HWND hwndEditFrame;
HWND	hwndMain;
static HWND hwndNextCBChain = NULL;
HWND	hDlgFindReplace = NULL;
static BOOL bInitDone = FALSE;
static HACCEL hAccMain;
static HACCEL hAccFindReplace;

// tab width for notification text
#define TAB_WIDTH_NOTIFICATION		8

#define TOOLBAR_COMMAND_BASE	IDT_FILE_NEW
#define DefaultToolbarButtons	L"22 3 0 1 2 0 4 18 19 0 5 6 0 7 8 9 20 0 10 11 0 12 0 24 0 13 14 0 15 16 0 17"
static TBBUTTON tbbMainWnd[] = {
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{0, 	IDT_FILE_NEW, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{1, 	IDT_FILE_OPEN, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{2, 	IDT_FILE_BROWSE, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{3, 	IDT_FILE_SAVE, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{4, 	IDT_EDIT_UNDO, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{5, 	IDT_EDIT_REDO, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{6, 	IDT_EDIT_CUT, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{7, 	IDT_EDIT_COPY, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{8, 	IDT_EDIT_PASTE, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{9, 	IDT_EDIT_FIND, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{10, 	IDT_EDIT_REPLACE, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{11, 	IDT_VIEW_WORDWRAP, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{12, 	IDT_VIEW_ZOOMIN, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{13, 	IDT_VIEW_ZOOMOUT, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{14, 	IDT_VIEW_SCHEME, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{15, 	IDT_VIEW_SCHEMECONFIG, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{16, 	IDT_FILE_EXIT, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{17, 	IDT_FILE_SAVEAS, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{18, 	IDT_FILE_SAVECOPY, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{19, 	IDT_EDIT_DELETE, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{20, 	IDT_FILE_PRINT, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{21, 	IDT_FILE_OPENFAV, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{22, 	IDT_FILE_ADDTOFAV, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{23, 	IDT_VIEW_TOGGLEFOLDS, 	TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN, {0}, 0, 0},
	{24, 	IDT_FILE_LAUNCH, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
};

WCHAR	szIniFile[MAX_PATH] = L"";
static WCHAR szIniFile2[MAX_PATH] = L"";
static BOOL bSaveSettings;
BOOL	bSaveRecentFiles;
static BOOL bSaveFindReplace;
static WCHAR tchLastSaveCopyDir[MAX_PATH] = L"";
WCHAR	tchOpenWithDir[MAX_PATH];
WCHAR	tchFavoritesDir[MAX_PATH];
static WCHAR tchDefaultDir[MAX_PATH];
static WCHAR tchToolbarButtons[MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE];
static LPWSTR tchToolbarBitmap = NULL;
static LPWSTR tchToolbarBitmapHot = NULL;
static LPWSTR tchToolbarBitmapDisabled = NULL;
static int iPathNameFormat;
BOOL	fWordWrap;
BOOL	fWordWrapG;
int		iWordWrapMode;
int		iWordWrapIndent;
int		iWordWrapSymbols;
BOOL	bShowWordWrapSymbols;
BOOL	bWordWrapSelectSubLine;
static BOOL bShowUnicodeControlCharacter;
static BOOL bMatchBraces;
static BOOL bShowIndentGuides;
static BOOL bHighlightCurrentBlock;
BOOL	bHighlightCurrentSubLine;
INT		iHighlightCurrentLine;
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
static BOOL bMarkLongLines;
int		iLongLinesLimit;
int		iLongLinesLimitG;
int		iLongLineMode;
int		iWrapCol = 0;
BOOL	bShowBookmarkMargin;
static BOOL bShowLineNumbers;
static BOOL bMarkOccurrences;
static BOOL bMarkOccurrencesMatchCase;
static BOOL bMarkOccurrencesMatchWords;
static BOOL bMarkOccurrencesBookmark;
struct EditAutoCompletionConfig autoCompletionConfig;
static BOOL bEnableLineSelectionMode;
static BOOL bShowCodeFolding;
#if NP2_ENABLE_SHOW_CALLTIPS
static BOOL bShowCallTips = TRUE;
static int iCallTipsWaitTime = 500; // 500 ms
#endif
static BOOL bViewWhiteSpace;
static BOOL bViewEOLs;
int		iDefaultEncoding;
BOOL	bSkipUnicodeDetection;
BOOL	bLoadANSIasUTF8;
BOOL	bLoadASCIIasUTF8;
BOOL	bLoadNFOasOEM;
BOOL	bNoEncodingTags;
int		iSrcEncoding = -1;
int		iWeakSrcEncoding = -1;
#if defined(_WIN64)
BOOL	bLargeFileMode = FALSE;
#endif
int		iDefaultEOLMode;
BOOL	bWarnLineEndings;
BOOL	bFixLineEndings;
BOOL	bAutoStripBlanks;
int		iPrintHeader;
int		iPrintFooter;
int		iPrintColor;
int		iPrintZoom = 100;
RECT	pageSetupMargin;
static BOOL bSaveBeforeRunningTools;
BOOL bOpenFolderWithMetapath;
int		iFileWatchingMode;
int		iFileWatchingMethod;
BOOL	bFileWatchingKeepAtEnd;
BOOL	bResetFileWatching;
static DWORD dwFileCheckInterval;
static DWORD dwAutoReloadTimeout;
static int iEscFunction;
static BOOL bAlwaysOnTop;
static BOOL bMinimizeToTray;
static BOOL bTransparentMode;
static int	iEndAtLastLine;
BOOL	bFindReplaceTransparentMode;
BOOL	bFindReplaceUseMonospacedFont;
BOOL	bFindReplaceFindAllBookmark;
static BOOL bEditLayoutRTL;
BOOL	bWindowLayoutRTL;
static int iRenderingTechnology;
static BOOL bUseInlineIME;
static int iBidirectional;
static BOOL bShowToolbar;
static BOOL bAutoScaleToolbar;
static BOOL bShowStatusbar;
static BOOL bInFullScreenMode;
static int iFullScreenMode;

typedef struct WININFO {
	int x;
	int y;
	int cx;
	int cy;
	BOOL max;
} WININFO;

static WININFO wi;

static int cyReBar;
static int cyReBarFrame;
static int cxEditFrame;
static int cyEditFrame;

int		cxRunDlg;
int		cxEncodingDlg;
int		cyEncodingDlg;
int		cxFileMRUDlg;
int		cyFileMRUDlg;
int		cxOpenWithDlg;
int		cyOpenWithDlg;
int		cxFavoritesDlg;
int		cyFavoritesDlg;
int		cxAddFavoritesDlg;
int		cxModifyLinesDlg;
int		cyModifyLinesDlg;
int		cxEncloseSelectionDlg;
int		cyEncloseSelectionDlg;
int		cxInsertTagDlg;
int		cyInsertTagDlg;
int		xFindReplaceDlg;
int		yFindReplaceDlg;
int		cxFindReplaceDlg;

extern int cxStyleSelectDlg;
extern int cyStyleSelectDlg;
extern int cxStyleCustomizeDlg;
extern int cyStyleCustomizeDlg;

static LPWSTR lpFileList[32];
static int cFileList = 0;
static int cchiFileList = 0;
static LPWSTR lpFileArg = NULL;
static LPWSTR lpSchemeArg = NULL;
static LPWSTR lpMatchArg = NULL;
static LPWSTR lpEncodingArg = NULL;
LPMRULIST	pFileMRU;
LPMRULIST	mruFind;
LPMRULIST	mruReplace;

DWORD	dwLastIOError;
WCHAR	szCurFile[MAX_PATH + 40];
FILEVARS	fvCurFile;
static BOOL bModified;
static BOOL bReadOnly = FALSE;
BOOL bLockedForEditing = FALSE; // save call to SciCall_GetReadOnly()
static int iOriginalEncoding;
static int iEOLMode;

int		iEncoding;
// DBCS code page
int		iDefaultCodePage;
int		iDefaultCharSet;

static Sci_Line iInitialLine;
static Sci_Position iInitialColumn;
static int iInitialLexer;

static BOOL bLastCopyFromMe = FALSE;
static DWORD dwLastCopyTime;

static UINT uidsAppTitle = IDS_APPTITLE;
static WCHAR szTitleExcerpt[128] = L"";
static int fKeepTitleExcerpt = 0;

static HANDLE hChangeHandle = NULL;
static BOOL bRunningWatch = FALSE;
static DWORD dwChangeNotifyTime = 0;

static UINT msgTaskbarCreated = 0;

static WIN32_FIND_DATA fdCurFile;
static EDITFINDREPLACE efrData;
UINT	cpLastFind = 0;
BOOL	bReplaceInitialized = FALSE;
EditMarkAllStatus editMarkAllStatus;
HANDLE idleTaskTimer;

static int iSortOptions = 0;
static int iAlignMode	= 0;
extern int iFontQuality;
extern int iCaretStyle;
extern int iOvrCaretStyle;
extern BOOL bBlockCaretOutSelection;
extern int iCaretBlinkPeriod;

BOOL fIsElevated = FALSE;
static WCHAR wchWndClass[16] = WC_NOTEPAD2;

enum StatusBarUpdateMask {
	StatusBarUpdateMask_Lexer = 1,
	StatusBarUpdateMask_CodePage = 2,
	StatusBarUpdateMask_EOLMode = 4,
	StatusBarUpdateMask_OVRMode = 8,
	StatusBarUpdateMask_DocZoom = 16,
	StatusBarUpdateMask_LineColumn = 32,
};
// rarely changed statusbar items
struct CachedStatusItem {
	UINT updateMask;
	BOOL overType;

	Sci_Line iLine;
	Sci_Position iLineChar;
	Sci_Position iLineColumn;

	LPCWSTR pszLexerName;
	LPCWSTR pszEOLMode;
	LPCWSTR pszOvrMode;
	WCHAR tchZoom[8];

	WCHAR tchLexerName[MAX_EDITLEXER_NAME_SIZE];
	WCHAR tchDocPosFmt[96];
} cachedStatusItem;

#define UpdateStatusBarCacheLineColumn()	cachedStatusItem.updateMask |= StatusBarUpdateMask_LineColumn

HINSTANCE	g_hInstance;
HANDLE		g_hDefaultHeap;
HANDLE		g_hScintilla;
#if _WIN32_WINNT < _WIN32_WINNT_WIN7
DWORD		g_uWinVer;
#endif
#if _WIN32_WINNT < _WIN32_WINNT_WIN8
DWORD		kSystemLibraryLoadFlags = 0;
#endif
UINT		g_uCurrentDPI = USER_DEFAULT_SCREEN_DPI;
UINT		g_uSystemDPI = USER_DEFAULT_SCREEN_DPI;
WCHAR 		g_wchAppUserModelID[64] = L"";
static WCHAR g_wchWorkingDirectory[MAX_PATH] = L"";
#if NP2_ENABLE_APP_LOCALIZATION_DLL
static HMODULE hResDLL;
LANGID uiLanguage;
static UINT languageMenu;
#endif

//=============================================================================
//
// Flags
//
static int	flagNoReuseWindow		= 0;
static int	flagReuseWindow			= 0;
static BOOL bSingleFileInstance		= TRUE;
static BOOL bReuseWindow			= FALSE;
static int	flagMultiFileArg		= 0;
static int	flagSingleFileInstance	= 1;
static int	flagStartAsTrayIcon		= 0;
static int	flagAlwaysOnTop			= 0;
static int	flagRelativeFileMRU		= 0;
static int	flagPortableMyDocs		= 0;
int			flagNoFadeHidden		= 0;
static int	iOpacityLevel			= 75;
int			iFindReplaceOpacityLevel= 75;
static int	flagToolbarLook			= 0;
int			flagSimpleIndentGuides	= 0;
BOOL 		fNoHTMLGuess			= 0;
BOOL 		fNoCGIGuess				= 0;
BOOL 		fNoAutoDetection		= 0;
BOOL		fNoFileVariables		= 0;
static int	flagPosParam			= 0;
static int	flagDefaultPos			= 0;
static int	flagNewFromClipboard	= 0;
static int	flagPasteBoard			= 0;
static int	flagSetEncoding			= 0;
static int	flagSetEOLMode			= 0;
static int	flagJumpTo				= 0;
static int	flagMatchText			= 0;
static int	flagChangeNotify		= 0;
static int	flagLexerSpecified		= 0;
static int	flagQuietCreate			= 0;
int			flagUseSystemMRU		= 0;
static int	flagRelaunchElevated	= 0;
static int	flagDisplayHelp			= 0;

static inline BOOL IsDocumentModified(void) {
	return bModified || iEncoding != iOriginalEncoding;
}

static inline BOOL IsTopMost(void) {
	return (bAlwaysOnTop || flagAlwaysOnTop == 2) && flagAlwaysOnTop != 1;
}

static inline void ToggleFullScreenModeConfig(int config) {
	if (iFullScreenMode & config) {
		iFullScreenMode &= ~config;
	} else {
		iFullScreenMode |= config;
	}
	if (bInFullScreenMode && config != FullScreenMode_OnStartup) {
		ToggleFullScreenMode();
	}
}

static inline void UpdateStatusBarCache_OVRMode(BOOL force) {
	const BOOL overType = SciCall_GetOvertype();
	if (force || overType != cachedStatusItem.overType) {
		cachedStatusItem.overType = overType;
		cachedStatusItem.pszOvrMode = overType ? L"OVR" : L"INS";
		cachedStatusItem.updateMask |= StatusBarUpdateMask_OVRMode;
	}
}

// temporary fix for https://github.com/zufuliu/notepad2/issues/77: force InvalidateStyleRedraw().
static inline void InvalidateStyleRedraw(void) {
	SciCall_SetViewEOL(bViewEOLs);
}

// temporary fix for https://github.com/zufuliu/notepad2/issues/134: Direct2D on arm32
static inline int GetDefualtRenderingTechnology(void) {
#if defined(__arm__) || defined(_ARM_) || defined(_M_ARM)
	return SC_TECHNOLOGY_DIRECTWRITERETAIN;
#else
	return IsVistaAndAbove()? SC_TECHNOLOGY_DIRECTWRITE : SC_TECHNOLOGY_DEFAULT;
#endif
}

//=============================================================================
//
// WinMain()
//
//
static void CleanUpResources(BOOL initialized) {
	if (tchToolbarBitmap != NULL) {
		LocalFree(tchToolbarBitmap);
	}
	if (tchToolbarBitmapHot != NULL) {
		LocalFree(tchToolbarBitmapHot);
	}
	if (tchToolbarBitmapDisabled != NULL) {
		LocalFree(tchToolbarBitmapDisabled);
	}
	if (lpSchemeArg) {
		LocalFree(lpSchemeArg);
	}

	Encoding_ReleaseResources();
	Style_ReleaseResources();
	Edit_ReleaseResources();
	Scintilla_ReleaseResources();

	if (initialized) {
		UnregisterClass(wchWndClass, g_hInstance);
	}
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	if (hResDLL) {
		FreeLibrary(hResDLL);
	}
#endif
	OleUninitialize();
}

static void DispatchMessageMain(MSG *msg) {
	if (IsWindow(hDlgFindReplace) && (msg->hwnd == hDlgFindReplace || IsChild(hDlgFindReplace, msg->hwnd))) {
		if (TranslateAccelerator(hDlgFindReplace, hAccFindReplace, msg) || IsDialogMessage(hDlgFindReplace, msg)) {
			return;
		}
	}

	if (!TranslateAccelerator(hwndMain, hAccMain, msg)) {
		TranslateMessage(msg);
		DispatchMessage(msg);
	}
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT) {
		SendWMCommand(hwndMain, IDM_FILE_EXIT);
		return TRUE;
	}
	return FALSE;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#if 0 // used for Clang UBSan or printing debug message on console.
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		fprintf(stdout, "\n%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
	}
#endif
#if 0 && defined(__clang__)
	SetEnvironmentVariable(L"UBSAN_OPTIONS", L"log_path=" WC_NOTEPAD2 L"-UBSan.log");
#endif

	// Set global variable g_hInstance
	g_hInstance = hInstance;
#if _WIN32_WINNT < _WIN32_WINNT_WIN7
	// Set the Windows version global variable
	NP2_COMPILER_WARNING_PUSH
	NP2_IGNORE_WARNING_DEPRECATED_DECLARATIONS
	g_uWinVer = LOWORD(GetVersion());
	NP2_COMPILER_WARNING_POP
	g_uWinVer = MAKEWORD(HIBYTE(g_uWinVer), LOBYTE(g_uWinVer));
#endif

	g_hDefaultHeap = GetProcessHeap();
	// https://docs.microsoft.com/en-us/windows/desktop/Memory/low-fragmentation-heap
#if 0 // default enabled since Vista
	{
		// Enable the low-fragmenation heap (LFH).
		ULONG HeapInformation = /*HEAP_LFH*/2;
		HeapSetInformation(g_hDefaultHeap, HeapCompatibilityInformation, &HeapInformation, sizeof(HeapInformation));
		// Enable heap terminate-on-corruption.
		HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	}
#endif

	// Don't keep working directory locked
	WCHAR wchWorkingDirectory[MAX_PATH];
	GetCurrentDirectory(COUNTOF(g_wchWorkingDirectory), g_wchWorkingDirectory);
	GetModuleFileName(NULL, wchWorkingDirectory, COUNTOF(wchWorkingDirectory));
	PathRemoveFileSpec(wchWorkingDirectory);
	SetCurrentDirectory(wchWorkingDirectory);

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

	// Check if running with elevated privileges
	fIsElevated = IsElevated();

	// Default Encodings (may already be used for command line parsing)
	Encoding_InitDefaults();

	// Command Line, Ini File and Flags
	ParseCommandLine();
	FindIniFile();
	TestIniFile();
	CreateIniFile(szIniFile);
	LoadFlags();

	// set AppUserModelID
	PrivateSetCurrentProcessExplicitAppUserModelID(g_wchAppUserModelID);

	// Command Line Help Dialog
	if (flagDisplayHelp) {
#if NP2_ENABLE_APP_LOCALIZATION_DLL
		hResDLL = LoadLocalizedResourceDLL(uiLanguage, WC_NOTEPAD2 L".dll");
		if (hResDLL) {
			g_hInstance = (HINSTANCE)hResDLL;
		}
#endif
		DisplayCmdLineHelp(NULL);
#if NP2_ENABLE_APP_LOCALIZATION_DLL
		if (hResDLL) {
			FreeLibrary(hResDLL);
		}
#endif
		return 0;
	}

	// Adapt window class name
	if (fIsElevated) {
		lstrcat(wchWndClass, L"U");
	}
	if (flagPasteBoard) {
		lstrcat(wchWndClass, L"B");
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

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC	= ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	msgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

#if _WIN32_WINNT < _WIN32_WINNT_WIN8
	// see LoadD2D() in PlatWin.cxx
	kSystemLibraryLoadFlags = (DLLFunctionEx(FARPROC, L"kernel32.dll", "SetDefaultDllDirectories") != NULL) ? LOAD_LIBRARY_SEARCH_SYSTEM32 : 0;
#endif

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	hResDLL = LoadLocalizedResourceDLL(uiLanguage, WC_NOTEPAD2 L".dll");
	if (hResDLL) {
		g_hInstance = hInstance = (HINSTANCE)hResDLL;
	}
#endif

	// we need DPI-related functions before create Scintilla window.
#if NP2_HAS_GETDPIFORWINDOW
	g_uSystemDPI = GetDpiForSystem();
#else
	Scintilla_LoadDpiForWindow();
#endif
	Scintilla_RegisterClasses(hInstance);

	// Load Settings
	LoadSettings();

	if (!InitApplication(hInstance)) {
		CleanUpResources(FALSE);
		return FALSE;
	}

	// create the timer first, to make flagMatchText working.
	HANDLE timer = idleTaskTimer = WaitableTimer_Create();
	QueryPerformanceFrequency(&editMarkAllStatus.watch.freq);
	InitInstance(hInstance, nShowCmd);
	hAccMain = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	hAccFindReplace = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCFINDREPLACE));
	MSG msg;

	while (TRUE) {
		if (editMarkAllStatus.pending) {
			WaitableTimer_Set(timer, WaitableTimer_IdleTaskDelayTime);
			while (editMarkAllStatus.pending && WaitableTimer_Continue(timer)) {
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					DispatchMessageMain(&msg);
				}
			}
			if (editMarkAllStatus.pending) {
				EditMarkAll_Continue(&editMarkAllStatus, timer);
			}
		}
		if (GetMessage(&msg, NULL, 0, 0)) {
			DispatchMessageMain(&msg);
		} else {
			break;
		}
	}

	WaitableTimer_Destroy(timer);
	CleanUpResources(TRUE);
	return (int)(msg.wParam);
}

//=============================================================================
//
// InitApplication()
//
//
BOOL InitApplication(HINSTANCE hInstance) {
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
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
	wc.hIconSm       = NULL;

	return RegisterClassEx(&wc);
}

//=============================================================================
//
// InitInstance()
//
//
void InitInstance(HINSTANCE hInstance, int nCmdShow) {
#if 0
	StopWatch watch;
	StopWatch_Start(watch);
#endif
	const BOOL defaultPos = (wi.x == CW_USEDEFAULT || wi.y == CW_USEDEFAULT || wi.cx == CW_USEDEFAULT || wi.cy == CW_USEDEFAULT);
	RECT rc = { wi.x, wi.y, (defaultPos ? CW_USEDEFAULT : (wi.x + wi.cx)), (defaultPos ? CW_USEDEFAULT : (wi.y + wi.cy)) };

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
	} else if (flagDefaultPos == 2 || flagDefaultPos == 3 || defaultPos) {
		// default window position
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		wi.y = rc.top + 16;
		wi.cy = rc.bottom - rc.top - 32;
		wi.cx = min_i(rc.right - rc.left - 32, wi.cy);
		wi.x = (flagDefaultPos == 3) ? rc.left + 16 : rc.right - wi.cx - 16;
	} else {
		// fit window into working area of current monitor
		HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

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
		RECT rc2;
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

	if (IsTopMost()) {
		SetWindowPos(hwndMain, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if (bTransparentMode) {
		SetWindowTransparentMode(hwndMain, TRUE, iOpacityLevel);
	}

	if (!flagStartAsTrayIcon) {
		ShowWindow(hwndMain, nCmdShow);
		UpdateWindow(hwndMain);
	} else {
		ShowWindow(hwndMain, SW_HIDE); // trick ShowWindow()
		ShowNotifyIcon(hwndMain, TRUE);
	}

	// Source Encoding
	if (lpEncodingArg) {
		iSrcEncoding = Encoding_Match(lpEncodingArg);
	}

	UpdateStatusBarCache_OVRMode(TRUE);
	UpdateStatusBarCache(STATUS_DOCZOOM);
	BOOL bOpened = FALSE;
	BOOL bFileLoadCalled = FALSE;
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
					EditJumpTo(iInitialLine, iInitialColumn);
					EditEnsureSelectionVisible();
				}
			}
		}
		NP2HeapFree(lpFileArg);

		if (bOpened) {
			if (flagChangeNotify == 1) {
				iFileWatchingMode = 0;
				bResetFileWatching = TRUE;
				InstallFileWatching(FALSE);
			} else if (flagChangeNotify == 2) {
				iFileWatchingMode = 2;
				bResetFileWatching = TRUE;
				InstallFileWatching(FALSE);
			}
		}
	} else {
		if (iSrcEncoding != -1) {
			iEncoding = iSrcEncoding;
			iOriginalEncoding = iSrcEncoding;
			SciCall_SetCodePage((iEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
		}
	}

	if (!bFileLoadCalled) {
		bOpened = FileLoad(TRUE, TRUE, FALSE, FALSE, L"");
	}
	if (!bOpened) {
		UpdateStatusBarCache(STATUS_CODEPAGE);
		UpdateStatusBarCache(STATUS_EOLMODE);
		UpdateStatusBarCacheLineColumn();
	}

	// reset
	iSrcEncoding = -1;
	flagQuietCreate = 0;
	fKeepTitleExcerpt = 0;

	// Check for /c [if no file is specified] -- even if a file is specified
	/*else */
	if (flagNewFromClipboard) {
		if (SciCall_CanPaste()) {
			const BOOL back = autoCompletionConfig.bIndentText;
			autoCompletionConfig.bIndentText = FALSE;
			SciCall_DocumentEnd();
			SciCall_BeginUndoAction();
			if (SciCall_GetLength() > 0) {
				SciCall_NewLine();
			}
			SciCall_Paste(FALSE);
			SciCall_NewLine();
			SciCall_EndUndoAction();
			autoCompletionConfig.bIndentText = back;
			if (flagJumpTo) {
				EditJumpTo(iInitialLine, iInitialColumn);
			}
			EditEnsureSelectionVisible();
		}
	}

	// Encoding
	if (0 != flagSetEncoding) {
		SendWMCommand(hwndMain, IDM_ENCODING_ANSI + flagSetEncoding - 1);
		flagSetEncoding = 0;
	}

	// EOL mode
	if (0 != flagSetEOLMode) {
		SendWMCommand(hwndMain, IDM_LINEENDINGS_CRLF + flagSetEOLMode - 1);
		flagSetEOLMode = 0;
	}

	// Match Text
	if (flagMatchText && lpMatchArg) {
		if (StrNotEmpty(lpMatchArg) && SciCall_GetLength()) {
			const UINT cpEdit = SciCall_GetCodePage();
			WideCharToMultiByte(cpEdit, 0, lpMatchArg, -1, efrData.szFind, COUNTOF(efrData.szFind), NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, lpMatchArg, -1, efrData.szFindUTF8, COUNTOF(efrData.szFindUTF8), NULL, NULL);
			cpLastFind = cpEdit;

			if (flagMatchText & 4) {
				efrData.fuFlags |= SCFIND_REGEXP | SCFIND_POSIX;
			} else if (flagMatchText & 8) {
				efrData.bTransformBS = TRUE;
			}

			if (flagMatchText & 2) {
				if (!flagJumpTo) {
					SciCall_DocumentEnd();
				}
				EditFindPrev(&efrData, FALSE);
				EditEnsureSelectionVisible();
			} else {
				if (!flagJumpTo) {
					SciCall_DocumentStart();
				}
				EditFindNext(&efrData, FALSE);
				EditEnsureSelectionVisible();
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
			Style_SetLexerFromName(szCurFile, lpSchemeArg);
			LocalFree(lpSchemeArg);
			lpSchemeArg = NULL;
		} else {
			Style_SetLexerFromID(iInitialLexer);
		}
		flagLexerSpecified = 0;
	}

	// If start as tray icon, set current filename as tooltip
	if (flagStartAsTrayIcon) {
		SetNotifyIconTitle(hwndMain);
	}

	if (!bInitDone) {
		bInitDone = TRUE;
		UpdateStatusBarWidth();
	}
	if (SciCall_GetLength() == 0) {
		UpdateToolbar();
		UpdateStatusbar();
	}

#if 0
	StopWatch_Stop(watch);
	StopWatch_ShowLog(&watch, "InitInstance() time");
#endif
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

static inline void EditMarkAll_Stop(void) {
	editMarkAllStatus.pending = FALSE;
	editMarkAllStatus.matchCount = 0;
	WaitableTimer_Set(idleTaskTimer, 0);
	EditMarkAll_Clear();
}

static inline void ExitApplication(HWND hwnd) {
	if (FileSave(FALSE, TRUE, FALSE, FALSE)) {
		if (bInFullScreenMode) {
			bInFullScreenMode = FALSE;
			ToggleFullScreenMode();
		}
		EditMarkAll_Stop();
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

#if NP2_ENABLE_DOT_LOG_FEATURE
static inline BOOL IsFileStartsWithDotLog(void) {
	char tch[5] = "";
	const Sci_Position len = SciCall_GetText(COUNTOF(tch), tch);
	// upper case
	return len >= 4 && strcmp(tch, ".LOG") == 0;
}
#endif

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

			EditMarkAll_Stop();
			// Terminate file watching
			InstallFileWatching(TRUE);

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
			ExitApplication(hwnd);
		}
		break;

	case WM_QUERYENDSESSION:
		if (FileSave(FALSE, TRUE, FALSE, FALSE)) {
			return TRUE;
		}
		return FALSE;

	case WM_THEMECHANGED:
		// Reinitialize theme-dependent values and resize windows
		MsgThemeChanged(hwnd, wParam, lParam);
		break;

	case WM_DPICHANGED:
		MsgDPIChanged(hwnd, wParam, lParam);
		break;

	case WM_SETTINGCHANGE:
		// TODO: detect system theme and high contrast mode changes
		Style_UpdateCaret();
		SendMessage(hwndEdit, WM_SETTINGCHANGE, wParam, lParam);
		break;

	case WM_SYSCOLORCHANGE:
		// update Scintilla colors
		Style_SetLexer(pLexCurrent, FALSE);
		SendMessage(hwndEdit, WM_SYSCOLORCHANGE, wParam, lParam);
		break;

	//case WM_TIMER:
	//	break;

	case WM_SIZE:
		MsgSize(hwnd, wParam, lParam);
		break;

	case WM_GETMINMAXINFO:
		if (bInFullScreenMode) {
			HMONITOR hMonitor = MonitorFromWindow(hwndMain, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMonitor, &mi);

			const int w = mi.rcMonitor.right - mi.rcMonitor.left;
			const int h = mi.rcMonitor.bottom - mi.rcMonitor.top;

			LPMINMAXINFO pmmi = (LPMINMAXINFO)lParam;
			pmmi->ptMaxSize.x = w + 2 * SystemMetricsForDpi(SM_CXSIZEFRAME, g_uCurrentDPI);
			pmmi->ptMaxSize.y = h + SystemMetricsForDpi(SM_CYCAPTION, g_uCurrentDPI)
				+ SystemMetricsForDpi(SM_CYMENU, g_uCurrentDPI)
				+ 2 * SystemMetricsForDpi(SM_CYSIZEFRAME, g_uCurrentDPI);
			pmmi->ptMaxTrackSize.x = pmmi->ptMaxSize.x;
			pmmi->ptMaxTrackSize.y = pmmi->ptMaxSize.y;
			return 0;
		}
		return DefWindowProc(hwnd, umsg, wParam, lParam);

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		//if (bPendingChangeNotify)
		//	PostMessage(hwnd, APPM_CHANGENOTIFY, 0, 0);
		break;

	case WM_DROPFILES: {
		WCHAR szBuf[MAX_PATH + 40];
		HDROP hDrop = (HDROP)wParam;

		DragQueryFile(hDrop, 0, szBuf, COUNTOF(szBuf));
		OnDropOneFile(hwnd, szBuf);

		//if (DragQueryFile(hDrop, (UINT)(-1), NULL, 0) > 1) {
		//	MsgBoxWarn(MB_OK, IDS_ERR_DROP);
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
			LPNP2PARAMS params = (LPNP2PARAMS)NP2HeapAlloc(pcds->cbData);
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
						InstallFileWatching(FALSE);
					} else if (params->flagChangeNotify == 2) {
						iFileWatchingMode = 2;
						bResetFileWatching = TRUE;
						InstallFileWatching(FALSE);
					}

					if (0 != params->flagSetEncoding) {
						flagSetEncoding = params->flagSetEncoding;
						SendWMCommand(hwnd, IDM_ENCODING_ANSI + flagSetEncoding - 1);
						flagSetEncoding = 0;
					}

					if (0 != params->flagSetEOLMode) {
						flagSetEOLMode = params->flagSetEOLMode;
						SendWMCommand(hwnd, IDM_LINEENDINGS_CRLF + flagSetEOLMode - 1);
						flagSetEOLMode = 0;
					}

					if (params->flagLexerSpecified) {
						if (params->iInitialLexer <= 0) {
							WCHAR wchExt[32] = L".";
							lstrcpyn(CharNext(wchExt), StrEnd(&params->wchData) + 1, 30);
							Style_SetLexerFromName(&params->wchData, wchExt);
						} else {
							Style_SetLexerFromID(params->iInitialLexer);
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
				EditJumpTo(params->iInitialLine, params->iInitialColumn);
				EditEnsureSelectionVisible();
			}

			flagLexerSpecified = 0;
			flagQuietCreate = 0;

			NP2HeapFree(params);

			UpdateStatusbar();
		}
	}
	return TRUE;

	case WM_CONTEXTMENU: {
		const int nID = GetDlgCtrlID((HWND)wParam);

		if (!(nID == IDC_EDIT || nID == IDC_STATUSBAR || nID == IDC_REBAR || nID == IDC_TOOLBAR)) {
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}

		HMENU hmenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_POPUPMENU));
		//SetMenuDefaultItem(GetSubMenu(hmenu, IDP_POPUP_SUBMENU_BAR), 0, FALSE);
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		int imenu = 0;
		switch (nID) {
		case IDC_EDIT: {
			if (SciCall_IsSelectionEmpty() && pt.x != -1 && pt.y != -1) {
				POINT ptc = pt;
				ScreenToClient(hwndEdit, &ptc);
				const Sci_Position iNewPos = SciCall_PositionFromPoint(ptc.x, ptc.y);
				SciCall_GotoPos(iNewPos);
			}

			if (pt.x == -1 && pt.y == -1) {
				const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
				pt.x = SciCall_PointXFromPosition(iCurrentPos);
				pt.y = SciCall_PointYFromPosition(iCurrentPos);
				ClientToScreen(hwndEdit, &pt);
			}
			imenu = IDP_POPUP_SUBMENU_EDIT;
		}
		break;

		case IDC_TOOLBAR:
		case IDC_STATUSBAR:
		case IDC_REBAR:
			if (pt.x == -1 && pt.y == -1) {
				GetCursorPos(&pt);
			}
			imenu = IDP_POPUP_SUBMENU_BAR;
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
			const LRESULT lrv = DefWindowProc(hwnd, umsg, wParam, lParam);
			ShowOwnedPopups(hwnd, TRUE);
			return lrv;
		}
		}
		return DefWindowProc(hwnd, umsg, wParam, lParam);

	case APPM_CHANGENOTIFY:
		if (iFileWatchingMode == 1 || IsDocumentModified()) {
			SetForegroundWindow(hwnd);
		}

		if (PathIsFile(szCurFile)) {
			if ((iFileWatchingMode == 2 && !IsDocumentModified()) || MsgBoxWarn(MB_YESNO, IDS_FILECHANGENOTIFY) == IDYES) {
				const BOOL bIsTail = (iFileWatchingMode == 2) && (bFileWatchingKeepAtEnd || (SciCall_LineFromPosition(SciCall_GetCurrentPos()) + 1 == SciCall_GetLineCount()));

				iWeakSrcEncoding = iEncoding;
				if (FileLoad(TRUE, FALSE, TRUE, FALSE, szCurFile)) {
					if (bIsTail) {
						SciCall_DocumentEnd();
						EditEnsureSelectionVisible();
					}
				}
			}
		} else {
			if (MsgBoxWarn(MB_YESNO, IDS_FILECHANGENOTIFY2) == IDYES) {
				FileSave(TRUE, FALSE, FALSE, FALSE);
			}
		}

		if (!bRunningWatch) {
			InstallFileWatching(FALSE);
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
			HMENU hMenuPopup = GetSubMenu(hMenu, IDP_POPUP_SUBMENU_TRAY);

			SetForegroundWindow(hwnd);

			POINT pt;
			GetCursorPos(&pt);
			SetMenuDefaultItem(hMenuPopup, IDM_TRAY_RESTORE, FALSE);
			const int iCmd = TrackPopupMenu(hMenuPopup,
								  TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
								  pt.x, pt.y, 0, hwnd, NULL);

			PostMessage(hwnd, WM_NULL, 0, 0);
			DestroyMenu(hMenu);

			if (iCmd == IDM_TRAY_RESTORE) {
				NP2RestoreWind(hwnd);
			} else if (iCmd == IDM_TRAY_EXIT) {
				ExitApplication(hwnd);
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
				iPathNameFormat, IsDocumentModified(),
				IDS_READONLY, bReadOnly,
				IDS_LOCKED, bLockedForEditing,
				szTitleExcerpt);
}

static inline void UpdateDocumentModificationStatus(void) {
	UpdateWindowTitle();
	UpdateToolbar();
}

void UpdateBookmarkMarginWidth(void) {
	// see LineMarker::Draw() for minDim.
	const int width = bShowBookmarkMargin ? SciCall_TextHeight() - 2 : 0;
	// 16px for XPM bookmark symbol.
	//const int width = bShowBookmarkMargin ? max_i(SciCall_TextHeight() - 2, 16) : 0;
	SciCall_SetMarginWidth(MarginNumber_Bookmark, width);
}

void UpdateFoldMarginWidth(void) {
	const int width = bShowCodeFolding ? SciCall_TextWidth(STYLE_LINENUMBER, "+_") : 0;
	SciCall_SetMarginWidth(MarginNumber_CodeFolding, width);
}

void SetWrapStartIndent(void) {
	int indent = 0;
	switch (iWordWrapIndent) {
	case EditWrapIndentOneCharacter:
		indent = 1;
		break;
	case EditWrapIndentTwoCharacter:
		indent = 2;
		break;
	case EditWrapIndentOneLevel:
		indent = (iIndentWidth) ? iIndentWidth : iTabWidth;
		break;
	case EditWrapIndentTwoLevel:
		indent = (iIndentWidth) ? 2 * iIndentWidth : 2 * iTabWidth;
		break;
	}
	SciCall_SetWrapStartIndent(indent);
}

void SetWrapIndentMode(void) {
	int indentMode;
	switch (iWordWrapIndent) {
	case EditWrapIndentSameAsSubline:
		indentMode = SC_WRAPINDENT_SAME;
		break;
	case EditWrapIndentOneLevelThanSubline:
		indentMode = SC_WRAPINDENT_INDENT;
		break;
	case EditWrapIndentTwoLevelThanSubline:
		indentMode = SC_WRAPINDENT_DEEPINDENT;
		break;
	default:
		indentMode = SC_WRAPINDENT_FIXED;
		SetWrapStartIndent();
		break;
	}
	SciCall_SetWrapIndentMode(indentMode);
}

void SetWrapVisualFlags(void) {
	if (bShowWordWrapSymbols) {
		int wrapVisualFlags = 0;
		int wrapVisualFlagsLocation = 0;
		if (iWordWrapSymbols == 0) {
			iWordWrapSymbols = EditWrapSymbolDefaultValue;
		}
		switch (iWordWrapSymbols % 10) {
		case EditWrapSymbolBeforeNearText:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_END;
			wrapVisualFlagsLocation |= SC_WRAPVISUALFLAGLOC_END_BY_TEXT;
			break;
		case EditWrapSymbolBeforeNearBorder:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_END;
			break;
		}
		switch (iWordWrapSymbols / 10) {
		case EditWrapSymbolAfterNearText:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_START;
			wrapVisualFlagsLocation |= SC_WRAPVISUALFLAGLOC_START_BY_TEXT;
			break;
		case EditWrapSymbolAfterNearBorder:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_START;
			break;
		case EditWrapSymbolLineNumberMargin:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_MARGIN;
			if (!bShowLineNumbers) {
				bShowLineNumbers = TRUE;
				UpdateLineNumberWidth();
			}
			break;
		}
		SciCall_SetWrapVisualFlagsLocation(wrapVisualFlagsLocation);
		SciCall_SetWrapVisualFlags(wrapVisualFlags);
	} else {
		SciCall_SetWrapVisualFlags(SC_WRAPVISUALFLAG_NONE);
	}
}

static void EditFrameOnThemeChanged(void) {
	if (IsAppThemed()) {
		SetWindowExStyle(hwndEdit, GetWindowExStyle(hwndEdit) & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwndEdit, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

		if (IsVistaAndAbove()) {
			cxEditFrame = 0;
			cyEditFrame = 0;
		} else {
			RECT rc;
			RECT rc2;
			SetWindowPos(hwndEditFrame, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			GetClientRect(hwndEditFrame, &rc);
			GetWindowRect(hwndEditFrame, &rc2);

			cxEditFrame = ((rc2.right - rc2.left) - (rc.right - rc.left)) / 2;
			cyEditFrame = ((rc2.bottom - rc2.top) - (rc.bottom - rc.top)) / 2;
		}
	} else {
		SetWindowExStyle(hwndEdit, GetWindowExStyle(hwndEdit) | WS_EX_CLIENTEDGE);
		SetWindowPos(hwndEdit, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

		cxEditFrame = 0;
		cyEditFrame = 0;
	}
}

//=============================================================================
//
// EditCreate()
//
HWND EditCreate(HWND hwndParent) {
	HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
						  L"Scintilla",
						  NULL,
						  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						  0, 0, 0, 0,
						  hwndParent,
						  (HMENU)IDC_EDIT,
						  g_hInstance,
						  NULL);

	InitScintillaHandle(hwnd);
	Style_InitDefaultColor();
	SciCall_SetTechnology(iRenderingTechnology);
	SciCall_SetBidirectional(iBidirectional);
	SciCall_SetIMEInteraction(bUseInlineIME);
	SciCall_SetPasteConvertEndings(TRUE);
	SciCall_SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
	SciCall_SetCommandEvents(FALSE);
	SciCall_UsePopUp(SC_POPUP_NEVER);
	SciCall_SetScrollWidthTracking(TRUE);
	SciCall_SetEndAtLastLine(iEndAtLastLine);
	SciCall_SetCaretSticky(SC_CARETSTICKY_OFF);
	SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
	SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	SciCall_SetMultipleSelection(TRUE);
	SciCall_SetAdditionalSelectionTyping(TRUE);
	SciCall_SetMultiPaste(SC_MULTIPASTE_EACH);
	SciCall_SetVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);
	SciCall_SetAdditionalCaretsBlink(TRUE);
	SciCall_SetAdditionalCaretsVisible(TRUE);
	// style both before and after the visible text in the background
	SciCall_SetIdleStyling(SC_IDLESTYLING_ALL);

	SciCall_AssignCmdKey((SCK_NEXT + (SCMOD_CTRL << 16)), SCI_PARADOWN);
	SciCall_AssignCmdKey((SCK_PRIOR + (SCMOD_CTRL << 16)), SCI_PARAUP);
	SciCall_AssignCmdKey((SCK_NEXT + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARADOWNEXTEND);
	SciCall_AssignCmdKey((SCK_PRIOR + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARAUPEXTEND);
	SciCall_AssignCmdKey((SCK_HOME + (SCMOD_NORM << 16)), SCI_VCHOMEWRAP);
	SciCall_AssignCmdKey((SCK_END + (SCMOD_NORM << 16)), SCI_LINEENDWRAP);
	SciCall_AssignCmdKey((SCK_HOME + (SCMOD_SHIFT << 16)), SCI_VCHOMEWRAPEXTEND);
	SciCall_AssignCmdKey((SCK_END + (SCMOD_SHIFT << 16)), SCI_LINEENDWRAPEXTEND);

	//SciInitThemes(hwnd);
	if (bEditLayoutRTL) {
		SetWindowLayoutRTL(hwnd, TRUE);
	}

	iRenderingTechnology = SciCall_GetTechnology();
	iBidirectional = SciCall_GetBidirectional();

	SciCall_SetZoom(iZoomLevel);
	// Tabs
	SciCall_SetUseTabs(!bTabsAsSpaces);
	SciCall_SetTabIndents(bTabIndents);
	SciCall_SetBackSpaceUnIndents(bBackspaceUnindents);
	SciCall_SetTabWidth(iTabWidth);
	SciCall_SetIndent(iIndentWidth);

	// Indent Guides
	Style_SetIndentGuides(bShowIndentGuides);

	// Word wrap
	SciCall_SetWrapMode(fWordWrap? iWordWrapMode : SC_WRAP_NONE);
	SetWrapIndentMode();
	SetWrapVisualFlags();

	if (bShowUnicodeControlCharacter) {
		EditShowUnicodeControlCharacter(TRUE);
	}

	// current line
	SciCall_SetCaretLineVisibleAlways(TRUE);

	// Long Lines
	if (bMarkLongLines) {
		SciCall_SetEdgeMode((iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND);
		Style_SetLongLineColors();
	} else {
		SciCall_SetEdgeMode(EDGE_NONE);
	}
	SciCall_SetEdgeColumn(iLongLinesLimit);

	// Margins
	SciCall_SetMarginSensitive(MarginNumber_Bookmark, TRUE);
	SciCall_SetMarginCursor(MarginNumber_Bookmark, SC_CURSORARROW);
	SciCall_SetMarginType(MarginNumber_CodeFolding, SC_MARGIN_SYMBOL);
	SciCall_SetMarginMask(MarginNumber_CodeFolding, SC_MASK_FOLDERS);
	SciCall_SetMarginSensitive(MarginNumber_CodeFolding, TRUE);
	// only select sub line of wrapped line
	SciCall_SetMarginOptions(bWordWrapSelectSubLine ? SC_MARGINOPTION_SUBLINESELECT : SC_MARGINOPTION_NONE);
	// Code folding, Box tree
	SciCall_MarkerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
	SciCall_MarkerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);
#if NP2_DEBUG_FOLD_LEVEL
	SciCall_SetFoldFlags(SC_FOLDFLAG_LEVELNUMBERS);
#else
	// Don't draw folding line below when collapsed.
	SciCall_SetFoldFlags(0);
#endif
	SciCall_FoldDisplayTextSetStyle(SC_FOLDDISPLAYTEXT_BOXED);
	const char *text = GetFoldDisplayEllipsis(SC_CP_UTF8, 0); // internal default encoding
	SciCall_SetDefaultFoldDisplayText(text);
	// highlight current folding block
	SciCall_MarkerEnableHighlight(bHighlightCurrentBlock);
	SciCall_BraceHighlightIndicator(TRUE, IndicatorNumber_MatchBrace);
	SciCall_BraceBadLightIndicator(TRUE, IndicatorNumber_MatchBraceError);

	// CallTips
	SciCall_CallTipUseStyle(TAB_WIDTH_NOTIFICATION);
#if NP2_ENABLE_SHOW_CALLTIPS
	SciCall_SetMouseDwellTime(bShowCallTips? iCallTipsWaitTime : SC_TIME_FOREVER);
#endif

	// Nonprinting characters
	SciCall_SetViewWS((bViewWhiteSpace) ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
	SciCall_SetViewEOL(bViewEOLs);

	return hwnd;
}

void EditReplaceDocument(HANDLE pdoc) {
	const UINT cpEdit = SciCall_GetCodePage();
	SciCall_SetDocPointer(pdoc);
	// reduce reference count to 1
	SciCall_ReleaseDocument(pdoc);
	SciCall_SetCodePage(cpEdit);
	SciCall_SetEOLMode(iEOLMode);

	SciCall_SetUseTabs(!bTabsAsSpaces);
	SciCall_SetTabIndents(bTabIndents);
	SciCall_SetBackSpaceUnIndents(bBackspaceUnindents);
	SciCall_SetTabWidth(iTabWidth);
	SciCall_SetIndent(iIndentWidth);
}

//=============================================================================
//
// MsgCreate() - Handles WM_CREATE
//
//
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
	g_uCurrentDPI = GetWindowDPI(hwnd);
	Style_DetectBaseFontSize(hwnd);

	// Setup edit control
	// create edit control and frame with zero size to avoid
	// a white/black window fades out on startup when using Direct2D.
	hwndEdit = EditCreate(hwnd);
	efrData.hwnd = hwndEdit;

	hwndEditFrame = CreateWindowEx(
						WS_EX_CLIENTEDGE,
						WC_LISTVIEW,
						NULL,
						WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
						0, 0, 0, 0,
						hwnd,
						(HMENU)IDC_EDITFRAME,
						hInstance,
						NULL);

	EditFrameOnThemeChanged();

	// Create Toolbar and Statusbar
	CreateBars(hwnd, hInstance);

	// Window Initialization

	(void)CreateWindow(
		WC_STATIC,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 10, 10,
		hwnd,
		(HMENU)IDC_FILENAME,
		hInstance,
		NULL);

	SetDlgItemText(hwnd, IDC_FILENAME, szCurFile);

	(void)CreateWindow(
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
	pFileMRU = MRU_Create(MRU_KEY_RECENT_FILES, MRUFlags_CaseInsensitive, MRU_MAX_RECENT_FILES);
	MRU_Load(pFileMRU);

	mruFind = MRU_Create(MRU_KEY_RECENT_FIND, MRUFlags_QuoteValue, MRU_MAX_RECENT_FIND);
	MRU_Load(mruFind);

	mruReplace = MRU_Create(MRU_KEY_RECENT_REPLACE, MRUFlags_QuoteValue, MRU_MAX_RECENT_REPLACE);
	MRU_Load(mruReplace);

	if (bInFullScreenMode) {
		bInFullScreenMode = FALSE;
		PostWMCommand(hwnd, IDM_VIEW_TOGGLE_FULLSCREEN);
	}
	return 0;
}

//=============================================================================
//
// CreateBars() - Create Toolbar and Statusbar
//
//
void CreateBars(HWND hwnd, HINSTANCE hInstance) {
	const BOOL bIsAppThemed = IsAppThemed();

	const DWORD dwToolbarStyle = WS_TOOLBAR;
	hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, dwToolbarStyle,
								 0, 0, 0, 0, hwnd, (HMENU)IDC_TOOLBAR, hInstance, NULL);

	SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	BOOL bExternalBitmap = FALSE;
	// Add normal Toolbar Bitmap
	HBITMAP hbmp = NULL;
	if (tchToolbarBitmap != NULL) {
		hbmp = LoadBitmapFile(tchToolbarBitmap);
	}
	if (hbmp != NULL) {
		bExternalBitmap = TRUE;
	} else {
		hbmp = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDR_MAINWND), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	}
	if (bAutoScaleToolbar) {
		hbmp = ResizeToolbarImageForCurrentDPI(hbmp);
	}
	HBITMAP hbmpCopy = NULL;
	if (!bExternalBitmap) {
		hbmpCopy = (HBITMAP)CopyImage(hbmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	}
	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);

	HIMAGELIST himl = ImageList_Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
	ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
	DeleteObject(hbmp);
	SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)himl);

	// Optionally add hot Toolbar Bitmap
	if (tchToolbarBitmapHot != NULL) {
		hbmp = LoadBitmapFile(tchToolbarBitmapHot);
		if (hbmp != NULL) {
			if (bAutoScaleToolbar) {
				hbmp = ResizeToolbarImageForCurrentDPI(hbmp);
			}
			GetObject(hbmp, sizeof(BITMAP), &bmp);
			himl = ImageList_Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
			ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
			DeleteObject(hbmp);
			SendMessage(hwndToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)himl);
		}
	}

	// Optionally add disabled Toolbar Bitmap
	if (tchToolbarBitmapDisabled != NULL) {
		hbmp = LoadBitmapFile(tchToolbarBitmapDisabled);
		if (hbmp != NULL) {
			if (bAutoScaleToolbar) {
				hbmp = ResizeToolbarImageForCurrentDPI(hbmp);
			}
			GetObject(hbmp, sizeof(BITMAP), &bmp);
			himl = ImageList_Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
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
		} else if (flagToolbarLook == 2) {
			fProcessed = BitmapGrayScale(hbmpCopy);
		}
		if (fProcessed) {
			himl = ImageList_Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
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
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_TOOLBAR_LABELS);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, COUNTOF(tbbMainWnd));
	LoadIniSection(INI_SECTION_NAME_TOOLBAR_LABELS, pIniSectionBuf, cchIniSection);
	IniSectionParseArray(pIniSection, pIniSectionBuf, FALSE);
	const int count = pIniSection->count;

	for (int i = 0; i < count; i++) {
		const IniKeyValueNode *node = &pIniSection->nodeList[i];
		const UINT n = (UINT)wcstol(node->key, NULL, 10);
		if (n == 0 || n >= COUNTOF(tbbMainWnd)) {
			continue;
		}

		LPCWSTR tchDesc = node->value;
		if (StrNotEmpty(tchDesc)) {
			tbbMainWnd[n].iString = SendMessage(hwndToolbar, TB_ADDSTRING, 0, (LPARAM)tchDesc);
			tbbMainWnd[n].fsStyle |= BTNS_AUTOSIZE | BTNS_SHOWTEXT;
		} else {
			tbbMainWnd[n].fsStyle &= ~(BTNS_AUTOSIZE | BTNS_SHOWTEXT);
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
#endif // NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS

	SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0,
				SendMessage(hwndToolbar, TB_GETEXTENDEDSTYLE, 0, 0) | TBSTYLE_EX_MIXEDBUTTONS);

	if (Toolbar_SetButtons(hwndToolbar, tchToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd)) == 0) {
		Toolbar_SetButtons(hwndToolbar, DefaultToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd));
	}

	RECT rc;
	SendMessage(hwndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rc);
	//SendMessage(hwndToolbar, TB_SETINDENT, 2, 0);

	GetString(IDS_DOCPOS, cachedStatusItem.tchDocPosFmt, COUNTOF(cachedStatusItem.tchDocPosFmt));
	const DWORD dwStatusbarStyle = bShowStatusbar ? (WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE) : (WS_CHILD | WS_CLIPSIBLINGS);
	hwndStatus = CreateStatusWindow(dwStatusbarStyle, NULL, hwnd, IDC_STATUSBAR);

	// Create ReBar and add Toolbar
	const DWORD dwReBarStyle = bShowToolbar ? (WS_REBAR | WS_VISIBLE) : WS_REBAR;
	hwndReBar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, dwReBarStyle,
							   0, 0, 0, 0, hwnd, (HMENU)IDC_REBAR, hInstance, NULL);

	REBARINFO rbi;
	rbi.cbSize = sizeof(REBARINFO);
	rbi.fMask = 0;
	rbi.himl = (HIMAGELIST)NULL;
	SendMessage(hwndReBar, RB_SETBARINFO, 0, (LPARAM)&rbi);

	REBARBANDINFO rbBand;
	rbBand.cbSize = sizeof(REBARBANDINFO);
	rbBand.fMask = /*RBBIM_COLORS | RBBIM_TEXT | RBBIM_BACKGROUND | */
		RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE /*| RBBIM_SIZE*/;
	rbBand.fStyle	 = /*RBBS_CHILDEDGE |*//* RBBS_BREAK |*/ RBBS_FIXEDSIZE /*| RBBS_GRIPPERALWAYS*/;
	if (bIsAppThemed) {
		rbBand.fStyle |= RBBS_CHILDEDGE;
	}
	rbBand.hbmBack = NULL;
	rbBand.lpText = (LPWSTR)L"Toolbar";
	rbBand.hwndChild = hwndToolbar;
	rbBand.cxMinChild = (rc.right - rc.left) * COUNTOF(tbbMainWnd);
	rbBand.cyMinChild = (rc.bottom - rc.top) + 2 * rc.top;
	rbBand.cx		= 0;
	SendMessage(hwndReBar, RB_INSERTBAND, (WPARAM)(-1), (LPARAM)&rbBand);

	SetWindowPos(hwndReBar, NULL, 0, 0, 0, 0, SWP_NOZORDER);
	GetWindowRect(hwndReBar, &rc);
	cyReBar = rc.bottom - rc.top;

	cyReBarFrame = bIsAppThemed ? 0 : 2;
}

void RecreateBars(HWND hwnd, HINSTANCE hInstance) {
	cachedStatusItem.updateMask = UINT_MAX;
	Toolbar_GetButtons(hwndToolbar, TOOLBAR_COMMAND_BASE, tchToolbarButtons, COUNTOF(tchToolbarButtons));

	DestroyWindow(hwndToolbar);
	DestroyWindow(hwndReBar);
	DestroyWindow(hwndStatus);
	CreateBars(hwnd, hInstance);
}

//=============================================================================
//
// MsgDPIChanged() - Handle WM_DPICHANGED
//
//
void MsgDPIChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	g_uCurrentDPI = HIWORD(wParam);
	const RECT* const rc = (RECT *)lParam;

	// recreate toolbar and statusbar
	RecreateBars(hwnd, g_hInstance);
	SetWindowPos(hwnd, NULL, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, SWP_NOZORDER | SWP_NOACTIVATE);

	Style_DetectBaseFontSize(hwnd);
	UpdateStatusBarWidth();
	Style_OnDPIChanged(pLexCurrent);
	SendMessage(hwndEdit, WM_DPICHANGED, wParam, lParam);
	UpdateLineNumberWidth();
	UpdateBookmarkMarginWidth();
	UpdateFoldMarginWidth();
	EditEnsureSelectionVisible();
	UpdateToolbar();
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

	HINSTANCE hInstance = GetWindowInstance(hwnd);

	// reinitialize edit frame
	EditFrameOnThemeChanged();

	// recreate toolbar and statusbar
	RecreateBars(hwnd, hInstance);

	RECT rc;
	GetClientRect(hwnd, &rc);
	SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
	UpdateToolbar();
	UpdateStatusbar();
}

static void OnStyleThemeChanged(int theme) {
	if (theme == np2StyleTheme) {
		return;
	}
	Style_OnStyleThemeChanged(theme);
}

//=============================================================================
//
// MsgSize() - Handles WM_SIZE
//
//
void MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);

	if (wParam == SIZE_MINIMIZED) {
		return;
	}

	const int x = 0;
	int y = 0;

	const int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

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
		RECT rc;
		GetWindowRect(hwndStatus, &rc);
		cy -= (rc.bottom - rc.top);
	}

	HDWP hdwp = BeginDeferWindowPos(2);

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
		cachedStatusItem.pszLexerName = Style_GetCurrentLexerName(cachedStatusItem.tchLexerName, MAX_EDITLEXER_NAME_SIZE);
		cachedStatusItem.updateMask |= StatusBarUpdateMask_Lexer;
		UpdateStatusBarWidth();
		break;

	case STATUS_CODEPAGE:
		Encoding_GetLabel(iEncoding);
		cachedStatusItem.updateMask |= StatusBarUpdateMask_CodePage;
		UpdateStatusBarWidth();
		break;

	case STATUS_EOLMODE:
		cachedStatusItem.pszEOLMode = (iEOLMode == SC_EOL_LF) ? L"LF" : ((iEOLMode == SC_EOL_CR) ? L"CR" : L"CR+LF");
		cachedStatusItem.updateMask |= StatusBarUpdateMask_EOLMode;
		break;

	case STATUS_DOCZOOM:
		wsprintf(cachedStatusItem.tchZoom, L"%i%%", iZoomLevel);
		cachedStatusItem.updateMask |= StatusBarUpdateMask_DocZoom;
		break;
	}
}

void UpdateStatusBarWidth(void) {
	if (!bInitDone) {
		return;
	}
	int aWidth[7];

	RECT rc;
	GetClientRect(hwndMain, &rc);
	const int cx = rc.right - rc.left;
	const Sci_Position iBytes = SciCall_GetLength();

	aWidth[1] = StatusCalcPaneWidth(hwndStatus, cachedStatusItem.pszLexerName) + 4;
	aWidth[2] = StatusCalcPaneWidth(hwndStatus, mEncoding[iEncoding].wchLabel) + 4;
	aWidth[3] = StatusCalcPaneWidth(hwndStatus, L"CR+LF");
	aWidth[4] = StatusCalcPaneWidth(hwndStatus, L"OVR");
	aWidth[5] = StatusCalcPaneWidth(hwndStatus, L"500%");
	aWidth[6] = StatusCalcPaneWidth(hwndStatus, ((iBytes < 1024)? L"1,023 Bytes" : L"99.9 MiB"))
		+ SystemMetricsForDpi(SM_CXHTHUMB, g_uCurrentDPI);

	aWidth[0] = max_i(120, cx - (aWidth[1] + aWidth[2] + aWidth[3] + aWidth[4] + aWidth[5] + aWidth[6]));
	aWidth[1] += aWidth[0];
	aWidth[2] += aWidth[1];
	aWidth[3] += aWidth[2];
	aWidth[4] += aWidth[3];
	aWidth[5] += aWidth[4];
	aWidth[6] = -1;

	SendMessage(hwndStatus, SB_SETPARTS, COUNTOF(aWidth), (LPARAM)aWidth);
}

#if NP2_ENABLE_APP_LOCALIZATION_DLL
void ValidateUILangauge(void) {
	const LANGID subLang = SUBLANGID(uiLanguage);
	switch (PRIMARYLANGID(uiLanguage)) {
	case LANG_ENGLISH:
		languageMenu = IDM_LANG_ENGLISH_US;
		break;
	case LANG_CHINESE:
		languageMenu = IsChineseTraditionalSubLang(subLang)? IDM_LANG_CHINESE_TRADITIONAL : IDM_LANG_CHINESE_SIMPLIFIED;
		break;
	case LANG_GERMAN:
		languageMenu = IDM_LANG_GERMAN;
		break;
	case LANG_ITALIAN:
		languageMenu = IDM_LANG_ITALIAN;
		break;
	case LANG_JAPANESE:
		languageMenu = IDM_LANG_JAPANESE;
		break;
	case LANG_NEUTRAL:
	default:
		languageMenu = IDM_LANG_USER_DEFAULT;
		uiLanguage = LANG_USER_DEFAULT;
		break;
	}
}

void SetUILanguage(int menu) {
	LANGID lang = uiLanguage;
	switch (menu) {
	case IDM_LANG_USER_DEFAULT:
		lang = LANG_USER_DEFAULT;
		break;
	case IDM_LANG_ENGLISH_US:
		lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		break;
	case IDM_LANG_CHINESE_SIMPLIFIED:
		lang = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
		break;
	case IDM_LANG_CHINESE_TRADITIONAL:
		lang = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL);
		break;
	case LANG_GERMAN:
		lang = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN);
		break;
	case IDM_LANG_ITALIAN:
		lang = MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN);
		break;
	case IDM_LANG_JAPANESE:
		lang = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
		break;
	}

	if (uiLanguage == lang) {
		return;
	}

	const int result = MsgBoxInfo(MB_YESNOCANCEL, IDS_CHANGE_LANG_RESTART);
	if (result != IDCANCEL) {
		languageMenu = menu;
		uiLanguage = lang;
		IniSetInt(INI_SECTION_NAME_FLAGS, L"UILanguage", lang);
		if (result == IDYES) {
			PostWMCommand(hwndMain, IDM_FILE_RESTART);
		}
	}
}
#endif

BOOL IsIMEInNativeMode(void) {
	BOOL result = FALSE;
	HIMC himc = ImmGetContext(hwndEdit);
	if (himc) {
		if (ImmGetOpenStatus(himc)) {
			DWORD dwConversion = IME_CMODE_ALPHANUMERIC;
			DWORD dwSentence = IME_SMODE_NONE;
			if (ImmGetConversionStatus(himc, &dwConversion, &dwSentence)) {
				result = dwConversion != IME_CMODE_ALPHANUMERIC;
			}
		}
		ImmReleaseContext(hwndEdit, himc);
	}
	return result;
}

void MsgNotifyZoom(void) {
	iZoomLevel = SciCall_GetZoom();

#if 0
	char buf[64];
	sprintf(buf, "Zoom: %d%%", iZoomLevel);
	ShowNotificationA(SC_NOTIFICATIONPOSITION_CENTER, buf);
#endif

	// See https://sourceforge.net/p/scintilla/bugs/2118/
	// set minimum visual tab width to 1 when font size smaller than 3.5pt.
	SciCall_SetTabMinimumWidth((iZoomLevel < 40)? 1 : 2);

	UpdateStatusBarCache(STATUS_DOCZOOM);
	Style_OnDPIChanged(pLexCurrent);
	UpdateLineNumberWidth();
	UpdateBookmarkMarginWidth();
	UpdateFoldMarginWidth();
	EditEnsureSelectionVisible();
	UpdateStatusbar();
}

//=============================================================================
//
// MsgInitMenu() - Handles WM_INITMENU
//
//
void MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	HMENU hmenu = (HMENU)wParam;

	int i = StrNotEmpty(szCurFile);
	EnableCmd(hmenu, IDM_FILE_SAVE, IsDocumentModified());
	EnableCmd(hmenu, IDM_FILE_REVERT, i);
	EnableCmd(hmenu, CMD_RELOADUTF8, i);
	EnableCmd(hmenu, CMD_RELOADANSI, i);
	EnableCmd(hmenu, CMD_RELOADOEM, i);
	EnableCmd(hmenu, CMD_RELOADNOFILEVARS, i);
	EnableCmd(hmenu, CMD_RECODEDEFAULT, i);
#if defined(_WIN64)
	EnableCmd(hmenu, IDM_FILE_LARGE_FILE_MODE, !bLargeFileMode);
	EnableCmd(hmenu, IDM_FILE_LARGE_FILE_MODE_RELOAD, !bLargeFileMode);
#endif
	EnableCmd(hmenu, IDM_FILE_LAUNCH, i);
	EnableCmd(hmenu, IDM_FILE_PROPERTIES, i);
	EnableCmd(hmenu, IDM_FILE_CREATELINK, i);
	EnableCmd(hmenu, IDM_FILE_ADDTOFAV, i);

	EnableCmd(hmenu, IDM_FILE_RELAUNCH_ELEVATED, IsVistaAndAbove() && !fIsElevated);
	EnableCmd(hmenu, IDM_FILE_OPEN_CONTAINING_FOLDER, i);
	EnableCmd(hmenu, IDM_FILE_READONLY, i);
	CheckCmd(hmenu, IDM_FILE_READONLY, bReadOnly);
	CheckCmd(hmenu, IDM_FILE_LOCK_EDITING, bLockedForEditing);

	//EnableCmd(hmenu, IDM_ENCODING_UNICODEREV, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_UNICODE, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_UTF8SIGN, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_UTF8, !bReadOnly);
	//EnableCmd(hmenu, IDM_ENCODING_ANSI, !bReadOnly);
	//EnableCmd(hmenu, IDM_LINEENDINGS_CRLF, !bReadOnly);
	//EnableCmd(hmenu, IDM_LINEENDINGS_LF, !bReadOnly);
	//EnableCmd(hmenu, IDM_LINEENDINGS_CR, !bReadOnly);

	EnableCmd(hmenu, IDM_RECODE_SELECT, i);

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

	EnableCmd(hmenu, IDM_EDIT_UNDO, SciCall_CanUndo() /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_REDO, SciCall_CanRedo() /*&& !bReadOnly*/);

	i = !SciCall_IsSelectionEmpty();
	const BOOL canPaste = SciCall_CanPaste();
	const BOOL nonEmpty = SciCall_GetLength() != 0;

	EnableCmd(hmenu, IDM_EDIT_CUT, nonEmpty /*&& !bReadOnly*/);
	//EnableCmd(hmenu, IDM_EDIT_CUT_BINARY, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_COPY, nonEmpty /*&& !bReadOnly*/);
	//EnableCmd(hmenu, IDM_EDIT_COPY_BINARY, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_COPYALL, nonEmpty /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_SELECTALL, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_COPYADD, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_PASTE, canPaste /*&& !bReadOnly*/);
	//EnableCmd(hmenu, IDM_EDIT_PASTE_BINARY, canPaste /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_SWAP, i || canPaste /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_DELETE, nonEmpty /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_CLEARDOCUMENT, nonEmpty /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_COPYRTF, i /*&& !bReadOnly*/);

	OpenClipboard(hwnd);
	EnableCmd(hmenu, IDM_EDIT_CLEARCLIPBOARD, CountClipboardFormats());
	CloseClipboard();

	EnableCmd(hmenu, CMD_OPEN_PATH_OR_LINK, nonEmpty);
	EnableCmd(hmenu, CMD_OPEN_CONTAINING_FOLDER, nonEmpty);
	EnableCmd(hmenu, CMD_ONLINE_SEARCH_GOOGLE, i);
	EnableCmd(hmenu, CMD_ONLINE_SEARCH_BING, i);
	EnableCmd(hmenu, CMD_ONLINE_SEARCH_WIKI, i);
	EnableCmd(hmenu, CMD_CUSTOM_ACTION1, i);
	EnableCmd(hmenu, CMD_CUSTOM_ACTION2, i);

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

	EnableCmd(hmenu, IDM_EDIT_SORTLINES, EditGetSelectedLineCount() > 1);

	EnableCmd(hmenu, IDM_EDIT_COLUMNWRAP, i /*&& IsWindowsNT()*/);
	EnableCmd(hmenu, IDM_EDIT_SPLITLINES, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_JOINLINES, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_JOINLINESEX, i /*&& !bReadOnly*/);

	EnableCmd(hmenu, IDM_EDIT_CONVERTUPPERCASE, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_CONVERTLOWERCASE, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_INVERTCASE, i /*&& !bReadOnly*/ /*&& IsWindowsNT()*/);
	EnableCmd(hmenu, IDM_EDIT_TITLECASE, i /*&& !bReadOnly*/ /*&& IsWindowsNT()*/);
	EnableCmd(hmenu, IDM_EDIT_SENTENCECASE, i /*&& !bReadOnly*/ /*&& IsWindowsNT()*/);
	EnableCmd(hmenu, IDM_EDIT_MAP_FULLWIDTH, i);
	EnableCmd(hmenu, IDM_EDIT_MAP_HALFWIDTH, i);
	EnableCmd(hmenu, IDM_EDIT_MAP_SIMPLIFIED_CHINESE, i);
	EnableCmd(hmenu, IDM_EDIT_MAP_TRADITIONAL_CHINESE, i);
	EnableCmd(hmenu, IDM_EDIT_MAP_HIRAGANA, i);
	EnableCmd(hmenu, IDM_EDIT_MAP_KATAKANA, i);
	EnableCmd(hmenu, IDM_EDIT_MAP_MALAYALAM_LATIN, i && IsWin7AndAbove());
	EnableCmd(hmenu, IDM_EDIT_MAP_DEVANAGARI_LATIN, i && IsWin7AndAbove());
	EnableCmd(hmenu, IDM_EDIT_MAP_CYRILLIC_LATIN, i && IsWin7AndAbove());
	EnableCmd(hmenu, IDM_EDIT_MAP_BENGALI_LATIN, i && IsWin7AndAbove());
	EnableCmd(hmenu, IDM_EDIT_MAP_HANGUL_DECOMPOSITION, i && IsWin7AndAbove());

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

	EnableCmd(hmenu, IDM_EDIT_LINECOMMENT, bCurrentLexerHasLineComment);
	EnableCmd(hmenu, IDM_EDIT_STREAMCOMMENT, bCurrentLexerHasBlockComment);

	EnableCmd(hmenu, IDM_EDIT_INSERT_ENCODING, *mEncoding[iEncoding].pszParseNames);

	//EnableCmd(hmenu, IDM_EDIT_INSERT_SHORTDATE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_INSERT_LONGDATE, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_INSERT_FILENAME, !bReadOnly);
	//EnableCmd(hmenu, IDM_EDIT_INSERT_PATHNAME, !bReadOnly);

	i = nonEmpty;
	EnableCmd(hmenu, IDM_EDIT_FIND, i);
	EnableCmd(hmenu, IDM_EDIT_SAVEFIND, i);
	EnableCmd(hmenu, IDM_EDIT_FINDNEXT, i);
	EnableCmd(hmenu, IDM_EDIT_FINDPREV, i && StrNotEmptyA(efrData.szFind));
	EnableCmd(hmenu, IDM_EDIT_REPLACE, i /*&& !bReadOnly*/);
	EnableCmd(hmenu, IDM_EDIT_REPLACENEXT, i);
	EnableCmd(hmenu, IDM_EDIT_SELECTWORD, i);
	EnableCmd(hmenu, IDM_EDIT_SELECTLINE, i);
	EnableCmd(hmenu, IDM_EDIT_SELECTLINE_BLOCK, i);
	EnableCmd(hmenu, IDM_EDIT_SELTODOCEND, i);
	EnableCmd(hmenu, IDM_EDIT_SELTODOCSTART, i);
	EnableCmd(hmenu, IDM_EDIT_SELTONEXT, i && StrNotEmptyA(efrData.szFind));
	EnableCmd(hmenu, IDM_EDIT_SELTOPREV, i && StrNotEmptyA(efrData.szFind));
	EnableCmd(hmenu, IDM_EDIT_FINDMATCHINGBRACE, i);
	EnableCmd(hmenu, IDM_EDIT_SELTOMATCHINGBRACE, i);
	EnableCmd(hmenu, CMD_JUMP2SELSTART, i);
	EnableCmd(hmenu, CMD_JUMP2SELEND, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKPREV, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKNEXT, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKTOGGLE, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKSELECT, i);
	EnableCmd(hmenu, BME_EDIT_BOOKMARKCLEAR, i);
	EnableCmd(hmenu, IDM_EDIT_GOTOLINE, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_GOTO_BLOCK_START, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_GOTO_BLOCK_END, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_GOTO_PREVIOUS_BLOCK, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_GOTO_NEXT_BLOCK, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_GOTO_PREV_SIBLING_BLOCK, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_GOTO_NEXT_SIBLING_BLOCK, nonEmpty);
	EnableCmd(hmenu, IDM_EDIT_DELETELINELEFT, i);
	EnableCmd(hmenu, IDM_EDIT_DELETELINERIGHT, i);
	EnableCmd(hmenu, CMD_CTRLBACK, i);
	EnableCmd(hmenu, CMD_CTRLDEL, i);
	EnableCmd(hmenu, CMD_TIMESTAMPS, i);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_DEFAULT, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_CURRENT_BLOCK, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_ALL, i && bShowCodeFolding);
	//EnableCmd(hmenu, IDM_VIEW_FOLD_CURRENT_LEVEL, i && bShowCodeFolding);
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
	CheckCmd(hmenu, IDM_VIEW_SHOW_FOLDING, bShowCodeFolding);

	CheckCmd(hmenu, IDM_VIEW_USE2NDGLOBALSTYLE, bUse2ndGlobalStyle);
	CheckCmd(hmenu, IDM_VIEW_USEDEFAULT_CODESTYLE, pLexCurrent->bUseDefaultCodeStyle);
	i = IDM_VIEW_STYLE_THEME_DEFAULT + np2StyleTheme;
	CheckMenuRadioItem(hmenu, IDM_VIEW_STYLE_THEME_DEFAULT, IDM_VIEW_STYLE_THEME_DARK, i, MF_BYCOMMAND);

	CheckCmd(hmenu, IDM_VIEW_WORDWRAP, fWordWrap);
	i = IDM_VIEW_FONTQUALITY_DEFAULT + iFontQuality;
	CheckMenuRadioItem(hmenu, IDM_VIEW_FONTQUALITY_DEFAULT, IDM_VIEW_FONTQUALITY_CLEARTYPE, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_CARET_STYLE_BLOCK_OVR, iOvrCaretStyle);
	i = IDM_VIEW_CARET_STYLE_BLOCK + iCaretStyle;
	CheckMenuRadioItem(hmenu, IDM_VIEW_CARET_STYLE_BLOCK, IDM_VIEW_CARET_STYLE_WIDTH3, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_CARET_STYLE_NOBLINK, iCaretBlinkPeriod == 0);
	CheckCmd(hmenu, IDM_VIEW_CARET_STYLE_SELECTION, !bBlockCaretOutSelection);
	CheckCmd(hmenu, IDM_VIEW_LONGLINEMARKER, bMarkLongLines);
	CheckCmd(hmenu, IDM_VIEW_TABSASSPACES, bTabsAsSpaces);
	CheckCmd(hmenu, IDM_VIEW_SHOWINDENTGUIDES, bShowIndentGuides);
	CheckCmd(hmenu, IDM_VIEW_LINENUMBERS, bShowLineNumbers);
	CheckCmd(hmenu, IDM_VIEW_MARGIN, bShowBookmarkMargin);
	EnableCmd(hmenu, IDM_EDIT_COMPLETEWORD, i);
	CheckCmd(hmenu, IDM_VIEW_AUTOCOMPLETION_IGNORECASE, autoCompletionConfig.bIgnoreCase);
	CheckCmd(hmenu, IDM_SET_LINE_SELECTION_MODE, bEnableLineSelectionMode);

	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_OFF, !bMarkOccurrences);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_CASE, bMarkOccurrencesMatchCase);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_WORD, bMarkOccurrencesMatchWords);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_BOOKMARK, bMarkOccurrencesBookmark);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_CASE, bMarkOccurrences);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_WORD, bMarkOccurrences);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_BOOKMARK, bMarkOccurrences);

	CheckCmd(hmenu, IDM_VIEW_SHOWWHITESPACE, bViewWhiteSpace);
	CheckCmd(hmenu, IDM_VIEW_SHOWEOLS, bViewEOLs);
	CheckCmd(hmenu, IDM_VIEW_WORDWRAPSYMBOLS, bShowWordWrapSymbols);
	CheckCmd(hmenu, IDM_VIEW_UNICODE_CONTROL_CHAR, bShowUnicodeControlCharacter);
#if NP2_ENABLE_SHOW_CALLTIPS
	CheckCmd(hmenu, IDM_VIEW_SHOWCALLTIPS, bShowCallTips);
#endif
	CheckCmd(hmenu, IDM_VIEW_TOOLBAR, bShowToolbar);
	EnableCmd(hmenu, IDM_VIEW_CUSTOMIZE_TOOLBAR, bShowToolbar);
	CheckCmd(hmenu, IDM_VIEW_AUTO_SCALE_TOOLBAR, bAutoScaleToolbar);
	CheckCmd(hmenu, IDM_VIEW_STATUSBAR, bShowStatusbar);
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	CheckMenuRadioItem(hmenu, IDM_LANG_USER_DEFAULT, IDM_LANG_LAST_LANGUAGE, languageMenu, MF_BYCOMMAND);
#endif
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_ON_START, iFullScreenMode & FullScreenMode_OnStartup);
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_HIDE_TITLE, iFullScreenMode & FullScreenMode_HideCaption);
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_HIDE_MENU, iFullScreenMode & FullScreenMode_HideMenu);

	CheckCmd(hmenu, IDM_VIEW_MATCHBRACES, bMatchBraces);
	CheckCmd(hmenu, IDM_VIEW_HIGHLIGHTCURRENT_BLOCK, bHighlightCurrentBlock);
	i = IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE + iHighlightCurrentLine;
	CheckMenuRadioItem(hmenu, IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE, IDM_VIEW_HIGHLIGHTCURRENTLINE_FRAME, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_HIGHLIGHTCURRENTLINE_SUBLINE, bHighlightCurrentSubLine);

	CheckCmd(hmenu, IDM_VIEW_REUSEWINDOW, bReuseWindow);
	CheckCmd(hmenu, IDM_VIEW_SINGLEFILEINSTANCE, bSingleFileInstance);
	CheckCmd(hmenu, IDM_VIEW_ALWAYSONTOP, IsTopMost());
	CheckCmd(hmenu, IDM_VIEW_MINTOTRAY, bMinimizeToTray);
	CheckCmd(hmenu, IDM_VIEW_TRANSPARENT, bTransparentMode);
	EnableCmd(hmenu, IDM_VIEW_TRANSPARENT, i);
	i = IDM_VIEW_SCROLLPASTLASTLINE_ONE + iEndAtLastLine;
	CheckMenuRadioItem(hmenu, IDM_VIEW_SCROLLPASTLASTLINE_ONE, IDM_VIEW_SCROLLPASTLASTLINE_QUARTER, i, MF_BYCOMMAND);

	// Rendering Technology
	i = IsVistaAndAbove();
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2D, i);
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2DRETAIN, i);
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2DDC, i);
	i = IDM_SET_RENDER_TECH_GDI + iRenderingTechnology;
	CheckMenuRadioItem(hmenu, IDM_SET_RENDER_TECH_GDI, IDM_SET_RENDER_TECH_D2DDC, i, MF_BYCOMMAND);
	// RTL Layout
	EnableCmd(hmenu, IDM_SET_RTL_LAYOUT_EDIT, iRenderingTechnology == SC_TECHNOLOGY_DEFAULT);
	CheckCmd(hmenu, IDM_SET_RTL_LAYOUT_EDIT, bEditLayoutRTL);
	CheckCmd(hmenu, IDM_SET_RTL_LAYOUT_OTHER, bWindowLayoutRTL);
	// Bidirectional
	i = iRenderingTechnology != SC_TECHNOLOGY_DEFAULT;
	EnableCmd(hmenu, IDM_SET_BIDIRECTIONAL_L2R, i);
	EnableCmd(hmenu, IDM_SET_BIDIRECTIONAL_R2L, i);
	i = IDM_SET_BIDIRECTIONAL_NONE + iBidirectional;
	CheckMenuRadioItem(hmenu, IDM_SET_BIDIRECTIONAL_NONE, IDM_SET_BIDIRECTIONAL_R2L, i, MF_BYCOMMAND);

	CheckCmd(hmenu, IDM_SET_USE_INLINE_IME, bUseInlineIME);

	CheckCmd(hmenu, IDM_VIEW_NOSAVERECENT, bSaveRecentFiles);
	CheckCmd(hmenu, IDM_VIEW_NOSAVEFINDREPL, bSaveFindReplace);
	CheckCmd(hmenu, IDM_VIEW_SAVEBEFORERUNNINGTOOLS, bSaveBeforeRunningTools);
	CheckCmd(hmenu, IDM_SET_OPEN_FOLDER_METAPATH, bOpenFolderWithMetapath);

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
	EnableCmd(hmenu, IDM_VIEW_SINGLEFILEINSTANCE, i);
	EnableCmd(hmenu, IDM_VIEW_NOSAVERECENT, i);
	EnableCmd(hmenu, IDM_VIEW_NOSAVEFINDREPL, i);
	EnableCmd(hmenu, IDM_VIEW_SAVESETTINGS, i);

	i = i || StrNotEmpty(szIniFile2);
	EnableCmd(hmenu, IDM_VIEW_SAVESETTINGSNOW, i);

	Style_UpdateSchemeMenu(hmenu);
}

static void ConvertLineEndings(int iNewEOLMode) {
	iEOLMode = iNewEOLMode;
	SciCall_SetEOLMode(iNewEOLMode);
	EditEnsureConsistentLineEndings();
	UpdateStatusBarCache(STATUS_EOLMODE);
	UpdateToolbar();
	UpdateStatusbar();
	UpdateWindowTitle();
}

static inline BOOL IsBraceMatchChar(int ch) {
#if 0
	return ch == '(' || ch == ')'
		|| ch == '[' || ch == ']'
		|| ch == '{' || ch == '}'
		|| ch == '<' || ch == '>';
#else
	// tools/GenerateTable.py
#if defined(_WIN64)
	static const uint64_t table[4] = { UINT64_C(0x5000030000000000), UINT64_C(0x2800000028000000) };
	return (table[ch >> 6] >> (ch & 63)) & 1;
#else
	static const uint32_t table[8] = { 0, 0x50000300, 0x28000000, 0x28000000 };
	return (table[ch >> 5] >> (ch & 31)) & 1;
#endif
#endif
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

	case IDM_FILE_REVERT:
		if (StrNotEmpty(szCurFile)) {
			if (IsDocumentModified() && MsgBoxWarn(MB_OKCANCEL, IDS_ASK_REVERT) != IDOK) {
				return 0;
			}

			iWeakSrcEncoding = iEncoding;
			FileLoad(TRUE, FALSE, TRUE, FALSE, szCurFile);
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
		if (StrNotEmpty(szCurFile)) {
			DWORD dwFileAttributes = GetFileAttributes(szCurFile);
			if (dwFileAttributes != INVALID_FILE_ATTRIBUTES) {
				if (bReadOnly) {
					dwFileAttributes = (dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
				} else {
					dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
				}
				if (!SetFileAttributes(szCurFile, dwFileAttributes)) {
					MsgBoxWarn(MB_OK, IDS_READONLY_MODIFY, szCurFile);
				}
			} else {
				MsgBoxWarn(MB_OK, IDS_READONLY_MODIFY, szCurFile);
			}

			dwFileAttributes = GetFileAttributes(szCurFile);
			bReadOnly = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);
			UpdateWindowTitle();
		}
		break;

	case IDM_FILE_LOCK_EDITING:
		bLockedForEditing = !bLockedForEditing;
		SciCall_SetReadOnly(bLockedForEditing);
		UpdateWindowTitle();
		break;

	case IDM_FILE_BROWSE:
		TryBrowseFile(hwnd, szCurFile, TRUE);
		break;

	case IDM_FILE_NEWWINDOW:
	case IDM_FILE_NEWWINDOW2:
	case IDM_FILE_RESTART: {
		const BOOL emptyWind = LOWORD(wParam) == IDM_FILE_NEWWINDOW2;
		if (!emptyWind && bSaveBeforeRunningTools && !FileSave(FALSE, TRUE, FALSE, FALSE)) {
			break;
		}

		WCHAR szModuleName[MAX_PATH];
		GetModuleFileName(NULL, szModuleName, COUNTOF(szModuleName));
		LPWSTR szParameters = (LPWSTR)NP2HeapAlloc(sizeof(WCHAR) * 1024);
		GetRelaunchParameters(szParameters, szCurFile, TRUE, emptyWind);

		SHELLEXECUTEINFO sei;
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOZONECHECKS;
		sei.hwnd = hwnd;
		sei.lpVerb = NULL;
		sei.lpFile = szModuleName;
		sei.lpParameters = szParameters;
		sei.lpDirectory = g_wchWorkingDirectory;
		sei.nShow = SW_SHOWNORMAL;

		const BOOL result = ShellExecuteEx(&sei);
		NP2HeapFree(szParameters);
		if (result && LOWORD(wParam) == IDM_FILE_RESTART) {
			DestroyWindow(hwnd);
		}
	}
	break;

	case IDM_FILE_RELAUNCH_ELEVATED:
		flagRelaunchElevated = 2;
		if (RelaunchElevated()) {
			DestroyWindow(hwnd);
		}
		break;

	case IDM_FILE_OPEN_CONTAINING_FOLDER:
		OpenContainingFolder(hwnd, szCurFile, TRUE);
		break;

	case IDM_FILE_LAUNCH: {
		if (StrIsEmpty(szCurFile)) {
			break;
		}

		if (bSaveBeforeRunningTools && !FileSave(FALSE, TRUE, FALSE, FALSE)) {
			break;
		}

		WCHAR wchDirectory[MAX_PATH] = L"";
		if (StrNotEmpty(szCurFile)) {
			lstrcpy(wchDirectory, szCurFile);
			PathRemoveFileSpec(wchDirectory);
		}

		SHELLEXECUTEINFO sei;
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
		if (bSaveBeforeRunningTools && !FileSave(FALSE, TRUE, FALSE, FALSE)) {
			break;
		}

		WCHAR tchCmdLine[MAX_PATH + 4];
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
		WCHAR tchUntitled[128];

		if (StrNotEmpty(szCurFile)) {
			SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			pszTitle = shfi.szDisplayName;
		} else {
			GetString(IDS_UNTITLED, tchUntitled, COUNTOF(tchUntitled));
			pszTitle = tchUntitled;
		}

		if (!EditPrint(hwndEdit, pszTitle)) {
			MsgBoxWarn(MB_OK, IDS_PRINT_ERROR, pszTitle);
		}
	}
	break;

	case IDM_FILE_PROPERTIES: {
		if (StrIsEmpty(szCurFile)) {
			break;
		}

		SHELLEXECUTEINFO sei;
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
			MsgBoxWarn(MB_OK, IDS_ERR_CREATELINK);
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

					if (OpenFileDlg(hwnd, tchFile, COUNTOF(tchFile), tchSelItem)) {
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
		sei.lpVerb = L"open";
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
		ExitApplication(hwnd);
		break;

	case IDM_ENCODING_ANSI:
	case IDM_ENCODING_UNICODE:
	case IDM_ENCODING_UNICODEREV:
	case IDM_ENCODING_UTF8:
	case IDM_ENCODING_UTF8SIGN:
	case IDM_ENCODING_SELECT: {
		int iNewEncoding = iEncoding;
		if (LOWORD(wParam) == IDM_ENCODING_SELECT && !SelectEncodingDlg(hwnd, &iNewEncoding, IDS_SELRECT_CURRENT_ENCODING)) {
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

		if (EditSetNewEncoding(iEncoding, iNewEncoding, (flagSetEncoding), StrIsEmpty(szCurFile))) {
			if (SciCall_GetLength() == 0) {
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

	case IDM_RECODE_SELECT:
		if (StrNotEmpty(szCurFile)) {
			int iNewEncoding;
			switch (iEncoding) {
			case CPI_DEFAULT:
				iNewEncoding = -1;
				break;
			case CPI_UTF8SIGN:
				iNewEncoding = CPI_UTF8;
				break;
			case CPI_UNICODEBOM:
				iNewEncoding = CPI_UNICODE;
				break;
			case CPI_UNICODEBEBOM:
				iNewEncoding = CPI_UNICODEBE;
				break;
			default:
				iNewEncoding = iEncoding;
				break;
			}

			if (IsDocumentModified() && MsgBoxWarn(MB_OKCANCEL, IDS_ASK_RECODE) != IDOK) {
				return 0;
			}

			if (SelectEncodingDlg(hwnd, &iNewEncoding, IDS_SELRECT_RELOAD_ENCODING)) {
				iSrcEncoding = iNewEncoding;
				FileLoad(TRUE, FALSE, TRUE, FALSE, szCurFile);
			}
		}
		break;

	case IDM_ENCODING_SETDEFAULT:
		SelectDefEncodingDlg(hwnd, &iDefaultEncoding);
		break;

	case IDM_LINEENDINGS_CRLF:
	case IDM_LINEENDINGS_LF:
	case IDM_LINEENDINGS_CR: {
		const int iNewEOLMode = GetScintillaEOLMode(LOWORD(wParam) - IDM_LINEENDINGS_CRLF);
		ConvertLineEndings(iNewEOLMode);
	}
	break;

	case IDM_LINEENDINGS_SETDEFAULT:
		SelectDefLineEndingDlg(hwnd, &iDefaultEOLMode);
		break;

	case IDM_EDIT_UNDO:
		SciCall_Undo();
		break;

	case IDM_EDIT_REDO:
		SciCall_Redo();
		break;

	case IDM_EDIT_CUT:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		if (SciCall_IsSelectionEmpty()) {
			Sci_Position iCurrentPos = SciCall_GetCurrentPos();
			const Sci_Position iCol = SciCall_GetColumn(iCurrentPos) + 1;
			SciCall_LineCut(bEnableLineSelectionMode);
			iCurrentPos = SciCall_GetCurrentPos();
			const Sci_Line iCurLine = SciCall_LineFromPosition(iCurrentPos);
			EditJumpTo(iCurLine, iCol);
		} else {
			SciCall_Cut(FALSE);
		}
		break;

	//case IDM_EDIT_CUT_BINARY:
	//	if (flagPasteBoard) {
	//		bLastCopyFromMe = TRUE;
	//	}
	//	SciCall_Cut(TRUE);
	//	break;

	case IDM_EDIT_COPY:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		if (SciCall_IsSelectionEmpty()) {
			SciCall_LineCopy(bEnableLineSelectionMode);
		} else {
			SciCall_Copy(FALSE);
		}
		UpdateToolbar();
		break;

	//case IDM_EDIT_COPY_BINARY:
	//	if (flagPasteBoard) {
	//		bLastCopyFromMe = TRUE;
	//	}
	//	SciCall_Copy(TRUE);
	//	UpdateToolbar();
	//	break;

	case IDM_EDIT_COPYALL:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SciCall_CopyRange(0, SciCall_GetLength());
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
	//case IDM_EDIT_PASTE_BINARY:
		SciCall_Paste(LOWORD(wParam) == IDM_EDIT_PASTE_BINARY);
		break;

	case IDM_EDIT_SWAP:
		if (SciCall_IsSelectionEmpty()) {
			const Sci_Position iPos = SciCall_GetCurrentPos();
			SciCall_Paste(FALSE);
			const Sci_Position iNewPos = SciCall_GetCurrentPos();
			SciCall_SetSel(iPos, iNewPos);
			SendWMCommand(hwnd, IDM_EDIT_CLEARCLIPBOARD);
		} else {
			char *pClip = EditGetClipboardText(hwndEdit);
			if (pClip == NULL) {
				break;
			}
			if (flagPasteBoard) {
				bLastCopyFromMe = TRUE;
			}
			const Sci_Position iPos = SciCall_GetCurrentPos();
			const Sci_Position iAnchor = SciCall_GetAnchor();
			SciCall_BeginUndoAction();
			SciCall_Cut(FALSE);
			SciCall_ReplaceSel(pClip);
			if (iPos > iAnchor) {
				SciCall_SetSel(iAnchor, iAnchor + strlen(pClip));
			} else {
				SciCall_SetSel(iPos + strlen(pClip), iPos);
			}
			SciCall_EndUndoAction();
			LocalFree(pClip);
		}
		break;

	case IDM_EDIT_DELETE:
		SciCall_Clear();
		break;

	case IDM_EDIT_CLEARDOCUMENT:
		SciCall_ClearAll();
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
		SciCall_SelectAll();
		break;

	case IDM_EDIT_SELECTWORD:
		EditSelectWord();
		break;

	case IDM_EDIT_SELECTLINE:
	case IDM_EDIT_SELECTLINE_BLOCK:
		EditSelectLines(LOWORD(wParam) == IDM_EDIT_SELECTLINE_BLOCK, bEnableLineSelectionMode);
		break;

	case IDM_EDIT_MOVELINEUP:
		EditMoveUp();
		break;

	case IDM_EDIT_MOVELINEDOWN:
		EditMoveDown();
		break;

	case IDM_EDIT_LINETRANSPOSE:
		SciCall_LineTranspose();
		break;

	case IDM_EDIT_DUPLICATELINE:
		SciCall_LineDuplicate();
		break;

	case IDM_EDIT_CUTLINE:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SciCall_LineCut(bEnableLineSelectionMode);
		break;

	case IDM_EDIT_COPYLINE:
		if (flagPasteBoard) {
			bLastCopyFromMe = TRUE;
		}
		SciCall_LineCopy(bEnableLineSelectionMode);
		UpdateToolbar();
		break;

	case IDM_EDIT_DELETELINE:
		SciCall_LineDelete();
		break;

	case IDM_EDIT_DELETELINELEFT:
		SciCall_DelLineLeft();
		break;

	case IDM_EDIT_DELETELINERIGHT:
		SciCall_DelLineRight();
		break;

	case IDM_EDIT_INDENT: {
		const Sci_Line iLineSelStart = SciCall_LineFromPosition(SciCall_GetSelectionStart());
		const Sci_Line iLineSelEnd = SciCall_LineFromPosition(SciCall_GetSelectionEnd());

		SciCall_SetTabIndents(TRUE);
		if (iLineSelStart == iLineSelEnd) {
			SciCall_VCHome();
		}
		SciCall_Tab();
		SciCall_SetTabIndents(bTabIndents);
	}
	break;

	case IDM_EDIT_UNINDENT: {
		const Sci_Line iLineSelStart = SciCall_LineFromPosition(SciCall_GetSelectionStart());
		const Sci_Line iLineSelEnd = SciCall_LineFromPosition(SciCall_GetSelectionEnd());

		SciCall_SetTabIndents(TRUE);
		if (iLineSelStart == iLineSelEnd) {
			SciCall_VCHome();
		}
		SciCall_BackTab();
		SciCall_SetTabIndents(bTabIndents);
	}
	break;

	case IDM_EDIT_ENCLOSESELECTION:
		EditEncloseSelectionDlg(hwnd);
		break;

	case IDM_EDIT_SELECTIONDUPLICATE:
		SciCall_BeginUndoAction();
		SciCall_SelectionDuplicate();
		SciCall_EndUndoAction();
		break;

	case IDM_EDIT_PADWITHSPACES:
		BeginWaitCursor();
		EditPadWithSpaces(FALSE, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_STRIP1STCHAR:
		BeginWaitCursor();
		EditStripFirstCharacter();
		EndWaitCursor();
		break;

	case IDM_EDIT_STRIPLASTCHAR:
		BeginWaitCursor();
		EditStripLastCharacter();
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
		EditCompressSpaces();
		EndWaitCursor();
		break;

	case IDM_EDIT_MERGEBLANKLINES:
		BeginWaitCursor();
		EditRemoveBlankLines(TRUE);
		EndWaitCursor();
		break;

	case IDM_EDIT_REMOVEBLANKLINES:
		BeginWaitCursor();
		EditRemoveBlankLines(FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_MODIFYLINES:
		EditModifyLinesDlg(hwnd);
		break;

	case IDM_EDIT_ALIGN:
		if (EditAlignDlg(hwnd, &iAlignMode)) {
			BeginWaitCursor();
			EditAlignText(iAlignMode);
			EndWaitCursor();
		}
		break;

	case IDM_EDIT_SORTLINES:
		if (EditSortDlg(hwnd, &iSortOptions)) {
			BeginWaitCursor();
			StatusSetText(hwndStatus, STATUS_HELP, L"...");
			StatusSetSimple(hwndStatus, TRUE);
			InvalidateRect(hwndStatus, NULL, TRUE);
			UpdateWindow(hwndStatus);
			EditSortLines(iSortOptions);
			StatusSetSimple(hwndStatus, FALSE);
			EndWaitCursor();
		}
		break;

	case IDM_EDIT_COLUMNWRAP: {
		if (iWrapCol == 0) {
			iWrapCol = iLongLinesLimit;
		}

		if (ColumnWrapDlg(hwnd, &iWrapCol)) {
			iWrapCol = clamp_i(iWrapCol, 1, 512);
			BeginWaitCursor();
			EditWrapToColumn(iWrapCol);
			EndWaitCursor();
		}
	}
	break;

	case IDM_EDIT_SPLITLINES:
		BeginWaitCursor();
		SciCall_TargetFromSelection();
		SciCall_LinesSplit(0);
		EndWaitCursor();
		break;

	case IDM_EDIT_JOINLINES:
		BeginWaitCursor();
		SciCall_TargetFromSelection();
		SciCall_LinesJoin();
		EditJoinLinesEx();
		EndWaitCursor();
		break;

	case IDM_EDIT_JOINLINESEX:
		BeginWaitCursor();
		EditJoinLinesEx();
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTUPPERCASE:
		BeginWaitCursor();
		SciCall_UpperCase();
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTLOWERCASE:
		BeginWaitCursor();
		SciCall_LowerCase();
		EndWaitCursor();
		break;

	case IDM_EDIT_INVERTCASE:
		BeginWaitCursor();
		EditInvertCase();
		EndWaitCursor();
		break;

	case IDM_EDIT_TITLECASE:
	case IDM_EDIT_MAP_FULLWIDTH:
	case IDM_EDIT_MAP_HALFWIDTH:
	case IDM_EDIT_MAP_SIMPLIFIED_CHINESE:
	case IDM_EDIT_MAP_TRADITIONAL_CHINESE:
	case IDM_EDIT_MAP_HIRAGANA:
	case IDM_EDIT_MAP_KATAKANA:
	case IDM_EDIT_MAP_MALAYALAM_LATIN:
	case IDM_EDIT_MAP_DEVANAGARI_LATIN:
	case IDM_EDIT_MAP_CYRILLIC_LATIN:
	case IDM_EDIT_MAP_BENGALI_LATIN:
	case IDM_EDIT_MAP_HANGUL_DECOMPOSITION:
		BeginWaitCursor();
		EditMapTextCase(LOWORD(wParam));
		EndWaitCursor();
		break;

	case IDM_EDIT_SENTENCECASE:
		BeginWaitCursor();
		EditSentenceCase();
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTTABS:
		BeginWaitCursor();
		EditTabsToSpaces(iTabWidth, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTSPACES:
		BeginWaitCursor();
		EditSpacesToTabs(iTabWidth, FALSE);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTTABS2:
		BeginWaitCursor();
		EditTabsToSpaces(iTabWidth, TRUE);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTSPACES2:
		BeginWaitCursor();
		EditSpacesToTabs(iTabWidth, TRUE);
		EndWaitCursor();
		break;

	case IDM_EDIT_INSERT_XMLTAG:
		EditInsertTagDlg(hwnd);
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
			SciCall_ReplaceSel(guidBuf);
		}
	}
	break;

	case IDM_INSERT_UNICODE_LRM:
	case IDM_INSERT_UNICODE_RLM:
	case IDM_INSERT_UNICODE_ZWJ:
	case IDM_INSERT_UNICODE_ZWNJ:
	case IDM_INSERT_UNICODE_LRE:
	case IDM_INSERT_UNICODE_RLE:
	case IDM_INSERT_UNICODE_LRO:
	case IDM_INSERT_UNICODE_RLO:
	case IDM_INSERT_UNICODE_PDF:
	case IDM_INSERT_UNICODE_NADS:
	case IDM_INSERT_UNICODE_NODS:
	case IDM_INSERT_UNICODE_ASS:
	case IDM_INSERT_UNICODE_ISS:
	case IDM_INSERT_UNICODE_AAFS:
	case IDM_INSERT_UNICODE_IAFS:
	case IDM_INSERT_UNICODE_RS:
	case IDM_INSERT_UNICODE_US:
	case IDM_INSERT_UNICODE_LS:
	case IDM_INSERT_UNICODE_PS:
	case IDM_INSERT_UNICODE_ZWSP:
	case IDM_INSERT_UNICODE_WJ:
	case IDM_INSERT_UNICODE_LRI:
	case IDM_INSERT_UNICODE_RLI:
	case IDM_INSERT_UNICODE_FSI:
	case IDM_INSERT_UNICODE_PDI:
		EditInsertUnicodeControlCharacter(LOWORD(wParam));
		break;

	case IDM_EDIT_INSERT_ENCODING: {
		if (*mEncoding[iEncoding].pszParseNames) {
			char msz[64];
			BOOL done = FALSE;
			strncpy(msz, mEncoding[iEncoding].pszParseNames, COUNTOF(msz) - 1);
			char *p;
			if ((p = strchr(msz, ',')) != NULL) {
				*p = '\0';
			}
			if (pLexCurrent->iLexer == SCLEX_PYTHON) {
				const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
				const Sci_Line iCurLine = SciCall_LineFromPosition(iCurrentPos);
				const Sci_Position iCurrentLinePos = iCurrentPos - SciCall_PositionFromLine(iCurLine);
				if (iCurLine < 2 && iCurrentLinePos == 0) {
					char cmsz[128];
					sprintf(cmsz, "#-*- coding: %s -*-", msz);
					SciCall_ReplaceSel(cmsz);
					done = TRUE;
				}
			}
			if (!done) {
				SciCall_ReplaceSel(msz);
			}
		}
	}
	break;

	case IDM_EDIT_INSERT_SHEBANG:
		EditInsertScriptShebangLine();
		break;

	case IDM_EDIT_INSERT_SHORTDATE:
	case IDM_EDIT_INSERT_LONGDATE:
		EditInsertDateTime(LOWORD(wParam) == IDM_EDIT_INSERT_SHORTDATE);
		break;

	case IDM_EDIT_INSERT_LOC_DATE:
	case IDM_EDIT_INSERT_LOC_DATETIME: {
		SYSTEMTIME lt;
		char mszBuf[38];
		// Local
		GetLocalTime(&lt);
		if (LOWORD(wParam) == IDM_EDIT_INSERT_LOC_DATE) {
			sprintf(mszBuf, "%04d-%02d-%02d", lt.wYear, lt.wMonth, lt.wDay);
		} else {
			sprintf(mszBuf, "%04d-%02d-%02d %02d:%02d:%02d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
		}
		SciCall_ReplaceSel(mszBuf);
	}
	break;

	case IDM_EDIT_INSERT_UTC_DATETIME: {
		SYSTEMTIME lt;
		char mszBuf[38];
		// UTC
		GetSystemTime(&lt);
		sprintf(mszBuf, "%04d-%02d-%02dT%02d:%02d:%02dZ", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
		SciCall_ReplaceSel(mszBuf);
	}
	break;

	// https://www.frenk.com/2009/12/convert-filetime-to-unix-timestamp/
	case IDM_EDIT_INSERT_TIMESTAMP:		// second	1000 milli
	case IDM_EDIT_INSERT_TIMESTAMP_MS:	// milli	1000 micro
	case IDM_EDIT_INSERT_TIMESTAMP_US:	// micro 	1000 nano
	case IDM_EDIT_INSERT_TIMESTAMP_NS: {// nano
		char mszBuf[32];
		FILETIME ft;
		// Windows timestamp in 100-nanosecond
#if _WIN32_WINNT < _WIN32_WINNT_WIN8
		GetSystemTimeAsFileTime(&ft);
#else
		GetSystemTimePreciseAsFileTime(&ft);
#endif
		uint64_t timestamp = (((uint64_t)(ft.dwHighDateTime)) << 32) | ft.dwLowDateTime;
		// Between Jan 1, 1601 and Jan 1, 1970 there are 11644473600 seconds
		timestamp -= UINT64_C(11644473600) * 1000 * 1000 * 10;
		switch (LOWORD(wParam)) {
		case IDM_EDIT_INSERT_TIMESTAMP:		// second	1000 milli
			timestamp /= 1000U * 1000 * 10;
			break;
		case IDM_EDIT_INSERT_TIMESTAMP_MS:	// milli	1000 micro
			timestamp /= 1000U * 10;
			break;
		case IDM_EDIT_INSERT_TIMESTAMP_US:	// micro 	1000 nano
			timestamp /= 10U;
			break;
		case IDM_EDIT_INSERT_TIMESTAMP_NS:	// nano
			timestamp *= 100U;
			break;
		}
		sprintf(mszBuf, "%" PRIu64, timestamp);
		SciCall_ReplaceSel(mszBuf);
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

		const int cmd = LOWORD(wParam);
		if (StrNotEmpty(szCurFile)) {
			if (cmd == IDM_EDIT_INSERT_FILENAME || cmd == CMD_COPYFILENAME || cmd == CMD_INSERTFILENAME_NOEXT || cmd == CMD_COPYFILENAME_NOEXT) {
				SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
				pszInsert = shfi.szDisplayName;
			} else {
				pszInsert = szCurFile;
			}
		} else {
			GetString(IDS_UNTITLED, tchUntitled, COUNTOF(tchUntitled));
			pszInsert = tchUntitled;
		}
		if (cmd == CMD_INSERTFILENAME_NOEXT || cmd == CMD_COPYFILENAME_NOEXT) {
			PathRemoveExtension(pszInsert);
		}

		if (cmd == CMD_COPYFILENAME || cmd == CMD_COPYFILENAME_NOEXT || cmd == CMD_COPYPATHNAME) {
			SetClipData(hwnd, pszInsert);
		} else {
			//int iSelStart;
			char mszBuf[MAX_PATH * kMaxMultiByteCount];
			const UINT cpEdit = SciCall_GetCodePage();
			WideCharToMultiByte(cpEdit, 0, pszInsert, -1, mszBuf, COUNTOF(mszBuf), NULL, NULL);
			//const Sci_Position iSelStart = SciCall_GetSelectionStart();
			SciCall_ReplaceSel(mszBuf);
			//SciCall_SetAnchor(iSelStart);
		}
	}
	break;

	case IDM_EDIT_LINECOMMENT:
		EditToggleCommentLine();
		break;

	case IDM_EDIT_STREAMCOMMENT:
		EditToggleCommentBlock();
		break;

	case IDM_EDIT_URLENCODE:
		BeginWaitCursor();
		EditURLEncode();
		EndWaitCursor();
		break;

	case IDM_EDIT_URLDECODE:
		BeginWaitCursor();
		EditURLDecode();
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
		EditChar2Hex();
		EndWaitCursor();
		break;

	case IDM_EDIT_HEX2CHAR:
		BeginWaitCursor();
		EditHex2Char();
		EndWaitCursor();
		break;

	case IDM_EDIT_SHOW_HEX:
		BeginWaitCursor();
		EditShowHex();
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2HEX:
		BeginWaitCursor();
		EditConvertNumRadix(16);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2DEC:
		BeginWaitCursor();
		EditConvertNumRadix(10);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2BIN:
		BeginWaitCursor();
		EditConvertNumRadix(2);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2OCT:
		BeginWaitCursor();
		EditConvertNumRadix(8);
		EndWaitCursor();
		break;

	case IDM_EDIT_FINDMATCHINGBRACE: {
		Sci_Position iBrace2 = -1;
		Sci_Position iPos = SciCall_GetCurrentPos();
		int ch = SciCall_GetCharAt(iPos);
		if (IsBraceMatchChar(ch)) {
			iBrace2 = SciCall_BraceMatch(iPos);
		} else { // Try one before
			iPos = SciCall_PositionBefore(iPos);
			ch = SciCall_GetCharAt(iPos);
			if (IsBraceMatchChar(ch)) {
				iBrace2 = SciCall_BraceMatch(iPos);
			}
		}
		if (iBrace2 != -1) {
			SciCall_GotoPos(iBrace2);
		}
	}
	break;

	case IDM_EDIT_SELTOMATCHINGBRACE: {
		Sci_Position iBrace2 = -1;
		Sci_Position iPos = SciCall_GetCurrentPos();
		int ch = SciCall_GetCharAt(iPos);
		if (IsBraceMatchChar(ch)) {
			iBrace2 = SciCall_BraceMatch(iPos);
		} else { // Try one before
			iPos = SciCall_PositionBefore(iPos);
			ch = SciCall_GetCharAt(iPos);
			if (IsBraceMatchChar(ch)) {
				iBrace2 = SciCall_BraceMatch(iPos);
			}
		}
		if (iBrace2 != -1) {
			if (iBrace2 > iPos) {
				SciCall_SetSel(iPos, iBrace2 + 1);
			} else {
				SciCall_SetSel(iPos + 1, iBrace2);
			}
		}
	}
	break;

	// Main Bookmark Functions
	case BME_EDIT_BOOKMARKNEXT: {
		const Sci_Position iPos = SciCall_GetCurrentPos();
		const Sci_Line iLine = SciCall_LineFromPosition(iPos);

		Sci_Line iNextLine = SciCall_MarkerNext(iLine + 1, MarkerBitmask_Bookmark);
		if (iNextLine == -1) {
			iNextLine = SciCall_MarkerNext(0, MarkerBitmask_Bookmark);
		}

		if (iNextLine != -1) {
			editMarkAllStatus.ignoreSelectionUpdate = TRUE;
			SciCall_EnsureVisible(iNextLine);
			SciCall_GotoLine(iNextLine);
			SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 10);
			SciCall_ScrollCaret();
			SciCall_SetYCaretPolicy(CARET_EVEN, 0);
		}
	}
	break;

	case BME_EDIT_BOOKMARKPREV: {
		const Sci_Position iPos = SciCall_GetCurrentPos();
		const Sci_Line iLine = SciCall_LineFromPosition(iPos);

		Sci_Line iNextLine = SciCall_MarkerPrevious(iLine - 1, MarkerBitmask_Bookmark);
		if (iNextLine == -1) {
			const Sci_Line nLines = SciCall_GetLineCount();
			iNextLine = SciCall_MarkerPrevious(nLines, MarkerBitmask_Bookmark);
		}

		if (iNextLine != -1) {
			editMarkAllStatus.ignoreSelectionUpdate = TRUE;
			SciCall_EnsureVisible(iNextLine);
			SciCall_GotoLine(iNextLine);
			SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 10);
			SciCall_ScrollCaret();
			SciCall_SetYCaretPolicy(CARET_EVEN, 0);
		}
	}
	break;

	case BME_EDIT_BOOKMARKTOGGLE:
		EditToggleBookmarkAt(-1);
		break;

	case BME_EDIT_BOOKMARKSELECT:
		EditBookmarkSelectAll();
		break;

	case BME_EDIT_BOOKMARKCLEAR:
		SciCall_MarkerDeleteAll(MarkerNumber_Bookmark);
		break;

	case IDM_EDIT_FIND:
	case IDM_EDIT_REPLACE: {
		const BOOL bReplace = LOWORD(wParam) == IDM_EDIT_REPLACE;
		if (!IsWindow(hDlgFindReplace)) {
			hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, bReplace);
		} else {
			if (bReplace ^ (GetDlgItem(hDlgFindReplace, IDC_REPLACETEXT) != NULL)) {
				SendWMCommand(hDlgFindReplace, IDC_TOGGLEFINDREPLACE);
				DestroyWindow(hDlgFindReplace);
				hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, bReplace);
			} else {
				SendMessage(hDlgFindReplace, WM_COPYDATA, 0, 0);
				SetForegroundWindow(hDlgFindReplace);
			}
		}
	}
	break;

	case IDM_EDIT_FINDNEXT:
	case IDM_EDIT_FINDPREV:
	case IDM_EDIT_REPLACENEXT:
	case IDM_EDIT_SELTONEXT:
	case IDM_EDIT_SELTOPREV:
		if (SciCall_GetLength() == 0) {
			break;
		}

		if (StrIsEmptyA(efrData.szFind)) {
			if (LOWORD(wParam) != IDM_EDIT_REPLACENEXT) {
				SendWMCommand(hwnd, IDM_EDIT_FIND);
			} else {
				SendWMCommand(hwnd, IDM_EDIT_REPLACE);
			}
		} else {
			const UINT cpEdit = SciCall_GetCodePage();
			if (cpLastFind != cpEdit) {
				if (cpEdit != SC_CP_UTF8) {
					WCHAR wch[NP2_FIND_REPLACE_LIMIT];

					MultiByteToWideChar(CP_UTF8, 0, efrData.szFindUTF8, -1, wch, COUNTOF(wch));
					WideCharToMultiByte(cpEdit, 0, wch, -1, efrData.szFind, COUNTOF(efrData.szFind), NULL, NULL);

					MultiByteToWideChar(CP_UTF8, 0, efrData.szReplaceUTF8, -1, wch, COUNTOF(wch));
					WideCharToMultiByte(cpEdit, 0, wch, -1, efrData.szReplace, COUNTOF(efrData.szReplace), NULL, NULL);
				} else {
					strcpy(efrData.szFind, efrData.szFindUTF8);
					strcpy(efrData.szReplace, efrData.szReplaceUTF8);
				}
			}

			cpLastFind = cpEdit;
			switch (LOWORD(wParam)) {
			case IDM_EDIT_FINDNEXT:
				EditFindNext(&efrData, FALSE);
				break;

			case IDM_EDIT_FINDPREV:
				EditFindPrev(&efrData, FALSE);
				break;

			case IDM_EDIT_REPLACENEXT:
				if (bReplaceInitialized) {
					EditReplace(hwndEdit, &efrData);
				} else {
					SendWMCommand(hwnd, IDM_EDIT_REPLACE);
				}
				break;

			case IDM_EDIT_SELTONEXT:
				EditFindNext(&efrData, TRUE);
				break;

			case IDM_EDIT_SELTOPREV:
				EditFindPrev(&efrData, TRUE);
				break;
			}
		}
		break;

	case IDM_EDIT_SELTODOCEND:
	case IDM_EDIT_SELTODOCSTART: {
		Sci_Position selStart;
		Sci_Position selEnd;
		if (LOWORD(wParam) == IDM_EDIT_SELTODOCEND) {
			selStart = SciCall_GetSelectionStart();
			selEnd = SciCall_GetLength();
		} else {
			selStart = 0;
			selEnd = SciCall_GetSelectionEnd();
		}
		SciCall_SetSelectionStart(selStart);
		SciCall_SetSelectionEnd(selEnd);
	}
	break;

	case IDM_EDIT_COMPLETEWORD:
		EditCompleteWord(AutoCompleteCondition_Normal, TRUE);
		break;

	case IDM_EDIT_GOTOLINE:
		EditLineNumDlg(hwndEdit);
		break;

	//case IDM_EDIT_NAVIGATE_BACKWARD:
	//case IDM_EDIT_NAVIGATE_FORWARD:
	//	break;

	case IDM_EDIT_GOTO_BLOCK_START:
	case IDM_EDIT_GOTO_BLOCK_END:
	case IDM_EDIT_GOTO_PREVIOUS_BLOCK:
	case IDM_EDIT_GOTO_NEXT_BLOCK:
	case IDM_EDIT_GOTO_PREV_SIBLING_BLOCK:
	case IDM_EDIT_GOTO_NEXT_SIBLING_BLOCK:
		EditGotoBlock(LOWORD(wParam));
		break;

	case IDM_VIEW_SCHEME:
	case IDM_VIEW_SCHEME_FAVORITE:
		Style_SelectLexerDlg(hwndEdit, LOWORD(wParam) == IDM_VIEW_SCHEME_FAVORITE);
		break;

	case IDM_VIEW_SCHEME_CONFIG:
		Style_ConfigDlg(hwndEdit);
		break;

	case IDM_VIEW_USE2NDGLOBALSTYLE:
		Style_ToggleUse2ndGlobalStyle();
		break;

	case IDM_VIEW_USEDEFAULT_CODESTYLE:
		Style_ToggleUseDefaultCodeStyle();
		break;

	case IDM_VIEW_STYLE_THEME_DEFAULT:
	case IDM_VIEW_STYLE_THEME_DARK:
		OnStyleThemeChanged(LOWORD(wParam) - IDM_VIEW_STYLE_THEME_DEFAULT);
		break;

	case IDM_VIEW_DEFAULT_CODE_FONT:
	case IDM_VIEW_DEFAULT_TEXT_FONT:
		Style_SetDefaultFont(hwndEdit, LOWORD(wParam) == IDM_VIEW_DEFAULT_CODE_FONT);
		break;

	case IDM_VIEW_WORDWRAP:
		fWordWrap = !fWordWrap;
		SciCall_SetWrapMode(fWordWrap? iWordWrapMode : SC_WRAP_NONE);
		fWordWrapG = fWordWrap;
		EditEnsureSelectionVisible();
		UpdateToolbar();
		break;

	case IDM_VIEW_WORDWRAPSETTINGS:
		if (WordWrapSettingsDlg(hwnd, &iWordWrapIndent)) {
			SciCall_SetWrapMode(fWordWrap? iWordWrapMode : SC_WRAP_NONE);
			SciCall_SetMarginOptions(bWordWrapSelectSubLine ? SC_MARGINOPTION_SUBLINESELECT : SC_MARGINOPTION_NONE);
			fWordWrapG = fWordWrap;
			SetWrapIndentMode();
			SetWrapVisualFlags();
			EditEnsureSelectionVisible();
			UpdateToolbar();
		}
		break;

	case IDM_VIEW_LONGLINEMARKER:
		bMarkLongLines = !bMarkLongLines;
		if (bMarkLongLines) {
			SciCall_SetEdgeMode((iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND);
			Style_SetLongLineColors();
		} else {
			SciCall_SetEdgeMode(EDGE_NONE);
		}
		break;

	case IDM_VIEW_LONGLINESETTINGS:
		if (LongLineSettingsDlg(hwnd, &iLongLinesLimit)) {
			bMarkLongLines = TRUE;
			SciCall_SetEdgeMode((iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND);
			Style_SetLongLineColors();
			iLongLinesLimit = clamp_i(iLongLinesLimit, 0, NP2_LONG_LINE_LIMIT);
			SciCall_SetEdgeColumn(iLongLinesLimit);
			iLongLinesLimitG = iLongLinesLimit;
		}
		break;

	case IDM_VIEW_TABSASSPACES:
		bTabsAsSpaces = !bTabsAsSpaces;
		SciCall_SetUseTabs(!bTabsAsSpaces);
		bTabsAsSpacesG = bTabsAsSpaces;
		break;

	case IDM_VIEW_TABSETTINGS:
		if (TabSettingsDlg(hwnd)) {
			SciCall_SetUseTabs(!bTabsAsSpaces);
			SciCall_SetTabIndents(bTabIndents);
			SciCall_SetBackSpaceUnIndents(bBackspaceUnindents);
			iTabWidth = clamp_i(iTabWidth, 1, 256);
			iIndentWidth = clamp_i(iIndentWidth, 0, 256);
			SciCall_SetTabWidth(iTabWidth);
			SciCall_SetIndent(iIndentWidth);
			bTabsAsSpacesG = bTabsAsSpaces;
			bTabIndentsG = bTabIndents;
			iTabWidthG = iTabWidth;
			iIndentWidthG = iIndentWidth;
			if (SciCall_GetWrapIndentMode() == SC_WRAPINDENT_FIXED) {
				SetWrapStartIndent();
			}
		}
		break;

	case IDM_VIEW_SHOWINDENTGUIDES:
		bShowIndentGuides = !bShowIndentGuides;
		Style_SetIndentGuides(bShowIndentGuides);
		break;

	case IDM_VIEW_LINENUMBERS:
		bShowLineNumbers = !bShowLineNumbers;
		UpdateLineNumberWidth();
		break;

	case IDM_VIEW_MARGIN:
		bShowBookmarkMargin = !bShowBookmarkMargin;
		UpdateBookmarkMarginWidth();
		Style_SetBookmark();
		break;

	case IDM_VIEW_AUTOCOMPLETION_SETTINGS:
		if (AutoCompletionSettingsDlg(hwnd)) {
			if (!autoCompletionConfig.bCompleteWord) {
				SciCall_AutoCCancel();
			}
		}
		break;

	case IDM_VIEW_AUTOCOMPLETION_IGNORECASE:
		autoCompletionConfig.bIgnoreCase = !autoCompletionConfig.bIgnoreCase;
		SciCall_AutoCCancel();
		break;

	case IDM_SET_LINE_SELECTION_MODE:
		bEnableLineSelectionMode = !bEnableLineSelectionMode;
		if (!bEnableLineSelectionMode) {
			SciCall_SetSelectionMode(SC_SEL_STREAM);
		}
		break;

	case IDM_VIEW_MARKOCCURRENCES_OFF:
	case IDM_VIEW_MARKOCCURRENCES_CASE:
	case IDM_VIEW_MARKOCCURRENCES_WORD:
	case IDM_VIEW_MARKOCCURRENCES_BOOKMARK:
		switch (LOWORD(wParam)) {
		case IDM_VIEW_MARKOCCURRENCES_OFF:
			bMarkOccurrences = !bMarkOccurrences;
			break;
		case IDM_VIEW_MARKOCCURRENCES_CASE:
			bMarkOccurrencesMatchCase = !bMarkOccurrencesMatchCase;
			break;
		case IDM_VIEW_MARKOCCURRENCES_WORD:
			bMarkOccurrencesMatchWords = !bMarkOccurrencesMatchWords;
			break;
		case IDM_VIEW_MARKOCCURRENCES_BOOKMARK:
			bMarkOccurrencesBookmark = !bMarkOccurrencesBookmark;
			break;
		}
		if (bMarkOccurrences) {
			EditMarkAll(FALSE, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords, bMarkOccurrencesBookmark);
		} else {
			EditMarkAll_Clear();
		}
		UpdateStatusbar();
		break;

	case IDM_VIEW_SHOW_FOLDING:
		bShowCodeFolding = !bShowCodeFolding;
		UpdateFoldMarginWidth();
		UpdateToolbar();
		if (!bShowCodeFolding) {
			FoldToggleAll(FOLD_ACTION_EXPAND);
		}
		break;

	case IDM_VIEW_FOLD_DEFAULT:
		if (bShowCodeFolding) {
			FoldToggleDefault(FOLD_ACTION_SNIFF);
		}
		break;

	case IDM_VIEW_FOLD_ALL:
		if (bShowCodeFolding) {
			FoldToggleAll(FOLD_ACTION_SNIFF);
		}
		break;

	case IDM_VIEW_FOLD_CURRENT_BLOCK:
		if (bShowCodeFolding) {
			FoldToggleCurrentBlock(FOLD_ACTION_SNIFF);
		}
		break;

	case IDM_VIEW_FOLD_CURRENT_LEVEL:
		if (bShowCodeFolding) {
			FoldToggleCurrentLevel(FOLD_ACTION_SNIFF);
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
		SciCall_SetViewWS((bViewWhiteSpace) ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
		break;

	case IDM_VIEW_SHOWEOLS:
		bViewEOLs = !bViewEOLs;
		SciCall_SetViewEOL(bViewEOLs);
		break;

	case IDM_VIEW_WORDWRAPSYMBOLS:
		bShowWordWrapSymbols = !bShowWordWrapSymbols;
		SetWrapVisualFlags();
		break;

	case IDM_VIEW_UNICODE_CONTROL_CHAR:
		bShowUnicodeControlCharacter = !bShowUnicodeControlCharacter;
		EditShowUnicodeControlCharacter(bShowUnicodeControlCharacter);
		InvalidateStyleRedraw();
		break;

#if NP2_ENABLE_SHOW_CALLTIPS
	case IDM_VIEW_SHOWCALLTIPS:
		bShowCallTips = !bShowCallTips;
		SciCall_SetMouseDwellTime(bShowCallTips? iCallTipsWaitTime : SC_TIME_FOREVER);
		break;
#endif

	case IDM_VIEW_MATCHBRACES:
		bMatchBraces = !bMatchBraces;
		if (bMatchBraces) {
			struct SCNotification scn;
			scn.nmhdr.hwndFrom = hwndEdit;
			scn.nmhdr.idFrom = IDC_EDIT;
			scn.nmhdr.code = SCN_UPDATEUI;
			scn.updated = NP2_CUSTOM_UPDATE;
			SendMessage(hwnd, WM_NOTIFY, IDC_EDIT, (LPARAM)&scn);
		} else {
			SciCall_BraceHighlight(-1, -1);
		}
		break;

	case IDM_VIEW_HIGHLIGHTCURRENT_BLOCK:
		bHighlightCurrentBlock = !bHighlightCurrentBlock;
		SciCall_MarkerEnableHighlight(bHighlightCurrentBlock);
		break;

	case IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE:
	case IDM_VIEW_HIGHLIGHTCURRENTLINE_BACK:
	case IDM_VIEW_HIGHLIGHTCURRENTLINE_FRAME:
		iHighlightCurrentLine = LOWORD(wParam) - IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE;
		Style_HighlightCurrentLine();
		break;

	case IDM_VIEW_HIGHLIGHTCURRENTLINE_SUBLINE:
		bHighlightCurrentSubLine = !bHighlightCurrentSubLine;
		break;

	case IDM_VIEW_ZOOMIN:
		SciCall_ZoomIn();
		break;

	case IDM_VIEW_ZOOMOUT:
		SciCall_ZoomOut();
		break;

	case IDM_VIEW_ZOOM_LEVEL:
		ZoomLevelDlg(hwnd, FALSE);
		break;

	case IDM_VIEW_RESETZOOM:
		SciCall_SetZoom(100);
		break;

	case IDM_VIEW_TOOLBAR:
		bShowToolbar = !bShowToolbar;
		if (bShowToolbar) {
			UpdateToolbar();
			ShowWindow(hwndReBar, SW_SHOW);
		} else {
			ShowWindow(hwndReBar, SW_HIDE);
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_CUSTOMIZE_TOOLBAR:
		SendMessage(hwndToolbar, TB_CUSTOMIZE, 0, 0);
		break;

	case IDM_VIEW_AUTO_SCALE_TOOLBAR:
		bAutoScaleToolbar = !bAutoScaleToolbar;
		if (g_uCurrentDPI > USER_DEFAULT_SCREEN_DPI) {
			MsgThemeChanged(hwnd, 0, 0);
		}
		break;

	case IDM_VIEW_STATUSBAR:
		bShowStatusbar = !bShowStatusbar;
		if (bShowStatusbar) {
			UpdateStatusbar();
			ShowWindow(hwndStatus, SW_SHOW);
		} else {
			ShowWindow(hwndStatus, SW_HIDE);
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_CLEARWINPOS:
		ClearWindowPositionHistory();
		break;

	case IDM_VIEW_REUSEWINDOW:
		bReuseWindow = !bReuseWindow;
		IniSetBool(INI_SECTION_NAME_FLAGS, L"ReuseWindow", bReuseWindow);
		break;

	case IDM_VIEW_SINGLEFILEINSTANCE:
		bSingleFileInstance = !bSingleFileInstance;
		IniSetBool(INI_SECTION_NAME_FLAGS, L"SingleFileInstance", bSingleFileInstance);
		break;

	case IDM_VIEW_ALWAYSONTOP:
		if (IsTopMost()) {
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
		SetWindowTransparentMode(hwnd, bTransparentMode, iOpacityLevel);
		break;

	case IDM_VIEW_SCROLLPASTLASTLINE_NO:
	case IDM_VIEW_SCROLLPASTLASTLINE_ONE:
	case IDM_VIEW_SCROLLPASTLASTLINE_HALF:
	case IDM_VIEW_SCROLLPASTLASTLINE_THIRD:
	case IDM_VIEW_SCROLLPASTLASTLINE_QUARTER:
		iEndAtLastLine = LOWORD(wParam) - IDM_VIEW_SCROLLPASTLASTLINE_ONE;
		SciCall_SetEndAtLastLine(iEndAtLastLine);
		break;

	case IDM_SET_RENDER_TECH_GDI:
	case IDM_SET_RENDER_TECH_D2D:
	case IDM_SET_RENDER_TECH_D2DRETAIN:
	case IDM_SET_RENDER_TECH_D2DDC: {
		const int back = iRenderingTechnology;
		iRenderingTechnology = LOWORD(wParam) - IDM_SET_RENDER_TECH_GDI;
		SciCall_SetTechnology(iRenderingTechnology);
		iRenderingTechnology = SciCall_GetTechnology();
		iBidirectional = SciCall_GetBidirectional();
		if (back != iRenderingTechnology) {
			UpdateLineNumberWidth();
			UpdateBookmarkMarginWidth();
			UpdateFoldMarginWidth();
		}
	}
	break;

	case IDM_SET_RTL_LAYOUT_EDIT:
		bEditLayoutRTL = !bEditLayoutRTL;
		SetWindowLayoutRTL(hwndEdit, bEditLayoutRTL);
		InvalidateStyleRedraw();
		break;

	case IDM_SET_RTL_LAYOUT_OTHER:
		bWindowLayoutRTL = !bWindowLayoutRTL;
		break;

	case IDM_SET_BIDIRECTIONAL_NONE:
	case IDM_SET_BIDIRECTIONAL_L2R:
	case IDM_SET_BIDIRECTIONAL_R2L:
		SciCall_SetBidirectional(LOWORD(wParam) - IDM_SET_BIDIRECTIONAL_NONE);
		iBidirectional = SciCall_GetBidirectional();
		break;

	case IDM_SET_USE_INLINE_IME:
		bUseInlineIME = bUseInlineIME? SC_IME_WINDOWED : SC_IME_INLINE;
		SciCall_SetIMEInteraction(bUseInlineIME);
		break;

	case IDM_VIEW_FONTQUALITY_DEFAULT:
	case IDM_VIEW_FONTQUALITY_NONE:
	case IDM_VIEW_FONTQUALITY_STANDARD:
	case IDM_VIEW_FONTQUALITY_CLEARTYPE:
		iFontQuality = LOWORD(wParam) - IDM_VIEW_FONTQUALITY_DEFAULT;
		SciCall_SetFontQuality(iFontQuality);
		break;

	case IDM_VIEW_CARET_STYLE_BLOCK_OVR:
		iOvrCaretStyle = !iOvrCaretStyle;
		Style_UpdateCaret();
		break;

	case IDM_VIEW_CARET_STYLE_BLOCK:
	case IDM_VIEW_CARET_STYLE_WIDTH1:
	case IDM_VIEW_CARET_STYLE_WIDTH2:
	case IDM_VIEW_CARET_STYLE_WIDTH3:
		iCaretStyle = LOWORD(wParam) - IDM_VIEW_CARET_STYLE_BLOCK;
		Style_UpdateCaret();
		break;

	case IDM_VIEW_CARET_STYLE_NOBLINK:
		iCaretBlinkPeriod = (iCaretBlinkPeriod == 0)? -1 : 0;
		Style_UpdateCaret();
		break;

	case IDM_VIEW_CARET_STYLE_SELECTION:
		bBlockCaretOutSelection = !bBlockCaretOutSelection;
		Style_UpdateCaret();
		break;

	case IDM_VIEW_SHOWFILENAMEONLY:
	case IDM_VIEW_SHOWFILENAMEFIRST:
	case IDM_VIEW_SHOWFULLPATH:
		iPathNameFormat = LOWORD(wParam) - IDM_VIEW_SHOWFILENAMEONLY;
		lstrcpy(szTitleExcerpt, L"");
		UpdateWindowTitle();
		break;

	case IDM_VIEW_SHOWEXCERPT:
		EditGetExcerpt(szTitleExcerpt, COUNTOF(szTitleExcerpt));
		UpdateWindowTitle();
		break;

	case IDM_VIEW_NOSAVERECENT:
		bSaveRecentFiles = !bSaveRecentFiles;
		break;

	case IDM_VIEW_NOSAVEFINDREPL:
		bSaveFindReplace = !bSaveFindReplace;
		break;

	case IDM_VIEW_SAVEBEFORERUNNINGTOOLS:
		bSaveBeforeRunningTools = !bSaveBeforeRunningTools;
		break;

	case IDM_SET_OPEN_FOLDER_METAPATH:
		bOpenFolderWithMetapath = !bOpenFolderWithMetapath;
		break;

	case IDM_VIEW_CHANGENOTIFY:
		if (ChangeNotifyDlg(hwnd)) {
			InstallFileWatching(FALSE);
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

	case IDM_VIEW_SAVESETTINGSNOW:
		SaveSettingsNow(FALSE, FALSE);
		break;

	case IDM_HELP_ABOUT:
		ThemedDialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
		break;

	case IDM_CMDLINE_HELP:
		DisplayCmdLineHelp(hwnd);
		break;

	case IDM_HELP_PROJECT_HOME:
	case IDM_HELP_LATEST_RELEASE:
	case IDM_HELP_LATEST_BUILD:
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
		ToggleFullScreenModeConfig(FullScreenMode_OnStartup);
		break;

	case IDM_VIEW_FULLSCREEN_HIDE_TITLE:
		ToggleFullScreenModeConfig(FullScreenMode_HideCaption);
		break;

	case IDM_VIEW_FULLSCREEN_HIDE_MENU:
		ToggleFullScreenModeConfig(FullScreenMode_HideMenu);
		break;

	case CMD_ESCAPE:
		if (SciCall_AutoCActive()) {
			SciCall_AutoCCancel();
		} else if (SciCall_CallTipActive()) {
			SciCall_CallTipCancel();
		} else if (bInFullScreenMode) {
			bInFullScreenMode = FALSE;
			ToggleFullScreenMode();
		} else if (iEscFunction == 1) {
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		} else if (iEscFunction == 2) {
			ExitApplication(hwnd);
		}
		break;

	case CMD_SHIFTESC:
		ExitApplication(hwnd);
		break;

	// Newline with toggled auto indent setting
	case CMD_CTRLENTER:
		autoCompletionConfig.bIndentText = !autoCompletionConfig.bIndentText;
		SciCall_NewLine();
		autoCompletionConfig.bIndentText = !autoCompletionConfig.bIndentText;
		break;

	case CMD_CTRLBACK: {
		const Sci_Position iPos = SciCall_GetCurrentPos();
		const Sci_Position iAnchor = SciCall_GetAnchor();

		if (iPos != iAnchor) {
			SciCall_SetSel(iPos, iPos);
		} else {
			const Sci_Line iLine = SciCall_LineFromPosition(iPos);
			const Sci_Position iStartPos = SciCall_PositionFromLine(iLine);
			const Sci_Position iIndentPos = SciCall_GetLineIndentPosition(iLine);
			if (iPos == iStartPos) {
				SciCall_DeleteBack();
			} else if (iPos <= iIndentPos) {
				SciCall_DelLineLeft();
			} else {
				SciCall_DelWordLeft();
			}
		}
	}
	break;

	case CMD_CTRLDEL: {
		const Sci_Position iPos = SciCall_GetCurrentPos();
		const Sci_Position iAnchor = SciCall_GetAnchor();

		if (iPos != iAnchor) {
			SciCall_SetSel(iPos, iPos);
		} else {
			const Sci_Line iLine = SciCall_LineFromPosition(iPos);
			const Sci_Position iStartPos = SciCall_PositionFromLine(iLine);
			const Sci_Position iEndPos = SciCall_GetLineEndPosition(iLine);
			if (iStartPos != iEndPos) {
				SciCall_DelWordRight();
			} else {
				SciCall_LineDelete();
			}
		}
	}
	break;

	case CMD_CTRLTAB:
		SciCall_SetTabIndents(FALSE);
		SciCall_SetUseTabs(TRUE);
		SciCall_Tab();
		SciCall_SetUseTabs(!bTabsAsSpaces);
		SciCall_SetTabIndents(bTabIndents);
		break;

	case CMD_RECODEDEFAULT:
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
		break;

	case CMD_RELOADANSI:
		if (StrNotEmpty(szCurFile)) {
			iSrcEncoding = CPI_DEFAULT;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
		}
		break;

	case CMD_RELOADOEM:
		if (StrNotEmpty(szCurFile)) {
			iSrcEncoding = CPI_OEM;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
		}
		break;

	case CMD_RELOADUTF8:
		if (StrNotEmpty(szCurFile)) {
			iSrcEncoding = CPI_UTF8;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
		}
		break;

	case CMD_RELOADNOFILEVARS:
		if (StrNotEmpty(szCurFile)) {
			const int _fNoFileVariables = fNoFileVariables;
			const BOOL _bNoEncodingTags = bNoEncodingTags;
			fNoFileVariables = 1;
			bNoEncodingTags = 1;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
			fNoFileVariables = _fNoFileVariables;
			bNoEncodingTags = _bNoEncodingTags;
		}
		break;

#if defined(_WIN64)
	case IDM_FILE_LARGE_FILE_MODE_RELOAD:
		if (StrNotEmpty(szCurFile)) {
			bLargeFileMode = TRUE;
			iSrcEncoding = iEncoding;
			FileLoad(FALSE, FALSE, TRUE, FALSE, szCurFile);
			bLargeFileMode = SciCall_GetDocumentOptions() & SC_DOCUMENTOPTION_TEXT_LARGE;
		}
		if (!bLargeFileMode) {
			EditConvertToLargeMode();
		}
		break;
	case IDM_FILE_LARGE_FILE_MODE:
		EditConvertToLargeMode();
		break;
#endif

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	case IDM_LANG_USER_DEFAULT:
	case IDM_LANG_ENGLISH_US:
	case IDM_LANG_CHINESE_SIMPLIFIED:
	case IDM_LANG_CHINESE_TRADITIONAL:
	case IDM_LANG_JAPANESE:
		SetUILanguage(LOWORD(wParam));
		break;
#endif

	case IDM_LEXER_TEXTFILE:
	case IDM_LEXER_2NDTEXTFILE:
	case IDM_LEXER_APACHE:
	case IDM_LEXER_WEB:
	case IDM_LEXER_PHP:
	case IDM_LEXER_JSP:
	case IDM_LEXER_ASPX_CS:
	case IDM_LEXER_ASPX_VB:
	case IDM_LEXER_ASP_VBS:
	case IDM_LEXER_ASP_JS:
	case IDM_LEXER_XML:
	case IDM_LEXER_XSD:
	case IDM_LEXER_XSLT:
	case IDM_LEXER_DTD:
	case IDM_LEXER_ANT_BUILD:
	case IDM_LEXER_MAVEN_POM:
	case IDM_LEXER_MAVEN_SETTINGS:
	case IDM_LEXER_IVY_MODULE:
	case IDM_LEXER_IVY_SETTINGS:
	case IDM_LEXER_PMD_RULESET:
	case IDM_LEXER_CHECKSTYLE:
	case IDM_LEXER_TOMCAT:
	case IDM_LEXER_WEB_JAVA:
	case IDM_LEXER_STRUTS:
	case IDM_LEXER_HIB_CFG:
	case IDM_LEXER_HIB_MAP:
	case IDM_LEXER_SPRING_BEANS:
	case IDM_LEXER_JBOSS:
	case IDM_LEXER_WEB_NET:
	case IDM_LEXER_RESX:
	case IDM_LEXER_XAML:
	case IDM_LEXER_PROPERTY_LIST:
	case IDM_LEXER_ANDROID_MANIFEST:
	case IDM_LEXER_ANDROID_LAYOUT:
	case IDM_LEXER_SVG:
	case IDM_LEXER_BASH:
	case IDM_LEXER_CSHELL:
	case IDM_LEXER_M4:
	case IDM_LEXER_MATLAB:
	case IDM_LEXER_OCTAVE:
	case IDM_LEXER_SCILAB:
	case IDM_LEXER_CSS:
	case IDM_LEXER_SCSS:
	case IDM_LEXER_LESS:
	case IDM_LEXER_HSS:
		Style_SetLexerByLangIndex(LOWORD(wParam));
		break;

	case CMD_TIMESTAMPS:
		EditUpdateTimestampMatchTemplate(hwndEdit);
		break;

	case CMD_OPEN_PATH_OR_LINK:
		EditOpenSelection(0);
		break;
	case CMD_OPEN_CONTAINING_FOLDER:
		EditOpenSelection(4);
		break;

	case CMD_ONLINE_SEARCH_GOOGLE:
	case CMD_ONLINE_SEARCH_BING:
	case CMD_ONLINE_SEARCH_WIKI:
	case CMD_CUSTOM_ACTION1:
	case CMD_CUSTOM_ACTION2:
		EditSelectionAction(LOWORD(wParam));
		break;

	case CMD_FINDNEXTSEL:
	case CMD_FINDPREVSEL:
	case IDM_EDIT_SAVEFIND: {
		Sci_Position cchSelection = SciCall_GetSelTextLength() - 1;
		if (cchSelection == 0) {
			SendWMCommand(hwnd, IDM_EDIT_SELECTWORD);
			cchSelection = SciCall_GetSelTextLength() - 1;
		}

		if (cchSelection > 0 && cchSelection < NP2_FIND_REPLACE_LIMIT) {
			char mszSelection[NP2_FIND_REPLACE_LIMIT];

			SciCall_GetSelText(mszSelection);
			mszSelection[cchSelection] = 0; // zero terminate

			// Check lpszSelection and truncate newlines
			char *lpsz = strpbrk(mszSelection, "\r\n");
			if (lpsz) {
				*lpsz = '\0';
			}

			cpLastFind = SciCall_GetCodePage();
			strcpy(efrData.szFind, mszSelection);

			if (cpLastFind != SC_CP_UTF8) {
				WCHAR wszBuf[NP2_FIND_REPLACE_LIMIT];
				MultiByteToWideChar(cpLastFind, 0, mszSelection, -1, wszBuf, COUNTOF(wszBuf));
				WideCharToMultiByte(CP_UTF8, 0, wszBuf, -1, efrData.szFindUTF8, COUNTOF(efrData.szFindUTF8), NULL, NULL);
			} else {
				strcpy(efrData.szFindUTF8, mszSelection);
			}

			efrData.fuFlags &= (~(SCFIND_REGEXP | SCFIND_POSIX));
			efrData.bTransformBS = FALSE;

			switch (LOWORD(wParam)) {
			case IDM_EDIT_SAVEFIND:
				break;

			case CMD_FINDNEXTSEL:
				EditFindNext(&efrData, FALSE);
				break;

			case CMD_FINDPREVSEL:
				EditFindPrev(&efrData, FALSE);
				break;
			}
		}
	}
	break;

	case CMD_INCLINELIMIT:
	case CMD_DECLINELIMIT:
		if (!bMarkLongLines) {
			SendWMCommand(hwnd, IDM_VIEW_LONGLINEMARKER);
		} else {
			if (LOWORD(wParam) == CMD_INCLINELIMIT) {
				iLongLinesLimit++;
			} else {
				iLongLinesLimit--;
			}
			iLongLinesLimit = clamp_i(iLongLinesLimit, 0, NP2_LONG_LINE_LIMIT);
			SciCall_SetEdgeColumn(iLongLinesLimit);
			UpdateStatusbar();
			iLongLinesLimitG = iLongLinesLimit;
		}
		break;

	case CMD_ENCLOSE_TRIPLE_SQ:
		EditEncloseSelection(L"'''", L"'''");
		break;
	case CMD_ENCLOSE_TRIPLE_DQ:
		EditEncloseSelection(L"\"\"\"", L"\"\"\"");
		break;
	case CMD_ENCLOSE_TRIPLE_BT:
		EditEncloseSelection(L"```", L"```");
		break;

	case CMD_INCREASENUM:
		EditModifyNumber(TRUE);
		break;

	case CMD_DECREASENUM:
		EditModifyNumber(FALSE);
		break;

	case CMD_JUMP2SELSTART:
		if (!SciCall_IsRectangleSelection()) {
			const Sci_Position iAnchorPos = SciCall_GetAnchor();
			const Sci_Position iCursorPos = SciCall_GetCurrentPos();
			if (iCursorPos > iAnchorPos) {
				const int mode = SciCall_GetSelectionMode();
				SciCall_SetSel(iCursorPos, iAnchorPos);
				SciCall_SetSelectionMode(mode);
				SciCall_ChooseCaretX();
			}
		}
		break;

	case CMD_JUMP2SELEND:
		if (!SciCall_IsRectangleSelection()) {
			const Sci_Position iAnchorPos = SciCall_GetAnchor();
			const Sci_Position iCursorPos = SciCall_GetCurrentPos();
			if (iCursorPos < iAnchorPos) {
				const int mode = SciCall_GetSelectionMode();
				SciCall_SetSel(iCursorPos, iAnchorPos);
				SciCall_SetSelectionMode(mode);
				SciCall_ChooseCaretX();
			}
		}
		break;

	case CMD_COPYWINPOS: {
		WINDOWPLACEMENT wndpl;
		wndpl.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwndMain, &wndpl);

		const int x = wndpl.rcNormalPosition.left;
		const int y = wndpl.rcNormalPosition.top;
		const int cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		const int cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		const BOOL max = IsZoomed(hwndMain) || (wndpl.flags & WPF_RESTORETOMAXIMIZED);

		WCHAR wszWinPos[64];
		wsprintf(wszWinPos, L"/pos %i,%i,%i,%i,%i", x, y, cx, cy, max);

		SetClipData(hwnd, wszWinPos);
		UpdateToolbar();
	}
	break;

	case CMD_DEFAULTWINPOS:
		SnapToDefaultPos(hwnd);
		break;

	case CMD_OPENINIFILE:
		if (StrNotEmpty(szIniFile)) {
			CreateIniFile(szIniFile);
			FileLoad(FALSE, FALSE, FALSE, FALSE, szIniFile);
		}
		break;

	case IDM_SET_SYSTEM_INTEGRATION:
		SystemIntegrationDlg(hwnd);
		break;

	case IDT_FILE_NEW:
		SendWMCommandOrBeep(hwnd, IDM_FILE_NEW);
		break;

	case IDT_FILE_OPEN:
		SendWMCommandOrBeep(hwnd, IDM_FILE_OPEN);
		break;

	case IDT_FILE_BROWSE:
		SendWMCommandOrBeep(hwnd, IDM_FILE_BROWSE);
		break;

	case IDT_FILE_SAVE:
		SendWMCommandOrBeep(hwnd, IDM_FILE_SAVE);
		break;

	case IDT_EDIT_UNDO:
		SendWMCommandOrBeep(hwnd, IDM_EDIT_UNDO);
		break;

	case IDT_EDIT_REDO:
		SendWMCommandOrBeep(hwnd, IDM_EDIT_REDO);
		break;

	case IDT_EDIT_CUT:
		SendWMCommandOrBeep(hwnd, IDM_EDIT_CUT);
		break;

	case IDT_EDIT_COPY:
		if (IsCmdEnabled(hwnd, IDM_EDIT_COPY)) {
			SendWMCommand(hwnd, IDM_EDIT_COPY);
		} else {
			SendWMCommand(hwnd, IDM_EDIT_COPYALL);
		}
		break;

	case IDT_EDIT_PASTE:
		SendWMCommandOrBeep(hwnd, IDM_EDIT_PASTE);
		break;

	case IDT_EDIT_FIND:
		SendWMCommandOrBeep(hwnd, IDM_EDIT_FIND);
		break;

	case IDT_EDIT_REPLACE:
		SendWMCommandOrBeep(hwnd, IDM_EDIT_REPLACE);
		break;

	case IDT_VIEW_WORDWRAP:
		SendWMCommandOrBeep(hwnd, IDM_VIEW_WORDWRAP);
		break;

	case IDT_VIEW_ZOOMIN:
		SendWMCommandOrBeep(hwnd, IDM_VIEW_ZOOMIN);
		break;

	case IDT_VIEW_ZOOMOUT:
		SendWMCommandOrBeep(hwnd, IDM_VIEW_ZOOMOUT);
		break;

	case IDT_VIEW_SCHEME:
		SendWMCommandOrBeep(hwnd, IDM_VIEW_SCHEME);
		break;

	case IDT_VIEW_SCHEMECONFIG:
		SendWMCommandOrBeep(hwnd, IDM_VIEW_SCHEME_CONFIG);
		break;

	case IDT_FILE_EXIT:
		ExitApplication(hwnd);
		break;

	case IDT_FILE_SAVEAS:
		SendWMCommandOrBeep(hwnd, IDM_FILE_SAVEAS);
		break;

	case IDT_FILE_SAVECOPY:
		SendWMCommandOrBeep(hwnd, IDM_FILE_SAVECOPY);
		break;

	case IDT_EDIT_DELETE:
		SendWMCommandOrBeep(hwnd, IDM_EDIT_DELETE);
		break;

	case IDT_FILE_PRINT:
		SendWMCommandOrBeep(hwnd, IDM_FILE_PRINT);
		break;

	case IDT_FILE_OPENFAV:
		SendWMCommandOrBeep(hwnd, IDM_FILE_OPENFAV);
		break;

	case IDT_FILE_ADDTOFAV:
		SendWMCommandOrBeep(hwnd, IDM_FILE_ADDTOFAV);
		break;

	case IDT_FILE_LAUNCH:
		SendWMCommandOrBeep(hwnd, IDM_FILE_LAUNCH);
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
	const struct SCNotification * const scn = (struct SCNotification *)lParam;

	switch (pnmh->idFrom) {
	case IDC_EDIT:
		switch (pnmh->code) {
		case SCN_UPDATEUI:
			if (scn->updated & ~(SC_UPDATE_V_SCROLL | SC_UPDATE_H_SCROLL)) {
				UpdateToolbar();

				BOOL updated = FALSE;
				if (scn->updated & (SC_UPDATE_SELECTION)) {
					UpdateStatusBarCache_OVRMode(FALSE);
					// mark occurrences of text currently selected
					if (editMarkAllStatus.ignoreSelectionUpdate) {
						editMarkAllStatus.ignoreSelectionUpdate = FALSE;
					} else if (bMarkOccurrences) {
						if (SciCall_IsSelectionEmpty()) {
							if (editMarkAllStatus.matchCount) {
								EditMarkAll_Clear();
							}
						} else {
							updated = EditMarkAll((scn->updated & SC_UPDATE_CONTENT), bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords, bMarkOccurrencesBookmark);
						}
					}
				} else if (scn->updated & (SC_UPDATE_CONTENT)) {
					if (editMarkAllStatus.matchCount) {
						updated = EditMarkAll(TRUE, bMarkOccurrencesMatchCase, bMarkOccurrencesMatchWords, bMarkOccurrencesBookmark);
					}
				}
				if (!updated) {
					UpdateStatusbar();
				}

				// Brace Match
				if (bMatchBraces) {
					Sci_Position iPos = SciCall_GetCurrentPos();
					int ch = SciCall_GetCharAt(iPos);
					if (IsBraceMatchChar(ch)) {
						const Sci_Position iBrace2 = SciCall_BraceMatch(iPos);
						if (iBrace2 != -1) {
							const Sci_Position col1 = SciCall_GetColumn(iPos);
							const Sci_Position col2 = SciCall_GetColumn(iBrace2);
							SciCall_BraceHighlight(iPos, iBrace2);
							SciCall_SetHighlightGuide(min_pos(col1, col2));
						} else {
							SciCall_BraceBadLight(iPos);
							SciCall_SetHighlightGuide(0);
						}
					} else { // Try one before
						iPos = SciCall_PositionBefore(iPos);
						ch = SciCall_GetCharAt(iPos);
						if (IsBraceMatchChar(ch)) {
							const Sci_Position iBrace2 = SciCall_BraceMatch(iPos);
							if (iBrace2 != -1) {
								const Sci_Position col1 = SciCall_GetColumn(iPos);
								const Sci_Position col2 = SciCall_GetColumn(iBrace2);
								SciCall_BraceHighlight(iPos, iBrace2);
								SciCall_SetHighlightGuide(min_pos(col1, col2));
							} else {
								SciCall_BraceBadLight(iPos);
								SciCall_SetHighlightGuide(0);
							}
						} else {
							SciCall_BraceHighlight(-1, -1);
							SciCall_SetHighlightGuide(0);
						}
					}
				}
			}
			break;

		case SCN_CHARADDED: {
			//if (SciCall_IsMultipleSelection()) {
			//	// not work for multiple selection.
			//	return 0;
			//}
			// tentative input characters already ignored in Editor::InsertCharacter()
			const int ch = scn->ch;
			if (ch < 0x80) {
				// Auto indent
				if (ch == '\r' || ch == '\n') {
					// in CRLF mode handle LF only...
					if (autoCompletionConfig.bIndentText && ((SC_EOL_CRLF == iEOLMode && ch != '\n') || SC_EOL_CRLF != iEOLMode)) {
						EditAutoIndent();
					}
					return 0;
				}
				// Auto close tags
				if (ch == '>') {
					if (autoCompletionConfig.bCloseTags || autoCompletionConfig.bCompleteWord) {
						EditAutoCloseXMLTag();
					}
					return 0;
				}
				// Auto close braces/quotes
				if (ch == '(' || ch == '[' || ch == '{' || ch == '<' || ch == '\"' || ch == '\'' || ch == '`' || ch == ',') {
					if (autoCompletionConfig.fAutoInsertMask) {
						EditAutoCloseBraceQuote(ch);
					}
					return 0;
				}
			}

			// auto complete word
			if (!autoCompletionConfig.bCompleteWord
				// ignore IME input
				|| (scn->characterSource != SC_CHARACTERSOURCE_DIRECT_INPUT && (ch >= 0x80 || autoCompletionConfig.bEnglistIMEModeOnly))
				|| !IsAutoCompletionWordCharacter(ch)
			) {
				return 0;
			}

			const int iCondition = SciCall_AutoCActive() ? AutoCompleteCondition_OnCharAdded : AutoCompleteCondition_Normal;
			EditCompleteWord(iCondition, FALSE);
		}
		break;

		case SCN_AUTOCSELECTION:
		case SCN_USERLISTSELECTION: {
			if ((scn->listCompletionMethod == SC_AC_NEWLINE && !(autoCompletionConfig.fAutoCompleteFillUpMask & AutoCompleteFillUpEnter))
			|| (scn->listCompletionMethod == SC_AC_TAB && !(autoCompletionConfig.fAutoCompleteFillUpMask & AutoCompleteFillUpTab))) {
				SciCall_AutoCCancel();
				if (scn->listCompletionMethod == SC_AC_NEWLINE) {
					SciCall_NewLine();
				} else {
					SciCall_Tab();
				}
				return 0;
			}

			LPCSTR text = scn->text;
			// function/array/template/generic
			LPSTR braces = (LPSTR)strpbrk(text, "([{<");
			const Sci_Position iCurPos = SciCall_GetCurrentPos();
			Sci_Position offset;
			BOOL closeBrace = FALSE;
			if (braces != NULL) {
				Sci_Position iPos = iCurPos;
				int ch = SciCall_GetCharAt(iPos);
				while (ch == ' ' || ch == '\t') {
					++iPos;
					ch = SciCall_GetCharAt(iPos);
				}

				offset = braces - text + 1;
				const char brace = *braces;
				if (brace == ch) {
					*braces = L'\0'; // delete open and close braces
					offset += iPos - iCurPos;
				} else {
					const int chNext = (brace == '(') ? ')' : brace + 2;
					if (ch == chNext) {
						if (SciCall_BraceMatchNext(iPos, SciCall_PositionBefore(iCurPos)) == -1) {
							*(braces + 1) = L'\0'; // delete close brace
						}
					} else {
						closeBrace = brace != '<' && braces[1] == chNext;
					}
				}
			} else {
				offset = strlen(text);
			}

			const Sci_Position iNewPos = scn->position + offset;
			SciCall_BeginUndoAction();
			SciCall_SetSel(scn->position, iCurPos);
			SciCall_ReplaceSel(text);
			SciCall_SetSel(iNewPos, iNewPos);
			if (closeBrace && EditIsOpenBraceMatched(iNewPos - 1, iNewPos + 1)) {
				SciCall_Clear(); // delete close brace
			}
			SciCall_EndUndoAction();
			SciCall_AutoCCancel();
		}
		break;

		case SCN_AUTOCCHARDELETED:
			EditCompleteWord(AutoCompleteCondition_OnCharDeleted, FALSE);
			break;

		case SCN_AUTOCCOMPLETED:
		case SCN_AUTOCCANCELLED:
			autoCompletionConfig.iPreviousItemCount = 0;
			break;

#if NP2_ENABLE_SHOW_CALLTIPS
		case SCN_DWELLSTART:
			if (bShowCallTips && scn->position >= 0) {
				EditShowCallTips(scn->position);
			}
			break;

		case SCN_DWELLEND:
			// if calltip source changed
			SciCall_CallTipCancel();
			break;
#endif

		case SCN_CALLTIPCLICK:
			SciCall_CallTipCancel();
			break;

		case SCN_MODIFIED:
			UpdateStatusBarCacheLineColumn();
			if (scn->linesAdded) {
				UpdateLineNumberWidth();
			}
			break;

		case SCN_ZOOM:
			MsgNotifyZoom();
			break;

		case SCN_SAVEPOINTREACHED:
			bModified = FALSE;
			UpdateDocumentModificationStatus();
			break;

		case SCN_MARGINCLICK:
			switch (scn->margin) {
			case MarginNumber_CodeFolding:
				FoldClickAt(scn->position, scn->modifiers);
				break;
			case MarginNumber_Bookmark:
				EditToggleBookmarkAt(scn->position);
				break;
			}
			break;

		case SCN_KEY:
			// Also see the corresponding patch??? in scintilla\src\Editor.cxx
			if (bShowCodeFolding) {
				FoldAltArrow(scn->ch, scn->modifiers);
			}
			break;

		case SCN_SAVEPOINTLEFT:
			bModified = TRUE;
			UpdateDocumentModificationStatus();
			break;

		case SCN_URIDROPPED: {
			WCHAR szBuf[MAX_PATH + 40];
			if (MultiByteToWideChar(CP_UTF8, 0, scn->text, -1, szBuf, COUNTOF(szBuf)) > 0) {
				OnDropOneFile(hwnd, szBuf);
			}
		}
		break;

		case SCN_CODEPAGECHANGED:
			EditOnCodePageChanged(scn->oldCodePage);
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
			LPTBNOTIFY lpTbNotify = (LPTBNOTIFY)lParam;
			if (lpTbNotify->iItem < (int)COUNTOF(tbbMainWnd)) {
				WCHAR tch[256];
				GetString(tbbMainWnd[lpTbNotify->iItem].idCommand, tch, COUNTOF(tch));
				lstrcpyn(lpTbNotify->pszText, tch, lpTbNotify->cchText);
				CopyMemory(&lpTbNotify->tbButton, &tbbMainWnd[lpTbNotify->iItem], sizeof(TBBUTTON));
				return TRUE;
			}
		}
		return FALSE;

		case TBN_RESET:
			Toolbar_SetButtons(hwndToolbar, DefaultToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd));
			return FALSE;

		case TBN_DROPDOWN: {
			LPTBNOTIFY lpTbNotify = (LPTBNOTIFY)lParam;
			RECT rc;
			SendMessage(hwndToolbar, TB_GETRECT, lpTbNotify->iItem, (LPARAM)&rc);
			MapWindowPoints(hwndToolbar, HWND_DESKTOP, (LPPOINT)&rc, 2);
			HMENU hmenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_POPUPMENU));
			TPMPARAMS tpm;
			tpm.cbSize = sizeof(TPMPARAMS);
			tpm.rcExclude = rc;
			TrackPopupMenuEx(GetSubMenu(hmenu, IDP_POPUP_SUBMENU_FOLD),
							TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
							rc.left, rc.bottom, hwndMain, &tpm);
			DestroyMenu(hmenu);
		}
		return FALSE;
		}
		break;

	case IDC_STATUSBAR:
		switch (pnmh->code) {
		case NM_CLICK: {
			LPNMMOUSE pnmm = (LPNMMOUSE)lParam;
			switch (pnmm->dwItemSpec) {
			case STATUS_EOLMODE:
				EditEnsureConsistentLineEndings();
				return TRUE;

			case STATUS_DOCZOOM:
				ZoomLevelDlg(hwnd, TRUE);
				return TRUE;

			default:
				return FALSE;
			}
		}

		case NM_DBLCLK: {
			LPNMMOUSE pnmm = (LPNMMOUSE)lParam;
			switch (pnmm->dwItemSpec) {
			case STATUS_CODEPAGE:
				SendWMCommand(hwnd, IDM_ENCODING_SELECT);
				return TRUE;

			case STATUS_EOLMODE:
				if (iEOLMode == SC_EOL_CRLF) {
					iEOLMode = SC_EOL_LF;
				} else if (iEOLMode == SC_EOL_LF) {
					iEOLMode = SC_EOL_CR;
				} else {
					iEOLMode = SC_EOL_CRLF;
				}
				ConvertLineEndings(iEOLMode);
				return TRUE;

			case STATUS_LEXER:
				SendWMCommand(hwnd, IDM_VIEW_SCHEME);
				return TRUE;

			case STATUS_OVRMODE:
				SciCall_EditToggleOvertype();
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
			LPTOOLTIPTEXT pTTT = (LPTOOLTIPTEXT)lParam;
			if (pTTT->uFlags & TTF_IDISHWND) {
				//nop;
			} else {
				WCHAR tch[256];
				GetString((UINT)pnmh->idFrom, tch, COUNTOF(tch));
				lstrcpyn(pTTT->szText, tch, COUNTOF(pTTT->szText));
			}
		}
		break;

		}
		break;
	}

	return 0;
}

static void GetWindowPositionSectionName(WCHAR sectionName[96]) {
	HMONITOR hMonitor = MonitorFromWindow(hwndMain, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	const int cxScreen = mi.rcMonitor.right - mi.rcMonitor.left;
	const int cyScreen = mi.rcMonitor.bottom - mi.rcMonitor.top;

	wsprintf(sectionName, L"%s %ix%i", INI_SECTION_NAME_WINDOW_POSITION, cxScreen, cyScreen);
}

//=============================================================================
//
// LoadSettings()
//
//
void LoadSettings(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 128);

	LoadIniSection(INI_SECTION_NAME_SETTINGS, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	const int iSettingsVersion = IniSectionGetInt(pIniSection, L"SettingsVersion", NP2SettingsVersion_Current);
	bSaveSettings = IniSectionGetBool(pIniSection, L"SaveSettings", 1);
	bSaveRecentFiles = IniSectionGetBool(pIniSection, L"SaveRecentFiles", 0);
	bSaveFindReplace = IniSectionGetBool(pIniSection, L"SaveFindReplace", 0);
	bFindReplaceTransparentMode = IniSectionGetBool(pIniSection, L"FindReplaceTransparentMode", 1);
	bFindReplaceUseMonospacedFont = IniSectionGetBool(pIniSection, L"FindReplaceUseMonospacedFont", 0);
	bFindReplaceFindAllBookmark = IniSectionGetBool(pIniSection, L"FindReplaceFindAllBookmark", 0);

	efrData.bFindClose = IniSectionGetBool(pIniSection, L"CloseFind", 0);
	efrData.bReplaceClose = IniSectionGetBool(pIniSection, L"CloseReplace", 0);
	efrData.bNoFindWrap = IniSectionGetBool(pIniSection, L"NoFindWrap", 0);

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
		efrData.bWildcardSearch = IniSectionGetBool(pIniSection, L"FindReplaceWildcardSearch", 0);
	}

	LPCWSTR strValue = IniSectionGetValue(pIniSection, L"OpenWithDir");
	if (StrIsEmpty(strValue)) {
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
		SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, tchOpenWithDir);
#else
		LPWSTR pszPath = NULL;
		if (S_OK == SHGetKnownFolderPath(&FOLDERID_Desktop, KF_FLAG_DEFAULT, NULL, &pszPath)) {
			lstrcpy(tchOpenWithDir, pszPath);
			CoTaskMemFree(pszPath);
		}
#endif
	} else {
		PathAbsoluteFromApp(strValue, tchOpenWithDir, COUNTOF(tchOpenWithDir), TRUE);
	}

	strValue = IniSectionGetValue(pIniSection, L"Favorites");
	if (StrIsEmpty(strValue)) {
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, tchFavoritesDir);
#else
		LPWSTR pszPath = NULL;
		if (S_OK == SHGetKnownFolderPath(&FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &pszPath)) {
			lstrcpy(tchFavoritesDir, pszPath);
			CoTaskMemFree(pszPath);
		}
#endif
	} else {
		PathAbsoluteFromApp(strValue, tchFavoritesDir, COUNTOF(tchFavoritesDir), TRUE);
	}

	int iValue = IniSectionGetInt(pIniSection, L"PathNameFormat", 1);
	iPathNameFormat = clamp_i(iValue, 0, 2);

	fWordWrap = IniSectionGetBool(pIniSection, L"WordWrap", 1);
	fWordWrapG = fWordWrap;

	iValue = IniSectionGetInt(pIniSection, L"WordWrapMode", SC_WRAP_AUTO);
	iWordWrapMode = clamp_i(iValue, SC_WRAP_WORD, SC_WRAP_AUTO);

	iValue = IniSectionGetInt(pIniSection, L"WordWrapIndent", EditWrapIndentDefaultValue);
	iWordWrapIndent = clamp_i(iValue, EditWrapIndentNone, EditWrapIndentMaxValue);

	iValue = IniSectionGetInt(pIniSection, L"WordWrapSymbols", EditWrapSymbolDefaultValue);
	iValue = clamp_i(iValue, 0, EditWrapSymbolMaxValue);
	iWordWrapSymbols = clamp_i(iValue % 10, EditWrapSymbolBeforeNone, EditWrapSymbolBeforeMaxValue) + (iValue / 10) * 10;

	bShowWordWrapSymbols = IniSectionGetBool(pIniSection, L"ShowWordWrapSymbols", 0);
	bWordWrapSelectSubLine = IniSectionGetBool(pIniSection, L"WordWrapSelectSubLine", 0);
	bShowUnicodeControlCharacter = IniSectionGetBool(pIniSection, L"ShowUnicodeControlCharacter", 0);

	bMatchBraces = IniSectionGetBool(pIniSection, L"MatchBraces", 1);
	bHighlightCurrentBlock = IniSectionGetBool(pIniSection, L"HighlightCurrentBlock", 1);
	iValue = IniSectionGetInt(pIniSection, L"HighlightCurrentLine", 12);
	iValue = (iSettingsVersion < NP2SettingsVersion_V1) ? 12 : iValue;
	bHighlightCurrentSubLine = iValue > 10;
	iHighlightCurrentLine = clamp_i(iValue % 10, 0, 2);
	bShowIndentGuides = IniSectionGetBool(pIniSection, L"ShowIndentGuides", 0);

	autoCompletionConfig.bIndentText = IniSectionGetBool(pIniSection, L"AutoIndent", 1);
	autoCompletionConfig.bCloseTags = IniSectionGetBool(pIniSection, L"AutoCloseTags", 1);
	autoCompletionConfig.bCompleteWord = IniSectionGetBool(pIniSection, L"AutoCompleteWords", 1);
	autoCompletionConfig.bScanWordsInDocument = IniSectionGetBool(pIniSection, L"AutoCScanWordsInDocument", 1);
	iValue = IniSectionGetInt(pIniSection, L"AutoCScanWordsTimeout", AUTOC_SCAN_WORDS_DEFAULT_TIMEOUT);
	autoCompletionConfig.dwScanWordsTimeout = max_i(iValue, AUTOC_SCAN_WORDS_MIN_TIMEOUT);
	autoCompletionConfig.bEnglistIMEModeOnly = IniSectionGetBool(pIniSection, L"AutoCEnglishIMEModeOnly", 0);
	autoCompletionConfig.bIgnoreCase = IniSectionGetBool(pIniSection, L"AutoCIgnoreCase", 0);
	iValue = IniSectionGetInt(pIniSection, L"AutoCVisibleItemCount", 16);
	autoCompletionConfig.iVisibleItemCount = max_i(iValue, MIN_AUTO_COMPLETION_VISIBLE_ITEM_COUNT);
	iValue = IniSectionGetInt(pIniSection, L"AutoCMinWordLength", 1);
	autoCompletionConfig.iMinWordLength = max_i(iValue, MIN_AUTO_COMPLETION_WORD_LENGTH);
	iValue = IniSectionGetInt(pIniSection, L"AutoCMinNumberLength", 3);
	autoCompletionConfig.iMinNumberLength = max_i(iValue, MIN_AUTO_COMPLETION_NUMBER_LENGTH);
	autoCompletionConfig.fAutoCompleteFillUpMask = IniSectionGetInt(pIniSection, L"AutoCFillUpMask", AutoCompleteFillUpDefault);
	autoCompletionConfig.fAutoInsertMask = IniSectionGetInt(pIniSection, L"AutoInsertMask", AutoInsertDefaultMask);
	iValue = IniSectionGetInt(pIniSection, L"AsmLineCommentChar", AsmLineCommentCharSemicolon);
	autoCompletionConfig.iAsmLineCommentChar = clamp_i(iValue, AsmLineCommentCharSemicolon, AsmLineCommentCharAt);
	strValue = IniSectionGetValue(pIniSection, L"AutoCFillUpPunctuation");
	if (StrIsEmpty(strValue)) {
		lstrcpy(autoCompletionConfig.wszAutoCompleteFillUp, AUTO_COMPLETION_FILLUP_DEFAULT);
	} else {
		lstrcpyn(autoCompletionConfig.wszAutoCompleteFillUp, strValue, COUNTOF(autoCompletionConfig.wszAutoCompleteFillUp));
	}
	EditCompleteUpdateConfig();

	bEnableLineSelectionMode = IniSectionGetBool(pIniSection, L"LineSelection", 1);
#if NP2_ENABLE_SHOW_CALLTIPS
	bShowCallTips = IniSectionGetBool(pIniSection, L"ShowCallTips", TRUE);
	iValue = IniSectionGetInt(pIniSection, L"CallTipsWaitTime", 500);
	iCallTipsWaitTime = max_i(iValue, 100);
#endif

	bTabsAsSpaces = IniSectionGetBool(pIniSection, L"TabsAsSpaces", 0);
	bTabsAsSpacesG = bTabsAsSpaces;
	bTabIndents = IniSectionGetBool(pIniSection, L"TabIndents", 1);
	bTabIndentsG = bTabIndents;

	bBackspaceUnindents = IniSectionGetBool(pIniSection, L"BackspaceUnindents", 0);

	iValue = IniSectionGetInt(pIniSection, L"ZoomLevel", 100);
	// Added in v4.2.25.1172, stored as a relative font size in point, in range [-10, 20].
	iValue = (iSettingsVersion < NP2SettingsVersion_V1)? 100 : iValue;
	iZoomLevel = clamp_i(iValue, SC_MIN_ZOOM_LEVEL, SC_MAX_ZOOM_LEVEL);

	iValue = IniSectionGetInt(pIniSection, L"TabWidth", 4);
	iValue = clamp_i(iValue, 1, 256);
	iTabWidth = iValue;
	iTabWidthG = iValue;

	iValue = IniSectionGetInt(pIniSection, L"IndentWidth", 4);
	iValue = clamp_i(iValue, 0, 256);
	iIndentWidth = iValue;
	iIndentWidthG = iValue;

	bMarkLongLines = IniSectionGetBool(pIniSection, L"MarkLongLines", 0);

	iValue = IniSectionGetInt(pIniSection, L"LongLinesLimit", 80);
	iValue = clamp_i(iValue, 0, NP2_LONG_LINE_LIMIT);
	iLongLinesLimit = iValue;
	iLongLinesLimitG = iValue;

	iValue = IniSectionGetInt(pIniSection, L"LongLineMode", EDGE_LINE);
	iLongLineMode = clamp_i(iValue, EDGE_LINE, EDGE_BACKGROUND);

	bShowBookmarkMargin = IniSectionGetBool(pIniSection, L"ShowBookmarkMargin", 0);
	bShowLineNumbers = IniSectionGetBool(pIniSection, L"ShowLineNumbers", 1);
	bShowCodeFolding = IniSectionGetBool(pIniSection, L"ShowCodeFolding", 1);

	bMarkOccurrences = IniSectionGetBool(pIniSection, L"MarkOccurrences", 1);
	bMarkOccurrencesMatchCase = IniSectionGetBool(pIniSection, L"MarkOccurrencesMatchCase", 0);
	bMarkOccurrencesMatchWords = IniSectionGetBool(pIniSection, L"MarkOccurrencesMatchWholeWords", 0);
	bMarkOccurrencesBookmark = IniSectionGetBool(pIniSection, L"MarkOccurrencesBookmark", 0);

	bViewWhiteSpace = IniSectionGetBool(pIniSection, L"ViewWhiteSpace", 0);
	bViewEOLs = IniSectionGetBool(pIniSection, L"ViewEOLs", 0);

	iValue = Encoding_MapIniSetting(FALSE, CPI_UTF8);
	iValue = IniSectionGetInt(pIniSection, L"DefaultEncoding", iValue);
	iValue = Encoding_MapIniSetting(TRUE, iValue);
	if (!Encoding_IsValid(iValue)) {
		iValue = CPI_UTF8;
	}
	iDefaultEncoding = iValue;

	bSkipUnicodeDetection = IniSectionGetBool(pIniSection, L"SkipUnicodeDetection", 1);
	bLoadANSIasUTF8 = IniSectionGetBool(pIniSection, L"LoadANSIasUTF8", 0);
	bLoadASCIIasUTF8 = IniSectionGetBool(pIniSection, L"LoadASCIIasUTF8", 1);
	bLoadNFOasOEM = IniSectionGetBool(pIniSection, L"LoadNFOasOEM", 1);
	bNoEncodingTags = IniSectionGetBool(pIniSection, L"NoEncodingTags", 0);

	iValue = IniSectionGetInt(pIniSection, L"DefaultEOLMode", 0);
	iDefaultEOLMode = clamp_i(iValue, SC_EOL_CRLF, SC_EOL_LF);

	bWarnLineEndings = IniSectionGetBool(pIniSection, L"WarnLineEndings", 1);
	bFixLineEndings = IniSectionGetBool(pIniSection, L"FixLineEndings", 1);
	bAutoStripBlanks = IniSectionGetBool(pIniSection, L"FixTrailingBlanks", 0);

	iValue = IniSectionGetInt(pIniSection, L"PrintHeader", 1);
	iPrintHeader = clamp_i(iValue, 0, 3);

	iValue = IniSectionGetInt(pIniSection, L"PrintFooter", 0);
	iPrintFooter = clamp_i(iValue, 0, 1);

	iValue = IniSectionGetInt(pIniSection, L"PrintColorMode", SC_PRINT_COLOURONWHITE);
	iPrintColor = clamp_i(iValue, SC_PRINT_NORMAL, SC_PRINT_SCREENCOLOURS);

	iValue = IniSectionGetInt(pIniSection, L"PrintZoom", 100);
	// previously stored as a relative font size in point plus 10, in range [-10, 20] + 10.
	iValue = (iSettingsVersion < NP2SettingsVersion_V1)? 100 : iValue;
	iPrintZoom = clamp_i(iValue, SC_MIN_ZOOM_LEVEL, SC_MAX_ZOOM_LEVEL);

	iValue = IniSectionGetInt(pIniSection, L"PrintMarginLeft", -1);
	pageSetupMargin.left = max_i(iValue, -1);

	iValue = IniSectionGetInt(pIniSection, L"PrintMarginTop", -1);
	pageSetupMargin.top = max_i(iValue, -1);

	iValue = IniSectionGetInt(pIniSection, L"PrintMarginRight", -1);
	pageSetupMargin.right = max_i(iValue, -1);

	iValue = IniSectionGetInt(pIniSection, L"PrintMarginBottom", -1);
	pageSetupMargin.bottom = max_i(iValue, -1);

	bSaveBeforeRunningTools = IniSectionGetBool(pIniSection, L"SaveBeforeRunningTools", 0);
	bOpenFolderWithMetapath = IniSectionGetBool(pIniSection, L"OpenFolderWithMetapath", 1);

	iValue = IniSectionGetInt(pIniSection, L"FileWatchingMode", 2);
	iFileWatchingMode = clamp_i(iValue, 0, 2);
	iFileWatchingMethod = IniSectionGetBool(pIniSection, L"FileWatchingMethod", 0);
	bFileWatchingKeepAtEnd = IniSectionGetBool(pIniSection, L"FileWatchingKeepAtEnd", 0);
	bResetFileWatching = IniSectionGetBool(pIniSection, L"ResetFileWatching", 0);

	iValue = IniSectionGetInt(pIniSection, L"EscFunction", 0);
	iEscFunction = clamp_i(iValue, 0, 2);

	bAlwaysOnTop = IniSectionGetBool(pIniSection, L"AlwaysOnTop", 0);
	bMinimizeToTray = IniSectionGetBool(pIniSection, L"MinimizeToTray", 0);
	bTransparentMode = IniSectionGetBool(pIniSection, L"TransparentMode", 0);
	iValue = IniSectionGetInt(pIniSection, L"EndAtLastLine", 1);
	iEndAtLastLine = clamp_i(iValue, 0, 4);
	bEditLayoutRTL = IniSectionGetBool(pIniSection, L"EditLayoutRTL", 0);
	bWindowLayoutRTL = IniSectionGetBool(pIniSection, L"WindowLayoutRTL", 0);

	iValue = IniSectionGetInt(pIniSection, L"RenderingTechnology", GetDefualtRenderingTechnology());
	iValue = clamp_i(iValue, SC_TECHNOLOGY_DEFAULT, SC_TECHNOLOGY_DIRECTWRITEDC);
	iRenderingTechnology = iValue;
	bEditLayoutRTL = bEditLayoutRTL && iValue == SC_TECHNOLOGY_DEFAULT;

	iValue = IniSectionGetInt(pIniSection, L"Bidirectional", SC_BIDIRECTIONAL_DISABLED);
	iBidirectional = clamp_i(iValue, SC_BIDIRECTIONAL_DISABLED, SC_BIDIRECTIONAL_R2L);

	iValue = IniSectionGetInt(pIniSection, L"FontQuality", SC_EFF_QUALITY_LCD_OPTIMIZED);
	iFontQuality = clamp_i(iValue, SC_EFF_QUALITY_DEFAULT, SC_EFF_QUALITY_LCD_OPTIMIZED);

	iValue = IniSectionGetInt(pIniSection, L"CaretStyle", 1);
	bBlockCaretOutSelection = (iValue / 100) & 1;
	iValue %= 100;
	iOvrCaretStyle = (iValue / 10) & 1;
	iCaretStyle = clamp_i(iValue % 10, 0, 3);
	iCaretBlinkPeriod = IniSectionGetInt(pIniSection, L"CaretBlinkPeriod", -1);
	bUseInlineIME = IniSectionGetBool(pIniSection, L"UseInlineIME", 0);

	strValue = IniSectionGetValue(pIniSection, L"ToolbarButtons");
	if (StrIsEmpty(strValue)) {
		CopyMemory(tchToolbarButtons, DefaultToolbarButtons, sizeof(DefaultToolbarButtons));
	} else {
		lstrcpyn(tchToolbarButtons, strValue, COUNTOF(tchToolbarButtons));
	}

	bShowToolbar = IniSectionGetBool(pIniSection, L"ShowToolbar", 1);
	bAutoScaleToolbar = IniSectionGetBool(pIniSection, L"AutoScaleToolbar", 1);
	bShowStatusbar = IniSectionGetBool(pIniSection, L"ShowStatusbar", 1);

	iValue = IniSectionGetInt(pIniSection, L"FullScreenMode", FullScreenMode_Default);
	iFullScreenMode = iValue;
	bInFullScreenMode = iValue & FullScreenMode_OnStartup;

	// toolbar image section
	{
		LoadIniSection(INI_SECTION_NAME_TOOLBAR_IMAGES, pIniSectionBuf, cchIniSection);
		IniSectionParse(pIniSection, pIniSectionBuf);

		strValue = IniSectionGetValue(pIniSection, L"BitmapDefault");
		if (StrNotEmpty(strValue)) {
			tchToolbarBitmap = StrDup(strValue);
		}
		strValue = IniSectionGetValue(pIniSection, L"BitmapHot");
		if (StrNotEmpty(strValue)) {
			tchToolbarBitmapHot = StrDup(strValue);
		}
		strValue = IniSectionGetValue(pIniSection, L"BitmapDisabled");
		if (StrNotEmpty(strValue)) {
			tchToolbarBitmapDisabled = StrDup(strValue);
		}
	}

	// window position section
	{
		WCHAR sectionName[96];
		GetWindowPositionSectionName(sectionName);
		LoadIniSection(sectionName, pIniSectionBuf, cchIniSection);
		IniSectionParse(pIniSection, pIniSectionBuf);

		// ignore window position if /p was specified
		if (!flagPosParam) {
			wi.x	= IniSectionGetInt(pIniSection, L"WindowPosX", CW_USEDEFAULT);
			wi.y	= IniSectionGetInt(pIniSection, L"WindowPosY", CW_USEDEFAULT);
			wi.cx	= IniSectionGetInt(pIniSection, L"WindowSizeX", CW_USEDEFAULT);
			wi.cy	= IniSectionGetInt(pIniSection, L"WindowSizeY", CW_USEDEFAULT);
			wi.max	= IniSectionGetBool(pIniSection, L"WindowMaximized", 0);
		}

		cxRunDlg = IniSectionGetInt(pIniSection, L"RunDlgSizeX", 0);
		cxEncodingDlg = IniSectionGetInt(pIniSection, L"EncodingDlgSizeX", 0);
		cyEncodingDlg = IniSectionGetInt(pIniSection, L"EncodingDlgSizeY", 0);

		cxFileMRUDlg = IniSectionGetInt(pIniSection, L"FileMRUDlgSizeX", 0);
		cyFileMRUDlg = IniSectionGetInt(pIniSection, L"FileMRUDlgSizeY", 0);
		cxOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeX", 0);
		cyOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeY", 0);
		cxFavoritesDlg = IniSectionGetInt(pIniSection, L"FavoritesDlgSizeX", 0);
		cyFavoritesDlg = IniSectionGetInt(pIniSection, L"FavoritesDlgSizeY", 0);
		cxAddFavoritesDlg = IniSectionGetInt(pIniSection, L"AddFavoritesDlgSizeX", 0);

		cxModifyLinesDlg = IniSectionGetInt(pIniSection, L"ModifyLinesDlgSizeX", 0);
		cyModifyLinesDlg = IniSectionGetInt(pIniSection, L"ModifyLinesDlgSizeY", 0);
		cxEncloseSelectionDlg = IniSectionGetInt(pIniSection, L"EncloseSelectionDlgSizeX", 0);
		cyEncloseSelectionDlg = IniSectionGetInt(pIniSection, L"EncloseSelectionDlgSizeY", 0);
		cxInsertTagDlg = IniSectionGetInt(pIniSection, L"InsertTagDlgSizeX", 0);
		cyInsertTagDlg = IniSectionGetInt(pIniSection, L"InsertTagDlgSizeY", 0);

		xFindReplaceDlg = IniSectionGetInt(pIniSection, L"FindReplaceDlgPosX", 0);
		yFindReplaceDlg = IniSectionGetInt(pIniSection, L"FindReplaceDlgPosY", 0);
		cxFindReplaceDlg = IniSectionGetInt(pIniSection, L"FindReplaceDlgSizeX", 0);

		cxStyleSelectDlg = IniSectionGetInt(pIniSection, L"StyleSelectDlgSizeX", 0);
		cyStyleSelectDlg = IniSectionGetInt(pIniSection, L"StyleSelectDlgSizeY", 0);
		cxStyleCustomizeDlg = IniSectionGetInt(pIniSection, L"StyleCustomizeDlgSizeX", 0);
		cyStyleCustomizeDlg = IniSectionGetInt(pIniSection, L"StyleCustomizeDlgSizeY", 0);
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

void SaveSettingsNow(BOOL bOnlySaveStyle, BOOL bQuiet) {
	BOOL bCreateFailure = FALSE;

	if (StrIsEmpty(szIniFile)) {
		if (StrNotEmpty(szIniFile2)) {
			if (CreateIniFile(szIniFile2)) {
				lstrcpy(szIniFile, szIniFile2);
				lstrcpy(szIniFile2, L"");
			} else {
				bCreateFailure = TRUE;
			}
		} else {
			return;
		}
	}

	if (!bCreateFailure) {
		LPCWSTR section = bOnlySaveStyle ? INI_SECTION_NAME_STYLES : INI_SECTION_NAME_SETTINGS;
		if (WritePrivateProfileString(section, L"WriteTest", L"ok", szIniFile)) {
			BeginWaitCursor();
			StatusSetTextID(hwndStatus, STATUS_HELP, IDS_SAVINGSETTINGS);
			StatusSetSimple(hwndStatus, TRUE);
			InvalidateRect(hwndStatus, NULL, TRUE);
			UpdateWindow(hwndStatus);
			if (CreateIniFile(szIniFile)) {
				if (bOnlySaveStyle) {
					Style_Save();
				} else {
					SaveSettings(TRUE);
				}
			} else {
				bCreateFailure = TRUE;
			}
			StatusSetSimple(hwndStatus, FALSE);
			EndWaitCursor();
			if (!bCreateFailure && !bQuiet) {
				MsgBoxInfo(MB_OK, IDS_SAVEDSETTINGS);
			}
		} else {
			dwLastIOError = GetLastError();
			MsgBoxLastError(MB_OK, IDS_WRITEINI_FAIL);
		}
	}
	if (bCreateFailure) {
		MsgBoxLastError(MB_OK, IDS_CREATEINI_FAIL);
	}
}

//=============================================================================
//
// SaveSettings()
//
//
void SaveSettings(BOOL bSaveSettingsNow) {
	if (!CreateIniFile(szIniFile)) {
		return;
	}

	if (!bSaveSettings && !bSaveSettingsNow) {
		IniSetBool(INI_SECTION_NAME_SETTINGS, L"SaveSettings", bSaveSettings);
		return;
	}

	WCHAR wchTmp[MAX_PATH];
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	IniSectionSetInt(pIniSection, L"SettingsVersion", NP2SettingsVersion_Current);
	IniSectionSetBoolEx(pIniSection, L"SaveSettings", bSaveSettings, 1);
	IniSectionSetBoolEx(pIniSection, L"SaveRecentFiles", bSaveRecentFiles, 0);
	IniSectionSetBoolEx(pIniSection, L"SaveFindReplace", bSaveFindReplace, 0);

	IniSectionSetBoolEx(pIniSection, L"CloseFind", efrData.bFindClose, 0);
	IniSectionSetBoolEx(pIniSection, L"CloseReplace", efrData.bReplaceClose, 0);
	IniSectionSetBoolEx(pIniSection, L"NoFindWrap", efrData.bNoFindWrap, 0);
	IniSectionSetBoolEx(pIniSection, L"FindReplaceTransparentMode", bFindReplaceTransparentMode, 1);
	IniSectionSetBoolEx(pIniSection, L"FindReplaceUseMonospacedFont", bFindReplaceUseMonospacedFont, 0);
	IniSectionSetBoolEx(pIniSection, L"FindReplaceFindAllBookmark", bFindReplaceFindAllBookmark, 0);
	if (bSaveFindReplace) {
		IniSectionSetBoolEx(pIniSection, L"FindReplaceMatchCase", (efrData.fuFlags & SCFIND_MATCHCASE), 0);
		IniSectionSetBoolEx(pIniSection, L"FindReplaceMatchWholeWorldOnly", (efrData.fuFlags & SCFIND_WHOLEWORD), 0);
		IniSectionSetBoolEx(pIniSection, L"FindReplaceMatchBeginingWordOnly", (efrData.fuFlags & SCFIND_WORDSTART), 0);
		IniSectionSetBoolEx(pIniSection, L"FindReplaceRegExpSearch", (efrData.fuFlags & (SCFIND_REGEXP | SCFIND_POSIX)), 0);
		IniSectionSetBoolEx(pIniSection, L"FindReplaceTransformBackslash", efrData.bTransformBS, 0);
		IniSectionSetBoolEx(pIniSection, L"FindReplaceWildcardSearch", efrData.bWildcardSearch, 0);
	}

	PathRelativeToApp(tchOpenWithDir, wchTmp, COUNTOF(wchTmp), FALSE, TRUE, flagPortableMyDocs);
	IniSectionSetString(pIniSection, L"OpenWithDir", wchTmp);
	PathRelativeToApp(tchFavoritesDir, wchTmp, COUNTOF(wchTmp), FALSE, TRUE, flagPortableMyDocs);
	IniSectionSetString(pIniSection, L"Favorites", wchTmp);
	IniSectionSetIntEx(pIniSection, L"PathNameFormat", iPathNameFormat, 1);

	IniSectionSetBoolEx(pIniSection, L"WordWrap", fWordWrapG, 1);
	IniSectionSetIntEx(pIniSection, L"WordWrapMode", iWordWrapMode, SC_WRAP_AUTO);
	IniSectionSetIntEx(pIniSection, L"WordWrapIndent", iWordWrapIndent, EditWrapIndentNone);
	IniSectionSetIntEx(pIniSection, L"WordWrapSymbols", iWordWrapSymbols, EditWrapSymbolDefaultValue);
	IniSectionSetBoolEx(pIniSection, L"ShowWordWrapSymbols", bShowWordWrapSymbols, 0);
	IniSectionSetBoolEx(pIniSection, L"WordWrapSelectSubLine", bWordWrapSelectSubLine, 0);
	IniSectionSetBoolEx(pIniSection, L"ShowUnicodeControlCharacter", bShowUnicodeControlCharacter, 0);
	IniSectionSetBoolEx(pIniSection, L"MatchBraces", bMatchBraces, 1);
	IniSectionSetBoolEx(pIniSection, L"HighlightCurrentBlock", bHighlightCurrentBlock, 1);
	IniSectionSetIntEx(pIniSection, L"HighlightCurrentLine", iHighlightCurrentLine + (bHighlightCurrentSubLine ? 10 : 0), 12);
	IniSectionSetBoolEx(pIniSection, L"ShowIndentGuides", bShowIndentGuides, 0);

	IniSectionSetBoolEx(pIniSection, L"AutoIndent", autoCompletionConfig.bIndentText, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCloseTags", autoCompletionConfig.bCloseTags, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCompleteWords", autoCompletionConfig.bCompleteWord, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoCScanWordsInDocument", autoCompletionConfig.bScanWordsInDocument, 1);
	IniSectionSetIntEx(pIniSection, L"AutoCScanWordsTimeout", autoCompletionConfig.dwScanWordsTimeout, AUTOC_SCAN_WORDS_DEFAULT_TIMEOUT);
	IniSectionSetBoolEx(pIniSection, L"AutoCEnglishIMEModeOnly", autoCompletionConfig.bEnglistIMEModeOnly, 0);
	IniSectionSetBoolEx(pIniSection, L"AutoCIgnoreCase", autoCompletionConfig.bIgnoreCase, 0);
	IniSectionSetIntEx(pIniSection, L"AutoCVisibleItemCount", autoCompletionConfig.iVisibleItemCount, 16);
	IniSectionSetIntEx(pIniSection, L"AutoCMinWordLength", autoCompletionConfig.iMinWordLength, 1);
	IniSectionSetIntEx(pIniSection, L"AutoCMinNumberLength", autoCompletionConfig.iMinNumberLength, 3);
	IniSectionSetIntEx(pIniSection, L"AutoCFillUpMask", autoCompletionConfig.fAutoCompleteFillUpMask, AutoCompleteFillUpDefault);
	IniSectionSetIntEx(pIniSection, L"AutoInsertMask", autoCompletionConfig.fAutoInsertMask, AutoInsertDefaultMask);
	IniSectionSetIntEx(pIniSection, L"AsmLineCommentChar", autoCompletionConfig.iAsmLineCommentChar, AsmLineCommentCharSemicolon);
	IniSectionSetStringEx(pIniSection, L"AutoCFillUpPunctuation", autoCompletionConfig.wszAutoCompleteFillUp, AUTO_COMPLETION_FILLUP_DEFAULT);
	IniSectionSetBoolEx(pIniSection, L"LineSelection", bEnableLineSelectionMode, 1);
#if NP2_ENABLE_SHOW_CALLTIPS
	IniSectionSetBoolEx(pIniSection, L"ShowCallTips", bShowCallTips, TRUE);
	IniSectionSetIntEx(pIniSection, L"CallTipsWaitTime", iCallTipsWaitTime, 500);
#endif
	IniSectionSetBoolEx(pIniSection, L"TabsAsSpaces", bTabsAsSpacesG, 0);
	IniSectionSetBoolEx(pIniSection, L"TabIndents", bTabIndentsG, 1);
	IniSectionSetBoolEx(pIniSection, L"BackspaceUnindents", bBackspaceUnindents, 0);
	IniSectionSetIntEx(pIniSection, L"ZoomLevel", iZoomLevel, 100);
	IniSectionSetIntEx(pIniSection, L"TabWidth", iTabWidthG, 4);
	IniSectionSetIntEx(pIniSection, L"IndentWidth", iIndentWidthG, 4);
	IniSectionSetBoolEx(pIniSection, L"MarkLongLines", bMarkLongLines, 0);
	IniSectionSetIntEx(pIniSection, L"LongLinesLimit", iLongLinesLimitG, 80);
	IniSectionSetIntEx(pIniSection, L"LongLineMode", iLongLineMode, EDGE_LINE);
	IniSectionSetBoolEx(pIniSection, L"ShowBookmarkMargin", bShowBookmarkMargin, 0);
	IniSectionSetBoolEx(pIniSection, L"ShowLineNumbers", bShowLineNumbers, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowCodeFolding", bShowCodeFolding, 1);
	IniSectionSetBoolEx(pIniSection, L"MarkOccurrences", bMarkOccurrences, 1);
	IniSectionSetBoolEx(pIniSection, L"MarkOccurrencesMatchCase", bMarkOccurrencesMatchCase, 0);
	IniSectionSetBoolEx(pIniSection, L"MarkOccurrencesMatchWholeWords", bMarkOccurrencesMatchWords, 0);
	IniSectionSetBoolEx(pIniSection, L"MarkOccurrencesBookmark", bMarkOccurrencesBookmark, 0);
	IniSectionSetBoolEx(pIniSection, L"ViewWhiteSpace", bViewWhiteSpace, 0);
	IniSectionSetBoolEx(pIniSection, L"ViewEOLs", bViewEOLs, 0);
	IniSectionSetIntEx(pIniSection, L"DefaultEncoding", Encoding_MapIniSetting(FALSE, iDefaultEncoding), Encoding_MapIniSetting(FALSE, CPI_UTF8));
	IniSectionSetBoolEx(pIniSection, L"SkipUnicodeDetection", bSkipUnicodeDetection, 1);
	IniSectionSetBoolEx(pIniSection, L"LoadANSIasUTF8", bLoadANSIasUTF8, 0);
	IniSectionSetBoolEx(pIniSection, L"LoadASCIIasUTF8", bLoadASCIIasUTF8, 1);
	IniSectionSetBoolEx(pIniSection, L"LoadNFOasOEM", bLoadNFOasOEM, 1);
	IniSectionSetBoolEx(pIniSection, L"NoEncodingTags", bNoEncodingTags, 0);
	IniSectionSetIntEx(pIniSection, L"DefaultEOLMode", iDefaultEOLMode, 0);
	IniSectionSetIntEx(pIniSection, L"WarnLineEndings", bWarnLineEndings, 1);
	IniSectionSetBoolEx(pIniSection, L"FixLineEndings", bFixLineEndings, 1);
	IniSectionSetBoolEx(pIniSection, L"FixTrailingBlanks", bAutoStripBlanks, 0);

	IniSectionSetIntEx(pIniSection, L"PrintHeader", iPrintHeader, 1);
	IniSectionSetIntEx(pIniSection, L"PrintFooter", iPrintFooter, 0);
	IniSectionSetIntEx(pIniSection, L"PrintColorMode", iPrintColor, SC_PRINT_COLOURONWHITE);
	IniSectionSetIntEx(pIniSection, L"PrintZoom", iPrintZoom, 100);
	IniSectionSetIntEx(pIniSection, L"PrintMarginLeft", pageSetupMargin.left, -1);
	IniSectionSetIntEx(pIniSection, L"PrintMarginTop", pageSetupMargin.top, -1);
	IniSectionSetIntEx(pIniSection, L"PrintMarginRight", pageSetupMargin.right, -1);
	IniSectionSetIntEx(pIniSection, L"PrintMarginBottom", pageSetupMargin.bottom, -1);
	IniSectionSetBoolEx(pIniSection, L"SaveBeforeRunningTools", bSaveBeforeRunningTools, 0);
	IniSectionSetBoolEx(pIniSection, L"OpenFolderWithMetapath", bOpenFolderWithMetapath, 1);
	IniSectionSetIntEx(pIniSection, L"FileWatchingMode", iFileWatchingMode, 2);
	IniSectionSetBoolEx(pIniSection, L"FileWatchingMethod", iFileWatchingMethod, 0);
	IniSectionSetBoolEx(pIniSection, L"FileWatchingKeepAtEnd", bFileWatchingKeepAtEnd, 0);
	IniSectionSetBoolEx(pIniSection, L"ResetFileWatching", bResetFileWatching, 0);
	IniSectionSetIntEx(pIniSection, L"EscFunction", iEscFunction, 0);
	IniSectionSetBoolEx(pIniSection, L"AlwaysOnTop", bAlwaysOnTop, 0);
	IniSectionSetBoolEx(pIniSection, L"MinimizeToTray", bMinimizeToTray, 0);
	IniSectionSetBoolEx(pIniSection, L"TransparentMode", bTransparentMode, 0);
	IniSectionSetIntEx(pIniSection, L"EndAtLastLine", iEndAtLastLine, 1);
	IniSectionSetBoolEx(pIniSection, L"EditLayoutRTL", bEditLayoutRTL, 0);
	IniSectionSetBoolEx(pIniSection, L"WindowLayoutRTL", bWindowLayoutRTL, 0);
	IniSectionSetIntEx(pIniSection, L"RenderingTechnology", iRenderingTechnology, GetDefualtRenderingTechnology());
	IniSectionSetIntEx(pIniSection, L"Bidirectional", iBidirectional, SC_BIDIRECTIONAL_DISABLED);
	IniSectionSetIntEx(pIniSection, L"FontQuality", iFontQuality, SC_EFF_QUALITY_LCD_OPTIMIZED);
	IniSectionSetIntEx(pIniSection, L"CaretStyle", iCaretStyle + iOvrCaretStyle*10 + bBlockCaretOutSelection*100, 1);
	IniSectionSetIntEx(pIniSection, L"CaretBlinkPeriod", iCaretBlinkPeriod, -1);
	IniSectionSetBoolEx(pIniSection, L"UseInlineIME", bUseInlineIME, 0);
	Toolbar_GetButtons(hwndToolbar, TOOLBAR_COMMAND_BASE, tchToolbarButtons, COUNTOF(tchToolbarButtons));
	IniSectionSetStringEx(pIniSection, L"ToolbarButtons", tchToolbarButtons, DefaultToolbarButtons);
	IniSectionSetBoolEx(pIniSection, L"ShowToolbar", bShowToolbar, 1);
	IniSectionSetBoolEx(pIniSection, L"AutoScaleToolbar", bAutoScaleToolbar, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowStatusbar", bShowStatusbar, 1);
	IniSectionSetIntEx(pIniSection, L"FullScreenMode", iFullScreenMode, FullScreenMode_Default);

	SaveIniSection(INI_SECTION_NAME_SETTINGS, pIniSectionBuf);

	SaveWindowPosition(bSaveSettingsNow, pIniSectionBuf);
	// Scintilla Styles
	Style_Save();
}

void SaveWindowPosition(BOOL bSaveSettingsNow, WCHAR *pIniSectionBuf) {
	IniSectionOnSave section;
	if (pIniSectionBuf == NULL) {
		pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS);
	} else {
		ZeroMemory(pIniSectionBuf, NP2HeapSize(pIniSectionBuf));
	}
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	WCHAR sectionName[96];
	GetWindowPositionSectionName(sectionName);

	// query window dimensions when window is not minimized
	if (bSaveSettingsNow && !IsIconic(hwndMain)) {
		WINDOWPLACEMENT wndpl;
		wndpl.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwndMain, &wndpl);

		wi.x = wndpl.rcNormalPosition.left;
		wi.y = wndpl.rcNormalPosition.top;
		wi.cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		wi.cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		wi.max = (IsZoomed(hwndMain) || (wndpl.flags & WPF_RESTORETOMAXIMIZED));
	}

	IniSectionSetInt(pIniSection, L"WindowPosX", wi.x);
	IniSectionSetInt(pIniSection, L"WindowPosY", wi.y);
	IniSectionSetInt(pIniSection, L"WindowSizeX", wi.cx);
	IniSectionSetInt(pIniSection, L"WindowSizeY", wi.cy);
	IniSectionSetBoolEx(pIniSection, L"WindowMaximized", wi.max, 0);

	IniSectionSetIntEx(pIniSection, L"RunDlgSizeX", cxRunDlg, 0);
	IniSectionSetIntEx(pIniSection, L"EncodingDlgSizeX", cxEncodingDlg, 0);
	IniSectionSetIntEx(pIniSection, L"EncodingDlgSizeY", cyEncodingDlg, 0);

	IniSectionSetIntEx(pIniSection, L"FileMRUDlgSizeX", cxFileMRUDlg, 0);
	IniSectionSetIntEx(pIniSection, L"FileMRUDlgSizeY", cyFileMRUDlg, 0);
	IniSectionSetIntEx(pIniSection, L"OpenWithDlgSizeX", cxOpenWithDlg, 0);
	IniSectionSetIntEx(pIniSection, L"OpenWithDlgSizeY", cyOpenWithDlg, 0);
	IniSectionSetIntEx(pIniSection, L"FavoritesDlgSizeX", cxFavoritesDlg, 0);
	IniSectionSetIntEx(pIniSection, L"FavoritesDlgSizeY", cyFavoritesDlg, 0);
	IniSectionSetIntEx(pIniSection, L"AddFavoritesDlgSizeX", cxAddFavoritesDlg, 0);

	IniSectionSetIntEx(pIniSection, L"ModifyLinesDlgSizeX", cxModifyLinesDlg, 0);
	IniSectionSetIntEx(pIniSection, L"ModifyLinesDlgSizeY", cyModifyLinesDlg, 0);
	IniSectionSetIntEx(pIniSection, L"EncloseSelectionDlgSizeX", cxEncloseSelectionDlg, 0);
	IniSectionSetIntEx(pIniSection, L"EncloseSelectionDlgSizeY", cyEncloseSelectionDlg, 0);
	IniSectionSetIntEx(pIniSection, L"InsertTagDlgSizeX", cxInsertTagDlg, 0);
	IniSectionSetIntEx(pIniSection, L"InsertTagDlgSizeY", cyInsertTagDlg, 0);

	IniSectionSetIntEx(pIniSection, L"FindReplaceDlgPosX", xFindReplaceDlg, 0);
	IniSectionSetIntEx(pIniSection, L"FindReplaceDlgPosY", yFindReplaceDlg, 0);
	IniSectionSetIntEx(pIniSection, L"FindReplaceDlgSizeX", cxFindReplaceDlg, 0);

	IniSectionSetIntEx(pIniSection, L"StyleSelectDlgSizeX", cxStyleSelectDlg, 0);
	IniSectionSetIntEx(pIniSection, L"StyleSelectDlgSizeY", cyStyleSelectDlg, 0);
	IniSectionSetIntEx(pIniSection, L"StyleCustomizeDlgSizeX", cxStyleCustomizeDlg, 0);
	IniSectionSetIntEx(pIniSection, L"StyleCustomizeDlgSizeY", cyStyleCustomizeDlg, 0);

	SaveIniSection(sectionName, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
}

void ClearWindowPositionHistory(void) {
	cxRunDlg = 0;
	cxEncodingDlg = 0;
	cyEncodingDlg = 0;

	cxFileMRUDlg = 0;
	cyFileMRUDlg = 0;
	cxOpenWithDlg = 0;
	cyOpenWithDlg = 0;
	cxFavoritesDlg = 0;
	cyFavoritesDlg = 0;
	cxAddFavoritesDlg = 0;

	cxModifyLinesDlg = 0;
	cyModifyLinesDlg = 0;
	cxEncloseSelectionDlg = 0;
	cyEncloseSelectionDlg = 0;
	cxInsertTagDlg = 0;
	cyInsertTagDlg = 0;

	xFindReplaceDlg = 0;
	yFindReplaceDlg = 0;
	cxFindReplaceDlg = 0;

	cxStyleSelectDlg = 0;
	cyStyleSelectDlg = 0;
	cxStyleCustomizeDlg = 0;
	cyStyleCustomizeDlg = 0;

	IniDeleteAllSection(INI_SECTION_NAME_WINDOW_POSITION);
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
	if (StrHasPrefixCase(opt, L"LE")){
		flag = idmLE;
		opt += CSTRLEN(L"LE");
		if (*opt == '-') {
			++opt;
		}
	} else if (StrHasPrefixCase(opt, L"BE")) {
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
	LPWSTR opt = lp1 + 1;
	// only accept /opt, -opt, --opt
	if (*opt == L'-') {
		++opt;
	}
	if (*opt == L'\0') {
		return 0;
	}

	int state = 0;
	const int ch = ToUpperA(*opt);

	if (opt[1] == L'\0') {
		switch (ch) {
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
			iInitialLexer = NP2LEX_TEXTFILE;
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
				lstrcpyn(szIniFile, lp1, COUNTOF(szIniFile));
				TrimString(szIniFile);
				PathUnquoteSpaces(szIniFile);
				state = 1;
			}
			break;

		case L'G':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
#if defined(_WIN64)
				int64_t cord[2] = { 0 };
				const int itok = ParseCommaList64(lp1, cord, COUNTOF(cord));
#else
				int cord[2] = { 0 };
				const int itok = ParseCommaList(lp1, cord, COUNTOF(cord));
#endif
				if (itok != 0) {
					flagJumpTo = 1;
					state = 1;
					iInitialLine = cord[0];
					iInitialColumn = cord[1];
				}
			}
			break;

		case L'H':
			if (lpSchemeArg) {
				LocalFree(lpSchemeArg);
				lpSchemeArg = NULL;
			}
			iInitialLexer = NP2LEX_HTML;
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
				lstrcpyn(szTitleExcerpt, lp1, COUNTOF(szTitleExcerpt));
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
			iInitialLexer = NP2LEX_XML;
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
	} else if (opt[2] == L'\0') {
		const int chNext = ToUpperA(opt[1]);
		switch (ch) {
		case L'C':
			if (chNext == L'R') {
				flagSetEOLMode = IDM_LINEENDINGS_CR - IDM_LINEENDINGS_CRLF + 1;
				state = 1;
			}
			break;

		case L'F':
			if (chNext == L'0' || chNext == L'O') {
				lstrcpy(szIniFile, L"*?");
				state = 1;
			}
			break;

		case L'L':
			if (chNext == L'F') {
				flagSetEOLMode = IDM_LINEENDINGS_LF - IDM_LINEENDINGS_CRLF + 1;
				state = 1;
			} else if (chNext == L'0' || chNext == L'-' || chNext == L'O') {
				flagChangeNotify = 1;
				state = 1;
			}
			break;

		case L'N':
			if (chNext == L'S') {
				flagReuseWindow = 0;
				flagNoReuseWindow = 1;
				flagSingleFileInstance = 1;
				state = 1;
			}
			break;

		case L'O':
			if (chNext == L'0' || chNext == L'-' || chNext == L'O') {
				flagAlwaysOnTop = 1;
				state = 1;
			}
			break;

		case L'R':
			if (chNext == L'S') {
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
	switch (ch) {
	case L'A':
		if (StrCaseEqual(opt, L"ANSI")) {
			flagSetEncoding = IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1;
			state = 1;
		} else if (StrHasPrefixCase(opt, L"appid=")) {
			// Shell integration
			opt += CSTRLEN(L"appid=");
			lstrcpyn(g_wchAppUserModelID, opt, COUNTOF(g_wchAppUserModelID));
			StrTrim(g_wchAppUserModelID, L"\" ");
			if (StrIsEmpty(g_wchAppUserModelID)) {
				lstrcpy(g_wchAppUserModelID, MY_APPUSERMODELID);
			}
			state = 1;
		}
		break;

	case L'C':
		if (opt[1] == L'R' || opt[1] == L'r') {
			opt += 2;
			if (*opt == L'-') {
				++opt;
			}
			if (opt[2] == L'\0' && (*opt == L'L' || *opt == L'l') && (opt[1] == L'F' || opt[1] == L'f')) {
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
			switch (ToUpperA(*opt)) {
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
			if (*opt != L'\0') {
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
			if (opt[1] == L'T' || opt[1] == L't') {
				ExtractFirstArgument(lp2, lp1, lp2);
			}
			state = 1;
			break;
		}

		if (StrHasPrefixCase(opt, L"POS")) {
			opt += CSTRLEN(L"POS");
		} else {
			++opt;
		}
		if (*opt == L':') {
			++opt;
		}

		switch (ToUpperA(*opt)) {
		case L'0':
		case L'O':
			if (opt[1] == L'\0') {
				flagPosParam = 1;
				flagDefaultPos = 1;
				state = 1;
			}
			break;

		case L'D':
		case L'S':
			if (opt[1] == L'\0' || (opt[2] == L'\0' && (opt[1] == L'L' || opt[1] == L'l'))) {
				flagPosParam = 1;
				flagDefaultPos = (opt[1] == L'\0')? 2 : 3;
				state = 1;
			}
			break;

		case L'F':
		case L'L':
		case L'R':
		case L'T':
		case L'B':
		case L'M': {
			LPCWSTR p = opt;
			flagPosParam = 1;
			flagDefaultPos = 0;
			state = 1;
			while (*p && state == 1) {
				switch (ToUpperA(*p)) {
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
				int cord[5] = { 0 };
				const int itok = ParseCommaList(lp1, cord, COUNTOF(cord));
				if (itok >= 4) {
					flagPosParam = 1;
					flagDefaultPos = 0;
					state = 1;
					wi.x = cord[0];
					wi.y = cord[1];
					wi.cx = cord[2];
					wi.cy = cord[3];
					wi.max = cord[4] != 0;
					if (wi.cx < 1) {
						wi.cx = CW_USEDEFAULT;
					}
					if (wi.cy < 1) {
						wi.cy = CW_USEDEFAULT;
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
		if (StrHasPrefixCase(opt, L"sysmru=")) {
			opt += CSTRLEN(L"sysmru=");
			if (opt[1] == L'\0') {
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
		if (StrHasPrefixCase(opt, L"UTF")) {
			opt += CSTRLEN(L"UTF");
			if (*opt == '-') {
				++opt;
			}
			if (*opt == L'8') {
				++opt;
				if (*opt == '-') {
					++opt;
				}
				if (*opt == L'\0') {
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
		} else if (StrHasPrefixCase(opt, L"UNICODE")) {
			opt += CSTRLEN(L"UNICODE");
			state = ParseCommandLineEncoding(opt, IDM_ENCODING_UNICODE, IDM_ENCODING_UNICODEREV);
		}
		break;
	}

	return state;
}

void ParseCommandLine(void) {
	LPWSTR lpCmdLine = GetCommandLine();
	const size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLine) + 1);

	if (cmdSize == sizeof(WCHAR)) {
		return;
	}

#if 0
	FILE *fp = fopen("args-dump.txt", "wb");
	fwrite("\xFF\xFE", 1, 2, fp);
	fwrite(lpCmdLine, 1, cmdSize - 2, fp);
	fclose(fp);
#endif

	// Good old console can also send args separated by Tabs
	StrTab2Space(lpCmdLine);

	LPWSTR lp1 = (LPWSTR)NP2HeapAlloc(cmdSize);
	LPWSTR lp3 = (LPWSTR)NP2HeapAlloc(cmdSize);

	// Start with 2nd argument
	if (!(ExtractFirstArgument(lpCmdLine, lp1, lp3) && *lp3)) {
		NP2HeapFree(lp1);
		NP2HeapFree(lp3);
		return;
	}

	BOOL bIsFileArg = FALSE;
	BOOL bIsNotepadReplacement = FALSE;
	LPWSTR lp2 = (LPWSTR)NP2HeapAlloc(cmdSize);
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
			LPWSTR lpFileBuf = (LPWSTR)NP2HeapAlloc(cmdSize);

			if (lpFileArg) {
				NP2HeapFree(lpFileArg);
			}

			lpFileArg = (LPWSTR)NP2HeapAlloc(sizeof(WCHAR) * (MAX_PATH + 2)); // changed for ActivatePrevInst() needs
			if (flagMultiFileArg == 2) {
				// multiple file arguments with quoted spaces
				lstrcpyn(lpFileArg, lp1, MAX_PATH);
			} else {
				lstrcpyn(lpFileArg, lp3, MAX_PATH);
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
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_FLAGS);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;
	IniSectionInit(pIniSection, 64);

	LoadIniSection(INI_SECTION_NAME_FLAGS, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	uiLanguage = (LANGID)IniSectionGetInt(pIniSection, L"UILanguage", LANG_USER_DEFAULT);
	ValidateUILangauge();
#endif

	bSingleFileInstance = IniSectionGetBool(pIniSection, L"SingleFileInstance", 1);
	bReuseWindow = IniSectionGetBool(pIniSection, L"ReuseWindow", 0);

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

	IniSectionGetString(pIniSection, L"DefaultDirectory", L"", tchDefaultDir, COUNTOF(tchDefaultDir));

	dwFileCheckInterval = IniSectionGetInt(pIniSection, L"FileCheckInterval", 1000);
	dwAutoReloadTimeout = IniSectionGetInt(pIniSection, L"AutoReloadTimeout", 1000);

	flagNoFadeHidden = IniSectionGetBool(pIniSection, L"NoFadeHidden", 0);

	int iValue = IniSectionGetInt(pIniSection, L"OpacityLevel", 75);
	iOpacityLevel = validate_i(iValue, 0, 100, 75);

	iValue = IniSectionGetInt(pIniSection, L"FindReplaceOpacityLevel", 75);
	iFindReplaceOpacityLevel = validate_i(iValue, 0, 100, 75);

	iValue = IniSectionGetInt(pIniSection, L"ToolbarLook", 1);
	flagToolbarLook = clamp_i(iValue, 0, 2);

	flagSimpleIndentGuides = IniSectionGetBool(pIniSection, L"SimpleIndentGuides", 0);
	fNoHTMLGuess = IniSectionGetBool(pIniSection, L"NoHTMLGuess", 0);
	fNoCGIGuess = IniSectionGetBool(pIniSection, L"NoCGIGuess", 0);
	fNoAutoDetection = IniSectionGetBool(pIniSection, L"NoAutoDetection", 0);
	fNoFileVariables = IniSectionGetBool(pIniSection, L"NoFileVariables", 0);

	if (StrIsEmpty(g_wchAppUserModelID)) {
		LPCWSTR strValue = IniSectionGetValue(pIniSection, L"ShellAppUserModelID");
		if (StrNotEmpty(strValue)) {
			lstrcpyn(g_wchAppUserModelID, strValue, COUNTOF(g_wchAppUserModelID));
		} else {
			lstrcpy(g_wchAppUserModelID, MY_APPUSERMODELID);
		}
	}

	if (flagUseSystemMRU == 0) {
		if (IniSectionGetBool(pIniSection, L"ShellUseSystemMRU", 1)) {
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
BOOL CheckIniFile(LPWSTR lpszFile, LPCWSTR lpszModule) {
	WCHAR tchFileExpanded[MAX_PATH];
	ExpandEnvironmentStrings(lpszFile, tchFileExpanded, COUNTOF(tchFileExpanded));

	if (PathIsRelative(tchFileExpanded)) {
		WCHAR tchBuild[MAX_PATH];
		// program directory
		lstrcpy(tchBuild, lpszModule);
		lstrcpy(PathFindFileName(tchBuild), tchFileExpanded);
		if (PathIsFile(tchBuild)) {
			lstrcpy(lpszFile, tchBuild);
			return TRUE;
		}

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
		const int csidlList[] = {
			// %LOCALAPPDATA%
			// C:\Users\<username>\AppData\Local
			// C:\Documents and Settings\<username>\Local Settings\Application Data
			CSIDL_LOCAL_APPDATA,
			// %APPDATA%
			// C:\Users\<username>\AppData\Roaming
			// C:\Documents and Settings\<username>\Application Data
			CSIDL_APPDATA,
			// Home
			// C:\Users\<username>
			CSIDL_PROFILE,
		};
		for (UINT i = 0; i < COUNTOF(csidlList); i++) {
			if (S_OK == SHGetFolderPath(NULL, csidlList[i], NULL, SHGFP_TYPE_CURRENT, tchBuild)) {
				PathAppend(tchBuild, WC_NOTEPAD2);
				PathAppend(tchBuild, tchFileExpanded);
				if (PathIsFile(tchBuild)) {
					lstrcpy(lpszFile, tchBuild);
					return TRUE;
				}
			}
		}
#else
		REFKNOWNFOLDERID rfidList[] = {
			&FOLDERID_LocalAppData,
			&FOLDERID_RoamingAppData,
			&FOLDERID_Profile,
		};
		for (UINT i = 0; i < COUNTOF(rfidList); i++) {
			LPWSTR pszPath = NULL;
			if (S_OK == SHGetKnownFolderPath(rfidList[i], KF_FLAG_DEFAULT, NULL, &pszPath)) {
				lstrcpy(tchBuild, pszPath);
				CoTaskMemFree(pszPath);
				PathAppend(tchBuild, WC_NOTEPAD2);
				PathAppend(tchBuild, tchFileExpanded);
				if (PathIsFile(tchBuild)) {
					lstrcpy(lpszFile, tchBuild);
					return TRUE;
				}
			}
		}
#endif
	} else if (PathIsFile(tchFileExpanded)) {
		lstrcpy(lpszFile, tchFileExpanded);
		return TRUE;
	}

	return FALSE;
}

BOOL CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule, LPCWSTR redirectKey) {
	WCHAR tch[MAX_PATH];
	if (GetPrivateProfileString(INI_SECTION_NAME_NOTEPAD2, redirectKey, L"", tch, COUNTOF(tch), lpszFile)) {
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
		return TRUE;
	}
	return FALSE;
}

BOOL FindIniFile(void) {
	WCHAR tchTest[MAX_PATH];
	WCHAR tchModule[MAX_PATH];
	GetModuleFileName(NULL, tchModule, COUNTOF(tchModule));

	if (StrNotEmpty(szIniFile)) {
		if (StrEqual(szIniFile, L"*?")) {
			return FALSE;
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
		return TRUE;
	}

	lstrcpy(tchTest, PathFindFileName(tchModule));
	PathRenameExtension(tchTest, L".ini");
	BOOL bFound = CheckIniFile(tchTest, tchModule);

	if (!bFound) {
		lstrcpy(tchTest, L"Notepad2.ini");
		bFound = CheckIniFile(tchTest, tchModule);
	}

	if (bFound) {
		// allow two redirections: administrator -> user -> custom
		if (CheckIniFileRedirect(tchTest, tchModule, L"Notepad2.ini")) {
			CheckIniFileRedirect(tchTest, tchModule, L"Notepad2.ini");
		}
		lstrcpy(szIniFile, tchTest);
	} else {
		lstrcpy(szIniFile, tchModule);
		PathRenameExtension(szIniFile, L".ini");
	}

	return TRUE;
}

BOOL TestIniFile(void) {
	if (StrEqual(szIniFile, L"*?")) {
		lstrcpy(szIniFile2, L"");
		lstrcpy(szIniFile, L"");
		return FALSE;
	}

	DWORD dwFileAttributes = GetFileAttributes(szIniFile);
	if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
		return TRUE;
	}

	if ((dwFileAttributes != INVALID_FILE_ATTRIBUTES) || *CharPrev(szIniFile, StrEnd(szIniFile)) == L'\\') {
		WCHAR wchModule[MAX_PATH];
		GetModuleFileName(NULL, wchModule, COUNTOF(wchModule));
		PathAppend(szIniFile, PathFindFileName(wchModule));
		PathRenameExtension(szIniFile, L".ini");
		dwFileAttributes = GetFileAttributes(szIniFile);
		if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			lstrcpy(PathFindFileName(szIniFile), L"Notepad2.ini");
			dwFileAttributes = GetFileAttributes(szIniFile);
			if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
				lstrcpy(PathFindFileName(szIniFile), PathFindFileName(wchModule));
				PathRenameExtension(szIniFile, L".ini");
				dwFileAttributes = GetFileAttributes(szIniFile);
			}
		}
	}

	if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		lstrcpy(szIniFile2, szIniFile);
		lstrcpy(szIniFile, L"");
		return FALSE;
	}

	return TRUE;
}

void FindExtraIniFile(LPWSTR lpszIniFile, LPCWSTR defaultName, LPCWSTR redirectKey) {
	if (StrNotEmpty(szIniFile)) {
		WCHAR tch[MAX_PATH];
		if (GetPrivateProfileString(INI_SECTION_NAME_NOTEPAD2, redirectKey, L"", tch, COUNTOF(tch), szIniFile)) {
			if (FindUserResourcePath(tch, lpszIniFile)) {
				return;
			}
		}
	}
	if (FindUserResourcePath(defaultName, lpszIniFile)) {
		return;
	}

	if (StrNotEmpty(szIniFile)) {
		// relative to program ini file
		lstrcpy(lpszIniFile, szIniFile);
	} else {
		// relative to program exe file
		GetModuleFileName(NULL, lpszIniFile, MAX_PATH);
	}
	lstrcpy(PathFindFileName(lpszIniFile), defaultName);
}

BOOL CreateIniFile(LPCWSTR lpszIniFile) {
	if (StrNotEmpty(lpszIniFile)) {
		WCHAR *pwchTail;

		if ((pwchTail = StrRChr(lpszIniFile, NULL, L'\\')) != NULL) {
			*pwchTail = 0;
			SHCreateDirectoryEx(NULL, lpszIniFile, NULL);
			*pwchTail = L'\\';
		}

		HANDLE hFile = CreateFile(lpszIniFile,
						   GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
						   NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		dwLastIOError = GetLastError();
		if (hFile != INVALID_HANDLE_VALUE) {
			LARGE_INTEGER fileSize;
			fileSize.QuadPart = 0;
			if (GetFileSizeEx(hFile, &fileSize) && fileSize.QuadPart < 2) {
				DWORD dw;
				WriteFile(hFile, (LPCVOID)L"\xFEFF[Notepad2]\r\n", 26, &dw, NULL);
			}
			CloseHandle(hFile);
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
// UpdateToolbar()
//
//
#define EnableTool(id, b)		SendMessage(hwndToolbar, TB_ENABLEBUTTON, id, MAKELPARAM(((b) ? 1 : 0), 0))
#define CheckTool(id, b)		SendMessage(hwndToolbar, TB_CHECKBUTTON, id, MAKELPARAM(b, 0))

void UpdateToolbar(void) {
	if (!bShowToolbar || !bInitDone) {
		return;
	}

	EnableTool(IDT_FILE_ADDTOFAV, StrNotEmpty(szCurFile));

	EnableTool(IDT_FILE_SAVE, IsDocumentModified());
	EnableTool(IDT_EDIT_UNDO, SciCall_CanUndo() /*&& !bReadOnly*/);
	EnableTool(IDT_EDIT_REDO, SciCall_CanRedo() /*&& !bReadOnly*/);
	EnableTool(IDT_EDIT_PASTE, SciCall_CanPaste() /*&& !bReadOnly*/);

	int i = SciCall_GetLength() != 0;
	EnableTool(IDT_EDIT_CUT, i /*&& !bReadOnly*/);
	EnableTool(IDT_EDIT_COPY, i);
	EnableTool(IDT_EDIT_FIND, i);
	//EnableTool(IDT_EDIT_FINDNEXT, i);
	//EnableTool(IDT_EDIT_FINDPREV, i && StrNotEmptyA(efrData.szFind));
	EnableTool(IDT_EDIT_REPLACE, i /*&& !bReadOnly*/);
	EnableTool(IDT_EDIT_DELETE, i /*&& !bReadOnly*/);

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
	if (!bShowStatusbar || !bInitDone) {
		return;
	}

	const Sci_Position iPos = SciCall_GetCurrentPos();
	const Sci_Line iLine = SciCall_LineFromPosition(iPos);
	const Sci_Line iLines = SciCall_GetLineCount();

#if 0
	StopWatch watch;
	StopWatch_Start(watch);
#endif
	struct Sci_TextToFind ft = { { SciCall_PositionFromLine(iLine), iPos }, NULL, { 0, 0 } };
	SciCall_CountCharactersAndColumns(&ft);
	const Sci_Position iChar = ft.chrgText.cpMin;
	const Sci_Position iCol = ft.chrgText.cpMax;
	Sci_Position iLineChar;
	Sci_Position iLineColumn;

	const UINT updateMask = cachedStatusItem.updateMask;
	if ((updateMask & StatusBarUpdateMask_LineColumn) || (iLine != cachedStatusItem.iLine)) {
		ft.chrg.cpMin = ft.chrg.cpMax;
		ft.chrg.cpMax = SciCall_GetLineEndPosition(iLine);
		SciCall_CountCharactersAndColumns(&ft);
		iLineChar = ft.chrgText.cpMin;
		iLineColumn = ft.chrgText.cpMax;
		cachedStatusItem.iLine = iLine;
		cachedStatusItem.iLineChar = iLineChar;
		cachedStatusItem.iLineColumn = iLineColumn;
	} else {
		iLineChar = cachedStatusItem.iLineChar;
		iLineColumn = cachedStatusItem.iLineColumn;
	}
#if 0
	StopWatch_Stop(watch);
	StopWatch_ShowLog(&watch, "CountCharacters time");
#endif

	WCHAR tchCurLine[32];
	WCHAR tchDocLine[32];
	PosToStrW(iLine + 1, tchCurLine);
	PosToStrW(iLines, tchDocLine);
	FormatNumberStr(tchCurLine);
	FormatNumberStr(tchDocLine);

	WCHAR tchCurColumn[32];
	WCHAR tchLineColumn[32];
	PosToStrW(iCol + 1, tchCurColumn);
	PosToStrW(iLineColumn, tchLineColumn);
	FormatNumberStr(tchCurColumn);
	FormatNumberStr(tchLineColumn);

	WCHAR tchCurChar[32];
	WCHAR tchLineChar[32];
	PosToStrW(iChar + 1, tchCurChar);
	PosToStrW(iLineChar, tchLineChar);
	FormatNumberStr(tchCurChar);
	FormatNumberStr(tchLineChar);

	WCHAR tchSelByte[32];
	WCHAR tchSelChar[32];
	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	if (iSelStart == iSelEnd) {
		lstrcpy(tchSelByte, L"0");
		lstrcpy(tchSelChar, L"0");
	} else if (!SciCall_IsRectangleSelection()) {
		Sci_Position iSel = SciCall_GetSelTextLength() - 1;
		PosToStrW(iSel, tchSelByte);
		FormatNumberStr(tchSelByte);
		iSel = SciCall_CountCharacters(iSelStart, iSelEnd);
		PosToStrW(iSel, tchSelChar);
		FormatNumberStr(tchSelChar);
	} else {
		lstrcpy(tchSelByte, L"--");
		lstrcpy(tchSelChar, L"--");
	}

	// Print number of selected lines in statusbar
	WCHAR tchLinesSelected[32];
	WCHAR tchMatchesCount[32];
	if (iSelStart == iSelEnd) {
		lstrcpy(tchLinesSelected, L"0");
	} else {
		const Sci_Line iStartLine = SciCall_LineFromPosition(iSelStart);
		const Sci_Line iEndLine = SciCall_LineFromPosition(iSelEnd);
		const Sci_Position iStartOfLinePos = SciCall_PositionFromLine(iEndLine);
		Sci_Line iLinesSelected = iEndLine - iStartLine;
		if (iSelStart != iSelEnd && iStartOfLinePos != iSelEnd) {
			iLinesSelected += 1;
		}
		PosToStrW(iLinesSelected, tchLinesSelected);
		FormatNumberStr(tchLinesSelected);
	}

	// find all and mark occurrences
	PosToStrW(editMarkAllStatus.matchCount, tchMatchesCount);
	FormatNumberStr(tchMatchesCount);
	if (editMarkAllStatus.pending) {
		lstrcat(tchMatchesCount, L" ...");
	}

	WCHAR tchDocPos[256];
	wsprintf(tchDocPos, cachedStatusItem.tchDocPosFmt, tchCurLine, tchDocLine,
				 tchCurColumn, tchLineColumn, tchCurChar, tchLineChar,
				 tchSelChar, tchSelByte, tchLinesSelected, tchMatchesCount);

	WCHAR tchDocSize[32];
	const Sci_Position iBytes = SciCall_GetLength();
	StrFormatByteSize(iBytes, tchDocSize, COUNTOF(tchDocSize));

	StatusSetText(hwndStatus, STATUS_DOCPOS, tchDocPos);
	StatusSetText(hwndStatus, STATUS_DOCSIZE, tchDocSize);
	if (updateMask & StatusBarUpdateMask_Lexer) {
		StatusSetText(hwndStatus, STATUS_LEXER, cachedStatusItem.pszLexerName);
	}
	if (updateMask & StatusBarUpdateMask_CodePage) {
		StatusSetText(hwndStatus, STATUS_CODEPAGE, mEncoding[iEncoding].wchLabel);
	}
	if (updateMask & StatusBarUpdateMask_EOLMode) {
		StatusSetText(hwndStatus, STATUS_EOLMODE, cachedStatusItem.pszEOLMode);
	}
	if (updateMask & StatusBarUpdateMask_OVRMode) {
		StatusSetText(hwndStatus, STATUS_OVRMODE, cachedStatusItem.pszOvrMode);
	}
	if (updateMask & StatusBarUpdateMask_DocZoom) {
		StatusSetText(hwndStatus, STATUS_DOCZOOM, cachedStatusItem.tchZoom);
	}
	cachedStatusItem.updateMask = 0;
}

//=============================================================================
//
// UpdateLineNumberWidth()
//
//
void UpdateLineNumberWidth(void) {
	int width = 0;
	if (bShowLineNumbers) {
#if NP2_DEBUG_FOLD_LEVEL
		width = 100;
#else
		char tchLines[32];

		const Sci_Line iLines = SciCall_GetLineCount();
		PosToStrA(iLines, tchLines + 2);
		tchLines[0] = '_';
		tchLines[1] = '_';

		width = SciCall_TextWidth(STYLE_LINENUMBER, tchLines);
#endif
	}
	SciCall_SetMarginWidth(MarginNumber_LineNumber, width);
}

// based on SciTEWin::FullScreenToggle()
void ToggleFullScreenMode(void) {
	static BOOL bSaved;
	static WINDOWPLACEMENT wndpl;
	static RECT rcWorkArea;
	static DWORD exStyle;

	HWND wTaskBar = FindWindow(L"Shell_TrayWnd", L"");
	HWND wStartButton = FindWindow(L"Button", NULL);

	if (bInFullScreenMode) {
		if (!bSaved) {
			bSaved = TRUE;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
			wndpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwndMain, &wndpl);
			exStyle = GetWindowExStyle(hwndEdit);
		}

		HMONITOR hMonitor = MonitorFromWindow(hwndMain, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		const int x = mi.rcMonitor.left;
		const int y = mi.rcMonitor.top;
		const int w = mi.rcMonitor.right - x;
		const int h = mi.rcMonitor.bottom - y;
		const int cx = SystemMetricsForDpi(SM_CXSIZEFRAME, g_uCurrentDPI);
		const int cy = SystemMetricsForDpi(SM_CYSIZEFRAME, g_uCurrentDPI);

		int top = cy;
		if (iFullScreenMode & (FullScreenMode_HideCaption | FullScreenMode_HideMenu)) {
			top += SystemMetricsForDpi(SM_CYCAPTION, g_uCurrentDPI);
		}
		if (iFullScreenMode & FullScreenMode_HideMenu) {
			top += SystemMetricsForDpi(SM_CYMENU, g_uCurrentDPI);
		}

		SystemParametersInfo(SPI_SETWORKAREA, 0, NULL, SPIF_SENDCHANGE);

		if (wStartButton) {
			ShowWindow(wStartButton, SW_HIDE);
		}
		ShowWindow(wTaskBar, SW_HIDE);

		SetWindowExStyle(hwndEdit, 0);
		SetWindowPos(hwndMain, (IsTopMost() ? HWND_TOPMOST : HWND_TOP), x - cx, y - top, x + w + 2 * cx , y + h + top + cy, 0);
	} else {
		bSaved = FALSE;
		ShowWindow(wTaskBar, SW_SHOW);
		if (wStartButton) {
			ShowWindow(wStartButton, SW_SHOW);
		}
		SetWindowExStyle(hwndEdit, exStyle);
		if (!IsTopMost()) {
			SetWindowPos(hwndMain, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		if (wndpl.length) {
			SystemParametersInfo(SPI_SETWORKAREA, 0, &rcWorkArea, 0);
			if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
				ShowWindow(hwndMain, SW_RESTORE);
				ShowWindow(hwndMain, SW_SHOWMAXIMIZED);
			} else {
				SetWindowPlacement(hwndMain, &wndpl);
			}
		}
	}
	SetForegroundWindow(hwndMain);
}

//=============================================================================
//
//	FileIO()
//
//
BOOL FileIO(BOOL fLoad, LPWSTR pszFile, BOOL bFlag, EditFileIOStatus *status) {
	BeginWaitCursor();

	WCHAR tch[MAX_PATH + 128];
	WCHAR fmt[128];
	FormatString(tch, fmt, (fLoad ? IDS_LOADFILE : IDS_SAVEFILE), pszFile);

	StatusSetText(hwndStatus, STATUS_HELP, tch);
	StatusSetSimple(hwndStatus, TRUE);

	InvalidateRect(hwndStatus, NULL, TRUE);
	UpdateWindow(hwndStatus);

	const BOOL fSuccess = fLoad ? EditLoadFile(pszFile, bFlag, status) : EditSaveFile(hwndEdit, pszFile, bFlag, status);
	const DWORD dwFileAttributes = GetFileAttributes(pszFile);
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
	BOOL fSuccess = FALSE;
	BOOL bRestoreView = FALSE;
	Sci_Position iCurPos = 0;
	Sci_Position iAnchorPos = 0;
	Sci_Line iLine = 0;
	Sci_Position iCol = 0;
	Sci_Line iVisTopLine = 0;
	Sci_Line iDocTopLine = 0;
	int iXOffset = 0;
	int keepTitleExcerpt = fKeepTitleExcerpt;
	int lexerSpecified = flagLexerSpecified;

	if (!bNew && StrNotEmpty(lpszFile)) {
		lstrcpy(tch, lpszFile);
		if (lpszFile == szCurFile || StrCaseEqual(lpszFile, szCurFile)) {
			iCurPos = SciCall_GetCurrentPos();
			iAnchorPos = SciCall_GetAnchor();
			iLine = SciCall_LineFromPosition(iCurPos) + 1;
			iCol = SciCall_GetColumn(iCurPos) + 1;
			iVisTopLine = SciCall_GetFirstVisibleLine();
			iDocTopLine = SciCall_DocLineFromVisible(iVisTopLine);
			iXOffset = SciCall_GetXOffset();
			bRestoreView = iLine > 1 || iCol > 1;
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
		EditSetEmptyText();
		bModified = FALSE;
		bReadOnly = FALSE;
		iEOLMode = GetScintillaEOLMode(iDefaultEOLMode);
		SciCall_SetEOLMode(iEOLMode);
		iEncoding = iDefaultEncoding;
		iOriginalEncoding = iDefaultEncoding;
		SciCall_SetCodePage((iDefaultEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
		Style_SetLexer(NULL, TRUE);
		UpdateStatusBarCache(STATUS_CODEPAGE);
		UpdateStatusBarCache(STATUS_EOLMODE);
		UpdateStatusBarCacheLineColumn();
		UpdateDocumentModificationStatus();

		// Terminate file watching
		if (bResetFileWatching) {
			iFileWatchingMode = 0;
		}
		InstallFileWatching(TRUE);

		return TRUE;
	}

	if (!fSuccess) {
		if (!OpenFileDlg(hwndMain, tch, COUNTOF(tch), NULL)) {
			return FALSE;
		}
	}
	fSuccess = FALSE;

	WCHAR szFileName[MAX_PATH] = L"";
	ExpandEnvironmentStringsEx(tch, COUNTOF(tch));

	if (PathIsRelative(tch)) {
		lstrcpyn(szFileName, g_wchWorkingDirectory, COUNTOF(szFileName));
		PathAppend(szFileName, tch);
	} else {
		lstrcpy(szFileName, tch);
	}

	PathCanonicalizeEx(szFileName);
	GetLongPathNameEx(szFileName, COUNTOF(szFileName));

	if (PathIsLnkFile(szFileName)) {
		PathGetLnkPath(szFileName, szFileName, COUNTOF(szFileName));
	}

#if NP2_USE_DESIGNATED_INITIALIZER
	EditFileIOStatus status = {
		.iEncoding = iEncoding,
		.iEOLMode = iEOLMode,
	};
#else
	EditFileIOStatus status = { iEncoding, iEOLMode };
#endif

	// Ask to create a new file...
	if (!bReload && !PathIsFile(szFileName)) {
		int result = IDCANCEL;
		if (flagQuietCreate || (result = MsgBoxWarn(MB_YESNOCANCEL, IDS_ASK_CREATE, szFileName)) == IDYES) {
			HANDLE hFile = CreateFile(szFileName,
									  GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
									  NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			dwLastIOError = GetLastError();
			if (hFile != INVALID_HANDLE_VALUE) {
				fSuccess = TRUE;
				CloseHandle(hFile);
				FileVars_Init(NULL, 0, &fvCurFile);
				EditSetEmptyText();
				iEOLMode = GetScintillaEOLMode(iDefaultEOLMode);
				SciCall_SetEOLMode(iEOLMode);
				if (iSrcEncoding != -1) {
					iEncoding = iSrcEncoding;
					iOriginalEncoding = iSrcEncoding;
				} else {
					iEncoding = iDefaultEncoding;
					iOriginalEncoding = iDefaultEncoding;
				}
				SciCall_SetCodePage((iEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
				Style_SetLexer(NULL, TRUE);
				bReadOnly = FALSE;
			}
		} else if (result == IDCANCEL) {
			PostWMCommand(hwndMain, IDM_FILE_EXIT);
			return FALSE;
		} else {
			return FALSE;
		}
	} else {
		fSuccess = FileIO(TRUE, szFileName, bNoEncDetect, &status);
		if (fSuccess) {
			iEncoding = status.iEncoding;
			iEOLMode = status.iEOLMode;
		}
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
		SciCall_SetEOLMode(iEOLMode);
		UpdateStatusBarCache(STATUS_CODEPAGE);
		UpdateStatusBarCache(STATUS_EOLMODE);
		UpdateStatusBarCacheLineColumn();

		BOOL bUnknownFile = FALSE;
		if (!lexerSpecified) {
			if (flagLexerSpecified) {
				if (pLexCurrent->rid == iInitialLexer) {
					UpdateLineNumberWidth();
				} else if (lpSchemeArg) {
					Style_SetLexerFromName(szCurFile, lpSchemeArg);
					LocalFree(lpSchemeArg);
					lpSchemeArg = NULL;
				} else {
					Style_SetLexerFromID(iInitialLexer);
				}
				flagLexerSpecified = 0;
			} else {
				np2LexLangIndex = 0;
				bUnknownFile = !Style_SetLexerFromFile(szCurFile);
			}
		} else {
			UpdateLineNumberWidth();
		}

		MRU_AddFile(pFileMRU, szFileName, flagRelativeFileMRU, flagPortableMyDocs);
		if (flagUseSystemMRU == 2) {
			SHAddToRecentDocs(SHARD_PATHW, szFileName);
		}

		// Install watching of the current file
		if (!bReload && bResetFileWatching) {
			iFileWatchingMode = 0;
		}
		InstallFileWatching(FALSE);

		// check for binary file (file with unknown encoding: ANSI)
		const BOOL binary = (iEncoding == CPI_DEFAULT) && Style_MaybeBinaryFile(szCurFile);
		// lock binary file for editing
		if (binary) {
			bLockedForEditing = TRUE;
			SciCall_SetReadOnly(TRUE);
		} else {
#if NP2_ENABLE_DOT_LOG_FEATURE
			if (IsFileStartsWithDotLog()) {
				bRestoreView = TRUE;
				SciCall_DocumentEnd();
				SciCall_BeginUndoAction();
				SciCall_NewLine();
				SendWMCommand(hwndMain, IDM_EDIT_INSERT_SHORTDATE);
				SciCall_DocumentEnd();
				SciCall_NewLine();
				SciCall_EndUndoAction();
				SciCall_DocumentEnd();
			}
#endif
			if (bRestoreView) {
				SciCall_SetSel(iAnchorPos, iCurPos);
				const Sci_Line iCurLine = iLine - SciCall_LineFromPosition(SciCall_GetCurrentPos());
				if (abs_pos(iCurLine) > 5) {
					EditJumpTo(iLine, iCol);
				} else {
					SciCall_EnsureVisible(iDocTopLine);
					const Sci_Line iNewTopLine = SciCall_GetFirstVisibleLine();
					SciCall_LineScroll(0, iVisTopLine - iNewTopLine);
					SciCall_SetXOffset(iXOffset);
				}
				EditEnsureSelectionVisible();
			}
		}

		if (!bInitDone) {
			bInitDone = TRUE;
			UpdateStatusBarWidth();
		}
		UpdateStatusbar();
		UpdateDocumentModificationStatus();
		// Show warning: Unicode file loaded as ANSI
		if (status.bUnicodeErr) {
			MsgBoxWarn(MB_OK, IDS_ERR_UNICODE);
		}
		// notify binary file been locked for editing
		if (binary) {
			ShowNotificationMessage(SC_NOTIFICATIONPOSITION_BOTTOMRIGHT, IDS_BINARY_FILE_LOCKED);
			return fSuccess;
		}
		// Show inconsistent line endings warning
		if (status.bInconsistent && bWarnLineEndings) {
			// file with unknown lexer and unknown encoding
			bUnknownFile = bUnknownFile && (iEncoding == CPI_DEFAULT);
			// Set default button to "No" for diff/patch and unknown file.
			// diff/patch file may contains content from files with different line endings.
			status.bLineEndingsDefaultNo = bUnknownFile || pLexCurrent->iLexer == SCLEX_DIFF;
			if (WarnLineEndingDlg(hwndMain, &status)) {
				const int iNewEOLMode = GetScintillaEOLMode(status.iEOLMode);
				ConvertLineEndings(iNewEOLMode);
			}
		}
	} else if (!status.bFileTooBig) {
		MsgBoxLastError(MB_OK, IDS_ERR_LOADFILE, szFileName);
	}

	return fSuccess;
}

//=============================================================================
//
// FileSave()
//
//
BOOL FileSave(BOOL bSaveAlways, BOOL bAsk, BOOL bSaveAs, BOOL bSaveCopy) {
	const BOOL Untitled = StrIsEmpty(szCurFile);
	BOOL bIsEmptyNewFile = FALSE;

	if (Untitled) {
		const Sci_Position cchText = SciCall_GetLength();
		if (cchText == 0) {
			bIsEmptyNewFile = TRUE;
		} else if (cchText < 2048) {
			char tchText[2048] = "";
			SciCall_GetText(COUNTOF(tchText), tchText);
			StrTrimA(tchText, " \t\n\r");
			if (StrIsEmptyA(tchText)) {
				bIsEmptyNewFile = TRUE;
			}
		}
	}

	if (!bSaveAlways && (!IsDocumentModified() || bIsEmptyNewFile) && !bSaveAs) {
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

		switch (MsgBoxAsk(MB_YESNOCANCEL, IDS_ASK_SAVE, tch)) {
		case IDCANCEL:
			return FALSE;
		case IDNO:
			return TRUE;
		}
	}

	BOOL fSuccess = FALSE;
	WCHAR tchFile[MAX_PATH];
#if NP2_USE_DESIGNATED_INITIALIZER
	EditFileIOStatus status = {
		.iEncoding = iEncoding,
		.iEOLMode = iEOLMode,
	};
#else
	EditFileIOStatus status = { iEncoding, iEOLMode };
#endif

	// Read only...
	if (!bSaveAs && !bSaveCopy && !Untitled) {
		const DWORD dwFileAttributes = GetFileAttributes(szCurFile);
		bReadOnly = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);
		if (bReadOnly) {
			UpdateWindowTitle();
			if (MsgBoxWarn(MB_YESNO, IDS_READONLY_SAVE, szCurFile) == IDYES) {
				bSaveAs = TRUE;
			} else {
				return FALSE;
			}
		}
		if (!bSaveAs) {
			fSuccess = FileIO(FALSE, szCurFile, FALSE, &status);
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

		if (SaveFileDlg(hwndMain, Untitled, tchFile, COUNTOF(tchFile), tchInitialDir)) {
			fSuccess = FileIO(FALSE, tchFile, bSaveCopy, &status);
			if (fSuccess) {
				if (!bSaveCopy) {
					lstrcpy(szCurFile, tchFile);
					SetDlgItemText(hwndMain, IDC_FILENAME, szCurFile);
					SetDlgItemInt(hwndMain, IDC_REUSELOCK, GetTickCount(), FALSE);
					if (!fKeepTitleExcerpt) {
						lstrcpy(szTitleExcerpt, L"");
					}
					if (flagLexerSpecified) {
						if (pLexCurrent->rid == iInitialLexer) {
							UpdateLineNumberWidth();
						} else if (lpSchemeArg) {
							Style_SetLexerFromName(szCurFile, lpSchemeArg);
							LocalFree(lpSchemeArg);
							lpSchemeArg = NULL;
						} else {
							Style_SetLexerFromID(iInitialLexer);
						}
						flagLexerSpecified = 0;
					} else {
						Style_SetLexerFromFile(szCurFile);
					}
				} else {
					lstrcpy(tchLastSaveCopyDir, tchFile);
					PathRemoveFileSpec(tchLastSaveCopyDir);
				}
			}
		} else {
			return FALSE;
		}
	} else if (!fSuccess) {
		fSuccess = FileIO(FALSE, szCurFile, FALSE, &status);
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
			UpdateDocumentModificationStatus();

			// Install watching of the current file
			if (bSaveAs && bResetFileWatching) {
				iFileWatchingMode = 0;
			}
			InstallFileWatching(FALSE);
		}
	} else if (!status.bCancelDataLoss) {
		if (StrNotEmpty(szCurFile) != 0) {
			lstrcpy(tchFile, szCurFile);
		}

		UpdateWindowTitle();
		MsgBoxLastError(MB_OK, IDS_ERR_SAVEFILE, tchFile);
	}

	return fSuccess;
}

//=============================================================================
//
// OpenFileDlg()
//
//
BOOL OpenFileDlg(HWND hwnd, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) {
	WCHAR tchInitialDir[MAX_PATH] = L"";
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

	WCHAR szFile[MAX_PATH];
	lstrcpy(szFile, L"");
	int lexers[1 + OPENDLG_MAX_LEXER_COUNT] = {0}; // 1-based filter index
	LPWSTR szFilter = Style_GetOpenDlgFilterStr(TRUE, szCurFile, lexers);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFile;
	ofn.lpstrInitialDir = (lpstrInitialDir) ? lpstrInitialDir : tchInitialDir;
	ofn.lpstrDefExt = L""; // auto add first extension from current filter
	ofn.nMaxFile = COUNTOF(szFile);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | /* OFN_NOCHANGEDIR |*/
				OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST |
				OFN_SHAREAWARE /*| OFN_NODEREFERENCELINKS*/ | OFN_NOVALIDATE;

	const BOOL success = GetOpenFileName(&ofn);
	if (success) {
		lstrcpyn(lpstrFile, szFile, cchFile);
		const int iLexer = lexers[ofn.nFilterIndex];
		flagLexerSpecified = iLexer != 0;
		iInitialLexer = iLexer;
	}
	NP2HeapFree(szFilter);
	return success;
}

//=============================================================================
//
// SaveFileDlg()
//
//
BOOL SaveFileDlg(HWND hwnd, BOOL Untitled, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) {
	WCHAR tchInitialDir[MAX_PATH] = L"";
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

	WCHAR szNewFile[MAX_PATH];
	lstrcpy(szNewFile, lpstrFile);
	int lexers[1 + OPENDLG_MAX_LEXER_COUNT] = {0}; // 1-based filter index
	LPWSTR szFilter = Style_GetOpenDlgFilterStr(FALSE, szCurFile, lexers);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szNewFile;
	ofn.lpstrInitialDir = tchInitialDir;
	ofn.lpstrDefExt = L""; // auto add first extension from current filter
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY /*| OFN_NOCHANGEDIR*/ |
				/*OFN_NODEREFERENCELINKS |*/ OFN_OVERWRITEPROMPT |
				OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST;

	const BOOL success = GetSaveFileName(&ofn);
	if (success) {
		lstrcpyn(lpstrFile, szNewFile, cchFile);
		const int iLexer = lexers[ofn.nFilterIndex];
		// default scheme, current scheme and selected file type all are Text File => no lexer specified.
		flagLexerSpecified = iLexer != 0 && !(Untitled && iLexer == NP2LEX_TEXTFILE && iLexer == lexers[0] && iLexer == pLexCurrent->rid);
		iInitialLexer = iLexer;
	}
	NP2HeapFree(szFilter);
	return success;
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
			const DWORD dwReuseLock = GetDlgItemInt(hwnd, IDC_REUSELOCK, NULL, FALSE);
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
			const DWORD dwReuseLock = GetDlgItemInt(hwnd, IDC_REUSELOCK, NULL, FALSE);
			if (GetTickCount() - dwReuseLock >= REUSEWINDOWLOCKTIMEOUT) {
				if (IsWindowEnabled(hwnd)) {
					bContinue = FALSE;
				}

				WCHAR tchFileName[MAX_PATH] = L"";
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
	if ((flagNoReuseWindow && !flagSingleFileInstance) || flagStartAsTrayIcon || flagNewFromClipboard || flagPasteBoard) {
		return FALSE;
	}

	if (flagSingleFileInstance && lpFileArg) {
		ExpandEnvironmentStringsEx(lpFileArg, (DWORD)(NP2HeapSize(lpFileArg) / sizeof(WCHAR)));

		if (PathIsRelative(lpFileArg)) {
			WCHAR tchTmp[MAX_PATH];
			lstrcpyn(tchTmp, g_wchWorkingDirectory, COUNTOF(tchTmp));
			PathAppend(tchTmp, lpFileArg);
			lstrcpy(lpFileArg, tchTmp);
		}

		GetLongPathNameEx(lpFileArg, MAX_PATH);

		HWND hwnd = NULL;
		EnumWindows(EnumWndProc2, (LPARAM)&hwnd);

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

				DWORD cb = sizeof(NP2PARAMS);
				if (lpSchemeArg) {
					cb += (lstrlen(lpSchemeArg) + 1) * sizeof(WCHAR);
				}

				LPNP2PARAMS params = (LPNP2PARAMS)GlobalAlloc(GPTR, cb);
				params->flagFileSpecified = FALSE;
				params->flagChangeNotify = 0;
				params->flagQuietCreate = FALSE;
				params->flagLexerSpecified = flagLexerSpecified;
				if (flagLexerSpecified && lpSchemeArg) {
					lstrcpy(StrEnd(&params->wchData) + 1, lpSchemeArg);
					params->iInitialLexer = 0;
				} else {
					params->iInitialLexer = iInitialLexer;
				}
				params->flagJumpTo = flagJumpTo;
				params->iInitialLine = iInitialLine;
				params->iInitialColumn = iInitialColumn;

				params->iSrcEncoding = (lpEncodingArg) ? Encoding_Match(lpEncodingArg) : -1;
				params->flagSetEncoding = flagSetEncoding;
				params->flagSetEOLMode = flagSetEOLMode;
				params->flagTitleExcerpt = 0;

				COPYDATASTRUCT cds;
				cds.dwData = DATA_NOTEPAD2_PARAMS;
				cds.cbData = (DWORD)GlobalSize(params);
				cds.lpData = params;

				SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&cds);
				GlobalFree(params);

				return TRUE;
			}

			// Ask...
			if (IDYES == MsgBoxAsk(MB_YESNO, IDS_ERR_PREVWINDISABLED)) {
				return FALSE;
			}
			return TRUE;
		}
	}

	if (flagNoReuseWindow) {
		return FALSE;
	}

	HWND hwnd = NULL;
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
				ExpandEnvironmentStringsEx(lpFileArg, (DWORD)(NP2HeapSize(lpFileArg) / sizeof(WCHAR)));

				if (PathIsRelative(lpFileArg)) {
					WCHAR tchTmp[MAX_PATH];
					lstrcpyn(tchTmp, g_wchWorkingDirectory, COUNTOF(tchTmp));
					PathAppend(tchTmp, lpFileArg);
					lstrcpy(lpFileArg, tchTmp);
				}

				DWORD cb = sizeof(NP2PARAMS);
				cb += (lstrlen(lpFileArg) + 1) * sizeof(WCHAR);

				if (lpSchemeArg) {
					cb += (lstrlen(lpSchemeArg) + 1) * sizeof(WCHAR);
				}

				const int cchTitleExcerpt = lstrlen(szTitleExcerpt);
				if (cchTitleExcerpt) {
					cb += (cchTitleExcerpt + 1) * sizeof(WCHAR);
				}

				LPNP2PARAMS params = (LPNP2PARAMS)GlobalAlloc(GPTR, cb);
				params->flagFileSpecified = TRUE;
				lstrcpy(&params->wchData, lpFileArg);
				params->flagChangeNotify = flagChangeNotify;
				params->flagQuietCreate = flagQuietCreate;
				params->flagLexerSpecified = flagLexerSpecified;
				if (flagLexerSpecified && lpSchemeArg) {
					lstrcpy(StrEnd(&params->wchData) + 1, lpSchemeArg);
					params->iInitialLexer = 0;
				} else {
					params->iInitialLexer = iInitialLexer;
				}
				params->flagJumpTo = flagJumpTo;
				params->iInitialLine = iInitialLine;
				params->iInitialColumn = iInitialColumn;

				params->iSrcEncoding = (lpEncodingArg) ? Encoding_Match(lpEncodingArg) : -1;
				params->flagSetEncoding = flagSetEncoding;
				params->flagSetEOLMode = flagSetEOLMode;

				if (cchTitleExcerpt) {
					lstrcpy(StrEnd(&params->wchData) + 1, szTitleExcerpt);
					params->flagTitleExcerpt = 1;
				} else {
					params->flagTitleExcerpt = 0;
				}

				COPYDATASTRUCT cds;
				cds.dwData = DATA_NOTEPAD2_PARAMS;
				cds.cbData = (DWORD)GlobalSize(params);
				cds.lpData = params;

				SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&cds);
				GlobalFree(params);
				NP2HeapFree(lpFileArg);
			}
			return TRUE;
		}

		// Ask...
		if (IDYES == MsgBoxAsk(MB_YESNO, IDS_ERR_PREVWINDISABLED)) {
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
		LPWSTR lpCmdLineNew = StrDup(GetCommandLine());
		const size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLineNew) + 1);
		LPWSTR lp1 = (LPWSTR)NP2HeapAlloc(cmdSize);
		LPWSTR lp2 = (LPWSTR)NP2HeapAlloc(cmdSize);

		StrTab2Space(lpCmdLineNew);
		lstrcpy(lpCmdLineNew + cchiFileList, L"");

		LPWSTR pwch = CharPrev(lpCmdLineNew, StrEnd(lpCmdLineNew));
		int i = 0;
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

			STARTUPINFO si;
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			PROCESS_INFORMATION pi;
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

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwndMain, &wndpl);

	HMONITOR hMonitor = MonitorFromRect(&wndpl.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	// offset new window position +10/+10
	int x = wndpl.rcNormalPosition.left + (newWind? 10 : 0);
	int y = wndpl.rcNormalPosition.top	+ (newWind? 10 : 0);
	const int cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
	const int cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

	// check if window fits monitor
	if ((x + cx) > mi.rcWork.right || (y + cy) > mi.rcWork.bottom) {
		x = mi.rcMonitor.left;
		y = mi.rcMonitor.top;
	}

	const BOOL imax = IsZoomed(hwndMain);
	wsprintf(tch, L" -pos %i,%i,%i,%i,%i", x, y, cx, cy, imax);
	lstrcat(szParameters, tch);

	if (!emptyWind && StrNotEmpty(lpszFile)) {
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
			const char *sep = strchr(enc, ',');
			ZeroMemory(tch, sizeof(tch));
			MultiByteToWideChar(CP_UTF8, 0, enc, (int)(sep - enc), tch, COUNTOF(tch));
			lstrcat(szParameters, L" -e \"");
			lstrcat(szParameters, tch);
			lstrcat(szParameters, L"\"");
		}
		}

		// scheme
		switch (pLexCurrent->rid) {
		case NP2LEX_TEXTFILE:
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
		const Sci_Position pos = SciCall_GetCurrentPos();
		if (pos > 0) {
			const Sci_Line line = SciCall_LineFromPosition(pos) + 1;
			const Sci_Position col = SciCall_GetColumn(pos) + 1;
#if defined(_WIN64)
			WCHAR tchLn[32];
			WCHAR tchCol[32];
			PosToStrW(line, tchLn);
			PosToStrW(col, tchCol);
			wsprintf(tch, L" -g %s,%s", tchLn, tchCol);
#else
			wsprintf(tch, L" -g %d,%d", (int)line, (int)col);
#endif
			lstrcat(szParameters, tch);
		}

		WCHAR szFileName[MAX_PATH + 4];
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
		LPWSTR lpArg1;
		LPWSTR lpArg2;
		BOOL exit = TRUE;

		if (flagRelaunchElevated == 2) {
			WCHAR tchFile[MAX_PATH];
			lstrcpy(tchFile, szCurFile);
			if (!FileSave(FALSE, TRUE, FALSE, FALSE)) {
				return FALSE;
			}

			exit = StrCaseEqual(tchFile, szCurFile);
			lpArg1 = (LPWSTR)NP2HeapAlloc(sizeof(WCHAR) * MAX_PATH);
			GetModuleFileName(NULL, lpArg1, MAX_PATH);
			lpArg2 = (LPWSTR)NP2HeapAlloc(sizeof(WCHAR) * 1024);
			GetRelaunchParameters(lpArg2, tchFile, !exit, FALSE);
			exit = !IsDocumentModified();
		} else {
			const LPCWSTR lpCmdLine = GetCommandLine();
			const size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLine) + 1);
			lpArg1 = (LPWSTR)NP2HeapAlloc(cmdSize);
			lpArg2 = (LPWSTR)NP2HeapAlloc(cmdSize);
			ExtractFirstArgument(lpCmdLine, lpArg1, lpArg2);
		}

		if (StrNotEmpty(lpArg1)) {
			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC | SEE_MASK_NOZONECHECKS;
			sei.hwnd = GetForegroundWindow();
			sei.lpVerb = L"runas";
			sei.lpFile = lpArg1;
			sei.lpParameters = lpArg2;
			sei.lpDirectory = g_wchWorkingDirectory;
			sei.nShow = SW_SHOWNORMAL;

			if (!ShellExecuteEx(&sei)) {
				exit = FALSE;
			}
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
	RECT rcOld;
	GetWindowRect(hwnd, &rcOld);

	HMONITOR hMonitor = MonitorFromRect(&rcOld, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	const int y = mi.rcWork.top + 16;
	const int cy = mi.rcWork.bottom - mi.rcWork.top - 32;
	const int cx = min_i(mi.rcWork.right - mi.rcWork.left - 32, cy);
	int x = mi.rcWork.right - cx - 16;

	WINDOWPLACEMENT wndpl;
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

	if (!hIcon) {
		hIcon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	}

	NOTIFYICONDATA nid;
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
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 0;
	nid.uFlags = NIF_TIP;

	WCHAR tchTitle[128];
	if (StrNotEmpty(szTitleExcerpt)) {
		WCHAR tchFormat[32];
		FormatString(tchTitle, tchFormat, IDS_TITLEEXCERPT, szTitleExcerpt);
	} else if (StrNotEmpty(szCurFile)) {
		SHFILEINFO shfi;
		SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
		PathCompactPathEx(tchTitle, shfi.szDisplayName, COUNTOF(tchTitle) - 4, 0);
	} else {
		GetString(IDS_UNTITLED, tchTitle, COUNTOF(tchTitle) - 4);
	}

	if (IsDocumentModified()) {
		lstrcpy(nid.szTip, L"* ");
	} else {
		lstrcpy(nid.szTip, L"");
	}
	lstrcat(nid.szTip, tchTitle);

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void ShowNotificationA(int notifyPos, LPCSTR lpszText) {
	SciCall_CallTipUseStyle(TAB_WIDTH_NOTIFICATION);
	SciCall_ShowNotification(notifyPos, lpszText);
}

void ShowNotificationW(int notifyPos, LPCWSTR lpszText) {
	const int cpEdit = SciCall_GetCodePage();
	const int wchLen = lstrlen(lpszText);
	const int cchLen = wchLen*kMaxMultiByteCount + 1;
	char *cchText = (char *)NP2HeapAlloc(cchLen);
	WideCharToMultiByte(cpEdit, 0, lpszText, -1, cchText, cchLen, NULL, NULL);
	ShowNotificationA(notifyPos, cchText);
	NP2HeapFree(cchText);
}

void ShowNotificationMessage(int notifyPos, UINT uidMessage, ...) {
	WCHAR wchFormat[1024] = L"";
	WCHAR wchMessage[2048] = L"";
	GetString(uidMessage, wchFormat, COUNTOF(wchFormat));

	va_list va;
	va_start(va, uidMessage);
	wvsprintf(wchMessage, wchFormat, va);
	va_end(va);

	ShowNotificationW(notifyPos, wchMessage);
}

//=============================================================================
//
// InstallFileWatching()
//
//
void InstallFileWatching(BOOL terminate) {
	terminate = terminate || !iFileWatchingMode || StrIsEmpty(szCurFile);
	// Terminate
	if (bRunningWatch) {
		if (hChangeHandle) {
			FindCloseChangeNotification(hChangeHandle);
			hChangeHandle = NULL;
		}
		if (terminate) {
			KillTimer(NULL, ID_WATCHTIMER);
		}
	}

	bRunningWatch = !terminate;
	dwChangeNotifyTime = 0;
	if (!terminate) {
		// Install
		SetTimer(NULL, ID_WATCHTIMER, dwFileCheckInterval, WatchTimerProc);

		WCHAR tchDirectory[MAX_PATH];
		lstrcpy(tchDirectory, szCurFile);
		PathRemoveFileSpec(tchDirectory);

		// Save data of current file
		if (!GetFileAttributesEx(szCurFile, GetFileExInfoStandard, &fdCurFile)) {
			ZeroMemory(&fdCurFile, sizeof(WIN32_FIND_DATA));
		}

		hChangeHandle = iFileWatchingMethod ? NULL : FindFirstChangeNotification(tchDirectory, FALSE,
						FILE_NOTIFY_CHANGE_FILE_NAME	| \
						FILE_NOTIFY_CHANGE_DIR_NAME		| \
						FILE_NOTIFY_CHANGE_ATTRIBUTES	| \
						FILE_NOTIFY_CHANGE_SIZE			| \
						FILE_NOTIFY_CHANGE_LAST_WRITE);
	}
}

static inline BOOL IsCurrentFileChangedOutsideApp(void) {
	// Check if the file has been changed
	WIN32_FIND_DATA fdUpdated;
	if (!GetFileAttributesEx(szCurFile, GetFileExInfoStandard, &fdUpdated)) {
		// The current file has been removed
		return TRUE;
	}

	const BOOL changed = (fdCurFile.nFileSizeLow != fdUpdated.nFileSizeLow)
			|| (fdCurFile.nFileSizeHigh != fdUpdated.nFileSizeHigh)
			// CompareFileTime(&fdCurFile.ftLastWriteTime, &fdUpdated.ftLastWriteTime) != 0
			|| (fdCurFile.ftLastWriteTime.dwLowDateTime != fdUpdated.ftLastWriteTime.dwLowDateTime)
			|| (fdCurFile.ftLastWriteTime.dwHighDateTime != fdUpdated.ftLastWriteTime.dwHighDateTime);
	return changed;
}

static void CheckCurrentFileChangedOutsideApp(void) {
	// Check if the changes affect the current file
	if (IsCurrentFileChangedOutsideApp()) {
		// Shutdown current watching and give control to main window
		if (hChangeHandle) {
			FindCloseChangeNotification(hChangeHandle);
			hChangeHandle = NULL;
		}
		if (iFileWatchingMode == 2) {
			bRunningWatch = TRUE;
			dwChangeNotifyTime = GetTickCount();
		} else {
			KillTimer(NULL, ID_WATCHTIMER);
			bRunningWatch = FALSE;
			dwChangeNotifyTime = 0;
			SendMessage(hwndMain, APPM_CHANGENOTIFY, 0, 0);
		}
	} else if (iFileWatchingMethod == 0) {
		FindNextChangeNotification(hChangeHandle);
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
		// polling, not very efficient but useful for watching continuously updated file
		else if (iFileWatchingMethod) {
			if (dwChangeNotifyTime == 0) {
				CheckCurrentFileChangedOutsideApp();
			}
		}
		// Check Change Notification Handle
		// TODO: notification not fired for continuously updated file
		else if (WAIT_OBJECT_0 == WaitForSingleObject(hChangeHandle, 0)) {
			CheckCurrentFileChangedOutsideApp();
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
		if (SciCall_CanPaste()) {
			const BOOL back = autoCompletionConfig.bIndentText;
			autoCompletionConfig.bIndentText = FALSE;
			SciCall_DocumentEnd();
			SciCall_BeginUndoAction();
			if (SciCall_GetLength() > 0) {
				SciCall_NewLine();
			}
			SciCall_Paste(FALSE);
			SciCall_NewLine();
			SciCall_EndUndoAction();
			EditEnsureSelectionVisible();
			autoCompletionConfig.bIndentText = back;
		}

		dwLastCopyTime = 0;
	}
}
