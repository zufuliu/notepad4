/******************************************************************************
*
*
* matepath - The universal Explorer-like Plugin
*
* matepath.cpp
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
#include <commctrl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <cstdio>
#include "config.h"
#include "Helpers.h"
#include "Dlapi.h"
#include "Dialogs.h"
#include "matepath.h"
#include "resource.h"

/******************************************************************************
*
* Local Variables for matepath.cpp
*
*/
HWND	hwndStatus;
static HWND	hwndToolbar;
static HWND hwndReBar;

#define TB_DEL_FILTER_BMP 13
#define TB_ADD_FILTER_BMP 14

#define TOOLBAR_COMMAND_BASE	IDT_HISTORY_BACK
#define DefaultToolbarButtons	L"1 2 3 4 5 0 8"
static TBBUTTON tbbMainWnd[] = {
	{0, 0, 0, TBSTYLE_SEP, {0}, 0, 0},
	{0, IDT_HISTORY_BACK, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{1, IDT_HISTORY_FORWARD, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{2, IDT_UP_DIR, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{3, IDT_ROOT_DIR, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{4, IDT_VIEW_FAVORITES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{5, IDT_FILE_PREV, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{6, IDT_FILE_NEXT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{7, IDT_FILE_RUN, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{8, IDT_FILE_QUICKVIEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{9, IDT_FILE_SAVEAS, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{10, IDT_FILE_COPYMOVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{11, IDT_FILE_DELETE_RECYCLE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{12, IDT_FILE_DELETE_PERM, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{13, IDT_VIEW_FILTER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	// TB_ADD_FILTER_BMP and TB_DEL_FILTER_BMP both used for IDT_VIEW_FILTER
};

static HWND hwndDriveBox;
HWND	hwndDirList;
HWND	hwndMain;
static HICON hTrayIcon = nullptr;
static UINT uTrayIconDPI = 0;

static HANDLE hChangeHandle = nullptr;
HistoryList	mHistory;

WCHAR	szIniFile[MAX_PATH] = L"";
WCHAR	szIniFile2[MAX_PATH] = L"";
bool	bSaveSettings;
static bool bHasQuickview = false;
WCHAR	szQuickview[MAX_PATH];
WCHAR	szQuickviewParams[MAX_PATH];
WCHAR	tchFavoritesDir[MAX_PATH];
WCHAR	tchOpenWithDir[MAX_PATH];
static WCHAR tchToolbarButtons[MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE];
static LPWSTR tchToolbarBitmap = nullptr;
bool	bClearReadOnly;
bool	bRenameOnCollision;
bool	bSingleClick;
bool	bOpenFileInSameWindow;
static int iDefaultOpenMenu;
static int iShiftOpenMenu;
bool	bTrackSelect;
bool	bFullRowSelect;
StartupDirectory iStartupDir;
EscFunction iEscFunction;
bool	bFocusEdit;
bool	bAlwaysOnTop;
static bool bTransparentMode;
bool bUseXPFileDialog;
bool	bWindowLayoutRTL;
bool	bMinimizeToTray;
bool	fUseRecycleBin;
bool	fNoConfirmDelete;
static bool bShowToolbar;
static int iAutoScaleToolbar;
static bool bShowStatusbar;
static bool bShowDriveBox;
int		cxRunDlg;
int		cxGotoDlg;
int		cxFileFilterDlg;
int		cxRenameFileDlg;
int		cxNewDirectoryDlg;
int		cxOpenWithDlg;
int		cyOpenWithDlg;
int		cxCopyMoveDlg;
int		cxTargetApplicationDlg;
int		cxFindWindowDlg;

WCHAR		tchFilter[DL_FILTER_BUFSIZE];
bool		bNegFilter;
bool		bDefColorNoFilter;
bool		bDefColorFilter;
COLORREF	colorNoFilter;
COLORREF	colorFilter;
COLORREF	colorCustom[16];

struct WININFO {
	int x;
	int y;
	int cx;
	int cy;
};

static WININFO wi;

static int cyReBar;
static int cyReBarFrame;
static int cyDriveBoxFrame;

int		nIdFocus = IDC_DIRLIST;

WCHAR	szCurDir[MAX_PATH + 40];
static WCHAR szMRUDirectory[MAX_PATH];
static DWORD dwFillMask;
static int nSortFlags;
static bool fSortRev;

static LPWSTR lpPathArg = nullptr;
static LPWSTR lpFilterArg = nullptr;

static UINT wFuncCopyMove = FO_COPY;

static UINT msgTaskbarCreated = 0;

UseTargetApplication iUseTargetApplication = UseTargetApplication_NotSet;
TargetApplicationMode iTargetApplicationMode = TargetApplicationMode_None;
WCHAR	szTargetApplication[MAX_PATH] = L"";
WCHAR	szTargetApplicationParams[MAX_PATH] = L"";
WCHAR	szTargetApplicationWndClass[MAX_PATH] = L"";
WCHAR	szDDEMsg[256] = L"";
WCHAR	szDDEApp[256] = L"";
WCHAR	szDDETopic[256] = L"";

HINSTANCE	g_hInstance;
#if NP2_ENABLE_APP_LOCALIZATION_DLL
HINSTANCE	g_exeInstance;
#endif
HANDLE		g_hDefaultHeap;
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
DWORD		g_uWinVer;
#endif
UINT		g_uCurrentDPI = USER_DEFAULT_SCREEN_DPI;
UINT		g_uSystemDPI = USER_DEFAULT_SCREEN_DPI;
#if !NP2_HAS_GETDPIFORWINDOW
namespace {
// scintilla\win32\PlatWin.cxx
using GetDpiForWindowSig = UINT (WINAPI *)(HWND hwnd);
GetDpiForWindowSig fnGetDpiForWindow = nullptr;

#ifndef DPI_ENUMS_DECLARED
#define MDT_EFFECTIVE_DPI	0
#endif

using GetDpiForMonitorSig = HRESULT (WINAPI *)(HMONITOR hmonitor, /*MONITOR_DPI_TYPE*/int dpiType, UINT *dpiX, UINT *dpiY);
HMODULE hShcoreDLL {};
GetDpiForMonitorSig fnGetDpiForMonitor = nullptr;

using GetSystemMetricsForDpiSig = int (WINAPI *)(int nIndex, UINT dpi);
GetSystemMetricsForDpiSig fnGetSystemMetricsForDpi = nullptr;

using AdjustWindowRectExForDpiSig = BOOL (WINAPI *)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
AdjustWindowRectExForDpiSig fnAdjustWindowRectExForDpi = nullptr;
}

static void LoadDpiForWindow() noexcept;
#endif // NP2_HAS_GETDPIFORWINDOW

WCHAR g_wchAppUserModelID[64] = L"";
#if NP2_ENABLE_APP_LOCALIZATION_DLL
static HMODULE hResDLL;
LANGID uiLanguage;
UINT languageResID;
#endif

//=============================================================================
//
// Flags
//
static bool	flagNoReuseWindow	= false;
bool		bReuseWindow		= false;
static bool	flagStartAsTrayIcon	= false;
bool		flagPortableMyDocs	= false;
bool		flagGotoFavorites	= false;
static DWORD iAutoRefreshRate	= 0;
bool		flagNoFadeHidden	= false;
static int	iOpacityLevel		= 75;
static bool	flagPosParam		= false;

static inline bool HasFilter() noexcept {
	return !StrEqualEx(tchFilter, L"*.*") || bNegFilter;
}

//=============================================================================
//
//  WinMain()
//
//
static void CleanUpResources(bool initialized) noexcept {
	if (tchToolbarBitmap != nullptr) {
		LocalFree(tchToolbarBitmap);
	}
	if (hTrayIcon) {
		DestroyIcon(hTrayIcon);
	}
	if (initialized) {
		UnregisterClass(WC_MATEPATH, g_hInstance);
	}
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	if (hResDLL) {
		FreeLibrary(hResDLL);
	}
#endif
#if !NP2_HAS_GETDPIFORWINDOW
	if (hShcoreDLL) {
		FreeLibrary(hShcoreDLL);
	}
#endif
	OleUninitialize();
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) noexcept {
	if (dwCtrlType == CTRL_C_EVENT) {
		ShowNotifyIcon(hwndMain, false);
		SendMessage(hwndMain, WM_CLOSE, 0, 0);
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
	SetEnvironmentVariable(L"UBSAN_OPTIONS", L"log_path=" WC_MATEPATH L"-UBSan.log");
#endif

	// Set global variable g_hInstance
	g_hInstance = hInstance;
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	g_exeInstance = hInstance;
#endif
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
	// Set the Windows version global variable
	NP2_COMPILER_WARNING_PUSH
	NP2_IGNORE_WARNING_DEPRECATED_DECLARATIONS
	g_uWinVer = LOWORD(GetVersion());
	NP2_COMPILER_WARNING_POP
	g_uWinVer = MAKEWORD(HIBYTE(g_uWinVer), LOBYTE(g_uWinVer));
#endif

	g_hDefaultHeap = GetProcessHeap();
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
	// Command Line, Ini File and Flags
	ParseCommandLine();
	FindIniFile();
	TestIniFile();
	CreateIniFile(szIniFile);
	LoadFlags();

	// Try to activate another window
	if (ActivatePrevInst()) {
		return 0;
	}

	// set AppUserModelID
	PrivateSetCurrentProcessExplicitAppUserModelID(g_wchAppUserModelID);

	// Init OLE and Common Controls
	OleInitialize(nullptr);

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	msgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	hResDLL = LoadLocalizedResourceDLL(uiLanguage, WC_MATEPATH L".dll");
	if (hResDLL) {
		g_hInstance = hInstance = hResDLL;
	}
#endif

// since Windows 10, version 1607
#if (defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)) && !defined(__MINGW32__)
// 1709 was the first version for Windows 10 on ARM64.
	g_uSystemDPI = GetDpiForSystem();
#else
	LoadDpiForWindow();
#endif

	// Load Settings
	LoadSettings();

	if (!InitApplication(hInstance)) {
		CleanUpResources(false);
		return FALSE;
	}

	InitInstance(hInstance, nShowCmd);
	HWND hwnd = hwndMain;
	HACCEL hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(hwnd, hAcc, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CleanUpResources(true);
	return static_cast<int>(msg.wParam);
}

//=============================================================================
//
//  InitApplication()
//
//
BOOL InitApplication(HINSTANCE hInstance) noexcept {
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc   = MainWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = AsPointer<HBRUSH, ULONG_PTR>(COLOR_3DFACE + 1);
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = WC_MATEPATH;
	wc.hIconSm       = nullptr;

	return RegisterClassEx(&wc);
}

//=============================================================================
//
//  InitInstance()
//
//
void InitInstance(HINSTANCE hInstance, int nCmdShow) {
	const bool defaultPos = (wi.x == CW_USEDEFAULT || wi.y == CW_USEDEFAULT || wi.cx == CW_USEDEFAULT || wi.cy == CW_USEDEFAULT);
	RECT rc = { wi.x, wi.y, (defaultPos ? CW_USEDEFAULT : (wi.x + wi.cx)), (defaultPos ? CW_USEDEFAULT : (wi.y + wi.cy)) };

	if (defaultPos) {
		// default window position
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		wi.x = rc.left + 16;
		wi.y = rc.top + 16;
		wi.cx = 272;
		wi.cy = 640;
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
				   WC_MATEPATH,
				   WC_MATEPATH,
				   WS_MATEPATH,
				   wi.x,
				   wi.y,
				   wi.cx,
				   wi.cy,
				   nullptr,
				   nullptr,
				   hInstance,
				   nullptr);
	if (bAlwaysOnTop) {
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	if (bTransparentMode) {
		SetWindowTransparentMode(hwnd, true, iOpacityLevel);
	}
	if (!flagStartAsTrayIcon) {
		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);
	} else {
		ShowWindow(hwnd, SW_HIDE);   // trick ShowWindow()
		ShowNotifyIcon(hwnd, true);
	}

	// Pathname parameter
	if (lpPathArg) {
		DisplayPath(lpPathArg, IDS_ERR_CMDLINE);
		GlobalFree(lpPathArg);
	} else if (iStartupDir != StartupDirectory_None) {
		// Use a startup directory
		if (iStartupDir == StartupDirectory_MRU) {
			if (!StrIsEmpty(szMRUDirectory)) {
				GetModuleFileName(nullptr, szMRUDirectory, COUNTOF(szMRUDirectory));
				PathRemoveFileSpec(szMRUDirectory);
			}
			DisplayPath(szMRUDirectory, IDS_ERR_STARTUPDIR);
		} else {
			DisplayPath(tchFavoritesDir, IDS_ERR_STARTUPDIR);
		}
	} else if (flagGotoFavorites) {
		// Favorites
		DisplayPath(tchFavoritesDir, IDS_ERR_FAVORITES);
	}

	// Update Dirlist
	if (!ListView_GetItemCount(hwndDirList)) {
		PostWMCommand(hwnd, IDM_VIEW_UPDATE);
	}
}

//=============================================================================
//
//  MainWndProc()
//
//  Messages are distributed to the MsgXXX-handlers
//
//
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static bool bShutdownOK;

	switch (umsg) {
	case WM_CREATE: {
		// Init directory watching
		if (iAutoRefreshRate) {
			SetTimer(hwnd, ID_TIMER, iAutoRefreshRate, nullptr);
		}
		return MsgCreate(hwnd, wParam, lParam);
	}

	case WM_DESTROY:
	case WM_ENDSESSION:
		if (!bShutdownOK) {
			// Terminate directory watching
			KillTimer(hwnd, ID_TIMER);
			FindCloseChangeNotification(hChangeHandle);

			DirList_Destroy(hwndDirList);
			DragAcceptFiles(hwnd, FALSE);

			mHistory.Empty();

			SaveSettings(false);

			bShutdownOK = true;
		}
		if (umsg == WM_DESTROY) {
			PostQuitMessage(0);
		}
		break;

	// Reinitialize theme-dependent values and resize windows
	case WM_THEMECHANGED:
		MsgThemeChanged(hwnd, wParam, lParam);
		break;

	case WM_DPICHANGED:
		MsgDPIChanged(hwnd, wParam, lParam);
		break;

	// update colors of DirList manually
	case WM_SYSCOLORCHANGE: {
		SendMessage(hwndToolbar, WM_SYSCOLORCHANGE, wParam, lParam);
		const LRESULT lret = DefWindowProc(hwnd, umsg, wParam, lParam);

		if (HasFilter()) {
			ListView_SetTextColor(hwndDirList, (bDefColorFilter) ? GetSysColor(COLOR_WINDOWTEXT) : colorFilter);
			ListView_RedrawItems(hwndDirList, 0, ListView_GetItemCount(hwndDirList) - 1);
		} else {
			ListView_SetTextColor(hwndDirList, (bDefColorNoFilter) ? GetSysColor(COLOR_WINDOWTEXT) : colorNoFilter);
			ListView_RedrawItems(hwndDirList, 0, ListView_GetItemCount(hwndDirList) - 1);
		}
		return lret;
	}

	case WM_NCLBUTTONDOWN:
	case WM_NCMOUSEMOVE:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP: {
		HWND hwndTT = AsPointer<HWND>(SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0));

		if (wParam != HTCAPTION) {
			SendMessage(hwndTT, TTM_POP, 0, 0);
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}

		MSG msg;
		msg.hwnd = hwnd;
		msg.message = umsg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = GetMessageTime();
		msg.pt.x = GET_X_LPARAM(lParam);
		msg.pt.y = GET_Y_LPARAM(lParam);

		SendMessage(hwndTT, TTM_RELAYEVENT, 0, AsInteger<LPARAM>(&msg));
	}
	return DefWindowProc(hwnd, umsg, wParam, lParam);

	case WM_TIMER:
		// Check Change Notification Handle
		if (WAIT_OBJECT_0 == WaitForSingleObject(hChangeHandle, 0)) {
			// Store information about currently selected item
			DirListItem dli;
			dli.mask = DLI_ALL;
			dli.ntype = DLE_NONE;
			DirList_GetItem(hwndDirList, -1, &dli);

			FindNextChangeNotification(hChangeHandle);
			SendWMCommand(hwnd, IDM_VIEW_UPDATE);

			// must use SendMessage() !!
			if (dli.ntype != DLE_NONE) {
				DirList_SelectItem(hwndDirList, dli.szDisplayName, dli.szFileName);
			}
		}
		break;

	case WM_SIZE:
		MsgSize(hwnd, wParam, lParam);
		break;

	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hwnd, nIdFocus));
		break;

	case WM_DROPFILES: {
		WCHAR szBuf[MAX_PATH + 40];
		HDROP hDrop = AsPointer<HDROP>(wParam);

		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}

		//SetForegroundWindow(hwnd);
		DragQueryFile(hDrop, 0, szBuf, COUNTOF(szBuf));
		DisplayPath(szBuf, IDS_ERR_DROP1);

		//if (DragQueryFile(hDrop, UINT_MAX, nullptr, 0) > 1) {
		//	MsgBoxWarn(MB_OK, IDS_ERR_DROP2);
		//}

		DragFinish(hDrop);
	}
	break;

	case WM_COPYDATA: {
		PCOPYDATASTRUCT pcds = AsPointer<PCOPYDATASTRUCT>(lParam);

		if (pcds->dwData == DATA_MATEPATH_PATHARG) {
			LPCWSTR lpsz = static_cast<LPCWSTR>(pcds->lpData);
			DisplayPath(lpsz, IDS_ERR_CMDLINE);
		}
	}
	return TRUE;

	case WM_CONTEXTMENU: {
		const int nID = GetDlgCtrlID(AsPointer<HWND>(wParam));

		if (!(nID == IDC_DIRLIST || nID == IDC_DRIVEBOX || nID == IDC_TOOLBAR || nID == IDC_STATUSBAR || nID == IDC_REBAR)) {
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}

		HMENU hmenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND));

		int imenu = 0;
		switch (nID) {
		case IDC_DIRLIST:
			if (ListView_GetSelectedCount(hwndDirList)) {
				imenu = IDP_POPUP_SUBMENU_PATH;
			} else {
				imenu = IDP_POPUP_SUBMENU_LIST;
			}
			break;

		case IDC_DRIVEBOX:
			imenu = IDP_POPUP_SUBMENU_DRIVE;
			break;

		case IDC_TOOLBAR:
		case IDC_STATUSBAR:
		case IDC_REBAR:
			imenu = IDP_POPUP_SUBMENU_BAR;
			break;
		}

		const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		HMENU hMenuPopup = GetSubMenu(hmenu, imenu);
		if (imenu == IDP_POPUP_SUBMENU_PATH) {
			SetMenuDefaultItem(hMenuPopup, iDefaultOpenMenu, FALSE);
		}
		TrackPopupMenuEx(hMenuPopup,
						 TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
						 pt.x + 1, pt.y + 1, hwnd, nullptr);
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
		case SC_MINIMIZE | 0x02:
			ShowOwnedPopups(hwnd, FALSE);
			if (!bMinimizeToTray) {
				return DefWindowProc(hwnd, umsg, wParam, lParam);
			}
			MinimizeWndToTray(hwnd);
			ShowNotifyIcon(hwnd, true);
			break;

		case SC_RESTORE: {
			const LRESULT lrv = DefWindowProc(hwnd, umsg, wParam, lParam);
			ShowOwnedPopups(hwnd, TRUE);
			return lrv;
		}

		default:
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}
		break;

	case APPM_TRAYMESSAGE:
		switch (lParam) {
		case WM_RBUTTONUP: {
			HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND));
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
				ShowNotifyIcon(hwnd, false);
				RestoreWndFromTray(hwnd);
				ShowOwnedPopups(hwnd, TRUE);
			} else if (iCmd == IDM_TRAY_EXIT) {
				ShowNotifyIcon(hwnd, false);
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			}
		}
		return TRUE;

		case WM_LBUTTONUP:
			ShowNotifyIcon(hwnd, false);
			RestoreWndFromTray(hwnd);
			ShowOwnedPopups(hwnd, TRUE);
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

	default:
		if (umsg == msgTaskbarCreated) {
			if (!IsWindowVisible(hwnd)) {
				ShowNotifyIcon(hwnd, true);
			}
			return 0;
		}
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}
	return 0;
}

//=============================================================================
//
//  MsgCreate() - Handles WM_CREATE
//
//
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(wParam);
	hwndMain = hwnd;
	g_uCurrentDPI = GetWindowDPI(hwnd);
	HINSTANCE hInstance = (AsPointer<LPCREATESTRUCT>(lParam))->hInstance;
	const DWORD dwExStyle = IsAppThemed() ? 0 : WS_EX_CLIENTEDGE;
	hwndDirList = CreateWindowEx(
					  dwExStyle,
					  WC_LISTVIEW,
					  nullptr,
					  WS_DIRLIST,
					  0, 0, 0, 0,
					  hwnd,
					  AsPointer<HMENU, ULONG_PTR>(IDC_DIRLIST),
					  hInstance,
					  nullptr);

	InitWindowCommon(hwndDirList);

	const DWORD dwDriveBoxStyle = bShowDriveBox ? (WS_DRIVEBOX | WS_VISIBLE) : WS_DRIVEBOX;
	hwndDriveBox = CreateWindowEx(
					   0,
					   WC_COMBOBOXEX,
					   nullptr,
					   dwDriveBoxStyle,
					   0, 0, 0, GetSystemMetrics(SM_CYFULLSCREEN),
					   hwnd,
					   AsPointer<HMENU, ULONG_PTR>(IDC_DRIVEBOX),
					   hInstance,
					   nullptr);

	// Create Toolbar and Statusbar
	CreateBars(hwnd, hInstance);

	// Window Initialization
	// DriveBox
	DriveBox_Init(hwndDriveBox);
	ComboBox_SetExtendedUI(hwndDriveBox, TRUE);
	// DirList
	const LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, nullptr, -1, 0, 0, 0
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			, 0, 0, 0
#endif
	};
	ListView_SetExtendedListViewStyle(hwndDirList, LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
	ListView_InsertColumn(hwndDirList, 0, &lvc);
	DirList_Init(hwndDirList);
	if (bTrackSelect) {
		ListView_SetExtendedListViewStyleEx(hwndDirList,
											LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE,
											LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE);
	}
	if (bFullRowSelect) {
		ListView_SetExtendedListViewStyleEx(hwndDirList,
											LVS_EX_FULLROWSELECT,
											LVS_EX_FULLROWSELECT);
		SetExplorerTheme(hwndDirList);
	}

	ListView_SetHoverTime(hwndDirList, 50);
	// Drag & Drop
	DragAcceptFiles(hwnd, TRUE);
	// History
	mHistory.Init();
	mHistory.UpdateToolbar(hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
	// ToolTip with Current Directory
	TOOLINFO ti;
	memset(&ti, 0, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND;
	ti.hwnd = hwnd;
	ti.uId = AsInteger<UINT_PTR>(hwnd);
	ti.lpszText = LPSTR_TEXTCALLBACK;

	HWND hwndTT = AsPointer<HWND>(SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0));
	SendMessage(hwndTT, TTM_ADDTOOL, 0, AsInteger<LPARAM>(&ti));

	// System Menu
	HMENU hmenu = GetSystemMenu(hwnd, FALSE);

	// Remove unwanted items
	DeleteMenu(hmenu, SC_RESTORE, MF_BYCOMMAND);
	DeleteMenu(hmenu, SC_MAXIMIZE, MF_BYCOMMAND);

	// Mofify the L"Minimize" item
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_ID;
	GetMenuItemInfo(hmenu, SC_MINIMIZE, FALSE, &mii);
	mii.wID = SC_MINIMIZE | 0x02;
	SetMenuItemInfo(hmenu, SC_MINIMIZE, FALSE, &mii);
	return 0;
}

//=============================================================================
//
//  CreateBars() - Create Toolbar and Statusbar
//
//
void CreateBars(HWND hwnd, HINSTANCE hInstance) noexcept {
	constexpr DWORD dwToolbarStyle = WS_TOOLBAR | TBSTYLE_FLAT | CCS_ADJUSTABLE;
	hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr, dwToolbarStyle,
								 0, 0, 0, 0, hwnd, AsPointer<HMENU, ULONG_PTR>(IDC_TOOLBAR), hInstance, nullptr);

	SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	bool internalBitmap = false;
	const int scale = iAutoScaleToolbar;
#if NP2_ENABLE_HIDPI_IMAGE_RESOURCE
	const UINT dpi = (scale > USER_DEFAULT_SCREEN_DPI) ? (g_uCurrentDPI + scale - USER_DEFAULT_SCREEN_DPI) : g_uCurrentDPI;
#else
	const UINT dpi = g_uCurrentDPI;
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
	const DWORD cchIniSection = static_cast<DWORD>NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);

	section.Init(COUNTOF(tbbMainWnd));
	LoadIniSection(INI_SECTION_NAME_TOOLBAR_LABELS, pIniSectionBuf, cchIniSection);
	section.ParseArray(pIniSectionBuf);

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
				SendMessage(hwndToolbar, TB_GETEXTENDEDSTYLE, 0, 0) | TBSTYLE_EX_MIXEDBUTTONS);

	//SendMessage(hwndToolbar, TB_SAVERESTORE, FALSE, (LPARAM)lptbsp);
	if (Toolbar_SetButtons(hwndToolbar, tchToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd)) == 0) {
		Toolbar_SetButtons(hwndToolbar, DefaultToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd));
	}

	RECT rc;
	SendMessage(hwndToolbar, TB_GETITEMRECT, 0, AsInteger<LPARAM>(&rc));
	//SendMessage(hwndToolbar, TB_SETINDENT, 2, 0);

	const DWORD dwStatusbarStyle = bShowStatusbar ? (WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE) : (WS_CHILD | WS_CLIPSIBLINGS);
	hwndStatus = CreateStatusWindow(dwStatusbarStyle, nullptr, hwnd, IDC_STATUSBAR);

	// Create ReBar and add Toolbar
	const DWORD dwReBarStyle = bShowToolbar ? (WS_REBAR | WS_VISIBLE) : WS_REBAR;
	hwndReBar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, nullptr, dwReBarStyle,
							   0, 0, 0, 0, hwnd, AsPointer<HMENU, ULONG_PTR>(IDC_REBAR), hInstance, nullptr);

	REBARINFO rbi;
	rbi.cbSize = sizeof(REBARINFO);
	rbi.fMask  = 0;
	rbi.himl   = nullptr;
	SendMessage(hwndReBar, RB_SETBARINFO, 0, AsInteger<LPARAM>(&rbi));

	const BOOL bIsAppThemed = IsAppThemed();
	REBARBANDINFO rbBand;
	rbBand.cbSize  = sizeof(REBARBANDINFO);
	rbBand.fMask   = /*RBBIM_COLORS | RBBIM_TEXT | RBBIM_BACKGROUND | */
		RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE /*| RBBIM_SIZE*/;
	rbBand.fStyle  = /*RBBS_CHILDEDGE | *//*RBBS_BREAK |*/ RBBS_FIXEDSIZE /*| RBBS_GRIPPERALWAYS*/;
	if (bIsAppThemed) {
		rbBand.fStyle |= RBBS_CHILDEDGE;
	}
	rbBand.hbmBack = nullptr;
	rbBand.lpText     = const_cast<LPWSTR>(L"Toolbar");
	rbBand.hwndChild  = hwndToolbar;
	rbBand.cxMinChild = (rc.right - rc.left) * COUNTOF(tbbMainWnd);
	rbBand.cyMinChild = (rc.bottom - rc.top) + 2 * rc.top;
	rbBand.cx         = 0;
	SendMessage(hwndReBar, RB_INSERTBAND, static_cast<WPARAM>(-1), AsInteger<LPARAM>(&rbBand));

	SetWindowPos(hwndReBar, nullptr, 0, 0, 0, 0, SWP_NOZORDER);
	GetWindowRect(hwndReBar, &rc);
	cyReBar = rc.bottom - rc.top;

	cyReBarFrame = bIsAppThemed ? 0 : 2;
	cyDriveBoxFrame = bIsAppThemed ? 0 : 2;
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

	// recreate toolbar and statusbar
	WCHAR chStatus[255];
	SendMessage(hwndStatus, SB_GETTEXT, ID_FILEINFO, AsInteger<LPARAM>(chStatus));
	RecreateBars(hwnd, g_hInstance);

	const int cx = rc->right - rc->left;
	const int cy = rc->bottom - rc->top;
	SetWindowPos(hwnd, nullptr, rc->left, rc->top, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	if (bShowToolbar) {
		// on Window 8.1 when move Notepad4 to another monitor with same scaling settings
		// WM_DPICHANGED is sent with same DPI, and WM_SIZE is not sent after WM_DPICHANGED.
		SetWindowPos(hwndReBar, nullptr, 0, 0, cx, cyReBar, SWP_NOZORDER);
	}

	StatusSetText(hwndStatus, ID_FILEINFO, chStatus);
}

//=============================================================================
//
//  MsgThemeChanged() - Handles WM_THEMECHANGED
//
//
void MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HINSTANCE hInstance = GetWindowInstance(hwnd);

	DWORD dwExStyle = GetWindowExStyle(hwndDirList);
	if (IsAppThemed()) {
		dwExStyle &= ~WS_EX_CLIENTEDGE;
		if (bFullRowSelect) {
			SetExplorerTheme(hwndDirList);
		} else {
			SetListViewTheme(hwndDirList);
		}
	} else {
		dwExStyle |= WS_EX_CLIENTEDGE;
	}
	SetWindowExStyle(hwndDirList, dwExStyle);
	SetWindowPos(hwndDirList, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

	// recreate toolbar and statusbar
	WCHAR chStatus[255];
	SendMessage(hwndStatus, SB_GETTEXT, ID_FILEINFO, AsInteger<LPARAM>(chStatus));
	RecreateBars(hwnd, hInstance);

	RECT rc;
	GetClientRect(hwnd, &rc);
	SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
	StatusSetText(hwndStatus, ID_FILEINFO, chStatus);
}

//=============================================================================
//
//  MsgSize() - Handles WM_SIZE
//
//
void MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(hwnd);

	if (wParam == SIZE_MINIMIZED) {
		return;
	}

	RECT rc;
	constexpr int x = 0;
	int y = 0;
	const int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	if (bShowToolbar) {
		//SendMessage(hwndToolbar, WM_SIZE, 0, 0);
		//GetWindowRect(hwndToolbar, &rc);
		//y = (rc.bottom - rc.top);
		//cy -= (rc.bottom - rc.top);

		//SendMessage(hwndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rc);
		SetWindowPos(hwndReBar, nullptr, 0, 0, cx, cyReBar, SWP_NOZORDER);
		// the ReBar automatically sets the correct height
		// calling SetWindowPos() with the height of one toolbar button
		// causes the control not to temporarily use the whole client area
		// and prevents flickering

		//GetWindowRect(hwndReBar, &rc);
		y = cyReBar + cyReBarFrame;    // define
		cy -= cyReBar + cyReBarFrame;  // border
	}

	if (bShowStatusbar) {
		SendMessage(hwndStatus, WM_SIZE, 0, 0);
		GetWindowRect(hwndStatus, &rc);
		cy -= (rc.bottom - rc.top);
	}

	HDWP hdwp = BeginDeferWindowPos(2);
	DeferWindowPos(hdwp, hwndDriveBox, nullptr, x, y, cx, max(cy, 100), SWP_NOZORDER | SWP_NOACTIVATE);

	if (bShowDriveBox) {
		GetWindowRect(hwndDriveBox, &rc);
		y += (rc.bottom - rc.top) + cyDriveBoxFrame;
		cy -= (rc.bottom - rc.top) + cyDriveBoxFrame;
	}

	DeferWindowPos(hdwp, hwndDirList, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	EndDeferWindowPos(hdwp);

	// Rebuild DirList Columns
	ListView_SetColumnWidth(hwndDirList, 0, LVSCW_AUTOSIZE_USEHEADER);

	GetClientRect(hwndStatus, &rc);
	const int aWidth[1] = { -1 };
	SendMessage(hwndStatus, SB_SETPARTS, COUNTOF(aWidth), AsInteger<LPARAM>(aWidth));
	InvalidateRect(hwndStatus, nullptr, TRUE);
}

//=============================================================================
//
//  MsgInitMenu() - Handles WM_INITMENU
//
//
void MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(lParam);

	HMENU hmenu = AsPointer<HMENU>(wParam);

	int i = ListView_GetSelectedCount(hwndDirList);
	DirListItem dli;
	dli.mask = DLI_TYPE;
	dli.ntype = DLE_NONE;
	DirList_GetItem(hwndDirList, -1, &dli);

	EnableCmd(hmenu, IDM_FILE_LAUNCH, (i && dli.ntype == DLE_FILE));
	EnableCmd(hmenu, IDM_FILE_QUICKVIEW, (i && dli.ntype == DLE_FILE) && bHasQuickview);
	EnableCmd(hmenu, IDM_FILE_OPENWITH, i);
	EnableCmd(hmenu, IDM_FILE_CREATELINK, i);
	EnableCmd(hmenu, IDM_FILE_SAVEAS, (i && dli.ntype == DLE_FILE));
	EnableCmd(hmenu, IDM_FILE_COPYMOVE, i);
	EnableCmd(hmenu, IDM_FILE_DELETE, i);
	EnableCmd(hmenu, IDM_FILE_RENAME, i);

	i = (ComboBox_GetCurSel(hwndDriveBox) != CB_ERR);
	EnableCmd(hmenu, IDM_FILE_DRIVEPROP, i);

	CheckCmd(hmenu, IDM_VIEW_FOLDERS, (dwFillMask & DL_FOLDERS));
	CheckCmd(hmenu, IDM_VIEW_FILES, (dwFillMask & DL_NONFOLDERS));
	CheckCmd(hmenu, IDM_VIEW_HIDDEN, (dwFillMask & DL_INCLHIDDEN));

	EnableCmd(hmenu, IDM_VIEW_FILTERALL, HasFilter());

	CheckCmd(hmenu, IDM_VIEW_TOOLBAR, bShowToolbar);
	EnableCmd(hmenu, IDM_VIEW_CUSTOMIZETB, bShowToolbar);
	CheckCmd(hmenu, IDM_VIEW_AUTO_SCALE_TOOLBAR, iAutoScaleToolbar);
#if NP2_ENABLE_HIDPI_IMAGE_RESOURCE
	CheckCmd(hmenu, IDM_VIEW_USE_LARGE_TOOLBAR, iAutoScaleToolbar > USER_DEFAULT_SCREEN_DPI);
#endif
	CheckCmd(hmenu, IDM_VIEW_STATUSBAR, bShowStatusbar);
	CheckCmd(hmenu, IDM_VIEW_DRIVEBOX, bShowDriveBox);

	CheckMenuRadioItem(hmenu, IDM_SORT_NAME, IDM_SORT_DATE, IDM_SORT_NAME + nSortFlags, MF_BYCOMMAND);

	CheckCmd(hmenu, IDM_SORT_REVERSE, fSortRev);
	CheckCmd(hmenu, IDM_VIEW_ALWAYSONTOP, bAlwaysOnTop);

	i = StrNotEmpty(szIniFile) || StrNotEmpty(szIniFile2);
	EnableCmd(hmenu, IDM_VIEW_SAVESETTINGS, i);
}

//=============================================================================
//
//  MsgCommand() - Handles WM_COMMAND
//
//
LRESULT MsgCommand(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam)) {
	case IDC_DRIVEBOX:
		switch (HIWORD(wParam)) {
		case CBN_SETFOCUS:
			nIdFocus = IDC_DRIVEBOX;
			break;

		case CBN_CLOSEUP: {
			WCHAR tch[64];

			if (DriveBox_GetSelDrive(hwndDriveBox, tch, COUNTOF(tch), true) && !PathIsSameRoot(szCurDir, tch)) {
				if (!ChangeDirectory(hwnd, tch, true)) {
					MsgBoxWarn(MB_OK, IDS_ERR_CD);
					DriveBox_SelectDrive(hwndDriveBox, szCurDir);
				}
			}
			SetFocus(hwndDirList);
		}
		break;
		}
		break;

	case IDM_FILE_OPENSAME:
	case IDM_FILE_OPENNEW: {
		DirListItem dli = { DLI_ALL, DLE_NONE, L"", L"" };
		DirList_GetItem(hwndDirList, -1, &dli);

		switch (dli.ntype) {
		case DLE_DIR:
			if (!ChangeDirectory(hwnd, dli.szFileName, true)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
			break;

		case DLE_FILE: {
			const bool bOpenNew = LOWORD(wParam) == IDM_FILE_OPENNEW;
			WCHAR tch[MAX_PATH];
			BeginWaitCursor();
			if (PathGetLnkPath(dli.szFileName, tch)) {
				const DWORD dwAttr = GetFileAttributes(tch);
				if ((dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
					DisplayLnkFile(dli.szFileName, tch);
				} else {
					// Made sure link points to a file
					LaunchTarget(tch, bOpenNew);
				}
			} else {
				LaunchTarget(dli.szFileName, bOpenNew);
			}
			EndWaitCursor();
		} break;
		}
	}
	break;

	case IDM_FILE_RUN:
		RunDlg(hwnd);
		break;

	case IDM_FILE_LAUNCH: {
		if (!DirList_IsFileSelected(hwndDirList)) {
			MessageBeep(MB_OK);
			return 0;
		}

		DirListItem dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) < 0) {
			break;
		}

		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = nullptr;
		sei.lpFile = dli.szFileName;
		sei.lpParameters = nullptr;
		sei.lpDirectory = szCurDir;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);
	}
	break;

	case IDM_FILE_QUICKVIEW: {
		if (!DirList_IsFileSelected(hwndDirList)) {
			MessageBeep(MB_OK);
			return 0;
		}

		DirListItem dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) < 0) {
			break;
		}

		WCHAR szTmp[MAX_PATH];
		if (PathGetLnkPath(dli.szFileName, szTmp)) {
			GetShortPathName(szTmp, szTmp, COUNTOF(szTmp));
		} else {
			GetShortPathName(dli.szFileName, szTmp, COUNTOF(szTmp));
		}

		WCHAR szParam[MAX_PATH] = L"";
		if (StrNotEmpty(szQuickviewParams)) {
			StrCatBuff(szParam, szQuickviewParams, COUNTOF(szParam));
			StrCatBuff(szParam, L" ", COUNTOF(szParam));
		}
		StrCatBuff(szParam, szTmp, COUNTOF(szParam));

		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = nullptr;
		sei.lpFile = szQuickview;
		sei.lpParameters = szParam;
		sei.lpDirectory = szCurDir;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);
	}
	break;

	case IDM_FILE_OPENWITH: {
		if (!ListView_GetSelectedCount(hwndDirList)) {
			MessageBeep(MB_OK);
			return 0;
		}

		DirListItem dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) < 0) {
			break;
		}

		OpenWithDlg(hwnd, &dli);
	}
	break;

	case IDM_FILE_GOTO:
		GotoDlg(hwnd);
		break;

	case IDM_FILE_NEW: {
		WCHAR szNewFile[MAX_PATH];
		WCHAR szFilter[128];
		WCHAR szTitle[32];

		StrCpyEx(szNewFile, L"");
		GetString(IDS_FILTER_ALL, szFilter, COUNTOF(szFilter));
		PrepareFilterStr(szFilter);
		GetString(IDS_NEWFILE, szTitle, COUNTOF(szTitle));

		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = szFilter;
		ofn.lpstrFile = szNewFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = szTitle;
		ofn.lpstrInitialDir = szCurDir;
		ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT |
					OFN_NODEREFERENCELINKS | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		if (bUseXPFileDialog) {
			ofn.Flags |= OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK;
			ofn.lpfnHook = OpenSaveFileDlgHookProc;
		}

		if (!GetSaveFileName(&ofn)) {
			break;
		}

		HANDLE hFile = CreateFile(szNewFile,
						   GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   nullptr,
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   nullptr);

		if (hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);

			// Extract dir from filename
			WCHAR szPath[MAX_PATH];
			lstrcpy(szPath, szNewFile);
			PathRemoveFileSpec(szPath);
			SetCurrentDirectory(szPath);

			// Select new file
			SendWMCommand(hwnd, IDM_VIEW_UPDATE);
			SHFILEINFO shfi;
			SHGetFileInfo(szNewFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, szNewFile)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}
		} else {
			MsgBoxWarn(MB_OK, IDS_ERR_NEW);
		}
	}
	break;

	case IDM_FILE_NEWDIR: {
		WCHAR tchNewDir[MAX_PATH];

		if (NewDirDlg(hwnd, tchNewDir)) {
			if (!CreateDirectory(tchNewDir, nullptr)) {
				MsgBoxWarn(MB_OK, IDS_ERR_NEWDIR);
			}
		}
	}
	break;

	case IDM_FILE_CREATELINK: {
		WCHAR tchLinkDestination[MAX_PATH];

		DirListItem dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) < 0) {
			break;
		}

		if (GetDirectory(hwnd, IDS_CREATELINK, tchLinkDestination, nullptr)) {
			if (!PathCreateLnk(tchLinkDestination, dli.szFileName)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CREATELINK);
			}
		}
	}
	break;

	case IDM_FILE_SAVEAS: {
		if (!DirList_IsFileSelected(hwndDirList)) {
			MessageBeep(MB_OK);
			return 0;
		}

		DirListItem dli;
		dli.mask = DLI_ALL;
		if (DirList_GetItem(hwndDirList, -1, &dli) < 0) {
			break;
		}

		WCHAR szNewFile[MAX_PATH];
		lstrcpy(szNewFile, dli.szFileName);

		WCHAR szFilter[128];
		GetString(IDS_FILTER_ALL, szFilter, COUNTOF(szFilter));
		PrepareFilterStr(szFilter);

		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = szFilter;
		ofn.lpstrFile = szNewFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT |
					OFN_NODEREFERENCELINKS | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		if (bUseXPFileDialog) {
			ofn.Flags |= OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK;
			ofn.lpfnHook = OpenSaveFileDlgHookProc;
		}

		if (!GetSaveFileName(&ofn)) {
			break;
		}

		BeginWaitCursor();

		WCHAR tch[MAX_PATH];
		WCHAR fmt[64];
		FormatString(tch, fmt, IDS_SAVEFILE, dli.szDisplayName);
		StatusSetText(hwndStatus, ID_MENUHELP, tch);
		StatusSetSimple(hwndStatus, TRUE);
		InvalidateRect(hwndStatus, nullptr, TRUE);
		UpdateWindow(hwndStatus);

		const BOOL bSuccess = CopyFile(dli.szFileName, szNewFile, FALSE);

		if (!bSuccess) {
			MsgBoxWarn(MB_OK, IDS_ERR_SAVEAS1, dli.szDisplayName);
		}

		if (bSuccess && bClearReadOnly) {
			DWORD dwFileAttributes = GetFileAttributes(szNewFile);
			if (dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
				dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
				if (!SetFileAttributes(szNewFile, dwFileAttributes)) {
					MsgBoxWarn(MB_OK, IDS_ERR_SAVEAS2);
				}
			}
		}
		StatusSetSimple(hwndStatus, FALSE);

		EndWaitCursor();
	}
	break;

	case IDM_FILE_COPYMOVE:
		if (ListView_GetSelectedCount(hwndDirList)) {
			CopyMoveDlg(hwnd, &wFuncCopyMove);
		} else {
			MessageBeep(MB_OK);
		}
		break;

	case IDM_FILE_DELETE:
	case IDM_FILE_DELETE2:
	case IDM_FILE_DELETE3: {
		DirListItem dli;
		dli.mask = DLI_ALL;

		int iItem = DirList_GetItem(hwndDirList, -1, &dli);
		if (iItem < 0) {
			break;
		}

		WCHAR tch[512];
		memset(tch, 0, sizeof(tch));
		lstrcpy(tch, dli.szFileName);

		SHFILEOPSTRUCT shfos;
		memset(&shfos, 0, sizeof(SHFILEOPSTRUCT));
		shfos.hwnd = hwnd;
		shfos.wFunc = FO_DELETE;
		shfos.pFrom = tch;
		shfos.pTo = nullptr;
		if (fUseRecycleBin && (LOWORD(wParam) != IDM_FILE_DELETE2)) {
			shfos.fFlags = FOF_ALLOWUNDO;
		}
		if (fNoConfirmDelete || LOWORD(wParam) == IDM_FILE_DELETE3) {
			shfos.fFlags |= FOF_NOCONFIRMATION;
		}

		SHFileOperation(&shfos);

		// Check if there are any changes in the directory, then update!
		if (WAIT_OBJECT_0 == WaitForSingleObject(hChangeHandle, 0)) {

			SendWMCommand(hwnd, IDM_VIEW_UPDATE);
			if (iItem > 0) {
				iItem--;
			}
			iItem = min(iItem, ListView_GetItemCount(hwndDirList) - 1);
			ListView_SetItemState(hwndDirList, iItem, LVIS_FOCUSED, LVIS_FOCUSED);
			ListView_EnsureVisible(hwndDirList, iItem, FALSE);

			FindNextChangeNotification(hChangeHandle);
		}
	}
	break;

	case IDM_FILE_RENAME:
		if (ListView_GetSelectedCount(hwndDirList)) {
			RenameFileDlg(hwnd);
		} else {
			MessageBeep(MB_OK);
		}
		break;

	case IDM_FILE_PROPERTIES:
		if (!ListView_GetSelectedCount(hwndDirList)) {
			MessageBeep(MB_OK);
		} else {
			DirList_PropertyDlg(hwndDirList, -1);
		}
		break;

	case IDM_FILE_CHANGEDIR: {
		WCHAR tch[MAX_PATH];

		if (GetDirectory(hwnd, IDS_GETDIRECTORY, tch, nullptr)) {
			if (!ChangeDirectory(hwnd, tch, true)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		}
	}
	break;

	case IDM_FILE_DRIVEPROP:
		DriveBox_PropertyDlg(hwndDriveBox);
		break;

	case IDM_FILE_EXPLORER: {
		if (ListView_GetSelectedCount(hwndDirList)) {
			DirListItem dli;
			dli.mask = DLI_FILENAME;
			DirList_GetItem(hwndDirList, -1, &dli);
			OpenContainingFolder(hwnd, dli.szFileName, true);
		} else {
			OpenContainingFolder(hwnd, szCurDir, false);
		}
	}
	break;

	case IDM_VIEW_NEWWINDOW:
	case IDM_FILE_RESTART: {
		WCHAR szModuleName[MAX_PATH];
		WCHAR szParameters[1024];

		GetModuleFileName(nullptr, szModuleName, COUNTOF(szModuleName));
		GetRelaunchParameters(szParameters);
		const LONG_PTR result = AsInteger<LONG_PTR>(ShellExecute(hwnd, nullptr, szModuleName, szParameters, nullptr, SW_SHOWNORMAL));
		if (result > 32 && LOWORD(wParam) == IDM_FILE_RESTART) {
			DestroyWindow(hwnd);
		}
	}
	break;

	case IDM_VIEW_FOLDERS:
		if (dwFillMask & DL_FOLDERS) {
			dwFillMask &= (~DL_FOLDERS);
		} else {
			dwFillMask |= DL_FOLDERS;
		}
		SendWMCommand(hwnd, IDM_VIEW_UPDATE);
		ListView_EnsureVisible(hwndDirList, 0, FALSE); // not done by update
		break;

	case IDM_VIEW_FILES:
		if (dwFillMask & DL_NONFOLDERS) {
			dwFillMask &= (~DL_NONFOLDERS);
		} else {
			dwFillMask |= DL_NONFOLDERS;
		}
		SendWMCommand(hwnd, IDM_VIEW_UPDATE);
		ListView_EnsureVisible(hwndDirList, 0, FALSE); // not done by update
		break;

	case IDM_VIEW_HIDDEN:
		if (dwFillMask & DL_INCLHIDDEN) {
			dwFillMask &= (~DL_INCLHIDDEN);
		} else {
			dwFillMask |= DL_INCLHIDDEN;
		}
		SendWMCommand(hwnd, IDM_VIEW_UPDATE);
		ListView_EnsureVisible(hwndDirList, 0, FALSE); // not done by update
		break;

	case IDM_VIEW_FILTER:
		if (GetFilterDlg(hwnd)) {
			// Store information about currently selected item
			DirListItem dli;
			dli.mask = DLI_ALL;
			dli.ntype = DLE_NONE;
			DirList_GetItem(hwndDirList, -1, &dli);

			SendWMCommand(hwnd, IDM_VIEW_UPDATE);

			if (dli.ntype != DLE_NONE) {
				if (!DirList_SelectItem(hwndDirList, dli.szDisplayName, dli.szFileName)) {
					ListView_EnsureVisible(hwndDirList, 0, FALSE);
				}
			}
		}
		Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, HasFilter() ? TB_DEL_FILTER_BMP : TB_ADD_FILTER_BMP);
		break;

	case IDM_VIEW_FILTERALL:
		if (HasFilter()) {
			StrCpyEx(tchFilter, L"*.*");
			bNegFilter = false;

			// Store information about currently selected item
			DirListItem dli;
			dli.mask = DLI_ALL;
			dli.ntype = DLE_NONE;
			DirList_GetItem(hwndDirList, -1, &dli);

			SendWMCommand(hwnd, IDM_VIEW_UPDATE);

			if (dli.ntype != DLE_NONE) {
				if (!DirList_SelectItem(hwndDirList, dli.szDisplayName, dli.szFileName)) {
					ListView_EnsureVisible(hwndDirList, 0, FALSE);
				}
			}
		}
		Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, TB_ADD_FILTER_BMP);
		break;

	case IDM_VIEW_UPDATE:
		ChangeDirectory(hwnd, nullptr, true);
		break;

	case IDM_VIEW_FAVORITES:
		// Goto Favorites Directory
		DisplayPath(tchFavoritesDir, IDS_ERR_FAVORITES);
		break;

	case IDM_VIEW_EDITFAVORITES: {
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

	case IDM_VIEW_TOOLBAR:
		bShowToolbar = !bShowToolbar;
		ShowWindow(hwndReBar, bShowToolbar ? SW_SHOW : SW_HIDE);
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_CUSTOMIZETB:
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
		ShowWindow(hwndStatus, bShowStatusbar ? SW_SHOW : SW_HIDE);
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_DRIVEBOX:
		bShowDriveBox = !bShowDriveBox;
		ShowWindow(hwndDriveBox, bShowDriveBox ? SW_SHOW : SW_HIDE);
		if (!bShowDriveBox) {
			if (GetDlgCtrlID(GetFocus()) == IDC_DRIVEBOX) {
				SetFocus(hwndDirList);
			}
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_SAVESETTINGS:
		SaveSettingsNow();
		break;

	case IDM_VIEW_FINDTARGET:
		ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FINDTARGET), hwnd, FindTargetDlgProc, 0);
		break;

	case IDM_VIEW_OPTIONS: {
		const bool back = bWindowLayoutRTL;
		OptionsPropSheet(hwnd, g_hInstance);
		if (back != bWindowLayoutRTL) {
			SetWindowLayoutRTL(hwndDirList, bWindowLayoutRTL);
		}
		bHasQuickview = PathIsFile(szQuickview);
		iDefaultOpenMenu = bOpenFileInSameWindow ? IDM_FILE_OPENSAME : IDM_FILE_OPENNEW;
		iShiftOpenMenu = bOpenFileInSameWindow ? IDM_FILE_OPENNEW : IDM_FILE_OPENSAME;
	}
	break;

	case IDM_VIEW_ALWAYSONTOP:
		bAlwaysOnTop = !bAlwaysOnTop;
		SetWindowPos(hwnd, (bAlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		break;

	case IDM_VIEW_ABOUT:
		ThemedDialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
		break;

	case IDM_SORT_NAME:
	case IDM_SORT_SIZE:
	case IDM_SORT_TYPE:
	case IDM_SORT_DATE:
		nSortFlags = LOWORD(wParam) - IDM_SORT_NAME;
		DirList_Sort(hwndDirList, nSortFlags, fSortRev);
		break;

	case IDM_SORT_REVERSE:
		fSortRev = !fSortRev;
		DirList_Sort(hwndDirList, nSortFlags, fSortRev);
		break;

	case IDM_POP_COPY_PATHNAME:
	case IDM_POP_COPY_FILENAME: {
		DirListItem dli;
		dli.mask = DLI_FILENAME;
		DirList_GetItem(hwndDirList, -1, &dli);

		LPCWSTR path = dli.szFileName;
		if (LOWORD(wParam) == IDM_POP_COPY_FILENAME) {
			path = PathFindFileName(path);
		}
		SetClipData(hwnd, path);
	}
	break;

	case ACC_ESCAPE:
		if (ComboBox_GetDroppedState(hwndDriveBox)) {
			ComboBox_ShowDropdown(hwndDriveBox, FALSE);
		} else if (iEscFunction == EscFunction_Minimize) {
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		} else if (iEscFunction == EscFunction_Exit) {
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;

	case ACC_NEXTCTL:
	case ACC_PREVCTL: {
		int nId = GetDlgCtrlID(GetFocus());

		if (LOWORD(wParam) == ACC_NEXTCTL) {
			if (++nId > IDC_DIRLIST) {
				nId = IDC_DRIVEBOX;
			}
		} else {
			if (--nId < IDC_DRIVEBOX) {
				nId = IDC_DIRLIST;
			}
		}

		if (nId == IDC_DRIVEBOX && !bShowDriveBox) {
			nId = IDC_DIRLIST;
		}

		SetFocus(GetDlgItem(hwnd, nId));
	}
	break;

	case ACC_TOGGLE_FOCUSEDIT:
		bFocusEdit = !bFocusEdit;
		break;

	case ACC_SWITCHTRANSPARENCY:
		bTransparentMode = !bTransparentMode;
		SetWindowTransparentMode(hwnd, bTransparentMode, iOpacityLevel);
		break;

	case ACC_GOTOTARGET: {
		DirListItem dli = { DLI_ALL, DLE_NONE, L"", L"" };
		DirList_GetItem(hwndDirList, -1, &dli);

		if (dli.ntype == DLE_FILE) {
			WCHAR szFullPath[MAX_PATH];

			//SetFocus(hwndDirList);
			if (PathGetLnkPath(dli.szFileName, szFullPath)) {
				if (GetFileAttributes(szFullPath) != INVALID_FILE_ATTRIBUTES) {
					WCHAR szDir[MAX_PATH];
					lstrcpy(szDir, szFullPath);
					WCHAR *p = StrRChr(szDir, nullptr, L'\\');
					if (p != nullptr) {
						p[1] = L'\0';
						if (!PathIsRoot(szDir)) {
							*p = L'\0';
						}

						SetCurrentDirectory(szDir);
						SendWMCommand(hwnd, IDM_VIEW_UPDATE);
						if (!DirList_SelectItem(hwndDirList, nullptr, szFullPath)) {
							ListView_EnsureVisible(hwndDirList, 0, FALSE);
						}
					}
				}
			}
		}
	}
	break;

	case ACC_SELECTTARGET:
		ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FINDTARGET), hwnd, FindTargetDlgProc, 0);
		break;

	case ACC_FIRETARGET:
		LaunchTarget(L"", true);
		break;

	case ACC_SNAPTOTARGET:
		SnapToTarget(hwnd);
		break;

	case ACC_DEFAULTWINPOS:
		SnapToDefaultPos(hwnd);
		break;

	case ACC_SELECTINIFILE:
		if (StrNotEmpty(szIniFile)) {
			CreateIniFile(szIniFile);
			DisplayPath(szIniFile, IDS_ERR_INIOPEN);
		}
		break;

	case IDT_HISTORY_BACK:
		if (mHistory.CanBack()) {
			WCHAR tch[MAX_PATH];
			mHistory.Back(tch, COUNTOF(tch));
			if (!ChangeDirectory(hwnd, tch, false)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		} else {
			MessageBeep(MB_OK);
		}
		mHistory.UpdateToolbar(hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		break;

	case IDT_HISTORY_FORWARD:
		if (mHistory.CanForward()) {
			WCHAR tch[MAX_PATH];
			mHistory.Forward(tch, COUNTOF(tch));
			if (!ChangeDirectory(hwnd, tch, false)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		} else {
			MessageBeep(MB_OK);
		}
		mHistory.UpdateToolbar(hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		break;

	case IDT_UP_DIR: {
		if (!PathIsRoot(szCurDir)) {
			if (!ChangeDirectory(hwnd, L"..", true)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		} else {
			MessageBeep(MB_OK);
		}
	}
	break;

	case IDT_ROOT_DIR: {
		if (!PathIsRoot(szCurDir)) {
			if (!ChangeDirectory(hwnd, L"\\", true)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		} else {
			MessageBeep(MB_OK);
		}
	}
	break;

	case IDT_VIEW_FAVORITES:
		SendWMCommand(hwnd, IDM_VIEW_FAVORITES);
		break;

	case IDT_FILE_NEXT: {
		const int iItem = ListView_GetNextItem(hwndDirList, -1, LVNI_ALL | LVNI_FOCUSED);
		const int d = ListView_GetSelectedCount(hwndDirList) ? 1 : 0;

		DirListItem dli;
		dli.ntype = DLE_NONE;
		dli.mask = DLI_TYPE | DLI_FILENAME;

		int i;
		WCHAR tch[MAX_PATH];
		for (i = iItem + d; DirList_GetItem(hwndDirList, i, &dli) != (-1); i++) {
			if (dli.ntype == DLE_FILE && !(PathGetLnkPath(dli.szFileName, tch) && PathIsDirectory(tch))) {
				break;
			}
		}

		if (dli.ntype != DLE_FILE) {
			for (i = 0; i <= iItem; i++) {
				DirList_GetItem(hwndDirList, i, &dli);
				if (dli.ntype == DLE_FILE && !(PathGetLnkPath(dli.szFileName, tch) && PathIsDirectory(tch))) {
					break;
				}
			}
		}

		if (dli.ntype == DLE_FILE) {
			ListView_SetItemState(hwndDirList, i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			ListView_EnsureVisible(hwndDirList, i, FALSE);
			ListView_Update(hwndDirList, i);
			SendWMCommand(hwnd, iDefaultOpenMenu);
		} else {
			MessageBeep(MB_OK);
		}
	}
	break;

	case IDT_FILE_PREV: {
		const int iItem = ListView_GetNextItem(hwndDirList, -1, LVNI_ALL | LVNI_FOCUSED);
		const int d = (ListView_GetSelectedCount(hwndDirList) || iItem == 0) ? 1 : 0;

		DirListItem dli;
		dli.ntype = DLE_NONE;
		dli.mask = DLI_TYPE | DLI_FILENAME;

		int i;
		WCHAR tch[MAX_PATH];
		for (i = iItem - d; i > (-1); i--) {
			DirList_GetItem(hwndDirList, i, &dli);
			if (dli.ntype == DLE_FILE && !(PathGetLnkPath(dli.szFileName, tch) && PathIsDirectory(tch))) {
				break;
			}
		}

		if (dli.ntype != DLE_FILE) {
			for (i = ListView_GetItemCount(hwndDirList) - 1; i >= iItem; i--) {
				DirList_GetItem(hwndDirList, i, &dli);
				if (dli.ntype == DLE_FILE && !(PathGetLnkPath(dli.szFileName, tch) && PathIsDirectory(tch))) {
					break;
				}
			}
		}

		if (dli.ntype == DLE_FILE) {
			ListView_SetItemState(hwndDirList, i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			ListView_EnsureVisible(hwndDirList, i, FALSE);
			ListView_Update(hwndDirList, i);
			SendWMCommand(hwnd, iDefaultOpenMenu);
		} else {
			MessageBeep(MB_OK);
		}
	}
	break;

	case IDT_FILE_RUN:
		SendWMCommand(hwnd, IDM_FILE_RUN);
		break;

	case IDT_FILE_QUICKVIEW:
		if (DirList_IsFileSelected(hwndDirList)) {
			SendWMCommand(hwnd, IDM_FILE_QUICKVIEW);
		} else {
			MessageBeep(MB_OK);
		}
		break;

	case IDT_FILE_SAVEAS:
		if (DirList_IsFileSelected(hwndDirList)) {
			SendWMCommand(hwnd, IDM_FILE_SAVEAS);
		} else {
			MessageBeep(MB_OK);
		}
		break;

	case IDT_FILE_COPYMOVE:
		if (ListView_GetSelectedCount(hwndDirList)) {
			SendWMCommand(hwnd, IDM_FILE_COPYMOVE);
		} else {
			MessageBeep(MB_OK);
		}
		break;

	case IDT_FILE_DELETE_RECYCLE:
		if (ListView_GetSelectedCount(hwndDirList)) {
			const bool fUseRecycleBin2 = fUseRecycleBin;
			fUseRecycleBin = true;
			SendWMCommand(hwnd, IDM_FILE_DELETE);
			fUseRecycleBin = fUseRecycleBin2;
		} else {
			MessageBeep(MB_OK);
		}
		break;

	case IDT_FILE_DELETE_PERM:
		if (ListView_GetSelectedCount(hwndDirList)) {
			SendWMCommand(hwnd, IDM_FILE_DELETE2);
		} else {
			MessageBeep(MB_OK);
		}
		break;

	case IDT_VIEW_FILTER:
		SendWMCommand(hwnd, IDM_VIEW_FILTER);
		break;
	}

	return 0;
}

//=============================================================================
//
//  MsgNotify() - Handles WM_NOTIFY
//
//
LRESULT MsgNotify(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	LPNMHDR pnmh = AsPointer<LPNMHDR>(lParam);

	switch (pnmh->idFrom) {
	case IDC_DIRLIST:
		switch (pnmh->code) {
		case NM_SETFOCUS:
			nIdFocus = IDC_DIRLIST;
			break;

		case LVN_GETDISPINFO:
			DirList_GetDispInfo(hwndDirList, lParam);
			break;

		case LVN_DELETEITEM:
			DirList_DeleteItem(hwndDirList, lParam);
			break;

		case LVN_BEGINDRAG:
		case LVN_BEGINRDRAG:
			DirList_DoDragDrop(hwndDirList, lParam);
			break;

		case LVN_ITEMCHANGED: {
			const NM_LISTVIEW *pnmlv = AsPointer<NM_LISTVIEW *>(lParam);

			if ((pnmlv->uNewState & (LVIS_SELECTED | LVIS_FOCUSED)) !=
					(pnmlv->uOldState & (LVIS_SELECTED | LVIS_FOCUSED))) {

				WCHAR tch[64];
				if ((pnmlv->uNewState & LVIS_SELECTED)) {
					WIN32_FIND_DATA fd;
					DirListItem dli;
					dli.mask  = DLI_FILENAME;
					dli.ntype = DLE_NONE;
					DirList_GetItem(hwndDirList, -1, &dli);
					DirList_GetItemEx(hwndDirList, -1, &fd);

					if (fd.nFileSizeLow >= MAXDWORD) {
						GetFileAttributesEx(dli.szFileName, GetFileExInfoStandard, &fd);
					}

					const LONGLONG isize = (static_cast<LONGLONG>(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;
					WCHAR tchsize[64];
					StrFormatByteSize(isize, tchsize, COUNTOF(tchsize));

					WCHAR tchdate[64];
					WCHAR tchtime[64];
					FILETIME ft;
					SYSTEMTIME st;
					FileTimeToLocalFileTime(&fd.ftLastWriteTime, &ft);
					FileTimeToSystemTime(&ft, &st);
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
					GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, tchdate, COUNTOF(tchdate), nullptr);
					GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, nullptr, tchtime, COUNTOF(tchtime));
#else
					GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, tchdate, COUNTOF(tchdate));
					GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, nullptr, tchtime, COUNTOF(tchtime));
#endif

					WCHAR tchattr[6];
					tchattr[0] = (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? L'A' : L'-';
					tchattr[1] = (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? L'R' : L'-';
					tchattr[2] = (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? L'H' : L'-';
					tchattr[3] = (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? L'S' : L'-';
					tchattr[4] = L'\0';

					wsprintf(tch, L"%s | %s %s | %s", tchsize, tchdate, tchtime, tchattr);
				} else {
					WCHAR tchnum[64];
					WCHAR fmt[64];
					FormatNumber(tchnum, ListView_GetItemCount(hwndDirList));
					FormatString(tch, fmt, HasFilter() ? IDS_NUMFILES_FILTER : IDS_NUMFILES, tchnum);
				}

				StatusSetText(hwndStatus, ID_FILEINFO, tch);
			}
		}
		break;

		case NM_CLICK:
			if (bSingleClick && ListView_GetSelectedCount(hwndDirList)) {
				if (KeyboardIsKeyDown(VK_MENU)) {
					SendWMCommand(hwnd, IDM_FILE_PROPERTIES);
				} else if (KeyboardIsKeyDown(VK_SHIFT)) {
					SendWMCommand(hwnd, iShiftOpenMenu);
				} else {
					SendWMCommand(hwnd, iDefaultOpenMenu);
				}
			}
			break;

		case NM_DBLCLK:
		case NM_RETURN:
			if (KeyboardIsKeyDown(VK_MENU)) {
				SendWMCommand(hwnd, IDM_FILE_PROPERTIES);
			} else if (KeyboardIsKeyDown(VK_SHIFT)) {
				SendWMCommand(hwnd, iShiftOpenMenu);
			} else {
				SendWMCommand(hwnd, iDefaultOpenMenu);
			}
			break;
		}
		break;

	case IDC_DRIVEBOX:
		switch (pnmh->code) {
		case CBEN_GETDISPINFO:
			DriveBox_GetDispInfo(hwndDriveBox, lParam);
			break;

		case CBEN_DELETEITEM:
			DriveBox_DeleteItem(hwndDriveBox, lParam);
			break;
		}
		break;

	case IDC_TOOLBAR:
		switch (pnmh->code) {
		case TBN_ENDADJUST:
			mHistory.UpdateToolbar(hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
			Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, HasFilter() ? TB_DEL_FILTER_BMP : TB_ADD_FILTER_BMP);
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
		}
		break;

	default:
		switch (pnmh->code) {
		case TTN_NEEDTEXT: {
			LPTOOLTIPTEXT pTTT = AsPointer<LPTOOLTIPTEXT>(lParam);
			if (pTTT->uFlags & TTF_IDISHWND) {
				PathCompactPathEx(pTTT->szText, szCurDir, COUNTOF(pTTT->szText), 0);
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

void SetWindowPathTitle(HWND hwnd, LPCWSTR lpszFile) noexcept {
	WCHAR szTitle[MAX_PATH] = L"";
	if (StrNotEmpty(lpszFile)) {
		if (!PathIsRoot(lpszFile)) {
			SHFILEINFO shfi;
			SHGetFileInfo(lpszFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			lstrcpy(szTitle, shfi.szDisplayName);
		} else {
			lstrcpy(szTitle, lpszFile);
		}
	}

	SetWindowText(hwnd, szTitle);
}

//=============================================================================
//
//  ChangeDirectory()
//
//
bool ChangeDirectory(HWND hwnd, LPCWSTR lpszNewDir, bool bUpdateHistory) {
	if (lpszNewDir && !SetCurrentDirectory(lpszNewDir)) {
		return false;
	}

	const bool fUpdate = lpszNewDir != nullptr;
	if (fUpdate) {
		// Update call
		WCHAR szTest[MAX_PATH];

		GetCurrentDirectory(COUNTOF(szTest), szTest);
		if (!PathIsDirectory(szTest)) {
			WCHAR szWinDir[MAX_PATH];
			GetWindowsDirectory(szWinDir, COUNTOF(szWinDir));
			SetCurrentDirectory(szWinDir);
			MsgBoxWarn(MB_OK, IDS_ERR_CD);
		}
	}

	BeginWaitCursor();
	{
		const int iTopItem = ListView_GetTopIndex(hwndDirList);

		GetCurrentDirectory(COUNTOF(szCurDir), szCurDir);
		SetWindowPathTitle(hwnd, szCurDir);

		if (HasFilter()) {
			ListView_SetTextColor(hwndDirList, (bDefColorFilter) ? GetSysColor(COLOR_WINDOWTEXT) : colorFilter);
			Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, TB_DEL_FILTER_BMP);
		} else {
			ListView_SetTextColor(hwndDirList, (bDefColorNoFilter) ? GetSysColor(COLOR_WINDOWTEXT) : colorNoFilter);
			Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, TB_ADD_FILTER_BMP);
		}

		const int cItems = DirList_Fill(hwndDirList, szCurDir, dwFillMask, tchFilter, bNegFilter, flagNoFadeHidden, nSortFlags, fSortRev);
		DirList_StartIconThread(hwndDirList);

		// Get long pathname
		DirList_GetLongPathName(hwndDirList, szCurDir);
		SetCurrentDirectory(szCurDir);

		if (cItems > 0) {
			ListView_SetItemState(hwndDirList, 0, LVIS_FOCUSED, LVIS_FOCUSED);
		}

		// new directory -- scroll to (0, 0)
		if (!fUpdate) {
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
		} else {
			const int iJump = min(iTopItem + ListView_GetCountPerPage(hwndDirList), cItems - 1);
			ListView_EnsureVisible(hwndDirList, iJump, TRUE);
			ListView_EnsureVisible(hwndDirList, iTopItem, TRUE);
		}

		// setup new change notification handle
		FindCloseChangeNotification(hChangeHandle);
		hChangeHandle = FindFirstChangeNotification(szCurDir, FALSE,
						FILE_NOTIFY_CHANGE_FILE_NAME  | \
						FILE_NOTIFY_CHANGE_DIR_NAME   | \
						FILE_NOTIFY_CHANGE_ATTRIBUTES | \
						FILE_NOTIFY_CHANGE_SIZE | \
						FILE_NOTIFY_CHANGE_LAST_WRITE);

		DriveBox_Fill(hwndDriveBox);
		DriveBox_SelectDrive(hwndDriveBox, szCurDir);

		WCHAR tch[256];
		WCHAR tchnum[64];
		FormatNumber(tchnum, cItems);
		WCHAR fmt[64];
		FormatString(tch, fmt, HasFilter() ? IDS_NUMFILES_FILTER : IDS_NUMFILES, tchnum);
		StatusSetText(hwndStatus, ID_FILEINFO, tch);

		// Update History
		if (bUpdateHistory) {
			mHistory.Add(szCurDir);
			mHistory.UpdateToolbar(hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		}
	}
	EndWaitCursor();

	return true;
}

static void GetWindowPositionSectionName(HMONITOR hMonitor, WCHAR (&sectionName)[96]) noexcept {
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	const int cxScreen = mi.rcMonitor.right - mi.rcMonitor.left;
	const int cyScreen = mi.rcMonitor.bottom - mi.rcMonitor.top;

	wsprintf(sectionName, L"%s %ix%i", INI_SECTION_NAME_WINDOW_POSITION, cxScreen, cyScreen);
}

#if NP2_ENABLE_APP_LOCALIZATION_DLL
void ValidateUILangauge() noexcept {
	const LANGID subLang = SUBLANGID(uiLanguage);
	switch (PRIMARYLANGID(uiLanguage)) {
	case LANG_ENGLISH:
		languageResID = IDS_LANG_ENGLISH_US;
		break;
	case LANG_CHINESE:
		languageResID = IsChineseTraditionalSubLang(subLang)? IDS_LANG_CHINESE_TRADITIONAL : IDS_LANG_CHINESE_SIMPLIFIED;
		break;
	case LANG_FRENCH:
		languageResID = IDS_LANG_FRENCH_FRANCE;
		break;
	case LANG_GERMAN:
		languageResID = IDS_LANG_GERMAN;
		break;
	case LANG_ITALIAN:
		languageResID = IDS_LANG_ITALIAN;
		break;
	case LANG_JAPANESE:
		languageResID = IDS_LANG_JAPANESE;
		break;
	case LANG_KOREAN:
		languageResID = IDS_LANG_KOREAN;
		break;
	case LANG_POLISH:
		languageResID = IDS_LANG_POLISH;
		break;
	case LANG_PORTUGUESE:
		languageResID = IDS_LANG_PORTUGUESE_BRAZIL;
		break;
	case LANG_RUSSIAN:
		languageResID = IDS_LANG_RUSSIAN;
		break;
	case LANG_NEUTRAL:
	default:
		languageResID = IDS_LANG_USER_DEFAULT;
		uiLanguage = LANG_USER_DEFAULT;
		break;
	}
}

void SetUILanguage(int resID) noexcept {
	LANGID lang = uiLanguage;
	switch (resID) {
	case IDS_LANG_USER_DEFAULT:
		lang = LANG_USER_DEFAULT;
		break;
	case IDS_LANG_ENGLISH_US:
		lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		break;
	case IDS_LANG_CHINESE_SIMPLIFIED:
		lang = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
		break;
	case IDS_LANG_CHINESE_TRADITIONAL:
		lang = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL);
		break;
	case IDS_LANG_FRENCH_FRANCE:
		lang = MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
		break;
	case IDS_LANG_GERMAN:
		lang = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN);
		break;
	case IDS_LANG_ITALIAN:
		lang = MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN);
		break;
	case IDS_LANG_JAPANESE:
		lang = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
		break;
	case IDS_LANG_KOREAN:
		lang = MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT);
		break;
	case IDS_LANG_POLISH:
		lang = MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT);
		break;
	case IDS_LANG_PORTUGUESE_BRAZIL:
		lang = MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN);
		break;
	case IDS_LANG_RUSSIAN:
		lang = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
		break;
	}

	if (uiLanguage == lang) {
		return;
	}

	const int result = MsgBoxInfo(MB_YESNOCANCEL, IDS_CHANGE_LANG_RESTART);
	if (result != IDCANCEL) {
		languageResID = resID;
		uiLanguage = lang;
		IniSetInt(INI_SECTION_NAME_FLAGS, L"UILanguage", lang);
		if (result == IDYES) {
			PostWMCommand(hwndMain, IDM_FILE_RESTART);
		}
	}
}
#endif

//=============================================================================
//
//  LoadSettings()
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
	int iValue = section.GetInt(L"StartupDirectory", StartupDirectory_MRU);
	iStartupDir = clamp(static_cast<StartupDirectory>(iValue), StartupDirectory_None, StartupDirectory_Favorite);
	section.GetString(L"MRUDirectory", L"", szMRUDirectory);

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

	strValue = section.GetValue(L"Quikview.exe");
	if (StrIsEmpty(strValue)) {
		GetSystemDirectory(szQuickview, COUNTOF(szQuickview));
		PathAddBackslash(szQuickview);
		lstrcat(szQuickview, L"Viewers\\Quikview.exe");
	} else {
		PathAbsoluteFromApp(strValue, szQuickview, true);
	}

	bHasQuickview = PathIsFile(szQuickview);
	section.GetString(L"QuikviewParams", L"", szQuickviewParams);

	POINT pt;
	pt.x = section.GetInt(L"WindowPosX", 0);
	pt.y = section.GetInt(L"WindowPosY", 0);

	bSingleClick = section.GetBool(L"SingleClick", true);
	bOpenFileInSameWindow = section.GetBool(L"OpenFileInSameWindow", false);
	iDefaultOpenMenu = bOpenFileInSameWindow ? IDM_FILE_OPENSAME : IDM_FILE_OPENNEW;
	iShiftOpenMenu = bOpenFileInSameWindow ? IDM_FILE_OPENNEW : IDM_FILE_OPENSAME;

	bTrackSelect = section.GetBool(L"TrackSelect", true);
	bFullRowSelect = section.GetBool(L"FullRowSelect", false);
	fUseRecycleBin = section.GetBool(L"UseRecycleBin", true);
	fNoConfirmDelete = section.GetBool(L"NoConfirmDelete", false);
	bClearReadOnly = section.GetBool(L"ClearReadOnly", true);
	bRenameOnCollision = section.GetBool(L"RenameOnCollision", false);
	bFocusEdit = section.GetBool(L"FocusEdit", true);
	bAlwaysOnTop = section.GetBool(L"AlwaysOnTop", false);
	bMinimizeToTray = section.GetBool(L"MinimizeToTray", false);
	bTransparentMode = section.GetBool(L"TransparentMode", false);
	bWindowLayoutRTL = section.GetBool(L"WindowLayoutRTL", false);

	iValue = section.GetInt(L"EscFunction", EscFunction_None);
	iEscFunction = clamp(static_cast<EscFunction>(iValue), EscFunction_None, EscFunction_Exit);

	if (IsVistaAndAbove()) {
		bUseXPFileDialog = section.GetBool(L"UseXPFileDialog", false);
	} else {
		bUseXPFileDialog = true;
	}

	dwFillMask = section.GetInt(L"FillMask", DL_ALLOBJECTS);
	if (dwFillMask & ~DL_ALLOBJECTS) {
		dwFillMask = DL_ALLOBJECTS;
	}

	iValue = section.GetInt(L"SortOptions", DS_NAME);
	nSortFlags = clamp(iValue, DS_NAME, DS_LASTMOD);

	fSortRev = section.GetBool(L"SortReverse", false);

	if (!lpFilterArg) {
		strValue = section.GetValue(L"FileFilter");
		if (StrIsEmpty(strValue)) {
			StrCpyEx(tchFilter, L"*.*");
		} else {
			lstrcpyn(tchFilter, strValue, COUNTOF(tchFilter));
		}
		bNegFilter = section.GetBool(L"NegativeFilter", false);
	} else { // ignore filter if /m was specified
		if (*lpFilterArg == L'-') {
			bNegFilter = true;
			lstrcpyn(tchFilter, lpFilterArg + 1, COUNTOF(tchFilter));
		} else {
			bNegFilter = false;
			lstrcpyn(tchFilter, lpFilterArg, COUNTOF(tchFilter));
		}
	}

	bDefColorNoFilter = section.GetBool(L"DefColorNoFilter", true);
	bDefColorFilter = section.GetBool(L"DefColorFilter", true);

	colorNoFilter = section.GetInt(L"ColorNoFilter", GetSysColor(COLOR_WINDOWTEXT));
	colorFilter = section.GetInt(L"ColorFilter", GetSysColor(COLOR_HIGHLIGHT));

	strValue = section.GetValue(L"ToolbarButtons");
	if (StrIsEmpty(strValue)) {
		memcpy(tchToolbarButtons, DefaultToolbarButtons, sizeof(DefaultToolbarButtons));
	} else {
		lstrcpyn(tchToolbarButtons, strValue, COUNTOF(tchToolbarButtons));
	}

	bShowToolbar = section.GetBool(L"ShowToolbar", true);
	iAutoScaleToolbar = section.GetInt(L"AutoScaleToolbar", USER_DEFAULT_SCREEN_DPI);
	bShowStatusbar = section.GetBool(L"ShowStatusbar", true);
	bShowDriveBox = section.GetBool(L"ShowDriveBox", true);

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
		}

		cxRunDlg = section.GetInt(L"RunDlgSizeX", 0);
		cxGotoDlg = section.GetInt(L"GotoDlgSizeX", 0);
		cxFileFilterDlg = section.GetInt(L"FileFilterDlgX", 0);
		cxRenameFileDlg = section.GetInt(L"RenameFileDlgX", 0);
		cxNewDirectoryDlg = section.GetInt(L"NewDirectoryDlgX", 0);
		cxOpenWithDlg = section.GetInt(L"OpenWithDlgSizeX", 0);
		cyOpenWithDlg = section.GetInt(L"OpenWithDlgSizeY", 0);
		cxCopyMoveDlg = section.GetInt(L"CopyMoveDlgSizeX", 0);
		cxTargetApplicationDlg = section.GetInt(L"TargetApplicationDlgSizeX", 0);
		cxFindWindowDlg = section.GetInt(L"FindWindowDlgSizeX", 0);
	}

	section.Free();
	NP2HeapFree(pIniSectionBuf);

	// Initialize custom colors for ChooseColor()
	colorCustom[0] = RGB(0, 0, 128);
	colorCustom[8]  = RGB(255, 255, 226);
	colorCustom[1] = GetSysColor(COLOR_WINDOWTEXT);
	colorCustom[9]  = GetSysColor(COLOR_WINDOW);
	colorCustom[2] = GetSysColor(COLOR_INFOTEXT);
	colorCustom[10] = GetSysColor(COLOR_INFOBK);
	colorCustom[3] = GetSysColor(COLOR_HIGHLIGHTTEXT);
	colorCustom[11] = GetSysColor(COLOR_HIGHLIGHT);
	colorCustom[4] = GetSysColor(COLOR_ACTIVECAPTION);
	colorCustom[12] = GetSysColor(COLOR_DESKTOP);
	colorCustom[5] = GetSysColor(COLOR_3DFACE);
	colorCustom[13] = GetSysColor(COLOR_3DFACE);
	colorCustom[6] = GetSysColor(COLOR_3DFACE);
	colorCustom[14] = GetSysColor(COLOR_3DFACE);
	colorCustom[7] = GetSysColor(COLOR_3DFACE);
	colorCustom[15] = GetSysColor(COLOR_3DFACE);
}

void SaveSettingsNow() noexcept {
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
		if (WritePrivateProfileString(INI_SECTION_NAME_SETTINGS, L"WriteTest", L"ok", szIniFile)) {
			BeginWaitCursor();
			if (CreateIniFile(szIniFile)) {
				SaveSettings(true);
			} else {
				bCreateFailure = true;
			}
			EndWaitCursor();
			if (!bCreateFailure) {
				MsgBoxInfo(MB_OK, IDS_SAVESETTINGS);
			}
		} else {
			MsgBoxWarn(MB_OK, IDS_ERR_INIWRITE);
		}
	}
	if (bCreateFailure) {
		MsgBoxWarn(MB_OK, IDS_ERR_INICREATE);
	}
}

//=============================================================================
//
//  SaveSettings()
//
//
void SaveSettings(bool bSaveSettingsNow) noexcept {
	if (!CreateIniFile(szIniFile)) {
		return;
	}

	if (!bSaveSettings && !bSaveSettingsNow) {
		if (iStartupDir == StartupDirectory_MRU) {
			IniSetString(INI_SECTION_NAME_SETTINGS, L"MRUDirectory", szCurDir);
		}
		IniSetBoolEx(INI_SECTION_NAME_SETTINGS, L"SaveSettings", bSaveSettings, true);
		return;
	}

	WCHAR wchTmp[MAX_PATH];
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS));
	SaveWindowPosition(pIniSectionBuf);
	memset(pIniSectionBuf, 0, 2*sizeof(WCHAR));

	IniSectionBuilder section = { pIniSectionBuf };

	section.SetBoolEx(L"SaveSettings", bSaveSettings, true);
	section.SetIntEx(L"StartupDirectory", static_cast<int>(iStartupDir), StartupDirectory_MRU);
	if (iStartupDir == StartupDirectory_MRU) {
		section.SetString(L"MRUDirectory", szCurDir);
	}
	PathRelativeToApp(tchFavoritesDir, wchTmp, FILE_ATTRIBUTE_DIRECTORY, true, flagPortableMyDocs);
	section.SetString(L"Favorites", wchTmp);
	PathRelativeToApp(szQuickview, wchTmp, FILE_ATTRIBUTE_DIRECTORY, true, flagPortableMyDocs);
	section.SetString(L"Quikview.exe", wchTmp);
	section.SetStringEx(L"QuikviewParams", szQuickviewParams, L"");
	PathRelativeToApp(tchOpenWithDir, wchTmp, FILE_ATTRIBUTE_DIRECTORY, true, flagPortableMyDocs);
	section.SetString(L"OpenWithDir", wchTmp);
	section.SetInt(L"WindowPosX", wi.x);
	section.SetInt(L"WindowPosY", wi.y);

	section.SetBoolEx(L"SingleClick", bSingleClick, true);
	section.SetBoolEx(L"OpenFileInSameWindow", bOpenFileInSameWindow, false);
	section.SetBoolEx(L"TrackSelect", bTrackSelect, true);
	section.SetBoolEx(L"FullRowSelect", bFullRowSelect, false);
	section.SetBoolEx(L"UseRecycleBin", fUseRecycleBin, true);
	section.SetBoolEx(L"NoConfirmDelete", fNoConfirmDelete, false);
	section.SetBoolEx(L"ClearReadOnly", bClearReadOnly, true);
	section.SetBoolEx(L"RenameOnCollision", bRenameOnCollision, false);
	section.SetBoolEx(L"FocusEdit", bFocusEdit, true);
	section.SetBoolEx(L"AlwaysOnTop", bAlwaysOnTop, false);
	section.SetBoolEx(L"MinimizeToTray", bMinimizeToTray, false);
	section.SetBoolEx(L"TransparentMode", bTransparentMode, false);
	section.SetBoolEx(L"WindowLayoutRTL", bWindowLayoutRTL, false);
	section.SetIntEx(L"EscFunction", static_cast<int>(iEscFunction), EscFunction_None);

	if (IsVistaAndAbove()) {
		section.SetBoolEx(L"UseXPFileDialog", bUseXPFileDialog, false);
	}

	section.SetIntEx(L"FillMask", dwFillMask, DL_ALLOBJECTS);
	section.SetIntEx(L"SortOptions", nSortFlags, DS_NAME);
	section.SetBoolEx(L"SortReverse", fSortRev, false);
	section.SetStringEx(L"FileFilter", tchFilter, L"*.*");
	section.SetBoolEx(L"NegativeFilter", bNegFilter, false);
	section.SetBoolEx(L"DefColorNoFilter", bDefColorNoFilter, true);
	section.SetBoolEx(L"DefColorFilter", bDefColorFilter, true);
	section.SetIntEx(L"ColorNoFilter", colorNoFilter, GetSysColor(COLOR_WINDOWTEXT));
	section.SetIntEx(L"ColorFilter", colorFilter, GetSysColor(COLOR_HIGHLIGHT));

	Toolbar_GetButtons(hwndToolbar, TOOLBAR_COMMAND_BASE, tchToolbarButtons, COUNTOF(tchToolbarButtons));
	section.SetStringEx(L"ToolbarButtons", tchToolbarButtons, DefaultToolbarButtons);
	section.SetBoolEx(L"ShowToolbar", bShowToolbar, true);
	section.SetIntEx(L"AutoScaleToolbar", iAutoScaleToolbar, USER_DEFAULT_SCREEN_DPI);
	section.SetBoolEx(L"ShowStatusbar", bShowStatusbar, true);
	section.SetBoolEx(L"ShowDriveBox", bShowDriveBox, true);

	SaveIniSection(INI_SECTION_NAME_SETTINGS, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
}

void SaveWindowPosition(WCHAR *pIniSectionBuf) noexcept {
	IniSectionBuilder section = { pIniSectionBuf };

	WCHAR sectionName[96];
	HMONITOR hMonitor = MonitorFromWindow(hwndMain, MONITOR_DEFAULTTONEAREST);
	GetWindowPositionSectionName(hMonitor, sectionName);

	// query window dimensions when window is not minimized
	if (!IsIconic(hwndMain)) {
		WINDOWPLACEMENT wndpl;
		wndpl.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwndMain, &wndpl);

		wi.x = wndpl.rcNormalPosition.left;
		wi.y = wndpl.rcNormalPosition.top;
		wi.cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
		wi.cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
	}

	section.SetInt(L"WindowPosX", wi.x);
	section.SetInt(L"WindowPosY", wi.y);
	section.SetInt(L"WindowSizeX", wi.cx);
	section.SetInt(L"WindowSizeY", wi.cy);

	section.SetIntEx(L"RunDlgSizeX", cxRunDlg, 0);
	section.SetIntEx(L"GotoDlgSizeX", cxGotoDlg, 0);
	section.SetIntEx(L"FileFilterDlgX", cxFileFilterDlg, 0);
	section.SetIntEx(L"RenameFileDlgX", cxRenameFileDlg, 0);
	section.SetIntEx(L"NewDirectoryDlgX", cxNewDirectoryDlg, 0);
	section.SetIntEx(L"OpenWithDlgSizeX", cxOpenWithDlg, 0);
	section.SetIntEx(L"OpenWithDlgSizeY", cyOpenWithDlg, 0);
	section.SetIntEx(L"CopyMoveDlgSizeX", cxCopyMoveDlg, 0);
	section.SetIntEx(L"TargetApplicationDlgSizeX", cxTargetApplicationDlg, 0);
	section.SetIntEx(L"FindWindowDlgSizeX", cxFindWindowDlg, 0);

	SaveIniSection(sectionName, pIniSectionBuf);
}

void ClearWindowPositionHistory() noexcept {
	cxRunDlg = 0;
	cxGotoDlg = 0;
	cxFileFilterDlg = 0;
	cxRenameFileDlg = 0;
	cxNewDirectoryDlg = 0;
	cxOpenWithDlg = 0;
	cyOpenWithDlg = 0;
	cxCopyMoveDlg = 0;
	cxTargetApplicationDlg = 0;
	cxFindWindowDlg = 0;

	IniDeleteAllSection(INI_SECTION_NAME_WINDOW_POSITION);
}

//=============================================================================
//
//  ParseCommandLine()
//
//
enum CommandParseState {
	CommandParseState_None,
	CommandParseState_Consumed,
	CommandParseState_Argument,
};

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
			flagGotoFavorites = true;
			state = CommandParseState_Consumed;
			break;

		case L'I':
			flagStartAsTrayIcon = true;
			state = CommandParseState_Consumed;
			break;

		case L'M':
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpFilterArg) {
					NP2HeapFree(lpFilterArg);
				}

				lpFilterArg = static_cast<LPWSTR>(NP2HeapAlloc(sizeof(WCHAR) * (lstrlen(lp1) + 1)));
				lstrcpy(lpFilterArg, lp1);
				state = CommandParseState_Consumed;
			}
			break;

		case L'N':
			flagNoReuseWindow = true;
			state = CommandParseState_Consumed;
			break;

		case L'P':
			state = CommandParseState_Argument;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				int cord[4] = { 0 };
				const int itok = ParseCommaList(lp1, cord, COUNTOF(cord));
				if (itok == 4) {
					flagPosParam = true;
					state = CommandParseState_Consumed;
					wi.x = cord[0];
					wi.y = cord[1];
					wi.cx = cord[2];
					wi.cy = cord[3];
					if (wi.cx < 1) {
						wi.cx = CW_USEDEFAULT;
					}
					if (wi.cy < 1) {
						wi.cy = CW_USEDEFAULT;
					}
				}
			}
			break;

		default:
			break;
		}
	} else if (opt[2] == L'\0') {
		const int chNext = ToUpperA(opt[1]);
		switch (ch) {
		case L'F':
			if (chNext == L'0' || chNext == L'O') {
				StrCpyEx(szIniFile, L"*?");
				state = CommandParseState_Consumed;
			}
			break;

		case L'P':
			switch (chNext) {
			case L'D':
			case L'S':
				flagPosParam = true;
				state = CommandParseState_Consumed;
				wi.x = wi.y = wi.cx = wi.cy = CW_USEDEFAULT;
				break;
			}
			break;

		default:
			break;
		}
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

	LPWSTR lp2 = static_cast<LPWSTR>(NP2HeapAlloc(cmdSize));
	while (ExtractFirstArgument(lp3, lp1, lp2)) {
		// options
		if (*lp1 == L'/' || *lp1 == L'-') {
			const CommandParseState state = ParseCommandLineOption(lp1, lp2);
			if (state == CommandParseState_Consumed) {
				lstrcpy(lp3, lp2);
				continue;
			}
		}

		// pathname
		{
			if (lpPathArg) {
				GlobalFree(lpPathArg);
			}

			lpPathArg = static_cast<LPWSTR>(GlobalAlloc(GPTR, sizeof(WCHAR) * (MAX_PATH + 2)));
			lstrcpyn(lpPathArg, lp3, MAX_PATH);
			PathFixBackslashes(lpPathArg);
			StrTrim(lpPathArg, L" \"");
			break;
		}
	}

	NP2HeapFree(lp2);
	NP2HeapFree(lp1);
	NP2HeapFree(lp3);
}

//=============================================================================
//
//  LoadFlags()
//
//
void LoadFlags() noexcept {
	IniSectionParser section;
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_FLAGS));
	const DWORD cchIniSection = static_cast<DWORD>(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));

	section.Init(16);
	LoadIniSection(INI_SECTION_NAME_FLAGS, pIniSectionBuf, cchIniSection);
	section.Parse(pIniSectionBuf);

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	uiLanguage = static_cast<LANGID>(section.GetInt(L"UILanguage", LANG_USER_DEFAULT));
	ValidateUILangauge();
#endif

	bReuseWindow = section.GetBool(L"ReuseWindow", false);
	if (!flagNoReuseWindow) {
		flagNoReuseWindow = !bReuseWindow;
	}

	flagPortableMyDocs = section.GetBool(L"PortableMyDocs", true);
	iAutoRefreshRate = section.GetInt(L"AutoRefreshRate", 3000);
	flagNoFadeHidden = section.GetBool(L"NoFadeHidden", false);

	const int iValue = section.GetInt(L"OpacityLevel", 75);
	iOpacityLevel = validate(iValue, 0, 100, 75);

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

	section.Free();
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
//  FindIniFile()
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

bool CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule) noexcept {
	WCHAR tch[MAX_PATH];
	if (GetPrivateProfileString(INI_SECTION_NAME_MATEPATH, L"matepath.ini", L"", tch, COUNTOF(tch), lpszFile)) {
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
		lstrcpy(tchTest, L"matepath.ini");
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
			lstrcpy(PathFindFileName(szIniFile), L"matepath.ini");
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
		if (hFile != INVALID_HANDLE_VALUE) {
			LARGE_INTEGER fileSize;
			fileSize.QuadPart = 0;
			if (GetFileSizeEx(hFile, &fileSize) && fileSize.QuadPart < 2) {
				DWORD dw;
				WriteFile(hFile, L"\xFEFF[matepath]\r\n", 26, &dw, nullptr);
			}
			CloseHandle(hFile);
			return true;
		}
	}

	return false;
}

//=============================================================================
//
//  DisplayPath()
//
//
bool DisplayPath(LPCWSTR lpPath, UINT uIdError) {
	if (StrIsEmpty(lpPath)) {
		return false;
	}

	WCHAR szTmp[MAX_PATH];
	lstrcpy(szTmp, lpPath);
	ExpandEnvironmentStringsEx(szTmp, COUNTOF(szTmp));

	WCHAR szPath[MAX_PATH];
	if (!SearchPathEx(szTmp, COUNTOF(szPath), szPath)) {
		lstrcpy(szPath, szTmp);
	}
	PathGetRealPath(nullptr, szPath, szPath);

	if (PathGetLnkPath(szPath, szTmp)) {
		return DisplayLnkFile(szPath, szTmp);
	}

	const DWORD dwAttr = GetFileAttributes(szPath);
	if (dwAttr != INVALID_FILE_ATTRIBUTES) {
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
			if (!SetCurrentDirectory(szPath)) {
				MsgBoxWarn(MB_OK, uIdError);
				return false;
			}
			PostWMCommand(hwndMain, IDM_VIEW_UPDATE);
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
			return true;
		}
		{
			// szPath will be modified...
			lstrcpy(szTmp, szPath);

			SHFILEINFO shfi;
			SHGetFileInfo(szPath, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);

			WCHAR *p = StrRChr(szPath, nullptr, L'\\');
			if (p != nullptr) {
				p[1] = L'\0';
				if (!PathIsRoot(szPath)) {
					*p = L'\0';
				}
				SetCurrentDirectory(szPath);
			}

			SendWMCommand(hwndMain, IDM_VIEW_UPDATE);

			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, szTmp)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}

			return true;
		}
	}

	MsgBoxWarn(MB_OK, uIdError);
	return false;
}

//=============================================================================
//
//  DisplayLnkFile()
//
//
bool DisplayLnkFile(LPCWSTR pszLnkFile, LPCWSTR pszResPath) {
	WCHAR szPath[MAX_PATH];
	if (!SearchPathEx(pszResPath, COUNTOF(szPath), szPath)) {
		lstrcpy(szPath, pszResPath);
	}
	PathGetRealPath(nullptr, szPath, szPath);

	const DWORD dwAttr = GetFileAttributes(szPath);
	if (dwAttr != INVALID_FILE_ATTRIBUTES) {
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
			if (!SetCurrentDirectory(szPath)) {
				MsgBoxWarn(MB_OK, IDS_ERR_LNK_NOACCESS);
				return false;
			}
			PostWMCommand(hwndMain, IDM_VIEW_UPDATE);
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
			return true;
		}

		// Current file is ShellLink, get dir and desc
		lstrcpy(szPath, pszLnkFile);

		SHFILEINFO shfi;
		SHGetFileInfo(szPath, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);

		WCHAR *p = StrRChr(szPath, nullptr, L'\\');
		if (p != nullptr) {
			p[1] = L'\0';
			if (!PathIsRoot(szPath)) {
				*p = L'\0';
			}
			SetCurrentDirectory(szPath);
		}

		LV_FINDINFO lvfi;
		lvfi.flags = LVFI_STRING;
		lvfi.psz   = shfi.szDisplayName;

		SendWMCommand(hwndMain, IDM_VIEW_UPDATE);
		const int i = ListView_FindItem(hwndDirList, -1, &lvfi);

		// found item that is currently displayed
		if (i >= 0) {
			ListView_SetItemState(hwndDirList, i, LVIS_SELECTED | LVIS_FOCUSED,
								  LVIS_SELECTED | LVIS_FOCUSED);
			ListView_EnsureVisible(hwndDirList, i, FALSE);
		} else {
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
		}

		return true;
	}

	// GetFileAttributes() failed
	// Select lnk-file if target is not available
	if (PathIsFile(pszLnkFile)) {
		lstrcpy(szPath, pszLnkFile);
		PathRemoveFileSpec(szPath);
		SetCurrentDirectory(szPath);

		// Select new file
		SendWMCommand(hwndMain, IDM_VIEW_UPDATE);
		SHFILEINFO shfi;
		SHGetFileInfo(pszLnkFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
		if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, pszLnkFile)) {
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
		}
	}

	MsgBoxWarn(MB_OK, IDS_ERR_LNK_NOACCESS);
	return false;
}

/******************************************************************************
*
* ActivatePrevInst()
*
* Tries to find and activate an already open matepath Window
*
*
******************************************************************************/
static BOOL CALLBACK EnumWindProcReuseWindow(HWND hwnd, LPARAM lParam) noexcept {
	BOOL bContinue = TRUE;
	WCHAR szClassName[64];

	if (GetClassName(hwnd, szClassName, COUNTOF(szClassName))) {
		if (StrCaseEqual(szClassName, WC_MATEPATH)) {
			*AsPointer<HWND *>(lParam) = hwnd;
			if (IsWindowEnabled(hwnd)) {
				bContinue = FALSE;
			}
		}
	}

	return bContinue;
}

bool ActivatePrevInst() noexcept {
	if (flagNoReuseWindow || flagStartAsTrayIcon) {
		return false;
	}

	HWND hwnd = nullptr;
	EnumWindows(EnumWindProcReuseWindow, AsInteger<LPARAM>(&hwnd));

	// Found a window
	if (hwnd != nullptr) {
		// Enabled
		if (IsWindowEnabled(hwnd)) {
			if (IsIconic(hwnd)) {
				ShowWindowAsync(hwnd, SW_RESTORE);
			}

			if (!IsWindowVisible(hwnd)) {
				SendMessage(hwnd, APPM_TRAYMESSAGE, 0, WM_LBUTTONDBLCLK);
				SendMessage(hwnd, APPM_TRAYMESSAGE, 0, WM_LBUTTONUP);
			}

			SetForegroundWindow(hwnd);

			if (lpPathArg) {
				ExpandEnvironmentStringsEx(lpPathArg, static_cast<DWORD>(GlobalSize(lpPathArg) / sizeof(WCHAR)));

				if (PathIsRelative(lpPathArg)) {
					WCHAR tchTmp[MAX_PATH];
					GetCurrentDirectory(COUNTOF(tchTmp), tchTmp);
					PathAppend(tchTmp, lpPathArg);
					lstrcpy(lpPathArg, tchTmp);
				}

				COPYDATASTRUCT cds;
				cds.dwData = DATA_MATEPATH_PATHARG;
				cds.cbData = static_cast<DWORD>(GlobalSize(lpPathArg));
				cds.lpData = lpPathArg;

				// Send lpPathArg to previous instance
				SendMessage(hwnd, WM_COPYDATA, 0, AsInteger<LPARAM>(&cds));

				GlobalFree(lpPathArg);
			}
			return true;
		}

		if (MsgBoxAsk(MB_YESNO, IDS_ERR_PREVWINDISABLED) == IDYES) {
			return false;
		}
		return true;
	}

	return false;
}

void GetRelaunchParameters(LPWSTR szParameters) noexcept {
	StrCpyEx(szParameters, L" -f");
	if (StrNotEmpty(szIniFile)) {
		lstrcat(szParameters, L" \"");
		lstrcat(szParameters, szIniFile);
		lstrcat(szParameters, L"\"");
	} else {
		lstrcat(szParameters, L"0");
	}

	lstrcat(szParameters, L" -n");

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwndMain, &wndpl);

	HMONITOR hMonitor = MonitorFromRect(&wndpl.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	// offset new window position +10/+10
	int x = wndpl.rcNormalPosition.left + 10;
	int y = wndpl.rcNormalPosition.top + 10;
	const int cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
	const int cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

	// check if window fits monitor
	if ((x + cx) > mi.rcWork.right || (y + cy) > mi.rcWork.bottom) {
		x = mi.rcMonitor.left;
		y = mi.rcMonitor.top;
	}

	WCHAR tch[64];
	wsprintf(tch, L" -p %i,%i,%i,%i", x, y, cx, cy);
	lstrcat(szParameters, tch);

	WCHAR szDirName[MAX_PATH + 4];
	lstrcpy(szDirName, szCurDir);
	PathQuoteSpaces(szDirName);
	lstrcat(szParameters, L" ");
	lstrcat(szParameters, szDirName);
}

//=============================================================================
//
//  ShowNotifyIcon()
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
	lstrcpy(nid.szTip, WC_MATEPATH);
	Shell_NotifyIcon(bAdd ? NIM_ADD : NIM_DELETE, &nid);
}

//=============================================================================
//
//  LaunchTarget()
//
//  Launches the selected file in an existing target window
//  Runs target.exe if necessary
//
//
WCHAR szGlobalWndClass[256] = L"";
bool bLoadLaunchSetingsLoaded = false;

static BOOL CALLBACK EnumWindProcTargetApplication(HWND hwnd, LPARAM lParam) noexcept {
	BOOL bContinue = TRUE;
	WCHAR szClassName[64];

	if (GetClassName(hwnd, szClassName, COUNTOF(szClassName))) {
		if (StrCaseEqual(szClassName, szGlobalWndClass)) {
			*AsPointer<HWND *>(lParam) = hwnd;
			if (IsWindowEnabled(hwnd)) {
				bContinue = FALSE;
			}
		}
	}

	return bContinue;
}

void LoadLaunchSetings() noexcept {
	IniSectionParser section;
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_TARGET_APPLICATION));
	const DWORD cchIniSection = static_cast<DWORD>(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));

	section.Init(16);
	LoadIniSection(INI_SECTION_NAME_TARGET_APPLICATION, pIniSectionBuf, cchIniSection);
	section.Parse(pIniSectionBuf);

	int iValue = section.GetInt(L"UseTargetApplication", UseTargetApplication_NotSet);
	if (iValue != UseTargetApplication_NotSet) {
		iUseTargetApplication = static_cast<UseTargetApplication>(iValue);
		section.GetString(L"TargetApplicationPath", szTargetApplication, szTargetApplication);
		section.GetString(L"TargetApplicationParams", szTargetApplicationParams, szTargetApplicationParams);
		iValue = section.GetInt(L"TargetApplicationMode", static_cast<int>(iTargetApplicationMode));
		iTargetApplicationMode = clamp(static_cast<TargetApplicationMode>(iValue), TargetApplicationMode_None, TargetApplicationMode_UseDDE);
		section.GetString(L"TargetApplicationWndClass", szTargetApplicationWndClass, szTargetApplicationWndClass);
		section.GetString(L"DDEMessage", szDDEMsg, szDDEMsg);
		section.GetString(L"DDEApplication", szDDEApp, szDDEApp);
		section.GetString(L"DDETopic", szDDETopic, szDDETopic);
	} else if (iUseTargetApplication != UseTargetApplication_None && StrIsEmpty(szTargetApplication)) {
		iUseTargetApplication = UseTargetApplication_Use;
		iTargetApplicationMode = TargetApplicationMode_SendMsg;
		lstrcpy(szTargetApplication, L"Notepad4.exe");
		StrCpyEx(szTargetApplicationParams, L"");
		lstrcpy(szTargetApplicationWndClass, WC_NOTEPAD4);
		StrCpyEx(szDDEMsg, L"");
		StrCpyEx(szDDEApp, L"");
		StrCpyEx(szDDETopic, L"");
	}

	lstrcpy(szGlobalWndClass, szTargetApplicationWndClass);
	section.Free();
	NP2HeapFree(pIniSectionBuf);
	bLoadLaunchSetingsLoaded = true;
}

void LaunchTarget(LPCWSTR lpFileName, bool bOpenNew) {
	if (!bLoadLaunchSetingsLoaded) {
		LoadLaunchSetings();
	}
	if (iUseTargetApplication == UseTargetApplication_NotSet
		|| (iUseTargetApplication != UseTargetApplication_None && StrIsEmpty(szTargetApplication))) {
		ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FINDTARGET), hwndMain, FindTargetDlgProc, 0);
		return;
	}

	if (iUseTargetApplication != UseTargetApplication_None && iTargetApplicationMode == TargetApplicationMode_SendMsg) {
		HWND hwnd = nullptr;
		if (!bOpenNew) { // hwnd == nullptr
			EnumWindows(EnumWindProcTargetApplication, AsInteger<LPARAM>(&hwnd));
		}

		// Found a window
		if (hwnd != nullptr && IsWindowEnabled(hwnd)) {
			if (IsIconic(hwnd)) {
				ShowWindowAsync(hwnd, SW_RESTORE);
			}
			if (bFocusEdit) {
				SetForegroundWindow(hwnd);
			}

			if (lpFileName) {
				HDROP hDrop = CreateDropHandle(lpFileName);
				PostMessage(hwnd, WM_DROPFILES, AsInteger<WPARAM>(hDrop), 0);
			}
		} else { // Either no window or disabled - run target.exe
			if (hwnd) { // disabled window
				if (MsgBoxAsk(MB_YESNO, IDS_ERR_TARGETDISABLED) == IDNO) {
					return;
				}
			}

			LPWSTR lpParam;
			WCHAR szTmp[MAX_PATH];
			if (PathGetLnkPath(lpFileName, szTmp)) {
				lpParam = szTmp;
			} else {
				lpParam = const_cast<LPWSTR>(lpFileName);
			}

			//if (Is32bitExe(szTargetApplication))
			//  PathQuoteSpaces(lpParam);
			//else
			GetShortPathName(lpParam, lpParam, MAX_PATH);

			WCHAR szParam[MAX_PATH] = L"";
			if (StrNotEmpty(szTargetApplicationParams)) {
				lstrcpyn(szParam, szTargetApplicationParams, COUNTOF(szParam));
				StrCatBuff(szParam, L" ", COUNTOF(szParam));
			}
			StrCatBuff(szParam, lpParam, COUNTOF(szParam));

			lstrcpy(szTmp, szTargetApplication);
			WCHAR szFile[MAX_PATH];
			PathAbsoluteFromApp(szTmp, szFile, true);

			SHELLEXECUTEINFO sei;
			memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = 0;
			sei.hwnd = hwndMain;
			sei.lpVerb = nullptr;
			sei.lpFile = szFile;
			sei.lpParameters = szParam;
			sei.lpDirectory = szCurDir;
			sei.nShow = SW_SHOWNORMAL;

			ShellExecuteEx(&sei);
		}
	} else {
		if (iUseTargetApplication != UseTargetApplication_None
			&& iTargetApplicationMode == TargetApplicationMode_UseDDE
			&& ExecDDECommand(lpFileName, szDDEMsg, szDDEApp, szDDETopic)) {
			return;
		}

		if (iUseTargetApplication == UseTargetApplication_None && StrIsEmpty(lpFileName)) {
			return;
		}

		LPWSTR lpParam;
		WCHAR szTmp[MAX_PATH];
		if (PathGetLnkPath(lpFileName, szTmp)) {
			lpParam = szTmp;
		} else {
			lpParam = const_cast<LPWSTR>(lpFileName);
		}

		//if (Is32bitExe(szTargetApplication))
		//  PathQuoteSpaces(lpParam);
		//else
		GetShortPathName(lpParam, lpParam, MAX_PATH);

		WCHAR szParam[MAX_PATH] = L"";
		if (StrNotEmpty(szTargetApplicationParams)) {
			lstrcpyn(szParam, szTargetApplicationParams, COUNTOF(szParam));
			StrCatBuff(szParam, L" ", COUNTOF(szParam));
		}
		StrCatBuff(szParam, lpParam, COUNTOF(szParam));

		lstrcpy(szTmp, szTargetApplication);
		ExpandEnvironmentStringsEx(szTmp, COUNTOF(szTmp));

		WCHAR szFile[MAX_PATH];
		PathAbsoluteFromApp(szTmp, szFile, true);

		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwndMain;
		sei.lpVerb = nullptr;
		if (iUseTargetApplication != UseTargetApplication_None) {
			sei.lpFile = szFile;
			sei.lpParameters = szParam;
		} else {
			sei.lpFile = lpParam;
			sei.lpParameters = nullptr;
		}
		sei.lpDirectory = szCurDir;
		sei.nShow = SW_SHOWNORMAL;

		ShellExecuteEx(&sei);
	}
}

//=============================================================================
//
//  SnapToTarget()
//
//  Aligns matepath to either side of target window
//
//
void SnapToTarget(HWND hwnd) noexcept {
	if (!bLoadLaunchSetingsLoaded) {
		LoadLaunchSetings();
	}

	HWND hwnd2 = nullptr;
	EnumWindows(EnumWindProcTargetApplication, AsInteger<LPARAM>(&hwnd2));

	// Found a window
	if (hwnd2 != nullptr) {
		if (IsIconic(hwnd2) || IsZoomed(hwnd2)) {
			SendMessage(hwnd2, WM_SYSCOMMAND, SC_RESTORE, 0);
		}

		SetForegroundWindow(hwnd2);
		BringWindowToTop(hwnd2);
		SetForegroundWindow(hwnd);

		RECT rcOld;
		RECT rc2;
		GetWindowRect(hwnd, &rcOld);
		GetWindowRect(hwnd2, &rc2);

		HMONITOR hMonitor = MonitorFromRect(&rc2, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		const int cxScreen = mi.rcMonitor.right - mi.rcMonitor.left;
		RECT rcNew;

		if (rc2.left > cxScreen - rc2.right) {
			rcNew.left = rc2.left - (rcOld.right - rcOld.left);
		} else {
			rcNew.left = rc2.right;
		}

		rcNew.top = rc2.top;
		rcNew.right = rcNew.left + (rcOld.right - rcOld.left);
		rcNew.bottom = rcNew.top + (rcOld.bottom - rcOld.top);

		if (!EqualRect(&rcOld, &rcNew)) {
			if (GetDoAnimateMinimize()) {
				DrawAnimatedRects(hwnd, IDANI_CAPTION, &rcOld, &rcNew);
			}

			SetWindowPos(hwnd, nullptr, rcNew.left, rcNew.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
	}
}

//=============================================================================
//
//  SnapToDefaultPos()
//
//  Aligns matepath to the default window position on the current screen
//
//
void SnapToDefaultPos(HWND hwnd) noexcept {
	RECT rcOld;
	GetWindowRect(hwnd, &rcOld);

	HMONITOR hMonitor = MonitorFromRect(&rcOld, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	int x = mi.rcWork.left + 16;
	const int y = mi.rcWork.top + 16;
	constexpr int cx = 272;
	constexpr int cy = 640;

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	wndpl.flags = WPF_ASYNCWINDOWPLACEMENT;
	wndpl.showCmd = SW_RESTORE;

	wndpl.rcNormalPosition.left = x;
	wndpl.rcNormalPosition.top = y;
	wndpl.rcNormalPosition.right = x + cx;
	wndpl.rcNormalPosition.bottom = y + cy;

	if (EqualRect(&rcOld, &wndpl.rcNormalPosition)) {
		x = mi.rcWork.right - cx - 16;
		wndpl.rcNormalPosition.left = x;
		wndpl.rcNormalPosition.right = x + cx;
	}

	if (GetDoAnimateMinimize()) {
		DrawAnimatedRects(hwnd, IDANI_CAPTION, &rcOld, &wndpl.rcNormalPosition);
		OffsetRect(&wndpl.rcNormalPosition, mi.rcMonitor.left - mi.rcWork.left, mi.rcMonitor.top - mi.rcWork.top);
	}

	SetWindowPlacement(hwnd, &wndpl);
}

#if !NP2_HAS_GETDPIFORWINDOW
// scintilla\win32\PlatWin.cxx
static void LoadDpiForWindow() noexcept {
	HMODULE user32 = GetModuleHandleW(L"user32.dll");
	fnGetDpiForWindow = DLLFunction<GetDpiForWindowSig>(user32, "GetDpiForWindow");
	fnGetSystemMetricsForDpi = DLLFunction<GetSystemMetricsForDpiSig>(user32, "GetSystemMetricsForDpi");
	fnAdjustWindowRectExForDpi = DLLFunction<AdjustWindowRectExForDpiSig>(user32, "AdjustWindowRectExForDpi");

	using GetDpiForSystemSig = UINT (WINAPI *)(void);
	GetDpiForSystemSig fnGetDpiForSystem = DLLFunction<GetDpiForSystemSig>(user32, "GetDpiForSystem");
	if (fnGetDpiForSystem) {
		g_uSystemDPI = fnGetDpiForSystem();
	} else {
		HDC hDC = GetDC(nullptr);
		g_uSystemDPI = GetDeviceCaps(hDC, LOGPIXELSY);
		ReleaseDC(nullptr, hDC);
	}

	if (!fnGetDpiForWindow) {
		hShcoreDLL = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hShcoreDLL) {
			fnGetDpiForMonitor = DLLFunction<GetDpiForMonitorSig>(hShcoreDLL, "GetDpiForMonitor");
		}
	}
}

UINT GetWindowDPI(HWND hwnd) noexcept {
	if (fnGetDpiForWindow) {
		return fnGetDpiForWindow(hwnd);
	}
	if (fnGetDpiForMonitor) {
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		UINT dpiX = 0;
		UINT dpiY = 0;
		if (fnGetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK) {
			return dpiY;
		}
	}
	return g_uSystemDPI;
}

int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept {
	if (fnGetSystemMetricsForDpi) {
		return fnGetSystemMetricsForDpi(nIndex, dpi);
	}

	int value = GetSystemMetrics(nIndex);
	value = (dpi == g_uSystemDPI) ? value : MulDiv(value, dpi, g_uSystemDPI);
	return value;
}

BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) noexcept {
	if (fnAdjustWindowRectExForDpi) {
		return fnAdjustWindowRectExForDpi(lpRect, dwStyle, FALSE, dwExStyle, dpi);
	}
	return AdjustWindowRectEx(lpRect, dwStyle, FALSE, dwExStyle);
}
#endif // NP2_HAS_GETDPIFORWINDOW
