/******************************************************************************
*
*
* Notepad2
*
* Edit.c
*   Text File Editing Helper Stuff
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

#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commdlg.h>
#if 0
#include <uxtheme.h>
#include <vssym32.h>
#endif
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include "SciCall.h"
#include "VectorISA.h"
#include "Helpers.h"
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
#include "resource.h"

extern HWND hwndMain;
extern DWORD dwLastIOError;
extern HWND hDlgFindReplace;
extern UINT cpLastFind;
extern BOOL bReplaceInitialized;

extern int xFindReplaceDlg;
extern int yFindReplaceDlg;
extern int cxFindReplaceDlg;

extern int iDefaultEncoding;
extern int iDefaultEOLMode;
extern BOOL bFixLineEndings;
extern BOOL bAutoStripBlanks;

// Default Codepage and Character Set
extern int iDefaultCodePage;
//extern int iDefaultCharSet;
extern BOOL bLoadANSIasUTF8;
extern BOOL bLoadNFOasOEM;
extern int iSrcEncoding;
extern int iWeakSrcEncoding;

extern int g_DOSEncoding;

extern LPMRULIST mruFind;
extern LPMRULIST mruReplace;
extern WCHAR szCurFile[MAX_PATH + 40];

static DStringW wchPrefixSelection;
static DStringW wchAppendSelection;
static DStringW wchPrefixLines;
static DStringW wchAppendLines;

#define MAX_NON_UTF8_SIZE	(UINT_MAX/2 - 16)

static struct EditMarkAllStatus {
	int findFlag;
	Sci_Position iSelCount;
	LPSTR pszText;
} editMarkAllStatus;

void Edit_ReleaseResources(void) {
	DStringW_Free(&wchPrefixSelection);
	DStringW_Free(&wchAppendSelection);
	DStringW_Free(&wchPrefixLines);
	DStringW_Free(&wchAppendLines);
	if (editMarkAllStatus.pszText) {
		NP2HeapFree(editMarkAllStatus.pszText);
	}
}

static inline void NotifyRectangleSelection(void) {
	//MsgBox(MBWARN, IDS_SELRECT);
	ShowNotificationMessage(SC_NOTIFICATIONPOSITION_CENTER, IDS_SELRECT);
}

//=============================================================================
//
// EditSetNewText()
//
extern BOOL bFreezeAppTitle;
extern BOOL bLockedForEditing;
#if defined(_WIN64)
extern BOOL bLargeFileMode;
#endif
extern FILEVARS fvCurFile;

void EditSetNewText(LPCSTR lpstrText, DWORD cbText, Sci_Line lineCount) {
	bFreezeAppTitle = TRUE;
	bLockedForEditing = FALSE;

	SciCall_SetReadOnly(FALSE);
	SciCall_Cancel();
	SciCall_SetUndoCollection(FALSE);
	SciCall_EmptyUndoBuffer();
	SciCall_ClearAll();
	SciCall_ClearMarker();
	SciCall_SetXOffset(0);

	FileVars_Apply(&fvCurFile);

#if defined(_WIN64)
	// enable conversion between line endings
	if (bLargeFileMode || cbText + lineCount >= MAX_NON_UTF8_SIZE) {
		int options = SciCall_GetDocumentOptions();
		if (!(options & SC_DOCUMENTOPTION_TEXT_LARGE)) {
			options |= SC_DOCUMENTOPTION_TEXT_LARGE;
			HANDLE pdoc = SciCall_CreateDocument(cbText + 1, options);
			EditReplaceDocument(pdoc);
			bLargeFileMode = TRUE;
		}
	}
#endif

	if (cbText > 0) {
		SciCall_SetModEventMask(SC_MOD_NONE);
#if 0
		StopWatch watch;
		StopWatch_Start(watch);
#endif
		SciCall_SetInitLineCount(lineCount);
		SciCall_AddText(cbText, lpstrText);
#if 0
		StopWatch_Stop(watch);
		StopWatch_ShowLog(&watch, "AddText time");
#endif
		SciCall_SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
	}

	SciCall_SetUndoCollection(TRUE);
	SciCall_EmptyUndoBuffer();
	SciCall_SetSavePoint();
	SciCall_GotoPos(0);
	SciCall_ChooseCaretX();

	bFreezeAppTitle = FALSE;
}

//=============================================================================
//
// EditConvertText()
//
BOOL EditConvertText(UINT cpSource, UINT cpDest, BOOL bSetSavePoint) {
	if (cpSource == cpDest) {
		return TRUE;
	}

	const Sci_Position length = SciCall_GetLength();
	if (length >= (int)MAX_NON_UTF8_SIZE) {
		return TRUE;
	}

	char *pchText = NULL;
	int cbText = 0;
	if (length > 0) {
		// DBCS: length -> WCHAR: sizeof(WCHAR) * (length / 2) -> UTF-8: kMaxMultiByteCount * (length / 2)
		pchText = (char *)NP2HeapAlloc((length + 1) * sizeof(WCHAR));
		SciCall_GetText(NP2HeapSize(pchText), pchText);

		WCHAR *pwchText = (WCHAR *)NP2HeapAlloc((length + 1) * sizeof(WCHAR));
		const int cbwText = MultiByteToWideChar(cpSource, 0, pchText, (int)length, pwchText, (int)(NP2HeapSize(pwchText) / sizeof(WCHAR)));
		cbText = WideCharToMultiByte(cpDest, 0, pwchText, cbwText, pchText, (int)(NP2HeapSize(pchText)), NULL, NULL);
		NP2HeapFree(pwchText);
	}

	bLockedForEditing = FALSE;
	SciCall_SetReadOnly(FALSE);
	SciCall_Cancel();
	SciCall_SetUndoCollection(FALSE);
	SciCall_EmptyUndoBuffer();
	SciCall_ClearAll();
	SciCall_ClearMarker();
	SciCall_SetCodePage(cpDest);

	if (cbText > 0) {
		SciCall_AddText(cbText, pchText);
	}
	if (pchText != NULL) {
		NP2HeapFree(pchText);
	}

	SciCall_EmptyUndoBuffer();
	SciCall_SetUndoCollection(TRUE);
	SciCall_GotoPos(0);
	SciCall_ChooseCaretX();
	if (length == 0 && bSetSavePoint) {
		SciCall_SetSavePoint();
	}
	return TRUE;
}

#if defined(_WIN64)
void EditConvertToLargeMode(void) {
	int options = SciCall_GetDocumentOptions();
	if (options & SC_DOCUMENTOPTION_TEXT_LARGE) {
		return;
	}

	options |= SC_DOCUMENTOPTION_TEXT_LARGE;
	const Sci_Position length = SciCall_GetLength();
	HANDLE pdoc = SciCall_CreateDocument(length + 1, options);
	char *pchText = NULL;
	if (length > 0) {
		pchText = (char *)NP2HeapAlloc(length + 1);
		SciCall_GetText(NP2HeapSize(pchText), pchText);
	}

	bLockedForEditing = FALSE;
	SciCall_SetReadOnly(FALSE);
	SciCall_Cancel();
	SciCall_SetUndoCollection(FALSE);
	SciCall_EmptyUndoBuffer();
	SciCall_ClearAll();
	SciCall_ClearMarker();

	EditReplaceDocument(pdoc);
	if (length > 0) {
		SciCall_SetModEventMask(SC_MOD_NONE);
		SciCall_AddText(length, pchText);
		SciCall_SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
	}
	if (pchText != NULL) {
		NP2HeapFree(pchText);
	}

	SciCall_SetUndoCollection(TRUE);
	SciCall_EmptyUndoBuffer();
	SciCall_GotoPos(0);
	SciCall_ChooseCaretX();
	SciCall_SetSavePoint();

	Style_SetLexer(pLexCurrent, TRUE);
	bLargeFileMode = TRUE;
}
#endif

//=============================================================================
//
// EditGetClipboardText()
//
char* EditGetClipboardText(HWND hwnd) {
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT) || !OpenClipboard(GetParent(hwnd))) {
		return NULL;
	}

	HANDLE hmem = GetClipboardData(CF_UNICODETEXT);
	LPCWSTR pwch = (LPCWSTR)GlobalLock(hmem);
	const int wlen = lstrlen(pwch);

	const UINT cpEdit = SciCall_GetCodePage();
	const int mlen = WideCharToMultiByte(cpEdit, 0, pwch, wlen + 1, NULL, 0, NULL, NULL) - 1;
	char *pmch = (char *)LocalAlloc(LPTR, mlen + 1);
	char *ptmp = (char *)NP2HeapAlloc(mlen * 4 + 1);

	if (pmch && ptmp) {
		const char *s = pmch;
		char *d = ptmp;

		WideCharToMultiByte(cpEdit, 0, pwch, wlen + 1, pmch, mlen + 1, NULL, NULL);

		const int iEOLMode = SciCall_GetEOLMode();
		for (int i = 0; (i < mlen) && (*s != 0); i++) {
			if (*s == '\n' || *s == '\r') {
				if (iEOLMode == SC_EOL_CR) {
					*d++ = '\r';
				} else if (iEOLMode == SC_EOL_LF) {
					*d++ = '\n';
				} else { // iEOLMode == SC_EOL_CRLF
					*d++ = '\r';
					*d++ = '\n';
				}
				if ((*s == '\r') && (i + 1 < mlen) && (*(s + 1) == '\n')) {
					i++;
					s++;
				}
				s++;
			} else {
				*d++ = *s++;
			}
		}

		*d++ = 0;
		LocalFree(pmch);
		pmch = (char *)LocalAlloc(LPTR, (d - ptmp));
		strcpy(pmch, ptmp);
		NP2HeapFree(ptmp);
	}

	GlobalUnlock(hmem);
	CloseClipboard();

	return pmch;
}

LPWSTR EditGetClipboardTextW(void) {
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT) || !OpenClipboard(hwndMain)) {
		return NULL;
	}

	HANDLE hmem = GetClipboardData(CF_UNICODETEXT);
	LPCWSTR pwch = (LPCWSTR)GlobalLock(hmem);
	const int wlen = lstrlen(pwch);
	LPWSTR ptmp = (LPWSTR)NP2HeapAlloc((2*wlen + 1)*sizeof(WCHAR));

	if (pwch && ptmp) {
		LPCWSTR s = pwch;
		LPWSTR d = ptmp;

		const int iEOLMode = SciCall_GetEOLMode();
		for (int i = 0; (i < wlen) && (*s != 0); i++) {
			if (*s == '\n' || *s == '\r') {
				if (iEOLMode == SC_EOL_CR) {
					*d++ = '\r';
				} else if (iEOLMode == SC_EOL_LF) {
					*d++ = '\n';
				} else { // iEOLMode == SC_EOL_CRLF
					*d++ = '\r';
					*d++ = '\n';
				}
				if ((*s == '\r') && (i + 1 < wlen) && (*(s + 1) == '\n')) {
					i++;
					s++;
				}
				s++;
			} else {
				*d++ = *s++;
			}
		}

		*d++ = 0;
	}

	GlobalUnlock(hmem);
	CloseClipboard();

	return ptmp;
}

//=============================================================================
//
// EditCopyAppend()
//
BOOL EditCopyAppend(HWND hwnd) {
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		SciCall_Copy(FALSE);
		return TRUE;
	}

	char *pszText;
	if (!SciCall_IsSelectionEmpty()) {
		if (SciCall_IsRectangleSelection()) {
			NotifyRectangleSelection();
			return FALSE;
		}

		const Sci_Position iSelCount = SciCall_GetSelTextLength();
		pszText = (char *)NP2HeapAlloc(iSelCount);
		SciCall_GetSelText(pszText);
	} else {
		const Sci_Position cchText = SciCall_GetLength();
		pszText = (char *)NP2HeapAlloc(cchText + 1);
		SciCall_GetText(NP2HeapSize(pszText), pszText);
	}

	const UINT cpEdit = SciCall_GetCodePage();
	const int cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, -1, NULL, 0);

	WCHAR *pszTextW = NULL;
	if (cchTextW > 0) {
		const WCHAR *pszSep = L"\r\n\r\n";
		pszTextW = (WCHAR *)NP2HeapAlloc(sizeof(WCHAR) * (CSTRLEN(L"\r\n\r\n") + cchTextW + 1));
		lstrcpy(pszTextW, pszSep);
		MultiByteToWideChar(cpEdit, 0, pszText, -1, StrEnd(pszTextW), (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));
	}

	NP2HeapFree(pszText);

	BOOL succ = FALSE;
	if (OpenClipboard(GetParent(hwnd))) {
		HANDLE hOld = GetClipboardData(CF_UNICODETEXT);
		LPCWSTR pszOld = (LPCWSTR)GlobalLock(hOld);

		HANDLE hNew = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
						   sizeof(WCHAR) * (lstrlen(pszOld) + lstrlen(pszTextW) + 1));
		WCHAR *pszNew = (WCHAR *)GlobalLock(hNew);

		lstrcpy(pszNew, pszOld);
		lstrcat(pszNew, pszTextW);

		GlobalUnlock(hOld);
		GlobalUnlock(hNew);

		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hNew);
		CloseClipboard();

		succ = TRUE;
	}

	if (pszTextW != NULL) {
		NP2HeapFree(pszTextW);
	}
	return succ;
}

// https://docs.microsoft.com/en-us/cpp/intrinsics/popcnt16-popcnt-popcnt64
// use __popcnt() or _mm_popcnt_u32() require testing __cpuid():
/*
* int cpuInfo[4];
* __cpuid(cpuInfo, 0x00000001);
* const BOOL cpuPOPCNT = cpuInfo[2] & (1 << 23);
*/
// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
// Bit Twiddling Hacks copyright 1997-2005 Sean Eron Anderson
#if !defined(__clang__) && !defined(__GNUC__)
static __forceinline unsigned int bth_popcount(unsigned int v) {
	v = v - ((v >> 1) & 0x55555555U);
	v = (v & 0x33333333U) + ((v >> 2) & 0x33333333U);
	return (((v + (v >> 4)) & 0x0F0F0F0FU) * 0x01010101U) >> 24;
}
#endif
#if defined(__clang__) || defined(__GNUC__)
#define np2_popcount	__builtin_popcount
#elif NP2_USE_AVX2
#define np2_popcount	_mm_popcnt_u32
#else
//#define np2_popcount	__popcnt
#define np2_popcount	bth_popcount
#endif

//=============================================================================
//
// EditDetectEOLMode()
//
void EditDetectEOLMode(LPCSTR lpData, DWORD cbData, EditFileIOStatus *status) {
	if (cbData == 0) {
		return;
	}

	/* '\r' and '\n' is not reused (e.g. as trailing byte in DBCS) by any known encoding,
	it's safe to check whole data byte by byte.*/

	Sci_Line lineCountCRLF = 0;
	Sci_Line lineCountCR = 0;
	Sci_Line lineCountLF = 0;
#if 0
	StopWatch watch;
	StopWatch_Start(watch);
#endif

	// tools/GenerateTable.py
	static const uint8_t eol_table[16] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, // 00 - 0F
	};

	const uint8_t *ptr = (const uint8_t *)lpData;
	// No NULL-terminated requirement for *ptr == '\n'
	const uint8_t * const end = ptr + cbData - 1;

#if NP2_USE_AVX2
	#define LAST_CR_MASK	(1U << (sizeof(__m256i) - 1))
	const __m256i vectCR = _mm256_set1_epi8('\r');
	const __m256i vectLF = _mm256_set1_epi8('\n');
	while (ptr + sizeof(__m256i) <= end) {
		// unaligned loading: line starts at random position.
		const __m256i chunk = _mm256_loadu_si256((__m256i *)ptr);
		uint32_t maskCR = _mm256_movemask_epi8(_mm256_cmpeq_epi8(chunk, vectCR));
		uint32_t maskLF = _mm256_movemask_epi8(_mm256_cmpeq_epi8(chunk, vectLF));

		ptr += sizeof(__m256i);
		if (maskCR) {
			if (maskCR & LAST_CR_MASK) {
				maskCR &= LAST_CR_MASK - 1;
				if (*ptr == '\n') {
					// CR+LF across boundary
					++ptr;
					++lineCountCRLF;
				} else {
					// clear highest bit (last CR) to avoid using following code:
					// maskCR = (maskCR_LF ^ maskLF) | (maskCR & LAST_CR_MASK);
					++lineCountCR;
				}
			}

			// maskCR and maskLF never have some bit set. after shifting maskCR by 1 bit,
			// the bits both set in maskCR and maskLF represents CR+LF;
			// the bits only set in maskCR or maskLF represents individual CR or LF.
			const uint32_t maskCRLF = (maskCR << 1) & maskLF; // CR+LF
			const uint32_t maskCR_LF = (maskCR << 1) ^ maskLF;// CR alone or LF alone
			maskLF = maskCR_LF & maskLF; // LF alone
			maskCR = maskCR_LF ^ maskLF; // CR alone (with one position offset)
			if (maskCRLF) {
				lineCountCRLF += np2_popcount(maskCRLF);
			}
			if (maskCR) {
				lineCountCR += np2_popcount(maskCR);
			}
		}
		if (maskLF) {
			lineCountLF += np2_popcount(maskLF);
		}
	}

	#undef LAST_CR_MASK
	// end NP2_USE_AVX2
#elif NP2_USE_SSE2
	#define LAST_CR_MASK	(1U << (2*sizeof(__m128i) - 1))
	const __m128i vectCR = _mm_set1_epi8('\r');
	const __m128i vectLF = _mm_set1_epi8('\n');
	while (ptr + 2*sizeof(__m128i) <= end) {
		// unaligned loading: line starts at random position.
		__m128i chunk = _mm_loadu_si128((__m128i *)ptr);
		uint32_t maskCR = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk, vectCR));
		uint32_t maskLF = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk, vectLF));
		chunk = _mm_loadu_si128((__m128i *)(ptr + sizeof(__m128i)));
		maskCR |= ((uint32_t)_mm_movemask_epi8(_mm_cmpeq_epi8(chunk, vectCR))) << sizeof(__m128i);
		maskLF |= ((uint32_t)_mm_movemask_epi8(_mm_cmpeq_epi8(chunk, vectLF))) << sizeof(__m128i);

		ptr += 2*sizeof(__m128i);
		if (maskCR) {
			if (maskCR & LAST_CR_MASK) {
				maskCR &= LAST_CR_MASK - 1;
				if (*ptr == '\n') {
					// CR+LF across boundary
					++ptr;
					++lineCountCRLF;
				} else {
					// clear highest bit (last CR) to avoid using following code:
					// maskCR = (maskCR_LF ^ maskLF) | (maskCR & LAST_CR_MASK);
					++lineCountCR;
				}
			}

			// maskCR and maskLF never have some bit set. after shifting maskCR by 1 bit,
			// the bits both set in maskCR and maskLF represents CR+LF;
			// the bits only set in maskCR or maskLF represents individual CR or LF.
			const uint32_t maskCRLF = (maskCR << 1) & maskLF; // CR+LF
			const uint32_t maskCR_LF = (maskCR << 1) ^ maskLF;// CR alone or LF alone
			maskLF = maskCR_LF & maskLF; // LF alone
			maskCR = maskCR_LF ^ maskLF; // CR alone (with one position offset)
			if (maskCRLF) {
				lineCountCRLF += np2_popcount(maskCRLF);
			}
			if (maskCR) {
				lineCountCR += np2_popcount(maskCR);
			}
		}
		if (maskLF) {
			lineCountLF += np2_popcount(maskLF);
		}
	}

	#undef LAST_CR_MASK
	// end NP2_USE_SSE2
#endif

	do {
		// skip to line end
		uint8_t ch;
		uint8_t type = 0;
		while (ptr < end && ((ch = *ptr++) > '\r' || (type = eol_table[ch]) == 0)) {
			// nop
		}
		switch (type) {
		case 1: //'\n'
			++lineCountLF;
			break;
		case 2: //'\r'
			if (*ptr == '\n') {
				++ptr;
				++lineCountCRLF;
			} else {
				++lineCountCR;
			}
			break;
		}
	} while (ptr < end);

	if (ptr == end) {
		switch (*ptr) {
		case '\n':
			++lineCountLF;
			break;
		case '\r':
			++lineCountCR;
			break;
		}
	}

	const Sci_Line linesMax = max_pos(max_pos(lineCountCRLF, lineCountCR), lineCountLF);
	// values must kept in same order as SC_EOL_CRLF, SC_EOL_CR, SC_EOL_LF
	const Sci_Line linesCount[3] = { lineCountCRLF, lineCountCR, lineCountLF };
	int iEOLMode = status->iEOLMode;
	if (linesMax != linesCount[iEOLMode]) {
		if (linesMax == lineCountCRLF) {
			iEOLMode = SC_EOL_CRLF;
		} else if (linesMax == lineCountLF) {
			iEOLMode = SC_EOL_LF;
		} else {
			iEOLMode = SC_EOL_CR;
		}
	}

#if 0
	StopWatch_Stop(watch);
	StopWatch_ShowLog(&watch, "EOL time");
#if defined(_WIN64)
	printf("%s CR+LF:%" PRId64 ", LF: %" PRId64 ", CR: %" PRId64 "\n", __func__, lineCountCRLF, lineCountLF, lineCountCR);
#else
	printf("%s CR+LF:%d, LF: %d, CR: %d\n", __func__, lineCountCRLF, lineCountLF, lineCountCR);
#endif
#endif

	status->iEOLMode = iEOLMode;
	status->bInconsistent = ((!!lineCountCRLF) + (!!lineCountCR) + (!!lineCountLF)) > 1;
	status->totalLineCount = lineCountCRLF + lineCountCR + lineCountLF + 1;
	status->linesCount[0] = lineCountCRLF;
	status->linesCount[1] = lineCountLF;
	status->linesCount[2] = lineCountCR;
}

//=============================================================================
//
// EditLoadFile()
//
BOOL EditLoadFile(LPWSTR pszFile, BOOL bSkipEncodingDetection, EditFileIOStatus *status) {
	HANDLE hFile = CreateFile(pszFile,
					   GENERIC_READ,
					   FILE_SHARE_READ | FILE_SHARE_WRITE,
					   NULL, OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);
	dwLastIOError = GetLastError();

	if (hFile == INVALID_HANDLE_VALUE) {
		iSrcEncoding = -1;
		iWeakSrcEncoding = -1;
		return FALSE;
	}

	LARGE_INTEGER fileSize;
	fileSize.QuadPart = 0;
	if (!GetFileSizeEx(hFile, &fileSize)) {
		dwLastIOError = GetLastError();
		CloseHandle(hFile);
		iSrcEncoding = -1;
		iWeakSrcEncoding = -1;
		return FALSE;
	}

	// display real path name
	if (IsVistaAndAbove()) {
		WCHAR path[MAX_PATH] = L"";
		// since Windows Vista
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
		if (GetFinalPathNameByHandleW(hFile, path, MAX_PATH, FILE_NAME_OPENED))
#else
		typedef DWORD (WINAPI *GetFinalPathNameByHandleSig)(HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
		GetFinalPathNameByHandleSig pfnGetFinalPathNameByHandle = (GetFinalPathNameByHandleSig)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetFinalPathNameByHandleW");
		if (pfnGetFinalPathNameByHandle && pfnGetFinalPathNameByHandle(hFile, path, MAX_PATH, FILE_NAME_OPENED))
#endif
		{
			if (StrNEqual(path, L"\\\\?\\", CSTRLEN(L"\\\\?\\"))) {
				WCHAR *p = path + 4;
				if (StrNEqual(p, L"UNC\\", CSTRLEN(L"UNC\\"))) {
					p += 2;
					*p = L'\\';
				}
				lstrcpy(pszFile, p);
			}
		}
	}

	// Check if a warning message should be displayed for large files
#if defined(_WIN64)
	// less than 1/3 available physical memory:
	//     1. The buffers we allocated below or when saving file, depends on encoding.
	//     2. Scintilla's content buffer and style buffer, see CellBuffer class. The style buffer can be disabled by using SCLEX_NULL and SC_DOCUMENTOPTION_STYLES_NONE.
	//     3. Extra memory when moving gaps on editing, it may requires more than 2/3 physical memory.
	// large file TODO: https://github.com/zufuliu/notepad2/issues/125
	// [ ] [> 4 GiB] use SetFilePointerEx() and ReadFile()/WriteFile() to read/write file.
	// [-] [> 2 GiB] fix encoding conversion with MultiByteToWideChar() and WideCharToMultiByte().
	LONGLONG maxFileSize = INT64_C(0x100000000);
#else
	// 2 GiB: ptrdiff_t / Sci_Position used in Scintilla
	LONGLONG maxFileSize = INT64_C(0x80000000);
#endif

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if (GlobalMemoryStatusEx(&statex)) {
		ULONGLONG maxMem = statex.ullTotalPhys/3U;
		if (maxMem < (ULONGLONG)maxFileSize) {
			maxFileSize = (LONGLONG)maxMem;
		}
	} else {
		dwLastIOError = GetLastError();
	}

	if (fileSize.QuadPart > maxFileSize) {
		CloseHandle(hFile);
		status->bFileTooBig = TRUE;
		iSrcEncoding = -1;
		iWeakSrcEncoding = -1;
		WCHAR tchDocSize[32];
		WCHAR tchMaxSize[32];
		WCHAR tchDocBytes[32];
		WCHAR tchMaxBytes[32];
		StrFormatByteSize(fileSize.QuadPart, tchDocSize, COUNTOF(tchDocSize));
		StrFormatByteSize(maxFileSize, tchMaxSize, COUNTOF(tchMaxSize));
		_i64tow(fileSize.QuadPart, tchDocBytes, 10);
		_i64tow(maxFileSize, tchMaxBytes, 10);
		FormatNumberStr(tchDocBytes);
		FormatNumberStr(tchMaxBytes);
		MsgBox(MBWARN, IDS_WARNLOADBIGFILE, pszFile, tchDocSize, tchDocBytes, tchMaxSize, tchMaxBytes);
		return FALSE;
	}

	char *lpData = (char *)NP2HeapAlloc((SIZE_T)(fileSize.QuadPart) + 16);
	DWORD cbData = 0;
	// prevent unsigned integer overflow.
	const DWORD readLen = max_u((DWORD)(NP2HeapSize(lpData) - 2), (DWORD)fileSize.QuadPart);
	const BOOL bReadSuccess = ReadFile(hFile, lpData, readLen, &cbData, NULL);
	dwLastIOError = GetLastError();
	CloseHandle(hFile);

	if (!bReadSuccess) {
		NP2HeapFree(lpData);
		iSrcEncoding = -1;
		iWeakSrcEncoding = -1;
		return FALSE;
	}

	BOOL bPreferOEM = FALSE;
	if (bLoadNFOasOEM) {
		LPCWSTR const pszExt = pszFile + lstrlen(pszFile) - 4;
		if (pszExt >= pszFile && (StrCaseEqual(pszExt, L".nfo") || StrCaseEqual(pszExt, L".diz"))) {
			bPreferOEM = TRUE;
		}
	}

	if (!Encoding_IsValid(iDefaultEncoding)) {
		iDefaultEncoding = CPI_UTF8;
	}

	int _iDefaultEncoding = bPreferOEM ? g_DOSEncoding : iDefaultEncoding;
	if (iWeakSrcEncoding != -1 && Encoding_IsValid(iWeakSrcEncoding)) {
		_iDefaultEncoding = iWeakSrcEncoding;
	}

	int iEncoding = CPI_DEFAULT;
	const BOOL utf8Sig = cbData? IsUTF8Signature(lpData) : FALSE;
	BOOL bBOM = FALSE;
	BOOL bReverse = FALSE;

	status->iEOLMode = iLineEndings[iDefaultEOLMode];
	status->bInconsistent = FALSE;
	status->totalLineCount = 1;

	if (cbData == 0) {
		FileVars_Init(NULL, 0, &fvCurFile);

		if (iSrcEncoding == -1) {
			if (bLoadANSIasUTF8 && !bPreferOEM) {
				iEncoding = CPI_UTF8;
			} else {
				iEncoding = _iDefaultEncoding;
			}
		} else {
			iEncoding = iSrcEncoding;
		}

		SciCall_SetCodePage((mEncoding[iEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8);
		EditSetEmptyText();
		SciCall_SetEOLMode(status->iEOLMode);
	} else if (cbData < MAX_NON_UTF8_SIZE && ((iSrcEncoding == CPI_UNICODE || iSrcEncoding == CPI_UNICODEBE) // reload as UTF-16
		|| (!bSkipEncodingDetection && iSrcEncoding == -1 && !utf8Sig && IsUnicode(lpData, cbData, &bBOM, &bReverse))
		)) {
		if (iSrcEncoding == CPI_UNICODE) {
			bBOM = (lpData[0] == '\xFF' && lpData[1] == '\xFE');
			bReverse = FALSE;
		} else if (iSrcEncoding == CPI_UNICODEBE) {
			bBOM = (lpData[0] == '\xFE' && lpData[1] == '\xFF');
		}

		if (iSrcEncoding == CPI_UNICODEBE || bReverse) {
			_swab(lpData, lpData, cbData);
			if (bBOM) {
				iEncoding = CPI_UNICODEBEBOM;
			} else {
				iEncoding = CPI_UNICODEBE;
			}
		} else {
			if (bBOM) {
				iEncoding = CPI_UNICODEBOM;
			} else {
				iEncoding = CPI_UNICODE;
			}
		}

		// cbData/2 => WCHAR, WCHAR*3 => UTF-8
		char *lpDataUTF8 = (char *)NP2HeapAlloc((cbData + 1)*sizeof(WCHAR));
		cbData = WideCharToMultiByte(CP_UTF8, 0, (bBOM ? (LPWSTR)lpData + 1 : (LPWSTR)lpData),
									 (int)(bBOM ? (cbData / sizeof(WCHAR)) : (cbData / sizeof(WCHAR)) + 1),
									 lpDataUTF8, (int)NP2HeapSize(lpDataUTF8), NULL, NULL);
		if (cbData == 0) {
			cbData = WideCharToMultiByte(CP_ACP, 0, (bBOM ? (LPWSTR)lpData + 1 : (LPWSTR)lpData),
										 -1, lpDataUTF8, (int)NP2HeapSize(lpDataUTF8), NULL, NULL);
			status->bUnicodeErr = TRUE;
		}

		EditDetectEOLMode(lpDataUTF8, cbData - 1, status);
		FileVars_Init(lpDataUTF8, cbData - 1, &fvCurFile);
		SciCall_SetCodePage(SC_CP_UTF8);
		EditSetNewText(lpDataUTF8, cbData - 1, status->totalLineCount);
		NP2HeapFree(lpDataUTF8);
	} else {
		FileVars_Init(lpData, cbData, &fvCurFile);
		if (iSrcEncoding == -1) {
			iSrcEncoding = FileVars_GetEncoding(&fvCurFile);
		}
		if ((iSrcEncoding == CPI_UTF8 || iSrcEncoding == CPI_UTF8SIGN) // reload as UTF-8 or UTF-8 filevar
			|| ((iSrcEncoding == -1) && ((bLoadANSIasUTF8 && !bPreferOEM) // load ANSI as UTF-8
				|| ((!bSkipEncodingDetection || cbData >= MAX_NON_UTF8_SIZE) && (utf8Sig || IsUTF8(lpData, cbData)))
			))
		) {
			SciCall_SetCodePage(SC_CP_UTF8);
			if (utf8Sig) {
				EditDetectEOLMode(lpData + 3, cbData - 3, status);
				EditSetNewText(lpData + 3, cbData - 3, status->totalLineCount);
				iEncoding = CPI_UTF8SIGN;
			} else {
				EditDetectEOLMode(lpData, cbData, status);
				EditSetNewText(lpData, cbData, status->totalLineCount);
				iEncoding = CPI_UTF8;
			}
		} else {
			iEncoding = iSrcEncoding;
			if (iEncoding == -1) {
				if (fvCurFile.mask & FV_ENCODING) {
					iEncoding = CPI_DEFAULT;
				} else {
					if ((iWeakSrcEncoding != -1) && (mEncoding[iWeakSrcEncoding].uFlags & NCP_INTERNAL)) {
						iEncoding = iDefaultEncoding;
					} else {
						iEncoding = _iDefaultEncoding;
					}
				}
			}

			const UINT uCodePage = mEncoding[iEncoding].uCodePage;
			if (cbData < MAX_NON_UTF8_SIZE && ((mEncoding[iEncoding].uFlags & NCP_8BIT)
				|| ((mEncoding[iEncoding].uFlags & NCP_7BIT) && IsUTF7(lpData, cbData))
			)) {
				LPWSTR lpDataWide = (LPWSTR)NP2HeapAlloc(cbData * sizeof(WCHAR) + 16);
				const int cbDataWide = MultiByteToWideChar(uCodePage, 0, lpData, cbData, lpDataWide, (int)(NP2HeapSize(lpDataWide) / sizeof(WCHAR)));
				NP2HeapFree(lpData);
				lpData = (char *)NP2HeapAlloc(cbDataWide * kMaxMultiByteCount + 16);
				cbData = WideCharToMultiByte(CP_UTF8, 0, lpDataWide, cbDataWide, lpData, (int)NP2HeapSize(lpData), NULL, NULL);
				NP2HeapFree(lpDataWide);

				EditDetectEOLMode(lpData, cbData, status);
				SciCall_SetCodePage(SC_CP_UTF8);
				EditSetNewText(lpData, cbData, status->totalLineCount);
			} else {
				EditDetectEOLMode(lpData, cbData, status);
				SciCall_SetCodePage(iDefaultCodePage);
				EditSetNewText(lpData, cbData, status->totalLineCount);
				iEncoding = CPI_DEFAULT;
			}
		}
	}

	NP2HeapFree(lpData);
	status->iEncoding = iEncoding;
	iSrcEncoding = -1;
	iWeakSrcEncoding = -1;

	return TRUE;
}

//=============================================================================
//
// EditSaveFile()
//
BOOL EditSaveFile(HWND hwnd, LPCWSTR pszFile, BOOL bSaveCopy, EditFileIOStatus *status) {
	HANDLE hFile = CreateFile(pszFile,
					   GENERIC_WRITE,
					   FILE_SHARE_READ | FILE_SHARE_WRITE,
					   NULL, OPEN_ALWAYS,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);
	dwLastIOError = GetLastError();

	// failure could be due to missing attributes (2k/XP)
	if (hFile == INVALID_HANDLE_VALUE) {
		DWORD dwAttributes = GetFileAttributes(pszFile);
		if (dwAttributes != INVALID_FILE_ATTRIBUTES) {
			dwAttributes = dwAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
			hFile = CreateFile(pszFile,
							   GENERIC_WRITE,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_ALWAYS,
							   FILE_ATTRIBUTE_NORMAL | dwAttributes,
							   NULL);
			dwLastIOError = GetLastError();
		}
	}

	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// ensure consistent line endings
	if (bFixLineEndings) {
		EditEnsureConsistentLineEndings();
	}

	// strip trailing blanks
	if (bAutoStripBlanks) {
		EditStripTrailingBlanks(hwnd, TRUE);
	}

	BOOL bWriteSuccess;
	// get text
	DWORD cbData = (DWORD)SciCall_GetLength();
	char *lpData = NULL;

	if (cbData == 0) {
		bWriteSuccess = SetEndOfFile(hFile);
		dwLastIOError = GetLastError();
	} else {
		DWORD dwBytesWritten;
		int iEncoding = status->iEncoding;
		UINT uFlags = mEncoding[iEncoding].uFlags;
		if (cbData >= MAX_NON_UTF8_SIZE) {
			// save in UTF-8 or ANSI
			if (!(uFlags & (NCP_DEFAULT | NCP_UTF8))) {
				if (uFlags & NCP_UNICODE_BOM) {
					iEncoding = CPI_UTF8SIGN;
				} else {
					iEncoding = CPI_UTF8;
				}
				uFlags = mEncoding[iEncoding].uFlags;
			}
		}

		lpData = (char *)NP2HeapAlloc(cbData + 1);
		SciCall_GetText(NP2HeapSize(lpData), lpData);
		// FIXME: move checks in front of disk file access
		/*if ((uFlags & NCP_UNICODE) == 0 && (uFlags & NCP_UTF8_SIGN) == 0) {
				BOOL bEncodingMismatch = TRUE;
				FILEVARS fv;
				FileVars_Init(lpData, cbData, &fv);
				if (fv.mask & FV_ENCODING) {
					int iAltEncoding;
					if (FileVars_IsValidEncoding(&fv)) {
						iAltEncoding = FileVars_GetEncoding(&fv);
						if (iAltEncoding == iEncoding)
							bEncodingMismatch = FALSE;
						else if ((mEncoding[iAltEncoding].uFlags & NCP_UTF8) && (uFlags & NCP_UTF8))
							bEncodingMismatch = FALSE;
					}
					if (bEncodingMismatch) {
						Encoding_GetLabel(iAltEncoding);
						Encoding_GetLabel(iEncoding);
						InfoBox(0, L"MsgEncodingMismatch", IDS_ENCODINGMISMATCH,
							mEncoding[iAltEncoding].wchLabel,
							mEncoding[iEncoding].wchLabel);
					}
				}
			}*/
		if (uFlags & NCP_UNICODE) {
			SetEndOfFile(hFile);

			LPWSTR lpDataWide = (LPWSTR)NP2HeapAlloc(cbData * sizeof(WCHAR) + 16);
			const int cbDataWide = MultiByteToWideChar(CP_UTF8, 0, lpData, cbData, lpDataWide, (int)(NP2HeapSize(lpDataWide) / sizeof(WCHAR)));

			if (uFlags & NCP_UNICODE_BOM) {
				if (uFlags & NCP_UNICODE_REVERSE) {
					WriteFile(hFile, (LPCVOID)"\xFE\xFF", 2, &dwBytesWritten, NULL);
				} else {
					WriteFile(hFile, (LPCVOID)"\xFF\xFE", 2, &dwBytesWritten, NULL);
				}
			}

			if (uFlags & NCP_UNICODE_REVERSE) {
				_swab((char *)lpDataWide, (char *)lpDataWide, (int)(cbDataWide * sizeof(WCHAR)));
			}

			bWriteSuccess = WriteFile(hFile, lpDataWide, cbDataWide * sizeof(WCHAR), &dwBytesWritten, NULL);
			dwLastIOError = GetLastError();

			NP2HeapFree(lpDataWide);
		} else if (uFlags & NCP_UTF8) {
			SetEndOfFile(hFile);

			if (uFlags & NCP_UTF8_SIGN) {
				WriteFile(hFile, (LPCVOID)"\xEF\xBB\xBF", 3, &dwBytesWritten, NULL);
			}

			bWriteSuccess = WriteFile(hFile, lpData, cbData, &dwBytesWritten, NULL);
			dwLastIOError = GetLastError();
		} else if (uFlags & (NCP_8BIT | NCP_7BIT)) {
			BOOL bCancelDataLoss = FALSE;
			const UINT uCodePage = mEncoding[iEncoding].uCodePage;
			const BOOL zeroFlags = IsZeroFlagsCodePage(uCodePage);

			LPWSTR lpDataWide = (LPWSTR)NP2HeapAlloc(cbData * sizeof(WCHAR) + 16);
			const int cbDataWide = MultiByteToWideChar(CP_UTF8, 0, lpData, cbData, lpDataWide, (int)(NP2HeapSize(lpDataWide) / sizeof(WCHAR)));

			if (zeroFlags) {
				NP2HeapFree(lpData);
				lpData = (char *)NP2HeapAlloc(NP2HeapSize(lpDataWide) * 2);
			} else {
				ZeroMemory(lpData, NP2HeapSize(lpData));
			}

			if (zeroFlags) {
				cbData = WideCharToMultiByte(uCodePage, 0, lpDataWide, cbDataWide, lpData, (int)NP2HeapSize(lpData), NULL, NULL);
			} else {
				cbData = WideCharToMultiByte(uCodePage, WC_NO_BEST_FIT_CHARS, lpDataWide, cbDataWide, lpData, (int)NP2HeapSize(lpData), NULL, &bCancelDataLoss);
				if (!bCancelDataLoss) {
					cbData = WideCharToMultiByte(uCodePage, 0, lpDataWide, cbDataWide, lpData, (int)NP2HeapSize(lpData), NULL, NULL);
					bCancelDataLoss = FALSE;
				}
			}
			NP2HeapFree(lpDataWide);

			if (!bCancelDataLoss || InfoBox(MBOKCANCEL, L"MsgConv3", IDS_ERR_UNICODE2) == IDOK) {
				SetEndOfFile(hFile);
				bWriteSuccess = WriteFile(hFile, lpData, cbData, &dwBytesWritten, NULL);
				dwLastIOError = GetLastError();
			} else {
				bWriteSuccess = FALSE;
				status->bCancelDataLoss = TRUE;
			}
		} else {
			SetEndOfFile(hFile);
			bWriteSuccess = WriteFile(hFile, lpData, cbData, &dwBytesWritten, NULL);
			dwLastIOError = GetLastError();
		}
	}

	if (lpData != NULL) {
		NP2HeapFree(lpData);
	}

	CloseHandle(hFile);
	if (bWriteSuccess) {
		if (!bSaveCopy) {
			SciCall_SetSavePoint();
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditInvertCase()
//
void EditInvertCase(void) {
	const Sci_Position iCurPos = SciCall_GetCurrentPos();
	const Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Position iSelCount = SciCall_GetSelTextLength() - 1;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

	SciCall_GetSelText(pszText);
	const UINT cpEdit = SciCall_GetCodePage();
	const int cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));

	BOOL bChanged = FALSE;
	for (int i = 0; i < cchTextW; i++) {
		if (IsCharUpper(pszTextW[i])) {
			pszTextW[i] = LOWORD(CharLower((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
			bChanged = TRUE;
		} else if (IsCharLower(pszTextW[i])) {
			pszTextW[i] = LOWORD(CharUpper((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
			bChanged = TRUE;
		}
	}

	if (bChanged) {
		WideCharToMultiByte(cpEdit, 0, pszTextW, cchTextW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

		SciCall_BeginUndoAction();
		SciCall_Clear();
		SciCall_AddText(iSelCount, pszText);
		SciCall_SetSel(iAnchorPos, iCurPos);
		SciCall_EndUndoAction();
	}

	NP2HeapFree(pszText);
	NP2HeapFree(pszTextW);
}

//=============================================================================
//
// EditTitleCase()
//
void EditTitleCase(void) {
	const Sci_Position iCurPos = SciCall_GetCurrentPos();
	const Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Position iSelCount = SciCall_GetSelTextLength() - 1;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

	SciCall_GetSelText(pszText);
	const UINT cpEdit = SciCall_GetCodePage();
	const int cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));

	BOOL bChanged = FALSE;
	if (IsWin7AndAbove()) {
		LPWSTR pszMappedW = (LPWSTR)NP2HeapAlloc(NP2HeapSize(pszTextW));
		if (LCMapString(LOCALE_SYSTEM_DEFAULT,
						LCMAP_LINGUISTIC_CASING | LCMAP_TITLECASE,
						pszTextW, cchTextW, pszMappedW, (int)(NP2HeapSize(pszMappedW) / sizeof(WCHAR)))) {
			lstrcpyn(pszTextW, pszMappedW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));
			bChanged = TRUE;
		} else {
			bChanged = FALSE;
		}
		NP2HeapFree(pszMappedW);
	} else {
#if 1 // BOOKMARK_EDITION
		//Slightly enhanced function to make Title Case:
		//Added some '-characters and bPrevWasSpace makes it better (for example "'Don't'" will now work)
		BOOL bNewWord = TRUE;
		BOOL bPrevWasSpace = TRUE;
		for (int i = 0; i < cchTextW; i++) {
			if (!IsCharAlphaNumeric(pszTextW[i]) && (!StrChr(L"\x0027\x0060\x0384\x2019", pszTextW[i]) ||	bPrevWasSpace)) {
				bNewWord = TRUE;
			} else {
				if (bNewWord) {
					if (IsCharLower(pszTextW[i])) {
						pszTextW[i] = LOWORD(CharUpper((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
						bChanged = TRUE;
					}
				} else {
					if (IsCharUpper(pszTextW[i])) {
						pszTextW[i] = LOWORD(CharLower((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
						bChanged = TRUE;
					}
				}
				bNewWord = FALSE;
			}

			if (StrChr(L" \r\n\t[](){}", pszTextW[i])) {
				bPrevWasSpace = TRUE;
			} else {
				bPrevWasSpace = FALSE;
			}
		}
#else
		BOOL bNewWord = TRUE;
		BOOL bWordEnd = TRUE;
		for (int i = 0; i < cchTextW; i++) {
			const BOOL bAlphaNumeric = IsCharAlphaNumeric(pszTextW[i]);
			if (!bAlphaNumeric && (!StrChr(L"\x0027\x2019\x0060\x00B4", pszTextW[i]) || bWordEnd)) {
				bNewWord = TRUE;
			} else {
				if (bNewWord) {
					if (IsCharLower(pszTextW[i])) {
						pszTextW[i] = LOWORD(CharUpper((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
						bChanged = TRUE;
					}
				} else {
					if (IsCharUpper(pszTextW[i])) {
						pszTextW[i] = LOWORD(CharLower((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
						bChanged = TRUE;
					}
				}
				bNewWord = FALSE;
			}
			bWordEnd = !bAlphaNumeric;
		}
#endif
	}

	if (bChanged) {
		WideCharToMultiByte(cpEdit, 0, pszTextW, cchTextW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

		SciCall_BeginUndoAction();
		SciCall_Clear();
		SciCall_AddText(iSelCount, pszText);
		SciCall_SetSel(iAnchorPos, iCurPos);
		SciCall_EndUndoAction();
	}

	NP2HeapFree(pszText);
	NP2HeapFree(pszTextW);
}

//=============================================================================
//
// EditSentenceCase()
//
void EditSentenceCase(void) {
	const Sci_Position iCurPos = SciCall_GetCurrentPos();
	const Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Position iSelCount = SciCall_GetSelTextLength() - 1;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

	SciCall_GetSelText(pszText);
	const UINT cpEdit = SciCall_GetCodePage();
	const int cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));

	BOOL bNewSentence = TRUE;
	BOOL bChanged = FALSE;
	for (int i = 0; i < cchTextW; i++) {
		if (StrChr(L".;!?\r\n", pszTextW[i])) {
			bNewSentence = TRUE;
		} else {
			if (IsCharAlphaNumeric(pszTextW[i])) {
				if (bNewSentence) {
					if (IsCharLower(pszTextW[i])) {
						pszTextW[i] = LOWORD(CharUpper((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
						bChanged = TRUE;
					}
					bNewSentence = FALSE;
				} else {
					if (IsCharUpper(pszTextW[i])) {
						pszTextW[i] = LOWORD(CharLower((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
						bChanged = TRUE;
					}
				}
			}
		}
	}

	if (bChanged) {
		WideCharToMultiByte(cpEdit, 0, pszTextW, cchTextW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

		SciCall_BeginUndoAction();
		SciCall_Clear();
		SciCall_AddText(iSelCount, pszText);
		SciCall_SetSel(iAnchorPos, iCurPos);
		SciCall_EndUndoAction();
	}

	NP2HeapFree(pszText);
	NP2HeapFree(pszTextW);
}

//=============================================================================
//
// EditURLEncode()
//
LPWSTR EditURLEncodeSelection(int *pcchEscaped, BOOL bTrim) {
	*pcchEscaped = 0;
	const Sci_Position iSelCount = SciCall_GetSelTextLength();
	if (iSelCount <= 1) {
		return NULL;
	}

	char *pszText = (char *)NP2HeapAlloc(iSelCount);
	SciCall_GetSelText(pszText);

	if (bTrim) {
		StrTrimA(pszText, " \t\r\n");
	}
	if (StrIsEmptyA(pszText)) {
		NP2HeapFree(pszText);
		return NULL;
	}

	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc(iSelCount * sizeof(WCHAR));
	const UINT cpEdit = SciCall_GetCodePage();
	MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));

	// https://docs.microsoft.com/en-us/windows/desktop/api/shlwapi/nf-shlwapi-urlescapew
	LPWSTR pszEscapedW = (LPWSTR)NP2HeapAlloc(NP2HeapSize(pszTextW) * 3);  // '&', H1, H0

	DWORD cchEscapedW = (int)NP2HeapSize(pszEscapedW) / sizeof(WCHAR);
	UrlEscape(pszTextW, pszEscapedW, &cchEscapedW, URL_ESCAPE_SEGMENT_ONLY);

	NP2HeapFree(pszText);
	NP2HeapFree(pszTextW);
	*pcchEscaped = cchEscapedW;
	return pszEscapedW;
}

void EditURLEncode(void) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	int cchEscapedW;
	LPWSTR pszEscapedW = EditURLEncodeSelection(&cchEscapedW, FALSE);
	if (pszEscapedW == NULL) {
		return;
	}

	const UINT cpEdit = SciCall_GetCodePage();
	char *pszEscaped = (char *)NP2HeapAlloc(cchEscapedW * kMaxMultiByteCount);
	const int cchEscaped = WideCharToMultiByte(cpEdit, 0, pszEscapedW, cchEscapedW, pszEscaped, (int)NP2HeapSize(pszEscaped), NULL, NULL);
	if (iCurPos < iAnchorPos) {
		iAnchorPos = iCurPos + cchEscaped;
	} else {
		iCurPos = iAnchorPos + cchEscaped;
	}

	SciCall_BeginUndoAction();
	SciCall_Clear();
	SciCall_AddText(cchEscaped, pszEscaped);
	SciCall_SetSel(iAnchorPos, iCurPos);
	SciCall_EndUndoAction();

	NP2HeapFree(pszEscaped);
	NP2HeapFree(pszEscapedW);
}

//=============================================================================
//
// EditURLDecode()
//
void EditURLDecode(void) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Position iSelCount = SciCall_GetSelTextLength();

	char *pszText = (char *)NP2HeapAlloc(iSelCount);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc(iSelCount * sizeof(WCHAR));

	SciCall_GetSelText(pszText);
	const UINT cpEdit = SciCall_GetCodePage();
	MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));

	char *pszUnescaped = (char *)NP2HeapAlloc(NP2HeapSize(pszText) * 3);
	LPWSTR pszUnescapedW = (LPWSTR)NP2HeapAlloc(NP2HeapSize(pszTextW) * 3);

	DWORD cchUnescapedW = (DWORD)(NP2HeapSize(pszUnescapedW) / sizeof(WCHAR));
	UrlUnescape(pszTextW, pszUnescapedW, &cchUnescapedW, 0);
	const int cchUnescaped = WideCharToMultiByte(cpEdit, 0, pszUnescapedW, cchUnescapedW, pszUnescaped, (int)NP2HeapSize(pszUnescaped), NULL, NULL);

	if (iCurPos < iAnchorPos) {
		iAnchorPos = iCurPos + cchUnescaped;
	} else {
		iCurPos = iAnchorPos + cchUnescaped;
	}

	SciCall_BeginUndoAction();
	SciCall_Clear();
	SciCall_AddText(cchUnescaped, pszUnescaped);
	SciCall_SetSel(iAnchorPos, iCurPos);
	SciCall_EndUndoAction();

	NP2HeapFree(pszText);
	NP2HeapFree(pszTextW);
	NP2HeapFree(pszUnescaped);
	NP2HeapFree(pszUnescapedW);
}

//=============================================================================
//
// EditEscapeCChars()
//
void EditEscapeCChars(HWND hwnd) {
	if (SciCall_IsSelectionEmpty()) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

#if NP2_USE_DESIGNATED_INITIALIZER
	EDITFINDREPLACE efr = {
		.hwnd = hwnd,
	};
#else
	EDITFINDREPLACE efr = { "", "", "", "", hwnd };
#endif

	SciCall_BeginUndoAction();

	strcpy(efr.szFind, "\\");
	strcpy(efr.szReplace, "\\\\");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "\"");
	strcpy(efr.szReplace, "\\\"");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "\'");
	strcpy(efr.szReplace, "\\\'");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditUnescapeCChars()
//
void EditUnescapeCChars(HWND hwnd) {
	if (SciCall_IsSelectionEmpty()) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

#if NP2_USE_DESIGNATED_INITIALIZER
	EDITFINDREPLACE efr = {
		.hwnd = hwnd,
	};
#else
	EDITFINDREPLACE efr = { "", "", "", "", hwnd };
#endif

	SciCall_BeginUndoAction();

	strcpy(efr.szFind, "\\\\");
	strcpy(efr.szReplace, "\\");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "\\\"");
	strcpy(efr.szReplace, "\"");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "\\\'");
	strcpy(efr.szReplace, "\'");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	SciCall_EndUndoAction();
}

// XML/HTML predefined entity
// https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
// &quot;	["]
// &amp;	[&]
// &apos;	[']
// &lt;		[<]
// &gt;		[>]
// &nbsp;	[ ]
// &emsp;	[\t]
//=============================================================================
//
// EditEscapeXHTMLChars()
//
void EditEscapeXHTMLChars(HWND hwnd) {
	if (SciCall_IsSelectionEmpty()) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

#if NP2_USE_DESIGNATED_INITIALIZER
	EDITFINDREPLACE efr = {
		.hwnd = hwnd,
	};
#else
	EDITFINDREPLACE efr = { "", "", "", "", hwnd };
#endif

	SciCall_BeginUndoAction();

	strcpy(efr.szFind, "&");
	strcpy(efr.szReplace, "&amp;");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "\"");
	strcpy(efr.szReplace, "&quot;");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "\'");
	strcpy(efr.szReplace, "&apos;");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "<");
	strcpy(efr.szReplace, "&lt;");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, ">");
	strcpy(efr.szReplace, "&gt;");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	if (pLexCurrent->iLexer != SCLEX_XML) {
		strcpy(efr.szFind, " ");
		strcpy(efr.szReplace, "&nbsp;");
		EditReplaceAllInSelection(hwnd, &efr, FALSE);

		strcpy(efr.szFind, "\t");
		strcpy(efr.szReplace, "&emsp;");
		EditReplaceAllInSelection(hwnd, &efr, FALSE);
	}

	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditUnescapeXHTMLChars()
//
void EditUnescapeXHTMLChars(HWND hwnd) {
	if (SciCall_IsSelectionEmpty()) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

#if NP2_USE_DESIGNATED_INITIALIZER
	EDITFINDREPLACE efr = {
		.hwnd = hwnd,
	};
#else
	EDITFINDREPLACE efr = { "", "", "", "", hwnd };
#endif

	SciCall_BeginUndoAction();

	strcpy(efr.szFind, "&quot;");
	strcpy(efr.szReplace, "\"");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "&apos;");
	strcpy(efr.szReplace, "\'");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "&lt;");
	strcpy(efr.szReplace, "<");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "&gt;");
	strcpy(efr.szReplace, ">");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "&nbsp;");
	strcpy(efr.szReplace, " ");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "&amp;");
	strcpy(efr.szReplace, "&");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);

	strcpy(efr.szFind, "&emsp;");
	strcpy(efr.szReplace, "\t");
	EditReplaceAllInSelection(hwnd, &efr, FALSE);
	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditChar2Hex()
//

/*				C/C++	C#	Java	JS	JSON	Python	PHP	Lua		Go
\ooo		3	1			1					1		1	1/ddd	1
\xHH		2	1		1			1			1		1			1
\uHHHH		4	1			1		1	1		1					1
\UHHHHHHHH	8	1								1					1
\xHHHH		4			1
\uHHHHHH	6				1
*/
#define MAX_ESCAPE_HEX_DIGIT	4

void EditChar2Hex(void) {
	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	Sci_Position count = SciCall_GetSelTextLength() - 1;
	if (count == 0) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	count *= 2 + MAX_ESCAPE_HEX_DIGIT;
	char *ch = (char *)NP2HeapAlloc(count + 1);
	WCHAR *wch = (WCHAR *)NP2HeapAlloc((count + 1) * sizeof(WCHAR));
	SciCall_GetSelText(ch);

	if (ch[0] == 0) {
		strcpy(ch, "\\x00");
	} else {
		const UINT cpEdit = SciCall_GetCodePage();
		char uesc = 'u';
		if (pLexCurrent->rid == NP2LEX_CSHARP) {
			uesc = 'x';
		}
		count = MultiByteToWideChar(cpEdit, 0, ch, -1, wch, (int)(count + 1)) - 1; // '\0'
		for (Sci_Position i = 0, j = 0; i < count; i++) {
			if (wch[i] <= 0xFF) {
				sprintf(ch + j, "\\x%02X", wch[i] & 0xFF); // \xhh
				j += 4;
			} else {
				sprintf(ch + j, "\\%c%04X", uesc, wch[i]); // \uhhhh \xhhhh
				j += 6;
			}
		}
	}

	SciCall_ReplaceSel(ch);
	SciCall_SetSel(iSelStart, iSelStart + strlen(ch));
	NP2HeapFree(ch);
	NP2HeapFree(wch);
}

//=============================================================================
//
// EditHex2Char()
//
void EditHex2Char(void) {
	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	Sci_Position count = SciCall_GetSelTextLength() - 1;
	if (count == 0) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	count *= 2 + MAX_ESCAPE_HEX_DIGIT;
	char *ch = (char *)NP2HeapAlloc(count + 1);
	WCHAR *wch = (WCHAR *)NP2HeapAlloc((count + 1) * sizeof(WCHAR));
	const UINT cpEdit = SciCall_GetCodePage();
	int ci = 0;
	int cch = 0;

	SciCall_GetSelText(ch);

	char *p = ch;
	while (*p) {
		if (*p == '\\') {
			p++;
			if (*p == 'x' || *p == 'u') {
				p++;
				ci = 0;
				int ucc = 0;
				while (*p && (ucc++ < MAX_ESCAPE_HEX_DIGIT)) {
					if (*p >= '0' && *p <= '9') {
						ci = ci * 16 + (*p++ - '0');
					} else if (*p >= 'a' && *p <= 'f') {
						ci = ci * 16 + (*p++ - 'a') + 10;
					} else if (*p >= 'A' && *p <=  'F') {
						ci = ci * 16 + (*p++ - 'A') + 10;
					} else {
						break;
					}
				}
			} else {
				ci = *p++;
			}
		} else {
			ci = *p++;
		}
		wch[cch++] = (WCHAR)ci;
		if (ci == 0) {
			break;
		}
	}

	wch[cch] = 0;
	cch = WideCharToMultiByte(cpEdit, 0, wch, -1, ch, (int)(count + 1), NULL, NULL) - 1; // '\0'

	SciCall_SetTargetRange(iSelStart, iSelEnd);
	SciCall_ReplaceTarget(cch, ch);
	SciCall_SetSel(iSelStart, iSelStart + cch);
	NP2HeapFree(ch);
	NP2HeapFree(wch);
}

void EditShowHex(void) {
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	const Sci_Position count = SciCall_GetSelTextLength() - 1;
	if (count == 0) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	char *ch = (char *)NP2HeapAlloc(count + 1);
	char *cch = (char *)NP2HeapAlloc(count * 3 + 3);
	char *p = ch;
	char *t = cch;
	SciCall_GetSelText(ch);
	*t++ = '[';
	while (*p) {
		int c = *p++;
		if (c < 0) {
			c += 256;
		}
		int v = c >> 4;
		*t++ = (char)((v >= 10) ? v - 10 + 'a' : v + '0');
		v = c & 0x0f;
		*t++ = (char)((v >= 10) ? v - 10 + 'a' : v + '0');
		*t++ = ' ';
	}
	*--t = ']';
	SciCall_InsertText(iSelEnd, cch);
	SciCall_SetSel(iSelEnd, iSelEnd + strlen(cch));
	NP2HeapFree(ch);
	NP2HeapFree(cch);
}

//=============================================================================
//
// EditConvertNumRadix()
//
static inline BOOL iswordstart(int ch) {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '.' || ch == '_';
}

static inline BOOL iswordchar(int ch) {
	return iswordstart(ch) || (ch >= '0' && ch <= '9') || ch == '$';
}

static int ConvertNumRadix(char *tch, uint64_t num, int radix) {
	switch (radix) {
	case 16:
		return sprintf(tch, "0x%" PRIx64, num);

	case 10:
		return sprintf(tch, "%" PRIu64, num);

	case 8: {
		char buf[2 + 22 + 1] = "";
		int index = 2 + 22;
		int length = 0;
		while (num) {
			const int bit = (int)(num & 7);
			num >>= 3;
			buf[index--] = (char)('0' + bit);
			++length;
		}
		if (length == 0) {
			buf[index--] = '0';
			++length;
		}
		buf[index--] = 'O';
		buf[index] = '0';
		length += 2;
		strcat(tch, buf + index);
		return length;
	}
	break;

	case 2: {
		char buf[2 + 64 + 8 + 1] = "";
		int index = 2 + 64 + 8;
		int length = 0;
		int bit_count = 0;
		while (num) {
			const int bit = (int)(num & 1);
			num >>= 1;
			buf[index--] = (char)('0' + bit);
			++bit_count;
			++length;
			if (num && (bit_count & 7) == 0) {
				buf[index--] = '_';
				++length;
			}
		}
		if (length == 0) {
			buf[index--] = '0';
			++length;
		}
		buf[index--] = 'b';
		buf[index] = '0';
		length += 2;
		strcat(tch, buf + index);
		return length;
	}
	break;

	}
	return 0;
}

void EditConvertNumRadix(int radix) {
	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	const Sci_Position count = SciCall_GetSelTextLength() - 1;
	if (count == 0) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	char *ch = (char *)NP2HeapAlloc(count + 1);
	char *tch = (char *)NP2HeapAlloc(2 + count * 4 + 8 + 1);
	Sci_Position cch = 0;
	char *p = ch;
	uint64_t ci = 0;

	SciCall_GetSelText(ch);

	while (*p) {
		if (*p == '0') {
			ci = 0;
			p++;
			if ((*p == 'x' || *p == 'X') && radix != 16) {
				p++;
				while (*p) {
					if (*p >= '0' && *p <= '9') {
						ci <<= 4;
						ci += (*p++ - '0');
					} else if (*p >= 'a' && *p <= 'f') {
						ci <<= 4;
						ci += (*p++ - 'a') + 10;;
					} else if (*p >= 'A' && *p <=  'F') {
						ci <<= 4;
						ci += (*p++ - 'A') + 10;
					} else if (*p == '_') {
						p++;
					} else {
						break;
					}
				}
				cch += ConvertNumRadix(tch + cch, ci, radix);
			} else if ((*p == 'o' || *p == 'O') && radix != 8) {
				p++;
				while (*p) {
					if (*p >= '0' && *p <= '7') {
						ci <<= 3;
						ci += (*p++ - '0');
					} else if (*p == '_') {
						p++;
					} else {
						break;
					}
				}
				cch += ConvertNumRadix(tch + cch, ci, radix);
			} else if ((*p == 'b' || *p == 'B') && radix != 2) {
				p++;
				while (*p) {
					if (*p == '0') {
						ci <<= 1;
						p++;
					} else if (*p == '1') {
						ci <<= 1;
						ci |= 1;
						p++;
					} else if (*p == '_') {
						p++;
					} else {
						break;
					}
				}
				cch += ConvertNumRadix(tch + cch, ci, radix);
			} else if ((*p >= '0' && *p <= '9') && radix != 10) {
				ci = *p++ - '0';
				while (*p) {
					if (*p >= '0' && *p <= '9') {
						ci *= 10;
						ci += (*p++ - '0');
					} else if (*p == '_') {
						p++;
					} else {
						break;
					}
				}
				cch += ConvertNumRadix(tch + cch, ci, radix);
			} else {
				tch[cch++] = '0';
			}
		} else if ((*p >= '1' && *p <= '9') && radix != 10) {
			ci = *p++ - '0';
			while (*p) {
				if (*p >= '0' && *p <= '9') {
					ci *= 10;
					ci += (*p++ - '0');
				} else if (*p == '_') {
					p++;
				} else {
					break;
				}
			}
			cch += ConvertNumRadix(tch + cch, ci, radix);
		} else if (iswordstart(*p)) {
			tch[cch++] = *p++;
			while (iswordchar(*p)) {
				tch[cch++] = *p++;
			}
		} else {
			tch[cch++] = *p++;
		}
	}
	tch[cch] = '\0';

	SciCall_SetTargetRange(iSelStart, iSelEnd);
	SciCall_ReplaceTarget(cch, tch);
	SciCall_SetSel(iSelStart, iSelStart + cch);
	NP2HeapFree(ch);
	NP2HeapFree(tch);
}

//=============================================================================
//
// EditModifyNumber()
//
void EditModifyNumber(BOOL bIncrease) {
	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	if (iSelStart == iSelEnd) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	if (SciCall_GetSelTextLength() <= 32) {
		char chNumber[32] = "";
		SciCall_GetSelText(chNumber);
		if (strchr(chNumber, '-')) {
			return;
		}

		const int radix = StrChrIA(chNumber, 'x') ? 16 : 10;
		char *end;
		int iNumber = (int)strtol(chNumber, &end, radix);
		if (end == chNumber || iNumber < 0) {
			return;
		}

		if (bIncrease && iNumber < INT_MAX) {
			iNumber++;
		}
		if (!bIncrease && iNumber > 0) {
			iNumber--;
		}

		const int iWidth = (int)strlen(chNumber) - ((radix == 16) ? 2 : 0);
		if (radix == 16) {
			const int len = iWidth + 1;
			BOOL bUppercase = FALSE;
			for (int i = len; i >= 0; i--) {
				if (IsCharLowerA(chNumber[i])) {
					break;
				}
				if (IsCharUpperA(chNumber[i])) {
					bUppercase = TRUE;
					break;
				}
			}
			if (bUppercase) {
				sprintf(chNumber, "%#0*X", iWidth, iNumber);
			} else {
				sprintf(chNumber, "%#0*x", iWidth, iNumber);
			}
		} else {
			sprintf(chNumber, "%0*i", iWidth, iNumber);
		}
		SciCall_ReplaceSel(chNumber);
		SciCall_SetSel(iSelStart, iSelStart + strlen(chNumber));
	}
}

extern int iTabWidth;
extern int iTabWidthG;
extern int iIndentWidth;
extern int iIndentWidthG;
extern BOOL bTabsAsSpaces;
extern BOOL bTabsAsSpacesG;
extern BOOL bTabIndents;
extern BOOL bTabIndentsG;

//=============================================================================
//
// EditTabsToSpaces()
//
void EditTabsToSpaces(int nTabWidth, BOOL bOnlyIndentingWS) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	const Sci_Line iLine = SciCall_LineFromPosition(iSelStart);
	iSelStart = SciCall_PositionFromLine(iLine);

	const Sci_Position iSelCount = iSelEnd - iSelStart;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

	struct Sci_TextRange tr;
	tr.chrg.cpMin = (Sci_PositionCR)iSelStart;
	tr.chrg.cpMax = (Sci_PositionCR)iSelEnd;
	tr.lpstrText = pszText;
	SciCall_GetTextRange(&tr);

	const UINT cpEdit = SciCall_GetCodePage();
	const int cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));
	NP2HeapFree(pszText);

	LPWSTR pszConvW = (LPWSTR)NP2HeapAlloc(cchTextW * sizeof(WCHAR) * nTabWidth + 2);
	int cchConvW = 0;

	BOOL bIsLineStart = TRUE;
	BOOL bModified = FALSE;
	// Contributed by Homam, Thank you very much!
	int i = 0;
	for (int iTextW = 0; iTextW < cchTextW; iTextW++) {
		const WCHAR w = pszTextW[iTextW];
		if (w == L'\t' && (!bOnlyIndentingWS || bIsLineStart)) {
			for (int j = 0; j < nTabWidth - i % nTabWidth; j++) {
				pszConvW[cchConvW++] = L' ';
			}
			i = 0;
			bModified = TRUE;
		} else {
			i++;
			if (w == L'\n' || w == L'\r') {
				i = 0;
				bIsLineStart = TRUE;
			} else if (w != L' ') {
				bIsLineStart = FALSE;
			}
			pszConvW[cchConvW++] = w;
		}
	}

	NP2HeapFree(pszTextW);

	if (bModified) {
		pszText = (char *)NP2HeapAlloc(cchConvW * kMaxMultiByteCount);

		const int cchConvM = WideCharToMultiByte(cpEdit, 0, pszConvW, cchConvW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + cchConvM;
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + cchConvM;
		}

		SciCall_BeginUndoAction();
		SciCall_SetTargetRange(iSelStart, iSelEnd);
		SciCall_ReplaceTarget(cchConvM, pszText);
		//SciCall_Clear();
		//SciCall_AddText(cchConvW, pszText);
		SciCall_SetSel(iAnchorPos, iCurPos);
		SciCall_EndUndoAction();

		NP2HeapFree(pszText);
	}

	NP2HeapFree(pszConvW);
}

//=============================================================================
//
// EditSpacesToTabs()
//
void EditSpacesToTabs(int nTabWidth, BOOL bOnlyIndentingWS) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	const Sci_Line iLine = SciCall_LineFromPosition(iSelStart);
	iSelStart = SciCall_PositionFromLine(iLine);

	const Sci_Position iSelCount = iSelEnd - iSelStart;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

	struct Sci_TextRange tr;
	tr.chrg.cpMin = (Sci_PositionCR)iSelStart;
	tr.chrg.cpMax = (Sci_PositionCR)iSelEnd;
	tr.lpstrText = pszText;
	SciCall_GetTextRange(&tr);;

	const UINT cpEdit = SciCall_GetCodePage();

	const int cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));
	NP2HeapFree(pszText);

	LPWSTR pszConvW = (LPWSTR)NP2HeapAlloc(cchTextW * sizeof(WCHAR) + 2);
	int cchConvW = 0;

	BOOL bIsLineStart = TRUE;
	BOOL bModified = FALSE;
	// Contributed by Homam, Thank you very much!
	int i = 0;
	int j = 0;
	WCHAR space[256];
	for (int iTextW = 0; iTextW < cchTextW; iTextW++) {
		const WCHAR w = pszTextW[iTextW];
		if ((w == L' ' || w == L'\t') && (!bOnlyIndentingWS ||  bIsLineStart)) {
			space[j++] = w;
			if (j == nTabWidth - i % nTabWidth || w == L'\t') {
				if (j > 1 || pszTextW[iTextW + 1] == L' ' || pszTextW[iTextW + 1] == L'\t') {
					pszConvW[cchConvW++] = L'\t';
				} else {
					pszConvW[cchConvW++] = w;
				}
				i = j = 0;
				bModified = bModified || (w != pszConvW[cchConvW - 1]);
			}
		} else {
			i += j + 1;
			if (j > 0) {
				//space[j] = '\0';
				for (int t = 0; t < j; t++) {
					pszConvW[cchConvW++] = space[t];
				}
				j = 0;
			}
			if (w == L'\n' || w == L'\r') {
				i = 0;
				bIsLineStart = TRUE;
			} else {
				bIsLineStart = FALSE;
			}
			pszConvW[cchConvW++] = w;
		}
	}

	if (j > 0) {
		for (int t = 0; t < j; t++) {
			pszConvW[cchConvW++] = space[t];
		}
	}

	NP2HeapFree(pszTextW);

	if (bModified || cchConvW != cchTextW) {
		pszText = (char *)NP2HeapAlloc(cchConvW * kMaxMultiByteCount + 1);
		const int cchConvM = WideCharToMultiByte(cpEdit, 0, pszConvW, cchConvW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + cchConvM;
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + cchConvM;
		}

		SciCall_BeginUndoAction();
		SciCall_SetTargetRange(iSelStart, iSelEnd);
		SciCall_ReplaceTarget(cchConvM, pszText);
		//SciCall_Clear();
		//SciCall_AddText(cchConvW, pszText);
		SciCall_SetSel(iAnchorPos, iCurPos);
		SciCall_EndUndoAction();

		NP2HeapFree(pszText);
	}

	NP2HeapFree(pszConvW);
}

//=============================================================================
//
// EditMoveUp()
//
void EditMoveUp(void) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	const Sci_Line iCurLine = SciCall_LineFromPosition(iCurPos);
	const Sci_Line iAnchorLine = SciCall_LineFromPosition(iAnchorPos);

	if (iCurLine == iAnchorLine) {
		const Sci_Position iLineCurPos = iCurPos - SciCall_PositionFromLine(iCurLine);
		const Sci_Position iLineAnchorPos = iAnchorPos - SciCall_PositionFromLine(iAnchorLine);
		//const Sci_Line iLineCur = SciCall_DocLineFromVisible(iCurLine);
		//const Sci_Line iLineAnchor = SciCall_DocLineFromVisible(iAnchorLine);
		//if (iLineCur == iLineAnchor) {
		//}

		if (iCurLine > 0) {
			SciCall_BeginUndoAction();
			SciCall_LineTranspose();
			SciCall_SetSel(SciCall_PositionFromLine(iAnchorLine - 1) + iLineAnchorPos, SciCall_PositionFromLine(iCurLine - 1) + iLineCurPos);
			SciCall_ChooseCaretX();
			SciCall_EndUndoAction();
		}
		return;
	}

	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Line iLineSrc = min_pos(iCurLine, iAnchorLine) - 1;
	//const Sci_Line iLineCur = SciCall_DocLineFromVisible(iCurLine);
	//const Sci_Line iLineAnchor = SciCall_DocLineFromVisible(iAnchorLine);
	//if (iLineCur == iLineAnchor) {
	//}

	if (iLineSrc >= 0) {
		const Sci_Position cLine = SciCall_GetLineLength(iLineSrc);
		char *pLine = (char *)NP2HeapAlloc(cLine + 1);
		SciCall_GetLine(iLineSrc, pLine);

		const Sci_Position iLineSrcStart = SciCall_PositionFromLine(iLineSrc);
		const Sci_Position iLineSrcEnd = SciCall_PositionFromLine(iLineSrc + 1);

		Sci_Line iLineDest = max_pos(iCurLine, iAnchorLine);
		if (max_pos(iCurPos, iAnchorPos) <= SciCall_PositionFromLine(iLineDest)) {
			if (iLineDest >= 1) {
				iLineDest--;
			}
		}

		SciCall_BeginUndoAction();

		SciCall_SetTargetRange(iLineSrcStart, iLineSrcEnd);
		SciCall_ReplaceTarget(0, "");

		const Sci_Position iLineDestStart = SciCall_PositionFromLine(iLineDest);
		SciCall_InsertText(iLineDestStart, pLine);
		NP2HeapFree(pLine);

		if (iLineDest == SciCall_GetLineCount() - 1) {
			char chaEOL[] = "\r\n";
			const int iEOLMode = SciCall_GetEOLMode();
			if (iEOLMode == SC_EOL_CR) {
				chaEOL[1] = 0;
			} else if (iEOLMode == SC_EOL_LF) {
				chaEOL[0] = '\n';
				chaEOL[1] = 0;
			}

			SciCall_InsertText(iLineDestStart, chaEOL);
			SciCall_SetTargetRange(SciCall_GetLineEndPosition(iLineDest), SciCall_GetLength());
			SciCall_ReplaceTarget(0, "");
		}

		if (iCurPos < iAnchorPos) {
			iCurPos = SciCall_PositionFromLine(iCurLine - 1);
			iAnchorPos = SciCall_PositionFromLine(iLineDest);
		} else {
			iAnchorPos = SciCall_PositionFromLine(iAnchorLine - 1);
			iCurPos = SciCall_PositionFromLine(iLineDest);
		}

		SciCall_SetSel(iAnchorPos, iCurPos);

		SciCall_EndUndoAction();
	}
}

//=============================================================================
//
// EditMoveDown()
//
void EditMoveDown(void) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	const Sci_Line iCurLine = SciCall_LineFromPosition(iCurPos);
	const Sci_Line iAnchorLine = SciCall_LineFromPosition(iAnchorPos);

	if (iCurLine == iAnchorLine) {
		const Sci_Position iLineCurPos = iCurPos - SciCall_PositionFromLine(iCurLine);
		const Sci_Position iLineAnchorPos = iAnchorPos - SciCall_PositionFromLine(iAnchorLine);
		//const Sci_Line iLineCur = SciCall_DocLineFromVisible(iCurLine);
		//const Sci_Line iLineAnchor = SciCall_DocLineFromVisible(iAnchorLine);
		//if (iLineCur == iLineAnchor) {
		//}

		if (iCurLine < SciCall_GetLineCount() - 1) {
			SciCall_BeginUndoAction();
			SciCall_GotoLine(iCurLine + 1);
			SciCall_LineTranspose();
			SciCall_SetSel(SciCall_PositionFromLine(iAnchorLine + 1) + iLineAnchorPos, SciCall_PositionFromLine(iCurLine + 1) + iLineCurPos);
			SciCall_ChooseCaretX();
			SciCall_EndUndoAction();
		}
		return;
	}

	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	Sci_Line iLineSrc = max_pos(iCurLine, iAnchorLine) + 1;
	//const Sci_Line iLineCur = SciCall_DocLineFromVisible(iCurLine);
	//const Sci_Line iLineAnchor = SciCall_DocLineFromVisible(iAnchorLine);
	//if (iLineCur == iLineAnchor) {
	//}

	if (max_pos(iCurPos, iAnchorPos) <= SciCall_PositionFromLine(iLineSrc - 1)) {
		if (iLineSrc >= 1) {
			iLineSrc--;
		}
	}

	if (iLineSrc <= SciCall_GetLineCount() - 1) {
		const BOOL bLastLine = (iLineSrc == SciCall_GetLineCount() - 1);

		if (bLastLine &&
				(SciCall_GetLineEndPosition(iLineSrc) == SciCall_PositionFromLine(iLineSrc)) &&
				(SciCall_GetLineEndPosition(iLineSrc - 1) == SciCall_PositionFromLine(iLineSrc - 1))) {
			return;
		}

		if (bLastLine) {
			char chaEOL[] = "\r\n";
			const int iEOLMode = SciCall_GetEOLMode();
			if (iEOLMode == SC_EOL_CR) {
				chaEOL[1] = 0;
			} else if (iEOLMode == SC_EOL_LF) {
				chaEOL[0] = '\n';
				chaEOL[1] = 0;
			}
			SciCall_AppendText(strlen(chaEOL), chaEOL);
		}

		const Sci_Position cLine = SciCall_GetLineLength(iLineSrc);
		char *pLine = (char *)NP2HeapAlloc(cLine + 3);
		SciCall_GetLine(iLineSrc, pLine);

		const Sci_Position iLineSrcStart = SciCall_PositionFromLine(iLineSrc);
		const Sci_Position iLineSrcEnd = SciCall_PositionFromLine(iLineSrc + 1);
		const Sci_Line iLineDest = min_pos(iCurLine, iAnchorLine);

		SciCall_BeginUndoAction();

		SciCall_SetTargetRange(iLineSrcStart, iLineSrcEnd);
		SciCall_ReplaceTarget(0, "");

		const Sci_Position iLineDestStart = SciCall_PositionFromLine(iLineDest);
		SciCall_InsertText(iLineDestStart, pLine);

		if (bLastLine) {
			SciCall_SetTargetRange(SciCall_GetLineEndPosition(SciCall_GetLineCount() - 2), SciCall_GetLength());
			SciCall_ReplaceTarget(0, "");
		}

		NP2HeapFree(pLine);

		if (iCurPos < iAnchorPos) {
			iCurPos = SciCall_PositionFromLine(iCurLine + 1);
			iAnchorPos = SciCall_PositionFromLine(iLineSrc + 1);
		} else {
			iAnchorPos = SciCall_PositionFromLine(iAnchorLine + 1);
			iCurPos = SciCall_PositionFromLine(iLineSrc + 1);
		}

		SciCall_SetSel(iAnchorPos, iCurPos);

		SciCall_EndUndoAction();
	}
}

// only convert CR+LF
static void ConvertWinEditLineEndingsEx(char *s, int iEOLMode, int *lineCount) {
	int count = 0;
	if (iEOLMode != SC_EOL_CRLF) {
		char *p = s;
		const char chaEOL = (iEOLMode == SC_EOL_LF) ? '\n' : '\r';
		while (*s) {
			switch (*s) {
			case '\r':
				++count;
				if (s[1] == '\n') {
					++s;
					*p++ = chaEOL;
				} else {
					*p++ = '\r';
				}
				++s;
				break;

			case '\n':
				++count;
				*p++ = '\n';
				++s;
				break;

			default:
				*p++ = *s++;
				break;
			}
		}

		*p = '\0';
		if (lineCount != NULL) {
			*lineCount = count;
		}
	} else if (lineCount != NULL) {
		while (*s) {
			switch (*s++) {
			case '\r':
				++count;
				if (*s == '\n') {
					++s;
				}
				break;

			case '\n':
				++count;
				break;
			}
		}
		*lineCount = count;
	}
}

static inline void ConvertWinEditLineEndings(char *s, int iEOLMode) {
	ConvertWinEditLineEndingsEx(s, iEOLMode, NULL);
}

//=============================================================================
//
// EditModifyLines()
//
void EditModifyLines(LPCWSTR pwszPrefix, LPCWSTR pwszAppend) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}
	if (StrIsEmpty(pwszPrefix) && StrIsEmpty(pwszAppend)) {
		return;
	}

	BeginWaitCursor();

	const UINT cpEdit = SciCall_GetCodePage();
	const int iEOLMode = SciCall_GetEOLMode();

	const int iPrefixLen = lstrlen(pwszPrefix);
	char *mszPrefix1 = NULL;
	int iPrefixLine = 0;
	if (iPrefixLen != 0) {
		const int size = iPrefixLen * kMaxMultiByteCount + 1;
		mszPrefix1 = (char *)NP2HeapAlloc(size);
		WideCharToMultiByte(cpEdit, 0, pwszPrefix, -1, mszPrefix1, size, NULL, NULL);
		ConvertWinEditLineEndingsEx(mszPrefix1, iEOLMode, &iPrefixLine);
	}

	const int iAppendLen = lstrlen(pwszAppend);
	char *mszAppend1 = NULL;
	int iAppendLine = 0;
	if (iAppendLen != 0) {
		const int size = iAppendLen * kMaxMultiByteCount + 1;
		mszAppend1 = (char *)NP2HeapAlloc(size);
		WideCharToMultiByte(cpEdit, 0, pwszAppend, -1, mszAppend1, size, NULL, NULL);
		ConvertWinEditLineEndingsEx(mszAppend1, iEOLMode, &iAppendLine);
	}

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	const Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
	Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);

	//if (iSelStart > SciCall_PositionFromLine(iLineStart)) {
	//	iLineStart++;
	//}

	if (iSelEnd <= SciCall_PositionFromLine(iLineEnd)) {
		if (iLineEnd - iLineStart >= 1) {
			iLineEnd--;
		}
	}

	BOOL bPrefixNumPadZero = FALSE;
	char *mszPrefix2 = NULL;
	Sci_Line iPrefixNum = 0;
	int iPrefixNumWidth = 1;
	BOOL bPrefixNum = FALSE;

	BOOL bAppendNumPadZero = FALSE;
	char *mszAppend2 = NULL;
	Sci_Line iAppendNum = 0;
	int iAppendNumWidth = 1;
	BOOL bAppendNum = FALSE;

	if (iPrefixLen != 0) {
		char *p = mszPrefix1;
		Sci_Line lineCount = 0;
		mszPrefix2 = (char *)NP2HeapAlloc(iPrefixLen * kMaxMultiByteCount + 1);
		while (!bPrefixNum && (p = strstr(p, "$(")) != NULL) {
			if (strncmp(p, "$(I)", CSTRLEN("$(I)")) == 0) {
				*p = 0;
				strcpy(mszPrefix2, p + CSTRLEN("$(I)"));
				bPrefixNum = TRUE;
				iPrefixNum = 0;
				lineCount = iLineEnd - iLineStart;
				bPrefixNumPadZero = FALSE;
			} else if (strncmp(p, "$(0I)", CSTRLEN("$(0I)")) == 0) {
				*p = 0;
				strcpy(mszPrefix2, p + CSTRLEN("$(0I)"));
				bPrefixNum = TRUE;
				iPrefixNum = 0;
				lineCount = iLineEnd - iLineStart;
				bPrefixNumPadZero = TRUE;
			} else if (strncmp(p, "$(N)", CSTRLEN("$(N)")) == 0) {
				*p = 0;
				strcpy(mszPrefix2, p + CSTRLEN("$(N)"));
				bPrefixNum = TRUE;
				iPrefixNum = 1;
				lineCount = iLineEnd - iLineStart + 1;
				bPrefixNumPadZero = FALSE;
			} else if (strncmp(p, "$(0N)", CSTRLEN("$(0N)")) == 0) {
				*p = 0;
				strcpy(mszPrefix2, p + CSTRLEN("$(0N)"));
				bPrefixNum = TRUE;
				iPrefixNum = 1;
				lineCount = iLineEnd - iLineStart + 1;
				bPrefixNumPadZero = TRUE;
			} else if (strncmp(p, "$(L)", CSTRLEN("$(L)")) == 0) {
				*p = 0;
				strcpy(mszPrefix2, p + CSTRLEN("$(L)"));
				bPrefixNum = TRUE;
				iPrefixNum = iLineStart + 1;
				lineCount = iLineEnd + 1;
				bPrefixNumPadZero = FALSE;
			} else if (strncmp(p, "$(0L)", CSTRLEN("$(0L)")) == 0) {
				*p = 0;
				strcpy(mszPrefix2, p + CSTRLEN("$(0L)"));
				bPrefixNum = TRUE;
				iPrefixNum = iLineStart + 1;
				lineCount = iLineEnd + 1;
				bPrefixNumPadZero = TRUE;
			}
			p += CSTRLEN("$(");
		}
		if (bPrefixNum) {
			while (lineCount >= 10) {
				++iPrefixNumWidth;
				lineCount /= 10;
			}
		}
	}

	if (iAppendLen != 0) {
		char *p = mszAppend1;
		Sci_Line lineCount = 0;
		mszAppend2 = (char *)NP2HeapAlloc(iAppendLen * kMaxMultiByteCount + 1);
		while (!bAppendNum && (p = strstr(p, "$(")) != NULL) {
			if (strncmp(p, "$(I)", CSTRLEN("$(I)")) == 0) {
				*p = 0;
				strcpy(mszAppend2, p + CSTRLEN("$(I)"));
				bAppendNum = TRUE;
				iAppendNum = 0;
				lineCount = iLineEnd - iLineStart;
				bAppendNumPadZero = FALSE;
			} else if (strncmp(p, "$(0I)", CSTRLEN("$(0I)")) == 0) {
				*p = 0;
				strcpy(mszAppend2, p + CSTRLEN("$(0I)"));
				bAppendNum = TRUE;
				iAppendNum = 0;
				lineCount = iLineEnd - iLineStart;
				bAppendNumPadZero = TRUE;
			} else if (strncmp(p, "$(N)", CSTRLEN("$(N)")) == 0) {
				*p = 0;
				strcpy(mszAppend2, p + CSTRLEN("$(N)"));
				bAppendNum = TRUE;
				iAppendNum = 1;
				lineCount = iLineEnd - iLineStart + 1;
				bAppendNumPadZero = FALSE;
			} else if (strncmp(p, "$(0N)", CSTRLEN("$(0N)")) == 0) {
				*p = 0;
				strcpy(mszAppend2, p + CSTRLEN("$(0N)"));
				bAppendNum = TRUE;
				iAppendNum = 1;
				lineCount = iLineEnd - iLineStart + 1;
				bAppendNumPadZero = TRUE;
			} else if (strncmp(p, "$(L)", CSTRLEN("$(L)")) == 0) {
				*p = 0;
				strcpy(mszAppend2, p + CSTRLEN("$(L)"));
				bAppendNum = TRUE;
				iAppendNum = iLineStart + 1;
				lineCount = iLineEnd + 1;
				bAppendNumPadZero = FALSE;
			} else if (strncmp(p, "$(0L)", CSTRLEN("$(0L)")) == 0) {
				*p = 0;
				strcpy(mszAppend2, p + CSTRLEN("$(0L)"));
				bAppendNum = TRUE;
				iAppendNum = iLineStart + 1;
				lineCount = iLineEnd + 1;
				bAppendNumPadZero = TRUE;
			}
			p += CSTRLEN("$(");
		}
		if (bAppendNum) {
			while (lineCount >= 10) {
				++iAppendNumWidth;
				lineCount /= 10;
			}
		}
	}

	char *mszInsert = (char *)NP2HeapAlloc(2 * max_i(iPrefixLen, iAppendLen) * kMaxMultiByteCount + 1);
	SciCall_BeginUndoAction();
	for (Sci_Line iLine = iLineStart, iLineDest = iLineStart; iLine <= iLineEnd; iLine++, iLineDest++) {
		if (iPrefixLen != 0) {
			strcpy(mszInsert, mszPrefix1);

			if (bPrefixNum) {
				char tchNum[64];
#if defined(_WIN64)
				if (bPrefixNumPadZero) {
					sprintf(tchNum, "%0*" PRId64, iPrefixNumWidth, iPrefixNum);
				} else {
					sprintf(tchNum, "%*" PRId64, iPrefixNumWidth, iPrefixNum);
				}
#else
				if (bPrefixNumPadZero) {
					sprintf(tchNum, "%0*d", iPrefixNumWidth, (int)iPrefixNum);
				} else {
					sprintf(tchNum, "%*d", iPrefixNumWidth, (int)iPrefixNum);
				}
#endif
				strcat(mszInsert, tchNum);
				strcat(mszInsert, mszPrefix2);
				iPrefixNum++;
			}

			const Sci_Position iPos = SciCall_PositionFromLine(iLineDest);
			SciCall_SetTargetRange(iPos, iPos);
			SciCall_ReplaceTarget(strlen(mszInsert), mszInsert);
			iLineDest += iPrefixLine;
		}

		if (iAppendLen != 0) {
			strcpy(mszInsert, mszAppend1);

			if (bAppendNum) {
				char tchNum[64];
#if defined(_WIN64)
				if (bAppendNumPadZero) {
					sprintf(tchNum, "%0*" PRId64, iAppendNumWidth, iAppendNum);
				} else {
					sprintf(tchNum, "%*" PRId64, iAppendNumWidth, iAppendNum);
				}
#else
				if (bAppendNumPadZero) {
					sprintf(tchNum, "%0*d", iAppendNumWidth, (int)iAppendNum);
				} else {
					sprintf(tchNum, "%*d", iAppendNumWidth, (int)iAppendNum);
				}
#endif
				strcat(mszInsert, tchNum);
				strcat(mszInsert, mszAppend2);
				iAppendNum++;
			}

			const Sci_Position iPos = SciCall_GetLineEndPosition(iLineDest);
			SciCall_SetTargetRange(iPos, iPos);
			SciCall_ReplaceTarget(strlen(mszInsert), mszInsert);
			iLineDest += iAppendLine;
		}
	}
	SciCall_EndUndoAction();

	//// Fix selection
	//if (iSelStart != iSelEnd && SciCall_GetTargetEnd() > SciCall_GetSelectionEnd()) {
	//	Sci_Position iCurPos = SciCall_GetCurrentPos();
	//	Sci_Position iAnchorPos = SciCall_GetAnchor();
	//	if (iCurPos > iAnchorPos)
	//		iCurPos = SciCall_GetTargetEnd();
	//	else
	//		iAnchorPos = SciCall_GetTargetEnd();
	//	SciCall_SetSel(iAnchorPos, iCurPos);
	//}

	// extend selection to start of first line
	// the above code is not required when last line has been excluded
	if (iSelStart != iSelEnd) {
		Sci_Position iCurPos = SciCall_GetCurrentPos();
		Sci_Position iAnchorPos = SciCall_GetAnchor();
		if (iCurPos < iAnchorPos) {
			iCurPos = SciCall_PositionFromLine(iLineStart);
			iAnchorPos = SciCall_PositionFromLine(iLineEnd + 1);
		} else {
			iAnchorPos = SciCall_PositionFromLine(iLineStart);
			iCurPos = SciCall_PositionFromLine(iLineEnd + 1);
		}
		SciCall_SetSel(iAnchorPos, iCurPos);
	}

	EndWaitCursor();
	if (mszPrefix1 != NULL) {
		NP2HeapFree(mszPrefix1);
	}
	if (mszAppend1 != NULL) {
		NP2HeapFree(mszAppend1);
	}
	if (mszPrefix2 != NULL) {
		NP2HeapFree(mszPrefix2);
	}
	if (mszAppend2 != NULL) {
		NP2HeapFree(mszAppend2);
	}
	NP2HeapFree(mszInsert);
}

//=============================================================================
//
// EditAlignText()
//
void EditAlignText(int nMode) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

#define BUFSIZE_ALIGN 1024

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	const UINT cpEdit = SciCall_GetCodePage();

	BOOL bModified = FALSE;
	const Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
	Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);

	if (iSelEnd <= SciCall_PositionFromLine(iLineEnd)) {
		if (iLineEnd - iLineStart >= 1) {
			iLineEnd--;
		}
	}

	Sci_Position iMinIndent = BUFSIZE_ALIGN;
	Sci_Position iMaxLength = 0;
	for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
		Sci_Position iLineEndPos = SciCall_GetLineEndPosition(iLine);
		const Sci_Position iLineIndentPos = SciCall_GetLineIndentPosition(iLine);

		if (iLineIndentPos != iLineEndPos) {
			const Sci_Position iIndentCol = SciCall_GetLineIndentation(iLine);
			Sci_Position iTail = iLineEndPos - 1;
			int ch = SciCall_GetCharAt(iTail);
			while (iTail >= iLineStart && (ch == ' ' || ch == '\t')) {
				iTail--;
				ch = SciCall_GetCharAt(iTail);
				iLineEndPos--;
			}

			const Sci_Position iEndCol = SciCall_GetColumn(iLineEndPos);
			iMinIndent = min_pos(iMinIndent, iIndentCol);
			iMaxLength = max_pos(iMaxLength, iEndCol);
		}
	}

	if (iMaxLength < BUFSIZE_ALIGN) {
		for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			const Sci_Position iIndentPos = SciCall_GetLineIndentPosition(iLine);
			const Sci_Position iEndPos = SciCall_GetLineEndPosition(iLine);

			if (iIndentPos == iEndPos && iEndPos > 0) {
				if (!bModified) {
					SciCall_BeginUndoAction();
					bModified = TRUE;
				}

				SciCall_SetTargetRange(SciCall_PositionFromLine(iLine), iEndPos);
				SciCall_ReplaceTarget(0, "");
			} else {
				char tchLineBuf[BUFSIZE_ALIGN * kMaxMultiByteCount] = "";
				WCHAR wchLineBuf[BUFSIZE_ALIGN] = L"";
				WCHAR *pWords[BUFSIZE_ALIGN];
				WCHAR *p = wchLineBuf;

				int iWords = 0;
				Sci_Position iWordsLength = 0;
				const Sci_Position cchLine = SciCall_GetLine(iLine, tchLineBuf);

				if (!bModified) {
					SciCall_BeginUndoAction();
					bModified = TRUE;
				}

				MultiByteToWideChar(cpEdit, 0, tchLineBuf, (int)cchLine, wchLineBuf, COUNTOF(wchLineBuf));
				StrTrim(wchLineBuf, L"\r\n\t ");

				while (*p) {
					if (*p != L' ' && *p != L'\t') {
						pWords[iWords++] = p++;
						iWordsLength++;
						while (*p && *p != L' ' && *p != L'\t') {
							p++;
							iWordsLength++;
						}
					} else {
						*p++ = 0;
					}
				}

				if (iWords > 0) {
					if (nMode == ALIGN_JUSTIFY || nMode == ALIGN_JUSTIFY_EX) {
						BOOL bNextLineIsBlank = FALSE;
						if (nMode == ALIGN_JUSTIFY_EX) {
							if (SciCall_GetLineCount() <= iLine + 1) {
								bNextLineIsBlank = TRUE;
							} else {
								const Sci_Position iLineEndPos = SciCall_GetLineEndPosition(iLine + 1);
								const Sci_Position iLineIndentPos = SciCall_GetLineIndentPosition(iLine + 1);
								if (iLineIndentPos == iLineEndPos) {
									bNextLineIsBlank = TRUE;
								}
							}
						}

						if ((nMode == ALIGN_JUSTIFY || nMode == ALIGN_JUSTIFY_EX) &&
								iWords > 1 && iWordsLength >= 2 &&
								((nMode != ALIGN_JUSTIFY_EX || !bNextLineIsBlank || iLineStart == iLineEnd) ||
								 (bNextLineIsBlank && iWordsLength*4 > (iMaxLength - iMinIndent)*3))) {
							const int iGaps = iWords - 1;
							const Sci_Position iSpacesPerGap = (iMaxLength - iMinIndent - iWordsLength) / iGaps;
							const Sci_Position iExtraSpaces = (iMaxLength - iMinIndent - iWordsLength) % iGaps;

							WCHAR wchNewLineBuf[BUFSIZE_ALIGN * 3];
							lstrcpy(wchNewLineBuf, pWords[0]);
							p = StrEnd(wchNewLineBuf);

							for (int i = 1; i < iWords; i++) {
								for (Sci_Position j = 0; j < iSpacesPerGap; j++) {
									*p++ = L' ';
									*p = 0;
								}
								if (i > iGaps - iExtraSpaces) {
									*p++ = L' ';
									*p = 0;
								}
								lstrcat(p, pWords[i]);
								p = StrEnd(p);
							}

							WideCharToMultiByte(cpEdit, 0, wchNewLineBuf, -1, tchLineBuf, COUNTOF(tchLineBuf), NULL, NULL);

							SciCall_SetTargetRange(SciCall_PositionFromLine(iLine), SciCall_GetLineEndPosition(iLine));
							SciCall_ReplaceTarget(strlen(tchLineBuf), tchLineBuf);

							SciCall_SetLineIndentation(iLine, iMinIndent);
						} else {
							WCHAR wchNewLineBuf[BUFSIZE_ALIGN];
							lstrcpy(wchNewLineBuf, pWords[0]);
							p = StrEnd(wchNewLineBuf);

							for (int i = 1; i < iWords; i++) {
								*p++ = L' ';
								*p = 0;
								lstrcat(wchNewLineBuf, pWords[i]);
								p = StrEnd(p);
							}

							WideCharToMultiByte(cpEdit, 0, wchNewLineBuf, -1, tchLineBuf, COUNTOF(tchLineBuf), NULL, NULL);

							SciCall_SetTargetRange(SciCall_PositionFromLine(iLine), SciCall_GetLineEndPosition(iLine));
							SciCall_ReplaceTarget(strlen(tchLineBuf), tchLineBuf);

							SciCall_SetLineIndentation(iLine, iMinIndent);
						}
					} else {
						const Sci_Position iExtraSpaces = iMaxLength - iMinIndent - iWordsLength - iWords + 1;
						Sci_Position iOddSpaces = iExtraSpaces % 2;

						WCHAR wchNewLineBuf[BUFSIZE_ALIGN * 3] = L"";
						p = wchNewLineBuf;

						if (nMode == ALIGN_RIGHT) {
							for (Sci_Position i = 0; i < iExtraSpaces; i++) {
								*p++ = L' ';
							}
							*p = 0;
						}

						if (nMode == ALIGN_CENTER) {
							for (Sci_Position i = 1; i < iExtraSpaces - iOddSpaces; i += 2) {
								*p++ = L' ';
							}
							*p = 0;
						}

						for (int i = 0; i < iWords; i++) {
							lstrcat(p, pWords[i]);
							if (i < iWords - 1) {
								lstrcat(p, L" ");
							}
							if (nMode == ALIGN_CENTER && iWords > 1 && iOddSpaces > 0 && i + 1 >= iWords / 2) {
								lstrcat(p, L" ");
								iOddSpaces--;
							}
							p = StrEnd(p);
						}

						WideCharToMultiByte(cpEdit, 0, wchNewLineBuf, -1, tchLineBuf, COUNTOF(tchLineBuf), NULL, NULL);

						Sci_Position iPos;
						if (nMode == ALIGN_RIGHT || nMode == ALIGN_CENTER) {
							SciCall_SetLineIndentation(iLine, iMinIndent);
							iPos = SciCall_GetLineIndentPosition(iLine);
						} else {
							iPos = SciCall_PositionFromLine(iLine);
						}

						SciCall_SetTargetRange(iPos, SciCall_GetLineEndPosition(iLine));
						SciCall_ReplaceTarget(strlen(tchLineBuf), tchLineBuf);

						if (nMode == ALIGN_LEFT) {
							SciCall_SetLineIndentation(iLine, iMinIndent);
						}
					}
				}
			}
		}
		if (bModified) {
			SciCall_EndUndoAction();
		}
	} else {
		MsgBox(MBINFO, IDS_BUFFERTOOSMALL);
	}

	if (iCurPos < iAnchorPos) {
		iCurPos = SciCall_PositionFromLine(iLineStart);
		iAnchorPos = SciCall_PositionFromLine(iLineEnd + 1);
	} else {
		iAnchorPos = SciCall_PositionFromLine(iLineStart);
		iCurPos = SciCall_PositionFromLine(iLineEnd + 1);
	}
	SciCall_SetSel(iAnchorPos, iCurPos);
}

//=============================================================================
//
// EditEncloseSelection()
//
void EditEncloseSelection(LPCWSTR pwszOpen, LPCWSTR pwszClose) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}
	if (StrIsEmpty(pwszOpen) && StrIsEmpty(pwszClose)) {
		return;
	}

	BeginWaitCursor();

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	const UINT cpEdit = SciCall_GetCodePage();
	const int iEOLMode = SciCall_GetEOLMode();

	char *mszOpen = NULL;
	int len = lstrlen(pwszOpen);
	if (len != 0) {
		const int size = kMaxMultiByteCount * len + 1;
		mszOpen = (char *)NP2HeapAlloc(size);
		WideCharToMultiByte(cpEdit, 0, pwszOpen, -1, mszOpen, size, NULL, NULL);
		ConvertWinEditLineEndings(mszOpen, iEOLMode);
	}

	char *mszClose = NULL;
	len = lstrlen(pwszClose);
	if (len != 0) {
		const int size = kMaxMultiByteCount * len + 1;
		mszClose = (char *)NP2HeapAlloc(size);
		WideCharToMultiByte(cpEdit, 0, pwszClose, -1, mszClose, size, NULL, NULL);
		ConvertWinEditLineEndings(mszClose, iEOLMode);
	}

	SciCall_BeginUndoAction();
	len = 0;

	if (StrNotEmptyA(mszOpen)) {
		len = (int)strlen(mszOpen);
		SciCall_SetTargetRange(iSelStart, iSelStart);
		SciCall_ReplaceTarget(len, mszOpen);
	}

	if (StrNotEmptyA(mszClose)) {
		SciCall_SetTargetRange(iSelEnd + len, iSelEnd + len);
		SciCall_ReplaceTarget(strlen(mszClose), mszClose);
	}

	SciCall_EndUndoAction();

	// Fix selection
	if (iSelStart == iSelEnd) {
		SciCall_SetSel(iSelStart + len, iSelStart + len);
	} else {
		Sci_Position iCurPos = SciCall_GetCurrentPos();
		Sci_Position iAnchorPos = SciCall_GetAnchor();

		if (iCurPos < iAnchorPos) {
			iCurPos = iSelStart + len;
			iAnchorPos = iSelEnd + len;
		} else {
			iAnchorPos = iSelStart + len;
			iCurPos = iSelEnd + len;
		}
		SciCall_SetSel(iAnchorPos, iCurPos);
	}

	EndWaitCursor();
	if (mszOpen != NULL) {
		NP2HeapFree(mszOpen);
	}
	if (mszClose != NULL) {
		NP2HeapFree(mszClose);
	}
}

//=============================================================================
//
// EditToggleLineComments()
//
void EditToggleLineComments(LPCWSTR pwszComment, BOOL bInsertAtStart) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	BeginWaitCursor();

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	Sci_Position iCurPos = SciCall_GetCurrentPos();

	char mszComment[256] = "";
	const UINT cpEdit = SciCall_GetCodePage();
	WideCharToMultiByte(cpEdit, 0, pwszComment, -1, mszComment, COUNTOF(mszComment), NULL, NULL);

	const int cchComment = (int)strlen(mszComment);
	const Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
	Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);

	if (iSelEnd <= SciCall_PositionFromLine(iLineEnd)) {
		if (iLineEnd - iLineStart >= 1) {
			iLineEnd--;
		}
	}

	Sci_Position iCommentCol = 0;
	if (!bInsertAtStart) {
		iCommentCol = 1024;
		for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			const Sci_Position iLineEndPos = SciCall_GetLineEndPosition(iLine);
			const Sci_Position iLineIndentPos = SciCall_GetLineIndentPosition(iLine);

			if (iLineIndentPos != iLineEndPos) {
				const Sci_Position iIndentColumn = SciCall_GetColumn(iLineIndentPos);
				iCommentCol = min_pos(iCommentCol, iIndentColumn);
			}
		}
	}

	SciCall_BeginUndoAction();
	int iAction = 0;

	for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
		const Sci_Position iIndentPos = SciCall_GetLineIndentPosition(iLine);
		BOOL bWhitespaceLine = FALSE;
		// a line with [space/tab] only
		if (iCommentCol && iIndentPos == SciCall_GetLineEndPosition(iLine)) {
			//continue;
			bWhitespaceLine = TRUE;
		}

		char tchBuf[32] = "";
		struct Sci_TextRange tr;
		tr.chrg.cpMin = (Sci_PositionCR)iIndentPos;
		tr.chrg.cpMax = tr.chrg.cpMin + min_i(31, cchComment);
		tr.lpstrText = tchBuf;
		SciCall_GetTextRange(&tr);;

		Sci_Position iCommentPos;
		if (_strnicmp(tchBuf, mszComment, cchComment) == 0) {
			int ch;
			switch (iAction) {
			case 0:
				iAction = 2;
				FALLTHROUGH_ATTR;
				// fall through
			case 2:
				iCommentPos = iIndentPos;
				// a line with [space/tab] comment only
				ch = SciCall_GetCharAt(iIndentPos + cchComment);
				if (ch == '\n' || ch == '\r') {
					iCommentPos = SciCall_PositionFromLine(iLine);
				}
				SciCall_SetTargetRange(iCommentPos, iIndentPos + cchComment);
				SciCall_ReplaceTarget(0, "");
				break;
			case 1:
				iCommentPos = SciCall_FindColumn(iLine, iCommentCol);
				ch = SciCall_GetCharAt(iCommentPos);
				if (ch == '\t' || ch == ' ') {
					SciCall_InsertText(iCommentPos, mszComment);
				}
				break;
			}
		} else {
			switch (iAction) {
			case 0:
				iAction = 1;
				FALLTHROUGH_ATTR;
				// fall through
			case 1:
				iCommentPos = SciCall_FindColumn(iLine, iCommentCol);
				if (!bWhitespaceLine || (iLineStart == iLineEnd)) {
					SciCall_InsertText(iCommentPos, mszComment);
				} else {
					char tchComment[1024] = "";
					Sci_Position tab = 0;
					Sci_Position count = iCommentCol;
					if (!bTabsAsSpaces && iTabWidth > 0) {
						tab = iCommentCol / iTabWidth;
						FillMemory(tchComment, tab, '\t');
						count -= tab * iTabWidth;
					}
					FillMemory(tchComment + tab, count, ' ');
					strcat(tchComment, mszComment);
					SciCall_InsertText(iCommentPos, tchComment);
				}
				break;
			case 2:
				break;
			}
		}
	}

	SciCall_EndUndoAction();

	if (iSelStart != iSelEnd) {
		Sci_Position iAnchorPos;
		if (iCurPos == iSelStart) {
			iCurPos = SciCall_PositionFromLine(iLineStart);
			iAnchorPos = SciCall_PositionFromLine(iLineEnd + 1);
		} else {
			iAnchorPos = SciCall_PositionFromLine(iLineStart);
			iCurPos = SciCall_PositionFromLine(iLineEnd + 1);
		}
		SciCall_SetSel(iAnchorPos, iCurPos);
	}

	EndWaitCursor();
}

//=============================================================================
//
// EditPadWithSpaces()
//
void EditPadWithSpaces(BOOL bSkipEmpty, BOOL bNoUndoGroup) {
	Sci_Position iMaxColumn = 0;
	BOOL bReducedSelection = FALSE;

	Sci_Position iSelStart = 0;
	Sci_Position iSelEnd = 0;

	Sci_Line iLineStart = 0;
	Sci_Line iLineEnd = 0;

	Sci_Line iRcCurLine = 0;
	Sci_Line iRcAnchorLine = 0;
	Sci_Position iRcCurCol = 0;
	Sci_Position iRcAnchorCol = 0;

	const BOOL bIsRectangular = SciCall_IsRectangleSelection();
	if (!bIsRectangular ) {
		iSelStart = SciCall_GetSelectionStart();
		iSelEnd = SciCall_GetSelectionEnd();

		iLineStart = SciCall_LineFromPosition(iSelStart);
		iLineEnd = SciCall_LineFromPosition(iSelEnd);

		if (iLineStart == iLineEnd) {
			iLineStart = 0;
			iLineEnd = SciCall_GetLineCount() - 1;
		} else {
			if (iSelEnd <= SciCall_PositionFromLine(iLineEnd)) {
				if (iLineEnd - iLineStart >= 1) {
					iLineEnd--;
					bReducedSelection = TRUE;
				}
			}
		}

		for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			const Sci_Position iPos = SciCall_GetLineEndPosition(iLine);
			iMaxColumn = max_pos(iMaxColumn, SciCall_GetColumn(iPos));
		}
	} else {
		const Sci_Position iCurPos = SciCall_GetCurrentPos();
		const Sci_Position iAnchorPos = SciCall_GetAnchor();

		iRcCurLine = SciCall_LineFromPosition(iCurPos);
		iRcAnchorLine = SciCall_LineFromPosition(iAnchorPos);

		iRcCurCol = SciCall_GetColumn(iCurPos);
		iRcAnchorCol = SciCall_GetColumn(iAnchorPos);

		iLineStart = 0;
		iLineEnd = SciCall_GetLineCount() - 1;

		for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			const Sci_Position iPos = SciCall_GetLineSelEndPosition(iLine);
			if (iPos != INVALID_POSITION) {
				iMaxColumn = max_pos(iMaxColumn, SciCall_GetColumn(iPos));
			}
		}
	}

	char *pmszPadStr = (char *)NP2HeapAlloc((iMaxColumn + 1) * sizeof(char));
	if (pmszPadStr) {
		FillMemory(pmszPadStr, NP2HeapSize(pmszPadStr), ' ');

		if (!bNoUndoGroup) {
			SciCall_BeginUndoAction();
		}

		for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			const Sci_Position iLineSelEndPos = SciCall_GetLineSelEndPosition(iLine);
			if (bIsRectangular && INVALID_POSITION == iLineSelEndPos) {
				continue;
			}

			const Sci_Position iPos = SciCall_GetLineEndPosition(iLine);
			if (bIsRectangular && iPos > iLineSelEndPos) {
				continue;
			}

			if (bSkipEmpty && SciCall_PositionFromLine(iLine) >= iPos) {
				continue;
			}

			const Sci_Position iPadLen = iMaxColumn - SciCall_GetColumn(iPos);

			SciCall_SetTargetRange(iPos, iPos);
			SciCall_ReplaceTarget(iPadLen, pmszPadStr);
		}

		NP2HeapFree(pmszPadStr);

		if (!bNoUndoGroup) {
			SciCall_EndUndoAction();
		}
	}

	if (!bIsRectangular && SciCall_LineFromPosition(iSelStart) != SciCall_LineFromPosition(iSelEnd)) {
		Sci_Position iCurPos = SciCall_GetCurrentPos();
		Sci_Position iAnchorPos = SciCall_GetAnchor();

		if (iCurPos < iAnchorPos) {
			iCurPos = SciCall_PositionFromLine(iLineStart);
			if (!bReducedSelection) {
				iAnchorPos = SciCall_GetLineEndPosition(iLineEnd);
			} else {
				iAnchorPos = SciCall_PositionFromLine(iLineEnd + 1);
			}
		} else {
			iAnchorPos = SciCall_PositionFromLine(iLineStart);
			if (!bReducedSelection) {
				iCurPos = SciCall_GetLineEndPosition(iLineEnd);
			} else {
				iCurPos = SciCall_PositionFromLine(iLineEnd + 1);
			}
		}
		SciCall_SetSel(iAnchorPos, iCurPos);
	} else if (bIsRectangular) {
		const Sci_Position iCurPos = SciCall_FindColumn(iRcCurLine, iRcCurCol);
		const Sci_Position iAnchorPos = SciCall_FindColumn(iRcAnchorLine, iRcAnchorCol);

		SciCall_SetRectangularSelectionCaret(iCurPos);
		SciCall_SetRectangularSelectionAnchor(iAnchorPos);
	}
}

//=============================================================================
//
// EditStripFirstCharacter()
//
void EditStripFirstCharacter(void) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
	Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);

	if (iLineStart != iLineEnd) {
		if (iSelStart > SciCall_PositionFromLine(iLineStart)) {
			iLineStart++;
		}

		if (iSelEnd <= SciCall_PositionFromLine(iLineEnd)) {
			iLineEnd--;
		}
	}

	SciCall_BeginUndoAction();

	for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
		const Sci_Position iPos = SciCall_PositionFromLine(iLine);
		if (SciCall_GetLineEndPosition(iLine) > iPos) {
			SciCall_SetTargetRange(iPos, SciCall_PositionAfter(iPos));
			SciCall_ReplaceTarget(0, "");
		}
	}
	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditStripLastCharacter()
//
void EditStripLastCharacter(void) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
	Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);

	if (iLineStart != iLineEnd) {
		if (iSelStart >= SciCall_GetLineEndPosition(iLineStart)) {
			iLineStart++;
		}

		if (iSelEnd < SciCall_GetLineEndPosition(iLineEnd)) {
			iLineEnd--;
		}
	}

	SciCall_BeginUndoAction();

	for (Sci_Line iLine = iLineStart; iLine <= iLineEnd; iLine++) {
		const Sci_Position iStartPos = SciCall_PositionFromLine(iLine);
		const Sci_Position iEndPos = SciCall_GetLineEndPosition(iLine);

		if (iEndPos > iStartPos) {
			SciCall_SetTargetRange(SciCall_PositionBefore(iEndPos), iEndPos);
			SciCall_ReplaceTarget(0, "");
		}
	}
	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditStripTrailingBlanks()
//
void EditStripTrailingBlanks(HWND hwnd, BOOL bIgnoreSelection) {
	// Check if there is any selection... simply use a regular expression replace!
	if (!bIgnoreSelection && !SciCall_IsSelectionEmpty()) {
		if (!SciCall_IsRectangleSelection()) {
#if NP2_USE_DESIGNATED_INITIALIZER
			EDITFINDREPLACE efrTrim = {
				.szFind = "[ \t]+$",
				.hwnd = hwnd,
				.fuFlags = SCFIND_REGEXP,
			};
#else
			EDITFINDREPLACE efrTrim = { "[ \t]+$", "", "", "", hwnd, SCFIND_REGEXP };
#endif
			if (EditReplaceAllInSelection(hwnd, &efrTrim, FALSE)) {
				return;
			}
		}
	}

	// Code from SciTE...
	SciCall_BeginUndoAction();
	const Sci_Line maxLines = SciCall_GetLineCount();
	for (Sci_Line line = 0; line < maxLines; line++) {
		const Sci_Position lineStart = SciCall_PositionFromLine(line);
		const Sci_Position lineEnd = SciCall_GetLineEndPosition(line);
		Sci_Position i = lineEnd - 1;
		int ch = SciCall_GetCharAt(i);
		while ((i >= lineStart) && (ch == ' ' || ch == '\t')) {
			i--;
			ch = SciCall_GetCharAt(i);
		}
		if (i < (lineEnd - 1)) {
			SciCall_SetTargetRange(i + 1, lineEnd);
			SciCall_ReplaceTarget(0, "");
		}
	}
	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditStripLeadingBlanks()
//
void EditStripLeadingBlanks(HWND hwnd, BOOL bIgnoreSelection) {
	// Check if there is any selection... simply use a regular expression replace!
	if (!bIgnoreSelection && !SciCall_IsSelectionEmpty()) {
		if (!SciCall_IsRectangleSelection()) {
#if NP2_USE_DESIGNATED_INITIALIZER
			EDITFINDREPLACE efrTrim = {
				.szFind = "^[ \t]+",
				.hwnd = hwnd,
				.fuFlags = SCFIND_REGEXP,
			};
#else
			EDITFINDREPLACE efrTrim = { "^[ \t]+", "", "", "", hwnd, SCFIND_REGEXP };
#endif
			if (EditReplaceAllInSelection(hwnd, &efrTrim, FALSE)) {
				return;
			}
		}
	}

	// Code from SciTE...
	SciCall_BeginUndoAction();
	const Sci_Line maxLines = SciCall_GetLineCount();
	for (Sci_Line line = 0; line < maxLines; line++) {
		const Sci_Position lineStart = SciCall_PositionFromLine(line);
		const Sci_Position lineEnd = SciCall_GetLineEndPosition(line);
		Sci_Position i = lineStart;
		int ch = SciCall_GetCharAt(i);
		while ((i <= lineEnd - 1) && (ch == ' ' || ch == '\t')) {
			i++;
			ch = SciCall_GetCharAt(i);
		}
		if (i > lineStart) {
			SciCall_SetTargetRange(lineStart, i);
			SciCall_ReplaceTarget(0, "");
		}
	}
	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditCompressSpaces()
//
void EditCompressSpaces(void) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();

	char *pszIn;
	char *pszOut;
	BOOL bIsLineStart;
	BOOL bIsLineEnd;

	if (iSelStart != iSelEnd) {
		const Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
		const Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);
		const Sci_Position cch = SciCall_GetSelTextLength();
		pszIn = (char *)NP2HeapAlloc(cch);
		pszOut = (char *)NP2HeapAlloc(cch);
		SciCall_GetSelText(pszIn);
		bIsLineStart = (iSelStart == SciCall_PositionFromLine(iLineStart));
		bIsLineEnd = (iSelEnd == SciCall_GetLineEndPosition(iLineEnd));
	} else {
		const Sci_Position cch = SciCall_GetLength() + 1;
		pszIn = (char *)NP2HeapAlloc(cch);
		pszOut = (char *)NP2HeapAlloc(cch);
		SciCall_GetText(cch, pszIn);
		bIsLineStart = TRUE;
		bIsLineEnd = TRUE;
	}

	BOOL bModified = FALSE;
	char *ci;
	char *co = pszOut;
	for (ci = pszIn; *ci; ci++) {
		if (*ci == ' ' || *ci == '\t') {
			if (*ci == '\t') {
				bModified = TRUE;
			}
			while (*(ci + 1) == ' ' || *(ci + 1) == '\t') {
				ci++;
				bModified = TRUE;
			}
			if (!bIsLineStart && (*(ci + 1) != '\n' && *(ci + 1) != '\r')) {
				*co++ = ' ';
			} else {
				bModified = TRUE;
			}
		} else {
			if (*ci == '\n' || *ci == '\r') {
				bIsLineStart = TRUE;
			} else {
				bIsLineStart = FALSE;
			}
			*co++ = *ci;
		}
	}
	if (bIsLineEnd && co > pszOut && *(co - 1) == ' ') {
		*--co = 0;
		bModified = TRUE;
	}

	if (bModified) {
		if (iSelStart != iSelEnd) {
			SciCall_TargetFromSelection();
		} else {
			SciCall_TargetWholeDocument();
		}
		SciCall_BeginUndoAction();
		SciCall_ReplaceTarget(-1, pszOut);
		if (iCurPos > iAnchorPos) {
			iCurPos = SciCall_GetTargetEnd();
			iAnchorPos = SciCall_GetTargetStart();
		} else if (iCurPos < iAnchorPos) {
			iCurPos = SciCall_GetTargetStart();
			iAnchorPos = SciCall_GetTargetEnd();
		}
		SciCall_SetSel(iAnchorPos, iCurPos);
		SciCall_EndUndoAction();
	}

	NP2HeapFree(pszIn);
	NP2HeapFree(pszOut);
}

//=============================================================================
//
// EditRemoveBlankLines()
//
void EditRemoveBlankLines(BOOL bMerge) {
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	Sci_Position iSelStart = SciCall_GetSelectionStart();
	Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	if (iSelStart == iSelEnd) {
		iSelStart = 0;
		iSelEnd = SciCall_GetLength();
	}

	Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
	Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);

	if (iSelStart > SciCall_PositionFromLine(iLineStart)) {
		iLineStart++;
	}

	if (iSelEnd <= SciCall_PositionFromLine(iLineEnd) && iLineEnd != SciCall_GetLineCount() - 1) {
		iLineEnd--;
	}

	SciCall_BeginUndoAction();

	for (Sci_Line iLine = iLineStart; iLine <= iLineEnd;) {
		Sci_Line nBlanks = 0;
		while (iLine + nBlanks <= iLineEnd && SciCall_PositionFromLine(iLine + nBlanks) == SciCall_GetLineEndPosition(iLine + nBlanks)) {
			nBlanks++;
		}

		if (nBlanks == 0 || (nBlanks == 1 && bMerge)) {
			iLine += nBlanks + 1;
		} else {
			if (bMerge) {
				nBlanks--;
			}

			const Sci_Position iTargetStart = SciCall_PositionFromLine(iLine);
			const Sci_Position iTargetEnd = SciCall_PositionFromLine(iLine + nBlanks);

			SciCall_SetTargetRange(iTargetStart, iTargetEnd);
			SciCall_ReplaceTarget(0, "");

			if (bMerge) {
				iLine++;
			}
			iLineEnd -= nBlanks;
		}
	}
	SciCall_EndUndoAction();
}

//=============================================================================
//
// EditWrapToColumn()
//
void EditWrapToColumn(int nColumn/*, int nTabWidth*/) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	const Sci_Line iLine = SciCall_LineFromPosition(iSelStart);
	iSelStart = SciCall_PositionFromLine(iLine);

	const Sci_Position iSelCount = iSelEnd - iSelStart;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1 + 2);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc((iSelCount + 1 + 2) * sizeof(WCHAR));

	struct Sci_TextRange tr;
	tr.chrg.cpMin = (Sci_PositionCR)iSelStart;
	tr.chrg.cpMax = (Sci_PositionCR)iSelEnd;
	tr.lpstrText = pszText;
	SciCall_GetTextRange(&tr);;

	const UINT cpEdit = SciCall_GetCodePage();
	const int cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));
	NP2HeapFree(pszText);

	LPWSTR pszConvW = (LPWSTR)NP2HeapAlloc(cchTextW * sizeof(WCHAR) * 3 + 2);

	WCHAR wszEOL[] = L"\r\n";
	int cchEOL = 2;
	const int iEOLMode = SciCall_GetEOLMode();
	if (iEOLMode == SC_EOL_CR) {
		cchEOL = 1;
	} else if (iEOLMode == SC_EOL_LF) {
		cchEOL = 1;
		wszEOL[0] = L'\n';
	}

#define ISDELIMITER(wc) StrChr(L",;.:-+%&\xA6|/*?!\"\'~\xB4#=", wc)
#define ISWHITE(wc) StrChr(L" \t", wc)
#define ISWORDEND(wc) (/*ISDELIMITER(wc) ||*/ StrChr(L" \t\r\n", wc))

	int cchConvW = 0;
	int iLineLength = 0;
	BOOL bModified = FALSE;
	for (int iTextW = 0; iTextW < cchTextW; iTextW++) {
		const WCHAR w = pszTextW[iTextW];

		//if (ISDELIMITER(w)) {
		//	int iNextWordLen = 0;
		//	WCHAR w2 = pszTextW[iTextW + 1];

		//	if (iLineLength + iNextWordLen + 1 > nColumn) {
		//		pszConvW[cchConvW++] = wszEOL[0];
		//		if (cchEOL > 1)
		//			pszConvW[cchConvW++] = wszEOL[1];
		//		iLineLength = 0;
		//		bModified = TRUE;
		//	}

		//	while (w2 != L'\0' && !ISWORDEND(w2)) {
		//		iNextWordLen++;
		//		w2 = pszTextW[iTextW + iNextWordLen + 1];
		//	}

		//	if (ISDELIMITER(w2) && iNextWordLen > 0) // delimiters go with the word
		//		iNextWordLen++;

		//	pszConvW[cchConvW++] = w;
		//	iLineLength++;

		//	if (iNextWordLen > 0) {
		//		if (iLineLength + iNextWordLen + 1 > nColumn) {
		//			pszConvW[cchConvW++] = wszEOL[0];
		//			if (cchEOL > 1)
		//				pszConvW[cchConvW++] = wszEOL[1];
		//			iLineLength = 0;
		//			bModified = TRUE;
		//		}
		//	}
		//}

		if (ISWHITE(w)) {
			while (pszTextW[iTextW + 1] == L' ' || pszTextW[iTextW + 1] == L'\t') {
				iTextW++;
				bModified = TRUE;
			} // Modified: left out some whitespaces

			WCHAR w2 = pszTextW[iTextW + 1];
			int iNextWordLen = 0;

			while (w2 != L'\0' && !ISWORDEND(w2)) {
				iNextWordLen++;
				w2 = pszTextW[iTextW + iNextWordLen + 1];
			}

			//if (ISDELIMITER(w2) /*&& iNextWordLen > 0*/) // delimiters go with the word
			//	iNextWordLen++;
			if (iNextWordLen > 0) {
				if (iLineLength + iNextWordLen + 1 > nColumn) {
					pszConvW[cchConvW++] = wszEOL[0];
					if (cchEOL > 1) {
						pszConvW[cchConvW++] = wszEOL[1];
					}
					iLineLength = 0;
					bModified = TRUE;
				} else {
					if (iLineLength > 0) {
						pszConvW[cchConvW++] = L' ';
						iLineLength++;
					}
				}
			}
		} else {
			pszConvW[cchConvW++] = w;
			if (w == L'\r' || w == L'\n') {
				iLineLength = 0;
			} else {
				iLineLength++;
			}
		}
	}

	NP2HeapFree(pszTextW);

	if (bModified) {
		pszText = (char *)NP2HeapAlloc(cchConvW * kMaxMultiByteCount);
		const int cchConvM = WideCharToMultiByte(cpEdit, 0, pszConvW, cchConvW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + cchConvM;
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + cchConvM;
		}

		SciCall_BeginUndoAction();

		SciCall_SetTargetRange(iSelStart, iSelEnd);
		SciCall_ReplaceTarget(cchConvM, pszText);
		//SciCall_Clear();
		//SciCall_AddText(cchConvW, pszText);
		SciCall_SetSel(iAnchorPos, iCurPos);

		SciCall_EndUndoAction();

		NP2HeapFree(pszText);
	}

	NP2HeapFree(pszConvW);
}

//=============================================================================
//
// EditJoinLinesEx()
//
void EditJoinLinesEx(void) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return;
	}

	Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	const Sci_Line iLine = SciCall_LineFromPosition(iSelStart);
	iSelStart = SciCall_PositionFromLine(iLine);

	const Sci_Position iSelCount = iSelEnd - iSelStart;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1 + 2);
	char *pszJoin = (char *)NP2HeapAlloc(NP2HeapSize(pszText));

	struct Sci_TextRange tr;
	tr.chrg.cpMin = (Sci_PositionCR)iSelStart;
	tr.chrg.cpMax = (Sci_PositionCR)iSelEnd;
	tr.lpstrText = pszText;
	SciCall_GetTextRange(&tr);;

	char szEOL[] = "\r\n";
	int cchEOL = 2;
	const int iEOLMode = SciCall_GetEOLMode();
	if (iEOLMode == SC_EOL_CR) {
		cchEOL = 1;
	} else if (iEOLMode == SC_EOL_LF) {
		cchEOL = 1;
		szEOL[0] = '\n';
	}

	Sci_Position cchJoin = 0;
	BOOL bModified = FALSE;
	for (Sci_Position i = 0; i < iSelCount; i++) {
		if (pszText[i] == '\r' || pszText[i] == '\n') {
			if (pszText[i] == '\r' && pszText[i + 1] == '\n') {
				i++;
			}
			if (!strchr("\r\n", pszText[i + 1]) && pszText[i + 1] != 0) {
				pszJoin[cchJoin++] = ' ';
				bModified = TRUE;
			} else {
				while (strchr("\r\n", pszText[i + 1])) {
					i++;
					bModified = TRUE;
				}
				if (pszText[i + 1] != 0) {
					pszJoin[cchJoin++] = szEOL[0];
					if (cchEOL > 1) {
						pszJoin[cchJoin++] = szEOL[1];
					}
					if (cchJoin > cchEOL) {
						pszJoin[cchJoin++] = szEOL[0];
						if (cchEOL > 1) {
							pszJoin[cchJoin++] = szEOL[1];
						}
					}
				}
			}
		} else {
			pszJoin[cchJoin++] = pszText[i];
		}
	}

	NP2HeapFree(pszText);

	if (bModified) {
		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + cchJoin;
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + cchJoin;
		}

		SciCall_BeginUndoAction();

		SciCall_SetTargetRange(iSelStart, iSelEnd);
		SciCall_ReplaceTarget(cchJoin, pszJoin);
		SciCall_SetSel(iAnchorPos, iCurPos);

		SciCall_EndUndoAction();
	}

	NP2HeapFree(pszJoin);
}

//=============================================================================
//
// EditSortLines()
//
typedef struct _SORTLINE {
	WCHAR *pwszLine;
	WCHAR *pwszSortEntry;
} SORTLINE;

typedef int (__stdcall *FNSTRCMP)(LPCWSTR, LPCWSTR);
typedef int (__cdecl *QSortCmp)(const void *, const void *);

static int __cdecl CmpStd(const void *p1, const void *p2) {
	const SORTLINE *s1 = (const SORTLINE *)p1;
	const SORTLINE *s2 = (const SORTLINE *)p2;
	const int cmp = StrCmpW(s1->pwszSortEntry, s2->pwszSortEntry);
	return cmp ? cmp : StrCmpW(s1->pwszLine, s2->pwszLine);
}

static int __cdecl CmpIStd(const void *p1, const void *p2) {
	const SORTLINE *s1 = (const SORTLINE *)p1;
	const SORTLINE *s2 = (const SORTLINE *)p2;
	const int cmp = StrCmpIW(s1->pwszSortEntry, s2->pwszSortEntry);
	return cmp ? cmp : StrCmpIW(s1->pwszLine, s2->pwszLine);
}

static int __cdecl CmpStdRev(const void *p1, const void *p2) {
	return CmpStd(p2, p1);
}

static int __cdecl CmpIStdRev(const void *p1, const void *p2) {
	return CmpIStd(p2, p1);
}

static int __cdecl CmpLogical(const void *p1, const void *p2) {
	const SORTLINE *s1 = (const SORTLINE *)p1;
	const SORTLINE *s2 = (const SORTLINE *)p2;
	int cmp = StrCmpLogicalW(s1->pwszSortEntry, s1->pwszSortEntry);
	if (cmp == 0) {
		cmp = StrCmpLogicalW(s2->pwszLine, s2->pwszLine);
	}
	return cmp ? cmp : CmpStd(p1, p2);
}

static int __cdecl CmpILogical(const void *p1, const void *p2) {
	const SORTLINE *s1 = (const SORTLINE *)p1;
	const SORTLINE *s2 = (const SORTLINE *)p2;
	int cmp = StrCmpLogicalW(s1->pwszSortEntry, s1->pwszSortEntry);
	if (cmp == 0) {
		cmp = StrCmpLogicalW(s2->pwszLine, s2->pwszLine);
	}
	return cmp ? cmp : CmpIStd(p1, p2);
}

static int __cdecl CmpLogicalRev(const void *p1, const void *p2) {
	return CmpLogical(p2, p1);
}

static int __cdecl CmpILogicalRev(const void *p1, const void *p2) {
	return CmpILogical(p2, p1);
}

void EditSortLines(int iSortFlags) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	Sci_Position iAnchorPos = SciCall_GetAnchor();
	if (iCurPos == iAnchorPos) {
		return;
	}

	Sci_Line iRcCurLine = 0;
	Sci_Line iRcAnchorLine = 0;
	Sci_Position iRcCurCol = 0;
	Sci_Position iRcAnchorCol = 0;

	Sci_Position iSelStart = 0;
	Sci_Line iLineStart;
	Sci_Line iLineEnd;
	Sci_Position iSortColumn;

	const BOOL bIsRectangular = SciCall_IsRectangleSelection();
	if (bIsRectangular) {
		iRcCurLine = SciCall_LineFromPosition(iCurPos);
		iRcAnchorLine = SciCall_LineFromPosition(iAnchorPos);

		iRcCurCol = SciCall_GetColumn(iCurPos);
		iRcAnchorCol = SciCall_GetColumn(iAnchorPos);

		iLineStart = min_pos(iRcCurLine, iRcAnchorLine);
		iLineEnd = max_pos(iRcCurLine, iRcAnchorLine);
		iSortColumn = min_pos(iRcCurCol, iRcAnchorCol);
	} else {
		iSelStart = SciCall_GetSelectionStart();
		const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

		const Sci_Line iLine = SciCall_LineFromPosition(iSelStart);
		iSelStart = SciCall_PositionFromLine(iLine);

		iLineStart = SciCall_LineFromPosition(iSelStart);
		iLineEnd = SciCall_LineFromPosition(iSelEnd);

		if (iSelEnd <= SciCall_PositionFromLine(iLineEnd)) {
			iLineEnd--;
		}

		iSortColumn = SciCall_GetColumn(iCurPos);
	}

	const Sci_Line iLineCount = iLineEnd - iLineStart + 1;
	if (iLineCount < 2) {
		return;
	}

	char mszEOL[] = "\r\n";
	const UINT cpEdit = SciCall_GetCodePage();
	const int iEOLMode = SciCall_GetEOLMode();
	if (iEOLMode == SC_EOL_CR) {
		mszEOL[1] = 0;
	} else if (iEOLMode == SC_EOL_LF) {
		mszEOL[0] = '\n';
		mszEOL[1] = 0;
	}

	SciCall_BeginUndoAction();
	if (bIsRectangular) {
		EditPadWithSpaces(!(iSortFlags & SORT_SHUFFLE), TRUE);
	}

	SORTLINE *pLines = (SORTLINE *)NP2HeapAlloc(sizeof(SORTLINE) * iLineCount);
	Sci_Position cchTotal = 0;
	Sci_Position ichlMax = 3;
	for (Sci_Line i = 0, iLine = iLineStart; iLine <= iLineEnd; i++, iLine++) {
		const Sci_Position cchm = SciCall_GetLineLength(iLine);
		char *pmsz = (char *)NP2HeapAlloc(cchm + 1);
		SciCall_GetLine(iLine, pmsz);
		StrTrimA(pmsz, "\r\n");
		cchTotal += cchm;
		ichlMax = max_pos(ichlMax, cchm);

		const int cchw = MultiByteToWideChar(cpEdit, 0, pmsz, -1, NULL, 0) - 1;
		if (cchw > 0) {
			pLines[i].pwszLine = (LPWSTR)LocalAlloc(LPTR, sizeof(WCHAR) * (cchw + 1));
			MultiByteToWideChar(cpEdit, 0, pmsz, -1, pLines[i].pwszLine, (int)(LocalSize(pLines[i].pwszLine) / sizeof(WCHAR)));
			pLines[i].pwszSortEntry = pLines[i].pwszLine;

			if (iSortFlags & SORT_COLUMN) {
				Sci_Position col = 0;
				Sci_Position tabs = iTabWidth;
				while (*(pLines[i].pwszSortEntry)) {
					if (*(pLines[i].pwszSortEntry) == L'\t') {
						if (col + tabs <= iSortColumn) {
							col += tabs;
							tabs = iTabWidth;
							pLines[i].pwszSortEntry = CharNext(pLines[i].pwszSortEntry);
						} else {
							break;
						}
					} else if (col < iSortColumn) {
						col++;
						if (--tabs == 0) {
							tabs = iTabWidth;
						}
						pLines[i].pwszSortEntry = CharNext(pLines[i].pwszSortEntry);
					} else {
						break;
					}
				}
			}
		} else {
			pLines[i].pwszLine = StrDup(L"");
			pLines[i].pwszSortEntry = pLines[i].pwszLine;
		}
		NP2HeapFree(pmsz);
	}

	if (iSortFlags & SORT_DESCENDING) {
		QSortCmp cmpFunc = (iSortFlags & SORT_LOGICAL)
			? ((iSortFlags & SORT_NOCASE) ? CmpILogicalRev : CmpLogicalRev)
			: ((iSortFlags & SORT_NOCASE) ? CmpIStdRev : CmpStdRev);
		qsort(pLines, iLineCount, sizeof(SORTLINE), cmpFunc);
	} else if (iSortFlags & SORT_SHUFFLE) {
		srand(GetTickCount());
		for (Sci_Line i = iLineCount - 1; i > 0; i--) {
			const Sci_Line j = rand() % i;
			const SORTLINE sLine = pLines[i];
			pLines[i] = pLines[j];
			pLines[j] = sLine;
		}
	} else {
		QSortCmp cmpFunc = (iSortFlags & SORT_LOGICAL)
			? ((iSortFlags & SORT_NOCASE) ? CmpILogical : CmpLogical)
			: ((iSortFlags & SORT_NOCASE) ? CmpIStd : CmpStd);
		qsort(pLines, iLineCount, sizeof(SORTLINE), cmpFunc);
	}

	char *pmszResult = (char *)NP2HeapAlloc(cchTotal + 2 * iLineCount + 1);
	char *pmszBuf = (char *)NP2HeapAlloc(ichlMax + 1);
	const int cbPmszBuf = (int)NP2HeapSize(pmszBuf);
	const int cbPmszResult = (int)NP2HeapSize(pmszResult);
	FNSTRCMP pfnStrCmp = (iSortFlags & SORT_NOCASE) ? StrCmpIW : StrCmpW;

	BOOL bLastDup = FALSE;
	for (Sci_Line i = 0; i < iLineCount; i++) {
		if (pLines[i].pwszLine && ((iSortFlags & SORT_SHUFFLE) || StrNotEmpty(pLines[i].pwszLine))) {
			BOOL bDropLine = FALSE;
			if (!(iSortFlags & SORT_SHUFFLE)) {
				if ((iSortFlags & SORT_MERGEDUP) || (iSortFlags & SORT_UNIQDUP) || (iSortFlags & SORT_UNIQUNIQ)) {
					if (i < iLineCount - 1) {
						if (pfnStrCmp(pLines[i].pwszLine, pLines[i + 1].pwszLine) == 0) {
							bLastDup = TRUE;
							bDropLine = ((iSortFlags & SORT_MERGEDUP) || (iSortFlags & SORT_UNIQDUP));
						} else {
							bDropLine = (!bLastDup && (iSortFlags & SORT_UNIQUNIQ)) || (bLastDup && (iSortFlags & SORT_UNIQDUP));
							bLastDup = FALSE;
						}
					} else {
						bDropLine = (!bLastDup && (iSortFlags & SORT_UNIQUNIQ)) || (bLastDup && (iSortFlags & SORT_UNIQDUP));
						bLastDup = FALSE;
					}
				}
			}

			if (!bDropLine) {
				WideCharToMultiByte(cpEdit, 0, pLines[i].pwszLine, -1, pmszBuf, cbPmszBuf, NULL, NULL);
				strncat(pmszResult, pmszBuf, cbPmszResult);
				strncat(pmszResult, mszEOL, cbPmszResult);
			}
		}
	}

	NP2HeapFree(pmszBuf);
	for (Sci_Line i = 0; i < iLineCount; i++) {
		if (pLines[i].pwszLine) {
			LocalFree(pLines[i].pwszLine);
		}
	}
	NP2HeapFree(pLines);

	if (!bIsRectangular) {
		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + strlen(pmszResult);
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + strlen(pmszResult);
		}
	}

	SciCall_SetTargetRange(SciCall_PositionFromLine(iLineStart), SciCall_PositionFromLine(iLineEnd + 1));
	SciCall_ReplaceTarget(strlen(pmszResult), pmszResult);
	SciCall_EndUndoAction();

	NP2HeapFree(pmszResult);

	if (!bIsRectangular) {
		SciCall_SetSel(iAnchorPos, iCurPos);
	} else {
		const Sci_Position iTargetStart = SciCall_GetTargetStart();
		Sci_Position iTargetEnd = SciCall_GetTargetEnd();
		SciCall_ClearSelections();
		if (iTargetStart != iTargetEnd) {
			iTargetEnd -= strlen(mszEOL);
			if (iRcAnchorLine > iRcCurLine) {
				iCurPos = SciCall_FindColumn(SciCall_LineFromPosition(iTargetStart), iRcCurCol);
				iAnchorPos = SciCall_FindColumn(SciCall_LineFromPosition(iTargetEnd), iRcAnchorCol);
			} else {
				iCurPos = SciCall_FindColumn(SciCall_LineFromPosition(iTargetEnd), iRcCurCol);
				iAnchorPos = SciCall_FindColumn(SciCall_LineFromPosition(iTargetStart), iRcAnchorCol);
			}
			if (iCurPos != iAnchorPos) {
				SciCall_SetRectangularSelectionCaret(iCurPos);
				SciCall_SetRectangularSelectionAnchor(iAnchorPos);
			} else {
				SciCall_SetSel(iTargetStart, iTargetStart);
			}
		} else {
			SciCall_SetSel(iTargetStart, iTargetStart);
		}
	}
}

//=============================================================================
//
// EditJumpTo()
//
void EditJumpTo(Sci_Line iNewLine, Sci_Position iNewCol) {
	const Sci_Line iMaxLine = SciCall_GetLineCount();

	// Jumpt to end with line set to -1
	if (iNewLine < 0) {
		SciCall_DocumentEnd();
		return;
	}

	// Line maximum is iMaxLine
	iNewLine = min_pos(iNewLine, iMaxLine);

	// Column minimum is 1
	iNewCol = max_pos(iNewCol, 1);

	if (iNewLine > 0 && iNewLine <= iMaxLine && iNewCol > 0) {
		Sci_Position iNewPos = SciCall_PositionFromLine(iNewLine - 1);
		const Sci_Position iLineEndPos = SciCall_GetLineEndPosition(iNewLine - 1);

		while (iNewCol - 1 > SciCall_GetColumn(iNewPos)) {
			if (iNewPos >= iLineEndPos) {
				break;
			}

			iNewPos = SciCall_PositionAfter(iNewPos);
		}

		iNewPos = min_pos(iNewPos, iLineEndPos);
		EditSelectEx(-1, iNewPos); // SciCall_GotoPos(pos) is equivalent to SciCall_SetSel(-1, pos)
		SciCall_ChooseCaretX();
	}
}

//=============================================================================
//
// EditSelectEx()
//
void EditSelectEx(Sci_Position iAnchorPos, Sci_Position iCurrentPos) {
	const Sci_Line iNewLine = SciCall_LineFromPosition(iCurrentPos);
	const Sci_Line iAnchorLine = SciCall_LineFromPosition(iAnchorPos);

	// Ensure that the first and last lines of a selection are always unfolded
	// This needs to be done *before* the SciCall_SetSel() message
	SciCall_EnsureVisible(iAnchorLine);
	if (iAnchorLine != iNewLine) {
		SciCall_EnsureVisible(iNewLine);
	}

	SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
	SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
	SciCall_SetSel(iAnchorPos, iCurrentPos);
	SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
	SciCall_SetYCaretPolicy(CARET_EVEN, 0);
}

//=============================================================================
//
// EditFixPositions()
//
void EditFixPositions(void) {
	const Sci_Position iMaxPos = SciCall_GetLength();
	Sci_Position iCurrentPos = SciCall_GetCurrentPos();
	const Sci_Position iAnchorPos = SciCall_GetAnchor();

	if (iCurrentPos > 0 && iCurrentPos < iMaxPos) {
		const Sci_Position iNewPos = SciCall_PositionAfter(SciCall_PositionBefore(iCurrentPos));
		if (iNewPos != iCurrentPos) {
			SciCall_SetCurrentPos(iNewPos);
			iCurrentPos = iNewPos;
		}
	}

	if (iAnchorPos != iCurrentPos && iAnchorPos > 0 && iAnchorPos < iMaxPos) {
		const Sci_Position iNewPos = SciCall_PositionAfter(SciCall_PositionBefore(iAnchorPos));
		if (iNewPos != iAnchorPos) {
			SciCall_SetAnchor(iNewPos);
		}
	}
}

//=============================================================================
//
// EditEnsureSelectionVisible()
//
void EditEnsureSelectionVisible(void) {
	const Sci_Position iAnchorPos = SciCall_GetAnchor();
	const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
	EditSelectEx(iAnchorPos, iCurrentPos);
}

void EditEnsureConsistentLineEndings(void) {
	const int iEOLMode = SciCall_GetEOLMode();
	if (iEOLMode == SC_EOL_CRLF) {
#if defined(_WIN64)
		const int options = SciCall_GetDocumentOptions();
		if (!(options & SC_DOCUMENTOPTION_TEXT_LARGE)) {
			const Sci_Position dwLength = SciCall_GetLength() + SciCall_GetLineCount();
			if (dwLength >= INT_MAX) {
				return;
			}
		}
#else
		const DWORD dwLength = (DWORD)SciCall_GetLength() + (DWORD)SciCall_GetLineCount();
		if (dwLength >= INT_MAX) {
			return;
		}
#endif
	}

	SciCall_ConvertEOLs(iEOLMode);
	EditFixPositions();
}

//=============================================================================
//
// EditGetExcerpt()
//
void EditGetExcerpt(LPWSTR lpszExcerpt, DWORD cchExcerpt) {
	if (SciCall_IsSelectionEmpty() || SciCall_IsRectangleSelection()) {
		lstrcpy(lpszExcerpt, L"");
		return;
	}

	WCHAR tch[256] = L"";
	DWORD cch = 0;
	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = min_pos(min_pos(SciCall_GetSelectionEnd(), iSelStart + COUNTOF(tch)), SciCall_GetLength());
	const Sci_Position iSelCount = iSelEnd - iSelStart;

	char *pszText = (char *)NP2HeapAlloc(iSelCount + 2);
	LPWSTR pszTextW = (LPWSTR)NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

	struct Sci_TextRange tr;
	tr.chrg.cpMin = (Sci_PositionCR)iSelStart;
	tr.chrg.cpMax = (Sci_PositionCR)iSelEnd;
	tr.lpstrText = pszText;
	SciCall_GetTextRange(&tr);;
	const UINT cpEdit = SciCall_GetCodePage();
	MultiByteToWideChar(cpEdit, 0, pszText, (int)iSelCount, pszTextW, (int)(NP2HeapSize(pszTextW) / sizeof(WCHAR)));

	for (WCHAR *p = pszTextW; *p && cch < COUNTOF(tch) - 1; p++) {
		if (*p == L'\r' || *p == L'\n' || *p == L'\t' || *p == L' ') {
			tch[cch++] = L' ';
			while (*(p + 1) == L'\r' || *(p + 1) == L'\n' || *(p + 1) == L'\t' || *(p + 1) == L' ') {
				p++;
			}
		} else {
			tch[cch++] = *p;
		}
	}
	tch[cch++] = L'\0';
	StrTrim(tch, L" ");

	if (cch == 1) {
		lstrcpy(tch, L" ... ");
	}

	if (cch > cchExcerpt) {
		tch[cchExcerpt - 2] = L'.';
		tch[cchExcerpt - 3] = L'.';
		tch[cchExcerpt - 4] = L'.';
	}
	lstrcpyn(lpszExcerpt, tch, cchExcerpt);

	NP2HeapFree(pszText);
	NP2HeapFree(pszTextW);
}

void EditSelectWord(void) {
	const Sci_Position iPos = SciCall_GetCurrentPos();

	if (SciCall_IsSelectionEmpty()) {
		Sci_Position iWordStart = SciCall_WordStartPosition(iPos, TRUE);
		Sci_Position iWordEnd = SciCall_WordEndPosition(iPos, TRUE);

		if (iWordStart == iWordEnd) {// we are in whitespace salad...
			iWordStart = SciCall_WordEndPosition(iPos, FALSE);
			iWordEnd = SciCall_WordEndPosition(iWordStart, TRUE);
			if (iWordStart != iWordEnd) {
				//if (IsDocWordChar(SciCall_GetCharAt(iWordStart - 1))) {
				//	--iWordStart;
				//}
				SciCall_SetSel(iWordStart, iWordEnd);
			}
		} else {
			//if (IsDocWordChar(SciCall_GetCharAt(iWordStart - 1))) {
			//	--iWordStart;
			//}
			SciCall_SetSel(iWordStart, iWordEnd);
		}

		if (!SciCall_IsSelectionEmpty()) {
			return;
		}
	}

	const Sci_Line iLine = SciCall_LineFromPosition(iPos);
	const Sci_Position iLineStart = SciCall_GetLineIndentPosition(iLine);
	const Sci_Position iLineEnd = SciCall_GetLineEndPosition(iLine);
	SciCall_SetSel(iLineStart, iLineEnd);
}

void EditSelectLine(void) {
	Sci_Position iSelStart = SciCall_GetSelectionStart();
	Sci_Position iSelEnd = SciCall_GetSelectionStart();
	const Sci_Line iLineStart = SciCall_LineFromPosition(iSelStart);
	const Sci_Line iLineEnd = SciCall_LineFromPosition(iSelEnd);
	iSelStart = SciCall_PositionFromLine(iLineStart);
	iSelEnd = SciCall_PositionFromLine(iLineEnd + 1);
	SciCall_SetSel(iSelStart, iSelEnd);
	SciCall_ChooseCaretX();
}

static LRESULT CALLBACK AddBackslashEditProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	UNREFERENCED_PARAMETER(uIdSubclass);
	UNREFERENCED_PARAMETER(dwRefData);

	switch (umsg) {
	case WM_PASTE: {
		BOOL done = FALSE;
		LPWSTR lpsz = EditGetClipboardTextW();
		if (StrNotEmpty(lpsz)) {
			const int len = lstrlen(lpsz);
			LPWSTR lpszEsc = (LPWSTR)NP2HeapAlloc((2*len + 1)*sizeof(WCHAR));
			if (lpszEsc != NULL) {
				AddBackslashW(lpszEsc, lpsz);
				SendMessage(hwnd, EM_REPLACESEL, TRUE, (LPARAM)(lpszEsc));
				NP2HeapFree(lpszEsc);
				done = TRUE;
			}
		}
		if (lpsz != NULL) {
			NP2HeapFree(lpsz);
		}
		if (done) {
			return TRUE;
		}
	} break;
	}

	return DefSubclassProc(hwnd, umsg, wParam, lParam);
}

void AddBackslashComboBoxSetup(HWND hwndDlg, int nCtlId) {
	HWND hwnd = GetDlgItem(hwndDlg, nCtlId);
	COMBOBOXINFO info;
	info.cbSize = sizeof(COMBOBOXINFO);
	if (GetComboBoxInfo(hwnd, &info)) {
		SetWindowSubclass(info.hwndItem, AddBackslashEditProc, 0, 0);
	}
}

extern BOOL bFindReplaceTransparentMode;
extern int iFindReplaceOpacityLevel;
//=============================================================================
//
// EditFindReplaceDlgProc()
//
static INT_PTR CALLBACK EditFindReplaceDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static BOOL bSwitchedFindReplace = FALSE;
	static int xFindReplaceDlgSave;
	static int yFindReplaceDlgSave;
	static EDITFINDREPLACE efrSave;

	WCHAR tch[NP2_FIND_REPLACE_LIMIT + 32];

	switch (umsg) {
	case WM_INITDIALOG: {
		static BOOL bFirstTime = TRUE;

		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		ResizeDlg_InitX(hwnd, cxFindReplaceDlg, IDC_RESIZEGRIP2);
		AddBackslashComboBoxSetup(hwnd, IDC_FINDTEXT);

		LPEDITFINDREPLACE lpefr = (LPEDITFINDREPLACE)lParam;
		// Get the current code page for Unicode conversion
		const UINT cpEdit = SciCall_GetCodePage();

		// Load MRUs
		for (int i = 0; i < MRU_GetCount(mruFind); i++) {
			MRU_Enum(mruFind, i, tch, COUNTOF(tch));
			SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_ADDSTRING, 0, (LPARAM)tch);
		}

		if (!bSwitchedFindReplace) {
			Sci_Position cchSelection = SciCall_GetSelTextLength();
			if (cchSelection <= NP2_FIND_REPLACE_LIMIT) {
				char *lpszSelection = (char *)NP2HeapAlloc(cchSelection);
				SciCall_GetSelText(lpszSelection);

				// First time you bring up find/replace dialog, copy content from clipboard to find box (but only if nothing is selected in the editor)
				if (StrIsEmptyA(lpszSelection) && bFirstTime) {
					char *pClip = EditGetClipboardText(lpefr->hwnd);
					if (pClip != NULL) {
						const size_t len = strlen(pClip);
						if (len > 0 && len <= NP2_FIND_REPLACE_LIMIT) {
							NP2HeapFree(lpszSelection);
							lpszSelection = (char *)NP2HeapAlloc(len + 2);
							strcpy(lpszSelection, pClip);
						}
						LocalFree(pClip);
					}
				}
				bFirstTime = FALSE;

				// Check lpszSelection and truncate bad chars
				//char *lpsz = strpbrk(lpszSelection, "\r\n\t");
				//if (lpsz) {
				//	*lpsz = '\0';
				//}
				char *lpszEscSel = (char *)NP2HeapAlloc((2 * NP2_FIND_REPLACE_LIMIT));
				lpefr->bTransformBS = AddBackslashA(lpszEscSel, lpszSelection);

				SetDlgItemTextA2W(cpEdit, hwnd, IDC_FINDTEXT, lpszEscSel);
				NP2HeapFree(lpszSelection);
				NP2HeapFree(lpszEscSel);
			}
		}

		SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_LIMITTEXT, NP2_FIND_REPLACE_LIMIT, 0);
		SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_SETEXTENDEDUI, TRUE, 0);

		if (!GetWindowTextLength(GetDlgItem(hwnd, IDC_FINDTEXT))) {
			SetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8);
		}

		if (GetDlgItem(hwnd, IDC_REPLACETEXT)) {
			AddBackslashComboBoxSetup(hwnd, IDC_REPLACETEXT);
			for (int i = 0; i < MRU_GetCount(mruReplace); i++) {
				MRU_Enum(mruReplace, i, tch, COUNTOF(tch));
				SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_ADDSTRING, 0, (LPARAM)tch);
			}

			SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_LIMITTEXT, NP2_FIND_REPLACE_LIMIT, 0);
			SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_SETEXTENDEDUI, TRUE, 0);
			SetDlgItemTextA2W(CP_UTF8, hwnd, IDC_REPLACETEXT, lpefr->szReplaceUTF8);
		}

		if (lpefr->fuFlags & SCFIND_MATCHCASE) {
			CheckDlgButton(hwnd, IDC_FINDCASE, BST_CHECKED);
		}

		if (lpefr->fuFlags & SCFIND_WHOLEWORD) {
			CheckDlgButton(hwnd, IDC_FINDWORD, BST_CHECKED);
		}

		if (lpefr->fuFlags & SCFIND_WORDSTART) {
			CheckDlgButton(hwnd, IDC_FINDSTART, BST_CHECKED);
		}

		if (lpefr->fuFlags & SCFIND_REGEXP) {
			CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_CHECKED);
		}

		if (lpefr->bTransformBS) {
			CheckDlgButton(hwnd, IDC_FINDTRANSFORMBS, BST_CHECKED);
		}

		if (lpefr->bWildcardSearch) {
			CheckDlgButton(hwnd, IDC_WILDCARDSEARCH, BST_CHECKED);
			CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
		}

		if (lpefr->bNoFindWrap) {
			CheckDlgButton(hwnd, IDC_NOWRAP, BST_CHECKED);
		}

		if (GetDlgItem(hwnd, IDC_REPLACE)) {
			if (bSwitchedFindReplace) {
				if (lpefr->bFindClose) {
					CheckDlgButton(hwnd, IDC_FINDCLOSE, BST_CHECKED);
				}
			} else {
				if (lpefr->bReplaceClose) {
					CheckDlgButton(hwnd, IDC_FINDCLOSE, BST_CHECKED);
				}
			}
		} else {
			if (bSwitchedFindReplace) {
				if (lpefr->bReplaceClose) {
					CheckDlgButton(hwnd, IDC_FINDCLOSE, BST_CHECKED);
				}
			} else {
				if (lpefr->bFindClose) {
					CheckDlgButton(hwnd, IDC_FINDCLOSE, BST_CHECKED);
				}
			}
		}

		if (!bSwitchedFindReplace) {
			if (xFindReplaceDlg == 0 || yFindReplaceDlg == 0) {
				CenterDlgInParent(hwnd);
			} else {
				SetDlgPos(hwnd, xFindReplaceDlg, yFindReplaceDlg);
			}
		} else {
			SetDlgPos(hwnd, xFindReplaceDlgSave, yFindReplaceDlgSave);
			bSwitchedFindReplace = FALSE;
			CopyMemory(lpefr, &efrSave, sizeof(EDITFINDREPLACE));
		}

		if (bFindReplaceTransparentMode) {
			CheckDlgButton(hwnd, IDC_TRANSPARENT, BST_CHECKED);
		}
	}
	return TRUE;

	case WM_DESTROY:
		ResizeDlg_Destroy(hwnd, &cxFindReplaceDlg, NULL);
		return FALSE;

	case WM_SIZE: {
		int dx;

		const BOOL isReplace = GetDlgItem(hwnd, IDC_REPLACETEXT) != NULL;
		ResizeDlg_Size(hwnd, lParam, &dx, NULL);
		HDWP hdwp = BeginDeferWindowPos(isReplace ? 13 : 8);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP2, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_FINDTEXT, dx, 0, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_CLEAR_FIND, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_FINDPREV, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_SAVEPOSITION, dx, 0, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESETPOSITION, dx, 0, SWP_NOSIZE);
		if (isReplace) {
			hdwp = DeferCtlPos(hdwp, hwnd, IDC_REPLACETEXT, dx, 0, SWP_NOMOVE);
			hdwp = DeferCtlPos(hdwp, hwnd, IDC_CLEAR_REPLACE, dx, 0, SWP_NOSIZE);
			hdwp = DeferCtlPos(hdwp, hwnd, IDC_REPLACE, dx, 0, SWP_NOSIZE);
			hdwp = DeferCtlPos(hdwp, hwnd, IDC_REPLACEALL, dx, 0, SWP_NOSIZE);
			hdwp = DeferCtlPos(hdwp, hwnd, IDC_REPLACEINSEL, dx, 0, SWP_NOSIZE);
		}
		EndDeferWindowPos(hdwp);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FINDTEXT:
		case IDC_REPLACETEXT: {
			const BOOL bEnable = (GetWindowTextLength(GetDlgItem(hwnd, IDC_FINDTEXT)) ||
							CB_ERR != SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_GETCURSEL, 0, 0));

			EnableWindow(GetDlgItem(hwnd, IDOK), bEnable);
			EnableWindow(GetDlgItem(hwnd, IDC_FINDPREV), bEnable);
			EnableWindow(GetDlgItem(hwnd, IDC_REPLACE), bEnable);
			EnableWindow(GetDlgItem(hwnd, IDC_REPLACEALL), bEnable);
			EnableWindow(GetDlgItem(hwnd, IDC_REPLACEINSEL), bEnable);

			if (HIWORD(wParam) == CBN_CLOSEUP) {
				LONG lSelEnd;
				SendDlgItemMessage(hwnd, LOWORD(wParam), CB_GETEDITSEL, 0, (LPARAM)&lSelEnd);
				SendDlgItemMessage(hwnd, LOWORD(wParam), CB_SETEDITSEL, 0, MAKELPARAM(lSelEnd, lSelEnd));
			}
		}
		break;

		case IDC_FINDREGEXP:
			if (IsButtonChecked(hwnd, IDC_FINDREGEXP)) {
				CheckDlgButton(hwnd, IDC_FINDTRANSFORMBS, BST_UNCHECKED);
				// Can not use wildcard search together with regexp
				CheckDlgButton(hwnd, IDC_WILDCARDSEARCH, BST_UNCHECKED);
			}
			break;

		case IDC_FINDTRANSFORMBS:
			if (IsButtonChecked(hwnd, IDC_FINDTRANSFORMBS)) {
				CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			}
			break;

		case IDC_WILDCARDSEARCH:
			CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			//if (IsButtonChecked(hwnd, IDC_FINDWILDCARDS))
			//	CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			break;

		case IDC_TRANSPARENT:
			bFindReplaceTransparentMode = IsButtonChecked(hwnd, IDC_TRANSPARENT);
			break;

		case IDOK:
		case IDC_FINDPREV:
		case IDC_REPLACE:
		case IDC_REPLACEALL:
		case IDC_REPLACEINSEL:
		case IDACC_SELTONEXT:
		case IDACC_SELTOPREV:
		case IDMSG_SWITCHTOFIND:
		case IDMSG_SWITCHTOREPLACE: {
			LPEDITFINDREPLACE lpefr = (LPEDITFINDREPLACE)GetWindowLongPtr(hwnd, DWLP_USER);
			const BOOL bIsFindDlg = (GetDlgItem(hwnd, IDC_REPLACE) == NULL);

			if ((bIsFindDlg && LOWORD(wParam) == IDMSG_SWITCHTOREPLACE) ||
					(!bIsFindDlg && LOWORD(wParam) == IDMSG_SWITCHTOFIND)) {
				GetDlgPos(hwnd, &xFindReplaceDlgSave, &yFindReplaceDlgSave);
				bSwitchedFindReplace = TRUE;
				CopyMemory(&efrSave, lpefr, sizeof(EDITFINDREPLACE));
			}

			// Get current code page for Unicode conversion
			const UINT cpEdit = SciCall_GetCodePage();
			cpLastFind = cpEdit;

			if (!bSwitchedFindReplace &&
					!GetDlgItemTextA2W(cpEdit, hwnd, IDC_FINDTEXT, lpefr->szFind, COUNTOF(lpefr->szFind))) {
				EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_FINDPREV), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_REPLACE), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_REPLACEALL), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_REPLACEINSEL), FALSE);
				return TRUE;
			}

			if (GetDlgItem(hwnd, IDC_REPLACETEXT)) {
				GetDlgItemTextA2W(cpEdit, hwnd, IDC_REPLACETEXT, lpefr->szReplace, COUNTOF(lpefr->szReplace));
			}

			lpefr->bWildcardSearch = IsButtonChecked(hwnd, IDC_WILDCARDSEARCH);
			lpefr->fuFlags = 0;

			if (IsButtonChecked(hwnd, IDC_FINDCASE)) {
				lpefr->fuFlags |= SCFIND_MATCHCASE;
			}

			if (IsButtonChecked(hwnd, IDC_FINDWORD)) {
				lpefr->fuFlags |= SCFIND_WHOLEWORD;
			}

			if (IsButtonChecked(hwnd, IDC_FINDSTART)) {
				lpefr->fuFlags |= SCFIND_WORDSTART;
			}

			if (IsButtonChecked(hwnd, IDC_FINDREGEXP)) {
				lpefr->fuFlags |= SCFIND_REGEXP | SCFIND_POSIX;
			}

			lpefr->bTransformBS = IsButtonChecked(hwnd, IDC_FINDTRANSFORMBS);
			lpefr->bNoFindWrap = IsButtonChecked(hwnd, IDC_NOWRAP);

			if (bIsFindDlg) {
				lpefr->bFindClose = IsButtonChecked(hwnd, IDC_FINDCLOSE);
			} else {
				lpefr->bReplaceClose = IsButtonChecked(hwnd, IDC_FINDCLOSE);
			}

			if (!bSwitchedFindReplace) {
				// Save MRUs
				if (StrNotEmptyA(lpefr->szFind)) {
					if (GetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8, COUNTOF(lpefr->szFindUTF8))) {
						GetDlgItemText(hwnd, IDC_FINDTEXT, tch, COUNTOF(tch));
						MRU_AddMultiline(mruFind, tch);
					}
				}
				if (StrNotEmptyA(lpefr->szReplace)) {
					if (GetDlgItemTextA2W(CP_UTF8, hwnd, IDC_REPLACETEXT, lpefr->szReplaceUTF8, COUNTOF(lpefr->szReplaceUTF8))) {
						GetDlgItemText(hwnd, IDC_REPLACETEXT, tch, COUNTOF(tch));
						MRU_AddMultiline(mruReplace, tch);
					}
				} else {
					strcpy(lpefr->szReplaceUTF8, "");
				}
			} else {
				GetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8, COUNTOF(lpefr->szFindUTF8));
				if (!GetDlgItemTextA2W(CP_UTF8, hwnd, IDC_REPLACETEXT, lpefr->szReplaceUTF8, COUNTOF(lpefr->szReplaceUTF8))) {
					strcpy(lpefr->szReplaceUTF8, "");
				}
			}

			BOOL bCloseDlg;
			if (bIsFindDlg) {
				bCloseDlg = lpefr->bFindClose;
			} else {
				if (LOWORD(wParam) == IDOK) {
					bCloseDlg = FALSE;
				} else {
					bCloseDlg = lpefr->bReplaceClose;
				}
			}

			// Reload MRUs
			SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_RESETCONTENT, 0, 0);

			for (int i = 0; i < MRU_GetCount(mruFind); i++) {
				MRU_Enum(mruFind, i, tch, COUNTOF(tch));
				SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_ADDSTRING, 0, (LPARAM)tch);
			}

			for (int i = 0; i < MRU_GetCount(mruReplace); i++) {
				MRU_Enum(mruReplace, i, tch, COUNTOF(tch));
				SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_ADDSTRING, 0, (LPARAM)tch);
			}

			SetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8);
			SetDlgItemTextA2W(CP_UTF8, hwnd, IDC_REPLACETEXT, lpefr->szReplaceUTF8);

			if (!bSwitchedFindReplace) {
				SendMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetFocus()), 1);
			}

			if (bCloseDlg) {
				//EndDialog(hwnd, LOWORD(wParam));
				DestroyWindow(hwnd);
				hDlgFindReplace = NULL;
			}

			switch (LOWORD(wParam)) {
			case IDOK: // find next
			case IDACC_SELTONEXT:
				if (!bIsFindDlg) {
					bReplaceInitialized = TRUE;
				}
				EditFindNext(lpefr, LOWORD(wParam) == IDACC_SELTONEXT || KeyboardIsKeyDown(VK_SHIFT));
				break;

			case IDC_FINDPREV: // find previous
			case IDACC_SELTOPREV:
				if (!bIsFindDlg) {
					bReplaceInitialized = TRUE;
				}
				EditFindPrev(lpefr, LOWORD(wParam) == IDACC_SELTOPREV || KeyboardIsKeyDown(VK_SHIFT));
				break;

			case IDC_REPLACE:
				bReplaceInitialized = TRUE;
				EditReplace(lpefr->hwnd, lpefr);
				break;

			case IDC_REPLACEALL:
				bReplaceInitialized = TRUE;
				EditReplaceAll(lpefr->hwnd, lpefr, TRUE);
				break;

			case IDC_REPLACEINSEL:
				bReplaceInitialized = TRUE;
				EditReplaceAllInSelection(lpefr->hwnd, lpefr, TRUE);
				break;
			}

			// Wildcard search will enable regexp, so I turn it off again otherwise it will be on in the gui
			if (lpefr->bWildcardSearch	&&	(lpefr->fuFlags & SCFIND_REGEXP)) {
				lpefr->fuFlags ^= SCFIND_REGEXP;
			}
		}
		break;

		case IDCANCEL:
			//EndDialog(hwnd, IDCANCEL);
			DestroyWindow(hwnd);
			break;

		case IDACC_FIND:
			PostWMCommand(hwndMain, IDM_EDIT_FIND);
			break;

		case IDACC_REPLACE:
			PostWMCommand(hwndMain, IDM_EDIT_REPLACE);
			break;

		case IDACC_SAVEPOS:
			GetDlgPos(hwnd, &xFindReplaceDlg, &yFindReplaceDlg);
			break;

		case IDACC_RESETPOS:
			CenterDlgInParent(hwnd);
			xFindReplaceDlg = yFindReplaceDlg = 0;
			break;

		case IDACC_FINDNEXT:
			PostWMCommand(hwnd, IDOK);
			break;

		case IDACC_FINDPREV:
			PostWMCommand(hwnd, IDC_FINDPREV);
			break;

		case IDACC_REPLACENEXT:
			if (GetDlgItem(hwnd, IDC_REPLACE) != NULL) {
				PostWMCommand(hwnd, IDC_REPLACE);
			}
			break;

		case IDACC_SAVEFIND: {
			SendWMCommand(hwndMain, IDM_EDIT_SAVEFIND);
			LPCEDITFINDREPLACE lpefr = (LPCEDITFINDREPLACE)GetWindowLongPtr(hwnd, DWLP_USER);
			SetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8);
			CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			CheckDlgButton(hwnd, IDC_FINDTRANSFORMBS, BST_UNCHECKED);
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_FINDTEXT)), 1);
		}
		break;
		}
		return TRUE;

	case WM_NOTIFY: {
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		switch (pnmhdr->code) {
		case NM_CLICK:
		case NM_RETURN:
			switch (pnmhdr->idFrom) {
			case IDC_TOGGLEFINDREPLACE:
				if (GetDlgItem(hwnd, IDC_REPLACE)) {
					PostWMCommand(hwndMain, IDM_EDIT_FIND);
				} else {
					PostWMCommand(hwndMain, IDM_EDIT_REPLACE);
				}
				break;

			// Display help messages in the find/replace windows
			case IDC_BACKSLASHHELP:
				MsgBox(MBINFO, IDS_BACKSLASHHELP);
				//ShowNotificationMessage(SC_NOTIFICATIONPOSITION_CENTER, IDS_BACKSLASHHELP);
				break;

			case IDC_REGEXPHELP:
				MsgBox(MBINFO, IDS_REGEXPHELP);
				//ShowNotificationMessage(SC_NOTIFICATIONPOSITION_CENTER, IDS_REGEXPHELP);
				break;

			case IDC_WILDCARDHELP:
				MsgBox(MBINFO, IDS_WILDCARDHELP);
				//ShowNotificationMessage(SC_NOTIFICATIONPOSITION_CENTER, IDS_WILDCARDHELP);
				break;

			case IDC_CLEAR_FIND:
				GetDlgItemText(hwnd, IDC_FINDTEXT, tch, COUNTOF(tch));
				SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_RESETCONTENT, 0, 0);
				MRU_Empty(mruFind);
				MRU_Save(mruFind);
				SetDlgItemText(hwnd, IDC_FINDTEXT, tch);
				break;

			case IDC_CLEAR_REPLACE:
				GetDlgItemText(hwnd, IDC_REPLACETEXT, tch, COUNTOF(tch));
				SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_RESETCONTENT, 0, 0);
				MRU_Empty(mruReplace);
				MRU_Save(mruReplace);
				SetDlgItemText(hwnd, IDC_REPLACETEXT, tch);
				break;

			case IDC_SAVEPOSITION:
				PostWMCommand(hwnd, IDACC_SAVEPOS);
				break;

			case IDC_RESETPOSITION:
				PostWMCommand(hwnd, IDACC_RESETPOS);
				break;
			}
			break;
		}
	}
	break;

	case WM_ACTIVATE :
		SetWindowTransparentMode(hwnd, (LOWORD(wParam) == WA_INACTIVE && bFindReplaceTransparentMode), iFindReplaceOpacityLevel);
		break;
	}

	return FALSE;
}

//=============================================================================
//
// EditFindReplaceDlg()
//
HWND EditFindReplaceDlg(HWND hwnd, LPEDITFINDREPLACE lpefr, BOOL bReplace) {
	lpefr->hwnd = hwnd;
	HWND hDlg = CreateThemedDialogParam(g_hInstance,
								   (bReplace) ? MAKEINTRESOURCE(IDD_REPLACE) : MAKEINTRESOURCE(IDD_FIND),
								   GetParent(hwnd),
								   EditFindReplaceDlgProc,
								   (LPARAM)lpefr);

	ShowWindow(hDlg, SW_SHOW);
	return hDlg;
}

// Wildcard search uses the regexp engine to perform a simple search with * ? as wildcards instead of more advanced and user-unfriendly regexp syntax
void EscapeWildcards(char *szFind2, LPEDITFINDREPLACE lpefr) {
	char szWildcardEscaped[NP2_FIND_REPLACE_LIMIT];
	int iSource = 0;
	int iDest = 0;

	lpefr->fuFlags |= SCFIND_REGEXP;

	while (szFind2[iSource]) {
		const char c = szFind2[iSource];
		if (c == '*') {
			szWildcardEscaped[iDest++] = '.';
			szWildcardEscaped[iDest] = '*';
		} else if (c == '?') {
			szWildcardEscaped[iDest] = '.';
		} else {
			if (c == '.' || c == '^' || c == '$' || c == '\\' || c == '[' || c == ']' || c == '+') {
				szWildcardEscaped[iDest++] = '\\';
			}
			szWildcardEscaped[iDest] = c;
		}
		iSource++;
		iDest++;
	}
	szWildcardEscaped[iDest] = 0;
	strncpy(szFind2, szWildcardEscaped, COUNTOF(szWildcardEscaped));
}

//=============================================================================
//
// EditFindNext()
//
BOOL EditFindNext(LPEDITFINDREPLACE lpefr, BOOL fExtendSelection) {
	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, FALSE)*/FALSE;
	}

	char szFind2[NP2_FIND_REPLACE_LIMIT];
	strncpy(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		const UINT cpEdit = SciCall_GetCodePage();
		TransformBackslashes(szFind2, (lpefr->fuFlags & SCFIND_REGEXP), cpEdit);
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}

	const Sci_Position iSelPos = SciCall_GetCurrentPos();
	const Sci_Position iSelAnchor = SciCall_GetAnchor();

	struct Sci_TextToFind ttf;
	ZeroMemory(&ttf, sizeof(ttf));
	ttf.chrg.cpMin = (Sci_PositionCR)SciCall_GetSelectionEnd();
	ttf.chrg.cpMax = (Sci_PositionCR)SciCall_GetLength();
	ttf.lpstrText = szFind2;

	Sci_Position iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
	BOOL bSuppressNotFound = FALSE;

	if (iPos == -1 && ttf.chrg.cpMin > 0 && !lpefr->bNoFindWrap && !fExtendSelection) {
		if (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap1", IDS_FIND_WRAPFW)) {
			ttf.chrg.cpMin = 0;
			iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
		} else {
			bSuppressNotFound = TRUE;
		}
	}

	if (iPos == -1) {
		// notfound
		if (!bSuppressNotFound) {
			InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		}
		return FALSE;
	}

	if (!fExtendSelection) {
		EditSelectEx(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
	} else {
		EditSelectEx(min_pos(iSelAnchor, iSelPos), ttf.chrgText.cpMax);
	}

	return TRUE;
}

//=============================================================================
//
// EditFindPrev()
//
BOOL EditFindPrev(LPEDITFINDREPLACE lpefr, BOOL fExtendSelection) {
	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, FALSE)*/FALSE;
	}

	char szFind2[NP2_FIND_REPLACE_LIMIT];
	strncpy(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		const UINT cpEdit = SciCall_GetCodePage();
		TransformBackslashes(szFind2, (lpefr->fuFlags & SCFIND_REGEXP), cpEdit);
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}

	const Sci_Position iSelPos = SciCall_GetCurrentPos();
	const Sci_Position iSelAnchor = SciCall_GetAnchor();

	struct Sci_TextToFind ttf;
	ZeroMemory(&ttf, sizeof(ttf));
	ttf.chrg.cpMin = (Sci_PositionCR)max_pos(0, SciCall_GetSelectionStart());
	ttf.chrg.cpMax = 0;
	ttf.lpstrText = szFind2;

	Sci_Position iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
	const Sci_Position iLength = SciCall_GetLength();
	BOOL bSuppressNotFound = FALSE;

	if (iPos == -1 && ttf.chrg.cpMin < iLength && !lpefr->bNoFindWrap && !fExtendSelection) {
		if (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap2", IDS_FIND_WRAPRE)) {
			ttf.chrg.cpMin = (Sci_PositionCR)iLength;
			iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
		} else {
			bSuppressNotFound = TRUE;
		}
	}

	if (iPos == -1) {
		// notfound
		if (!bSuppressNotFound) {
			InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		}
		return FALSE;
	}

	if (!fExtendSelection) {
		EditSelectEx(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
	} else {
		EditSelectEx(max_pos(iSelPos, iSelAnchor), ttf.chrgText.cpMin);
	}

	return TRUE;
}

//=============================================================================
//
// EditReplace()
//
BOOL EditReplace(HWND hwnd, LPEDITFINDREPLACE lpefr) {
	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, TRUE)*/FALSE;
	}

	BOOL bReplaceRE = (lpefr->fuFlags & SCFIND_REGEXP);
	const UINT cpEdit = SciCall_GetCodePage();
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	strncpy(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, bReplaceRE, cpEdit);
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}

	char *pszReplace2;
	if (strcmp(lpefr->szReplace, "^c") == 0) {
		bReplaceRE = FALSE;
		pszReplace2 = EditGetClipboardText(hwnd);
	} else {
		//strcpy(szReplace2, lpefr->szReplace);
		pszReplace2 = StrDupA(lpefr->szReplace);
		if (lpefr->bTransformBS) {
			TransformBackslashes(pszReplace2, bReplaceRE, cpEdit);
		}
	}

	if (pszReplace2 == NULL) {
		pszReplace2 = StrDupA("");
	}

	const Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();

	struct Sci_TextToFind ttf;
	ZeroMemory(&ttf, sizeof(ttf));
	ttf.chrg.cpMin = (Sci_PositionCR)iSelStart; // Start!
	ttf.chrg.cpMax = (Sci_PositionCR)SciCall_GetLength();
	ttf.lpstrText = szFind2;

	Sci_Position iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
	BOOL bSuppressNotFound = FALSE;

	if (iPos == -1 && ttf.chrg.cpMin > 0 && !lpefr->bNoFindWrap) {
		if (!lpefr->bNoFindWrap || (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap1", IDS_FIND_WRAPFW))) {
			ttf.chrg.cpMin = 0;
			iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
		} else {
			bSuppressNotFound = TRUE;
		}
	}

	if (iPos == -1) {
		// notfound
		LocalFree(pszReplace2);
		if (!bSuppressNotFound) {
			InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		}
		return FALSE;
	}

	if (iSelStart != ttf.chrgText.cpMin || iSelEnd != ttf.chrgText.cpMax) {
		LocalFree(pszReplace2);
		EditSelectEx(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
		return FALSE;
	}

	SciCall_SetTargetRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
	SciCall_ReplaceTargetEx(bReplaceRE, -1, pszReplace2);

	ttf.chrg.cpMin = (Sci_PositionCR)SciCall_GetTargetEnd();
	ttf.chrg.cpMax = (Sci_PositionCR)SciCall_GetLength();

	iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
	bSuppressNotFound = FALSE;

	if (iPos == -1 && ttf.chrg.cpMin > 0 && !lpefr->bNoFindWrap) {
		if (!lpefr->bNoFindWrap || (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap1", IDS_FIND_WRAPFW))) {
			ttf.chrg.cpMin = 0;
			iPos = SciCall_FindText(lpefr->fuFlags, &ttf);
		} else {
			bSuppressNotFound = TRUE;
		}
	}

	if (iPos != -1) {
		EditSelectEx(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
	} else {
		iPos = SciCall_GetTargetEnd();
		EditSelectEx(iPos, iPos); //?
		if (!bSuppressNotFound) {
			InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		}
	}

	LocalFree(pszReplace2);

	return TRUE;
}

//=============================================================================
//
// EditMarkAll()
// Mark all occurrences of the text currently selected (by Aleksandar Lekov)
//

extern Sci_Position iMatchesCount;

void EditMarkAll_Clear(void) {
	if (iMatchesCount == 0) {
		return;
	}

	iMatchesCount = 0;
	// clear existing indicator
	SciCall_SetIndicatorCurrent(IndicatorNumber_MarkOccurrences);
	SciCall_IndicatorClearRange(0, SciCall_GetLength());

	if (editMarkAllStatus.pszText) {
		NP2HeapFree(editMarkAllStatus.pszText);
	}
	editMarkAllStatus.findFlag = 0;
	editMarkAllStatus.iSelCount= 0;
	editMarkAllStatus.pszText = NULL;
}

void EditMarkAll(BOOL bChanged, BOOL bMarkOccurrencesMatchCase, BOOL bMarkOccurrencesMatchWords) {
	// get current selection
	Sci_Position iSelStart = SciCall_GetSelectionStart();
	const Sci_Position iSelEnd = SciCall_GetSelectionEnd();
	Sci_Position iSelCount = iSelEnd - iSelStart;

	// if nothing selected or multiple lines are selected exit
	if (iSelCount == 0 || SciCall_LineFromPosition(iSelStart) != SciCall_LineFromPosition(iSelEnd)) {
		EditMarkAll_Clear();
		return;
	}

	// scintilla/src/Editor.h SelectionText.LengthWithTerminator()
	iSelCount = SciCall_GetSelTextLength() - 1;
	char *pszText = (char *)NP2HeapAlloc(iSelCount + 1);
	SciCall_GetSelText(pszText);

	// exit if selection is not a word and Match whole words only is enabled
	if (bMarkOccurrencesMatchWords) {
		const UINT cpEdit = SciCall_GetCodePage();
		const BOOL dbcs = !(cpEdit == CP_UTF8 || cpEdit == 0);
		// CharClassify::SetDefaultCharClasses()
		for (iSelStart = 0; iSelStart < iSelCount; ++iSelStart) {
			const unsigned char ch = pszText[iSelStart];
			if (dbcs && IsDBCSLeadByteEx(cpEdit, ch)) {
				++iSelStart;
			} else if (!(ch >= 0x80 || IsDocWordChar(ch))) {
				NP2HeapFree(pszText);
				EditMarkAll_Clear();
				return;
			}
		}
	}

	const int findFlag = (bMarkOccurrencesMatchCase ? SCFIND_MATCHCASE : 0) | (bMarkOccurrencesMatchWords ? SCFIND_WHOLEWORD : 0);
	if (!bChanged && findFlag == editMarkAllStatus.findFlag && editMarkAllStatus.iSelCount == iSelCount) {
		// _stricmp() is not safe for DBCS string.
		if (memcmp(pszText, editMarkAllStatus.pszText, iSelCount) == 0) {
			return;
		}
	}

	EditMarkAll_Clear();
	SciCall_SetIndicatorCurrent(IndicatorNumber_MarkOccurrences);

	SciCall_SetSearchFlags(findFlag);
	const Sci_Position iDocLen = SciCall_GetLength();

	Sci_Position iPos = 0;
	do {
		SciCall_SetTargetRange(iPos, iDocLen);
		iPos = SciCall_SearchInTarget(iSelCount, pszText);
		if (iPos == -1) {
			break;
		}
		// mark this match
		++iMatchesCount;
		SciCall_IndicatorFillRange(iPos, iSelCount);
		iPos += iSelCount;
	} while (iPos < iDocLen);

	if (iMatchesCount > 1) {
		editMarkAllStatus.findFlag = findFlag;
		editMarkAllStatus.iSelCount = iSelCount;
		editMarkAllStatus.pszText = pszText;
	} else {
		NP2HeapFree(pszText);
	}
}

static void ShwowReplaceCount(Sci_Position iCount) {
	if (iCount > 0) {
		WCHAR tchNum[32];
		PosToStrW(iCount, tchNum);
		FormatNumberStr(tchNum);
		InfoBox(0, L"MsgReplaceCount", IDS_REPLCOUNT, tchNum);
	} else {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
	}
}

//=============================================================================
//
// EditReplaceAll()
//
BOOL EditReplaceAll(HWND hwnd, LPEDITFINDREPLACE lpefr, BOOL bShowInfo) {
	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, TRUE)*/FALSE;
	}

	// Show wait cursor...
	BeginWaitCursor();

	BOOL bReplaceRE = (lpefr->fuFlags & SCFIND_REGEXP);
	const UINT cpEdit = SciCall_GetCodePage();
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	strncpy(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, bReplaceRE, cpEdit);
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}

	const BOOL bRegexStartOfLine = bReplaceRE && (szFind2[0] == '^');
	const BOOL bRegexStartOrEndOfLine = bReplaceRE && ((!strcmp(szFind2, "$") || !strcmp(szFind2, "^") || !strcmp(szFind2, "^$")));

	char *pszReplace2;
	if (strcmp(lpefr->szReplace, "^c") == 0) {
		bReplaceRE = FALSE;
		pszReplace2 = EditGetClipboardText(hwnd);
	} else {
		//strcpy(szReplace2, lpefr->szReplace);
		pszReplace2 = StrDupA(lpefr->szReplace);
		if (lpefr->bTransformBS) {
			TransformBackslashes(pszReplace2, bReplaceRE, cpEdit);
		}
	}

	if (pszReplace2 == NULL) {
		pszReplace2 = StrDupA("");
	}

	struct Sci_TextToFind ttf;
	ZeroMemory(&ttf, sizeof(ttf));
	ttf.chrg.cpMin = 0;
	ttf.chrg.cpMax = (Sci_PositionCR)SciCall_GetLength();
	ttf.lpstrText = szFind2;

	Sci_Position iCount = 0;
	while (SciCall_FindText(lpefr->fuFlags, &ttf) != -1) {
		if (iCount == 0 && bRegexStartOrEndOfLine) {
			if (0 == SciCall_GetLineEndPosition(0)) {
				ttf.chrgText.cpMin = 0;
				ttf.chrgText.cpMax = 0;
			}
		}

		if (++iCount == 1) {
			SciCall_BeginUndoAction();
		}

		SciCall_SetTargetRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
		const Sci_Position iReplacedLen = SciCall_ReplaceTargetEx(bReplaceRE, -1, pszReplace2);

		ttf.chrg.cpMin = (Sci_PositionCR)(ttf.chrgText.cpMin + iReplacedLen);
		ttf.chrg.cpMax = (Sci_PositionCR)SciCall_GetLength();

		if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
			break;
		}

		//const int ch = SciCall_GetCharAt(SciCall_GetTargetEnd());
		if (/*ch == '\r' || ch == '\n' || iReplacedLen == 0 || */
			ttf.chrgText.cpMin == ttf.chrgText.cpMax &&
			!(bRegexStartOrEndOfLine && iReplacedLen > 0)) {
			ttf.chrg.cpMin = (Sci_PositionCR)SciCall_PositionAfter(ttf.chrg.cpMin);
		}

		if (bRegexStartOfLine) {
			const Sci_Line iLine = SciCall_LineFromPosition(ttf.chrg.cpMin);
			const Sci_Position ilPos = SciCall_PositionFromLine(iLine);

			if (ilPos == ttf.chrg.cpMin) {
				ttf.chrg.cpMin = (Sci_PositionCR)SciCall_PositionFromLine(iLine + 1);
			}
			if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
				break;
			}
		}
	}

	if (iCount) {
		SciCall_EndUndoAction();
	}

	// Remove wait cursor
	EndWaitCursor();

	if (bShowInfo) {
		ShwowReplaceCount(iCount);
	}

	LocalFree(pszReplace2);

	return TRUE;
}

//=============================================================================
//
// EditReplaceAllInSelection()
//
BOOL EditReplaceAllInSelection(HWND hwnd, LPEDITFINDREPLACE lpefr, BOOL bShowInfo) {
	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, TRUE)*/FALSE;
	}
	if (SciCall_IsRectangleSelection()) {
		NotifyRectangleSelection();
		return FALSE;
	}

	// Show wait cursor...
	BeginWaitCursor();

	BOOL bReplaceRE = (lpefr->fuFlags & SCFIND_REGEXP);
	const UINT cpEdit = SciCall_GetCodePage();
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	strncpy(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, bReplaceRE, cpEdit);
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}

	const BOOL bRegexStartOfLine = bReplaceRE && (szFind2[0] == '^');
	const BOOL bRegexStartOrEndOfLine = bReplaceRE && ((!strcmp(szFind2, "$") || !strcmp(szFind2, "^") || !strcmp(szFind2, "^$")));

	char *pszReplace2;
	if (strcmp(lpefr->szReplace, "^c") == 0) {
		bReplaceRE = FALSE;
		pszReplace2 = EditGetClipboardText(hwnd);
	} else {
		//strcpy(szReplace2, lpefr->szReplace);
		pszReplace2 = StrDupA(lpefr->szReplace);
		if (lpefr->bTransformBS) {
			TransformBackslashes(pszReplace2, bReplaceRE, cpEdit);
		}
	}

	if (pszReplace2 == NULL) {
		pszReplace2 = StrDupA("");
	}

	struct Sci_TextToFind ttf;
	ZeroMemory(&ttf, sizeof(ttf));
	ttf.chrg.cpMin = (Sci_PositionCR)SciCall_GetSelectionStart();
	ttf.chrg.cpMax = (Sci_PositionCR)SciCall_GetLength();
	ttf.lpstrText = szFind2;

	Sci_Position iCount = 0;
	BOOL fCancel = FALSE;
	while (!fCancel && SciCall_FindText(lpefr->fuFlags, &ttf) != -1) {
		if (ttf.chrgText.cpMin >= SciCall_GetSelectionStart() && ttf.chrgText.cpMax <= SciCall_GetSelectionEnd()) {

			if (ttf.chrg.cpMin == 0 && iCount == 0 && bRegexStartOrEndOfLine) {
				if (0 == SciCall_GetLineEndPosition(0)) {
					ttf.chrgText.cpMin = 0;
					ttf.chrgText.cpMax = 0;
				}
			}

			if (++iCount == 1) {
				SciCall_BeginUndoAction();
			}

			SciCall_SetTargetRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax);
			const Sci_Position iReplacedLen = SciCall_ReplaceTargetEx(bReplaceRE, -1, pszReplace2);

			ttf.chrg.cpMin = (Sci_PositionCR)(ttf.chrgText.cpMin + iReplacedLen);
			ttf.chrg.cpMax = (Sci_PositionCR)SciCall_GetLength();

			if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
				fCancel = TRUE;
			}

			//const int ch = SciCall_GetCharAt(SciCall_GetTargetEnd());
			if (/*ch == '\r' || ch == '\n' || iReplacedLen == 0 || */
				ttf.chrgText.cpMin == ttf.chrgText.cpMax &&
				!(bRegexStartOrEndOfLine && iReplacedLen > 0)) {
				ttf.chrg.cpMin = (Sci_PositionCR)SciCall_PositionAfter(ttf.chrg.cpMin);
			}

			if (bRegexStartOfLine) {
				const Sci_Line iLine = SciCall_LineFromPosition(ttf.chrg.cpMin);
				const Sci_Position ilPos = SciCall_PositionFromLine(iLine);

				if (ilPos == ttf.chrg.cpMin) {
					ttf.chrg.cpMin = (Sci_PositionCR)SciCall_PositionFromLine(iLine + 1);
				}
				if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
					break;
				}
			}
		} else { // gone across selection, cancel
			fCancel = TRUE;
		}
	}

	if (iCount) {
		const Sci_Position iPos = SciCall_GetTargetEnd();
		if (SciCall_GetSelectionEnd() <	iPos) {
			Sci_Position iAnchorPos = SciCall_GetAnchor();
			Sci_Position iCurrentPos = SciCall_GetCurrentPos();

			if (iAnchorPos > iCurrentPos) {
				iAnchorPos = iPos;
			} else {
				iCurrentPos = iPos;
			}

			EditSelectEx(iAnchorPos, iCurrentPos);
		}

		SciCall_EndUndoAction();
	}

	// Remove wait cursor
	EndWaitCursor();

	if (bShowInfo) {
		ShwowReplaceCount(iCount);
	}

	LocalFree(pszReplace2);

	return TRUE;
}

//=============================================================================
//
// EditLineNumDlgProc()
//
static INT_PTR CALLBACK EditLineNumDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG: {
		const Sci_Line iCurLine = SciCall_LineFromPosition(SciCall_GetCurrentPos()) + 1;
		const Sci_Line iMaxLine = SciCall_GetLineCount();
		const Sci_Position iLength = SciCall_GetLength();

		SendDlgItemMessage(hwnd, IDC_LINENUM, EM_LIMITTEXT, 20, 0);
		SendDlgItemMessage(hwnd, IDC_COLNUM, EM_LIMITTEXT, 20, 0);

		WCHAR tchLn[32];
		WCHAR tchLines[64];
		WCHAR tchFmt[64];

		PosToStrW(iCurLine, tchLn);
		SetDlgItemText(hwnd, IDC_LINENUM, tchLn);

		PosToStrW(iMaxLine, tchLn);
		FormatNumberStr(tchLn);
		GetDlgItemText(hwnd, IDC_LINE_RANGE, tchFmt, COUNTOF(tchFmt));
		wsprintf(tchLines, tchFmt, tchLn);
		SetDlgItemText(hwnd, IDC_LINE_RANGE, tchLines);

		PosToStrW(iLength, tchLn);
		FormatNumberStr(tchLn);
		GetDlgItemText(hwnd, IDC_COLUMN_RANGE, tchFmt, COUNTOF(tchFmt));
		wsprintf(tchLines, tchFmt, tchLn);
		SetDlgItemText(hwnd, IDC_COLUMN_RANGE, tchLines);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated;
			BOOL fTranslated2;
			WCHAR tchLn[32];
			Sci_Line iNewLine = 0;
			Sci_Position iNewCol = 0;

			// Extract line number from the text entered
			// For example: "5410:" will result in 5410
			GetDlgItemText(hwnd, IDC_LINENUM, tchLn, COUNTOF(tchLn));
#if defined(_WIN64)
			int64_t iLine = 0;
			fTranslated = CRTStrToInt64(tchLn, &iLine);
#else
			int iLine = 0;
			fTranslated = CRTStrToInt(tchLn, &iLine);
#endif
			iNewLine = iLine;

			if (SendDlgItemMessage(hwnd, IDC_COLNUM, WM_GETTEXTLENGTH, 0, 0) > 0) {
				GetDlgItemText(hwnd, IDC_COLNUM, tchLn, COUNTOF(tchLn));
#if defined(_WIN64)
				fTranslated2 = CRTStrToInt64(tchLn, &iLine);
#else
				fTranslated2 = CRTStrToInt(tchLn, &iLine);
#endif
				iNewCol = iLine;
			} else {
				iNewCol = 1;
				fTranslated2 = fTranslated;
			}

			if (!(fTranslated || fTranslated2)) {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_LINENUM)), 1);
				return TRUE;
			}

			const Sci_Line iMaxLine = SciCall_GetLineCount();
			const Sci_Position iLength = SciCall_GetLength();
			// directly goto specific position
			if (fTranslated2 && !fTranslated) {
				if (iNewCol > 0 && iNewCol <= iLength) {
					SciCall_GotoPos(iNewCol - 1);
					SciCall_ChooseCaretX();
					EndDialog(hwnd, IDOK);
				} else {
					PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_COLNUM)), 1);
				}
			} else if (iNewLine > 0 && iNewLine <= iMaxLine && iNewCol > 0) {
				//Sci_Position iNewPos = SciCall_PositionFromLine(iNewLine - 1);
				//const Sci_Position iLineEndPos = SciCall_GetLineEndPosition(iNewLine - 1);
				//while (iNewCol-1 > SciCall_GetColumn(iNewPos)) {
				//	if (iNewPos >= iLineEndPos) {
				//		break;
				//	}
				//	iNewPos = SciCall_PositionAfter(iNewPos);
				//}
				//iNewPos = min_pos(iNewPos, iLineEndPos);
				//SciCall_GotoPos(iNewPos);
				//SciCall_ChooseCaretX();
				EditJumpTo(iNewLine, iNewCol);
				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, ((iNewCol > 0) ? IDC_LINENUM : IDC_COLNUM))), 1);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditLinenumDlg()
//
BOOL EditLineNumDlg(HWND hwnd) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_LINENUM), GetParent(hwnd), EditLineNumDlgProc, (LPARAM)hwnd);
	return iResult == IDOK;
}

//=============================================================================
//
// EditModifyLinesDlg()
//
//
extern int cxModifyLinesDlg;
extern int cyModifyLinesDlg;
extern int cxEncloseSelectionDlg;
extern int cyEncloseSelectionDlg;
extern int cxInsertTagDlg;
extern int cyInsertTagDlg;

static INT_PTR CALLBACK EditModifyLinesDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static int id_hover;
	static int id_capture;
	static HFONT hFontHover;
	static HCURSOR hCursorNormal;
	static HCURSOR hCursorHover;

	switch (umsg) {
	case WM_INITDIALOG: {
		ResizeDlg_InitY2(hwnd, cxModifyLinesDlg, cyModifyLinesDlg, IDC_RESIZEGRIP2, IDC_MODIFY_LINE_PREFIX, IDC_MODIFY_LINE_APPEND);

		id_hover = 0;
		id_capture = 0;

		HFONT hFontNormal = (HFONT)SendDlgItemMessage(hwnd, IDC_MODIFY_LINE_DLN_NP, WM_GETFONT, 0, 0);
		if (hFontNormal == NULL) {
			hFontNormal = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		}

		LOGFONT lf;
		GetObject(hFontNormal, sizeof(LOGFONT), &lf);
		lf.lfUnderline = TRUE;
		hFontHover = CreateFontIndirect(&lf);

		hCursorNormal = LoadCursor(NULL, IDC_ARROW);
		if ((hCursorHover = LoadCursor(NULL, IDC_HAND)) == NULL) {
			hCursorHover = LoadCursor(g_hInstance, IDC_ARROW);
		}

		MultilineEditSetup(hwnd, IDC_MODIFY_LINE_PREFIX);
		SetDlgItemText(hwnd, IDC_MODIFY_LINE_PREFIX, wchPrefixLines.buffer);
		MultilineEditSetup(hwnd, IDC_MODIFY_LINE_APPEND);
		SetDlgItemText(hwnd, IDC_MODIFY_LINE_APPEND, wchAppendLines.buffer);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		ResizeDlg_Destroy(hwnd, &cxModifyLinesDlg, &cyModifyLinesDlg);
		DeleteObject(hFontHover);
		return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);
		const int cy = ResizeDlg_CalcDeltaY2(hwnd, dy, 50, IDC_MODIFY_LINE_PREFIX, IDC_MODIFY_LINE_APPEND);
		HDWP hdwp = BeginDeferWindowPos(15);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP2, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_PREFIX, dx, cy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_APPEND, 0, cy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_TIP2, 0, cy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_DLN_NP, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_DLN_ZP, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_TIP_DLN, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_CN_NP, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_CN_ZP, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_TIP_CN, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_ZCN_NP, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_ZCN_ZP, 0, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_TIP_ZCN, 0, dy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);
		ResizeDlgCtl(hwnd, IDC_MODIFY_LINE_APPEND, dx, dy - cy);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_NCACTIVATE:
		if (!wParam) {
			if (id_hover != 0) {
				//int _id_hover = id_hover;
				id_hover = 0;
				id_capture = 0;
				//InvalidateRect(GetDlgItem(hwnd, id_hover), NULL, FALSE);
			}
		}
		return FALSE;

	case WM_CTLCOLORSTATIC: {
		const DWORD dwId = GetWindowLong((HWND)lParam, GWL_ID);

		if (dwId >= IDC_MODIFY_LINE_DLN_NP && dwId <= IDC_MODIFY_LINE_ZCN_ZP) {
			HDC hdc = (HDC)wParam;
			SetBkMode(hdc, TRANSPARENT);
			if (GetSysColorBrush(COLOR_HOTLIGHT)) {
				SetTextColor(hdc, GetSysColor(COLOR_HOTLIGHT));
			} else {
				SetTextColor(hdc, RGB(0, 0, 255));
			}
			SelectObject(hdc, /*dwId == id_hover?*/hFontHover/*:hFontNormal*/);
			return (LONG_PTR)GetSysColorBrush(COLOR_BTNFACE);
		}
	}
	break;

	case WM_MOUSEMOVE: {
		const POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		HWND hwndHover = ChildWindowFromPoint(hwnd, pt);
		const DWORD dwId = GetWindowLong(hwndHover, GWL_ID);

		if (GetActiveWindow() == hwnd) {
			if (dwId >= IDC_MODIFY_LINE_DLN_NP && dwId <= IDC_MODIFY_LINE_ZCN_ZP) {
				if (id_capture == (int)dwId || id_capture == 0) {
					if (id_hover != id_capture || id_hover == 0) {
						id_hover = dwId;
						//InvalidateRect(GetDlgItem(hwnd, dwId), NULL, FALSE);
					}
				} else if (id_hover != 0) {
					//int _id_hover = id_hover;
					id_hover = 0;
					//InvalidateRect(GetDlgItem(hwnd, _id_hover), NULL, FALSE);
				}
			} else if (id_hover != 0) {
				//int _id_hover = id_hover;
				id_hover = 0;
				//InvalidateRect(GetDlgItem(hwnd, _id_hover), NULL, FALSE);
			}
			SetCursor(id_hover != 0 ? hCursorHover : hCursorNormal);
		}
	}
	break;

	case WM_LBUTTONDOWN: {
		const POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		HWND hwndHover = ChildWindowFromPoint(hwnd, pt);
		const DWORD dwId = GetWindowLong(hwndHover, GWL_ID);

		if (dwId >= IDC_MODIFY_LINE_DLN_NP && dwId <= IDC_MODIFY_LINE_ZCN_ZP) {
			GetCapture();
			id_hover = dwId;
			id_capture = dwId;
			//InvalidateRect(GetDlgItem(hwnd, dwId), NULL, FALSE);
		}
		SetCursor(id_hover != 0 ? hCursorHover : hCursorNormal);
	}
	break;

	case WM_LBUTTONUP: {
		//const POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		//HWND hwndHover = ChildWindowFromPoint(hwnd, pt);
		//const DWORD dwId = GetWindowLong(hwndHover, GWL_ID);

		if (id_capture != 0) {
			ReleaseCapture();
			if (id_hover == id_capture) {
				const int id_focus = GetWindowLong(GetFocus(), GWL_ID);
				if (id_focus == IDC_MODIFY_LINE_PREFIX || id_focus == IDC_MODIFY_LINE_APPEND) {
					WCHAR wch[8];
					GetDlgItemText(hwnd, id_capture, wch, COUNTOF(wch));
					SendDlgItemMessage(hwnd, id_focus, EM_SETSEL, 0, -1);
					SendDlgItemMessage(hwnd, id_focus, EM_REPLACESEL, TRUE, (LPARAM)wch);
					PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetFocus()), 1);
				}
			}
			id_capture = 0;
		}
		SetCursor(id_hover != 0 ? hCursorHover : hCursorNormal);
	}
	break;

	case WM_CANCELMODE:
		if (id_capture != 0) {
			ReleaseCapture();
			id_hover = 0;
			id_capture = 0;
			SetCursor(hCursorNormal);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			DStringW_GetDlgItemText(&wchPrefixLines, hwnd, IDC_MODIFY_LINE_PREFIX);
			DStringW_GetDlgItemText(&wchAppendLines, hwnd, IDC_MODIFY_LINE_APPEND);

			EditModifyLines(wchPrefixLines.buffer, wchAppendLines.buffer);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditModifyLinesDlg()
//
void EditModifyLinesDlg(HWND hwnd) {
	ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_MODIFYLINES), hwnd, EditModifyLinesDlgProc, 0);
}

//=============================================================================
//
// EditAlignDlgProc()
//
//
static INT_PTR CALLBACK EditAlignDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		const int iAlignMode = *((int *)lParam);
		CheckRadioButton(hwnd, IDC_ALIGN_LEFT, IDC_ALIGN_JUSTIFY_PAR, iAlignMode + IDC_ALIGN_LEFT);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			int *piAlignMode = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
			const int iAlignMode = GetCheckedRadioButton(hwnd, IDC_ALIGN_LEFT, IDC_ALIGN_JUSTIFY_PAR) - IDC_ALIGN_LEFT;
			*piAlignMode = iAlignMode;
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditAlignDlg()
//
BOOL EditAlignDlg(HWND hwnd, int *piAlignMode) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ALIGN), hwnd, EditAlignDlgProc, (LPARAM)piAlignMode);
	return iResult == IDOK;
}

//=============================================================================
//
// EditEncloseSelectionDlgProc()
//
//
static INT_PTR CALLBACK EditEncloseSelectionDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		ResizeDlg_InitY2(hwnd, cxEncloseSelectionDlg, cyEncloseSelectionDlg, IDC_RESIZEGRIP2, IDC_MODIFY_LINE_PREFIX, IDC_MODIFY_LINE_APPEND);

		MultilineEditSetup(hwnd, IDC_MODIFY_LINE_PREFIX);
		SetDlgItemText(hwnd, IDC_MODIFY_LINE_PREFIX, wchPrefixSelection.buffer);
		MultilineEditSetup(hwnd, IDC_MODIFY_LINE_APPEND);
		SetDlgItemText(hwnd, IDC_MODIFY_LINE_APPEND, wchAppendSelection.buffer);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		ResizeDlg_Destroy(hwnd, &cxEncloseSelectionDlg, &cyEncloseSelectionDlg);
		return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);
		const int cy = ResizeDlg_CalcDeltaY2(hwnd, dy, 50, IDC_MODIFY_LINE_PREFIX, IDC_MODIFY_LINE_APPEND);
		HDWP hdwp = BeginDeferWindowPos(6);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP2, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_PREFIX, dx, cy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_APPEND, 0, cy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_TIP2, 0, cy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);
		ResizeDlgCtl(hwnd, IDC_MODIFY_LINE_APPEND, dx, dy - cy);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			DStringW_GetDlgItemText(&wchPrefixSelection, hwnd, IDC_MODIFY_LINE_PREFIX);
			DStringW_GetDlgItemText(&wchAppendSelection, hwnd, IDC_MODIFY_LINE_APPEND);

			EditEncloseSelection(wchPrefixSelection.buffer, wchAppendSelection.buffer);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditEncloseSelectionDlg()
//
void EditEncloseSelectionDlg(HWND hwnd) {
	ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ENCLOSESELECTION), hwnd, EditEncloseSelectionDlgProc, 0);
}

//=============================================================================
//
// EditInsertTagDlgProc()
//
//
static INT_PTR CALLBACK EditInsertTagDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		ResizeDlg_InitY2(hwnd, cxInsertTagDlg, cyInsertTagDlg, IDC_RESIZEGRIP2, IDC_MODIFY_LINE_PREFIX, IDC_MODIFY_LINE_APPEND);

		MultilineEditSetup(hwnd, IDC_MODIFY_LINE_PREFIX);
		SetDlgItemText(hwnd, IDC_MODIFY_LINE_PREFIX, L"<tag>");
		MultilineEditSetup(hwnd, IDC_MODIFY_LINE_APPEND);
		SetDlgItemText(hwnd, IDC_MODIFY_LINE_APPEND, L"</tag>");

		SetFocus(GetDlgItem(hwnd, IDC_MODIFY_LINE_PREFIX));
		PostMessage(GetDlgItem(hwnd, IDC_MODIFY_LINE_PREFIX), EM_SETSEL, 1, 4);
		CenterDlgInParent(hwnd);
	}
	return FALSE;

	case WM_DESTROY:
		ResizeDlg_Destroy(hwnd, &cxInsertTagDlg, &cyInsertTagDlg);
		return FALSE;

	case WM_SIZE: {
		int dx;
		int dy;

		ResizeDlg_Size(hwnd, lParam, &dx, &dy);
		const int cy = ResizeDlg_CalcDeltaY2(hwnd, dy, 75, IDC_MODIFY_LINE_PREFIX, IDC_MODIFY_LINE_APPEND);
		HDWP hdwp = BeginDeferWindowPos(6);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_RESIZEGRIP2, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDOK, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDCANCEL, dx, dy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_PREFIX, dx, cy, SWP_NOMOVE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_APPEND, 0, cy, SWP_NOSIZE);
		hdwp = DeferCtlPos(hdwp, hwnd, IDC_MODIFY_LINE_TIP2, 0, cy, SWP_NOSIZE);
		EndDeferWindowPos(hdwp);
		ResizeDlgCtl(hwnd, IDC_MODIFY_LINE_APPEND, dx, dy - cy);
	}
	return TRUE;

	case WM_GETMINMAXINFO:
		ResizeDlg_GetMinMaxInfo(hwnd, lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_MODIFY_LINE_PREFIX: {
			if (HIWORD(wParam) == EN_CHANGE) {
				DStringW wszOpen = DSTRINGW_INIT;
				BOOL bClear = TRUE;

				DStringW_GetDlgItemText(&wszOpen, hwnd, IDC_MODIFY_LINE_PREFIX);
				const int len = lstrlen(wszOpen.buffer);
				if (len >= 3) {
					LPCWSTR pwsz1 = StrChr(wszOpen.buffer, L'<');
					if (pwsz1 != NULL) {
						LPWSTR wchIns = (LPWSTR)NP2HeapAlloc((len + 5) * sizeof(WCHAR));
						lstrcpy(wchIns, L"</");
						int	cchIns = 2;
						const WCHAR *pwCur = pwsz1 + 1;

						while (*pwCur &&
								*pwCur != L'<' &&
								*pwCur != L'>' &&
								*pwCur != L' ' &&
								*pwCur != L'\t' &&
								*pwCur != L'\r' &&
								*pwCur != L'\n' &&
								(StrChr(L":_-.", *pwCur) || isalnum(*pwCur))) {
							wchIns[cchIns++] = *pwCur++;
						}

						while (*pwCur && *pwCur != L'>') {
							pwCur++;
						}

						if (*pwCur == L'>' && *(pwCur - 1) != L'/') {
							wchIns[cchIns++] = L'>';
							wchIns[cchIns] = L'\0';

							if (cchIns > 3 && !(
									StrCaseEqual(wchIns, L"</base>") &&
									StrCaseEqual(wchIns, L"</bgsound>") &&
									StrCaseEqual(wchIns, L"</br>") &&
									StrCaseEqual(wchIns, L"</embed>") &&
									StrCaseEqual(wchIns, L"</hr>") &&
									StrCaseEqual(wchIns, L"</img>") &&
									StrCaseEqual(wchIns, L"</input>") &&
									StrCaseEqual(wchIns, L"</link>") &&
									StrCaseEqual(wchIns, L"</meta>"))) {

								SetDlgItemText(hwnd, IDC_MODIFY_LINE_APPEND, wchIns);
								bClear = FALSE;
							}
						}
						NP2HeapFree(wchIns);
					}
				}

				if (bClear) {
					SetDlgItemText(hwnd, IDC_MODIFY_LINE_APPEND, L"");
				}
				DStringW_Free(&wszOpen);
			}
		}
		break;

		case IDOK: {
			DStringW wszOpen = DSTRINGW_INIT;
			DStringW wszClose = DSTRINGW_INIT;
			DStringW_GetDlgItemText(&wszOpen, hwnd, IDC_MODIFY_LINE_PREFIX);
			DStringW_GetDlgItemText(&wszClose, hwnd, IDC_MODIFY_LINE_APPEND);

			EditEncloseSelection(wszOpen.buffer, wszClose.buffer);
			DStringW_Free(&wszOpen);
			DStringW_Free(&wszClose);
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditInsertTagDlg()
//
void EditInsertTagDlg(HWND hwnd) {
	ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_INSERTTAG), hwnd, EditInsertTagDlgProc, 0);
}

void EditInsertDateTime(BOOL bShort) {
	WCHAR tchDateTime[256];
	WCHAR tchTemplate[256];

	SYSTEMTIME st;
	GetLocalTime(&st);

	if (IniGetString(INI_SECTION_NAME_FLAGS, bShort ? L"DateTimeShort" : L"DateTimeLong",
					 L"", tchTemplate, COUNTOF(tchTemplate))) {
		struct tm sst;
		sst.tm_isdst	= -1;
		sst.tm_sec		= (int)st.wSecond;
		sst.tm_min		= (int)st.wMinute;
		sst.tm_hour		= (int)st.wHour;
		sst.tm_mday		= (int)st.wDay;
		sst.tm_mon		= (int)st.wMonth - 1;
		sst.tm_year		= (int)st.wYear - 1900;
		sst.tm_wday		= (int)st.wDayOfWeek;
		mktime(&sst);
		wcsftime(tchDateTime, COUNTOF(tchDateTime), tchTemplate, &sst);
	} else {
		WCHAR tchDate[128];
		WCHAR tchTime[128];
		GetDateFormat(LOCALE_USER_DEFAULT, bShort ? DATE_SHORTDATE : DATE_LONGDATE,
					  &st, NULL, tchDate, COUNTOF(tchDate));
		GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, tchTime, COUNTOF(tchTime));

		wsprintf(tchDateTime, L"%s %s", tchTime, tchDate);
	}

	const UINT cpEdit = SciCall_GetCodePage();
	char mszBuf[256 * kMaxMultiByteCount];
	WideCharToMultiByte(cpEdit, 0, tchDateTime, -1, mszBuf, COUNTOF(mszBuf), NULL, NULL);
	SciCall_ReplaceSel(mszBuf);
}

void EditUpdateTimestampMatchTemplate(HWND hwnd) {
	WCHAR wchFind[256] = {0};
	IniGetString(INI_SECTION_NAME_FLAGS, L"TimeStamp", L"\\$Date:[^\\$]+\\$ | $Date: %Y/%m/%d %H:%M:%S $", wchFind, COUNTOF(wchFind));

	WCHAR wchTemplate[256] = {0};
	WCHAR *pwchSep;
	if ((pwchSep = StrChr(wchFind, L'|')) != NULL) {
		lstrcpy(wchTemplate, pwchSep + 1);
		*pwchSep = 0;
	}

	StrTrim(wchFind, L" ");
	StrTrim(wchTemplate, L" ");

	if (StrIsEmpty(wchFind) || StrIsEmpty(wchTemplate)) {
		return;
	}

	SYSTEMTIME st;
	struct tm sst;
	GetLocalTime(&st);
	sst.tm_isdst = -1;
	sst.tm_sec	 = (int)st.wSecond;
	sst.tm_min	 = (int)st.wMinute;
	sst.tm_hour	 = (int)st.wHour;
	sst.tm_mday	 = (int)st.wDay;
	sst.tm_mon	 = (int)st.wMonth - 1;
	sst.tm_year	 = (int)st.wYear - 1900;
	sst.tm_wday	 = (int)st.wDayOfWeek;
	mktime(&sst);

	WCHAR wchReplace[256];
	wcsftime(wchReplace, COUNTOF(wchReplace), wchTemplate, &sst);

	const UINT cpEdit = SciCall_GetCodePage();
#if NP2_USE_DESIGNATED_INITIALIZER
	EDITFINDREPLACE efrTS = {
		.hwnd = hwnd,
		.fuFlags = SCFIND_REGEXP,
	};
#else
	EDITFINDREPLACE efrTS = { "", "", "", "", hwnd, SCFIND_REGEXP };
#endif

	WideCharToMultiByte(cpEdit, 0, wchFind, -1, efrTS.szFind, COUNTOF(efrTS.szFind), NULL, NULL);
	WideCharToMultiByte(cpEdit, 0, wchReplace, -1, efrTS.szReplace, COUNTOF(efrTS.szReplace), NULL, NULL);

	if (!SciCall_IsSelectionEmpty()) {
		EditReplaceAllInSelection(hwnd, &efrTS, TRUE);
	} else {
		EditReplaceAll(hwnd, &efrTS, TRUE);
	}
}

typedef struct UnicodeControlCharacter {
	LPCSTR uccUTF8;
	LPCSTR representation;
} UnicodeControlCharacter;

// https://en.wikipedia.org/wiki/Unicode_control_characters
// https://www.unicode.org/charts/PDF/U2000.pdf
// scintilla/scripts/GenerateCharTable.py
static const UnicodeControlCharacter kUnicodeControlCharacterTable[] = {
	{ "\xe2\x80\x8e", "LRM" },	// U+200E	LRM		Left-to-right mark
	{ "\xe2\x80\x8f", "RLM" },	// U+200F	RLM		Right-to-left mark
	{ "\xe2\x80\x8d", "ZWJ" },	// U+200D	ZWJ		Zero width joiner
	{ "\xe2\x80\x8c", "ZWNJ" },	// U+200C	ZWNJ	Zero width non-joiner
	{ "\xe2\x80\xaa", "LRE" },	// U+202A	LRE		Start of left-to-right embedding
	{ "\xe2\x80\xab", "RLE" },	// U+202B	RLE		Start of right-to-left embedding
	{ "\xe2\x80\xad", "LRO" },	// U+202D	LRO		Start of left-to-right override
	{ "\xe2\x80\xae", "RLO" },	// U+202E	RLO		Start of right-to-left override
	{ "\xe2\x80\xac", "PDF" },	// U+202C	PDF		Pop directional formatting
	{ "\xe2\x81\xae", "NADS" },	// U+206E	NADS	National digit shapes substitution
	{ "\xe2\x81\xaf", "NODS" },	// U+206F	NODS	Nominal (European) digit shapes
	{ "\xe2\x81\xab", "ASS" },	// U+206B	ASS		Activate symmetric swapping
	{ "\xe2\x81\xaa", "ISS" },	// U+206A	ISS		Inhibit symmetric swapping
	{ "\xe2\x81\xad", "AAFS" },	// U+206D	AAFS	Activate Arabic form shaping
	{ "\xe2\x81\xac", "IAFS" },	// U+206C	IAFS	Inhibit Arabic form shaping
	// Scintilla built-in, Editor::SetRepresentations()
	{ "\x1e", NULL },			// U+001E	RS		Record Separator (Block separator)
	{ "\x1f", NULL },			// U+001F	US		Unit Separator (Segment separator)
	{ "\xe2\x80\xa8", NULL },	// U+2028	LS		Line Separator
	{ "\xe2\x80\xa9", NULL },	// U+2029	PS		Paragraph Separator
	// Other
	{ "\xe2\x80\x8b", "ZWSP" },	// U+200B	ZWSP	Zero width space
	{ "\xe2\x81\xa0", "WJ" },	// U+2060	WJ		Word joiner
	{ "\xe2\x81\xa6", "LRI" },	// U+2066	LRI		Left-to-right isolate
	{ "\xe2\x81\xa7", "RLI" },	// U+2067	RLI		Right-to-left isolate
	{ "\xe2\x81\xa8", "FSI" },	// U+2068	FSI		First strong isolate
	{ "\xe2\x81\xa9", "PDI" },	// U+2069	PDI		Pop directional isolate
};

void EditInsertUnicodeControlCharacter(int menu) {
	menu = menu - IDM_INSERT_UNICODE_LRM;
	const UnicodeControlCharacter ucc = kUnicodeControlCharacterTable[menu];
	SciCall_ReplaceSel(ucc.uccUTF8);
}

void EditShowUnicodeControlCharacter(BOOL bShow) {
	for (UINT i = 0; i < (UINT)COUNTOF(kUnicodeControlCharacterTable); i++) {
		const UnicodeControlCharacter ucc = kUnicodeControlCharacterTable[i];
		if (StrIsEmptyA(ucc.representation)) {
			// built-in
			continue;
		}
		if (bShow) {
			SciCall_SetRepresentation(ucc.uccUTF8, ucc.representation);
		} else {
			SciCall_ClearRepresentation(ucc.uccUTF8);
		}
	}
}

//=============================================================================
//
// EditSortDlgProc()
//
//
static INT_PTR CALLBACK EditSortDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
	case WM_INITDIALOG: {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
		int *piSortFlags = (int *)lParam;
		const int iSortFlags = *piSortFlags;

		if (iSortFlags & SORT_DESCENDING) {
			CheckRadioButton(hwnd, IDC_SORT_ASC, IDC_SORT_SHUFFLE, IDC_SORT_DESC);
		} else if (iSortFlags & SORT_SHUFFLE) {
			CheckRadioButton(hwnd, IDC_SORT_ASC, IDC_SORT_SHUFFLE, IDC_SORT_SHUFFLE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_MERGE_DUP), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_REMOVE_DUP), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_REMOVE_UNIQUE), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_IGNORE_CASE), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_LOGICAL_NUMBER), FALSE);
		} else {
			CheckRadioButton(hwnd, IDC_SORT_ASC, IDC_SORT_SHUFFLE, IDC_SORT_ASC);
		}

		if (iSortFlags & SORT_MERGEDUP) {
			CheckDlgButton(hwnd, IDC_SORT_MERGE_DUP, BST_CHECKED);
		}

		if (iSortFlags & SORT_UNIQDUP) {
			CheckDlgButton(hwnd, IDC_SORT_REMOVE_DUP, BST_CHECKED);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_MERGE_DUP), FALSE);
		}

		if (iSortFlags & SORT_UNIQUNIQ) {
			CheckDlgButton(hwnd, IDC_SORT_REMOVE_UNIQUE, BST_CHECKED);
		}

		if (iSortFlags & SORT_NOCASE) {
			CheckDlgButton(hwnd, IDC_SORT_IGNORE_CASE, BST_CHECKED);
		}

		if (iSortFlags & SORT_LOGICAL) {
			CheckDlgButton(hwnd, IDC_SORT_LOGICAL_NUMBER, BST_CHECKED);
		}

		if (!SciCall_IsRectangleSelection()) {
			*piSortFlags &= ~SORT_COLUMN;
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_COLUMN), FALSE);
		} else {
			*piSortFlags |= SORT_COLUMN;
			CheckDlgButton(hwnd, IDC_SORT_COLUMN, BST_CHECKED);
		}
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			int *piSortFlags = (int *)GetWindowLongPtr(hwnd, DWLP_USER);
			int iSortFlags = 0;
			if (IsButtonChecked(hwnd, IDC_SORT_DESC)) {
				iSortFlags |= SORT_DESCENDING;
			}
			if (IsButtonChecked(hwnd, IDC_SORT_SHUFFLE)) {
				iSortFlags |= SORT_SHUFFLE;
			}
			if (IsButtonChecked(hwnd, IDC_SORT_MERGE_DUP)) {
				iSortFlags |= SORT_MERGEDUP;
			}
			if (IsButtonChecked(hwnd, IDC_SORT_REMOVE_DUP)) {
				iSortFlags |= SORT_UNIQDUP;
			}
			if (IsButtonChecked(hwnd, IDC_SORT_REMOVE_UNIQUE)) {
				iSortFlags |= SORT_UNIQUNIQ;
			}
			if (IsButtonChecked(hwnd, IDC_SORT_IGNORE_CASE)) {
				iSortFlags |= SORT_NOCASE;
			}
			if (IsButtonChecked(hwnd, IDC_SORT_LOGICAL_NUMBER)) {
				iSortFlags |= SORT_LOGICAL;
			}
			if (IsButtonChecked(hwnd, IDC_SORT_COLUMN)) {
				iSortFlags |= SORT_COLUMN;
			}
			*piSortFlags = iSortFlags;
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case IDC_SORT_ASC:
		case IDC_SORT_DESC:
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_MERGE_DUP), !IsButtonChecked(hwnd, IDC_SORT_REMOVE_UNIQUE));
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_REMOVE_DUP), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_REMOVE_UNIQUE), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_IGNORE_CASE), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_LOGICAL_NUMBER), TRUE);
			break;

		case IDC_SORT_SHUFFLE:
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_MERGE_DUP), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_REMOVE_DUP), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_REMOVE_UNIQUE), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_IGNORE_CASE), FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_LOGICAL_NUMBER), FALSE);
			break;

		case IDC_SORT_REMOVE_DUP:
			EnableWindow(GetDlgItem(hwnd, IDC_SORT_MERGE_DUP), !IsButtonChecked(hwnd, IDC_SORT_REMOVE_DUP));
			break;
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditSortDlg()
//
BOOL EditSortDlg(HWND hwnd, int *piSortFlags) {
	const INT_PTR iResult = ThemedDialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_SORT), hwnd, EditSortDlgProc, (LPARAM)piSortFlags);
	return iResult == IDOK;
}

void EditSelectionAction(int action) {
	static const LPCWSTR kActionKeys[] = {
		L"GoogleSearchUrl",
		L"BingSearchUrl",
		L"WikiSearchUrl",
		L"CustomAction1",
		L"CustomAction2",
	};

	WCHAR szCmdTemplate[256];
	action -= CMD_ONLINE_SEARCH_GOOGLE;
	LPCWSTR actionKey = kActionKeys[action];
	BOOL bCmdEnabled = IniGetString(INI_SECTION_NAME_FLAGS, actionKey, L"", szCmdTemplate, COUNTOF(szCmdTemplate));
	if (!bCmdEnabled && action < 3) {
		bCmdEnabled = GetString(IDS_GOOGLE_SEARCH_URL + action, szCmdTemplate, COUNTOF(szCmdTemplate));
	}
	if (!bCmdEnabled) {
		return;
	}

	int cchEscapedW;
	LPWSTR pszEscapedW = EditURLEncodeSelection(&cchEscapedW, TRUE);
	if (pszEscapedW == NULL) {
		return;
	}

	LPWSTR lpszCommand = (LPWSTR)NP2HeapAlloc(sizeof(WCHAR) * (cchEscapedW + COUNTOF(szCmdTemplate) + MAX_PATH + 32));
	const size_t cbCommand = NP2HeapSize(lpszCommand);
	wsprintf(lpszCommand, szCmdTemplate, pszEscapedW);
	ExpandEnvironmentStringsEx(lpszCommand, (DWORD)(cbCommand / sizeof(WCHAR)));

	LPWSTR lpszArgs = (LPWSTR)NP2HeapAlloc(cbCommand);
	ExtractFirstArgument(lpszCommand, lpszCommand, lpszArgs);

	WCHAR wchDirectory[MAX_PATH] = L"";
	if (StrNotEmpty(szCurFile)) {
		lstrcpy(wchDirectory, szCurFile);
		PathRemoveFileSpec(wchDirectory);
	}

	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOZONECHECKS;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = lpszCommand;
	sei.lpParameters = lpszArgs;
	sei.lpDirectory = wchDirectory;
	sei.nShow = SW_SHOWNORMAL;

	ShellExecuteEx(&sei);

	NP2HeapFree(pszEscapedW);
	NP2HeapFree(lpszCommand);
	NP2HeapFree(lpszArgs);
}

void TryBrowseFile(HWND hwnd, LPCWSTR pszFile, BOOL bWarn) {
	WCHAR tchParam[MAX_PATH + 4] = L"";
	WCHAR tchExeFile[MAX_PATH + 4] = L"";
	WCHAR tchTemp[MAX_PATH + 4];

	if (IniGetString(INI_SECTION_NAME_FLAGS, L"filebrowser.exe", L"", tchTemp, COUNTOF(tchTemp))) {
		ExtractFirstArgument(tchTemp, tchExeFile, tchParam);
	}
	if (StrIsEmpty(tchExeFile)) {
		lstrcpy(tchExeFile, L"metapath.exe");
	}
	if (PathIsRelative(tchExeFile)) {
		GetModuleFileName(NULL, tchTemp, COUNTOF(tchTemp));
		PathRemoveFileSpec(tchTemp);
		PathAppend(tchTemp, tchExeFile);
		if (PathFileExists(tchTemp)) {
			lstrcpy(tchExeFile, tchTemp);
		}
	}

	if (StrNotEmpty(tchParam) && StrNotEmpty(pszFile)) {
		StrCatBuff(tchParam, L" ", COUNTOF(tchParam));
	}

	if (StrNotEmpty(pszFile)) {
		lstrcpy(tchTemp, pszFile);
		PathQuoteSpaces(tchTemp);
		StrCatBuff(tchParam, tchTemp, COUNTOF(tchParam));
	}

	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOZONECHECKS;
	sei.hwnd = hwnd;
	sei.lpVerb = NULL;
	sei.lpFile = tchExeFile;
	sei.lpParameters = tchParam;
	sei.lpDirectory = NULL;
	sei.nShow = SW_SHOWNORMAL;

	ShellExecuteEx(&sei);

	if ((INT_PTR)sei.hInstApp < 32) {
		if (bWarn) {
			if (MsgBox(MBYESNOWARN, IDS_ERR_BROWSE) == IDYES) {
				OpenHelpLink(hwnd, IDM_HELP_LATEST_RELEASE);
			}
		} else if (StrNotEmpty(pszFile)) {
			OpenContainingFolder(hwnd, pszFile, FALSE);
		}
	}
}

char* EditGetStringAroundCaret(LPCSTR delimiters) {
	const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
	const Sci_Line iLine = SciCall_LineFromPosition(iCurrentPos);
	Sci_Position iLineStart = SciCall_PositionFromLine(iLine);
	Sci_Position iLineEnd = SciCall_GetLineEndPosition(iLine);
	if (iLineStart == iLineEnd) {
		// empty line
		return NULL;
	}

	struct Sci_TextToFind ft = { { (Sci_PositionCR)iCurrentPos, 0 }, delimiters, { 0, 0 } };
	const int findFlag = SCFIND_REGEXP | SCFIND_POSIX;

	// forward
	if (iCurrentPos < iLineEnd) {
		ft.chrg.cpMax = (Sci_PositionCR)iLineEnd;
		const Sci_Position iPos = SciCall_FindText(findFlag, &ft);
		if (iPos >= 0) {
			iLineEnd = SciCall_PositionBefore(ft.chrgText.cpMax);
		}
	}

	// backword
	if (iCurrentPos > iLineStart) {
		ft.chrg.cpMax = (Sci_PositionCR)iLineStart;
		const Sci_Position iPos = SciCall_FindText(findFlag, &ft);
		if (iPos >= 0) {
			iLineStart = SciCall_PositionAfter(ft.chrgText.cpMin);
		}
	}

	if (iLineStart >= iLineEnd) {
		return NULL;
	}

	char *mszSelection = (char *)NP2HeapAlloc(iLineEnd - iLineStart + 1);
	struct Sci_TextRange tr = { { (Sci_PositionCR)iLineStart, (Sci_PositionCR)iLineEnd }, mszSelection };
	SciCall_GetTextRange(&tr);;

	return mszSelection;
}

extern BOOL bOpenFolderWithMetapath;

void EditOpenSelection(int type) {
	Sci_Position cchSelection = SciCall_GetSelTextLength();
	char *mszSelection = NULL;
	if (cchSelection > 1) {
		mszSelection = (char *)NP2HeapAlloc(cchSelection);
		SciCall_GetSelText(mszSelection);
		char *lpsz = strpbrk(mszSelection, "\r\n\t");
		if (lpsz) {
			*lpsz = '\0';
		}
	} else {
		// string terminated by space or quotes
		mszSelection = EditGetStringAroundCaret("[\\s'`\"<>|*,;]");
	}

	if (mszSelection == NULL) {
		return;
	}
	/* remove quotes, spaces and some invalid filename characters (except '/', '\' and '?') */
	StrTrimA(mszSelection, " \t\r\n'`\"<>|:*,;");
	cchSelection = strlen(mszSelection);
	if (cchSelection != 0) {
		LPWSTR wszSelection = (LPWSTR)NP2HeapAlloc((max_pos(MAX_PATH, cchSelection) + 32) * sizeof(WCHAR));
		LPWSTR link = wszSelection + 16;

		const UINT cpEdit = SciCall_GetCodePage();
		MultiByteToWideChar(cpEdit, 0, mszSelection, -1, link, (int)cchSelection);

		WCHAR path[MAX_PATH];
		WCHAR wchDirectory[MAX_PATH] = L"";
		DWORD dwAttributes = GetFileAttributes(link);
		if (dwAttributes == INVALID_FILE_ATTRIBUTES) {
			if (StrNotEmpty(szCurFile)) {
				lstrcpy(wchDirectory, szCurFile);
				PathRemoveFileSpec(wchDirectory);
				PathCombine(path, wchDirectory, link);
				dwAttributes = GetFileAttributes(path);
			}
			if (dwAttributes == INVALID_FILE_ATTRIBUTES && GetFullPathName(link, COUNTOF(path), path, NULL)) {
				dwAttributes = GetFileAttributes(path);
			}
			if (dwAttributes != INVALID_FILE_ATTRIBUTES) {
				lstrcpy(link, path);
			}
		} else if (GetFullPathName(link, COUNTOF(path), path, NULL)) {
			lstrcpy(link, path);
		}

		if (type == 4) { // containing folder
			if (dwAttributes == INVALID_FILE_ATTRIBUTES) {
				type = 0;
			}
		} else if (dwAttributes != INVALID_FILE_ATTRIBUTES) {
			if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				type = 3;
			} else {
				const BOOL can = Style_CanOpenFile(link);
				// open supported file in a new window
				type = can ? 2 : 1;
			}
		} else if (StrChr(link, L':')) { // link
			// TODO: check scheme
			type = 1;
		} else if (StrChr(link, L'@')) { // email
			lstrcpy(wszSelection, L"mailto:");
			lstrcpy(wszSelection + CSTRLEN(L"mailto:"), link);
			type = 1;
			link = wszSelection;
		}

		switch (type) {
		case 1:
			ShellExecute(hwndMain, L"open", link, NULL, NULL, SW_SHOWNORMAL);
			break;

		case 2: {
			WCHAR szModuleName[MAX_PATH];
			GetModuleFileName(NULL, szModuleName, COUNTOF(szModuleName));

			lstrcpyn(wchDirectory, link, COUNTOF(wchDirectory));
			PathRemoveFileSpec(wchDirectory);
			PathQuoteSpaces(link);

			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_NOZONECHECKS;
			sei.hwnd = hwndMain;
			sei.lpVerb = NULL;
			sei.lpFile = szModuleName;
			sei.lpParameters = link;
			sei.lpDirectory = wchDirectory;
			sei.nShow = SW_SHOWNORMAL;

			ShellExecuteEx(&sei);
		}
		break;

		case 3:
			if (bOpenFolderWithMetapath) {
				TryBrowseFile(hwndMain, link, FALSE);
			} else {
				OpenContainingFolder(hwndMain, link, FALSE);
			}
			break;

		case 4:
			OpenContainingFolder(hwndMain, link, TRUE);
			break;
		}

		NP2HeapFree(wszSelection);
	}

	NP2HeapFree(mszSelection);
}

//=============================================================================
//
// FileVars_Init()
//

extern BOOL bNoEncodingTags;
extern int fNoFileVariables;

BOOL FileVars_Init(LPCSTR lpData, DWORD cbData, LPFILEVARS lpfv) {
	ZeroMemory(lpfv, sizeof(FILEVARS));
	if ((fNoFileVariables && bNoEncodingTags) || !lpData || !cbData) {
		return TRUE;
	}

	char tch[512];
	strncpy(tch, lpData, min_u(cbData, sizeof(tch)));
	tch[sizeof(tch) - 1] = '\0';
	const BOOL utf8Sig = IsUTF8Signature(lpData);
	BOOL bDisableFileVariables = FALSE;

	if (!fNoFileVariables) {
		int i;
		if (FileVars_ParseInt(tch, "enable-local-variables", &i) && (!i)) {
			bDisableFileVariables = TRUE;
		}

		if (!bDisableFileVariables) {
			if (FileVars_ParseInt(tch, "tab-width", &i)) {
				lpfv->iTabWidth = clamp_i(i, 1, 256);
				lpfv->mask |= FV_TABWIDTH;
			}

			if (FileVars_ParseInt(tch, "c-basic-indent", &i)) {
				lpfv->iIndentWidth = clamp_i(i, 0, 256);
				lpfv->mask |= FV_INDENTWIDTH;
			}

			if (FileVars_ParseInt(tch, "indent-tabs-mode", &i)) {
				lpfv->bTabsAsSpaces = i == 0;
				lpfv->mask |= FV_TABSASSPACES;
			}

			if (FileVars_ParseInt(tch, "c-tab-always-indent", &i)) {
				lpfv->bTabIndents = i != 0;
				lpfv->mask |= FV_TABINDENTS;
			}

			if (FileVars_ParseInt(tch, "truncate-lines", &i)) {
				lpfv->fWordWrap = i == 0;
				lpfv->mask |= FV_WORDWRAP;
			}

			if (FileVars_ParseInt(tch, "fill-column", &i)) {
				lpfv->iLongLinesLimit = clamp_i(i, 0, NP2_LONG_LINE_LIMIT);
				lpfv->mask |= FV_LONGLINESLIMIT;
			}
		}
	}

	if (!utf8Sig && !bNoEncodingTags && !bDisableFileVariables) {
		if (FileVars_ParseStr(tch, "encoding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding)) ||
			FileVars_ParseStr(tch, "charset", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding)) ||
			FileVars_ParseStr(tch, "coding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
			lpfv->mask |= FV_ENCODING;
		}
	}

	if (!fNoFileVariables && !bDisableFileVariables) {
		if (FileVars_ParseStr(tch, "mode", lpfv->tchMode, COUNTOF(lpfv->tchMode))) {
			lpfv->mask |= FV_MODE;
		}
	}

	if (lpfv->mask == 0 && cbData > COUNTOF(tch)) {
		strncpy(tch, lpData + cbData - COUNTOF(tch) + 1, COUNTOF(tch) - 1);
		if (!fNoFileVariables) {
			int i;
			if (FileVars_ParseInt(tch, "enable-local-variables", &i) && (!i)) {
				bDisableFileVariables = TRUE;
			}

			if (!bDisableFileVariables) {
				if (FileVars_ParseInt(tch, "tab-width", &i)) {
					lpfv->iTabWidth = clamp_i(i, 1, 256);
					lpfv->mask |= FV_TABWIDTH;
				}

				if (FileVars_ParseInt(tch, "c-basic-indent", &i)) {
					lpfv->iIndentWidth = clamp_i(i, 0, 256);
					lpfv->mask |= FV_INDENTWIDTH;
				}

				if (FileVars_ParseInt(tch, "indent-tabs-mode", &i)) {
					lpfv->bTabsAsSpaces = i == 0;
					lpfv->mask |= FV_TABSASSPACES;
				}

				if (FileVars_ParseInt(tch, "c-tab-always-indent", &i)) {
					lpfv->bTabIndents = i != 0;
					lpfv->mask |= FV_TABINDENTS;
				}

				if (FileVars_ParseInt(tch, "truncate-lines", &i)) {
					lpfv->fWordWrap = i == 0;
					lpfv->mask |= FV_WORDWRAP;
				}

				if (FileVars_ParseInt(tch, "fill-column", &i)) {
					lpfv->iLongLinesLimit = clamp_i(i, 0, NP2_LONG_LINE_LIMIT);
					lpfv->mask |= FV_LONGLINESLIMIT;
				}
			}
		}

		if (!utf8Sig && !bNoEncodingTags && !bDisableFileVariables) {
			if (FileVars_ParseStr(tch, "encoding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding)) ||
				FileVars_ParseStr(tch, "charset", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding)) ||
				FileVars_ParseStr(tch, "coding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
				lpfv->mask |= FV_ENCODING;
			}
		}

		if (!fNoFileVariables && !bDisableFileVariables) {
			if (FileVars_ParseStr(tch, "mode", lpfv->tchMode, COUNTOF(lpfv->tchMode))) {
				lpfv->mask |= FV_MODE;
			}
		}
	}

	if (lpfv->mask & FV_ENCODING) {
		lpfv->iEncoding = Encoding_MatchA(lpfv->tchEncoding);
	}

	return TRUE;
}

//=============================================================================
//
// FileVars_Apply()
//

extern BOOL fWordWrap;
extern BOOL fWordWrapG;
extern int iWordWrapMode;
extern int iLongLinesLimit;
extern int iLongLinesLimitG;
extern int iWrapCol;

BOOL FileVars_Apply(LPCFILEVARS lpfv) {
	if (lpfv->mask & FV_TABWIDTH) {
		iTabWidth = lpfv->iTabWidth;
	} else {
		iTabWidth = iTabWidthG;
	}
	SciCall_SetTabWidth(iTabWidth);

	if (lpfv->mask & FV_INDENTWIDTH) {
		iIndentWidth = lpfv->iIndentWidth;
	} else if (lpfv->mask & FV_TABWIDTH) {
		iIndentWidth = 0;
	} else {
		iIndentWidth = iIndentWidthG;
	}
	SciCall_SetIndent(iIndentWidth);

	if (lpfv->mask & FV_TABSASSPACES) {
		bTabsAsSpaces = lpfv->bTabsAsSpaces;
	} else {
		bTabsAsSpaces = bTabsAsSpacesG;
	}
	SciCall_SetUseTabs(!bTabsAsSpaces);

	if (lpfv->mask & FV_TABINDENTS) {
		bTabIndents = lpfv->bTabIndents;
	} else {
		bTabIndents = bTabIndentsG;
	}
	SciCall_SetTabIndents(bTabIndents);

	if (lpfv->mask & FV_WORDWRAP) {
		fWordWrap = lpfv->fWordWrap;
	} else {
		fWordWrap = fWordWrapG;
	}

	SciCall_SetWrapMode(fWordWrap? iWordWrapMode : SC_WRAP_NONE);

	if (lpfv->mask & FV_LONGLINESLIMIT) {
		iLongLinesLimit = lpfv->iLongLinesLimit;
	} else {
		iLongLinesLimit = iLongLinesLimitG;
	}
	SciCall_SetEdgeColumn(iLongLinesLimit);

	iWrapCol = 0;

	return TRUE;
}

//=============================================================================
//
// FileVars_ParseInt()
//
BOOL FileVars_ParseInt(LPCSTR pszData, LPCSTR pszName, int *piValue) {
	LPCSTR pvStart = pszData;

	while ((pvStart = strstr(pvStart, pszName)) != NULL) {
		const unsigned char chPrev = (pvStart > pszData) ? *(pvStart - 1) : 0;
		if (!isalpha(chPrev) && chPrev != '-' && chPrev != '_') {
			pvStart += strlen(pszName);
			while (*pvStart == ' ') {
				pvStart++;
			}
			if (*pvStart == ':' || *pvStart == '=') {
				break;
			}
		} else {
			pvStart += strlen(pszName);
		}
	}

	if (pvStart) {
		while (*pvStart && strchr(":=\"' \t", *pvStart)) {
			pvStart++;
		}

		char *pvEnd;
		*piValue = (int)strtol(pvStart, &pvEnd, 10);
		if (pvEnd != pvStart) {
			return TRUE;
		}

		switch (*pvStart) {
		case 't':
		case 'y':
			*piValue = 1;
			return TRUE;
		case 'f':
		case 'n':
			*piValue = 0;
			return FALSE;
		}
	}

	return FALSE;
}

//=============================================================================
//
// FileVars_ParseStr()
//
BOOL FileVars_ParseStr(LPCSTR pszData, LPCSTR pszName, char *pszValue, int cchValue) {
	LPCSTR pvStart = pszData;

	while ((pvStart = strstr(pvStart, pszName)) != NULL) {
		const unsigned char chPrev = (pvStart > pszData) ? *(pvStart - 1) : 0;
		if (!isalpha(chPrev) && chPrev != '-' && chPrev != '_') {
			pvStart += strlen(pszName);
			while (*pvStart == ' ') {
				pvStart++;
			}
			if (*pvStart == ':' || *pvStart == '=') {
				break;
			}
		} else {
			pvStart += strlen(pszName);
		}
	}

	if (pvStart) {
		BOOL bQuoted = FALSE;

		while (*pvStart && strchr(":=\"' \t", *pvStart)) {
			if (*pvStart == '\'' || *pvStart == '"') {
				bQuoted = TRUE;
			}
			pvStart++;
		}

		char tch[32];
		strncpy(tch, pvStart, COUNTOF(tch) - 1);

		char *pvEnd = tch;
		while (*pvEnd && (isalnum((unsigned char)(*pvEnd)) || strchr("+-/_", *pvEnd) || (bQuoted && *pvEnd == ' '))) {
			pvEnd++;
		}
		*pvEnd = '\0';
		StrTrimA(tch, " \t:=\"'");

		*pszValue = '\0';
		strncpy(pszValue, tch, cchValue);
		return TRUE;
	}

	return FALSE;
}

#if 0
//=============================================================================
//
// SciInitThemes()
//
static WNDPROC pfnSciWndProc = NULL;

static LRESULT CALLBACK SciThemedWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

void SciInitThemes(HWND hwnd) {
	pfnSciWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)SciThemedWndProc);
}

//=============================================================================
//
// SciThemedWndProc()
//
LRESULT CALLBACK SciThemedWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static RECT rcContent;

	switch (umsg) {
	case WM_NCCALCSIZE:
		if (wParam) {
			const LRESULT lresult = CallWindowProc(pfnSciWndProc, hwnd, WM_NCCALCSIZE, wParam, lParam);
			if (IsAppThemed()) {
				HTHEME hTheme = OpenThemeData(hwnd, L"edit");
				if (hTheme) {
					NCCALCSIZE_PARAMS *csp = (NCCALCSIZE_PARAMS *)lParam;
					BOOL bSuccess = FALSE;
					RECT rcClient;

					if (GetThemeBackgroundContentRect(hTheme, NULL, EP_EDITTEXT, ETS_NORMAL, &csp->rgrc[0], &rcClient) == S_OK) {
						InflateRect(&rcClient, -1, -1);

						rcContent.left = rcClient.left - csp->rgrc[0].left;
						rcContent.top = rcClient.top - csp->rgrc[0].top;
						rcContent.right = csp->rgrc[0].right - rcClient.right;
						rcContent.bottom = csp->rgrc[0].bottom - rcClient.bottom;

						CopyRect(&csp->rgrc[0], &rcClient);
						bSuccess = TRUE;
					}
					CloseThemeData(hTheme);

					if (bSuccess) {
						return WVR_REDRAW;
					}
				}
			}
			return lresult;
		}
		break;

	case WM_NCPAINT: {
		const LRESULT lresult = CallWindowProc(pfnSciWndProc, hwnd, WM_NCPAINT, wParam, lParam);
		if (IsAppThemed()) {
			HTHEME hTheme = OpenThemeData(hwnd, L"edit");
			if (hTheme) {
				HDC hdc = GetWindowDC(hwnd);

				RECT rcBorder;
				GetWindowRect(hwnd, &rcBorder);
				OffsetRect(&rcBorder, -rcBorder.left, -rcBorder.top);

				RECT rcClient;
				CopyRect(&rcClient, &rcBorder);
				rcClient.left += rcContent.left;
				rcClient.top += rcContent.top;
				rcClient.right -= rcContent.right;
				rcClient.bottom -= rcContent.bottom;

				ExcludeClipRect(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

				if (IsThemeBackgroundPartiallyTransparent(hTheme, EP_EDITTEXT, ETS_NORMAL)) {
					DrawThemeParentBackground(hwnd, hdc, &rcBorder);
				}

				int nState;
				if (!IsWindowEnabled(hwnd)) {
					nState = ETS_DISABLED;
				} else if (GetFocus() == hwnd) {
					nState = ETS_FOCUSED;
				} else if (SciCall_GetReadOnly()) {
					nState = ETS_READONLY;
				} else {
					nState = ETS_NORMAL;
				}

				DrawThemeBackground(hTheme, hdc, EP_EDITTEXT, nState, &rcBorder, NULL);
				CloseThemeData(hTheme);

				ReleaseDC(hwnd, hdc);
				return 0;
			}
		}
		return lresult;
	}
	}

	return CallWindowProc(pfnSciWndProc, hwnd, umsg, wParam, lParam);
}

#endif


//==============================================================================
//
// Folding Functions
//
//
#define FOLD_CHILDREN SCMOD_CTRL
#define FOLD_SIBLINGS SCMOD_SHIFT

#define MAX_EDIT_TOGGLE_FOLD_LEVEL		63
struct EditFoldStack {
	int level_count; // 1-based level number at current header line
	int level_stack[MAX_EDIT_TOGGLE_FOLD_LEVEL];
};

static void EditFoldStack_Push(struct EditFoldStack *foldStack, int level) {
	while (foldStack->level_count != 0 && level <= foldStack->level_stack[foldStack->level_count - 1]) {
		--foldStack->level_count;
	}

	foldStack->level_stack[foldStack->level_count] = level;
	++foldStack->level_count;
}

static inline BOOL IsFoldIndentationBased(int iLexer) {
	return iLexer == SCLEX_PYTHON
		|| iLexer == SCLEX_YAML
		|| iLexer == SCLEX_NULL;
}

static UINT Style_GetDefaultFoldState(int rid, int *maxLevel) {
	switch (rid) {
	case NP2LEX_TEXTFILE:
	case NP2LEX_2NDTEXTFILE:
	case NP2LEX_ANSI:
		*maxLevel = 2;
		return (1 << 1) | (1 << 2);
	case NP2LEX_CPP:
	case NP2LEX_CSHARP:
	case NP2LEX_XML:
	case NP2LEX_HTML:
	case NP2LEX_JSON:
		*maxLevel = 3;
		return (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
	case NP2LEX_JAVA:
	case NP2LEX_RC:
	case NP2LEX_SCALA:
	case NP2LEX_RUBY:
		*maxLevel = 2;
		return (1 << 0) | (1 << 1) | (1 << 2);
	case NP2LEX_INI:
		*maxLevel = 0;
		return (1 << 0);
	case NP2LEX_DIFF:
		*maxLevel = 2;
		return (1 << 0) | (1 << 2);
	case NP2LEX_PYTHON:
		*maxLevel = 3;
		return (1 << 1) | (1 << 2) | (1 << 3);
	default:
		*maxLevel = 1;
		return (1 << 0) | (1 << 1);
	}
}

static void FoldToggleNode(Sci_Line line, FOLD_ACTION *pAction, BOOL *fToggled) {
	const BOOL fExpanded = SciCall_GetFoldExpanded(line);
	FOLD_ACTION action = *pAction;
	if (action == FOLD_ACTION_SNIFF) {
		action = fExpanded ? FOLD_ACTION_FOLD : FOLD_ACTION_EXPAND;
	}

	if (action ^ fExpanded) {
		SciCall_ToggleFold(line);
		if (*fToggled == FALSE || *pAction == FOLD_ACTION_SNIFF) {
			// empty INI section not changed after toggle (issue #48).
			const BOOL after = SciCall_GetFoldExpanded(line);
			if (fExpanded != after) {
				*fToggled = TRUE;
				if (*pAction == FOLD_ACTION_SNIFF) {
					*pAction = action;
				}
			}
		}
	}
}

void FoldToggleAll(FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	SciCall_ColouriseAll();
	const Sci_Line lineCount = SciCall_GetLineCount();

	for (Sci_Line line = 0; line < lineCount; ++line) {
		const int level = SciCall_GetFoldLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			FoldToggleNode(line, &action, &fToggled);
		}
	}

	if (fToggled) {
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

void FoldToggleLevel(int lev, FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	SciCall_ColouriseAll();
	const Sci_Line lineCount = SciCall_GetLineCount();
	Sci_Line line = 0;

	if (IsFoldIndentationBased(pLexCurrent->iLexer)) {
		struct EditFoldStack foldStack = { 0, { 0 }};
		++lev;
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				EditFoldStack_Push(&foldStack, level);
				if (lev == foldStack.level_count) {
					FoldToggleNode(line, &action, &fToggled);
					line = SciCall_GetLastChild(line);
				}
			}
			++line;
		}
	} else {
		lev += SC_FOLDLEVELBASE;
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				if (lev == level) {
					FoldToggleNode(line, &action, &fToggled);
					line = SciCall_GetLastChild(line);
				}
			}
			++line;
		}
	}

	if (fToggled) {
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

void FoldToggleCurrentBlock(FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	Sci_Line line = SciCall_LineFromPosition(SciCall_GetCurrentPos());
	const int level = SciCall_GetFoldLevel(line);

	if (!(level & SC_FOLDLEVELHEADERFLAG)) {
		line = SciCall_GetFoldParent(line);
		if (line < 0) {
			return;
		}
	}

	FoldToggleNode(line, &action, &fToggled);
	if (fToggled) {
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

void FoldToggleCurrentLevel(FOLD_ACTION action) {
	SciCall_ColouriseAll();
	Sci_Line line = SciCall_LineFromPosition(SciCall_GetCurrentPos());
	int level = SciCall_GetFoldLevel(line);

	if (!(level & SC_FOLDLEVELHEADERFLAG)) {
		line = SciCall_GetFoldParent(line);
		if (line < 0) {
			return;
		}
		level = SciCall_GetFoldLevel(line);
	}

	level &= SC_FOLDLEVELNUMBERMASK;
	level -= SC_FOLDLEVELBASE;

	if (level != 0 && IsFoldIndentationBased(pLexCurrent->iLexer)) {
		level = 0;
		while (line > 0) {
			--line;
			if (!(SciCall_GetFoldLevel(line) & SC_FOLDLEVELHEADERFLAG)) {
				line = SciCall_GetFoldParent(line);
				if (line < 0) {
					break;
				}
			}
			++level;
		}
#if 1
		if (level > MAX_EDIT_TOGGLE_FOLD_LEVEL - 1) {
			return;
		}
#else
		level = min_i(level, MAX_EDIT_TOGGLE_FOLD_LEVEL - 1);
#endif
	}

	FoldToggleLevel(level, action);
}

void FoldToggleDefault(FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	int maxLevel = 0;
	SciCall_ColouriseAll();
	const UINT state = Style_GetDefaultFoldState(pLexCurrent->rid, &maxLevel);
	const Sci_Line lineCount = SciCall_GetLineCount();
	Sci_Line line = 0;

	if (IsFoldIndentationBased(pLexCurrent->iLexer)) {
		struct EditFoldStack foldStack = { 0, { 0 }};
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				EditFoldStack_Push(&foldStack, level);
				level = foldStack.level_count;
				if (state & (1U << level)) {
					FoldToggleNode(line, &action, &fToggled);
					if (level == maxLevel) {
						line = SciCall_GetLastChild(line);
					}
				}
			}
			++line;
		}
	} else {
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				level -= SC_FOLDLEVELBASE;
				if (state & (1U << level)) {
					FoldToggleNode(line, &action, &fToggled);
					if (level == maxLevel) {
						line = SciCall_GetLastChild(line);
					}
				}
			}
			++line;
		}
	}

	if (fToggled) {
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

static void FoldPerformAction(Sci_Line ln, int mode, FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	if (mode & (FOLD_CHILDREN | FOLD_SIBLINGS)) {
		// ln/lvNode: line and level of the source of this fold action
		const Sci_Line lnNode = ln;
		const int lvNode = SciCall_GetFoldLevel(lnNode) & SC_FOLDLEVELNUMBERMASK;
		const Sci_Line lnTotal = SciCall_GetLineCount();

		// lvStop: the level over which we should not cross
		int lvStop = lvNode;

		if (mode & FOLD_SIBLINGS) {
			ln = SciCall_GetFoldParent(lnNode) + 1;	 // -1 + 1 = 0 if no parent
			--lvStop;
		}

		for (; ln < lnTotal; ++ln) {
			int lv = SciCall_GetFoldLevel(ln);
			const BOOL fHeader = (lv & SC_FOLDLEVELHEADERFLAG) != 0;
			lv &= SC_FOLDLEVELNUMBERMASK;

			if (lv < lvStop || (lv == lvStop && fHeader && ln != lnNode)) {
				return;
			}
			if (fHeader && (lv == lvNode || (lv > lvNode && (mode & FOLD_CHILDREN)))) {
				FoldToggleNode(ln, &action, &fToggled);
			}
		}
	} else {
		FoldToggleNode(ln, &action, &fToggled);
	}
}

void FoldClick(Sci_Line ln, int mode) {
	static struct {
		Sci_Line ln;
		int mode;
		DWORD dwTickCount;
	} prev;

	BOOL fGotoFoldPoint = mode & FOLD_SIBLINGS;

	if (!(SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG)) {
		// Not a fold point: need to look for a double-click
		if (prev.ln == ln && prev.mode == mode && (GetTickCount() - prev.dwTickCount <= GetDoubleClickTime())) {
			prev.ln = -1; // Prevent re-triggering on a triple-click
			ln = SciCall_GetFoldParent(ln);

			if (ln >= 0 && SciCall_GetFoldExpanded(ln)) {
				fGotoFoldPoint = TRUE;
			} else {
				return;
			}
		} else {
			// Save the info needed to match this click with the next click
			prev.ln = ln;
			prev.mode = mode;
			prev.dwTickCount = GetTickCount();
			return;
		}
	}

	FoldPerformAction(ln, mode, FOLD_ACTION_SNIFF);
	if (fGotoFoldPoint) {
		EditJumpTo(ln + 1, 0);
	}
}

void FoldAltArrow(int key, int mode) {
	// Because Alt-Shift is already in use (and because the sibling fold feature
	// is not as useful from the keyboard), only the Ctrl modifier is supported

	if ((mode & (SCMOD_ALT | SCMOD_SHIFT)) == SCMOD_ALT) {
		Sci_Line ln = SciCall_LineFromPosition(SciCall_GetCurrentPos());

		// Jump to the next visible fold point
		if (key == SCK_DOWN && !(mode & SCMOD_CTRL)) {
			const Sci_Line lnTotal = SciCall_GetLineCount();
			for (ln = ln + 1; ln < lnTotal; ++ln) {
				if ((SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG) && SciCall_GetLineVisible(ln)) {
					EditJumpTo(ln + 1, 0);
					return;
				}
			}
		} else if (key == SCK_UP && !(mode & SCMOD_CTRL)) {// Jump to the previous visible fold point
			for (ln = ln - 1; ln >= 0; --ln) {
				if ((SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG) && SciCall_GetLineVisible(ln)) {
					EditJumpTo(ln + 1, 0);
					return;
				}
			}
		} else if (SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG) {// Perform a fold/unfold operation
			if (key == SCK_LEFT) {
				FoldPerformAction(ln, mode, FOLD_ACTION_FOLD);
			}
			if (key == SCK_RIGHT) {
				FoldPerformAction(ln, mode, FOLD_ACTION_EXPAND);
			}
		}
	}
}
