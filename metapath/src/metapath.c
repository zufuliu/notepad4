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
#include <shlwapi.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commdlg.h>
#include <stdio.h>
#include "Helpers.h"
#include "Dlapi.h"
#include "Dialogs.h"
#include "metapath.h"
#include "resource.h"

// enable customize toolbar labels
#define NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS		0

/******************************************************************************
*
* Local Variables for metapath.c
*
*/
HWND      hwndStatus;
HWND      hwndToolbar;
HWND      hwndReBar;

#define NUMTOOLBITMAPS  15
#define NUMINITIALTOOLS  6

#define TBFILTERBMP 13

#define DefaultToolbarButtons	L"1 2 3 4 5 0 8"
static TBBUTTON tbbMainWnd[] = {
	{0, IDT_HISTORY_BACK, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{1, IDT_HISTORY_FORWARD, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{2, IDT_UPDIR, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{3, IDT_ROOT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{4, IDT_VIEW_FAVORITES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{5, IDT_FILE_PREV, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{6, IDT_FILE_NEXT, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{7, IDT_FILE_RUN, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{8, IDT_FILE_QUICKVIEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{9, IDT_FILE_SAVEAS, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{10, IDT_FILE_COPYMOVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{11, IDT_FILE_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{12, IDT_FILE_DELETE2, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{13, IDT_VIEW_FILTER, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, 0},
	{0, 0, 0, TBSTYLE_SEP, {0}, 0, 0}
};

HWND      hwndDriveBox;
HWND      hwndDirList;

HWND      hwndMain;

HANDLE    hChangeHandle = NULL;

HISTORY   mHistory;

WCHAR      szIniFile[MAX_PATH] = L"";
WCHAR      szIniFile2[MAX_PATH] = L"";
BOOL      bSaveSettings;
WCHAR      szQuickview[MAX_PATH];
WCHAR      szQuickviewParams[MAX_PATH];
WCHAR      tchFavoritesDir[MAX_PATH];
WCHAR      tchOpenWithDir[MAX_PATH];
WCHAR      tchToolbarButtons[512];
WCHAR      tchToolbarBitmap[MAX_PATH];
WCHAR      tchToolbarBitmapHot[MAX_PATH];
WCHAR      tchToolbarBitmapDisabled[MAX_PATH];
BOOL      bClearReadOnly;
BOOL      bRenameOnCollision;
BOOL      bSingleClick;
BOOL      bTrackSelect;
BOOL      bFullRowSelect;
int       iStartupDir;
int       iEscFunction;
BOOL      bFocusEdit;
BOOL      bAlwaysOnTop;
BOOL      bTransparentMode;
BOOL      bMinimizeToTray;
BOOL      fUseRecycleBin;
BOOL      fNoConfirmDelete;
BOOL      bShowToolbar;
BOOL      bShowStatusbar;
BOOL      bShowDriveBox;
int       cxGotoDlg;
int       cxOpenWithDlg;
int       cyOpenWithDlg;
int       cxCopyMoveDlg;

WCHAR      tchFilter[DL_FILTER_BUFSIZE];
BOOL      bNegFilter;
BOOL      bDefCrNoFilt;
BOOL      bDefCrFilter;
COLORREF  crNoFilt;
COLORREF  crFilter;
COLORREF  crCustom[16];

typedef struct _wi {
	int x;
	int y;
	int cx;
	int cy;
} WININFO;

WININFO   wi;

int       cyReBar;
int       cyReBarFrame;
int       cyDriveBoxFrame;

int       nIdFocus = IDC_DIRLIST;

WCHAR      szCurDir[MAX_PATH + 40];
WCHAR	szMRUDirectory[MAX_PATH];
DWORD     dwFillMask;
int       nSortFlags;
BOOL      fSortRev;

LPWSTR     lpPathArg = NULL;
LPWSTR     lpFilterArg = NULL;

UINT      wFuncCopyMove = FO_COPY;

UINT      msgTaskbarCreated = 0;

int iUseTargetApplication = 4;
int iTargetApplicationMode = 0;
WCHAR szTargetApplication[MAX_PATH] = L"";
WCHAR szTargetApplicationParams[MAX_PATH] = L"";
WCHAR szTargetApplicationWndClass[MAX_PATH] = L"";
WCHAR szDDEMsg[256] = L"";
WCHAR szDDEApp[256] = L"";
WCHAR szDDETopic[256] = L"";

HINSTANCE	g_hInstance;
HANDLE		g_hDefaultHeap;
UINT16		g_uWinVer;

//=============================================================================
//
// Flags
//
int flagNoReuseWindow   = 0;
BOOL bReuseWindow       = FALSE;
int flagStartAsTrayIcon = 0;
int flagPortableMyDocs  = 0;
int flagGotoFavorites   = 0;
int iAutoRefreshRate    = 0; // unit: 1/10 sec
int flagNoFadeHidden    = 0;
int iOpacityLevel       = 75;
int flagToolbarLook     = 0;
int flagPosParam        = 0;

//=============================================================================
//
//  WinMain()
//
//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG    msg;
	HWND   hwnd;
	HACCEL hAcc;
	INITCOMMONCONTROLSEX icex;

	// Set global variable g_hInstance
	g_hInstance = hInstance;
	// Set the Windows version global variable
	g_uWinVer = LOWORD(GetVersion());
	g_uWinVer = MAKEWORD(HIBYTE(g_uWinVer), LOBYTE(g_uWinVer));
	g_hDefaultHeap = GetProcessHeap();

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
	// Command Line, Ini File and Flags
	ParseCommandLine();
	FindIniFile();
	TestIniFile();
	CreateIniFile();
	LoadFlags();

	// Try to activate another window
	if (ActivatePrevInst()) {
		return 0;
	}

	// Init OLE and Common Controls
	OleInitialize(NULL);

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	msgTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

	// Load Settings
	LoadSettings();

	if (!InitApplication(hInstance)) {
		return FALSE;
	}

	if ((hwnd = InitInstance(hInstance, nShowCmd)) == NULL) {
		return FALSE;
	}

	hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINWND));

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(hwnd, hAcc, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	OleUninitialize();

	return (int)(msg.wParam);
}

//=============================================================================
//
//  InitApplication()
//
//
BOOL InitApplication(HINSTANCE hInstance) {
	WNDCLASS wc;

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

	return RegisterClass(&wc);
}

//=============================================================================
//
//  InitInstance()
//
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow) {
	RECT rc = { wi.x, wi.y, wi.x + wi.cx, wi.y + wi.cy };
	RECT rc2;
	MONITORINFO mi;

	HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	if (wi.x == CW_USEDEFAULT || wi.y == CW_USEDEFAULT || wi.cx == CW_USEDEFAULT || wi.cy == CW_USEDEFAULT) {
		// default window position
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		wi.x = rc.left + 16;
		wi.y = rc.top + 16;
		wi.cx = 272;
		wi.cy = 640;
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
		SetWindowTransparentMode(hwndMain, TRUE);
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
		PostMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
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
			WINDOWPLACEMENT wndpl;

			// Terminate directory watching
			KillTimer(hwnd, ID_TIMER);
			FindCloseChangeNotification(hChangeHandle);

			// GetWindowPlacement
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
		LRESULT lret = DefWindowProc(hwnd, umsg, wParam, lParam);

		if (!StrEqual(tchFilter, L"*.*") || bNegFilter) {
			ListView_SetTextColor(hwndDirList, (bDefCrFilter) ? GetSysColor(COLOR_WINDOWTEXT) : crFilter);
			ListView_RedrawItems(hwndDirList, 0, ListView_GetItemCount(hwndDirList) - 1);
		} else {
			ListView_SetTextColor(hwndDirList, (bDefCrNoFilt) ? GetSysColor(COLOR_WINDOWTEXT) : crNoFilt);
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
		MSG  msg;
		HWND hwndTT = (HWND)SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0);

		if (wParam != HTCAPTION) {
			SendMessage(hwndTT, TTM_POP, 0, 0);
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}

		msg.hwnd = hwnd;
		msg.message = umsg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = GetMessageTime();
		msg.pt.x = HIWORD(GetMessagePos());
		msg.pt.y = LOWORD(GetMessagePos());

		SendMessage(hwndTT, TTM_RELAYEVENT, 0, (LPARAM)&msg);
	}
	return DefWindowProc(hwnd, umsg, wParam, lParam);

	case WM_TIMER:
		// Check Change Notification Handle
		if (WAIT_OBJECT_0 == WaitForSingleObject(hChangeHandle, 0)) {
			// Store information about currently selected item
			DLITEM dli;
			dli.mask  = DLI_ALL;
			dli.ntype = DLE_NONE;
			DirList_GetItem(hwndDirList, -1, &dli);

			FindNextChangeNotification(hChangeHandle);
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);

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
		//	ErrorMessage(1, IDS_ERR_DROP2);
		//}

		DragFinish(hDrop);
	}
	break;

	case WM_COPYDATA: {
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

		if (pcds->dwData == DATA_METAPATH_PATHARG) {
			LPWSTR lpsz = NP2HeapAlloc(pcds->cbData);
			CopyMemory(lpsz, pcds->lpData, pcds->cbData);

			DisplayPath(lpsz, IDS_ERR_CMDLINE);

			NP2HeapFree(lpsz);
		}
	}
	return TRUE;

	case WM_CONTEXTMENU: {
		HMENU hmenu;
		int   imenu = 0;
		DWORD dwpts;
		int   nID = GetDlgCtrlID((HWND)wParam);

		if (nID != IDC_DIRLIST && nID != IDC_DRIVEBOX &&
				nID != IDC_TOOLBAR && nID != IDC_STATUSBAR &&
				nID != IDC_REBAR) {
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}

		hmenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND));
		SetMenuDefaultItem(GetSubMenu(hmenu, 0), IDM_FILE_OPEN, FALSE);

		switch (nID) {
		case IDC_DIRLIST:
			if (ListView_GetSelectedCount(hwndDirList)) {
				imenu = 0;
			} else {
				imenu = 1;
			}
			break;

		case IDC_DRIVEBOX:
			imenu = 2;
			break;

		case IDC_TOOLBAR:
		case IDC_STATUSBAR:
		case IDC_REBAR:
			imenu = 3;
			break;
		}

		dwpts = GetMessagePos();

		TrackPopupMenuEx(GetSubMenu(hmenu, imenu),
						 TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
						 (int)(short)LOWORD(dwpts) + 1, (int)(short)HIWORD(dwpts) + 1, hwnd, NULL);

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
			LRESULT lrv = DefWindowProc(hwnd, umsg, wParam, lParam);
			ShowOwnedPopups(hwnd, TRUE);
			return (lrv);
		}

		case SC_ALWAYSONTOP:
			if (bAlwaysOnTop) {
				bAlwaysOnTop = 0;
				SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			} else {
				bAlwaysOnTop = 1;
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			break;

		case SC_ABOUT:
			ThemedDialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
			break;

		default:
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}
		break;

	case APPM_TRAYMESSAGE:
		switch (lParam) {
		case WM_RBUTTONUP: {

			HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND));
			HMENU hMenuPopup = GetSubMenu(hMenu, 4);

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

	HWND hwndTT;
	TOOLINFO ti;

	LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, L"", -1, 0, 0, 0 };

	HMENU hmenu;
	WCHAR  tch[64];
	MENUITEMINFO mii;

	HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
	DWORD dwDriveBoxStyle = WS_DRIVEBOX;

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

	if (IsVistaAndAbove() && PrivateIsAppThemed()) {
		SetWindowLongPtr(hwndDirList, GWL_EXSTYLE, GetWindowLongPtr(hwndDirList, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwndDirList, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
	}

	if (bShowDriveBox) {
		dwDriveBoxStyle |= WS_VISIBLE;
	}

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
	SendMessage(hwndDriveBox, CB_SETEXTENDEDUI, TRUE, 0);
	// DirList
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
		if (IsVistaAndAbove()) {
			SetTheme(hwndDirList, L"Explorer");
		}
	}

	ListView_SetHoverTime(hwndDirList, 10);
	// Drag & Drop
	DragAcceptFiles(hwnd, TRUE);
	// History
	History_Init(&mHistory);
	History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
	// ToolTip with Current Directory
	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND;
	ti.hwnd = hwnd;
	ti.uId = (UINT_PTR)hwnd;
	ti.lpszText = LPSTR_TEXTCALLBACK;

	hwndTT = (HWND)SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0);
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

	// System Menu
	hmenu = GetSystemMenu(hwnd, FALSE);

	// Remove unwanted items
	DeleteMenu(hmenu, SC_RESTORE, MF_BYCOMMAND);
	DeleteMenu(hmenu, SC_MAXIMIZE, MF_BYCOMMAND);

	// Mofify the L"Minimize" item
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_ID;
	GetMenuItemInfo(hmenu, SC_MINIMIZE, FALSE, &mii);
	mii.wID = SC_MINIMIZE | 0x02;
	SetMenuItemInfo(hmenu, SC_MINIMIZE, FALSE, &mii);

	// Add specific items
	GetString(SC_ALWAYSONTOP, tch, COUNTOF(tch));
	InsertMenu(hmenu, SC_MOVE, MF_BYCOMMAND | MF_STRING | MF_ENABLED, SC_ALWAYSONTOP, tch);
	GetString(SC_ABOUT, tch, COUNTOF(tch));
	InsertMenu(hmenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING | MF_ENABLED, SC_ABOUT, tch);
	InsertMenu(hmenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, NULL);

	return 0;
}

//=============================================================================
//
//  CreateBars() - Create Toolbar and Statusbar
//
//
void CreateBars(HWND hwnd, HINSTANCE hInstance) {
	RECT rc;

	BITMAP bmp;
	HBITMAP hbmp, hbmpCopy = NULL;
	HIMAGELIST himl;
	WCHAR szTmp[MAX_PATH];
	BOOL bExternalBitmap = FALSE;

	BOOL bIsAppThemed = PrivateIsAppThemed();

	const DWORD dwToolbarStyle = WS_TOOLBAR | TBSTYLE_FLAT | CCS_ADJUSTABLE;
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
	const int cbIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, COUNTOF(tbbMainWnd));
	LoadIniSection(L"Toolbar Labels", pIniSectionBuf, cbIniSection);
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
	//SendMessage(hwndToolbar, TB_SAVERESTORE, FALSE, (LPARAM)lptbsp);
	if (Toolbar_SetButtons(hwndToolbar, IDT_HISTORY_BACK, tchToolbarButtons, tbbMainWnd, COUNTOF(tbbMainWnd)) == 0) {
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
	rbBand.lpText     = L"Toolbar";
	rbBand.hwndChild  = hwndToolbar;
	rbBand.cxMinChild = (rc.right - rc.left) * COUNTOF(tbbMainWnd);
	rbBand.cyMinChild = (rc.bottom - rc.top) + 2 * rc.top;
	rbBand.cx         = 0;
	SendMessage(hwndReBar, RB_INSERTBAND, (WPARAM) - 1, (LPARAM)&rbBand);

	SetWindowPos(hwndReBar, NULL, 0, 0, 0, 0, SWP_NOZORDER);
	GetWindowRect(hwndReBar, &rc);
	cyReBar = rc.bottom - rc.top;

	cyReBarFrame = bIsAppThemed ? 0 : 2;
	cyDriveBoxFrame = (bIsAppThemed && IsVistaAndAbove()) ? 0 : 2;
}

//=============================================================================
//
//  MsgThemeChanged() - Handles WM_THEMECHANGED
//
//
void MsgThemeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	RECT rc;
	WCHAR chStatus[255];
	HINSTANCE hInstance = (HINSTANCE)(INT_PTR)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

	BOOL bIsAppThemed = PrivateIsAppThemed();

	if (IsVistaAndAbove() && bIsAppThemed) {
		SetWindowLongPtr(hwndDirList, GWL_EXSTYLE, GetWindowLongPtr(hwndDirList, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwndDirList, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		if (bFullRowSelect) {
			SetTheme(hwndDirList, L"Explorer");
		} else {
			SetTheme(hwndDirList, L"Listview");
		}
	} else {
		SetWindowLongPtr(hwndDirList, GWL_EXSTYLE, WS_EX_CLIENTEDGE | GetWindowLongPtr(hwndDirList, GWL_EXSTYLE));
		SetWindowPos(hwndDirList, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		if (bIsAppThemed) {
			SetTheme(hwndDirList, L"Listview");
		}
	}

	// recreate toolbar and statusbar
	SendMessage(hwndStatus, SB_GETTEXT, ID_FILEINFO, (LPARAM)chStatus);

	// recreate toolbar and statusbar
	Toolbar_GetButtons(hwndToolbar, IDT_HISTORY_BACK, tchToolbarButtons, COUNTOF(tchToolbarButtons));

	DestroyWindow(hwndToolbar);
	DestroyWindow(hwndReBar);
	DestroyWindow(hwndStatus);
	CreateBars(hwnd, hInstance);

	GetClientRect(hwnd, &rc);
	SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELONG(rc.right, rc.bottom));

	StatusSetText(hwndStatus, ID_FILEINFO, chStatus);
}

//=============================================================================
//
//  MsgSize() - Handles WM_SIZE
//
//
void MsgSize(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);

	RECT rc;
	int  x, y, cx, cy;
	HDWP hdwp;
	int  aWidth[1];

	if (wParam == SIZE_MINIMIZED) {
		return;
	}

	x  = 0;
	y  = 0;

	cx = LOWORD(lParam);
	cy = HIWORD(lParam);

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

	hdwp = BeginDeferWindowPos(2);
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
	aWidth[0] = -1;
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

	int i; // Helper
	DLITEM dli;
	HMENU hmenu = (HMENU)wParam;

	i = ListView_GetSelectedCount(hwndDirList);
	dli.mask = DLI_TYPE;
	dli.ntype = DLE_NONE;
	DirList_GetItem(hwndDirList, -1, &dli);

	EnableCmd(hmenu, IDM_FILE_LAUNCH, (i && dli.ntype == DLE_FILE));
	EnableCmd(hmenu, IDM_FILE_QUICKVIEW, (i && dli.ntype == DLE_FILE));
	EnableCmd(hmenu, IDM_FILE_OPENWITH, i);
	EnableCmd(hmenu, IDM_FILE_CREATELINK, i);
	EnableCmd(hmenu, IDM_FILE_SAVEAS, (i && dli.ntype == DLE_FILE));
	EnableCmd(hmenu, IDM_FILE_COPYMOVE, i);
	EnableCmd(hmenu, IDM_FILE_DELETE, i);
	EnableCmd(hmenu, IDM_FILE_RENAME, i);

	i = (SendMessage(hwndDriveBox, CB_GETCURSEL, 0, 0) != CB_ERR);
	EnableCmd(hmenu, IDM_FILE_DRIVEPROP, i);

	CheckCmd(hmenu, IDM_VIEW_FOLDERS, (dwFillMask & DL_FOLDERS));
	CheckCmd(hmenu, IDM_VIEW_FILES, (dwFillMask & DL_NONFOLDERS));
	CheckCmd(hmenu, IDM_VIEW_HIDDEN, (dwFillMask & DL_INCLHIDDEN));

	EnableCmd(hmenu, IDM_VIEW_FILTERALL, (!StrEqual(tchFilter, L"*.*") || bNegFilter));

	CheckCmd(hmenu, IDM_VIEW_TOOLBAR, bShowToolbar);
	EnableCmd(hmenu, IDM_VIEW_CUSTOMIZETB, bShowToolbar);
	CheckCmd(hmenu, IDM_VIEW_STATUSBAR, bShowStatusbar);
	CheckCmd(hmenu, IDM_VIEW_DRIVEBOX, bShowDriveBox);

	CheckMenuRadioItem(hmenu, IDM_SORT_NAME, IDM_SORT_DATE, IDM_SORT_NAME + nSortFlags, MF_BYCOMMAND);

	CheckCmd(hmenu, IDM_SORT_REVERSE, fSortRev);
	CheckCmd(hmenu, SC_ALWAYSONTOP, bAlwaysOnTop);

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

			if (DriveBox_GetSelDrive(hwndDriveBox, tch, COUNTOF(tch), TRUE)
					&& !PathIsSameRoot(szCurDir, tch)) {
				if (!ChangeDirectory(hwnd, tch, 1)) {
					ErrorMessage(2, IDS_ERR_CD);
					DriveBox_SelectDrive(hwndDriveBox, szCurDir);
				}
			}
			SetFocus(hwndDirList);
		}
		break;
		}
		break;

	case IDM_FILE_OPEN: {
		DLITEM dli = { DLI_ALL, L"", L"", DLE_NONE };

		DirList_GetItem(hwndDirList, -1, &dli);

		switch (dli.ntype) {
		case DLE_DIR:
			if (!ChangeDirectory(hwnd, dli.szFileName, 1)) {
				ErrorMessage(2, IDS_ERR_CD);
			}
			break;

		case DLE_FILE:
			BeginWaitCursor();

			if (!PathIsLnkFile(dli.szFileName)) {
				LaunchTarget(dli.szFileName, 0);
			} else {
				// PathIsLinkFile()
				WCHAR  tch[MAX_PATH];

				if (PathGetLnkPath(dli.szFileName, tch, COUNTOF(tch))) {
					DWORD dwAttr;
					ExpandEnvironmentStringsEx(tch, COUNTOF(tch));
					dwAttr = GetFileAttributes(tch);
					if ((dwAttr == (DWORD)(-1)) || (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
						DisplayLnkFile(dli.szFileName);
					} else {
						// Made sure link points to a file
						LaunchTarget(tch, 0);
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

	case IDM_FILE_OPENNEW: {
		DLITEM dli;
		dli.mask = DLI_ALL;

		DirList_GetItem(hwndDirList, -1, &dli);

		if (dli.ntype == DLE_FILE) {
			BeginWaitCursor();

			if (!PathIsLnkFile(dli.szFileName)) {
				LaunchTarget(dli.szFileName, 1);
			} else {
				// PathIsLinkFile()
				WCHAR  tch[MAX_PATH];

				if (PathGetLnkPath(dli.szFileName, tch, COUNTOF(tch))) {
					DWORD dwAttr;
					ExpandEnvironmentStringsEx(tch, COUNTOF(tch));
					dwAttr = GetFileAttributes(tch);
					if ((dwAttr == (DWORD)(-1)) || (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
						DisplayLnkFile(dli.szFileName);
					} else {
						// Made sure link points to a file
						LaunchTarget(tch, 1);
					}
				} else {
					DisplayLnkFile(dli.szFileName);
				}
			}

			EndWaitCursor();
		} else {
			MessageBeep(0);
		}
	}
	break;

	case IDM_FILE_RUN:
		RunDlg(hwnd);
		break;

	case IDM_FILE_LAUNCH: {
		DLITEM dli;
		SHELLEXECUTEINFO sei;

		if (!DirList_IsFileSelected(hwndDirList)) {
			MessageBeep(0);
			return 0;
		}

		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

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
		DLITEM dli;
		SHELLEXECUTEINFO sei;
		WCHAR szParam[MAX_PATH] = L"";
		WCHAR szTmp[MAX_PATH];

		if (!DirList_IsFileSelected(hwndDirList)) {
			MessageBeep(0);
			return 0;
		}

		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

		if (PathIsLnkFile(dli.szFileName) &&
				PathGetLnkPath(dli.szFileName, szTmp, COUNTOF(szTmp))) {
			GetShortPathName(szTmp, szTmp, COUNTOF(szTmp));
		} else {
			GetShortPathName(dli.szFileName, szTmp, COUNTOF(szTmp));
		}

		if (StrNotEmpty(szQuickviewParams)) {
			StrCatBuff(szParam, szQuickviewParams, COUNTOF(szParam));
			StrCatBuff(szParam, L" ", COUNTOF(szParam));
		}
		StrCatBuff(szParam, szTmp, COUNTOF(szParam));

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
		DLITEM dli;

		if (!ListView_GetSelectedCount(hwndDirList)) {
			MessageBeep(0);
			return 0;
		}

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
		OPENFILENAME ofn;
		HANDLE       hFile;
		WCHAR szNewFile[MAX_PATH];
		WCHAR szFilter[128];
		WCHAR szTitle[32];

		lstrcpy(szNewFile, L"");
		GetString(IDS_FILTER_ALL, szFilter, COUNTOF(szFilter));
		PrepareFilterStr(szFilter);
		GetString(IDS_NEWFILE, szTitle, COUNTOF(szTitle));

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

		hFile = CreateFile(szNewFile,
						   GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   NULL,
						   CREATE_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);

		if (hFile != INVALID_HANDLE_VALUE) {
			WCHAR szPath[MAX_PATH];
			SHFILEINFO shfi;

			CloseHandle(hFile);

			// Extract dir from filename
			lstrcpy(szPath, szNewFile);
			PathRemoveFileSpec(szPath);
			SetCurrentDirectory(szPath);

			// Select new file
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
			SHGetFileInfo(szNewFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, szNewFile)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}
		} else {
			ErrorMessage(2, IDS_ERR_NEW);
		}
	}
	break;

	case IDM_FILE_NEWDIR: {
		WCHAR tchNewDir[MAX_PATH];

		if (NewDirDlg(hwnd, tchNewDir)) {
			if (!CreateDirectory(tchNewDir, NULL)) {
				ErrorMessage(2, IDS_ERR_NEWDIR);
			}
		}
	}
	break;

	case IDM_FILE_CREATELINK: {
		DLITEM dli;
		WCHAR tchLinkDestination[MAX_PATH];

		dli.mask = DLI_FILENAME;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

		if (GetDirectory(hwnd, IDS_CREATELINK, tchLinkDestination, NULL)) {
			if (!PathCreateLnk(tchLinkDestination, dli.szFileName)) {
				ErrorMessage(2, IDS_ERR_CREATELINK);
			}
		}
	}
	break;

	case IDM_FILE_SAVEAS: {
		DLITEM dli;
		OPENFILENAME ofn;
		WCHAR szNewFile[MAX_PATH];
		WCHAR tch[MAX_PATH];
		WCHAR szFilter[128];
		BOOL bSuccess = FALSE;

		if (!DirList_IsFileSelected(hwndDirList)) {
			MessageBeep(0);
			return 0;
		}

		dli.mask = DLI_ALL;
		if (DirList_GetItem(hwndDirList, -1, &dli) == -1) {
			break;
		}

		lstrcpy(szNewFile, dli.szFileName);
		GetString(IDS_FILTER_ALL, szFilter, COUNTOF(szFilter));
		PrepareFilterStr(szFilter);

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

		FormatString(tch, COUNTOF(tch), IDS_SAVEFILE, dli.szDisplayName);
		StatusSetText(hwndStatus, ID_MENUHELP, tch);
		StatusSetSimple(hwndStatus, TRUE);
		InvalidateRect(hwndStatus, NULL, TRUE);
		UpdateWindow(hwndStatus);

		bSuccess = CopyFile(dli.szFileName, szNewFile, FALSE);

		if (!bSuccess) {
			ErrorMessage(2, IDS_ERR_SAVEAS1, dli.szDisplayName);
		}

		if (bSuccess && bClearReadOnly) {
			DWORD dwFileAttributes = GetFileAttributes(szNewFile);
			if (dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
				dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
				if (!SetFileAttributes(szNewFile, dwFileAttributes)) {
					ErrorMessage(2, IDS_ERR_SAVEAS2);
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
			MessageBeep(0);
		}
		break;

	case IDM_FILE_DELETE:
	case IDM_FILE_DELETE2:
	case IDM_FILE_DELETE3: {
		DLITEM dli;
		int    iItem;
		WCHAR   tch[512];
		SHFILEOPSTRUCT shfos;

		dli.mask = DLI_ALL;
		if ((iItem = DirList_GetItem(hwndDirList, -1, &dli)) == -1) {
			break;
		}

		ZeroMemory(&shfos, sizeof(SHFILEOPSTRUCT));
		ZeroMemory(tch, sizeof(tch));
		lstrcpy(tch, dli.szFileName);

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

			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
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
			MessageBeep(0);
		}
		break;

	case IDM_FILE_PROPERTIES:
		if (!ListView_GetSelectedCount(hwndDirList)) {
			MessageBeep(0);
		} else {
			DirList_PropertyDlg(hwndDirList, -1);
		}
		break;

	case IDM_FILE_CHANGEDIR: {
		WCHAR tch[MAX_PATH];

		if (GetDirectory(hwnd, IDS_GETDIRECTORY, tch, NULL)) {
			if (!ChangeDirectory(hwnd, tch, 1)) {
				ErrorMessage(2, IDS_ERR_CD);
			}
		}
	}
	break;

	case IDM_FILE_DRIVEPROP:
		DriveBox_PropertyDlg(hwndDriveBox);
		break;

	case IDM_VIEW_NEWWINDOW: {
		WCHAR szModuleName[MAX_PATH];
		WCHAR szParameters[1024];

		GetModuleFileName(NULL, szModuleName, COUNTOF(szModuleName));
		GetRelaunchParameters(szParameters);
		ShellExecute(hwnd, NULL, szModuleName, szParameters, NULL, SW_SHOWNORMAL);
	}
	break;

	case IDM_VIEW_FOLDERS:
		if (dwFillMask & DL_FOLDERS) {
			dwFillMask &= (~DL_FOLDERS);
		} else {
			dwFillMask |= DL_FOLDERS;
		}
		SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
		ListView_EnsureVisible(hwndDirList, 0, FALSE); // not done by update
		break;

	case IDM_VIEW_FILES:
		if (dwFillMask & DL_NONFOLDERS) {
			dwFillMask &= (~DL_NONFOLDERS);
		} else {
			dwFillMask |= DL_NONFOLDERS;
		}
		SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
		ListView_EnsureVisible(hwndDirList, 0, FALSE); // not done by update
		break;

	case IDM_VIEW_HIDDEN:
		if (dwFillMask & DL_INCLHIDDEN) {
			dwFillMask &= (~DL_INCLHIDDEN);
		} else {
			dwFillMask |= DL_INCLHIDDEN;
		}
		SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
		ListView_EnsureVisible(hwndDirList, 0, FALSE); // not done by update
		break;

	case IDM_VIEW_FILTER:
		if (GetFilterDlg(hwnd)) {
			// Store information about currently selected item
			DLITEM dli;
			dli.mask  = DLI_ALL;
			dli.ntype = DLE_NONE;
			DirList_GetItem(hwndDirList, -1, &dli);

			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);

			if (dli.ntype != DLE_NONE) {
				if (!DirList_SelectItem(hwndDirList, dli.szDisplayName, dli.szFileName)) {
					ListView_EnsureVisible(hwndDirList, 0, FALSE);
				}
			}
		}
		Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER,
							   (!StrEqual(tchFilter, L"*.*") || bNegFilter) ? TBFILTERBMP : TBFILTERBMP + 1);
		break;

	case IDM_VIEW_FILTERALL:
		if (!StrEqual(tchFilter, L"*.*") || bNegFilter) {
			DLITEM dli;

			lstrcpy(tchFilter, L"*.*");
			bNegFilter = FALSE;

			// Store information about currently selected item
			dli.mask  = DLI_ALL;
			dli.ntype = DLE_NONE;
			DirList_GetItem(hwndDirList, -1, &dli);

			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);

			if (dli.ntype != DLE_NONE) {
				if (!DirList_SelectItem(hwndDirList, dli.szDisplayName, dli.szFileName)) {
					ListView_EnsureVisible(hwndDirList, 0, FALSE);
				}
			}
		}
		Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, TBFILTERBMP + 1);
		break;

	case IDM_VIEW_UPDATE:
		ChangeDirectory(hwnd, NULL, 1);
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
		sei.lpVerb = NULL;
		sei.lpFile = tchFavoritesDir;
		sei.lpParameters = NULL;
		sei.lpDirectory = NULL;
		sei.nShow = SW_SHOWNORMAL;

		// Run favorites directory
		ShellExecuteEx(&sei);
	}
	break;

	case IDM_VIEW_TOOLBAR:
		if (bShowToolbar) {
			ShowWindow(hwndReBar, SW_HIDE);
			bShowToolbar = 0;
		} else {
			ShowWindow(hwndReBar, SW_SHOW);
			bShowToolbar = 1;
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_CUSTOMIZETB:
		SendMessage(hwndToolbar, TB_CUSTOMIZE, 0, 0);
		break;

	case IDM_VIEW_STATUSBAR:
		if (bShowStatusbar) {
			ShowWindow(hwndStatus, SW_HIDE);
			bShowStatusbar = 0;
		} else {
			ShowWindow(hwndStatus, SW_SHOW);
			bShowStatusbar = 1;
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_DRIVEBOX:
		if (bShowDriveBox) {
			ShowWindow(hwndDriveBox, SW_HIDE);
			bShowDriveBox = 0;
			if (GetDlgCtrlID(GetFocus()) == IDC_DRIVEBOX) {
				SetFocus(hwndDirList);
			}
		} else {
			ShowWindow(hwndDriveBox, SW_SHOW);
			bShowDriveBox = 1;
		}
		SendWMSize(hwnd);
		break;

	case IDM_VIEW_SAVESETTINGS: {
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
				SaveSettings(TRUE);
				EndWaitCursor();
				ErrorMessage(0, IDS_SAVESETTINGS);
			} else {
				ErrorMessage(2, IDS_ERR_INIWRITE);
			}
		} else {
			ErrorMessage(2, IDS_ERR_INICREATE);
		}
	}
	break;

	case IDM_VIEW_FINDTARGET:
		ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FINDTARGET),
							 hwnd, FindTargetDlgProc, (LPARAM)NULL);
		break;

	case IDM_VIEW_OPTIONS:
		OptionsPropSheet(hwnd, g_hInstance);
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

	case IDM_POP_COPYNAME: {
		DLITEM  dli;
		HGLOBAL hData;
		LPWSTR   pData;

		dli.mask = DLI_FILENAME;
		DirList_GetItem(hwndDirList, -1, &dli);

		hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
							sizeof(WCHAR) * (lstrlen(dli.szFileName) + 1));
		pData = GlobalLock(hData);
		lstrcpy(pData, dli.szFileName);
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
		if (SendMessage(hwndDriveBox, CB_GETDROPPEDSTATE, 0, 0)) {
			SendMessage(hwndDriveBox, CB_SHOWDROPDOWN, 0, 0);
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
		if (bFocusEdit) {
			bFocusEdit = 0;
		} else {
			bFocusEdit = 1;
		}
		break;

	case ACC_SWITCHTRANSPARENCY:
		bTransparentMode = bTransparentMode ? 0 : 1;
		SetWindowTransparentMode(hwnd, bTransparentMode);
		break;

	case ACC_GOTOTARGET: {
		DLITEM dli = { DLI_ALL, L"", L"", DLE_NONE };
		DirList_GetItem(hwndDirList, -1, &dli);

		if (dli.ntype == DLE_FILE) {
			if (PathIsLnkFile(dli.szFileName)) {
				WCHAR szFullPath[MAX_PATH];

				//SetFocus(hwndDirList);
				if (PathGetLnkPath(dli.szFileName, szFullPath, COUNTOF(szFullPath))) {
					if (PathFileExists(szFullPath)) {
						WCHAR szDir[MAX_PATH];
						WCHAR *p;
						lstrcpy(szDir, szFullPath);
						if ((p = StrRChr(szDir, NULL, L'\\')) != NULL) {
							*(p + 1) = 0;
							if (!PathIsRoot(szDir)) {
								*p = 0;
							}

							SetCurrentDirectory(szDir);
							SendMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
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
		ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FINDTARGET),
							 hwnd, FindTargetDlgProc, (LPARAM)NULL);
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
			CreateIniFile();
			DisplayPath(szIniFile, IDS_ERR_INIOPEN);
		}
		break;

	case IDT_HISTORY_BACK:
		if (History_CanBack(&mHistory)) {
			WCHAR tch[MAX_PATH];
			History_Back(&mHistory, tch, COUNTOF(tch));
			if (!ChangeDirectory(hwnd, tch, 0)) {
				ErrorMessage(2, IDS_ERR_CD);
			}
		} else {
			MessageBeep(0);
		}
		History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		break;

	case IDT_HISTORY_FORWARD:
		if (History_CanForward(&mHistory)) {
			WCHAR tch[MAX_PATH];
			History_Forward(&mHistory, tch, COUNTOF(tch));
			if (!ChangeDirectory(hwnd, tch, 0)) {
				ErrorMessage(2, IDS_ERR_CD);
			}
		} else {
			MessageBeep(0);
		}
		History_UpdateToolbar(&mHistory, hwndToolbar, IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		break;

	case IDT_UPDIR: {
		if (!PathIsRoot(szCurDir)) {
			if (!ChangeDirectory(hwnd, L"..", 1)) {
				ErrorMessage(2, IDS_ERR_CD);
			}
		} else {
			MessageBeep(0);
		}
	}
	break;

	case IDT_ROOT: {
		if (!PathIsRoot(szCurDir)) {
			if (!ChangeDirectory(hwnd, L"\\", 1)) {
				ErrorMessage(2, IDS_ERR_CD);
			}
		} else {
			MessageBeep(0);
		}
	}
	break;

	case IDT_VIEW_FAVORITES:
		SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_FAVORITES, 1), 0);
		break;

	case IDT_FILE_NEXT: {
		DLITEM dli;
		WCHAR   tch[MAX_PATH];

		int i, d;
		int iItem = ListView_GetNextItem(hwndDirList, -1, LVNI_ALL | LVNI_FOCUSED);

		dli.ntype = DLE_NONE;
		dli.mask = DLI_TYPE | DLI_FILENAME;

		d = ListView_GetSelectedCount(hwndDirList) ? 1 : 0;

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
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPEN, 1), 0);
		} else {
			MessageBeep(0);
		}
	}
	break;

	case IDT_FILE_PREV: {
		DLITEM dli;
		WCHAR   tch[MAX_PATH];

		int i, d;
		int iItem = ListView_GetNextItem(hwndDirList, -1, LVNI_ALL | LVNI_FOCUSED);

		dli.ntype = DLE_NONE;
		dli.mask = DLI_TYPE | DLI_FILENAME;

		d = (ListView_GetSelectedCount(hwndDirList) || iItem == 0) ? 1 : 0;

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
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPEN, 1), 0);
		} else {
			MessageBeep(0);
		}
	}
	break;

	case IDT_FILE_RUN:
		SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_RUN, 1), 0);
		break;

	case IDT_FILE_QUICKVIEW:
		if (DirList_IsFileSelected(hwndDirList)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_QUICKVIEW, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_SAVEAS:
		if (DirList_IsFileSelected(hwndDirList)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_SAVEAS, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_COPYMOVE:
		if (ListView_GetSelectedCount(hwndDirList)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_COPYMOVE, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_DELETE:
		if (ListView_GetSelectedCount(hwndDirList)) {
			BOOL fUseRecycleBin2 = fUseRecycleBin;
			fUseRecycleBin = 1;
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_DELETE, 1), 0);
			fUseRecycleBin = fUseRecycleBin2;
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_FILE_DELETE2:
		if (ListView_GetSelectedCount(hwndDirList)) {
			SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_DELETE2, 1), 0);
		} else {
			MessageBeep(0);
		}
		break;

	case IDT_VIEW_FILTER:
		SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_VIEW_FILTER, 1), 0);
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
			NM_LISTVIEW *pnmlv = (NM_LISTVIEW *)lParam;

			if ((pnmlv->uNewState & (LVIS_SELECTED | LVIS_FOCUSED)) !=
					(pnmlv->uOldState & (LVIS_SELECTED | LVIS_FOCUSED))) {

				WCHAR tch[64];
				if ((pnmlv->uNewState & LVIS_SELECTED)) {
					WIN32_FIND_DATA fd;
					DLITEM dli;
					FILETIME ft;
					SYSTEMTIME st;
					WCHAR tchsize[64], tchdate[64], tchtime[64], tchattr[64];
					LONGLONG isize;
					dli.mask  = DLI_FILENAME;
					dli.ntype = DLE_NONE;
					DirList_GetItem(hwndDirList, -1, &dli);
					DirList_GetItemEx(hwndDirList, -1, &fd);

					if (fd.nFileSizeLow >= MAXDWORD) {
						HANDLE hFile;
						if ((hFile = FindFirstFile(dli.szFileName, &fd)) != INVALID_HANDLE_VALUE) {
							FindClose(hFile);
						}
					}

					isize = (((LONGLONG)fd.nFileSizeHigh) << 32) + fd.nFileSizeLow;
					StrFormatByteSize((LONGLONG)isize, tchsize, COUNTOF(tchsize));

					FileTimeToLocalFileTime(&fd.ftLastWriteTime, &ft);
					FileTimeToSystemTime(&ft, &st);
					GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, tchdate, COUNTOF(tchdate));
					GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, tchtime, COUNTOF(tchdate));

					lstrcpy(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? L"A" : L"-");
					lstrcat(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? L"R" : L"-");
					lstrcat(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? L"H" : L"-");
					lstrcat(tchattr, (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? L"S" : L"-");

					wsprintf(tch, L"%s | %s %s | %s", tchsize, tchdate, tchtime, tchattr);
				} else {
					WCHAR tchnum[64];
					wsprintf(tchnum, L"%i", ListView_GetItemCount(hwndDirList));
					FormatNumberStr(tchnum);
					FormatString(tch, COUNTOF(tch),
								 (!StrEqual(tchFilter, L"*.*") || bNegFilter) ? IDS_NUMFILES2 : IDS_NUMFILES, tchnum);
				}

				StatusSetText(hwndStatus, ID_FILEINFO, tch);
			}
		}
		break;

		case NM_CLICK:
			if (bSingleClick && ListView_GetSelectedCount(hwndDirList)) {
				if (HIBYTE(GetKeyState(VK_MENU))) {
					SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_PROPERTIES, 1), 0);
				} else if (HIBYTE(GetKeyState(VK_SHIFT))) {
					SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPENNEW, 1), 0);
				} else {
					SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPEN, 1), 0);
				}
			}
			break;

		case NM_DBLCLK:
		case NM_RETURN:
			if (HIBYTE(GetKeyState(VK_MENU))) {
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_PROPERTIES, 1), 0);
			} else if (HIBYTE(GetKeyState(VK_SHIFT))) {
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPENNEW, 1), 0);
			} else {
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_OPEN, 1), 0);
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
			History_UpdateToolbar(&mHistory, hwndToolbar,
								  IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
			Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER,
								   (!StrEqual(tchFilter, L"*.*") || bNegFilter) ? TBFILTERBMP : TBFILTERBMP + 1);
			break;

		case TBN_QUERYDELETE:
		case TBN_QUERYINSERT:
			return TRUE;

		case TBN_GETBUTTONINFO: {
			if (((LPTBNOTIFY)lParam)->iItem < (int)COUNTOF(tbbMainWnd)) {
				WCHAR tch[256];
				GetString(tbbMainWnd[((LPTBNOTIFY)lParam)->iItem].idCommand, tch, COUNTOF(tch));
				lstrcpyn(((LPTBNOTIFY)lParam)->pszText, tch, ((LPTBNOTIFY)lParam)->cchText);
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
			if (Toolbar_SetButtons(hwndToolbar, IDT_HISTORY_BACK, L"1 2 3 4 5 0 8", tbbMainWnd, COUNTOF(tbbMainWnd)) == 0) {
				SendMessage(hwndToolbar, TB_ADDBUTTONS, NUMINITIALTOOLS, (LPARAM)tbbMainWnd);
			}
			return 0;
		}
		}
		break;

	default:
		switch (pnmh->code) {
		case TTN_NEEDTEXT: {
			if (((LPTOOLTIPTEXT)lParam)->uFlags & TTF_IDISHWND) {
				PathCompactPathEx(((LPTOOLTIPTEXT)lParam)->szText, szCurDir,
								  COUNTOF(((LPTOOLTIPTEXT)lParam)->szText), 0);
			} else {
				WCHAR tch[256];
				GetString((UINT)pnmh->idFrom, tch, COUNTOF(tch));
				lstrcpyn(((LPTOOLTIPTEXT)lParam)->szText, tch, 80);
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
	BOOL fUpdate = FALSE;

	if (lpszNewDir && !SetCurrentDirectory(lpszNewDir)) {
		return FALSE;
	}

	if (!lpszNewDir) {
		// Update call
		WCHAR szTest[MAX_PATH];

		GetCurrentDirectory(COUNTOF(szTest), szTest);
		if (!PathFileExists(szTest)) {
			WCHAR szWinDir[MAX_PATH];
			GetWindowsDirectory(szWinDir, COUNTOF(szWinDir));
			SetCurrentDirectory(szWinDir);
			ErrorMessage(2, IDS_ERR_CD);
		}

		fUpdate = TRUE;
	}

	BeginWaitCursor();
	{
		WCHAR tch[256], tchnum[64];
		int cItems;

		int iTopItem = ListView_GetTopIndex(hwndDirList);

		GetCurrentDirectory(COUNTOF(szCurDir), szCurDir);

		SetWindowPathTitle(hwnd, szCurDir);

		if (!StrEqual(tchFilter, L"*.*") || bNegFilter) {
			ListView_SetTextColor(hwndDirList, (bDefCrFilter) ? GetSysColor(COLOR_WINDOWTEXT) : crFilter);
			Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, TBFILTERBMP);
		} else {
			ListView_SetTextColor(hwndDirList, (bDefCrNoFilt) ? GetSysColor(COLOR_WINDOWTEXT) : crNoFilt);
			Toolbar_SetButtonImage(hwndToolbar, IDT_VIEW_FILTER, TBFILTERBMP + 1);
		}

		cItems = DirList_Fill(hwndDirList, szCurDir, dwFillMask, tchFilter, bNegFilter, flagNoFadeHidden, nSortFlags, fSortRev);
		DirList_StartIconThread(hwndDirList);

		// Get long pathname
		DirList_GetLongPathName(hwndDirList, szCurDir);
		SetCurrentDirectory(szCurDir);

		if (cItems > 0) {
			ListView_SetItemState(hwndDirList, 0, LVIS_FOCUSED, LVIS_FOCUSED);
		}

		// new directory -- scroll to 0,0
		if (!fUpdate) {
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
		} else {
			int iJump = min_i(iTopItem + ListView_GetCountPerPage(hwndDirList), cItems - 1);
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

		wsprintf(tchnum, L"%d", cItems);
		FormatNumberStr(tchnum);
		FormatString(tch, COUNTOF(tch),
					 (!StrEqual(tchFilter, L"*.*") || bNegFilter) ? IDS_NUMFILES2 : IDS_NUMFILES, tchnum);
		StatusSetText(hwndStatus, ID_FILEINFO, tch);

		// Update History
		if (bUpdateHistory) {
			History_Add(&mHistory, szCurDir);
			History_UpdateToolbar(&mHistory, hwndToolbar,
								  IDT_HISTORY_BACK, IDT_HISTORY_FORWARD);
		}
	}
	EndWaitCursor();

	return TRUE;
}

//=============================================================================
//
//  LoadSettings()
//
//
void LoadSettings(void) {
	IniSection section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	const int cbIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, 128);
	LoadIniSection(L"Settings", pIniSectionBuf, cbIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	bSaveSettings = IniSectionGetBool(pIniSection, L"SaveSettings", 1);
	bSingleClick = IniSectionGetBool(pIniSection, L"SingleClick", 1);
	bTrackSelect = IniSectionGetBool(pIniSection, L"TrackSelect", 1);
	bFullRowSelect = IniSectionGetBool(pIniSection, L"FullRowSelect", 0);
	fUseRecycleBin = IniSectionGetBool(pIniSection, L"UseRecycleBin", 0);
	fNoConfirmDelete = IniSectionGetBool(pIniSection, L"NoConfirmDelete", 0);
	bClearReadOnly = IniSectionGetBool(pIniSection, L"ClearReadOnly", 1);
	bRenameOnCollision = IniSectionGetBool(pIniSection, L"RenameOnCollision", 0);
	bFocusEdit = IniSectionGetBool(pIniSection, L"FocusEdit", 1);
	bAlwaysOnTop = IniSectionGetBool(pIniSection, L"AlwaysOnTop", 0);
	bMinimizeToTray = IniSectionGetBool(pIniSection, L"MinimizeToTray", 0);
	bTransparentMode = IniSectionGetBool(pIniSection, L"TransparentMode", 0);

	iEscFunction = IniSectionGetInt(pIniSection, L"EscFunction", 0);
	iEscFunction = clamp_i(iEscFunction, 0, 2);

	iStartupDir = IniSectionGetInt(pIniSection, L"StartupDirectory", 1);
	iStartupDir = clamp_i(iStartupDir, 0, 2);

	IniSectionGetString(pIniSection, L"MRUDirectory", L"", szMRUDirectory, COUNTOF(szMRUDirectory));

	if (!IniSectionGetString(pIniSection, L"Favorites", L"",
							 tchFavoritesDir, COUNTOF(tchFavoritesDir))) {
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, tchFavoritesDir);
	} else {
		PathAbsoluteFromApp(tchFavoritesDir, NULL, COUNTOF(tchFavoritesDir), TRUE);
	}

	if (!IniSectionGetString(pIniSection, L"Quikview.exe", L"",
							 szQuickview, COUNTOF(szQuickview))) {
		GetSystemDirectory(szQuickview, COUNTOF(szQuickview));
		PathAddBackslash(szQuickview);
		lstrcat(szQuickview, L"Viewers\\Quikview.exe");
	} else {
		PathAbsoluteFromApp(szQuickview, NULL, COUNTOF(szQuickview), TRUE);
	}

	IniSectionGetString(pIniSection, L"QuikviewParams", L"",
						szQuickviewParams, COUNTOF(szQuickviewParams));

	if (!IniSectionGetString(pIniSection, L"OpenWithDir", L"",
							 tchOpenWithDir, COUNTOF(tchOpenWithDir))) {
		SHGetSpecialFolderPath(NULL, tchOpenWithDir, CSIDL_DESKTOPDIRECTORY, TRUE);
	} else {
		PathAbsoluteFromApp(tchOpenWithDir, NULL, COUNTOF(tchOpenWithDir), TRUE);
	}

	dwFillMask = IniSectionGetInt(pIniSection, L"FillMask", DL_ALLOBJECTS);
	if (dwFillMask & ~DL_ALLOBJECTS) {
		dwFillMask = DL_ALLOBJECTS;
	}

	nSortFlags = IniSectionGetInt(pIniSection, L"SortOptions", DS_NAME);
	nSortFlags = clamp_i(nSortFlags, 0, 3);

	fSortRev = IniSectionGetBool(pIniSection, L"SortReverse", 0);

	if (!lpFilterArg) {
		if (!IniSectionGetString(pIniSection, L"FileFilter", L"",
								 tchFilter, COUNTOF(tchFilter))) {
			lstrcpy(tchFilter, L"*.*");
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

	bDefCrNoFilt = IniSectionGetBool(pIniSection, L"DefColorNoFilter", 1);
	bDefCrFilter = IniSectionGetBool(pIniSection, L"DefColorFilter", 1);

	crNoFilt = IniSectionGetInt(pIniSection, L"ColorNoFilter", GetSysColor(COLOR_WINDOWTEXT));
	crFilter = IniSectionGetInt(pIniSection, L"ColorFilter", GetSysColor(COLOR_HIGHLIGHT));

	if (!IniSectionGetString(pIniSection, L"ToolbarButtons", L"",
							tchToolbarButtons, COUNTOF(tchToolbarButtons))) {
		lstrcpy(tchToolbarButtons, DefaultToolbarButtons);
	}

	bShowToolbar = IniSectionGetBool(pIniSection, L"ShowToolbar", 1);
	bShowStatusbar = IniSectionGetBool(pIniSection, L"ShowStatusbar", 1);
	bShowDriveBox = IniSectionGetBool(pIniSection, L"ShowDriveBox", 1);

	cxGotoDlg = IniSectionGetInt(pIniSection, L"GotoDlgSizeX", 0);
	cxGotoDlg = max_i(cxGotoDlg, 0);

	cxOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeX", 0);
	cxOpenWithDlg = max_i(cxOpenWithDlg, 0);

	cyOpenWithDlg = IniSectionGetInt(pIniSection, L"OpenWithDlgSizeY", 0);
	cyOpenWithDlg = max_i(cyOpenWithDlg, 0);

	cxCopyMoveDlg = IniSectionGetInt(pIniSection, L"CopyMoveDlgSizeX", 0);
	cxCopyMoveDlg = max_i(cxCopyMoveDlg, 0);

	if (!flagPosParam) {
		// ignore window position if /p was specified
		WCHAR tchPosX[32], tchPosY[32], tchSizeX[32], tchSizeY[32];
		int ResX = GetSystemMetrics(SM_CXSCREEN);
		int ResY = GetSystemMetrics(SM_CYSCREEN);

		wsprintf(tchPosX, L"%ix%i PosX", ResX, ResY);
		wsprintf(tchPosY, L"%ix%i PosY", ResX, ResY);
		wsprintf(tchSizeX, L"%ix%i SizeX", ResX, ResY);
		wsprintf(tchSizeY, L"%ix%i SizeY", ResX, ResY);

		LoadIniSection(L"Window", pIniSectionBuf, cbIniSection);
		IniSectionParse(pIniSection, pIniSectionBuf);

		wi.x = IniSectionGetIntEx(pIniSection, tchPosX, CW_USEDEFAULT);
		wi.y = IniSectionGetIntEx(pIniSection, tchPosY, CW_USEDEFAULT);
		wi.cx = IniSectionGetIntEx(pIniSection, tchSizeX, CW_USEDEFAULT);
		wi.cy = IniSectionGetIntEx(pIniSection, tchSizeY, CW_USEDEFAULT);
	}

	LoadIniSection(L"Toolbar Images", pIniSectionBuf, cbIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	IniSectionGetString(pIniSection, L"BitmapDefault", L"",
						tchToolbarBitmap, COUNTOF(tchToolbarBitmap));
	IniSectionGetString(pIniSection, L"BitmapHot", L"",
						tchToolbarBitmapHot, COUNTOF(tchToolbarBitmap));
	IniSectionGetString(pIniSection, L"BitmapDisabled", L"",
						tchToolbarBitmapDisabled, COUNTOF(tchToolbarBitmap));

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);

	// Initialize custom colors for ChooseColor()
	crCustom[0] = RGB(0, 0, 128);
	crCustom[8]  = RGB(255, 255, 226);
	crCustom[1] = GetSysColor(COLOR_WINDOWTEXT);
	crCustom[9]  = GetSysColor(COLOR_WINDOW);
	crCustom[2] = GetSysColor(COLOR_INFOTEXT);
	crCustom[10] = GetSysColor(COLOR_INFOBK);
	crCustom[3] = GetSysColor(COLOR_HIGHLIGHTTEXT);
	crCustom[11] = GetSysColor(COLOR_HIGHLIGHT);
	crCustom[4] = GetSysColor(COLOR_ACTIVECAPTION);
	crCustom[12] = GetSysColor(COLOR_DESKTOP);
	crCustom[5] = GetSysColor(COLOR_3DFACE);
	crCustom[13] = GetSysColor(COLOR_3DFACE);
	crCustom[6] = GetSysColor(COLOR_3DFACE);
	crCustom[14] = GetSysColor(COLOR_3DFACE);
	crCustom[7] = GetSysColor(COLOR_3DFACE);
	crCustom[15] = GetSysColor(COLOR_3DFACE);
}

//=============================================================================
//
//  SaveSettings()
//
//
void SaveSettings(BOOL bSaveSettingsNow) {
	if (StrIsEmpty(szIniFile)) {
		return;
	}

	CreateIniFile();

	if (!bSaveSettings && !bSaveSettingsNow) {
		if (iStartupDir == 1) {
			IniSetString(L"Settings", L"MRUDirectory", szCurDir);
		}
		IniSetBool(L"Settings", L"SaveSettings", bSaveSettings);
		return;
	}

	WCHAR wchTmp[MAX_PATH];
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	IniSectionSetBool(pIniSection, L"SaveSettings", bSaveSettings);
	IniSectionSetBoolEx(pIniSection, L"SingleClick", bSingleClick, 1);
	IniSectionSetBoolEx(pIniSection, L"TrackSelect", bTrackSelect, 1);
	IniSectionSetBoolEx(pIniSection, L"FullRowSelect", bFullRowSelect, 0);
	IniSectionSetBoolEx(pIniSection, L"UseRecycleBin", fUseRecycleBin, 0);
	IniSectionSetBoolEx(pIniSection, L"NoConfirmDelete", fNoConfirmDelete, 0);
	IniSectionSetBoolEx(pIniSection, L"ClearReadOnly", bClearReadOnly, 1);
	IniSectionSetBoolEx(pIniSection, L"RenameOnCollision", bRenameOnCollision, 0);
	IniSectionSetBoolEx(pIniSection, L"FocusEdit", bFocusEdit, 1);
	IniSectionSetBoolEx(pIniSection, L"AlwaysOnTop", bAlwaysOnTop, 0);
	IniSectionSetBoolEx(pIniSection, L"MinimizeToTray", bMinimizeToTray, 0);
	IniSectionSetBoolEx(pIniSection, L"TransparentMode", bTransparentMode, 0);
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
	IniSectionSetBoolEx(pIniSection, L"DefColorNoFilter", bDefCrNoFilt, 1);
	IniSectionSetBoolEx(pIniSection, L"DefColorFilter", bDefCrFilter, 1);
	IniSectionSetIntEx(pIniSection, L"ColorNoFilter", crNoFilt, GetSysColor(COLOR_WINDOWTEXT));
	IniSectionSetIntEx(pIniSection, L"ColorFilter", crFilter, GetSysColor(COLOR_HIGHLIGHT));
	Toolbar_GetButtons(hwndToolbar, IDT_HISTORY_BACK, tchToolbarButtons, COUNTOF(tchToolbarButtons));
	IniSectionSetStringEx(pIniSection, L"ToolbarButtons", tchToolbarButtons, DefaultToolbarButtons);
	IniSectionSetBoolEx(pIniSection, L"ShowToolbar", bShowToolbar, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowStatusbar", bShowStatusbar, 1);
	IniSectionSetBoolEx(pIniSection, L"ShowDriveBox", bShowDriveBox, 1);
	IniSectionSetInt(pIniSection, L"GotoDlgSizeX", cxGotoDlg);
	IniSectionSetInt(pIniSection, L"OpenWithDlgSizeX", cxOpenWithDlg);
	IniSectionSetInt(pIniSection, L"OpenWithDlgSizeY", cyOpenWithDlg);
	IniSectionSetInt(pIniSection, L"CopyMoveDlgSizeX", cxCopyMoveDlg);

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
	}

	{
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
	}
}

//=============================================================================
//
//  ParseCommandLine()
//
//
int ParseCommandLineOption(LPWSTR lp1, LPWSTR lp2) {
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

				lpFilterArg = NP2HeapAlloc(sizeof(WCHAR) * (lstrlen(lp1) + 1));
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
				int x = 0, y = 0, cx = 0, cy = 0;
				int itok = swscanf(lp1, L"%i,%i,%i,%i", &x, &y, &cx, &cy);
				if (itok == 4) { // scan successful
					flagPosParam = 1;
					state = 1;
					wi.x = x;
					wi.y = y;
					wi.cx = cx;
					wi.cy = cy;
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
	} else if (opt[2] == 0) {
		switch (*CharUpper(opt)) {
		case L'F':
			if (opt[1] == L'0' || *CharUpper(opt + 1) == L'O') {
				lstrcpy(szIniFile, L"*?");
				state = 1;
			}
			break;

		case L'P':
			switch (*CharUpper(opt + 1)) {
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
	LPWSTR lp1, lp2, lp3;
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
		if ((*lp1 == L'/' || *lp1 == L'-') && lp1[1] != 0) {
			int state = ParseCommandLineOption(lp1, lp2);
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

			lpPathArg = GlobalAlloc(GPTR, sizeof(WCHAR) * (MAX_PATH + 2));
			StrCpyN(lpPathArg, lp3, MAX_PATH);
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
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	const int cchIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, 16);
	LoadIniSection(L"Settings2", pIniSectionBuf, cchIniSection);
	IniSectionParse(pIniSection, pIniSectionBuf);

	bReuseWindow = IniSectionGetBool(pIniSection, L"ReuseWindow", 0);
	if (!flagNoReuseWindow) {
		flagNoReuseWindow = !bReuseWindow;
	}

	flagPortableMyDocs = IniSectionGetBool(pIniSection, L"PortableMyDocs", 1);

	iAutoRefreshRate = IniSectionGetInt(pIniSection, L"AutoRefreshRate", 30);
	iAutoRefreshRate = max_i(iAutoRefreshRate, 0);

	flagNoFadeHidden = IniSectionGetBool(pIniSection, L"NoFadeHidden", 0);

	iOpacityLevel = IniSectionGetInt(pIniSection, L"OpacityLevel", 75);
	if (iOpacityLevel < 0 || iOpacityLevel > 100) {
		iOpacityLevel = 75;
	}

	flagToolbarLook = IniSectionGetInt(pIniSection, L"ToolbarLook", IsWinXPAndAbove() ? 1 : 2);
	flagToolbarLook = clamp_i(flagToolbarLook, 0, 2);

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
}

//=============================================================================
//
//  FindIniFile()
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
	if (GetPrivateProfileString(L"metapath", L"metapath.ini", L"", tch, COUNTOF(tch), lpszFile)) {
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
			lstrcpy(PathFindFileName(szIniFile), L"metapath.ini");
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
		if (hFile != INVALID_HANDLE_VALUE) {
			if (GetFileSize(hFile, NULL) == 0) {
				DWORD dw;
				WriteFile(hFile, (LPCVOID)L"\xFEFF[metapath]\r\n", 26, &dw, NULL);
			}
			CloseHandle(hFile);
			return 1;
		}
	}

	return 0;
}

//=============================================================================
//
//  DisplayPath()
//
//
BOOL DisplayPath(LPCWSTR lpPath, UINT uIdError) {
	DWORD dwAttr;
	WCHAR  szPath[MAX_PATH];
	WCHAR  szTmp[MAX_PATH];

	if (StrIsEmpty(lpPath)) {
		return FALSE;
	}

	lstrcpy(szTmp, lpPath);
	ExpandEnvironmentStringsEx(szTmp, COUNTOF(szTmp));

	if (!SearchPathEx(NULL, szTmp, NULL, COUNTOF(szPath), szPath, NULL)) {
		lstrcpy(szPath, szTmp);
	}

	if (PathIsLnkFile(szPath)) {
		return DisplayLnkFile(szPath);
	}

	dwAttr = GetFileAttributes(szPath);

	if (dwAttr != INVALID_FILE_ATTRIBUTES) {
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
			if (!SetCurrentDirectory(szPath)) {
				ErrorMessage(2, uIdError);
				return FALSE;
			}
			PostMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
			return TRUE;
		}
		{ // !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
			WCHAR *p;
			SHFILEINFO shfi;

			// szPath will be modified...
			lstrcpy(szTmp, szPath);

			SHGetFileInfo(szPath, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);

			if ((p = StrRChr(szPath, NULL, L'\\')) != NULL) {
				*(p + 1) = 0;
				if (!PathIsRoot(szPath)) {
					*p = 0;
				}
				SetCurrentDirectory(szPath);
			}

			SendMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);

			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, szTmp)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}

			return TRUE;
		}
	}

	// dwAttr != (DWORD)(-1)
	ErrorMessage(2, uIdError);
	return FALSE;
}

//=============================================================================
//
//  DisplayLnkFile()
//
//
BOOL DisplayLnkFile(LPCWSTR pszLnkFile) {
	DWORD dwAttr;
	WCHAR  szPath[MAX_PATH];
	WCHAR  szTmp[MAX_PATH];

	if (!PathGetLnkPath(pszLnkFile, szTmp, COUNTOF(szTmp))) {
		// Select lnk-file if target is not available
		if (PathFileExists(pszLnkFile)) {
			SHFILEINFO shfi;

			lstrcpy(szTmp, pszLnkFile);
			PathRemoveFileSpec(szTmp);
			SetCurrentDirectory(szTmp);

			// Select new file
			SendMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
			SHGetFileInfo(pszLnkFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, pszLnkFile)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}
		}

		ErrorMessage(2, IDS_ERR_LNK_GETPATH);
		return FALSE;
	}

	ExpandEnvironmentStringsEx(szTmp, COUNTOF(szTmp));

	if (!SearchPathEx(NULL, szTmp, NULL, COUNTOF(szPath), szPath, NULL)) {
		lstrcpy(szPath, szTmp);
	}

	dwAttr = GetFileAttributes(szPath);

	if (dwAttr != INVALID_FILE_ATTRIBUTES) {
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
			if (!SetCurrentDirectory(szPath)) {
				ErrorMessage(2, IDS_ERR_LNK_NOACCESS);
				return FALSE;
			}
			PostMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
			ListView_EnsureVisible(hwndDirList, 0, FALSE);
			return TRUE;
		}
		{ // !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
			WCHAR *p;
			int  i;
			SHFILEINFO  shfi;
			LV_FINDINFO lvfi;

			// Current file is ShellLink, get dir and desc
			lstrcpy(szPath, pszLnkFile);

			SHGetFileInfo(szPath, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);

			if ((p = StrRChr(szPath, NULL, L'\\')) != NULL) {
				*(p + 1) = 0;
				if (!PathIsRoot(szPath)) {
					*p = 0;
				}
				SetCurrentDirectory(szPath);
			}

			lvfi.flags = LVFI_STRING;
			lvfi.psz   = shfi.szDisplayName;

			SendMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
			i = ListView_FindItem(hwndDirList, -1, &lvfi);

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
	}

	// GetFileAttributes() failed
	{
		// Select lnk-file if target is not available
		if (PathFileExists(pszLnkFile)) {
			SHFILEINFO shfi;

			lstrcpy(szTmp, pszLnkFile);
			PathRemoveFileSpec(szTmp);
			SetCurrentDirectory(szTmp);

			// Select new file
			SendMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_VIEW_UPDATE, 1), 0);
			SHGetFileInfo(pszLnkFile, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
			if (!DirList_SelectItem(hwndDirList, shfi.szDisplayName, pszLnkFile)) {
				ListView_EnsureVisible(hwndDirList, 0, FALSE);
			}
		}

		ErrorMessage(2, IDS_ERR_LNK_NOACCESS);
		return FALSE;
	}
}

/******************************************************************************
*
* ActivatePrevInst()
*
* Tries to find and activate an already open metapath Window
*
*
******************************************************************************/
BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam) {
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
	HWND hwnd = NULL;
	COPYDATASTRUCT cds;

	if (flagNoReuseWindow || flagStartAsTrayIcon) {
		return FALSE;
	}

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
				// Search working directory from second instance, first!
				// lpPathArg is at least MAX_PATH+2 bytes
				WCHAR tchTmp[MAX_PATH];

				ExpandEnvironmentStringsEx(lpPathArg, (DWORD)GlobalSize(lpPathArg) / sizeof(WCHAR));

				if (PathIsRelative(lpPathArg)) {
					GetCurrentDirectory(COUNTOF(tchTmp), tchTmp);
					PathAppend(tchTmp, lpPathArg);
					if (PathFileExists(tchTmp)) {
						lstrcpy(lpPathArg, tchTmp);
					} else {
						if (SearchPath(NULL, lpPathArg, NULL, COUNTOF(tchTmp), tchTmp, NULL)) {
							lstrcpy(lpPathArg, tchTmp);
						}
					}
				} else if (SearchPath(NULL, lpPathArg, NULL, COUNTOF(tchTmp), tchTmp, NULL)) {
					lstrcpy(lpPathArg, tchTmp);
				}

				cds.dwData = DATA_METAPATH_PATHARG;
				cds.cbData = (DWORD)GlobalSize(lpPathArg);
				cds.lpData = lpPathArg;

				// Send lpPathArg to previous instance
				SendMessage(hwnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);

				GlobalFree(lpPathArg);
			}
			return TRUE;
		}

		{ // !IsWindowEnabled()
			WCHAR szBuf[256];
			WCHAR *c;

			// Prepare message
			GetString(IDS_ERR_PREVWINDISABLED, szBuf, COUNTOF(szBuf));
			c = StrChr(szBuf, L'\n');
			if (c) {
				*c = 0;
				c++;
			}

			// Ask...
			if (MessageBox(NULL, c, szBuf, MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND) == IDYES) {
				return FALSE;
			}
			return TRUE;

		}
	}

	return FALSE;
}

void GetRelaunchParameters(LPWSTR szParameters) {
	MONITORINFO mi;
	HMONITOR hMonitor;
	WINDOWPLACEMENT wndpl;
	int x, y, cx, cy;
	WCHAR tch[64];
	WCHAR szDirName[MAX_PATH + 4];

	lstrcpy(szParameters, L" -f");
	if (StrNotEmpty(szIniFile)) {
		lstrcat(szParameters, L" \"");
		lstrcat(szParameters, szIniFile);
		lstrcat(szParameters, L"\"");
	} else {
		lstrcat(szParameters, L"0");
	}

	lstrcat(szParameters, L" -n");

	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwndMain, &wndpl);

	hMonitor = MonitorFromRect(&wndpl.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	// offset new window position +10/+10
	x = wndpl.rcNormalPosition.left + 10;
	y = wndpl.rcNormalPosition.top  + 10;
	cx = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
	cy = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

	// check if window fits monitor
	if ((x + cx) > mi.rcWork.right || (y + cy) > mi.rcWork.bottom) {
		x = mi.rcMonitor.left;
		y = mi.rcMonitor.top;
	}

	wsprintf(tch, L" -p %i,%i,%i,%i", x, y, cx, cy);
	lstrcat(szParameters, tch);

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
	NOTIFYICONDATA nid;

	if (!hIcon) {
		hIcon = LoadImage(g_hInstance, MAKEINTRESOURCE(IDR_MAINWND),
						  IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	}

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

BOOL CALLBACK EnumWndProc2(HWND hwnd, LPARAM lParam) {
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
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * 32 * 1024);
	const int cbIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;

	IniSectionInit(pIniSection, 16);
	LoadIniSection(L"Target Application", pIniSectionBuf, cbIniSection);
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
	HWND  hwnd  = NULL;
	HDROP hDrop = NULL;

	if (!bLoadLaunchSetingsLoaded) {
		LoadLaunchSetings();
	}
	if (iUseTargetApplication == 4 ||
			(iUseTargetApplication && StrIsEmpty(szTargetApplication))) {
		ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FINDTARGET),
							 hwndMain, FindTargetDlgProc, (LPARAM)NULL);
		return;
	}

	if (iUseTargetApplication && iTargetApplicationMode == 1) {
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
				hDrop = CreateDropHandle(lpFileName);
				PostMessage(hwnd, WM_DROPFILES, (WPARAM)hDrop, (LPARAM)0);
			}
		} else { // Either no window or disabled - run target.exe
			SHELLEXECUTEINFO sei;
			WCHAR  szFile[MAX_PATH];
			LPWSTR lpParam;
			WCHAR  szParam[MAX_PATH] = L"";
			WCHAR  szTmp[MAX_PATH];

			if (hwnd) { // disabled window
				WCHAR szBuf[256];
				WCHAR *c;

				// Prepare message
				GetString(IDS_ERR_TARGETDISABLED, szBuf, COUNTOF(szBuf));
				c = StrChr(szBuf, L'\n');
				if (c) {
					*c = 0;
					c++;
				}

				// Ask...
				if (MessageBox(hwndMain, c, szBuf,
							   MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND) == IDNO) {
					return;
				}
			}

			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

			if (PathIsLnkFile(lpFileName) && PathGetLnkPath(lpFileName, szTmp, COUNTOF(szTmp))) {
				lpParam = szTmp;
			} else {
				lpParam = (LPWSTR)lpFileName;
			}

			//if (Is32bitExe(szTargetApplication))
			//  PathQuoteSpaces(lpParam);
			//else
			GetShortPathName(lpParam, lpParam, MAX_PATH);

			if (StrNotEmpty(szTargetApplicationParams)) {
				StrCpyN(szParam, szTargetApplicationParams, COUNTOF(szParam));
				StrCatBuff(szParam, L" ", COUNTOF(szParam));
			}
			StrCatBuff(szParam, lpParam, COUNTOF(szParam));

			lstrcpy(szTmp, szTargetApplication);
			PathAbsoluteFromApp(szTmp, szFile, COUNTOF(szFile), TRUE);
			if (!PathFileExists(szFile)) {
				if (!SearchPath(NULL, szTmp, NULL, COUNTOF(szFile), szFile, NULL)) {
					GetModuleFileName(NULL, szFile, COUNTOF(szFile));
					PathRemoveFileSpec(szFile);
					PathAppend(szFile, szTmp);
				}
			}

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
		SHELLEXECUTEINFO sei;
		WCHAR  szFile[MAX_PATH];
		LPWSTR lpParam;
		WCHAR  szParam[MAX_PATH] = L"";
		WCHAR  szTmp[MAX_PATH];

		if (iUseTargetApplication &&
				iTargetApplicationMode == 2 &&
				ExecDDECommand(lpFileName, szDDEMsg, szDDEApp, szDDETopic)) {
			return;
		}

		if (!iUseTargetApplication && StrIsEmpty(lpFileName)) {
			return;
		}
		{

			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

			if (PathIsLnkFile(lpFileName) &&
					PathGetLnkPath(lpFileName, szTmp, COUNTOF(szTmp))) {
				lpParam = szTmp;
			} else {
				lpParam = (LPWSTR)lpFileName;
			}

			//if (Is32bitExe(szTargetApplication))
			//  PathQuoteSpaces(lpParam);
			//else
			GetShortPathName(lpParam, lpParam, MAX_PATH);

			if (StrNotEmpty(szTargetApplicationParams)) {
				StrCpyN(szParam, szTargetApplicationParams, COUNTOF(szParam));
				StrCatBuff(szParam, L" ", COUNTOF(szParam));
			}
			StrCatBuff(szParam, lpParam, COUNTOF(szParam));

			lstrcpy(szTmp, szTargetApplication);
			ExpandEnvironmentStringsEx(szTmp, COUNTOF(szTmp));

			if (PathIsRelative(szTmp)) {
				PathAbsoluteFromApp(szTmp, szFile, COUNTOF(szFile), TRUE);
				if (!PathFileExists(szFile)) {
					if (!SearchPath(NULL, szTmp, NULL, COUNTOF(szFile), szFile, NULL)) {
						GetModuleFileName(NULL, szFile, COUNTOF(szFile));
						PathRemoveFileSpec(szFile);
						PathAppend(szFile, szTmp);
					}
				}
			} else {
				lstrcpy(szFile, szTmp);
			}

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
}

//=============================================================================
//
//  SnapToTarget()
//
//  Aligns metapath to either side of target window
//
//
void SnapToTarget(HWND hwnd) {
	RECT rcOld, rcNew, rc2;
	HWND hwnd2;

	if (!bLoadLaunchSetingsLoaded) {
		LoadLaunchSetings();
	}

	hwnd2 = NULL;
	EnumWindows(EnumWndProc2, (LPARAM)&hwnd2);

	// Found a window
	if (hwnd2 != NULL) {
		int  cxScreen;
		if (IsIconic(hwnd2) || IsZoomed(hwnd2)) {
			SendMessage(hwnd2, WM_SYSCOMMAND, SC_RESTORE, 0);
		}

		SetForegroundWindow(hwnd2);
		BringWindowToTop(hwnd2);

		SetForegroundWindow(hwnd);

		cxScreen = GetSystemMetrics(SM_CXSCREEN);
		GetWindowRect(hwnd, &rcOld);
		GetWindowRect(hwnd2, &rc2);

		if (rc2.left > cxScreen - rc2.right) {
			rcNew.left = rc2.left - (rcOld.right - rcOld.left);
		} else {
			rcNew.left = rc2.right;
		}

		rcNew.top  = rc2.top;
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
	WINDOWPLACEMENT wndpl;
	HMONITOR hMonitor;
	MONITORINFO mi;
	int x, y, cx, cy;
	RECT rcOld;

	GetWindowRect(hwnd, &rcOld);

	hMonitor = MonitorFromRect(&rcOld, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	x = mi.rcWork.left + 16;
	y = mi.rcWork.top + 16;
	cx = 272;
	cy = 640;

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

///  End of metapath.c
