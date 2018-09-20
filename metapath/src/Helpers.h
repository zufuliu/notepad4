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
*                                               http://www.flos-freeware.ch
*
*
******************************************************************************/

#ifndef METAPATH_HELPERS_H_
#define METAPATH_HELPERS_H_

#if defined(__cplusplus)
#undef NULL
#define NULL	nullptr
#endif

#if (defined(__GNUC__) || defined(__clang__)) && !defined(__cplusplus)
// https://stackoverflow.com/questions/19452971/array-size-macro-that-rejects-pointers
// trigger error for pointer: GCC: void value not ignored as it ought to be. Clang: invalid operands to binary expression.
#define COUNTOF(ar)	_Generic(&(ar), __typeof__((ar)[0]) **: (void)0, default: _countof(ar))
// trigger warning for non-literal string: GCC: division by zero [-Wdiv-by-zero]. Clang: division by zero is undefined [-Wdivision-by-zero].
#define CSTRLEN(s)	(__builtin_constant_p(s) ? (_countof(s) - 1) : (1 / 0))
#else
// C++ template based version of _countof(), or plain old unsafe version
#define COUNTOF(ar)	_countof(ar)
#define CSTRLEN(s)	(_countof(s) - 1)
#endif

inline int min_i(int x, int y) {
	return (x < y) ? x : y;
}

inline int max_i(int x, int y) {
	return (x > y) ? x : y;
}

inline int clamp_i(int x, int lower, int upper) {
	return (x < lower) ? lower : (x > upper) ? upper : x;
}

inline BOOL StrIsEmpty(LPCWSTR s) {
	return s == NULL || *s == L'\0';
}

inline BOOL StrNotEmpty(LPCWSTR s) {
	return s != NULL && *s != L'\0';
}

inline BOOL StrEqual(LPCWSTR s1, LPCWSTR s2) {
	//return CompareStringW(LOCALE_INVARIANT, 0, s1, -1, s2, -1) == CSTR_EQUAL;
	return wcscmp(s1, s2) == 0;
}

inline BOOL StrCaseEqual(LPCWSTR s1, LPCWSTR s2) {
	//return CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, s1, -1, s2, -1) == CSTR_EQUAL;
	return _wcsicmp(s1, s2) == 0;
}

inline BOOL StrNEqual(LPCWSTR s1, LPCWSTR s2, int cch) {
	return wcsncmp(s1, s2, cch) == 0;
}

inline BOOL StrNCaseEqual(LPCWSTR s1, LPCWSTR s2, int cch) {
	return _wcsnicmp(s1, s2, cch) == 0;
}

extern HINSTANCE g_hInstance;
extern HANDLE g_hDefaultHeap;
extern UINT16 g_uWinVer;

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
#define IniDeleteSection(lpSection) \
	WritePrivateProfileSection(lpSection, NULL, szIniFile)
#define IniClearSection(lpSection) \
	WritePrivateProfileSection(lpSection, L"", szIniFile)

inline void IniSetInt(LPCWSTR lpSection, LPCWSTR lpName, int i) {
	WCHAR tch[16];
	wsprintf(tch, L"%i", i);
	IniSetString(lpSection, lpName, tch);
}

inline void IniSetBool(LPCWSTR lpSection, LPCWSTR lpName, BOOL b) {
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

inline void IniSectionInit(IniSection *section, int capacity) {
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

inline void IniSectionFree(IniSection *section) {
	NP2HeapFree(section->nodeList);
}

inline void IniSectionClear(IniSection *section) {
	section->count = 0;
	section->head = NULL;
}

inline BOOL IniSectionIsEmpty(const IniSection *section) {
	return section->count == 0;
}

BOOL IniSectionParseArray(IniSection *section, LPWSTR lpCachedIniSection);
BOOL IniSectionParse(IniSection *section, LPWSTR lpCachedIniSection);
LPCWSTR IniSectionUnsafeGetValue(IniSection *section, LPCWSTR key, int keyLen);

inline LPCWSTR IniSectionGetValueImpl(IniSection *section, LPCWSTR key, int keyLen) {
	return section->count ? IniSectionUnsafeGetValue(section, key, keyLen) : NULL;
}

BOOL IniSectionGetStringImpl(IniSection *section, LPCWSTR key, int keyLen, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString);
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

inline LPCWSTR IniSectionGetValueEx(IniSection *section, LPCWSTR key) {
	return IniSectionGetValueImpl(section, key, -1);
}

inline int IniSectionGetIntEx(IniSection *section, LPCWSTR key, int iDefault) {
	return IniSectionGetIntImpl(section, key, -1, iDefault);
}

inline BOOL IniSectionGetBoolEx(IniSection *section, LPCWSTR key, BOOL bDefault) {
	return IniSectionGetBoolImpl(section, key, -1, bDefault);
}

inline BOOL IniSectionGetStringEx(IniSection *section, LPCWSTR key, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString) {
	return IniSectionGetStringImpl(section, key, -1, lpDefault, lpReturnedString, cchReturnedString);
}

typedef struct IniSectionOnSave {
	LPWSTR next;
} IniSectionOnSave;

void IniSectionSetString(IniSectionOnSave *section, LPCWSTR key, LPCWSTR value);

inline void IniSectionSetInt(IniSectionOnSave *section, LPCWSTR key, int i) {
	WCHAR tch[16];
	wsprintf(tch, L"%i", i);
	IniSectionSetString(section, key, tch);
}

inline void IniSectionSetBool(IniSectionOnSave *section, LPCWSTR key, BOOL b) {
	IniSectionSetString(section, key, (b ? L"1" : L"0"));
}

inline void IniSectionSetStringEx(IniSectionOnSave *section, LPCWSTR key, LPCWSTR value, LPCWSTR lpDefault) {
	if (!StrCaseEqual(value, lpDefault)) {
		IniSectionSetString(section, key, value);
	}
}

inline void IniSectionSetIntEx(IniSectionOnSave *section, LPCWSTR key, int i, int iDefault) {
	if (i != iDefault) {
		IniSectionSetInt(section, key, i);
	}
}

inline void IniSectionSetBoolEx(IniSectionOnSave *section, LPCWSTR key, BOOL b, BOOL bDefault) {
	if (b != bDefault) {
		IniSectionSetString(section, key, (b ? L"1" : L"0"));
	}
}


void BeginWaitCursor(void);
void EndWaitCursor(void);

#if 0
#define IsWin2KAndAbove()	(g_uWinVer >= 0x0500)
#define IsWinXPAndAbove()	(g_uWinVer >= 0x0501)
#else
#define IsWin2KAndAbove()	TRUE
#define IsWinXPAndAbove()	TRUE
#endif
#define IsVistaAndAbove()	(g_uWinVer >= 0x0600)
#define IsWin7AndAbove()	(g_uWinVer >= 0x0601)
#define IsWin8AndAbove()	(g_uWinVer >= 0x0602)

BOOL ExeNameFromWnd(HWND hwnd, LPWSTR szExeName, int cchExeName);
//BOOL Is32bitExe(LPCWSTR lpszExeName);
BOOL PrivateIsAppThemed(void);
//BOOL SetExplorerTheme(HWND hwnd);
BOOL SetTheme(HWND hwnd, LPCWSTR lpszTheme);
BOOL BitmapMergeAlpha(HBITMAP hbmp, COLORREF crDest);
BOOL BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha);
BOOL BitmapGrayScale(HBITMAP hbmp);

BOOL SetWindowPathTitle(HWND hwnd, LPCWSTR lpszFile);
void CenterDlgInParentEx(HWND hDlg, HWND hParent);
void CenterDlgInParent(HWND hDlg);
void SnapToDefaultButton(HWND hwndBox);
void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, UINT uBmpId);
void DeleteBitmapButton(HWND hwnd, int nCtlId);
void SetWindowTransparentMode(HWND hwnd, BOOL bTransparentMode);

#define StatusSetSimple(hwnd,b) SendMessage(hwnd, SB_SIMPLE, (WPARAM)(b), 0)
BOOL StatusSetText(HWND hwnd, UINT nPart, LPCWSTR lpszText);

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

#define IsButtonChecked(hwnd, uId)	(IsDlgButtonChecked(hwnd, (uId)) == BST_CHECKED)

#define GetString(id, pb, cb)	LoadString(g_hInstance, id, pb, cb)
#define StrEnd(pStart)			((pStart) + lstrlen(pStart))

int FormatString(LPWSTR lpOutput, int nOutput, UINT uIdFormat, ...);

void PathRelativeToApp(LPWSTR lpszSrc, LPWSTR lpszDest, int cchDest, BOOL bSrcIsFile,
					   BOOL bUnexpandEnv, BOOL bUnexpandMyDocs);
void PathAbsoluteFromApp(LPWSTR lpszSrc, LPWSTR lpszDest, int cchDest, BOOL bExpandEnv);

BOOL PathIsLnkFile(LPCWSTR pszPath);
BOOL PathGetLnkPath(LPCWSTR pszLnkFile, LPWSTR pszResPath, int cchResPath);
BOOL PathIsLnkToDirectory(LPCWSTR pszPath, LPWSTR pszResPath, int cchResPath);
BOOL PathCreateLnk(LPCWSTR pszLnkDir, LPCWSTR pszPath);

BOOL TrimString(LPWSTR lpString);
BOOL ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2);

LPWSTR QuotateFilenameStr(LPWSTR lpFile);
LPWSTR GetFilenameStr(LPWSTR lpFile);

void PrepareFilterStr(LPWSTR lpFilter);
void StrTab2Space(LPWSTR lpsz);
void ExpandEnvironmentStringsEx(LPWSTR lpSrc, DWORD dwSrc);
void PathCanonicalizeEx(LPWSTR lpSrc);
DWORD SearchPathEx(LPCWSTR lpPath, LPCWSTR lpFileName, LPCWSTR lpExtension,
				   DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR *lpFilePart);
int  FormatNumberStr(LPWSTR lpNumberStr);

void GetDefaultFavoritesDir(LPWSTR lpFavDir, int cchFavDir);
void GetDefaultOpenWithDir(LPWSTR lpOpenWithDir, int cchOpenWithDir);

HDROP CreateDropHandle(LPCWSTR lpFileName);

BOOL ExecDDECommand(LPCWSTR lpszCmdLine, LPCWSTR lpszDDEMsg, LPCWSTR lpszDDEApp, LPCWSTR lpszDDETopic);

//==== History Functions ======================================================
#define HISTORY_ITEMS 50

typedef struct tagHISTORY {
	WCHAR *psz[HISTORY_ITEMS]; // Strings
	int  iCurItem;            // Current Item
} HISTORY, *PHISTORY;

BOOL History_Init(PHISTORY ph);
BOOL History_Uninit(PHISTORY ph);
BOOL History_Add(PHISTORY ph, LPCWSTR pszNew);
BOOL History_Forward(PHISTORY ph, LPWSTR pszItem, int cItem);
BOOL History_Back(PHISTORY ph, LPWSTR pszItem, int cItem);
BOOL History_CanForward(PHISTORY ph);
BOOL History_CanBack(PHISTORY ph);
void History_UpdateToolbar(PHISTORY ph, HWND hwnd, int cmdBack, int cmdForward);

//==== MRU Functions ==========================================================
#define MRU_MAXITEMS 24
#define MRU_NOCASE    1
#define MRU_UTF8      2

typedef struct _mrulist {
	WCHAR  szRegKey[256];
	int   iFlags;
	int   iSize;
	LPWSTR pszItems[MRU_MAXITEMS];
} MRULIST, *PMRULIST, *LPMRULIST;

LPMRULIST MRU_Create(LPCWSTR pszRegKey, int iFlags, int iSize);
BOOL MRU_Destroy(LPMRULIST pmru);
BOOL MRU_Add(LPMRULIST pmru, LPCWSTR pszNew);
BOOL MRU_Delete(LPMRULIST pmru, int iIndex);
BOOL MRU_Empty(LPMRULIST pmru);
int MRU_Enum(LPMRULIST pmru, int iIndex, LPWSTR pszItem, int cchItem);
inline int MRU_GetCount(LPMRULIST pmru) {
	return MRU_Enum(pmru, 0, NULL, 0);
}
BOOL MRU_Load(LPMRULIST pmru);
BOOL MRU_Save(LPMRULIST pmru);
void MRU_LoadToCombobox(HWND hwnd, LPCWSTR pszKey);
void MRU_AddOneItem(LPCWSTR pszKey, LPCWSTR pszNewItem);
void MRU_ClearCombobox(HWND hwnd, LPCWSTR pszKey);

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

BOOL GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize);
DLGTEMPLATE *LoadThemedDialogTemplate(LPCTSTR lpDialogTemplateID, HINSTANCE hInstance);
#define ThemedDialogBox(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
	ThemedDialogBoxParam(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0)
INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance,
							 LPCTSTR lpTemplate,
							 HWND hWndParent,
							 DLGPROC lpDialogFunc,
							 LPARAM dwInitParam);

//==== MinimizeToTray Functions - see comments in Helpers.c ===================
BOOL GetDoAnimateMinimize(VOID);
VOID MinimizeWndToTray(HWND hwnd);
VOID RestoreWndFromTray(HWND hwnd);

#endif // METAPATH_HELPERS_H_

///   End of Helpers.h
