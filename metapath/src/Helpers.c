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
*                                              https://www.flos-freeware.ch
*
*
******************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <dde.h>
#include <ddeml.h>
#include <dlgs.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <psapi.h>
#include <stdio.h>
#include "config.h"
#include "Helpers.h"
#include "../../scintilla/include/VectorISA.h"
#include "../../scintilla/include/GraphicUtils.h"
#include "Dlapi.h"
#include "resource.h"

void IniClearSectionEx(LPCWSTR lpSection, LPCWSTR lpszIniFile, bool bDelete) {
	if (StrIsEmpty(lpszIniFile)) {
		return;
	}

	WritePrivateProfileSection(lpSection, (bDelete ? NULL : L""), lpszIniFile);
}

void IniClearAllSectionEx(LPCWSTR lpszPrefix, LPCWSTR lpszIniFile, bool bDelete) {
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
bool IniSectionParseArray(IniSection *section, LPWSTR lpCachedIniSection) {
	IniSectionClear(section);
	if (StrIsEmpty(lpCachedIniSection)) {
		return false;
	}

	const UINT capacity = section->capacity;
	LPWSTR p = lpCachedIniSection;
	UINT count = 0;

	do {
		LPWSTR v = StrChr(p, L'=');
		if (v != NULL) {
			*v++ = L'\0';
			IniKeyValueNode *node = &section->nodeList[count];
			node->key = p;
			node->value = v;
			++count;
			p = v;
		}
		p = StrEnd(p) + 1;
	} while (*p && count < capacity);

	section->count = count;
	return count != 0;
}

bool IniSectionParse(IniSection *section, LPWSTR lpCachedIniSection) {
	IniSectionClear(section);
	if (StrIsEmpty(lpCachedIniSection)) {
		return false;
	}

	const UINT capacity = section->capacity;
	LPWSTR p = lpCachedIniSection;
	UINT count = 0;

	do {
		LPWSTR v = StrChr(p, L'=');
		if (v != NULL) {
			*v++ = L'\0';
			const UINT keyLen = (UINT)(v - p - 1);
			IniKeyValueNode *node = &section->nodeList[count];
			node->hash = keyLen | ((*(const UINT *)p) << 8);
			node->key = p;
			node->value = v;
			++count;
			p = v;
		}
		p = StrEnd(p) + 1;
	} while (*p && count < capacity);

	if (count == 0) {
		return false;
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
	return true;
}

LPCWSTR IniSectionUnsafeGetValue(IniSection *section, LPCWSTR key, int keyLen) {
	if (keyLen == 0) {
		keyLen = lstrlen(key);
	}

	const UINT hash = keyLen | ((*(const UINT *)key) << 8);
	IniKeyValueNode *node = section->head;
	IniKeyValueNode *prev = NULL;
#if IniSectionImplUseSentinelNode
	section->sentinel->hash = hash;
	while (true) {
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

bool IniSectionGetBoolImpl(IniSection *section, LPCWSTR key, int keyLen, bool bDefault) {
	LPCWSTR value = IniSectionGetValueImpl(section, key, keyLen);
	if (value) {
		const UINT t = *value - L'0';
		if (t <= true) {
			return t & true;
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
	const LSTATUS status = RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE *)lpszText, len);
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

bool FindUserResourcePath(LPCWSTR path, LPWSTR outPath) {
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
				return true;
			}
		}

		// relative to program exe file
		GetProgramRealPath(tchBuild, COUNTOF(tchBuild));
		lstrcpy(PathFindFileName(tchBuild), tchFileExpanded);
		if (PathIsFile(tchBuild)) {
			lstrcpy(outPath, tchBuild);
			return true;
		}
	} else if (PathIsFile(tchFileExpanded)) {
		lstrcpy(outPath, tchFileExpanded);
		return true;
	}
	return false;
}

HBITMAP LoadBitmapFile(LPCWSTR path) {
	WCHAR szTmp[MAX_PATH];
	if (!FindUserResourcePath(path, szTmp)) {
		return NULL;
	}

	HBITMAP hbmp = (HBITMAP)LoadImage(NULL, szTmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	return hbmp;
}

HBITMAP ResizeImageForCurrentDPI(HBITMAP hbmp) {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		// assume 16x16 at 100% scaling
		const int height = (g_uCurrentDPI*16) / USER_DEFAULT_SCREEN_DPI;
		if (height == bmp.bmHeight && bmp.bmBitsPixel == 32) {
			return hbmp;
		}
		// keep aspect ratio
		const int width = MulDiv(height, bmp.bmWidth, bmp.bmHeight);
		HBITMAP hCopy = (HBITMAP)CopyImage(hbmp, IMAGE_BITMAP, width, height, LR_COPYRETURNORG | LR_COPYDELETEORG);
		if (hCopy != NULL) {
#if 0
			BITMAP bmp2;
			if (GetObject(hCopy, sizeof(BITMAP), &bmp2)) {
				printf("%s %u: (%d x %d, %d) => (%d x %d, %d)\n", __func__, g_uCurrentDPI,
				bmp.bmWidth, bmp.bmHeight, bmp.bmBitsPixel, bmp2.bmWidth, bmp2.bmHeight, bmp2.bmBitsPixel);
			}
#endif
			hbmp = hCopy;
		}
	}

	return hbmp;
}


void BackgroundWorker_Init(BackgroundWorker *worker, HWND hwnd) {
	worker->hwnd = hwnd;
	worker->eventCancel = CreateEvent(NULL, TRUE, FALSE, NULL);
	worker->workerThread = NULL;
}

void BackgroundWorker_Stop(BackgroundWorker *worker) {
	SetEvent(worker->eventCancel);
	HANDLE workerThread = InterlockedExchangePointer(&worker->workerThread, NULL);
	if (workerThread) {
		while (WaitForSingleObject(workerThread, 0) != WAIT_OBJECT_0) {
			MSG msg;
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		CloseHandle(workerThread);
	}
}

void BackgroundWorker_Cancel(BackgroundWorker *worker) {
	BackgroundWorker_Stop(worker);
	ResetEvent(worker->eventCancel);
}

void BackgroundWorker_Destroy(BackgroundWorker *worker) {
	BackgroundWorker_Stop(worker);
	CloseHandle(worker->eventCancel);
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
bool IsElevated(void) {
	bool bIsElevated = false;
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION te;
		DWORD dwReturnLength = 0;

		if (GetTokenInformation(hToken, TokenElevation, &te, sizeof(te), &dwReturnLength)) {
			if (dwReturnLength == sizeof(te)) {
				bIsElevated = te.TokenIsElevated != 0;
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
bool ExeNameFromWnd(HWND hwnd, LPWSTR szExeName, DWORD cchExeName) {
	DWORD dwProcessId;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	QueryFullProcessImageName(hProcess, 0, szExeName, &cchExeName);
#else
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
	GetModuleFileNameEx(hProcess, NULL, szExeName, cchExeName);
#endif
	CloseHandle(hProcess);
	return true;
}

////=============================================================================
////
////  Is32bitExe()
////
/*
bool Is32bitExe(LPCWSTR lpszExeName) {
	SHFILEINFO shfi;
	DWORD dwExeType;
	WCHAR tch[MAX_PATH];

	if (PathGetLnkPath(lpszExeName, tch)) {
		dwExeType = SHGetFileInfo(tch, 0, &shfi, sizeof(SHFILEINFO), SHGFI_EXETYPE);
	} else {
		dwExeType = SHGetFileInfo(lpszExeName, 0, &shfi, sizeof(SHFILEINFO), SHGFI_EXETYPE);
	}

	return (HIWORD(dwExeType) && LOWORD(dwExeType)) != 0;
}
*/

//=============================================================================
//
//  BitmapAlphaBlend()
//  Perform alpha blending to color channel only
//
bool BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha) {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		if (bmp.bmBitsPixel == 32) {
			//StopWatch watch;
			//StopWatch_Start(watch);
			//FILE *fp = fopen("bitmap.dat", "wb");
			//fwrite(bmp.bmBits, 1, bmp.bmHeight*bmp.bmWidth*4, fp);
			//fclose(fp);
#if NP2_USE_AVX2
#if 1
			#define BitmapAlphaBlend_Tag	"avx2 4x1"
			const ULONG count = (bmp.bmHeight * bmp.bmWidth) / 4;
			__m128i *prgba = (__m128i *)bmp.bmBits;

			const __m256i i16x16Alpha = _mm256_broadcastw_epi16(_mm_cvtsi32_si128(alpha));
			const __m256i i16x16Back = _mm256_broadcastq_epi64(_mm_mullo_epi16(rgba_to_bgra_epi16_sse4_si32(crDest), mm_xor_alpha_epi16(_mm256_castsi256_si128(i16x16Alpha))));
			const __m256i i16x16_0x8081 = _mm256_broadcastsi128_si256(_mm_set1_epi16(-0x8000 | 0x81));
			for (ULONG x = 0; x < count; x++, prgba++) {
				const __m256i origin = _mm256_cvtepu8_epi16(*prgba);
				__m256i i16x16Fore = _mm256_mullo_epi16(origin, i16x16Alpha);
				i16x16Fore = _mm256_add_epi16(i16x16Fore, i16x16Back);
				i16x16Fore = _mm256_srli_epi16(_mm256_mulhi_epu16(i16x16Fore, i16x16_0x8081), 7);
				i16x16Fore = _mm256_blend_epi16(origin, i16x16Fore, 0x77);
				i16x16Fore = _mm256_packus_epi16(i16x16Fore, i16x16Fore);
				i16x16Fore = _mm256_permute4x64_epi64(i16x16Fore, 8);
				_mm_storeu_si128(prgba, _mm256_castsi256_si128(i16x16Fore));
			}
#else
			#define BitmapAlphaBlend_Tag	"sse4 2x1"
			const ULONG count = (bmp.bmHeight * bmp.bmWidth) / 2;
			uint64_t *prgba = (uint64_t *)bmp.bmBits;

			const __m128i i16x8Alpha = _mm_broadcastw_epi16(_mm_cvtsi32_si128(alpha));
			const __m128i i16x8Back = _mm_mullo_epi16(rgba_to_bgra_epi16x8_sse4_si32(crDest), mm_xor_alpha_epi16(i16x8Alpha));
			for (ULONG x = 0; x < count; x++, prgba++) {
				const __m128i origin = unpack_color_epi16_sse4_ptr64(prgba);
				__m128i i16x8Fore = _mm_mullo_epi16(origin, i16x8Alpha);
				i16x8Fore = _mm_add_epi16(i16x8Fore, i16x8Back);
				i16x8Fore = mm_div_epu16_by_255(i16x8Fore);
				i16x8Fore = _mm_blend_epi16(origin, i16x8Fore, 0x77);
				i16x8Fore = pack_color_epi16_sse2_si128(i16x8Fore);
				_mm_storel_epi64((__m128i *)prgba, i16x8Fore);
			}
#endif // NP2_USE_AVX2
#elif NP2_USE_SSE2
			#define BitmapAlphaBlend_Tag	"sse2 1x4"
			const ULONG count = (bmp.bmHeight * bmp.bmWidth) / 4;
			__m128i *prgba = (__m128i *)bmp.bmBits;

			const __m128i i16x8Alpha = _mm_shuffle_epi32(mm_setlo_alpha_epi16(alpha), 0x44);
			__m128i i16x8Back = _mm_shuffle_epi32(rgba_to_bgra_epi16_sse2_si32(crDest), 0x44);
			i16x8Back = _mm_mullo_epi16(i16x8Back, mm_xor_alpha_epi16(i16x8Alpha));
			for (ULONG x = 0; x < count; x++, prgba++) {
				__m128i origin = _mm_loadu_si128(prgba);
				__m128i color42 = _mm_shuffle_epi32(origin, 0x31);

				__m128i i16x8Fore = unpacklo_color_epi16_sse2_si32(origin);
				i16x8Fore = _mm_unpacklo_epi64(i16x8Fore, unpacklo_color_epi16_sse2_si32(color42));
				i16x8Fore = _mm_mullo_epi16(i16x8Fore, i16x8Alpha);
				i16x8Fore = _mm_add_epi16(i16x8Fore, i16x8Back);
				i16x8Fore = mm_div_epu16_by_255(i16x8Fore);
				__m128i i32x4Fore = pack_color_epi16_sse2_si128(i16x8Fore);

				i16x8Fore = unpackhi_color_epi16_sse2_si128(origin);
				i16x8Fore = _mm_unpacklo_epi64(i16x8Fore, unpackhi_color_epi16_sse2_si128(color42));
				i16x8Fore = _mm_mullo_epi16(i16x8Fore, i16x8Alpha);
				i16x8Fore = _mm_add_epi16(i16x8Fore, i16x8Back);
				i16x8Fore = mm_div_epu16_by_255(i16x8Fore);
				color42 = pack_color_epi16_sse2_si128(i16x8Fore);

				i32x4Fore = _mm_unpacklo_epi64(i32x4Fore, color42);
				i32x4Fore = _mm_and_si128(_mm_set1_epi32(0x00ffffff), i32x4Fore);
				origin = _mm_andnot_si128(_mm_set1_epi32(0x00ffffff), origin);
				i32x4Fore = _mm_or_si128(origin, i32x4Fore);
				_mm_storeu_si128(prgba, i32x4Fore);
			}

#else
			#define BitmapAlphaBlend_Tag	"scalar"
			const ULONG count = bmp.bmHeight * bmp.bmWidth;
			RGBQUAD *prgba = (RGBQUAD *)bmp.bmBits;

			const WORD red = GetRValue(crDest) * (255 ^ alpha);
			const WORD green = GetGValue(crDest) * (255 ^ alpha);
			const WORD blue = GetBValue(crDest) * (255 ^ alpha);
			for (ULONG x = 0; x < count; x++) {
				prgba[x].rgbRed = ((prgba[x].rgbRed * alpha) + red) >> 8;
				prgba[x].rgbGreen = ((prgba[x].rgbGreen * alpha) + green) >> 8;
				prgba[x].rgbBlue = ((prgba[x].rgbBlue * alpha) + blue) >> 8;
			}
#endif
			//StopWatch_Stop(watch);
			//StopWatch_ShowLog(&watch, "BitmapAlphaBlend " BitmapAlphaBlend_Tag);
			#undef BitmapAlphaBlend_Tag
			return true;
		}
	}
	return false;
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
#define MAX_RESIZEDLG_ATTR_COUNT	2
// temporary fix for moving dialog to monitor with different DPI
// TODO: all dimensions no longer valid after window DPI changed.
#define NP2_ENABLE_RESIZEDLG_TEMP_FIX	0

typedef struct RESIZEDLG {
	int direction;
	UINT dpi;
	int cxClient;
	int cyClient;
	int mmiPtMinX;
	int mmiPtMinY;
	int mmiPtMaxX;	// only Y direction
	int mmiPtMaxY;	// only X direction
	int attrs[MAX_RESIZEDLG_ATTR_COUNT];
} RESIZEDLG, *PRESIZEDLG;

typedef const RESIZEDLG * LPCRESIZEDLG;

void ResizeDlg_InitEx(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int iDirection) {
	const UINT dpi = GetWindowDPI(hwnd);
	RESIZEDLG *pm = (RESIZEDLG *)NP2HeapAlloc(sizeof(RESIZEDLG));
	pm->direction = iDirection;
	pm->dpi = dpi;

	RECT rc;
	GetClientRect(hwnd, &rc);
	pm->cxClient = rc.right - rc.left;
	pm->cyClient = rc.bottom - rc.top;

	const DWORD style = GetWindowStyle(hwnd) | WS_THICKFRAME;
	AdjustWindowRectForDpi(&rc, style, 0, dpi);
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
	const int cGrip = SystemMetricsForDpi(SM_CXHTHUMB, dpi);
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
#if NP2_ENABLE_RESIZEDLG_TEMP_FIX
	const UINT dpi = GetWindowDPI(hwnd);
	const UINT old = pm->dpi;
	if (cx) {
		*cx = cxClient - MulDiv(pm->cxClient, dpi, old);
	}
	if (cy) {
		*cy = cyClient - MulDiv(pm->cyClient, dpi, old);
	}
	// store in original DPI.
	pm->cxClient = MulDiv(cxClient, old, dpi);
	pm->cyClient = MulDiv(cyClient, old, dpi);
#else
	if (cx) {
		*cx = cxClient - pm->cxClient;
	}
	if (cy) {
		*cy = cyClient - pm->cyClient;
	}
	pm->cxClient = cxClient;
	pm->cyClient = cyClient;
#endif
}

void ResizeDlg_GetMinMaxInfo(HWND hwnd, LPARAM lParam) {
	LPCRESIZEDLG pm = (LPCRESIZEDLG)GetProp(hwnd, RESIZEDLG_PROP_KEY);
	LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;
#if NP2_ENABLE_RESIZEDLG_TEMP_FIX
	const UINT dpi = GetWindowDPI(hwnd);
	const UINT old = pm->dpi;

	lpmmi->ptMinTrackSize.x = MulDiv(pm->mmiPtMinX, dpi, old);
	lpmmi->ptMinTrackSize.y = MulDiv(pm->mmiPtMinY, dpi, old);

	// only one direction
	switch (pm->direction) {
	case ResizeDlgDirection_OnlyX:
		lpmmi->ptMaxTrackSize.y = MulDiv(pm->mmiPtMaxY, dpi, old);
		break;

	case ResizeDlgDirection_OnlyY:
		lpmmi->ptMaxTrackSize.x = MulDiv(pm->mmiPtMaxX, dpi, old);
		break;
	}
#else
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
#endif
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
void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, int wBmpId) {
#if NP2_ENABLE_HIDPI_IMAGE_RESOURCE
	if (hInstance) {
		wBmpId = GetBitmapResourceIdForCurrentDPI(wBmpId);
	}
#endif
	HWND hwndCtl = GetDlgItem(hwnd, nCtlId);
	HBITMAP hBmp = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(wBmpId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	hBmp = ResizeImageForCurrentDPI(hBmp);
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
// SetClipData()
//
void SetClipData(HWND hwnd, LPCWSTR pszData) {
	if (OpenClipboard(hwnd)) {
		EmptyClipboard();
		HANDLE hData = GlobalAlloc(GHND, sizeof(WCHAR) * (lstrlen(pszData) + 1));
		WCHAR *pData = (WCHAR *)GlobalLock(hData);
		lstrcpyn(pData, pszData, (int)(GlobalSize(hData) / sizeof(WCHAR)));
		GlobalUnlock(hData);
		SetClipboardData(CF_UNICODETEXT, hData);
		CloseClipboard();
	}
}

//=============================================================================
//
//  SetWindowTransparentMode()
//
void SetWindowTransparentMode(HWND hwnd, bool bTransparentMode, int iOpacityLevel) {
	// https://docs.microsoft.com/en-us/windows/win32/winmsg/using-windows#using-layered-windows
	DWORD exStyle = GetWindowExStyle(hwnd);
	exStyle = bTransparentMode ? (exStyle | WS_EX_LAYERED) : (exStyle & ~WS_EX_LAYERED);
	SetWindowExStyle(hwnd, exStyle);
	if (bTransparentMode) {
		const BYTE bAlpha = (BYTE)(iOpacityLevel * 255 / 100);
		SetLayeredWindowAttributes(hwnd, 0, bAlpha, LWA_ALPHA);
	}
	// Ask the window and its children to repaint
	RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}

void SetWindowLayoutRTL(HWND hwnd, bool bRTL) {
	// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-features#window-layout-and-mirroring
	DWORD exStyle = GetWindowExStyle(hwnd);
	exStyle = bRTL ? (exStyle | WS_EX_LAYOUTRTL) : (exStyle & ~WS_EX_LAYOUTRTL);
	SetWindowExStyle(hwnd, exStyle);
	// update layout in the client area
	InvalidateRect(hwnd, NULL, TRUE);
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
	while (true) {
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
	case LANG_FRENCH:
		folder = L"fr-FR";
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
	case LANG_KOREAN:
		folder = L"ko";
		break;
	case LANG_PORTUGUESE:
		folder = L"pt-BR";
		break;
	}

	if (folder == NULL) {
		return NULL;
	}

	WCHAR path[MAX_PATH];
	GetProgramRealPath(path, COUNTOF(path));
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
		// https://docs.microsoft.com/en-us/typography/font-list/segoe-ui
		font = L"Segoe UI";
		break;
	case LANG_CHINESE:
		// https://docs.microsoft.com/en-us/typography/font-list/microsoft-yahei
		// https://docs.microsoft.com/en-us/typography/font-list/microsoft-jhenghei
		font = IsChineseTraditionalSubLang(subLang) ? L"Microsoft JhengHei UI" : L"Microsoft YaHei UI";
		break;
	case LANG_JAPANESE:
		// https://docs.microsoft.com/en-us/typography/font-list/meiryo
		font = L"Meiryo UI";
		break;
	case LANG_KOREAN:
		// https://docs.microsoft.com/en-us/typography/font-list/malgun-gothic
		font = L"Malgun Gothic";
		break;
	}
	*wSize = 9;
	lstrcpy(lpFaceName, font);
}
#endif
#endif // NP2_ENABLE_APP_LOCALIZATION_DLL

bool PathGetRealPath(HANDLE hFile, LPCWSTR lpszSrc, LPWSTR lpszDest) {
	WCHAR path[8 + MAX_PATH] = L"";
	if (IsVistaAndAbove()) {
		const bool closing = hFile == NULL;
		if (closing) {
			hFile = CreateFile(lpszSrc, FILE_READ_ATTRIBUTES,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		}
		if (hFile != INVALID_HANDLE_VALUE) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			DWORD cch = GetFinalPathNameByHandleW(hFile, path, COUNTOF(path), FILE_NAME_OPENED);
#else
			typedef DWORD (WINAPI *GetFinalPathNameByHandleSig)(HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
			static GetFinalPathNameByHandleSig pfnGetFinalPathNameByHandle = NULL;
			if (pfnGetFinalPathNameByHandle == NULL) {
				pfnGetFinalPathNameByHandle = DLLFunctionEx(GetFinalPathNameByHandleSig, L"kernel32.dll", "GetFinalPathNameByHandleW");
			}
			DWORD cch = pfnGetFinalPathNameByHandle(hFile, path, COUNTOF(path), FILE_NAME_OPENED);
#endif
			// TODO: support long path
			if (closing) {
				CloseHandle(hFile);
			}
			if (cch != 0 && StrHasPrefix(path, L"\\\\?\\")) {
				cch -= CSTRLEN(L"\\\\?\\");
				WCHAR *p = path + CSTRLEN(L"\\\\?\\");
				if (StrHasPrefix(p, L"UNC\\")) {
					cch -= 2;
					p += 2;
					*p = L'\\'; // replace 'C' with backslash
				}
				if (cch > 0 && cch < MAX_PATH) {
					memcpy(lpszDest, p, (cch + 1)*sizeof(WCHAR));
					return true;
				}
			}
		}
	}

	DWORD cch = GetFullPathName(lpszSrc, COUNTOF(path), path, NULL);
	if (cch > 0 && cch < COUNTOF(path)) {
		WCHAR *p = path;
		if (StrHasPrefix(path, L"\\\\?\\")) {
			cch -= CSTRLEN(L"\\\\?\\");
			p += CSTRLEN(L"\\\\?\\");
			if (StrHasPrefix(p, L"UNC\\")) {
				cch -= 2;
				p += 2;
				*p = L'\\'; // replace 'C' with backslash
			}
		}
		if (cch > 0 && cch < MAX_PATH) {
			memcpy(lpszDest, p, (cch + 1)*sizeof(WCHAR));
			return true;
		}
	}
	return false;
}

//=============================================================================
//
//  PathRelativeToApp()
//
void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, DWORD dwAttrTo, bool bUnexpandEnv, bool bUnexpandMyDocs) {
	WCHAR wchPath[MAX_PATH];

	if (!PathIsRelative(lpszSrc)) {
		WCHAR wchAppPath[MAX_PATH];
		WCHAR wchWinDir[MAX_PATH];
		GetModuleFileName(NULL, wchAppPath, COUNTOF(wchAppPath));
		PathRemoveFileSpec(wchAppPath);

		if (bUnexpandMyDocs) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			LPWSTR wchUserFiles = NULL;
			if (S_OK != SHGetKnownFolderPath(KnownFolderId_Documents, KF_FLAG_DEFAULT, NULL, &wchUserFiles)) {
				return;
			}
#else
			WCHAR wchUserFiles[MAX_PATH];
			if (S_OK != SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, wchUserFiles)) {
				return;
			}
#endif
			if (!PathIsPrefix(wchUserFiles, wchAppPath) && PathIsPrefix(wchUserFiles, lpszSrc)
				&& PathRelativePathTo(wchWinDir, wchUserFiles, FILE_ATTRIBUTE_DIRECTORY, lpszSrc, dwAttrTo)) {
				PathCombine(wchPath, L"%CSIDL:MYDOCUMENTS%", wchWinDir);
				lpszSrc = wchPath;
			}
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			CoTaskMemFree(wchUserFiles);
#endif
		}
		if (lpszSrc != wchPath) {
			GetWindowsDirectory(wchWinDir, COUNTOF(wchWinDir));
			if (!PathCommonPrefix(wchAppPath, wchWinDir, NULL)
				&& PathRelativePathTo(wchPath, wchAppPath, FILE_ATTRIBUTE_DIRECTORY, lpszSrc, dwAttrTo)) {
				lpszSrc = wchPath;
			}
		}
	}

	if (bUnexpandEnv) {
		if (lpszSrc == lpszDest) {
			lstrcpyn(wchPath, lpszSrc, COUNTOF(wchPath));
			lpszSrc = wchPath;
		}
		if (PathUnExpandEnvStrings(lpszSrc, lpszDest, MAX_PATH)) {
			return;
		}
	}
	if (lpszSrc != lpszDest) {
		lstrcpy(lpszDest, lpszSrc);
	}
}

//=============================================================================
//
//  PathAbsoluteFromApp()
//
void PathAbsoluteFromApp(LPCWSTR lpszSrc, LPWSTR lpszDest, bool bExpandEnv) {
	WCHAR wchPath[MAX_PATH];

	if (StrHasPrefix(lpszSrc, L"%CSIDL:MYDOCUMENTS%")) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		LPWSTR pszPath = NULL;
		if (S_OK != SHGetKnownFolderPath(KnownFolderId_Documents, KF_FLAG_DEFAULT, NULL, &pszPath)) {
			return;
		}
		PathCombine(wchPath, pszPath, lpszSrc + CSTRLEN("%CSIDL:MYDOCUMENTS%"));
		CoTaskMemFree(pszPath);
#else
		if (S_OK != SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, wchPath)) {
			return;
		}
		PathAppend(wchPath, lpszSrc + CSTRLEN("%CSIDL:MYDOCUMENTS%"));
#endif
	} else {
		lstrcpyn(wchPath, lpszSrc, COUNTOF(wchPath));
	}

	if (bExpandEnv) {
		ExpandEnvironmentStringsEx(wchPath, COUNTOF(wchPath));
	}

	WCHAR wchResult[MAX_PATH];
	lpszSrc = wchPath;
	if (PathIsRelative(wchPath)) {
		GetModuleFileName(NULL, wchResult, COUNTOF(wchResult));
		PathRemoveFileSpec(wchResult);
		PathAppend(wchResult, wchPath);
		lpszSrc = wchResult;
	}
	if (!PathCanonicalize(lpszDest, lpszSrc)) {
		lstrcpy(lpszDest, lpszSrc);
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
bool PathGetLnkPath(LPCWSTR pszLnkFile, LPWSTR pszResPath) {
	if (StrIsEmpty(pszLnkFile)) {
		return false;
	}
	if (!StrCaseEqual(PathFindExtension(pszLnkFile), L".lnk")) {
		return false;
	}

	IShellLink *psl;
	HRESULT hr = S_FALSE;
	WCHAR tchPath[MAX_PATH];
	tchPath[0] = L'\0';

#if defined(__cplusplus)
	if (SUCCEEDED(CoCreateInstance(IID_IShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void **)(&ppf)))) {
			if (SUCCEEDED(ppf->Load(pszLnkFile, STGM_READ))) {
				hr = psl->GetPath(tchPath, COUNTOF(tchPath), nullptr, 0);
			}
			ppf->Release();
		}
		psl->Release();
	}
#else
	if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void **)(&ppf)))) {
			if (SUCCEEDED(ppf->lpVtbl->Load(ppf, pszLnkFile, STGM_READ))) {
				hr = psl->lpVtbl->GetPath(psl, tchPath, COUNTOF(tchPath), NULL, 0);
			}
			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}
#endif

	if (hr == S_OK && StrNotEmpty(tchPath)) {
		ExpandEnvironmentStringsEx(tchPath, COUNTOF(tchPath));
		if (!PathCanonicalize(pszResPath, tchPath)) {
			lstrcpy(pszResPath, tchPath);
		}
		return true;
	}

	return false;
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
bool PathCreateLnk(LPCWSTR pszLnkDir, LPCWSTR pszPath) {
	WCHAR tchLnkFileName[MAX_PATH];
	BOOL fMustCopy;

	// Try to construct a valid filename...
	if (!SHGetNewLinkInfo(pszPath, pszLnkDir, tchLnkFileName, &fMustCopy, SHGNLI_PREFIXNAME)) {
		return FALSE;
	}

	IShellLink *psl;
	bool bSucceeded = false;

#if defined(__cplusplus)
	if (SUCCEEDED(CoCreateInstance(IID_IShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void **)(&ppf)))) {
			psl->SetPath(pszPath);

			if (SUCCEEDED(ppf->Save(tchLnkFileName, TRUE))) {
				bSucceeded = true;
			}
			ppf->Release();
		}
		psl->Release();
	}
#else
	if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID *)(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void **)(&ppf)))) {
			psl->lpVtbl->SetPath(psl, pszPath);

			if (SUCCEEDED(ppf->lpVtbl->Save(ppf, tchLnkFileName, TRUE))) {
				bSucceeded = true;
			}
			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}
#endif

	return bSucceeded;
}

void OpenContainingFolder(HWND hwnd, LPCWSTR pszFile, bool bSelect) {
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
			bSelect = false;
		} else {
			path = pszFile;
		}
	}

	dwAttributes = GetFileAttributes(wchDirectory);
	if (dwAttributes == INVALID_FILE_ATTRIBUTES || !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		return;
	}

	PCIDLIST_ABSOLUTE pidl = ILCreateFromPath(wchDirectory);
	if (pidl) {
		HRESULT hr;
		PCIDLIST_ABSOLUTE pidlEntry = path ? ILCreateFromPath(path) : NULL;
		if (pidlEntry) {
			hr = SHOpenFolderAndSelectItems(pidl, 1, (PCUITEMID_CHILD_ARRAY)(&pidlEntry), 0);
			CoTaskMemFree((LPVOID)pidlEntry);
		} else if (!bSelect) {
#if 0
			// Use an invalid item to open the folder?
			hr = SHOpenFolderAndSelectItems(pidl, 1, (LPCITEMIDLIST *)(&pidl), 0);
#else
			SHELLEXECUTEINFO sei;
			memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

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
bool ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2) {
	bool bQuoted = false;

	lstrcpy(lpArg1, lpArgs);
	if (lpArg2) {
		*lpArg2 = L'\0';
	}

	TrimString(lpArg1);
	if (*lpArg1 == L'\0') {
		return false;
	}

	LPWSTR psz = lpArg1;
	WCHAR ch = *psz;
	if (ch == L'\"') {
		*psz++ = L' ';
		bQuoted = true;
	} else if (ch == L'-' || ch == L'/') {
		// fix -appid="string with space"
		++psz;
		while ((ch = *psz) != L'\0' && ch != L' ') {
			++psz;
			if (ch == L'=' && *psz == L'\"') {
				bQuoted = true;
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

	return true;
}

//=============================================================================
//
//  PrepareFilterStr()
//
void PrepareFilterStr(LPWSTR lpFilter) {
	LPWSTR psz = StrEnd(lpFilter);
	while (psz != lpFilter) {
		--psz;
		if (*psz == L'|') {
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
bool PathFixBackslashes(LPWSTR lpsz) {
	WCHAR *c = lpsz;
	bool bFixed = false;
	while ((c = StrChr(c, L'/')) != NULL) {
		if (c != lpsz && c[-1] == L':' && c[1] == L'/') {
			c += 2;
		} else {
			*c++ = L'\\';
			bFixed = true;
		}
	}
	return bFixed;
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
//  SearchPathEx()
//
//  This Expansion also searches the L"Favorites" folder
//
extern WCHAR tchFavoritesDir[MAX_PATH];
extern WCHAR szCurDir[MAX_PATH + 40];

bool SearchPathEx(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer) {
	if (StrEqualExW(lpFileName, L"..") || StrEqualExW(lpFileName, L".")) {
		if (StrEqualExW(lpFileName, L"..") && PathIsRoot(szCurDir)) {
			StrCpyExW(lpBuffer, L"*.*");
			return true;
		}
	}

	DWORD cch = SearchPath(szCurDir, lpFileName, NULL, nBufferLength, lpBuffer, NULL);
	if (cch == 0) {
		// Search Favorites if no result
		cch = SearchPath(tchFavoritesDir, lpFileName, NULL, nBufferLength, lpBuffer, NULL);
	}

	return cch != 0 && cch < MAX_PATH;
}

//=============================================================================
//
//  FormatNumber()
//
void FormatNumber(LPWSTR lpNumberStr, UINT value) {
	_ltow(value, lpNumberStr, 10);
	if (value < 1000) {
		return;
	}

	// https://docs.microsoft.com/en-us/windows/desktop/Intl/locale-sthousand
	// https://docs.microsoft.com/en-us/windows/desktop/Intl/locale-sgrouping
	WCHAR szSep[4];
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	const WCHAR sep = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_STHOUSAND, szSep, COUNTOF(szSep))? szSep[0] : L',';
#else
	const WCHAR sep = GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szSep, COUNTOF(szSep))? szSep[0] : L',';
#endif

	WCHAR *c = lpNumberStr + lstrlen(lpNumberStr);
	WCHAR *end = c;
	lpNumberStr += 3;
	do {
		c -= 3;
		memmove(c + 1, c, sizeof(WCHAR) * (end - c + 1));
		*c = sep;
		++end;
	} while (c > lpNumberStr);
}

//=============================================================================
//
//  GetDefaultFavoritesDir()
//
void GetDefaultFavoritesDir(LPWSTR lpFavDir, int cchFavDir) {
	PIDLIST_ABSOLUTE pidl;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	if (S_OK == SHGetKnownFolderIDList(KnownFolderId_Documents, KF_FLAG_DEFAULT, NULL, &pidl))
#else
	if (S_OK == SHGetFolderLocation(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_DEFAULT, &pidl))
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
	PIDLIST_ABSOLUTE pidl;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	if (S_OK == SHGetKnownFolderIDList(KnownFolderId_Desktop, KF_FLAG_DEFAULT, NULL, &pidl))
#else
	if (S_OK == SHGetFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_DEFAULT, &pidl))
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

bool ExecDDECommand(LPCWSTR lpszCmdLine, LPCWSTR lpszDDEMsg, LPCWSTR lpszDDEApp, LPCWSTR lpszDDETopic) {
	if (StrIsEmpty(lpszCmdLine) || StrIsEmpty(lpszDDEMsg) || StrIsEmpty(lpszDDEApp) || StrIsEmpty(lpszDDETopic)) {
		return false;
	}

	bool bSuccess = true;
	WCHAR lpszDDEMsgBuf[256];

	lstrcpyn(lpszDDEMsgBuf, lpszDDEMsg, COUNTOF(lpszDDEMsgBuf));
	WCHAR *pSubst = StrStr(lpszDDEMsgBuf, L"%1");
	if (pSubst != NULL) {
		pSubst[1] = L's';
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
				bSuccess = false;
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
void History_Init(PHISTORY ph) {
	memset(ph, 0, sizeof(HISTORY));
	ph->iCurItem = -1;
}

void History_Uninit(PHISTORY ph) {
	for (int i = 0; i < HISTORY_ITEMS; i++) {
		if (ph->psz[i]) {
			LocalFree(ph->psz[i]);
		}
		ph->psz[i] = NULL;
	}
}

bool History_Add(PHISTORY ph, LPCWSTR pszNew) {
	// Item to be added is equal to current item
	if (ph->iCurItem >= 0 && ph->iCurItem < HISTORY_ITEMS) {
		if (ph->psz[ph->iCurItem] != NULL && PathEqual(pszNew, ph->psz[ph->iCurItem])) {
			return false;
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

		memmove(ph->psz, ph->psz + 1, (HISTORY_ITEMS - 1) * sizeof(WCHAR *));
	}

	ph->psz[ph->iCurItem] = StrDup(pszNew);

	return true;
}

bool History_Forward(PHISTORY ph, LPWSTR pszItem, int cItem) {
	if (ph->iCurItem < (HISTORY_ITEMS - 1)) {
		if (ph->psz[ph->iCurItem + 1]) {
			ph->iCurItem++;
			lstrcpyn(pszItem, ph->psz[ph->iCurItem], cItem);
			return true;
		}
	}

	return false;
}

bool History_Back(PHISTORY ph, LPWSTR pszItem, int cItem) {
	if (ph->iCurItem > 0) {
		if (ph->psz[ph->iCurItem - 1]) {
			ph->iCurItem--;
			lstrcpyn(pszItem, ph->psz[ph->iCurItem], cItem);
			return true;
		}
	}

	return false;
}

bool History_CanForward(LCPHISTORY ph) {
	if (ph->iCurItem < (HISTORY_ITEMS - 1)) {
		if (ph->psz[ph->iCurItem + 1]) {
			return true;
		}
	}

	return false;
}

bool History_CanBack(LCPHISTORY ph) {
	if (ph->iCurItem > 0) {
		if (ph->psz[ph->iCurItem - 1]) {
			return true;
		}
	}

	return false;
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

void MRU_Destroy(LPMRULIST pmru) {
	for (int i = 0; i < pmru->iSize; i++) {
		if (pmru->pszItems[i]) {
			LocalFree(pmru->pszItems[i]);
		}
	}

	memset(pmru, 0, sizeof(MRULIST));
	NP2HeapFree(pmru);
}

static inline bool MRU_Equal(int flags, LPCWSTR psz1, LPCWSTR psz2) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return CompareStringOrdinal(psz1, -1, psz2, -1, flags & MRUFlags_FilePath) == CSTR_EQUAL;
#else
	return (flags & MRUFlags_FilePath) ? PathEqual(psz1, psz2) : StrEqual(psz1, psz2);
#endif
}

bool MRU_Add(LPMRULIST pmru, LPCWSTR pszNew) {
	const int flags = pmru->iFlags;
	int i;
	for (i = 0; i < pmru->iSize; i++) {
		WCHAR * const item = pmru->pszItems[i];
		if (item == NULL) {
			break;
		}
		if (MRU_Equal(flags, item, pszNew)) {
			LocalFree(item);
			break;
		}
	}
	i = min_i(i, pmru->iSize - 1);
	for (; i > 0; i--) {
		pmru->pszItems[i] = pmru->pszItems[i - 1];
	}
	pmru->pszItems[0] = StrDup(pszNew);
	return true;
}

bool MRU_Delete(LPMRULIST pmru, int iIndex) {
	if (iIndex < 0 || iIndex >= pmru->iSize) {
		return false;
	}
	if (pmru->pszItems[iIndex]) {
		LocalFree(pmru->pszItems[iIndex]);
	}
	for (int i = iIndex; i < pmru->iSize - 1; i++) {
		pmru->pszItems[i] = pmru->pszItems[i + 1];
		pmru->pszItems[i + 1] = NULL;
	}
	return true;
}

void MRU_Empty(LPMRULIST pmru) {
	for (int i = 0; i < pmru->iSize; i++) {
		if (pmru->pszItems[i]) {
			LocalFree(pmru->pszItems[i]);
			pmru->pszItems[i] = NULL;
		}
	}
}

int MRU_Enum(LPCMRULIST pmru, int iIndex, LPWSTR pszItem, int cchItem) {
	if (pszItem == NULL || cchItem == 0) {
		int i = 0;
		while (i < pmru->iSize && pmru->pszItems[i]) {
			i++;
		}
		return i;
	}

	if (iIndex < 0 || iIndex >= pmru->iSize || pmru->pszItems[iIndex] == NULL) {
		return -1;
	}
	lstrcpyn(pszItem, pmru->pszItems[iIndex], cchItem);
	return TRUE;
}

bool MRU_Load(LPMRULIST pmru) {
	if (StrIsEmpty(szIniFile)) {
		return true;
	}

	IniSection section;
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU);
	const int cchIniSection = (int)(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));
	IniSection * const pIniSection = &section;

	MRU_Empty(pmru);
	IniSectionInit(pIniSection, MRU_MAXITEMS);

	LoadIniSection(pmru->szRegKey, pIniSectionBuf, cchIniSection);
	IniSectionParseArray(pIniSection, pIniSectionBuf);
	const UINT count = pIniSection->count;
	const UINT size = pmru->iSize;

	for (UINT i = 0, n = 0; i < count && n < size; i++) {
		const IniKeyValueNode *node = &pIniSection->nodeList[i];
		LPCWSTR tchItem = node->value;
		if (StrNotEmpty(tchItem)) {
			pmru->pszItems[n++] = StrDup(tchItem);
		}
	}

	IniSectionFree(pIniSection);
	NP2HeapFree(pIniSectionBuf);
	return true;
}

bool MRU_Save(LPCMRULIST pmru) {
	if (StrIsEmpty(szIniFile)) {
		return true;
	}
	if (MRU_GetCount(pmru) == 0) {
		IniClearSection(pmru->szRegKey);
		return true;
	}

	WCHAR tchName[16];
	WCHAR *pIniSectionBuf = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU);
	IniSectionOnSave section = { pIniSectionBuf };
	IniSectionOnSave * const pIniSection = &section;

	for (int i = 0; i < pmru->iSize; i++) {
		if (StrNotEmpty(pmru->pszItems[i])) {
			wsprintf(tchName, L"%02i", i + 1);
			IniSectionSetString(pIniSection, tchName, pmru->pszItems[i]);
		}
	}

	SaveIniSection(pmru->szRegKey, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
	return true;
}

void MRU_LoadToCombobox(HWND hwnd, LPCWSTR pszKey) {
	WCHAR tch[MAX_PATH];
	LPMRULIST pmru = MRU_Create(pszKey, MRUFlags_FilePath, MRU_MAX_COPY_MOVE_HISTORY);
	MRU_Load(pmru);
	for (int i = 0; i < MRU_GetCount(pmru); i++) {
		MRU_Enum(pmru, i, tch, COUNTOF(tch));
		ComboBox_AddString(hwnd, tch);
	}
	MRU_Destroy(pmru);
}

void MRU_AddOneItem(LPCWSTR pszKey, LPCWSTR pszNewItem) {
	if (StrNotEmpty(pszNewItem)) {
		LPMRULIST pmru = MRU_Create(pszKey, MRUFlags_FilePath, MRU_MAX_COPY_MOVE_HISTORY);
		MRU_Load(pmru);
		MRU_Add(pmru, pszNewItem);
		MRU_Save(pmru);
		MRU_Destroy(pmru);
	}
}

void MRU_ClearCombobox(HWND hwnd, LPCWSTR pszKey) {
	LPMRULIST pmru = MRU_Create(pszKey, MRUFlags_FilePath, MRU_MAX_COPY_MOVE_HISTORY);
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
bool GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize) {
#if NP2_ENABLE_APP_LOCALIZATION_DLL && NP2_ENABLE_TEST_LOCALIZATION_LAYOUT
	extern LANGID uiLanguage;
	GetLocaleDefaultUIFont(uiLanguage, lpFaceName, wSize);
	return true;
#else

	bool bSucceed = false;
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
				bSucceed = true;
			}
			CloseThemeData(hTheme);
		}
	}

	if (!bSucceed) {
		NONCLIENTMETRICS ncm;
		memset(&ncm, 0, sizeof(ncm));
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
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
			bSucceed = true;
		}
	}

	if (bSucceed && !IsVistaAndAbove()) {
		// Windows 2000, XP, 2003
		lstrcpy(lpFaceName, L"Tahoma");
	}
	return bSucceed;
#endif
}

static inline bool DialogTemplate_IsDialogEx(const DLGTEMPLATE *pTemplate) {
	return ((DLGTEMPLATEEX *)pTemplate)->signature == 0xFFFF;
}

static inline BOOL DialogTemplate_HasFont(const DLGTEMPLATE *pTemplate) {
	return (DS_SETFONT & (DialogTemplate_IsDialogEx(pTemplate) ? ((DLGTEMPLATEEX *)pTemplate)->style : pTemplate->style));
}

static inline int DialogTemplate_FontAttrSize(bool bDialogEx) {
	return (int)sizeof(WORD) * (bDialogEx ? 3 : 1);
}

static inline BYTE *DialogTemplate_GetFontSizeField(const DLGTEMPLATE *pTemplate) {
	const bool bDialogEx = DialogTemplate_IsDialogEx(pTemplate);
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

	memcpy((BYTE *)pTemplate, pRsrcMem, (size_t)dwTemplateSize);
	FreeResource(hRsrcMem);

	WCHAR wchFaceName[LF_FACESIZE];
	WORD wFontSize;
	if (!GetThemedDialogFont(wchFaceName, &wFontSize)) {
		return pTemplate;
	}

	const bool bDialogEx = DialogTemplate_IsDialogEx(pTemplate);
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
		memmove(pNewControls, pOldControls, (size_t)(dwTemplateSize - (pOldControls - (BYTE *)pTemplate)));
	}

	*(WORD *)pb = wFontSize;
	memmove(pb + cbFontAttr, pbNew, (size_t)(cbNew - cbFontAttr));

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

//=============================================================================
//
// File Dialog Hook for GetOpenFileName/GetSaveFileName
// https://docs.microsoft.com/en-us/windows/win32/dlgbox/open-and-save-as-dialog-boxes
//
static LRESULT CALLBACK OpenSaveFileDlgSubProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	UNREFERENCED_PARAMETER(dwRefData);

	switch (umsg) {
	case WM_COMMAND:
		switch (wParam) {
		case IDOK: {
			WCHAR szPath[MAX_PATH];
			HWND hCmbPath = GetDlgItem(hwnd, cmb13); // cmb13: dlgs.h
			GetWindowText(hCmbPath, szPath, MAX_PATH);
			if (PathFixBackslashes(szPath)) {
				SetWindowText(hCmbPath, szPath);
			}
		} break;
	} break;

	case WM_NCDESTROY:
		RemoveWindowSubclass(hwnd, OpenSaveFileDlgSubProc, uIdSubclass);
		break;
	}

	return DefSubclassProc(hwnd, umsg, wParam, lParam);
}

UINT_PTR CALLBACK OpenSaveFileDlgHookProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);

	switch (umsg) {
	case WM_NOTIFY: {
		LPOFNOTIFY pOFNOTIFY = (LPOFNOTIFY)lParam;
		switch (pOFNOTIFY->hdr.code) {
		case CDN_INITDONE:
			// OFN_OVERWRITEPROMPT is tested before OFNHookProc making "D:\d" like folder path trigger a prompt.
			// Hook the default (parent) dialog box procedure.
			SetWindowSubclass(GetParent(hwnd), OpenSaveFileDlgSubProc, 0, 0);
			break;
		}
	} break;
	}
	return FALSE;
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
static void GetTrayWndRect(LPRECT lpTrayRect) {
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
/*static */bool GetDoAnimateMinimize(void) {
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
