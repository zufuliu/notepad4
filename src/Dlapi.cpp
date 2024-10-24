/******************************************************************************
*
*
* Notepad4
*
* Dlapi.cpp
*   Directory Listing APIs used in Notepad4
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

struct IUnknown;
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include "Helpers.h"
#include "Dlapi.h"

//==== DirList ================================================================

//==== DLDATA Structure =======================================================
struct DLDATA {
	BackgroundWorker worker;	// where HWND is ListView Control
	UINT cbidl;					// Size of pidl
	bool bNoFadeHidden;			// Flag passed from GetDispInfo()
	LPITEMIDLIST pidl;			// Directory Id
	LPSHELLFOLDER lpsf;			// IShellFolder Interface to pidl
	WCHAR szPath[MAX_PATH];		// Pathname to Directory Id
	int iDefIconFolder;			// Default Folder Icon
	int iDefIconFile;			// Default File Icon
};

//==== Property Name ==========================================================
static const WCHAR *pDirListProp = L"DirListData";
// same as ILNext()
static inline LPCITEMIDLIST IL_Next(LPCITEMIDLIST pidl) noexcept {
	return reinterpret_cast<LPCITEMIDLIST>(reinterpret_cast<const char *>(pidl) + pidl->mkid.cb);
}

//=============================================================================
//
// DirList_Init()
//
// Initializes the DLDATA structure and sets up the listview control
//
void DirList_Init(HWND hwnd) noexcept {
	// Allocate DirListData Property
	DLDATA *lpdl = static_cast<DLDATA *>(GlobalAlloc(GPTR, sizeof(DLDATA)));
	SetProp(hwnd, pDirListProp, lpdl);

	// Setup dl
	lpdl->worker.Init(hwnd);
	lpdl->cbidl = 0;
	lpdl->pidl = nullptr;
	lpdl->lpsf = nullptr;
	StrCpyEx(lpdl->szPath, L"");

	SHFILEINFO shfi;
	// Add Imagelists
	HIMAGELIST hil = AsPointer<HIMAGELIST>(SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO), SHGFI_SMALLICON | SHGFI_SYSICONINDEX));
	ListView_SetImageList(hwnd, hil, LVSIL_SMALL);

	hil = AsPointer<HIMAGELIST>(SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO), SHGFI_LARGEICON | SHGFI_SYSICONINDEX));
	ListView_SetImageList(hwnd, hil, LVSIL_NORMAL);

	// Initialize default icons - done in DirList_Fill()
	//SHGetFileInfo(L"Icon", FILE_ATTRIBUTE_DIRECTORY, &shfi, sizeof(SHFILEINFO),
	//	SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	//lpdl->iDefIconFolder = shfi.iIcon;

	//SHGetFileInfo(L"Icon", FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
	//	SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	//lpdl->iDefIconFile = shfi.iIcon;

	lpdl->iDefIconFolder = 0;
	lpdl->iDefIconFile = 0;
}

//=============================================================================
//
// DirList_Destroy()
//
// Free memory used by dl structure
//
void DirList_Destroy(HWND hwnd) {
	DLDATA * const lpdl = static_cast<DLDATA *>(GetProp(hwnd, pDirListProp));

	lpdl->worker.Destroy();

	if (lpdl->pidl) {
		CoTaskMemFree(lpdl->pidl);
	}

	if (lpdl->lpsf) {
		lpdl->lpsf->Release();
	}

	// Free DirListData Property
	RemoveProp(hwnd, pDirListProp);
	GlobalFree(lpdl);
}

//=============================================================================
//
// DirList_StartIconThread()
//
// Start thread to extract file icons in the background
//
void DirList_StartIconThread(HWND hwnd) noexcept {
	DLDATA * const lpdl = static_cast<DLDATA *>(GetProp(hwnd, pDirListProp));

	lpdl->worker.Cancel();
	lpdl->worker.workerThread = CreateThread(nullptr, 0, DirList_IconThread, lpdl, 0, nullptr);
}

//=============================================================================
//
// DirList_Fill()
//
// Snapshots a directory and displays the items in the listview control
//
int DirList_Fill(HWND hwnd, LPCWSTR lpszDir, DWORD grfFlags, LPCWSTR lpszFileSpec,
				 bool bExcludeFilter, bool bNoFadeHidden, int iSortFlags, bool fSortRev) {
	DLDATA * const lpdl = static_cast<DLDATA *>(GetProp(hwnd, pDirListProp));
	SHFILEINFO shfi;

	// Initialize default icons
	SHGetFileInfo(L"Icon", FILE_ATTRIBUTE_DIRECTORY, &shfi, sizeof(SHFILEINFO),
				  SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	lpdl->iDefIconFolder = shfi.iIcon;

	SHGetFileInfo(L"Icon", FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
				  SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	lpdl->iDefIconFile = shfi.iIcon;

	// First of all terminate running icon thread
	lpdl->worker.Cancel();

	// A Directory is strongly required
	if (StrIsEmpty(lpszDir)) {
		return -1;
	}

	lstrcpy(lpdl->szPath, lpszDir);

	// Init ListView
	SendMessage(hwnd, WM_SETREDRAW, 0, 0);
	ListView_DeleteAllItems(hwnd);

	// Init Filter
	DirListFilter dlf;
	dlf.Create(lpszFileSpec, bExcludeFilter);

	// Init lvi
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.cchTextMax = MAX_PATH;
	lvi.iImage = I_IMAGECALLBACK;

	WCHAR wszDir[MAX_PATH];
	lstrcpy(wszDir, lpszDir);

	// Get Desktop Folder
	LPSHELLFOLDER lpsfDesktop = nullptr;
	PIDLIST_RELATIVE pidl = nullptr;
	LPSHELLFOLDER lpsf = nullptr;
	if (S_OK == SHGetDesktopFolder(&lpsfDesktop)) {
		// Convert wszDir into a pidl
		ULONG chParsed = 0;
		ULONG dwAttributes = 0;
		if (S_OK == lpsfDesktop->ParseDisplayName(hwnd, nullptr, wszDir, &chParsed, &pidl, &dwAttributes)) {
			// Bind pidl to IShellFolder
			if (S_OK == lpsfDesktop->BindToObject(pidl, nullptr, IID_IShellFolder, AsPPVArgs(&lpsf))) {
				// Create an Enumeration object for lpsf
				LPENUMIDLIST lpe = nullptr;
				if (S_OK == lpsf->EnumObjects(hwnd, grfFlags, &lpe)) {
					// Enumerate the contents of lpsf
					PITEMID_CHILD pidlEntry = nullptr;
					while (S_OK == lpe->Next(1, &pidlEntry, nullptr)) {
						// Add found item to the List
						// Check if it's part of the Filesystem
						dwAttributes = SFGAO_FILESYSTEM | SFGAO_FOLDER;
						lpsf->GetAttributesOf(1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&pidlEntry), &dwAttributes);

						if (dwAttributes & SFGAO_FILESYSTEM) {

							// Check if item matches specified filter
							if (dlf.Match(lpsf, pidlEntry)) {
								LV_ITEMDATA *lplvid = static_cast<LV_ITEMDATA *>(CoTaskMemAlloc(sizeof(LV_ITEMDATA)));
								lplvid->pidl = pidlEntry;
								lplvid->lpsf = lpsf;
								lpsf->AddRef();
								lvi.lParam = AsInteger<LPARAM>(lplvid);
								// Setup default Icon - Folder or File
								lvi.iImage = (dwAttributes & SFGAO_FOLDER) ? lpdl->iDefIconFolder : lpdl->iDefIconFile;
								ListView_InsertItem(hwnd, &lvi);
								lvi.iItem++;
							}
						}

					} // IEnumIDList::Next()

					lpe->Release();

				} // IShellFolder::EnumObjects()

			} // IShellFolder::BindToObject()

		} // IShellFolder::ParseDisplayName()

		lpsfDesktop->Release();
	} // SHGetDesktopFolder()

	if (lpdl->pidl) {
		CoTaskMemFree(lpdl->pidl);
	}

	if (lpdl->lpsf) {
		lpdl->lpsf->Release();
	}

	// Set lpdl
	lpdl->cbidl = IL_GetSize(pidl);
	lpdl->pidl = pidl;
	lpdl->lpsf = lpsf;
	lpdl->bNoFadeHidden = bNoFadeHidden;

	// Set column width to fit window
	ListView_SetColumnWidth(hwnd, 0, LVSCW_AUTOSIZE_USEHEADER);

	// Sort before display is updated
	DirList_Sort(hwnd, iSortFlags, fSortRev);

	// Redraw Listview
	SendMessage(hwnd, WM_SETREDRAW, 1, 0);

	// Return number of items in the control
	return ListView_GetItemCount(hwnd);
}

//=============================================================================
//
// DirList_IconThread()
//
// Thread to extract file icons in the background
//
DWORD WINAPI DirList_IconThread(LPVOID lpParam) {
	DLDATA * const lpdl = static_cast<DLDATA *>(lpParam);
	const BackgroundWorker &worker = lpdl->worker;

	// Exit immediately if DirList_Fill() hasn't been called
	if (!lpdl->lpsf) {
		return 0;
	}

	HWND hwnd = worker.hwnd;
	const int iMaxItem = ListView_GetItemCount(hwnd);

	// Get IShellIcon
	IShellIcon *lpshi;
	lpdl->lpsf->QueryInterface(IID_IShellIcon, AsPPVArgs(&lpshi));

	int iItem = 0;
	while (iItem < iMaxItem && worker.Continue()) {
		LV_ITEM lvi;
		lvi.iItem = iItem;
		lvi.mask = LVIF_PARAM;
		if (ListView_GetItem(hwnd, &lvi)) {
			LV_ITEMDATA *lplvid = AsPointer<LV_ITEMDATA *>(lvi.lParam);
			lvi.mask = LVIF_IMAGE;

			if (!lpshi || S_OK != lpshi->GetIconOf(reinterpret_cast<PCUITEMID_CHILD>(lplvid->pidl), GIL_FORSHELL, &lvi.iImage)) {
				SHFILEINFO shfi;
				LPITEMIDLIST pidl = IL_Create(lpdl->pidl, lpdl->cbidl, lplvid->pidl, 0);
				SHGetFileInfo(reinterpret_cast<LPCWSTR>(pidl), 0, &shfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
				CoTaskMemFree(pidl);
				lvi.iImage = shfi.iIcon;
			}

			// It proved necessary to reset the state bits...
			lvi.stateMask = 0;
			lvi.state = 0;

			DWORD dwAttributes = SFGAO_LINK | SFGAO_SHARE;
			// Link and Share Overlay
			lplvid->lpsf->GetAttributesOf(1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&lplvid->pidl), &dwAttributes);

			if (dwAttributes & SFGAO_LINK) {
				lvi.mask |= LVIF_STATE;
				lvi.stateMask |= LVIS_OVERLAYMASK;
				lvi.state |= INDEXTOOVERLAYMASK(2);
			}

			if (dwAttributes & SFGAO_SHARE) {
				lvi.mask |= LVIF_STATE;
				lvi.stateMask |= LVIS_OVERLAYMASK;
				lvi.state |= INDEXTOOVERLAYMASK(1);
			}

			// Fade hidden/system files
			if (!lpdl->bNoFadeHidden) {
				WIN32_FIND_DATA fd;
				if (S_OK == SHGetDataFromIDList(lplvid->lpsf, reinterpret_cast<PCUITEMID_CHILD>(lplvid->pidl), SHGDFIL_FINDDATA, &fd, sizeof(WIN32_FIND_DATA))) {
					if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) || (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
						lvi.mask |= LVIF_STATE;
						lvi.stateMask |= LVIS_CUT;
						lvi.state |= LVIS_CUT;
					}
				}
			}
			lvi.iSubItem = 0;
			ListView_SetItem(hwnd, &lvi);
		}
		iItem++;
	}

	if (lpshi) {
		lpshi->Release();
	}

	return 0;
}

//=============================================================================
//
// DirList_GetDispInfo()
//
// Must be called in response to a WM_NOTIFY/LVN_GETDISPINFO message from
// the listview control
//
bool DirList_GetDispInfo(HWND hwnd, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);

	LV_DISPINFO *lpdi = AsPointer<LV_DISPINFO *>(lParam);
	LV_ITEMDATA *lplvid = AsPointer<LV_ITEMDATA *>(lpdi->item.lParam);

	// SubItem 0 is handled only
	if (lpdi->item.iSubItem != 0) {
		return false;
	}

	// Text
	if (lpdi->item.mask & LVIF_TEXT) {
		IL_GetDisplayName(lplvid->lpsf, lplvid->pidl, SHGDN_INFOLDER, lpdi->item.pszText, lpdi->item.cchTextMax);
	}

	// Set values
	lpdi->item.mask |= LVIF_DI_SETITEM;

	return true;
}

//=============================================================================
//
// DirList_DeleteItem()
//
// Must be called in response to a WM_NOTIFY/LVN_DELETEITEM message
// from the control
//
bool DirList_DeleteItem(HWND hwnd, LPARAM lParam) {
	const NM_LISTVIEW *lpnmlv = AsPointer<NM_LISTVIEW *>(lParam);

	LV_ITEM lvi;
	lvi.iItem = lpnmlv->iItem;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;

	if (ListView_GetItem(hwnd, &lvi)) {
		// Free mem
		LV_ITEMDATA *lplvid = AsPointer<LV_ITEMDATA *>(lvi.lParam);
		CoTaskMemFree(lplvid->pidl);
		lplvid->lpsf->Release();
		CoTaskMemFree(lplvid);
		return true;
	}

	return false;
}

//=============================================================================
//
// DirList_CompareProc()
//
// Compares two list items
//
int CALLBACK DirList_CompareProcFw(LPARAM lp1, LPARAM lp2, LPARAM lFlags) {
	const LV_ITEMDATA * const lplvid1 = AsPointer<const LV_ITEMDATA *>(lp1);
	const LV_ITEMDATA * const lplvid2 = AsPointer<const LV_ITEMDATA *>(lp2);

	HRESULT hr = lplvid1->lpsf->CompareIDs(lFlags, reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid1->pidl), reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid2->pidl));
	int result = static_cast<short>(HRESULT_CODE(hr));

	if (result != 0 || lFlags == 0) {
		return result;
	}

	hr = lplvid1->lpsf->CompareIDs(0, reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid1->pidl), reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid2->pidl));
	result = static_cast<short>(HRESULT_CODE(hr));

	return result;
}

int CALLBACK DirList_CompareProcRw(LPARAM lp1, LPARAM lp2, LPARAM lFlags) {
	const LV_ITEMDATA * const lplvid1 = AsPointer<const LV_ITEMDATA *>(lp1);
	const LV_ITEMDATA * const lplvid2 = AsPointer<const LV_ITEMDATA *>(lp2);

	HRESULT hr = lplvid1->lpsf->CompareIDs(lFlags, reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid1->pidl), reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid2->pidl));
	int result = -static_cast<short>(HRESULT_CODE(hr));

	if (result != 0) {
		return result;
	}

	hr = lplvid1->lpsf->CompareIDs(0, reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid1->pidl), reinterpret_cast<PCUIDLIST_RELATIVE>(lplvid2->pidl));
	result = -static_cast<short>(HRESULT_CODE(hr));

	return result;
}

//=============================================================================
//
// DirList_Sort()
//
// Sorts the listview control by the specified order
//
BOOL DirList_Sort(HWND hwnd, int lFlags, bool fRev) noexcept {
	return ListView_SortItems(hwnd, (fRev? DirList_CompareProcRw : DirList_CompareProcFw), lFlags);
}

//=============================================================================
//
// DirList_GetItem()
//
// Copies the data of the specified item in the listview control to a buffer
//
int DirList_GetItem(HWND hwnd, int iItem, DirListItem *lpdli) {
	if (iItem < 0) {
		if (ListView_GetSelectedCount(hwnd)) {
			iItem = ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_SELECTED);
		} else {
			return -1;
		}
	}

	LV_ITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;

	if (!ListView_GetItem(hwnd, &lvi)) {
		if (lpdli->mask & DLI_TYPE) {
			lpdli->ntype = DLE_NONE;
		}
		return -1;
	}

	LV_ITEMDATA *lplvid = AsPointer<LV_ITEMDATA *>(lvi.lParam);

	// Filename
	if (lpdli->mask & DLI_FILENAME) {
		IL_GetDisplayName(lplvid->lpsf, lplvid->pidl, SHGDN_FORPARSING, lpdli->szFileName, MAX_PATH);
	}

	// Displayname
	if (lpdli->mask & DLI_DISPNAME) {
		IL_GetDisplayName(lplvid->lpsf, lplvid->pidl, SHGDN_INFOLDER, lpdli->szDisplayName, MAX_PATH);
	}

	// Type (File / Directory)
	if (lpdli->mask & DLI_TYPE) {
		WIN32_FIND_DATA fd;
		//ULONG dwAttributes = SFGAO_FILESYSTEM;

		if (S_OK == SHGetDataFromIDList(lplvid->lpsf, reinterpret_cast<PCUITEMID_CHILD>(lplvid->pidl), SHGDFIL_FINDDATA, &fd, sizeof(WIN32_FIND_DATA))) {
			lpdli->ntype = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DLE_DIR : DLE_FILE;
		}

		//lplvid->lpsf->GetAttributesOf(1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&lplvid->pidl), &dwAttributes);
		//lpdli->ntype = (dwAttributes & SFGAO_FOLDER) ? DLE_DIR : DLE_FILE;
	}

	return iItem;
}

//=============================================================================
//
// DirList_GetItemEx()
//
// Retrieves extended infomration on a dirlist item
//
int DirList_GetItemEx(HWND hwnd, int iItem, LPWIN32_FIND_DATA pfd) noexcept {
	if (iItem < 0) {
		if (ListView_GetSelectedCount(hwnd)) {
			iItem = ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_SELECTED);
		} else {
			return -1;
		}
	}

	LV_ITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;

	if (!ListView_GetItem(hwnd, &lvi)) {
		return -1;
	}

	LV_ITEMDATA *lplvid = AsPointer<LV_ITEMDATA *>(lvi.lParam);
	if (S_OK == SHGetDataFromIDList(lplvid->lpsf, reinterpret_cast<PCUITEMID_CHILD>(lplvid->pidl), SHGDFIL_FINDDATA, pfd, sizeof(WIN32_FIND_DATA))) {
		return iItem;
	}
	return -1;
}

//=============================================================================
//
// DirList_PropertyDlg()
//
// Shows standard Win95 Property Dlg for selected Item
//
bool DirList_PropertyDlg(HWND hwnd, int iItem) {
	if (iItem < 0) {
		if (ListView_GetSelectedCount(hwnd)) {
			iItem = ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_SELECTED);
		} else {
			return false;
		}
	}

	LV_ITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;

	if (!ListView_GetItem(hwnd, &lvi)) {
		return false;
	}

	bool bSuccess = true;
	LV_ITEMDATA *lplvid = AsPointer<LV_ITEMDATA *>(lvi.lParam);
	LPCONTEXTMENU lpcm;

	if (S_OK == lplvid->lpsf->GetUIObjectOf(GetParent(hwnd), 1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&lplvid->pidl), IID_IContextMenu, nullptr, AsPPVArgs(&lpcm))) {
		CMINVOKECOMMANDINFO cmi;
		cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
		cmi.fMask = 0;
		cmi.hwnd = GetParent(hwnd);
		cmi.lpVerb = "properties";
		cmi.lpParameters = nullptr;
		cmi.lpDirectory = nullptr;
		cmi.nShow = SW_SHOWNORMAL;
		cmi.dwHotKey = 0;
		cmi.hIcon = nullptr;

		if (S_OK != lpcm->InvokeCommand(&cmi)) {
			bSuccess = false;
		}

		lpcm->Release();
	} else {
		bSuccess = false;
	}
	return bSuccess;
}

//=============================================================================
//
// DirList_GetLongPathName()
//
// Get long pathname for currently displayed directory
//
bool DirList_GetLongPathName(HWND hwnd, LPWSTR lpszLongPath) noexcept {
	WCHAR tch[MAX_PATH];
	const DLDATA * const lpdl = static_cast<DLDATA *>(GetProp(hwnd, pDirListProp));
	if (SHGetPathFromIDList(reinterpret_cast<PCIDLIST_ABSOLUTE>(lpdl->pidl), tch)) {
		lstrcpy(lpszLongPath, tch);
		return true;
	}
	return false;
}

//=============================================================================
//
// DirList_SelectItem()
//
// Select specified item in the list
//
bool DirList_SelectItem(HWND hwnd, LPCWSTR lpszDisplayName, LPCWSTR lpszFullPath) {
#define LVIS_FLAGS (LVIS_SELECTED | LVIS_FOCUSED)

	if (StrIsEmpty(lpszFullPath)) {
		return false;
	}

	WCHAR szShortPath[MAX_PATH];
	SHFILEINFO shfi;

	GetShortPathName(lpszFullPath, szShortPath, MAX_PATH);
	if (StrIsEmpty(lpszDisplayName)) {
		SHGetFileInfo(lpszFullPath, 0, &shfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
	} else {
		lstrcpyn(shfi.szDisplayName, lpszDisplayName, MAX_PATH);
	}

	LV_FINDINFO lvfi;
	lvfi.flags = LVFI_STRING;
	lvfi.psz = shfi.szDisplayName;

	DirListItem dli;
	dli.mask = DLI_ALL;

	int i = -1;
	while ((i = ListView_FindItem(hwnd, i, &lvfi)) >= 0) {
		DirList_GetItem(hwnd, i, &dli);
		GetShortPathName(dli.szFileName, dli.szFileName, MAX_PATH);

		if (PathEqual(dli.szFileName, szShortPath)) {
			ListView_SetItemState(hwnd, i, LVIS_FLAGS, LVIS_FLAGS);
			ListView_EnsureVisible(hwnd, i, FALSE);
			return true;
		}
	}

	return false;
}

//=============================================================================
//
// Create a valid DirListFilter structure
//
void DirListFilter::Create(LPCWSTR lpszFileSpec, bool bExclude) noexcept {
	memset(this, 0, sizeof(DirListFilter));
	if (StrIsEmpty(lpszFileSpec) || StrEqualEx(lpszFileSpec, L"*.*")) {
		return;
	}

	lstrcpyn(tFilterBuf, lpszFileSpec, (DL_FILTER_BUFSIZE - 1));
	bExcludeFilter = bExclude;
	nCount = 1;
	pFilter[0] = tFilterBuf; // Zeile zum Ausprobieren

	WCHAR *p;
	while ((p = StrChr(pFilter[nCount - 1], L';')) != nullptr) {
		*p = L'\0';			// Replace L';' by L'\0'
		pFilter[nCount] = (p + 1);	// Next position after L';'
		nCount++;		// Increase number of filters
	}
}

//=============================================================================
//
// Check if a specified item matches a given filter
//
bool DirListFilter::Match(LPSHELLFOLDER lpsf, LPCITEMIDLIST pidl) const noexcept {
	// Immediately return true if lpszFileSpec is *.* or nullptr
	if (nCount == 0 && !bExcludeFilter) {
		return true;
	}

	WIN32_FIND_DATA fd;
	SHGetDataFromIDList(lpsf, reinterpret_cast<PCUITEMID_CHILD>(pidl), SHGDFIL_FINDDATA, &fd, sizeof(WIN32_FIND_DATA));

	// All the directories are added
	if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		return true;
	}

	// Check if exclude *.* after directories have been added
	if (nCount == 0 && bExcludeFilter) {
		return false;
	}

	for (int i = 0; i < nCount; i++) {
		if (*pFilter[i]) { // Filters like L"\0" are ignored
			const BOOL bMatchSpec = PathMatchSpec(fd.cFileName, pFilter[i]);
			if (bMatchSpec) {
				return !bExcludeFilter;
			}
		}
	}

	// No matching
	return bExcludeFilter;
}

//==== DriveBox ===============================================================

//=============================================================================
//
// Internal Itemdata Structure
//
struct DC_ITEMDATA {
	LPITEMIDLIST pidl;
	LPSHELLFOLDER lpsf;
};

//=============================================================================
//
// DriveBox_Init()
//
// Initializes the drive box
//
bool DriveBox_Init(HWND hwnd) noexcept {
	SHFILEINFO shfi;
	DWORD_PTR hil = SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	SendMessage(hwnd, CBEM_SETIMAGELIST, 0, hil);
	SendMessage(hwnd, CBEM_SETEXTENDEDSTYLE, CBES_EX_NOSIZELIMIT, CBES_EX_NOSIZELIMIT);

	return true;
}

//=============================================================================
//
// DriveBox_Fill
//
int DriveBox_Fill(HWND hwnd) {
	// Init ComboBox
	SendMessage(hwnd, WM_SETREDRAW, 0, 0);
	ComboBox_ResetContent(hwnd);

	COMBOBOXEXITEM cbei;
	memset(&cbei, 0, sizeof(COMBOBOXEXITEM));
	cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
	cbei.pszText = LPSTR_TEXTCALLBACK;
	cbei.cchTextMax = MAX_PATH;
	cbei.iImage = I_IMAGECALLBACK;
	cbei.iSelectedImage = I_IMAGECALLBACK;

	// Get pidl to [My Computer]
	PIDLIST_ABSOLUTE pidl;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	if (S_OK == SHGetKnownFolderIDList(FOLDERID_ComputerFolder, KF_FLAG_DEFAULT, nullptr, &pidl))
#else
	if (S_OK == SHGetFolderLocation(hwnd, CSIDL_DRIVES, nullptr, SHGFP_TYPE_DEFAULT, &pidl))
#endif
	{
		// Get Desktop Folder
		LPSHELLFOLDER lpsfDesktop;
		if (S_OK == SHGetDesktopFolder(&lpsfDesktop)) {
			// Bind pidl to IShellFolder
			LPSHELLFOLDER lpsf; // Workspace == CSIDL_DRIVES
			if (S_OK == lpsfDesktop->BindToObject(pidl, nullptr, IID_IShellFolder, AsPPVArgs(&lpsf))) {
				// Create an Enumeration object for lpsf
				constexpr DWORD grfFlags = SHCONTF_FOLDERS;
				LPENUMIDLIST lpe;
				if (S_OK == lpsf->EnumObjects(hwnd, grfFlags, &lpe)) {
					// Enumerate the contents of [My Computer]
					PITEMID_CHILD pidlEntry;
					while (S_OK == lpe->Next(1, &pidlEntry, nullptr)) {
						// Add item to the List if it is part of the
						// Filesystem
						ULONG dwAttributes = SFGAO_FILESYSTEM;
						lpsf->GetAttributesOf(1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&pidlEntry), &dwAttributes);

						if (dwAttributes & SFGAO_FILESYSTEM) {

							// Windows XP: check if pidlEntry is a drive
							SHDESCRIPTIONID di;
							HRESULT hr = SHGetDataFromIDList(lpsf, pidlEntry, SHGDFIL_DESCRIPTIONID, &di, sizeof(SHDESCRIPTIONID));
							if (hr != S_OK || (di.dwDescriptionId >= SHDID_COMPUTER_DRIVE35 && di.dwDescriptionId <= SHDID_COMPUTER_OTHER)) {
								DC_ITEMDATA * const lpdcid = static_cast<DC_ITEMDATA *>(CoTaskMemAlloc(sizeof(DC_ITEMDATA)));

								//lpdcid->pidl = IL_Copy(pidlEntry);
								lpdcid->pidl = pidlEntry;
								lpdcid->lpsf = lpsf;

								lpsf->AddRef();

								// Insert sorted ...
								{
									COMBOBOXEXITEM cbei2;
									cbei2.mask = CBEIF_LPARAM;
									cbei2.iItem = 0;

									while ((SendMessage(hwnd, CBEM_GETITEM, 0, AsInteger<LPARAM>(&cbei2)))) {
										const DC_ITEMDATA * const lpdcid2 = AsPointer<const DC_ITEMDATA *>(cbei2.lParam);
										hr = lpdcid->lpsf->CompareIDs(0, reinterpret_cast<PCUIDLIST_RELATIVE>(lpdcid->pidl), reinterpret_cast<PCUIDLIST_RELATIVE>(lpdcid2->pidl));
										if (static_cast<short>(HRESULT_CODE(hr)) < 0) {
											break;
										}
										cbei2.iItem++;
									}

									cbei.iItem = cbei2.iItem;
									cbei.lParam = AsInteger<LPARAM>(lpdcid);
									SendMessage(hwnd, CBEM_INSERTITEM, 0, AsInteger<LPARAM>(&cbei));
								}
							}
						}
					} // IEnumIDList::Next()

					lpe->Release();
				} // IShellFolder::EnumObjects()

				lpsf->Release();
			} // IShellFolder::BindToObject()
			CoTaskMemFree(pidl);
		} // SHGetDesktopFolder()

		lpsfDesktop->Release();
	} // SHGetKnownFolderIDList()

	SendMessage(hwnd, WM_SETREDRAW, 1, 0);
	// Return number of items added to combo box
	return ComboBox_GetCount(hwnd);
}

//=============================================================================
//
// DriveBox_GetSelDrive
//
bool DriveBox_GetSelDrive(HWND hwnd, LPWSTR lpszDrive, int nDrive, bool fNoSlash) {
	const int i = ComboBox_GetCurSel(hwnd);
	// CB_ERR means no Selection
	if (i == CB_ERR) {
		return false;
	}

	// Get DC_ITEMDATA* of selected Item
	COMBOBOXEXITEM cbei;
	cbei.mask = CBEIF_LPARAM;
	cbei.iItem = i;
	SendMessage(hwnd, CBEM_GETITEM, 0, AsInteger<LPARAM>(&cbei));
	const DC_ITEMDATA * const lpdcid = AsPointer<const DC_ITEMDATA *>(cbei.lParam);

	// Get File System Path for Drive
	IL_GetDisplayName(lpdcid->lpsf, lpdcid->pidl, SHGDN_FORPARSING, lpszDrive, nDrive);

	// Remove Backslash if required (makes Drive relative!!!)
	if (fNoSlash) {
		PathRemoveBackslash(lpszDrive);
	}

	return true;
}

//=============================================================================
//
// DriveBox_SelectDrive
//
bool DriveBox_SelectDrive(HWND hwnd, LPCWSTR lpszPath) {
	const int cbItems = ComboBox_GetCount(hwnd);
	// No Drives in Combo Box
	if (!cbItems) {
		return false;
	}

	COMBOBOXEXITEM cbei;
	cbei.mask = CBEIF_LPARAM;

	for (int i = 0; i < cbItems; i++) {
		WCHAR szRoot[64] = L"";
		// Get DC_ITEMDATA* of Item i
		cbei.iItem = i;
		SendMessage(hwnd, CBEM_GETITEM, 0, AsInteger<LPARAM>(&cbei));
		const DC_ITEMDATA * const lpdcid = AsPointer<const DC_ITEMDATA *>(cbei.lParam);

		// Get File System Path for Drive
		IL_GetDisplayName(lpdcid->lpsf, lpdcid->pidl, SHGDN_FORPARSING, szRoot, COUNTOF(szRoot));

		// Compare Root Directory with Path
		if (PathIsSameRoot(lpszPath, szRoot)) {
			// Select matching Drive
			ComboBox_SetCurSel(hwnd, i);
			return true;
		}
	}

	// Don't select anything
	ComboBox_SetCurSel(hwnd, -1);
	return false;
}

//=============================================================================
//
// DriveBox_PropertyDlg()
//
// Shows standard Win95 Property Dlg for selected Drive
//
bool DriveBox_PropertyDlg(HWND hwnd) {
	const int iItem = ComboBox_GetCurSel(hwnd);
	if (iItem == CB_ERR) {
		return false;
	}

	bool bSuccess = true;
	COMBOBOXEXITEM cbei;
	cbei.mask = CBEIF_LPARAM;
	cbei.iItem = iItem;
	SendMessage(hwnd, CBEM_GETITEM, 0, AsInteger<LPARAM>(&cbei));
	const DC_ITEMDATA * const lpdcid = AsPointer<const DC_ITEMDATA *>(cbei.lParam);
	LPCONTEXTMENU lpcm;

	if (S_OK == lpdcid->lpsf->GetUIObjectOf(GetParent(hwnd), 1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&lpdcid->pidl), IID_IContextMenu, nullptr, AsPPVArgs(&lpcm))) {
		CMINVOKECOMMANDINFO cmi;
		cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
		cmi.fMask = 0;
		cmi.hwnd = GetParent(hwnd);
		cmi.lpVerb = "properties";
		cmi.lpParameters = nullptr;
		cmi.lpDirectory = nullptr;
		cmi.nShow = SW_SHOWNORMAL;
		cmi.dwHotKey = 0;
		cmi.hIcon = nullptr;

		if (S_OK != lpcm->InvokeCommand(&cmi)) {
			bSuccess = false;
		}

		lpcm->Release();
	} else {
		bSuccess = false;
	}

	return bSuccess;
}

//=============================================================================
//
// DriveBox_DeleteItem
//
bool DriveBox_DeleteItem(HWND hwnd, LPARAM lParam) {
	const NMCOMBOBOXEX *lpnmcbe = AsPointer<NMCOMBOBOXEX *>(lParam);
	COMBOBOXEXITEM cbei;
	cbei.iItem = lpnmcbe->ceItem.iItem;

	cbei.mask = CBEIF_LPARAM;
	SendMessage(hwnd, CBEM_GETITEM, 0, AsInteger<LPARAM>(&cbei));
	DC_ITEMDATA * const lpdcid = AsPointer<DC_ITEMDATA *>(cbei.lParam);

	// Free pidl
	CoTaskMemFree(lpdcid->pidl);
	// Release lpsf
	lpdcid->lpsf->Release();
	// Free lpdcid itself
	CoTaskMemFree(lpdcid);

	return true;
}

//=============================================================================
//
// DriveBox_GetDispInfo
//
bool DriveBox_GetDispInfo(HWND hwnd, LPARAM lParam) {
	UNREFERENCED_PARAMETER(hwnd);

	NMCOMBOBOXEX *lpnmcbe = AsPointer<NMCOMBOBOXEX *>(lParam);
	const DC_ITEMDATA * const lpdcid = AsPointer<const DC_ITEMDATA *>(lpnmcbe->ceItem.lParam);

	if (!lpdcid) {
		return false;
	}

	// Get Display Name
	if (lpnmcbe->ceItem.mask & CBEIF_TEXT) {
		IL_GetDisplayName(lpdcid->lpsf, lpdcid->pidl, SHGDN_NORMAL, lpnmcbe->ceItem.pszText, lpnmcbe->ceItem.cchTextMax);
	}

	// Get Icon Index
	if (lpnmcbe->ceItem.mask & (CBEIF_IMAGE | CBEIF_SELECTEDIMAGE)) {
		WCHAR szTemp[MAX_PATH];
		SHFILEINFO shfi;
		IL_GetDisplayName(lpdcid->lpsf, lpdcid->pidl, SHGDN_FORPARSING, szTemp, MAX_PATH);
		SHGetFileInfo(szTemp, 0, &shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		lpnmcbe->ceItem.iImage = shfi.iIcon;
		lpnmcbe->ceItem.iSelectedImage = shfi.iIcon;
	}

	// Set values
	lpnmcbe->ceItem.mask |= CBEIF_DI_SETITEM;

	return true;
}

//==== ItemID =================================================================

//=============================================================================
//
// IL_Create()
//
// Creates an ITEMIDLIST by concatenating pidl1 and pidl2
// cb1 and cb2 indicate the sizes of the pidls, where cb1
// can be zero and pidl1 can be nullptr
//
// If cb2 is zero, the size of pidl2 is retrieved using
// IL_GetSize(pidl2)
//
LPITEMIDLIST IL_Create(LPCITEMIDLIST pidl1, UINT cb1, LPCITEMIDLIST pidl2, UINT cb2) noexcept {
	if (!pidl2) {
		return nullptr;
	}

	if (!cb2) {
		cb2 = IL_GetSize(pidl2) + 2;    // Space for terminating Bytes
	}

	if (!cb1) {
		cb1 = IL_GetSize(pidl1);
	}

	// Allocate Memory
	LPITEMIDLIST pidl = static_cast<LPITEMIDLIST>(CoTaskMemAlloc(cb1 + cb2));

	// Init new ITEMIDLIST
	if (pidl1) {
		memcpy(reinterpret_cast<char *>(pidl), reinterpret_cast<const char *>(pidl1), cb1);
	}

	// pidl2 can't be nullptr here
	memcpy(reinterpret_cast<char *>(pidl) + cb1, reinterpret_cast<const char *>(pidl2), cb2);

	return pidl;
}

//=============================================================================
//
// IL_GetSize()
//
// Retrieves the number of bytes in a pidl
// Does not add space for zero terminators !!
//
UINT IL_GetSize(LPCITEMIDLIST pidl) noexcept {
	if (!pidl) {
		return 0;
	}

	UINT cb = 0;
	for (LPCITEMIDLIST pidlTmp = pidl; pidlTmp->mkid.cb; pidlTmp = IL_Next(pidlTmp)) {
		cb += pidlTmp->mkid.cb;
	}

	return cb;
}

//=============================================================================
//
// IL_GetDisplayName()
//
// Gets the Display Name of a pidl. lpsf is the parent IShellFolder Interface
// dwFlags specify a SHGDN_xx value
//
bool IL_GetDisplayName(LPSHELLFOLDER lpsf, LPCITEMIDLIST pidl, DWORD dwFlags, LPWSTR lpszDisplayName, int nDisplayName) {
	STRRET str;

	if (S_OK == lpsf->GetDisplayNameOf(reinterpret_cast<PCUITEMID_CHILD>(pidl), dwFlags, &str)) {
		return StrRetToBuf(&str, reinterpret_cast<PCUITEMID_CHILD>(pidl), lpszDisplayName, nDisplayName) == S_OK;
	}

	return false;
}
