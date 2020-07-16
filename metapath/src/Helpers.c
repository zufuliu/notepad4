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
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <psapi.h>
#include <stdio.h>
#include "config.h"
#include "Helpers.h"
#include "Dlapi.h"
#include "resource.h"

void IniClearSectionEx(LPCWSTR lpSection, LPCWSTR lpszIniFile, BOOL bDelete) {
	if (StrIsEmpty(lpszIniFile)) {
		return;
	}

	WritePrivateProfileSection(lpSection, (bDelete ? NULL : L""), lpszIniFile);
}

void IniClearAllSectionEx(LPCWSTR lpszPrefix, LPCWSTR lpszIniFile, BOOL bDelete) {
	if (StrIsEmpty(lpszIniFile)) {
		return;
	}

	WCHAR sections[1024] = L"";
	GetPrivateProfileSectionNames(sections, COUNTOF(sections), lpszIniFile);

	LPCWSTR p = sections;
	LPCWSTR value = bDelete ? NULL : L"";
	const int len = lstrlen(lpszPrefix);

	while (*p) {
		if (StrHasPrefixCaseEx(p, lpszPrefix, len)) {
			WritePrivateProfileSection(p, value, lpszIniFile);
		}
		p = StrEnd(p) + 1;
	}
}

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
		LPWSTR v = StrChr(p, L'=');
		if (v != NULL) {
			*v++ = L'\0';
			IniKeyValueNode *node = &section->nodeList[count];
			node->key = p;
			node->value = v;
			++count;
			p = StrEnd(v) + 1;
		} else {
			p = StrDup(p) + 1;
		}
	} while (*p && count < capacity);

	section->count = count;
	return count != 0;
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
		LPWSTR v = StrChr(p, L'=');
		if (v != NULL) {
			*v++ = L'\0';
			const UINT keyLen = (UINT)(v - p - 1);
			IniKeyValueNode *node = &section->nodeList[count];
			node->hash = keyLen | (p[0] << 8) | (p[1] << 16);
			node->key = p;
			node->value = v;
			++count;
			p = StrEnd(v) + 1;
		} else {
			p = StrEnd(p) + 1;
		}
	} while (*p && count < capacity);

	if (count == 0) {
		return FALSE;
	}

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
	if (keyLen == 0) {
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
	if (value && CRTStrToInt(value, &keyLen)) {
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

LPWSTR Registry_GetString(HKEY hKey, LPCWSTR valueName) {
	LPWSTR lpszText = NULL;
	DWORD type = REG_SZ;
	DWORD size = 0;

	LSTATUS status = RegQueryValueEx(hKey, valueName, NULL, &type, NULL, &size);
	if (status == ERROR_SUCCESS && type == REG_SZ && size != 0) {
		size = (size + 1)*sizeof(WCHAR);
		lpszText = (LPWSTR)NP2HeapAlloc(size);
		status = RegQueryValueEx(hKey, valueName, NULL, &type, (LPBYTE)lpszText, &size);
		if (status != ERROR_SUCCESS || type != REG_SZ || size == 0) {
			NP2HeapFree(lpszText);
			lpszText = NULL;
		}
	}
	return lpszText;
}

LSTATUS Registry_SetString(HKEY hKey, LPCWSTR valueName, LPCWSTR lpszText) {
	DWORD len = lstrlen(lpszText);
	len = len ? ((len + 1)*sizeof(WCHAR)) : 0;
	LSTATUS status = RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE *)lpszText, len);
	return status;
}

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
LSTATUS Registry_DeleteTree(HKEY hKey, LPCWSTR lpSubKey) {
	typedef LSTATUS (WINAPI *RegDeleteTreeSig)(HKEY hKey, LPCWSTR lpSubKey);
	RegDeleteTreeSig pfnRegDeleteTree = DLLFunctionEx(RegDeleteTreeSig, L"advapi32.dll", "RegDeleteTreeW");

	LSTATUS status;
	if (pfnRegDeleteTree != NULL) {
		status = pfnRegDeleteTree(hKey, lpSubKey);
	} else {
		status = RegDeleteKey(hKey, lpSubKey);
		if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND) {
			// TODO: Deleting a Key with Subkeys on Windows XP.
			// https://docs.microsoft.com/en-us/windows/win32/sysinfo/deleting-a-key-with-subkeys
		}
	}

	return status;
}
#endif

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

BOOL FindUserResourcePath(LPCWSTR path, LPWSTR outPath) {
	// similar to CheckIniFile()
	WCHAR tchFileExpanded[MAX_PATH];
	ExpandEnvironmentStrings(path, tchFileExpanded, COUNTOF(tchFileExpanded));

	if (PathIsRelative(tchFileExpanded)) {
		WCHAR tchBuild[MAX_PATH];
		// relative to program ini file
		if (StrNotEmpty(szIniFile)) {
			lstrcpy(tchBuild, szIniFile);
			lstrcpy(PathFindFileName(tchBuild), tchFileExpanded);
			if (PathIsFile(tchBuild)) {
				lstrcpy(outPath, tchBuild);
				return TRUE;
			}
		}

		// relative to program exe file
		GetModuleFileName(NULL, tchBuild, COUNTOF(tchBuild));
		lstrcpy(PathFindFileName(tchBuild), tchFileExpanded);
		if (PathIsFile(tchBuild)) {
			lstrcpy(outPath, tchBuild);
			return TRUE;
		}
	} else if (PathIsFile(tchFileExpanded)) {
		lstrcpy(outPath, tchFileExpanded);
		return TRUE;
	}
	return FALSE;
}

HBITMAP LoadBitmapFile(LPCWSTR path) {
	WCHAR szTmp[MAX_PATH];
	if (!FindUserResourcePath(path, szTmp)) {
		return NULL;
	}

	HBITMAP hbmp = (HBITMAP)LoadImage(NULL, szTmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	return hbmp;
}

//=============================================================================
//
// PrivateSetCurrentProcessExplicitAppUserModelID()
//
HRESULT PrivateSetCurrentProcessExplicitAppUserModelID(PCWSTR AppID) {
	if (StrIsEmpty(AppID)) {
		return S_OK;
	}
	if (StrCaseEqual(AppID, L"(default)")) {
		return S_OK;
	}

	// since Windows 7
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
	return SetCurrentProcessExplicitAppUserModelID(AppID);
#else
	typedef HRESULT (WINAPI *SetCurrentProcessExplicitAppUserModelIDSig)(PCWSTR AppID);
	SetCurrentProcessExplicitAppUserModelIDSig pfnSetCurrentProcessExplicitAppUserModelID =
		DLLFunctionEx(SetCurrentProcessExplicitAppUserModelIDSig, L"shell32.dll", "SetCurrentProcessExplicitAppUserModelID");
	if (pfnSetCurrentProcessExplicitAppUserModelID) {
		return pfnSetCurrentProcessExplicitAppUserModelID(AppID);
	}
	return S_OK;
#endif
}

//=============================================================================
//
// IsElevated()
//
BOOL IsElevated(void) {
	BOOL bIsElevated = FALSE;
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION te;
		DWORD dwReturnLength = 0;

		if (GetTokenInformation(hToken, TokenElevation, &te, sizeof(te), &dwReturnLength)) {
			if (dwReturnLength == sizeof(te)) {
				bIsElevated = te.TokenIsElevated;
			}
		}
		CloseHandle(hToken);
	}
	return bIsElevated;
}

//=============================================================================
//
//  ExeNameFromWnd()
//
BOOL ExeNameFromWnd(HWND hwnd, LPWSTR szExeName, int cchExeName) {
	DWORD dwProcessId;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

	HMODULE hModule;
	DWORD cbNeeded = 0;
	EnumProcessModules(hProcess, &hModule, sizeof(HMODULE), &cbNeeded);
	GetModuleFileNameEx(hProcess, hModule, szExeName, cchExeName);
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
//  BitmapMergeAlpha()
//  Merge alpha channel into color channel
//
BOOL BitmapMergeAlpha(HBITMAP hbmp, COLORREF crDest) {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		if (bmp.bmBitsPixel == 32) {
			RGBQUAD *prgba = (RGBQUAD *)bmp.bmBits;

			for (int y = 0; y < bmp.bmHeight; y++) {
				for (int x = 0; x < bmp.bmWidth; x++) {
					const BYTE alpha = prgba[x].rgbReserved;
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
			RGBQUAD *prgba = (RGBQUAD *)bmp.bmBits;

			for (int y = 0; y < bmp.bmHeight; y++) {
				for (int x = 0; x < bmp.bmWidth; x++) {
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
			RGBQUAD *prgba = (RGBQUAD *)bmp.bmBits;

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
//  CenterDlgInParentEx()
//
void CenterDlgInParentEx(HWND hDlg, HWND hParent) {
	RECT rcDlg;
	RECT rcParent;

	GetWindowRect(hDlg, &rcDlg);
	GetWindowRect(hParent, &rcParent);

	HMONITOR hMonitor = MonitorFromRect(&rcParent, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	const int xMin = mi.rcWork.left;
	const int yMin = mi.rcWork.top;

	const int xMax = (mi.rcWork.right) - (rcDlg.right - rcDlg.left);
	const int yMax = (mi.rcWork.bottom) - (rcDlg.bottom - rcDlg.top);

	int x;
	if ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left) > 20) {
		x = rcParent.left + (((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
	} else {
		x = rcParent.left + 70;
	}

	int y;
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
				GetWindowRect(btn, &rect);
				const int x = rect.left + (rect.right - rect.left) / 2;
				const int y = rect.top + (rect.bottom - rect.top) / 2;
				SetCursorPos(x, y);
			}
		}
	}
}

//=============================================================================
//
// GetDlgPos()
//
void GetDlgPos(HWND hDlg, LPINT xDlg, LPINT yDlg) {
	RECT rcDlg;
	GetWindowRect(hDlg, &rcDlg);

	HWND hParent = GetParent(hDlg);
	RECT rcParent;
	GetWindowRect(hParent, &rcParent);

	// return positions relative to parent window
	*xDlg = rcDlg.left - rcParent.left;
	*yDlg = rcDlg.top - rcParent.top;
}

//=============================================================================
//
// SetDlgPos()
//
void SetDlgPos(HWND hDlg, int xDlg, int yDlg) {
	RECT rcDlg;
	GetWindowRect(hDlg, &rcDlg);

	HWND hParent = GetParent(hDlg);
	RECT rcParent;
	GetWindowRect(hParent, &rcParent);

	HMONITOR hMonitor = MonitorFromRect(&rcParent, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	const int xMin = mi.rcWork.left;
	const int yMin = mi.rcWork.top;

	const int xMax = (mi.rcWork.right) - (rcDlg.right - rcDlg.left);
	const int yMax = (mi.rcWork.bottom) - (rcDlg.bottom - rcDlg.top);

	// desired positions relative to parent window
	const int x = rcParent.left + xDlg;
	const int y = rcParent.top + yDlg;

	SetWindowPos(hDlg, NULL, clamp_i(x, xMin, xMax), clamp_i(y, yMin, yMax), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

//=============================================================================
//
// Resize Dialog Helpers()
//
#define RESIZEDLG_PROP_KEY	L"ResizeDlg"
typedef struct RESIZEDLG {
	int direction;
	int cxClient;
	int cyClient;
	int mmiPtMinX;
	int mmiPtMinY;
	int mmiPtMaxX;	// only Y direction
	int mmiPtMaxY;	// only X direction
} RESIZEDLG, *PRESIZEDLG;

typedef const RESIZEDLG *LPCRESIZEDLG;

void ResizeDlg_InitEx(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int iDirection) {
	RESIZEDLG *pm = (RESIZEDLG *)NP2HeapAlloc(sizeof(RESIZEDLG));
	pm->direction = iDirection;

	RECT rc;
	GetClientRect(hwnd, &rc);
	pm->cxClient = rc.right - rc.left;
	pm->cyClient = rc.bottom - rc.top;

	const DWORD style = GetWindowStyle(hwnd) | WS_THICKFRAME;
	AdjustWindowRectEx(&rc, style, FALSE, 0);
	pm->mmiPtMinX = rc.right - rc.left;
	pm->mmiPtMinY = rc.bottom - rc.top;
	// only one direction
	switch (iDirection) {
	case ResizeDlgDirection_OnlyX:
		pm->mmiPtMaxY = pm->mmiPtMinY;
		break;

	case ResizeDlgDirection_OnlyY:
		pm->mmiPtMaxX = pm->mmiPtMinX;
		break;
	}

	cxFrame = max_i(cxFrame, pm->mmiPtMinX);
	cyFrame = max_i(cyFrame, pm->mmiPtMinY);

	SetProp(hwnd, RESIZEDLG_PROP_KEY, (HANDLE)pm);

	SetWindowPos(hwnd, NULL, rc.left, rc.top, cxFrame, cyFrame, SWP_NOZORDER);

	SetWindowStyle(hwnd, style);
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

	WCHAR wch[64];
	GetMenuString(GetSystemMenu(GetParent(hwnd), FALSE), SC_SIZE, wch, COUNTOF(wch), MF_BYCOMMAND);
	InsertMenu(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_STRING | MF_ENABLED, SC_SIZE, wch);
	InsertMenu(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, NULL);

	HWND hwndCtl = GetDlgItem(hwnd, nIdGrip);
	SetWindowStyle(hwndCtl, GetWindowStyle(hwndCtl) | SBS_SIZEGRIP | WS_CLIPSIBLINGS);
	const int cGrip = GetSystemMetrics(SM_CXHTHUMB);
	SetWindowPos(hwndCtl, NULL, pm->cxClient - cGrip, pm->cyClient - cGrip, cGrip, cGrip, SWP_NOZORDER);
}

void ResizeDlg_Destroy(HWND hwnd, int *cxFrame, int *cyFrame) {
	PRESIZEDLG pm = (PRESIZEDLG)GetProp(hwnd, RESIZEDLG_PROP_KEY);

	RECT rc;
	GetWindowRect(hwnd, &rc);
	if (cxFrame) {
		*cxFrame = rc.right - rc.left;
	}
	if (cyFrame) {
		*cyFrame = rc.bottom - rc.top;
	}

	RemoveProp(hwnd, RESIZEDLG_PROP_KEY);
	NP2HeapFree(pm);
}

void ResizeDlg_Size(HWND hwnd, LPARAM lParam, int *cx, int *cy) {
	PRESIZEDLG pm = (PRESIZEDLG)GetProp(hwnd, RESIZEDLG_PROP_KEY);
	const int cxClient = LOWORD(lParam);
	const int cyClient = HIWORD(lParam);
	if (cx) {
		*cx = cxClient - pm->cxClient;
	}
	if (cy) {
		*cy = cyClient - pm->cyClient;
	}
	pm->cxClient = cxClient;
	pm->cyClient = cyClient;
}

void ResizeDlg_GetMinMaxInfo(HWND hwnd, LPARAM lParam) {
	const LPCRESIZEDLG pm = (LPCRESIZEDLG)GetProp(hwnd, RESIZEDLG_PROP_KEY);

	LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;
	lpmmi->ptMinTrackSize.x = pm->mmiPtMinX;
	lpmmi->ptMinTrackSize.y = pm->mmiPtMinY;

	// only one direction
	switch (pm->direction) {
	case ResizeDlgDirection_OnlyX:
		lpmmi->ptMaxTrackSize.y = pm->mmiPtMaxY;
		break;

	case ResizeDlgDirection_OnlyY:
		lpmmi->ptMaxTrackSize.x = pm->mmiPtMaxX;
		break;
	}
}

HDWP DeferCtlPos(HDWP hdwp, HWND hwndDlg, int nCtlId, int dx, int dy, UINT uFlags) {
	HWND hwndCtl = GetDlgItem(hwndDlg, nCtlId);
	RECT rc;
	GetWindowRect(hwndCtl, &rc);
	MapWindowPoints(NULL, hwndDlg, (LPPOINT)&rc, 2);
	if (uFlags & SWP_NOSIZE) {
		return DeferWindowPos(hdwp, hwndCtl, NULL, rc.left + dx, rc.top + dy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	return DeferWindowPos(hdwp, hwndCtl, NULL, 0, 0, rc.right - rc.left + dx, rc.bottom - rc.top + dy, SWP_NOZORDER | SWP_NOMOVE);
}

void ResizeDlgCtl(HWND hwndDlg, int nCtlId, int dx, int dy) {
	HWND hwndCtl = GetDlgItem(hwndDlg, nCtlId);
	RECT rc;
	GetWindowRect(hwndCtl, &rc);
	MapWindowPoints(NULL, hwndDlg, (LPPOINT)&rc, 2);
	SetWindowPos(hwndCtl, NULL, 0, 0, rc.right - rc.left + dx, rc.bottom - rc.top + dy, SWP_NOZORDER | SWP_NOMOVE);
	InvalidateRect(hwndCtl, NULL, TRUE);
}

//=============================================================================
//
//  MakeBitmapButton()
//
void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, WORD wBmpId) {
	HWND hwndCtl = GetDlgItem(hwnd, nCtlId);
	HBITMAP hBmp = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(wBmpId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	BUTTON_IMAGELIST bi;
	bi.himl = ImageList_Create(bmp.bmWidth, bmp.bmHeight, ILC_COLOR32 | ILC_MASK, 1, 0);
	ImageList_AddMasked(bi.himl, hBmp, CLR_DEFAULT);
	DeleteObject(hBmp);
	SetRect(&bi.margin, 0, 0, 0, 0);
	bi.uAlign = BUTTON_IMAGELIST_ALIGN_CENTER;
	Button_SetImageList(hwndCtl, &bi);
}

//=============================================================================
//
//  DeleteBitmapButton()
//
void DeleteBitmapButton(HWND hwnd, int nCtlId) {
	HWND hwndCtl = GetDlgItem(hwnd, nCtlId);
	BUTTON_IMAGELIST bi;
	if (Button_GetImageList(hwndCtl, &bi)) {
		ImageList_Destroy(bi.himl);
	}
}

//=============================================================================
//
//  SetWindowTransparentMode()
//
void SetWindowTransparentMode(HWND hwnd, BOOL bTransparentMode, int iOpacityLevel) {
	const DWORD exStyle = GetWindowExStyle(hwnd);
	if (bTransparentMode) {
		SetWindowExStyle(hwnd, exStyle | WS_EX_LAYERED);
		const BYTE bAlpha = (BYTE)(iOpacityLevel * 255 / 100);
		SetLayeredWindowAttributes(hwnd, 0, bAlpha, LWA_ALPHA);
	} else {
		SetWindowExStyle(hwnd, exStyle & ~WS_EX_LAYERED);
	}
}

void SetWindowLayoutRTL(HWND hwnd, BOOL bRTL) {
	const DWORD exStyle = GetWindowExStyle(hwnd);
	if (bRTL) {
		SetWindowExStyle(hwnd, exStyle | WS_EX_LAYOUTRTL);
	} else {
		SetWindowExStyle(hwnd, exStyle & ~WS_EX_LAYOUTRTL);
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
		SendMessage(hwnd, TB_GETBUTTON, i, (LPARAM)&tbb);
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
			SendMessage(hwnd, TB_ADDBUTTONS, 1, (LPARAM)&ptbb[iCmd]);
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

	SendMessage(hwnd, TB_SETBUTTONINFO, idCommand, (LPARAM)&tbbi);
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

#if NP2_ENABLE_APP_LOCALIZATION_DLL
HMODULE LoadLocalizedResourceDLL(LANGID lang, LPCWSTR dllName) {
	if (lang == LANG_USER_DEFAULT) {
		lang = GetUserDefaultUILanguage();
	}

	LPCWSTR folder = NULL;
	const LANGID subLang = SUBLANGID(lang);
	switch (PRIMARYLANGID(lang)) {
	case LANG_ENGLISH:
		break;
	case LANG_CHINESE:
		folder = IsChineseTraditionalSubLang(subLang) ? L"zh-Hant" : L"zh-Hans";
		break;
	case LANG_GERMAN:
		folder = L"de";
		break;
	case LANG_ITALIAN:
		folder = L"it";
		break;
	case LANG_JAPANESE:
		folder = L"ja";
		break;
	//case LANG_KOREAN:
	//	folder = L"ko";
	//	break;
	}

	if (folder == NULL) {
		return NULL;
	}

	WCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, COUNTOF(path));
	PathRemoveFileSpec(path);
	PathAppend(path, L"locale");
	PathAppend(path, folder);
	PathAppend(path, dllName);

	const DWORD flags = IsVistaAndAbove() ? (LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE) : LOAD_LIBRARY_AS_DATAFILE;
	HMODULE hDLL = LoadLibraryEx(path, NULL, flags);
	return hDLL;
}

#if NP2_ENABLE_TEST_LOCALIZATION_LAYOUT
void GetLocaleDefaultUIFont(LANGID lang, LPWSTR lpFaceName, WORD *wSize) {
	LPCWSTR font;
	const LANGID subLang = SUBLANGID(lang);
	switch (PRIMARYLANGID(lang)) {
	default:
	case LANG_ENGLISH:
		font = L"Segoe UI";
		*wSize = 9;
		break;
	case LANG_CHINESE:
		font = IsChineseTraditionalSubLang(subLang) ? L"Microsoft JhengHei UI" : L"Microsoft YaHei UI";
		*wSize = 9;
		break;
	case LANG_JAPANESE:
		font = L"Meiryo UI";
		*wSize = 9;
		break;
	case LANG_KOREAN:
		font = L"Malgun Gothic";
		*wSize = 9;
		break;
	}
	lstrcpy(lpFaceName, font);
}
#endif
#endif

//=============================================================================
//
//  PathRelativeToApp()
//
void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, int cchDest, BOOL bSrcIsFile, BOOL bUnexpandEnv, BOOL bUnexpandMyDocs) {
	WCHAR wchAppPath[MAX_PATH];
	WCHAR wchWinDir[MAX_PATH];
	WCHAR wchUserFiles[MAX_PATH];
	WCHAR wchPath[MAX_PATH];
	const DWORD dwAttrTo = bSrcIsFile ? 0 : FILE_ATTRIBUTE_DIRECTORY;

	GetModuleFileName(NULL, wchAppPath, COUNTOF(wchAppPath));
	PathRemoveFileSpec(wchAppPath);
	GetWindowsDirectory(wchWinDir, COUNTOF(wchWinDir));
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
	if (S_OK != SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, wchUserFiles)) {
		return;
	}
#else
	LPWSTR pszPath = NULL;
	if (S_OK != SHGetKnownFolderPath(&FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &pszPath)) {
		return;
	}
	lstrcpy(wchUserFiles, pszPath);
	CoTaskMemFree(pszPath);
#endif

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

	WCHAR wchResult[MAX_PATH];
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

	if (StrHasPrefix(lpszSrc, L"%CSIDL:MYDOCUMENTS%")) {
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
		if (S_OK != SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, wchPath)) {
			return;
		}
#else
		LPWSTR pszPath = NULL;
		if (S_OK != SHGetKnownFolderPath(&FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &pszPath)) {
			return;
		}
		lstrcpy(wchPath, pszPath);
		CoTaskMemFree(pszPath);
#endif
		PathAppend(wchPath, lpszSrc + CSTRLEN("%CSIDL:MYDOCUMENTS%"));
	} else {
		lstrcpyn(wchPath, lpszSrc, COUNTOF(wchPath));
	}

	if (bExpandEnv) {
		ExpandEnvironmentStringsEx(wchPath, COUNTOF(wchPath));
	}

	WCHAR wchResult[MAX_PATH];
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
	if (StrIsEmpty(pszPath)) {
		return FALSE;
	}

	/*
	LPCWSTR pszExt = StrRChr(pszPath, L'.');
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
	BOOL bSucceeded = FALSE;

#if defined(__cplusplus)
	if (SUCCEEDED(CoCreateInstance(IID_IShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void **)(&ppf)))) {
			WCHAR wsz[MAX_PATH];
			lstrcpy(wsz, pszLnkFile);

			if (SUCCEEDED(ppf->Load(wsz, STGM_READ))) {
				WIN32_FIND_DATA fd;
				if (S_OK == psl->GetPath(pszResPath, cchResPath, &fd, 0)) {
					// This additional check seems reasonable
					bSucceeded = StrNotEmpty(pszResPath);
				}
			}
			ppf->Release();
		}
		psl->Release();
	}
#else
	if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void **)(&ppf)))) {
			WCHAR wsz[MAX_PATH];
			lstrcpy(wsz, pszLnkFile);

			if (SUCCEEDED(ppf->lpVtbl->Load(ppf, wsz, STGM_READ))) {
				WIN32_FIND_DATA fd;
				if (S_OK == psl->lpVtbl->GetPath(psl, pszResPath, cchResPath, &fd, 0)) {
					// This additional check seems reasonable
					bSucceeded = StrNotEmpty(pszResPath);
				}
			}
			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}
#endif

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
		if (PathGetLnkPath(pszPath, tchResPath, COUNTOF(tchResPath))) {
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
	BOOL fMustCopy;

	// Try to construct a valid filename...
	if (!SHGetNewLinkInfo(pszPath, pszLnkDir, tchLnkFileName, &fMustCopy, SHGNLI_PREFIXNAME)) {
		return FALSE;
	}

	IShellLink *psl;
	BOOL bSucceeded = FALSE;

#if defined(__cplusplus)
	if (SUCCEEDED(CoCreateInstance(IID_IShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void **)(&ppf)))) {
			WCHAR wsz[MAX_PATH];
			lstrcpy(wsz, tchLnkFileName);

			if (S_OK == psl->SetPath(pszPath) && SUCCEEDED(ppf->Save(wsz, TRUE))) {
				bSucceeded = TRUE;
			}

			ppf->Release();
		}
		psl->Release();
	}
#else
	if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void **)(&ppf)))) {
			WCHAR wsz[MAX_PATH];
			lstrcpy(wsz, tchLnkFileName);

			if (S_OK == psl->lpVtbl->SetPath(psl, pszPath) && SUCCEEDED(ppf->lpVtbl->Save(ppf, wsz, TRUE))) {
				bSucceeded = TRUE;
			}

			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}
#endif

	return bSucceeded;
}

void OpenContainingFolder(HWND hwnd, LPCWSTR pszFile, BOOL bSelect) {
	WCHAR wchDirectory[MAX_PATH];
	lstrcpyn(wchDirectory, pszFile, COUNTOF(wchDirectory));

	LPCWSTR path = NULL;
	DWORD dwAttributes = GetFileAttributes(pszFile);
	if (bSelect || dwAttributes == INVALID_FILE_ATTRIBUTES || !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		PathRemoveFileSpec(wchDirectory);
	}
	if (bSelect && dwAttributes != INVALID_FILE_ATTRIBUTES) {
		// if pszFile is root, open the volume instead of open My Computer and select the volume
		if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) && PathIsRoot(pszFile)) {
			bSelect = FALSE;
		} else {
			path = pszFile;
		}
	}

	dwAttributes = GetFileAttributes(wchDirectory);
	if (dwAttributes == INVALID_FILE_ATTRIBUTES || !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		return;
	}

	LPITEMIDLIST pidl = ILCreateFromPath(wchDirectory);
	if (pidl) {
		HRESULT hr;
		LPITEMIDLIST pidlEntry = path ? ILCreateFromPath(path) : NULL;
		if (pidlEntry) {
			hr = SHOpenFolderAndSelectItems(pidl, 1, (LPCITEMIDLIST *)(&pidlEntry), 0);
			CoTaskMemFree((LPVOID)pidlEntry);
		} else if (!bSelect) {
#if 0
			// Use an invalid item to open the folder?
			hr = SHOpenFolderAndSelectItems(pidl, 1, (LPCITEMIDLIST *)(&pidl), 0);
#else
			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_IDLIST;
			sei.hwnd = hwnd;
			//sei.lpVerb = L"explore";
			sei.lpVerb = L"open";
			sei.lpIDList = (void *)pidl;
			sei.nShow = SW_SHOW;

			const BOOL result = ShellExecuteEx(&sei);
			hr = result ? S_OK : S_FALSE;
#endif
		} else {
			// open parent folder and select the folder
			hr = SHOpenFolderAndSelectItems(pidl, 0, NULL, 0);
		}
		CoTaskMemFree((LPVOID)pidl);
		if (hr == S_OK) {
			return;
		}
	}

#if 0
	if (path == NULL) {
		path = wchDirectory;
	}

	// open a new explorer window every time
	LPWSTR szParameters = (LPWSTR)NP2HeapAlloc((lstrlen(path) + 64) * sizeof(WCHAR));
	lstrcpy(szParameters, bSelect ? L"/select," : L"");
	lstrcat(szParameters, L"\"");
	lstrcat(szParameters, path);
	lstrcat(szParameters, L"\"");
	ShellExecute(hwnd, L"open", L"explorer", szParameters, NULL, SW_SHOW);
	NP2HeapFree(szParameters);
#endif
}

//=============================================================================
//
//  ExtractFirstArgument()
//
BOOL ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2) {
	BOOL bQuoted = FALSE;

	lstrcpy(lpArg1, lpArgs);
	if (lpArg2) {
		*lpArg2 = L'\0';
	}

	TrimString(lpArg1);
	if (*lpArg1 == L'\0') {
		return FALSE;
	}

	LPWSTR psz = lpArg1;
	WCHAR ch = *psz;
	if (ch == L'\"') {
		*psz++ = L' ';
		bQuoted = TRUE;
	} else if (ch == L'-' || ch == L'/') {
		// fix -appid="string with space"
		++psz;
		while ((ch = *psz) != L'\0' && ch != L' ') {
			++psz;
			if (ch == L'=' && *psz == L'\"') {
				bQuoted = TRUE;
				++psz;
				break;
			}
		}
	}

	psz = StrChr(psz, (bQuoted ? L'\"' : L' '));
	if (psz) {
		*psz = L'\0';
		if (lpArg2) {
			lstrcpy(lpArg2, psz + 1);
			TrimString(lpArg2);
		}
	}

	TrimString(lpArg1);

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
		if (*(psz = CharPrev(lpFilter, psz)) == L'|') {
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
	while ((c = StrChr(c, L'\t')) != NULL) {
		*c++ = L' ';
	}
}

//=============================================================================
//
// PathFixBackslashes() - in place conversion
//
void PathFixBackslashes(LPWSTR lpsz) {
	WCHAR *c = lpsz;
	while ((c = StrChr(c, L'/')) != NULL) {
		if (*CharPrev(lpsz, c) == L':' && *CharNext(c) == L'/') {
			c += 2;
		} else {
			*c++ = L'\\';
		}
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
extern WCHAR szCurDir[MAX_PATH + 40];

BOOL SearchPathEx(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer) {
	DWORD dwRetVal = 0;

	if (StrEqual(lpFileName, L"..") || StrEqual(lpFileName, L".")) {
		if (StrEqual(lpFileName, L"..") && PathIsRoot(szCurDir)) {
			lstrcpyn(lpBuffer, L"*.*", nBufferLength);
			dwRetVal = 1;
		}
	}

	if (!dwRetVal) {
		dwRetVal = SearchPath(szCurDir, lpFileName, NULL, nBufferLength, lpBuffer, NULL);
	}

	// Search Favorites if no result
	if (!dwRetVal) {
		dwRetVal = SearchPath(tchFavoritesDir, lpFileName, NULL, nBufferLength, lpBuffer, NULL);
	}

	return dwRetVal != 0;
}

//=============================================================================
//
//  FormatNumberStr()
//
void FormatNumberStr(LPWSTR lpNumberStr) {
	const int i = lstrlen(lpNumberStr);
	if (i <= 3) {
		return;
	}

	// https://docs.microsoft.com/en-us/windows/desktop/Intl/locale-sthousand
	// https://docs.microsoft.com/en-us/windows/desktop/Intl/locale-sgrouping
	WCHAR szSep[4];
	const WCHAR sep = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szSep, COUNTOF(szSep))? szSep[0] : L',';

	WCHAR *c = lpNumberStr + i;
	WCHAR *end = c;
#if 0
	i = 0;
	while ((c = CharPrev(lpNumberStr, c)) != lpNumberStr) {
		if (++i == 3) {
			i = 0;
			MoveMemory(c + 1, c, sizeof(WCHAR) * (end - c + 1));
			*c = sep;
			++end;
		}
	}
#endif
	lpNumberStr += 3;
	do {
		c -= 3;
		MoveMemory(c + 1, c, sizeof(WCHAR) * (end - c + 1));
		*c = sep;
		++end;
	} while (c > lpNumberStr);
}

//=============================================================================
//
//  GetDefaultFavoritesDir()
//
void GetDefaultFavoritesDir(LPWSTR lpFavDir, int cchFavDir) {
	LPITEMIDLIST pidl;

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
	if (S_OK == SHGetFolderLocation(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_DEFAULT, &pidl))
#else
	if (S_OK == SHGetKnownFolderIDList(&FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &pidl))
#endif
	{
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

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
	if (S_OK == SHGetFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_DEFAULT, &pidl))
#else
	if (S_OK == SHGetKnownFolderIDList(&FOLDERID_Desktop, KF_FLAG_DEFAULT, NULL, &pidl))
#endif
	{
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
	HGLOBAL hDrop = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DROPFILES) + sizeof(WCHAR) * (lstrlen(lpFileName) + 2));
	LPDROPFILES lpdf = (LPDROPFILES)GlobalLock(hDrop);

	lpdf->pFiles = sizeof(DROPFILES);
	lpdf->pt.x   = 0;
	lpdf->pt.y   = 0;
	lpdf->fNC    = TRUE;
	lpdf->fWide  = TRUE;

	lstrcpy((WCHAR *)(lpdf + 1), lpFileName);
	GlobalUnlock(hDrop);

	return (HDROP)hDrop;
}

//=============================================================================
//
//  ExecDDECommand()
//
//  Execute a DDE command (Msg, App, Topic)
//
//
HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, ULONG_PTR dwData1, ULONG_PTR dwData2) {
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
	if (StrIsEmpty(lpszCmdLine) || StrIsEmpty(lpszDDEMsg) || StrIsEmpty(lpszDDEApp) || StrIsEmpty(lpszDDETopic)) {
		return FALSE;
	}

	BOOL bSuccess = TRUE;
	WCHAR lpszDDEMsgBuf[256];
	WCHAR *pSubst;

	lstrcpyn(lpszDDEMsgBuf, lpszDDEMsg, COUNTOF(lpszDDEMsgBuf));
	if ((pSubst = StrStr(lpszDDEMsgBuf, L"%1")) != NULL) {
		*(pSubst + 1) = L's';
	}

	WCHAR lpszURLExec[512];
	wsprintf(lpszURLExec, lpszDDEMsgBuf, lpszCmdLine);

	DWORD idInst = 0;
	if (DdeInitialize(&idInst, DdeCallback, APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0L) == DMLERR_NO_ERROR) {
		HSZ hszService = DdeCreateStringHandle(idInst, lpszDDEApp, CP_WINUNICODE);
		HSZ hszTopic = DdeCreateStringHandle(idInst, lpszDDETopic, CP_WINUNICODE);
		if (hszService && hszTopic) {
			HCONV hConv = DdeConnect(idInst, hszService, hszTopic, NULL);
			if (hConv) {
				DdeClientTransaction((LPBYTE)lpszURLExec, sizeof(WCHAR) * (lstrlen(lpszURLExec) + 1), hConv, NULL, 0, XTYP_EXECUTE, TIMEOUT_ASYNC, NULL);
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
	ph->iCurItem = -1;

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

BOOL History_CanForward(LCPHISTORY ph) {
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

BOOL History_CanBack(LCPHISTORY ph) {
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

void History_UpdateToolbar(LCPHISTORY ph, HWND hwnd, int cmdBack, int cmdForward) {
	if (History_CanBack(ph)) {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdBack, MAKELPARAM(1, 0));
	} else {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdBack, MAKELPARAM(0, 0));
	}

	if (History_CanForward(ph)) {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdForward, MAKELPARAM(1, 0));
	} else {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdForward, MAKELPARAM(0, 0));
	}
}

//=============================================================================
//
//  MRU functions
//
LPMRULIST MRU_Create(LPCWSTR pszRegKey, int iFlags, int iSize) {
	LPMRULIST pmru = (LPMRULIST)NP2HeapAlloc(sizeof(MRULIST));
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

int MRU_Compare(LPCMRULIST pmru, LPCWSTR psz1, LPCWSTR psz2) {
	return (pmru->iFlags & MRUFlags_CaseInsensitive) ? StrCmpI(psz1, psz2) : StrCmp(psz1, psz2);
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

int MRU_Enum(LPCMRULIST pmru, int iIndex, LPWSTR pszItem, int cchItem) {
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
	if (StrIsEmpty(szIniFile)) {
		return TRUE;
	}

	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection *pIniSection = &section;

	MRU_Empty(pmru);
	IniSectionInit(pIniSection, MRU_MAXITEMS);

	LoadIniSection(pmru->szRegKey, pIniSectionBuf, cchIniSection);
	IniSectionParseArray(pIniSection, pIniSectionBuf);
	const int count = pIniSection->count;
	const int size = pmru->iSize;

	for (int i = 0, n = 0; i < count && n < size; i++) {
		const IniKeyValueNode *node = &pIniSection->nodeList[i];
		LPCWSTR tchItem = node->value;
		if (StrNotEmpty(tchItem)) {
			pmru->pszItems[n++] = StrDup(tchItem);
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
	return TRUE;
}

BOOL MRU_Save(LPCMRULIST pmru) {
	if (StrIsEmpty(szIniFile)) {
		return TRUE;
	}
	if (MRU_GetCount(pmru) == 0) {
		IniClearSection(pmru->szRegKey);
		return TRUE;
	}

	WCHAR tchName[16];
	IniSectionOnSave section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU);
	IniSectionOnSave *pIniSection = &section;
	pIniSection->next = pIniSectionBuf;

	for (int i = 0; i < pmru->iSize; i++) {
		if (StrNotEmpty(pmru->pszItems[i])) {
			wsprintf(tchName, L"%02i", i + 1);
			IniSectionSetString(pIniSection, tchName, pmru->pszItems[i]);
		}
	}

	SaveIniSection(pmru->szRegKey, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
	return TRUE;
}

void MRU_LoadToCombobox(HWND hwnd, LPCWSTR pszKey) {
	WCHAR tch[MAX_PATH];
	LPMRULIST pmru = MRU_Create(pszKey, MRUFlags_CaseInsensitive, MRU_MAX_COPY_MOVE_HISTORY);
	MRU_Load(pmru);
	for (int i = 0; i < MRU_GetCount(pmru); i++) {
		MRU_Enum(pmru, i, tch, COUNTOF(tch));
		ComboBox_AddString(hwnd, tch);
	}
	MRU_Destroy(pmru);
}

void MRU_AddOneItem(LPCWSTR pszKey, LPCWSTR pszNewItem) {
	if (StrNotEmpty(pszNewItem)) {
		LPMRULIST pmru = MRU_Create(pszKey, MRUFlags_CaseInsensitive, MRU_MAX_COPY_MOVE_HISTORY);
		MRU_Load(pmru);
		MRU_Add(pmru, pszNewItem);
		MRU_Save(pmru);
		MRU_Destroy(pmru);
	}
}

void MRU_ClearCombobox(HWND hwnd, LPCWSTR pszKey) {
	LPMRULIST pmru = MRU_Create(pszKey, MRUFlags_CaseInsensitive, MRU_MAX_COPY_MOVE_HISTORY);
	MRU_Load(pmru);
	MRU_Empty(pmru);
	MRU_Save(pmru);
	MRU_Destroy(pmru);
	ComboBox_ResetContent(hwnd);
}

/*
  Themed Dialogs
  Modify dialog templates to use current theme font
  Based on code of MFC helper class CDialogTemplate
*/
BOOL GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize) {
#if NP2_ENABLE_APP_LOCALIZATION_DLL && NP2_ENABLE_TEST_LOCALIZATION_LAYOUT
	extern LANGID uiLanguage;
	GetLocaleDefaultUIFont(uiLanguage, lpFaceName, wSize);
	return TRUE;
#else

	BOOL bSucceed = FALSE;
	const UINT iLogPixelsY = g_uSystemDPI;

	if (IsAppThemed()) {
		HTHEME hTheme = OpenThemeData(NULL, L"WINDOWSTYLE;WINDOW");
		if (hTheme) {
			LOGFONT lf;
			if (S_OK == GetThemeSysFont(hTheme, TMT_MSGBOXFONT, &lf)) {
				if (lf.lfHeight < 0) {
					lf.lfHeight = -lf.lfHeight;
				}
				*wSize = (WORD)MulDiv(lf.lfHeight, 72, iLogPixelsY);
				if (*wSize < 8) {
					*wSize = 8;
				}
				lstrcpyn(lpFaceName, lf.lfFaceName, LF_FACESIZE);
				bSucceed = TRUE;
			}
			CloseThemeData(hTheme);
		}
	}

	if (!bSucceed) {
		NONCLIENTMETRICS ncm;
		ZeroMemory(&ncm, sizeof(ncm));
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
		if (!IsVistaAndAbove()) {
			ncm.cbSize -= sizeof(ncm.iPaddedBorderWidth);
		}
#endif
		if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0)) {
			if (ncm.lfMessageFont.lfHeight < 0) {
				ncm.lfMessageFont.lfHeight = -ncm.lfMessageFont.lfHeight;
			}
			*wSize = (WORD)MulDiv(ncm.lfMessageFont.lfHeight, 72, iLogPixelsY);
			if (*wSize < 8) {
				*wSize = 8;
			}
			lstrcpyn(lpFaceName, ncm.lfMessageFont.lfFaceName, LF_FACESIZE);
			bSucceed = TRUE;
		}
	}

	if (bSucceed && !IsVistaAndAbove()) {
		// Windows 2000, XP, 2003
		lstrcpy(lpFaceName, L"Tahoma");
	}
	return bSucceed;
#endif
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
	const BOOL bDialogEx = DialogTemplate_IsDialogEx(pTemplate);
	WORD *pw;

	if (bDialogEx) {
		pw = (WORD *)((DLGTEMPLATEEX *)pTemplate + 1);
	} else {
		pw = (WORD *)(pTemplate + 1);
	}

	if (*pw == (WORD)(-1)) {
		pw += 2;
	} else {
		while (*pw++){}
	}

	if (*pw == (WORD)(-1)) {
		pw += 2;
	} else {
		while (*pw++){}
	}

	while (*pw++){}

	return (BYTE *)pw;
}

DLGTEMPLATE *LoadThemedDialogTemplate(LPCWSTR lpDialogTemplateID, HINSTANCE hInstance) {
	HRSRC hRsrc = FindResource(hInstance, lpDialogTemplateID, RT_DIALOG);
	if (hRsrc == NULL) {
		return NULL;
	}

	HGLOBAL hRsrcMem = LoadResource(hInstance, hRsrc);
	const DLGTEMPLATE *pRsrcMem = (DLGTEMPLATE *)LockResource(hRsrcMem);
	const UINT dwTemplateSize = (UINT)SizeofResource(hInstance, hRsrc);

	DLGTEMPLATE *pTemplate = dwTemplateSize ? (DLGTEMPLATE *)NP2HeapAlloc(dwTemplateSize + LF_FACESIZE * 2) : NULL;
	if (pTemplate == NULL) {
		FreeResource(hRsrcMem);
		return NULL;
	}

	CopyMemory((BYTE *)pTemplate, pRsrcMem, (size_t)dwTemplateSize);
	FreeResource(hRsrcMem);

	WCHAR wchFaceName[LF_FACESIZE];
	WORD wFontSize;
	if (!GetThemedDialogFont(wchFaceName, &wFontSize)) {
		return pTemplate;
	}

	const BOOL bDialogEx = DialogTemplate_IsDialogEx(pTemplate);
	const BOOL bHasFont = DialogTemplate_HasFont(pTemplate);
	const int cbFontAttr = DialogTemplate_FontAttrSize(bDialogEx);

	if (bDialogEx) {
		((DLGTEMPLATEEX *)pTemplate)->style |= DS_SHELLFONT;
	} else {
		pTemplate->style |= DS_SHELLFONT;
	}

	const int cbNew = cbFontAttr + (int)((lstrlen(wchFaceName) + 1) * sizeof(WCHAR));
	const BYTE *pbNew = (BYTE *)wchFaceName;

	BYTE *pb = DialogTemplate_GetFontSizeField(pTemplate);
	const int cbOld = (int)(bHasFont ? cbFontAttr + 2 * (lstrlen((WCHAR *)(pb + cbFontAttr)) + 1) : 0);

	const BYTE *pOldControls = (BYTE *)(((DWORD_PTR)pb + cbOld + 3) & ~(DWORD_PTR)3);
	BYTE *pNewControls = (BYTE *)(((DWORD_PTR)pb + cbNew + 3) & ~(DWORD_PTR)3);

	const WORD nCtrl = bDialogEx ? (WORD)((DLGTEMPLATEEX *)pTemplate)->cDlgItems : (WORD)pTemplate->cdit;

	if (cbNew != cbOld && nCtrl > 0) {
		MoveMemory(pNewControls, pOldControls, (size_t)(dwTemplateSize - (pOldControls - (BYTE *)pTemplate)));
	}

	*(WORD *)pb = wFontSize;
	MoveMemory(pb + cbFontAttr, pbNew, (size_t)(cbNew - cbFontAttr));

	return pTemplate;
}

INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
	DLGTEMPLATE *pDlgTemplate = LoadThemedDialogTemplate(lpTemplate, hInstance);
	const INT_PTR ret = DialogBoxIndirectParam(hInstance, pDlgTemplate, hWndParent, lpDialogFunc, dwInitParam);
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
	// First, we'll use a quick hack method. We know that the taskbar is a window
	// of class Shell_TrayWnd, and the status tray is a child of this of class
	// TrayNotifyWnd. This provides us a window rect to minimize to. Note, however,
	// that this is not guaranteed to work on future versions of the shell. If we
	// use this method, make sure we have a backup!
	HWND hShellTrayWnd = FindWindowEx(NULL, NULL, L"Shell_TrayWnd", NULL);
	if (hShellTrayWnd) {
		HWND hTrayNotifyWnd = FindWindowEx(hShellTrayWnd, NULL, L"TrayNotifyWnd", NULL);
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
	APPBARDATA appBarData;
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
	hShellTrayWnd = FindWindowEx(NULL, NULL, L"Shell_TrayWnd", NULL);
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
/*static */BOOL GetDoAnimateMinimize(void) {
	ANIMATIONINFO ai;

	ai.cbSize = sizeof(ai);
	SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0);

	return ai.iMinAnimate != 0;
}

void MinimizeWndToTray(HWND hwnd) {
	if (GetDoAnimateMinimize()) {
		RECT rcFrom;
		RECT rcTo;

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

void RestoreWndFromTray(HWND hwnd) {
	if (GetDoAnimateMinimize()) {
		// Get the rect of the tray and the window. Note that the window rect
		// is still valid even though the window is hidden
		RECT rcFrom;
		RECT rcTo;
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
