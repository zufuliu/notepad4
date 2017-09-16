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

#define COUNTOF(ar)		(sizeof(ar)/sizeof(ar[0]))
#define CSTRLEN(s)		(COUNTOF(s)-1)

static inline int min_i(int x, int y) {
	return (x < y) ? x : y;
}

static inline int max_i(int x, int y) {
	return (x > y) ? x : y;
}

static inline int maxmin_i(int x, int y, int z) {
	return max_i(min_i(x, y), z);
}

static inline int minmax_i(int x, int y, int z) {
	return min_i(max_i(x, y), z);
}

extern HINSTANCE g_hInstance;
extern UINT16 g_uWinVer;

extern WCHAR szIniFile[MAX_PATH];
#define IniGetString(lpSection, lpName, lpDefault, lpReturnedStr, nSize) \
	GetPrivateProfileString(lpSection, lpName, lpDefault, lpReturnedStr, nSize, szIniFile)
#define IniGetInt(lpSection, lpName, nDefault) \
	GetPrivateProfileInt(lpSection, lpName, nDefault, szIniFile)
#define IniSetString(lpSection, lpName, lpString) \
	WritePrivateProfileString(lpSection, lpName, lpString, szIniFile)
#define IniDeleteSection(lpSection) \
	WritePrivateProfileSection(lpSection, NULL, szIniFile)
static inline BOOL IniSetInt(LPCWSTR lpSection, LPCWSTR lpName, int i) {
	WCHAR tch[32];
	wsprintf(tch, L"%i", i);
	return WritePrivateProfileString(lpSection, lpName, tch, szIniFile);
}
#define LoadIniSection(lpSection, lpBuf, cchBuf) \
	GetPrivateProfileSection(lpSection, lpBuf, cchBuf, szIniFile);
#define SaveIniSection(lpSection, lpBuf) \
	WritePrivateProfileSection(lpSection, lpBuf, szIniFile)
int IniSectionGetString(LPCWSTR lpCachedIniSection, LPCWSTR lpName, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString);
int IniSectionGetInt(LPCWSTR lpCachedIniSection, LPCWSTR lpName, int iDefault);
BOOL IniSectionGetBool(LPCWSTR lpCachedIniSection, LPCWSTR lpName, BOOL bDefault);
BOOL IniSectionSetString(LPWSTR lpCachedIniSection, LPCWSTR lpName, LPCWSTR lpString);

static inline BOOL IniSectionSetInt(LPWSTR lpCachedIniSection, LPCWSTR lpName, int i) {
	WCHAR tch[32];
	wsprintf(tch, L"%i", i);
	return IniSectionSetString(lpCachedIniSection, lpName, tch);
}

static inline BOOL IniSectionSetBool(LPWSTR lpCachedIniSection, LPCWSTR lpName, BOOL b) {
	return IniSectionSetString(lpCachedIniSection, lpName, (b ? L"1" : L"0"));
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

int Toolbar_GetButtons(HWND hwnd, int cmdBase, LPWSTR lpszButtons, int cchButtons);
int Toolbar_SetButtons(HWND hwnd, int cmdBase, LPCWSTR lpszButtons, LPCTBBUTTON ptbb, int ctbb);
void Toolbar_SetButtonImage(HWND hwnd, int idCommand, int iImage);

LRESULT SendWMSize(HWND hwnd);

#define EnableCmd(hmenu, id, b) EnableMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_GRAYED))
#define CheckCmd(hmenu, id, b)  CheckMenuItem(hmenu, id, (b)? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED))

#define GetString(id,pb,cb) LoadString(g_hInstance, id, pb, cb)
#define StrEnd(pStart) (pStart + lstrlen(pStart))

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
BOOL MRU_Load(LPMRULIST pmru);
BOOL MRU_Save(LPMRULIST pmru);
void MRU_LoadToCombobox(HWND hwnd, LPCWSTR pszKey);
void MRU_AddOneItem(LPCWSTR pszKey, LPCWSTR pszNewItem);

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
