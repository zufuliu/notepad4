/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* Helpers.h
*   Definitions for general helper functions and macros
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
#pragma once

#include <stdint.h>
#include "compiler.h"

NP2_inline int min_i(int x, int y) {
	return (x < y) ? x : y;
}

NP2_inline int max_i(int x, int y) {
	return (x > y) ? x : y;
}

NP2_inline int clamp_i(int x, int lower, int upper) {
	return (x < lower) ? lower : (x > upper) ? upper : x;
}

NP2_inline int validate_i(int x, int lower, int upper, int defaultValue) {
	return (x < lower || x > upper) ? defaultValue : x;
}

NP2_inline bool StrIsEmpty(LPCWSTR s) {
	return s == NULL || *s == L'\0';
}

NP2_inline bool StrNotEmpty(LPCWSTR s) {
	return s != NULL && *s != L'\0';
}

NP2_inline int ToUpperA(int ch) {
	return (ch >= 'a' && ch <= 'z') ? (ch - 'a' + 'A') : ch;
}

NP2_inline int ToLowerA(int ch) {
	return (ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch;
}

NP2_inline bool StrEqual(LPCWSTR s1, LPCWSTR s2) {
	return wcscmp(s1, s2) == 0;
}

NP2_inline bool StrCaseEqual(LPCWSTR s1, LPCWSTR s2) {
	return _wcsicmp(s1, s2) == 0;
}

#define StrCpyExW(s, t)						memcpy((s), (t), STRSIZE(t))
#define StrEqualExW(s, t)					(memcmp((s), (t), STRSIZE(t)) == 0)
#define StrHasPrefix(s, prefix)				(memcmp((s), (prefix), STRSIZE(prefix) - sizeof(WCHAR)) == 0)
#define StrHasPrefixCase(s, prefix)			(_wcsnicmp((s), (prefix), CSTRLEN(prefix)) == 0)
#define StrHasPrefixCaseEx(s, prefix, len)	(_wcsnicmp((s), (prefix), (len)) == 0)

NP2_inline bool CRTStrToInt(LPCWSTR str, int *value) {
	LPWSTR end;
	*value = (int)wcstol(str, &end, 10);
	return str != end;
}

int ParseCommaList(LPCWSTR str, int result[], int count);

extern HINSTANCE g_hInstance;
#if defined(NP2_ENABLE_APP_LOCALIZATION_DLL) && NP2_ENABLE_APP_LOCALIZATION_DLL
extern HINSTANCE g_exeInstance;
#else
#define g_exeInstance	g_hInstance
#endif
extern HANDLE g_hDefaultHeap;
#if _WIN32_WINNT < _WIN32_WINNT_VISTA
extern DWORD g_uWinVer;
#endif

// Operating System Version
// https://docs.microsoft.com/en-us/windows/win32/sysinfo/operating-system-version

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#define IsVistaAndAbove()	true
#else
#define IsVistaAndAbove()	(g_uWinVer >= _WIN32_WINNT_VISTA)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
#define IsWin7AndAbove()	true
#else
#define IsWin7AndAbove()	(g_uWinVer >= _WIN32_WINNT_WIN7)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
#define IsWin8AndAbove()	true
#else
#define IsWin8AndAbove()	(g_uWinVer >= _WIN32_WINNT_WIN8)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
#define IsWin8p1AndAbove()	true
#else
#define IsWin8p1AndAbove()	(g_uWinVer >= _WIN32_WINNT_WINBLUE)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WIN10
#define IsWin10AndAbove()	true
#else
#define IsWin10AndAbove()	(g_uWinVer >= _WIN32_WINNT_WIN10)
#endif

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32	0x00000800
#endif
#ifndef LOAD_LIBRARY_AS_IMAGE_RESOURCE
#define LOAD_LIBRARY_AS_IMAGE_RESOURCE	0x00000020
#endif

#if (defined(__GNUC__) && __GNUC__ >= 8) || (defined(__clang__) && __clang_major__ >= 18)
// GCC statement expression
#define DLLFunction(funcSig, hModule, funcName) __extension__({			\
	_Pragma("GCC diagnostic push")										\
	_Pragma("GCC diagnostic ignored \"-Wcast-function-type\"")			\
	funcSig PP_CONCAT(temp, __LINE__) = (funcSig)GetProcAddress((hModule), (funcName));\
	_Pragma("GCC diagnostic pop")										\
	PP_CONCAT(temp, __LINE__);											\
	})
#define DLLFunctionEx(funcSig, dllName, funcName) __extension__({		\
	_Pragma("GCC diagnostic push")										\
	_Pragma("GCC diagnostic ignored \"-Wcast-function-type\"")			\
	funcSig PP_CONCAT(temp, __LINE__) = (funcSig)GetProcAddress(GetModuleHandleW(dllName), (funcName));\
	_Pragma("GCC diagnostic pop")										\
	PP_CONCAT(temp, __LINE__);											\
	})
#else
#define DLLFunction(funcSig, hModule, funcName)		(funcSig)GetProcAddress((hModule), (funcName))
#define DLLFunctionEx(funcSig, dllName, funcName)	(funcSig)GetProcAddress(GetModuleHandleW(dllName), (funcName))
#endif

// High DPI Reference
// https://docs.microsoft.com/en-us/windows/desktop/hidpi/high-dpi-reference
#ifndef WM_DPICHANGED
#define WM_DPICHANGED	0x02E0				// _WIN32_WINNT >= _WIN32_WINNT_WIN7
#endif
#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI		96		// _WIN32_WINNT >= _WIN32_WINNT_VISTA
#endif

// use large icon when window DPI is greater than or equal to this value.
#define NP2_LARGER_ICON_SIZE_DPI	192		// 200%

// current DPI for main/editor window
extern UINT g_uCurrentDPI;
// system DPI, same for all monitor.
extern UINT g_uSystemDPI;

// since Windows 10, version 1607
#if (defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)) && !defined(__MINGW32__)
// 1709 was the first version for Windows 10 on ARM64.
#define NP2_HAS_GETDPIFORWINDOW				1
#define GetWindowDPI(hwnd)					GetDpiForWindow(hwnd)
#define SystemMetricsForDpi(nIndex, dpi)	GetSystemMetricsForDpi((nIndex), (dpi))
#define AdjustWindowRectForDpi(lpRect, dwStyle, dwExStyle, dpi) \
		AdjustWindowRectExForDpi((lpRect), (dwStyle), FALSE, (dwExStyle), (dpi))

#else
#define NP2_HAS_GETDPIFORWINDOW				0
extern UINT GetWindowDPI(HWND hwnd) NP2_noexcept;
extern int SystemMetricsForDpi(int nIndex, UINT dpi) NP2_noexcept;
extern BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) NP2_noexcept;
#endif

#if defined(NP2_ENABLE_HIDPI_IMAGE_RESOURCE) && NP2_ENABLE_HIDPI_IMAGE_RESOURCE
NP2_inline int GetBitmapResourceIdForCurrentDPI(int resourceId)	{
	if (g_uCurrentDPI > USER_DEFAULT_SCREEN_DPI + USER_DEFAULT_SCREEN_DPI/4) {
		int scale = (g_uCurrentDPI + USER_DEFAULT_SCREEN_DPI/4 - 1) / (USER_DEFAULT_SCREEN_DPI/2);
		scale = min_i(scale, 6);
		resourceId += scale - 2;
	}
	return resourceId;
}
#else
#define GetBitmapResourceIdForCurrentDPI(resourceId)	(resourceId)
#endif

// https://docs.microsoft.com/en-us/windows/desktop/Memory/comparing-memory-allocation-methods
// https://blogs.msdn.microsoft.com/oldnewthing/20120316-00/?p=8083/
#define NP2HeapAlloc(size)			HeapAlloc(g_hDefaultHeap, HEAP_ZERO_MEMORY, (size))
#define NP2HeapFree(hMem)			HeapFree(g_hDefaultHeap, 0, (hMem))
#define NP2HeapSize(hMem)			HeapSize(g_hDefaultHeap, 0, (hMem))

extern WCHAR szIniFile[MAX_PATH];
#define IniGetString(lpSection, lpName, lpDefault, lpReturnedStr, nSize) \
	GetPrivateProfileString(lpSection, lpName, lpDefault, lpReturnedStr, nSize, szIniFile)
#define IniGetInt(lpSection, lpName, nDefault) \
	GetPrivateProfileInt(lpSection, lpName, nDefault, szIniFile)
#define IniSetString(lpSection, lpName, lpString) \
	WritePrivateProfileString(lpSection, lpName, lpString, szIniFile)

void IniClearSectionEx(LPCWSTR lpSection, LPCWSTR lpszIniFile, bool bDelete);
#define IniClearSection(lpSection)			IniClearSectionEx((lpSection), szIniFile, false)
#define IniDeleteSection(lpSection) 		IniClearSectionEx((lpSection), szIniFile, true)

void IniClearAllSectionEx(LPCWSTR lpszPrefix, LPCWSTR lpszIniFile, bool bDelete);
#define IniClearAllSection(lpszPrefix)		IniClearAllSectionEx((lpszPrefix), szIniFile, false)
#define IniDeleteAllSection(lpszPrefix)		IniClearAllSectionEx((lpszPrefix), szIniFile, true)

NP2_inline void IniSetInt(LPCWSTR lpSection, LPCWSTR lpName, int i) {
	WCHAR tch[16];
	_ltow(i, tch, 10);
	IniSetString(lpSection, lpName, tch);
}

NP2_inline void IniSetBool(LPCWSTR lpSection, LPCWSTR lpName, bool b) {
	IniSetString(lpSection, lpName, (b ? L"1" : L"0"));
}

NP2_inline void IniSetIntEx(LPCWSTR lpSection, LPCWSTR lpName, int i, int iDefault) {
	if (i != iDefault) {
		IniSetInt(lpSection, lpName, i);
	} else {
		IniSetString(lpSection, lpName, NULL);
	}
}

NP2_inline void IniSetBoolEx(LPCWSTR lpSection, LPCWSTR lpName, bool b, bool bDefault) {
	IniSetString(lpSection, lpName, (b == bDefault) ? NULL : (b ? L"1" : L"0"));
}

#define LoadIniSection(lpSection, lpBuf, cchBuf) \
	GetPrivateProfileSection(lpSection, lpBuf, cchBuf, szIniFile);
#define SaveIniSection(lpSection, lpBuf) \
	WritePrivateProfileSection(lpSection, lpBuf, szIniFile)

struct IniKeyValueNode;
typedef struct IniKeyValueNode {
	struct IniKeyValueNode *next;
	UINT hash;
	LPCWSTR key;
	LPCWSTR value;
} IniKeyValueNode;

// https://en.wikipedia.org/wiki/Sentinel_node
// https://en.wikipedia.org/wiki/Sentinel_value
#define IniSectionImplUseSentinelNode	1

typedef struct IniSection {
	UINT count;
	UINT capacity;
	IniKeyValueNode *head;
#if IniSectionImplUseSentinelNode
	IniKeyValueNode *sentinel;
#endif
	IniKeyValueNode *nodeList;
} IniSection;

NP2_inline void IniSectionInit(IniSection *section, UINT capacity) {
	section->count = 0;
	section->capacity = capacity;
	section->head = NULL;
#if IniSectionImplUseSentinelNode
	section->nodeList = (IniKeyValueNode *)NP2HeapAlloc((capacity + 1) * sizeof(IniKeyValueNode));
	section->sentinel = &section->nodeList[capacity];
#else
	section->nodeList = (IniKeyValueNode *)NP2HeapAlloc(capacity * sizeof(IniKeyValueNode));
#endif
}

NP2_inline void IniSectionFree(IniSection *section) {
	NP2HeapFree(section->nodeList);
}

NP2_inline void IniSectionClear(IniSection *section) {
	section->count = 0;
	section->head = NULL;
}

NP2_inline bool IniSectionIsEmpty(const IniSection *section) {
	return section->count == 0;
}

bool IniSectionParseArray(IniSection *section, LPWSTR lpCachedIniSection);
bool IniSectionParse(IniSection *section, LPWSTR lpCachedIniSection);
LPCWSTR IniSectionUnsafeGetValue(IniSection *section, LPCWSTR key, int keyLen);

NP2_inline LPCWSTR IniSectionGetValueImpl(IniSection *section, LPCWSTR key, int keyLen) {
	return section->count ? IniSectionUnsafeGetValue(section, key, keyLen) : NULL;
}

void IniSectionGetStringImpl(IniSection *section, LPCWSTR key, int keyLen, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString);
int IniSectionGetIntImpl(IniSection *section, LPCWSTR key, int keyLen, int iDefault);
bool IniSectionGetBoolImpl(IniSection *section, LPCWSTR key, int keyLen, bool bDefault);

#define IniSectionGetValue(section, key) \
	IniSectionGetValueImpl(section, key, CSTRLEN(key))
#define IniSectionGetInt(section, key, iDefault) \
	IniSectionGetIntImpl(section, key, CSTRLEN(key), (iDefault))
#define IniSectionGetBool(section, key, bDefault) \
	IniSectionGetBoolImpl(section, key, CSTRLEN(key), (bDefault))
#define IniSectionGetString(section, key, lpDefault, lpReturnedString, cchReturnedString) \
	IniSectionGetStringImpl(section, key, CSTRLEN(key), (lpDefault), (lpReturnedString), (cchReturnedString))

NP2_inline LPCWSTR IniSectionGetValueEx(IniSection *section, LPCWSTR key) {
	return IniSectionGetValueImpl(section, key, 0);
}

NP2_inline int IniSectionGetIntEx(IniSection *section, LPCWSTR key, int iDefault) {
	return IniSectionGetIntImpl(section, key, 0, iDefault);
}

NP2_inline bool IniSectionGetBoolEx(IniSection *section, LPCWSTR key, bool bDefault) {
	return IniSectionGetBoolImpl(section, key, 0, bDefault);
}

NP2_inline void IniSectionGetStringEx(IniSection *section, LPCWSTR key, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString) {
	IniSectionGetStringImpl(section, key, 0, lpDefault, lpReturnedString, cchReturnedString);
}

typedef struct IniSectionOnSave {
	LPWSTR next;
} IniSectionOnSave;

void IniSectionSetString(IniSectionOnSave *section, LPCWSTR key, LPCWSTR value);

NP2_inline void IniSectionSetInt(IniSectionOnSave *section, LPCWSTR key, int i) {
	WCHAR tch[16];
	_ltow(i, tch, 10);
	IniSectionSetString(section, key, tch);
}

NP2_inline void IniSectionSetBool(IniSectionOnSave *section, LPCWSTR key, bool b) {
	IniSectionSetString(section, key, (b ? L"1" : L"0"));
}

NP2_inline void IniSectionSetStringEx(IniSectionOnSave *section, LPCWSTR key, LPCWSTR value, LPCWSTR lpDefault) {
	if (!StrCaseEqual(value, lpDefault)) {
		IniSectionSetString(section, key, value);
	}
}

NP2_inline void IniSectionSetIntEx(IniSectionOnSave *section, LPCWSTR key, int i, int iDefault) {
	if (i != iDefault) {
		IniSectionSetInt(section, key, i);
	}
}

NP2_inline void IniSectionSetBoolEx(IniSectionOnSave *section, LPCWSTR key, bool b, bool bDefault) {
	if (b != bDefault) {
		IniSectionSetString(section, key, (b ? L"1" : L"0"));
	}
}

#define NP2RegSubKey_ContextMenu	L"Folder\\shell\\metapath"
#define NP2RegSubKey_JumpList		L"Applications\\metapath.exe"

LPWSTR Registry_GetString(HKEY hKey, LPCWSTR valueName);
LSTATUS Registry_SetString(HKEY hKey, LPCWSTR valueName, LPCWSTR lpszText);
#define Registry_GetDefaultString(hKey)				Registry_GetString((hKey), NULL)
#define Registry_SetDefaultString(hKey, lpszText)	Registry_SetString((hKey), NULL, (lpszText))
NP2_inline LSTATUS Registry_CreateKey(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult) {
	return RegCreateKeyEx(hKey, lpSubKey, 0, NULL, 0, KEY_WRITE, NULL, phkResult, NULL);
}
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#define Registry_DeleteTree(hKey, lpSubKey)			RegDeleteTree((hKey), (lpSubKey))
#else
LSTATUS Registry_DeleteTree(HKEY hKey, LPCWSTR lpSubKey);
#endif


NP2_inline bool KeyboardIsKeyDown(int key) {
	return (GetKeyState(key) & 0x8000) != 0;
}

NP2_inline void BeginWaitCursor(void) {
	DestroyCursor(SetCursor(LoadCursor(NULL, IDC_WAIT)));
}

NP2_inline void EndWaitCursor(void) {
	DestroyCursor(SetCursor(LoadCursor(NULL, IDC_ARROW)));
}

typedef struct BackgroundWorker {
	HWND hwnd;
	HANDLE eventCancel;
	HANDLE workerThread;
} BackgroundWorker;

void BackgroundWorker_Init(BackgroundWorker *worker, HWND hwnd);
void BackgroundWorker_Cancel(BackgroundWorker *worker);
void BackgroundWorker_Destroy(BackgroundWorker *worker);
#define BackgroundWorker_Continue(worker)	\
	(WaitForSingleObject((worker)->eventCancel, 0) != WAIT_OBJECT_0)

HRESULT PrivateSetCurrentProcessExplicitAppUserModelID(PCWSTR AppID);
bool IsElevated(void);
bool ExeNameFromWnd(HWND hwnd, LPWSTR szExeName, DWORD cchExeName);
//bool Is32bitExe(LPCWSTR lpszExeName);

#define SetExplorerTheme(hwnd)		SetWindowTheme((hwnd), L"Explorer", NULL)
#define SetListViewTheme(hwnd)		SetWindowTheme((hwnd), L"Listview", NULL)

bool FindUserResourcePath(LPCWSTR path, LPWSTR outPath);
HBITMAP LoadBitmapFile(LPCWSTR path);
HBITMAP ResizeImageForCurrentDPI(HBITMAP hbmp);

bool BitmapMergeAlpha(HBITMAP hbmp, COLORREF crDest);
bool BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha);
bool BitmapGrayScale(HBITMAP hbmp);

void CenterDlgInParentEx(HWND hDlg, HWND hParent);
NP2_inline void CenterDlgInParent(HWND hDlg) {
	CenterDlgInParentEx(hDlg, GetParent(hDlg));
}
void SnapToDefaultButton(HWND hwndBox);

void GetDlgPos(HWND hDlg, LPINT xDlg, LPINT yDlg);
void SetDlgPos(HWND hDlg, int xDlg, int yDlg);

#define ResizeDlgDirection_Both		0
#define ResizeDlgDirection_OnlyX	1
#define ResizeDlgDirection_OnlyY	2
void ResizeDlg_InitEx(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int iDirection);
NP2_inline void ResizeDlg_Init(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip) {
	ResizeDlg_InitEx(hwnd, cxFrame, cyFrame, nIdGrip, ResizeDlgDirection_Both);
}
NP2_inline void ResizeDlg_InitX(HWND hwnd, int cxFrame, int nIdGrip) {
	ResizeDlg_InitEx(hwnd, cxFrame, 0, nIdGrip, ResizeDlgDirection_OnlyX);
}
NP2_inline void ResizeDlg_InitY(HWND hwnd, int cyFrame, int nIdGrip) {
	ResizeDlg_InitEx(hwnd, 0, cyFrame, nIdGrip, ResizeDlgDirection_OnlyY);
}
void ResizeDlg_Destroy(HWND hwnd, int *cxFrame, int *cyFrame);
void ResizeDlg_Size(HWND hwnd, LPARAM lParam, int *cx, int *cy);
void ResizeDlg_GetMinMaxInfo(HWND hwnd, LPARAM lParam);
HDWP DeferCtlPos(HDWP hdwp, HWND hwndDlg, int nCtlId, int dx, int dy, UINT uFlags);
void ResizeDlgCtl(HWND hwndDlg, int nCtlId, int dx, int dy);
void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, int wBmpId);
void DeleteBitmapButton(HWND hwnd, int nCtlId);
void SetClipData(HWND hwnd, LPCWSTR pszData);
void SetWindowTransparentMode(HWND hwnd, bool bTransparentMode, int iOpacityLevel);
void SetWindowLayoutRTL(HWND hwnd, bool bRTL);

#define SendWMCommandEx(hwnd, id, extra)	SendMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), (extra)), 0)
#define SendWMCommand(hwnd, id)				SendWMCommandEx(hwnd, (id), 1)
#define PostWMCommand(hwnd, id)				PostMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), 1), 0)

#define SetWindowStyle(hwnd, style)			SetWindowLong(hwnd, GWL_STYLE, (style))
#define SetWindowExStyle(hwnd, style)		SetWindowLong(hwnd, GWL_EXSTYLE, (style))

#define ComboBox_HasText(hwnd)					(ComboBox_GetTextLength(hwnd) || CB_ERR != ComboBox_GetCurSel(hwnd))
#define ComboBox_GetEditSelStart(hwnd)			LOWORD(ComboBox_GetEditSel(hwnd))
#define ComboBox_GetEditSelEnd(hwnd)			HIWORD(ComboBox_GetEditSel(hwnd))

#define StatusSetSimple(hwnd, b)				SendMessage(hwnd, SB_SIMPLE, (b), 0)
#define StatusSetText(hwnd, nPart, lpszText)	SendMessage(hwnd, SB_SETTEXT, (nPart), (LPARAM)(lpszText))

/**
 * we only have 14 commands in toolbar
 * max size = 14*(3 + 2) + 1 (each command with a separator)
 */
#define MAX_TOOLBAR_ITEM_COUNT_WITH_SEPARATOR	50
#define MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE	128
int Toolbar_GetButtons(HWND hwnd, int cmdBase, LPWSTR lpszButtons, int cchButtons);
int Toolbar_SetButtons(HWND hwnd, LPCWSTR lpszButtons, LPCTBBUTTON ptbb, int ctbb);
void Toolbar_SetButtonImage(HWND hwnd, int idCommand, int iImage);

LRESULT SendWMSize(HWND hwnd);

#define EnableCmd(hmenu, id, b) EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_GRAYED))
#define CheckCmd(hmenu, id, b)  CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED))
#define DisableCmd(hmenu, id, b)	EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_GRAYED) : (MF_BYCOMMAND | MF_ENABLED))
#define UncheckCmd(hmenu, id, b)	CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_UNCHECKED) : (MF_BYCOMMAND | MF_CHECKED))

#define IsButtonChecked(hwnd, uId)	(IsDlgButtonChecked(hwnd, (uId)) == BST_CHECKED)

HMODULE LoadLocalizedResourceDLL(LANGID lang, LPCWSTR dllName);
NP2_inline bool IsChineseTraditionalSubLang(LANGID subLang) {
	return subLang == SUBLANG_CHINESE_TRADITIONAL
		|| subLang == SUBLANG_CHINESE_HONGKONG
		|| subLang == SUBLANG_CHINESE_MACAU;
}

#define GetString(id, pb, cb)	LoadString(g_hInstance, id, pb, cb)
#define StrEnd(pStart)			((pStart) + lstrlen(pStart))

/**
 * Variadic Macros
 * use __VA_ARGS__ instead of ##__VA_ARGS__ to force GCC syntax error
 * for trailing comma when no format argument is given.
 * https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 * https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview
 */
#define FormatString(lpOutput, lpFormat, uIdFormat, ...) do {	\
		GetString((uIdFormat), (lpFormat), COUNTOF(lpFormat));	\
		wsprintf((lpOutput), (lpFormat), __VA_ARGS__);			\
	} while (0)

NP2_inline bool PathIsFile(LPCWSTR pszPath) {
	// note: INVALID_FILE_ATTRIBUTES is -1.
	return (GetFileAttributes(pszPath) & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

NP2_inline bool PathIsSymbolicLink(LPCWSTR pszPath) {
	// assume file exists, no check for INVALID_FILE_ATTRIBUTES.
	return (GetFileAttributes(pszPath) & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
}

// https://docs.microsoft.com/en-us/windows/win32/intl/handling-sorting-in-your-applications#sort-strings-ordinally
NP2_inline bool PathEqual(LPCWSTR pszPath1, LPCWSTR pszPath2) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	// the function maps case using the operating system uppercasing table
	return CompareStringOrdinal(pszPath1, -1, pszPath2, -1, TRUE) == CSTR_EQUAL;
#else
	return CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, pszPath1, -1, pszPath2, -1) == CSTR_EQUAL;
#endif
}

// similar to realpath() and std::filesystem::canonical()
bool PathGetRealPath(HANDLE hFile, LPCWSTR lpszSrc, LPWSTR lpszDest);
NP2_inline void GetProgramRealPath(LPWSTR tchModule, DWORD nSize) {
	GetModuleFileName(NULL, tchModule, nSize);
	// for symbolic link, module path is link's path not target's.
	if (PathIsSymbolicLink(tchModule)) {
		PathGetRealPath(NULL, tchModule, tchModule);
	}
}

void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, DWORD dwAttrTo, bool bUnexpandEnv, bool bUnexpandMyDocs);
void PathAbsoluteFromApp(LPCWSTR lpszSrc, LPWSTR lpszDest, bool bExpandEnv);
bool PathGetLnkPath(LPCWSTR pszLnkFile, LPWSTR pszResPath);
bool PathCreateLnk(LPCWSTR pszLnkDir, LPCWSTR pszPath);
void OpenContainingFolder(HWND hwnd, LPCWSTR pszFile, bool bSelect);

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#if defined(__cplusplus)
#define KnownFolderId_Desktop			FOLDERID_Desktop
#define KnownFolderId_Documents			FOLDERID_Documents
#define KnownFolderId_ComputerFolder	FOLDERID_ComputerFolder
#else
#define KnownFolderId_Desktop			(&FOLDERID_Desktop)
#define KnownFolderId_Documents			(&FOLDERID_Documents)
#define KnownFolderId_ComputerFolder	(&FOLDERID_ComputerFolder)
#endif
#endif

NP2_inline void TrimString(LPWSTR lpString) {
	StrTrim(lpString, L" ");
}

bool ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2);
void PrepareFilterStr(LPWSTR lpFilter);
void StrTab2Space(LPWSTR lpsz);
bool PathFixBackslashes(LPWSTR lpsz);
void ExpandEnvironmentStringsEx(LPWSTR lpSrc, DWORD dwSrc);
bool SearchPathEx(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer);
void FormatNumber(LPWSTR lpNumberStr, UINT value);

void GetDefaultFavoritesDir(LPWSTR lpFavDir, int cchFavDir);
void GetDefaultOpenWithDir(LPWSTR lpOpenWithDir, int cchOpenWithDir);

HDROP CreateDropHandle(LPCWSTR lpFileName);

bool ExecDDECommand(LPCWSTR lpszCmdLine, LPCWSTR lpszDDEMsg, LPCWSTR lpszDDEApp, LPCWSTR lpszDDETopic);

//==== History Functions ======================================================
#define HISTORY_ITEMS 50

typedef struct HISTORY {
	int  iCurItem;            // Current Item
	WCHAR *psz[HISTORY_ITEMS]; // Strings
} HISTORY, *PHISTORY, *LPHISTORY;

typedef const HISTORY *LCPHISTORY;

void History_Init(PHISTORY ph);
void History_Empty(PHISTORY ph);
bool History_Add(PHISTORY ph, LPCWSTR pszNew);
bool History_Forward(PHISTORY ph, LPWSTR pszItem, int cItem);
bool History_Back(PHISTORY ph, LPWSTR pszItem, int cItem);
bool History_CanForward(LCPHISTORY ph);
bool History_CanBack(LCPHISTORY ph);
void History_UpdateToolbar(LCPHISTORY ph, HWND hwnd, int cmdBack, int cmdForward);

//==== MRU Functions ==========================================================
#define MRU_MAXITEMS	24

enum {
	MRUFlags_Default = 0,
	MRUFlags_FilePath = 1,
};

// MRU_MAXITEMS * (MAX_PATH + 4)
#define MAX_INI_SECTION_SIZE_MRU	(8 * 1024)

typedef struct MRULIST {
	int		iSize;
	int		iFlags;
	LPCWSTR szRegKey;
	LPWSTR pszItems[MRU_MAXITEMS];
} MRULIST, *PMRULIST, *LPMRULIST;

typedef const MRULIST *LPCMRULIST;

void MRU_Init(LPMRULIST pmru, LPCWSTR pszRegKey, int iFlags);
void MRU_Add(LPMRULIST pmru, LPCWSTR pszNew);
void MRU_Delete(LPMRULIST pmru, int iIndex);
void MRU_Empty(LPMRULIST pmru, bool save);
void MRU_Load(LPMRULIST pmru);
void MRU_Save(LPCMRULIST pmru);
void MRU_AddToCombobox(LPCMRULIST pmru, HWND hwnd);

//==== Themed Dialogs =========================================================
#ifndef DLGTEMPLATEEX
#pragma pack(push, 1)
typedef struct {
	WORD      dlgVer;
	WORD      signature;
	DWORD     helpID;
	DWORD     exStyle;
	DWORD     style;
	WORD      cDlgItems;
	short     x;
	short     y;
	short     cx;
	short     cy;
} DLGTEMPLATEEX;
#pragma pack(pop)
#endif

bool GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize);
DLGTEMPLATE *LoadThemedDialogTemplate(LPCWSTR lpDialogTemplateID, HINSTANCE hInstance);
#define ThemedDialogBox(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
	ThemedDialogBoxParam(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0)
INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);

//==== File Dialog Hook =========================================================
UINT_PTR CALLBACK OpenSaveFileDlgHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==== MinimizeToTray Functions - see comments in Helpers.c ===================
bool GetDoAnimateMinimize(void);
void MinimizeWndToTray(HWND hwnd);
void RestoreWndFromTray(HWND hwnd);
