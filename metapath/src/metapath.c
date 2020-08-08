/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* metapath.c
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
#include <commctrl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <stdio.h>
#include "config.h"
#include "Helpers.h"
#include "Dlapi.h"
#include "Dialogs.h"
#include "metapath.h"
#include "resource.h"

/******************************************************************************
*
* Local Variables for metapath.c
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

static HANDLE hChangeHandle = NULL;
HISTORY	mHistory;

WCHAR	szIniFile[MAX_PATH] = L"";
WCHAR	szIniFile2[MAX_PATH] = L"";
BOOL	bSaveSettings;
static BOOL bHasQuickview = FALSE;
WCHAR	szQuickview[MAX_PATH];
WCHAR	szQuickviewParams[MAX_PATH];
WCHAR	tchFavoritesDir[MAX_PATH];
WCHAR	tchOpenWithDir[MAX_PATH];
static WCHAR tchToolbarButtons[MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE];
static LPWSTR tchToolbarBitmap = NULL;
static LPWSTR tchToolbarBitmapHot = NULL;
static LPWSTR tchToolbarBitmapDisabled = NULL;
BOOL	bClearReadOnly;
BOOL	bRenameOnCollision;
BOOL	bSingleClick;
BOOL	bOpenFileInSameWindow;
static int iDefaultOpenMenu;
static int iShiftOpenMenu;
BOOL	bTrackSelect;
BOOL	bFullRowSelect;
int		iStartupDir;
int		iEscFunction;
BOOL	bFocusEdit;
BOOL	bAlwaysOnTop;
static BOOL bTransparentMode;
BOOL	bWindowLayoutRTL;
BOOL	bMinimizeToTray;
BOOL	fUseRecycleBin;
BOOL	fNoConfirmDelete;
static BOOL bShowToolbar;
static BOOL bShowStatusbar;
static BOOL bShowDriveBox;
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
BOOL		bNegFilter;
BOOL		bDefColorNoFilter;
BOOL		bDefColorFilter;
COLORREF	colorNoFilter;
COLORREF	colorFilter;
COLORREF	colorCustom[16];

typedef struct WININFO {
	int x;
	int y;
	int cx;
	int cy;
} WININFO;

static WININFO wi;

static int cyReBar;
static int cyReBarFrame;
static int cyDriveBoxFrame;

int		nIdFocus = IDC_DIRLIST;

WCHAR	szCurDir[MAX_PATH + 40];
static WCHAR szMRUDirectory[MAX_PATH];
static DWORD dwFillMask;
static int nSortFlags;
static BOOL fSortRev;

static LPWSTR lpPathArg = NULL;
static LPWSTR lpFilterArg = NULL;

static UINT wFuncCopyMove = FO_COPY;

static UINT msgTaskbarCreated = 0;

int		iUseTargetApplication = 4;
int		iTargetApplicationMode = 0;
WCHAR	szTargetApplication[MAX_PATH] = L"";
WCHAR	szTargetApplicationParams[MAX_PATH] = L"";
WCHAR	szTargetApplicationWndClass[MAX_PATH] = L"";
WCHAR	szDDEMsg[256] = L"";
WCHAR	szDDEApp[256] = L"";
WCHAR	szDDETopic[256] = L"";

HINSTANCE	g_hInstance;
HANDLE		g_hDefaultHeap;
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
DWORD		g_uWinVer;
#endif
UINT		g_uSystemDPI = USER_DEFAULT_SCREEN_DPI;
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
static int	flagNoReuseWindow	= 0;
BOOL		bReuseWindow		= FALSE;
static int	flagStartAsTrayIcon	= 0;
int			flagPortableMyDocs	= 0;
int			flagGotoFavorites	= 0;
static int	iAutoRefreshRate	= 0; // unit: 1/10 sec
int			flagNoFadeHidden	= 0;
static int	iOpacityLevel		= 75;
static int	flagToolbarLook		= 0;
static int	flagPosParam		= 0;

static inline BOOL HasFilter(void) {
	return !StrEqual(tchFilter, L"*.*") || bNegFilter;
}

//=============================================================================
//
//  WinMain()
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
	if (initialized) {
		UnregisterClass(WC_METAPATH, g_hInstance);
	}
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	if (hResDLL) {
		FreeLibrary(hResDLL);
	}
#endif
	OleUninitialize();
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT) {
		ShowNotifyIcon(hwndMain, FALSE);
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
	SetEnvironmentVariable(L"UBSAN_OPTIONS", L"log_path=" WC_METAPATH L"-UBSan.log");
#endif

	// Set global variable g_hInstance
	g_hInstance = hInstance;
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
	OleInitialize(NULL);

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	msgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	hResDLL = LoadLocalizedResourceDLL(uiLanguage, WC_METAPATH L".dll");
	if (hResDLL) {
		g_hInstance = hInstance = (HINSTANCE)hResDLL;
	}
#endif

// since Windows 10, version 1607
#if defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)
// 1709 was the first version for Windows 10 on ARM64.
	g_uSystemDPI = GetDpiForSystem();
#else
	typedef UINT (WINAPI *GetDpiForSystemSig)(void);
	GetDpiForSystemSig pfnGetDpiForSystem = DLLFunctionEx(GetDpiForSystemSig, L"user32.dll", "GetDpiForSystem");
	if (pfnGetDpiForSystem) {
		g_uSystemDPI = pfnGetDpiForSystem();
	} else {
		HDC hDC = GetDC(NULL);
		g_uSystemDPI = GetDeviceCaps(hDC, LOGPIXELSY);
		ReleaseDC(NULL, hDC);
	}
#endif

	// Load Settings
	LoadSettings();

	if (!InitApplication(hInstance)) {
		CleanUpResources(FALSE);
		return FALSE;
	}

	HWND hwnd;
	if ((hwnd = InitInstance(hInstance, nShowCmd)) == NULL) {
		CleanUpResources(TRUE);
		return FALSE;
	}

	HACCEL hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(hwnd, hAcc, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CleanUpResources(TRUE);
	return (int)(msg.wParam);
}

//=============================================================================
//
//  InitApplication()
//
//
BOOL InitApplication(HINSTANCE hInstance) {
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc   = (WNDPROC)MainWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINWND));
	wc.hCursor       = LoadCursor(hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WC_METAPATH;
	wc.hIconSm       = NULL;

	return RegisterClassEx(&wc);
}

//=============================================================================
//
//  InitInstance()
//
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow) {
	const BOOL defaultPos = (wi.x == CW_USEDEFAULT || wi.y == CW_USEDEFAULT || wi.cx == CW_USEDEFAULT || wi.cy == CW_USEDEFAULT);
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
			wi.cx = min_i(mi.rcWork.right - mi.rcWork.left - 32, wi.cy);
			wi.x = mi.rcWork.right - wi.cx - 16;
		}
	}

	hwndMain = CreateWindowEx(
				   0,
				   WC_METAPATH,
				   L"metapath",
				   WS_METAPATH,
				   wi.x,
				   wi.y,
				   wi.cx,
				   wi.cy,
				   NULL,
				   NULL,
				   hInstance,
				   NULL);

	if (bAlwaysOnTop) {
		SetWindowPos(hwndMain, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if (bTransparentMode) {
		SetWindowTransparentMode(hwndMain, TRUE, iOpacityLevel);
	}

	if (!flagStartAsTrayIcon) {
		ShowWindow(hwndMain, nCmdShow);
		UpdateWindow(hwndMain);
	} else {
		ShowWindow(hwndMain, SW_HIDE);   // trick ShowWindow()
		ShowNotifyIcon(hwndMain, TRUE);
	}

	// Pathname parameter
	if (lpPathArg) {
		DisplayPath(lpPathArg, IDS_ERR_CMDLINE);
		GlobalFree(lpPathArg);
	} else if (iStartupDir) {
		// Use a startup directory
		if (iStartupDir == 1) {
			if (!StrIsEmpty(szMRUDirectory)) {
				GetModuleFileName(NULL, szMRUDirectory, COUNTOF(szMRUDirectory));
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
		PostWMCommand(hwndMain, IDM_VIEW_UPDATE);
	}

	return hwndMain;
}

//=============================================================================
//
//  MainWndProc()
//
//  Messages are distributed to the MsgXXX-handlers
//
//
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static BOOL bShutdownOK;

	switch (umsg) {
	case WM_CREATE: {
		// Init directory watching
		if (iAutoRefreshRate) {
			SetTimer(hwnd, ID_TIMER, (iAutoRefreshRate * 100), NULL);
		}
		return MsgCreate(hwnd, wParam, lParam);
	}

	case WM_DESTROY:
	case WM_ENDSESSION:
		if (!bShutdownOK) {
			// Terminate directory watching
			KillTimer(hwnd, ID_TIMER);
			FindCloseChangeNotification(hChangeHandle);

			// GetWindowPlacement
			WINDOWPLACEMENT wndpl;
			wndpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwnd, &wndpl);

			wi.x = wndpl.rcNormalPosition.left;
			wi.y = wndpl.rcNormalPosition.top;
			wi.cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
			wi.cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

			DirList_Destroy(hwndDirList);
			DragAcceptFiles(hwnd, FALSE);

			History_Uninit(&mHistory);

			SaveSettings(FALSE);

			bShutdownOK = TRUE;
		}
		if (umsg == WM_DESTROY) {
			PostQuitMessage(0);
		}
		break;

	// Reinitialize theme-dependent values and resize windows
	case WM_THEMECHANGED:
		MsgThemeChanged(hwnd, wParam, lParam);
		break;

	// update colors of DirList manually
	case WM_SYSCOLORCHANGE: {
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
		HWND hwndTT = (HWND)SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0);

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

		SendMessage(hwndTT, TTM_RELAYEVENT, 0, (LPARAM)&msg);
	}
	return DefWindowProc(hwnd, umsg, wParam, lParam);

	case WM_TIMER:
		// Check Change Notification Handle
		if (WAIT_OBJECT_0 == WaitForSingleObject(hChangeHandle, 0)) {
			// Store information about currently selected item
			DLITEM dli;
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
		HDROP hDrop = (HDROP)wParam;

		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}

		//SetForegroundWindow(hwnd);
		DragQueryFile(hDrop, 0, szBuf, COUNTOF(szBuf));
		DisplayPath(szBuf, IDS_ERR_DROP1);

		//if (DragQueryFile(hDrop, (UINT)(-1), NULL, 0) > 1) {
		//	MsgBoxWarn(MB_OK, IDS_ERR_DROP2);
		//}

		DragFinish(hDrop);
	}
	break;

	case WM_COPYDATA: {
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

		if (pcds->dwData == DATA_METAPATH_PATHARG) {
			LPWSTR lpsz = (LPWSTR)NP2HeapAlloc(pcds->cbData);
			CopyMemory(lpsz, pcds->lpData, pcds->cbData);

			DisplayPath(lpsz, IDS_ERR_CMDLINE);

			NP2HeapFree(lpsz);
		}
	}
	return TRUE;

	case WM_CONTEXTMENU: {
		const int nID = GetDlgCtrlID((HWND)wParam);

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
						 pt.x + 1, pt.y + 1, hwnd, NULL);
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
			ShowNotifyIcon(hwnd, TRUE);
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
								  pt.x, pt.y, 0, hwnd, NULL);

			PostMessage(hwnd, WM_NULL, 0, 0);

			DestroyMenu(hMenu);

			if (iCmd == IDM_TRAY_RESTORE) {
				ShowNotifyIcon(hwnd, FALSE);
				RestoreWndFromTray(hwnd);
				ShowOwnedPopups(hwnd, TRUE);
			} else if (iCmd == IDM_TRAY_EXIT) {
				ShowNotifyIcon(hwnd, FALSE);
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			}
		}
		return TRUE;

		case WM_LBUTTONUP:
			ShowNotifyIcon(hwnd, FALSE);
			RestoreWndFromTray(hwnd);
			ShowOwnedPopups(hwnd, TRUE);
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
LRESULT MsgCreate(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

	hwndDirList = CreateWindowEx(
					  WS_EX_CLIENTEDGE,
					  WC_LISTVIEW,
					  NULL,
					  WS_DIRLIST,
					  0, 0, 0, 0,
					  hwnd,
					  (HMENU)IDC_DIRLIST,
					  hInstance,
					  NULL);

	if (IsAppThemed()) {
		SetWindowExStyle(hwndDirList, GetWindowExStyle(hwndDirList) & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwndDirList, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
	}
	InitWindowCommon(hwndDirList);

	const DWORD dwDriveBoxStyle = bShowDriveBox ? (WS_DRIVEBOX | WS_VISIBLE) : WS_DRIVEBOX;
	hwndDriveBox = CreateWindowEx(
					   0,
					   WC_COMBOBOXEX,
					   NULL,
					   dwDriveBoxStyle,
					   0, 0, 0, GetSystemMetrics(SM_CYFULLSCREEN),
					   hwnd,
					   (HMENU)IDC_DRIVEBOX,
					   hInstance,
					   NULL);

	// Create Toolbar and Statusbar
	CreateBars(hwnd, hInstance);

	// Window Initialization
	// DriveBox
	DriveBox_Init(hwndDriveBox);
	ComboBox_SetExtendedUI(hwndDriveBox, TRUE);
	// DirList
	LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, NULL, -1, 0, 0, 0
#if (NTDDI_VERSION >= NTDDI_VISTA)
			, 0, 0, 0
#endif
	};
	ListView_SetExtendedListViewStyle(hwndDirList, LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
	ListView_InsertColumn(hwndDirList, 0, &lvc);
	DirList_Init(hwndDirList, NULL);
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
	History_Init(&mHistory);
	History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
	// ToolTip with Current Directory
	TOOLINFO ti;
	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND;
	ti.hwnd = hwnd;
	ti.uId = (UINT_PTR)hwnd;
	ti.lpszText = LPSTR_TEXTCALLBACK;

	HWND hwndTT = (HWND)SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0);
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

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
void CreateBars(HWND hwnd, HINSTANCE hInstance) {
	const BOOL bIsAppThemed = IsAppThemed();

	const DWORD dwToolbarStyle = WS_TOOLBAR | TBSTYLE_FLAT | CCS_ADJUSTABLE;
	hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, dwToolbarStyle,
								 0, 0, 0, 0, hwnd, (HMENU)IDC_TOOLBAR, hInstance, NULL);

	SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	// Add normal Toolbar Bitmap
	HBITMAP hbmp = NULL;
	HBITMAP hbmpCopy = NULL;
	BOOL bExternalBitmap = FALSE;

	if (tchToolbarBitmap != NULL) {
		hbmp = LoadBitmapFile(tchToolbarBitmap);
	}
	if (hbmp != NULL) {
		bExternalBitmap = TRUE;
	} else {
		hbmp = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDR_MAINWND), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
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
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, COUNTOF(tbbMainWnd));
	LoadIniSection(INI_SECTION_NAME_TOOLBAR_LABELS, pIniSectionBuf, cchIniSection);
	IniSectionParseArray(pIniSection, pIniSectionBuf);
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

	//SendMessage(hwndToolbar, TB_SAVERESTORE, FALSE, (LPARAM)lptbsp);
	if (Toolbar_SetButtons(hwndToolbar, tchToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd)) == 0) {
		Toolbar_SetButtons(hwndToolbar, DefaultToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd));
	}

	RECT rc;
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
	rbi.fMask  = 0;
	rbi.himl   = (HIMAGELIST)NULL;
	SendMessage(hwndReBar, RB_SETBARINFO, 0, (LPARAM)&rbi);

	REBARBANDINFO rbBand;
	rbBand.cbSize  = sizeof(REBARBANDINFO);
	rbBand.fMask   = /*RBBIM_COLORS | RBBIM_TEXT | RBBIM_BACKGROUND | */
		RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE /*| RBBIM_SIZE*/;
	rbBand.fStyle  = /*RBBS_CHILDEDGE | *//*RBBS_BREAK |*/ RBBS_FIXEDSIZE /*| RBBS_GRIPPERALWAYS*/;
	if (bIsAppThemed) {
		rbBand.fStyle |= RBBS_CHILDEDGE;
	}
	rbBand.hbmBack = NULL;
	rbBand.lpText     = (LPWSTR)L"Toolbar";
	rbBand.hwndChild  = hwndToolbar;
	rbBand.cxMinChild = (rc.right - rc.left) * COUNTOF(tbbMainWnd);
	rbBand.cyMinChild = (rc.bottom - rc.top) + 2 * rc.top;
	rbBand.cx         = 0;
	SendMessage(hwndReBar, RB_INSERTBAND, (WPARAM)(-1), (LPARAM)&rbBand);

	SetWindowPos(hwndReBar, NULL, 0, 0, 0, 0, SWP_NOZORDER);
	GetWindowRect(hwndReBar, &rc);
	cyReBar = rc.bottom - rc.top;

	cyReBarFrame = bIsAppThemed ? 0 : 2;
	cyDriveBoxFrame = bIsAppThemed ? 0 : 2;
}

//=============================================================================
//
//  MsgThemeChanged() - Handles WM_THEMECHANGED
//
//
void MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HINSTANCE hInstance = GetWindowInstance(hwnd);

	if (IsAppThemed()) {
		SetWindowExStyle(hwndDirList, GetWindowExStyle(hwndDirList) & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwndDirList, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		if (bFullRowSelect) {
			SetExplorerTheme(hwndDirList);
		} else {
			SetListViewTheme(hwndDirList);
		}
	} else {
		SetWindowExStyle(hwndDirList, GetWindowExStyle(hwndDirList) | WS_EX_CLIENTEDGE);
		SetWindowPos(hwndDirList, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
	}

	// recreate toolbar and statusbar
	WCHAR chStatus[255];
	SendMessage(hwndStatus, SB_GETTEXT, ID_FILEINFO, (LPARAM)chStatus);

	// recreate toolbar and statusbar
	Toolbar_GetButtons(hwndToolbar, TOOLBAR_COMMAND_BASE, tchToolbarButtons, COUNTOF(tchToolbarButtons));

	DestroyWindow(hwndToolbar);
	DestroyWindow(hwndReBar);
	DestroyWindow(hwndStatus);
	CreateBars(hwnd, hInstance);

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
void MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);

	if (wParam == SIZE_MINIMIZED) {
		return;
	}

	RECT rc;
	const int x = 0;
	int y = 0;
	const int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	if (bShowToolbar) {
		//SendMessage(hwndToolbar, WM_SIZE, 0, 0);
		//GetWindowRect(hwndToolbar, &rc);
		//y = (rc.bottom - rc.top);
		//cy -= (rc.bottom - rc.top);

		//SendMessage(hwndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rc);
		SetWindowPos(hwndReBar, NULL, 0, 0, LOWORD(lParam), cyReBar, SWP_NOZORDER);
		// the ReBar automatically sets the correct height
		// calling SetWindowPos() with the height of one toolbar button
		// causes the control not to temporarily use the whole client area
		// and prevents flickering

		GetWindowRect(hwndReBar, &rc);
		y = cyReBar + cyReBarFrame;    // define
		cy -= cyReBar + cyReBarFrame;  // border
	}

	if (bShowStatusbar) {
		SendMessage(hwndStatus, WM_SIZE, 0, 0);
		GetWindowRect(hwndStatus, &rc);
		cy -= (rc.bottom - rc.top);
	}

	HDWP hdwp = BeginDeferWindowPos(2);
	DeferWindowPos(hdwp, hwndDriveBox, NULL, x, y, cx, max_i(cy, 100), SWP_NOZORDER | SWP_NOACTIVATE);

	if (bShowDriveBox) {
		GetWindowRect(hwndDriveBox, &rc);
		y += (rc.bottom - rc.top) + cyDriveBoxFrame;
		cy -= (rc.bottom - rc.top) + cyDriveBoxFrame;
	}

	DeferWindowPos(hdwp, hwndDirList, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	EndDeferWindowPos(hdwp);

	// Rebuild DirList Columns
	ListView_SetColumnWidth(hwndDirList, 0, LVSCW_AUTOSIZE_USEHEADER);

	GetClientRect(hwndStatus, &rc);
	const int aWidth[1] = { -1 };
	SendMessage(hwndStatus, SB_SETPARTS, COUNTOF(aWidth), (LPARAM)aWidth);
	InvalidateRect(hwndStatus, NULL, TRUE);
}

//=============================================================================
//
//  MsgInitMenu() - Handles WM_INITMENU
//
//
void MsgInitMenu(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(lParam);

	HMENU hmenu = (HMENU)wParam;

	int i = ListView_GetSelectedCount(hwndDirList);
	DLITEM dli;
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

			if (DriveBox_GetSelDrive(hwndDriveBox, tch, COUNTOF(tch), TRUE) && !PathIsSameRoot(szCurDir, tch)) {
				if (!ChangeDirectory(hwnd, tch, TRUE)) {
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
		DLITEM dli = { DLI_ALL, L"", L"", DLE_NONE };
		DirList_GetItem(hwndDirList, -1, &dli);

		switch (dli.ntype) {
		case DLE_DIR:
			if (!ChangeDirectory(hwnd, dli.szFileName, TRUE)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
			break;

		case DLE_FILE:
			BeginWaitCursor();
			const BOOL bOpenNew = LOWORD(wParam) == IDM_FILE_OPENNEW;
			if (!PathIsLnkFile(dli.szFileName)) {
				LaunchTarget(dli.szFileName, bOpenNew);
			} else {
				// PathIsLinkFile()
				WCHAR tch[MAX_PATH];

				if (PathGetLnkPath(dli.szFileName, tch, COUNTOF(tch))) {
					ExpandEnvironmentStringsEx(tch, COUNTOF(tch));
					const DWORD dwAttr = GetFileAttributes(tch);
					if ((dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
						DisplayLnkFile(dli.szFileName);
					} else {
						// Made sure link points to a file
						LaunchTarget(tch, bOpenNew);
					}
				} else {
					DisplayLnkFile(dli.szFileName);
				}
			}

			EndWaitCursor();
			break;
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

		DLITEM dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

		SHELLEXECUTEINFO sei;
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = NULL;
		sei.lpFile = dli.szFileName;
		sei.lpParameters = NULL;
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

		DLITEM dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

		WCHAR szTmp[MAX_PATH];
		if (PathIsLnkFile(dli.szFileName) && PathGetLnkPath(dli.szFileName, szTmp, COUNTOF(szTmp))) {
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
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwnd;
		sei.lpVerb = NULL;
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

		DLITEM dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
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

		lstrcpy(szNewFile, L"");
		GetString(IDS_FILTER_ALL, szFilter, COUNTOF(szFilter));
		PrepareFilterStr(szFilter);
		GetString(IDS_NEWFILE, szTitle, COUNTOF(szTitle));

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = szFilter;
		ofn.lpstrFile = szNewFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = szTitle;
		ofn.lpstrInitialDir = szCurDir;
		ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT |
					OFN_NODEREFERENCELINKS | OFN_OVERWRITEPROMPT |
					OFN_PATHMUSTEXIST;

		if (!GetSaveFileName(&ofn)) {
			break;
		}

		HANDLE hFile = CreateFile(szNewFile,
						   GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   NULL,
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);

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
			if (!CreateDirectory(tchNewDir, NULL)) {
				MsgBoxWarn(MB_OK, IDS_ERR_NEWDIR);
			}
		}
	}
	break;

	case IDM_FILE_CREATELINK: {
		WCHAR tchLinkDestination[MAX_PATH];

		DLITEM dli;
		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

		if (GetDirectory(hwnd, IDS_CREATELINK, tchLinkDestination, NULL)) {
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

		DLITEM dli;
		dli.mask = DLI_ALL;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

		WCHAR szNewFile[MAX_PATH];
		lstrcpy(szNewFile, dli.szFileName);

		WCHAR szFilter[128];
		GetString(IDS_FILTER_ALL, szFilter, COUNTOF(szFilter));
		PrepareFilterStr(szFilter);

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = szFilter;
		ofn.lpstrFile = szNewFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT |
					OFN_NODEREFERENCELINKS | OFN_OVERWRITEPROMPT |
					OFN_PATHMUSTEXIST;

		if (!GetSaveFileName(&ofn)) {
			break;
		}

		BeginWaitCursor();

		WCHAR tch[MAX_PATH];
		WCHAR fmt[64];
		FormatString(tch, fmt, IDS_SAVEFILE, dli.szDisplayName);
		StatusSetText(hwndStatus, ID_MENUHELP, tch);
		StatusSetSimple(hwndStatus, TRUE);
		InvalidateRect(hwndStatus, NULL, TRUE);
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
		DLITEM dli;
		int iItem;

		dli.mask = DLI_ALL;
		if ((iItem = DirList_GetItem(hwndDirList, -1, &dli)) == -1) {
			break;
		}

		WCHAR tch[512];
		ZeroMemory(tch, sizeof(tch));
		lstrcpy(tch, dli.szFileName);

		SHFILEOPSTRUCT shfos;
		ZeroMemory(&shfos, sizeof(SHFILEOPSTRUCT));
		shfos.hwnd = hwnd;
		shfos.wFunc = FO_DELETE;
		shfos.pFrom = tch;
		shfos.pTo = NULL;
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
			iItem = min_i(iItem, ListView_GetItemCount(hwndDirList) - 1);
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

		if (GetDirectory(hwnd, IDS_GETDIRECTORY, tch, NULL)) {
			if (!ChangeDirectory(hwnd, tch, TRUE)) {
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
			DLITEM dli;
			dli.mask = DLI_FILENAME;
			DirList_GetItem(hwndDirList, -1, &dli);
			OpenContainingFolder(hwndMain, dli.szFileName, TRUE);
		} else {
			OpenContainingFolder(hwndMain, szCurDir, FALSE);
		}
	}
	break;

	case IDM_VIEW_NEWWINDOW:
	case IDM_FILE_RESTART: {
		WCHAR szModuleName[MAX_PATH];
		WCHAR szParameters[1024];

		GetModuleFileName(NULL, szModuleName, COUNTOF(szModuleName));
		GetRelaunchParameters(szParameters);
		const LONG_PTR result = (LONG_PTR)ShellExecute(hwnd, NULL, szModuleName, szParameters, NULL, SW_SHOWNORMAL);
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
			DLITEM dli;
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
			lstrcpy(tchFilter, L"*.*");
			bNegFilter = FALSE;

			// Store information about currently selected item
			DLITEM dli;
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
		ChangeDirectory(hwnd, NULL, TRUE);
		break;

	case IDM_VIEW_FAVORITES:
		// Goto Favorites Directory
		DisplayPath(tchFavoritesDir, IDS_ERR_FAVORITES);
		break;

	case IDM_VIEW_EDITFAVORITES: {
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

	case IDM_VIEW_TOOLBAR:
		bShowToolbar = !bShowToolbar;
		ShowWindow(hwndReBar, bShowToolbar ? SW_SHOW : SW_HIDE);
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_CUSTOMIZETB:
		SendMessage(hwndToolbar, TB_CUSTOMIZE, 0, 0);
		break;

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
		const BOOL back = bWindowLayoutRTL;
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
		nSortFlags = DS_NAME;
		DirList_Sort(hwndDirList, nSortFlags, fSortRev);
		break;

	case IDM_SORT_SIZE:
		nSortFlags = DS_SIZE;
		DirList_Sort(hwndDirList, nSortFlags, fSortRev);
		break;

	case IDM_SORT_TYPE:
		nSortFlags = DS_TYPE;
		DirList_Sort(hwndDirList, nSortFlags, fSortRev);
		break;

	case IDM_SORT_DATE:
		nSortFlags = DS_LASTMOD;
		DirList_Sort(hwndDirList, nSortFlags, fSortRev);
		break;

	case IDM_SORT_REVERSE:
		fSortRev = !fSortRev;
		DirList_Sort(hwndDirList, nSortFlags, fSortRev);
		break;

	case IDM_POP_COPY_PATHNAME:
	case IDM_POP_COPY_FILENAME: {
		DLITEM dli;
		dli.mask = DLI_FILENAME;
		DirList_GetItem(hwndDirList, -1, &dli);

		LPCWSTR path = dli.szFileName;
		if (LOWORD(wParam) == IDM_POP_COPY_FILENAME) {
			path = PathFindFileName(path);
		}

		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							sizeof(WCHAR) * (lstrlen(path) + 1));
		LPWSTR pData = (LPWSTR)GlobalLock(hData);
		lstrcpy(pData, path);
		GlobalUnlock(hData);

		if (OpenClipboard(hwnd)) {
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hData);
			CloseClipboard();
		} else {
			GlobalFree(hData);
		}
	}
	break;

	case ACC_ESCAPE:
		if (ComboBox_GetDroppedState(hwndDriveBox)) {
			ComboBox_ShowDropdown(hwndDriveBox, FALSE);
		} else if (iEscFunction == 1) {
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		} else if (iEscFunction == 2) {
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
		bTransparentMode = bTransparentMode ? 0 : 1;
		SetWindowTransparentMode(hwnd, bTransparentMode, iOpacityLevel);
		break;

	case ACC_GOTOTARGET: {
		DLITEM dli = { DLI_ALL, L"", L"", DLE_NONE };
		DirList_GetItem(hwndDirList, -1, &dli);

		if (dli.ntype == DLE_FILE) {
			if (PathIsLnkFile(dli.szFileName)) {
				WCHAR szFullPath[MAX_PATH];

				//SetFocus(hwndDirList);
				if (PathGetLnkPath(dli.szFileName, szFullPath, COUNTOF(szFullPath))) {
					if (GetFileAttributes(szFullPath) != INVALID_FILE_ATTRIBUTES) {
						WCHAR szDir[MAX_PATH];
						WCHAR *p;
						lstrcpy(szDir, szFullPath);
						if ((p = StrRChr(szDir, NULL, L'\\')) != NULL) {
							*(p + 1) = 0;
							if (!PathIsRoot(szDir)) {
								*p = 0;
							}

							SetCurrentDirectory(szDir);
							SendWMCommand(hwndMain, IDM_VIEW_UPDATE);
							if (!DirList_SelectItem(hwndDirList, NULL, szFullPath)) {
								ListView_EnsureVisible(hwndDirList, 0, FALSE);
							}
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
		LaunchTarget(L"", TRUE);
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
		if (History_CanBack(&mHistory)) {
			WCHAR tch[MAX_PATH];
			History_Back(&mHistory, tch, COUNTOF(tch));
			if (!ChangeDirectory(hwnd, tch, FALSE)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		} else {
			MessageBeep(MB_OK);
		}
		History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		break;

	case IDT_HISTORY_FORWARD:
		if (History_CanForward(&mHistory)) {
			WCHAR tch[MAX_PATH];
			History_Forward(&mHistory, tch, COUNTOF(tch));
			if (!ChangeDirectory(hwnd, tch, FALSE)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		} else {
			MessageBeep(MB_OK);
		}
		History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		break;

	case IDT_UP_DIR: {
		if (!PathIsRoot(szCurDir)) {
			if (!ChangeDirectory(hwnd, L"..", TRUE)) {
				MsgBoxWarn(MB_OK, IDS_ERR_CD);
			}
		} else {
			MessageBeep(MB_OK);
		}
	}
	break;

	case IDT_ROOT_DIR: {
		if (!PathIsRoot(szCurDir)) {
			if (!ChangeDirectory(hwnd, L"\\", TRUE)) {
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

		DLITEM dli;
		dli.ntype = DLE_NONE;
		dli.mask = DLI_TYPE | DLI_FILENAME;

		int i;
		WCHAR tch[MAX_PATH];
		for (i = iItem + d; DirList_GetItem(hwndDirList, i, &dli) != (-1); i++) {
			if (dli.ntype == DLE_FILE && !PathIsLnkToDirectory(dli.szFileName, tch, COUNTOF(tch))) {
				break;
			}
		}

		if (dli.ntype != DLE_FILE) {
			for (i = 0; i <= iItem; i++) {
				DirList_GetItem(hwndDirList, i, &dli);
				if (dli.ntype == DLE_FILE && !PathIsLnkToDirectory(dli.szFileName, tch, COUNTOF(tch))) {
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

		DLITEM dli;
		dli.ntype = DLE_NONE;
		dli.mask = DLI_TYPE | DLI_FILENAME;

		int i;
		WCHAR tch[MAX_PATH];
		for (i = iItem - d; i > (-1); i--) {
			DirList_GetItem(hwndDirList, i, &dli);
			if (dli.ntype == DLE_FILE && !PathIsLnkToDirectory(dli.szFileName, tch, COUNTOF(tch))) {
				break;
			}
		}

		if (dli.ntype != DLE_FILE) {
			for (i = ListView_GetItemCount(hwndDirList) - 1; i >= iItem; i--) {
				DirList_GetItem(hwndDirList, i, &dli);
				if (dli.ntype == DLE_FILE && !PathIsLnkToDirectory(dli.szFileName, tch, COUNTOF(tch))) {
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
			const BOOL fUseRecycleBin2 = fUseRecycleBin;
			fUseRecycleBin = 1;
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

	LPNMHDR pnmh = (LPNMHDR)lParam;

	switch (pnmh->idFrom) {
	case IDC_DIRLIST:
		switch (pnmh->code) {
		case NM_SETFOCUS:
			nIdFocus = IDC_DIRLIST;
			break;

		case LVN_GETDISPINFO:
			DirList_GetDispInfo(hwndDirList, lParam, flagNoFadeHidden);
			break;

		case LVN_DELETEITEM:
			DirList_DeleteItem(hwndDirList, lParam);
			break;

		case LVN_BEGINDRAG:
		case LVN_BEGINRDRAG:
			DirList_DoDragDrop(hwndDirList, lParam);
			break;

		case LVN_ITEMCHANGED: {
			const NM_LISTVIEW *pnmlv = (NM_LISTVIEW *)lParam;

			if ((pnmlv->uNewState & (LVIS_SELECTED | LVIS_FOCUSED)) !=
					(pnmlv->uOldState & (LVIS_SELECTED | LVIS_FOCUSED))) {

				WCHAR tch[64];
				if ((pnmlv->uNewState & LVIS_SELECTED)) {
					WIN32_FIND_DATA fd;
					DLITEM dli;
					dli.mask  = DLI_FILENAME;
					dli.ntype = DLE_NONE;
					DirList_GetItem(hwndDirList, -1, &dli);
					DirList_GetItemEx(hwndDirList, -1, &fd);

					if (fd.nFileSizeLow >= MAXDWORD) {
						GetFileAttributesEx(dli.szFileName, GetFileExInfoStandard, &fd);
					}

					const LONGLONG isize = (((LONGLONG)fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;
					WCHAR tchsize[64];
					StrFormatByteSize(isize, tchsize, COUNTOF(tchsize));

					WCHAR tchdate[64];
					WCHAR tchtime[64];
					FILETIME ft;
					SYSTEMTIME st;
					FileTimeToLocalFileTime(&fd.ftLastWriteTime, &ft);
					FileTimeToSystemTime(&ft, &st);
					GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, tchdate, COUNTOF(tchdate));
					GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, tchtime, COUNTOF(tchdate));

					WCHAR tchattr[64];
					lstrcpy(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? L"A" : L"-");
					lstrcat(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? L"R" : L"-");
					lstrcat(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? L"H" : L"-");
					lstrcat(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? L"S" : L"-");

					wsprintf(tch, L"%s | %s %s | %s", tchsize, tchdate, tchtime, tchattr);
				} else {
					WCHAR tchnum[64];
					WCHAR fmt[64];
					_ltow(ListView_GetItemCount(hwndDirList), tchnum, 10);
					FormatNumberStr(tchnum);
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
			History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
			Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, HasFilter() ? TB_DEL_FILTER_BMP : TB_ADD_FILTER_BMP);
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
		}
		break;

	default:
		switch (pnmh->code) {
		case TTN_NEEDTEXT: {
			LPTOOLTIPTEXT pTTT = (LPTOOLTIPTEXT)lParam;
			if (pTTT->uFlags & TTF_IDISHWND) {
				PathCompactPathEx(pTTT->szText, szCurDir, COUNTOF(pTTT->szText), 0);
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

//=============================================================================
//
//  ChangeDirectory()
//
//
BOOL ChangeDirectory(HWND hwnd, LPCWSTR lpszNewDir, BOOL bUpdateHistory) {
	if (lpszNewDir && !SetCurrentDirectory(lpszNewDir)) {
		return FALSE;
	}

	const BOOL fUpdate = lpszNewDir != NULL;
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
			const int iJump = min_i(iTopItem + ListView_GetCountPerPage(hwndDirList), cItems - 1);
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
		_ltow(cItems, tchnum, 10);
		FormatNumberStr(tchnum);
		WCHAR fmt[64];
		FormatString(tch, fmt, HasFilter() ? IDS_NUMFILES_FILTER : IDS_NUMFILES, tchnum);
		StatusSetText(hwndStatus, ID_FILEINFO, tch);

		// Update History
		if (bUpdateHistory) {
			History_Add(&mHistory, szCurDir);
			History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		}
	}
	EndWaitCursor();

	return TRUE;
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

#if NP2_ENABLE_APP_LOCALIZATION_DLL
void ValidateUILangauge(void) {
	const LANGID subLang = SUBLANGID(uiLanguage);
	switch (PRIMARYLANGID(uiLanguage)) {
	case LANG_ENGLISH:
		languageResID = IDS_LANG_ENGLISH_US;
		break;
	case LANG_CHINESE:
		languageResID = IsChineseTraditionalSubLang(subLang)? IDS_LANG_CHINESE_TRADITIONAL : IDS_LANG_CHINESE_SIMPLIFIED;
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
	case LANG_NEUTRAL:
	default:
		languageResID = IDS_LANG_USER_DEFAULT;
		uiLanguage = LANG_USER_DEFAULT;
		break;
	}
}

void SetUILanguage(int resID) {
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
	case IDS_LANG_GERMAN:
		lang = MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN);
		break;
	case IDS_LANG_ITALIAN:
		lang = MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN);
		break;
	case IDS_LANG_JAPANESE:
		lang = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
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
void LoadSettings(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, 128);
	LoadIniSection(INI_SECTION_NAME_SETTINGS, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	bSaveSettings = IniSectionGetBool(pIniSection, L"SaveSettings", 1);
	bSingleClick = IniSectionGetBool(pIniSection, L"SingleClick", 1);
	bOpenFileInSameWindow = IniSectionGetBool(pIniSection, L"OpenFileInSameWindow", 0);
	iDefaultOpenMenu = bOpenFileInSameWindow ? IDM_FILE_OPENSAME : IDM_FILE_OPENNEW;
	iShiftOpenMenu = bOpenFileInSameWindow ? IDM_FILE_OPENNEW : IDM_FILE_OPENSAME;

	bTrackSelect = IniSectionGetBool(pIniSection, L"TrackSelect", 1);
	bFullRowSelect = IniSectionGetBool(pIniSection, L"FullRowSelect", 0);
	fUseRecycleBin = IniSectionGetBool(pIniSection, L"UseRecycleBin", 1);
	fNoConfirmDelete = IniSectionGetBool(pIniSection, L"NoConfirmDelete", 0);
	bClearReadOnly = IniSectionGetBool(pIniSection, L"ClearReadOnly", 1);
	bRenameOnCollision = IniSectionGetBool(pIniSection, L"RenameOnCollision", 0);
	bFocusEdit = IniSectionGetBool(pIniSection, L"FocusEdit", 1);
	bAlwaysOnTop = IniSectionGetBool(pIniSection, L"AlwaysOnTop", 0);
	bMinimizeToTray = IniSectionGetBool(pIniSection, L"MinimizeToTray", 0);
	bTransparentMode = IniSectionGetBool(pIniSection, L"TransparentMode", 0);
	bWindowLayoutRTL = IniSectionGetBool(pIniSection, L"WindowLayoutRTL", 0);

	int iValue = IniSectionGetInt(pIniSection, L"EscFunction", 0);
	iEscFunction = clamp_i(iValue, 0, 2);

	iValue = IniSectionGetInt(pIniSection, L"StartupDirectory", 1);
	iStartupDir = clamp_i(iValue, 0, 2);

	IniSectionGetString(pIniSection, L"MRUDirectory", L"", szMRUDirectory, COUNTOF(szMRUDirectory));

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

	strValue = IniSectionGetValue(pIniSection, L"Quikview.exe");
	if (StrIsEmpty(strValue)) {
		GetSystemDirectory(szQuickview, COUNTOF(szQuickview));
		PathAddBackslash(szQuickview);
		lstrcat(szQuickview, L"Viewers\\Quikview.exe");
	} else {
		PathAbsoluteFromApp(strValue, szQuickview, COUNTOF(szQuickview), TRUE);
	}

	bHasQuickview = PathIsFile(szQuickview);
	IniSectionGetString(pIniSection, L"QuikviewParams", L"",
						szQuickviewParams, COUNTOF(szQuickviewParams));

	dwFillMask = IniSectionGetInt(pIniSection, L"FillMask", DL_ALLOBJECTS);
	if (dwFillMask & ~DL_ALLOBJECTS) {
		dwFillMask = DL_ALLOBJECTS;
	}

	iValue = IniSectionGetInt(pIniSection, L"SortOptions", DS_NAME);
	nSortFlags = clamp_i(iValue, 0, 3);

	fSortRev = IniSectionGetBool(pIniSection, L"SortReverse", 0);

	if (!lpFilterArg) {
		strValue = IniSectionGetValue(pIniSection, L"FileFilter");
		if (StrIsEmpty(strValue)) {
			lstrcpy(tchFilter, L"*.*");
		} else {
			lstrcpyn(tchFilter, strValue, COUNTOF(tchFilter));
		}
		bNegFilter = IniSectionGetBool(pIniSection, L"NegativeFilter", 0);
	} else { // ignore filter if /m was specified
		if (*lpFilterArg == L'-') {
			bNegFilter = TRUE;
			lstrcpyn(tchFilter, lpFilterArg + 1, COUNTOF(tchFilter));
		} else {
			bNegFilter = FALSE;
			lstrcpyn(tchFilter, lpFilterArg, COUNTOF(tchFilter));
		}
	}

	bDefColorNoFilter = IniSectionGetBool(pIniSection, L"DefColorNoFilter", 1);
	bDefColorFilter = IniSectionGetBool(pIniSection, L"DefColorFilter", 1);

	colorNoFilter = IniSectionGetInt(pIniSection, L"ColorNoFilter", GetSysColor(COLOR_WINDOWTEXT));
	colorFilter = IniSectionGetInt(pIniSection, L"ColorFilter", GetSysColor(COLOR_HIGHLIGHT));

	strValue = IniSectionGetValue(pIniSection, L"ToolbarButtons");
	if (StrIsEmpty(strValue)) {
		CopyMemory(tchToolbarButtons, DefaultToolbarButtons, sizeof(DefaultToolbarButtons));
	} else {
		lstrcpyn(tchToolbarButtons, strValue, COUNTOF(tchToolbarButtons));
	}

	bShowToolbar = IniSectionGetBool(pIniSection, L"ShowToolbar", 1);
	bShowStatusbar = IniSectionGetBool(pIniSection, L"ShowStatusbar", 1);
	bShowDriveBox = IniSectionGetBool(pIniSection, L"ShowDriveBox", 1);

	// toolbar image
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
		}

		cxRunDlg = IniSectionGetInt(pIniSection, L"RunDlgSizeX", 0);
		cxGotoDlg = IniSectionGetInt(pIniSection, L"GotoDlgSizeX", 0);
		cxFileFilterDlg = IniSectionGetInt(pIniSection, L"FileFilterDlgX", 0);
		cxRenameFileDlg = IniSectionGetInt(pIniSection, L"RenameFileDlgX", 0);
		cxNewDirectoryDlg = IniSectionGetInt(pIniSection, L"NewDirectoryDlgX", 0);
		cxOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeX", 0);
		cyOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeY", 0);
		cxCopyMoveDlg = IniSectionGetInt(pIniSection, L"CopyMoveDlgSizeX", 0);
		cxTargetApplicationDlg = IniSectionGetInt(pIniSection, L"TargetApplicationDlgSizeX", 0);
		cxFindWindowDlg = IniSectionGetInt(pIniSection, L"FindWindowDlgSizeX", 0);
	}

	IniSectionFree(pIniSection);
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

void SaveSettingsNow(void) {
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
		if (WritePrivateProfileString(INI_SECTION_NAME_SETTINGS, L"WriteTest", L"ok", szIniFile)) {
			BeginWaitCursor();
			if (CreateIniFile(szIniFile)) {
				SaveSettings(TRUE);
			} else {
				bCreateFailure = TRUE;
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
void SaveSettings(BOOL bSaveSettingsNow) {
	if (!CreateIniFile(szIniFile)) {
		return;
	}

	if (!bSaveSettings && !bSaveSettingsNow) {
		if (iStartupDir == 1) {
			IniSetString(INI_SECTION_NAME_SETTINGS, L"MRUDirectory", szCurDir);
		}
		IniSetBool(INI_SECTION_NAME_SETTINGS, L"SaveSettings", bSaveSettings);
		return;
	}

	WCHAR wchTmp[MAX_PATH];
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_SETTINGS);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	IniSectionSetBoolEx(pIniSection, L"SaveSettings", bSaveSettings, 1);
	IniSectionSetBoolEx(pIniSection, L"SingleClick", bSingleClick, 1);
	IniSectionSetBoolEx(pIniSection, L"OpenFileInSameWindow", bOpenFileInSameWindow, 0);
	IniSectionSetBoolEx(pIniSection, L"TrackSelect", bTrackSelect, 1);
	IniSectionSetBoolEx(pIniSection, L"FullRowSelect", bFullRowSelect, 0);
	IniSectionSetBoolEx(pIniSection, L"UseRecycleBin", fUseRecycleBin, 1);
	IniSectionSetBoolEx(pIniSection, L"NoConfirmDelete", fNoConfirmDelete, 0);
	IniSectionSetBoolEx(pIniSection, L"ClearReadOnly", bClearReadOnly, 1);
	IniSectionSetBoolEx(pIniSection, L"RenameOnCollision", bRenameOnCollision, 0);
	IniSectionSetBoolEx(pIniSection, L"FocusEdit", bFocusEdit, 1);
	IniSectionSetBoolEx(pIniSection, L"AlwaysOnTop", bAlwaysOnTop, 0);
	IniSectionSetBoolEx(pIniSection, L"MinimizeToTray", bMinimizeToTray, 0);
	IniSectionSetBoolEx(pIniSection, L"TransparentMode", bTransparentMode, 0);
	IniSectionSetBoolEx(pIniSection, L"WindowLayoutRTL", bWindowLayoutRTL, 0);
	IniSectionSetBoolEx(pIniSection, L"EscFunction", iEscFunction, 0);

	IniSectionSetIntEx(pIniSection, L"StartupDirectory", iStartupDir, 1);
	if (iStartupDir == 1) {
		IniSectionSetString(pIniSection, L"MRUDirectory", szCurDir);
	}
	PathRelativeToApp(tchFavoritesDir, wchTmp, COUNTOF(wchTmp), FALSE, TRUE, flagPortableMyDocs);
	IniSectionSetString(pIniSection, L"Favorites", wchTmp);
	PathRelativeToApp(szQuickview, wchTmp, COUNTOF(wchTmp), FALSE, TRUE, flagPortableMyDocs);
	IniSectionSetString(pIniSection, L"Quikview.exe", wchTmp);
	IniSectionSetStringEx(pIniSection, L"QuikviewParams", szQuickviewParams, L"");
	PathRelativeToApp(tchOpenWithDir, wchTmp, COUNTOF(wchTmp), FALSE, TRUE, flagPortableMyDocs);
	IniSectionSetString(pIniSection, L"OpenWithDir", wchTmp);
	IniSectionSetIntEx(pIniSection, L"FillMask", dwFillMask, DL_ALLOBJECTS);
	IniSectionSetIntEx(pIniSection, L"SortOptions", nSortFlags, DS_NAME);
	IniSectionSetBoolEx(pIniSection, L"SortReverse", fSortRev, 0);
	IniSectionSetStringEx(pIniSection, L"FileFilter", tchFilter, L"*.*");
	IniSectionSetBoolEx(pIniSection, L"NegativeFilter", bNegFilter, 0);
	IniSectionSetBoolEx(pIniSection, L"DefColorNoFilter", bDefColorNoFilter, 1);
	IniSectionSetBoolEx(pIniSection, L"DefColorFilter", bDefColorFilter, 1);
	IniSectionSetIntEx(pIniSection, L"ColorNoFilter", colorNoFilter, GetSysColor(COLOR_WINDOWTEXT));
	IniSectionSetIntEx(pIniSection, L"ColorFilter", colorFilter, GetSysColor(COLOR_HIGHLIGHT));
	Toolbar_GetButtons(hwndToolbar, TOOLBAR_COMMAND_BASE, tchToolbarButtons, COUNTOF(tchToolbarButtons));
	IniSectionSetStringEx(pIniSection, L"ToolbarButtons", tchToolbarButtons, DefaultToolbarButtons);
	IniSectionSetBoolEx(pIniSection, L"ShowToolbar", bShowToolbar, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowStatusbar", bShowStatusbar, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowDriveBox", bShowDriveBox, 1);

	SaveIniSection(INI_SECTION_NAME_SETTINGS, pIniSectionBuf);
	SaveWindowPosition(bSaveSettingsNow, pIniSectionBuf);
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
	}

	IniSectionSetInt(pIniSection, L"WindowPosX", wi.x);
	IniSectionSetInt(pIniSection, L"WindowPosY", wi.y);
	IniSectionSetInt(pIniSection, L"WindowSizeX", wi.cx);
	IniSectionSetInt(pIniSection, L"WindowSizeY", wi.cy);

	IniSectionSetIntEx(pIniSection, L"RunDlgSizeX", cxRunDlg, 0);
	IniSectionSetIntEx(pIniSection, L"GotoDlgSizeX", cxGotoDlg, 0);
	IniSectionSetIntEx(pIniSection, L"FileFilterDlgX", cxFileFilterDlg, 0);
	IniSectionSetIntEx(pIniSection, L"RenameFileDlgX", cxRenameFileDlg, 0);
	IniSectionSetIntEx(pIniSection, L"NewDirectoryDlgX", cxNewDirectoryDlg, 0);
	IniSectionSetIntEx(pIniSection, L"OpenWithDlgSizeX", cxOpenWithDlg, 0);
	IniSectionSetIntEx(pIniSection, L"OpenWithDlgSizeY", cyOpenWithDlg, 0);
	IniSectionSetIntEx(pIniSection, L"CopyMoveDlgSizeX", cxCopyMoveDlg, 0);
	IniSectionSetIntEx(pIniSection, L"TargetApplicationDlgSizeX", cxTargetApplicationDlg, 0);
	IniSectionSetIntEx(pIniSection, L"FindWindowDlgSizeX", cxFindWindowDlg, 0);

	SaveIniSection(sectionName, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
}

void ClearWindowPositionHistory(void) {
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
int ParseCommandLineOption(LPWSTR lp1, LPWSTR lp2) {
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
			flagGotoFavorites = 1;
			state = 1;
			break;

		case L'I':
			flagStartAsTrayIcon = 1;
			state = 1;
			break;

		case L'M':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				if (lpFilterArg) {
					NP2HeapFree(lpFilterArg);
				}

				lpFilterArg = (LPWSTR)NP2HeapAlloc(sizeof(WCHAR) * (lstrlen(lp1) + 1));
				lstrcpy(lpFilterArg, lp1);
				state = 1;
			}
			break;

		case L'N':
			flagNoReuseWindow = 1;
			state = 1;
			break;

		case L'P':
			state = 2;
			if (ExtractFirstArgument(lp2, lp1, lp2)) {
				int cord[4] = { 0 };
				const int itok = ParseCommaList(lp1, cord, COUNTOF(cord));
				if (itok == 4) {
					flagPosParam = 1;
					state = 1;
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
				lstrcpy(szIniFile, L"*?");
				state = 1;
			}
			break;

		case L'P':
			switch (chNext) {
			case L'D':
			case L'S':
				flagPosParam = 1;
				state = 1;
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

	LPWSTR lp2 = (LPWSTR)NP2HeapAlloc(cmdSize);
	while (ExtractFirstArgument(lp3, lp1, lp2)) {
		// options
		if (*lp1 == L'/' || *lp1 == L'-') {
			const int state = ParseCommandLineOption(lp1, lp2);
			if (state == 1) {
				lstrcpy(lp3, lp2);
				continue;
			}
		}

		// pathname
		{
			if (lpPathArg) {
				GlobalFree(lpPathArg);
			}

			lpPathArg = (LPWSTR)GlobalAlloc(GPTR, sizeof(WCHAR) * (MAX_PATH + 2));
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
void LoadFlags(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_FLAGS);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, 16);
	LoadIniSection(INI_SECTION_NAME_FLAGS, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	uiLanguage = (LANGID)IniSectionGetInt(pIniSection, L"UILanguage", LANG_USER_DEFAULT);
	ValidateUILangauge();
#endif

	bReuseWindow = IniSectionGetBool(pIniSection, L"ReuseWindow", 0);
	if (!flagNoReuseWindow) {
		flagNoReuseWindow = !bReuseWindow;
	}

	flagPortableMyDocs = IniSectionGetBool(pIniSection, L"PortableMyDocs", 1);

	int iValue = IniSectionGetInt(pIniSection, L"AutoRefreshRate", 30);
	iAutoRefreshRate = max_i(iValue, 0);

	flagNoFadeHidden = IniSectionGetBool(pIniSection, L"NoFadeHidden", 0);

	iValue = IniSectionGetInt(pIniSection, L"OpacityLevel", 75);
	iOpacityLevel = validate_i(iValue, 0, 100, 75);

	iValue = IniSectionGetInt(pIniSection, L"ToolbarLook", 1);
	flagToolbarLook = clamp_i(iValue, 0, 2);

	if (StrIsEmpty(g_wchAppUserModelID)) {
		LPCWSTR strValue = IniSectionGetValue(pIniSection, L"ShellAppUserModelID");
		if (StrNotEmpty(strValue)) {
			lstrcpyn(g_wchAppUserModelID, strValue, COUNTOF(g_wchAppUserModelID));
		} else {
			lstrcpy(g_wchAppUserModelID, MY_APPUSERMODELID);
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
//  FindIniFile()
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

BOOL CheckIniFileRedirect(LPWSTR lpszFile, LPCWSTR lpszModule) {
	WCHAR tch[MAX_PATH];
	if (GetPrivateProfileString(INI_SECTION_NAME_METAPATH, L"metapath.ini", L"", tch, COUNTOF(tch), lpszFile)) {
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
		return TRUE;
	}

	lstrcpy(tchTest, PathFindFileName(tchModule));
	PathRenameExtension(tchTest, L".ini");
	BOOL bFound = CheckIniFile(tchTest, tchModule);

	if (!bFound) {
		lstrcpy(tchTest, L"metapath.ini");
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

	return TRUE;
}

BOOL TestIniFile(void) {
	if (StrEqual(szIniFile, L"*?")) {
		lstrcpy(szIniFile2, L"");
		lstrcpy(szIniFile, L"");
		return 0;
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
			lstrcpy(PathFindFileName(szIniFile), L"metapath.ini");
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
		if (hFile != INVALID_HANDLE_VALUE) {
			LARGE_INTEGER fileSize;
			fileSize.QuadPart = 0;
			if (GetFileSizeEx(hFile, &fileSize) && fileSize.QuadPart < 2) {
				DWORD dw;
				WriteFile(hFile, (LPCVOID)L"\xFEFF[metapath]\r\n", 26, &dw, NULL);
			}
			CloseHandle(hFile);
			return TRUE;
		}
	}

	return FALSE;
}

//=============================================================================
//
//  DisplayPath()
//
//
BOOL DisplayPath(LPCWSTR lpPath, UINT uIdError) {
	if (StrIsEmpty(lpPath)) {
		return FALSE;
	}

	WCHAR szTmp[MAX_PATH];
	lstrcpy(szTmp, lpPath);
	ExpandEnvironmentStringsEx(szTmp, COUNTOF(szTmp));

	WCHAR szPath[MAX_PATH];
	if (!SearchPathEx(szTmp, COUNTOF(szPath), szPath)) {
		lstrcpy(szPath, szTmp);
	}

	if (PathIsLnkFile(szPath)) {
		return DisplayLnkFile(szPath);
	}

	const DWORD dwAttr = GetFileAttributes(szPath);
	if (dwAttr != INVALID_FILE_ATTRIBUTES) {
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
			if (!SetCurrentDirectory(szPath)) {
				MsgBoxWarn(MB_OK, uIdError);
				return FALSE;
			}
			PostWMCommand(hwndMain, IDM_VIEW_UPDATE);
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
			return TRUE;
		}
		{
			// szPath will be modified...
			lstrcpy(szTmp, szPath);

			SHFILEINFO shfi;
			SHGetFileInfo(szPath, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);

			WCHAR *p;
			if ((p = StrRChr(szPath, NULL, L'\\')) != NULL) {
				*(p + 1) = 0;
				if (!PathIsRoot(szPath)) {
					*p = 0;
				}
				SetCurrentDirectory(szPath);
			}

			SendWMCommand(hwndMain, IDM_VIEW_UPDATE);

			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, szTmp)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}

			return TRUE;
		}
	}

	MsgBoxWarn(MB_OK, uIdError);
	return FALSE;
}

//=============================================================================
//
//  DisplayLnkFile()
//
//
BOOL DisplayLnkFile(LPCWSTR pszLnkFile) {
	WCHAR szTmp[MAX_PATH];
	if (!PathGetLnkPath(pszLnkFile, szTmp, COUNTOF(szTmp))) {
		// Select lnk-file if target is not available
		if (PathIsFile(pszLnkFile)) {
			lstrcpy(szTmp, pszLnkFile);
			PathRemoveFileSpec(szTmp);
			SetCurrentDirectory(szTmp);

			// Select new file
			SendWMCommand(hwndMain, IDM_VIEW_UPDATE);
			SHFILEINFO shfi;
			SHGetFileInfo(pszLnkFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, pszLnkFile)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}
		}

		MsgBoxWarn(MB_OK, IDS_ERR_LNK_GETPATH);
		return FALSE;
	}

	ExpandEnvironmentStringsEx(szTmp, COUNTOF(szTmp));

	WCHAR szPath[MAX_PATH];
	if (!SearchPathEx(szTmp, COUNTOF(szPath), szPath)) {
		lstrcpy(szPath, szTmp);
	}

	const DWORD dwAttr = GetFileAttributes(szPath);
	if (dwAttr != INVALID_FILE_ATTRIBUTES) {
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
			if (!SetCurrentDirectory(szPath)) {
				MsgBoxWarn(MB_OK, IDS_ERR_LNK_NOACCESS);
				return FALSE;
			}
			PostWMCommand(hwndMain, IDM_VIEW_UPDATE);
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
			return TRUE;
		}

		// Current file is ShellLink, get dir and desc
		lstrcpy(szPath, pszLnkFile);

		SHFILEINFO shfi;
		SHGetFileInfo(szPath, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);

		WCHAR *p;
		if ((p = StrRChr(szPath, NULL, L'\\')) != NULL) {
			*(p + 1) = 0;
			if (!PathIsRoot(szPath)) {
				*p = 0;
			}
			SetCurrentDirectory(szPath);
		}

		LV_FINDINFO lvfi;
		lvfi.flags = LVFI_STRING;
		lvfi.psz   = shfi.szDisplayName;

		SendWMCommand(hwndMain, IDM_VIEW_UPDATE);
		const int i = ListView_FindItem(hwndDirList, -1, &lvfi);

		// found item that is currently displayed
		if (i != -1) {
			ListView_SetItemState(hwndDirList, i, LVIS_SELECTED | LVIS_FOCUSED,
								  LVIS_SELECTED | LVIS_FOCUSED);
			ListView_EnsureVisible(hwndDirList, i, FALSE);
		} else {
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
		}

		return TRUE;
	}

	// GetFileAttributes() failed
	// Select lnk-file if target is not available
	if (PathIsFile(pszLnkFile)) {
		lstrcpy(szTmp, pszLnkFile);
		PathRemoveFileSpec(szTmp);
		SetCurrentDirectory(szTmp);

		// Select new file
		SendWMCommand(hwndMain, IDM_VIEW_UPDATE);
		SHFILEINFO shfi;
		SHGetFileInfo(pszLnkFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
		if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, pszLnkFile)) {
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
		}
	}

	MsgBoxWarn(MB_OK, IDS_ERR_LNK_NOACCESS);
	return FALSE;
}

/******************************************************************************
*
* ActivatePrevInst()
*
* Tries to find and activate an already open metapath Window
*
*
******************************************************************************/
static BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam) {
	BOOL bContinue = TRUE;
	WCHAR szClassName[64];

	if (GetClassName(hwnd, szClassName, COUNTOF(szClassName))) {
		if (StrCaseEqual(szClassName, WC_METAPATH)) {
			*(HWND *)lParam = hwnd;
			if (IsWindowEnabled(hwnd)) {
				bContinue = FALSE;
			}
		}
	}

	return bContinue;
}

BOOL ActivatePrevInst(void) {
	if (flagNoReuseWindow || flagStartAsTrayIcon) {
		return FALSE;
	}

	HWND hwnd = NULL;
	EnumWindows(EnumWndProc, (LPARAM)&hwnd);

	// Found a window
	if (hwnd != NULL) {
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
				ExpandEnvironmentStringsEx(lpPathArg, (DWORD)(GlobalSize(lpPathArg) / sizeof(WCHAR)));

				if (PathIsRelative(lpPathArg)) {
					WCHAR tchTmp[MAX_PATH];
					GetCurrentDirectory(COUNTOF(tchTmp), tchTmp);
					PathAppend(tchTmp, lpPathArg);
					lstrcpy(lpPathArg, tchTmp);
				}

				COPYDATASTRUCT cds;
				cds.dwData = DATA_METAPATH_PATHARG;
				cds.cbData = (DWORD)GlobalSize(lpPathArg);
				cds.lpData = lpPathArg;

				// Send lpPathArg to previous instance
				SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&cds);

				GlobalFree(lpPathArg);
			}
			return TRUE;
		}

		if (MsgBoxAsk(MB_YESNO, IDS_ERR_PREVWINDISABLED) == IDYES) {
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}

void GetRelaunchParameters(LPWSTR szParameters) {
	lstrcpy(szParameters, L" -f");
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
void ShowNotifyIcon(HWND hwnd, BOOL bAdd) {
	static HICON hIcon;

	if (!hIcon) {
		hIcon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND),
						  IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	}

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = APPM_TRAYMESSAGE;
	nid.hIcon = hIcon;
	lstrcpy(nid.szTip, L"metapath");

	if (bAdd) {
		Shell_NotifyIcon(NIM_ADD, &nid);
	} else {
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}
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
BOOL bLoadLaunchSetingsLoaded = FALSE;

static BOOL CALLBACK EnumWndProc2(HWND hwnd, LPARAM lParam) {
	BOOL bContinue = TRUE;
	WCHAR szClassName[64];

	if (GetClassName(hwnd, szClassName, COUNTOF(szClassName))) {
		if (StrCaseEqual(szClassName, szGlobalWndClass)) {
			*(HWND *)lParam = hwnd;
			if (IsWindowEnabled(hwnd)) {
				bContinue = FALSE;
			}
		}
	}

	return bContinue;
}

void LoadLaunchSetings(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_TARGET_APPLICATION);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, 16);
	LoadIniSection(INI_SECTION_NAME_TARGET_APPLICATION, pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	iUseTargetApplication = IniSectionGetInt(pIniSection, L"UseTargetApplication", 0xFB);
	if (iUseTargetApplication != 0xFB) {
		IniSectionGetString(pIniSection, L"TargetApplicationPath", szTargetApplication, szTargetApplication, COUNTOF(szTargetApplication));
		IniSectionGetString(pIniSection, L"TargetApplicationParams", szTargetApplicationParams, szTargetApplicationParams, COUNTOF(szTargetApplicationParams));
		iTargetApplicationMode = IniSectionGetInt(pIniSection, L"TargetApplicationMode", iTargetApplicationMode);
		IniSectionGetString(pIniSection, L"TargetApplicationWndClass", szTargetApplicationWndClass, szTargetApplicationWndClass, COUNTOF(szTargetApplicationWndClass));
		IniSectionGetString(pIniSection, L"DDEMessage", szDDEMsg, szDDEMsg, COUNTOF(szDDEMsg));
		IniSectionGetString(pIniSection, L"DDEApplication", szDDEApp, szDDEApp, COUNTOF(szDDEApp));
		IniSectionGetString(pIniSection, L"DDETopic", szDDETopic, szDDETopic, COUNTOF(szDDETopic));
	} else if (iUseTargetApplication && StrIsEmpty(szTargetApplication)) {
		iUseTargetApplication = 1;
		iTargetApplicationMode = 1;
		lstrcpy(szTargetApplication, L"Notepad2.exe");
		lstrcpy(szTargetApplicationParams, L"");
		lstrcpy(szTargetApplicationWndClass, L"Notepad2");
		lstrcpy(szDDEMsg, L"");
		lstrcpy(szDDEApp, L"");
		lstrcpy(szDDETopic, L"");
	}

	lstrcpy(szGlobalWndClass, szTargetApplicationWndClass);
	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
	bLoadLaunchSetingsLoaded = TRUE;
}

void LaunchTarget(LPCWSTR lpFileName, BOOL bOpenNew) {
	if (!bLoadLaunchSetingsLoaded) {
		LoadLaunchSetings();
	}
	if (iUseTargetApplication == 4 || (iUseTargetApplication && StrIsEmpty(szTargetApplication))) {
		ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FINDTARGET), hwndMain, FindTargetDlgProc, 0);
		return;
	}

	if (iUseTargetApplication && iTargetApplicationMode == 1) {
		HWND hwnd = NULL;
		if (!bOpenNew) { // hwnd == NULL
			EnumWindows(EnumWndProc2, (LPARAM)&hwnd);
		}

		// Found a window
		if (hwnd != NULL && IsWindowEnabled(hwnd)) {
			if (IsIconic(hwnd)) {
				ShowWindowAsync(hwnd, SW_RESTORE);
			}
			if (bFocusEdit) {
				SetForegroundWindow(hwnd);
			}

			if (lpFileName) {
				HDROP hDrop = CreateDropHandle(lpFileName);
				PostMessage(hwnd, WM_DROPFILES, (WPARAM)hDrop, 0);
			}
		} else { // Either no window or disabled - run target.exe
			if (hwnd) { // disabled window
				if (MsgBoxAsk(MB_YESNO, IDS_ERR_TARGETDISABLED) == IDNO) {
					return;
				}
			}

			LPWSTR lpParam;
			WCHAR szTmp[MAX_PATH];
			if (PathIsLnkFile(lpFileName) && PathGetLnkPath(lpFileName, szTmp, COUNTOF(szTmp))) {
				lpParam = szTmp;
			} else {
				lpParam = (LPWSTR)lpFileName;
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
			PathAbsoluteFromApp(szTmp, szFile, COUNTOF(szFile), TRUE);

			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = 0;
			sei.hwnd = hwndMain;
			sei.lpVerb = NULL;
			sei.lpFile = szFile;
			sei.lpParameters = szParam;
			sei.lpDirectory = szCurDir;
			sei.nShow = SW_SHOWNORMAL;

			ShellExecuteEx(&sei);
		}
	} else {
		if (iUseTargetApplication &&
				iTargetApplicationMode == 2 &&
				ExecDDECommand(lpFileName, szDDEMsg, szDDEApp, szDDETopic)) {
			return;
		}

		if (!iUseTargetApplication && StrIsEmpty(lpFileName)) {
			return;
		}

		LPWSTR lpParam;
		WCHAR szTmp[MAX_PATH];
		if (PathIsLnkFile(lpFileName) && PathGetLnkPath(lpFileName, szTmp, COUNTOF(szTmp))) {
			lpParam = szTmp;
		} else {
			lpParam = (LPWSTR)lpFileName;
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
		PathAbsoluteFromApp(szTmp, szFile, COUNTOF(szFile), TRUE);

		SHELLEXECUTEINFO sei;
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = 0;
		sei.hwnd = hwndMain;
		sei.lpVerb = NULL;
		if (iUseTargetApplication) {
			sei.lpFile = szFile;
			sei.lpParameters = szParam;
		} else {
			sei.lpFile = lpParam;
			sei.lpParameters = NULL;
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
//  Aligns metapath to either side of target window
//
//
void SnapToTarget(HWND hwnd) {
	if (!bLoadLaunchSetingsLoaded) {
		LoadLaunchSetings();
	}

	HWND hwnd2 = NULL;
	EnumWindows(EnumWndProc2, (LPARAM)&hwnd2);

	// Found a window
	if (hwnd2 != NULL) {
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

			SetWindowPos(hwnd, NULL, rcNew.left, rcNew.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
	}
}

//=============================================================================
//
//  SnapToDefaultPos()
//
//  Aligns metapath to the default window position on the current screen
//
//
void SnapToDefaultPos(HWND hwnd) {
	RECT rcOld;
	GetWindowRect(hwnd, &rcOld);

	HMONITOR hMonitor = MonitorFromRect(&rcOld, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	int x = mi.rcWork.left + 16;
	const int y = mi.rcWork.top + 16;
	const int cx = 272;
	const int cy = 640;

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
