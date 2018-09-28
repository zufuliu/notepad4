/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* Helpers.c
*   General helper functions
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
#include <commctrl.h>
#include <uxtheme.h>
#include <psapi.h>
#include <stdio.h>
#include "Dlapi.h"
#include "Helpers.h"
#include "resource.h"

//=============================================================================
//
//  Manipulation of (cached) ini file sections
//
BOOL IniSectionParseArray(IniSection *section, LPWSTR lpCachedIniSection) {
	IniSectionClear(section);
	if (StrIsEmpty(lpCachedIniSection)) {
		return FALSE;
	}

	const int capacity = section->capacity;
	LPWSTR p = lpCachedIniSection;
	int count = 0;

	do {
		IniKeyValueNode *node = &section->nodeList[count];
		LPWSTR v = StrChr(p, L'=');
		*v++ = L'\0';
		node->key = p;
		node->value = v;
		++count;
		p = StrEnd(v) + 1;
	} while (*p && count < capacity);

	section->count = count;
	return TRUE;
}

BOOL IniSectionParse(IniSection *section, LPWSTR lpCachedIniSection) {
	IniSectionClear(section);
	if (StrIsEmpty(lpCachedIniSection)) {
		return FALSE;
	}

	const int capacity = section->capacity;
	LPWSTR p = lpCachedIniSection;
	int count = 0;

	do {
		IniKeyValueNode *node = &section->nodeList[count];
		LPWSTR v = StrChr(p, L'=');
		*v++ = L'\0';
		const UINT keyLen = (UINT)(v - p - 1);
		node->hash = keyLen | (p[0] << 8) | (p[1] << 16);
		node->key = p;
		node->value = v;
		++count;
		p = StrEnd(v) + 1;
	} while (*p && count < capacity);

	section->count = count;
	section->head = &section->nodeList[0];
	--count;
#if IniSectionImplUseSentinelNode
	section->nodeList[count].next = section->sentinel;
#else
	section->nodeList[count].next = NULL;
#endif
	while (count != 0) {
		section->nodeList[count - 1].next = &section->nodeList[count];
		--count;
	}
	return TRUE;
}

LPCWSTR IniSectionUnsafeGetValue(IniSection *section, LPCWSTR key, int keyLen) {
	if (keyLen < 0) {
		keyLen = lstrlen(key);
	}

	const UINT hash = keyLen | (key[0] << 8) | (key[1] << 16);
	IniKeyValueNode *node = section->head;
	IniKeyValueNode *prev = NULL;
#if IniSectionImplUseSentinelNode
	section->sentinel->hash = hash;
	while (TRUE) {
		if (node->hash == hash) {
			if (node == section->sentinel) {
				return NULL;
			}
			if (StrEqual(node->key, key)) {
				// remove the node
				--section->count;
				if (prev == NULL) {
					section->head = node->next;
				} else {
					prev->next = node->next;
				}
				return node->value;
			}
		}
		prev = node;
		node = node->next;
	}
#else
	do {
		if (node->hash == hash && StrEqual(node->key, key)) {
			// remove the node
			--section->count;
			if (prev == NULL) {
				section->head = node->next;
			} else {
				prev->next = node->next;
			}
			return node->value;
		}
		prev = node;
		node = node->next;
	} while (node);
	return NULL;
#endif
}

void IniSectionGetStringImpl(IniSection *section, LPCWSTR key, int keyLen, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString) {
	LPCWSTR value = IniSectionGetValueImpl(section, key, keyLen);
	// allow empty string value
	lstrcpyn(lpReturnedString, ((value == NULL) ? lpDefault : value), cchReturnedString);
}

int IniSectionGetIntImpl(IniSection *section, LPCWSTR key, int keyLen, int iDefault) {
	LPCWSTR value = IniSectionGetValueImpl(section, key, keyLen);
	if (value && StrToIntEx(value, STIF_DEFAULT, &keyLen)) {
		return keyLen;
	}
	return iDefault;
}

BOOL IniSectionGetBoolImpl(IniSection *section, LPCWSTR key, int keyLen, BOOL bDefault) {
	LPCWSTR value = IniSectionGetValueImpl(section, key, keyLen);
	if (value) {
		switch (*value) {
		case L'1':
			return TRUE;
		case L'0':
			return FALSE;
		}
	}
	return bDefault;
}

void IniSectionSetString(IniSectionOnSave *section, LPCWSTR key, LPCWSTR value) {
	LPWSTR p = section->next;
	lstrcpy(p, key);
	lstrcat(p, L"=");
	lstrcat(p, value);
	p = StrEnd(p) + 1;
	*p = L'\0';
	section->next = p;
}

int ParseCommaList(LPCWSTR str, int result[], int count) {
	if (StrIsEmpty(str)) {
		return 0;
	}

	int index = 0;
	while (index < count) {
		LPWSTR end;
		result[index] = (int)wcstol(str, &end, 10);
		if (str == end) {
			break;
		}

		++index;
		if (*end == L',') {
			++end;
		}
		str = end;
	}
	return index;
}

//=============================================================================
//
//  ExeNameFromWnd()
//
BOOL ExeNameFromWnd(HWND hwnd, LPWSTR szExeName, int cchExeName) {
	DWORD dwProcessId;
	HANDLE hProcess;
	HMODULE hModule;
	DWORD cbNeeded = 0;

	GetWindowThreadProcessId(hwnd, &dwProcessId);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
	EnumProcessModules(hProcess, &hModule, sizeof(HMODULE), &cbNeeded);
	GetModuleFileNameExW(hProcess, hModule, szExeName, cchExeName);
	CloseHandle(hProcess);
	return TRUE;
}

////=============================================================================
////
////  Is32bitExe()
////
/*
BOOL Is32bitExe(LPCWSTR lpszExeName) {
	SHFILEINFO shfi;
	DWORD dwExeType;
	WCHAR tch[MAX_PATH];

	if (PathIsLnkFile(lpszExeName) && PathGetLnkPath(lpszExeName, tch, COUNTOF(tch))) {
		dwExeType = SHGetFileInfo(tch, 0, &shfi, sizeof(SHFILEINFO), SHGFI_EXETYPE);
	} else {
		dwExeType = SHGetFileInfo(lpszExeName, 0, &shfi, sizeof(SHFILEINFO), SHGFI_EXETYPE);
	}

	return (HIWORD(dwExeType) && LOWORD(dwExeType)) != 0;
}
*/

//=============================================================================
//
//  PrivateIsAppThemed()
//
extern HMODULE hModUxTheme;
BOOL PrivateIsAppThemed(void) {
	BOOL bIsAppThemed = FALSE;
	if (hModUxTheme) {
		FARPROC pfnIsAppThemed = GetProcAddress(hModUxTheme, "IsAppThemed");
		if (pfnIsAppThemed) {
			bIsAppThemed = (BOOL)pfnIsAppThemed();
		}
	}
	return bIsAppThemed;
}

//=============================================================================
//
//  SetTheme()
//
BOOL SetTheme(HWND hwnd, LPCWSTR lpszTheme) {
	if (hModUxTheme) {
		FARPROC pfnSetWindowTheme = GetProcAddress(hModUxTheme, "SetWindowTheme");

		if (pfnSetWindowTheme) {
			return (S_OK == pfnSetWindowTheme(hwnd, lpszTheme, NULL));
		}
	}
	return FALSE;
}

//=============================================================================
//
//  BitmapMergeAlpha()
//  Merge alpha channel into color channel
//
BOOL BitmapMergeAlpha(HBITMAP hbmp, COLORREF crDest) {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		if (bmp.bmBitsPixel == 32) {
			int x, y;
			RGBQUAD *prgba = bmp.bmBits;

			for (y = 0; y < bmp.bmHeight; y++) {
				for (x = 0; x < bmp.bmWidth; x++) {
					BYTE alpha = prgba[x].rgbReserved;
					prgba[x].rgbRed = ((prgba[x].rgbRed * alpha) + (GetRValue(crDest) * (255 - alpha))) >> 8;
					prgba[x].rgbGreen = ((prgba[x].rgbGreen * alpha) + (GetGValue(crDest) * (255 - alpha))) >> 8;
					prgba[x].rgbBlue = ((prgba[x].rgbBlue * alpha) + (GetBValue(crDest) * (255 - alpha))) >> 8;
					prgba[x].rgbReserved = 0xFF;
				}
				prgba = (RGBQUAD *)((LPBYTE)prgba + bmp.bmWidthBytes);
			}
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
//  BitmapAlphaBlend()
//  Perform alpha blending to color channel only
//
BOOL BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha) {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		if (bmp.bmBitsPixel == 32) {
			int x, y;
			RGBQUAD *prgba = bmp.bmBits;

			for (y = 0; y < bmp.bmHeight; y++) {
				for (x = 0; x < bmp.bmWidth; x++) {
					prgba[x].rgbRed = ((prgba[x].rgbRed * alpha) + (GetRValue(crDest) * (255 - alpha))) >> 8;
					prgba[x].rgbGreen = ((prgba[x].rgbGreen * alpha) + (GetGValue(crDest) * (255 - alpha))) >> 8;
					prgba[x].rgbBlue = ((prgba[x].rgbBlue * alpha) + (GetBValue(crDest) * (255 - alpha))) >> 8;
				}
				prgba = (RGBQUAD *)((LPBYTE)prgba + bmp.bmWidthBytes);
			}
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
//  BitmapGrayScale()
//  Gray scale color channel only
//
BOOL BitmapGrayScale(HBITMAP hbmp) {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		if (bmp.bmBitsPixel == 32) {
			RGBQUAD *prgba = bmp.bmBits;

			for (int y = 0; y < bmp.bmHeight; y++) {
				for (int x = 0; x < bmp.bmWidth; x++) {
					prgba[x].rgbRed = prgba[x].rgbGreen = prgba[x].rgbBlue =
							(((BYTE)((prgba[x].rgbRed * 38 + prgba[x].rgbGreen * 75 + prgba[x].rgbBlue * 15) >> 7) * 0x80) + (0xD0 * (255 - 0x80))) >> 8;
				}
				prgba = (RGBQUAD *)((LPBYTE)prgba + bmp.bmWidthBytes);
			}
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
//
//  SetWindowPathTitle()
//
BOOL SetWindowPathTitle(HWND hwnd, LPCWSTR lpszFile) {
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

	return SetWindowText(hwnd, szTitle);
}

//=============================================================================
//
//  CenterDlgInParent()
//
void CenterDlgInParent(HWND hDlg) {
	CenterDlgInParentEx(hDlg, GetParent(hDlg));
}

void CenterDlgInParentEx(HWND hDlg, HWND hParent) {
	RECT rcDlg;
	RECT rcParent;
	MONITORINFO mi;
	HMONITOR hMonitor;

	int xMin, yMin, xMax, yMax, x, y;

	GetWindowRect(hDlg, &rcDlg);
	GetWindowRect(hParent, &rcParent);

	hMonitor = MonitorFromRect(&rcParent, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	xMin = mi.rcWork.left;
	yMin = mi.rcWork.top;

	xMax = (mi.rcWork.right) - (rcDlg.right - rcDlg.left);
	yMax = (mi.rcWork.bottom) - (rcDlg.bottom - rcDlg.top);

	if ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left) > 20) {
		x = rcParent.left + (((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
	} else {
		x = rcParent.left + 70;
	}

	if ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top) > 20) {
		y = rcParent.top + (((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);
	} else {
		y = rcParent.top + 60;
	}

	SetWindowPos(hDlg, NULL, clamp_i(x, xMin, xMax), clamp_i(y, yMin, yMax), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

// Why doesn’t the "Automatically move pointer to the default button in a dialog box"
// work for nonstandard dialog boxes, and how do I add it to my own nonstandard dialog boxes?
// https://blogs.msdn.microsoft.com/oldnewthing/20130826-00/?p=3413/
void SnapToDefaultButton(HWND hwndBox) {
	BOOL fSnapToDefButton = FALSE;
	if (SystemParametersInfo(SPI_GETSNAPTODEFBUTTON, 0, &fSnapToDefButton, 0) && fSnapToDefButton) {
		// get child window at the top of the Z order.
		// for all our MessageBoxs it's the OK or YES button or NULL.
		HWND btn = GetWindow(hwndBox, GW_CHILD);
		if (btn != NULL) {
			WCHAR className[8] = L"";
			GetClassName(btn, className, COUNTOF(className));
			if (StrCaseEqual(className, L"Button")) {
				RECT rect;
				int x, y;
				GetWindowRect(btn, &rect);
				x = rect.left + (rect.right - rect.left) / 2;
				y = rect.top + (rect.bottom - rect.top) / 2;
				SetCursorPos(x, y);
			}
		}
	}
}

//=============================================================================
//
//  MakeBitmapButton()
//
void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, UINT uBmpId) {
	HWND hwndCtl = GetDlgItem(hwnd, nCtlId);
	BITMAP bmp;
	BUTTON_IMAGELIST bi;
	HBITMAP hBmp = LoadImage(hInstance, MAKEINTRESOURCE(uBmpId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	bi.himl = ImageList_Create(bmp.bmWidth, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 1, 0);
	ImageList_AddMasked(bi.himl, hBmp, CLR_DEFAULT);
	DeleteObject(hBmp);
	SetRect(&bi.margin, 0, 0, 0, 0);
	bi.uAlign = BUTTON_IMAGELIST_ALIGN_CENTER;
	SendMessage(hwndCtl, BCM_SETIMAGELIST, 0, (LPARAM)&bi);
}

//=============================================================================
//
//  DeleteBitmapButton()
//
void DeleteBitmapButton(HWND hwnd, int nCtlId) {
	HWND hwndCtl = GetDlgItem(hwnd, nCtlId);
	BUTTON_IMAGELIST bi;
	if (SendMessage(hwndCtl, BCM_GETIMAGELIST, 0, (LPARAM)&bi)) {
		ImageList_Destroy(bi.himl);
	}
}

//=============================================================================
//
//  SetWindowTransparentMode()
//
extern int iOpacityLevel;
void SetWindowTransparentMode(HWND hwnd, BOOL bTransparentMode) {
	if (bTransparentMode) {
		if (IsWin2KAndAbove()) {
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			const BYTE bAlpha = (BYTE)(iOpacityLevel * 255 / 100);
			SetLayeredWindowAttributes(hwnd, 0, bAlpha, LWA_ALPHA);
		}
	} else {
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
	}
}

//=============================================================================
//
//  Toolbar_Get/SetButtons()
//
int Toolbar_GetButtons(HWND hwnd, int cmdBase, LPWSTR lpszButtons, int cchButtons) {
	const int count = min_i(MAX_TOOLBAR_ITEM_COUNT_WITH_SEPARATOR, (int)SendMessage(hwnd, TB_BUTTONCOUNT, 0, 0));
	const int maxCch = cchButtons - 3; // two digits, one space and NULL
	int len = 0;

	for (int i = 0; i < count && len < maxCch; i++) {
		TBBUTTON tbb;
		SendMessage(hwnd, TB_GETBUTTON, (WPARAM)i, (LPARAM)&tbb);
		const int iCmd = (tbb.idCommand == 0) ? 0 : tbb.idCommand - cmdBase + 1;
		len += wsprintf(lpszButtons + len, L"%i ", iCmd);
	}

	lpszButtons[len--] = L'\0';
	if (len >= 0) {
		lpszButtons[len] = L'\0';
	}
	return count;
}

int Toolbar_SetButtons(HWND hwnd, LPCWSTR lpszButtons, LPCTBBUTTON ptbb, int ctbb) {
	int count = (int)SendMessage(hwnd, TB_BUTTONCOUNT, 0, 0);
	if (StrIsEmpty(lpszButtons)) {
		return count;
	}

	while (count) {
		SendMessage(hwnd, TB_DELETEBUTTON, 0, 0);
		--count;
	}

	LPCWSTR p = lpszButtons;
	--ctbb;
	while (TRUE) {
		LPWSTR end;
		int iCmd = (int)wcstol(p, &end, 10);
		if (p != end) {
			iCmd = clamp_i(iCmd, 0, ctbb);
			SendMessage(hwnd, TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&ptbb[iCmd]);
			p = end;
			++count;
			//if (count == MAX_TOOLBAR_ITEM_COUNT_WITH_SEPARATOR) {
			//	break;
			//}
		} else {
			break;
		}
	}

	return count;
}

//=============================================================================
//
//  Toolbar_SetButtonImage()
//
void Toolbar_SetButtonImage(HWND hwnd, int idCommand, int iImage) {
	TBBUTTONINFO tbbi;

	tbbi.cbSize = sizeof(TBBUTTONINFO);
	tbbi.dwMask = TBIF_IMAGE;
	tbbi.iImage = iImage;

	SendMessage(hwnd, TB_SETBUTTONINFO, (WPARAM)idCommand, (LPARAM)&tbbi);
}

//=============================================================================
//
//  SendWMSize()
//
LRESULT SendWMSize(HWND hwnd) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	return SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
}

//=============================================================================
//
//  PathRelativeToApp()
//
void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, int cchDest, BOOL bSrcIsFile, BOOL bUnexpandEnv, BOOL bUnexpandMyDocs) {
	WCHAR wchAppPath[MAX_PATH];
	WCHAR wchWinDir[MAX_PATH];
	WCHAR wchUserFiles[MAX_PATH];
	WCHAR wchPath[MAX_PATH];
	WCHAR wchResult[MAX_PATH];
	DWORD dwAttrTo = (bSrcIsFile) ? 0 : FILE_ATTRIBUTE_DIRECTORY;

	GetModuleFileName(NULL, wchAppPath, COUNTOF(wchAppPath));
	PathRemoveFileSpec(wchAppPath);
	GetWindowsDirectory(wchWinDir, COUNTOF(wchWinDir));
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, wchUserFiles);

	if (bUnexpandMyDocs &&
			!PathIsRelative(lpszSrc) &&
			!PathIsPrefix(wchUserFiles, wchAppPath) &&
			PathIsPrefix(wchUserFiles, lpszSrc) &&
			PathRelativePathTo(wchPath, wchUserFiles, FILE_ATTRIBUTE_DIRECTORY, lpszSrc, dwAttrTo)) {
		lstrcpy(wchUserFiles, L"%CSIDL:MYDOCUMENTS%");
		PathAppend(wchUserFiles, wchPath);
		lstrcpy(wchPath, wchUserFiles);
	} else if (PathIsRelative(lpszSrc) || PathCommonPrefix(wchAppPath, wchWinDir, NULL)) {
		lstrcpyn(wchPath, lpszSrc, COUNTOF(wchPath));
	} else {
		if (!PathRelativePathTo(wchPath, wchAppPath, FILE_ATTRIBUTE_DIRECTORY, lpszSrc, dwAttrTo)) {
			lstrcpyn(wchPath, lpszSrc, COUNTOF(wchPath));
		}
	}

	if (bUnexpandEnv) {
		if (!PathUnExpandEnvStrings(wchPath, wchResult, COUNTOF(wchResult))) {
			lstrcpyn(wchResult, wchPath, COUNTOF(wchResult));
		}
	} else {
		lstrcpyn(wchResult, wchPath, COUNTOF(wchResult));
	}

	lstrcpyn(lpszDest, wchResult, (cchDest == 0) ? MAX_PATH : cchDest);
}

//=============================================================================
//
//  PathAbsoluteFromApp()
//
void PathAbsoluteFromApp(LPCWSTR lpszSrc, LPWSTR lpszDest, int cchDest, BOOL bExpandEnv) {
	WCHAR wchPath[MAX_PATH];
	WCHAR wchResult[MAX_PATH];

	if (StrNEqual(lpszSrc, L"%CSIDL:MYDOCUMENTS%", CSTRLEN("%CSIDL:MYDOCUMENTS%"))) {
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, wchPath);
		PathAppend(wchPath, lpszSrc + CSTRLEN("%CSIDL:MYDOCUMENTS%"));
	} else {
		lstrcpyn(wchPath, lpszSrc, COUNTOF(wchPath));
	}

	if (bExpandEnv) {
		ExpandEnvironmentStringsEx(wchPath, COUNTOF(wchPath));
	}

	if (PathIsRelative(wchPath)) {
		GetModuleFileName(NULL, wchResult, COUNTOF(wchResult));
		PathRemoveFileSpec(wchResult);
		PathAppend(wchResult, wchPath);
	} else {
		lstrcpyn(wchResult, wchPath, COUNTOF(wchResult));
	}

	PathCanonicalizeEx(wchResult);
	if (PathGetDriveNumber(wchResult) != -1) {
		CharUpperBuff(wchResult, 1);
	}

	lstrcpyn(lpszDest, wchResult, (cchDest == 0) ? MAX_PATH : cchDest);
}

///////////////////////////////////////////////////////////////////////////////
//
//
//  Name: PathIsLnkFile()
//
//  Purpose: Determine wheter pszPath is a Windows Shell Link File by
//           comparing the filename extension with L".lnk"
//
//  Manipulates:
//
BOOL PathIsLnkFile(LPCWSTR pszPath) {
	//WCHAR *pszExt;

	if (StrIsEmpty(pszPath)) {
		return FALSE;
	}

	/*
	pszExt = StrRChr(pszPath, L'.');
	if (!pszExt) {
		return FALSE;
	}

	if (StrCaseEqual(pszExt, L".lnk")) {
		return TRUE;
	} else {
		return FALSE;
	}

	if (StrCaseEqual(PathFindExtension(pszPath), L".lnk")) {
		return TRUE;
	} else {
		return FALSE;
	}
	*/

	if (!StrCaseEqual(PathFindExtension(pszPath), L".lnk")) {
		return FALSE;
	}
	{
		WCHAR tchResPath[MAX_PATH];
		return PathGetLnkPath(pszPath, tchResPath, COUNTOF(tchResPath));
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//
//  Name: PathGetLnkPath()
//
//  Purpose: Try to get the path to which a lnk-file is linked
//
//
//  Manipulates: pszResPath
//
BOOL PathGetLnkPath(LPCWSTR pszLnkFile, LPWSTR pszResPath, int cchResPath) {
	IShellLink *psl;
	WIN32_FIND_DATA  fd;
	BOOL bSucceeded = FALSE;

	if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void **)(&ppf)))) {
			WCHAR wsz[MAX_PATH];

			/*MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, pszLnkFile, -1, wsz, MAX_PATH);*/
			lstrcpy(wsz, pszLnkFile);

			if (SUCCEEDED(ppf->lpVtbl->Load(ppf, wsz, STGM_READ))) {
				if (NOERROR == psl->lpVtbl->GetPath(psl, pszResPath, cchResPath, &fd, 0)) {
					// This additional check seems reasonable
					bSucceeded = StrNotEmpty(pszResPath);
				}
			}
			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}

	if (bSucceeded) {
		ExpandEnvironmentStringsEx(pszResPath, cchResPath);
		PathCanonicalizeEx(pszResPath);
	}

	return bSucceeded;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//  Name: PathIsLnkToDirectory()
//
//  Purpose: Determine wheter pszPath is a Windows Shell Link File which
//           refers to a directory
//
//  Manipulates: pszResPath
//
BOOL PathIsLnkToDirectory(LPCWSTR pszPath, LPWSTR pszResPath, int cchResPath) {
	if (PathIsLnkFile(pszPath)) {
		WCHAR tchResPath[MAX_PATH];
		if (PathGetLnkPath(pszPath, tchResPath, sizeof(WCHAR)*COUNTOF(tchResPath))) {
			if (PathIsDirectory(tchResPath)) {
				lstrcpyn(pszResPath, tchResPath, cchResPath);
				return TRUE;
			}
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//  Name: PathCreateLnk()
//
//  Purpose: Try to create a lnk-file in the specified directory referring
//           to the specified file or directory
//
//  Manipulates:
//
BOOL PathCreateLnk(LPCWSTR pszLnkDir, LPCWSTR pszPath) {
	WCHAR tchLnkFileName[MAX_PATH];

	IShellLink *psl;
	BOOL bSucceeded = FALSE;
	BOOL fMustCopy;

	// Try to construct a valid filename...
	if (!SHGetNewLinkInfo(pszPath, pszLnkDir, tchLnkFileName, &fMustCopy, SHGNLI_PREFIXNAME)) {
		return FALSE;
	}

	if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void **)(&ppf)))) {
			WCHAR wsz[MAX_PATH];
			/*MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, tchLnkFileName,-1,wsz,MAX_PATH);*/
			lstrcpy(wsz, tchLnkFileName);

			if (NOERROR == psl->lpVtbl->SetPath(psl, pszPath) && SUCCEEDED(ppf->lpVtbl->Save(ppf, wsz, TRUE))) {
				bSucceeded = TRUE;
			}

			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}

	return bSucceeded;
}

//=============================================================================
//
//  ExtractFirstArgument()
//
BOOL ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2) {
	LPWSTR psz;
	BOOL bQuoted = FALSE;

	lstrcpy(lpArg1, lpArgs);
	if (lpArg2) {
		*lpArg2 = L'\0';
	}

	TrimString(lpArg1);
	if (!*lpArg1) {
		return FALSE;
	}

	if (*lpArg1 == L'\"') {
		*lpArg1 = L' ';
		TrimString(lpArg1);
		bQuoted = TRUE;
	}

	if (bQuoted) {
		psz = StrChr(lpArg1, L'\"');
	} else {
		psz = StrChr(lpArg1, L' ');
	};

	if (psz) {
		*psz = L'\0';
		if (lpArg2) {
			lstrcpy(lpArg2, psz + 1);
		}
	}

	TrimString(lpArg1);

	if (lpArg2) {
		TrimString(lpArg2);
	}

	return TRUE;
}

//=============================================================================
//
//  QuotateFilenameStr()
//
LPWSTR QuotateFilenameStr(LPWSTR lpFile) {
	if (StrChr(lpFile, L' ')) {
		MoveMemory(lpFile + 1, lpFile, sizeof(WCHAR) * (lstrlen(lpFile) + 1));
		*lpFile = '\"';
		lstrcat(lpFile, L"\"");
	}
	return lpFile;
}

//=============================================================================
//
//  GetFilenameStr()
//
LPWSTR GetFilenameStr(LPWSTR lpFile) {
	LPWSTR psz = StrEnd(lpFile);

	while (psz != lpFile && *CharPrev(lpFile, psz) != L'\\') {
		psz = CharPrev(lpFile, psz);
	}

	return psz;
}

//=============================================================================
//
//  PrepareFilterStr()
//
void PrepareFilterStr(LPWSTR lpFilter) {
	LPWSTR psz = StrEnd(lpFilter);
	while (psz != lpFilter) {
		if (*(psz = CharPrev(lpFilter, psz)) == L'\n') {
			*psz = L'\0';
		}
	}
}

//=============================================================================
//
//  StrTab2Space() - in place conversion
//
void StrTab2Space(LPWSTR lpsz) {
	WCHAR *c = lpsz;
	while ((c = StrChr(lpsz, L'\t')) != NULL) {
		*c = L' ';
	}
}

//=============================================================================
//
//  ExpandEnvironmentStringsEx()
//
//  Adjusted for Windows 95
//
void ExpandEnvironmentStringsEx(LPWSTR lpSrc, DWORD dwSrc) {
	WCHAR szBuf[312];

	if (ExpandEnvironmentStrings(lpSrc, szBuf, COUNTOF(szBuf))) {
		lstrcpyn(lpSrc, szBuf, dwSrc);
	}
}

//=============================================================================
//
//  PathCanonicalizeEx()
//
//
void PathCanonicalizeEx(LPWSTR lpSrc) {
	WCHAR szDst[MAX_PATH];

	if (PathCanonicalize(szDst, lpSrc)) {
		lstrcpy(lpSrc, szDst);
	}
}

//=============================================================================
//
//  SearchPathEx()
//
//  This Expansion also searches the L"Favorites" folder
//
extern WCHAR tchFavoritesDir[MAX_PATH];
extern WCHAR szCurDir[MAX_PATH];

DWORD SearchPathEx(LPCWSTR lpPath, LPCWSTR lpFileName, LPCWSTR lpExtension, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR *lpFilePart) {
	DWORD dwRetVal = 0;

	if (StrEqual(lpFileName, L"..") || StrEqual(lpFileName, L".")) {
		if (StrEqual(lpFileName, L"..") && PathIsRoot(szCurDir)) {
			lstrcpyn(lpBuffer, L"*.*", nBufferLength);
			dwRetVal = 1;
		} else {
			dwRetVal = SearchPath(szCurDir, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
		}
	} else {
		dwRetVal = SearchPath(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

		// Search L"Favorites" if no result
		if (!dwRetVal) {
			dwRetVal = SearchPath(tchFavoritesDir, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
		}
	}

	return dwRetVal;
}

//=============================================================================
//
//  FormatNumberStr()
//
void FormatNumberStr(LPWSTR lpNumberStr) {
	int	i = lstrlen(lpNumberStr);
	if (i <= 3) {
		return;
	}

	WCHAR szSep[8];
	if (!GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szSep, COUNTOF(szSep))) {
		szSep[0] = L'\'';
	}

	WCHAR *c = lpNumberStr + i;
	i = 0;
	while ((c = CharPrev(lpNumberStr, c)) != lpNumberStr) {
		if (++i == 3) {
			i = 0;
			MoveMemory(c + 1, c, sizeof(WCHAR) * (lstrlen(c) + 1));
			*c = szSep[0];
		}
	}
}

//=============================================================================
//
//  GetDefaultFavoritesDir()
//
void GetDefaultFavoritesDir(LPWSTR lpFavDir, int cchFavDir) {
	LPITEMIDLIST pidl;

	if (NOERROR == SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl)) {
		SHGetPathFromIDList(pidl, lpFavDir);
		CoTaskMemFree((LPVOID)pidl);
	} else {
		GetWindowsDirectory(lpFavDir, cchFavDir);
	}
}

//=============================================================================
//
//  GetDefaultOpenWithDir()
//
void GetDefaultOpenWithDir(LPWSTR lpOpenWithDir, int cchOpenWithDir) {
	LPITEMIDLIST pidl;

	if (NOERROR == SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &pidl)) {
		SHGetPathFromIDList(pidl, lpOpenWithDir);
		CoTaskMemFree((LPVOID)pidl);
	} else {
		GetWindowsDirectory(lpOpenWithDir, cchOpenWithDir);
	}
}

//=============================================================================
//
//  CreateDropHandle()
//
//  Creates a HDROP to generate a WM_DROPFILES message
//
//
HDROP CreateDropHandle(LPCWSTR lpFileName) {
	LPDROPFILES  lpdf;
	HGLOBAL      hDrop;

	hDrop = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DROPFILES) + sizeof(WCHAR) * (lstrlen(lpFileName) + 2));
	lpdf = GlobalLock(hDrop);

	lpdf->pFiles = sizeof(DROPFILES);
	lpdf->pt.x   = 0;
	lpdf->pt.y   = 0;
	lpdf->fNC    = TRUE;
	lpdf->fWide  = TRUE;

	lstrcpy((WCHAR *)(lpdf + 1), lpFileName);
	GlobalUnlock(hDrop);

	return hDrop;
}

//=============================================================================
//
//  ExecDDECommand()
//
//  Execute a DDE command (Msg,App,Topic)
//
//
HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, ULONG_PTR  dwData1, ULONG_PTR  dwData2) {
	UNREFERENCED_PARAMETER(uFmt);
	UNREFERENCED_PARAMETER(hconv);
	UNREFERENCED_PARAMETER(hsz1);
	UNREFERENCED_PARAMETER(hsz2);
	UNREFERENCED_PARAMETER(hdata);
	UNREFERENCED_PARAMETER(dwData1);
	UNREFERENCED_PARAMETER(dwData2);

	switch (uType) {
	case XTYP_ADVDATA:
		return (HDDEDATA)DDE_FACK;

	default:
		return (HDDEDATA)NULL;
	}
}

BOOL ExecDDECommand(LPCWSTR lpszCmdLine, LPCWSTR lpszDDEMsg, LPCWSTR lpszDDEApp, LPCWSTR lpszDDETopic) {
	WCHAR  lpszURLExec[512];
	WCHAR  lpszDDEMsgBuf[256];
	WCHAR  *pSubst;
	DWORD idInst = 0;
	BOOL bSuccess = TRUE;

	if (StrIsEmpty(lpszCmdLine) || StrIsEmpty(lpszDDEMsg) || StrIsEmpty(lpszDDEApp) || StrIsEmpty(lpszDDETopic)) {
		return FALSE;
	}

	lstrcpyn(lpszDDEMsgBuf, lpszDDEMsg, COUNTOF(lpszDDEMsgBuf));
	if ((pSubst = StrStr(lpszDDEMsgBuf, L"%1")) != NULL) {
		*(pSubst + 1) = L's';
	}

	wsprintf(lpszURLExec, lpszDDEMsgBuf, lpszCmdLine);

	if (DdeInitialize(&idInst, DdeCallback, APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0L) == DMLERR_NO_ERROR) {
		HSZ hszService, hszTopic;
		hszService = DdeCreateStringHandle(idInst, lpszDDEApp, CP_WINUNICODE);
		hszTopic = DdeCreateStringHandle(idInst, lpszDDETopic, CP_WINUNICODE);
		if (hszService && hszTopic) {
			HCONV hConv;
			hConv = DdeConnect(idInst, hszService, hszTopic, NULL);
			if (hConv) {
				DdeClientTransaction((LPBYTE)lpszURLExec, sizeof(WCHAR) * (lstrlen(lpszURLExec) + 1), hConv, 0, 0, XTYP_EXECUTE, TIMEOUT_ASYNC, NULL);
				DdeDisconnect(hConv);
			} else {
				bSuccess = FALSE;
			}
		}

		if (hszTopic) {
			DdeFreeStringHandle(idInst, hszTopic);
		}
		if (hszService) {
			DdeFreeStringHandle(idInst, hszService);
		}
		DdeUninitialize(idInst);
	}

	return bSuccess;
}

//=============================================================================
//
//  History Functions
//
//
BOOL History_Init(PHISTORY ph) {
	if (!ph) {
		return FALSE;
	}

	ZeroMemory(ph, sizeof(HISTORY));
	ph->iCurItem = (-1);

	return TRUE;
}

BOOL History_Uninit(PHISTORY ph) {
	if (!ph) {
		return FALSE;
	}

	for (int i = 0; i < HISTORY_ITEMS; i++) {
		if (ph->psz[i]) {
			LocalFree(ph->psz[i]);
		}
		ph->psz[i] = NULL;
	}

	return TRUE;
}

BOOL History_Add(PHISTORY ph, LPCWSTR pszNew) {
	if (!ph) {
		return FALSE;
	}

	// Item to be added is equal to current item
	if (ph->iCurItem >= 0 && ph->iCurItem <= (HISTORY_ITEMS - 1)) {
		if (StrCaseEqual(pszNew, ph->psz[ph->iCurItem])) {
			return FALSE;
		}
	}

	if (ph->iCurItem < (HISTORY_ITEMS - 1)) {
		ph->iCurItem++;
		for (int i = ph->iCurItem; i < HISTORY_ITEMS; i++) {
			if (ph->psz[i]) {
				LocalFree(ph->psz[i]);
			}
			ph->psz[i] = NULL;
		}
	} else {
		// Shift
		if (ph->psz[0]) {
			LocalFree(ph->psz[0]);
		}

		MoveMemory(ph->psz, ph->psz + 1, (HISTORY_ITEMS - 1) * sizeof(WCHAR *));
	}

	ph->psz[ph->iCurItem] = StrDup(pszNew);

	return TRUE;
}

BOOL History_Forward(PHISTORY ph, LPWSTR pszItem, int cItem) {
	if (!ph) {
		return FALSE;
	}

	if (ph->iCurItem < (HISTORY_ITEMS - 1)) {
		if (ph->psz[ph->iCurItem + 1]) {
			ph->iCurItem++;
			lstrcpyn(pszItem, ph->psz[ph->iCurItem], cItem);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL History_Back(PHISTORY ph, LPWSTR pszItem, int cItem) {
	if (!ph) {
		return FALSE;
	}

	if (ph->iCurItem > 0) {
		if (ph->psz[ph->iCurItem - 1]) {
			ph->iCurItem--;
			lstrcpyn(pszItem, ph->psz[ph->iCurItem], cItem);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL History_CanForward(PHISTORY ph) {
	if (!ph) {
		return FALSE;
	}

	if (ph->iCurItem < (HISTORY_ITEMS - 1)) {
		if (ph->psz[ph->iCurItem + 1]) {
			return TRUE;
		}
	}

	return FALSE;
}

BOOL History_CanBack(PHISTORY ph) {
	if (!ph) {
		return FALSE;
	}

	if (ph->iCurItem > 0) {
		if (ph->psz[ph->iCurItem - 1]) {
			return TRUE;
		}
	}

	return FALSE;
}

void History_UpdateToolbar(PHISTORY ph, HWND hwnd, int cmdBack, int cmdForward) {
	if (History_CanBack(ph)) {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdBack, MAKELONG(1, 0));
	} else {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdBack, MAKELONG(0, 0));
	}

	if (History_CanForward(ph)) {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdForward, MAKELONG(1, 0));
	} else {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdForward, MAKELONG(0, 0));
	}
}

//=============================================================================
//
//  MRU functions
//
LPMRULIST MRU_Create(LPCWSTR pszRegKey, int iFlags, int iSize) {
	LPMRULIST pmru = NP2HeapAlloc(sizeof(MRULIST));
	pmru->szRegKey = pszRegKey;
	pmru->iFlags = iFlags;
	pmru->iSize = min_i(iSize, MRU_MAXITEMS);
	return pmru;
}

BOOL MRU_Destroy(LPMRULIST pmru) {
	for (int i = 0; i < pmru->iSize; i++) {
		if (pmru->pszItems[i]) {
			LocalFree(pmru->pszItems[i]);
		}
	}

	ZeroMemory(pmru, sizeof(MRULIST));
	NP2HeapFree(pmru);
	return 1;
}

int MRU_Compare(LPMRULIST pmru, LPCWSTR psz1, LPCWSTR psz2) {
	return (pmru->iFlags & MRU_NOCASE) ? StrCmpI(psz1, psz2) : StrCmp(psz1, psz2);
}

BOOL MRU_Add(LPMRULIST pmru, LPCWSTR pszNew) {
	int i;
	for (i = 0; i < pmru->iSize; i++) {
		if (MRU_Compare(pmru, pmru->pszItems[i], pszNew) == 0) {
			LocalFree(pmru->pszItems[i]);
			break;
		}
	}
	i = min_i(i, pmru->iSize - 1);
	for (; i > 0; i--) {
		pmru->pszItems[i] = pmru->pszItems[i - 1];
	}
	pmru->pszItems[0] = StrDup(pszNew);
	return TRUE;
}

BOOL MRU_Delete(LPMRULIST pmru, int iIndex) {
	if (iIndex < 0 || iIndex > pmru->iSize - 1) {
		return 0;
	}
	if (pmru->pszItems[iIndex]) {
		LocalFree(pmru->pszItems[iIndex]);
	}
	for (int i = iIndex; i < pmru->iSize - 1; i++) {
		pmru->pszItems[i] = pmru->pszItems[i + 1];
		pmru->pszItems[i + 1] = NULL;
	}
	return TRUE;
}

BOOL MRU_Empty(LPMRULIST pmru) {
	for (int i = 0; i < pmru->iSize; i++) {
		if (pmru->pszItems[i]) {
			LocalFree(pmru->pszItems[i]);
			pmru->pszItems[i] = NULL;
		}
	}
	return TRUE;
}

int MRU_Enum(LPMRULIST pmru, int iIndex, LPWSTR pszItem, int cchItem) {
	if (pszItem == NULL || cchItem == 0) {
		int i = 0;
		while (i < pmru->iSize && pmru->pszItems[i]) {
			i++;
		}
		return i;
	}

	if (iIndex < 0 || iIndex > pmru->iSize - 1 || !pmru->pszItems[iIndex]) {
		return -1;
	}
	lstrcpyn(pszItem, pmru->pszItems[iIndex], cchItem);
	return TRUE;
}

BOOL MRU_Load(LPMRULIST pmru) {
	IniSection section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU);
	const int cbIniSection = (int)NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR);
	IniSection *pIniSection = &section;

	MRU_Empty(pmru);
	IniSectionInit(pIniSection, MRU_MAXITEMS);

	LoadIniSection(pmru->szRegKey, pIniSectionBuf, cbIniSection);
	IniSectionParseArray(pIniSection, pIniSectionBuf);
	const int count = pIniSection->count;
	const int size = pmru->iSize;

	for (int i = 0, n = 0; i < count && n < size; i++) {
		const IniKeyValueNode *node = &pIniSection->nodeList[i];
		LPCWSTR tchItem = node->value;
		if (StrNotEmpty(tchItem)) {
			/*if (pmru->iFlags & MRU_UTF8) {
				WCHAR wchItem[1024];
				int cbw = MultiByteToWideChar(CP_UTF7, 0, tchItem, -1, wchItem, COUNTOF(wchItem));
				WideCharToMultiByte(CP_UTF8, 0, wchItem, cbw, tchItem, COUNTOF(tchItem), NULL, NULL);
				pmru->pszItems[n++] = StrDup(tchItem);
			}
			else*/
			pmru->pszItems[n++] = StrDup(tchItem);
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
	return TRUE;
}

BOOL MRU_Save(LPMRULIST pmru) {
	if (MRU_GetCount(pmru) == 0) {
		IniClearSection(pmru->szRegKey);
		return TRUE;
	}

	WCHAR tchName[16];
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	for (int i = 0; i < pmru->iSize; i++) {
		if (StrNotEmpty(pmru->pszItems[i])) {
			wsprintf(tchName, L"%02i", i + 1);
			/*if (pmru->iFlags & MRU_UTF8) {
				WCHAR  tchItem[1024];
				WCHAR wchItem[1024];
				int cbw = MultiByteToWideChar(CP_UTF8,0,pmru->pszItems[i],-1,wchItem,COUNTOF(wchItem));
				WideCharToMultiByte(CP_UTF7,0,wchItem,cbw,tchItem,COUNTOF(tchItem),NULL,NULL);
				IniSectionSetString(pIniSection,tchName,tchItem);
			}
			else*/
			IniSectionSetString(pIniSection, tchName, pmru->pszItems[i]);
		}
	}

	SaveIniSection(pmru->szRegKey, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
	return TRUE;
}

void MRU_LoadToCombobox(HWND hwnd, LPCWSTR pszKey) {
	WCHAR tch[MAX_PATH];
	LPMRULIST pmru = MRU_Create(pszKey, MRU_NOCASE, 8);
	MRU_Load(pmru);
	for (int i = 0; i < MRU_GetCount(pmru); i++) {
		MRU_Enum(pmru, i, tch, COUNTOF(tch));
		SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)tch);
	}
	MRU_Destroy(pmru);
}

void MRU_AddOneItem(LPCWSTR pszKey, LPCWSTR pszNewItem) {
	if (StrNotEmpty(pszNewItem)) {
		LPMRULIST pmru = MRU_Create(pszKey, MRU_NOCASE, 8);
		MRU_Load(pmru);
		MRU_Add(pmru, pszNewItem);
		MRU_Save(pmru);
		MRU_Destroy(pmru);
	}
}

void MRU_ClearCombobox(HWND hwnd, LPCWSTR pszKey) {
	LPMRULIST pmru = MRU_Create(pszKey, MRU_NOCASE, 8);
	MRU_Load(pmru);
	MRU_Empty(pmru);
	MRU_Save(pmru);
	MRU_Destroy(pmru);
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
}

/*

  Themed Dialogs
  Modify dialog templates to use current theme font
  Based on code of MFC helper class CDialogTemplate

*/
BOOL GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize) {
	LOGFONT lf;
	BOOL bSucceed = FALSE;

	HDC hDC = GetDC(NULL);
	int iLogPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
	ReleaseDC(NULL, hDC);

	if (hModUxTheme) {
		if ((BOOL)(GetProcAddress(hModUxTheme, "IsAppThemed"))()) {
			HTHEME hTheme = (HTHEME)(INT_PTR)(GetProcAddress(hModUxTheme, "OpenThemeData"))(NULL, L"WINDOWSTYLE;WINDOW");
			if (hTheme) {
				if (S_OK == (HRESULT)(GetProcAddress(hModUxTheme, "GetThemeSysFont"))(hTheme,/*TMT_MSGBOXFONT*/805, &lf)) {
					if (lf.lfHeight < 0) {
						lf.lfHeight = -lf.lfHeight;
					}
					*wSize = (WORD)MulDiv(lf.lfHeight, 72, iLogPixelsY);
					if (*wSize == 0) {
						*wSize = 8;
					}
					StrCpyN(lpFaceName, lf.lfFaceName, LF_FACESIZE);
					bSucceed = TRUE;
				}
				(GetProcAddress(hModUxTheme, "CloseThemeData"))(hTheme);
			}
		}
	}

	/*
	if (!bSucceed) {
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
		if (ncm.lfMessageFont.lfHeight < 0) {
			ncm.lfMessageFont.lfHeight = -ncm.lfMessageFont.lfHeight;
		}
		*wSize = (WORD)MulDiv(ncm.lfMessageFont.lfHeight, 72, iLogPixelsY);
		if (*wSize == 0) {
			*wSize = 8;
		}
		StrCpyN(lpFaceName, ncm.lfMessageFont.lfFaceName, LF_FACESIZE);
	}
	*/

	return bSucceed;
}

static inline BOOL DialogTemplate_IsDialogEx(const DLGTEMPLATE *pTemplate) {
	return ((DLGTEMPLATEEX *)pTemplate)->signature == 0xFFFF;
}

static inline BOOL DialogTemplate_HasFont(const DLGTEMPLATE *pTemplate) {
	return (DS_SETFONT & (DialogTemplate_IsDialogEx(pTemplate) ? ((DLGTEMPLATEEX *)pTemplate)->style : pTemplate->style));
}

static inline int DialogTemplate_FontAttrSize(BOOL bDialogEx) {
	return (int)sizeof(WORD) * (bDialogEx ? 3 : 1);
}

static inline BYTE *DialogTemplate_GetFontSizeField(const DLGTEMPLATE *pTemplate) {
	BOOL bDialogEx = DialogTemplate_IsDialogEx(pTemplate);
	WORD *pw;

	if (bDialogEx) {
		pw = (WORD *)((DLGTEMPLATEEX *)pTemplate + 1);
	} else {
		pw = (WORD *)(pTemplate + 1);
	}

	if (*pw == (WORD) - 1) {
		pw += 2;
	} else {
		while (*pw++){}
	}

	if (*pw == (WORD) - 1) {
		pw += 2;
	} else {
		while (*pw++){}
	}

	while (*pw++){}

	return (BYTE *)pw;
}

DLGTEMPLATE *LoadThemedDialogTemplate(LPCTSTR lpDialogTemplateID, HINSTANCE hInstance) {
	HRSRC hRsrc;
	HGLOBAL hRsrcMem;
	DLGTEMPLATE *pRsrcMem;
	DLGTEMPLATE *pTemplate;
	UINT dwTemplateSize = 0;
	WCHAR wchFaceName[LF_FACESIZE];
	WORD wFontSize;
	BOOL bDialogEx;
	BOOL bHasFont;
	int cbFontAttr;
	int cbNew;
	int cbOld;
	BYTE *pbNew;
	BYTE *pb;
	BYTE *pOldControls;
	BYTE *pNewControls;
	WORD nCtrl;

	hRsrc = FindResource(hInstance, lpDialogTemplateID, RT_DIALOG);
	if (hRsrc == NULL) {
		return NULL;
	}

	hRsrcMem = LoadResource(hInstance, hRsrc);
	pRsrcMem = (DLGTEMPLATE *)LockResource(hRsrcMem);
	dwTemplateSize = (UINT)SizeofResource(hInstance, hRsrc);

	pTemplate = dwTemplateSize ? NP2HeapAlloc(dwTemplateSize + LF_FACESIZE * 2) : NULL;
	if (pTemplate == NULL) {
		UnlockResource(hRsrcMem);
		FreeResource(hRsrcMem);
		return NULL;
	}

	CopyMemory((BYTE *)pTemplate, pRsrcMem, (size_t)dwTemplateSize);
	UnlockResource(hRsrcMem);
	FreeResource(hRsrcMem);

	if (!GetThemedDialogFont(wchFaceName, &wFontSize)) {
		return pTemplate;
	}

	bDialogEx = DialogTemplate_IsDialogEx(pTemplate);
	bHasFont = DialogTemplate_HasFont(pTemplate);
	cbFontAttr = DialogTemplate_FontAttrSize(bDialogEx);

	if (bDialogEx) {
		((DLGTEMPLATEEX *)pTemplate)->style |= DS_SHELLFONT;
	} else {
		pTemplate->style |= DS_SHELLFONT;
	}

	cbNew = cbFontAttr + ((lstrlen(wchFaceName) + 1) * sizeof(WCHAR));
	pbNew = (BYTE *)wchFaceName;

	pb = DialogTemplate_GetFontSizeField(pTemplate);
	cbOld = (int)(bHasFont ? cbFontAttr + 2 * (lstrlen((WCHAR *)(pb + cbFontAttr)) + 1) : 0);

	pOldControls = (BYTE *)(((DWORD_PTR)pb + cbOld + 3) & ~(DWORD_PTR)3);
	pNewControls = (BYTE *)(((DWORD_PTR)pb + cbNew + 3) & ~(DWORD_PTR)3);

	nCtrl = bDialogEx ? (WORD)((DLGTEMPLATEEX *)pTemplate)->cDlgItems : (WORD)pTemplate->cdit;

	if (cbNew != cbOld && nCtrl > 0) {
		MoveMemory(pNewControls, pOldControls, (size_t)(dwTemplateSize - (pOldControls - (BYTE *)pTemplate)));
	}

	*(WORD *)pb = wFontSize;
	MoveMemory(pb + cbFontAttr, pbNew, (size_t)(cbNew - cbFontAttr));

	return pTemplate;
}

INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
	INT_PTR ret;
	DLGTEMPLATE *pDlgTemplate;

	pDlgTemplate = LoadThemedDialogTemplate(lpTemplate, hInstance);
	ret = DialogBoxIndirectParam(hInstance, pDlgTemplate, hWndParent, lpDialogFunc, dwInitParam);
	if (pDlgTemplate) {
		NP2HeapFree(pDlgTemplate);
	}

	return ret;
}

/*
  MinimizeToTray - Copyright 2000 Matthew Ellis <m.t.ellis@bigfoot.com>

  Changes made by flo:
   - Commented out: #include "stdafx.h"
   - Moved variable declaration: APPBARDATA appBarData;
   - Declared GetDoAnimateMinimize() as non-static
*/

// MinimizeToTray
//
// A couple of routines to show how to make it produce a custom caption
// animation to make it look like we are minimizing to and maximizing
// from the system tray
//
// These routines are public domain, but it would be nice if you dropped
// me a line if you use them!
//
// 1.0 29.06.2000 Initial version
// 1.1 01.07.2000 The window retains it's place in the Z-order of windows
//     when minimized/hidden. This means that when restored/shown, it doen't
//     always appear as the foreground window unless we call SetForegroundWindow
//
// Copyright 2000 Matthew Ellis <m.t.ellis@bigfoot.com>
/*#include "stdafx.h"*/

// Odd. VC++6 winuser.h has IDANI_CAPTION defined (as well as IDANI_OPEN and
// IDANI_CLOSE), but the Platform SDK only has IDANI_OPEN...

// I don't know what IDANI_OPEN or IDANI_CLOSE do. Trying them in this code
// produces nothing. Perhaps they were intended for window opening and closing
// like the MAC provides...
#ifndef IDANI_OPEN
#define IDANI_OPEN 1
#endif
#ifndef IDANI_CLOSE
#define IDANI_CLOSE 2
#endif
#ifndef IDANI_CAPTION
#define IDANI_CAPTION 3
#endif

#define DEFAULT_RECT_WIDTH 150
#define DEFAULT_RECT_HEIGHT 30

// Returns the rect of where we think the system tray is. This will work for
// all current versions of the shell. If explorer isn't running, we try our
// best to work with a 3rd party shell. If we still can't find anything, we
// return a rect in the lower right hand corner of the screen
static VOID GetTrayWndRect(LPRECT lpTrayRect) {
	APPBARDATA appBarData;
	// First, we'll use a quick hack method. We know that the taskbar is a window
	// of class Shell_TrayWnd, and the status tray is a child of this of class
	// TrayNotifyWnd. This provides us a window rect to minimize to. Note, however,
	// that this is not guaranteed to work on future versions of the shell. If we
	// use this method, make sure we have a backup!
	HWND hShellTrayWnd = FindWindowEx(NULL, NULL, TEXT("Shell_TrayWnd"), NULL);
	if (hShellTrayWnd) {
		HWND hTrayNotifyWnd = FindWindowEx(hShellTrayWnd, NULL, TEXT("TrayNotifyWnd"), NULL);
		if (hTrayNotifyWnd) {
			GetWindowRect(hTrayNotifyWnd, lpTrayRect);
			return;
		}
	}

	// OK, we failed to get the rect from the quick hack. Either explorer isn't
	// running or it's a new version of the shell with the window class names
	// changed (how dare Microsoft change these undocumented class names!) So, we
	// try to find out what side of the screen the taskbar is connected to. We
	// know that the system tray is either on the right or the bottom of the
	// taskbar, so we can make a good guess at where to minimize to
	/*APPBARDATA appBarData;*/
	appBarData.cbSize = sizeof(appBarData);
	if (SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
		// We know the edge the taskbar is connected to, so guess the rect of the
		// system tray. Use various fudge factor to make it look good
		switch (appBarData.uEdge) {
		case ABE_LEFT:
		case ABE_RIGHT:
			// We want to minimize to the bottom of the taskbar
			lpTrayRect->top = appBarData.rc.bottom - 100;
			lpTrayRect->bottom = appBarData.rc.bottom - 16;
			lpTrayRect->left = appBarData.rc.left;
			lpTrayRect->right = appBarData.rc.right;
			break;

		case ABE_TOP:
		case ABE_BOTTOM:
			// We want to minimize to the right of the taskbar
			lpTrayRect->top = appBarData.rc.top;
			lpTrayRect->bottom = appBarData.rc.bottom;
			lpTrayRect->left = appBarData.rc.right - 100;
			lpTrayRect->right = appBarData.rc.right - 16;
			break;
		}
		return;
	}

	// Blimey, we really aren't in luck. It's possible that a third party shell
	// is running instead of explorer. This shell might provide support for the
	// system tray, by providing a Shell_TrayWnd window (which receives the
	// messages for the icons) So, look for a Shell_TrayWnd window and work out
	// the rect from that. Remember that explorer's taskbar is the Shell_TrayWnd,
	// and stretches either the width or the height of the screen. We can't rely
	// on the 3rd party shell's Shell_TrayWnd doing the same, in fact, we can't
	// rely on it being any size. The best we can do is just blindly use the
	// window rect, perhaps limiting the width and height to, say 150 square.
	// Note that if the 3rd party shell supports the same configuraion as
	// explorer (the icons hosted in NotifyTrayWnd, which is a child window of
	// Shell_TrayWnd), we would already have caught it above
	hShellTrayWnd = FindWindowEx(NULL, NULL, TEXT("Shell_TrayWnd"), NULL);
	if (hShellTrayWnd) {
		GetWindowRect(hShellTrayWnd, lpTrayRect);
		if (lpTrayRect->right - lpTrayRect->left > DEFAULT_RECT_WIDTH) {
			lpTrayRect->left = lpTrayRect->right - DEFAULT_RECT_WIDTH;
		}
		if (lpTrayRect->bottom - lpTrayRect->top > DEFAULT_RECT_HEIGHT) {
			lpTrayRect->top = lpTrayRect->bottom - DEFAULT_RECT_HEIGHT;
		}

		return;
	}

	// OK. Haven't found a thing. Provide a default rect based on the current work
	// area
	SystemParametersInfo(SPI_GETWORKAREA, 0, lpTrayRect, 0);
	lpTrayRect->left = lpTrayRect->right - DEFAULT_RECT_WIDTH;
	lpTrayRect->top = lpTrayRect->bottom - DEFAULT_RECT_HEIGHT;
}

// Check to see if the animation has been disabled
/*static */BOOL GetDoAnimateMinimize(VOID) {
	ANIMATIONINFO ai;

	ai.cbSize = sizeof(ai);
	SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0);

	return ai.iMinAnimate != 0;
}

VOID MinimizeWndToTray(HWND hwnd) {
	if (GetDoAnimateMinimize()) {
		RECT rcFrom, rcTo;

		// Get the rect of the window. It is safe to use the rect of the whole
		// window - DrawAnimatedRects will only draw the caption
		GetWindowRect(hwnd, &rcFrom);
		GetTrayWndRect(&rcTo);

		// Get the system to draw our animation for us
		DrawAnimatedRects(hwnd, IDANI_CAPTION, &rcFrom, &rcTo);
	}

	// Add the tray icon. If we add it before the call to DrawAnimatedRects,
	// the taskbar gets erased, but doesn't get redrawn until DAR finishes.
	// This looks untidy, so call the functions in this order

	// Hide the window
	ShowWindow(hwnd, SW_HIDE);
}

VOID RestoreWndFromTray(HWND hwnd) {
	if (GetDoAnimateMinimize()) {
		// Get the rect of the tray and the window. Note that the window rect
		// is still valid even though the window is hidden
		RECT rcFrom, rcTo;
		GetTrayWndRect(&rcFrom);
		GetWindowRect(hwnd, &rcTo);

		// Get the system to draw our animation for us
		DrawAnimatedRects(hwnd, IDANI_CAPTION, &rcFrom, &rcTo);
	}

	// Show the window, and make sure we're the foreground window
	ShowWindow(hwnd, SW_SHOW);
	SetActiveWindow(hwnd);
	SetForegroundWindow(hwnd);

	// Remove the tray icon. As described above, remove the icon after the
	// call to DrawAnimatedRects, or the taskbar will not refresh itself
	// properly until DAR finished
}

///   End of Helpers.c
