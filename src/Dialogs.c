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
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include "Scintilla.h"
#include "Notepad2.h"
#include "Edit.h"
#include "Helpers.h"
#include "Dlapi.h"
#include "Dialogs.h"
#include "resource.h"
#include "Version.h"

extern HWND		hwndMain;
extern DWORD	dwLastIOError;
extern BOOL		bSkipUnicodeDetection;
extern BOOL		bLoadASCIIasUTF8;
extern BOOL		bLoadNFOasOEM;
extern int		fNoFileVariables;
extern BOOL		bNoEncodingTags;
extern BOOL		bWarnLineEndings;
extern BOOL		bFixLineEndings;
extern BOOL		bAutoStripBlanks;
extern WCHAR	szCurFile[MAX_PATH + 40];

//=============================================================================
//
// MsgBox()
//
int MsgBox(int iType, UINT uIdMsg, ...) {
	WCHAR szBuf[1024];
	WCHAR szText[1024];

	GetString(uIdMsg, szBuf, COUNTOF(szBuf));

	va_list va;
	va_start(va, uIdMsg);
	wvsprintf(szText, szBuf, va);
	va_end(va);

	if (uIdMsg == IDS_ERR_LOADFILE || uIdMsg == IDS_ERR_SAVEFILE ||
			uIdMsg == IDS_CREATEINI_FAIL || uIdMsg == IDS_WRITEINI_FAIL ||
			uIdMsg == IDS_EXPORT_FAIL) {
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwLastIOError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&lpMsgBuf,
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

	WCHAR szTitle[64];
	GetString(IDS_APPTITLE, szTitle, COUNTOF(szTitle));

	int iIcon = 0;
	switch (iType) {
	case MBINFO:
		iIcon = MB_ICONINFORMATION;
		break;
	case MBWARN:
		iIcon = MB_ICONEXCLAMATION;
		break;
	case MBYESNO:
		iIcon = MB_ICONEXCLAMATION | MB_YESNO;
		break;
	case MBYESNOCANCEL:
		iIcon = MB_ICONEXCLAMATION | MB_YESNOCANCEL;
		break;
	case MBYESNOWARN:
		iIcon = MB_ICONEXCLAMATION | MB_YESNO;
		break;
	case MBOKCANCEL:
		iIcon = MB_ICONEXCLAMATION | MB_OKCANCEL;
		break;
	}

	HWND hwnd;
	if ((hwnd = GetActiveWindow()) == NULL) {
		hwnd = hwndMain;
	}

	PostMessage(hwndMain, APPM_CENTER_MESSAGE_BOX, (WPARAM)hwnd, 0);
	return MessageBoxEx(hwnd, szText, szTitle,
						MB_SETFOREGROUND | iIcon,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
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
	mbp.dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

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
		CoTaskMemFree((LPVOID)pidl);
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
		SetDlgItemText(hwnd, IDC_VERSION, VERSION_FILEVERSION_LONG);
		SetDlgItemText(hwnd, IDC_BUILD_INFO, VERSION_BUILD_INFO);
		SetDlgItemText(hwnd, IDC_COPYRIGHT, VERSION_LEGALCOPYRIGHT_SHORT);
		SetDlgItemText(hwnd, IDC_AUTHORNAME, VERSION_AUTHORNAME);

		HFONT hFontTitle = (HFONT)SendDlgItemMessage(hwnd, IDC_VERSION, WM_GETFONT, 0, 0);
		if (hFontTitle == NULL) {
			hFontTitle = GetStockObject(DEFAULT_GUI_FONT);
		}

		LOGFONT lf;
		GetObject(hFontTitle, sizeof(LOGFONT), &lf);
		lf.lfWeight = FW_BOLD;
		hFontTitle = CreateFontIndirect(&lf);
		SendDlgItemMessage(hwnd, IDC_VERSION, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
		SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)(hFontTitle));

		WCHAR wch[256];
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
static INT_PTR CALLBACK RunDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		MakeBitmapButton(hwnd, IDC_SEARCHEXE, g_hInstance, IDB_OPEN);

		SendDlgItemMessage(hwnd, IDC_COMMANDLINE, EM_LIMITTEXT, MAX_PATH - 1, 0);
		SetDlgItemText(hwnd, IDC_COMMANDLINE, (LPCWSTR)lParam);
		SHAutoComplete(GetDlgItem(hwnd, IDC_COMMANDLINE), SHACF_FILESYSTEM);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		DeleteBitmapButton(hwnd, IDC_SEARCHEXE);
		return FALSE;

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
						| OFN_PATHMUSTEXIST | OFN_SHAREAWARE | OFN_NODEREFERENCELINKS;

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
					sei.fMask |= /*SEE_MASK_NOZONECHECKS*/0x00800000;
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

		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT,  LVCFMT_LEFT,  0,  L"",  -1,  0,  0,  0 };
		ResizeDlg_Init(hwnd, cxOpenWithDlg, cyOpenWithDlg, IDC_RESIZEGRIP3);

		HWND hwndLV = GetDlgItem(hwnd, IDC_OPENWITHDIR);
		InitWindowCommon(hwndLV);
		//SetExplorerTheme(hwndLV);
		ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);

		ListView_InsertColumn(hwndLV, 0, &lvc);
		DirList_Init(hwndLV, NULL);
		DirList_Fill(hwndLV, tchOpenWithDir, DL_ALLOBJECTS, NULL, FALSE, flagNoFadeHidden, DS_NAME, FALSE);
		DirList_StartIconThread(hwndLV);
		ListView_SetItemState(hwndLV, 0, LVIS_FOCUSED, LVIS_FOCUSED);

		MakeBitmapButton(hwnd, IDC_GETOPENWITHDIR, g_hInstance, IDB_OPEN);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		DirList_Destroy(GetDlgItem(hwnd, IDC_OPENWITHDIR));
		DeleteBitmapButton(hwnd, IDC_GETOPENWITHDIR);
		ResizeDlg_Destroy(hwnd, &cxOpenWithDlg, &cyOpenWithDlg);
		return FALSE;

	case WM_SIZE: {
		int dx, dy;

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
				NM_LISTVIEW *pnmlv = (NM_LISTVIEW *)lParam;
				EnableWindow(GetDlgItem(hwnd, IDOK), (pnmlv->uNewState & LVIS_SELECTED));
			}
			break;

			case NM_DBLCLK:
				if (ListView_GetSelectedCount(GetDlgItem(hwnd, IDC_OPENWITHDIR))) {
					SendMessage(hwnd, WM_COMMAND, MAKELONG(IDOK, 1), 0);
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
				MessageBeep(0);
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

static INT_PTR CALLBACK FavoritesDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);

		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT,  LVCFMT_LEFT,  0,  L"",  -1,  0,  0,  0 };

		ResizeDlg_Init(hwnd, cxFavoritesDlg, cyFavoritesDlg, IDC_RESIZEGRIP3);

		HWND hwndLV = GetDlgItem(hwnd, IDC_FAVORITESDIR);
		InitWindowCommon(hwndLV);
		//SetExplorerTheme(hwndLV);
		ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
		ListView_InsertColumn(hwndLV, 0, &lvc);
		DirList_Init(hwndLV, NULL);
		DirList_Fill(hwndLV, tchFavoritesDir, DL_ALLOBJECTS, NULL, FALSE, flagNoFadeHidden, DS_NAME, FALSE);
		DirList_StartIconThread(hwndLV);
		ListView_SetItemState(hwndLV, 0, LVIS_FOCUSED, LVIS_FOCUSED);

		MakeBitmapButton(hwnd, IDC_GETFAVORITESDIR, g_hInstance, IDB_OPEN);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		DirList_Destroy(GetDlgItem(hwnd, IDC_FAVORITESDIR));
		DeleteBitmapButton(hwnd, IDC_GETFAVORITESDIR);

		ResizeDlg_Destroy(hwnd, &cxFavoritesDlg, &cyFavoritesDlg);
		return FALSE;

	case WM_SIZE: {
		int dx, dy;

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
				NM_LISTVIEW *pnmlv = (NM_LISTVIEW *)lParam;
				EnableWindow(GetDlgItem(hwnd, IDOK), (pnmlv->uNewState & LVIS_SELECTED));
			}
			break;

			case NM_DBLCLK:
				if (ListView_GetSelectedCount(GetDlgItem(hwnd, IDC_FAVORITESDIR))) {
					SendMessage(hwnd, WM_COMMAND, MAKELONG(IDOK, 1), 0);
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
				MessageBeep(0);
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
// Controls: 100 Edit
//
static INT_PTR CALLBACK AddToFavDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		LPWSTR pszName = (LPWSTR)lParam;

		SendDlgItemMessage(hwnd, 100, EM_LIMITTEXT, MAX_PATH - 1, 0);
		SetDlgItemText(hwnd, 100, pszName);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 100:
			EnableWindow(GetDlgItem(hwnd, IDOK),  GetWindowTextLength(GetDlgItem(hwnd, 100)));
			break;

		case IDOK: {
			LPWSTR pszName = (LPWSTR)GetWindowLongPtr(hwnd, DWLP_USER);
			GetDlgItemText(hwnd, 100, pszName,  MAX_PATH - 1);
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
			MsgBox(MBINFO, IDS_FAV_SUCCESS);
			return TRUE;
		}
		MsgBox(MBWARN, IDS_FAV_FAILURE);
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

typedef struct tagIconThreadInfo {
	HWND hwnd;					// HWND of ListView Control
	HANDLE hExitThread;			// Flag is set when Icon Thread should terminate
	HANDLE hTerminatedThread;	// Flag is set when Icon Thread has terminated
	HANDLE hFileMRUIconThread;
} ICONTHREADINFO,  *LPICONTHREADINFO;

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
			if (PathIsUNC(tch) || !PathFileExists(tch)) {
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

		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT,  LVCFMT_LEFT,  0,  L"",  -1,  0,  0,  0 };
		HWND hwndLV = GetDlgItem(hwnd, IDC_FILEMRU);
		InitWindowCommon(hwndLV);

		LPICONTHREADINFO lpit = (LPVOID)GlobalAlloc(GPTR, sizeof(ICONTHREADINFO));
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
		ListView_InsertColumn(hwndLV, 0, &lvc);

		// Update view
		SendMessage(hwnd, WM_COMMAND, MAKELONG(0x00A0, 1), 0);

		if (bSaveRecentFiles) {
			CheckDlgButton(hwnd, IDC_SAVEMRU, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY: {
		LPICONTHREADINFO lpit = (LPVOID)GetProp(hwnd, L"it");
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
		int dx, dy;

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
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDOK, 1), 0);
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
					if (!PathFileExists(tch)) {
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
				SendMessage(hwnd, WM_COMMAND, MAKELONG(0x00A0, 1), 0);
			}
		}
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0x00A0: {
			LPICONTHREADINFO lpit = (LPVOID)GetProp(hwnd, L"it");

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

			ListView_DeleteAllItems(GetDlgItem(hwnd, IDC_FILEMRU));

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
				//	SendDlgItemMessage(hwnd, IDC_FILEMRU, LB_ADDSTRING, 0, (LPARAM)tch); }
				//	SendDlgItemMessage(hwnd, IDC_FILEMRU, LB_SETCARETINDEX, 0, FALSE);
				lvi.iItem = i;
				lvi.pszText = tch;
				ListView_InsertItem(GetDlgItem(hwnd, IDC_FILEMRU), &lvi);
			}

			ListView_SetItemState(GetDlgItem(hwnd, IDC_FILEMRU), 0, LVIS_FOCUSED, LVIS_FOCUSED);
			ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_FILEMRU), 0, LVSCW_AUTOSIZE_USEHEADER);

			DWORD dwtid;
			lpit->hFileMRUIconThread = CreateThread(NULL, 0, FileMRUIconThread, (LPVOID)lpit, 0, &dwtid);
		}
		break;

		case IDC_FILEMRU:
			break;

		case IDOK: {
			WCHAR tch[MAX_PATH];
			//int iItem;

			//if ((iItem = SendDlgItemMessage(hwnd, IDC_FILEMRU, LB_GETCURSEL, 0, 0)) != LB_ERR)
			if (ListView_GetSelectedCount(GetDlgItem(hwnd, IDC_FILEMRU))) {
				//SendDlgItemMessage(hwnd, IDC_FILEMRU, LB_GETTEXT, iItem, (LPARAM)tch);
				LV_ITEM lvi;
				ZeroMemory(&lvi, sizeof(LV_ITEM));

				lvi.mask = LVIF_TEXT;
				lvi.pszText = tch;
				lvi.cchTextMax = COUNTOF(tch);
				lvi.iItem = ListView_GetNextItem(GetDlgItem(hwnd, IDC_FILEMRU), -1, LVNI_ALL | LVNI_SELECTED);

				ListView_GetItem(GetDlgItem(hwnd, IDC_FILEMRU), &lvi);

				PathUnquoteSpaces(tch);

				if (!PathFileExists(tch)) {
					// Ask...
					if (IDYES == MsgBox(MBYESNO, IDS_ERR_MRUDLG)) {

						MRU_Delete(pFileMRU, lvi.iItem);
						MRU_DeleteFileFromStore(pFileMRU, tch);

						//SendDlgItemMessage(hwnd, IDC_FILEMRU, LB_DELETESTRING, iItem, 0);
						//ListView_DeleteItem(GetDlgItem(hwnd, IDC_FILEMRU), lvi.iItem);
						// must use IDM_VIEW_REFRESH,  index might change...
						SendMessage(hwnd, WM_COMMAND, MAKELONG(0x00A0, 1), 0);

						//EnableWindow(GetDlgItem(hwnd, IDOK),
						//	(LB_ERR != SendDlgItemMessage(hwnd, IDC_GOTO, LB_GETCURSEL, 0, 0)));

						EnableWindow(GetDlgItem(hwnd, IDOK),
									 ListView_GetSelectedCount(GetDlgItem(hwnd, IDC_FILEMRU)));
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
// Controls: 100 Radio Button
// 101 Radio Button
// 102 Radio Button
// 103 Check Box
//
extern int iFileWatchingMode;
extern BOOL bResetFileWatching;

static INT_PTR CALLBACK ChangeNotifyDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG:
		CheckRadioButton(hwnd, 100, 102, 100 + iFileWatchingMode);
		if (bResetFileWatching) {
			CheckDlgButton(hwnd, 103, BST_CHECKED);
		}
		CenterDlgInParent(hwnd);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (IsButtonChecked(hwnd, 100)) {
				iFileWatchingMode = 0;
			} else if (IsButtonChecked(hwnd, 101)) {
				iFileWatchingMode = 1;
			} else {
				iFileWatchingMode = 2;
			}
			bResetFileWatching = IsButtonChecked(hwnd, 103);
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
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance,  MAKEINTRESOURCE(IDD_CHANGENOTIFY), hwnd, ChangeNotifyDlgProc, 0);
	return iResult == IDOK;
}

//=============================================================================
//
// ColumnWrapDlgProc()
//
// Controls: 100 Edit
//
static INT_PTR CALLBACK ColumnWrapDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const int iNumber = *((int *)lParam);

		SetDlgItemInt(hwnd, 100, iNumber, FALSE);
		SendDlgItemMessage(hwnd, 100, EM_LIMITTEXT, 15, 0);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated;
			const int iNewNumber = GetDlgItemInt(hwnd, 100, &fTranslated, FALSE);

			if (fTranslated) {
				int *piNumber = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
				*piNumber = iNewNumber;

				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, 100)), 1);
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
// Controls: 100 Combo
// 101 Combo
// 102 Combo
// 103 Combo
// 200 Text
// 201 Text
// 202 Text
// 203 Text
//
extern BOOL fWordWrap;
extern int iWordWrapMode;
extern int iWordWrapIndent;
extern int iWordWrapSymbols;
extern BOOL bShowWordWrapSymbols;

static INT_PTR CALLBACK WordWrapSettingsDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG: {
		WCHAR tch[512];
		for (int i = 0; i < 4; i++) {
			WCHAR *p1,  *p2;
			GetDlgItemText(hwnd, 200 + i, tch, COUNTOF(tch));
			lstrcat(tch, L"|");
			p1 = tch;
			while ((p2 = StrChr(p1, L'|')) != NULL) {
				*p2++ = L'\0';
				if (*p1) {
					SendDlgItemMessage(hwnd, 100 + i, CB_ADDSTRING, 0, (LPARAM)p1);
				}
				p1 = p2;
			}

			SendDlgItemMessage(hwnd, 100 + i, CB_SETEXTENDEDUI, TRUE, 0);
		}

		SendDlgItemMessage(hwnd, 100, CB_SETCURSEL, iWordWrapIndent, 0);
		SendDlgItemMessage(hwnd, 101, CB_SETCURSEL, bShowWordWrapSymbols ? iWordWrapSymbols % 10 : 0, 0);
		SendDlgItemMessage(hwnd, 102, CB_SETCURSEL, bShowWordWrapSymbols ? ((iWordWrapSymbols % 100) - (iWordWrapSymbols % 10)) / 10 : 0, 0);
		SendDlgItemMessage(hwnd, 103, CB_SETCURSEL, iWordWrapMode, 0);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			iWordWrapIndent = (int)SendDlgItemMessage(hwnd, 100, CB_GETCURSEL, 0, 0);
			bShowWordWrapSymbols = FALSE;
			int iSel = (int)SendDlgItemMessage(hwnd, 101, CB_GETCURSEL, 0, 0);
			const int iSel2 = (int)SendDlgItemMessage(hwnd, 102, CB_GETCURSEL, 0, 0);
			if (iSel > 0 || iSel2 > 0) {
				bShowWordWrapSymbols = TRUE;
				iWordWrapSymbols = iSel + iSel2 * 10;
			}

			iSel = (int)SendDlgItemMessage(hwnd, 103, CB_GETCURSEL, 0, 0);
			if (iSel == SC_WRAP_NONE) {
				fWordWrap = FALSE;
			} else {
				fWordWrap = TRUE;
				iWordWrapMode = iSel;
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
// Controls: 100 Edit
// 101 Radio1
// 102 Radio2
//
extern int iLongLineMode;

static INT_PTR CALLBACK LongLineSettingsDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const int iNumber = *((int *)lParam);

		SetDlgItemInt(hwnd, 100, iNumber, FALSE);
		SendDlgItemMessage(hwnd, 100, EM_LIMITTEXT, 15, 0);

		if (iLongLineMode == EDGE_LINE) {
			CheckRadioButton(hwnd, 101, 102, 101);
		} else {
			CheckRadioButton(hwnd, 101, 102, 102);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated;
			const int iNewNumber = GetDlgItemInt(hwnd, 100, &fTranslated, FALSE);

			if (fTranslated) {
				int *piNumber = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
				*piNumber = iNewNumber;
				iLongLineMode = IsButtonChecked(hwnd, 101) ? EDGE_LINE : EDGE_BACKGROUND;

				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, 100)), 1);
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
// Controls: 100 Edit
// 101 Edit
// 102 Check
// 103 Check
// 104 Check
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
		SetDlgItemInt(hwnd, 100, iTabWidth, FALSE);
		SendDlgItemMessage(hwnd, 100, EM_LIMITTEXT, 15, 0);

		SetDlgItemInt(hwnd, 101, iIndentWidth, FALSE);
		SendDlgItemMessage(hwnd, 101, EM_LIMITTEXT, 15, 0);

		if (bTabsAsSpaces) {
			CheckDlgButton(hwnd, 102, BST_CHECKED);
		}

		if (bTabIndents) {
			CheckDlgButton(hwnd, 103, BST_CHECKED);
		}

		if (bBackspaceUnindents) {
			CheckDlgButton(hwnd, 104, BST_CHECKED);
		}

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated1, fTranslated2;

			const int iNewTabWidth = GetDlgItemInt(hwnd, 100, &fTranslated1, FALSE);
			const int iNewIndentWidth = GetDlgItemInt(hwnd, 101, &fTranslated2, FALSE);

			if (fTranslated1 && fTranslated2) {
				iTabWidth = iNewTabWidth;
				iIndentWidth = iNewIndentWidth;

				bTabsAsSpaces = IsButtonChecked(hwnd, 102);
				bTabIndents = IsButtonChecked(hwnd, 103);
				bBackspaceUnindents = IsButtonChecked(hwnd, 104);

				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, fTranslated1 ? 101 : 100)), 1);
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
typedef struct encodedlg {
	BOOL bRecodeOnly;
	int  idEncoding;
	int  cxDlg;
	int  cyDlg;
} ENCODEDLG,  *PENCODEDLG;

static INT_PTR CALLBACK SelectDefEncodingDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		PENCODEDLG pdd = (PENCODEDLG)lParam;

		HBITMAP hbmp = LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_ENCODING), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		HIMAGELIST himl = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 0);
		ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
		DeleteObject(hbmp);

		HWND hwndLV = GetDlgItem(hwnd, IDC_ENCODINGLIST);
		SendMessage(hwndLV, CBEM_SETIMAGELIST, 0, (LPARAM)himl);
		SendMessage(hwndLV, CB_SETEXTENDEDUI, TRUE, 0);
		Encoding_AddToComboboxEx(hwndLV, pdd->idEncoding, 0);

		if (bSkipUnicodeDetection) {
			CheckDlgButton(hwnd, IDC_NOUNICODEDETECTION, BST_CHECKED);
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

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			PENCODEDLG pdd = (PENCODEDLG)GetWindowLongPtr(hwnd, DWLP_USER);
			if (Encoding_GetFromComboboxEx(GetDlgItem(hwnd, IDC_ENCODINGLIST), &pdd->idEncoding)) {
				bSkipUnicodeDetection = IsButtonChecked(hwnd, IDC_NOUNICODEDETECTION);
				bLoadASCIIasUTF8 = IsButtonChecked(hwnd, IDC_ASCIIASUTF8);
				bLoadNFOasOEM = IsButtonChecked(hwnd, IDC_NFOASOEM);
				bNoEncodingTags = IsButtonChecked(hwnd, IDC_ENCODINGFROMFILEVARS);
				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_ENCODINGLIST)), 1);
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
// SelectDefEncodingDlg()
//
BOOL SelectDefEncodingDlg(HWND hwnd, int *pidREncoding) {
	ENCODEDLG dd;

	dd.bRecodeOnly = FALSE;
	dd.idEncoding = *pidREncoding;

	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_DEFENCODING), hwnd, SelectDefEncodingDlgProc, (LPARAM)&dd);

	if (iResult == IDOK) {
		*pidREncoding = dd.idEncoding;
		return TRUE;
	}
	return FALSE;
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
		PENCODEDLG pdd = (PENCODEDLG)lParam;

		LVCOLUMN lvc = { LVCF_FMT | LVCF_TEXT,  LVCFMT_LEFT,  0,  L"",  -1,  0,  0,  0 };

		ResizeDlg_Init(hwnd, pdd->cxDlg, pdd->cyDlg, IDC_RESIZEGRIP4);

		HBITMAP hbmp = LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_ENCODING), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		HIMAGELIST himl = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 0);
		ImageList_AddMasked(himl, hbmp, CLR_DEFAULT);
		DeleteObject(hbmp);
		ListView_SetImageList(GetDlgItem(hwnd, IDC_ENCODINGLIST), himl, LVSIL_SMALL);

		HWND hwndLV = GetDlgItem(hwnd, IDC_ENCODINGLIST);
		InitWindowCommon(hwndLV);
		//SetExplorerTheme(hwndLV);
		ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
		ListView_InsertColumn(hwndLV, 0, &lvc);
		Encoding_AddToListView(hwndLV, pdd->idEncoding, pdd->bRecodeOnly);
		ListView_SetColumnWidth(hwndLV, 0, LVSCW_AUTOSIZE_USEHEADER);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY: {
		PENCODEDLG pdd = (PENCODEDLG)GetWindowLongPtr(hwnd, DWLP_USER);
		ResizeDlg_Destroy(hwnd, &pdd->cxDlg, &pdd->cyDlg);
	}
	return FALSE;

	case WM_SIZE: {
		int dx, dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);

		HDWP hdwp = BeginDeferWindowPos(4);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP4, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_ENCODINGLIST, dx, dy, SWP_NOMOVE);
		EndDeferWindowPos(hdwp);
		ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_ENCODINGLIST), 0, LVSCW_AUTOSIZE_USEHEADER);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NOTIFY: {
		if (((LPNMHDR)(lParam))->idFrom == IDC_ENCODINGLIST) {
			switch (((LPNMHDR)(lParam))->code) {
			case NM_DBLCLK:
				SendMessage(hwnd, WM_COMMAND, MAKELONG(IDOK, 1), 0);
				break;

			case LVN_ITEMCHANGED:
			case LVN_DELETEITEM: {
				const int i = ListView_GetNextItem(GetDlgItem(hwnd, IDC_ENCODINGLIST), -1, LVNI_ALL | LVNI_SELECTED);
				EnableWindow(GetDlgItem(hwnd, IDOK), i != -1);
			}
			break;
			}
		}
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			PENCODEDLG pdd = (PENCODEDLG)GetWindowLongPtr(hwnd, DWLP_USER);
			if (Encoding_GetFromListView(GetDlgItem(hwnd, IDC_ENCODINGLIST), &pdd->idEncoding)) {
				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hwnd, IDC_ENCODINGLIST), 1);
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

BOOL SelectEncodingDlg(HWND hwnd, int *pidREncoding) {
	ENCODEDLG dd;

	dd.bRecodeOnly = FALSE;
	dd.idEncoding = *pidREncoding;
	dd.cxDlg = cxEncodingDlg;
	dd.cyDlg = cyEncodingDlg;

	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ENCODING), hwnd, SelectEncodingDlgProc, (LPARAM)&dd);

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
// RecodeDlg()
//
extern int cxRecodeDlg;
extern int cyRecodeDlg;

BOOL RecodeDlg(HWND hwnd, int *pidREncoding) {
	ENCODEDLG dd;

	dd.bRecodeOnly = TRUE;
	dd.idEncoding = *pidREncoding;
	dd.cxDlg = cxRecodeDlg;
	dd.cyDlg = cyRecodeDlg;

	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_RECODE), hwnd, SelectEncodingDlgProc, (LPARAM)&dd);

	cxRecodeDlg = dd.cxDlg;
	cyRecodeDlg = dd.cyDlg;

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
		WCHAR wch[128];
		for (int i = 0; i < 3; i++) {
			GetString(IDS_EOLMODENAME_CRLF + i, wch, COUNTOF(wch));
			SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_ADDSTRING, 0, (LPARAM)wch);
		}

		SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_SETCURSEL, iOption, 0);
		SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_SETEXTENDEDUI, TRUE, 0);

		if (bWarnLineEndings) {
			CheckDlgButton(hwnd, IDC_WARNINCONSISTENTEOLS, BST_CHECKED);
		}

		if (bFixLineEndings) {
			CheckDlgButton(hwnd, IDC_CONSISTENTEOLS, BST_CHECKED);
		}

		if (bAutoStripBlanks) {
			CheckDlgButton(hwnd, IDC_AUTOSTRIPBLANKS,  BST_CHECKED);
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

static INT_PTR CALLBACK WarnLineEndingDlgDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		EditFileIOStatus *status = (EditFileIOStatus *)lParam;
		int iEOLMode = status->iEOLMode;
		iEOLMode = (iEOLMode == SC_EOL_CRLF) ? 0 : (iEOLMode == SC_EOL_LF) ? 1 : 2;

		// Load options
		WCHAR wch[128];
		for (int i = 0; i < 3; i++) {
			GetString(IDS_EOLMODENAME_CRLF + i, wch, COUNTOF(wch));
			SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_ADDSTRING, 0, (LPARAM)wch);
		}

		SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_SETCURSEL, iEOLMode, 0);
		SendDlgItemMessage(hwnd, IDC_EOLMODELIST, CB_SETEXTENDEDUI, TRUE, 0);

		WCHAR tchFmt[128];
		for (int i = 0; i < 3; i++) {
			WCHAR tchLn[32];
			wsprintf(tchLn, L"%u", status->linesCount[i]);
			FormatNumberStr(tchLn);
			GetDlgItemText(hwnd, IDC_EOL_SUM_CRLF + i, tchFmt, COUNTOF(tchFmt));
			wsprintf(wch, tchFmt, tchLn);
			SetDlgItemText(hwnd, IDC_EOL_SUM_CRLF + i, wch);
		}

		if (bWarnLineEndings) {
			CheckDlgButton(hwnd, IDC_WARNINCONSISTENTEOLS, BST_CHECKED);
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
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WarnLineEndingDlg(HWND hwnd, struct EditFileIOStatus *status) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_WARNLINEENDS), hwnd, WarnLineEndingDlgDlgProc, (LPARAM)status);
	return iResult == IDOK;
}

//=============================================================================
//
// InfoBoxDlgProc()
//
//
typedef struct _infobox {
	LPWSTR lpstrMessage;
	LPWSTR lpstrSetting;
	BOOL   bDisableCheckBox;
} INFOBOX, *LPINFOBOX;

static INT_PTR CALLBACK InfoBoxDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		LPINFOBOX lpib = (LPINFOBOX)lParam;

		SendDlgItemMessage(hwnd, IDC_INFOBOXICON, STM_SETICON, (WPARAM)LoadIcon(NULL, IDI_EXCLAMATION), 0);
		SetDlgItemText(hwnd, IDC_INFOBOXTEXT, lpib->lpstrMessage);
		if (lpib->bDisableCheckBox) {
			EnableWindow(GetDlgItem(hwnd, IDC_INFOBOXCHECK), FALSE);
		}
		NP2HeapFree(lpib->lpstrMessage);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

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
INT_PTR InfoBox(int iType, LPCWSTR lpstrSetting, int uidMessage, ...) {
	const int iMode = IniGetInt(INI_SECTION_NAME_SUPPRESSED_MESSAGES, lpstrSetting, 0);
	if (StrNotEmpty(lpstrSetting) && iMode == 1) {
		return (iType == MBYESNO) ? IDYES : IDOK;
	}

	WCHAR wchFormat[512];
	GetString(uidMessage, wchFormat, COUNTOF(wchFormat));

	INFOBOX ib;
	ib.lpstrMessage = NP2HeapAlloc(1024 * sizeof(WCHAR));

	va_list va;
	va_start(va, uidMessage);
	wvsprintf(ib.lpstrMessage, wchFormat, va);
	va_end(va);

	ib.lpstrSetting = (LPWSTR)lpstrSetting;
	ib.bDisableCheckBox = StrIsEmpty(szIniFile) || StrIsEmpty(lpstrSetting) || iMode == 2;

	const WORD idDlg = (iType == MBYESNO) ? IDD_INFOBOX2 : ((iType == MBOKCANCEL) ? IDD_INFOBOX3 : IDD_INFOBOX);

	HWND hwnd;
	if ((hwnd = GetActiveWindow()) == NULL) {
		hwnd = hwndMain;
	}

	MessageBeep(MB_ICONEXCLAMATION);

	return ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(idDlg), hwnd, InfoBoxDlgProc, (LPARAM)&ib);
}

// End of Dialogs.c
