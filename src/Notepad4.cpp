/******************************************************************************
*
*
* Notepad4
*
* Notepad4.cpp
*   Main application window functionality
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

struct IUnknown;
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <cstdio>
#include <cinttypes>
#include "SciCall.h"
#include "VectorISA.h"
#include "config.h"
#include "Helpers.h"
#include "Notepad4.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
#include "resource.h"

#ifndef SM_CXPADDEDBORDER
#define SM_CXPADDEDBORDER	92
#endif

//! show code folding level and state on line number margin
#define NP2_DEBUG_CODE_FOLDING		0

/******************************************************************************
*
* Local and global Variables for Notepad4.cpp
*
*/
HWND	hwndStatus;
static HWND hwndToolbar;
static HWND hwndReBar;
static HMONITOR hCurrentMonitor = nullptr;
HWND	hwndEdit;
HWND	hwndMain;
static HMENU hmenuMain;
static HWND hwndNextCBChain = nullptr;
HWND	hDlgFindReplace = nullptr;
static bool bInitDone = false;
static HACCEL hAccMain;
static HACCEL hAccFindReplace;
static HICON hTrayIcon = nullptr;
static UINT uTrayIconDPI = 0;

// tab width for notification text
#define CallTipTabWidthNotification		8
#define CallTipDefaultMouseDwellTime	250

#define TOOLBAR_COMMAND_BASE	IDT_FILE_NEW
#define DefaultToolbarButtons	L"22 3 0 1 2 0 4 18 19 0 5 6 0 7 8 9 20 0 10 11 0 12 0 24 0 13 14 0 15 16 0 17"
static TBBUTTON tbbMainWnd[] = {
	{0, 	0, 					0, 				 TBSTYLE_SEP, {0}, 0, 0},
	{0, 	IDT_FILE_NEW, 		TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{1, 	IDT_FILE_OPEN, 		TBSTATE_ENABLED, BTNS_DROPDOWN, {0}, 0, 0},
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
	{23, 	IDT_VIEW_TOGGLEFOLDS, 	TBSTATE_ENABLED, BTNS_DROPDOWN, {0}, 0, 0},
	{24, 	IDT_FILE_LAUNCH, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{25, 	IDT_VIEW_ALWAYSONTOP, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
};

WCHAR	szIniFile[MAX_PATH] = L"";
static WCHAR szIniFile2[MAX_PATH] = L"";
static bool bSaveSettings;
bool	bSaveRecentFiles;
int iMaxRecentFiles;
static bool bSaveFindReplace;
static WCHAR tchLastSaveCopyDir[MAX_PATH] = L"";
WCHAR	tchOpenWithDir[MAX_PATH];
WCHAR	tchFavoritesDir[MAX_PATH];
static WCHAR tchDefaultDir[MAX_PATH];
static WCHAR tchToolbarButtons[MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE];
static LPWSTR tchToolbarBitmap = nullptr;
static TitlePathNameFormat iPathNameFormat;
bool	fWordWrapG;
int		iWordWrapMode;
int		iWordWrapIndent;
int		iWordWrapSymbols;
bool	bShowWordWrapSymbols;
bool	bWordWrapSelectSubLine;
static bool bShowUnicodeControlCharacter;
static bool bMatchBraces;
static bool bShowIndentGuides;
static bool bHighlightCurrentBlock;
bool	bHighlightCurrentSubLine;
LineHighlightMode iHighlightCurrentLine;
EditTabSettings tabSettings;
static bool bMarkLongLines;
int		iLongLinesLimitG;
int		iLongLineMode;
int		iWrapColumn = 0;
int		iZoomLevel = 100;
bool	bShowBookmarkMargin;
static bool bShowLineNumbers;
static int bMarkOccurrences;
int	iChangeHistoryMarker;
EditAutoCompletionConfig autoCompletionConfig;
int iSelectOption;
static int iLineSelectionMode;
bool bShowCodeFolding;
extern CallTipInfo callTipInfo;
static bool bViewWhiteSpace;
static bool bViewEOLs;

// DBCS code page
int		iDefaultCodePage;
int		iDefaultCharSet;
int		iDefaultEncoding;
int		iCurrentEncoding;
static int iOriginalEncoding;
int		iSrcEncoding = CPI_NONE;
int		iWeakSrcEncoding = CPI_NONE;
bool	bSkipUnicodeDetection;
bool	bLoadANSIasUTF8;
bool	bLoadASCIIasUTF8;
bool	bLoadNFOasOEM;
bool	bNoEncodingTags;
extern int g_DOSEncoding;

#if defined(_WIN64)
bool	bLargeFileMode = false;
#endif
int		iDefaultEOLMode;
static int iCurrentEOLMode;
bool	bWarnLineEndings;
bool	bFixLineEndings;
bool	bAutoStripBlanks;
PrintHeaderOption iPrintHeader;
PrintFooterOption iPrintFooter;
int		iPrintColor;
int		iPrintZoom = 100;
RECT	pageSetupMargin;
static bool bSaveBeforeRunningTools;
bool bOpenFolderWithMatepath;
FileWatchingMode iFileWatchingMode;
int iFileWatchingOption;
bool	bResetFileWatching;
static DWORD dwFileCheckInterval;
static DWORD dwAutoReloadTimeout;
bool bUseXPFileDialog;
static EscFunction iEscFunction;
static bool bAlwaysOnTop;
static bool bMinimizeToTray;
static bool bTransparentMode;
static int	iEndAtLastLine;
int iFindReplaceOption;
static bool bEditLayoutRTL;
bool	bWindowLayoutRTL;
static int iRenderingTechnology;
static bool bUseInlineIME;
static int iBidirectional;
static bool bShowMenu;
static bool bShowToolbar;
static int iAutoScaleToolbar;
static bool bShowStatusbar;
static bool bInFullScreenMode;
static int iFullScreenMode;

struct WININFO {
	int x;
	int y;
	int cx;
	int cy;
	BOOL max;
};

static WININFO wi;

static int cyReBar;
static int cyReBarFrame;

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
static LPWSTR lpFileArg = nullptr;
static LPWSTR lpSchemeArg = nullptr;
static LPWSTR lpMatchArg = nullptr;
static LPWSTR lpEncodingArg = nullptr;
MRUList mruFile;
MRUList mruFind;
MRUList mruReplace;
static BitmapCache bitmapCache;

DWORD	dwLastIOError;
WCHAR	szCurFile[MAX_PATH + 40];
EditFileVars fvCurFile;
static bool bDocumentModified = false;
static bool bReadOnlyFile = false;
bool bReadOnlyMode = false; // save call to SciCall_GetReadOnly()

// AutoSave
int iAutoSaveOption;
DWORD dwAutoSavePeriod;
static DWORD dwCurrentDocReversion = 0;
static DWORD dwLastSavedDocReversion = 0;
static bool bAutoSaveTimerSet = false;
#define MaxAutoSaveCount	6	// normal
#define AllAutoSaveCount	(MaxAutoSaveCount + 2) // suspend, shutdown
static LPWSTR autoSavePathList[AllAutoSaveCount];
static int autoSaveCount = 0;
static WCHAR szAutoSaveFolder[MAX_PATH];

static Sci_Line iInitialLine;
static Sci_Position iInitialColumn;
static int iInitialLexer;

static bool bLastCopyFromMe = false;
static DWORD dwLastCopyTime;

bool bFreezeAppTitle = false;
static WCHAR szTitleExcerpt[128] = L"";
static bool fKeepTitleExcerpt = false;

static HANDLE hChangeHandle = nullptr;
static bool bRunningWatch = false;
static DWORD dwChangeNotifyTime = 0;

static UINT msgTaskbarCreated = 0;

static struct WatchFileInformation {
	FILETIME	ftLastWriteTime;
	DWORD		nFileSizeHigh;
	DWORD		nFileSizeLow;
} fdCurFile;

static EDITFINDREPLACE efrData;
bool	bReplaceInitialized = false;
EditMarkAll editMarkAll;
HANDLE idleTaskTimer;

static EditSortFlag iSortOptions = EditSortFlag_Ascending;
static EditAlignMode iAlignMode	= EditAlignMode_Left;
extern int iFontQuality;
extern CaretStyle iCaretStyle;
extern bool bBlockCaretForOVRMode;
extern bool bBlockCaretOutSelection;
extern int iCaretBlinkPeriod;

bool fIsElevated = false;
static WCHAR wchWndClass[16] = WC_NOTEPAD4;

// rarely changed statusbar items
struct CachedStatusItem {
	UINT updateMask;
	BOOL overType;

	Sci_Line iLine;
	Sci_Position iLineChar;
	Sci_Position iLineColumn;

	LPCWSTR pszLexerName;
	LPCWSTR pszEncoding;
	LPCWSTR pszEolMode;
	LPCWSTR pszOvrMode;
	WCHAR tchZoom[8];

	WCHAR tchItemFormat[128]; // IDS_STATUSITEM_FORMAT
	WCHAR tchLexerName[MAX_EDITLEXER_NAME_SIZE];
};
static CachedStatusItem cachedStatusItem;

#define UpdateStatusBarCacheLineColumn()	cachedStatusItem.updateMask |= (((1 << StatusItem_Find) - 1) | (1 << StatusItem_DocSize))
#define DisableDelayedStatusBarRedraw()		cachedStatusItem.updateMask |= (1 << StatusItem_ItemCount)

HINSTANCE	g_hInstance;
#if NP2_ENABLE_APP_LOCALIZATION_DLL
HINSTANCE	g_exeInstance;
#endif
HANDLE		g_hDefaultHeap;
HANDLE		g_hScintilla;
#if _WIN32_WINNT < _WIN32_WINNT_WIN8
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
enum {
	DefaultPositionFlag_None = 0,
	DefaultPositionFlag_SystemDefault = 1,
	DefaultPositionFlag_DefaultLeft = 2,
	DefaultPositionFlag_DefaultRight = 3,
	DefaultPositionFlag_Custom = 4,
	DefaultPositionFlag_AlignLeft = 4,
	DefaultPositionFlag_AlignRight = 8,
	DefaultPositionFlag_AlignTop = 16,
	DefaultPositionFlag_AlignBottom = 32,
	DefaultPositionFlag_FullArea = 64,
	DefaultPositionFlag_Margin = 128,
};

enum NotepadReplacementAction {
	NotepadReplacementAction_None,
	NotepadReplacementAction_Default,
	NotepadReplacementAction_PrintDialog,
	NotepadReplacementAction_PrintDefault,
};

enum RelaunchElevatedFlag {
	RelaunchElevatedFlag_None = 0,
	RelaunchElevatedFlag_Startup,
	RelaunchElevatedFlag_Manual,
};

static NotepadReplacementAction notepadAction = NotepadReplacementAction_None;
static bool	flagNoReuseWindow		= false;
static bool	flagReuseWindow			= false;
static bool bSingleFileInstance		= true;
static bool bReuseWindow			= false;
static bool bStickyWindowPosition	= false;
static int flagReadOnlyMode			= ReadOnlyMode_None;
static TripleBoolean flagMultiFileArg = TripleBoolean_NotSet;
static bool	flagSingleFileInstance	= true;
static bool	flagStartAsTrayIcon		= false;
static TripleBoolean flagAlwaysOnTop= TripleBoolean_NotSet;
static bool	flagRelativeFileMRU		= false;
static bool	flagPortableMyDocs		= false;
bool		flagNoFadeHidden		= false;
static int	iOpacityLevel			= 75;
int			iFindReplaceOpacityLevel= 75;
bool		flagSimpleIndentGuides	= false;
bool 		fNoHTMLGuess			= false;
bool 		fNoCGIGuess				= false;
bool 		fNoAutoDetection		= false;
bool		fNoFileVariables		= false;
static bool	flagPosParam			= false;
static int	flagDefaultPos			= DefaultPositionFlag_None;
static bool	flagNewFromClipboard	= false;
static bool	flagPasteBoard			= false;
static int	flagSetEncoding			= 0;
static int	flagSetEOLMode			= 0;
static bool	flagJumpTo				= false;
static MatchTextFlag flagMatchText	= MatchTextFlag_None;
static TripleBoolean flagChangeNotify = TripleBoolean_NotSet;
static bool	flagLexerSpecified		= false;
static bool	flagQuietCreate			= false;
TripleBoolean flagUseSystemMRU		= TripleBoolean_NotSet;
static RelaunchElevatedFlag flagRelaunchElevated = RelaunchElevatedFlag_None;
static bool	flagDisplayHelp			= false;

static inline bool IsDocumentModified() noexcept {
	return bDocumentModified || iCurrentEncoding != iOriginalEncoding;
}

static inline bool IsTopMost() noexcept {
	return (bAlwaysOnTop || flagAlwaysOnTop == TripleBoolean_True) && flagAlwaysOnTop != TripleBoolean_False;
}

// temporary fix for https://github.com/zufuliu/notepad4/issues/77: force InvalidateStyleRedraw().
static inline void InvalidateStyleRedraw() noexcept {
	SciCall_SetViewEOL(bViewEOLs);
}

// temporary fix for https://github.com/zufuliu/notepad4/issues/134: Direct2D on arm32
static inline int GetDefualtRenderingTechnology() noexcept {
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
static void CleanUpResources(bool initialized) noexcept {
	if (tchToolbarBitmap != nullptr) {
		LocalFree(tchToolbarBitmap);
	}
	if (lpSchemeArg) {
		LocalFree(lpSchemeArg);
	}

	Encoding_ReleaseResources();
	Style_ReleaseResources();
	Edit_ReleaseResources();
	Scintilla_ReleaseResources();

	if (hTrayIcon) {
		DestroyIcon(hTrayIcon);
	}
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

static void DispatchMessageMain(MSG *msg) noexcept {
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

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) noexcept {
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
	SetEnvironmentVariable(L"UBSAN_OPTIONS", L"log_path=" WC_NOTEPAD4 L"-UBSan.log");
#endif

	// Set global variable g_hInstance
	g_hInstance = hInstance;
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	g_exeInstance = hInstance;
#endif
#if _WIN32_WINNT < _WIN32_WINNT_WIN8
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
		HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
	}
#endif

	// Don't keep working directory locked
	WCHAR wchWorkingDirectory[MAX_PATH];
	GetCurrentDirectory(COUNTOF(g_wchWorkingDirectory), g_wchWorkingDirectory);
	GetModuleFileName(nullptr, wchWorkingDirectory, COUNTOF(wchWorkingDirectory));
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
		hResDLL = LoadLocalizedResourceDLL(uiLanguage, WC_NOTEPAD4 L".dll");
		if (hResDLL) {
			g_hInstance = hResDLL;
		}
#endif
		DisplayCmdLineHelp(nullptr);
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
		NP2HeapFree(lpFileArg);
		return 0;
	}

	// Init OLE and Common Controls
	OleInitialize(nullptr);

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC	= ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	msgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

#if _WIN32_WINNT < _WIN32_WINNT_WIN8
	// see LoadD2D() in PlatWin.cxx
	kSystemLibraryLoadFlags = (DLLFunctionEx<FARPROC>(L"kernel32.dll", "SetDefaultDllDirectories") != nullptr) ? LOAD_LIBRARY_SEARCH_SYSTEM32 : 0;
#endif

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	hResDLL = LoadLocalizedResourceDLL(uiLanguage, WC_NOTEPAD4 L".dll");
	if (hResDLL) {
		g_hInstance = hInstance = hResDLL;
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
		CleanUpResources(false);
		return FALSE;
	}

	// create the timer first, to make flagMatchText working.
	HANDLE timer = idleTaskTimer = WaitableTimer_Create();
	QueryPerformanceFrequency(&editMarkAll.watch.freq);
	InitInstance(hInstance, nShowCmd);
	hAccMain = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	hAccFindReplace = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCFINDREPLACE));
	MSG msg;

	while (true) {
		if (editMarkAll.pending) {
			WaitableTimer_Set(timer, WaitableTimer_IdleTaskDelayTime);
			while (editMarkAll.pending && WaitableTimer_Continue(timer)) {
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					DispatchMessageMain(&msg);
				}
			}
			if (editMarkAll.pending) {
				editMarkAll.Continue(timer);
			}
		}
		if (GetMessage(&msg, nullptr, 0, 0)) {
			DispatchMessageMain(&msg);
		} else {
			break;
		}
	}

	WaitableTimer_Destroy(timer);
	CleanUpResources(true);
	return static_cast<int>(msg.wParam);
}

//=============================================================================
//
// InitApplication()
//
//
BOOL InitApplication(HINSTANCE hInstance) noexcept {
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style		 = CS_BYTEALIGNWINDOW | CS_DBLCLKS;
	wc.lpfnWndProc	 = MainWndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	wc.hCursor		 = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = AsPointer<HBRUSH, ULONG_PTR>(COLOR_3DFACE + 1);
	wc.lpszMenuName	 = MAKEINTRESOURCE(IDR_MAINWND);
	wc.lpszClassName = wchWndClass;
	wc.hIconSm       = nullptr;

	return RegisterClassEx(&wc);
}

static void HandleMatchText(MatchTextFlag flag, LPCWSTR lpszText, bool jumpTo) noexcept {
	if (StrNotEmpty(lpszText) && SciCall_GetLength()) {
		const UINT cpEdit = SciCall_GetCodePage();
		WideCharToMultiByte(cpEdit, 0, lpszText, -1, efrData.szFind, COUNTOF(efrData.szFind), nullptr, nullptr);
		WideCharToMultiByte(CP_UTF8, 0, lpszText, -1, efrData.szFindUTF8, COUNTOF(efrData.szFindUTF8), nullptr, nullptr);

		if (flag & MatchTextFlag_Regex) {
			efrData.fuFlags |= (iFindReplaceOption & FindReplaceOption_UseCxxRegex) ? (SCFIND_REGEXP | SCFIND_CXX11REGEX) : (SCFIND_REGEXP | SCFIND_POSIX);
		} else {
			efrData.fuFlags &= SCFIND_REGEXP - 1; // clear all regex flags
			if (flag & MatchTextFlag_TransformBS) {
				efrData.option |= FindReplaceOption_TransformBackslash;
			}
		}

		if (flag & MatchTextFlag_FindUp) {
			if (!jumpTo) {
				SciCall_DocumentEnd();
			}
			EditFindPrev(&efrData, false);
		} else {
			if (!jumpTo) {
				SciCall_DocumentStart();
			}
			EditFindNext(&efrData, false);
		}
		EditEnsureSelectionVisible();
	}
}

//=============================================================================
//
// InitInstance()
//
//
void InitInstance(HINSTANCE hInstance, int nCmdShow) {
#if 0
	StopWatch watch;
	watch.Start();
#endif
	const bool defaultPos = (wi.x == CW_USEDEFAULT || wi.y == CW_USEDEFAULT || wi.cx == CW_USEDEFAULT || wi.cy == CW_USEDEFAULT);
	RECT rc = { wi.x, wi.y, (defaultPos ? CW_USEDEFAULT : (wi.x + wi.cx)), (defaultPos ? CW_USEDEFAULT : (wi.y + wi.cy)) };

	if (flagDefaultPos == DefaultPositionFlag_SystemDefault) {
		wi.x = wi.y = wi.cx = wi.cy = CW_USEDEFAULT;
		wi.max = 0;
	} else if (flagDefaultPos >= DefaultPositionFlag_Custom) {
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		const int width = rc.right - rc.left;
		const int height = rc.bottom - rc.top;
		if (flagDefaultPos & DefaultPositionFlag_AlignRight) {
			wi.x = width / 2;
		} else {
			wi.x = rc.left;
		}
		wi.cx = width;
		if (flagDefaultPos & (DefaultPositionFlag_AlignLeft | DefaultPositionFlag_AlignRight)) {
			wi.cx = width / 2;
		}
		if (flagDefaultPos & DefaultPositionFlag_AlignBottom) {
			wi.y = height / 2;
		} else {
			wi.y = rc.top;
		}
		wi.cy = height;
		if (flagDefaultPos & (DefaultPositionFlag_AlignTop | DefaultPositionFlag_AlignBottom)) {
			wi.cy = height / 2;
		}
		if (flagDefaultPos & DefaultPositionFlag_FullArea) {
			wi.x = rc.left;
			wi.y = rc.top;
			wi.cx = width;
			wi.cy = height;
		}
		if (flagDefaultPos & DefaultPositionFlag_Margin) {
			wi.x += (flagDefaultPos & DefaultPositionFlag_AlignRight) ? 4 : 8;
			wi.cx -= (flagDefaultPos & (DefaultPositionFlag_AlignLeft | DefaultPositionFlag_AlignRight)) ? 12 : 16;
			wi.y += (flagDefaultPos & DefaultPositionFlag_AlignBottom) ? 4 : 8;
			wi.cy -= (flagDefaultPos & (DefaultPositionFlag_AlignTop | DefaultPositionFlag_AlignBottom)) ? 12 : 16;
		}
	} else if (flagDefaultPos == DefaultPositionFlag_DefaultLeft || flagDefaultPos == DefaultPositionFlag_DefaultRight || defaultPos) {
		// default window position
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		wi.y = rc.top + 16;
		wi.cy = rc.bottom - rc.top - 32;
		wi.cx = min<int>(rc.right - rc.left - 32, wi.cy);
		wi.x = (flagDefaultPos == DefaultPositionFlag_DefaultLeft) ? rc.left + 16 : rc.right - wi.cx - 16;
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
			wi.cx = min<int>(mi.rcWork.right - mi.rcWork.left - 32, wi.cy);
			wi.x = mi.rcWork.right - wi.cx - 16;
		}
	}

	HWND hwnd = CreateWindowEx(
				   0,
				   wchWndClass,
				   WC_NOTEPAD4,
				   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
				   wi.x,
				   wi.y,
				   wi.cx,
				   wi.cy,
				   nullptr,
				   nullptr,
				   hInstance,
				   nullptr);
	if (IsTopMost()) {
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	if (bTransparentMode) {
		SetWindowTransparentMode(hwnd, true, iOpacityLevel);
	}
	if (!bShowMenu) {
		SetMenu(hwnd, nullptr);
	}
	if (!flagStartAsTrayIcon) {
		ShowWindow(hwnd, wi.max ? SW_SHOWMAXIMIZED : nCmdShow);
		UpdateWindow(hwnd);
	} else {
		ShowWindow(hwnd, SW_HIDE); // trick ShowWindow()
		ShowNotifyIcon(hwnd, true);
	}

	// Source Encoding
	if (lpEncodingArg) {
		iSrcEncoding = Encoding_Match(lpEncodingArg);
	}

	UpdateStatusBarCache(StatusItem_OvrMode);
	UpdateStatusBarCache(StatusItem_Zoom);
	bool bOpened = false;
	bool bFileLoadCalled = false;
	// Pathname parameter
	if (lpFileArg /*&& !flagNewFromClipboard*/) {

		// Open from Directory
		if (PathIsDirectory(lpFileArg)) {
			WCHAR tchFile[MAX_PATH];
			if (OpenFileDlg(tchFile, COUNTOF(tchFile), lpFileArg)) {
				bOpened = FileLoad(FileLoadFlag_Default, tchFile);
				bFileLoadCalled = true;
			}
		} else {
			bOpened = FileLoad(FileLoadFlag_Default, lpFileArg);
			bFileLoadCalled = bOpened;
		}
		NP2HeapFree(lpFileArg);

		if (bOpened) {
			if (flagJumpTo) { // Jump to position
				EditJumpTo(iInitialLine, iInitialColumn);
			}
			if (flagChangeNotify != TripleBoolean_NotSet) {
				iFileWatchingMode = (flagChangeNotify == TripleBoolean_False) ? FileWatchingMode_None : FileWatchingMode_AutoReload;
				bResetFileWatching = true;
				InstallFileWatching(false);
			}
		}
	} else {
		if (iSrcEncoding >= CPI_FIRST) {
			iCurrentEncoding = iSrcEncoding;
			iOriginalEncoding = iSrcEncoding;
			SciCall_SetCodePage((iSrcEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
		}
	}

	if (!bFileLoadCalled) {
		bOpened = FileLoad(static_cast<FileLoadFlag>(FileLoadFlag_DontSave | FileLoadFlag_New), L"");
	}
	if (!bOpened) {
		UpdateStatusBarCache(StatusItem_Encoding);
		UpdateStatusBarCache(StatusItem_EolMode);
		UpdateStatusBarCacheLineColumn();
	}

	// reset
	iSrcEncoding = CPI_NONE;
	flagQuietCreate = false;
	fKeepTitleExcerpt = false;

	// Check for /c [if no file is specified] -- even if a file is specified
	if (flagNewFromClipboard) {
		if (SciCall_CanPaste()) {
			const bool back = autoCompletionConfig.bIndentText;
			autoCompletionConfig.bIndentText = false;
			SciCall_DocumentEnd();
			SciCall_BeginUndoAction();
			if (SciCall_GetLength() > 0) {
				SciCall_NewLine();
			}
			SciCall_Paste(false);
			SciCall_NewLine();
			SciCall_EndUndoAction();
			autoCompletionConfig.bIndentText = back;
			if (flagJumpTo) {
				EditJumpTo(iInitialLine, iInitialColumn);
			} else {
				EditEnsureSelectionVisible();
			}
		}
	}

	// Encoding
	if (0 != flagSetEncoding) {
		SendWMCommand(hwnd, IDM_ENCODING_ANSI - 1 + flagSetEncoding);
		flagSetEncoding = 0;
	}

	// EOL mode
	if (0 != flagSetEOLMode) {
		SendWMCommand(hwnd, IDM_LINEENDINGS_CRLF - 1 + flagSetEOLMode);
		flagSetEOLMode = 0;
	}

	// Match Text
	if (lpMatchArg) {
		HandleMatchText(flagMatchText, lpMatchArg, flagJumpTo);
		LocalFree(lpMatchArg);
	}

	// Check for Paste Board option -- after loading files
	if (flagPasteBoard) {
		bLastCopyFromMe = true;
		hwndNextCBChain = SetClipboardViewer(hwnd);
		UpdateWindowTitle();
		bLastCopyFromMe = false;
		dwLastCopyTime = 0;
		SetTimer(hwnd, ID_PASTEBOARDTIMER, 100, PasteBoardTimer);
	}

	// check if a lexer was specified from the command line
	if (flagLexerSpecified) {
		if (lpSchemeArg) {
			Style_SetLexerFromName(szCurFile, lpSchemeArg);
			LocalFree(lpSchemeArg);
			lpSchemeArg = nullptr;
		} else {
			Style_SetLexerFromID(iInitialLexer);
		}
		flagLexerSpecified = false;
	}

	// If start as tray icon, set current filename as tooltip
	if (flagStartAsTrayIcon) {
		SetNotifyIconTitle(hwnd);
	} else if (bInFullScreenMode) {
		ToggleFullScreenMode();
	}

	bInitDone = true;
	if (SciCall_GetLength() == 0) {
		UpdateToolbar();
		UpdateStatusbar();
	}
	if (notepadAction > NotepadReplacementAction_Default) {
		PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDM_FILE_PRINT, 1), notepadAction);
	}

#if 0
	watch.Stop();
	watch.ShowLog("InitInstance() time");
#endif
}

static inline void NP2MinimizeWind(HWND hwnd) noexcept {
	MinimizeWndToTray(hwnd);
	ShowNotifyIcon(hwnd, true);
	SetNotifyIconTitle(hwnd);
}

static inline void NP2RestoreWind(HWND hwnd) noexcept {
	ShowNotifyIcon(hwnd, false);
	RestoreWndFromTray(hwnd);
	if (flagJumpTo) {
		// scroll caret to view for `Notepad4.exe /i /g -1`
		EditEnsureSelectionVisible();
	}
	ShowOwnedPopups(hwnd, TRUE);
}

static inline void ExitApplication(HWND hwnd) noexcept {
	if (FileSave(FileSaveFlag_Ask)) {
		DestroyWindow(hwnd);
	}
}

void MsgDropFiles(HWND hwnd, UINT umsg, WPARAM wParam) {
	UNREFERENCED_PARAMETER(umsg);
	HDROP hDrop = AsPointer<HDROP>(wParam);
	// fix drag & drop file from 32-bit app to 64-bit Notepad4 prior Win 10
#if defined(_WIN64) && (_WIN32_WINNT < _WIN32_WINNT_WIN10)
	if (umsg == WM_DROPFILES && !bReadOnlyMode) {
		POINT pt;
		if (DragQueryPoint(hDrop, &pt)) { // client area
			RECT rc;
			ClientToScreen(hwnd, &pt);
			GetWindowRect(hwndEdit, &rc);
			if (PtInRect(&rc, pt)) { // already processed APPM_DROPFILES
				DragFinish(hDrop);
				return;
			}
		}
	}
#endif
	//if (DragQueryFile(hDrop, UINT_MAX, nullptr, 0) > 1) {
	//	MsgBoxWarn(MB_OK, IDS_ERR_DROP);
	//}
	WCHAR szBuf[MAX_PATH + 40];
	if (DragQueryFile(hDrop, 0, szBuf, COUNTOF(szBuf))) {
		LPCWSTR path = szBuf;
		// Visual Studio: {UUID}|Solution\Project.[xx]proj|path
		LPCWSTR t = StrRChrW(path, nullptr, L'|');
		if (t) {
			path = t + 1;
		}
		// Reset Change Notify
		//bPendingChangeNotify = false;
		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}
		//SetForegroundWindow(hwnd);
		if (PathIsDirectory(path)) {
			WCHAR tchFile[MAX_PATH];
			if (OpenFileDlg(tchFile, COUNTOF(tchFile), path)) {
				FileLoad(FileLoadFlag_Default, tchFile);
			}
		} else {
			FileLoad(FileLoadFlag_Default, path);
		}
	}
	DragFinish(hDrop);
}

#if NP2_ENABLE_DOT_LOG_FEATURE
static inline bool IsFileStartsWithDotLog() noexcept {
	char tch[5] = "";
	const Sci_Position len = SciCall_GetText(COUNTOF(tch) - 1, tch);
	// upper case
	return len == 4 && StrEqualEx(tch, ".LOG");
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
	static bool bShutdownOK;

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
		return DefWindowProc(hwnd, umsg, wParam, lParam);

	case WM_WINDOWPOSCHANGED: {
		HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		if (monitor != hCurrentMonitor) {
			hCurrentMonitor = monitor;
			// Direct2D: render Scintilla window with parameters for current monitor
			SendMessage(hwndEdit, WM_SETTINGCHANGE, 0, 0);
			Style_SetLexer(pLexCurrent, false); // override base elements
		}
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}

	case WM_CREATE:
		return MsgCreate(hwnd, wParam, lParam);

	case WM_DESTROY:
	case WM_ENDSESSION:
		if (!bShutdownOK) {
			editMarkAll.Stop();
			AutoSave_Stop(TRUE);
			// Terminate file watching
			InstallFileWatching(true);
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
			SaveSettings(false);

			mruFile.MergeSave(bSaveRecentFiles);
			mruFind.MergeSave(bSaveFindReplace);
			mruReplace.MergeSave(bSaveFindReplace);
			bitmapCache.Empty();

			// Remove tray icon if necessary
			ShowNotifyIcon(hwnd, false);

			bShutdownOK = true;
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
		// we only have 5 seconds to save current file
		if (iAutoSaveOption & AutoSaveOption_Shutdown) {
			AutoSave_DoWork(FileSaveFlag_SaveCopy);
		}
		if (FileSave(static_cast<FileSaveFlag>(FileSaveFlag_Ask | FileSaveFlag_EndSession))) {
			return TRUE;
		}
		return FALSE;

	case WM_POWERBROADCAST:
		if (wParam == PBT_APMSUSPEND) {
			// we only have 2 seconds to save current file
			if (iAutoSaveOption & AutoSaveOption_Suspend) {
				AutoSave_DoWork(FileSaveFlag_SaveCopy);
			}
		}
		break;

	case WM_THEMECHANGED:
		// Reinitialize theme-dependent values and resize windows
		MsgThemeChanged(hwnd, wParam, lParam);
		break;

	case WM_DPICHANGED:
		MsgDPIChanged(hwnd, wParam, lParam);
		break;

	case WM_SETTINGCHANGE:
		bitmapCache.Invalidate();
		// TODO: detect system theme and high contrast mode changes
		SendMessage(hwndEdit, WM_SETTINGCHANGE, wParam, lParam);
		Style_SetLexer(pLexCurrent, false); // override base elements
		break;

	case WM_SYSCOLORCHANGE:
		bitmapCache.Invalidate();
		SendMessage(hwndToolbar, WM_SYSCOLORCHANGE, wParam, lParam);
		SendMessage(hwndEdit, WM_SYSCOLORCHANGE, wParam, lParam);
		Style_SetLexer(pLexCurrent, false); // override base elements
		break;

	case WM_TIMER:
		if (wParam == ID_AUTOSAVETIMER) {
			AutoSave_DoWork(FileSaveFlag_Default);
		}
		break;

	case WM_SIZE:
		MsgSize(hwnd, wParam, lParam);
		break;

	case WM_GETMINMAXINFO:
		if (bInFullScreenMode) {
			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hCurrentMonitor, &mi);

			const int w = mi.rcMonitor.right - mi.rcMonitor.left;
			const int h = mi.rcMonitor.bottom - mi.rcMonitor.top;
			const UINT dpi = g_uCurrentDPI;

			LPMINMAXINFO pmmi = AsPointer<LPMINMAXINFO>(lParam);
			const int padding = 2*SystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
			pmmi->ptMaxSize.x = w + padding + 2*SystemMetricsForDpi(SM_CXSIZEFRAME, dpi);
			pmmi->ptMaxSize.y = h + padding + SystemMetricsForDpi(SM_CYCAPTION, dpi) + 2*SystemMetricsForDpi(SM_CYSIZEFRAME, dpi);
			pmmi->ptMaxTrackSize = pmmi->ptMaxSize;
			return 0;
		}
		return DefWindowProc(hwnd, umsg, wParam, lParam);

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		//if (bPendingChangeNotify)
		//	PostMessage(hwnd, APPM_CHANGENOTIFY, 0, 0);
		break;

	case WM_DROPFILES:
	case APPM_DROPFILES:
		MsgDropFiles(hwnd, umsg, wParam);
		break;

	case WM_COPYDATA: {
		PCOPYDATASTRUCT pcds = AsPointer<PCOPYDATASTRUCT>(lParam);

		// Reset Change Notify
		//bPendingChangeNotify = false;

		if (pcds->dwData == DATA_NOTEPAD4_PARAMS) {
			const NP2PARAMS * const params = static_cast<NP2PARAMS *>(pcds->lpData);

			if (params->flagReadOnlyMode) {
				flagReadOnlyMode |= ReadOnlyMode_Current;
			}
			if (params->flagLexerSpecified) {
				flagLexerSpecified = true;
			}
			if (params->flagQuietCreate) {
				flagQuietCreate = true;
			}

			bool bOpened = true;
			LPCWSTR lpsz = &params->wchData;
			if (params->flagFileSpecified) {
				iSrcEncoding = params->iSrcEncoding;

				if (PathIsDirectory(lpsz)) {
					WCHAR tchFile[MAX_PATH];
					bOpened = false;
					if (OpenFileDlg(tchFile, COUNTOF(tchFile), lpsz)) {
						bOpened = FileLoad(FileLoadFlag_Default, tchFile);
					}
				} else {
					bOpened = FileLoad(FileLoadFlag_Default, lpsz);
				}

				if (bOpened) {
					if (params->flagChangeNotify != TripleBoolean_NotSet) {
						iFileWatchingMode = (params->flagChangeNotify == TripleBoolean_False) ? FileWatchingMode_None : FileWatchingMode_AutoReload;
						bResetFileWatching = true;
						InstallFileWatching(false);
					}

					if (0 != params->flagSetEncoding) {
						flagSetEncoding = params->flagSetEncoding;
						SendWMCommand(hwnd, IDM_ENCODING_ANSI - 1 + flagSetEncoding);
						flagSetEncoding = 0;
					}

					if (0 != params->flagSetEOLMode) {
						flagSetEOLMode = params->flagSetEOLMode;
						SendWMCommand(hwnd, IDM_LINEENDINGS_CRLF - 1 + flagSetEOLMode);
						flagSetEOLMode = 0;
					}

					if (params->flagLexerSpecified) {
						if (params->iInitialLexer <= 0) {
							WCHAR wchExt[32] = L".";
							lpsz = StrEnd(lpsz) + 1;
							lstrcpyn(wchExt + 1, lpsz, COUNTOF(wchExt) - 1);
							Style_SetLexerFromName(&params->wchData, wchExt);
						} else {
							Style_SetLexerFromID(params->iInitialLexer);
						}
					}

					if (params->flagTitleExcerpt) {
						lpsz = StrEnd(lpsz) + 1;
						lstrcpyn(szTitleExcerpt, lpsz, COUNTOF(szTitleExcerpt));
						UpdateWindowTitle();
					}
					if (params->flagMatchText != MatchTextFlag_None) {
						lpsz = StrEnd(lpsz) + 1;
					}
				}
				// reset
				iSrcEncoding = CPI_NONE;
			}

			if (params->flagJumpTo) {
				const Sci_Line iLine = params->iInitialLine ? params->iInitialLine : 1;
				EditJumpTo(iLine, params->iInitialColumn);
			}
			if (bOpened && params->flagMatchText != MatchTextFlag_None) {
				HandleMatchText(params->flagMatchText, lpsz, params->flagJumpTo);
			}

			flagLexerSpecified = false;
			flagQuietCreate = false;
			flagReadOnlyMode &= ReadOnlyMode_AllFile;
		}
	}
	return TRUE;

	case WM_CONTEXTMENU: {
		const int nID = GetDlgCtrlID(AsPointer<HWND>(wParam));

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
						 TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x + 1, pt.y + 1, hwnd, nullptr);

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
			if (flagJumpTo) {
				// scroll caret to view for `start /min Notepad4.exe /g -1`
				EditEnsureSelectionVisible();
			}
			ShowOwnedPopups(hwnd, TRUE);
			return lrv;
		}
		}
		return DefWindowProc(hwnd, umsg, wParam, lParam);

	case APPM_CHANGENOTIFY:
		if (iFileWatchingMode == FileWatchingMode_ShowMessage || IsDocumentModified()) {
			SetForegroundWindow(hwnd);
		}

		if (PathIsFile(szCurFile)) {
			if ((iFileWatchingMode == FileWatchingMode_AutoReload && !IsDocumentModified())
				|| MsgBoxWarn(MB_YESNO, IDS_FILECHANGENOTIFY) == IDYES) {
				const bool bIsTail = (iFileWatchingMode == FileWatchingMode_AutoReload)
					&& ((iFileWatchingOption & FileWatchingOption_KeepAtEnd) || (SciCall_LineFromPosition(SciCall_GetCurrentPos()) + 1 == SciCall_GetLineCount()));

				iWeakSrcEncoding = iCurrentEncoding;
				if (FileLoad(static_cast<FileLoadFlag>(FileLoadFlag_DontSave | FileLoadFlag_Reload), szCurFile)) {
					if (bIsTail) {
						EditJumpTo(INVALID_POSITION, 0);
					}
				}
			}
		} else {
			if (MsgBoxWarn(MB_YESNO, IDS_FILECHANGENOTIFY2) == IDYES) {
				FileSave(FileSaveFlag_SaveAlways);
			}
		}

		if (!bRunningWatch) {
			InstallFileWatching(false);
		}
		break;

	//// This message is posted before Notepad4 reactivates itself
	//case APPM_CHANGENOTIFYCLEAR:
	//	bPendingChangeNotify = false;
	//	break;

	case WM_DRAWCLIPBOARD:
		if (!bLastCopyFromMe) {
			dwLastCopyTime = GetTickCount();
		} else {
			bLastCopyFromMe = false;
		}
		if (hwndNextCBChain) {
			SendMessage(hwndNextCBChain, WM_DRAWCLIPBOARD, wParam, lParam);
		}
		break;

	case WM_CHANGECBCHAIN:
		if (AsPointer<HWND>(wParam) == hwndNextCBChain) {
			hwndNextCBChain = AsPointer<HWND>(lParam);
		}
		if (hwndNextCBChain) {
			SendMessage(hwndNextCBChain, WM_CHANGECBCHAIN, lParam, wParam);
		}
		break;

#if 0
	case WM_KEYDOWN: {
		//printf("%s:%d WM_KEYDOWN %u\n", __func__, __LINE__, (UINT)wParam);
		MSG msg;
		memset(&msg, 0, sizeof(msg));
		msg.hwnd = hwnd;
		msg.message = WM_KEYDOWN;
		msg.wParam = wParam;
		msg.lParam = lParam;
		return TranslateAccelerator(hwnd, hAccMain, &msg);
	}
#endif

	case WM_CHAR: {
		// handle control characters generated by Ctrl + letters
		//printf("%s:%d WM_CHAR %u\n", __func__, __LINE__, (UINT)wParam);
		MSG msg;
		memset(&msg, 0, sizeof(msg));
		msg.hwnd = hwnd;
		msg.message = WM_KEYDOWN;
		msg.lParam = lParam;
		msg.wParam = MapVirtualKey((lParam >> 16) & 0xff, MAPVK_VSC_TO_VK);
		return TranslateAccelerator(hwnd, hAccMain, &msg);
	}

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
								  pt.x, pt.y, 0, hwnd, nullptr);

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
		HWND box = FindWindow(L"#32770", nullptr);
		HWND parent = GetParent(box);
		// MessageBox belongs to us.
		if (parent == AsPointer<HWND>(wParam) || parent == hwnd) {
			CenterDlgInParentEx(box, parent);
			SnapToDefaultButton(box);
		}
	}
	break;

	case APPM_POST_HOTSPOTCLICK: {
		// release mouse capture and restore selection
		const int x = SciCall_PointXFromPosition(lParam);
		const int y = SciCall_PointYFromPosition(lParam);
		SendMessage(hwndEdit, WM_LBUTTONUP, MAKELPARAM(x, y), MK_CONTROL);
		EditSelectEx(wParam, lParam);
		SciCall_SetMultipleSelection(true);
	} break;

	default:
		if (umsg == msgTaskbarCreated) {
			if (!IsWindowVisible(hwnd)) {
				ShowNotifyIcon(hwnd, true);
			}
			SetNotifyIconTitle(hwnd);
			return 0;
		}

		return DefWindowProc(hwnd, umsg, wParam, lParam);

	}
	return 0;
}

void UpdateWindowTitle() noexcept {
	static WCHAR szCachedFile[MAX_PATH] = L"";
	static WCHAR szCachedDisplayName[MAX_PATH] = L"";

	if (bFreezeAppTitle) {
		return;
	}

	WCHAR szAppName[128];
	static_assert(IDS_APPTITLE + 1 == IDS_APPTITLE_PASTEBOARD);
	GetString(IDS_APPTITLE + static_cast<UINT>(flagPasteBoard), szAppName, COUNTOF(szAppName));

	if (fIsElevated) {
		WCHAR szElevatedAppName[128];
		WCHAR fmt[64];
		FormatString(szElevatedAppName, fmt, IDS_APPTITLE_ELEVATED, szAppName);
		lstrcpyn(szAppName, szElevatedAppName, COUNTOF(szAppName));
	}

	WCHAR szTitle[512];
	if (IsDocumentModified()) {
		StrCpyEx(szTitle, L"* ");
	} else {
		szTitle[0] = L'\0';
	}

	if (StrNotEmpty(szTitleExcerpt)) {
		WCHAR szExcerptQuote[256];
		WCHAR szExcerptFmt[32];
		FormatString(szExcerptQuote, szExcerptFmt, IDS_TITLEEXCERPT, szTitleExcerpt);
		lstrcat(szTitle, szExcerptQuote);
	} else if (StrNotEmpty(szCurFile)) {
		if (iPathNameFormat != TitlePathNameFormat_FullPath && !PathIsRoot(szCurFile)) {
			if (!StrEqual(szCachedFile, szCurFile)) {
				SHFILEINFO shfi;
				lstrcpy(szCachedFile, szCurFile);
				if (SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME)) {
					lstrcpy(szCachedDisplayName, shfi.szDisplayName);
				} else {
					lstrcpy(szCachedDisplayName, PathFindFileName(szCurFile));
				}
			}
			lstrcat(szTitle, szCachedDisplayName);
			if (iPathNameFormat == TitlePathNameFormat_NameFirst) {
				WCHAR tchPath[MAX_PATH];
				lstrcpyn(tchPath, szCurFile, COUNTOF(tchPath));
				PathRemoveFileSpec(tchPath);
				lstrcat(szTitle, L" [");
				lstrcat(szTitle, tchPath);
				lstrcat(szTitle, L"]");
			}
		} else {
			lstrcat(szTitle, szCurFile);
		}
	} else {
		StrCpyEx(szCachedFile, L"");
		StrCpyEx(szCachedDisplayName, L"");
		WCHAR szUntitled[128];
		GetString(IDS_UNTITLED, szUntitled, COUNTOF(szUntitled));
		lstrcat(szTitle, szUntitled);
	}

	if (bReadOnlyFile) {
		WCHAR szReadOnly[32];
		GetString(IDS_READONLY_FILE, szReadOnly, COUNTOF(szReadOnly));
		lstrcat(szTitle, L" ");
		lstrcat(szTitle, szReadOnly);
	}
	else if (bReadOnlyMode) {
		WCHAR szReadOnly[32];
		GetString(IDS_READONLY_MODE, szReadOnly, COUNTOF(szReadOnly));
		lstrcat(szTitle, L" ");
		lstrcat(szTitle, szReadOnly);
	}

	lstrcat(szTitle, L" - ");
	lstrcat(szTitle, szAppName);

	SetWindowText(hwndMain, szTitle);
}

static inline void UpdateDocumentModificationStatus() noexcept {
	UpdateWindowTitle();
	UpdateToolbar();
}

void UpdateBookmarkMarginWidth() noexcept {
	// see LineMarker::Draw() for minDim.
	const int width = (bShowBookmarkMargin || iChangeHistoryMarker != SC_CHANGE_HISTORY_DISABLED) ? SciCall_TextHeight() - 2 : 0;
	// 16px for XPM bookmark symbol.
	//const int width = (bShowBookmarkMargin || iChangeHistoryMarker != SC_CHANGE_HISTORY_DISABLED) ? max(SciCall_TextHeight() - 2, 16) : 0;
	SciCall_SetMarginWidth(MarginNumber_Bookmark, width);
}

void SetWrapVisualFlags() noexcept {
	if (bShowWordWrapSymbols) {
		int wrapVisualFlags = 0;
		int wrapVisualFlagsLocation = 0;
		if (iWordWrapSymbols == 0) {
			iWordWrapSymbols = EditWrapSymbol_DefaultValue;
		}
		switch (iWordWrapSymbols % 10) {
		case EditWrapSymbolBefore_NearText:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_END;
			wrapVisualFlagsLocation |= SC_WRAPVISUALFLAGLOC_END_BY_TEXT;
			break;
		case EditWrapSymbolBefore_NearBorder:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_END;
			break;
		}
		switch (iWordWrapSymbols / 10) {
		case EditWrapSymbolAfter_NearText:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_START;
			wrapVisualFlagsLocation |= SC_WRAPVISUALFLAGLOC_START_BY_TEXT;
			break;
		case EditWrapSymbolAfter_NearBorder:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_START;
			break;
		case EditWrapSymbolAfter_LineNumberMargin:
			wrapVisualFlags |= SC_WRAPVISUALFLAG_MARGIN;
			if (!bShowLineNumbers) {
				bShowLineNumbers = true;
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

//=============================================================================
//
// EditCreate()
//
void EditCreate(HWND hwndParent) noexcept {
	const DWORD dwExStyle = IsAppThemed() ? 0 : WS_EX_CLIENTEDGE;
	HWND hwnd = CreateWindowEx(dwExStyle,
						  L"Scintilla",
						  nullptr,
						  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						  0, 0, 0, 0,
						  hwndParent,
						  AsPointer<HMENU, ULONG_PTR>(IDC_EDIT),
						  g_hInstance,
						  nullptr);
	hwndEdit = hwnd;
	efrData.hwnd = hwnd;
	InitScintillaHandle(hwnd);
	//SciInitThemes(hwnd);
	if (bEditLayoutRTL) {
		SetWindowLayoutRTL(hwnd, true);
	}

	Style_InitDefaultColor();
	SciCall_SetTechnology(iRenderingTechnology);
	SciCall_SetBidirectional(iBidirectional);
	SciCall_SetIMEInteraction(bUseInlineIME);
	SciCall_SetPasteConvertEndings(true);
	SciCall_SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
	SciCall_SetCommandEvents(false);
	SciCall_UsePopUp(SC_POPUP_NEVER);
	SciCall_SetScrollWidthTracking(true);
	SciCall_SetEndAtLastLine(iEndAtLastLine);
	SciCall_SetCaretSticky(SC_CARETSTICKY_OFF);
	SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
	SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	SciCall_SetMultipleSelection(iSelectOption & SelectOption_EnableMultipleSelection);
	SciCall_SetAdditionalSelectionTyping(true);
	SciCall_SetMultiPaste(SC_MULTIPASTE_EACH);
	SciCall_SetVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);
	SciCall_SetAdditionalCaretsBlink(true);
	SciCall_SetAdditionalCaretsVisible(true);
	// style both before and after the visible text in the background
	SciCall_SetIdleStyling(SC_IDLESTYLING_ALL);
	// profile lexer performance
	//SciCall_SetIdleStyling(SC_IDLESTYLING_NONE);

	SciCall_AssignCmdKey((SCK_NEXT + (SCMOD_CTRL << 16)), SCI_PARADOWN);
	SciCall_AssignCmdKey((SCK_PRIOR + (SCMOD_CTRL << 16)), SCI_PARAUP);
	SciCall_AssignCmdKey((SCK_NEXT + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARADOWNEXTEND);
	SciCall_AssignCmdKey((SCK_PRIOR + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARAUPEXTEND);
	SciCall_AssignCmdKey((SCK_HOME + (SCMOD_NORM << 16)), SCI_VCHOMEWRAP);
	SciCall_AssignCmdKey((SCK_END + (SCMOD_NORM << 16)), SCI_LINEENDWRAP);
	SciCall_AssignCmdKey((SCK_HOME + (SCMOD_SHIFT << 16)), SCI_VCHOMEWRAPEXTEND);
	SciCall_AssignCmdKey((SCK_END + (SCMOD_SHIFT << 16)), SCI_LINEENDWRAPEXTEND);

	iRenderingTechnology = SciCall_GetTechnology();
	iBidirectional = SciCall_GetBidirectional();

	SciCall_SetZoom(iZoomLevel);
	// Tabs
	SciCall_SetTabWidth(tabSettings.globalTabWidth);
	SciCall_SetIndent(tabSettings.globalIndentWidth);
	SciCall_SetUseTabs(!tabSettings.globalTabsAsSpaces);
	SciCall_SetTabIndents(tabSettings.bTabIndents);
	SciCall_SetBackSpaceUnIndents(tabSettings.bBackspaceUnindents);

	// Indent Guides
	Style_SetIndentGuides(bShowIndentGuides);

	// Word wrap
	SciCall_SetWrapMode(fWordWrapG ? iWordWrapMode : SC_WRAP_NONE);
	EditSetWrapIndentMode(tabSettings.globalTabWidth, tabSettings.globalIndentWidth);
	SetWrapVisualFlags();

	if (bShowUnicodeControlCharacter) {
		EditShowUnicodeControlCharacter(true);
	}

	// current line
	SciCall_SetCaretLineVisibleAlways(true);
	SciCall_SetCaretLineHighlightSubLine(bHighlightCurrentSubLine);

	// Long Lines
	if (bMarkLongLines) {
		SciCall_SetEdgeMode((iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND);
		Style_SetLongLineColors();
	} else {
		SciCall_SetEdgeMode(EDGE_NONE);
	}
	SciCall_SetEdgeColumn(iLongLinesLimitG);

	// Margins
	SciCall_SetMarginSensitive(MarginNumber_Bookmark, true);
	SciCall_SetMarginCursor(MarginNumber_Bookmark, SC_CURSORARROW);
	SciCall_SetMarginType(MarginNumber_CodeFolding, SC_MARGIN_SYMBOL);
	SciCall_SetMarginMask(MarginNumber_CodeFolding, SC_MASK_FOLDERS);
	SciCall_SetMarginSensitive(MarginNumber_CodeFolding, true);
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
#if NP2_DEBUG_CODE_FOLDING
	SciCall_SetFoldFlags(SC_FOLDFLAG_LEVELNUMBERS);
#endif
	SciCall_SetAutomaticFold(SC_AUTOMATICFOLD_SHOW | SC_AUTOMATICFOLD_CLICK | SC_AUTOMATICFOLD_CHANGE);
	SciCall_FoldDisplayTextSetStyle(SC_FOLDDISPLAYTEXT_BOXED);
	const char *text = GetFoldDisplayEllipsis(SC_CP_UTF8, 0); // internal default encoding
	SciCall_SetDefaultFoldDisplayText(text);
	// highlight current folding block
	SciCall_MarkerEnableHighlight(bHighlightCurrentBlock);
	SciCall_BraceHighlightIndicator(true, IndicatorNumber_MatchBrace);
	SciCall_BraceBadLightIndicator(true, IndicatorNumber_MatchBraceError);

	// CallTip
	SciCall_SetMouseDwellTime((callTipInfo.showCallTip == ShowCallTip_None)? SC_TIME_FOREVER : CallTipDefaultMouseDwellTime);

	// Nonprinting characters
	SciCall_SetViewWS((bViewWhiteSpace) ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
	SciCall_SetViewEOL(bViewEOLs);
	SciCall_SetAutoInsertMask(autoCompletionConfig.fAutoInsertMask);
}

void EditReplaceDocument(HANDLE pdoc) noexcept {
	const UINT cpEdit = SciCall_GetCodePage();
	SciCall_SetDocPointer(pdoc);
	// reduce reference count to 1
	SciCall_ReleaseDocument(pdoc);
	SciCall_SetCodePage(cpEdit);
	SciCall_SetEOLMode(iCurrentEOLMode);
}

//=============================================================================
//
// MsgCreate() - Handles WM_CREATE
//
//
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(wParam);
	hwndMain = hwnd;
	g_uCurrentDPI = GetWindowDPI(hwnd);
	hmenuMain = GetMenu(hwnd);
	hCurrentMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	Style_DetectBaseFontSize(hCurrentMonitor);

	// Setup edit control
	// create edit control with zero size to avoid
	// a white/black window fades out on startup when using Direct2D.
	EditCreate(hwnd);

	HINSTANCE hInstance = (AsPointer<LPCREATESTRUCT>(lParam))->hInstance;

	// Create Toolbar and Statusbar
	CreateBars(hwnd, hInstance);

	// Window Initialization

	(void)CreateWindowEx(0,
		WC_STATIC,
		szCurFile,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 0, 0,
		hwnd,
		AsPointer<HMENU, ULONG_PTR>(IDC_FILENAME),
		hInstance,
		nullptr);

	// Drag & Drop
#if 0//_WIN32_WINNT >= _WIN32_WINNT_WIN7
	// enable drop file onto non-client area for elevated Notepad4
	ChangeWindowMessageFilterEx(hwnd, WM_DROPFILES, MSGFLT_ADD, nullptr);
	ChangeWindowMessageFilterEx(hwnd, WM_COPYDATA, MSGFLT_ADD, nullptr);
	ChangeWindowMessageFilterEx(hwnd, 0x0049 /*WM_COPYGLOBALDATA*/, MSGFLT_ADD, nullptr);
#endif
	DragAcceptFiles(hwnd, TRUE);

	// File MRU
	const int flags = MRUFlags_FilePath | (static_cast<int>(flagRelativeFileMRU) * MRUFlags_RelativePath) | (static_cast<int>(flagPortableMyDocs) * MRUFlags_PortableMyDocs);
	mruFile.Init(MRU_KEY_RECENT_FILES, iMaxRecentFiles, flags);
	mruFind.Init(MRU_KEY_RECENT_FIND, MRU_MAXITEMS, MRUFlags_QuoteValue);
	mruReplace.Init(MRU_KEY_RECENT_REPLACE, MRU_MAXITEMS, MRUFlags_QuoteValue);
	return 0;
}

//=============================================================================
//
// CreateBars() - Create Toolbar and Statusbar
//
//
void CreateBars(HWND hwnd, HINSTANCE hInstance) noexcept {
	constexpr DWORD dwToolbarStyle = WS_TOOLBAR;
	hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr, dwToolbarStyle,
								 0, 0, 0, 0, hwnd, AsPointer<HMENU, ULONG_PTR>(IDC_TOOLBAR), hInstance, nullptr);

	SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	bool internalBitmap = false;
	const int scale = iAutoScaleToolbar;
#if NP2_ENABLE_HIDPI_IMAGE_RESOURCE
	const UINT dpi = (scale > USER_DEFAULT_SCREEN_DPI) ? (g_uCurrentDPI + scale - USER_DEFAULT_SCREEN_DPI) : g_uCurrentDPI;
#else
	const int dpi = g_uCurrentDPI;
#endif
	// Add normal Toolbar Bitmap
	HBITMAP hbmp = nullptr;
	if (tchToolbarBitmap != nullptr) {
		hbmp = LoadBitmapFile(tchToolbarBitmap);
	}
	if (hbmp == nullptr) {
		internalBitmap = true;
		const int resource = GetBitmapResourceIdForDPI(IDB_TOOLBAR16, dpi);
		hbmp = static_cast<HBITMAP>(LoadImage(g_exeInstance, MAKEINTRESOURCE(resource), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
	}
	if (scale != 0) {
		hbmp = ResizeImageForDPI(hbmp, dpi);
	}

	BITMAP bmp;
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	HIMAGELIST himl = ImageList_Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
	ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
	SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, AsInteger<LPARAM>(himl));

	if (internalBitmap) {
		HBITMAP hbmpCopy = static_cast<HBITMAP>(CopyImage(hbmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
		const bool fProcessed = BitmapAlphaBlend(hbmpCopy, GetSysColor(COLOR_3DFACE), 0x60);
		if (fProcessed) {
			himl = ImageList_Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
			ImageList_AddMasked(himl, hbmpCopy, CLR_DEFAULT);
			SendMessage(hwndToolbar, TB_SETDISABLEDIMAGELIST, 0, AsInteger<LPARAM>(himl));
		}
		DeleteObject(hbmpCopy);
	}
	DeleteObject(hbmp);

#if NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS
	// Load toolbar labels
	IniSectionParser section;
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_TOOLBAR_LABELS));
	const DWORD cchIniSection = static_cast<DWORD>(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));

	section.Init(COUNTOF(tbbMainWnd));
	LoadIniSection(INI_SECTION_NAME_TOOLBAR_LABELS, pIniSectionBuf, cchIniSection);
	section.ParseArray(pIniSectionBuf, FALSE);

	for (UINT i = 0; i < section.count; i++) {
		const IniKeyValueNode &node = section.nodeList[i];
		const UINT n = static_cast<UINT>(wcstol(node.key, nullptr, 10));
		if (n == 0 || n >= COUNTOF(tbbMainWnd)) {
			continue;
		}

		LPCWSTR tchDesc = node.value;
		if (StrNotEmpty(tchDesc)) {
			tbbMainWnd[n].iString = SendMessage(hwndToolbar, TB_ADDSTRING, 0, AsInteger<LPARAM>(tchDesc));
			tbbMainWnd[n].fsStyle |= BTNS_AUTOSIZE | BTNS_SHOWTEXT;
		} else {
			tbbMainWnd[n].fsStyle &= ~(BTNS_AUTOSIZE | BTNS_SHOWTEXT);
		}
	}

	section.Free();
	NP2HeapFree(pIniSectionBuf);
#endif // NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS

	SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0,
				SendMessage(hwndToolbar, TB_GETEXTENDEDSTYLE, 0, 0) | TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS);

	if (Toolbar_SetButtons(hwndToolbar, tchToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd)) == 0) {
		Toolbar_SetButtons(hwndToolbar, DefaultToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd));
	}

	RECT rc;
	SendMessage(hwndToolbar, TB_GETITEMRECT, 0, AsInteger<LPARAM>(&rc));
	//SendMessage(hwndToolbar, TB_SETINDENT, 2, 0);

	cachedStatusItem.updateMask = ((1 << StatusItem_ItemCount) - 1) ^ (1 << StatusItem_Empty);
	GetString(IDS_STATUSITEM_FORMAT, cachedStatusItem.tchItemFormat, COUNTOF(cachedStatusItem.tchItemFormat));
	const DWORD dwStatusbarStyle = bShowStatusbar ? (WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE) : (WS_CHILD | WS_CLIPSIBLINGS);
	hwndStatus = CreateStatusWindow(dwStatusbarStyle, nullptr, hwnd, IDC_STATUSBAR);

	// Create ReBar and add Toolbar
	const DWORD dwReBarStyle = bShowToolbar ? (WS_REBAR | WS_VISIBLE) : WS_REBAR;
	hwndReBar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, nullptr, dwReBarStyle,
							   0, 0, 0, 0, hwnd, AsPointer<HMENU, ULONG_PTR>(IDC_REBAR), hInstance, nullptr);

	REBARINFO rbi;
	rbi.cbSize = sizeof(REBARINFO);
	rbi.fMask = 0;
	rbi.himl = nullptr;
	SendMessage(hwndReBar, RB_SETBARINFO, 0, AsInteger<LPARAM>(&rbi));

	const BOOL bIsAppThemed = IsAppThemed();
	REBARBANDINFO rbBand;
	rbBand.cbSize = sizeof(REBARBANDINFO);
	rbBand.fMask = /*RBBIM_COLORS | RBBIM_TEXT | RBBIM_BACKGROUND | */
		RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE /*| RBBIM_SIZE*/;
	rbBand.fStyle	 = /*RBBS_CHILDEDGE |*//* RBBS_BREAK |*/ RBBS_FIXEDSIZE /*| RBBS_GRIPPERALWAYS*/;
	if (bIsAppThemed) {
		rbBand.fStyle |= RBBS_CHILDEDGE;
	}
	rbBand.hbmBack = nullptr;
	rbBand.lpText = const_cast<LPWSTR>(L"Toolbar");
	rbBand.hwndChild = hwndToolbar;
	rbBand.cxMinChild = (rc.right - rc.left) * COUNTOF(tbbMainWnd);
	rbBand.cyMinChild = (rc.bottom - rc.top) + 2 * rc.top;
	rbBand.cx		= 0;
	SendMessage(hwndReBar, RB_INSERTBAND, static_cast<WPARAM>(-1), AsInteger<LPARAM>(&rbBand));

	SetWindowPos(hwndReBar, nullptr, 0, 0, 0, 0, SWP_NOZORDER);
	GetWindowRect(hwndReBar, &rc);
	cyReBar = rc.bottom - rc.top;
	cyReBarFrame = bIsAppThemed ? 0 : 2;
}

void RecreateBars(HWND hwnd, HINSTANCE hInstance) noexcept {
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
void MsgDPIChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	g_uCurrentDPI = HIWORD(wParam);
	const RECT* const rc = AsPointer<RECT *>(lParam);
	const Sci_Line iVisTopLine = SciCall_GetFirstVisibleLine();
	const Sci_Line iDocTopLine = SciCall_DocLineFromVisible(iVisTopLine);

	bitmapCache.Invalidate();
	// recreate toolbar and statusbar
	RecreateBars(hwnd, g_hInstance);
	const int cx = rc->right - rc->left;
	const int cy = rc->bottom - rc->top;
	SetWindowPos(hwnd, nullptr, rc->left, rc->top, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	if (bShowToolbar) {
		// on Window 8.1 when move Notepad4 to another monitor with same scaling settings
		// WM_DPICHANGED is sent with same DPI, and WM_SIZE is not sent after WM_DPICHANGED.
		SetWindowPos(hwndReBar, nullptr, 0, 0, cx, cyReBar, SWP_NOZORDER);
	}

	Style_DetectBaseFontSize(hCurrentMonitor);
	Style_OnDPIChanged(pLexCurrent);
	SendMessage(hwndEdit, WM_DPICHANGED, wParam, lParam);
	UpdateLineNumberWidth();
	UpdateBookmarkMarginWidth();
	UpdateFoldMarginWidth();
	SciCall_SetFirstVisibleLine(iVisTopLine);
	SciCall_EnsureVisible(iDocTopLine);
	UpdateToolbar();
	UpdateStatusbar();
}

//=============================================================================
//
// MsgThemeChanged() - Handle WM_THEMECHANGED
//
//
void MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	// reinitialize edit frame
	DWORD dwExStyle = GetWindowExStyle(hwndEdit);
	if (IsAppThemed()) {
		dwExStyle &= ~WS_EX_CLIENTEDGE;
	} else {
		dwExStyle |= WS_EX_CLIENTEDGE;
	}
	SetWindowExStyle(hwndEdit, dwExStyle);
	SetWindowPos(hwndEdit, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

	// recreate toolbar and statusbar
	HINSTANCE hInstance = GetWindowInstance(hwnd);
	RecreateBars(hwnd, hInstance);

	RECT rc;
	GetClientRect(hwnd, &rc);
	SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
	UpdateToolbar();
	UpdateStatusbar();
}

//=============================================================================
//
// MsgSize() - Handles WM_SIZE
//
//
void MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(hwnd);
	if (wParam == SIZE_MINIMIZED) {
		return;
	}

	constexpr int x = 0;
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
		SetWindowPos(hwndReBar, nullptr, 0, 0, cx, cyReBar, SWP_NOZORDER);
		// the ReBar automatically sets the correct height
		// calling SetWindowPos() with the height of one toolbar button
		// causes the control not to temporarily use the whole client area
		// and prevents flickering

		//GetWindowRect(hwndReBar, &rc);
		y = cyReBar + cyReBarFrame;		 // define
		cy -= cyReBar + cyReBarFrame;	 // border
	}

	if (bShowStatusbar) {
		DisableDelayedStatusBarRedraw();
		SendMessage(hwndStatus, WM_SIZE, 0, 0);
		RECT rc;
		GetWindowRect(hwndStatus, &rc);
		cy -= (rc.bottom - rc.top);
	}

	SetWindowPos(hwndEdit, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

	// resize Statusbar items
	UpdateStatusbar();
}

void UpdateStatusBarCache(int item) noexcept {
	cachedStatusItem.updateMask |= (1 << item);
	switch (item) {
	case StatusItem_Lexer:
		cachedStatusItem.pszLexerName = Style_GetCurrentLexerName(cachedStatusItem.tchLexerName, MAX_EDITLEXER_NAME_SIZE);
		break;

	case StatusItem_Encoding:
		Encoding_GetLabel(iCurrentEncoding);
		cachedStatusItem.pszEncoding = mEncoding[iCurrentEncoding].wchLabel;
		break;

	case StatusItem_EolMode:
		cachedStatusItem.pszEolMode = (iCurrentEOLMode == SC_EOL_LF) ? L"LF" : ((iCurrentEOLMode == SC_EOL_CR) ? L"CR" : L"CR+LF");
		break;

	case StatusItem_OvrMode: {
		const BOOL overType = SciCall_GetOvertype();
		cachedStatusItem.overType = overType;
		cachedStatusItem.pszOvrMode = overType ? L"OVR" : L"INS";
	} break;

	case StatusItem_Zoom:
		wsprintf(cachedStatusItem.tchZoom, L"%i%%", iZoomLevel);
		break;
	}
}

#if NP2_ENABLE_APP_LOCALIZATION_DLL
void ValidateUILangauge() noexcept {
	const LANGID subLang = SUBLANGID(uiLanguage);
	switch (PRIMARYLANGID(uiLanguage)) {
	case LANG_ENGLISH:
		languageMenu = IDM_LANG_ENGLISH_US;
		break;
	case LANG_CHINESE:
		languageMenu = IsChineseTraditionalSubLang(subLang)? IDM_LANG_CHINESE_TRADITIONAL : IDM_LANG_CHINESE_SIMPLIFIED;
		break;
	case LANG_FRENCH:
		languageMenu = IDM_LANG_FRENCH_FRANCE;
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
	case LANG_KOREAN:
		languageMenu = IDM_LANG_KOREAN;
		break;
	case LANG_POLISH:
		languageMenu = IDM_LANG_POLISH;
		break;
	case LANG_PORTUGUESE:
		languageMenu = IDM_LANG_PORTUGUESE_BRAZIL;
		break;
	case LANG_RUSSIAN:
		languageMenu = IDM_LANG_RUSSIAN;
		break;
	case LANG_NEUTRAL:
	default:
		languageMenu = IDM_LANG_USER_DEFAULT;
		uiLanguage = LANG_USER_DEFAULT;
		break;
	}
}

void SetUILanguage(int menu) noexcept {
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
	case IDM_LANG_FRENCH_FRANCE:
		lang = MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
		break;
	case IDM_LANG_GERMAN:
		lang = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN);
		break;
	case IDM_LANG_ITALIAN:
		lang = MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN);
		break;
	case IDM_LANG_JAPANESE:
		lang = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
		break;
	case IDM_LANG_KOREAN:
		lang = MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT);
		break;
	case IDM_LANG_POLISH:
		lang = MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT);
		break;
	case IDM_LANG_PORTUGUESE_BRAZIL:
		lang = MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN);
		break;
	case IDM_LANG_RUSSIAN:
		lang = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
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

bool IsIMEInNativeMode() noexcept {
	bool result = false;
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

void MsgNotifyZoom() noexcept {
	const Sci_Line iVisTopLine = SciCall_GetFirstVisibleLine();
	const Sci_Line iDocTopLine = SciCall_DocLineFromVisible(iVisTopLine);
	iZoomLevel = SciCall_GetZoom();

#if 0
	char buf[64];
	sprintf(buf, "Zoom: %d%%", iZoomLevel);
	ShowNotificationA(SC_NOTIFICATIONPOSITION_CENTER, buf);
#endif

	// See https://sourceforge.net/p/scintilla/bugs/2118/
	// set minimum visual tab width to 1 when font size smaller than 3.5pt.
	SciCall_SetTabMinimumWidth((iZoomLevel < 40)? 1 : 2);

	UpdateStatusBarCache(StatusItem_Zoom);
	Style_OnDPIChanged(pLexCurrent);
	UpdateLineNumberWidth();
	UpdateBookmarkMarginWidth();
	UpdateFoldMarginWidth();
	SciCall_SetFirstVisibleLine(iVisTopLine);
	SciCall_EnsureVisible(iDocTopLine);
	UpdateStatusbar();
}

//=============================================================================
//
// MsgInitMenu() - Handles WM_INITMENU
//
//
void MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(lParam);
	HMENU hmenu = AsPointer<HMENU>(wParam);

	const bool hasPath = StrNotEmpty(szCurFile);
	static const uint16_t menuRequiresPath[] = {
		CMD_RECODEDEFAULT,
		CMD_RELOADANSI,
		CMD_RELOADNOFILEVARS,
		CMD_RELOADOEM,
		CMD_RELOADUTF8,
		IDM_FILE_ADDTOFAV,
		IDM_FILE_CREATELINK,
		IDM_FILE_LAUNCH,
		IDM_FILE_OPEN_CONTAINING_FOLDER,
		IDM_FILE_PROPERTIES,
		IDM_FILE_READONLY_FILE,
		IDM_FILE_REVERT,
		IDM_RECODE_SELECT,
	};
	for (unsigned k = 0; k < COUNTOF(menuRequiresPath); k++) {
		EnableCmd(hmenu, menuRequiresPath[k], hasPath);
	}

	EnableCmd(hmenu, IDM_FILE_SAVE, IsDocumentModified());
#if defined(_WIN64)
	DisableCmd(hmenu, IDM_FILE_LARGE_FILE_MODE, bLargeFileMode);
	DisableCmd(hmenu, IDM_FILE_LARGE_FILE_MODE_RELOAD, bLargeFileMode);
#endif
	EnableCmd(hmenu, IDM_FILE_RELAUNCH_ELEVATED, IsVistaAndAbove() && !fIsElevated);
	CheckCmd(hmenu, IDM_FILE_READONLY_FILE, bReadOnlyFile);
	CheckCmd(hmenu, IDM_FILE_READONLY_MODE, bReadOnlyMode);

	int i = IDM_ENCODING_ANSI - 1;
	if (iCurrentEncoding <= CPI_UTF8SIGN) {
		constexpr UINT mask = ((IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1) << CPI_DEFAULT*4)
			| ((IDM_ENCODING_UNICODE - IDM_ENCODING_ANSI + 1) << CPI_UNICODEBOM*4)
			| ((IDM_ENCODING_UNICODEREV - IDM_ENCODING_ANSI + 1) << CPI_UNICODEBEBOM*4)
			| ((IDM_ENCODING_UTF8 - IDM_ENCODING_ANSI + 1) << CPI_UTF8*4)
			| ((IDM_ENCODING_UTF8SIGN - IDM_ENCODING_ANSI + 1) << CPI_UTF8SIGN*4);
		i += (mask >> (iCurrentEncoding*4)) & 15;
	}
	CheckMenuRadioItem(hmenu, IDM_ENCODING_ANSI, IDM_ENCODING_UTF8SIGN, i, MF_BYCOMMAND);

	i = IDM_LINEENDINGS_CRLF + iCurrentEOLMode;
	CheckMenuRadioItem(hmenu, IDM_LINEENDINGS_CRLF, IDM_LINEENDINGS_LF, i, MF_BYCOMMAND);

	EnableCmd(hmenu, IDM_FILE_RECENT, (mruFile.iSize > 0));

	EnableCmd(hmenu, IDM_EDIT_UNDO, SciCall_CanUndo());
	EnableCmd(hmenu, IDM_EDIT_REDO, SciCall_CanRedo());

	OpenClipboard(hwnd);
	EnableCmd(hmenu, IDM_EDIT_CLEARCLIPBOARD, CountClipboardFormats());
	CloseClipboard();

	const bool nonEmpty = SciCall_GetLength() != 0;
	static const uint16_t menuRequiresDoc[] = {
		BME_EDIT_BOOKMARKCLEAR,
		BME_EDIT_BOOKMARKNEXT,
		BME_EDIT_BOOKMARKPREV,
		BME_EDIT_BOOKMARKSELECT,
		BME_EDIT_BOOKMARKTOGGLE,
		CMD_CTRLBACK,
		CMD_CTRLDEL,
		CMD_OPEN_CONTAINING_FOLDER,
		CMD_OPEN_PATH_OR_LINK,
		CMD_TIMESTAMPS,
		IDM_EDIT_CLEARDOCUMENT,
		IDM_EDIT_CODE_COMPRESS,
		IDM_EDIT_CODE_PRETTY,
		IDM_EDIT_COMPLETEWORD,
		IDM_EDIT_COPY,
		IDM_EDIT_COPYALL,
		IDM_EDIT_CUT,
		IDM_EDIT_DELETE,
		IDM_EDIT_DELETELINELEFT,
		IDM_EDIT_DELETELINERIGHT,
		IDM_EDIT_FIND,
		IDM_EDIT_FINDMATCHINGBRACE,
		IDM_EDIT_FINDNEXT,
		IDM_EDIT_FINDPREV,
		IDM_EDIT_GOTOLINE,
		IDM_EDIT_GOTO_BLOCK_END,
		IDM_EDIT_GOTO_BLOCK_START,
		IDM_EDIT_GOTO_NEXT_BLOCK,
		IDM_EDIT_GOTO_NEXT_BLOCK,
		IDM_EDIT_GOTO_NEXT_SIBLING_BLOCK,
		IDM_EDIT_GOTO_PREVIOUS_BLOCK,
		IDM_EDIT_GOTO_PREV_SIBLING_BLOCK,
		IDM_EDIT_REPLACE,
		IDM_EDIT_REPLACENEXT,
		IDM_EDIT_SAVEFIND,
		IDM_EDIT_SELECTALL,
		IDM_EDIT_SELECTLINE,
		IDM_EDIT_SELECTLINE_BLOCK,
		IDM_EDIT_SELECTWORD,
		IDM_EDIT_SELTODOCEND,
		IDM_EDIT_SELTODOCSTART,
		IDM_EDIT_SELTOMATCHINGBRACE,
		IDM_EDIT_SELTONEXT,
		IDM_EDIT_SELTOPREV,
	};
	for (unsigned k = 0; k < COUNTOF(menuRequiresDoc); k++) {
		EnableCmd(hmenu, menuRequiresDoc[k], nonEmpty);
	}

	const bool hasSel = !SciCall_IsSelectionEmpty();
	static const uint16_t menuRequiresSelection[] = {
		// IDM_EDIT_COPY_BINARY,
		// IDM_EDIT_CUT_BINARY,
		CMD_CUSTOM_ACTION1,
		CMD_CUSTOM_ACTION2,
		CMD_JUMP2SELEND,
		CMD_JUMP2SELSTART,
		CMD_ONLINE_SEARCH_BING,
		CMD_ONLINE_SEARCH_GOOGLE,
		CMD_ONLINE_SEARCH_WIKI,
		IDM_EDIT_BASE64_DECODE,
		IDM_EDIT_BASE64_DECODE_AS_HEX,
		IDM_EDIT_BASE64_ENCODE,
		IDM_EDIT_BASE64_HTML_EMBEDDED_IMAGE,
		IDM_EDIT_BASE64_SAFE_ENCODE,
		IDM_EDIT_CHAR2HEX,
		IDM_EDIT_COLUMNWRAP,
		IDM_EDIT_CONVERTLOWERCASE,
		IDM_EDIT_CONVERTSPACES,
		IDM_EDIT_CONVERTSPACES2,
		IDM_EDIT_CONVERTTABS,
		IDM_EDIT_CONVERTTABS2,
		IDM_EDIT_CONVERTUPPERCASE,
		IDM_EDIT_COPYADD,
		IDM_EDIT_COPYRTF,
		IDM_EDIT_ESCAPECCHARS,
		IDM_EDIT_HEX2CHAR,
		IDM_EDIT_INVERTCASE,
		IDM_EDIT_JOINLINES,
		IDM_EDIT_JOINLINESEX,
		IDM_EDIT_MAP_BENGALI_LATIN,
		IDM_EDIT_MAP_CYRILLIC_LATIN,
		IDM_EDIT_MAP_DEVANAGARI_LATIN,
		IDM_EDIT_MAP_FULLWIDTH,
		IDM_EDIT_MAP_HALFWIDTH,
		IDM_EDIT_MAP_HANGUL_DECOMPOSITION,
		IDM_EDIT_MAP_HANJA_HANGUL,
		IDM_EDIT_MAP_HIRAGANA,
		IDM_EDIT_MAP_KATAKANA,
		IDM_EDIT_MAP_MALAYALAM_LATIN,
		IDM_EDIT_MAP_SIMPLIFIED_CHINESE,
		IDM_EDIT_MAP_TRADITIONAL_CHINESE,
		IDM_EDIT_NUM2BIN,
		IDM_EDIT_NUM2DEC,
		IDM_EDIT_NUM2HEX,
		IDM_EDIT_NUM2OCT,
		IDM_EDIT_SENTENCECASE,
		IDM_EDIT_SHOW_HEX,
		IDM_EDIT_SPLITLINES,
		IDM_EDIT_TITLECASE,
		IDM_EDIT_UNESCAPECCHARS,
		IDM_EDIT_URLCOMPONENTENCODE,
		IDM_EDIT_URLDECODE,
		IDM_EDIT_URLENCODE,
		IDM_EDIT_XHTML_ESCAPE_CHAR,
		IDM_EDIT_XHTML_UNESCAPE_CHAR,
		IDM_VIEW_SHOWEXCERPT,
	};
	for (unsigned k = 0; k < COUNTOF(menuRequiresSelection); k++) {
		EnableCmd(hmenu, menuRequiresSelection[k], hasSel);
	}

	const bool canPaste = SciCall_CanPaste();
	EnableCmd(hmenu, IDM_EDIT_PASTE, canPaste);
	//EnableCmd(hmenu, IDM_EDIT_PASTE_BINARY, canPaste);
	EnableCmd(hmenu, IDM_EDIT_SWAP, hasSel || canPaste);

	i = EditGetSelectedLineCount() > 1;
	EnableCmd(hmenu, IDM_EDIT_SORTLINES, i);
	EnableCmd(hmenu, IDM_EDIT_REMOVEDUPLICATELINE, i);
	EnableCmd(hmenu, IDM_EDIT_MERGEDUPLICATELINE, i);

	DisableCmd(hmenu, IDM_EDIT_LINECOMMENT, (pLexCurrent->lexerAttr & LexerAttr_NoLineComment));
	DisableCmd(hmenu, IDM_EDIT_STREAMCOMMENT, (pLexCurrent->lexerAttr & LexerAttr_NoBlockComment));

	CheckCmd(hmenu, IDM_VIEW_SHOW_FOLDING, bShowCodeFolding);
	CheckCmd(hmenu, IDM_VIEW_USEDEFAULT_CODESTYLE, pLexCurrent->bUseDefaultCodeStyle);
	i = IDM_VIEW_STYLE_THEME_DEFAULT + np2StyleTheme;
	CheckMenuRadioItem(hmenu, IDM_VIEW_STYLE_THEME_DEFAULT, IDM_VIEW_STYLE_THEME_DARK, i, MF_BYCOMMAND);

	CheckCmd(hmenu, IDM_VIEW_WORDWRAP, fvCurFile.fWordWrap);
	i = IDM_VIEW_FONTQUALITY_DEFAULT + iFontQuality;
	CheckMenuRadioItem(hmenu, IDM_VIEW_FONTQUALITY_DEFAULT, IDM_VIEW_FONTQUALITY_CLEARTYPE, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_CARET_STYLE_BLOCK_OVR, bBlockCaretForOVRMode);
	i = IDM_VIEW_CARET_STYLE_BLOCK + static_cast<int>(iCaretStyle);
	CheckMenuRadioItem(hmenu, IDM_VIEW_CARET_STYLE_BLOCK, IDM_VIEW_CARET_STYLE_WIDTH3, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_CARET_STYLE_NOBLINK, iCaretBlinkPeriod == 0);
	UncheckCmd(hmenu, IDM_VIEW_CARET_STYLE_SELECTION, bBlockCaretOutSelection);
	CheckCmd(hmenu, IDM_VIEW_LONGLINEMARKER, bMarkLongLines);
	CheckCmd(hmenu, IDM_VIEW_TABSASSPACES, fvCurFile.bTabsAsSpaces);
	CheckCmd(hmenu, IDM_VIEW_SHOWINDENTGUIDES, bShowIndentGuides);
	CheckCmd(hmenu, IDM_VIEW_LINENUMBERS, bShowLineNumbers);
	CheckCmd(hmenu, IDM_VIEW_MARGIN, bShowBookmarkMargin);
	CheckCmd(hmenu, IDM_VIEW_CHANGE_HISTORY_MARKER, iChangeHistoryMarker);
	CheckCmd(hmenu, IDM_VIEW_AUTOCOMPLETION_IGNORECASE, autoCompletionConfig.bIgnoreCase);
	CheckCmd(hmenu, IDM_SET_LATEX_INPUT_METHOD, autoCompletionConfig.bLaTeXInputMethod);
	CheckCmd(hmenu, IDM_SET_MULTIPLE_SELECTION, iSelectOption & SelectOption_EnableMultipleSelection);
	CheckCmd(hmenu, IDM_SET_SELECTIONASFINDTEXT, iSelectOption & SelectOption_CopySelectionAsFindText);
	CheckCmd(hmenu, IDM_SET_PASTEBUFFERASFINDTEXT, iSelectOption & SelectOption_CopyPasteBufferAsFindText);
	i = IDM_LINE_SELECTION_MODE_NONE + iLineSelectionMode;
	CheckMenuRadioItem(hmenu, IDM_LINE_SELECTION_MODE_NONE, IDM_LINE_SELECTION_MODE_OLDVS, i, MF_BYCOMMAND);

	UncheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_OFF, bMarkOccurrences & MarkOccurrences_Enable);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_CASE, bMarkOccurrences & MarkOccurrences_MatchCase);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_WORD, bMarkOccurrences & MarkOccurrences_WholeWord);
	CheckCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_BOOKMARK, bMarkOccurrences & MarkOccurrences_Bookmark);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_CASE, bMarkOccurrences & MarkOccurrences_Enable);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_WORD, bMarkOccurrences & MarkOccurrences_Enable);
	EnableCmd(hmenu, IDM_VIEW_MARKOCCURRENCES_BOOKMARK, bMarkOccurrences & MarkOccurrences_Enable);
	i = IDM_VIEW_SHOWCALLTIP_OFF + static_cast<int>(callTipInfo.showCallTip);
	CheckMenuRadioItem(hmenu, IDM_VIEW_SHOWCALLTIP_OFF, IDM_VIEW_SHOWCALLTIP_ABGR, i, MF_BYCOMMAND);

	CheckCmd(hmenu, IDM_VIEW_SHOWWHITESPACE, bViewWhiteSpace);
	CheckCmd(hmenu, IDM_VIEW_SHOWEOLS, bViewEOLs);
	CheckCmd(hmenu, IDM_VIEW_WORDWRAPSYMBOLS, bShowWordWrapSymbols);
	EnableCmd(hmenu, IDM_VIEW_UNICODE_CONTROL_CHAR, iCurrentEncoding != CPI_DEFAULT);
	CheckCmd(hmenu, IDM_VIEW_UNICODE_CONTROL_CHAR, bShowUnicodeControlCharacter);
	CheckCmd(hmenu, IDM_VIEW_MENU, bShowMenu);
	CheckCmd(hmenu, IDM_VIEW_TOOLBAR, bShowToolbar);
	EnableCmd(hmenu, IDM_VIEW_CUSTOMIZE_TOOLBAR, bShowToolbar);
	CheckCmd(hmenu, IDM_VIEW_AUTO_SCALE_TOOLBAR, iAutoScaleToolbar);
#if NP2_ENABLE_HIDPI_IMAGE_RESOURCE
	CheckCmd(hmenu, IDM_VIEW_USE_LARGE_TOOLBAR, iAutoScaleToolbar > USER_DEFAULT_SCREEN_DPI);
#endif
	CheckCmd(hmenu, IDM_VIEW_STATUSBAR, bShowStatusbar);
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	CheckMenuRadioItem(hmenu, IDM_LANG_USER_DEFAULT, IDM_LANG_LAST_LANGUAGE, languageMenu, MF_BYCOMMAND);
#endif
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_ON_START, iFullScreenMode & FullScreenMode_OnStartup);
	CheckCmd(hmenu, IDM_VIEW_FULLSCREEN_HIDE_TITLE, iFullScreenMode & FullScreenMode_HideCaption);

	CheckCmd(hmenu, IDM_VIEW_MATCHBRACES, bMatchBraces);
	CheckCmd(hmenu, IDM_VIEW_HIGHLIGHTCURRENT_BLOCK, bHighlightCurrentBlock);
	i = IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE + static_cast<int>(iHighlightCurrentLine);
	CheckMenuRadioItem(hmenu, IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE, IDM_VIEW_HIGHLIGHTCURRENTLINE_FRAME, i, MF_BYCOMMAND);
	CheckCmd(hmenu, IDM_VIEW_HIGHLIGHTCURRENTLINE_SUBLINE, bHighlightCurrentSubLine);

	CheckCmd(hmenu, IDM_VIEW_REUSEWINDOW, bReuseWindow);
	CheckCmd(hmenu, IDM_VIEW_STICKY_WINDOW_POSITION, bStickyWindowPosition);
	DisableCmd(hmenu, IDM_VIEW_CLEARWINPOS, bStickyWindowPosition);
	CheckCmd(hmenu, IDM_VIEW_SINGLEFILEINSTANCE, bSingleFileInstance);
	CheckCmd(hmenu, IDM_VIEW_ALWAYSONTOP, IsTopMost());
	CheckCmd(hmenu, IDM_VIEW_MINTOTRAY, bMinimizeToTray);
	CheckCmd(hmenu, IDM_VIEW_TRANSPARENT, bTransparentMode);
	i = IDM_VIEW_SCROLLPASTLASTLINE_ONE + iEndAtLastLine;
	CheckMenuRadioItem(hmenu, IDM_VIEW_SCROLLPASTLASTLINE_ONE, IDM_VIEW_SCROLLPASTLASTLINE_QUARTER, i, MF_BYCOMMAND);

	// Rendering Technology
	i = IsVistaAndAbove();
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2D, i);
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2DRETAIN, i);
	EnableCmd(hmenu, IDM_SET_RENDER_TECH_D2DDC, i);
	EnableCmd(hmenu, IDM_SET_USE_XP_FILE_DIALOG, i);
	CheckCmd(hmenu, IDM_SET_USE_XP_FILE_DIALOG, bUseXPFileDialog);
	i = IDM_SET_RENDER_TECH_GDI + iRenderingTechnology;
	CheckMenuRadioItem(hmenu, IDM_SET_RENDER_TECH_GDI, IDM_SET_RENDER_TECH_D2DDC, i, MF_BYCOMMAND);
	// RTL Layout
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
	CheckCmd(hmenu, IDM_SET_OPEN_FOLDER_MATEPATH, bOpenFolderWithMatepath);

	CheckCmd(hmenu, IDM_VIEW_CHANGENOTIFY, (iFileWatchingMode != FileWatchingMode_None));
	CheckCmd(hmenu, IDM_SET_FILE_AUTOSAVE, (iAutoSaveOption & AutoSaveOption_Periodic) && dwAutoSavePeriod != 0);

	if (StrNotEmpty(szTitleExcerpt)) {
		i = IDM_VIEW_SHOWEXCERPT;
	} else {
		i = IDM_VIEW_SHOWFILENAMEONLY + static_cast<int>(iPathNameFormat);
	}
	CheckMenuRadioItem(hmenu, IDM_VIEW_SHOWFILENAMEONLY, IDM_VIEW_SHOWEXCERPT, i, MF_BYCOMMAND);

	i = IDM_VIEW_NOESCFUNC + static_cast<int>(iEscFunction);
	CheckMenuRadioItem(hmenu, IDM_VIEW_NOESCFUNC, IDM_VIEW_ESCEXIT, i, MF_BYCOMMAND);

	i = StrNotEmpty(szIniFile);
	const uint16_t menuRequiresIni[] = {
		CMD_OPENINIFILE,
		IDM_VIEW_NOSAVEFINDREPL,
		IDM_VIEW_NOSAVERECENT,
		IDM_VIEW_REUSEWINDOW,
		IDM_VIEW_SAVESETTINGS,
		IDM_VIEW_SINGLEFILEINSTANCE,
		IDM_VIEW_STICKY_WINDOW_POSITION,
	};
	for (unsigned k = 0; k < COUNTOF(menuRequiresIni); k++) {
		EnableCmd(hmenu, menuRequiresIni[k], i);
	}

	CheckCmd(hmenu, IDM_VIEW_SAVESETTINGS, i && bSaveSettings);
	EnableCmd(hmenu, IDM_VIEW_SAVESETTINGSNOW, i || StrNotEmpty(szIniFile2));

	Style_UpdateSchemeMenu(hmenu);
}

static void ConvertLineEndings(int iNewEOLMode) noexcept {
	iCurrentEOLMode = iNewEOLMode;
	SciCall_SetEOLMode(iNewEOLMode);
	EditEnsureConsistentLineEndings();
	UpdateStatusBarCache(StatusItem_EolMode);
	UpdateWindowTitle();
}

static inline bool IsBraceMatchChar(uint32_t ch) noexcept {
#if 0
	return ch == '(' || ch == ')'
		|| ch == '[' || ch == ']'
		|| ch == '{' || ch == '}'
		|| ch == '<' || ch == '>';
#else
	// see GenerateBraceMatchTable() in tools/GenerateTable.py
	static const uint32_t table[8] = { 0, 0x50000300, 0x28000000, 0x28000000 };
	return BitTestEx(table, ch);
#endif
}

static inline void HandleTabCompletion() noexcept {
	SciCall_TabCompletion((autoCompletionConfig.bLaTeXInputMethod * TAB_COMPLETION_LATEX) | TAB_COMPLETION_DEFAULT);
}

//=============================================================================
//
// MsgCommand() - Handles WM_COMMAND
//
//
LRESULT MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam)) {
	case IDT_FILE_NEW:
	case IDM_FILE_NEW:
		FileLoad(FileLoadFlag_New, L"");
		break;

	case IDT_FILE_OPEN:
	case IDM_FILE_OPEN:
		FileLoad(FileLoadFlag_Default, L"");
		break;

	case IDM_FILE_REVERT:
		if (StrNotEmpty(szCurFile)) {
			if (IsDocumentModified() && MsgBoxWarn(MB_OKCANCEL, IDS_ASK_REVERT) != IDOK) {
				return 0;
			}

			iWeakSrcEncoding = iCurrentEncoding;
			FileLoad(static_cast<FileLoadFlag>(FileLoadFlag_DontSave | FileLoadFlag_Reload), szCurFile);
		}
		break;

	case IDT_FILE_SAVE:
	case IDM_FILE_SAVE:
		FileSave(FileSaveFlag_SaveAlways);
		break;

	case IDT_FILE_SAVEAS:
	case IDM_FILE_SAVEAS:
		FileSave(static_cast<FileSaveFlag>(FileSaveFlag_SaveAlways | FileSaveFlag_SaveAs));
		break;

	case IDM_FILE_SAVEBACKUP:
		AutoSave_DoWork(FileSaveFlag_SaveAlways);
		break;

	case IDT_FILE_SAVECOPY:
	case IDM_FILE_SAVECOPY:
		FileSave(static_cast<FileSaveFlag>(FileSaveFlag_SaveAlways | FileSaveFlag_SaveAs | FileSaveFlag_SaveCopy));
		break;

	case IDM_FILE_READONLY_FILE:
		if (StrNotEmpty(szCurFile)) {
			DWORD dwFileAttributes = GetFileAttributes(szCurFile);
			if (dwFileAttributes != INVALID_FILE_ATTRIBUTES) {
				if (bReadOnlyFile) {
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
			bReadOnlyFile = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);
			if (!bReadOnlyFile && bReadOnlyMode) {
				bReadOnlyMode = false;
				SciCall_SetReadOnly(false);
			}
			UpdateWindowTitle();
		}
		break;

	case IDM_FILE_READONLY_MODE:
		bReadOnlyMode = !bReadOnlyMode;
		SciCall_SetReadOnly(bReadOnlyMode);
		UpdateWindowTitle();
		break;

	case IDT_FILE_BROWSE:
	case IDM_FILE_BROWSE:
		TryBrowseFile(hwnd, szCurFile, true);
		break;

	case IDM_FILE_NEWWINDOW:
	case IDM_FILE_NEWWINDOW2:
	case IDM_FILE_RESTART: {
		const bool emptyWind = LOWORD(wParam) == IDM_FILE_NEWWINDOW2;
		if (!emptyWind && bSaveBeforeRunningTools && !FileSave(FileSaveFlag_Ask)) {
			break;
		}

		WCHAR szModuleName[MAX_PATH];
		GetModuleFileName(nullptr, szModuleName, COUNTOF(szModuleName));
		LPWSTR szParameters = static_cast<LPWSTR>(NP2HeapAlloc(sizeof(WCHAR) * 1024));
		GetRelaunchParameters(szParameters, szCurFile, true, emptyWind);

		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOZONECHECKS;
		sei.hwnd = hwnd;
		sei.lpVerb = nullptr;
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
		flagRelaunchElevated = RelaunchElevatedFlag_Manual;
		if (RelaunchElevated()) {
			DestroyWindow(hwnd);
		}
		break;

	case IDM_FILE_OPEN_CONTAINING_FOLDER:
		OpenContainingFolder(hwnd, szCurFile, true);
		break;

	case IDT_FILE_LAUNCH:
	case IDM_FILE_LAUNCH: {
		if (StrIsEmpty(szCurFile)) {
			break;
		}

		if (bSaveBeforeRunningTools && !FileSave(FileSaveFlag_Ask)) {
			break;
		}

		WCHAR wchDirectory[MAX_PATH] = L"";
		if (StrNotEmpty(szCurFile)) {
			lstrcpy(wchDirectory, szCurFile);
			PathRemoveFileSpec(wchDirectory);
		}

		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = nullptr;
		sei.lpFile = szCurFile;
		sei.lpParameters = nullptr;
		sei.lpDirectory = wchDirectory;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);
	}
	break;

	case IDM_FILE_RUN: {
		if (bSaveBeforeRunningTools && !FileSave(FileSaveFlag_Ask)) {
			break;
		}

		WCHAR tchCmdLine[MAX_PATH + 4];
		lstrcpy(tchCmdLine, szCurFile);
		PathQuoteSpaces(tchCmdLine);

		RunDlg(hwnd, tchCmdLine);
	}
	break;

	case IDM_FILE_OPENWITH:
		if (bSaveBeforeRunningTools && !FileSave(FileSaveFlag_Ask)) {
			break;
		}
		OpenWithDlg(hwnd, szCurFile);
		break;

	case IDM_FILE_PAGESETUP:
		EditPrintSetup(hwndEdit);
		break;

	case IDT_FILE_PRINT:
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

		if (!EditPrint(hwndEdit, pszTitle, lParam & TRUE)) {
			MsgBoxWarn(MB_OK, IDS_PRINT_ERROR, pszTitle);
		} else if (lParam) {
			SendWMCommand(hwnd, IDM_FILE_EXIT);
		}
	}
	break;

	case IDM_FILE_PROPERTIES: {
		if (StrIsEmpty(szCurFile)) {
			break;
		}

		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

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

	case IDT_FILE_OPENFAV:
	case IDM_FILE_OPENFAV:
		if (FileSave(FileSaveFlag_Ask)) {
			WCHAR tchSelItem[MAX_PATH];

			if (FavoritesDlg(hwnd, tchSelItem)) {
				PathGetLnkPath(tchSelItem, tchSelItem);
				if (PathIsDirectory(tchSelItem)) {
					WCHAR tchFile[MAX_PATH];

					if (OpenFileDlg(tchFile, COUNTOF(tchFile), tchSelItem)) {
						FileLoad(FileLoadFlag_DontSave, tchFile);
					}
				} else {
					FileLoad(FileLoadFlag_DontSave, tchSelItem);
				}
			}
		}
		break;

	case IDT_FILE_ADDTOFAV:
	case IDM_FILE_ADDTOFAV:
		if (StrNotEmpty(szCurFile)) {
			SHFILEINFO shfi;
			SHGetFileInfo2(szCurFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			AddToFavDlg(hwnd, shfi.szDisplayName, szCurFile);
		}
		break;

	case IDM_FILE_MANAGEFAV: {
		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = L"open";
		sei.lpFile = tchFavoritesDir;
		sei.lpParameters = nullptr;
		sei.lpDirectory = nullptr;
		sei.nShow = SW_SHOWNORMAL;

		// Run favorites directory
		ShellExecuteEx(&sei);
	}
	break;

	case IDM_FILE_RECENT:
		if (mruFile.iSize > 0) {
			if (FileSave(FileSaveFlag_Ask)) {
				WCHAR tchFile[MAX_PATH];
				if (FileMRUDlg(hwnd, tchFile)) {
					FileLoad(FileLoadFlag_DontSave, tchFile);
				}
			}
		}
		break;

	case IDT_FILE_EXIT:
	case IDM_FILE_EXIT:
		ExitApplication(hwnd);
		break;

	case IDM_ENCODING_ANSI:
	case IDM_ENCODING_UNICODE:
	case IDM_ENCODING_UNICODEREV:
	case IDM_ENCODING_UTF8:
	case IDM_ENCODING_UTF8SIGN:
	case IDM_ENCODING_SELECT: {
		int iNewEncoding = iCurrentEncoding;
		if (LOWORD(wParam) == IDM_ENCODING_SELECT) {
			if (!SelectEncodingDlg(hwnd, &iNewEncoding, IDS_SELRECT_CURRENT_ENCODING)) {
				break;
			}
		} else {
			constexpr UINT mask = (CPI_DEFAULT << 4*(IDM_ENCODING_ANSI - IDM_ENCODING_ANSI))
				| (CPI_UNICODEBOM << 4*(IDM_ENCODING_UNICODE - IDM_ENCODING_ANSI))
				| (CPI_UNICODEBEBOM << 4*(IDM_ENCODING_UNICODEREV - IDM_ENCODING_ANSI))
				| (CPI_UTF8 << 4*(IDM_ENCODING_UTF8 - IDM_ENCODING_ANSI))
				| (CPI_UTF8SIGN << 4*(IDM_ENCODING_UTF8SIGN - IDM_ENCODING_ANSI));
			iNewEncoding = (mask >> (4*(LOWORD(wParam) - IDM_ENCODING_ANSI))) & 15;
		}

		if (EditSetNewEncoding(iCurrentEncoding, iNewEncoding, flagSetEncoding)) {
			if (SciCall_GetLength() == 0) {
				iCurrentEncoding = iNewEncoding;
				if (StrIsEmpty(szCurFile) || Encoding_HasBOM(iNewEncoding) == Encoding_HasBOM(iOriginalEncoding)) {
					iOriginalEncoding = iNewEncoding;
				}
			} else {
				if (iCurrentEncoding == CPI_DEFAULT || iNewEncoding == CPI_DEFAULT) {
					iOriginalEncoding = CPI_NONE;
				}
				iCurrentEncoding = iNewEncoding;
			}

			UpdateStatusBarCache(StatusItem_Encoding);
			// no Scintilla notification when internal encoding is still UTF-8
			UpdateToolbar();
			UpdateStatusbar();
			UpdateWindowTitle();
		}
	}
	break;

	case IDM_RECODE_SELECT:
		if (StrNotEmpty(szCurFile)) {
			int iNewEncoding = iCurrentEncoding;
			if (iNewEncoding < CPI_UTF7) {
				constexpr UINT mask = ((CPI_NONE + 1) << 4*CPI_DEFAULT) // unknown encoding
					| ((CPI_OEM + 1) << 4*CPI_OEM)
					| ((CPI_UNICODE + 1) << 4*CPI_UNICODEBOM)
					| ((CPI_UNICODEBE + 1) << 4*CPI_UNICODEBEBOM)
					| ((CPI_UNICODE + 1) << 4*CPI_UNICODE)
					| ((CPI_UNICODEBE + 1) << 4*CPI_UNICODEBE)
					| ((CPI_UTF8 + 1) << 4*CPI_UTF8)
					| ((CPI_UTF8 + 1) << 4*CPI_UTF8SIGN);
				iNewEncoding = ((mask >> (4*iNewEncoding)) & 15) - 1;
			}

			if (IsDocumentModified() && MsgBoxWarn(MB_OKCANCEL, IDS_ASK_RECODE) != IDOK) {
				return 0;
			}

			if (SelectEncodingDlg(hwnd, &iNewEncoding, IDS_SELRECT_RELOAD_ENCODING)) {
				iSrcEncoding = iNewEncoding;
				FileLoad(static_cast<FileLoadFlag>(FileLoadFlag_DontSave | FileLoadFlag_Reload), szCurFile);
			}
		}
		break;

	case IDM_ENCODING_SETDEFAULT:
		SelectDefEncodingDlg(hwnd, &iDefaultEncoding);
		break;

	case IDM_LINEENDINGS_CRLF:
	case IDM_LINEENDINGS_LF:
	case IDM_LINEENDINGS_CR: {
		const int iNewEOLMode = LOWORD(wParam) - IDM_LINEENDINGS_CRLF;
		ConvertLineEndings(iNewEOLMode);
	}
	break;

	case IDM_LINEENDINGS_SETDEFAULT:
		SelectDefLineEndingDlg(hwnd, &iDefaultEOLMode);
		break;

	case IDT_EDIT_UNDO:
	case IDM_EDIT_UNDO:
		SciCall_Undo();
		break;

	case IDT_EDIT_REDO:
	case IDM_EDIT_REDO:
		SciCall_Redo();
		break;

	case IDT_EDIT_CUT:
	case IDM_EDIT_CUT:
		if (flagPasteBoard) {
			bLastCopyFromMe = true;
		}
		if (SciCall_IsSelectionEmpty() && iLineSelectionMode != LineSelectionMode_None) {
			const int mode = iLineSelectionMode;
			Sci_Position iCurrentPos = SciCall_GetCurrentPos();
			const Sci_Position iCol = SciCall_GetColumn(iCurrentPos) + 1;
			SciCall_LineCut(mode & LineSelectionMode_VisualStudio);
			iCurrentPos = SciCall_GetCurrentPos();
			const Sci_Line iCurLine = SciCall_LineFromPosition(iCurrentPos);
			EditJumpTo(iCurLine + (mode != LineSelectionMode_OldVisualStudio), iCol);
		} else {
			SciCall_Cut(false);
		}
		break;

	//case IDM_EDIT_CUT_BINARY:
	//	if (flagPasteBoard) {
	//		bLastCopyFromMe = true;
	//	}
	//	SciCall_Cut(true);
	//	break;

	case IDT_EDIT_COPY:
	case IDM_EDIT_COPY:
		if (flagPasteBoard) {
			bLastCopyFromMe = true;
		}
		if (SciCall_IsSelectionEmpty() && iLineSelectionMode != LineSelectionMode_None) {
			SciCall_LineCopy(iLineSelectionMode & LineSelectionMode_VisualStudio);
		} else {
			SciCall_Copy(false);
		}
		UpdateToolbar();
		break;

	//case IDM_EDIT_COPY_BINARY:
	//	if (flagPasteBoard) {
	//		bLastCopyFromMe = true;
	//	}
	//	SciCall_Copy(true);
	//	UpdateToolbar();
	//	break;

	case IDM_EDIT_COPYALL:
		if (flagPasteBoard) {
			bLastCopyFromMe = true;
		}
		SciCall_CopyRange(0, SciCall_GetLength());
		UpdateToolbar();
		break;

	case IDM_EDIT_COPYADD:
		if (flagPasteBoard) {
			bLastCopyFromMe = true;
		}
		EditCopyAppend(hwndEdit);
		UpdateToolbar();
		break;

	case IDT_EDIT_PASTE:
	case IDM_EDIT_PASTE:
	//case IDM_EDIT_PASTE_BINARY:
		SciCall_Paste(LOWORD(wParam) == IDM_EDIT_PASTE_BINARY);
		break;

	case IDM_EDIT_SWAP:
		if (SciCall_IsSelectionEmpty()) {
			const Sci_Position iPos = SciCall_GetCurrentPos();
			SciCall_Paste(false);
			const Sci_Position iNewPos = SciCall_GetCurrentPos();
			SciCall_SetSel(iPos, iNewPos);
			SendWMCommand(hwnd, IDM_EDIT_CLEARCLIPBOARD);
		} else {
			char *pClip = EditGetClipboardText(hwndEdit);
			if (pClip == nullptr) {
				break;
			}
			if (flagPasteBoard) {
				bLastCopyFromMe = true;
			}
			Sci_Position iPos = SciCall_GetCurrentPos();
			Sci_Position iAnchor = SciCall_GetAnchor();
			SciCall_BeginUndoAction();
			SciCall_Cut(false);
			SciCall_ReplaceSel(pClip);
			const size_t len = strlen(pClip);
			if (iPos > iAnchor) {
				iPos = iAnchor + len;
			} else {
				iAnchor = iPos + len;
			}
			SciCall_SetSel(iAnchor, iPos);
			SciCall_EndUndoAction();
			LocalFree(pClip);
		}
		break;

	case IDT_EDIT_DELETE:
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
		EditSelectLines(LOWORD(wParam) == IDM_EDIT_SELECTLINE_BLOCK, iLineSelectionMode & LineSelectionMode_VisualStudio);
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

	case IDM_EDIT_REMOVEDUPLICATELINE:
	case IDM_EDIT_MERGEDUPLICATELINE:
		BeginWaitCursor();
		EditSortLines(static_cast<EditSortFlag>(EditSortFlag_DontSort | ((LOWORD(wParam) == IDM_EDIT_REMOVEDUPLICATELINE) ? EditSortFlag_RemoveDuplicate : EditSortFlag_MergeDuplicate)));
		EndWaitCursor();
		break;

	case IDM_EDIT_CUTLINE:
		if (flagPasteBoard) {
			bLastCopyFromMe = true;
		}
		SciCall_LineCut(iLineSelectionMode & LineSelectionMode_VisualStudio);
		break;

	case IDM_EDIT_COPYLINE:
		if (flagPasteBoard) {
			bLastCopyFromMe = true;
		}
		SciCall_LineCopy(iLineSelectionMode & LineSelectionMode_VisualStudio);
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

	case IDM_EDIT_UNINDENT:
		SciCall_LineDedent();
		break;

	case IDM_EDIT_INDENT:
		SciCall_LineIndent();
		break;

	case IDM_EDIT_ENCLOSESELECTION:
		EditEncloseSelectionDlg(hwnd);
		break;

	case IDM_EDIT_SELECTIONDUPLICATE:
		SciCall_SelectionDuplicate();
		break;

	case IDM_EDIT_PADWITHSPACES:
		BeginWaitCursor();
		EditPadWithSpaces(false, false);
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
		EditStripTrailingBlanks(hwndEdit, false);
		EndWaitCursor();
		break;

	case IDM_EDIT_TRIMLEAD:
		BeginWaitCursor();
		EditStripLeadingBlanks(hwndEdit, false);
		EndWaitCursor();
		break;

	case IDM_EDIT_COMPRESSWS:
		BeginWaitCursor();
		EditCompressSpaces();
		EndWaitCursor();
		break;

	case IDM_EDIT_MERGEBLANKLINES:
	case IDM_EDIT_REMOVEBLANKLINES:
		BeginWaitCursor();
		EditRemoveBlankLines(LOWORD(wParam) == IDM_EDIT_MERGEBLANKLINES);
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
			EditSortLines(iSortOptions);
			EndWaitCursor();
		}
		break;

	case IDM_EDIT_COLUMNWRAP:
		if (ColumnWrapDlg(hwnd)) {
			BeginWaitCursor();
			EditWrapToColumn(iWrapColumn);
			EndWaitCursor();
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
	case IDM_EDIT_TITLECASE:
	case IDM_EDIT_SENTENCECASE:
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
	case IDM_EDIT_MAP_HANJA_HANGUL:
		BeginWaitCursor();
		EditMapTextCase(LOWORD(wParam));
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTTABS:
	case IDM_EDIT_CONVERTTABS2:
		BeginWaitCursor();
		EditTabsToSpaces(fvCurFile.iTabWidth, LOWORD(wParam) == IDM_EDIT_CONVERTTABS2);
		EndWaitCursor();
		break;

	case IDM_EDIT_CONVERTSPACES:
	case IDM_EDIT_CONVERTSPACES2:
		BeginWaitCursor();
		EditSpacesToTabs(fvCurFile.iTabWidth, LOWORD(wParam) == IDM_EDIT_CONVERTSPACES2);
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
					static_cast<unsigned int>(guid.Data1), guid.Data2, guid.Data3,
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
	case IDM_INSERT_UNICODE_ALM:
		EditInsertUnicodeControlCharacter(LOWORD(wParam));
		break;

	case IDM_EDIT_INSERT_ENCODING: {
		char msz[32] = {'\0'};
		const char *enc = mEncoding[iCurrentEncoding].pszParseNames;
		const char *sep = strchr(enc, ',');
		if (sep != nullptr) {
			strncpy(msz, enc, min<size_t>(sep - enc, COUNTOF(msz) - 1));
		} else {
			WideCharToMultiByte(CP_UTF8, 0, mEncoding[iCurrentEncoding].wchLabel, -1, msz, COUNTOF(msz), nullptr, nullptr);
		}
		if (pLexCurrent->iLexer == SCLEX_PYTHON || pLexCurrent->iLexer == SCLEX_RUBY) {
			const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
			const Sci_Line iCurLine = SciCall_LineFromPosition(iCurrentPos);
			const Sci_Position iCurrentLinePos = iCurrentPos - SciCall_PositionFromLine(iCurLine);
			if (iCurLine < 2 && iCurrentLinePos == 0) {
				char cmsz[128];
				sprintf(cmsz, "#-*- coding: %s -*-", msz);
				SciCall_ReplaceSel(cmsz);
				break;
			}
		}
		SciCall_ReplaceSel(msz);
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
#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
		GetSystemTimePreciseAsFileTime(&ft);
#else
		GetSystemTimeAsFileTime(&ft);
#endif
		uint64_t timestamp = (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
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
			WideCharToMultiByte(cpEdit, 0, pszInsert, -1, mszBuf, COUNTOF(mszBuf), nullptr, nullptr);
			//const Sci_Position iSelStart = SciCall_GetSelectionStart();
			SciCall_ReplaceSel(mszBuf);
			//SciCall_SetAnchor(iSelStart);
		}
	}
	break;

	case IDM_EDIT_LINECOMMENT:
		EditToggleCommentLine(false);
		break;

	case IDM_EDIT_STREAMCOMMENT:
		EditToggleCommentBlock(false);
		break;

	case IDM_EDIT_URLENCODE:
	case IDM_EDIT_URLCOMPONENTENCODE:
		EditURLEncode(LOWORD(wParam) == IDM_EDIT_URLCOMPONENTENCODE);
		break;

	case IDM_EDIT_URLDECODE:
		EditURLDecode();
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

	case IDM_EDIT_COPYRTF:
	case IDM_EDIT_CODE_COMPRESS:
	case IDM_EDIT_CODE_PRETTY:
		BeginWaitCursor();
		EditFormatCode(LOWORD(wParam));
		EndWaitCursor();
		break;

	case IDM_EDIT_BASE64_ENCODE:
	case IDM_EDIT_BASE64_SAFE_ENCODE:
	case IDM_EDIT_BASE64_HTML_EMBEDDED_IMAGE:
		BeginWaitCursor();
		EditBase64Encode(static_cast<Base64EncodingFlag>(LOWORD(wParam) - IDM_EDIT_BASE64_ENCODE));
		EndWaitCursor();
		break;

	case IDM_EDIT_BASE64_DECODE:
	case IDM_EDIT_BASE64_DECODE_AS_HEX:
		BeginWaitCursor();
		EditBase64Decode(LOWORD(wParam) == IDM_EDIT_BASE64_DECODE_AS_HEX);
		EndWaitCursor();
		break;

	case IDM_EDIT_NUM2HEX:
	case IDM_EDIT_NUM2DEC:
	case IDM_EDIT_NUM2BIN:
	case IDM_EDIT_NUM2OCT:
		EditConvertNumRadix(LOWORD(wParam));
		break;

	case IDM_EDIT_FINDMATCHINGBRACE: {
		Sci_Position iBrace2 = INVALID_POSITION;
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
		if (iBrace2 >= 0) {
			SciCall_GotoPos(iBrace2);
		}
	}
	break;

	case IDM_EDIT_SELTOMATCHINGBRACE: {
		Sci_Position iBrace2 = INVALID_POSITION;
		Sci_Position iCurPos = SciCall_GetCurrentPos();
		Sci_Position iPos = iCurPos;
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
		if (iBrace2 >= 0) {
			Sci_Position iAnchorPos = SciCall_GetAnchor();
			const Sci_Position iMinPos = min(iAnchorPos, iCurPos);
			const Sci_Position iMaxPos = max(iAnchorPos, iCurPos);
			if (iBrace2 > iPos) {
				iAnchorPos = min(iPos, iMinPos);
				iCurPos = max(iBrace2 + 1, iMaxPos);
			} else {
				iAnchorPos = max(iPos + 1, iMaxPos);
				iCurPos = min(iBrace2, iMinPos);
			}
			SciCall_SetSel(iAnchorPos, iCurPos);
		}
	}
	break;

	// Main Bookmark Functions
	case BME_EDIT_BOOKMARKNEXT:
	case BME_EDIT_BOOKMARKPREV: {
		const Sci_Position iPos = SciCall_GetCurrentPos();
		const Sci_Line iLine = SciCall_LineFromPosition(iPos);

		Sci_Line iNextLine;
		if (LOWORD(wParam) == BME_EDIT_BOOKMARKNEXT) {
			iNextLine = SciCall_MarkerNext(iLine + 1, MarkerBitmask_Bookmark);
			if (iNextLine < 0) {
				iNextLine = SciCall_MarkerNext(0, MarkerBitmask_Bookmark);
			}
		} else {
			iNextLine = SciCall_MarkerPrevious(iLine - 1, MarkerBitmask_Bookmark);
			if (iNextLine < 0) {
				const Sci_Line iLines = SciCall_GetLineCount();
				iNextLine = SciCall_MarkerPrevious(iLines, MarkerBitmask_Bookmark);
			}
		}

		if (iNextLine >= 0) {
			editMarkAll.ignoreSelectionUpdate = true;
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

	case IDT_EDIT_FIND:
	case IDT_EDIT_REPLACE:
	case IDM_EDIT_FIND:
	case IDM_EDIT_REPLACE: {
		const bool bReplace = (LOWORD(wParam) == IDM_EDIT_REPLACE) || (LOWORD(wParam) == IDT_EDIT_REPLACE);
		if (!IsWindow(hDlgFindReplace)) {
			hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, bReplace);
		} else {
			if (bReplace != (GetDlgItem(hDlgFindReplace, IDC_REPLACETEXT) != nullptr)) {
				SendWMCommand(hDlgFindReplace, IDC_TOGGLEFINDREPLACE);
				DestroyWindow(hDlgFindReplace);
				hDlgFindReplace = EditFindReplaceDlg(hwndEdit, &efrData, bReplace);
			} else {
				SetForegroundWindow(hDlgFindReplace);
				SendMessage(hDlgFindReplace, APPM_COPYDATA, 0, 0);
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

		if (StrIsEmpty(efrData.szFind) && LOWORD(wParam) != IDM_EDIT_REPLACENEXT) {
			EditSaveSelectionAsFindText(&efrData, IDM_EDIT_SAVEFIND, false);
			if (StrIsEmpty(efrData.szFind)) {
				SendWMCommand(hwnd, IDM_EDIT_FIND);
				break;
			}
		}

		switch (LOWORD(wParam)) {
		case IDM_EDIT_FINDNEXT:
			EditFindNext(&efrData, false);
			break;

		case IDM_EDIT_FINDPREV:
			EditFindPrev(&efrData, false);
			break;

		case IDM_EDIT_REPLACENEXT:
			if (bReplaceInitialized && StrNotEmpty(efrData.szFind)) {
				EditReplace(hwndEdit, &efrData);
			} else {
				SendWMCommand(hwnd, IDM_EDIT_REPLACE);
			}
			break;

		case IDM_EDIT_SELTONEXT:
			EditFindNext(&efrData, true);
			break;

		case IDM_EDIT_SELTOPREV:
			EditFindPrev(&efrData, true);
			break;
		}
		break;

	case IDM_EDIT_SELTODOCEND:
		SciCall_SetSelectionEnd(SciCall_GetLength());
		break;
	case IDM_EDIT_SELTODOCSTART:
		SciCall_SetSelectionStart(0);
		break;

	case IDM_EDIT_COMPLETEWORD:
		EditCompleteWord(AutoCompleteCondition_Normal, true);
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

	case IDT_VIEW_SCHEME:
	case IDM_VIEW_SCHEME:
	case IDM_VIEW_SCHEME_FAVORITE:
		Style_SelectLexerDlg(hwndEdit, LOWORD(wParam) == IDM_VIEW_SCHEME_FAVORITE);
		break;

	case IDT_VIEW_SCHEMECONFIG:
	case IDM_VIEW_SCHEME_CONFIG:
		Style_ConfigDlg(hwndEdit);
		break;

	case IDM_VIEW_USEDEFAULT_CODESTYLE:
		Style_ToggleUseDefaultCodeStyle();
		break;

	case IDM_VIEW_STYLE_THEME_DEFAULT:
	case IDM_VIEW_STYLE_THEME_DARK:
		Style_OnStyleThemeChanged(LOWORD(wParam) - IDM_VIEW_STYLE_THEME_DEFAULT);
		break;

	case IDM_VIEW_DEFAULT_CODE_FONT:
	case IDM_VIEW_DEFAULT_TEXT_FONT:
		Style_SetDefaultFont(hwndEdit, LOWORD(wParam) == IDM_VIEW_DEFAULT_CODE_FONT);
		break;

	case IDT_VIEW_WORDWRAP:
	case IDM_VIEW_WORDWRAP: {
		fWordWrapG = fvCurFile.fWordWrap = !fvCurFile.fWordWrap;
		SciCall_SetWrapMode(fvCurFile.fWordWrap ? iWordWrapMode : SC_WRAP_NONE);
		UpdateToolbar();
	} break;

	case IDM_VIEW_WORDWRAPSETTINGS:
		if (WordWrapSettingsDlg(hwnd)) {
			SciCall_SetWrapMode(fvCurFile.fWordWrap ? iWordWrapMode : SC_WRAP_NONE);
			SciCall_SetMarginOptions(bWordWrapSelectSubLine ? SC_MARGINOPTION_SUBLINESELECT : SC_MARGINOPTION_NONE);
			EditSetWrapIndentMode(fvCurFile.iTabWidth, fvCurFile.iIndentWidth);
			SetWrapVisualFlags();
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
		if (LongLineSettingsDlg(hwnd)) {
			bMarkLongLines = true;
			SciCall_SetEdgeMode((iLongLineMode == EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND);
			Style_SetLongLineColors();
			SciCall_SetEdgeColumn(fvCurFile.iLongLinesLimit);
		}
		break;

	case IDM_VIEW_TABSASSPACES:
		SciCall_SetUseTabs(fvCurFile.bTabsAsSpaces);
		fvCurFile.mask |= FV_TABSASSPACES;
		fvCurFile.bTabsAsSpaces = !fvCurFile.bTabsAsSpaces;
		break;

	case IDM_VIEW_TABSETTINGS:
		if (TabSettingsDlg(hwnd)) {
			fvCurFile.Apply();
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

	case IDM_VIEW_CHANGE_HISTORY_MARKER:
		if (iChangeHistoryMarker != SC_CHANGE_HISTORY_DISABLED || !SciCall_CanUndo()) {
			iChangeHistoryMarker = (iChangeHistoryMarker == SC_CHANGE_HISTORY_DISABLED)? (SC_CHANGE_HISTORY_ENABLED | SC_CHANGE_HISTORY_MARKERS) : SC_CHANGE_HISTORY_DISABLED;
			UpdateBookmarkMarginWidth();
			SciCall_SetChangeHistory(iChangeHistoryMarker);
		}
		break;

	case IDM_VIEW_AUTOCOMPLETION_SETTINGS:
		if (AutoCompletionSettingsDlg(hwnd)) {
			if ((autoCompletionConfig.iCompleteOption & AutoCompletionOption_CompleteWord) == 0) {
				SciCall_AutoCCancel();
			}
		}
		break;

	case IDM_VIEW_AUTOCOMPLETION_IGNORECASE:
		autoCompletionConfig.bIgnoreCase = !autoCompletionConfig.bIgnoreCase;
		SciCall_AutoCCancel();
		break;

	case IDM_SET_LATEX_INPUT_METHOD:
		autoCompletionConfig.bLaTeXInputMethod = !autoCompletionConfig.bLaTeXInputMethod;
		break;

	case IDM_SET_MULTIPLE_SELECTION:
	case IDM_SET_SELECTIONASFINDTEXT:
	case IDM_SET_PASTEBUFFERASFINDTEXT: {
		const int option = 1 << (LOWORD(wParam) - IDM_SET_MULTIPLE_SELECTION);
		if (iSelectOption & option) {
			iSelectOption &= ~option;
		} else {
			iSelectOption |= option;
		}
		if (option == SelectOption_EnableMultipleSelection) {
			SciCall_SetMultipleSelection(iSelectOption & SelectOption_EnableMultipleSelection);
		}
	} break;

	case IDM_LINE_SELECTION_MODE_NONE:
	case IDM_LINE_SELECTION_MODE_VS:
	case IDM_LINE_SELECTION_MODE_NORMAL:
	case IDM_LINE_SELECTION_MODE_OLDVS:
		iLineSelectionMode = LOWORD(wParam) - IDM_LINE_SELECTION_MODE_NONE;
		if (iLineSelectionMode == LineSelectionMode_None) {
			SciCall_SetSelectionMode(SC_SEL_STREAM);
		}
		break;

	case IDM_VIEW_MARKOCCURRENCES_OFF:
	case IDM_VIEW_MARKOCCURRENCES_CASE:
	case IDM_VIEW_MARKOCCURRENCES_WORD:
	case IDM_VIEW_MARKOCCURRENCES_BOOKMARK: {
		const int mask = 1 << (LOWORD(wParam) - IDM_VIEW_MARKOCCURRENCES_OFF);
		if (bMarkOccurrences & mask) {
			bMarkOccurrences &= ~mask;
		} else {
			bMarkOccurrences |= mask;
		}
		if (bMarkOccurrences & MarkOccurrences_Enable) {
			editMarkAll.MarkAll(FALSE, bMarkOccurrences);
		} else {
			editMarkAll.Clear();
		}
		UpdateStatusbar();
	} break;

	case IDM_VIEW_SHOW_FOLDING:
		bShowCodeFolding = !bShowCodeFolding;
		UpdateFoldMarginWidth();
		UpdateToolbar();
		if (!bShowCodeFolding) {
			FoldToggleAll(FOLD_ACTION_EXPAND);
		}
		break;

	case IDT_VIEW_TOGGLEFOLDS:
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

	case IDM_VIEW_SHOWCALLTIP_OFF:
	case IDM_VIEW_SHOWCALLTIP_RGBA:
	case IDM_VIEW_SHOWCALLTIP_ARGB:
	case IDM_VIEW_SHOWCALLTIP_BGRA:
	case IDM_VIEW_SHOWCALLTIP_ABGR:
		callTipInfo.showCallTip = static_cast<ShowCallTip>(LOWORD(wParam) - IDM_VIEW_SHOWCALLTIP_OFF);
		SciCall_SetMouseDwellTime((callTipInfo.showCallTip == ShowCallTip_None)? SC_TIME_FOREVER : CallTipDefaultMouseDwellTime);
		break;

	case IDM_VIEW_MATCHBRACES:
		bMatchBraces = !bMatchBraces;
		if (bMatchBraces) {
			SCNotification scn;
			scn.nmhdr.hwndFrom = hwndEdit;
			scn.nmhdr.idFrom = IDC_EDIT;
			scn.nmhdr.code = SCN_UPDATEUI;
			scn.updated = SC_UPDATE_APP_CUSTOM;
			SendMessage(hwnd, WM_NOTIFY, IDC_EDIT, AsInteger<LPARAM>(&scn));
		} else {
			SciCall_BraceHighlight(INVALID_POSITION, INVALID_POSITION);
		}
		break;

	case IDM_VIEW_HIGHLIGHTCURRENT_BLOCK:
		bHighlightCurrentBlock = !bHighlightCurrentBlock;
		SciCall_MarkerEnableHighlight(bHighlightCurrentBlock);
		break;

	case IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE:
	case IDM_VIEW_HIGHLIGHTCURRENTLINE_BACK:
	case IDM_VIEW_HIGHLIGHTCURRENTLINE_FRAME:
		iHighlightCurrentLine = static_cast<LineHighlightMode>(LOWORD(wParam) - IDM_VIEW_HIGHLIGHTCURRENTLINE_NONE);
		Style_HighlightCurrentLine();
		break;

	case IDM_VIEW_HIGHLIGHTCURRENTLINE_SUBLINE:
		bHighlightCurrentSubLine = !bHighlightCurrentSubLine;
		SciCall_SetCaretLineHighlightSubLine(bHighlightCurrentSubLine);
		break;

	case IDT_VIEW_ZOOMIN:
	case IDM_VIEW_ZOOMIN:
		SciCall_ZoomIn();
		break;

	case IDT_VIEW_ZOOMOUT:
	case IDM_VIEW_ZOOMOUT:
		SciCall_ZoomOut();
		break;

	case IDM_VIEW_ZOOM_LEVEL:
		ZoomLevelDlg(hwnd, false);
		break;

	case IDM_VIEW_RESETZOOM:
		SciCall_SetZoom(100);
		break;

	case IDM_VIEW_MENU:
		bShowMenu = !bShowMenu;
		SetMenu(hwnd, bShowMenu ? hmenuMain : nullptr);
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
		iAutoScaleToolbar = iAutoScaleToolbar ? 0 : USER_DEFAULT_SCREEN_DPI;
		MsgThemeChanged(hwnd, 0, 0);
		break;

#if NP2_ENABLE_HIDPI_IMAGE_RESOURCE
	case IDM_VIEW_USE_LARGE_TOOLBAR:
		if (iAutoScaleToolbar >= USER_DEFAULT_SCREEN_DPI && iAutoScaleToolbar < USER_DEFAULT_SCREEN_DPI*2) {
			iAutoScaleToolbar += USER_DEFAULT_SCREEN_DPI/2;
		} else {
			iAutoScaleToolbar = USER_DEFAULT_SCREEN_DPI;
		}
		MsgThemeChanged(hwnd, 0, 0);
		break;
#endif

	case IDM_VIEW_STATUSBAR:
		bShowStatusbar = !bShowStatusbar;
		if (bShowStatusbar) {
			ShowWindow(hwndStatus, SW_SHOW);
		} else {
			ShowWindow(hwndStatus, SW_HIDE);
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_CLEARWINPOS:
		ClearWindowPositionHistory();
		break;

	case IDM_VIEW_STICKY_WINDOW_POSITION:
		if (!bStickyWindowPosition) {
			SaveSettingsNow(false, true);
		}
		bStickyWindowPosition = !bStickyWindowPosition;
		IniSetBoolEx(INI_SECTION_NAME_FLAGS, L"StickyWindowPosition", bStickyWindowPosition, false);
		break;

	case IDM_VIEW_REUSEWINDOW:
		bReuseWindow = !bReuseWindow;
		IniSetBoolEx(INI_SECTION_NAME_FLAGS, L"ReuseWindow", bReuseWindow, false);
		break;

	case IDM_VIEW_SINGLEFILEINSTANCE:
		bSingleFileInstance = !bSingleFileInstance;
		IniSetBoolEx(INI_SECTION_NAME_FLAGS, L"SingleFileInstance", bSingleFileInstance, true);
		break;

	case IDM_SET_USE_XP_FILE_DIALOG:
		bUseXPFileDialog = !bUseXPFileDialog;
		IniSetBoolEx(INI_SECTION_NAME_FLAGS, L"UseXPFileDialog", bUseXPFileDialog, false);
		break;

	case IDT_VIEW_ALWAYSONTOP:
	case IDM_VIEW_ALWAYSONTOP:
		if (IsTopMost()) {
			bAlwaysOnTop = false;
			flagAlwaysOnTop = TripleBoolean_NotSet;
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		} else {
			bAlwaysOnTop = true;
			flagAlwaysOnTop = TripleBoolean_NotSet;
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		UpdateToolbar();
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
		if (bEditLayoutRTL && iRenderingTechnology != SC_TECHNOLOGY_DEFAULT) {
			SendWMCommand(hwnd, IDM_SET_RENDER_TECH_GDI);
		}
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
		bBlockCaretForOVRMode = !bBlockCaretForOVRMode;
		Style_UpdateCaret();
		break;

	case IDM_VIEW_CARET_STYLE_BLOCK:
	case IDM_VIEW_CARET_STYLE_WIDTH1:
	case IDM_VIEW_CARET_STYLE_WIDTH2:
	case IDM_VIEW_CARET_STYLE_WIDTH3:
		iCaretStyle = static_cast<CaretStyle>(LOWORD(wParam) - IDM_VIEW_CARET_STYLE_BLOCK);
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
		iPathNameFormat = static_cast<TitlePathNameFormat>(LOWORD(wParam) - IDM_VIEW_SHOWFILENAMEONLY);
		StrCpyEx(szTitleExcerpt, L"");
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

	case IDM_SET_OPEN_FOLDER_MATEPATH:
		bOpenFolderWithMatepath = !bOpenFolderWithMatepath;
		break;

	case IDM_VIEW_CHANGENOTIFY:
		if (ChangeNotifyDlg(hwnd)) {
			InstallFileWatching(false);
		}
		break;

	case IDM_SET_FILE_AUTOSAVE: {
		const DWORD backup = dwAutoSavePeriod;
		if (AutoSaveSettingsDlg(hwnd)) {
			if (iAutoSaveOption & AutoSaveOption_Periodic) {
				AutoSave_Start(backup != dwAutoSavePeriod);
			} else {
				AutoSave_Stop(FALSE);
			}
		}
	} break;

	case IDM_VIEW_NOESCFUNC:
	case IDM_VIEW_ESCMINIMIZE:
	case IDM_VIEW_ESCEXIT:
		iEscFunction = static_cast<EscFunction>(LOWORD(wParam) - IDM_VIEW_NOESCFUNC);
		break;

	case IDM_VIEW_SAVESETTINGS:
		bSaveSettings = !bSaveSettings;
		break;

	case IDM_VIEW_SAVESETTINGSNOW:
		SaveSettingsNow(false, false);
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
	case IDM_VIEW_FULLSCREEN_HIDE_TITLE: {
		const int config = 1 << (LOWORD(wParam) - IDM_VIEW_FULLSCREEN_ON_START);
		if (iFullScreenMode & config) {
			iFullScreenMode &= ~config;
		} else {
			iFullScreenMode |= config;
		}
		if (config != FullScreenMode_OnStartup && bInFullScreenMode) {
			ToggleFullScreenMode();
		}
	} break;

	case CMD_ESCAPE:
		if (SciCall_AutoCActive()) {
			SciCall_AutoCCancel();
		} else if (SciCall_CallTipActive()) {
			SciCall_CallTipCancel();
		} else if (bInFullScreenMode) {
			bInFullScreenMode = false;
			ToggleFullScreenMode();
		} else if (iEscFunction == EscFunction_Minimize) {
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		} else if (iEscFunction == EscFunction_Exit) {
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

	case CMD_TAB_COMPLETION:
		HandleTabCompletion();
		break;

	case CMD_CTRLTAB:
		SciCall_SetTabIndents(false);
		SciCall_SetUseTabs(true);
		SciCall_Tab();
		SciCall_SetUseTabs(!fvCurFile.bTabsAsSpaces);
		SciCall_SetTabIndents(fvCurFile.bTabIndents);
		break;

	case CMD_RECODEDEFAULT:
		if (StrNotEmpty(szCurFile)) {
			iSrcEncoding = iDefaultEncoding;
			if (iSrcEncoding < CPI_UTF7) {
				constexpr UINT mask = (CPI_DEFAULT << 4*CPI_DEFAULT)
					| (CPI_OEM << 4*CPI_OEM)
					| (CPI_UNICODE << 4*CPI_UNICODEBOM)
					| (CPI_UNICODEBE << 4*CPI_UNICODEBEBOM)
					| (CPI_UNICODE << 4*CPI_UNICODE)
					| (CPI_UNICODEBE << 4*CPI_UNICODEBE)
					| (CPI_UTF8 << 4*CPI_UTF8)
					| (CPI_UTF8 << 4*CPI_UTF8SIGN);
				iSrcEncoding = (mask >> (4*iSrcEncoding)) & 15;
			}
			FileLoad(FileLoadFlag_Reload, szCurFile);
		}
		break;

	case CMD_RELOADANSI:
	case CMD_RELOADOEM:
	case CMD_RELOADUTF8:
		if (StrNotEmpty(szCurFile)) {
			iSrcEncoding = LOWORD(wParam) - CMD_RELOADANSI;
			if (iSrcEncoding > CPI_OEM) {
				iSrcEncoding = CPI_UTF8;
			}
			FileLoad(FileLoadFlag_Reload, szCurFile);
		}
		break;

	case CMD_RELOADNOFILEVARS:
		if (StrNotEmpty(szCurFile)) {
			const bool _fNoFileVariables = fNoFileVariables;
			const bool _bNoEncodingTags = bNoEncodingTags;
			fNoFileVariables = false;
			bNoEncodingTags = false;
			FileLoad(FileLoadFlag_Reload, szCurFile);
			fNoFileVariables = _fNoFileVariables;
			bNoEncodingTags = _bNoEncodingTags;
		}
		break;

#if defined(_WIN64)
	case IDM_FILE_LARGE_FILE_MODE_RELOAD:
		if (StrNotEmpty(szCurFile)) {
			bLargeFileMode = true;
			iSrcEncoding = iCurrentEncoding;
			FileLoad(FileLoadFlag_Reload, szCurFile);
			bLargeFileMode = (SciCall_GetDocumentOptions() & SC_DOCUMENTOPTION_TEXT_LARGE) != 0;
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
	case IDM_LANG_GERMAN:
	case IDM_LANG_ITALIAN:
	case IDM_LANG_KOREAN:
	case IDM_LANG_PORTUGUESE_BRAZIL:
	case IDM_LANG_FRENCH_FRANCE:
	case IDM_LANG_RUSSIAN:
	case IDM_LANG_POLISH:
		SetUILanguage(LOWORD(wParam));
		break;
#endif

	case CMD_TIMESTAMPS:
		EditUpdateTimestampMatchTemplate(hwndEdit);
		break;

	case CMD_OPEN_PATH_OR_LINK:
		EditOpenSelection(OpenSelectionType_None);
		break;
	case CMD_OPEN_CONTAINING_FOLDER:
		EditOpenSelection(OpenSelectionType_ContainingFolder);
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
	case IDM_EDIT_SAVEFIND:
		EditSaveSelectionAsFindText(&efrData, LOWORD(wParam), true);
		break;

	case CMD_INCLINELIMIT:
	case CMD_DECLINELIMIT:
		if (!bMarkLongLines) {
			SendWMCommand(hwnd, IDM_VIEW_LONGLINEMARKER);
		} else {
			if (LOWORD(wParam) == CMD_INCLINELIMIT) {
				iLongLinesLimitG++;
			} else {
				iLongLinesLimitG--;
			}
			fvCurFile.iLongLinesLimit = iLongLinesLimitG = clamp(iLongLinesLimitG, 0, NP2_LONG_LINE_LIMIT);
			SciCall_SetEdgeColumn(fvCurFile.iLongLinesLimit);
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
	case CMD_DECREASENUM:
		EditModifyNumber(LOWORD(wParam) == CMD_INCREASENUM);
		break;

	case CMD_JUMP2SELSTART:
	case CMD_JUMP2SELEND:
		if (!SciCall_IsRectangleSelection()) {
			const Sci_Position iAnchorPos = SciCall_GetAnchor();
			const Sci_Position iCursorPos = SciCall_GetCurrentPos();
			if ((LOWORD(wParam) == CMD_JUMP2SELSTART && iCursorPos > iAnchorPos) || (LOWORD(wParam) != CMD_JUMP2SELSTART && iCursorPos < iAnchorPos)) {
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
		GetWindowPlacement(hwnd, &wndpl);

		const int x = wndpl.rcNormalPosition.left;
		const int y = wndpl.rcNormalPosition.top;
		const int cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		const int cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		const BOOL maximized = IsZoomed(hwnd) || (wndpl.flags & WPF_RESTORETOMAXIMIZED);

		WCHAR wszWinPos[64];
		wsprintf(wszWinPos, L"/pos %i,%i,%i,%i,%i", x, y, cx, cy, maximized);

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
			FileLoad(FileLoadFlag_Default, szIniFile);
		}
		break;

	case IDM_SET_SYSTEM_INTEGRATION:
		SystemIntegrationDlg(hwnd);
		break;

	default: {
		if (LOWORD(wParam) >= IDM_LEXER_TEXTFILE && LOWORD(wParam) < IDM_LEXER_LEXER_COUNT) {
			Style_SetLexerByLangIndex(LOWORD(wParam));
			break;
		}

		const UINT index = LOWORD(wParam) - IDM_RECENT_HISTORY_START;
		if (index < MRU_MAXITEMS) {
			LPCWSTR path = mruFile.pszItems[index];
			if (path) {
				if (!PathIsFile(path)) {
					if (IDYES == MsgBoxWarn(MB_YESNO, IDS_ERR_MRUDLG)) {
						mruFile.DeleteFileFromStore(path);
						mruFile.Delete(index);
					}
				} else if (FileSave(FileSaveFlag_Ask)) {
					FileLoad(FileLoadFlag_DontSave, path);
				}
			}
		}
	} break;
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

	LPNMHDR pnmh = AsPointer<LPNMHDR>(lParam);
	const SCNotification * const scn = AsPointer<SCNotification *>(lParam);

	switch (pnmh->idFrom) {
	case IDC_EDIT:
		switch (pnmh->code) {
		case SCN_UPDATEUI:
			if (scn->updated & ~(SC_UPDATE_V_SCROLL | SC_UPDATE_H_SCROLL)) {
				UpdateToolbar();

				if (scn->updated & SC_UPDATE_SELECTION) {
					const int overType = scn->listType;
					cachedStatusItem.updateMask |= (1 << StatusItem_Character) | (1 << StatusItem_Column)
						| (1 << StatusItem_Selection) | (1 << StatusItem_SelectedLine);
					if (overType != cachedStatusItem.overType) {
						cachedStatusItem.updateMask |= (1 << StatusItem_OvrMode);
						cachedStatusItem.overType = overType;
						cachedStatusItem.pszOvrMode = overType ? L"OVR" : L"INS";
					}

					// mark occurrences of text currently selected
					if (editMarkAll.ignoreSelectionUpdate) {
						editMarkAll.ignoreSelectionUpdate = false;
					} else if (bMarkOccurrences & MarkOccurrences_Enable) {
						if (SciCall_IsSelectionEmpty()) {
							if (editMarkAll.matchCount) {
								editMarkAll.Clear();
							}
						} else {
							editMarkAll.MarkAll((scn->updated & SC_UPDATE_CONTENT), bMarkOccurrences);
						}
					}
				} else if (scn->updated & SC_UPDATE_CONTENT) {
					// cachedStatusItem.updateMask is already set in SCN_MODIFIED.
					if (editMarkAll.matchCount) {
						editMarkAll.MarkAll(TRUE, bMarkOccurrences);
					}
				}
				if (cachedStatusItem.updateMask) {
					UpdateStatusbar();
				}

				// Brace Match
				if (bMatchBraces) {
					Sci_Position iPos = SciCall_GetCurrentPos();
					int ch = SciCall_GetCharAt(iPos);
					if (IsBraceMatchChar(ch)) {
						const Sci_Position iBrace2 = SciCall_BraceMatch(iPos);
						if (iBrace2 >= 0) {
							const Sci_Position col1 = SciCall_GetColumn(iPos);
							const Sci_Position col2 = SciCall_GetColumn(iBrace2);
							SciCall_BraceHighlight(iPos, iBrace2);
							SciCall_SetHighlightGuide(min(col1, col2));
						} else {
							SciCall_BraceBadLight(iPos);
							SciCall_SetHighlightGuide(0);
						}
					} else { // Try one before
						iPos = SciCall_PositionBefore(iPos);
						ch = SciCall_GetCharAt(iPos);
						if (IsBraceMatchChar(ch)) {
							const Sci_Position iBrace2 = SciCall_BraceMatch(iPos);
							if (iBrace2 >= 0) {
								const Sci_Position col1 = SciCall_GetColumn(iPos);
								const Sci_Position col2 = SciCall_GetColumn(iBrace2);
								SciCall_BraceHighlight(iPos, iBrace2);
								SciCall_SetHighlightGuide(min(col1, col2));
							} else {
								SciCall_BraceBadLight(iPos);
								SciCall_SetHighlightGuide(0);
							}
						} else {
							SciCall_BraceHighlight(INVALID_POSITION, INVALID_POSITION);
							SciCall_SetHighlightGuide(0);
						}
					}
				}
			}
			break;

		case SCN_CHARADDED: {
			// fix cursor flash on typing when "Hide pointer while typing" is enabled
			// this prevents WM_SETCURSOR & WM_MOUSEMOVE send to Scintilla window
			DisableDelayedStatusBarRedraw();
			// ignore tentative input character and multiple selection, see Editor::InsertCharacter()
			if (scn->listType) {
				return 0;
			}
			const int ch = scn->ch;
			if (ch < 0x80) {
				// Auto indent
				if (ch == '\r' || ch == '\n') {
					// in CRLF mode handle LF only...
					if (autoCompletionConfig.bIndentText && ((SC_EOL_CRLF == iCurrentEOLMode && ch != '\n') || SC_EOL_CRLF != iCurrentEOLMode)) {
						EditAutoIndent();
					}
					return 0;
				}
				// Auto close tags
				if (ch == '>') {
					if (autoCompletionConfig.iCompleteOption & (AutoCompletionOption_CloseTags | AutoCompletionOption_CompleteWord)) {
						EditAutoCloseXMLTag();
					}
					return 0;
				}
				// Auto close braces/quotes, see GenerateAutoInsertMask() in tools/GenerateTable.py
				uint32_t index = ch - '\"';
				if (index == '{' - '\"' || (index < 63 && (UINT64_C(0x4200000004000461) & (UINT64_C(1) << index)))) {
					index = (index + (index >> 4)) & 15;
					index = (UINT64_C(0x102370000500064) >> (4*index)) & 15;
					if (autoCompletionConfig.fAutoInsertMask & (1U << index)) {
						EditAutoCloseBraceQuote(ch, static_cast<AutoInsertCharacter>(index));
					}
					return 0;
				}
			}

			// auto complete word
			if ((autoCompletionConfig.iCompleteOption & AutoCompletionOption_CompleteWord) == 0
				// ignore IME input
				|| (scn->characterSource != SC_CHARACTERSOURCE_DIRECT_INPUT && (ch >= 0x80 || (autoCompletionConfig.iCompleteOption & AutoCompletionOption_EnglishIMEModeOnly) != 0))
				|| !IsAutoCompletionWordCharacter(ch)
			) {
				return 0;
			}

			const int iCondition = SciCall_AutoCActive() ? AutoCompleteCondition_OnCharAdded : AutoCompleteCondition_Normal;
			EditCompleteWord(iCondition, false);
		}
		break;

		case SCN_AUTOCSELECTION:
		case SCN_USERLISTSELECTION: {
			if ((scn->listCompletionMethod == SC_AC_NEWLINE && !(autoCompletionConfig.fAutoCompleteFillUpMask & AutoCompleteFillUpMask_Enter))
			|| (scn->listCompletionMethod == SC_AC_TAB && !(autoCompletionConfig.fAutoCompleteFillUpMask & AutoCompleteFillUpMask_Tab))) {
				SciCall_AutoCCancel();
				if (scn->listCompletionMethod == SC_AC_NEWLINE) {
					SciCall_NewLine();
				} else {
					HandleTabCompletion();
				}
				return 0;
			}

			LPCSTR text = scn->text;
			// function/array/template/generic
			LPSTR braces = const_cast<LPSTR>(strpbrk(text, "([{<"));
			const Sci_Position iCurPos = SciCall_GetCurrentPos();
			Sci_Position offset;
			bool closeBrace = false;
			if (braces != nullptr) {
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
						if (SciCall_BraceMatchNext(iPos, SciCall_PositionBefore(iCurPos)) < 0) {
							braces[1] = L'\0'; // delete close brace
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
			if (scn->listCompletionMethod == SC_AC_TAB && autoCompletionConfig.bLaTeXInputMethod) {
				SciCall_TabCompletion(TAB_COMPLETION_LATEX);
			}
		}
		break;

		case SCN_AUTOCCHARDELETED:
			EditCompleteWord(AutoCompleteCondition_OnCharDeleted, false);
			break;

		case SCN_AUTOCCOMPLETED:
		case SCN_AUTOCCANCELLED:
			autoCompletionConfig.iPreviousItemCount = 0;
			break;

		case SCN_DWELLSTART:
			EditShowCallTip(scn->position);
			break;

		//case SCN_DWELLEND:
		//	break;

		case SCN_CALLTIPCLICK:
			EditClickCallTip(hwnd);
			break;

		case SCN_MODIFIED:
			// we only watch SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT
			++dwCurrentDocReversion;
			UpdateStatusBarCacheLineColumn();
			if (scn->linesAdded) {
				UpdateLineNumberWidth();
			}
			AutoSave_Start(false);
			break;

		case SCN_ZOOM:
			MsgNotifyZoom();
			break;

		case SCN_SAVEPOINTREACHED:
			bDocumentModified = false;
			UpdateDocumentModificationStatus();
			break;

		case SCN_MARGINCLICK:
			switch (scn->margin) {
#if 0
			case MarginNumber_CodeFolding:
				FoldClickAt(scn->position, scn->modifiers);
				break;
#endif
			case MarginNumber_Bookmark:
				if (bShowBookmarkMargin) {
					EditToggleBookmarkAt(scn->position);
				}
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
			bDocumentModified = true;
			UpdateDocumentModificationStatus();
			break;

		case SCN_CODEPAGECHANGED:
			EditOnCodePageChanged(scn->oldCodePage, bShowUnicodeControlCharacter, &efrData);
			break;

		case SCN_HOTSPOTCLICK:
			if ((scn->modifiers & SCMOD_CTRL) && (iSelectOption & SelectOption_EnableMultipleSelection)) {
				// disable multiple selection to avoid two carets after Ctrl + click
				SciCall_SetMultipleSelection(false);
				SciCall_SetSel(scn->position, scn->position);
				EditOpenSelection(OpenSelectionType_None);
				const Sci_Position iAnchorPos = SciCall_GetAnchor();
				const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
				PostMessage(hwnd, APPM_POST_HOTSPOTCLICK, iAnchorPos, iCurrentPos);
			}
			break;

#if 0
		case SCN_NEEDSHOWN: {
			const Sci_Line lineStart = SciCall_LineFromPosition(scn->position);
			const Sci_Line lineEnd = SciCall_LineFromPosition(scn->position + scn->length);
			//printf("SCN_NEEDSHOWN %zd %zd\n", lineStart, lineEnd);
			FoldExpandRange(lineStart, lineEnd);
		} break;
#endif
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
			LPTBNOTIFY lpTbNotify = AsPointer<LPTBNOTIFY>(lParam);
			if (static_cast<UINT>(lpTbNotify->iItem) < COUNTOF(tbbMainWnd)) {
				WCHAR tch[128];
				GetString(tbbMainWnd[lpTbNotify->iItem].idCommand, tch, COUNTOF(tch));
				lstrcpyn(lpTbNotify->pszText, tch, lpTbNotify->cchText);
				memcpy(&lpTbNotify->tbButton, &tbbMainWnd[lpTbNotify->iItem], sizeof(TBBUTTON));
				return TRUE;
			}
		}
		return FALSE;

		case TBN_RESET:
			Toolbar_SetButtons(hwndToolbar, DefaultToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd));
			return FALSE;

		case TBN_DROPDOWN: {
			LPTBNOTIFY lpTbNotify = AsPointer<LPTBNOTIFY>(lParam);
			HMENU hmenu = nullptr;
			HMENU subMenu = nullptr;
			if (lpTbNotify->iItem == IDT_FILE_OPEN) {
				static_assert(IDM_RECENT_HISTORY_START + MRU_MAXITEMS == IDM_RECENT_HISTORY_END);
				if (mruFile.iSize <= 0) {
					return TBDDRET_TREATPRESSED;
				}
				hmenu = subMenu = CreatePopupMenu();
				bitmapCache.StartUse();
				MENUITEMINFO mii;
				mii.cbSize = sizeof(MENUITEMINFO);
				mii.fMask = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
				const int count = min(mruFile.iSize, MRU_MAXITEMS);
				for (int i = 0; i < count; i++) {
					LPCWSTR path = mruFile.pszItems[i];
					HBITMAP hbmp = bitmapCache.Get(path);
					mii.wID = i + IDM_RECENT_HISTORY_START;
					mii.dwTypeData = const_cast<LPWSTR>(path);
					mii.hbmpItem = hbmp;
					InsertMenuItem(subMenu, i, TRUE, &mii);
				}
			} else {
				hmenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_POPUPMENU));
				subMenu = GetSubMenu(hmenu, IDP_POPUP_SUBMENU_FOLD);
			}
			TPMPARAMS tpm;
			tpm.cbSize = sizeof(TPMPARAMS);
			SendMessage(hwndToolbar, TB_GETRECT, lpTbNotify->iItem, AsInteger<LPARAM>(&tpm.rcExclude));
			MapWindowPoints(hwndToolbar, HWND_DESKTOP, reinterpret_cast<LPPOINT>(&tpm.rcExclude), 2);
			TrackPopupMenuEx(subMenu,
							TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
							tpm.rcExclude.left, tpm.rcExclude.bottom, hwnd, &tpm);
			DestroyMenu(hmenu);
		}
		return FALSE;
		}
		break;

	case IDC_STATUSBAR:
		switch (pnmh->code) {
		case NM_CLICK: {
			LPNMMOUSE pnmm = AsPointer<LPNMMOUSE>(lParam);
			switch (pnmm->dwItemSpec) {
			case StatusItem_EolMode:
				EditEnsureConsistentLineEndings();
				return TRUE;

			case StatusItem_Zoom:
				ZoomLevelDlg(hwnd, true);
				return TRUE;

			default:
				return FALSE;
			}
		}

		case NM_DBLCLK: {
			LPNMMOUSE pnmm = AsPointer<LPNMMOUSE>(lParam);
			switch (pnmm->dwItemSpec) {
			case StatusItem_Line:
				EditLineNumDlg(hwndEdit);
				return TRUE;

			case StatusItem_Find:
				SendWMCommand(hwnd, IDM_EDIT_FIND);
				return TRUE;

			case StatusItem_Encoding:
				SendWMCommand(hwnd, IDM_ENCODING_SELECT);
				return TRUE;

			case StatusItem_EolMode: {
				constexpr UINT mask = (SC_EOL_LF << 2*SC_EOL_CRLF) | (SC_EOL_CR << 2*SC_EOL_LF) | (SC_EOL_CRLF << 2*SC_EOL_CR);
				const int iNewEOLMode = (mask >> (iCurrentEOLMode << 1)) & 3;
				ConvertLineEndings(iNewEOLMode);
				return TRUE;
			}

			case StatusItem_Lexer:
				SendWMCommand(hwnd, (pLexCurrent->iLexer == SCLEX_CSV) ? IDM_LEXER_CSV : IDM_VIEW_SCHEME);
				return TRUE;

			case StatusItem_OvrMode:
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
			LPTOOLTIPTEXT pTTT = AsPointer<LPTOOLTIPTEXT>(lParam);
			if (pTTT->uFlags & TTF_IDISHWND) {
				//nop;
			} else {
				WCHAR tch[128];
				GetString(static_cast<UINT>(pnmh->idFrom), tch, COUNTOF(tch));
				lstrcpyn(pTTT->szText, tch, COUNTOF(pTTT->szText));
			}
		}
		break;

		}
		break;
	}

	return 0;
}

static void GetWindowPositionSectionName(HMONITOR hMonitor, WCHAR (&sectionName)[96]) noexcept {
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
void LoadSettings() noexcept {
	IniSectionParser section;
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS));
	const DWORD cchIniSection = static_cast<DWORD>(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	section.Init(128);

	LoadIniSection(INI_SECTION_NAME_SETTINGS, pIniSectionBuf, cchIniSection);
	section.Parse(pIniSectionBuf);
	bSaveSettings = section.GetBool(L"SaveSettings", true);

	// TODO: sort loading order by item frequency to reduce UnsafeGetValue() calls
	LPCWSTR strValue = section.GetValue(L"OpenWithDir");
	if (StrIsEmpty(strValue)) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		LPWSTR pszPath = nullptr;
		if (S_OK == SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_DEFAULT, nullptr, &pszPath)) {
			lstrcpy(tchOpenWithDir, pszPath);
			CoTaskMemFree(pszPath);
		}
#else
		SHGetFolderPath(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_CURRENT, tchOpenWithDir);
#endif
	} else {
		PathAbsoluteFromApp(strValue, tchOpenWithDir, true);
	}

	strValue = section.GetValue(L"Favorites");
	if (StrIsEmpty(strValue)) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		LPWSTR pszPath = nullptr;
		if (S_OK == SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &pszPath)) {
			lstrcpy(tchFavoritesDir, pszPath);
			CoTaskMemFree(pszPath);
		}
#else
		SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, tchFavoritesDir);
#endif
	} else {
		PathAbsoluteFromApp(strValue, tchFavoritesDir, true);
	}

	int iValue = section.GetInt(L"PathNameFormat", TitlePathNameFormat_NameFirst);
	iPathNameFormat = clamp(static_cast<TitlePathNameFormat>(iValue), TitlePathNameFormat_NameOnly, TitlePathNameFormat_FullPath);

	POINT pt;
	pt.x = section.GetInt(L"WindowPosX", 0);
	pt.y = section.GetInt(L"WindowPosY", 0);

	iValue = section.GetInt(L"SaveRecentFiles", MRU_MAXITEMS << 1);
	bSaveRecentFiles = iValue & true;
	iMaxRecentFiles = max(iValue >> 1, MRU_MAXITEMS);
	bSaveFindReplace = section.GetBool(L"SaveFindReplace", false);
	iValue = section.GetInt(L"FindReplaceOption", FindReplaceOption_Default);
	iFindReplaceOption = iValue & 15;
	efrData.option = iValue >> 4;
	if (bSaveFindReplace) {
		iValue = section.GetInt(L"FindReplaceFlag", SCFIND_NONE);
		efrData.fuFlags = iValue & 1023;
		efrData.option |= iValue >> 10;
	}

	fWordWrapG = section.GetBool(L"WordWrap", true);
	iValue = section.GetInt(L"WordWrapMode", SC_WRAP_AUTO);
	iWordWrapMode = clamp(iValue, SC_WRAP_WORD, SC_WRAP_AUTO);

	iValue = section.GetInt(L"WordWrapIndent", EditWrapIndent_DefaultValue);
	iWordWrapIndent = clamp<int>(iValue, EditWrapIndent_None, EditWrapIndent_MaxValue);

	iValue = section.GetInt(L"WordWrapSymbols", EditWrapSymbol_DefaultValue);
	iValue = clamp<int>(iValue, 0, EditWrapSymbol_MaxValue);
	iWordWrapSymbols = clamp<int>(iValue % 10, EditWrapSymbolBefore_None, EditWrapSymbolBefore_MaxValue) + (iValue / 10) * 10;

	bShowWordWrapSymbols = section.GetBool(L"ShowWordWrapSymbols", false);
	bWordWrapSelectSubLine = section.GetBool(L"WordWrapSelectSubLine", false);
	bShowUnicodeControlCharacter = section.GetBool(L"ShowUnicodeControlCharacter", false);

	bMatchBraces = section.GetBool(L"MatchBraces", true);
	bHighlightCurrentBlock = section.GetBool(L"HighlightCurrentBlock", true);
	iValue = section.GetInt(L"HighlightCurrentLine", 10 + LineHighlightMode_OutlineFrame);
	bHighlightCurrentSubLine = (iValue >= 10);
	iHighlightCurrentLine = clamp(static_cast<LineHighlightMode>(iValue % 10), LineHighlightMode_None, LineHighlightMode_OutlineFrame);
	bShowIndentGuides = section.GetBool(L"ShowIndentGuides", false);

	autoCompletionConfig.bIndentText = section.GetBool(L"AutoIndent", true);
	autoCompletionConfig.iCompleteOption = section.GetInt(L"AutoCompleteOption", AutoCompletionOption_Default);
	iValue = section.GetInt(L"AutoCompleteScope", AutoCompleteScope_Default);
	autoCompletionConfig.fCompleteScope = iValue & 15;
	autoCompletionConfig.fScanWordScope = iValue >> 4;
	iValue = section.GetInt(L"AutoCScanWordsTimeout", AUTOC_SCAN_WORDS_DEFAULT_TIMEOUT);
	autoCompletionConfig.dwScanWordsTimeout = max(iValue, AUTOC_SCAN_WORDS_MIN_TIMEOUT);
	autoCompletionConfig.bIgnoreCase = section.GetBool(L"AutoCIgnoreCase", false);
	autoCompletionConfig.bLaTeXInputMethod = section.GetBool(L"LaTeXInputMethod", false);
	iValue = section.GetInt(L"AutoCVisibleItemCount", 16);
	autoCompletionConfig.iVisibleItemCount = max(iValue, MIN_AUTO_COMPLETION_VISIBLE_ITEM_COUNT);
	iValue = section.GetInt(L"AutoCMinWordLength", 1);
	autoCompletionConfig.iMinWordLength = max(iValue, MIN_AUTO_COMPLETION_WORD_LENGTH);
	iValue = section.GetInt(L"AutoCMinNumberLength", 3);
	autoCompletionConfig.iMinNumberLength = max(iValue, MIN_AUTO_COMPLETION_NUMBER_LENGTH);
	autoCompletionConfig.fAutoCompleteFillUpMask = section.GetInt(L"AutoCFillUpMask", AutoCompleteFillUpMask_Default);
	autoCompletionConfig.fAutoInsertMask = section.GetInt(L"AutoInsertMask", AutoInsertMask_Default);
	iValue = section.GetInt(L"AsmLineCommentChar", AsmLineCommentChar_Semicolon);
	autoCompletionConfig.iAsmLineCommentChar = clamp<int>(iValue, AsmLineCommentChar_Semicolon, AsmLineCommentChar_At);
	strValue = section.GetValue(L"AutoCFillUpPunctuation");
	if (StrIsEmpty(strValue)) {
		lstrcpy(autoCompletionConfig.wszAutoCompleteFillUp, AUTO_COMPLETION_FILLUP_DEFAULT);
	} else {
		lstrcpyn(autoCompletionConfig.wszAutoCompleteFillUp, strValue, COUNTOF(autoCompletionConfig.wszAutoCompleteFillUp));
	}
	EditCompleteUpdateConfig();

	iSelectOption = section.GetInt(L"SelectOption", SelectOption_Default);
	iLineSelectionMode = section.GetInt(L"LineSelection", LineSelectionMode_VisualStudio);

	iValue = section.GetInt(L"TabWidth", TAB_WIDTH_4);
	tabSettings.globalTabWidth = clamp(iValue, TAB_WIDTH_MIN, TAB_WIDTH_MAX);
	iValue = section.GetInt(L"IndentWidth", INDENT_WIDTH_4);
	tabSettings.globalIndentWidth = clamp(iValue, INDENT_WIDTH_MIN, INDENT_WIDTH_MAX);
	tabSettings.globalTabsAsSpaces = section.GetBool(L"TabsAsSpaces", false);
	tabSettings.bTabIndents = section.GetBool(L"TabIndents", true);
	tabSettings.bBackspaceUnindents = static_cast<uint8_t>(section.GetInt(L"BackspaceUnindents", 2));
	tabSettings.bDetectIndentation = section.GetBool(L"DetectIndentation", true);
	// for toolbar state
	fvCurFile.bTabsAsSpaces = tabSettings.globalTabsAsSpaces;
	fvCurFile.fWordWrap = fWordWrapG;

	bMarkLongLines = section.GetBool(L"MarkLongLines", false);
	iValue = section.GetInt(L"LongLinesLimit", 80);
	iLongLinesLimitG = clamp(iValue, 0, NP2_LONG_LINE_LIMIT);
	iValue = section.GetInt(L"LongLineMode", EDGE_LINE);
	iLongLineMode = clamp(iValue, EDGE_LINE, EDGE_BACKGROUND);

	iValue = section.GetInt(L"ZoomLevel", 100);
	iZoomLevel = clamp(iValue, SC_MIN_ZOOM_LEVEL, SC_MAX_ZOOM_LEVEL);

	bShowBookmarkMargin = section.GetBool(L"ShowBookmarkMargin", false);
	bShowLineNumbers = section.GetBool(L"ShowLineNumbers", true);
	bShowCodeFolding = section.GetBool(L"ShowCodeFolding", true);
	iChangeHistoryMarker = section.GetInt(L"ChangeHistoryMarker", SC_CHANGE_HISTORY_DISABLED);
	bMarkOccurrences = section.GetInt(L"MarkOccurrences", MarkOccurrences_Enable);

	bViewWhiteSpace = section.GetBool(L"ViewWhiteSpace", false);
	bViewEOLs = section.GetBool(L"ViewEOLs", false);
	callTipInfo.showCallTip = static_cast<ShowCallTip>(section.GetInt(L"ShowCallTip", ShowCallTip_None));

	iValue = section.GetInt(L"DefaultEncoding", -1);
	iDefaultEncoding = Encoding_MapIniSetting(true, iValue);
	bSkipUnicodeDetection = section.GetBool(L"SkipUnicodeDetection", true);
	bLoadANSIasUTF8 = section.GetBool(L"LoadANSIasUTF8", true);
	bLoadASCIIasUTF8 = section.GetBool(L"LoadASCIIasUTF8", true);
	bLoadNFOasOEM = section.GetBool(L"LoadNFOasOEM", true);
	bNoEncodingTags = section.GetBool(L"NoEncodingTags", false);

	iValue = section.GetInt(L"DefaultEOLMode", 0);
	iDefaultEOLMode = clamp(iValue, SC_EOL_CRLF, SC_EOL_LF);

	bWarnLineEndings = section.GetBool(L"WarnLineEndings", true);
	bFixLineEndings = section.GetBool(L"FixLineEndings", false);
	bAutoStripBlanks = section.GetBool(L"FixTrailingBlanks", false);

	iValue = section.GetInt(L"PrintHeader", PrintHeaderOption_FilenameAndDate);
	iPrintHeader = clamp(static_cast<PrintHeaderOption>(iValue), PrintHeaderOption_FilenameAndDateTime, PrintHeaderOption_LeaveBlank);

	iValue = section.GetInt(L"PrintFooter", PrintFooterOption_PageNumber);
	iPrintFooter = clamp(static_cast<PrintFooterOption>(iValue), PrintFooterOption_PageNumber, PrintFooterOption_LeaveBlank);

	iValue = section.GetInt(L"PrintColorMode", SC_PRINT_COLOURONWHITE);
	iPrintColor = clamp(iValue, SC_PRINT_NORMAL, SC_PRINT_SCREENCOLOURS);

	iValue = section.GetInt(L"PrintZoom", 100);
	iPrintZoom = clamp(iValue, SC_MIN_ZOOM_LEVEL, SC_MAX_ZOOM_LEVEL);

	iValue = section.GetInt(L"PrintMarginLeft", -1);
	pageSetupMargin.left = max(iValue, -1);

	iValue = section.GetInt(L"PrintMarginTop", -1);
	pageSetupMargin.top = max(iValue, -1);

	iValue = section.GetInt(L"PrintMarginRight", -1);
	pageSetupMargin.right = max(iValue, -1);

	iValue = section.GetInt(L"PrintMarginBottom", -1);
	pageSetupMargin.bottom = max(iValue, -1);

	bSaveBeforeRunningTools = section.GetBool(L"SaveBeforeRunningTools", false);
	bOpenFolderWithMatepath = section.GetBool(L"OpenFolderWithMatepath", true);

	iValue = section.GetInt(L"FileWatchingMode", FileWatchingMode_AutoReload);
	iFileWatchingMode = clamp(static_cast<FileWatchingMode>(iValue), FileWatchingMode_None, FileWatchingMode_AutoReload);
	iFileWatchingOption = section.GetInt(L"FileWatchingOption", FileWatchingOption_None);
	bResetFileWatching = section.GetBool(L"ResetFileWatching", false);

	iAutoSaveOption = section.GetInt(L"AutoSaveOption", AutoSaveOption_Default);
	dwAutoSavePeriod = section.GetInt(L"AutoSavePeriod", AutoSaveDefaultPeriod);

	iValue = section.GetInt(L"EscFunction", EscFunction_None);
	iEscFunction = clamp(static_cast<EscFunction>(iValue), EscFunction_None, EscFunction_Exit);

	bAlwaysOnTop = section.GetBool(L"AlwaysOnTop", false);
	bMinimizeToTray = section.GetBool(L"MinimizeToTray", false);
	bTransparentMode = section.GetBool(L"TransparentMode", false);
	iValue = section.GetInt(L"EndAtLastLine", 1);
	iEndAtLastLine = clamp(iValue, 0, 4);
	bEditLayoutRTL = section.GetBool(L"EditLayoutRTL", false);
	bWindowLayoutRTL = section.GetBool(L"WindowLayoutRTL", false);

	iValue = section.GetInt(L"RenderingTechnology", GetDefualtRenderingTechnology());
	iValue = clamp(iValue, SC_TECHNOLOGY_DEFAULT, SC_TECHNOLOGY_DIRECTWRITEDC);
	iRenderingTechnology = bEditLayoutRTL ? SC_TECHNOLOGY_DEFAULT : iValue;

	iValue = section.GetInt(L"Bidirectional", SC_BIDIRECTIONAL_DISABLED);
	iBidirectional = clamp(iValue, SC_BIDIRECTIONAL_DISABLED, SC_BIDIRECTIONAL_R2L);

	iValue = section.GetInt(L"FontQuality", SC_EFF_QUALITY_LCD_OPTIMIZED);
	iFontQuality = clamp(iValue, SC_EFF_QUALITY_DEFAULT, SC_EFF_QUALITY_LCD_OPTIMIZED);

	iValue = section.GetInt(L"CaretStyle", CaretStyle_LineWidth1);
	bBlockCaretOutSelection = (iValue >= 100);
	iValue %= 100;
	bBlockCaretForOVRMode = (iValue >= 10);
	iCaretStyle = clamp(static_cast<CaretStyle>(iValue % 10), CaretStyle_Block, CaretStyle_LineWidth3);
	iCaretBlinkPeriod = section.GetInt(L"CaretBlinkPeriod", -1);
	bUseInlineIME = section.GetBool(L"UseInlineIME", false);

	strValue = section.GetValue(L"ToolbarButtons");
	if (StrIsEmpty(strValue)) {
		memcpy(tchToolbarButtons, DefaultToolbarButtons, sizeof(DefaultToolbarButtons));
	} else {
		lstrcpyn(tchToolbarButtons, strValue, COUNTOF(tchToolbarButtons));
	}

	bShowMenu = section.GetBool(L"ShowMenu", true);
	bShowToolbar = section.GetBool(L"ShowToolbar", true);
	iAutoScaleToolbar = section.GetInt(L"AutoScaleToolbar", USER_DEFAULT_SCREEN_DPI);
	bShowStatusbar = section.GetBool(L"ShowStatusbar", true);

	iValue = section.GetInt(L"FullScreenMode", FullScreenMode_Default);
	iFullScreenMode = iValue;
	bInFullScreenMode = iValue & FullScreenMode_OnStartup;

	// window position section
	{
		WCHAR sectionName[96];
		HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
		GetWindowPositionSectionName(hMonitor, sectionName);
		LoadIniSection(sectionName, pIniSectionBuf, cchIniSection);
		section.Parse(pIniSectionBuf);

		// ignore window position if /p was specified
		if (!flagPosParam) {
			wi.x	= section.GetInt(L"WindowPosX", CW_USEDEFAULT);
			wi.y	= section.GetInt(L"WindowPosY", CW_USEDEFAULT);
			wi.cx	= section.GetInt(L"WindowSizeX", CW_USEDEFAULT);
			wi.cy	= section.GetInt(L"WindowSizeY", CW_USEDEFAULT);
			wi.max	= section.GetBool(L"WindowMaximized", false);
		}

		cxRunDlg = section.GetInt(L"RunDlgSizeX", 0);
		cxEncodingDlg = section.GetInt(L"EncodingDlgSizeX", 0);
		cyEncodingDlg = section.GetInt(L"EncodingDlgSizeY", 0);

		cxFileMRUDlg = section.GetInt(L"FileMRUDlgSizeX", 0);
		cyFileMRUDlg = section.GetInt(L"FileMRUDlgSizeY", 0);
		cxOpenWithDlg = section.GetInt(L"OpenWithDlgSizeX", 0);
		cyOpenWithDlg = section.GetInt(L"OpenWithDlgSizeY", 0);
		cxFavoritesDlg = section.GetInt(L"FavoritesDlgSizeX", 0);
		cyFavoritesDlg = section.GetInt(L"FavoritesDlgSizeY", 0);
		cxAddFavoritesDlg = section.GetInt(L"AddFavoritesDlgSizeX", 0);

		cxModifyLinesDlg = section.GetInt(L"ModifyLinesDlgSizeX", 0);
		cyModifyLinesDlg = section.GetInt(L"ModifyLinesDlgSizeY", 0);
		cxEncloseSelectionDlg = section.GetInt(L"EncloseSelectionDlgSizeX", 0);
		cyEncloseSelectionDlg = section.GetInt(L"EncloseSelectionDlgSizeY", 0);
		cxInsertTagDlg = section.GetInt(L"InsertTagDlgSizeX", 0);
		cyInsertTagDlg = section.GetInt(L"InsertTagDlgSizeY", 0);

		xFindReplaceDlg = section.GetInt(L"FindReplaceDlgPosX", 0);
		yFindReplaceDlg = section.GetInt(L"FindReplaceDlgPosY", 0);
		cxFindReplaceDlg = section.GetInt(L"FindReplaceDlgSizeX", 0);

		cxStyleSelectDlg = section.GetInt(L"StyleSelectDlgSizeX", 0);
		cyStyleSelectDlg = section.GetInt(L"StyleSelectDlgSizeY", 0);
		cxStyleCustomizeDlg = section.GetInt(L"StyleCustomizeDlgSizeX", 0);
		cyStyleCustomizeDlg = section.GetInt(L"StyleCustomizeDlgSizeY", 0);
	}

	section.Free();
	NP2HeapFree(pIniSectionBuf);

	// Scintilla Styles
	Style_Load();
}

void SaveSettingsNow(bool bOnlySaveStyle, bool bQuiet) noexcept {
	bool bCreateFailure = false;

	if (StrIsEmpty(szIniFile)) {
		if (StrNotEmpty(szIniFile2)) {
			if (CreateIniFile(szIniFile2)) {
				lstrcpy(szIniFile, szIniFile2);
				StrCpyEx(szIniFile2, L"");
			} else {
				bCreateFailure = true;
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
			InvalidateRect(hwndStatus, nullptr, TRUE);
			UpdateWindow(hwndStatus);
			if (CreateIniFile(szIniFile)) {
				if (bOnlySaveStyle) {
					Style_Save();
				} else {
					SaveSettings(true);
				}
			} else {
				bCreateFailure = true;
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
void SaveSettings(bool bSaveSettingsNow) noexcept {
	if (!CreateIniFile(szIniFile)) {
		return;
	}

	if (!bSaveSettings && !bSaveSettingsNow) {
		IniSetBoolEx(INI_SECTION_NAME_SETTINGS, L"SaveSettings", bSaveSettings, true);
		return;
	}

	WCHAR wchTmp[MAX_PATH];
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS));
	if (!bStickyWindowPosition) {
		SaveWindowPosition(pIniSectionBuf);
		memset(pIniSectionBuf, 0, 2*sizeof(WCHAR));
	}

	IniSectionBuilder section = { pIniSectionBuf };
	section.SetBoolEx(L"SaveSettings", bSaveSettings, true);

	PathRelativeToApp(tchOpenWithDir, wchTmp, FILE_ATTRIBUTE_DIRECTORY, true, flagPortableMyDocs);
	section.SetString(L"OpenWithDir", wchTmp);
	PathRelativeToApp(tchFavoritesDir, wchTmp, FILE_ATTRIBUTE_DIRECTORY, true, flagPortableMyDocs);
	section.SetString(L"Favorites", wchTmp);
	section.SetIntEx(L"PathNameFormat", static_cast<int>(iPathNameFormat), TitlePathNameFormat_NameFirst);
	if (!bStickyWindowPosition) {
		section.SetInt(L"WindowPosX", wi.x);
		section.SetInt(L"WindowPosY", wi.y);
	}

	int iValue = (iMaxRecentFiles << 1) | static_cast<int>(bSaveRecentFiles);
	section.SetIntEx(L"SaveRecentFiles", iValue, MRU_MAXITEMS << 1);
	section.SetBoolEx(L"SaveFindReplace", bSaveFindReplace, false);
	iValue = iFindReplaceOption | ((efrData.option & FindReplaceOption_BehaviorMask) << 4);
	section.SetIntEx(L"FindReplaceOption", iValue, FindReplaceOption_Default);
	if (bSaveFindReplace) {
		iValue = efrData.fuFlags | ((efrData.option & FindReplaceOption_SearchMask) << 10);
		section.SetIntEx(L"FindReplaceFlag", iValue, SCFIND_NONE);
	}

	section.SetBoolEx(L"WordWrap", fWordWrapG, true);
	section.SetIntEx(L"WordWrapMode", iWordWrapMode, SC_WRAP_AUTO);
	section.SetIntEx(L"WordWrapIndent", iWordWrapIndent, EditWrapIndent_None);
	section.SetIntEx(L"WordWrapSymbols", iWordWrapSymbols, EditWrapSymbol_DefaultValue);
	section.SetBoolEx(L"ShowWordWrapSymbols", bShowWordWrapSymbols, false);
	section.SetBoolEx(L"WordWrapSelectSubLine", bWordWrapSelectSubLine, false);
	section.SetBoolEx(L"ShowUnicodeControlCharacter", bShowUnicodeControlCharacter, false);
	section.SetBoolEx(L"MatchBraces", bMatchBraces, true);
	section.SetBoolEx(L"HighlightCurrentBlock", bHighlightCurrentBlock, true);
	iValue = static_cast<int>(iHighlightCurrentLine) + (static_cast<int>(bHighlightCurrentSubLine)*10);
	section.SetIntEx(L"HighlightCurrentLine", iValue, 10 + LineHighlightMode_OutlineFrame);
	section.SetBoolEx(L"ShowIndentGuides", bShowIndentGuides, false);

	section.SetBoolEx(L"AutoIndent", autoCompletionConfig.bIndentText, true);
	section.SetIntEx(L"AutoCompleteOption", autoCompletionConfig.iCompleteOption, AutoCompletionOption_Default);
	iValue = autoCompletionConfig.fCompleteScope | (autoCompletionConfig.fScanWordScope << 4);
	section.SetIntEx(L"AutoCompleteScope", iValue, AutoCompleteScope_Default);
	section.SetIntEx(L"AutoCScanWordsTimeout", autoCompletionConfig.dwScanWordsTimeout, AUTOC_SCAN_WORDS_DEFAULT_TIMEOUT);
	section.SetBoolEx(L"AutoCIgnoreCase", autoCompletionConfig.bIgnoreCase, false);
	section.SetBoolEx(L"LaTeXInputMethod", autoCompletionConfig.bLaTeXInputMethod, false);
	section.SetIntEx(L"AutoCVisibleItemCount", autoCompletionConfig.iVisibleItemCount, 16);
	section.SetIntEx(L"AutoCMinWordLength", autoCompletionConfig.iMinWordLength, 1);
	section.SetIntEx(L"AutoCMinNumberLength", autoCompletionConfig.iMinNumberLength, 3);
	section.SetIntEx(L"AutoCFillUpMask", autoCompletionConfig.fAutoCompleteFillUpMask, AutoCompleteFillUpMask_Default);
	section.SetIntEx(L"AutoInsertMask", autoCompletionConfig.fAutoInsertMask, AutoInsertMask_Default);
	section.SetIntEx(L"AsmLineCommentChar", autoCompletionConfig.iAsmLineCommentChar, AsmLineCommentChar_Semicolon);
	section.SetStringEx(L"AutoCFillUpPunctuation", autoCompletionConfig.wszAutoCompleteFillUp, AUTO_COMPLETION_FILLUP_DEFAULT);
	section.SetIntEx(L"SelectOption", iSelectOption, SelectOption_Default);
	section.SetIntEx(L"LineSelection", iLineSelectionMode, LineSelectionMode_VisualStudio);
	section.SetIntEx(L"TabWidth", tabSettings.globalTabWidth, TAB_WIDTH_4);
	section.SetIntEx(L"IndentWidth", tabSettings.globalIndentWidth, INDENT_WIDTH_4);
	section.SetBoolEx(L"TabsAsSpaces", tabSettings.globalTabsAsSpaces, false);
	section.SetBoolEx(L"TabIndents", tabSettings.bTabIndents, true);
	section.SetIntEx(L"BackspaceUnindents", tabSettings.bBackspaceUnindents, 2);
	section.SetBoolEx(L"DetectIndentation", tabSettings.bDetectIndentation, true);
	section.SetBoolEx(L"MarkLongLines", bMarkLongLines, false);
	section.SetIntEx(L"LongLinesLimit", iLongLinesLimitG, 80);
	section.SetIntEx(L"LongLineMode", iLongLineMode, EDGE_LINE);
	section.SetIntEx(L"ZoomLevel", iZoomLevel, 100);
	section.SetBoolEx(L"ShowBookmarkMargin", bShowBookmarkMargin, false);
	section.SetBoolEx(L"ShowLineNumbers", bShowLineNumbers, true);
	section.SetBoolEx(L"ShowCodeFolding", bShowCodeFolding, true);
	section.SetIntEx(L"ChangeHistoryMarker", iChangeHistoryMarker, SC_CHANGE_HISTORY_DISABLED);
	section.SetIntEx(L"MarkOccurrences", bMarkOccurrences, MarkOccurrences_Enable);
	section.SetBoolEx(L"ViewWhiteSpace", bViewWhiteSpace, false);
	section.SetBoolEx(L"ViewEOLs", bViewEOLs, false);
	section.SetIntEx(L"ShowCallTip", static_cast<int>(callTipInfo.showCallTip), ShowCallTip_None);

	if (iDefaultEncoding != CPI_GLOBAL_DEFAULT) {
		iValue = Encoding_MapIniSetting(false, iDefaultEncoding);
		section.SetInt(L"DefaultEncoding", iValue);
	}
	section.SetBoolEx(L"SkipUnicodeDetection", bSkipUnicodeDetection, true);
	section.SetBoolEx(L"LoadANSIasUTF8", bLoadANSIasUTF8, true);
	section.SetBoolEx(L"LoadASCIIasUTF8", bLoadASCIIasUTF8, true);
	section.SetBoolEx(L"LoadNFOasOEM", bLoadNFOasOEM, true);
	section.SetBoolEx(L"NoEncodingTags", bNoEncodingTags, false);

	section.SetIntEx(L"DefaultEOLMode", iDefaultEOLMode, 0);
	section.SetBoolEx(L"WarnLineEndings", bWarnLineEndings, true);
	section.SetBoolEx(L"FixLineEndings", bFixLineEndings, false);
	section.SetBoolEx(L"FixTrailingBlanks", bAutoStripBlanks, false);

	section.SetIntEx(L"PrintHeader", static_cast<int>(iPrintHeader), PrintHeaderOption_FilenameAndDate);
	section.SetIntEx(L"PrintFooter", static_cast<int>(iPrintFooter), PrintFooterOption_PageNumber);
	section.SetIntEx(L"PrintColorMode", iPrintColor, SC_PRINT_COLOURONWHITE);
	section.SetIntEx(L"PrintZoom", iPrintZoom, 100);
	section.SetIntEx(L"PrintMarginLeft", pageSetupMargin.left, -1);
	section.SetIntEx(L"PrintMarginTop", pageSetupMargin.top, -1);
	section.SetIntEx(L"PrintMarginRight", pageSetupMargin.right, -1);
	section.SetIntEx(L"PrintMarginBottom", pageSetupMargin.bottom, -1);

	section.SetBoolEx(L"SaveBeforeRunningTools", bSaveBeforeRunningTools, false);
	section.SetBoolEx(L"OpenFolderWithMatepath", bOpenFolderWithMatepath, true);

	section.SetIntEx(L"FileWatchingMode", iFileWatchingMode, FileWatchingMode_AutoReload);
	section.SetIntEx(L"FileWatchingOption", iFileWatchingOption, FileWatchingOption_None);
	section.SetBoolEx(L"ResetFileWatching", bResetFileWatching, false);
	section.SetIntEx(L"AutoSaveOption", iAutoSaveOption, AutoSaveOption_Default);
	section.SetIntEx(L"AutoSavePeriod", dwAutoSavePeriod, AutoSaveDefaultPeriod);

	section.SetIntEx(L"EscFunction", static_cast<int>(iEscFunction), EscFunction_None);
	section.SetBoolEx(L"AlwaysOnTop", bAlwaysOnTop, false);
	section.SetBoolEx(L"MinimizeToTray", bMinimizeToTray, false);
	section.SetBoolEx(L"TransparentMode", bTransparentMode, false);
	section.SetIntEx(L"EndAtLastLine", iEndAtLastLine, 1);
	section.SetBoolEx(L"EditLayoutRTL", bEditLayoutRTL, false);
	section.SetBoolEx(L"WindowLayoutRTL", bWindowLayoutRTL, false);
	section.SetIntEx(L"RenderingTechnology", iRenderingTechnology, GetDefualtRenderingTechnology());
	section.SetIntEx(L"Bidirectional", iBidirectional, SC_BIDIRECTIONAL_DISABLED);
	section.SetIntEx(L"FontQuality", iFontQuality, SC_EFF_QUALITY_LCD_OPTIMIZED);
	iValue = static_cast<int>(iCaretStyle) + static_cast<int>(bBlockCaretForOVRMode)*10 + static_cast<int>(bBlockCaretOutSelection)*100;
	section.SetIntEx(L"CaretStyle", iValue, CaretStyle_LineWidth1);
	section.SetIntEx(L"CaretBlinkPeriod", iCaretBlinkPeriod, -1);
	section.SetBoolEx(L"UseInlineIME", bUseInlineIME, false);
	Toolbar_GetButtons(hwndToolbar, TOOLBAR_COMMAND_BASE, tchToolbarButtons, COUNTOF(tchToolbarButtons));
	section.SetStringEx(L"ToolbarButtons", tchToolbarButtons, DefaultToolbarButtons);
	section.SetBoolEx(L"ShowMenu", bShowMenu, true);
	section.SetBoolEx(L"ShowToolbar", bShowToolbar, true);
	section.SetIntEx(L"AutoScaleToolbar", iAutoScaleToolbar, USER_DEFAULT_SCREEN_DPI);
	section.SetBoolEx(L"ShowStatusbar", bShowStatusbar, true);
	section.SetIntEx(L"FullScreenMode", iFullScreenMode, FullScreenMode_Default);

	SaveIniSection(INI_SECTION_NAME_SETTINGS, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
	// Scintilla Styles
	Style_Save();
}

void SaveWindowPosition(WCHAR *pIniSectionBuf) noexcept{
	IniSectionBuilder section = { pIniSectionBuf };

	WCHAR sectionName[96];
	GetWindowPositionSectionName(hCurrentMonitor, sectionName);

	// query window dimensions when window is not minimized
	if (!bInFullScreenMode && !IsIconic(hwndMain)) {
		WINDOWPLACEMENT wndpl;
		wndpl.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwndMain, &wndpl);

		wi.x = wndpl.rcNormalPosition.left;
		wi.y = wndpl.rcNormalPosition.top;
		wi.cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		wi.cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
		wi.max = (IsZoomed(hwndMain) || (wndpl.flags & WPF_RESTORETOMAXIMIZED));
	}

	section.SetInt(L"WindowPosX", wi.x);
	section.SetInt(L"WindowPosY", wi.y);
	section.SetInt(L"WindowSizeX", wi.cx);
	section.SetInt(L"WindowSizeY", wi.cy);
	section.SetBoolEx(L"WindowMaximized", wi.max, false);

	section.SetIntEx(L"RunDlgSizeX", cxRunDlg, 0);
	section.SetIntEx(L"EncodingDlgSizeX", cxEncodingDlg, 0);
	section.SetIntEx(L"EncodingDlgSizeY", cyEncodingDlg, 0);

	section.SetIntEx(L"FileMRUDlgSizeX", cxFileMRUDlg, 0);
	section.SetIntEx(L"FileMRUDlgSizeY", cyFileMRUDlg, 0);
	section.SetIntEx(L"OpenWithDlgSizeX", cxOpenWithDlg, 0);
	section.SetIntEx(L"OpenWithDlgSizeY", cyOpenWithDlg, 0);
	section.SetIntEx(L"FavoritesDlgSizeX", cxFavoritesDlg, 0);
	section.SetIntEx(L"FavoritesDlgSizeY", cyFavoritesDlg, 0);
	section.SetIntEx(L"AddFavoritesDlgSizeX", cxAddFavoritesDlg, 0);

	section.SetIntEx(L"ModifyLinesDlgSizeX", cxModifyLinesDlg, 0);
	section.SetIntEx(L"ModifyLinesDlgSizeY", cyModifyLinesDlg, 0);
	section.SetIntEx(L"EncloseSelectionDlgSizeX", cxEncloseSelectionDlg, 0);
	section.SetIntEx(L"EncloseSelectionDlgSizeY", cyEncloseSelectionDlg, 0);
	section.SetIntEx(L"InsertTagDlgSizeX", cxInsertTagDlg, 0);
	section.SetIntEx(L"InsertTagDlgSizeY", cyInsertTagDlg, 0);

	section.SetIntEx(L"FindReplaceDlgPosX", xFindReplaceDlg, 0);
	section.SetIntEx(L"FindReplaceDlgPosY", yFindReplaceDlg, 0);
	section.SetIntEx(L"FindReplaceDlgSizeX", cxFindReplaceDlg, 0);

	section.SetIntEx(L"StyleSelectDlgSizeX", cxStyleSelectDlg, 0);
	section.SetIntEx(L"StyleSelectDlgSizeY", cyStyleSelectDlg, 0);
	section.SetIntEx(L"StyleCustomizeDlgSizeX", cxStyleCustomizeDlg, 0);
	section.SetIntEx(L"StyleCustomizeDlgSizeY", cyStyleCustomizeDlg, 0);

	SaveIniSection(sectionName, pIniSectionBuf);
}

void ClearWindowPositionHistory() noexcept {
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
enum CommandParseState {
	CommandParseState_None,
	CommandParseState_Consumed,
	CommandParseState_Argument,
	CommandParseState_Unknown,
};

CommandParseState ParseCommandLineEncoding(LPCWSTR opt) noexcept {
	int flag = IDM_ENCODING_UNICODE;
	if (*opt == '-') {
		++opt;
	}
	if (StrStartsWithCase(opt, L"LE")){
		flag = IDM_ENCODING_UNICODE;
		opt += CSTRLEN(L"LE");
		if (*opt == '-') {
			++opt;
		}
	} else if (StrStartsWithCase(opt, L"BE")) {
		flag = IDM_ENCODING_UNICODEREV;
		opt += CSTRLEN(L"BE");
		if (*opt == '-') {
			++opt;
		}
	}
	if (*opt == L'\0' || StrCaseEqual(opt, L"BOM") || StrCaseEqual(opt, L"SIG") || StrCaseEqual(opt, L"SIGNATURE")) {
		flagSetEncoding = flag - IDM_ENCODING_ANSI + 1;
		return CommandParseState_Consumed;
	}
	return CommandParseState_None;
}

CommandParseState ParseCommandLineOption(LPWSTR lp1, LPWSTR lp2) noexcept {
	LPWSTR opt = lp1 + 1;
	// only accept /opt, -opt, --opt
	if (*opt == L'-') {
		++opt;
	}
	if (*opt == L'\0') {
		return CommandParseState_None;
	}

	CommandParseState state = CommandParseState_None;
	const int ch = ToUpperA(*opt);

	if (opt[1] == L'\0') {
		switch (ch) {
		case L'A':
			flagSetEncoding = IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1;
			state = CommandParseState_Consumed;
			break;

		case L'B':
			flagPasteBoard = true;
			state = CommandParseState_Consumed;
			break;

		case L'C':
			flagNewFromClipboard = true;
			state = CommandParseState_Consumed;
			break;

		case L'D':
			if (lpSchemeArg) {
				LocalFree(lpSchemeArg);
				lpSchemeArg = nullptr;
			}
			iInitialLexer = NP2LEX_TEXTFILE;
			flagLexerSpecified = true;
			state = CommandParseState_Consumed;
			break;

		case L'E':
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpEncodingArg) {
					LocalFree(lpEncodingArg);
				}
				lpEncodingArg = StrDup(lp1);
				state = CommandParseState_Consumed;
			}
			break;

		case L'F':
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				lstrcpyn(szIniFile, lp1, COUNTOF(szIniFile));
				TrimString(szIniFile);
				PathUnquoteSpaces(szIniFile);
				state = CommandParseState_Consumed;
			}
			break;

		case L'G':
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
#if defined(_WIN64)
				int64_t cord[2] = { 0 };
				const int itok = ParseCommaList64(lp1, cord, COUNTOF(cord));
#else
				int cord[2] = { 0 };
				const int itok = ParseCommaList(lp1, cord, COUNTOF(cord));
#endif
				if (itok != 0) {
					flagJumpTo = true;
					state = CommandParseState_Consumed;
					iInitialLine = cord[0];
					iInitialColumn = cord[1];
				}
			}
			break;

		case L'H':
			if (lpSchemeArg) {
				LocalFree(lpSchemeArg);
				lpSchemeArg = nullptr;
			}
			iInitialLexer = NP2LEX_HTML;
			flagLexerSpecified = true;
			state = CommandParseState_Consumed;
			break;

		case L'I':
			flagStartAsTrayIcon = true;
			state = CommandParseState_Consumed;
			break;

		case L'L':
			flagChangeNotify = TripleBoolean_True;
			state = CommandParseState_Consumed;
			break;

		case L'N':
			flagReuseWindow = false;
			flagNoReuseWindow = true;
			flagSingleFileInstance = false;
			state = CommandParseState_Consumed;
			break;

		case L'O':
			flagAlwaysOnTop = TripleBoolean_True;
			state = CommandParseState_Consumed;
			break;

		case L'Q':
			flagQuietCreate = true;
			state = CommandParseState_Consumed;
			break;

		case L'R':
			flagReuseWindow = true;
			flagNoReuseWindow = false;
			flagSingleFileInstance = false;
			state = CommandParseState_Consumed;
			break;

		case L'S':
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpSchemeArg) {
					LocalFree(lpSchemeArg);
				}
				lpSchemeArg = StrDup(lp1);
				flagLexerSpecified = true;
				state = CommandParseState_Consumed;
			}
			break;

		case L'T':
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				lstrcpyn(szTitleExcerpt, lp1, COUNTOF(szTitleExcerpt));
				fKeepTitleExcerpt = true;
				state = CommandParseState_Consumed;
			}
			break;

		case L'U':
			flagRelaunchElevated = RelaunchElevatedFlag_Startup;
			state = CommandParseState_Consumed;
			break;

		case L'W':
			flagSetEncoding = IDM_ENCODING_UNICODE - IDM_ENCODING_ANSI + 1;
			state = CommandParseState_Consumed;
			break;

		case L'X':
			if (lpSchemeArg) {
				LocalFree(lpSchemeArg);
				lpSchemeArg = nullptr;
			}
			iInitialLexer = NP2LEX_XML;
			flagLexerSpecified = true;
			state = CommandParseState_Consumed;
			break;

		case L'Z':
			// skip path for Notepad.exe
			ExtractFirstArgument(lp2, lp1, lp2);
			flagMultiFileArg = TripleBoolean_False;
			notepadAction = NotepadReplacementAction_Default;
			state = CommandParseState_Consumed;
			break;

		case L'?':
			flagDisplayHelp = true;
			state = CommandParseState_Consumed;
			break;

		default:
			state = CommandParseState_Unknown;
			break;
		}
	} else if (opt[2] == L'\0') {
		const int chNext = ToUpperA(opt[1]);
		switch (ch) {
		case L'C':
			if (chNext == L'R') {
				flagSetEOLMode = IDM_LINEENDINGS_CR - IDM_LINEENDINGS_CRLF + 1;
				state = CommandParseState_Consumed;
			}
			break;

		case L'F':
			if (chNext == L'0' || chNext == L'O') {
				StrCpyEx(szIniFile, L"*?");
				state = CommandParseState_Consumed;
			}
			break;

		case L'L':
			if (chNext == L'F') {
				flagSetEOLMode = IDM_LINEENDINGS_LF - IDM_LINEENDINGS_CRLF + 1;
				state = CommandParseState_Consumed;
			} else if (chNext == L'0' || chNext == L'-' || chNext == L'O') {
				flagChangeNotify = TripleBoolean_False;
				state = CommandParseState_Consumed;
			}
			break;

		case L'N':
			if (chNext == L'S') {
				flagReuseWindow = false;
				flagNoReuseWindow = true;
				flagSingleFileInstance = true;
				state = CommandParseState_Consumed;
			}
			break;

		case L'O':
			if (chNext == L'0' || chNext == L'-' || chNext == L'O') {
				flagAlwaysOnTop = TripleBoolean_False;
				state = CommandParseState_Consumed;
			}
			break;

		case L'R':
			if (chNext == L'S') {
				flagReuseWindow = true;
				flagNoReuseWindow = false;
				flagSingleFileInstance = true;
				state = CommandParseState_Consumed;
			} else if (chNext == L'O') {
				flagReadOnlyMode = ReadOnlyMode_Current;
				state = CommandParseState_Consumed;
			}
			break;

		default:
			state = CommandParseState_Unknown;
			break;
		}
	} else {
		state = CommandParseState_Unknown;
	}

	if (state != CommandParseState_Unknown) {
		return state;
	}

	state = CommandParseState_None;
	switch (ch) {
	case L'A':
		if (StrCaseEqual(opt, L"ANSI")) {
			flagSetEncoding = IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1;
			state = CommandParseState_Consumed;
		} else if (StrStartsWithCase(opt, L"appid=")) {
			// Shell integration
			opt += CSTRLEN(L"appid=");
			lstrcpyn(g_wchAppUserModelID, opt, COUNTOF(g_wchAppUserModelID));
			StrTrim(g_wchAppUserModelID, L"\" ");
			if (StrIsEmpty(g_wchAppUserModelID)) {
				lstrcpy(g_wchAppUserModelID, MY_APPUSERMODELID);
			}
			state = CommandParseState_Consumed;
		}
		break;

	case L'C':
		if (UnsafeUpper(opt[1]) == L'R') {
			opt += 2;
			if (*opt == L'-') {
				++opt;
			}
			if (opt[2] == L'\0' && UnsafeUpper(*opt) == L'L' && UnsafeUpper(opt[1]) == L'F') {
				flagSetEOLMode = IDM_LINEENDINGS_CRLF - IDM_LINEENDINGS_CRLF + 1;
				state = CommandParseState_Consumed;
			}
		}
		break;

	case L'M':
		if (StrCaseEqual(opt, L"MBCS")) {
			flagSetEncoding = IDM_ENCODING_ANSI - IDM_ENCODING_ANSI + 1;
			state = CommandParseState_Consumed;
		} else {
			int flag = MatchTextFlag_None;

			++opt;
			switch (UnsafeUpper(*opt)) {
			case L'R':
				flag |= MatchTextFlag_Regex;
				++opt;
				break;

			case L'B':
				flag |= MatchTextFlag_TransformBS;
				++opt;
				break;
			}

			if (*opt == L'-') {
				flag |= MatchTextFlag_FindUp;
				++opt;
			}
			if (*opt != L'\0') {
				break;
			}

			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpMatchArg) {
					LocalFree(lpMatchArg);
				}

				lpMatchArg = StrDup(lp1);
				flagMatchText = static_cast<MatchTextFlag>(flag | MatchTextFlag_Default);
				state = CommandParseState_Consumed;
			}
		}
		break;

	case L'P': {
		if (opt[1] == L'\0' || notepadAction == NotepadReplacementAction_Default) {
			notepadAction = NotepadReplacementAction_PrintDefault;
			if (UnsafeUpper(opt[1]) == L'T') {
				// ignore printer name
				notepadAction = NotepadReplacementAction_PrintDialog;
				ExtractFirstArgument(lp2, lp1, lp2);
			}
			state = CommandParseState_Consumed;
			break;
		}

		if (StrStartsWithCase(opt, L"POS")) {
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
				flagPosParam = true;
				flagDefaultPos = DefaultPositionFlag_SystemDefault;
				state = CommandParseState_Consumed;
			}
			break;

		case L'D':
		case L'S':
			if (opt[1] == L'\0' || (opt[2] == L'\0' && UnsafeUpper(opt[1]) == L'L')) {
				flagPosParam = true;
				flagDefaultPos = (opt[1] == L'\0')? DefaultPositionFlag_DefaultRight : DefaultPositionFlag_DefaultLeft;
				state = CommandParseState_Consumed;
			}
			break;

		case L'F':
		case L'L':
		case L'R':
		case L'T':
		case L'B':
		case L'M': {
			LPCWSTR p = opt;
			flagPosParam = true;
			flagDefaultPos = DefaultPositionFlag_None;
			state = CommandParseState_Consumed;
			while (*p && state == CommandParseState_Consumed) {
				switch (UnsafeUpper(*p++)) {
				case L'F':
					flagDefaultPos &= ~(DefaultPositionFlag_AlignLeft | DefaultPositionFlag_AlignRight | DefaultPositionFlag_AlignTop | DefaultPositionFlag_AlignBottom);
					flagDefaultPos |= DefaultPositionFlag_FullArea;
					break;

				case L'L':
					flagDefaultPos &= ~(DefaultPositionFlag_AlignRight | DefaultPositionFlag_FullArea);
					flagDefaultPos |= DefaultPositionFlag_AlignLeft;
					break;

				case L'R':
					flagDefaultPos &= ~(DefaultPositionFlag_AlignLeft | DefaultPositionFlag_FullArea);
					flagDefaultPos |= DefaultPositionFlag_AlignRight;
					break;

				case L'T':
					flagDefaultPos &= ~(DefaultPositionFlag_AlignBottom | DefaultPositionFlag_FullArea);
					flagDefaultPos |= DefaultPositionFlag_AlignTop;
					break;

				case L'B':
					flagDefaultPos &= ~(DefaultPositionFlag_AlignTop | DefaultPositionFlag_FullArea);
					flagDefaultPos |= DefaultPositionFlag_AlignBottom;
					break;

				case L'M':
					if (flagDefaultPos == DefaultPositionFlag_None) {
						flagDefaultPos |= DefaultPositionFlag_FullArea;
					}
					flagDefaultPos |= DefaultPositionFlag_Margin;
					break;

				default:
					state = CommandParseState_None;
					break;
				}
			}
		}
		break;

		default:
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				int cord[5] = { 0 };
				const int itok = ParseCommaList(lp1, cord, COUNTOF(cord));
				if (itok >= 4) {
					flagPosParam = true;
					flagDefaultPos = DefaultPositionFlag_None;
					state = CommandParseState_Consumed;
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
		if (StrStartsWithCase(opt, L"sysmru=")) {
			opt += CSTRLEN(L"sysmru=");
			if (opt[1] == L'\0') {
				const UINT value = opt[0] - L'0';
				if (value <= TRUE) {
					flagUseSystemMRU = static_cast<TripleBoolean>(value);
					state = CommandParseState_Consumed;
				}
			}
		}
		break;

	case L'U':
		if (StrStartsWithCase(opt, L"UTF")) {
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
					state = CommandParseState_Consumed;
				} else if (StrCaseEqual(opt, L"BOM") || StrCaseEqual(opt, L"SIG") || StrCaseEqual(opt, L"SIGNATURE")) {
					flagSetEncoding = IDM_ENCODING_UTF8SIGN - IDM_ENCODING_ANSI + 1;
					state = CommandParseState_Consumed;
				}
			} else if (*opt == L'1' && opt[1] == L'6') {
				opt += 2;
				state = ParseCommandLineEncoding(opt);
			}
		} else if (StrStartsWithCase(opt, L"UNICODE")) {
			opt += CSTRLEN(L"UNICODE");
			state = ParseCommandLineEncoding(opt);
		}
		break;
	}

	return state;
}

void ParseCommandLine() noexcept {
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

	LPWSTR lp1 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));
	LPWSTR lp3 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));

	// Start with 2nd argument
	if (!(ExtractFirstArgument(lpCmdLine, lp1, lp3) && *lp3)) {
		NP2HeapFree(lp1);
		NP2HeapFree(lp3);
		return;
	}

	bool bIsFileArg = false;
	LPWSTR lp2 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));
	while (ExtractFirstArgument(lp3, lp1, lp2)) {
		// options
		if (!bIsFileArg) {
			CommandParseState state = CommandParseState_None;
			if (lp1[1] == L'\0') {
				switch (*lp1) {
				case L'+':
					flagMultiFileArg = TripleBoolean_True;
					bIsFileArg = true;
					state = CommandParseState_Consumed;
					break;

				case L'-':
					flagMultiFileArg = TripleBoolean_False;
					bIsFileArg = true;
					state = CommandParseState_Consumed;
					break;
				}
			} else if (*lp1 == L'/' || *lp1 == L'-') {
				state = ParseCommandLineOption(lp1, lp2);
			}

			if (state == CommandParseState_Consumed) {
				lstrcpy(lp3, lp2);
				continue;
			}
			if (state == CommandParseState_Argument && flagMultiFileArg == TripleBoolean_True) {
				ExtractFirstArgument(lp3, lp1, lp2);
			}
		}

		// pathname
		{
			LPWSTR lpFileBuf = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));

			if (lpFileArg) {
				NP2HeapFree(lpFileArg);
			}

			lpFileArg = static_cast<LPWSTR>(NP2HeapAlloc(sizeof(WCHAR) * (MAX_PATH + 2))); // changed for ActivatePrevInst() needs
			if (flagMultiFileArg == TripleBoolean_True) {
				// multiple file arguments with quoted spaces
				lstrcpyn(lpFileArg, lp1, MAX_PATH);
			} else {
				lstrcpyn(lpFileArg, lp3, MAX_PATH);
			}

			PathFixBackslashes(lpFileArg);
			StrTrim(lpFileArg, L" \"");

			if (!PathIsRelative(lpFileArg) && !PathIsUNC(lpFileArg) && PathGetDriveNumber(lpFileArg) < 0) {
				WCHAR wchPath[MAX_PATH];
				lstrcpy(wchPath, g_wchWorkingDirectory);
				PathStripToRoot(wchPath);
				PathAppend(wchPath, lpFileArg);
				lstrcpy(lpFileArg, wchPath);
			}

			if (flagMultiFileArg == TripleBoolean_True) {
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
void LoadFlags() noexcept {
	IniSectionParser section;
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_FLAGS));
	const DWORD cchIniSection = static_cast<DWORD>(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	section.Init(64);

	LoadIniSection(INI_SECTION_NAME_FLAGS, pIniSectionBuf, cchIniSection);
	section.Parse(pIniSectionBuf);

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	uiLanguage = static_cast<LANGID>(section.GetInt(L"UILanguage", LANG_USER_DEFAULT));
	ValidateUILangauge();
#endif

	bSingleFileInstance = section.GetBool(L"SingleFileInstance", true);
	bReuseWindow = section.GetBool(L"ReuseWindow", false);
	bStickyWindowPosition = section.GetBool(L"StickyWindowPosition", false);

	if (!flagReuseWindow && !flagNoReuseWindow) {
		flagNoReuseWindow = !bReuseWindow;
		flagSingleFileInstance = bSingleFileInstance;
	}
	if (section.GetBool(L"ReadOnlyMode", false)) {
		flagReadOnlyMode |= ReadOnlyMode_AllFile;
	}
	if (flagMultiFileArg == TripleBoolean_NotSet) {
		if (section.GetBool(L"MultiFileArg", false)) {
			flagMultiFileArg = TripleBoolean_True;
		}
	}

	flagRelativeFileMRU = section.GetBool(L"RelativeFileMRU", true);
	flagPortableMyDocs = section.GetBool(L"PortableMyDocs", flagRelativeFileMRU);

	section.GetString(L"DefaultDirectory", L"", tchDefaultDir);

	dwFileCheckInterval = section.GetInt(L"FileCheckInterval", 1000);
	dwAutoReloadTimeout = section.GetInt(L"AutoReloadTimeout", 1000);

	if (IsVistaAndAbove()) {
		bUseXPFileDialog = section.GetBool(L"UseXPFileDialog", false);
	} else {
		bUseXPFileDialog = true;
	}

	flagNoFadeHidden = section.GetBool(L"NoFadeHidden", false);

	int iValue = section.GetInt(L"OpacityLevel", 75);
	iOpacityLevel = validate(iValue, 0, 100, 75);

	iValue = section.GetInt(L"FindReplaceOpacityLevel", 75);
	iFindReplaceOpacityLevel = validate(iValue, 0, 100, 75);

	flagSimpleIndentGuides = section.GetBool(L"SimpleIndentGuides", false);
	fNoHTMLGuess = section.GetBool(L"NoHTMLGuess", false);
	fNoCGIGuess = section.GetBool(L"NoCGIGuess", false);
	fNoAutoDetection = section.GetBool(L"NoAutoDetection", false);
	fNoFileVariables = section.GetBool(L"NoFileVariables", false);

	LPCWSTR strValue = section.GetValue(L"ToolbarImage");
	if (StrNotEmpty(strValue)) {
		tchToolbarBitmap = StrDup(strValue);
	}

	if (StrIsEmpty(g_wchAppUserModelID)) {
		strValue = section.GetValue(L"ShellAppUserModelID");
		if (StrNotEmpty(strValue)) {
			lstrcpyn(g_wchAppUserModelID, strValue, COUNTOF(g_wchAppUserModelID));
		} else {
			lstrcpy(g_wchAppUserModelID, MY_APPUSERMODELID);
		}
	}

	if (flagUseSystemMRU == TripleBoolean_NotSet) {
		if (section.GetBool(L"ShellUseSystemMRU", true)) {
			flagUseSystemMRU = TripleBoolean_True;
		}
	}

	section.Free();
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
// FindIniFile()
//
//
bool CheckIniFile(LPWSTR lpszFile, LPCWSTR lpszModule) noexcept {
	WCHAR tchFileExpanded[MAX_PATH];
	ExpandEnvironmentStrings(lpszFile, tchFileExpanded, COUNTOF(tchFileExpanded));

	if (PathIsRelative(tchFileExpanded)) {
		WCHAR tchBuild[MAX_PATH];
		// program directory
		lstrcpy(tchBuild, lpszModule);
		lstrcpy(PathFindFileName(tchBuild), tchFileExpanded);
		if (PathIsFile(tchBuild)) {
			lstrcpy(lpszFile, tchBuild);
			return true;
		}

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		const KNOWNFOLDERID * const rfidList[] = {
			&FOLDERID_LocalAppData,
			&FOLDERID_RoamingAppData,
			&FOLDERID_Profile,
		};
		for (UINT i = 0; i < COUNTOF(rfidList); i++) {
			LPWSTR pszPath = nullptr;
			if (S_OK == SHGetKnownFolderPath(*rfidList[i], KF_FLAG_DEFAULT, nullptr, &pszPath)) {
				PathCombine(tchBuild, pszPath, WC_NOTEPAD4);
				CoTaskMemFree(pszPath);
				PathAppend(tchBuild, tchFileExpanded);
				if (PathIsFile(tchBuild)) {
					lstrcpy(lpszFile, tchBuild);
					return true;
				}
			}
		}
#else
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
			if (S_OK == SHGetFolderPath(nullptr, csidlList[i], nullptr, SHGFP_TYPE_CURRENT, tchBuild)) {
				PathAppend(tchBuild, WC_NOTEPAD4);
				PathAppend(tchBuild, tchFileExpanded);
				if (PathIsFile(tchBuild)) {
					lstrcpy(lpszFile, tchBuild);
					return true;
				}
			}
		}
#endif
	} else if (PathIsFile(tchFileExpanded)) {
		lstrcpy(lpszFile, tchFileExpanded);
		return true;
	}

	return false;
}

bool CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule, LPCWSTR redirectKey) noexcept {
	WCHAR tch[MAX_PATH];
	if (GetPrivateProfileString(INI_SECTION_NAME_NOTEPAD4, redirectKey, L"", tch, COUNTOF(tch), lpszFile)) {
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
		return true;
	}
	return false;
}

bool FindIniFile() noexcept {
	if (StrEqualEx(szIniFile, L"*?")) {
		return false;
	}

	WCHAR tchTest[MAX_PATH];
	WCHAR tchModule[MAX_PATH];
	GetProgramRealPath(tchModule, COUNTOF(tchModule));

	if (StrNotEmpty(szIniFile)) {
		if (!CheckIniFile(szIniFile, tchModule)) {
			ExpandEnvironmentStringsEx(szIniFile, COUNTOF(szIniFile));
			if (PathIsRelative(szIniFile)) {
				lstrcpy(tchTest, tchModule);
				PathRemoveFileSpec(tchTest);
				PathAppend(tchTest, szIniFile);
				lstrcpy(szIniFile, tchTest);
			}
		}
		return true;
	}

	lstrcpy(tchTest, PathFindFileName(tchModule));
	PathRenameExtension(tchTest, L".ini");
	bool bFound = CheckIniFile(tchTest, tchModule);

	if (!bFound) {
		lstrcpy(tchTest, L"Notepad4.ini");
		bFound = CheckIniFile(tchTest, tchModule);
	}

	if (bFound) {
		// allow two redirections: administrator -> user -> custom
		if (CheckIniFileRedirect(tchTest, tchModule, L"Notepad4.ini")) {
			CheckIniFileRedirect(tchTest, tchModule, L"Notepad4.ini");
		}
		lstrcpy(szIniFile, tchTest);
	} else {
		lstrcpy(szIniFile, tchModule);
		PathRenameExtension(szIniFile, L".ini");
	}

	return true;
}

bool TestIniFile() noexcept {
	if (StrEqualEx(szIniFile, L"*?")) {
		StrCpyEx(szIniFile2, L"");
		StrCpyEx(szIniFile, L"");
		return false;
	}

	DWORD dwFileAttributes = GetFileAttributes(szIniFile);
	if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
		return true;
	}

	if ((dwFileAttributes != INVALID_FILE_ATTRIBUTES) || (StrNotEmpty(szIniFile) && szIniFile[lstrlen(szIniFile) - 1] == L'\\')) {
		WCHAR wchModule[MAX_PATH];
		GetProgramRealPath(wchModule, COUNTOF(wchModule));
		PathAppend(szIniFile, PathFindFileName(wchModule));
		PathRenameExtension(szIniFile, L".ini");
		dwFileAttributes = GetFileAttributes(szIniFile);
		if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			lstrcpy(PathFindFileName(szIniFile), L"Notepad4.ini");
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
		StrCpyEx(szIniFile, L"");
		return false;
	}

	return true;
}

void FindExtraIniFile(LPWSTR lpszIniFile, LPCWSTR defaultName, LPCWSTR redirectKey) noexcept {
	if (StrNotEmpty(szIniFile)) {
		WCHAR tch[MAX_PATH];
		if (GetPrivateProfileString(INI_SECTION_NAME_NOTEPAD4, redirectKey, L"", tch, COUNTOF(tch), szIniFile)) {
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
		GetProgramRealPath(lpszIniFile, MAX_PATH);
	}
	lstrcpy(PathFindFileName(lpszIniFile), defaultName);
}

bool CreateIniFile(LPCWSTR lpszIniFile) noexcept {
	if (StrNotEmpty(lpszIniFile)) {
		WCHAR *pwchTail = StrRChr(lpszIniFile, nullptr, L'\\');

		if (pwchTail != nullptr) {
			*pwchTail = L'\0';
			SHCreateDirectoryEx(nullptr, lpszIniFile, nullptr);
			*pwchTail = L'\\';
		}

		HANDLE hFile = CreateFile(lpszIniFile,
						   GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
						   nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		dwLastIOError = GetLastError();
		if (hFile != INVALID_HANDLE_VALUE) {
			LARGE_INTEGER fileSize;
			fileSize.QuadPart = 0;
			if (GetFileSizeEx(hFile, &fileSize) && fileSize.QuadPart < 2) {
				DWORD dw;
				WriteFile(hFile, L"\xFEFF[Notepad4]\r\n", 26, &dw, nullptr);
			}
			CloseHandle(hFile);
			return true;
		}
	}
	return false;
}

//=============================================================================
//
// UpdateToolbar()
//
//
void UpdateToolbar() noexcept {
	if (!bShowToolbar || !bInitDone) {
		return;
	}

#define EnableTool(id, b)		SendMessage(hwnd, TB_ENABLEBUTTON, id, MAKELPARAM(((b) ? 1 : 0), 0))
#define CheckTool(id, b)		SendMessage(hwnd, TB_CHECKBUTTON, id, MAKELPARAM(b, 0))
	HWND hwnd = hwndToolbar;
	const bool hasPath = StrNotEmpty(szCurFile);
	EnableTool(IDT_FILE_ADDTOFAV, hasPath);
	EnableTool(IDT_FILE_LAUNCH, hasPath);

	EnableTool(IDT_FILE_SAVE, IsDocumentModified());
	EnableTool(IDT_EDIT_UNDO, SciCall_CanUndo());
	EnableTool(IDT_EDIT_REDO, SciCall_CanRedo());
	EnableTool(IDT_EDIT_PASTE, SciCall_CanPaste());

	const bool nonEmpty = SciCall_GetLength() != 0;
	EnableTool(IDT_EDIT_CUT, nonEmpty);
	EnableTool(IDT_EDIT_COPY, nonEmpty);
	EnableTool(IDT_EDIT_FIND, nonEmpty);
	EnableTool(IDT_EDIT_REPLACE, nonEmpty);
	EnableTool(IDT_EDIT_DELETE, nonEmpty);

	CheckTool(IDT_VIEW_WORDWRAP, fvCurFile.fWordWrap);
	CheckTool(IDT_VIEW_ALWAYSONTOP, IsTopMost());
}

//=============================================================================
//
// UpdateStatusbar()
//
//
void UpdateStatusbar() noexcept {
	if (!bShowStatusbar || !bInitDone) {
		return;
	}

	const Sci_Position iPos = SciCall_GetCurrentPos();
	const Sci_Line iLine = SciCall_LineFromPosition(iPos);
	const Sci_Line iLines = SciCall_GetLineCount();

#if 0
	StopWatch watch;
	watch.Start();
#endif
	Sci_TextToFindFull ft = { { SciCall_PositionFromLine(iLine), iPos }, nullptr, { 0, 0 } };
	SciCall_CountCharactersAndColumns(&ft);
	const Sci_Position iChar = ft.chrgText.cpMin + 1;
	const Sci_Position iCol = ft.chrgText.cpMax + 1;
	Sci_Position iLineChar;
	Sci_Position iLineColumn;

	UINT updateMask = cachedStatusItem.updateMask;
	cachedStatusItem.updateMask = 0;
	if ((updateMask & (1 << StatusItem_Line)) || (iLine != cachedStatusItem.iLine)) {
		updateMask |= (1 << StatusItem_Line);
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
	watch.Stop();
	watch.ShowLog("CountCharacters time");
#endif

	WCHAR tchCurLine[32];
	WCHAR tchDocLine[32];
	FormatNumber(tchCurLine, iLine + 1);
	FormatNumber(tchDocLine, iLines);

	WCHAR tchCurColumn[32];
	WCHAR tchLineColumn[32];
	FormatNumber(tchCurColumn, iCol);
	FormatNumber(tchLineColumn, iLineColumn);

	WCHAR tchCurChar[32];
	WCHAR tchLineChar[32];
	FormatNumber(tchCurChar, iChar);
	FormatNumber(tchLineChar, iLineChar);

	WCHAR tchSelByte[32];
	WCHAR tchSelChar[32];
	WCHAR tchLinesSelected[32];
	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	if (iSelStart == iSelEnd) {
		tchSelByte[0] = L'0';
		tchSelByte[1] = L'\0';
		tchSelChar[0] = L'0';
		tchSelChar[1] = L'\0';
		tchLinesSelected[0] = L'0';
		tchLinesSelected[1] = L'\0';
	} else {
		if (!SciCall_IsRectangleSelection()) {
			const Sci_Position iSelByte = SciCall_GetSelTextLength();
			const Sci_Position iSelChar = SciCall_CountCharacters(iSelStart, iSelEnd);
			FormatNumber(tchSelByte, iSelByte);
			FormatNumber(tchSelChar, iSelChar);
		} else {
			tchSelByte[0] = L'-';
			tchSelByte[1] = L'-';
			tchSelByte[2] = L'\0';
			tchSelChar[0] = L'-';
			tchSelChar[1] = L'-';
			tchSelChar[2] = L'\0';
		}

		// Print number of selected lines in statusbar
		const Sci_Line iStartLine = SciCall_LineFromPosition(iSelStart);
		const Sci_Line iEndLine = SciCall_LineFromPosition(iSelEnd);
		const Sci_Position iStartOfLinePos = SciCall_PositionFromLine(iEndLine);
		Sci_Line iLinesSelected = iEndLine - iStartLine;
		if (iStartOfLinePos != iSelEnd) {
			iLinesSelected += 1;
		}
		FormatNumber(tchLinesSelected, iLinesSelected);
	}

	// find all and mark occurrences
	WCHAR tchMatchesCount[32];
	FormatNumber(tchMatchesCount, editMarkAll.matchCount);
	if (editMarkAll.pending) {
		lstrcat(tchMatchesCount, L" ...");
	}

	WCHAR tchDocSize[32];
	if (updateMask & (1 << StatusItem_DocSize)) {
		const Sci_Position iBytes = SciCall_GetLength();
		StrFormatByteSize(iBytes, tchDocSize, COUNTOF(tchDocSize));
	}

	WCHAR itemText[256];
	const int len = wsprintf(itemText, cachedStatusItem.tchItemFormat, tchCurLine, tchDocLine,
		tchCurColumn, tchLineColumn, tchCurChar, tchLineChar,
		tchSelChar, tchSelByte, tchLinesSelected, tchMatchesCount);

	LPCWSTR items[StatusItem_ItemCount];
	memset(AsVoidPointer(items), 0, StatusItem_Lexer * sizeof(LPCWSTR));
	LPWSTR start = itemText;
	UINT index = 0;
	for (int i = 0; i < len; i++) {
		if (itemText[i] == L'\n') {
			itemText[i] = L'\0';
			items[index] = start;
			start = itemText + i + 1;
			index += 1;
			if (index == StatusItem_Find) {
				break;
			}
		}
	}

	items[index] = start;
	memcpy(AsVoidPointer(&items[StatusItem_Lexer]), AsVoidPointer(&cachedStatusItem.pszLexerName), (StatusItem_Zoom - StatusItem_Lexer)*sizeof(LPCWSTR));
	items[StatusItem_Zoom] = cachedStatusItem.tchZoom;
	items[StatusItem_DocSize] = tchDocSize;

	static int cachedWidth[StatusItem_ItemCount];
	int aWidth[StatusItem_ItemCount];
	HWND hwnd = hwndStatus;
	// inline StatusCalcPaneWidth() function
	HDC hdc = GetDC(hwnd);
	HFONT hfont = GetWindowFont(hwnd);
	HFONT hfold = SelectFont(hdc, hfont);
	const int mmode = SetMapMode(hdc, MM_TEXT);
	int totalWidth = 0;
	for (int i = 0; i < StatusItem_ItemCount; i++) {
		int width;
		if (updateMask & 1) {
			SIZE size;
			LPCWSTR lpsz = items[i];
			//GetTextExtentPoint32(hdc, lpsz, lstrlen(lpsz), &size);
			GetTextExtentExPoint(hdc, lpsz, lstrlen(lpsz), 0, nullptr, nullptr, &size);
			width = NP2_align_up(size.cx + 9, 8);
			cachedWidth[i] = width;
		} else {
			width = cachedWidth[i];
			items[i] = nullptr;
		}
		updateMask >>= 1;
		totalWidth += width;
		aWidth[i] = width;
	}
	SetMapMode(hdc, mmode);
	SelectObject(hdc, hfold);
	ReleaseDC(hwnd, hdc);

	const int thumb = SystemMetricsForDpi(SM_CXHTHUMB, g_uCurrentDPI);
	RECT rc;
	GetClientRect(hwndMain, &rc);

	totalWidth += thumb;
	totalWidth -= aWidth[StatusItem_Empty];
	aWidth[StatusItem_Empty] = NP2_align_up(rc.right - rc.left, 8) - totalWidth;
	aWidth[StatusItem_DocSize] += thumb;
	for(int i = 1; i < StatusItem_ItemCount; i++) {
		aWidth[i] += aWidth[i - 1];
	}

	if (updateMask == 0) {
		SendMessage(hwnd, WM_SETREDRAW, FALSE, 0);
	}
	SendMessage(hwnd, SB_SETPARTS, COUNTOF(aWidth), AsInteger<LPARAM>(aWidth));
	for (int i = 0; i < StatusItem_ItemCount; i++) {
		LPCWSTR lpsz = items[i];
		if (lpsz) {
			StatusSetText(hwnd, i, lpsz);
		}
	}
	if (updateMask == 0) {
		SendMessage(hwnd, WM_SETREDRAW, TRUE, 0);
		InvalidateRect(hwnd, nullptr, TRUE);
	}
}

//=============================================================================
//
// UpdateLineNumberWidth()
//
//
void UpdateLineNumberWidth() noexcept {
	int width = 0;
	if (bShowLineNumbers) {
#if NP2_DEBUG_CODE_FOLDING
		width = 100;
#else
		char tchLines[32];

		const Sci_Line iLines = SciCall_GetLineCount();
		PosToStr(iLines, tchLines + 2);
		tchLines[0] = '_';
		tchLines[1] = '_';

		width = SciCall_TextWidth(STYLE_LINENUMBER, tchLines);
#endif
	}
	SciCall_SetMarginWidth(MarginNumber_LineNumber, width);
}

// based on SciTEWin::FullScreenToggle()
void ToggleFullScreenMode() noexcept {
	static bool bSaved;
	static WINDOWPLACEMENT wndpl;
	static DWORD mainStyle;

	HWND hwnd = hwndMain;
	if (bInFullScreenMode) {
		if (!bSaved) {
			bSaved = true;
			wndpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwnd, &wndpl);
			mainStyle = GetWindowStyle(hwnd);
		}

		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hCurrentMonitor, &mi);

		const UINT dpi = g_uCurrentDPI;
		const int padding = SystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
		const int cx = SystemMetricsForDpi(SM_CXSIZEFRAME, dpi) + padding;
		const int cy = SystemMetricsForDpi(SM_CYSIZEFRAME, dpi) + padding;
		int top = cy;
		if (iFullScreenMode & FullScreenMode_HideCaption) {
			top += SystemMetricsForDpi(SM_CYCAPTION, dpi);
		}

		const int x = mi.rcMonitor.left - cx;
		const int y = mi.rcMonitor.top - top;
		const int w = mi.rcMonitor.right - x + cx;
		const int h = mi.rcMonitor.bottom - y + cy;

		SetWindowStyle(hwnd, mainStyle & ~WS_THICKFRAME);
		SetWindowPos(hwnd, (IsTopMost() ? HWND_TOPMOST : HWND_TOP), x, y, w, h, 0);
	} else {
		bSaved = false;
		SetWindowStyle(hwnd, mainStyle);
		if (!IsTopMost()) {
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		if (wndpl.length) {
			if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
				ShowWindow(hwnd, SW_RESTORE);
				ShowWindow(hwnd, SW_SHOWMAXIMIZED);
			} else {
				SetWindowPlacement(hwnd, &wndpl);
			}
		}
	}
	SetForegroundWindow(hwnd);
}

//=============================================================================
//
//	FileIO()
//
//
bool FileIO(bool fLoad, LPWSTR pszFile, FileSaveFlag flag, EditFileIOStatus &status) noexcept {
	if (!(flag & FileSaveFlag_EndSession)) {
		BeginWaitCursor();

		WCHAR tch[MAX_PATH + 128];
		WCHAR fmt[128];
		FormatString(tch, fmt, (fLoad ? IDS_LOADFILE : IDS_SAVEFILE), pszFile);

		StatusSetText(hwndStatus, STATUS_HELP, tch);
		StatusSetSimple(hwndStatus, TRUE);

		InvalidateRect(hwndStatus, nullptr, TRUE);
		UpdateWindow(hwndStatus);
	}

	if (fLoad) {
		fLoad = EditLoadFile(pszFile, status);
		iSrcEncoding = CPI_NONE;
		iWeakSrcEncoding = CPI_NONE;
	} else {
		fLoad = EditSaveFile(hwndEdit, pszFile, flag, status);
	}

	const DWORD dwFileAttributes = GetFileAttributes(pszFile);
	bReadOnlyFile = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);

	if (!(flag & FileSaveFlag_EndSession)) {
		StatusSetSimple(hwndStatus, FALSE);
		EndWaitCursor();
	}

	return fLoad;
}

//=============================================================================
//
// FileLoad()
//
//
bool FileLoad(FileLoadFlag loadFlag, LPCWSTR lpszFile) {
	WCHAR tch[MAX_PATH] = L"";
	bool fSuccess = false;
	bool bRestoreView = false;
	Sci_Position iCurPos = 0;
	Sci_Position iAnchorPos = 0;
	Sci_Line iLine = 0;
	Sci_Position iCol = 0;
	Sci_Line iVisTopLine = 0;
	Sci_Line iDocTopLine = 0;
	int iXOffset = 0;
	bool keepTitleExcerpt = fKeepTitleExcerpt;
	bool keepCurrentLexer = false;

	if (!(loadFlag & FileLoadFlag_New) && StrNotEmpty(lpszFile)) {
		lstrcpy(tch, lpszFile);
		if (lpszFile == szCurFile || PathEqual(lpszFile, szCurFile)) {
			iCurPos = SciCall_GetCurrentPos();
			iAnchorPos = SciCall_GetAnchor();
			iLine = SciCall_LineFromPosition(iCurPos) + 1;
			iCol = SciCall_GetColumn(iCurPos) + 1;
			iVisTopLine = SciCall_GetFirstVisibleLine();
			iDocTopLine = SciCall_DocLineFromVisible(iVisTopLine);
			iXOffset = SciCall_GetXOffset();
			bRestoreView = true;
			keepTitleExcerpt = true;
			keepCurrentLexer = true;
			flagReadOnlyMode |= static_cast<int>(bReadOnlyMode);
		}
		fSuccess = true;
	}
	if (!(loadFlag & FileLoadFlag_DontSave)) {
		if (!FileSave(FileSaveFlag_Ask)) {
			return false;
		}
	}

	if (loadFlag & FileLoadFlag_New) {
		StrCpyEx(szCurFile, L"");
		SetDlgItemText(hwndMain, IDC_FILENAME, szCurFile);
		if (!keepTitleExcerpt) {
			StrCpyEx(szTitleExcerpt, L"");
		}
		fvCurFile.Init(nullptr, 0);
		EditSetEmptyText();
		bDocumentModified = false;
		bReadOnlyFile = false;
		iCurrentEOLMode = GetScintillaEOLMode(iDefaultEOLMode);
		SciCall_SetEOLMode(iCurrentEOLMode);
		iCurrentEncoding = iDefaultEncoding;
		iOriginalEncoding = iCurrentEncoding;
		SciCall_SetCodePage((iCurrentEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
		Style_SetLexer(nullptr, true);
		UpdateStatusBarCache(StatusItem_Encoding);
		UpdateStatusBarCache(StatusItem_EolMode);
		UpdateStatusBarCacheLineColumn();
		UpdateDocumentModificationStatus();

		AutoSave_Stop(TRUE);
		// Terminate file watching
		if (bResetFileWatching) {
			iFileWatchingMode = FileWatchingMode_None;
		}
		InstallFileWatching(true);

		return true;
	}

	if (!fSuccess) {
		if (!OpenFileDlg(tch, COUNTOF(tch), nullptr)) {
			return false;
		}
	}
	fSuccess = false;

	WCHAR szFileName[MAX_PATH] = L"";
	ExpandEnvironmentStringsEx(tch, COUNTOF(tch));

	if (PathIsRelative(tch)) {
		PathCombine(szFileName, g_wchWorkingDirectory, tch);
	} else {
		lstrcpy(szFileName, tch);
	}

	if (PathCanonicalize(tch, szFileName)) {
		lstrcpy(szFileName, tch);
	}
	GetLongPathName(szFileName, szFileName, COUNTOF(szFileName));
	PathGetLnkPath(szFileName, szFileName);

	EditFileIOStatus status{};
	status.iEncoding = iCurrentEncoding;
	status.iEOLMode = iCurrentEOLMode;

	// Ask to create a new file...
	if (!(loadFlag & FileLoadFlag_Reload) && !PathIsFile(szFileName)) {
		const int result = flagQuietCreate ? IDYES : MsgBoxWarn(MB_YESNOCANCEL, IDS_ASK_CREATE, szFileName);
		if (result == IDYES) {
			HANDLE hFile = CreateFile(szFileName,
									  GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
									  nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
			dwLastIOError = GetLastError();
			if (hFile != INVALID_HANDLE_VALUE) {
				fSuccess = true;
				CloseHandle(hFile);
				fvCurFile.Init(nullptr, 0);
				EditSetEmptyText();
				iCurrentEOLMode = GetScintillaEOLMode(iDefaultEOLMode);
				SciCall_SetEOLMode(iCurrentEOLMode);
				if (iSrcEncoding >= CPI_FIRST) {
					iCurrentEncoding = iSrcEncoding;
				} else {
					iCurrentEncoding = iDefaultEncoding;
				}
				iOriginalEncoding = iCurrentEncoding;
				SciCall_SetCodePage((iCurrentEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
				Style_SetLexer(nullptr, true);
				bReadOnlyFile = false;
			}
		} else if (result == IDCANCEL) {
			PostWMCommand(hwndMain, IDM_FILE_EXIT);
			return false;
		} else {
			return false;
		}
	} else {
		fSuccess = FileIO(true, szFileName, FileSaveFlag_Default, status);
		if (fSuccess) {
			iCurrentEncoding = status.iEncoding;
			iCurrentEOLMode = status.iEOLMode;
		}
	}

	if (fSuccess) {
		lstrcpy(szCurFile, szFileName);
		SetDlgItemText(hwndMain, IDC_FILENAME, szCurFile);
		if (!keepTitleExcerpt) {
			StrCpyEx(szTitleExcerpt, L"");
		}
		iOriginalEncoding = iCurrentEncoding;
		bDocumentModified = false;
		SciCall_SetEOLMode(iCurrentEOLMode);
		UpdateStatusBarCache(StatusItem_Encoding);
		UpdateStatusBarCache(StatusItem_EolMode);
		UpdateStatusBarCacheLineColumn();

		bool bUnknownFile = false;
		if (!keepCurrentLexer) {
			if (flagLexerSpecified) {
				if (pLexCurrent->rid == iInitialLexer) {
					Style_SetLexer(pLexCurrent, true);
				} else if (lpSchemeArg) {
					Style_SetLexerFromName(szCurFile, lpSchemeArg);
					LocalFree(lpSchemeArg);
					lpSchemeArg = nullptr;
				} else {
					Style_SetLexerFromID(iInitialLexer);
				}
				flagLexerSpecified = false;
			} else {
				np2LexLangIndex = 0;
				bUnknownFile = !Style_SetLexerFromFile(szCurFile);
			}
		} else {
			UpdateLineNumberWidth();
		}

		mruFile.Add(szFileName);
		if (flagUseSystemMRU == TripleBoolean_True) {
			SHAddToRecentDocs(SHARD_PATHW, szFileName);
		}

		AutoSave_Stop(!(loadFlag & FileLoadFlag_Reload));
		// Install watching of the current file
		if (!(loadFlag & FileLoadFlag_Reload) && bResetFileWatching) {
			iFileWatchingMode = FileWatchingMode_None;
		}
		InstallFileWatching(false);

		if (status.bBinaryFile || pLexCurrent->iLexer == SCLEX_DIFF) {
			// ignore auto "detected" Tab settings for binary file and diff file.
			if (fvCurFile.mask & FV_MaskHasFileTabSettings) {
				fvCurFile.mask &= ~FV_MaskHasFileTabSettings;
				Style_LoadTabSettings(pLexCurrent);
				fvCurFile.Apply();
			}
		}
		// open file in read only mode
		if (status.bBinaryFile || flagReadOnlyMode != ReadOnlyMode_None || bReadOnlyFile) {
			bReadOnlyMode = true;
			flagReadOnlyMode &= ReadOnlyMode_AllFile;
			SciCall_SetReadOnly(true);
		} else {
#if NP2_ENABLE_DOT_LOG_FEATURE
			if (IsFileStartsWithDotLog()) {
				bRestoreView = true;
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
				if (abs(iCurLine) > 5) {
					EditJumpTo(iLine, iCol);
				} else {
					SciCall_EnsureVisible(iDocTopLine);
					const Sci_Line iNewTopLine = SciCall_GetFirstVisibleLine();
					SciCall_LineScroll(0, iVisTopLine - iNewTopLine);
					SciCall_SetXOffset(iXOffset);
				}
			}
		}

		bInitDone = true;
		//! workaround for blank statusbar after loading large file: SCN_UPDATEUI is fired after Scintilla become idle.
		//DisableDelayedStatusBarRedraw(); // already set in MsgSize()
		UpdateStatusbar();
		UpdateWindowTitle();
		// Show warning: Unicode file loaded as ANSI
		if (status.bUnicodeErr) {
			MsgBoxWarn(MB_OK, IDS_ERR_UNICODE);
		}
		// notify binary file opened in read only mode
		if (status.bBinaryFile) {
			ShowNotificationMessage(SC_NOTIFICATIONPOSITION_BOTTOMRIGHT, IDS_BINARY_FILE_OPENED);
			return fSuccess;
		}
		// Show inconsistent line endings warning
		if (status.bInconsistent && bWarnLineEndings) {
			// file with unknown lexer and unknown encoding
			bUnknownFile = bUnknownFile && (iCurrentEncoding == CPI_DEFAULT);
			// Set default button to "No" for diff/patch and unknown file.
			// diff/patch file may contain content from files with different line endings.
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
bool FileSave(FileSaveFlag saveFlag) noexcept {
	const bool Untitled = StrIsEmpty(szCurFile);
	bool bIsEmptyNewFile = false;

	if (Untitled) {
		const Sci_Position cchText = SciCall_GetLength();
		if (cchText == 0) {
			bIsEmptyNewFile = true;
		} else if (cchText < 2048) {
			char tchText[2048] = "";
			SciCall_GetText(COUNTOF(tchText) - 1, tchText);
			StrTrimA(tchText, " \t\n\r"); // failure means not empty.
			if (StrIsEmpty(tchText)) {
				bIsEmptyNewFile = true;
			}
		}
	}

	if (!(saveFlag & (FileSaveFlag_SaveAlways | FileSaveFlag_SaveAs)) && (!IsDocumentModified() || bIsEmptyNewFile)) {
		AutoSave_Stop(saveFlag & FileSaveFlag_EndSession);
		return true;
	}

	// TODO: avoid message boxes when FileSaveFlag_EndSession is set
	if (saveFlag & FileSaveFlag_Ask) {
		// File or "Untitled" ...
		WCHAR tch[MAX_PATH];
		if (!Untitled) {
			lstrcpy(tch, szCurFile);
		} else {
			GetString(IDS_UNTITLED, tch, COUNTOF(tch));
		}

		switch (MsgBoxAsk(MB_YESNOCANCEL, IDS_ASK_SAVE, tch)) {
		case IDCANCEL:
			return false;
		case IDNO:
			AutoSave_Stop(FALSE);
			return true;
		}
	}

	bool fSuccess = false;
	WCHAR tchFile[MAX_PATH];
	EditFileIOStatus status{};
	status.iEncoding = iCurrentEncoding;
	status.iEOLMode = iCurrentEOLMode;

	// Read only...
	if (!(saveFlag & (FileSaveFlag_SaveAs | FileSaveFlag_SaveCopy)) && !Untitled) {
		const DWORD dwFileAttributes = GetFileAttributes(szCurFile);
		bReadOnlyFile = (dwFileAttributes != INVALID_FILE_ATTRIBUTES) && (dwFileAttributes & FILE_ATTRIBUTE_READONLY);
		if (bReadOnlyFile) {
			UpdateWindowTitle();
			if (MsgBoxWarn(MB_YESNO, IDS_READONLY_SAVE, szCurFile) == IDYES) {
				saveFlag = static_cast<FileSaveFlag>(saveFlag | FileSaveFlag_SaveAs);
			} else {
				return false;
			}
		}
		if (!(saveFlag & FileSaveFlag_SaveAs)) {
			fSuccess = FileIO(false, szCurFile, saveFlag, status);
			if (!fSuccess) {
				saveFlag = static_cast<FileSaveFlag>(saveFlag | FileSaveFlag_SaveAs);
			}
		}
	}

	// Save As...
	if ((saveFlag & (FileSaveFlag_SaveAs | FileSaveFlag_SaveCopy)) || Untitled) {
		WCHAR tchInitialDir[MAX_PATH] = L"";
		if ((saveFlag & FileSaveFlag_SaveCopy) && StrNotEmpty(tchLastSaveCopyDir)) {
			lstrcpy(tchInitialDir, tchLastSaveCopyDir);
			PathCombine(tchFile, tchInitialDir, PathFindFileName(szCurFile));
		} else {
			lstrcpy(tchFile, szCurFile);
		}

		if (SaveFileDlg(Untitled, tchFile, COUNTOF(tchFile), tchInitialDir)) {
			fSuccess = FileIO(false, tchFile, saveFlag, status);
			if (fSuccess) {
				if (!(saveFlag & FileSaveFlag_SaveCopy)) {
					lstrcpy(szCurFile, tchFile);
					SetDlgItemText(hwndMain, IDC_FILENAME, szCurFile);
					if (!fKeepTitleExcerpt) {
						StrCpyEx(szTitleExcerpt, L"");
					}
					if (flagLexerSpecified) {
						if (pLexCurrent->rid == iInitialLexer) {
							UpdateLineNumberWidth();
						} else if (lpSchemeArg) {
							Style_SetLexerFromName(szCurFile, lpSchemeArg);
							LocalFree(lpSchemeArg);
							lpSchemeArg = nullptr;
						} else {
							Style_SetLexerFromID(iInitialLexer);
						}
						flagLexerSpecified = false;
					} else {
						Style_SetLexerFromFile(szCurFile);
					}
				} else {
					lstrcpy(tchLastSaveCopyDir, tchFile);
					PathRemoveFileSpec(tchLastSaveCopyDir);
				}
			}
		} else {
			return false;
		}
	} else if (!fSuccess) {
		fSuccess = FileIO(false, szCurFile, saveFlag, status);
	}

	if (fSuccess) {
		if (!(saveFlag & FileSaveFlag_SaveCopy)) {
			bDocumentModified = false;
			iOriginalEncoding = iCurrentEncoding;
			mruFile.Add(szCurFile);
			if (flagUseSystemMRU == TripleBoolean_True) {
				SHAddToRecentDocs(SHARD_PATHW, szCurFile);
			}
			if (flagRelaunchElevated == RelaunchElevatedFlag_Manual && (saveFlag & FileSaveFlag_SaveAs)
				&& iPathNameFormat == TitlePathNameFormat_NameOnly) {
				iPathNameFormat = TitlePathNameFormat_NameFirst;
			}
			UpdateDocumentModificationStatus();

			// Install watching of the current file
			if ((saveFlag & FileSaveFlag_SaveAs) && bResetFileWatching) {
				iFileWatchingMode = FileWatchingMode_None;
			}
			InstallFileWatching(false);
		}

		AutoSave_Stop(saveFlag & FileSaveFlag_EndSession);
	} else if (!status.bCancelDataLoss) {
		if (StrNotEmpty(szCurFile)) {
			lstrcpy(tchFile, szCurFile);
		}

		UpdateWindowTitle();
		MsgBoxLastError(MB_OK, IDS_ERR_SAVEFILE, tchFile);
	}

	return fSuccess;
}

void EditApplyDefaultEncoding(LPCEDITLEXER pLex, BOOL bLexerChanged) noexcept {
	int iEncoding;
	int iEOLMode;
	switch (pLex->rid) {
	case NP2LEX_ANSI:
		iEncoding = bLoadNFOasOEM ? g_DOSEncoding : CPI_DEFAULT;
		iEOLMode = SC_EOL_CRLF;
		break;

	case NP2LEX_BASH:
		iEncoding = CPI_UTF8;
		iEOLMode = SC_EOL_LF;
		break;

	case NP2LEX_BATCH:
		iEncoding = CPI_DEFAULT;
		iEOLMode = SC_EOL_CRLF;
		break;

	case NP2LEX_VHDL:
		iEncoding = Encoding_GetIndex(1252); // ISO-8859-1
		iEOLMode = iCurrentEOLMode;
		break;

	default:
		// default encoding for empty file.
		if (bLexerChanged) {
			iEncoding = bLoadASCIIasUTF8 ? CPI_UTF8 : iDefaultEncoding;
		} else {
			iEncoding = iCurrentEncoding;
		}
		iEOLMode = GetScintillaEOLMode(iDefaultEOLMode);
		break;
	}

	if (iEOLMode != iCurrentEOLMode) {
		iCurrentEOLMode = iEOLMode;
		SciCall_SetEOLMode(iEOLMode);
		UpdateStatusBarCache(StatusItem_EolMode);
	}

	const UINT uFlags = mEncoding[iCurrentEncoding].uFlags;
	if (uFlags & (NCP_UNICODE_BOM | NCP_UTF8_SIGN)) {
		// retain encoding BOM for empty file
		return;
	}
	if (iEncoding == CPI_DEFAULT) {
		iEncoding = Encoding_GetAnsiIndex();
	}
	if (iEncoding != iCurrentEncoding) {
		iCurrentEncoding = iEncoding;
		iOriginalEncoding = iEncoding;
		SciCall_SetCodePage((iEncoding == CPI_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
		UpdateStatusBarCache(StatusItem_Encoding);
	}
}

//=============================================================================
//
// OpenFileDlg()
//
//
void SetupInitialOpenSaveDir(LPWSTR tchInitialDir, DWORD cchInitialDir, LPCWSTR lpstrInitialDir) noexcept {
	tchInitialDir[0] = L'\0';
	if (StrNotEmpty(lpstrInitialDir)) {
		lstrcpy(tchInitialDir, lpstrInitialDir);
	} else if (StrNotEmpty(szCurFile)) {
		lstrcpy(tchInitialDir, szCurFile);
		PathRemoveFileSpec(tchInitialDir);
	} else if (StrNotEmpty(tchDefaultDir)) {
		ExpandEnvironmentStrings(tchDefaultDir, tchInitialDir, cchInitialDir);
		if (PathIsRelative(tchInitialDir)) {
			WCHAR tchModule[MAX_PATH];
			GetModuleFileName(nullptr, tchModule, COUNTOF(tchModule));
			PathRemoveFileSpec(tchModule);
			PathAppend(tchModule, tchInitialDir);
			PathCanonicalize(tchInitialDir, tchModule);
		}
	} else if (StrNotEmpty(mruFile.pszItems[0])) {
		lstrcpy(tchInitialDir, mruFile.pszItems[0]);
		PathRemoveFileSpec(tchInitialDir);
	} else {
		lstrcpy(tchInitialDir, g_wchWorkingDirectory);
	}
}

BOOL OpenFileDlg(LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) noexcept {
	WCHAR tchInitialDir[MAX_PATH];
	SetupInitialOpenSaveDir(tchInitialDir, COUNTOF(tchInitialDir), lpstrInitialDir);
	WCHAR szFile[MAX_PATH];
	szFile[0] = L'\0';
	int lexers[1 + OPENDLG_MAX_LEXER_COUNT] = {0}; // 1-based filter index
	LPWSTR szFilter = Style_GetOpenDlgFilterStr(true, szCurFile, lexers);

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndMain;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFile;
	ofn.lpstrInitialDir = tchInitialDir;
	ofn.lpstrDefExt = L""; // auto add first extension from current filter
	ofn.nMaxFile = COUNTOF(szFile);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | /* OFN_NOCHANGEDIR |*/
				OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST |
				OFN_SHAREAWARE /*| OFN_NODEREFERENCELINKS*/;
	if (bUseXPFileDialog) {
		ofn.Flags |= OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK;
		ofn.lpfnHook = OpenSaveFileDlgHookProc;
	}

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
BOOL SaveFileDlg(bool Untitled, LPWSTR lpstrFile, int cchFile, LPCWSTR lpstrInitialDir) noexcept {
	WCHAR tchInitialDir[MAX_PATH];
	SetupInitialOpenSaveDir(tchInitialDir, COUNTOF(tchInitialDir), lpstrInitialDir);
	WCHAR szNewFile[MAX_PATH];
	lstrcpy(szNewFile, lpstrFile);
	int lexers[1 + OPENDLG_MAX_LEXER_COUNT] = {0}; // 1-based filter index
	LPWSTR szFilter = Style_GetOpenDlgFilterStr(false, szCurFile, lexers);

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndMain;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szNewFile;
	ofn.lpstrInitialDir = tchInitialDir;
	ofn.lpstrDefExt = L""; // auto add first extension from current filter
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY /*| OFN_NOCHANGEDIR*/ |
				/*OFN_NODEREFERENCELINKS |*/ OFN_OVERWRITEPROMPT |
				OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST;
	if (bUseXPFileDialog) {
		ofn.Flags |= OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK;
		ofn.lpfnHook = OpenSaveFileDlgHookProc;
	}

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
* Tries to find and activate an already open Notepad4 Window
*
*
******************************************************************************/
static BOOL CALLBACK EnumWindProcSingleFileInstance(HWND hwnd, LPARAM lParam) noexcept {
	BOOL bContinue = TRUE;
	WCHAR szClassName[64];

	if (GetClassName(hwnd, szClassName, COUNTOF(szClassName))) {
		if (StrCaseEqual(szClassName, wchWndClass)) {
			WCHAR tchFileName[MAX_PATH];
			if (lpFileArg == nullptr || (GetDlgItemText(hwnd, IDC_FILENAME, tchFileName, COUNTOF(tchFileName)) && PathEquivalent(tchFileName, lpFileArg))) {
				*AsPointer<HWND *>(lParam) = hwnd;
				if (IsWindowEnabled(hwnd)) {
					bContinue = FALSE;
				}
			}
		}
	}
	return bContinue;
}

static void ActivatePrevWindow(HWND hwnd, LPCWSTR lpszFile) noexcept {
	DWORD cb = sizeof(NP2PARAMS);
	int cchTitleExcerpt = 0;
	if (lpszFile) {
		cb += (lstrlen(lpszFile) + 1) * sizeof(WCHAR);
		cb += (lstrlen(lpSchemeArg) + 1) * sizeof(WCHAR);
		cchTitleExcerpt = lstrlen(szTitleExcerpt);
		if (cchTitleExcerpt) {
			cb += (cchTitleExcerpt + 1) * sizeof(WCHAR);
		}
	}
	if (lpMatchArg) {
		cb += (lstrlen(lpMatchArg) + 1) * sizeof(WCHAR);
	}

	static_assert(__is_standard_layout(NP2PARAMS));
	NP2PARAMS *params = static_cast<NP2PARAMS *>(GlobalAlloc(GPTR, cb));
	params->flagFileSpecified = false;
	params->flagReadOnlyMode = flagReadOnlyMode & ReadOnlyMode_Current;
	params->flagLexerSpecified = flagLexerSpecified;
	params->flagQuietCreate = flagQuietCreate;
	params->flagTitleExcerpt = false;
	params->flagJumpTo = flagJumpTo;
	params->flagChangeNotify = flagChangeNotify;

	LPWSTR lpsz = &params->wchData;
	if (lpszFile) {
		params->flagFileSpecified = true;
		lstrcpy(lpsz, lpszFile);
		params->iInitialLexer = iInitialLexer;
		if (flagLexerSpecified && lpSchemeArg) {
			params->iInitialLexer = 0;
			lpsz = StrEnd(lpsz) + 1;
			lstrcpy(lpsz, lpSchemeArg);
		}
		if (cchTitleExcerpt) {
			params->flagTitleExcerpt = true;
			lpsz = StrEnd(lpsz) + 1;
			lstrcpy(lpsz, szTitleExcerpt);
		}
		if (lpMatchArg) {
			lpsz = StrEnd(lpsz) + 1;
		}
	}
	if (lpMatchArg) {
		params->flagMatchText = flagMatchText;
		lstrcpy(lpsz, lpMatchArg);
	}

	params->iInitialLine = iInitialLine;
	params->iInitialColumn = iInitialColumn;

	params->iSrcEncoding = lpEncodingArg ? Encoding_Match(lpEncodingArg) : CPI_NONE;
	params->flagSetEncoding = flagSetEncoding;
	params->flagSetEOLMode = flagSetEOLMode;

	COPYDATASTRUCT cds;
	cds.dwData = DATA_NOTEPAD4_PARAMS;
	cds.cbData = static_cast<DWORD>(GlobalSize(params));
	cds.lpData = params;

	SendMessage(hwnd, WM_COPYDATA, 0, AsInteger<LPARAM>(&cds));
	GlobalFree(params);
}

bool ActivatePrevInst() noexcept {
	if (flagStartAsTrayIcon || flagNewFromClipboard || flagPasteBoard) {
		return false;
	}

	HWND hwnd = nullptr;
	LPWSTR lpszFile = lpFileArg;
	if (flagSingleFileInstance && lpszFile) {
		ExpandEnvironmentStringsEx(lpszFile, static_cast<DWORD>(NP2HeapSize(lpszFile) / sizeof(WCHAR)));

		if (PathIsRelative(lpszFile)) {
			WCHAR tchTmp[MAX_PATH];
			PathCombine(tchTmp, g_wchWorkingDirectory, lpszFile);
			lstrcpy(lpszFile, tchTmp);
		}

		GetLongPathName(lpszFile, lpszFile, MAX_PATH);
		EnumWindows(EnumWindProcSingleFileInstance, AsInteger<LPARAM>(&hwnd));
		lpszFile = nullptr;
	} else if (!flagNoReuseWindow) {
		lpFileArg = nullptr;
		EnumWindows(EnumWindProcSingleFileInstance, AsInteger<LPARAM>(&hwnd));
		lpFileArg = lpszFile;
	}

	// Found a window
	if (hwnd != nullptr) {
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

			if (lpszFile) {
				ExpandEnvironmentStringsEx(lpszFile, static_cast<DWORD>(NP2HeapSize(lpszFile) / sizeof(WCHAR)));

				if (PathIsRelative(lpszFile)) {
					WCHAR tchTmp[MAX_PATH];
					PathCombine(tchTmp, g_wchWorkingDirectory, lpszFile);
					lstrcpy(lpszFile, tchTmp);
				}
			}
			ActivatePrevWindow(hwnd, lpszFile);
			return true;
		}

		// Ask...
		if (IDYES == MsgBoxAsk(MB_YESNO, IDS_ERR_PREVWINDISABLED)) {
			return false;
		}
		return true;
	}
	return false;
}

//=============================================================================
//
// RelaunchMultiInst()
//
//
bool RelaunchMultiInst() noexcept {
	if (flagMultiFileArg == TripleBoolean_True && cFileList > 1) {
		LPWSTR lpCmdLineNew = StrDup(GetCommandLine());
		const size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLineNew) + 1);
		LPWSTR lp1 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));
		LPWSTR lp2 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));

		StrTab2Space(lpCmdLineNew);
		StrCpyEx(lpCmdLineNew + cchiFileList, L"");

		LPWSTR pwch = StrEnd(lpCmdLineNew) - 1;
		int i = 0;
		while (*pwch == L' ' || *pwch == L'-' || *pwch == L'+') {
			*pwch = L' ';
			pwch--;
			if (i++ > 1) {
				cchiFileList--;
			}
		}

		for (i = 0; i < cFileList; i++) {
			lstrcpy(lpCmdLineNew + cchiFileList, L" /n - ");
			lstrcat(lpCmdLineNew, lpFileList[i]);
			LocalFree(lpFileList[i]);

			STARTUPINFO si;
			memset(&si, 0, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			PROCESS_INFORMATION pi;
			memset(&pi, 0, sizeof(PROCESS_INFORMATION));

			if (CreateProcess(nullptr, lpCmdLineNew, nullptr, nullptr, FALSE, 0, nullptr, g_wchWorkingDirectory, &si, &pi)) {
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		}

		LocalFree(lpCmdLineNew);
		NP2HeapFree(lp1);
		NP2HeapFree(lp2);
		NP2HeapFree(lpFileArg);

		return true;
	}

	for (int i = 0; i < cFileList; i++) {
		LocalFree(lpFileList[i]);
	}
	return false;
}

void GetRelaunchParameters(LPWSTR szParameters, LPCWSTR lpszFile, bool newWind, bool emptyWind) noexcept {
	WCHAR tch[64];
	wsprintf(tch, L"-appid=\"%s\"", g_wchAppUserModelID);
	lstrcpy(szParameters, tch);

	wsprintf(tch, L" -sysmru=%i", (flagUseSystemMRU == TripleBoolean_True));
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
		// read only mode
		if (bReadOnlyMode) {
			lstrcat(szParameters, L" -ro");
		}

		// file encoding
		switch (iCurrentEncoding) {
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
			const char *enc = mEncoding[iCurrentEncoding].pszParseNames;
			const char *sep = strchr(enc, ',');
			memset(tch, 0, sizeof(tch));
			MultiByteToWideChar(CP_UTF8, 0, enc, static_cast<int>(sep - enc), tch, COUNTOF(tch));
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
			PosToStr(line, tchLn);
			PosToStr(col, tchCol);
			wsprintf(tch, L" -g %s,%s", tchLn, tchCol);
#else
			wsprintf(tch, L" -g %d,%d", static_cast<int>(line), static_cast<int>(col));
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
bool RelaunchElevated() noexcept {
	if (!IsVistaAndAbove() || fIsElevated || flagRelaunchElevated == RelaunchElevatedFlag_None || flagDisplayHelp) {
		return false;
	}
	{
		LPWSTR lpArg1;
		LPWSTR lpArg2;
		bool exit = true;

		if (flagRelaunchElevated == RelaunchElevatedFlag_Manual) {
			WCHAR tchFile[MAX_PATH];
			lstrcpy(tchFile, szCurFile);
			if (!FileSave(FileSaveFlag_Ask)) {
				return false;
			}

			exit = PathEqual(tchFile, szCurFile);
			lpArg1 = static_cast<LPWSTR>(NP2HeapAlloc(sizeof(WCHAR) * MAX_PATH));
			GetModuleFileName(nullptr, lpArg1, MAX_PATH);
			lpArg2 = static_cast<LPWSTR>(NP2HeapAlloc(sizeof(WCHAR) * 1024));
			GetRelaunchParameters(lpArg2, tchFile, !exit, false);
			exit = !IsDocumentModified();
		} else {
			const LPCWSTR lpCmdLine = GetCommandLine();
			const size_t cmdSize = sizeof(WCHAR) * (lstrlen(lpCmdLine) + 1);
			lpArg1 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));
			lpArg2 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));
			ExtractFirstArgument(lpCmdLine, lpArg1, lpArg2);
		}

		if (StrNotEmpty(lpArg1)) {
			SHELLEXECUTEINFO sei;
			memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC | SEE_MASK_NOZONECHECKS;
			sei.hwnd = GetForegroundWindow();
			sei.lpVerb = L"runas";
			sei.lpFile = lpArg1;
			sei.lpParameters = lpArg2;
			sei.lpDirectory = g_wchWorkingDirectory;
			sei.nShow = SW_SHOWNORMAL;

			if (!ShellExecuteEx(&sei)) {
				exit = false;
			}
		} else {
			exit = false;
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
// Aligns Notepad4 to the default window position on the current screen
//
//
void SnapToDefaultPos(HWND hwnd) noexcept {
	RECT rcOld;
	GetWindowRect(hwnd, &rcOld);

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hCurrentMonitor, &mi);

	const int y = mi.rcWork.top + 16;
	const int cy = mi.rcWork.bottom - mi.rcWork.top - 32;
	const int cx = min<int>(mi.rcWork.right - mi.rcWork.left - 32, cy);
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
void ShowNotifyIcon(HWND hwnd, bool bAdd) noexcept {
	if (bAdd && (hTrayIcon == nullptr || uTrayIconDPI != g_uCurrentDPI)) {
		if (hTrayIcon) {
			DestroyIcon(hTrayIcon);
			hTrayIcon = nullptr;
		}
		uTrayIconDPI = g_uCurrentDPI;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		LoadIconMetric(g_exeInstance, MAKEINTRESOURCE(IDR_MAINWND), LIM_SMALL, &hTrayIcon);
#else
		const int size = SystemMetricsForDpi(SM_CXSMICON, uTrayIconDPI);
		hTrayIcon = static_cast<HICON>(LoadImage(g_exeInstance, MAKEINTRESOURCE(IDR_MAINWND), IMAGE_ICON, size, size, LR_DEFAULTCOLOR));
#endif
	}

	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = APPM_TRAYMESSAGE;
	nid.hIcon = hTrayIcon;
	lstrcpy(nid.szTip, WC_NOTEPAD4);
	Shell_NotifyIcon(bAdd ? NIM_ADD : NIM_DELETE, &nid);
}

//=============================================================================
//
// SetNotifyIconTitle()
//
//
void SetNotifyIconTitle(HWND hwnd) noexcept {
	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
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
		StrCpyEx(nid.szTip, L"* ");
	}
	StrCatBuff(nid.szTip, tchTitle, COUNTOF(nid.szTip));
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void ShowNotificationA(WPARAM notifyPos, LPCSTR lpszText) noexcept {
	callTipInfo.type = CallTipType_Notification;
	SciCall_CallTipSetBack(callTipInfo.backColor);
	SciCall_CallTipSetFore(callTipInfo.foreColor);
	SciCall_CallTipUseStyle(CallTipTabWidthNotification);
	SciCall_ShowNotification(notifyPos, lpszText);
}

void ShowNotificationW(WPARAM notifyPos, LPCWSTR lpszText) noexcept {
	const int cpEdit = SciCall_GetCodePage();
	const int wchLen = lstrlen(lpszText);
	const int cchLen = wchLen*kMaxMultiByteCount + 1;
	char *cchText = static_cast<char *>(NP2HeapAlloc(cchLen));
	WideCharToMultiByte(cpEdit, 0, lpszText, -1, cchText, cchLen, nullptr, nullptr);
	ShowNotificationA(notifyPos, cchText);
	NP2HeapFree(cchText);
}

void ShowNotificationMessage(WPARAM notifyPos, UINT uidMessage, ...) noexcept {
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
void InstallFileWatching(bool terminate) noexcept {
	terminate = terminate || iFileWatchingMode == FileWatchingMode_None || StrIsEmpty(szCurFile);
	// Terminate
	if (bRunningWatch) {
		if (hChangeHandle) {
			FindCloseChangeNotification(hChangeHandle);
			hChangeHandle = nullptr;
		}
		if (terminate) {
			KillTimer(nullptr, ID_WATCHTIMER);
		}
	}

	dwChangeNotifyTime = 0;
	bRunningWatch = !terminate;
	if (bRunningWatch) {
		// Install
		SetTimer(nullptr, ID_WATCHTIMER, dwFileCheckInterval, WatchTimerProc);

		WCHAR tchDirectory[MAX_PATH];
		lstrcpy(tchDirectory, szCurFile);
		PathRemoveFileSpec(tchDirectory);

		// Save data of current file
		WIN32_FIND_DATA data;
		if (GetFileAttributesEx(szCurFile, GetFileExInfoStandard, &data)) {
			memcpy(&fdCurFile, &data.ftLastWriteTime, sizeof(fdCurFile));
		} else {
			memset(&fdCurFile, 0, sizeof(fdCurFile));
		}

		if ((iFileWatchingOption & FileWatchingOption_LogFile) == FileWatchingOption_None) {
			hChangeHandle = FindFirstChangeNotification(tchDirectory, FALSE,
						FILE_NOTIFY_CHANGE_FILE_NAME	| \
						FILE_NOTIFY_CHANGE_DIR_NAME		| \
						FILE_NOTIFY_CHANGE_ATTRIBUTES	| \
						FILE_NOTIFY_CHANGE_SIZE			| \
						FILE_NOTIFY_CHANGE_LAST_WRITE);
			if (hChangeHandle == INVALID_HANDLE_VALUE) {
				hChangeHandle = nullptr;
			}
		}
	}
}

static inline bool IsCurrentFileChangedOutsideApp() noexcept {
	// Check if the file has been changed
	WIN32_FIND_DATA fdUpdated;
	if (!GetFileAttributesEx(szCurFile, GetFileExInfoStandard, &fdUpdated)) {
		// The current file has been removed
		return true;
	}

	const bool changed = memcmp(&fdCurFile, &fdUpdated.ftLastWriteTime, sizeof(fdCurFile)) != 0;
	return changed;
}

static void CheckCurrentFileChangedOutsideApp() noexcept {
	// Check if the changes affect the current file
	if (IsCurrentFileChangedOutsideApp()) {
		// Shutdown current watching and give control to main window
		if (hChangeHandle) {
			FindCloseChangeNotification(hChangeHandle);
			hChangeHandle = nullptr;
		}
		if (iFileWatchingMode == FileWatchingMode_AutoReload) {
			bRunningWatch = true;
			dwChangeNotifyTime = GetTickCount();
		} else {
			KillTimer(nullptr, ID_WATCHTIMER);
			bRunningWatch = false;
			dwChangeNotifyTime = 0;
			SendMessage(hwndMain, APPM_CHANGENOTIFY, 0, 0);
		}
	} else if (hChangeHandle) {
		FindNextChangeNotification(hChangeHandle);
	}
}

//=============================================================================
//
// WatchTimerProc()
//
//
void CALLBACK WatchTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) noexcept {
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(idEvent);
	UNREFERENCED_PARAMETER(dwTime);

	if (bRunningWatch) {
		if (dwChangeNotifyTime > 0 && GetTickCount() - dwChangeNotifyTime > dwAutoReloadTimeout) {
			if (hChangeHandle) {
				FindCloseChangeNotification(hChangeHandle);
				hChangeHandle = nullptr;
			}
			KillTimer(nullptr, ID_WATCHTIMER);
			bRunningWatch = false;
			dwChangeNotifyTime = 0;
			SendMessage(hwndMain, APPM_CHANGENOTIFY, 0, 0);
		}
		// Check Change Notification Handle
		// TODO: notification not fired for continuously updated file
		else if (hChangeHandle) {
			if (WAIT_OBJECT_0 == WaitForSingleObject(hChangeHandle, 0)) {
				CheckCurrentFileChangedOutsideApp();
			}
		}
		// polling, not very efficient but useful for watching continuously updated file
		else {
			if (dwChangeNotifyTime == 0) {
				CheckCurrentFileChangedOutsideApp();
			}
		}
	}
}

//=============================================================================
//
// PasteBoardTimer()
//
//
void CALLBACK PasteBoardTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) noexcept {
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(idEvent);
	UNREFERENCED_PARAMETER(dwTime);

	if (dwLastCopyTime > 0 && GetTickCount() - dwLastCopyTime > 200) {
		if (SciCall_CanPaste()) {
			const bool back = autoCompletionConfig.bIndentText;
			autoCompletionConfig.bIndentText = false;
			SciCall_DocumentEnd();
			SciCall_BeginUndoAction();
			if (SciCall_GetLength() > 0) {
				SciCall_NewLine();
			}
			SciCall_Paste(false);
			const Sci_Position length = SciCall_GetLength();
			const int ch = SciCall_GetCharAt(length - 1);
			if (!IsEOLChar(ch)) {
				SciCall_NewLine();
			}
			SciCall_EndUndoAction();
			EditEnsureSelectionVisible();
			autoCompletionConfig.bIndentText = back;
		}

		dwLastCopyTime = 0;
	}
}

void AutoSave_Start(bool reset) noexcept {
	if ((iAutoSaveOption & AutoSaveOption_Periodic) && dwAutoSavePeriod != 0) {
		if (reset || !bAutoSaveTimerSet) {
			bAutoSaveTimerSet = true;
			SetTimer(hwndMain, ID_AUTOSAVETIMER, dwAutoSavePeriod, nullptr);
		}
		return;
	}
	if (bAutoSaveTimerSet) {
		bAutoSaveTimerSet = false;
		KillTimer(hwndMain, ID_AUTOSAVETIMER);
	}
}

void AutoSave_Stop(BOOL keepBackup) noexcept {
	dwCurrentDocReversion = 0;
	dwLastSavedDocReversion = 0;
	if (bAutoSaveTimerSet) {
		bAutoSaveTimerSet = false;
		KillTimer(hwndMain, ID_AUTOSAVETIMER);
	}
	if (autoSaveCount) {
		keepBackup |= iAutoSaveOption & AutoSaveOption_ManuallyDelete;
		for (int i = 0; i < autoSaveCount; i++) {
			LPWSTR path = autoSavePathList[i];
			if (path) {
				if (!keepBackup) {
					DeleteFile(path);
				}
				LocalFree(path);
			}
		}

		autoSaveCount = 0;
		memset(AsVoidPointer(autoSavePathList), 0, sizeof(LPWSTR) * AllAutoSaveCount);
	}
}

LPCWSTR AutoSave_GetDefaultFolder() noexcept {
	LPWSTR szFolder = szAutoSaveFolder;
	if (StrIsEmpty(szFolder)) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		LPWSTR pszPath = nullptr;
		const HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, nullptr, &pszPath);
		if (hr == S_OK) {
			PathCombine(szFolder, pszPath, WC_NOTEPAD4);
			CoTaskMemFree(pszPath);
		} else {
			GetModuleFileName(nullptr, szFolder, MAX_PATH);
			PathRemoveFileSpec(szFolder);
		}
#else
		const HRESULT hr = SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, szFolder);
		if (hr == S_OK) {
			PathAppend(szFolder, WC_NOTEPAD4);
		} else {
			GetModuleFileName(nullptr, szFolder, MAX_PATH);
			PathRemoveFileSpec(szFolder);
		}
#endif

		PathAppend(szFolder, L"AutoSave");
	}

	const DWORD dwFileAttributes = GetFileAttributes(szFolder);
	if (dwFileAttributes == INVALID_FILE_ATTRIBUTES) {
		SHCreateDirectoryEx(nullptr, szFolder, nullptr);
	}
	return szFolder;
}

void AutoSave_DoWork(FileSaveFlag saveFlag) noexcept {
	if (!(saveFlag & FileSaveFlag_SaveAlways) && (!IsDocumentModified() || dwCurrentDocReversion == dwLastSavedDocReversion)) {
		return;
	}

	const DWORD cbData = static_cast<DWORD>(SciCall_GetLength());
	if (cbData == 0) {
		return;
	}

	const bool Untitled = StrIsEmpty(szCurFile);
	if (!Untitled && saveFlag == FileSaveFlag_Default && (iAutoSaveOption & AutoSaveOption_OverwriteCurrent)) {
		// overwrite current file
		EditFileIOStatus status{};
		status.iEncoding = iCurrentEncoding;
		status.iEOLMode = iCurrentEOLMode;
		if (EditSaveFile(hwndEdit, szCurFile, FileSaveFlag_EndSession, status)) {
			dwLastSavedDocReversion = dwCurrentDocReversion;
			return;
		}
	}

	WCHAR tchPath[MAX_PATH + 40];
	LPCWSTR extension = L"bak";
	if (Untitled) {
		lstrcpy(tchPath, L"Untitled");
	} else {
		lstrcpy(tchPath, szCurFile);
		LPWSTR lpszExt = StrChr(tchPath, L'.'); // address for first file extension
		if (lpszExt) {
			lpszExt[0] = L'\0';
			extension = lpszExt + 1;
		}
	}

	// add timestamp + pid unique suffix
	WCHAR suffix[MAX_PATH + 60];
	int metaLen = 0;
	const UINT pid = GetCurrentProcessId();
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	//printf("%u AutoSave at %02d:%02d:%02d.%03d\n", pid, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
	wsprintf(suffix, L" %04d%02d%02d %02d%02d%02d %03d%u.%s",
		lt.wYear, lt.wMonth, lt.wDay,
		lt.wHour, lt.wMinute, lt.wSecond,
		lt.wMilliseconds, pid, extension);
	lstrcat(tchPath, suffix);

	// TODO: check free space with GetDiskFreeSpaceExW()
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (!Untitled) {
		// try to save backup in the same folder as current file
		hFile = CreateFile(tchPath,
						   GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   nullptr, CREATE_NEW,
						   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
						   nullptr);
		dwLastIOError = GetLastError();
	}
	if (hFile == INVALID_HANDLE_VALUE) {
		// save backup in AutoSave folder
		if (Untitled) {
			lstrcpy(suffix, tchPath);
		} else {
			metaLen = 1;
			lstrcpy(suffix, PathFindFileName(tchPath));
		}

		LPCWSTR szFolder = AutoSave_GetDefaultFolder();
		PathCombine(tchPath, szFolder, suffix);
		hFile = CreateFile(tchPath,
						   GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   nullptr, CREATE_NEW,
						   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
						   nullptr);
		dwLastIOError = GetLastError();
	}
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}

	DWORD length = cbData + COUNTOF(suffix)*kMaxMultiByteCount + 1;
	char *lpData = static_cast<char *>(NP2HeapAlloc(length));
	if (metaLen) {
		lstrcpy(suffix, L"AutoSave for ");
		lstrcat(suffix, szCurFile);
		metaLen = lstrlen(suffix);
		const UINT cpEdit = (iCurrentEncoding == CPI_DEFAULT) ? CP_ACP : CP_UTF8;
		metaLen = WideCharToMultiByte(cpEdit, 0, suffix, metaLen, lpData, length, nullptr, nullptr);
		switch (iCurrentEOLMode) {
		default: // SC_EOL_CRLF
			lpData[metaLen++] = '\r';
			lpData[metaLen++] = '\n';
			break;
		case SC_EOL_LF:
			lpData[metaLen++] = '\n';
			break;
		case SC_EOL_CR:
			lpData[metaLen++] = '\r';
			break;
		}
	}

	SciCall_GetText(cbData, lpData + metaLen);
	SetEndOfFile(hFile);
	// no encoding conversion, always saved in UTF-8 or ANSI encoding
	const BOOL bWriteSuccess = WriteFile(hFile, lpData, cbData + metaLen, &length, nullptr);
	dwLastIOError = GetLastError();
	CloseHandle(hFile);
	NP2HeapFree(lpData);

	if (bWriteSuccess) {
		if (saveFlag & FileSaveFlag_SaveAlways) {
			dwLastSavedDocReversion = dwCurrentDocReversion;
			return; // treat "Save Backup" as "Save As" with generated file name
		}
		if (!(saveFlag & FileSaveFlag_SaveCopy) && autoSaveCount == MaxAutoSaveCount) {
			// delete oldest backup
			LPWSTR old = autoSavePathList[0];
			if (old) {
				if (!(iAutoSaveOption & AutoSaveOption_ManuallyDelete)) {
					DeleteFile(old);
				}
				LocalFree(old);
			}
			memmove(AsVoidPointer(autoSavePathList), AsVoidPointer(autoSavePathList + 1), (AllAutoSaveCount - 1) * sizeof(LPWSTR));
			autoSavePathList[AllAutoSaveCount - 1] = nullptr;
			--autoSaveCount;
		}

		autoSavePathList[autoSaveCount++] = StrDup(tchPath);
		dwLastSavedDocReversion = dwCurrentDocReversion;
	} else {
		DeleteFile(tchPath);
	}
}
