/******************************************************************************
*
*
* Notepad2
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
*                                               http://www.flos-freeware.ch
*
*
******************************************************************************/

#ifndef NOTEPAD2_HELPERS_H_
#define NOTEPAD2_HELPERS_H_
#include <stdint.h>
#include "compiler.h"

NP2_inline int min_i(int x, int y) {
	return (x < y) ? x : y;
}

NP2_inline int max_i(int x, int y) {
	return (x > y) ? x : y;
}

NP2_inline UINT min_u(UINT x, UINT y) {
	return (x < y) ? x : y;
}

NP2_inline UINT max_u(UINT x, UINT y) {
	return (x > y) ? x : y;
}

NP2_inline long min_l(long x, long y) {
	return (x < y) ? x : y;
}

NP2_inline long max_l(long x, long y) {
	return (x > y) ? x : y;
}

NP2_inline int clamp_i(int x, int lower, int upper) {
	return (x < lower) ? lower : (x > upper) ? upper : x;
}

NP2_inline int validate_i(int x, int lower, int upper, int defaultValue) {
	return (x < lower || x > upper) ? defaultValue : x;
}

// required for SSE2
#define NP2DefaultPointerAlignment		16

NP2_inline unsigned int align_up(unsigned int value) {
	return (value + NP2DefaultPointerAlignment - 1) & (~(NP2DefaultPointerAlignment - 1));
}

NP2_inline void* align_ptr_ex(const void *ptr, size_t align) {
	return (void *)(((uintptr_t)(ptr) + align - 1) & (~(align - 1)));
}

NP2_inline void* align_ptr(const void *ptr) {
	return align_ptr_ex(ptr, NP2DefaultPointerAlignment);
}

NP2_inline unsigned int bswap32(unsigned int x) {
	return (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

NP2_inline BOOL StrIsEmptyA(LPCSTR s) {
	return s == NULL || *s == '\0';
}

NP2_inline BOOL StrIsEmpty(LPCWSTR s) {
	return s == NULL || *s == L'\0';
}

NP2_inline BOOL StrNotEmptyA(LPCSTR s) {
	return s != NULL && *s != '\0';
}

NP2_inline BOOL StrNotEmpty(LPCWSTR s) {
	return s != NULL && *s != L'\0';
}

NP2_inline BOOL StrEqual(LPCWSTR s1, LPCWSTR s2) {
	return wcscmp(s1, s2) == 0;
}

NP2_inline BOOL StrCaseEqual(LPCWSTR s1, LPCWSTR s2) {
	return _wcsicmp(s1, s2) == 0;
}

NP2_inline BOOL StrNEqual(LPCWSTR s1, LPCWSTR s2, int cch) {
	return wcsncmp(s1, s2, cch) == 0;
}

NP2_inline BOOL StrNCaseEqual(LPCWSTR s1, LPCWSTR s2, int cch) {
	return _wcsnicmp(s1, s2, cch) == 0;
}

// str MUST NOT be NULL, can be empty
NP2_inline BOOL StrToFloat(LPCWSTR str, float *value) {
	LPWSTR end;
	*value = wcstof(str, &end);
	return str != end;
}

NP2_inline BOOL CRTStrToInt(LPCWSTR str, int *value) {
	LPWSTR end;
	*value = (int)wcstol(str, &end, 10);
	return str != end;
}

// str MUST NOT be NULL, can be empty
NP2_inline BOOL HexStrToInt(LPCWSTR str, int *value) {
	LPWSTR end;
	*value = (int)wcstol(str, &end, 16);
	return str != end;
}

int ParseCommaList(LPCWSTR str, int result[], int count);

typedef struct StopWatch {
	LARGE_INTEGER freq; // not changed after system boot
	LARGE_INTEGER begin;
	LARGE_INTEGER end;
} StopWatch;

#define StopWatch_Start(watch) do { \
		QueryPerformanceFrequency(&(watch).freq);	\
		QueryPerformanceCounter(&(watch).begin);	\
	} while (0)

#define StopWatch_Reset(watch) do { \
		(watch).begin.QuadPart = 0;	\
		(watch).end.QuadPart = 0;	\
	} while (0)

#define StopWatch_Restart(watch) do { \
		(watch).end.QuadPart = 0;					\
		QueryPerformanceCounter(&(watch).begin);	\
	} while (0)

#define StopWatch_Stop(watch) \
	QueryPerformanceCounter(&(watch).end)

NP2_inline double StopWatch_Get(const StopWatch *watch) {
	const LONGLONG diff = watch->end.QuadPart - watch->begin.QuadPart;
	const double freq = (double)(watch->freq.QuadPart);
	return (diff / freq) * 1000;
}

void StopWatch_Show(const StopWatch *watch, LPCWSTR msg);
void StopWatch_ShowLog(const StopWatch *watch, LPCSTR msg);

#ifdef NDEBUG
#define DLog(msg)
#define DLogf(fmt, ...)
#else
#define DLog(msg)	OutputDebugStringA(msg)
void DLogf(const char *fmt, ...);
#endif

extern HINSTANCE g_hInstance;
extern HANDLE g_hDefaultHeap;
#if _WIN32_WINNT < _WIN32_WINNT_WIN10
extern DWORD g_uWinVer;
#endif
extern UINT g_uCurrentDPI;
extern UINT g_uDefaultDPI;
extern WCHAR szIniFile[MAX_PATH];

// Operating System Version
// https://docs.microsoft.com/en-us/windows/desktop/SysInfo/operating-system-version

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#define IsVistaAndAbove()	TRUE
#else
#define IsVistaAndAbove()	(g_uWinVer >= _WIN32_WINNT_VISTA)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
#define IsWin7AndAbove()	TRUE
#else
#define IsWin7AndAbove()	(g_uWinVer >= _WIN32_WINNT_WIN7)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
#define IsWin8AndAbove()	TRUE
#else
#define IsWin8AndAbove()	(g_uWinVer >= _WIN32_WINNT_WIN8)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
#define IsWin8p1AndAbove()	TRUE
#else
#define IsWin8p1AndAbove()	(g_uWinVer >= _WIN32_WINNT_WINBLUE)
#endif
#if _WIN32_WINNT >= _WIN32_WINNT_WIN10
#define IsWin10AndAbove()	TRUE
#else
#define IsWin10AndAbove()	(g_uWinVer >= _WIN32_WINNT_WIN10)
#endif

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#ifndef SEE_MASK_NOZONECHECKS
#define SEE_MASK_NOZONECHECKS		0x00800000		// NTDDI_VERSION >= NTDDI_WINXPSP1
#endif

#ifndef LCMAP_TITLECASE
#define LCMAP_TITLECASE				0x00000300		// WINVER >= _WIN32_WINNT_WIN7
#endif

// High DPI Reference
// https://docs.microsoft.com/en-us/windows/desktop/hidpi/high-dpi-reference
#ifndef WM_DPICHANGED
#define WM_DPICHANGED	0x02E0				// WINVER >= _WIN32_WINNT_WIN7
#endif
#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI		96		// _WIN32_WINNT >= _WIN32_WINNT_VISTA
#endif
#ifndef DPI_ENUMS_DECLARED
#define MDT_EFFECTIVE_DPI	0
#endif

NP2_inline int RoundToCurrentDPI(int value)	{
	return (g_uCurrentDPI == USER_DEFAULT_SCREEN_DPI) ? value : MulDiv(g_uCurrentDPI, value, USER_DEFAULT_SCREEN_DPI);
}

NP2_inline int DefaultToCurrentDPI(int value) {
	return (g_uCurrentDPI == g_uDefaultDPI) ? value : MulDiv(g_uCurrentDPI, value, g_uDefaultDPI);
}

// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getsystemmetrics
// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getsystemmetricsfordpi
int GetSystemMetricsEx(int nIndex);

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

void IniClearSectionEx(LPCWSTR lpSection, LPCWSTR lpszIniFile, BOOL bDelete);
#define IniClearSection(lpSection)			IniClearSectionEx((lpSection), szIniFile, FALSE)
#define IniDeleteSection(lpSection) 		IniClearSectionEx((lpSection), szIniFile, TRUE)

void IniClearAllSectionEx(LPCWSTR lpszPrefix, LPCWSTR lpszIniFile, BOOL bDelete);
#define IniClearAllSection(lpszPrefix)		IniClearAllSectionEx((lpszPrefix), szIniFile, FALSE)
#define IniDeleteAllSection(lpszPrefix)		IniClearAllSectionEx((lpszPrefix), szIniFile, TRUE)

NP2_inline void IniSetInt(LPCWSTR lpSection, LPCWSTR lpName, int i) {
	WCHAR tch[16];
	wsprintf(tch, L"%i", i);
	IniSetString(lpSection, lpName, tch);
}

NP2_inline void IniSetBool(LPCWSTR lpSection, LPCWSTR lpName, BOOL b) {
	IniSetString(lpSection, lpName, (b ? L"1" : L"0"));
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
	int count;
	int capacity;
	IniKeyValueNode *head;
#if IniSectionImplUseSentinelNode
	IniKeyValueNode *sentinel;
#endif
	IniKeyValueNode *nodeList;
} IniSection;

NP2_inline void IniSectionInit(IniSection *section, int capacity) {
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

NP2_inline BOOL IniSectionIsEmpty(const IniSection *section) {
	return section->count == 0;
}

BOOL IniSectionParseArray(IniSection *section, LPWSTR lpCachedIniSection);
BOOL IniSectionParse(IniSection *section, LPWSTR lpCachedIniSection);
LPCWSTR IniSectionUnsafeGetValue(IniSection *section, LPCWSTR key, int keyLen);

NP2_inline LPCWSTR IniSectionGetValueImpl(IniSection *section, LPCWSTR key, int keyLen) {
	return section->count ? IniSectionUnsafeGetValue(section, key, keyLen) : NULL;
}

void IniSectionGetStringImpl(IniSection *section, LPCWSTR key, int keyLen, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString);
int IniSectionGetIntImpl(IniSection *section, LPCWSTR key, int keyLen, int iDefault);
BOOL IniSectionGetBoolImpl(IniSection *section, LPCWSTR key, int keyLen, BOOL bDefault);


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

NP2_inline BOOL IniSectionGetBoolEx(IniSection *section, LPCWSTR key, BOOL bDefault) {
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
	wsprintf(tch, L"%i", i);
	IniSectionSetString(section, key, tch);
}

NP2_inline void IniSectionSetBool(IniSectionOnSave *section, LPCWSTR key, BOOL b) {
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

NP2_inline void IniSectionSetBoolEx(IniSectionOnSave *section, LPCWSTR key, BOOL b, BOOL bDefault) {
	if (b != bDefault) {
		IniSectionSetString(section, key, (b ? L"1" : L"0"));
	}
}

typedef struct DString {
	LPWSTR buffer;
	INT capacity;
} DString;

static inline void DString_Free(DString *s) {
	if (s->buffer) {
		NP2HeapFree(s->buffer);
	}
}

void DString_GetWindowText(DString *s, HWND hwnd);
NP2_inline void DString_GetDlgItemText(DString *s, HWND hwndDlg, int nCtlId) {
	DString_GetWindowText(s, GetDlgItem(hwndDlg, nCtlId));
}

NP2_inline BOOL KeyboardIsKeyDown(int key) {
	return (GetKeyState(key) & 0x8000) != 0;
}

UINT GetDefaultDPI(HWND hwnd);
UINT GetCurrentDPI(HWND hwnd);
HRESULT PrivateSetCurrentProcessExplicitAppUserModelID(PCWSTR AppID);
BOOL IsElevated(void);

//BOOL SetTheme(HWND hwnd, LPCWSTR lpszTheme)
//NP2_inline BOOL SetExplorerTheme(HWND hwnd) {
//	return SetTheme(hwnd, L"Explorer");
//}

HBITMAP LoadBitmapFile(LPCWSTR path);
HBITMAP ResizeImageForCurrentDPI(HBITMAP hbmp);
BOOL BitmapMergeAlpha(HBITMAP hbmp, COLORREF crDest);
BOOL BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha);
BOOL BitmapGrayScale(HBITMAP hbmp);
BOOL VerifyContrast(COLORREF cr1, COLORREF cr2);
BOOL IsFontAvailable(LPCWSTR lpszFontName);

void SetClipData(HWND hwnd, LPCWSTR pszData);
BOOL SetWindowTitle(HWND hwnd, UINT uIDAppName, BOOL bIsElevated, UINT uIDUntitled,
					LPCWSTR lpszFile, int iFormat, BOOL bModified,
					UINT uIDReadOnly, BOOL bReadOnly, LPCWSTR lpszExcerpt);
void SetWindowTransparentMode(HWND hwnd, BOOL bTransparentMode, int iOpacityLevel);
void SetWindowLayoutRTL(HWND hwnd, BOOL bRTL);

void CenterDlgInParentEx(HWND hDlg, HWND hParent);
void SetToRightBottomEx(HWND hDlg, HWND hParent);
NP2_inline void CenterDlgInParent(HWND hDlg) {
	CenterDlgInParentEx(hDlg, GetParent(hDlg));
}
NP2_inline void SetToRightBottom(HWND hDlg) {
	SetToRightBottomEx(hDlg, GetParent(hDlg));
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

#define MAX_RESIZEDLG_ATTR_COUNT	2
void ResizeDlg_SetAttr(HWND hwnd, int index, int value);
int ResizeDlg_GetAttr(HWND hwnd, int index);

void ResizeDlg_InitY2Ex(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int iDirection, int nCtlId1, int nCtlId2);
NP2_inline void ResizeDlg_InitY2(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int nCtlId1, int nCtlId2) {
	ResizeDlg_InitY2Ex(hwnd, cxFrame, cyFrame, nIdGrip, ResizeDlgDirection_Both, nCtlId1, nCtlId2);
}
int ResizeDlg_CalcDeltaY2(HWND hwnd, int dy, int cy, int nCtlId1, int nCtlId2);

HDWP DeferCtlPos(HDWP hdwp, HWND hwndDlg, int nCtlId, int dx, int dy, UINT uFlags);
void ResizeDlgCtl(HWND hwndDlg, int nCtlId, int dx, int dy);

#define SendWMCommandEx(hwnd, id, extra)	SendMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), (extra)), 0)
#define SendWMCommand(hwnd, id)				SendWMCommandEx(hwnd, (id), 1)
#define PostWMCommand(hwnd, id)				PostMessage(hwnd, WM_COMMAND, MAKEWPARAM((id), 1), 0)

void MultilineEditSetup(HWND hwndDlg, int nCtlId);
// EN_CHANGE is not sent when the ES_MULTILINE style is used and the text is sent through WM_SETTEXT.
// https://docs.microsoft.com/en-us/windows/desktop/Controls/en-change
#define NotifyEditTextChanged(hwndDlg, nCtlId)	SendWMCommandEx(hwndDlg, nCtlId, EN_CHANGE)

void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, WORD wBmpId);
void MakeColorPickButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, COLORREF crColor);
void DeleteBitmapButton(HWND hwnd, int nCtlId);

#define StatusSetSimple(hwnd, b)				SendMessage(hwnd, SB_SIMPLE, (b), 0)
#define StatusSetText(hwnd, nPart, lpszText)	SendMessage(hwnd, SB_SETTEXT, (nPart), (LPARAM)(lpszText))
BOOL StatusSetTextID(HWND hwnd, UINT nPart, UINT uID);
int  StatusCalcPaneWidth(HWND hwnd, LPCWSTR lpsz);

/**
 * we only have 25 commands in toolbar
 * max size = 25*(3 + 2) + 1 (each command with a separator)
 */
#define MAX_TOOLBAR_ITEM_COUNT_WITH_SEPARATOR	50
#define MAX_TOOLBAR_BUTTON_CONFIG_BUFFER_SIZE	160
int Toolbar_GetButtons(HWND hwnd, int cmdBase, LPWSTR lpszButtons, int cchButtons);
int Toolbar_SetButtons(HWND hwnd, LPCWSTR lpszButtons, LPCTBBUTTON ptbb, int ctbb);

LRESULT SendWMSize(HWND hwnd);

#define EnableCmd(hmenu, id, b)	EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_GRAYED))
#define CheckCmd(hmenu, id, b)	CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED))

BOOL IsCmdEnabled(HWND hwnd, UINT uId);
#define IsButtonChecked(hwnd, uId)	(IsDlgButtonChecked(hwnd, (uId)) == BST_CHECKED)
INT GetCheckedRadioButton(HWND hwnd, int nIDFirstButton, int nIDLastButton);

NP2_inline void SendWMCommandOrBeep(HWND hwnd, UINT id) {
	if (IsCmdEnabled(hwnd, id)) {
		SendWMCommand(hwnd, id);
	} else {
		MessageBeep(0);
	}
}

#define GetString(id, pb, cb)	LoadString(g_hInstance, id, pb, cb)
#define StrEnd(pStart)			((pStart) + lstrlen(pStart))

/**
 * Variadic Macros
 * https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 * https://docs.microsoft.com/en-us/cpp/preprocessor/variadic-macros?view=vs-2017
 */
#define FormatString(lpOutput, lpFormat, uIdFormat, ...) do {	\
		GetString((uIdFormat), (lpFormat), COUNTOF(lpFormat));	\
		wsprintf((lpOutput), (lpFormat), __VA_ARGS__);			\
	} while (0)

void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, int cchDest,
					   BOOL bSrcIsFile, BOOL bUnexpandEnv, BOOL bUnexpandMyDocs);
void PathAbsoluteFromApp(LPCWSTR lpszSrc, LPWSTR lpszDest, int cchDest, BOOL bExpandEnv);

BOOL PathIsLnkFile(LPCWSTR pszPath);
BOOL PathGetLnkPath(LPCWSTR pszLnkFile, LPWSTR pszResPath, int cchResPath);
BOOL PathIsLnkToDirectory(LPCWSTR pszPath, LPWSTR pszResPath, int cchResPath);
BOOL PathCreateDeskLnk(LPCWSTR pszDocument);
BOOL PathCreateFavLnk(LPCWSTR pszName, LPCWSTR pszTarget, LPCWSTR pszDir);
void OpenContainingFolder(HWND hwnd, LPCWSTR pszFile, BOOL bSelect);

NP2_inline void TrimString(LPWSTR lpString) {
	StrTrim(lpString, L" ");
}

BOOL ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2);

void PrepareFilterStr(LPWSTR lpFilter);

void	StrTab2Space(LPWSTR lpsz);
void	PathFixBackslashes(LPWSTR lpsz);

void	ExpandEnvironmentStringsEx(LPWSTR lpSrc, DWORD dwSrc);
void	PathCanonicalizeEx(LPWSTR lpSrc);
DWORD	GetLongPathNameEx(LPWSTR lpszPath, DWORD cchBuffer);
DWORD_PTR SHGetFileInfo2(LPCWSTR pszPath, DWORD dwFileAttributes,
						 SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags);

void	FormatNumberStr(LPWSTR lpNumberStr);
BOOL	SetDlgItemIntEx(HWND hwnd, int nIdItem, UINT uValue);

UINT	GetDlgItemTextA2W(UINT uCP, HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount);
UINT	SetDlgItemTextA2W(UINT uCP, HWND hDlg, int nIDDlgItem, LPCSTR lpString);
LRESULT ComboBox_AddStringA2W(UINT uCP, HWND hwnd, LPCSTR lpString);

UINT CodePageFromCharSet(UINT uCharSet);

//==== MRU Functions ==========================================================
#define MRU_MAXITEMS	32
#define MRU_NOCASE		1
#define MRU_UTF8		2

// MRU_MAXITEMS * (MAX_PATH + 4)
#define MAX_INI_SECTION_SIZE_MRU	(8 * 1024)

typedef struct _mrulist {
	LPCWSTR	szRegKey;
	int		iFlags;
	int		iSize;
	LPWSTR pszItems[MRU_MAXITEMS];
} MRULIST, *PMRULIST, *LPMRULIST;

typedef const MRULIST * LPCMRULIST;

LPMRULIST MRU_Create(LPCWSTR pszRegKey, int iFlags, int iSize);
BOOL	MRU_Destroy(LPMRULIST pmru);
BOOL	MRU_Add(LPMRULIST pmru, LPCWSTR pszNew);
BOOL	MRU_AddFile(LPMRULIST pmru, LPCWSTR pszFile, BOOL bRelativePath, BOOL bUnexpandMyDocs);
BOOL	MRU_Delete(LPMRULIST pmru, int iIndex);
BOOL	MRU_DeleteFileFromStore(LPCMRULIST pmru, LPCWSTR pszFile);
BOOL	MRU_Empty(LPMRULIST pmru);
int 	MRU_Enum(LPCMRULIST pmru, int iIndex, LPWSTR pszItem, int cchItem);
NP2_inline int MRU_GetCount(LPCMRULIST pmru) {
	return MRU_Enum(pmru, 0, NULL, 0);
}
BOOL	MRU_Load(LPMRULIST pmru);
BOOL	MRU_Save(LPCMRULIST pmru);
BOOL	MRU_MergeSave(LPCMRULIST pmru, BOOL bAddFiles, BOOL bRelativePath, BOOL bUnexpandMyDocs);

//==== Themed Dialogs =========================================================
#ifndef DLGTEMPLATEEX
#pragma pack(push, 1)
typedef struct {
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
} DLGTEMPLATEEX;
#pragma pack(pop)
#endif

BOOL	GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize);
DLGTEMPLATE *LoadThemedDialogTemplate(LPCWSTR lpDialogTemplateID, HINSTANCE hInstance);
#define ThemedDialogBox(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
	ThemedDialogBoxParam(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0)
INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
HWND	CreateThemedDialogParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);

//==== UnSlash Functions ======================================================
void TransformBackslashes(char *pszInput, BOOL bRegEx, UINT cpEdit);
BOOL AddBackslash(char *pszOut, const char *pszInput);

//==== MinimizeToTray Functions - see comments in Helpers.c ===================
BOOL GetDoAnimateMinimize(VOID);
VOID MinimizeWndToTray(HWND hwnd);
VOID RestoreWndFromTray(HWND hwnd);

#endif // NOTEPAD2_HELPERS_H_

// End of Helpers.h
