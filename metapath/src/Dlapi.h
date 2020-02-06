/******************************************************************************
*
*
* metapath - The universal Explorer-like Plugin
*
* Dlapi.h
*   Definitions for Directory Listing APIs
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
#pragma once

typedef struct LV_ITEMDATA { // lvid
	LPITEMIDLIST pidl; // Item Id
	LPSHELLFOLDER lpsf; // Parent IShellFolder Interface
} LV_ITEMDATA, *LPLV_ITEMDATA;

typedef const LV_ITEMDATA *LPCLV_ITEMDATA;

BOOL DirList_Init(HWND hwnd, LPCWSTR pszHeader);
BOOL DirList_Destroy(HWND hwnd);
BOOL DirList_StartIconThread(HWND hwnd);
BOOL DirList_TerminateIconThread(HWND hwnd);

#define DL_FOLDERS      32
#define DL_NONFOLDERS   64
#define DL_INCLHIDDEN  128
#define DL_ALLOBJECTS  (DL_FOLDERS | DL_NONFOLDERS | DL_INCLHIDDEN)
int DirList_Fill(HWND hwnd, LPCWSTR lpszDir, DWORD grfFlags, LPCWSTR lpszFileSpec,
				 BOOL bExcludeFilter, BOOL bNoFadeHidden,
				 int iSortFlags, BOOL fSortRev);
DWORD WINAPI DirList_IconThread(LPVOID lpParam);
BOOL DirList_GetDispInfo(HWND hwnd, LPARAM lParam, BOOL bNoFadeHidden);
BOOL DirList_DeleteItem(HWND hwnd, LPARAM lParam);

#define DS_NAME     0
#define DS_SIZE     1
#define DS_TYPE     2
#define DS_LASTMOD  3

BOOL DirList_Sort(HWND hwnd, int lFlags, BOOL fRev);

#define DLE_NONE 0
#define DLE_DIR  1
#define DLE_FILE 2

#define DLI_FILENAME	1
#define DLI_DISPNAME	2
#define DLI_TYPE		4
#define DLI_ALL			(DLI_FILENAME | DLI_DISPNAME | DLI_TYPE)

typedef struct DLITEM { // dli
	UINT mask;
	WCHAR szFileName[MAX_PATH];
	WCHAR szDisplayName[MAX_PATH];
	int ntype;
} DLITEM, *LPDLITEM;

typedef const DLITEM *LPCDLITEM;

int DirList_GetItem(HWND hwnd, int iItem, LPDLITEM lpdli);
int DirList_GetItemEx(HWND hwnd, int iItem, LPWIN32_FIND_DATA pfd);
BOOL DirList_PropertyDlg(HWND hwnd, int iItem);
void DirList_DoDragDrop(HWND hwnd, LPARAM lParam);
BOOL DirList_GetLongPathName(HWND hwnd, LPWSTR lpszLongPath);
BOOL DirList_SelectItem(HWND hwnd, LPCWSTR lpszDisplayName, LPCWSTR lpszFullPath);
BOOL DirList_IsFileSelected(HWND hwnd);

#define DL_FILTER_BUFSIZE 128
typedef struct DL_FILTER { //dlf
	int nCount;
	WCHAR tFilterBuf[DL_FILTER_BUFSIZE];
	LPWSTR pFilter[DL_FILTER_BUFSIZE];
	BOOL bExcludeFilter;
} DL_FILTER, *PDL_FILTER, *LPDL_FILTER;

typedef const DL_FILTER *LPCDL_FILTER;

void DirList_CreateFilter(PDL_FILTER pdlf, LPCWSTR lpszFileSpec, BOOL bExcludeFilter);
BOOL DirList_MatchFilter(LPSHELLFOLDER lpsf, LPCITEMIDLIST pidl, LPCDL_FILTER pdlf);

BOOL DriveBox_Init(HWND hwnd);
int  DriveBox_Fill(HWND hwnd);
BOOL DriveBox_GetSelDrive(HWND hwnd, LPWSTR lpszDrive, int nDrive, BOOL fNoSlash);
BOOL DriveBox_SelectDrive(HWND hwnd, LPCWSTR lpszPath);
BOOL DriveBox_PropertyDlg(HWND hwnd);
LRESULT DriveBox_DeleteItem(HWND hwnd, LPARAM lParam);
LRESULT DriveBox_GetDispInfo(HWND hwnd, LPARAM lParam);

static inline LPITEMIDLIST IL_Next(LPITEMIDLIST pidl) {
	return (LPITEMIDLIST)((LPBYTE)(pidl) + pidl->mkid.cb);
}

LPITEMIDLIST IL_Create(LPCITEMIDLIST pidl1, UINT cb1, LPCITEMIDLIST pidl2, UINT cb2);
UINT IL_GetSize(LPCITEMIDLIST pidl);
BOOL IL_GetDisplayName(LPSHELLFOLDER lpsf, LPCITEMIDLIST pidl, DWORD dwFlags, LPWSTR lpszDisplayName, int nDisplayName);
