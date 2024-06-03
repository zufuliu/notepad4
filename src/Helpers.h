/******************************************************************************
*
*
* Notepad4
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

constexpr bool StrIsEmpty(LPCSTR s) noexcept {
	return s == nullptr || *s == '\0';
}

constexpr bool StrIsEmpty(LPCWSTR s) noexcept {
	return s == nullptr || *s == L'\0';
}

constexpr bool StrNotEmpty(LPCSTR s) noexcept {
	return s != nullptr && *s != '\0';
}

constexpr bool StrNotEmpty(LPCWSTR s) noexcept {
	return s != nullptr && *s != L'\0';
}

// see scintilla/lexlib/CharacterSet.h
#define UnsafeLower(ch)		((ch) | 0x20)
#define UnsafeUpper(ch)		((ch) & ~0x20)

constexpr bool IsEOLChar(int ch) noexcept {
	return ch == '\r' || ch == '\n';
}

constexpr bool IsASpace(int ch) noexcept {
	return ch == ' ' || (ch >= 0x09 && ch <= 0x0d);
}

constexpr bool IsASpaceOrTab(int ch) noexcept {
	return ch == ' ' || ch == '\t';
}

constexpr bool IsADigit(int ch) noexcept {
	return ch >= '0' && ch <= '9';
}

constexpr bool IsOctalDigit(int ch) noexcept {
	return ch >= '0' && ch <= '7';
}

constexpr bool IsLowerCase(int ch) noexcept {
	return ch >= 'a' && ch <= 'z';
}

constexpr bool IsUpperCase(int ch) noexcept {
	return ch >= 'A' && ch <= 'Z';
}

constexpr bool IsAlpha(int ch) noexcept {
	return (ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z');
}

constexpr bool IsAlphaNumeric(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z');
}

constexpr bool IsPunctuation(int ch) noexcept {
	return (ch > 32 && ch < '0')
		|| (ch > '9' && ch < 'A')
		|| (ch > 'Z' && ch < 'a')
		|| (ch > 'z' && ch < 127);
}

constexpr bool IsHtmlTagChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == ':' || ch == '_' || ch == '-' || ch == '.';
}

constexpr bool IsSchemeNameChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '+' || ch == '-' || ch == '.';
}

constexpr int ToUpperA(int ch) noexcept {
	return (ch >= 'a' && ch <= 'z') ? (ch - 'a' + 'A') : ch;
}

constexpr int ToLowerA(int ch) noexcept {
	return (ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch;
}

constexpr bool IsHexDigit(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'A' && ch <= 'F')
		|| (ch >= 'a' && ch <= 'f');
}

constexpr int GetHexDigit(int ch) noexcept {
	unsigned int diff = ch - '0';
	if (diff < 10) {
		return diff;
	}
	diff = UnsafeLower(ch) - 'a';
	if (diff < 6) {
		return diff + 10;
	}
	return -1;
}

inline bool StrEqual(LPCWSTR s1, LPCWSTR s2) noexcept {
	return wcscmp(s1, s2) == 0;
}

inline bool StrCaseEqual(LPCWSTR s1, LPCWSTR s2) noexcept {
	return _wcsicmp(s1, s2) == 0;
}

#define StrCpyExW(s, t)						memcpy((s), (t), STRSIZE(t))
#define StrEqualExW(s, t)					(memcmp((s), (t), STRSIZE(t)) == 0)
#define StrHasPrefix(s, prefix)				(memcmp((s), (prefix), STRSIZE(prefix) - sizeof(WCHAR)) == 0)
#define StrHasPrefixCase(s, prefix)			(_wcsnicmp((s), (prefix), CSTRLEN(prefix)) == 0)
#define StrHasPrefixCaseEx(s, prefix, len)	(_wcsnicmp((s), (prefix), (len)) == 0)

#define StrEqualExA(s, t)					(memcmp((s), (t), COUNTOF(t)) == 0)
#define StrStartsWith(s, prefix)			(memcmp((s), (prefix), CSTRLEN(prefix)) == 0)
#define StrStartsWithCase(s, prefix)		(_strnicmp((s), (prefix), CSTRLEN(prefix)) == 0)
#define StrStartsWithCaseEx(s, prefix, len)	(_strnicmp((s), (prefix), (len)) == 0)

inline bool StrToFloat(LPCWSTR str, float *value) noexcept {
	LPWSTR end;
#if defined(__USE_MINGW_STRTOX)
	// Fix GCC warning when defined __USE_MINGW_ANSI_STDIO as 1:
	// 'wcstof' is static but used in inline function 'StrToFloat' which is not static.
	*value = __mingw_wcstof(str, &end);
#else
	*value = wcstof(str, &end);
#endif
	return str != end;
}

inline bool CRTStrToInt(LPCWSTR str, int *value) noexcept {
	LPWSTR end;
	*value = static_cast<int>(wcstol(str, &end, 10));
	return str != end;
}

inline bool CRTStrToInt64(LPCWSTR str, int64_t *value) noexcept {
	LPWSTR end;
	*value = _wcstoi64(str, &end, 10);
	return str != end;
}

inline bool HexStrToInt(LPCWSTR str, int *value) noexcept {
	LPWSTR end;
	*value = static_cast<int>(wcstol(str, &end, 16));
	return str != end;
}

int ParseCommaList(LPCWSTR str, int result[], int count) noexcept;
int ParseCommaList64(LPCWSTR str, int64_t result[], int count) noexcept;
LPCSTR GetCurrentLogTime() noexcept;

struct StopWatch {
	LARGE_INTEGER freq; // not changed after system boot
	LARGE_INTEGER begin;
	LARGE_INTEGER end;

	void Start() noexcept {
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&begin);
	}
	void Reset() noexcept {
		begin.QuadPart = 0;
		end.QuadPart = 0;
	}
	void Restart() noexcept {
		end.QuadPart = 0;
		QueryPerformanceCounter(&begin);
	}
	void Stop() noexcept {
		QueryPerformanceCounter(&end);
	}
	double Get() const noexcept {
		const LONGLONG diff = end.QuadPart - begin.QuadPart;
		return (diff * 1000) / static_cast<double>(freq.QuadPart);
	}
	void Show(LPCWSTR msg) const noexcept;
	void ShowLog(LPCSTR msg) const noexcept;
};

#define DebugPrint(msg)		OutputDebugStringA(msg)
#if defined(__GNUC__) || defined(__clang__)
void DebugPrintf(const char *fmt, ...) noexcept __attribute__((format(printf, 1, 2)));
#else
void DebugPrintf(const char *fmt, ...) noexcept;
#endif

extern HINSTANCE g_hInstance;
#if defined(NP2_ENABLE_APP_LOCALIZATION_DLL) && NP2_ENABLE_APP_LOCALIZATION_DLL
extern HINSTANCE g_exeInstance;
#else
#define g_exeInstance	g_hInstance
#endif
extern HANDLE g_hDefaultHeap;
#if _WIN32_WINNT < _WIN32_WINNT_WIN8
extern DWORD g_uWinVer;
extern DWORD kSystemLibraryLoadFlags;
#else
#define kSystemLibraryLoadFlags		LOAD_LIBRARY_SEARCH_SYSTEM32
#endif
extern WCHAR szIniFile[MAX_PATH];

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

#ifndef SEE_MASK_NOZONECHECKS
#define SEE_MASK_NOZONECHECKS		0x00800000		// NTDDI_VERSION >= NTDDI_WINXPSP1
#endif
#ifndef TVS_EX_DOUBLEBUFFER
#define TVS_EX_DOUBLEBUFFER			0x0004			// NTDDI_VERSION >= NTDDI_VISTA
#endif
#ifndef LCMAP_TITLECASE
#define LCMAP_TITLECASE				0x00000300		// _WIN32_WINNT >= _WIN32_WINNT_WIN7
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
UINT GetWindowDPI(HWND hwnd) noexcept;
int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept;
BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) noexcept;
#endif

constexpr DWORD GetIconIndexFlagsForDPI(UINT dpi) noexcept {
	return (dpi >= NP2_LARGER_ICON_SIZE_DPI)
			? (SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON | SHGFI_SYSICONINDEX)
			: (SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
}

constexpr DWORD GetIconHandleFlagsForDPI(UINT dpi) noexcept {
	return (dpi >= NP2_LARGER_ICON_SIZE_DPI)
			? (SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON | SHGFI_ICON)
			: (SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_ICON);
}

inline DWORD GetCurrentIconIndexFlags() noexcept {
	return GetIconIndexFlagsForDPI(g_uCurrentDPI);
}

inline DWORD GetCurrentIconHandleFlags() noexcept {
	return GetIconHandleFlagsForDPI(g_uCurrentDPI);
}

#if defined(NP2_ENABLE_HIDPI_IMAGE_RESOURCE) && NP2_ENABLE_HIDPI_IMAGE_RESOURCE
inline int GetBitmapResourceIdForCurrentDPI(int resourceId) noexcept {
	if (g_uCurrentDPI > USER_DEFAULT_SCREEN_DPI + USER_DEFAULT_SCREEN_DPI/4) {
		int scale = (g_uCurrentDPI + USER_DEFAULT_SCREEN_DPI/4 - 1) / (USER_DEFAULT_SCREEN_DPI/2);
		scale = min(scale, 6);
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
#define NP2HeapReAlloc(hMem, size)	HeapReAlloc(g_hDefaultHeap, HEAP_ZERO_MEMORY, (hMem), (size))
#define NP2HeapFree(hMem)			HeapFree(g_hDefaultHeap, 0, (hMem))
#define NP2HeapSize(hMem)			HeapSize(g_hDefaultHeap, 0, (hMem))

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

struct IniKeyValueNode;
struct IniKeyValueNode {
	IniKeyValueNode *next;
	UINT hash;
	LPCWSTR key;
	LPCWSTR value;
};

// https://en.wikipedia.org/wiki/Sentinel_node
// https://en.wikipedia.org/wiki/Sentinel_value
#define IniSectionImplUseSentinelNode	1

struct IniSection {
	UINT count;
	UINT capacity;
	IniKeyValueNode *head;
#if IniSectionImplUseSentinelNode
	IniKeyValueNode *sentinel;
#endif
	IniKeyValueNode *nodeList;
};

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

bool IniSectionParseArray(IniSection *section, LPWSTR lpCachedIniSection, BOOL quoted);
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

struct IniSectionOnSave {
	LPWSTR next;
};

void IniSectionSetString(IniSectionOnSave *section, LPCWSTR key, LPCWSTR value);
void IniSectionSetQuotedString(IniSectionOnSave *section, LPCWSTR key, LPCWSTR value);

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

#define NP2RegSubKey_ContextMenu	L"*\\shell\\Notepad4"
#define NP2RegSubKey_JumpList		L"Applications\\Notepad4.exe"

LPWSTR Registry_GetString(HKEY hKey, LPCWSTR valueName) noexcept;
LSTATUS Registry_SetString(HKEY hKey, LPCWSTR valueName, LPCWSTR lpszText) noexcept;
LSTATUS Registry_SetInt(HKEY hKey, LPCWSTR valueName, DWORD value) noexcept;
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

#define WaitableTimer_IdleTaskTimeSlot		100
#define WaitableTimer_IdleTaskDelayTime		50
#define WaitableTimer_Create()				CreateWaitableTimer(nullptr, TRUE, nullptr)
#define WaitableTimer_Destroy(timer)		CloseHandle(timer)
inline void WaitableTimer_Set(HANDLE timer, DWORD milliseconds) noexcept {
	LARGE_INTEGER dueTime;
	dueTime.QuadPart = -INT64_C(10*1000)*milliseconds; // convert to 100ns
	SetWaitableTimer(timer, &dueTime, 0, nullptr, nullptr, FALSE);
}
inline HANDLE WaitableTimer_New(DWORD milliseconds) noexcept {
	HANDLE timer = WaitableTimer_Create();
	WaitableTimer_Set(timer, milliseconds);
	return timer;
}
#define WaitableTimer_Continue(timer)	\
	(WaitForSingleObject((timer), 0) != WAIT_OBJECT_0)

struct BackgroundWorker {
	HWND hwnd;
	HANDLE eventCancel;
	HANDLE workerThread;
};

void BackgroundWorker_Init(BackgroundWorker *worker, HWND hwnd);
void BackgroundWorker_Cancel(BackgroundWorker *worker);
void BackgroundWorker_Destroy(BackgroundWorker *worker);
#define BackgroundWorker_Continue(worker)	\
	(WaitForSingleObject((worker)->eventCancel, 0) != WAIT_OBJECT_0)

HRESULT PrivateSetCurrentProcessExplicitAppUserModelID(LPCWSTR AppID) noexcept;
bool IsElevated() noexcept;

#define SetExplorerTheme(hwnd)		SetWindowTheme((hwnd), L"Explorer", nullptr)

bool FindUserResourcePath(LPCWSTR path, LPWSTR outPath) noexcept;
HBITMAP LoadBitmapFile(LPCWSTR path) noexcept;
HBITMAP ResizeImageForCurrentDPI(HBITMAP hbmp) noexcept;

bool BitmapMergeAlpha(HBITMAP hbmp, COLORREF crDest) noexcept;
bool BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha) noexcept;
bool BitmapGrayScale(HBITMAP hbmp) noexcept;
bool VerifyContrast(COLORREF cr1, COLORREF cr2) noexcept;
BOOL IsFontAvailable(LPCWSTR lpszFontName) noexcept;

void SetClipData(HWND hwnd, LPCWSTR pszData) noexcept;
void SetWindowTransparentMode(HWND hwnd, bool bTransparentMode, int iOpacityLevel) noexcept;
void SetWindowLayoutRTL(HWND hwnd, bool bRTL) noexcept;

void CenterDlgInParentEx(HWND hDlg, HWND hParent) noexcept;
void SetToRightBottomEx(HWND hDlg, HWND hParent) noexcept;
inline void CenterDlgInParent(HWND hDlg) noexcept {
	CenterDlgInParentEx(hDlg, GetParent(hDlg));
}
inline void SetToRightBottom(HWND hDlg) noexcept {
	SetToRightBottomEx(hDlg, GetParent(hDlg));
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

void ResizeDlg_InitY2Ex(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int iDirection, int nCtlId1, int nCtlId2) noexcept;
inline void ResizeDlg_InitY2(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int nCtlId1, int nCtlId2) noexcept {
	ResizeDlg_InitY2Ex(hwnd, cxFrame, cyFrame, nIdGrip, ResizeDlgDirection_Both, nCtlId1, nCtlId2);
}
int ResizeDlg_CalcDeltaY2(HWND hwnd, int dy, int cy, int nCtlId1, int nCtlId2) noexcept;

HDWP DeferCtlPos(HDWP hdwp, HWND hwndDlg, int nCtlId, int dx, int dy, UINT uFlags) noexcept;
void ResizeDlgCtl(HWND hwndDlg, int nCtlId, int dx, int dy) noexcept;

#define SendWMCommandEx(hwnd, id, extra)	SendMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), (extra)), 0)
#define SendWMCommand(hwnd, id)				SendWMCommandEx(hwnd, (id), 1)
#define PostWMCommand(hwnd, id)				PostMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), 1), 0)

void MultilineEditSetup(HWND hwndDlg, int nCtlId) noexcept;
// EN_CHANGE is not sent when the ES_MULTILINE style is used and the text is sent through WM_SETTEXT.
// https://docs.microsoft.com/en-us/windows/desktop/Controls/en-change
#define NotifyEditTextChanged(hwndDlg, nCtlId)	SendWMCommandEx(hwndDlg, nCtlId, EN_CHANGE)

void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, int wBmpId) noexcept;
void MakeColorPickButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, COLORREF crColor) noexcept;
void DeleteBitmapButton(HWND hwnd, int nCtlId) noexcept;

#define SetWindowStyle(hwnd, style)			SetWindowLong(hwnd, GWL_STYLE, (style))
#define SetWindowExStyle(hwnd, style)		SetWindowLong(hwnd, GWL_EXSTYLE, (style))

#define ComboBox_HasText(hwnd)					(ComboBox_GetTextLength(hwnd) || CB_ERR != ComboBox_GetCurSel(hwnd))
#define ComboBox_GetEditSelStart(hwnd)			LOWORD(ComboBox_GetEditSel(hwnd))
#define ComboBox_GetEditSelEnd(hwnd)			HIWORD(ComboBox_GetEditSel(hwnd))

#define StatusSetSimple(hwnd, b)				SendMessage(hwnd, SB_SIMPLE, (b), 0)
#define StatusSetText(hwnd, nPart, lpszText)	SendMessage(hwnd, SB_SETTEXT, (nPart), (LPARAM)(lpszText))
BOOL StatusSetTextID(HWND hwnd, UINT nPart, UINT uID) noexcept;
int  StatusCalcPaneWidth(HWND hwnd, LPCWSTR lpsz) noexcept;

/**
 * we only have 26 commands in toolbar
 * max size = 26*(3 + 2) + 1 (each command with a separator)
 */
#define MAX_TOOLBAR_ITEM_COUNT_WITH_SEPARATOR	64
#define MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE	160
int Toolbar_GetButtons(HWND hwnd, int cmdBase, LPWSTR lpszButtons, int cchButtons) noexcept;
int Toolbar_SetButtons(HWND hwnd, LPCWSTR lpszButtons, LPCTBBUTTON ptbb, int ctbb) noexcept;

LRESULT SendWMSize(HWND hwnd) noexcept;

#define EnableCmd(hmenu, id, b)	EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_GRAYED))
#define CheckCmd(hmenu, id, b)	CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED))
#define DisableCmd(hmenu, id, b)	EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_GRAYED) : (MF_BYCOMMAND | MF_ENABLED))
#define UncheckCmd(hmenu, id, b)	CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_UNCHECKED) : (MF_BYCOMMAND | MF_CHECKED))

bool IsCmdEnabled(HWND hwnd, UINT uId) noexcept;
#define IsButtonChecked(hwnd, uId)	(IsDlgButtonChecked(hwnd, (uId)) == BST_CHECKED)
INT GetCheckedRadioButton(HWND hwnd, int nIDFirstButton, int nIDLastButton) noexcept;

inline void SendWMCommandOrBeep(HWND hwnd, UINT id) noexcept {
	if (IsCmdEnabled(hwnd, id)) {
		SendWMCommand(hwnd, id);
	} else {
		MessageBeep(MB_OK);
	}
}

HMODULE LoadLocalizedResourceDLL(LANGID lang, LPCWSTR dllName) noexcept;
constexpr bool IsChineseTraditionalSubLang(LANGID subLang) noexcept {
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

// similar to std::filesystem::equivalent()
bool PathEquivalent(LPCWSTR pszPath1, LPCWSTR pszPath2) noexcept;
void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, DWORD dwAttrTo, bool bUnexpandEnv, bool bUnexpandMyDocs) noexcept;
void PathAbsoluteFromApp(LPCWSTR lpszSrc, LPWSTR lpszDest, bool bExpandEnv) noexcept;
bool PathGetLnkPath(LPCWSTR pszLnkFile, LPWSTR pszResPath);
bool PathCreateDeskLnk(LPCWSTR pszDocument);
bool PathCreateFavLnk(LPCWSTR pszName, LPCWSTR pszTarget, LPCWSTR pszDir);
void OpenContainingFolder(HWND hwnd, LPCWSTR pszFile, bool bSelect) noexcept;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#define KnownFolderId_Desktop			FOLDERID_Desktop
#define KnownFolderId_Documents			FOLDERID_Documents
#define KnownFolderId_LocalAppData		FOLDERID_LocalAppData
#define KnownFolderId_ComputerFolder	FOLDERID_ComputerFolder
#endif

inline void TrimString(LPWSTR lpString) noexcept {
	StrTrim(lpString, L" ");
}

bool ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2) noexcept;

void PrepareFilterStr(LPWSTR lpFilter) noexcept;

void	StrTab2Space(LPWSTR lpsz) noexcept;
bool	PathFixBackslashes(LPWSTR lpsz) noexcept;

void	ExpandEnvironmentStringsEx(LPWSTR lpSrc, DWORD dwSrc) noexcept;
DWORD_PTR SHGetFileInfo2(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags) noexcept;

// remove '&' from access key, i.e. SHStripMneumonic().
void	StripMnemonic(LPWSTR pszMenu) noexcept;

void	FormatNumber(LPWSTR lpNumberStr, size_t Value) noexcept;
#if defined(_WIN64)
#define FormatNumber64(lpNumberStr, Value)	FormatNumber(lpNumberStr, Value)
#else
void	FormatNumber64(LPWSTR lpNumberStr, uint64_t Value) noexcept;
#endif

LPWSTR GetDlgItemFullText(HWND hwndDlg, int nCtlId) noexcept;
int GetDlgItemTextA2W(UINT uCP, HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount) noexcept;
void SetDlgItemTextA2W(UINT uCP, HWND hDlg, int nIDDlgItem, LPCSTR lpString) noexcept;
void ComboBox_AddStringA2W(UINT uCP, HWND hwnd, LPCSTR lpString) noexcept;

//==== MRU Functions ==========================================================
#define MRU_MAXITEMS	32

enum {
	MRUFlags_Default = 0,
	MRUFlags_FilePath = 1,
	MRUFlags_QuoteValue = 2,
	MRUFlags_RelativePath = 4,
	MRUFlags_PortableMyDocs = 8,
};

// MRU_MAXITEMS * (MAX_PATH + 4)
#define MAX_INI_SECTION_SIZE_MRU	(8 * 1024)

typedef struct MRULIST {
	int		iSize;
	int		iFlags;
	LPCWSTR szRegKey;
	LPWSTR pszItems[MRU_MAXITEMS];
} MRULIST, *PMRULIST, *LPMRULIST;

typedef const MRULIST * LPCMRULIST;

void MRU_Init(LPMRULIST pmru, LPCWSTR pszRegKey, int iFlags);
void MRU_Add(LPMRULIST pmru, LPCWSTR pszNew);
void MRU_AddMultiline(LPMRULIST pmru, LPCWSTR pszNew);
void MRU_Delete(LPMRULIST pmru, int iIndex);
void MRU_DeleteFileFromStore(LPCMRULIST pmru, LPCWSTR pszFile);
void MRU_Empty(LPMRULIST pmru, bool save);
void MRU_Load(LPMRULIST pmru);
void MRU_Save(LPCMRULIST pmru);
void MRU_MergeSave(LPMRULIST pmru, bool keep);
void MRU_AddToCombobox(LPCMRULIST pmru, HWND hwnd);

struct BitmapCache {
	UINT count;
	UINT used;
	bool invalid;
	int iconIndex[MRU_MAXITEMS];
	HBITMAP items[MRU_MAXITEMS];
};

inline void BitmapCache_Invalidate(BitmapCache *cache) noexcept {
	cache->invalid = true; // mark all cache as invalid
}
inline void BitmapCache_StartUse(BitmapCache *cache) noexcept {
	cache->used = 0; // mark all cache as unused
}
void BitmapCache_Empty(BitmapCache *cache);
HBITMAP BitmapCache_Get(BitmapCache *cache, LPCWSTR path);

//==== Themed Dialogs =========================================================
#ifndef DLGTEMPLATEEX
#pragma pack(push, 1)
struct DLGTEMPLATEEX {
	WORD	dlgVer;
	WORD	signature;
	DWORD	helpID;
	DWORD	exStyle;
	DWORD	style;
	WORD	cDlgItems;
	short	x;
	short	y;
	short	cx;
	short	cy;
};
#pragma pack(pop)
#endif

bool	GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize) noexcept;
DLGTEMPLATE *LoadThemedDialogTemplate(LPCWSTR lpDialogTemplateID, HINSTANCE hInstance) noexcept;
#define ThemedDialogBox(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
	ThemedDialogBoxParam(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0)
INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) noexcept;
HWND	CreateThemedDialogParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) noexcept;

//==== File Dialog Hook =========================================================
UINT_PTR CALLBACK OpenSaveFileDlgHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

//==== UnSlash Functions ======================================================
void TransformBackslashes(char *pszInput, BOOL bRegEx, UINT cpEdit) noexcept;
bool AddBackslashA(char *pszOut, const char *pszInput) noexcept;
bool AddBackslashW(LPWSTR pszOut, LPCWSTR pszInput) noexcept;
void EscapeRegex(LPSTR pszOut, LPCSTR pszIn) noexcept;
size_t Base64Encode(char *output, const uint8_t *src, size_t length, bool urlSafe) noexcept;
size_t Base64Decode(uint8_t *output, const uint8_t *src, size_t length) noexcept;

//==== MinimizeToTray Functions - see comments in Helpers.cpp ===================
bool GetDoAnimateMinimize() noexcept;
void MinimizeWndToTray(HWND hwnd) noexcept;
void RestoreWndFromTray(HWND hwnd) noexcept;
