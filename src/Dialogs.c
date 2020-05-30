/******************************************************************************
*
*
* Notepad2
*
* Dialogs.c
*   Notepad2 dialog boxes implementation
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
#include "config.h"
#include "SciCall.h"
#include "Helpers.h"
#include "Notepad2.h"
#include "Edit.h"
#include "Dlapi.h"
#include "Dialogs.h"
#include "resource.h"
#include "Version.h"

extern HWND		hwndMain;
extern DWORD	dwLastIOError;
extern BOOL		bSkipUnicodeDetection;
extern BOOL		bLoadANSIasUTF8;
extern BOOL		bLoadASCIIasUTF8;
extern BOOL		bLoadNFOasOEM;
extern int		fNoFileVariables;
extern BOOL		bNoEncodingTags;
extern BOOL		bWarnLineEndings;
extern BOOL		bFixLineEndings;
extern BOOL		bAutoStripBlanks;
extern WCHAR	szCurFile[MAX_PATH + 40];
#if NP2_ENABLE_APP_LOCALIZATION_DLL
extern LANGID uiLanguage;
#endif

//=============================================================================
//
// MsgBox()
//
int MsgBox(UINT uType, UINT uIdMsg, ...) {
	WCHAR szBuf[1024];
	WCHAR szText[1024];

	GetString(uIdMsg, szBuf, COUNTOF(szBuf));

	va_list va;
	va_start(va, uIdMsg);
	wvsprintf(szText, szBuf, va);
	va_end(va);

#if NP2_ENABLE_APP_LOCALIZATION_DLL
	const LANGID lang = uiLanguage;
#else
	const LANGID lang = LANG_USER_DEFAULT;
#endif

	if (uType & MB_SERVICE_NOTIFICATION) {
		uType &= ~MB_SERVICE_NOTIFICATION;
		LPWSTR lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwLastIOError,
			lang,
			(LPWSTR)(&lpMsgBuf),
			0,
			NULL);
		StrTrim(lpMsgBuf, L" \a\b\f\n\r\t\v");
		StrCatBuff(szText, L"\n", COUNTOF(szText));
		StrCatBuff(szText, lpMsgBuf, COUNTOF(szText));
		LocalFree(lpMsgBuf);
		const WCHAR wcht = *CharPrev(szText, StrEnd(szText));
		if (IsCharAlphaNumeric(wcht) || wcht == L'"' || wcht == L'\'') {
			StrCatBuff(szText, L".", COUNTOF(szText));
		}
	}

	WCHAR szTitle[128];
	GetString(IDS_APPTITLE, szTitle, COUNTOF(szTitle));

	HWND hwnd;
	if ((hwnd = GetActiveWindow()) == NULL) {
		hwnd = hwndMain;
	}

	uType |= MB_SETFOREGROUND;
	if (bWindowLayoutRTL) {
		uType |= MB_RTLREADING;
	}

	PostMessage(hwndMain, APPM_CENTER_MESSAGE_BOX, (WPARAM)hwnd, 0);
	return MessageBoxEx(hwnd, szText, szTitle, uType, lang);
}

//=============================================================================
//
// DisplayCmdLineHelp()
//
void DisplayCmdLineHelp(HWND hwnd) {
	WCHAR szTitle[32];
	WCHAR szText[2048];

	GetString(IDS_APPTITLE, szTitle, COUNTOF(szTitle));
	GetString(IDS_CMDLINEHELP, szText, COUNTOF(szText));

	MSGBOXPARAMS mbp;
	mbp.cbSize = sizeof(MSGBOXPARAMS);
	mbp.hwndOwner = hwnd;
	mbp.hInstance = g_hInstance;
	mbp.lpszText = szText;
	mbp.lpszCaption = szTitle;
	mbp.dwStyle = MB_OK | MB_USERICON | MB_SETFOREGROUND;
	mbp.lpszIcon = MAKEINTRESOURCE(IDR_MAINWND);
	mbp.dwContextHelpId = 0;
	mbp.lpfnMsgBoxCallback = NULL;
#if NP2_ENABLE_APP_LOCALIZATION_DLL
	mbp.dwLanguageId = uiLanguage;
#else
	mbp.dwLanguageId = LANG_USER_DEFAULT;
#endif
	if (bWindowLayoutRTL) {
		mbp.dwStyle |= MB_RTLREADING;
	}

	if (hwnd != NULL) {
		PostMessage(hwndMain, APPM_CENTER_MESSAGE_BOX, (WPARAM)hwnd, 0);
	}
	MessageBoxIndirect(&mbp);
}

void OpenHelpLink(HWND hwnd, int cmd) {
	LPCWSTR link = NULL;
	switch (cmd) {
	case IDC_WEBPAGE_LINK:
		link = L"http://www.flos-freeware.ch";
		break;
	case IDC_EMAIL_LINK:
		link = L"mailto:florian.balmer@gmail.com";
		break;
	case IDC_MOD_PAGE_LINK:
		link = VERSION_MODPAGE_DISPLAY;
		break;
	case IDC_SCI_PAGE_LINK:
		link = VERSION_SCIPAGE_DISPLAY;
		break;
	case IDC_NEW_PAGE_LINK:
	case IDM_HELP_PROJECT_HOME:
		link = VERSION_NEWPAGE_DISPLAY;
		break;
	case IDM_HELP_LATEST_RELEASE:
		link = HELP_LINK_LATEST_RELEASE;
		break;
	case IDM_HELP_LATEST_BUILD:
		link = HELP_LINK_LATEST_BUILD;
		break;
	case IDM_HELP_REPORT_ISSUE:
		link = HELP_LINK_REPORT_ISSUE;
		break;
	case IDM_HELP_FEATURE_REQUEST:
		link = HELP_LINK_FEATURE_REQUEST;
		break;
	case IDM_HELP_ONLINE_WIKI:
		link = HELP_LINK_ONLINE_WIKI;
		break;
	}

	if (StrNotEmpty(link)) {
		ShellExecute(hwnd, L"open", link, NULL, NULL, SW_SHOWNORMAL);
	}
}

//=============================================================================
//
// BFFCallBack()
//
static int CALLBACK BFFCallBack(HWND hwnd, UINT umsg, LPARAM lParam, LPARAM lpData) {
	UNREFERENCED_PARAMETER(lParam);

	if (umsg == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}

//=============================================================================
//
// GetDirectory()
//
BOOL GetDirectory(HWND hwndParent, int iTitle, LPWSTR pszFolder, LPCWSTR pszBase) {
	WCHAR szTitle[256];
	lstrcpy(szTitle, L"");
	GetString(iTitle, szTitle, COUNTOF(szTitle));

	WCHAR szBase[MAX_PATH];
	if (StrIsEmpty(pszBase)) {
		GetCurrentDirectory(MAX_PATH, szBase);
	} else {
		lstrcpy(szBase, pszBase);
	}

	BROWSEINFO bi;
	bi.hwndOwner = hwndParent;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = pszFolder;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lpfn = &BFFCallBack;
	bi.lParam = (LPARAM)szBase;
	bi.iImage = 0;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl) {
		SHGetPathFromIDList(pidl, pszFolder);
		CoTaskMemFree(pidl);
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// AboutDlgProc()
//
INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		WCHAR wch[256];
#if defined(VERSION_BUILD_TOOL_BUILD)
		wsprintf(wch, VERSION_BUILD_INFO_FORMAT, VERSION_BUILD_TOOL_NAME,
			VERSION_BUILD_TOOL_MAJOR, VERSION_BUILD_TOOL_MINOR, VERSION_BUILD_TOOL_PATCH, VERSION_BUILD_TOOL_BUILD);
#else
		wsprintf(wch, VERSION_BUILD_INFO_FORMAT, VERSION_BUILD_TOOL_NAME,
			VERSION_BUILD_TOOL_MAJOR, VERSION_BUILD_TOOL_MINOR, VERSION_BUILD_TOOL_PATCH);
#endif

		SetDlgItemText(hwnd, IDC_VERSION, VERSION_FILEVERSION_LONG);
		SetDlgItemText(hwnd, IDC_BUILD_INFO, wch);
		SetDlgItemText(hwnd, IDC_COPYRIGHT, VERSION_LEGALCOPYRIGHT_SHORT);
		SetDlgItemText(hwnd, IDC_AUTHORNAME, VERSION_AUTHORNAME);

		HFONT hFontTitle = (HFONT)SendDlgItemMessage(hwnd, IDC_VERSION, WM_GETFONT, 0, 0);
		if (hFontTitle == NULL) {
			hFontTitle = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		}

		LOGFONT lf;
		GetObject(hFontTitle, sizeof(LOGFONT), &lf);
		lf.lfWeight = FW_BOLD;
		hFontTitle = CreateFontIndirect(&lf);
		SendDlgItemMessage(hwnd, IDC_VERSION, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
		SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)(hFontTitle));

		if (GetDlgItem(hwnd, IDC_WEBPAGE_LINK) == NULL) {
			SetDlgItemText(hwnd, IDC_WEBPAGE_TEXT, VERSION_WEBPAGE_DISPLAY);
			ShowWindow(GetDlgItem(hwnd, IDC_WEBPAGE_TEXT), SW_SHOWNORMAL);
		} else {
			wsprintf(wch, L"<A>%s</A>", VERSION_WEBPAGE_DISPLAY);
			SetDlgItemText(hwnd, IDC_WEBPAGE_LINK, wch);
		}

		if (GetDlgItem(hwnd, IDC_EMAIL_LINK) == NULL) {
			SetDlgItemText(hwnd, IDC_EMAIL_TEXT, VERSION_EMAIL_DISPLAY);
			ShowWindow(GetDlgItem(hwnd, IDC_EMAIL_TEXT), SW_SHOWNORMAL);
		} else {
			wsprintf(wch, L"<A>%s</A>", VERSION_EMAIL_DISPLAY);
			SetDlgItemText(hwnd, IDC_EMAIL_LINK, wch);
		}

		if (GetDlgItem(hwnd, IDC_MOD_PAGE_LINK) == NULL) {
			SetDlgItemText(hwnd, IDC_MOD_PAGE_LINK, VERSION_MODPAGE_DISPLAY);
			ShowWindow(GetDlgItem(hwnd, IDC_MOD_PAGE_TEXT), SW_SHOWNORMAL);
		} else {
			wsprintf(wch, L"<A>%s</A>", VERSION_MODPAGE_DISPLAY);
			SetDlgItemText(hwnd, IDC_MOD_PAGE_LINK, wch);
		}

		if (GetDlgItem(hwnd, IDC_NEW_PAGE_LINK) == NULL) {
			SetDlgItemText(hwnd, IDC_NEW_PAGE_TEXT, VERSION_NEWPAGE_DISPLAY);
			ShowWindow(GetDlgItem(hwnd, IDC_NEW_PAGE_TEXT), SW_SHOWNORMAL);
		} else {
			wsprintf(wch, L"<A>%s</A>", VERSION_NEWPAGE_DISPLAY);
			SetDlgItemText(hwnd, IDC_NEW_PAGE_LINK, wch);
		}

		if (GetDlgItem(hwnd, IDC_SCI_PAGE_LINK) == NULL) {
			SetDlgItemText(hwnd, IDC_SCI_PAGE_TEXT, VERSION_SCIPAGE_DISPLAY);
			ShowWindow(GetDlgItem(hwnd, IDC_SCI_PAGE_TEXT), SW_SHOWNORMAL);
		} else {
			wsprintf(wch, L"<A>%s</A>", VERSION_SCIPAGE_DISPLAY);
			SetDlgItemText(hwnd, IDC_SCI_PAGE_LINK, wch);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_NOTIFY: {
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		switch (pnmhdr->code) {
		case NM_CLICK:
		case NM_RETURN:
			OpenHelpLink(hwnd, (int)(pnmhdr->idFrom));
			break;
		}
	}
	break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			EndDialog(hwnd, IDOK);
			break;
		}
		return TRUE;

	case WM_DESTROY: {
		HFONT hFontTitle = (HFONT)GetWindowLongPtr(hwnd, DWLP_USER);
		DeleteObject(hFontTitle);
	}
	return FALSE;
	}
	return FALSE;
}

//=============================================================================
//
// RunDlgProc()
//
extern int cxRunDlg;
static INT_PTR CALLBACK RunDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		ResizeDlg_InitX(hwnd, cxRunDlg, IDC_RESIZEGRIP3);
		MakeBitmapButton(hwnd, IDC_SEARCHEXE, g_hInstance, IDB_OPEN_FOLDER);

		HWND hwndCtl = GetDlgItem(hwnd, IDC_COMMANDLINE);
		Edit_LimitText(hwndCtl, MAX_PATH - 1);
		Edit_SetText(hwndCtl, (LPCWSTR)lParam);
		SHAutoComplete(hwndCtl, SHACF_FILESYSTEM);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		ResizeDlg_Destroy(hwnd, &cxRunDlg, NULL);
		DeleteBitmapButton(hwnd, IDC_SEARCHEXE);
		return FALSE;

	case WM_SIZE: {
		int dx;

		ResizeDlg_Size(hwnd, lParam, &dx, NULL);
		HDWP hdwp = BeginDeferWindowPos(6);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP3, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RUNDESC, dx, 0, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_SEARCHEXE, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_COMMANDLINE, dx, 0, SWP_NOMOVE);
		EndDeferWindowPos(hdwp);
		InvalidateRect(GetDlgItem(hwnd, IDC_RUNDESC), NULL, TRUE);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_SEARCHEXE: {
			WCHAR szArgs[MAX_PATH];
			WCHAR szArg2[MAX_PATH];
			WCHAR szFile[MAX_PATH * 2];

			GetDlgItemText(hwnd, IDC_COMMANDLINE, szArgs, COUNTOF(szArgs));
			ExpandEnvironmentStringsEx(szArgs, COUNTOF(szArgs));
			ExtractFirstArgument(szArgs, szFile, szArg2);

			WCHAR szFilter[256];
			GetString(IDS_FILTER_EXE, szFilter, COUNTOF(szFilter));
			PrepareFilterStr(szFilter);

			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFilter = szFilter;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = COUNTOF(szFile);
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT
						| OFN_PATHMUSTEXIST | OFN_SHAREAWARE | OFN_NODEREFERENCELINKS | OFN_NOVALIDATE;

			if (GetOpenFileName(&ofn)) {
				PathQuoteSpaces(szFile);
				if (StrNotEmpty(szArg2)) {
					lstrcat(szFile, L" ");
					lstrcat(szFile, szArg2);
				}
				SetDlgItemText(hwnd, IDC_COMMANDLINE, szFile);
			}

			PostMessage(hwnd, WM_NEXTDLGCTL, 1, 0);
		}
		break;

		case IDC_COMMANDLINE: {
			BOOL bEnableOK = FALSE;
			WCHAR args[MAX_PATH];

			if (GetDlgItemText(hwnd, IDC_COMMANDLINE, args, MAX_PATH)) {
				if (ExtractFirstArgument(args, args, NULL)) {
					if (StrNotEmpty(args)) {
						bEnableOK = TRUE;
					}
				}
			}

			EnableWindow(GetDlgItem(hwnd, IDOK), bEnableOK);
		}
		break;

		case IDOK: {
			WCHAR arg1[MAX_PATH];
			if (GetDlgItemText(hwnd, IDC_COMMANDLINE, arg1, MAX_PATH)) {
				BOOL bQuickExit = FALSE;
				WCHAR arg2[MAX_PATH];

				ExpandEnvironmentStringsEx(arg1, COUNTOF(arg1));
				ExtractFirstArgument(arg1, arg1, arg2);

				if (StrCaseEqual(arg1, L"notepad2") || StrCaseEqual(arg1, L"notepad2.exe")) {
					GetModuleFileName(NULL, arg1, COUNTOF(arg1));
					bQuickExit = TRUE;
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
				sei.lpFile = arg1;
				sei.lpParameters = arg2;
				sei.lpDirectory = wchDirectory;
				sei.nShow = SW_SHOWNORMAL;

				if (bQuickExit) {
					sei.fMask |= SEE_MASK_NOZONECHECKS;
					EndDialog(hwnd, IDOK);
					ShellExecuteEx(&sei);
				} else {
					if (ShellExecuteEx(&sei)) {
						EndDialog(hwnd, IDOK);
					} else {
						PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_COMMANDLINE)), 1);
					}
				}
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// RunDlg()
//
void RunDlg(HWND hwnd, LPCWSTR lpstrDefault) {
	ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_RUN), hwnd, RunDlgProc, (LPARAM)lpstrDefault);
}

//=============================================================================
//
// OpenWithDlgProc()
//
extern WCHAR tchOpenWithDir[MAX_PATH];
extern int flagNoFadeHidden;

extern int cxOpenWithDlg;
extern int cyOpenWithDlg;

static INT_PTR CALLBACK OpenWithDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		ResizeDlg_Init(hwnd, cxOpenWithDlg, cyOpenWithDlg, IDC_RESIZEGRIP3);

		HWND hwndLV = GetDlgItem(hwnd, IDC_OPENWITHDIR);
		InitWindowCommon(hwndLV);
		//SetExplorerTheme(hwndLV);
		ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);

		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, NULL, -1, 0, 0, 0
#if (NTDDI_VERSION >= NTDDI_VISTA)
			, 0, 0, 0
#endif
		};
		ListView_InsertColumn(hwndLV, 0, &lvc);
		DirList_Init(hwndLV, NULL);
		DirList_Fill(hwndLV, tchOpenWithDir, DL_ALLOBJECTS, NULL, FALSE, flagNoFadeHidden, DS_NAME, FALSE);
		DirList_StartIconThread(hwndLV);
		ListView_SetItemState(hwndLV, 0, LVIS_FOCUSED, LVIS_FOCUSED);

		MakeBitmapButton(hwnd, IDC_GETOPENWITHDIR, g_hInstance, IDB_OPEN_FOLDER);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		DirList_Destroy(GetDlgItem(hwnd, IDC_OPENWITHDIR));
		DeleteBitmapButton(hwnd, IDC_GETOPENWITHDIR);
		ResizeDlg_Destroy(hwnd, &cxOpenWithDlg, &cyOpenWithDlg);
		return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);

		HDWP hdwp = BeginDeferWindowPos(6);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP3, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_OPENWITHDIR, dx, dy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_GETOPENWITHDIR, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_OPENWITHDESCR, 0, dy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);

		ResizeDlgCtl(hwnd, IDC_OPENWITHDESCR, dx, 0);
		ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_OPENWITHDIR), 0, LVSCW_AUTOSIZE_USEHEADER);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY: {
		LPNMHDR pnmh = (LPNMHDR)lParam;

		if (pnmh->idFrom == IDC_OPENWITHDIR) {
			switch (pnmh->code) {
			case LVN_GETDISPINFO:
				DirList_GetDispInfo(GetDlgItem(hwnd, IDC_OPENWITHDIR), lParam, flagNoFadeHidden);
				break;

			case LVN_DELETEITEM:
				DirList_DeleteItem(GetDlgItem(hwnd, IDC_OPENWITHDIR), lParam);
				break;

			case LVN_ITEMCHANGED: {
				const NM_LISTVIEW *pnmlv = (NM_LISTVIEW *)lParam;
				EnableWindow(GetDlgItem(hwnd, IDOK), (pnmlv->uNewState & LVIS_SELECTED));
			}
			break;

			case NM_DBLCLK:
				if (ListView_GetSelectedCount(GetDlgItem(hwnd, IDC_OPENWITHDIR))) {
					SendWMCommand(hwnd, IDOK);
				}
				break;
			}
		}
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_GETOPENWITHDIR: {
			HWND hwndLV = GetDlgItem(hwnd, IDC_OPENWITHDIR);
			if (GetDirectory(hwnd, IDS_OPENWITH, tchOpenWithDir, tchOpenWithDir)) {
				DirList_Fill(hwndLV, tchOpenWithDir, DL_ALLOBJECTS, NULL, FALSE, flagNoFadeHidden, DS_NAME, FALSE);
				DirList_StartIconThread(hwndLV);
				ListView_EnsureVisible(hwndLV, 0, FALSE);
				ListView_SetItemState(hwndLV, 0, LVIS_FOCUSED, LVIS_FOCUSED);
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(hwndLV), 1);
		}
		break;

		case IDOK: {
			LPDLITEM lpdli = (LPDLITEM)GetWindowLongPtr(hwnd, DWLP_USER);
			lpdli->mask = DLI_FILENAME | DLI_TYPE;
			lpdli->ntype = DLE_NONE;
			DirList_GetItem(GetDlgItem(hwnd, IDC_OPENWITHDIR), (-1), lpdli);

			if (lpdli->ntype != DLE_NONE) {
				EndDialog(hwnd, IDOK);
			} else {
				MessageBeep(MB_OK);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// OpenWithDlg()
//
BOOL OpenWithDlg(HWND hwnd, LPCWSTR lpstrFile) {
	DLITEM dliOpenWith;
	dliOpenWith.mask = DLI_FILENAME;

	if (IDOK == ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_OPENWITH), hwnd, OpenWithDlgProc, (LPARAM)&dliOpenWith)) {
		WCHAR szParam[MAX_PATH];
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
		sei.lpFile = dliOpenWith.szFileName;
		sei.lpParameters = szParam;
		sei.lpDirectory = wchDirectory;
		sei.nShow = SW_SHOWNORMAL;

		// resolve links and get short path name
		if (!(PathIsLnkFile(lpstrFile) && PathGetLnkPath(lpstrFile, szParam, COUNTOF(szParam)))) {
			lstrcpy(szParam, lpstrFile);
		}
		//GetShortPathName(szParam, szParam, sizeof(WCHAR)*COUNTOF(szParam));
		PathQuoteSpaces(szParam);

		ShellExecuteEx(&sei);

		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// FavoritesDlgProc()
//
extern WCHAR tchFavoritesDir[MAX_PATH];
//extern int flagNoFadeHidden;

extern int cxFavoritesDlg;
extern int cyFavoritesDlg;
extern int cxAddFavoritesDlg;

static INT_PTR CALLBACK FavoritesDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		ResizeDlg_Init(hwnd, cxFavoritesDlg, cyFavoritesDlg, IDC_RESIZEGRIP3);

		HWND hwndLV = GetDlgItem(hwnd, IDC_FAVORITESDIR);
		InitWindowCommon(hwndLV);
		//SetExplorerTheme(hwndLV);
		ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, NULL, -1, 0, 0, 0
#if (NTDDI_VERSION >= NTDDI_VISTA)
			, 0, 0, 0
#endif
		};
		ListView_InsertColumn(hwndLV, 0, &lvc);
		DirList_Init(hwndLV, NULL);
		DirList_Fill(hwndLV, tchFavoritesDir, DL_ALLOBJECTS, NULL, FALSE, flagNoFadeHidden, DS_NAME, FALSE);
		DirList_StartIconThread(hwndLV);
		ListView_SetItemState(hwndLV, 0, LVIS_FOCUSED, LVIS_FOCUSED);

		MakeBitmapButton(hwnd, IDC_GETFAVORITESDIR, g_hInstance, IDB_OPEN_FOLDER);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		DirList_Destroy(GetDlgItem(hwnd, IDC_FAVORITESDIR));
		DeleteBitmapButton(hwnd, IDC_GETFAVORITESDIR);
		ResizeDlg_Destroy(hwnd, &cxFavoritesDlg, &cyFavoritesDlg);
		return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);

		HDWP hdwp = BeginDeferWindowPos(6);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP3, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_FAVORITESDIR, dx, dy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_GETFAVORITESDIR, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_FAVORITESDESCR, 0, dy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);

		ResizeDlgCtl(hwnd, IDC_FAVORITESDESCR, dx, 0);
		ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_FAVORITESDIR), 0, LVSCW_AUTOSIZE_USEHEADER);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY: {
		LPNMHDR pnmh = (LPNMHDR)lParam;

		if (pnmh->idFrom == IDC_FAVORITESDIR) {
			switch (pnmh->code) {
			case LVN_GETDISPINFO:
				DirList_GetDispInfo(GetDlgItem(hwnd, IDC_OPENWITHDIR), lParam, flagNoFadeHidden);
				break;

			case LVN_DELETEITEM:
				DirList_DeleteItem(GetDlgItem(hwnd, IDC_FAVORITESDIR), lParam);
				break;

			case LVN_ITEMCHANGED: {
				const NM_LISTVIEW *pnmlv = (NM_LISTVIEW *)lParam;
				EnableWindow(GetDlgItem(hwnd, IDOK), (pnmlv->uNewState & LVIS_SELECTED));
			}
			break;

			case NM_DBLCLK:
				if (ListView_GetSelectedCount(GetDlgItem(hwnd, IDC_FAVORITESDIR))) {
					SendWMCommand(hwnd, IDOK);
				}
				break;
			}
		}
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_GETFAVORITESDIR: {
			HWND hwndLV = GetDlgItem(hwnd, IDC_FAVORITESDIR);
			if (GetDirectory(hwnd, IDS_FAVORITES, tchFavoritesDir, tchFavoritesDir)) {
				DirList_Fill(hwndLV, tchFavoritesDir, DL_ALLOBJECTS, NULL, FALSE, flagNoFadeHidden, DS_NAME, FALSE);
				DirList_StartIconThread(hwndLV);
				ListView_EnsureVisible(hwndLV, 0, FALSE);
				ListView_SetItemState(hwndLV, 0, LVIS_FOCUSED, LVIS_FOCUSED);
			}
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(hwndLV), 1);
		}
		break;

		case IDOK: {
			LPDLITEM lpdli = (LPDLITEM)GetWindowLongPtr(hwnd, DWLP_USER);
			lpdli->mask = DLI_FILENAME | DLI_TYPE;
			lpdli->ntype = DLE_NONE;
			DirList_GetItem(GetDlgItem(hwnd, IDC_FAVORITESDIR), (-1), lpdli);

			if (lpdli->ntype != DLE_NONE) {
				EndDialog(hwnd, IDOK);
			} else {
				MessageBeep(MB_OK);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// FavoritesDlg()
//
BOOL FavoritesDlg(HWND hwnd, LPWSTR lpstrFile) {
	DLITEM dliFavorite;
	dliFavorite.mask = DLI_FILENAME;

	if (IDOK == ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FAVORITES), hwnd, FavoritesDlgProc, (LPARAM)&dliFavorite)) {
		lstrcpyn(lpstrFile, dliFavorite.szFileName, MAX_PATH);
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// AddToFavDlgProc()
//
//
static INT_PTR CALLBACK AddToFavDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		ResizeDlg_InitX(hwnd, cxAddFavoritesDlg, IDC_RESIZEGRIP);

		HWND hwndCtl = GetDlgItem(hwnd, IDC_FAVORITESFILE);
		Edit_LimitText(hwndCtl, MAX_PATH - 1);
		Edit_SetText(hwndCtl, (LPCWSTR)lParam);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		ResizeDlg_Destroy(hwnd, &cxAddFavoritesDlg, NULL);
		return FALSE;

	case WM_SIZE: {
		int dx;

		ResizeDlg_Size(hwnd, lParam, &dx, NULL);
		HDWP hdwp = BeginDeferWindowPos(5);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_FAVORITESDESCR, dx, 0, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_FAVORITESFILE, dx, 0, SWP_NOMOVE);
		EndDeferWindowPos(hdwp);
		InvalidateRect(GetDlgItem(hwnd, IDC_FAVORITESDESCR), NULL, TRUE);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FAVORITESFILE:
			EnableWindow(GetDlgItem(hwnd, IDOK), GetWindowTextLength(GetDlgItem(hwnd, IDC_FAVORITESFILE)));
			break;

		case IDOK: {
			LPWSTR pszName = (LPWSTR)GetWindowLongPtr(hwnd, DWLP_USER);
			GetDlgItemText(hwnd, IDC_FAVORITESFILE, pszName, MAX_PATH - 1);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// AddToFavDlg()
//
BOOL AddToFavDlg(HWND hwnd, LPCWSTR lpszName, LPCWSTR lpszTarget) {
	WCHAR pszName[MAX_PATH];
	lstrcpy(pszName, lpszName);

	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ADDTOFAV), hwnd, AddToFavDlgProc, (LPARAM)pszName);

	if (iResult == IDOK) {
		if (PathCreateFavLnk(pszName, lpszTarget, tchFavoritesDir)) {
			MsgBoxInfo(MB_OK, IDS_FAV_SUCCESS);
			return TRUE;
		}
		MsgBoxWarn(MB_OK, IDS_FAV_FAILURE);
	}

	return FALSE;
}

//=============================================================================
//
// FileMRUDlgProc()
//
//
extern LPMRULIST pFileMRU;
extern BOOL bSaveRecentFiles;
extern int cxFileMRUDlg;
extern int cyFileMRUDlg;

typedef struct ICONTHREADINFO {
	HWND hwnd;					// HWND of ListView Control
	HANDLE hExitThread;			// Flag is set when Icon Thread should terminate
	HANDLE hTerminatedThread;	// Flag is set when Icon Thread has terminated
	HANDLE hFileMRUIconThread;
} ICONTHREADINFO, *LPICONTHREADINFO;

static DWORD WINAPI FileMRUIconThread(LPVOID lpParam) {
	WCHAR tch[MAX_PATH] = L"";
	DWORD dwFlags = SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_ATTRIBUTES | SHGFI_ATTR_SPECIFIED;

	LPICONTHREADINFO lpit = (LPICONTHREADINFO)lpParam;
	ResetEvent(lpit->hTerminatedThread);

	HWND hwnd = lpit->hwnd;
	const int iMaxItem = ListView_GetItemCount(hwnd);
	int iItem = 0;

	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));

	while (iItem < iMaxItem && WaitForSingleObject(lpit->hExitThread, 0) != WAIT_OBJECT_0) {
		lvi.mask = LVIF_TEXT;
		lvi.pszText = tch;
		lvi.cchTextMax = COUNTOF(tch);
		lvi.iItem = iItem;
		if (ListView_GetItem(hwnd, &lvi)) {
			SHFILEINFO shfi;
			DWORD dwAttr = 0;
			if (PathIsUNC(tch) || !PathIsFile(tch)) {
				dwFlags |= SHGFI_USEFILEATTRIBUTES;
				dwAttr = FILE_ATTRIBUTE_NORMAL;
				shfi.dwAttributes = 0;
				SHGetFileInfo(PathFindFileName(tch), dwAttr, &shfi, sizeof(SHFILEINFO), dwFlags);
			} else {
				shfi.dwAttributes = SFGAO_LINK | SFGAO_SHARE;
				SHGetFileInfo(tch, dwAttr, &shfi, sizeof(SHFILEINFO), dwFlags);
			}

			lvi.mask = LVIF_IMAGE;
			lvi.iImage = shfi.iIcon;
			lvi.stateMask = 0;
			lvi.state = 0;

			if (shfi.dwAttributes & SFGAO_LINK) {
				lvi.mask |= LVIF_STATE;
				lvi.stateMask |= LVIS_OVERLAYMASK;
				lvi.state |= INDEXTOOVERLAYMASK(2);
			}

			if (shfi.dwAttributes & SFGAO_SHARE) {
				lvi.mask |= LVIF_STATE;
				lvi.stateMask |= LVIS_OVERLAYMASK;
				lvi.state |= INDEXTOOVERLAYMASK(1);
			}

			if (PathIsUNC(tch)) {
				dwAttr = FILE_ATTRIBUTE_NORMAL;
			} else {
				dwAttr = GetFileAttributes(tch);
			}

			if (!flagNoFadeHidden &&
					dwAttr != INVALID_FILE_ATTRIBUTES &&
					dwAttr & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
				lvi.mask |= LVIF_STATE;
				lvi.stateMask |= LVIS_CUT;
				lvi.state |= LVIS_CUT;
			}

			lvi.iSubItem = 0;
			ListView_SetItem(hwnd, &lvi);
		}
		iItem++;
	}

	SetEvent(lpit->hTerminatedThread);
	ExitThread(0);
}

static INT_PTR CALLBACK FileMRUDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);

		HWND hwndLV = GetDlgItem(hwnd, IDC_FILEMRU);
		InitWindowCommon(hwndLV);

		LPICONTHREADINFO lpit = (LPICONTHREADINFO)GlobalAlloc(GPTR, sizeof(ICONTHREADINFO));
		SetProp(hwnd, L"it", (HANDLE)lpit);
		lpit->hwnd = hwndLV;
		lpit->hExitThread = CreateEvent(NULL, TRUE, FALSE, NULL);
		lpit->hTerminatedThread = CreateEvent(NULL, TRUE, TRUE, NULL);

		ResizeDlg_Init(hwnd, cxFileMRUDlg, cyFileMRUDlg, IDC_RESIZEGRIP);

		SHFILEINFO shfi;
		ListView_SetImageList(hwndLV,
							  (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO), SHGFI_SMALLICON | SHGFI_SYSICONINDEX),
							  LVSIL_SMALL);

		ListView_SetImageList(hwndLV,
							  (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO), SHGFI_LARGEICON | SHGFI_SYSICONINDEX),
							  LVSIL_NORMAL);

		//SetExplorerTheme(hwndLV);
		ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT, LVCFMT_LEFT, 0, NULL, -1, 0, 0, 0
#if (NTDDI_VERSION >= NTDDI_VISTA)
			, 0, 0, 0
#endif
		};
		ListView_InsertColumn(hwndLV, 0, &lvc);

		// Update view
		SendWMCommand(hwnd, IDC_FILEMRU_UPDATE_VIEW);

		if (bSaveRecentFiles) {
			CheckDlgButton(hwnd, IDC_SAVEMRU, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY: {
		LPICONTHREADINFO lpit = (LPICONTHREADINFO)GetProp(hwnd, L"it");
		SetEvent(lpit->hExitThread);
		while (WaitForSingleObject(lpit->hTerminatedThread, 0) != WAIT_OBJECT_0) {
			MSG msg;
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		CloseHandle(lpit->hExitThread);
		CloseHandle(lpit->hTerminatedThread);
		CloseHandle(lpit->hFileMRUIconThread);
		RemoveProp(hwnd, L"it");
		GlobalFree(lpit);

		bSaveRecentFiles = IsButtonChecked(hwnd, IDC_SAVEMRU);

		ResizeDlg_Destroy(hwnd, &cxFileMRUDlg, &cyFileMRUDlg);
	}
	return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);

		HDWP hdwp = BeginDeferWindowPos(6);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_FILEMRU, dx, dy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_EMPTY_MRU, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_SAVEMRU, 0, dy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);
		ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_FILEMRU), 0, LVSCW_AUTOSIZE_USEHEADER);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY: {
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		if (pnmhdr->idFrom == IDC_FILEMRU) {
			switch (pnmhdr->code) {
			case NM_DBLCLK:
				SendWMCommand(hwnd, IDOK);
				break;

			case LVN_GETDISPINFO: {
				/*
				LV_DISPINFO *lpdi = (LPVOID)lParam;

				if (lpdi->item.mask & LVIF_IMAGE) {
					WCHAR tch[MAX_PATH];

					LV_ITEM lvi;
					ZeroMemory(&lvi, sizeof(LV_ITEM));
					lvi.mask = LVIF_TEXT;
					lvi.pszText = tch;
					lvi.cchTextMax = COUNTOF(tch);
					lvi.iItem = lpdi->item.iItem;

					ListView_GetItem(GetDlgItem(hwnd, IDC_FILEMRU), &lvi);

					DWORD dwFlags = SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_ATTRIBUTES | SHGFI_ATTR_SPECIFIED;
					DWORD dwAttr = 0;
					SHFILEINFO shfi;
					if (!PathIsFile(tch)) {
						dwFlags |= SHGFI_USEFILEATTRIBUTES;
						dwAttr = FILE_ATTRIBUTE_NORMAL;
						shfi.dwAttributes = 0;
						SHGetFileInfo(PathFindFileName(tch), dwAttr, &shfi, sizeof(SHFILEINFO), dwFlags);
					} else {
						shfi.dwAttributes = SFGAO_LINK | SFGAO_SHARE;
						SHGetFileInfo(tch, dwAttr, &shfi, sizeof(SHFILEINFO), dwFlags);
					}

					lpdi->item.iImage = shfi.iIcon;
					lpdi->item.mask |= LVIF_DI_SETITEM;
					lpdi->item.stateMask = 0;
					lpdi->item.state = 0;

					if (shfi.dwAttributes & SFGAO_LINK) {
						lpdi->item.mask |= LVIF_STATE;
						lpdi->item.stateMask |= LVIS_OVERLAYMASK;
						lpdi->item.state |= INDEXTOOVERLAYMASK(2);
					}

					if (shfi.dwAttributes & SFGAO_SHARE) {
						lpdi->item.mask |= LVIF_STATE;
						lpdi->item.stateMask |= LVIS_OVERLAYMASK;
						lpdi->item.state |= INDEXTOOVERLAYMASK(1);
					}

					dwAttr = GetFileAttributes(tch);

					if (!flagNoFadeHidden &&
							dwAttr != INVALID_FILE_ATTRIBUTES &&
							dwAttr & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
						lpdi->item.mask |= LVIF_STATE;
						lpdi->item.stateMask |= LVIS_CUT;
						lpdi->item.state |= LVIS_CUT;
					}
				}
				*/
			}
			break;

			case LVN_ITEMCHANGED:
			case LVN_DELETEITEM:
				EnableWindow(GetDlgItem(hwnd, IDOK), ListView_GetSelectedCount(GetDlgItem(hwnd, IDC_FILEMRU)));
				break;
			}
		} else if (pnmhdr->idFrom == IDC_EMPTY_MRU) {
			if ((pnmhdr->code == NM_CLICK || pnmhdr->code == NM_RETURN)) {
				MRU_Empty(pFileMRU);
				if (StrNotEmpty(szCurFile)) {
					MRU_Add(pFileMRU, szCurFile);
				}
				MRU_Save(pFileMRU);
				SendWMCommand(hwnd, IDC_FILEMRU_UPDATE_VIEW);
			}
		}
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FILEMRU_UPDATE_VIEW: {
			LPICONTHREADINFO lpit = (LPICONTHREADINFO)GetProp(hwnd, L"it");

			SetEvent(lpit->hExitThread);
			while (WaitForSingleObject(lpit->hTerminatedThread, 0) != WAIT_OBJECT_0) {
				MSG msg;
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			ResetEvent(lpit->hExitThread);
			SetEvent(lpit->hTerminatedThread);

			HWND hwndLV = GetDlgItem(hwnd, IDC_FILEMRU);
			ListView_DeleteAllItems(hwndLV);

			LV_ITEM lvi;
			ZeroMemory(&lvi, sizeof(LV_ITEM));
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;

			SHFILEINFO shfi;
			SHGetFileInfo(L"Icon", FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
						  SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
			lvi.iImage = shfi.iIcon;

			WCHAR tch[MAX_PATH];
			for (int i = 0; i < MRU_GetCount(pFileMRU); i++) {
				MRU_Enum(pFileMRU, i, tch, COUNTOF(tch));
				PathAbsoluteFromApp(tch, tch, COUNTOF(tch), TRUE);
				lvi.iItem = i;
				lvi.pszText = tch;
				ListView_InsertItem(hwndLV, &lvi);
			}

			ListView_SetItemState(hwndLV, 0, LVIS_FOCUSED, LVIS_FOCUSED);
			ListView_SetColumnWidth(hwndLV, 0, LVSCW_AUTOSIZE_USEHEADER);

			DWORD dwtid;
			lpit->hFileMRUIconThread = CreateThread(NULL, 0, FileMRUIconThread, (LPVOID)lpit, 0, &dwtid);
		}
		break;

		case IDC_FILEMRU:
			break;

		case IDOK: {
			WCHAR tch[MAX_PATH];
			HWND hwndLV = GetDlgItem(hwnd, IDC_FILEMRU);

			if (ListView_GetSelectedCount(hwndLV)) {
				LV_ITEM lvi;
				ZeroMemory(&lvi, sizeof(LV_ITEM));

				lvi.mask = LVIF_TEXT;
				lvi.pszText = tch;
				lvi.cchTextMax = COUNTOF(tch);
				lvi.iItem = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_SELECTED);

				ListView_GetItem(hwndLV, &lvi);

				PathUnquoteSpaces(tch);

				if (!PathIsFile(tch)) {
					// Ask...
					if (IDYES == MsgBoxWarn(MB_YESNO, IDS_ERR_MRUDLG)) {
						MRU_Delete(pFileMRU, lvi.iItem);
						MRU_DeleteFileFromStore(pFileMRU, tch);

						// must use recreate the list, index might change...
						//ListView_DeleteItem(hwndLV, lvi.iItem);
						SendWMCommand(hwnd, IDC_FILEMRU_UPDATE_VIEW);

						EnableWindow(GetDlgItem(hwnd, IDOK), ListView_GetSelectedCount(hwndLV));
					}
				} else {
					lstrcpy((LPWSTR)GetWindowLongPtr(hwnd, DWLP_USER), tch);
					EndDialog(hwnd, IDOK);
				}
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// FileMRUDlg()
//
//
BOOL FileMRUDlg(HWND hwnd, LPWSTR lpstrFile) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_FILEMRU), hwnd, FileMRUDlgProc, (LPARAM)lpstrFile);
	return iResult == IDOK;
}

//=============================================================================
//
// ChangeNotifyDlgProc()
//
//
extern int iFileWatchingMode;
extern int iFileWatchingMethod;
extern BOOL bFileWatchingKeepAtEnd;
extern BOOL bResetFileWatching;

static INT_PTR CALLBACK ChangeNotifyDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG:
		CheckRadioButton(hwnd, IDC_CHANGENOTIFY_NONE, IDC_CHANGENOTIFY_AUTO_RELOAD, IDC_CHANGENOTIFY_NONE + iFileWatchingMode);
		if (iFileWatchingMethod) {
			CheckDlgButton(hwnd, IDC_CHANGENOTIFY_USE_POLLING, BST_CHECKED);
		}
		if (bFileWatchingKeepAtEnd) {
			CheckDlgButton(hwnd, IDC_CHANGENOTIFY_KEEP_AT_END, BST_CHECKED);
		}
		if (bResetFileWatching) {
			CheckDlgButton(hwnd, IDC_CHANGENOTIFY_RESET_WATCH, BST_CHECKED);
		}
		CenterDlgInParent(hwnd);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			iFileWatchingMode = GetCheckedRadioButton(hwnd, IDC_CHANGENOTIFY_NONE, IDC_CHANGENOTIFY_AUTO_RELOAD) - IDC_CHANGENOTIFY_NONE;
			iFileWatchingMethod = IsButtonChecked(hwnd, IDC_CHANGENOTIFY_USE_POLLING);
			bFileWatchingKeepAtEnd = IsButtonChecked(hwnd, IDC_CHANGENOTIFY_KEEP_AT_END);
			bResetFileWatching = IsButtonChecked(hwnd, IDC_CHANGENOTIFY_RESET_WATCH);
			EndDialog(hwnd, IDOK);
			break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// ChangeNotifyDlg()
//
BOOL ChangeNotifyDlg(HWND hwnd) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_CHANGENOTIFY), hwnd, ChangeNotifyDlgProc, 0);
	return iResult == IDOK;
}

//=============================================================================
//
// ColumnWrapDlgProc()
//
//
static INT_PTR CALLBACK ColumnWrapDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const int iNumber = *((int *)lParam);

		SetDlgItemInt(hwnd, IDC_COLUMNWRAP, iNumber, FALSE);
		SendDlgItemMessage(hwnd, IDC_COLUMNWRAP, EM_LIMITTEXT, 15, 0);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated;
			const int iNewNumber = GetDlgItemInt(hwnd, IDC_COLUMNWRAP, &fTranslated, FALSE);

			if (fTranslated) {
				int *piNumber = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
				*piNumber = iNewNumber;

				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_COLUMNWRAP)), 1);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// ColumnWrapDlg()
//
BOOL ColumnWrapDlg(HWND hwnd, int *iNumber) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_COLUMNWRAP), hwnd, ColumnWrapDlgProc, (LPARAM)iNumber);
	return iResult == IDOK;
}

//=============================================================================
//
// WordWrapSettingsDlgProc()
//
//
extern BOOL fWordWrap;
extern int iWordWrapMode;
extern int iWordWrapIndent;
extern int iWordWrapSymbols;
extern BOOL bShowWordWrapSymbols;
extern BOOL bWordWrapSelectSubLine;
extern BOOL bHighlightCurrentSubLine;

static INT_PTR CALLBACK WordWrapSettingsDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG: {
		WCHAR tch[512];
		for (int i = 0; i < 4; i++) {
			HWND hwndCtl = GetDlgItem(hwnd, IDC_WRAP_INDENT + i);
			GetDlgItemText(hwnd, IDC_WRAP_INDENT_OPTIONS + i, tch, COUNTOF(tch));
			lstrcat(tch, L"|");
			LPWSTR p1 = tch;
			LPWSTR p2;
			while ((p2 = StrChr(p1, L'|')) != NULL) {
				*p2++ = L'\0';
				if (*p1) {
					ComboBox_AddString(hwndCtl, p1);
				}
				p1 = p2;
			}

			ComboBox_SetExtendedUI(hwndCtl, TRUE);
		}

		SendDlgItemMessage(hwnd, IDC_WRAP_INDENT, CB_SETCURSEL, iWordWrapIndent, 0);
		SendDlgItemMessage(hwnd, IDC_WRAP_SYMBOL_BEFORE, CB_SETCURSEL, bShowWordWrapSymbols ? (iWordWrapSymbols % 10) : 0, 0);
		SendDlgItemMessage(hwnd, IDC_WRAP_SYMBOL_AFTER, CB_SETCURSEL, bShowWordWrapSymbols ? (iWordWrapSymbols / 10) : 0, 0);
		SendDlgItemMessage(hwnd, IDC_WRAP_MODE, CB_SETCURSEL, iWordWrapMode, 0);

		if (bWordWrapSelectSubLine) {
			CheckDlgButton(hwnd, IDC_WRAP_SELECT_SUBLINE, BST_CHECKED);
		}
		if (bHighlightCurrentSubLine) {
			CheckDlgButton(hwnd, IDC_WRAP_HIGHLIGHT_SUBLINE, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			iWordWrapIndent = (int)SendDlgItemMessage(hwnd, IDC_WRAP_INDENT, CB_GETCURSEL, 0, 0);
			bShowWordWrapSymbols = FALSE;
			int iSel = (int)SendDlgItemMessage(hwnd, IDC_WRAP_SYMBOL_BEFORE, CB_GETCURSEL, 0, 0);
			const int iSel2 = (int)SendDlgItemMessage(hwnd, IDC_WRAP_SYMBOL_AFTER, CB_GETCURSEL, 0, 0);
			if (iSel > 0 || iSel2 > 0) {
				bShowWordWrapSymbols = TRUE;
				iWordWrapSymbols = iSel + iSel2 * 10;
			}

			iSel = (int)SendDlgItemMessage(hwnd, IDC_WRAP_MODE, CB_GETCURSEL, 0, 0);
			if (iSel == SC_WRAP_NONE) {
				fWordWrap = FALSE;
			} else {
				fWordWrap = TRUE;
				iWordWrapMode = iSel;
			}

			bWordWrapSelectSubLine = IsButtonChecked(hwnd, IDC_WRAP_SELECT_SUBLINE);
			bHighlightCurrentSubLine = IsButtonChecked(hwnd, IDC_WRAP_HIGHLIGHT_SUBLINE);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// WordWrapSettingsDlg()
//
BOOL WordWrapSettingsDlg(HWND hwnd, int *iNumber) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_WORDWRAP), hwnd, WordWrapSettingsDlgProc, (LPARAM)iNumber);
	return iResult == IDOK;
}

//=============================================================================
//
// LongLineSettingsDlgProc()
//
//
extern int iLongLineMode;

static INT_PTR CALLBACK LongLineSettingsDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const int iNumber = *((int *)lParam);

		SetDlgItemInt(hwnd, IDC_LONGLINE_LIMIT, iNumber, FALSE);
		SendDlgItemMessage(hwnd, IDC_LONGLINE_LIMIT, EM_LIMITTEXT, 15, 0);

		if (iLongLineMode == EDGE_LINE) {
			CheckRadioButton(hwnd, IDC_LONGLINE_EDGE_LINE, IDC_LONGLINE_BACK_COLOR, IDC_LONGLINE_EDGE_LINE);
		} else {
			CheckRadioButton(hwnd, IDC_LONGLINE_EDGE_LINE, IDC_LONGLINE_BACK_COLOR, IDC_LONGLINE_BACK_COLOR);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated;
			const int iNewNumber = GetDlgItemInt(hwnd, IDC_LONGLINE_LIMIT, &fTranslated, FALSE);

			if (fTranslated) {
				int *piNumber = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
				*piNumber = iNewNumber;
				iLongLineMode = IsButtonChecked(hwnd, IDC_LONGLINE_EDGE_LINE) ? EDGE_LINE : EDGE_BACKGROUND;

				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_LONGLINE_LIMIT)), 1);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// LongLineSettingsDlg()
//
BOOL LongLineSettingsDlg(HWND hwnd, int *iNumber) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_LONGLINES), hwnd, LongLineSettingsDlgProc, (LPARAM)iNumber);
	return iResult == IDOK;
}

//=============================================================================
//
// TabSettingsDlgProc()
//
//
extern int iTabWidth;
extern int iIndentWidth;
extern BOOL bTabsAsSpaces;
extern BOOL bTabIndents;
extern BOOL bBackspaceUnindents;

static INT_PTR CALLBACK TabSettingsDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG: {
		SetDlgItemInt(hwnd, IDC_TAB_WIDTH, iTabWidth, FALSE);
		SendDlgItemMessage(hwnd, IDC_TAB_WIDTH, EM_LIMITTEXT, 15, 0);

		SetDlgItemInt(hwnd, IDC_INDENT_WIDTH, iIndentWidth, FALSE);
		SendDlgItemMessage(hwnd, IDC_INDENT_WIDTH, EM_LIMITTEXT, 15, 0);

		if (bTabsAsSpaces) {
			CheckDlgButton(hwnd, IDC_TAB_AS_SPACE, BST_CHECKED);
		}

		if (bTabIndents) {
			CheckDlgButton(hwnd, IDC_TAB_INDENT, BST_CHECKED);
		}

		if (bBackspaceUnindents) {
			CheckDlgButton(hwnd, IDC_BACKSPACE_UNINDENT, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated1;
			BOOL fTranslated2;

			const int iNewTabWidth = GetDlgItemInt(hwnd, IDC_TAB_WIDTH, &fTranslated1, FALSE);
			const int iNewIndentWidth = GetDlgItemInt(hwnd, IDC_INDENT_WIDTH, &fTranslated2, FALSE);

			if (fTranslated1 && fTranslated2) {
				iTabWidth = iNewTabWidth;
				iIndentWidth = iNewIndentWidth;

				bTabsAsSpaces = IsButtonChecked(hwnd, IDC_TAB_AS_SPACE);
				bTabIndents = IsButtonChecked(hwnd, IDC_TAB_INDENT);
				bBackspaceUnindents = IsButtonChecked(hwnd, IDC_BACKSPACE_UNINDENT);

				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, fTranslated1 ? IDC_INDENT_WIDTH : IDC_TAB_WIDTH)), 1);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// TabSettingsDlg()
//
BOOL TabSettingsDlg(HWND hwnd) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_TABSETTINGS), hwnd, TabSettingsDlgProc, 0);
	return iResult == IDOK;
}

//=============================================================================
//
// SelectDefEncodingDlgProc()
//
//
typedef struct ENCODEDLG {
	BOOL bRecodeOnly;
	int  idEncoding;
	int  cxDlg;
	int  cyDlg;
	UINT uidLabel;
} ENCODEDLG, *PENCODEDLG;

typedef const ENCODEDLG *LPCENCODEDLG;

static INT_PTR CALLBACK SelectDefEncodingDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);

		const int iEncoding = *((int *)lParam);
		Encoding_GetLabel(iEncoding);
		SetDlgItemText(hwnd, IDC_ENCODING_LABEL, mEncoding[iEncoding].wchLabel);

		if (bSkipUnicodeDetection) {
			CheckDlgButton(hwnd, IDC_NOUNICODEDETECTION, BST_CHECKED);
		}

		if (bLoadANSIasUTF8) {
			CheckDlgButton(hwnd, IDC_ANSIASUTF8, BST_CHECKED);
		}
		if (bLoadASCIIasUTF8) {
			CheckDlgButton(hwnd, IDC_ASCIIASUTF8, BST_CHECKED);
		}

		if (bLoadNFOasOEM) {
			CheckDlgButton(hwnd, IDC_NFOASOEM, BST_CHECKED);
		}

		if (bNoEncodingTags) {
			CheckDlgButton(hwnd, IDC_ENCODINGFROMFILEVARS, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_NOTIFY: {
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		switch (pnmhdr->code) {
		case NM_CLICK:
		case NM_RETURN:
			if (pnmhdr->idFrom == IDC_ENCODING_LINK) {
				int *pidREncoding = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
				if (SelectEncodingDlg(hwndMain, pidREncoding, IDS_SELRECT_DEFAULT_ENCODING)) {
					Encoding_GetLabel(*pidREncoding);
					SetDlgItemText(hwnd, IDC_ENCODING_LABEL, mEncoding[*pidREncoding].wchLabel);
				}
			}
			break;
		}
	}
	break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			bSkipUnicodeDetection = IsButtonChecked(hwnd, IDC_NOUNICODEDETECTION);
			bLoadANSIasUTF8 = IsButtonChecked(hwnd, IDC_ANSIASUTF8);
			bLoadASCIIasUTF8 = IsButtonChecked(hwnd, IDC_ASCIIASUTF8);
			bLoadNFOasOEM = IsButtonChecked(hwnd, IDC_NFOASOEM);
			bNoEncodingTags = IsButtonChecked(hwnd, IDC_ENCODINGFROMFILEVARS);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// SelectDefEncodingDlg()
//
BOOL SelectDefEncodingDlg(HWND hwnd, int *pidREncoding) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DEFENCODING), hwnd, SelectDefEncodingDlgProc, (LPARAM)(pidREncoding));
	return iResult == IDOK;
}

//=============================================================================
//
// SelectEncodingDlgProc()
//
//
static INT_PTR CALLBACK SelectEncodingDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const LPCENCODEDLG pdd = (LPCENCODEDLG)lParam;
		ResizeDlg_Init(hwnd, pdd->cxDlg, pdd->cyDlg, IDC_RESIZEGRIP);

		WCHAR wch[256];
		GetString(pdd->uidLabel, wch, COUNTOF(wch));
		SetDlgItemText(hwnd, IDC_ENCODING_LABEL, wch);

		// TODO: following code is buggy when icon size for shfi.hIcon is larger than bmp.bmHeight,
		// we need to determine icon size first, then resize the encoding mask bitmap accordingly.

		HBITMAP hbmp = (HBITMAP)LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_ENCODING), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		hbmp = ResizeImageForCurrentDPI(hbmp, 16); // 32x16
		BITMAP bmp;
		GetObject(hbmp, sizeof(BITMAP), &bmp);
		HIMAGELIST himl = ImageList_Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
		ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
		DeleteObject(hbmp);

		// folder icon
		const DWORD iconFlags = GetCurrentIconHandleFlags();
		SHFILEINFO shfi;
		SHGetFileInfo(L"Icon", FILE_ATTRIBUTE_DIRECTORY, &shfi, sizeof(SHFILEINFO), iconFlags);
		ImageList_AddIcon(himl, shfi.hIcon);

		HWND hwndTV = GetDlgItem(hwnd, IDC_ENCODINGLIST);
		InitWindowCommon(hwndTV);
		TreeView_SetExtendedStyle(hwndTV, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);
		SetExplorerTheme(hwndTV);

		TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL);
		Encoding_AddToTreeView(hwndTV, pdd->idEncoding, pdd->bRecodeOnly);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY: {
		PENCODEDLG pdd = (PENCODEDLG)GetWindowLongPtr(hwnd, DWLP_USER);
		ResizeDlg_Destroy(hwnd, &pdd->cxDlg, &pdd->cyDlg);
		HIMAGELIST himl = TreeView_GetImageList(GetDlgItem(hwnd, IDC_ENCODINGLIST), TVSIL_NORMAL);
		ImageList_Destroy(himl);
	}
	return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);

		HDWP hdwp = BeginDeferWindowPos(4);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_ENCODINGLIST, dx, dy, SWP_NOMOVE);
		EndDeferWindowPos(hdwp);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY: {
		LPNMHDR lpnmh = (LPNMHDR)lParam;
		if (lpnmh->idFrom == IDC_ENCODINGLIST) {
			switch (lpnmh->code) {
			case NM_DBLCLK: {
				int temp = -1;
				if (Encoding_GetFromTreeView(GetDlgItem(hwnd, IDC_ENCODINGLIST), &temp, TRUE)) {
					SendWMCommand(hwnd, IDOK);
				}
			}
			break;

			case TVN_SELCHANGED: {
				LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)lParam;
				EnableWindow(GetDlgItem(hwnd, IDOK), lpnmtv->itemNew.lParam != 0);
				if (lpnmtv->itemNew.lParam == 0) {
					TreeView_Expand(GetDlgItem(hwnd, IDC_ENCODINGLIST), lpnmtv->itemNew.hItem, TVE_EXPAND);
				}
			}
			break;
			}
		}
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			HWND hwndTV = GetDlgItem(hwnd, IDC_ENCODINGLIST);
			PENCODEDLG pdd = (PENCODEDLG)GetWindowLongPtr(hwnd, DWLP_USER);
			if (Encoding_GetFromTreeView(hwndTV, &pdd->idEncoding, FALSE)) {
				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)hwndTV, 1);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// SelectEncodingDlg()
//
extern int cxEncodingDlg;
extern int cyEncodingDlg;

BOOL SelectEncodingDlg(HWND hwnd, int *pidREncoding, UINT uidLabel) {
	ENCODEDLG dd;

	dd.bRecodeOnly = (uidLabel == IDS_SELRECT_RELOAD_ENCODING);
	dd.idEncoding = *pidREncoding;
	dd.cxDlg = cxEncodingDlg;
	dd.cyDlg = cyEncodingDlg;
	dd.uidLabel = uidLabel;

	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_SELECT_ENCODING), hwnd, SelectEncodingDlgProc, (LPARAM)&dd);

	cxEncodingDlg = dd.cxDlg;
	cyEncodingDlg = dd.cyDlg;

	if (iResult == IDOK) {
		*pidREncoding = dd.idEncoding;
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// SelectDefLineEndingDlgProc()
//
static INT_PTR CALLBACK SelectDefLineEndingDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const int iOption = *((int *)lParam);

		// Load options
		HWND hwndCtl = GetDlgItem(hwnd, IDC_EOLMODELIST);
		WCHAR wch[128];
		for (int i = 0; i < 3; i++) {
			GetString(IDS_EOLMODENAME_CRLF + i, wch, COUNTOF(wch));
			ComboBox_AddString(hwndCtl, wch);
		}

		ComboBox_SetCurSel(hwndCtl, iOption);
		ComboBox_SetExtendedUI(hwndCtl, TRUE);

		if (bWarnLineEndings) {
			CheckDlgButton(hwnd, IDC_WARNINCONSISTENTEOLS, BST_CHECKED);
		}

		if (bFixLineEndings) {
			CheckDlgButton(hwnd, IDC_CONSISTENTEOLS, BST_CHECKED);
		}

		if (bAutoStripBlanks) {
			CheckDlgButton(hwnd, IDC_AUTOSTRIPBLANKS, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			int *piOption = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
			*piOption = (int)SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_GETCURSEL, 0, 0);
			bWarnLineEndings = IsButtonChecked(hwnd, IDC_WARNINCONSISTENTEOLS);
			bFixLineEndings = IsButtonChecked(hwnd, IDC_CONSISTENTEOLS);
			bAutoStripBlanks = IsButtonChecked(hwnd, IDC_AUTOSTRIPBLANKS);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// SelectDefLineEndingDlg()
//
BOOL SelectDefLineEndingDlg(HWND hwnd, int *iOption) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DEFEOLMODE), hwnd, SelectDefLineEndingDlgProc, (LPARAM)iOption);
	return iResult == IDOK;
}

static INT_PTR CALLBACK WarnLineEndingDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const EditFileIOStatus * const status = (EditFileIOStatus *)lParam;
		int iEOLMode = status->iEOLMode;
		iEOLMode = (iEOLMode == SC_EOL_CRLF) ? 0 : (iEOLMode == SC_EOL_LF) ? 1 : 2;

		// Load options
		HWND hwndCtl = GetDlgItem(hwnd, IDC_EOLMODELIST);
		WCHAR wch[128];
		for (int i = 0; i < 3; i++) {
			GetString(IDS_EOLMODENAME_CRLF + i, wch, COUNTOF(wch));
			ComboBox_AddString(hwndCtl, wch);
		}

		ComboBox_SetCurSel(hwndCtl, iEOLMode);
		ComboBox_SetExtendedUI(hwndCtl, TRUE);

		WCHAR tchFmt[128];
		for (int i = 0; i < 3; i++) {
			WCHAR tchLn[32];
			PosToStrW(status->linesCount[i], tchLn);
			FormatNumberStr(tchLn);
			GetDlgItemText(hwnd, IDC_EOL_SUM_CRLF + i, tchFmt, COUNTOF(tchFmt));
			wsprintf(wch, tchFmt, tchLn);
			SetDlgItemText(hwnd, IDC_EOL_SUM_CRLF + i, wch);
		}

		if (bWarnLineEndings) {
			CheckDlgButton(hwnd, IDC_WARNINCONSISTENTEOLS, BST_CHECKED);
		}
		if (status->bLineEndingsDefaultNo) {
			SendMessage(hwnd, DM_SETDEFID, IDCANCEL, 0);
		}
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			EditFileIOStatus *status = (EditFileIOStatus *)GetWindowLongPtr(hwnd, DWLP_USER);
			const int iEOLMode = (int)SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_GETCURSEL, 0, 0);
			status->iEOLMode = iEOLMode;
			bWarnLineEndings = IsButtonChecked(hwnd, IDC_WARNINCONSISTENTEOLS);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			bWarnLineEndings = IsButtonChecked(hwnd, IDC_WARNINCONSISTENTEOLS);
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WarnLineEndingDlg(HWND hwnd, struct EditFileIOStatus *status) {
	MessageBeep(MB_ICONEXCLAMATION);
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_WARNLINEENDS), hwnd, WarnLineEndingDlgProc, (LPARAM)status);
	return iResult == IDOK;
}

void InitZoomLevelComboBox(HWND hwnd, int nCtlId, int zoomLevel) {
	WCHAR tch[16];
	int selIndex = -1;
	const int levelList[] = {
		500, 450, 400, 350, 300, 250,
		200, 175, 150, 125, 100, 75, 50, 25,
	};

	HWND hwndCtl = GetDlgItem(hwnd, nCtlId);
	ComboBox_LimitText(hwndCtl, 8);
	for (int i = 0; i < (int)COUNTOF(levelList); i++) {
		const int level = levelList[i];
		if (zoomLevel == level) {
			selIndex = i;
		}
		wsprintf(tch, L"%d%%", level);
		ComboBox_AddString(hwndCtl, tch);
	}

	ComboBox_SetExtendedUI(hwndCtl, TRUE);
	ComboBox_SetCurSel(hwndCtl, selIndex);
	if (selIndex == -1) {
		wsprintf(tch, L"%d%%", zoomLevel);
		SetWindowText(hwndCtl, tch);
	}
}

BOOL GetZoomLevelComboBoxValue(HWND hwnd, int nCtrId, int *zoomLevel) {
	WCHAR tch[16];
	GetDlgItemText(hwnd, nCtrId, tch, COUNTOF(tch));
	return CRTStrToInt(tch, zoomLevel) && *zoomLevel >= SC_MIN_ZOOM_LEVEL && *zoomLevel <= SC_MAX_ZOOM_LEVEL;
}

static INT_PTR CALLBACK ZoomLevelDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		const int zoomLevel = SciCall_GetZoom();
		InitZoomLevelComboBox(hwnd, IDC_ZOOMLEVEL, zoomLevel);
		if (lParam) {
			SetToRightBottom(hwnd);
		} else {
			CenterDlgInParent(hwnd);
		}
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDYES: {
			int zoomLevel;
			if (GetZoomLevelComboBoxValue(hwnd, IDC_ZOOMLEVEL, &zoomLevel)) {
				SciCall_SetZoom(zoomLevel);
			}
			if (LOWORD(wParam) == IDOK) {
				EndDialog(hwnd, IDOK);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

void ZoomLevelDlg(HWND hwnd, BOOL bBottom) {
	ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ZOOMLEVEL), hwnd, ZoomLevelDlgProc, bBottom);
}

extern struct EditAutoCompletionConfig autoCompletionConfig;

static INT_PTR CALLBACK AutoCompletionSettingsDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG: {
		if (autoCompletionConfig.bIndentText) {
			CheckDlgButton(hwnd, IDC_AUTO_INDENT_TEXT, BST_CHECKED);
		}

		if (autoCompletionConfig.bCloseTags) {
			CheckDlgButton(hwnd, IDC_AUTO_CLOSE_TAGS, BST_CHECKED);
		}

		if (autoCompletionConfig.bCompleteWord) {
			CheckDlgButton(hwnd, IDC_AUTO_COMPLETE_WORD, BST_CHECKED);
		}
		if (autoCompletionConfig.bScanWordsInDocument) {
			CheckDlgButton(hwnd, IDC_AUTOC_SCAN_DOCUMENT_WORDS, BST_CHECKED);
		}
		if (autoCompletionConfig.bEnglistIMEModeOnly) {
			CheckDlgButton(hwnd, IDC_AUTOC_ENGLISH_IME_ONLY, BST_CHECKED);
		}

		SetDlgItemInt(hwnd, IDC_AUTOC_VISIBLE_ITEM_COUNT, autoCompletionConfig.iVisibleItemCount, FALSE);
		SendDlgItemMessage(hwnd, IDC_AUTOC_VISIBLE_ITEM_COUNT, EM_LIMITTEXT, 8, 0);

		SetDlgItemInt(hwnd, IDC_AUTOC_MIN_WORD_LENGTH, autoCompletionConfig.iMinWordLength, FALSE);
		SendDlgItemMessage(hwnd, IDC_AUTOC_MIN_WORD_LENGTH, EM_LIMITTEXT, 8, 0);

		SetDlgItemInt(hwnd, IDC_AUTOC_MIN_NUMBER_LENGTH, autoCompletionConfig.iMinNumberLength, FALSE);
		SendDlgItemMessage(hwnd, IDC_AUTOC_MIN_NUMBER_LENGTH, EM_LIMITTEXT, 8, 0);

		int mask = autoCompletionConfig.fAutoCompleteFillUpMask;
		if (mask & AutoCompleteFillUpEnter) {
			CheckDlgButton(hwnd, IDC_AUTOC_FILLUP_ENTER, BST_CHECKED);
		}
		if (mask & AutoCompleteFillUpTab) {
			CheckDlgButton(hwnd, IDC_AUTOC_FILLUP_TAB, BST_CHECKED);
		}
		if (mask & AutoCompleteFillUpSpace) {
			CheckDlgButton(hwnd, IDC_AUTOC_FILLUP_SPACE, BST_CHECKED);
		}
		if (mask & AutoCompleteFillUpPunctuation) {
			CheckDlgButton(hwnd, IDC_AUTOC_FILLUP_PUNCTUATION, BST_CHECKED);
		}

		SetDlgItemText(hwnd, IDC_AUTOC_FILLUP_PUNCTUATION_LIST, autoCompletionConfig.wszAutoCompleteFillUp);
		SendDlgItemMessage(hwnd, IDC_AUTOC_FILLUP_PUNCTUATION_LIST, EM_LIMITTEXT, MAX_AUTO_COMPLETION_FILLUP_LENGTH, 0);

		mask = autoCompletionConfig.fAutoInsertMask;
		if (mask & AutoInsertParenthesis) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_PARENTHESIS, BST_CHECKED);
		}
		if (mask & AutoInsertBrace) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_BRACE, BST_CHECKED);
		}
		if (mask & AutoInsertSquareBracket) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_SQUARE_BRACKET, BST_CHECKED);
		}
		if (mask & AutoInsertAngleBracket) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_ANGLE_BRACKET, BST_CHECKED);
		}
		if (mask & AutoInsertDoubleQuote) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_DOUBLE_QUOTE, BST_CHECKED);
		}
		if (mask & AutoInsertSingleQuote) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_SINGLE_QUOTE, BST_CHECKED);
		}
		if (mask & AutoInsertBacktick) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_BACKTICK, BST_CHECKED);
		}
		if (mask & AutoInsertSpaceAfterComma) {
			CheckDlgButton(hwnd, IDC_AUTO_INSERT_SPACE_COMMA, BST_CHECKED);
		}

		mask = autoCompletionConfig.iAsmLineCommentChar;
		CheckRadioButton(hwnd, IDC_ASM_LINE_COMMENT_SEMICOLON, IDC_ASM_LINE_COMMENT_AT, IDC_ASM_LINE_COMMENT_SEMICOLON + mask);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			autoCompletionConfig.bIndentText = IsButtonChecked(hwnd, IDC_AUTO_INDENT_TEXT);
			autoCompletionConfig.bCloseTags = IsButtonChecked(hwnd, IDC_AUTO_CLOSE_TAGS);

			autoCompletionConfig.bCompleteWord = IsButtonChecked(hwnd, IDC_AUTO_COMPLETE_WORD);
			autoCompletionConfig.bScanWordsInDocument = IsButtonChecked(hwnd, IDC_AUTOC_SCAN_DOCUMENT_WORDS);
			autoCompletionConfig.bEnglistIMEModeOnly = IsButtonChecked(hwnd, IDC_AUTOC_ENGLISH_IME_ONLY);

			int mask = GetDlgItemInt(hwnd, IDC_AUTOC_VISIBLE_ITEM_COUNT, NULL, FALSE);
			autoCompletionConfig.iVisibleItemCount = max_i(mask, MIN_AUTO_COMPLETION_VISIBLE_ITEM_COUNT);

			mask = GetDlgItemInt(hwnd, IDC_AUTOC_MIN_WORD_LENGTH, NULL, FALSE);
			autoCompletionConfig.iMinWordLength = max_i(mask, MIN_AUTO_COMPLETION_WORD_LENGTH);

			mask = GetDlgItemInt(hwnd, IDC_AUTOC_MIN_NUMBER_LENGTH, NULL, FALSE);
			autoCompletionConfig.iMinNumberLength = max_i(mask, MIN_AUTO_COMPLETION_NUMBER_LENGTH);

			mask = 0;
			if (IsButtonChecked(hwnd, IDC_AUTOC_FILLUP_ENTER)) {
				mask |= AutoCompleteFillUpEnter;
			}
			if (IsButtonChecked(hwnd, IDC_AUTOC_FILLUP_TAB)) {
				mask |= AutoCompleteFillUpTab;
			}
			if (IsButtonChecked(hwnd, IDC_AUTOC_FILLUP_SPACE)) {
				mask |= AutoCompleteFillUpSpace;
			}
			if (IsButtonChecked(hwnd, IDC_AUTOC_FILLUP_PUNCTUATION)) {
				mask |= AutoCompleteFillUpPunctuation;
			}

			autoCompletionConfig.fAutoCompleteFillUpMask = mask;
			GetDlgItemText(hwnd, IDC_AUTOC_FILLUP_PUNCTUATION_LIST, autoCompletionConfig.wszAutoCompleteFillUp, COUNTOF(autoCompletionConfig.wszAutoCompleteFillUp));

			mask = 0;
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_PARENTHESIS)) {
				mask |= AutoInsertParenthesis;
			}
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_BRACE)) {
				mask |= AutoInsertBrace;
			}
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_SQUARE_BRACKET)) {
				mask |= AutoInsertSquareBracket;
			}
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_ANGLE_BRACKET)) {
				mask |= AutoInsertAngleBracket;
			}
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_DOUBLE_QUOTE)) {
				mask |= AutoInsertDoubleQuote;
			}
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_SINGLE_QUOTE)) {
				mask |= AutoInsertSingleQuote;
			}
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_BACKTICK)) {
				mask |= AutoInsertBacktick;
			}
			if (IsButtonChecked(hwnd, IDC_AUTO_INSERT_SPACE_COMMA)) {
				mask |= AutoInsertSpaceAfterComma;
			}

			autoCompletionConfig.fAutoInsertMask = mask;
			autoCompletionConfig.iAsmLineCommentChar = GetCheckedRadioButton(hwnd, IDC_ASM_LINE_COMMENT_SEMICOLON, IDC_ASM_LINE_COMMENT_AT) - IDC_ASM_LINE_COMMENT_SEMICOLON;
			EditCompleteUpdateConfig();
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL AutoCompletionSettingsDlg(HWND hwnd) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_AUTOCOMPLETION), hwnd, AutoCompletionSettingsDlgProc, 0);
	return iResult == IDOK;
}

//=============================================================================
//
// InfoBoxDlgProc()
//
//
typedef struct INFOBOX {
	LPWSTR lpstrMessage;
	LPCWSTR lpstrSetting;
	LPCWSTR idiIcon;
	BOOL   bDisableCheckBox;
} INFOBOX, *LPINFOBOX;

typedef const INFOBOX * LPCINFOBOX;

static INT_PTR CALLBACK InfoBoxDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const LPCINFOBOX lpib = (LPCINFOBOX)lParam;

		SendDlgItemMessage(hwnd, IDC_INFOBOXICON, STM_SETICON, (WPARAM)LoadIcon(NULL, lpib->idiIcon), 0);
		SetDlgItemText(hwnd, IDC_INFOBOXTEXT, lpib->lpstrMessage);
		if (lpib->bDisableCheckBox) {
			EnableWindow(GetDlgItem(hwnd, IDC_INFOBOXCHECK), FALSE);
		}
		NP2HeapFree(lpib->lpstrMessage);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_CTLCOLORSTATIC: {
		const DWORD dwId = GetWindowLong((HWND)lParam, GWL_ID);

		if (dwId >= IDC_INFOBOXRECT && dwId <= IDC_INFOBOXTEXT) {
			HDC hdc = (HDC)wParam;
			SetBkMode(hdc, TRANSPARENT);
			return (LONG_PTR)GetSysColorBrush(COLOR_WINDOW);
		}
	}
	break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
		case IDYES:
		case IDNO: {
			LPINFOBOX lpib = (LPINFOBOX)GetWindowLongPtr(hwnd, DWLP_USER);
			if (IsButtonChecked(hwnd, IDC_INFOBOXCHECK)) {
				IniSetBool(INI_SECTION_NAME_SUPPRESSED_MESSAGES, lpib->lpstrSetting, 1);
			}
			EndDialog(hwnd, LOWORD(wParam));
		}
		break;
		}
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
// InfoBox()
//
//
INT_PTR InfoBox(UINT uType, LPCWSTR lpstrSetting, UINT uidMessage, ...) {
	const UINT icon = uType & MB_ICONMASK;
	uType &= MB_TYPEMASK;
	const int iMode = IniGetInt(INI_SECTION_NAME_SUPPRESSED_MESSAGES, lpstrSetting, 0);
	if (StrNotEmpty(lpstrSetting) && iMode == 1) {
		return (uType == MB_YESNO) ? IDYES : IDOK;
	}

	WCHAR wchFormat[512];
	GetString(uidMessage, wchFormat, COUNTOF(wchFormat));

	INFOBOX ib;
	ib.lpstrMessage = (LPWSTR)NP2HeapAlloc(1024 * sizeof(WCHAR));

	va_list va;
	va_start(va, uidMessage);
	wvsprintf(ib.lpstrMessage, wchFormat, va);
	va_end(va);

	ib.lpstrSetting = lpstrSetting;
	ib.idiIcon = (icon == MB_ICONINFORMATION) ? IDI_INFORMATION : ((icon == MB_ICONQUESTION) ? IDI_QUESTION : IDI_EXCLAMATION);
	ib.bDisableCheckBox = StrIsEmpty(szIniFile) || StrIsEmpty(lpstrSetting) || iMode == 2;

	const WORD idDlg = (uType == MB_YESNO) ? IDD_INFOBOX_YESNO : ((uType == MB_OKCANCEL) ? IDD_INFOBOX_OKCANCEL : IDD_INFOBOX_OK);

	HWND hwnd;
	if ((hwnd = GetActiveWindow()) == NULL) {
		hwnd = hwndMain;
	}

	MessageBeep(MB_ICONEXCLAMATION);
	return ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(idDlg), hwnd, InfoBoxDlgProc, (LPARAM)&ib);
}

/*
HKEY_CLASSES_ROOT\*\shell\Notepad2
	(Default)				REG_SZ		Edit with Notepad2
	icon					REG_SZ		Notepad2.exe
	command
		(Default)			REG_SZ		"Notepad2.exe" "%1"

HKEY_CLASSES_ROOT\Applications\Notepad2.exe
	AppUserModelID			REG_SZ		Notepad2 Text Editor
	FriendlyAppName			REG_SZ		Notepad2 Text Editor
	shell\open\command
		(Default)			REG_SZ		"Notepad2.exe" "%1"

HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe
	Debugger				REG_SZ		"Notepad2.exe" /z
*/
extern BOOL fIsElevated;
extern int flagUseSystemMRU;
extern WCHAR g_wchAppUserModelID[64];

enum {
	SystemIntegration_ContextMenu = 1,
	SystemIntegration_JumpList = 2,
	SystemIntegration_ReplaceNotepad = 4,
	SystemIntegration_RestoreNotepad = 8,
};

struct SystemIntegrationInfo {
	LPWSTR lpszText;
	LPWSTR lpszName;
};

#define NP2RegSubKey_ReplaceNotepad	L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\notepad.exe"

int GetSystemIntegrationStatus(struct SystemIntegrationInfo *info) {
	int mask = 0;
	WCHAR tchModule[MAX_PATH];
	GetModuleFileName(NULL, tchModule, COUNTOF(tchModule));

	// context menu
	HKEY hKey;
	LSTATUS status = RegOpenKeyEx(HKEY_CLASSES_ROOT, NP2RegSubKey_ContextMenu, 0, KEY_READ, &hKey);
	if (status == ERROR_SUCCESS) {
		info->lpszText = Registry_GetDefaultString(hKey);
		HKEY hSubKey;
		status = RegOpenKeyEx(hKey, L"command", 0, KEY_READ, &hSubKey);
		if (status == ERROR_SUCCESS) {
			LPWSTR command = Registry_GetDefaultString(hSubKey);
			if (command != NULL) {
				if (StrStrI(command, tchModule) != NULL) {
					mask |= SystemIntegration_ContextMenu;
				}
				NP2HeapFree(command);
			}
			RegCloseKey(hSubKey);
		}
		RegCloseKey(hKey);
	}

	// jump list
	status = RegOpenKeyEx(HKEY_CLASSES_ROOT, NP2RegSubKey_JumpList, 0, KEY_READ, &hKey);
	if (status == ERROR_SUCCESS) {
		info->lpszName = Registry_GetString(hKey, L"FriendlyAppName");
		HKEY hSubKey;
		status = RegOpenKeyEx(hKey, L"shell\\open\\command", 0, KEY_READ, &hSubKey);
		if (status == ERROR_SUCCESS) {
			LPWSTR command = Registry_GetDefaultString(hSubKey);
			if (command != NULL) {
				LPWSTR userId = Registry_GetString(hKey, L"AppUserModelID");
				if (userId != NULL && StrEqual(userId, g_wchAppUserModelID) && StrStrI(command, tchModule) != NULL) {
					mask |= SystemIntegration_JumpList;
				}
				if (userId != NULL) {
					NP2HeapFree(userId);
				}
				NP2HeapFree(command);
			}
			RegCloseKey(hSubKey);
		}
		RegCloseKey(hKey);
	}

	// replace Windows Notepad
	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NP2RegSubKey_ReplaceNotepad, 0, KEY_READ, &hKey);
	if (status == ERROR_SUCCESS) {
		LPWSTR command = Registry_GetString(hKey, L"Debugger");
		if (command != NULL) {
			if (StrStrI(command, tchModule) != NULL) {
				mask |= SystemIntegration_ReplaceNotepad;
			}
			NP2HeapFree(command);
		}
		RegCloseKey(hKey);
	}

	return mask;
}

void UpdateSystemIntegrationStatus(int mask, LPCWSTR lpszText, LPCWSTR lpszName) {
	WCHAR tchModule[MAX_PATH];
	GetModuleFileName(NULL, tchModule, COUNTOF(tchModule));
	WCHAR command[300];
	wsprintf(command, L"\"%s\" \"%%1\"", tchModule);

	// context menu
	// delete the old one: HKEY_CLASSES_ROOT\*\shell\Notepad2.exe
	//Registry_DeleteTree(HKEY_CLASSES_ROOT, NP2RegSubKey_ContextMenu L".exe");
	if (mask & SystemIntegration_ContextMenu) {
		HKEY hSubKey;
		const LSTATUS status = Registry_CreateKey(HKEY_CLASSES_ROOT, NP2RegSubKey_ContextMenu L"\\command", &hSubKey);
		if (status == ERROR_SUCCESS) {
			HKEY hKey;
			RegOpenKeyEx(HKEY_CLASSES_ROOT, NP2RegSubKey_ContextMenu, 0, KEY_WRITE, &hKey);
			Registry_SetDefaultString(hKey, lpszText);
			Registry_SetString(hKey, L"icon", tchModule);
			Registry_SetDefaultString(hSubKey, command);
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);
		}
	} else {
		Registry_DeleteTree(HKEY_CLASSES_ROOT, NP2RegSubKey_ContextMenu);
	}

	// jump list
	if (mask & SystemIntegration_JumpList) {
		HKEY hSubKey;
		const LSTATUS status = Registry_CreateKey(HKEY_CLASSES_ROOT, NP2RegSubKey_JumpList L"\\shell\\open\\command", &hSubKey);
		if (status == ERROR_SUCCESS) {
			HKEY hKey;
			RegOpenKeyEx(HKEY_CLASSES_ROOT, NP2RegSubKey_JumpList, 0, KEY_WRITE, &hKey);
			Registry_SetString(hKey, L"AppUserModelID", g_wchAppUserModelID);
			Registry_SetString(hKey, L"FriendlyAppName", lpszName);
			Registry_SetDefaultString(hSubKey, command);
			RegCloseKey(hKey);
			RegCloseKey(hSubKey);

			if (flagUseSystemMRU != 2) {
				flagUseSystemMRU = 2;
				IniSetInt(INI_SECTION_NAME_FLAGS, L"ShellUseSystemMRU", 1);
			}
		}
	} else {
		Registry_DeleteTree(HKEY_CLASSES_ROOT, NP2RegSubKey_JumpList);
	}

	// replace Windows Notepad
	if (mask & SystemIntegration_ReplaceNotepad) {
		HKEY hKey;
		const LSTATUS status = Registry_CreateKey(HKEY_LOCAL_MACHINE, NP2RegSubKey_ReplaceNotepad, &hKey);
		if (status == ERROR_SUCCESS) {
			wsprintf(command, L"\"%s\" /z", tchModule);
			Registry_SetString(hKey, L"Debugger", command);
			RegCloseKey(hKey);
		}
	} else if (mask & SystemIntegration_RestoreNotepad) {
		Registry_DeleteTree(HKEY_LOCAL_MACHINE, NP2RegSubKey_ReplaceNotepad);
	}
}

static INT_PTR CALLBACK SystemIntegrationDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG: {
		struct SystemIntegrationInfo info = {NULL, NULL};
		const int mask = GetSystemIntegrationStatus(&info);
		SetWindowLongPtr(hwnd, DWLP_USER, mask);

		HWND hwndCtl = GetDlgItem(hwnd, IDC_CONTEXT_MENU_TEXT);
		if (StrIsEmpty(info.lpszText)) {
			WCHAR wch[128];
			GetString(IDS_LINKDESCRIPTION, wch, COUNTOF(wch));
			Edit_SetText(hwndCtl, wch);
		} else {
			Edit_SetText(hwndCtl, info.lpszText);
		}

		HWND hwndName = GetDlgItem(hwnd, IDC_APPLICATION_NAME);
		Edit_SetText(hwndName, StrIsEmpty(info.lpszName)? g_wchAppUserModelID : info.lpszName);
		if (info.lpszText) {
			NP2HeapFree(info.lpszText);
		}
		if (info.lpszName) {
			NP2HeapFree(info.lpszName);
		}

		if (mask & SystemIntegration_ContextMenu) {
			CheckDlgButton(hwnd, IDC_ENABLE_CONTEXT_MENU, BST_CHECKED);
		}
		if (mask & SystemIntegration_JumpList) {
			CheckDlgButton(hwnd, IDC_ENABLE_JUMP_LIST, BST_CHECKED);
		}
		if (mask & SystemIntegration_ReplaceNotepad) {
			CheckDlgButton(hwnd, IDC_REPLACE_WINDOWS_NOTEPAD, BST_CHECKED);
		}

		if (IsVistaAndAbove() && !fIsElevated) {
			EnableWindow(GetDlgItem(hwnd, IDC_ENABLE_CONTEXT_MENU), FALSE);
			Edit_SetReadOnly(hwndCtl, TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_ENABLE_JUMP_LIST), FALSE);
			Edit_SetReadOnly(hwndName, TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_REPLACE_WINDOWS_NOTEPAD), FALSE);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			if (IsWindowEnabled(GetDlgItem(hwnd, IDC_ENABLE_CONTEXT_MENU))) {
				int mask = 0;
				if (IsButtonChecked(hwnd, IDC_ENABLE_CONTEXT_MENU)) {
					mask |= SystemIntegration_ContextMenu;
				}
				if (IsButtonChecked(hwnd, IDC_ENABLE_JUMP_LIST)) {
					mask |= SystemIntegration_JumpList;
				}
				if (IsButtonChecked(hwnd, IDC_REPLACE_WINDOWS_NOTEPAD)) {
					mask |= SystemIntegration_ReplaceNotepad;
				} else {
					// don't remove third party Notepad replacement.
					const LONG_PTR prev = GetWindowLongPtr(hwnd, DWLP_USER);
					if (prev & SystemIntegration_ReplaceNotepad) {
						mask |= SystemIntegration_RestoreNotepad;
					}
				}

				WCHAR wchText[128];
				GetDlgItemText(hwnd, IDC_CONTEXT_MENU_TEXT, wchText, COUNTOF(wchText));
				TrimString(wchText);

				WCHAR wchName[128];
				GetDlgItemText(hwnd, IDC_APPLICATION_NAME, wchName, COUNTOF(wchName));
				TrimString(wchName);

				UpdateSystemIntegrationStatus(mask, wchText, wchName);
			}
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

void SystemIntegrationDlg(HWND hwnd) {
	ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_SYSTEM_INTEGRATION), hwnd, SystemIntegrationDlgProc, 0);
}
