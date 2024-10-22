/******************************************************************************
*
*
* matepath - The universal Explorer-like Plugin
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

#include <cstdint>
#include "compiler.h"

template <typename T>
constexpr T min(T x, T y) noexcept {
	return (x < y) ? x : y;
}

template <typename T>
constexpr T max(T x, T y) noexcept {
	return (x > y) ? x : y;
}

template <class T>
constexpr T clamp(T x, T lower, T upper) noexcept {
	return (x < lower) ? lower : (x > upper) ? upper : x;
}

template <typename T>
constexpr T validate(T x, T lower, T upper, T defaultValue) noexcept {
	return (x < lower || x > upper) ? defaultValue : x;
}

constexpr bool StrIsEmpty(LPCWSTR s) noexcept {
	return s == nullptr || *s == L'\0';
}

constexpr bool StrNotEmpty(LPCWSTR s) noexcept {
	return s != nullptr && *s != L'\0';
}

constexpr int ToUpperA(int ch) noexcept {
	return (ch >= 'a' && ch <= 'z') ? (ch - 'a' + 'A') : ch;
}

constexpr int ToLowerA(int ch) noexcept {
	return (ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch;
}

inline bool StrEqual(LPCWSTR s1, LPCWSTR s2) noexcept {
	return wcscmp(s1, s2) == 0;
}

inline bool StrCaseEqual(LPCWSTR s1, LPCWSTR s2) noexcept {
	return _wcsicmp(s1, s2) == 0;
}

template <typename T, size_t N>
inline void StrCpyEx(T *s, const T (&t)[N]) noexcept {
	memcpy(s, t, N*sizeof(T));
}

template <typename T, size_t N>
constexpr bool StrEqualEx(const T *s, const T (&t)[N]) noexcept {
	return __builtin_memcmp(s, t, N*sizeof(T)) == 0;
}

template <typename T, size_t N>
constexpr bool StrStartsWith(const T *s, const T (&t)[N]) noexcept {
	return __builtin_memcmp(s, t, (N - 1)*sizeof(T)) == 0;
}

inline bool CRTStrToInt(LPCWSTR str, int *value) noexcept {
	LPWSTR end;
	*value = static_cast<int>(wcstol(str, &end, 10));
	return str != end;
}

int ParseCommaList(LPCWSTR str, int result[], int count) noexcept;

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

template<typename T>
inline T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept {
	FARPROC function = ::GetProcAddress(hModule, lpProcName);
#if defined(__clang__) || defined(__GNUC__) || (_MSC_VER >= 1926)
	return __builtin_bit_cast(T, function);
#else
	static_assert(sizeof(T) == sizeof(function));
	T fp {};
	memcpy(&fp, &function, sizeof(T));
	return fp;
#endif
}

template<typename T>
inline T DLLFunctionEx(LPCWSTR lpDllName, LPCSTR lpProcName) noexcept {
	return DLLFunction<T>(::GetModuleHandleW(lpDllName), lpProcName);
}

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
UINT GetWindowDPI(HWND hwnd) noexcept;
int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept;
BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) noexcept;
#endif

#if defined(NP2_ENABLE_HIDPI_IMAGE_RESOURCE) && NP2_ENABLE_HIDPI_IMAGE_RESOURCE
inline int GetBitmapResourceIdForDPI(int resourceId, UINT dpi) noexcept {
	if (dpi > USER_DEFAULT_SCREEN_DPI + USER_DEFAULT_SCREEN_DPI/4) {
		int scale = (dpi + USER_DEFAULT_SCREEN_DPI/4 - 1) / (USER_DEFAULT_SCREEN_DPI/2);
		scale = min(scale, 6);
		resourceId += scale - 2;
	}
	return resourceId;
}
inline int GetBitmapResourceIdForCurrentDPI(int resourceId) noexcept {
	return GetBitmapResourceIdForDPI(resourceId, g_uCurrentDPI);
}
#else
#define GetBitmapResourceIdForDPI(resourceId, dpi)		(resourceId)
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

void IniClearSectionEx(LPCWSTR lpSection, LPCWSTR lpszIniFile, bool bDelete) noexcept;
#define IniClearSection(lpSection)			IniClearSectionEx((lpSection), szIniFile, false)
#define IniDeleteSection(lpSection) 		IniClearSectionEx((lpSection), szIniFile, true)

void IniClearAllSectionEx(LPCWSTR lpszPrefix, LPCWSTR lpszIniFile, bool bDelete) noexcept;
#define IniClearAllSection(lpszPrefix)		IniClearAllSectionEx((lpszPrefix), szIniFile, false)
#define IniDeleteAllSection(lpszPrefix)		IniClearAllSectionEx((lpszPrefix), szIniFile, true)

inline void IniSetInt(LPCWSTR lpSection, LPCWSTR lpName, int i) noexcept {
	WCHAR tch[16];
	_ltow(i, tch, 10);
	IniSetString(lpSection, lpName, tch);
}

inline void IniSetBool(LPCWSTR lpSection, LPCWSTR lpName, bool b) noexcept {
	IniSetString(lpSection, lpName, (b ? L"1" : L"0"));
}

inline void IniSetIntEx(LPCWSTR lpSection, LPCWSTR lpName, int i, int iDefault) noexcept {
	if (i != iDefault) {
		IniSetInt(lpSection, lpName, i);
	} else {
		IniSetString(lpSection, lpName, nullptr);
	}
}

inline void IniSetBoolEx(LPCWSTR lpSection, LPCWSTR lpName, bool b, bool bDefault) noexcept {
	IniSetString(lpSection, lpName, (b == bDefault) ? nullptr : (b ? L"1" : L"0"));
}

#define LoadIniSection(lpSection, lpBuf, cchBuf) \
	GetPrivateProfileSection(lpSection, lpBuf, cchBuf, szIniFile);
#define SaveIniSection(lpSection, lpBuf) \
	WritePrivateProfileSection(lpSection, lpBuf, szIniFile)

struct IniKeyValueNode {
	IniKeyValueNode *next;
	UINT hash;
	LPCWSTR key;
	LPCWSTR value;
};

// https://en.wikipedia.org/wiki/Sentinel_node
// https://en.wikipedia.org/wiki/Sentinel_value
#define IniSectionParserUseSentinelNode	1

struct IniSectionParser {
	UINT count;
	UINT capacity;
	IniKeyValueNode *head;
#if IniSectionParserUseSentinelNode
	IniKeyValueNode *sentinel;
#endif
	IniKeyValueNode *nodeList;

	void Init(UINT capacity_) noexcept;
	void Free() const noexcept {
		NP2HeapFree(nodeList);
	}
	void Clear() noexcept {
		count = 0;
		head = nullptr;
	}
	bool ParseArray(LPWSTR lpCachedIniSection) noexcept;
	bool Parse(LPWSTR lpCachedIniSection) noexcept;
	LPCWSTR UnsafeGetValue(LPCWSTR key, int keyLen) noexcept;
	LPCWSTR GetValueImpl(LPCWSTR key, int keyLen) noexcept {
		return count ? UnsafeGetValue(key, keyLen) : nullptr;
	}
	void GetStringImpl(LPCWSTR key, int keyLen, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString) noexcept;
	int GetIntImpl(LPCWSTR key, int keyLen, int iDefault) noexcept;
	bool GetBoolImpl(LPCWSTR key, int keyLen, bool bDefault) noexcept;

	template <size_t N>
	LPCWSTR GetValue(const wchar_t (&key)[N]) noexcept {
		return GetValueImpl(key, static_cast<int>(N - 1));
	}
	template <size_t N>
	int GetInt(const wchar_t (&key)[N], int iDefault) noexcept {
		return GetIntImpl(key, static_cast<int>(N - 1), iDefault);
	}
	template <size_t N>
	bool GetBool(const wchar_t (&key)[N], bool bDefault) noexcept {
		return GetBoolImpl(key, static_cast<int>(N - 1), bDefault);
	}
	template <size_t N, size_t M>
	void GetString(const wchar_t (&key)[N], LPCWSTR lpDefault, wchar_t (&lpReturnedString)[M]) noexcept {
		GetStringImpl(key, static_cast<int>(N - 1), lpDefault, lpReturnedString, static_cast<int>(M));
	}
};

struct IniSectionBuilder {
	LPWSTR next;
	void SetString(LPCWSTR key, LPCWSTR value) noexcept;
	void SetInt(LPCWSTR key, int i) noexcept {
		WCHAR tch[16];
		_ltow(i, tch, 10);
		SetString(key, tch);
	}
	void SetBool(LPCWSTR key, bool b) noexcept {
		SetString(key, (b ? L"1" : L"0"));
	}
	void SetStringEx(LPCWSTR key, LPCWSTR value, LPCWSTR lpDefault) noexcept {
		if (!StrCaseEqual(value, lpDefault)) {
			SetString(key, value);
		}
	}
	void SetIntEx(LPCWSTR key, int i, int iDefault) noexcept {
		if (i != iDefault) {
			SetInt(key, i);
		}
	}
	void SetBoolEx(LPCWSTR key, bool b, bool bDefault) noexcept {
		if (b != bDefault) {
			SetString(key, (b ? L"1" : L"0"));
		}
	}
};

#define NP2RegSubKey_ContextMenu	L"Folder\\shell\\matepath"
#define NP2RegSubKey_JumpList		L"Applications\\matepath.exe"

LPWSTR Registry_GetString(HKEY hKey, LPCWSTR valueName) noexcept;
LSTATUS Registry_SetString(HKEY hKey, LPCWSTR valueName, LPCWSTR lpszText) noexcept;
#define Registry_GetDefaultString(hKey)				Registry_GetString((hKey), nullptr)
#define Registry_SetDefaultString(hKey, lpszText)	Registry_SetString((hKey), nullptr, (lpszText))
inline LSTATUS Registry_CreateKey(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult) noexcept {
	return RegCreateKeyEx(hKey, lpSubKey, 0, nullptr, 0, KEY_WRITE, nullptr, phkResult, nullptr);
}
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#define Registry_DeleteTree(hKey, lpSubKey)			RegDeleteTree((hKey), (lpSubKey))
#else
LSTATUS Registry_DeleteTree(HKEY hKey, LPCWSTR lpSubKey) noexcept;
#endif


inline bool KeyboardIsKeyDown(int key) noexcept {
	return (GetKeyState(key) & 0x8000) != 0;
}

inline void BeginWaitCursor() noexcept {
	DestroyCursor(SetCursor(LoadCursor(nullptr, IDC_WAIT)));
}

inline void EndWaitCursor() noexcept {
	DestroyCursor(SetCursor(LoadCursor(nullptr, IDC_ARROW)));
}

struct BackgroundWorker {
	HWND hwnd;
	HANDLE eventCancel;
	HANDLE workerThread;

	void Init(HWND owner) noexcept;
	void Stop() noexcept;
	void Cancel() noexcept;
	void Destroy() noexcept;
	bool Continue() const noexcept {
		return WaitForSingleObject(eventCancel, 0) != WAIT_OBJECT_0;
	}
};

HRESULT PrivateSetCurrentProcessExplicitAppUserModelID(LPCWSTR AppID) noexcept;
bool IsElevated() noexcept;
bool ExeNameFromWnd(HWND hwnd, LPWSTR szExeName, DWORD cchExeName) noexcept;
//bool Is32bitExe(LPCWSTR lpszExeName) noexcept;

#define SetExplorerTheme(hwnd)		SetWindowTheme((hwnd), L"Explorer", nullptr)
#define SetListViewTheme(hwnd)		SetWindowTheme((hwnd), L"Listview", nullptr)

bool FindUserResourcePath(LPCWSTR path, LPWSTR outPath) noexcept;
HBITMAP LoadBitmapFile(LPCWSTR path) noexcept;
HBITMAP ResizeImageForDPI(HBITMAP hbmp, UINT dpi) noexcept;
inline HBITMAP ResizeImageForCurrentDPI(HBITMAP hbmp) noexcept {
	return ResizeImageForDPI(hbmp, g_uCurrentDPI);
}

bool BitmapMergeAlpha(HBITMAP hbmp, COLORREF crDest) noexcept;
bool BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha) noexcept;
bool BitmapGrayScale(HBITMAP hbmp) noexcept;

void CenterDlgInParentEx(HWND hDlg, HWND hParent) noexcept;
inline void CenterDlgInParent(HWND hDlg) noexcept {
	CenterDlgInParentEx(hDlg, GetParent(hDlg));
}
void SnapToDefaultButton(HWND hwndBox) noexcept;

void GetDlgPos(HWND hDlg, LPINT xDlg, LPINT yDlg) noexcept;
void SetDlgPos(HWND hDlg, int xDlg, int yDlg) noexcept;

#define ResizeDlgDirection_Both		0
#define ResizeDlgDirection_OnlyX	1
#define ResizeDlgDirection_OnlyY	2
void ResizeDlg_InitEx(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int iDirection) noexcept;
inline void ResizeDlg_Init(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip) noexcept {
	ResizeDlg_InitEx(hwnd, cxFrame, cyFrame, nIdGrip, ResizeDlgDirection_Both);
}
inline void ResizeDlg_InitX(HWND hwnd, int cxFrame, int nIdGrip) noexcept {
	ResizeDlg_InitEx(hwnd, cxFrame, 0, nIdGrip, ResizeDlgDirection_OnlyX);
}
inline void ResizeDlg_InitY(HWND hwnd, int cyFrame, int nIdGrip) noexcept {
	ResizeDlg_InitEx(hwnd, 0, cyFrame, nIdGrip, ResizeDlgDirection_OnlyY);
}
void ResizeDlg_Destroy(HWND hwnd, int *cxFrame, int *cyFrame) noexcept;
void ResizeDlg_Size(HWND hwnd, LPARAM lParam, int *cx, int *cy) noexcept;
void ResizeDlg_GetMinMaxInfo(HWND hwnd, LPARAM lParam) noexcept;
HDWP DeferCtlPos(HDWP hdwp, HWND hwndDlg, int nCtlId, int dx, int dy, UINT uFlags) noexcept;
void ResizeDlgCtl(HWND hwndDlg, int nCtlId, int dx, int dy) noexcept;
void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, int wBmpId) noexcept;
void DeleteBitmapButton(HWND hwnd, int nCtlId) noexcept;
void SetClipData(HWND hwnd, LPCWSTR pszData) noexcept;
void SetWindowTransparentMode(HWND hwnd, bool bTransparentMode, int iOpacityLevel) noexcept;
void SetWindowLayoutRTL(HWND hwnd, bool bRTL) noexcept;

#define SendWMCommandEx(hwnd, id, extra)	SendMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), (extra)), 0)
#define SendWMCommand(hwnd, id)				SendWMCommandEx(hwnd, (id), 1)
#define PostWMCommand(hwnd, id)				PostMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), 1), 0)

#define SetWindowStyle(hwnd, style)			SetWindowLong(hwnd, GWL_STYLE, (style))
#define SetWindowExStyle(hwnd, style)		SetWindowLong(hwnd, GWL_EXSTYLE, (style))

#define ComboBox_HasText(hwnd)					(ComboBox_GetTextLength(hwnd) || CB_ERR != ComboBox_GetCurSel(hwnd))
#define ComboBox_GetEditSelStart(hwnd)			LOWORD(ComboBox_GetEditSel(hwnd))
#define ComboBox_GetEditSelEnd(hwnd)			HIWORD(ComboBox_GetEditSel(hwnd))

#define StatusSetSimple(hwnd, b)				SendMessage(hwnd, SB_SIMPLE, (b), 0)
#define StatusSetText(hwnd, nPart, lpszText)	SendMessage(hwnd, SB_SETTEXT, (nPart), AsInteger<LPARAM>(lpszText))

/**
 * we only have 14 commands in toolbar
 * max size = 14*(3 + 2) + 1 (each command with a separator)
 */
#define MAX_TOOLBAR_ITEM_COUNT_WITH_SEPARATOR	50
#define MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE	128
int Toolbar_GetButtons(HWND hwnd, int cmdBase, LPWSTR lpszButtons, int cchButtons) noexcept;
int Toolbar_SetButtons(HWND hwnd, LPCWSTR lpszButtons, LPCTBBUTTON ptbb, int ctbb) noexcept;
void Toolbar_SetButtonImage(HWND hwnd, int idCommand, int iImage) noexcept;

LRESULT SendWMSize(HWND hwnd) noexcept;

#define EnableCmd(hmenu, id, b) EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_GRAYED))
#define CheckCmd(hmenu, id, b)  CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED))
#define DisableCmd(hmenu, id, b)	EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_GRAYED) : (MF_BYCOMMAND | MF_ENABLED))
#define UncheckCmd(hmenu, id, b)	CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_UNCHECKED) : (MF_BYCOMMAND | MF_CHECKED))

#define IsButtonChecked(hwnd, uId)	(IsDlgButtonChecked(hwnd, (uId)) == BST_CHECKED)

HMODULE LoadLocalizedResourceDLL(LANGID lang, LPCWSTR dllName) noexcept;
constexpr bool IsChineseTraditionalSubLang(LANGID subLang) noexcept {
	return subLang == SUBLANG_CHINESE_TRADITIONAL
		|| subLang == SUBLANG_CHINESE_HONGKONG
		|| subLang == SUBLANG_CHINESE_MACAU;
}

inline int GetString(UINT uID, LPWSTR lpBuffer, int cchBufferMax) noexcept {
	return LoadString(g_hInstance, uID, lpBuffer, cchBufferMax);
}
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

inline bool PathIsFile(LPCWSTR pszPath) noexcept {
	// note: INVALID_FILE_ATTRIBUTES is -1.
	return (GetFileAttributes(pszPath) & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

inline bool PathIsSymbolicLink(LPCWSTR pszPath) noexcept {
	// assume file exists, no check for INVALID_FILE_ATTRIBUTES.
	return (GetFileAttributes(pszPath) & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
}

// https://docs.microsoft.com/en-us/windows/win32/intl/handling-sorting-in-your-applications#sort-strings-ordinally
inline bool PathEqual(LPCWSTR pszPath1, LPCWSTR pszPath2) noexcept {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	// the function maps case using the operating system uppercasing table
	return CompareStringOrdinal(pszPath1, -1, pszPath2, -1, TRUE) == CSTR_EQUAL;
#else
	return CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, pszPath1, -1, pszPath2, -1) == CSTR_EQUAL;
#endif
}

// similar to realpath() and std::filesystem::canonical()
bool PathGetRealPath(HANDLE hFile, LPCWSTR lpszSrc, LPWSTR lpszDest) noexcept;
inline void GetProgramRealPath(LPWSTR tchModule, DWORD nSize) noexcept {
	GetModuleFileName(nullptr, tchModule, nSize);
	// for symbolic link, module path is link's path not target's.
	if (PathIsSymbolicLink(tchModule)) {
		PathGetRealPath(nullptr, tchModule, tchModule);
	}
}

void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, DWORD dwAttrTo, bool bUnexpandEnv, bool bUnexpandMyDocs) noexcept;
void PathAbsoluteFromApp(LPCWSTR lpszSrc, LPWSTR lpszDest, bool bExpandEnv) noexcept;
bool PathGetLnkPath(LPCWSTR pszLnkFile, LPWSTR pszResPath);
bool PathCreateLnk(LPCWSTR pszLnkDir, LPCWSTR pszPath);
void OpenContainingFolder(HWND hwnd, LPCWSTR pszFile, bool bSelect) noexcept;

inline void TrimString(LPWSTR lpString) noexcept {
	StrTrim(lpString, L" ");
}

bool ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2) noexcept;
void PrepareFilterStr(LPWSTR lpFilter) noexcept;
void StrTab2Space(LPWSTR lpsz) noexcept;
bool PathFixBackslashes(LPWSTR lpsz) noexcept;
void ExpandEnvironmentStringsEx(LPWSTR lpSrc, DWORD dwSrc) noexcept;
bool SearchPathEx(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer) noexcept;
void FormatNumber(LPWSTR lpNumberStr, UINT value) noexcept;

void GetDefaultFavoritesDir(LPWSTR lpFavDir, int cchFavDir) noexcept;
void GetDefaultOpenWithDir(LPWSTR lpOpenWithDir, int cchOpenWithDir) noexcept;

HDROP CreateDropHandle(LPCWSTR lpFileName) noexcept;

bool ExecDDECommand(LPCWSTR lpszCmdLine, LPCWSTR lpszDDEMsg, LPCWSTR lpszDDEApp, LPCWSTR lpszDDETopic) noexcept;

//==== History Functions ======================================================
#define HISTORY_ITEMS 50

struct HistoryList {
	int iCurItem;
	LPWSTR pszItems[HISTORY_ITEMS];
	void Init() noexcept;
	void Empty() noexcept;
	bool Add(LPCWSTR pszNew) noexcept;
	bool Forward(LPWSTR pszItem, int cItem) noexcept;
	bool Back(LPWSTR pszItem, int cItem) noexcept;
	bool CanForward() const noexcept;
	bool CanBack() const noexcept;
	void UpdateToolbar(HWND hwnd, int cmdBack, int cmdForward) const noexcept;
};

//==== MRU Functions ==========================================================
#define MRU_MAXITEMS	24

enum {
	MRUFlags_Default = 0,
	MRUFlags_FilePath = 1,
};

// MRU_MAXITEMS * (MAX_PATH + 4)
#define MAX_INI_SECTION_SIZE_MRU	(8 * 1024)

struct MRUList {
	int		iSize;
	int		iFlags;
	LPCWSTR szRegKey;
	LPWSTR pszItems[MRU_MAXITEMS];

	void Init(LPCWSTR pszRegKey, int flags) noexcept;
	void Add(LPCWSTR pszNew) noexcept;
	void Delete(int iIndex) noexcept;
	void Empty(bool save) noexcept;
	void Load() noexcept;
	void Save() const noexcept;
	void AddToCombobox(HWND hwnd) const noexcept;
};

//==== Themed Dialogs =========================================================
bool GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize) noexcept;
DLGTEMPLATE *LoadThemedDialogTemplate(LPCWSTR lpDialogTemplateID, HINSTANCE hInstance) noexcept;
#define ThemedDialogBox(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
	ThemedDialogBoxParam(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0)
INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) noexcept;

//==== File Dialog Hook =========================================================
UINT_PTR CALLBACK OpenSaveFileDlgHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

//==== MinimizeToTray Functions - see comments in Helpers.cpp ===================
bool GetDoAnimateMinimize() noexcept;
void MinimizeWndToTray(HWND hwnd) noexcept;
void RestoreWndFromTray(HWND hwnd) noexcept;

// similar to IID_PPV_ARGS() but without __uuidof() check
template <class T>
inline void** AsPPVArgs(T** pp) noexcept {
	static_assert(__is_base_of(IUnknown, T));
	return reinterpret_cast<void **>(pp);
}
