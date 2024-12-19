/******************************************************************************
*
*
* matepath - The universal Explorer-like Plugin
*
* Helpers.cpp
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

struct IUnknown;
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
#include <cstdio>
#include "config.h"
#include "Helpers.h"
#include "../../scintilla/include/VectorISA.h"
#include "../../scintilla/include/GraphicUtils.h"
#include "Dlapi.h"
#include "resource.h"

void IniClearSectionEx(LPCWSTR lpSection, LPCWSTR lpszIniFile, bool bDelete) noexcept {
	if (StrIsEmpty(lpszIniFile)) {
		return;
	}

	WritePrivateProfileSection(lpSection, (bDelete ? nullptr : L""), lpszIniFile);
}

void IniClearAllSectionEx(LPCWSTR lpszPrefix, LPCWSTR lpszIniFile, bool bDelete) noexcept {
	if (StrIsEmpty(lpszIniFile)) {
		return;
	}

	WCHAR sections[1024] = L"";
	GetPrivateProfileSectionNames(sections, COUNTOF(sections), lpszIniFile);

	LPCWSTR p = sections;
	LPCWSTR value = bDelete ? nullptr : L"";
	const int len = lstrlen(lpszPrefix);

	while (*p) {
		if (_wcsnicmp(p, lpszPrefix, len) == 0) {
			WritePrivateProfileSection(p, value, lpszIniFile);
		}
		p = StrEnd(p) + 1;
	}
}

//=============================================================================
//
//  Manipulation of (cached) ini file sections
//
void IniSectionParser::Init(UINT capacity_) noexcept {
	count = 0;
	capacity = capacity_;
	head = nullptr;
#if IniSectionParserUseSentinelNode
	nodeList = static_cast<IniKeyValueNode *>(NP2HeapAlloc((capacity_ + 1) * sizeof(IniKeyValueNode)));
	sentinel = &nodeList[capacity_];
#else
	nodeList = static_cast<IniKeyValueNode *>(NP2HeapAlloc(capacity_ * sizeof(IniKeyValueNode)));
#endif
}

bool IniSectionParser::ParseArray(LPWSTR lpCachedIniSection) noexcept {
	Clear();
	if (StrIsEmpty(lpCachedIniSection)) {
		return false;
	}

	LPWSTR p = lpCachedIniSection;
	UINT index = 0;

	do {
		LPWSTR v = StrChr(p, L'=');
		if (v != nullptr) {
			*v++ = L'\0';
			IniKeyValueNode &node = nodeList[index];
			node.key = p;
			node.value = v;
			++index;
			p = v;
		}
		p = StrEnd(p) + 1;
	} while (*p && index < capacity);

	count = index;
	return index != 0;
}

bool IniSectionParser::Parse(LPWSTR lpCachedIniSection) noexcept {
	Clear();
	if (StrIsEmpty(lpCachedIniSection)) {
		return false;
	}

	LPWSTR p = lpCachedIniSection;
	UINT index = 0;

	do {
		LPWSTR v = StrChr(p, L'=');
		if (v != nullptr) {
			*v++ = L'\0';
			const UINT keyLen = static_cast<UINT>(v - p - 1);
			IniKeyValueNode &node = nodeList[index];
			node.hash = keyLen | ((*reinterpret_cast<const UINT *>(p)) << 8);
			node.key = p;
			node.value = v;
			++index;
			p = v;
		}
		p = StrEnd(p) + 1;
	} while (*p && index < capacity);

	if (index == 0) {
		return false;
	}

	count = index;
	head = &nodeList[0];
	--index;
#if IniSectionParserUseSentinelNode
	nodeList[index].next = sentinel;
#else
	nodeList[index].next = nullptr;
#endif
	while (index != 0) {
		nodeList[index - 1].next = &nodeList[index];
		--index;
	}
	return true;
}

LPCWSTR IniSectionParser::UnsafeGetValue(LPCWSTR key, int keyLen) noexcept {
	if (keyLen == 0) {
		keyLen = lstrlen(key);
	}

	const UINT hash = keyLen | ((*reinterpret_cast<const UINT *>(key)) << 8);
	IniKeyValueNode *node = head;
	IniKeyValueNode *prev = nullptr;
#if IniSectionParserUseSentinelNode
	sentinel->hash = hash;
	while (true) {
		if (node->hash == hash) {
			if (node == sentinel) {
				return nullptr;
			}
			if (StrEqual(node->key, key)) {
				// remove the node
				--count;
				if (prev == nullptr) {
					head = node->next;
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
			--count;
			if (prev == nullptr) {
				head = node->next;
			} else {
				prev->next = node->next;
			}
			return node->value;
		}
		prev = node;
		node = node->next;
	} while (node);
	return nullptr;
#endif
}

void IniSectionParser::GetStringImpl(LPCWSTR key, int keyLen, LPCWSTR lpDefault, LPWSTR lpReturnedString, int cchReturnedString) noexcept {
	LPCWSTR value = GetValueImpl(key, keyLen);
	// allow empty string value
	lstrcpyn(lpReturnedString, ((value == nullptr) ? lpDefault : value), cchReturnedString);
}

int IniSectionParser::GetIntImpl(LPCWSTR key, int keyLen, int iDefault) noexcept {
	LPCWSTR value = GetValueImpl(key, keyLen);
	if (value && CRTStrToInt(value, &keyLen)) {
		return keyLen;
	}
	return iDefault;
}

bool IniSectionParser::GetBoolImpl(LPCWSTR key, int keyLen, bool bDefault) noexcept {
	LPCWSTR value = GetValueImpl(key, keyLen);
	if (value) {
		const UINT t = *value - L'0';
		if (t <= TRUE) {
			return t & true;
		}
	}
	return bDefault;
}

void IniSectionBuilder::SetString(LPCWSTR key, LPCWSTR value) noexcept {
	LPWSTR p = next;
	lstrcpy(p, key);
	lstrcat(p, L"=");
	lstrcat(p, value);
	p = StrEnd(p) + 1;
	*p = L'\0';
	next = p;
}

LPWSTR Registry_GetString(HKEY hKey, LPCWSTR valueName) noexcept {
	LPWSTR lpszText = nullptr;
	DWORD type = REG_NONE;
	DWORD size = 0;

	LSTATUS status = RegQueryValueEx(hKey, valueName, nullptr, &type, nullptr, &size);
	if (status == ERROR_SUCCESS && type == REG_SZ && size != 0) {
		size = (size + 1)*sizeof(WCHAR);
		lpszText = static_cast<LPWSTR>(NP2HeapAlloc(size));
		status = RegQueryValueEx(hKey, valueName, nullptr, &type, reinterpret_cast<LPBYTE>(lpszText), &size);
		if (status != ERROR_SUCCESS || type != REG_SZ || size == 0) {
			NP2HeapFree(lpszText);
			lpszText = nullptr;
		}
	}
	return lpszText;
}

LSTATUS Registry_SetString(HKEY hKey, LPCWSTR valueName, LPCWSTR lpszText) noexcept {
	DWORD len = lstrlen(lpszText);
	len = len ? ((len + 1)*sizeof(WCHAR)) : 0;
	const LSTATUS status = RegSetValueEx(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE *>(lpszText), len);
	return status;
}

#if _WIN32_WINNT < _WIN32_WINNT_VISTA
LSTATUS Registry_DeleteTree(HKEY hKey, LPCWSTR lpSubKey) noexcept {
	using RegDeleteTreeSig = LSTATUS (WINAPI *)(HKEY hKey, LPCWSTR lpSubKey);
	RegDeleteTreeSig pfnRegDeleteTree = DLLFunctionEx<RegDeleteTreeSig>(L"advapi32.dll", "RegDeleteTreeW");

	LSTATUS status;
	if (pfnRegDeleteTree != nullptr) {
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

int ParseCommaList(LPCWSTR str, int result[], int count) noexcept {
	if (StrIsEmpty(str)) {
		return 0;
	}

	int index = 0;
	while (index < count) {
		LPWSTR end;
		result[index] = static_cast<int>(wcstol(str, &end, 10));
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

bool FindUserResourcePath(LPCWSTR path, LPWSTR outPath) noexcept {
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

HBITMAP LoadBitmapFile(LPCWSTR path) noexcept {
	WCHAR szTmp[MAX_PATH];
	if (!FindUserResourcePath(path, szTmp)) {
		return nullptr;
	}

	HBITMAP hbmp = static_cast<HBITMAP>(LoadImage(nullptr, szTmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE));
	return hbmp;
}

HBITMAP ResizeImageForDPI(HBITMAP hbmp, UINT dpi) noexcept {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		// assume 16x16 at 100% scaling
		const int height = (dpi*16) / USER_DEFAULT_SCREEN_DPI;
		if (height == bmp.bmHeight && bmp.bmBitsPixel == 32) {
			return hbmp;
		}
		// keep aspect ratio
		const int width = MulDiv(height, bmp.bmWidth, bmp.bmHeight);
		HBITMAP hCopy = static_cast<HBITMAP>(CopyImage(hbmp, IMAGE_BITMAP, width, height, LR_COPYRETURNORG | LR_COPYDELETEORG));
		if (hCopy != nullptr) {
#if 0
			BITMAP bmp2;
			if (GetObject(hCopy, sizeof(BITMAP), &bmp2)) {
				printf("%s %u: (%d x %d, %d) => (%d x %d, %d)\n", __func__, dpi,
				bmp.bmWidth, bmp.bmHeight, bmp.bmBitsPixel, bmp2.bmWidth, bmp2.bmHeight, bmp2.bmBitsPixel);
			}
#endif
			hbmp = hCopy;
		}
	}

	return hbmp;
}


void BackgroundWorker::Init(HWND owner) noexcept {
	hwnd = owner;
	eventCancel = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	workerThread = nullptr;
}

void BackgroundWorker::Stop() noexcept {
	SetEvent(eventCancel);
	HANDLE worker = InterlockedExchangePointer(&workerThread, nullptr);
	if (worker) {
		while (WaitForSingleObject(worker, 0) != WAIT_OBJECT_0) {
			MSG msg;
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		CloseHandle(worker);
	}
}

void BackgroundWorker::Cancel() noexcept {
	Stop();
	ResetEvent(eventCancel);
}

void BackgroundWorker::Destroy() noexcept {
	Stop();
	CloseHandle(eventCancel);
}

//=============================================================================
//
// PrivateSetCurrentProcessExplicitAppUserModelID()
//
HRESULT PrivateSetCurrentProcessExplicitAppUserModelID(LPCWSTR AppID) noexcept {
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
	using SetCurrentProcessExplicitAppUserModelIDSig = HRESULT (WINAPI *)(LPCWSTR AppID);
	SetCurrentProcessExplicitAppUserModelIDSig pfnSetCurrentProcessExplicitAppUserModelID =
		DLLFunctionEx<SetCurrentProcessExplicitAppUserModelIDSig>(L"shell32.dll", "SetCurrentProcessExplicitAppUserModelID");
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
bool IsElevated() noexcept {
	bool bIsElevated = false;
	HANDLE hToken = nullptr;

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
bool ExeNameFromWnd(HWND hwnd, LPWSTR szExeName, DWORD cchExeName) noexcept {
	DWORD dwProcessId;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	QueryFullProcessImageName(hProcess, 0, szExeName, &cchExeName);
#else
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
	GetModuleFileNameEx(hProcess, nullptr, szExeName, cchExeName);
#endif
	CloseHandle(hProcess);
	return true;
}

////=============================================================================
////
////  Is32bitExe()
////
/*
bool Is32bitExe(LPCWSTR lpszExeName) noexcept {
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
bool BitmapAlphaBlend(HBITMAP hbmp, COLORREF crDest, BYTE alpha) noexcept {
	BITMAP bmp;
	if (GetObject(hbmp, sizeof(BITMAP), &bmp)) {
		if (bmp.bmBitsPixel == 32) {
#if NP2_USE_AVX2
#if 1
			#define BitmapAlphaBlend_Tag	"avx2 4x1"
			const ULONG count = (bmp.bmHeight * bmp.bmWidth) / 4;
			__m128i *prgba = static_cast<__m128i *>(bmp.bmBits);

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
			uint64_t *prgba = static_cast<uint64_t *>(bmp.bmBits);

			const __m128i i16x8Alpha = _mm_broadcastw_epi16(_mm_cvtsi32_si128(alpha));
			const __m128i i16x8Back = _mm_mullo_epi16(rgba_to_bgra_epi16x8_sse4_si32(crDest), mm_xor_alpha_epi16(i16x8Alpha));
			for (ULONG x = 0; x < count; x++, prgba++) {
				const __m128i origin = unpack_color_epi16_sse4_ptr64(prgba);
				__m128i i16x8Fore = _mm_mullo_epi16(origin, i16x8Alpha);
				i16x8Fore = _mm_add_epi16(i16x8Fore, i16x8Back);
				i16x8Fore = mm_div_epu16_by_255(i16x8Fore);
				i16x8Fore = _mm_blend_epi16(origin, i16x8Fore, 0x77);
				i16x8Fore = pack_color_epi16_sse2_si128(i16x8Fore);
				_mm_storel_epi64(reinterpret_cast<__m128i *>(prgba), i16x8Fore);
			}
#endif // NP2_USE_AVX2
#elif NP2_USE_SSE2
			#define BitmapAlphaBlend_Tag	"sse2 1x4"
			const ULONG count = (bmp.bmHeight * bmp.bmWidth) / 4;
			__m128i *prgba = static_cast<__m128i *>(bmp.bmBits);

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
			RGBQUAD *prgba = static_cast<RGBQUAD *>(bmp.bmBits);

			const WORD red = GetRValue(crDest) * (255 ^ alpha);
			const WORD green = GetGValue(crDest) * (255 ^ alpha);
			const WORD blue = GetBValue(crDest) * (255 ^ alpha);
			for (ULONG x = 0; x < count; x++) {
				prgba[x].rgbRed = ((prgba[x].rgbRed * alpha) + red) >> 8;
				prgba[x].rgbGreen = ((prgba[x].rgbGreen * alpha) + green) >> 8;
				prgba[x].rgbBlue = ((prgba[x].rgbBlue * alpha) + blue) >> 8;
			}
#endif
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
void CenterDlgInParentEx(HWND hDlg, HWND hParent) noexcept {
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

	SetWindowPos(hDlg, nullptr, clamp(x, xMin, xMax), clamp(y, yMin, yMax), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

// Why doesn’t the "Automatically move pointer to the default button in a dialog box"
// work for nonstandard dialog boxes, and how do I add it to my own nonstandard dialog boxes?
// https://blogs.msdn.microsoft.com/oldnewthing/20130826-00/?p=3413/
void SnapToDefaultButton(HWND hwndBox) noexcept {
	BOOL fSnapToDefButton = FALSE;
	if (SystemParametersInfo(SPI_GETSNAPTODEFBUTTON, 0, &fSnapToDefButton, 0) && fSnapToDefButton) {
		// get child window at the top of the Z order.
		// for all our MessageBoxs it's the OK or YES button or nullptr.
		HWND btn = GetWindow(hwndBox, GW_CHILD);
		if (btn != nullptr) {
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
void GetDlgPos(HWND hDlg, LPINT xDlg, LPINT yDlg) noexcept {
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
void SetDlgPos(HWND hDlg, int xDlg, int yDlg) noexcept {
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

	SetWindowPos(hDlg, nullptr, clamp(x, xMin, xMax), clamp(y, yMin, yMax), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
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

struct RESIZEDLG {
	int direction;
	UINT dpi;
	int cxClient;
	int cyClient;
	int mmiPtMinX;
	int mmiPtMinY;
	int mmiPtMaxX;	// only Y direction
	int mmiPtMaxY;	// only X direction
	int attrs[MAX_RESIZEDLG_ATTR_COUNT];
};

void ResizeDlg_InitEx(HWND hwnd, int cxFrame, int cyFrame, int nIdGrip, int iDirection) noexcept {
	const UINT dpi = GetWindowDPI(hwnd);
	RESIZEDLG * const pm = static_cast<RESIZEDLG *>(NP2HeapAlloc(sizeof(RESIZEDLG)));
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

	cxFrame = max(cxFrame, pm->mmiPtMinX);
	cyFrame = max(cyFrame, pm->mmiPtMinY);

	SetProp(hwnd, RESIZEDLG_PROP_KEY, pm);

	SetWindowPos(hwnd, nullptr, rc.left, rc.top, cxFrame, cyFrame, SWP_NOZORDER);

	SetWindowStyle(hwnd, style);
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

	WCHAR wch[64];
	GetMenuString(GetSystemMenu(GetParent(hwnd), FALSE), SC_SIZE, wch, COUNTOF(wch), MF_BYCOMMAND);
	InsertMenu(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_STRING | MF_ENABLED, SC_SIZE, wch);
	InsertMenu(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, nullptr);

	HWND hwndCtl = GetDlgItem(hwnd, nIdGrip);
	SetWindowStyle(hwndCtl, GetWindowStyle(hwndCtl) | SBS_SIZEGRIP | WS_CLIPSIBLINGS);
	const int cGrip = SystemMetricsForDpi(SM_CXHTHUMB, dpi);
	SetWindowPos(hwndCtl, nullptr, pm->cxClient - cGrip, pm->cyClient - cGrip, cGrip, cGrip, SWP_NOZORDER);
}

void ResizeDlg_Destroy(HWND hwnd, int *cxFrame, int *cyFrame) noexcept {
	RESIZEDLG * const pm = static_cast<RESIZEDLG *>(GetProp(hwnd, RESIZEDLG_PROP_KEY));

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

void ResizeDlg_Size(HWND hwnd, LPARAM lParam, int *cx, int *cy) noexcept {
	RESIZEDLG * const pm = static_cast<RESIZEDLG *>(GetProp(hwnd, RESIZEDLG_PROP_KEY));
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

void ResizeDlg_GetMinMaxInfo(HWND hwnd, LPARAM lParam) noexcept {
	const RESIZEDLG * const pm = static_cast<RESIZEDLG *>(GetProp(hwnd, RESIZEDLG_PROP_KEY));
	LPMINMAXINFO lpmmi = AsPointer<LPMINMAXINFO>(lParam);
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

HDWP DeferCtlPos(HDWP hdwp, HWND hwndDlg, int nCtlId, int dx, int dy, UINT uFlags) noexcept {
	HWND hwndCtl = GetDlgItem(hwndDlg, nCtlId);
	RECT rc;
	GetWindowRect(hwndCtl, &rc);
	MapWindowPoints(nullptr, hwndDlg, reinterpret_cast<LPPOINT>(&rc), 2);
	if (uFlags & SWP_NOSIZE) {
		return DeferWindowPos(hdwp, hwndCtl, nullptr, rc.left + dx, rc.top + dy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	return DeferWindowPos(hdwp, hwndCtl, nullptr, 0, 0, rc.right - rc.left + dx, rc.bottom - rc.top + dy, SWP_NOZORDER | SWP_NOMOVE);
}

void ResizeDlgCtl(HWND hwndDlg, int nCtlId, int dx, int dy) noexcept {
	HWND hwndCtl = GetDlgItem(hwndDlg, nCtlId);
	RECT rc;
	GetWindowRect(hwndCtl, &rc);
	MapWindowPoints(nullptr, hwndDlg, reinterpret_cast<LPPOINT>(&rc), 2);
	SetWindowPos(hwndCtl, nullptr, 0, 0, rc.right - rc.left + dx, rc.bottom - rc.top + dy, SWP_NOZORDER | SWP_NOMOVE);
	InvalidateRect(hwndCtl, nullptr, TRUE);
}

//=============================================================================
//
//  MakeBitmapButton()
//
void MakeBitmapButton(HWND hwnd, int nCtlId, HINSTANCE hInstance, int wBmpId) noexcept {
#if NP2_ENABLE_HIDPI_IMAGE_RESOURCE
	if (hInstance) {
		wBmpId = GetBitmapResourceIdForCurrentDPI(wBmpId);
	}
#endif
	HWND hwndCtl = GetDlgItem(hwnd, nCtlId);
	HBITMAP hBmp = static_cast<HBITMAP>(LoadImage(hInstance, MAKEINTRESOURCE(wBmpId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
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
void DeleteBitmapButton(HWND hwnd, int nCtlId) noexcept {
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
void SetClipData(HWND hwnd, LPCWSTR pszData) noexcept {
	if (OpenClipboard(hwnd)) {
		EmptyClipboard();
		HANDLE hData = GlobalAlloc(GHND, sizeof(WCHAR) * (lstrlen(pszData) + 1));
		WCHAR *pData = static_cast<WCHAR *>(GlobalLock(hData));
		lstrcpyn(pData, pszData, static_cast<int>(GlobalSize(hData) / sizeof(WCHAR)));
		GlobalUnlock(hData);
		SetClipboardData(CF_UNICODETEXT, hData);
		CloseClipboard();
	}
}

//=============================================================================
//
//  SetWindowTransparentMode()
//
void SetWindowTransparentMode(HWND hwnd, bool bTransparentMode, int iOpacityLevel) noexcept {
	// https://docs.microsoft.com/en-us/windows/win32/winmsg/using-windows#using-layered-windows
	DWORD exStyle = GetWindowExStyle(hwnd);
	exStyle = bTransparentMode ? (exStyle | WS_EX_LAYERED) : (exStyle & ~WS_EX_LAYERED);
	SetWindowExStyle(hwnd, exStyle);
	if (bTransparentMode) {
		const BYTE bAlpha = static_cast<BYTE>(iOpacityLevel * 255 / 100);
		SetLayeredWindowAttributes(hwnd, 0, bAlpha, LWA_ALPHA);
	}
	// Ask the window and its children to repaint
	RedrawWindow(hwnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}

void SetWindowLayoutRTL(HWND hwnd, bool bRTL) noexcept {
	// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-features#window-layout-and-mirroring
	DWORD exStyle = GetWindowExStyle(hwnd);
	exStyle = bRTL ? (exStyle | WS_EX_LAYOUTRTL) : (exStyle & ~WS_EX_LAYOUTRTL);
	SetWindowExStyle(hwnd, exStyle);
	// update layout in the client area
	InvalidateRect(hwnd, nullptr, TRUE);
}

//=============================================================================
//
//  Toolbar_Get/SetButtons()
//
int Toolbar_GetButtons(HWND hwnd, int cmdBase, LPWSTR lpszButtons, int cchButtons) noexcept {
	const int count = min(MAX_TOOLBAR_ITEM_COUNT_WITH_SEPARATOR, static_cast<int>(SendMessage(hwnd, TB_BUTTONCOUNT, 0, 0)));
	const int maxCch = cchButtons - 3; // two digits, one space and NULL
	int len = 0;

	for (int i = 0; i < count && len < maxCch; i++) {
		TBBUTTON tbb;
		SendMessage(hwnd, TB_GETBUTTON, i, AsInteger<LPARAM>(&tbb));
		const int iCmd = (tbb.idCommand == 0) ? 0 : tbb.idCommand - cmdBase + 1;
		len += wsprintf(lpszButtons + len, L"%i ", iCmd);
	}

	lpszButtons[len--] = L'\0';
	if (len >= 0) {
		lpszButtons[len] = L'\0';
	}
	return count;
}

int Toolbar_SetButtons(HWND hwnd, LPCWSTR lpszButtons, LPCTBBUTTON ptbb, int ctbb) noexcept {
	int count = static_cast<int>(SendMessage(hwnd, TB_BUTTONCOUNT, 0, 0));
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
		int iCmd = static_cast<int>(wcstol(p, &end, 10));
		if (p != end) {
			iCmd = clamp(iCmd, 0, ctbb);
			SendMessage(hwnd, TB_ADDBUTTONS, 1, AsInteger<LPARAM>(&ptbb[iCmd]));
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
void Toolbar_SetButtonImage(HWND hwnd, int idCommand, int iImage) noexcept {
	TBBUTTONINFO tbbi;

	tbbi.cbSize = sizeof(TBBUTTONINFO);
	tbbi.dwMask = TBIF_IMAGE;
	tbbi.iImage = iImage;

	SendMessage(hwnd, TB_SETBUTTONINFO, idCommand, AsInteger<LPARAM>(&tbbi));
}

//=============================================================================
//
//  SendWMSize()
//
LRESULT SendWMSize(HWND hwnd) noexcept {
	RECT rc;
	GetClientRect(hwnd, &rc);
	return SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
}

#if NP2_ENABLE_APP_LOCALIZATION_DLL
HMODULE LoadLocalizedResourceDLL(LANGID lang, LPCWSTR dllName) noexcept {
	if (lang == LANG_USER_DEFAULT) {
		lang = GetUserDefaultUILanguage();
	}

	LPCWSTR folder = nullptr;
	const LANGID subLang = SUBLANGID(lang);
	switch (PRIMARYLANGID(lang)) {
	case LANG_ENGLISH:
		break;
	case LANG_CHINESE:
		folder = IsChineseTraditionalSubLang(subLang) ? L"zh-Hant" : L"zh-Hans";
		break;
	case LANG_FRENCH:
		folder = L"fr";
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
	case LANG_POLISH:
		folder = L"pl";
		break;
	case LANG_PORTUGUESE:
		folder = L"pt-BR";
		break;
	case LANG_RUSSIAN:
		folder = L"ru";
		break;
	}

	if (folder == nullptr) {
		return nullptr;
	}

	WCHAR path[MAX_PATH];
	GetProgramRealPath(path, COUNTOF(path));
	PathRemoveFileSpec(path);
	PathAppend(path, L"locale");
	PathAppend(path, folder);
	PathAppend(path, dllName);

	const DWORD flags = IsVistaAndAbove() ? (LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE) : LOAD_LIBRARY_AS_DATAFILE;
	HMODULE hDLL = LoadLibraryEx(path, nullptr, flags);
	return hDLL;
}

#if NP2_ENABLE_TEST_LOCALIZATION_LAYOUT
void GetLocaleDefaultUIFont(LANGID lang, LPWSTR lpFaceName, WORD *wSize) noexcept {
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

bool PathGetRealPath(HANDLE hFile, LPCWSTR lpszSrc, LPWSTR lpszDest) noexcept {
	WCHAR path[8 + MAX_PATH] = L"";
	if (IsVistaAndAbove()) {
		const bool closing = hFile == nullptr;
		if (closing) {
			hFile = CreateFile(lpszSrc, FILE_READ_ATTRIBUTES,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
		}
		if (hFile != INVALID_HANDLE_VALUE) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			DWORD cch = GetFinalPathNameByHandleW(hFile, path, COUNTOF(path), FILE_NAME_OPENED);
#else
			using GetFinalPathNameByHandleSig = DWORD (WINAPI *)(HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
			static GetFinalPathNameByHandleSig pfnGetFinalPathNameByHandle = nullptr;
			if (pfnGetFinalPathNameByHandle == nullptr) {
				pfnGetFinalPathNameByHandle = DLLFunctionEx<GetFinalPathNameByHandleSig>(L"kernel32.dll", "GetFinalPathNameByHandleW");
			}
			DWORD cch = pfnGetFinalPathNameByHandle(hFile, path, COUNTOF(path), FILE_NAME_OPENED);
#endif
			// TODO: support long path
			if (closing) {
				CloseHandle(hFile);
			}
			if (cch != 0 && StrStartsWith(path, L"\\\\?\\")) {
				cch -= CSTRLEN(L"\\\\?\\");
				WCHAR *p = path + CSTRLEN(L"\\\\?\\");
				if (StrStartsWith(p, L"UNC\\")) {
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

	DWORD cch = GetFullPathName(lpszSrc, COUNTOF(path), path, nullptr);
	if (cch > 0 && cch < COUNTOF(path)) {
		WCHAR *p = path;
		if (StrStartsWith(path, L"\\\\?\\")) {
			cch -= CSTRLEN(L"\\\\?\\");
			p += CSTRLEN(L"\\\\?\\");
			if (StrStartsWith(p, L"UNC\\")) {
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
void PathRelativeToApp(LPCWSTR lpszSrc, LPWSTR lpszDest, DWORD dwAttrTo, bool bUnexpandEnv, bool bUnexpandMyDocs) noexcept {
	WCHAR wchPath[MAX_PATH];

	if (!PathIsRelative(lpszSrc)) {
		WCHAR wchAppPath[MAX_PATH];
		WCHAR wchWinDir[MAX_PATH];
		GetModuleFileName(nullptr, wchAppPath, COUNTOF(wchAppPath));
		PathRemoveFileSpec(wchAppPath);

		if (bUnexpandMyDocs) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			LPWSTR wchUserFiles = nullptr;
			if (S_OK != SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &wchUserFiles)) {
				return;
			}
#else
			WCHAR wchUserFiles[MAX_PATH];
			if (S_OK != SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, wchUserFiles)) {
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
			if (!PathCommonPrefix(wchAppPath, wchWinDir, nullptr)
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
void PathAbsoluteFromApp(LPCWSTR lpszSrc, LPWSTR lpszDest, bool bExpandEnv) noexcept {
	WCHAR wchPath[MAX_PATH];

	if (StrStartsWith(lpszSrc, L"%CSIDL:MYDOCUMENTS%")) {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		LPWSTR pszPath = nullptr;
		if (S_OK != SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &pszPath)) {
			return;
		}
		lpszSrc += CSTRLEN("%CSIDL:MYDOCUMENTS%");
		if (*lpszSrc == L'\\' || *lpszSrc == L'/') {
			++lpszSrc;
		}
		PathCombine(wchPath, pszPath, lpszSrc);
		CoTaskMemFree(pszPath);
#else
		if (S_OK != SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, wchPath)) {
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
		GetModuleFileName(nullptr, wchResult, COUNTOF(wchResult));
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

	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, AsPPVArgs(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, AsPPVArgs(&ppf)))) {
			if (SUCCEEDED(ppf->Load(pszLnkFile, STGM_READ))) {
				hr = psl->GetPath(tchPath, COUNTOF(tchPath), nullptr, 0);
			}
			ppf->Release();
		}
		psl->Release();
	}

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

	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, AsPPVArgs(&psl)))) {
		IPersistFile *ppf;

		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, AsPPVArgs(&ppf)))) {
			psl->SetPath(pszPath);

			if (SUCCEEDED(ppf->Save(tchLnkFileName, TRUE))) {
				bSucceeded = true;
			}
			ppf->Release();
		}
		psl->Release();
	}

	return bSucceeded;
}

void OpenContainingFolder(HWND hwnd, LPCWSTR pszFile, bool bSelect) noexcept {
	WCHAR wchDirectory[MAX_PATH];
	lstrcpyn(wchDirectory, pszFile, COUNTOF(wchDirectory));

	LPCWSTR path = nullptr;
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

	PIDLIST_ABSOLUTE pidl = ILCreateFromPath(wchDirectory);
	if (pidl) {
		HRESULT hr;
		PIDLIST_ABSOLUTE pidlEntry = path ? ILCreateFromPath(path) : nullptr;
		if (pidlEntry) {
			hr = SHOpenFolderAndSelectItems(pidl, 1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&pidlEntry), 0);
			CoTaskMemFree(pidlEntry);
		} else if (!bSelect) {
#if 0
			// Use an invalid item to open the folder?
			hr = SHOpenFolderAndSelectItems(pidl, 1, reinterpret_cast<PCUITEMID_CHILD_ARRAY>(&pidl), 0);
#else
			SHELLEXECUTEINFO sei;
			memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_IDLIST;
			sei.hwnd = hwnd;
			//sei.lpVerb = L"explore";
			sei.lpVerb = L"open";
			sei.lpIDList = pidl;
			sei.nShow = SW_SHOW;

			const BOOL result = ShellExecuteEx(&sei);
			hr = result ? S_OK : S_FALSE;
#endif
		} else {
			// open parent folder and select the folder
			hr = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
		}
		CoTaskMemFree(pidl);
		if (hr == S_OK) {
			return;
		}
	}

#if 0
	if (path == nullptr) {
		path = wchDirectory;
	}

	// open a new explorer window every time
	LPWSTR szParameters = static_cast<LPWSTR>(NP2HeapAlloc((lstrlen(path) + 64) * sizeof(WCHAR)));
	lstrcpy(szParameters, bSelect ? L"/select," : L"");
	lstrcat(szParameters, L"\"");
	lstrcat(szParameters, path);
	lstrcat(szParameters, L"\"");
	ShellExecute(hwnd, L"open", L"explorer", szParameters, nullptr, SW_SHOW);
	NP2HeapFree(szParameters);
#endif
}

//=============================================================================
//
//  ExtractFirstArgument()
//
bool ExtractFirstArgument(LPCWSTR lpArgs, LPWSTR lpArg1, LPWSTR lpArg2) noexcept {
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
void PrepareFilterStr(LPWSTR lpFilter) noexcept {
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
void StrTab2Space(LPWSTR lpsz) noexcept {
	WCHAR *c = lpsz;
	while ((c = StrChr(c, L'\t')) != nullptr) {
		*c++ = L' ';
	}
}

//=============================================================================
//
// PathFixBackslashes() - in place conversion
//
bool PathFixBackslashes(LPWSTR lpsz) noexcept {
	WCHAR *c = lpsz;
	bool bFixed = false;
	while ((c = StrChr(c, L'/')) != nullptr) {
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
void ExpandEnvironmentStringsEx(LPWSTR lpSrc, DWORD dwSrc) noexcept {
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

bool SearchPathEx(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer) noexcept {
	if (StrEqualEx(lpFileName, L"..") || StrEqualEx(lpFileName, L".")) {
		if (StrEqualEx(lpFileName, L"..") && PathIsRoot(szCurDir)) {
			StrCpyEx(lpBuffer, L"*.*");
			return true;
		}
	}

	DWORD cch = SearchPath(szCurDir, lpFileName, nullptr, nBufferLength, lpBuffer, nullptr);
	if (cch == 0) {
		// Search Favorites if no result
		cch = SearchPath(tchFavoritesDir, lpFileName, nullptr, nBufferLength, lpBuffer, nullptr);
	}

	return cch != 0 && cch < MAX_PATH;
}

//=============================================================================
//
//  FormatNumber()
//
void FormatNumber(LPWSTR lpNumberStr, UINT value) noexcept {
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
void GetDefaultFavoritesDir(LPWSTR lpFavDir, int cchFavDir) noexcept {
	PIDLIST_ABSOLUTE pidl;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	if (S_OK == SHGetKnownFolderIDList(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &pidl))
#else
	if (S_OK == SHGetFolderLocation(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_DEFAULT, &pidl))
#endif
	{
		SHGetPathFromIDList(pidl, lpFavDir);
		CoTaskMemFree(pidl);
	} else {
		GetWindowsDirectory(lpFavDir, cchFavDir);
	}
}

//=============================================================================
//
//  GetDefaultOpenWithDir()
//
void GetDefaultOpenWithDir(LPWSTR lpOpenWithDir, int cchOpenWithDir) noexcept {
	PIDLIST_ABSOLUTE pidl;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	if (S_OK == SHGetKnownFolderIDList(FOLDERID_Desktop, KF_FLAG_DEFAULT, nullptr, &pidl))
#else
	if (S_OK == SHGetFolderLocation(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_DEFAULT, &pidl))
#endif
	{
		SHGetPathFromIDList(pidl, lpOpenWithDir);
		CoTaskMemFree(pidl);
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
HDROP CreateDropHandle(LPCWSTR lpFileName) noexcept {
	HGLOBAL hDrop = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DROPFILES) + sizeof(WCHAR) * (lstrlen(lpFileName) + 2));
	LPDROPFILES lpdf = static_cast<LPDROPFILES>(GlobalLock(hDrop));

	lpdf->pFiles = sizeof(DROPFILES);
	lpdf->pt.x   = 0;
	lpdf->pt.y   = 0;
	lpdf->fNC    = TRUE;
	lpdf->fWide  = TRUE;

	lstrcpy(reinterpret_cast<WCHAR *>(lpdf + 1), lpFileName);
	GlobalUnlock(hDrop);

	return static_cast<HDROP>(hDrop);
}

//=============================================================================
//
//  ExecDDECommand()
//
//  Execute a DDE command (Msg, App, Topic)
//
//
HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, ULONG_PTR dwData1, ULONG_PTR dwData2) noexcept {
	UNREFERENCED_PARAMETER(uFmt);
	UNREFERENCED_PARAMETER(hconv);
	UNREFERENCED_PARAMETER(hsz1);
	UNREFERENCED_PARAMETER(hsz2);
	UNREFERENCED_PARAMETER(hdata);
	UNREFERENCED_PARAMETER(dwData1);
	UNREFERENCED_PARAMETER(dwData2);

	switch (uType) {
	case XTYP_ADVDATA:
		return AsPointer<HDDEDATA, ULONG_PTR>(DDE_FACK);

	default:
		return nullptr;
	}
}

bool ExecDDECommand(LPCWSTR lpszCmdLine, LPCWSTR lpszDDEMsg, LPCWSTR lpszDDEApp, LPCWSTR lpszDDETopic) noexcept {
	if (StrIsEmpty(lpszCmdLine) || StrIsEmpty(lpszDDEMsg) || StrIsEmpty(lpszDDEApp) || StrIsEmpty(lpszDDETopic)) {
		return false;
	}

	bool bSuccess = true;
	WCHAR lpszDDEMsgBuf[256];

	lstrcpyn(lpszDDEMsgBuf, lpszDDEMsg, COUNTOF(lpszDDEMsgBuf));
	WCHAR *pSubst = StrStr(lpszDDEMsgBuf, L"%1");
	if (pSubst != nullptr) {
		pSubst[1] = L's';
	}

	WCHAR lpszURLExec[512];
	wsprintf(lpszURLExec, lpszDDEMsgBuf, lpszCmdLine);

	DWORD idInst = 0;
	if (DdeInitialize(&idInst, DdeCallback, APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0L) == DMLERR_NO_ERROR) {
		HSZ hszService = DdeCreateStringHandle(idInst, lpszDDEApp, CP_WINUNICODE);
		HSZ hszTopic = DdeCreateStringHandle(idInst, lpszDDETopic, CP_WINUNICODE);
		if (hszService && hszTopic) {
			HCONV hConv = DdeConnect(idInst, hszService, hszTopic, nullptr);
			if (hConv) {
				DdeClientTransaction(reinterpret_cast<LPBYTE>(lpszURLExec), sizeof(WCHAR) * (lstrlen(lpszURLExec) + 1), hConv, nullptr, 0, XTYP_EXECUTE, TIMEOUT_ASYNC, nullptr);
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
void HistoryList::Init() noexcept {
	iCurItem = -1;
	memset(AsVoidPointer(pszItems), 0, sizeof(pszItems));
}

void HistoryList::Empty() noexcept {
	for (int i = 0; i < HISTORY_ITEMS; i++) {
		if (pszItems[i]) {
			LocalFree(pszItems[i]);
			pszItems[i] = nullptr;
		}
	}
}

bool HistoryList::Add(LPCWSTR pszNew) noexcept {
	// Item to be added is equal to current item
	if (iCurItem >= 0 && iCurItem < HISTORY_ITEMS) {
		if (pszItems[iCurItem] != nullptr && PathEqual(pszNew, pszItems[iCurItem])) {
			return false;
		}
	}

	if (iCurItem < (HISTORY_ITEMS - 1)) {
		iCurItem++;
		for (int i = iCurItem; i < HISTORY_ITEMS; i++) {
			if (pszItems[i]) {
				LocalFree(pszItems[i]);
				pszItems[i] = nullptr;
			}
		}
	} else {
		// Shift
		if (pszItems[0]) {
			LocalFree(pszItems[0]);
		}

		memmove(AsVoidPointer(pszItems), AsVoidPointer(pszItems + 1), (HISTORY_ITEMS - 1) * sizeof(LPWSTR));
	}

	pszItems[iCurItem] = StrDup(pszNew);

	return true;
}

bool HistoryList::Forward(LPWSTR pszItem, int cItem) noexcept {
	if (iCurItem < (HISTORY_ITEMS - 1)) {
		if (pszItems[iCurItem + 1]) {
			iCurItem++;
			lstrcpyn(pszItem, pszItems[iCurItem], cItem);
			return true;
		}
	}

	return false;
}

bool HistoryList::Back(LPWSTR pszItem, int cItem) noexcept {
	if (iCurItem > 0) {
		if (pszItems[iCurItem - 1]) {
			iCurItem--;
			lstrcpyn(pszItem, pszItems[iCurItem], cItem);
			return true;
		}
	}

	return false;
}

bool HistoryList::CanForward() const noexcept {
	if (iCurItem < (HISTORY_ITEMS - 1)) {
		if (pszItems[iCurItem + 1]) {
			return true;
		}
	}

	return false;
}

bool HistoryList::CanBack() const noexcept {
	if (iCurItem > 0) {
		if (pszItems[iCurItem - 1]) {
			return true;
		}
	}

	return false;
}

void HistoryList::UpdateToolbar(HWND hwnd, int cmdBack, int cmdForward) const noexcept {
	if (CanBack()) {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdBack, MAKELPARAM(1, 0));
	} else {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdBack, MAKELPARAM(0, 0));
	}

	if (CanForward()) {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdForward, MAKELPARAM(1, 0));
	} else {
		SendMessage(hwnd, TB_ENABLEBUTTON, cmdForward, MAKELPARAM(0, 0));
	}
}

//=============================================================================
//
//  MRU functions
//
void MRUList::Init(LPCWSTR pszRegKey, int flags) noexcept {
	iSize = 0;
	iFlags = flags;
	szRegKey = pszRegKey;
	memset(AsVoidPointer(pszItems), 0, sizeof(pszItems));
	Load();
}

static inline bool MRU_Equal(int flags, LPCWSTR psz1, LPCWSTR psz2) noexcept {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return CompareStringOrdinal(psz1, -1, psz2, -1, flags & MRUFlags_FilePath) == CSTR_EQUAL;
#else
	return (flags & MRUFlags_FilePath) ? PathEqual(psz1, psz2) : StrEqual(psz1, psz2);
#endif
}

void MRUList::Add(LPCWSTR pszNew) noexcept {
	const int flags = iFlags;
	LPWSTR tchItem = nullptr;
	int i;
	for (i = 0; i < MRU_MAXITEMS; i++) {
		WCHAR * const item = pszItems[i];
		if (item == nullptr) {
			break;
		}
		if (MRU_Equal(flags, item, pszNew)) {
			tchItem = item;
			break;
		}
	}
	if (i == MRU_MAXITEMS) {
		--i;
		LocalFree(pszItems[i]);
	} else if (i == iSize) {
		iSize += 1;
	}
	for (; i > 0; i--) {
		pszItems[i] = pszItems[i - 1];
	}
	if (tchItem == nullptr) {
		tchItem = StrDup(pszNew);
	}
	pszItems[0] = tchItem;
}

void MRUList::Delete(int iIndex) noexcept {
	if (iIndex < 0 || iIndex >= iSize) {
		return;
	}
	LocalFree(pszItems[iIndex]);
	pszItems[iIndex] = nullptr;
	iSize -= 1;
	for (int i = iIndex; i < iSize; i++) {
		pszItems[i] = pszItems[i + 1];
		pszItems[i + 1] = nullptr;
	}
}

void MRUList::Empty(bool save) noexcept {
	for (int i = 0; i < iSize; i++) {
		LocalFree(pszItems[i]);
		pszItems[i] = nullptr;
	}
	iSize = 0;
	if (save && StrNotEmpty(szIniFile)) {
		IniClearSection(szRegKey);
	}
}

void MRUList::Load() noexcept {
	if (StrIsEmpty(szIniFile)) {
		return;
	}

	IniSectionParser section;
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU));
	const DWORD cchIniSection = static_cast<DWORD>(NP2HeapSize(pIniSectionBuf) / sizeof(WCHAR));

	section.Init(MRU_MAXITEMS);
	LoadIniSection(szRegKey, pIniSectionBuf, cchIniSection);
	section.ParseArray(pIniSectionBuf);
	UINT n = 0;

	for (UINT i = 0; i < section.count; i++) {
		LPCWSTR tchItem = section.nodeList[i].value;
		if (StrNotEmpty(tchItem)) {
			pszItems[n++] = StrDup(tchItem);
		}
	}

	iSize = n;
	section.Free();
	NP2HeapFree(pIniSectionBuf);
}

void MRUList::Save() const noexcept {
	if (StrIsEmpty(szIniFile)) {
		return;
	}
	if (iSize <= 0) {
		IniClearSection(szRegKey);
		return;
	}

	WCHAR tchName[16];
	WCHAR *pIniSectionBuf = static_cast<WCHAR *>(NP2HeapAlloc(sizeof(WCHAR) * MAX_INI_SECTION_SIZE_MRU));
	IniSectionBuilder section = { pIniSectionBuf };

	for (int i = 0; i < iSize; i++) {
		LPCWSTR tchItem = pszItems[i];
		if (StrNotEmpty(tchItem)) {
			wsprintf(tchName, L"%02i", i + 1);
			section.SetString(tchName, tchItem);
		}
	}

	SaveIniSection(szRegKey, pIniSectionBuf);
	NP2HeapFree(pIniSectionBuf);
}

void MRUList::AddToCombobox(HWND hwnd) const noexcept {
	for (int i = 0; i < iSize; i++) {
		LPCWSTR str = pszItems[i];
		ComboBox_AddString(hwnd, str);
	}
}

/*
  Themed Dialogs
  Modify dialog templates to use current theme font
  Based on code of MFC helper class CDialogTemplate
*/
bool GetThemedDialogFont(LPWSTR lpFaceName, WORD *wSize) noexcept {
#if NP2_ENABLE_APP_LOCALIZATION_DLL && NP2_ENABLE_TEST_LOCALIZATION_LAYOUT
	extern LANGID uiLanguage;
	GetLocaleDefaultUIFont(uiLanguage, lpFaceName, wSize);
	return true;
#else

	bool bSucceed = false;
	int lfHeight = 0;

	if (IsAppThemed()) {
		HTHEME hTheme = OpenThemeData(nullptr, L"WINDOWSTYLE;WINDOW");
		if (hTheme) {
			LOGFONT lf;
			if (S_OK == GetThemeSysFont(hTheme, TMT_MSGBOXFONT, &lf)) {
				lfHeight = lf.lfHeight;
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
			lfHeight = ncm.lfMessageFont.lfHeight;
			lstrcpyn(lpFaceName, ncm.lfMessageFont.lfFaceName, LF_FACESIZE);
			bSucceed = true;
		}
	}

	if (bSucceed) {
		if (lfHeight < 0) {
			lfHeight = -lfHeight;
		}
		lfHeight = MulDiv(lfHeight, 72, g_uSystemDPI);
		lfHeight = max(lfHeight, 8);
		*wSize = static_cast<WORD>(lfHeight);
		if (!IsVistaAndAbove()) {
			// Windows 2000, XP, 2003
			lstrcpy(lpFaceName, L"Tahoma");
		}
	}
	return bSucceed;
#endif
}

namespace {

// https://learn.microsoft.com/en-us/windows/win32/dlgbox/dlgtemplateex
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

bool DialogTemplate_IsDialogEx(const DLGTEMPLATE *pTemplate) noexcept {
	return (reinterpret_cast<const DLGTEMPLATEEX *>(pTemplate))->signature == 0xFFFF;
}

BOOL DialogTemplate_HasFont(const DLGTEMPLATE *pTemplate, bool bDialogEx) noexcept {
	return (DS_SETFONT & (bDialogEx ? (reinterpret_cast<const DLGTEMPLATEEX *>(pTemplate))->style : pTemplate->style));
}

constexpr DWORD DialogTemplate_FontAttrSize(bool bDialogEx) noexcept {
	return sizeof(WORD) * (bDialogEx ? 3 : 1);
}

BYTE *DialogTemplate_GetFontSizeField(DLGTEMPLATE *pTemplate, bool bDialogEx) noexcept {
	WORD *pw;

	if (bDialogEx) {
		pw = reinterpret_cast<WORD *>(reinterpret_cast<DLGTEMPLATEEX *>(pTemplate) + 1);
	} else {
		pw = reinterpret_cast<WORD *>(pTemplate + 1);
	}
	// menu
	if (*pw == 0xFFFF) {
		pw += 2;
	} else {
		while (*pw++){}
	}
	// window class
	if (*pw == 0xFFFF) {
		pw += 2;
	} else {
		while (*pw++){}
	}
	// title
	while (*pw++){}

	return reinterpret_cast<BYTE *>(pw);
}

}

DLGTEMPLATE *LoadThemedDialogTemplate(LPCWSTR lpDialogTemplateID, HINSTANCE hInstance) noexcept {
	HRSRC hRsrc = FindResource(hInstance, lpDialogTemplateID, RT_DIALOG);
	if (hRsrc == nullptr) {
		return nullptr;
	}

	HGLOBAL hRsrcMem = LoadResource(hInstance, hRsrc);
	const DLGTEMPLATE *pRsrcMem = static_cast<DLGTEMPLATE *>(LockResource(hRsrcMem));
	const DWORD dwTemplateSize = SizeofResource(hInstance, hRsrc);

	DLGTEMPLATE *pTemplate = dwTemplateSize ? static_cast<DLGTEMPLATE *>(NP2HeapAlloc(dwTemplateSize + LF_FACESIZE*sizeof(WCHAR))) : nullptr;
	if (pTemplate == nullptr) {
		FreeResource(hRsrcMem);
		return nullptr;
	}

	memcpy(reinterpret_cast<BYTE *>(pTemplate), pRsrcMem, dwTemplateSize);
	FreeResource(hRsrcMem);

	WCHAR wchFaceName[LF_FACESIZE];
	WORD wFontSize;
	if (!GetThemedDialogFont(wchFaceName, &wFontSize)) {
		return pTemplate;
	}

	const bool bDialogEx = DialogTemplate_IsDialogEx(pTemplate);
	const BOOL bHasFont = DialogTemplate_HasFont(pTemplate, bDialogEx);
	const DWORD cbFontAttr = DialogTemplate_FontAttrSize(bDialogEx);

	if (bDialogEx) {
		(reinterpret_cast<DLGTEMPLATEEX *>(pTemplate))->style |= DS_SHELLFONT;
	} else {
		pTemplate->style |= DS_SHELLFONT;
	}

	const DWORD cbNew = cbFontAttr + ((lstrlen(wchFaceName) + 1) * sizeof(WCHAR));
	const BYTE *pbNew = reinterpret_cast<BYTE *>(wchFaceName);

	BYTE *pb = DialogTemplate_GetFontSizeField(pTemplate, bDialogEx);
	const DWORD cbOld = bHasFont ? cbFontAttr + sizeof(WCHAR) * (lstrlen(reinterpret_cast<WCHAR *>(pb + cbFontAttr)) + 1) : 0;

	const BYTE *pOldControls = reinterpret_cast<BYTE *>(NP2_align_up(AsInteger<DWORD_PTR>(pb) + cbOld, sizeof(DWORD)));
	BYTE *pNewControls = reinterpret_cast<BYTE *>(NP2_align_up(AsInteger<DWORD_PTR>(pb) + cbNew, sizeof(DWORD)));

	const WORD nCtrl = bDialogEx ? (reinterpret_cast<DLGTEMPLATEEX *>(pTemplate))->cDlgItems : pTemplate->cdit;
	if (cbNew != cbOld && nCtrl > 0) {
		memmove(pNewControls, pOldControls, dwTemplateSize - (pOldControls - reinterpret_cast<BYTE *>(pTemplate)));
	}

	*reinterpret_cast<WORD *>(pb) = wFontSize;
	memmove(pb + cbFontAttr, pbNew, cbNew - cbFontAttr);

	return pTemplate;
}

INT_PTR ThemedDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) noexcept {
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
static LRESULT CALLBACK OpenSaveFileDlgSubProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept {
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

UINT_PTR CALLBACK OpenSaveFileDlgHookProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) noexcept {
	UNREFERENCED_PARAMETER(wParam);

	switch (umsg) {
	case WM_NOTIFY: {
		LPOFNOTIFY pOFNOTIFY = AsPointer<LPOFNOTIFY>(lParam);
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
// 1.1 01.07.2000 The window retains its place in the Z-order of windows
//     when minimized/hidden. This means that when restored/shown, it doesn't
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
static void GetTrayWndRect(LPRECT lpTrayRect) noexcept {
	// First, we'll use a quick hack method. We know that the taskbar is a window
	// of class Shell_TrayWnd, and the status tray is a child of this of class
	// TrayNotifyWnd. This provides us a window rect to minimize to. Note, however,
	// that this is not guaranteed to work on future versions of the shell. If we
	// use this method, make sure we have a backup!
	HWND hShellTrayWnd = FindWindowEx(nullptr, nullptr, L"Shell_TrayWnd", nullptr);
	if (hShellTrayWnd) {
		HWND hTrayNotifyWnd = FindWindowEx(hShellTrayWnd, nullptr, L"TrayNotifyWnd", nullptr);
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
		// system tray. Use various fudge factors to make it look good
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
	// Note that if the 3rd party shell supports the same configuration as
	// explorer (the icons hosted in NotifyTrayWnd, which is a child window of
	// Shell_TrayWnd), we would already have caught it above
	hShellTrayWnd = FindWindowEx(nullptr, nullptr, L"Shell_TrayWnd", nullptr);
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
/*static */bool GetDoAnimateMinimize() noexcept {
	ANIMATIONINFO ai;

	ai.cbSize = sizeof(ai);
	SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0);

	return ai.iMinAnimate != 0;
}

void MinimizeWndToTray(HWND hwnd) noexcept {
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

void RestoreWndFromTray(HWND hwnd) noexcept {
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
