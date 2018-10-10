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
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include "Notepad2.h"
#include "Edit.h"
#include "Styles.h"
#include "Dialogs.h"
#include "Helpers.h"
#include "SciCall.h"
#include "resource.h"

extern HWND hwndMain;
extern DWORD dwLastIOError;
extern HWND hDlgFindReplace;
extern UINT cpLastFind;
extern BOOL bReplaceInitialized;

static EDITFINDREPLACE efrSave;
static BOOL bSwitchedFindReplace = FALSE;
static int xFindReplaceDlgSave;
static int yFindReplaceDlgSave;
extern int xFindReplaceDlg;
extern int yFindReplaceDlg;

extern int iDefaultEncoding;
extern int iDefaultEOLMode;
extern const int iLineEndings[3];
extern BOOL bFixLineEndings;
extern BOOL bAutoStripBlanks;

extern int iRenderingTechnology;
extern int iBidirectional;
extern BOOL bUseInlineIME;
extern BOOL bInlineIMEUseBlockCaret;
// Default Codepage and Character Set
extern int iDefaultCodePage;
//extern int iDefaultCharSet;
extern BOOL bLoadASCIIasUTF8;
extern BOOL bLoadNFOasOEM;
extern int iSrcEncoding;
extern int iWeakSrcEncoding;

extern int g_DOSEncoding;
extern const NP2ENCODING mEncoding[];

extern LPMRULIST mruFind;
extern LPMRULIST mruReplace;

//=============================================================================
//
// EditCreate()
//
HWND EditCreate(HWND hwndParent) {
	HWND hwnd;

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
						  L"Scintilla",
						  NULL,
						  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						  0, 0, 0, 0,
						  hwndParent,
						  (HMENU)IDC_EDIT,
						  g_hInstance,
						  NULL);

	InitScintillaHandle(hwnd);
	SendMessage(hwnd, SCI_SETBUFFEREDDRAW, (iRenderingTechnology == SC_TECHNOLOGY_DEFAULT), 0);
	SendMessage(hwnd, SCI_SETTECHNOLOGY, iRenderingTechnology, 0);
	SendMessage(hwnd, SCI_SETBIDIRECTIONAL, iBidirectional, 0);
	SendMessage(hwnd, SCI_SETIMEINTERACTION, bUseInlineIME, 0);
	SendMessage(hwnd, SCI_SETINLINEIMEUSEBLOCKCARET, bInlineIMEUseBlockCaret, 0);
	SendMessage(hwnd, SCI_SETPASTECONVERTENDINGS, 1, 0);
	SendMessage(hwnd, SCI_SETMODEVENTMASK, /*SC_MODEVENTMASKALL*/SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT, 0);
	SendMessage(hwnd, SCI_USEPOPUP, FALSE, 0);
	SendMessage(hwnd, SCI_SETSCROLLWIDTH, 2048, 0);
	SendMessage(hwnd, SCI_SETSCROLLWIDTHTRACKING, TRUE, 0);
	SendMessage(hwnd, SCI_SETENDATLASTLINE, TRUE, 0);
	SendMessage(hwnd, SCI_SETCARETSTICKY, SC_CARETSTICKY_OFF, 0);
	SendMessage(hwnd, SCI_SETXCARETPOLICY, CARET_SLOP | CARET_EVEN, 50);
	SendMessage(hwnd, SCI_SETYCARETPOLICY, CARET_EVEN, 0);
	SendMessage(hwnd, SCI_SETMULTIPLESELECTION, FALSE, 0);
	SendMessage(hwnd, SCI_SETADDITIONALSELECTIONTYPING, FALSE, 0);
	SendMessage(hwnd, SCI_SETVIRTUALSPACEOPTIONS, SCVS_NONE, 0);
	SendMessage(hwnd, SCI_SETADDITIONALCARETSBLINK, FALSE, 0);
	SendMessage(hwnd, SCI_SETADDITIONALCARETSVISIBLE, FALSE, 0);
	// style both before and after the visible text in the background
	SendMessage(hwnd, SCI_SETIDLESTYLING, SC_IDLESTYLING_ALL, 0);

	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_NEXT + (SCMOD_CTRL << 16)), SCI_PARADOWN);
	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_PRIOR + (SCMOD_CTRL << 16)), SCI_PARAUP);
	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_NEXT + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARADOWNEXTEND);
	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_PRIOR + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARAUPEXTEND);
	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_HOME + (0 << 16)), SCI_VCHOMEWRAP);
	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_END + (0 << 16)), SCI_LINEENDWRAP);
	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_HOME + (SCMOD_SHIFT << 16)), SCI_VCHOMEWRAPEXTEND);
	SendMessage(hwnd, SCI_ASSIGNCMDKEY, (SCK_END + (SCMOD_SHIFT << 16)), SCI_LINEENDWRAPEXTEND);

	return hwnd;
}

//=============================================================================
//
// EditSetNewText()
//
extern BOOL bFreezeAppTitle;
extern FILEVARS fvCurFile;

void EditSetNewText(HWND hwnd, LPCSTR lpstrText, DWORD cbText) {
	bFreezeAppTitle = TRUE;

	if (SendMessage(hwnd, SCI_GETREADONLY, 0, 0)) {
		SendMessage(hwnd, SCI_SETREADONLY, FALSE, 0);
	}

	SendMessage(hwnd, SCI_CANCEL, 0, 0);
	SendMessage(hwnd, SCI_SETUNDOCOLLECTION, 0, 0);
	SendMessage(hwnd, SCI_EMPTYUNDOBUFFER, 0, 0);
	SendMessage(hwnd, SCI_CLEARALL, 0, 0);
	SendMessage(hwnd, SCI_MARKERDELETEALL, (WPARAM) - 1, 0);
	SendMessage(hwnd, SCI_SETSCROLLWIDTH, 2048, 0);
	SendMessage(hwnd, SCI_SETXOFFSET, 0, 0);

	FileVars_Apply(hwnd, &fvCurFile);

	if (cbText > 0) {
		SendMessage(hwnd, SCI_ADDTEXT, cbText, (LPARAM)lpstrText);
	}

	SendMessage(hwnd, SCI_SETUNDOCOLLECTION, 1, 0);
	SendMessage(hwnd, EM_EMPTYUNDOBUFFER, 0, 0);
	SendMessage(hwnd, SCI_SETSAVEPOINT, 0, 0);
	SendMessage(hwnd, SCI_GOTOPOS, 0, 0);
	SendMessage(hwnd, SCI_CHOOSECARETX, 0, 0);

	bFreezeAppTitle = FALSE;
}

//=============================================================================
//
// EditConvertText()
//
BOOL EditConvertText(HWND hwnd, UINT cpSource, UINT cpDest, BOOL bSetSavePoint) {
	int length;

	if (cpSource == cpDest) {
		return TRUE;
	}

	length = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);

	if (length == 0) {
		SendMessage(hwnd, SCI_CANCEL, 0, 0);
		SendMessage(hwnd, SCI_SETUNDOCOLLECTION, 0, 0);
		SendMessage(hwnd, SCI_EMPTYUNDOBUFFER, 0, 0);
		SendMessage(hwnd, SCI_CLEARALL, 0, 0);
		SendMessage(hwnd, SCI_MARKERDELETEALL, (WPARAM) - 1, 0);
		SendMessage(hwnd, SCI_SETCODEPAGE, cpDest, 0);
		SendMessage(hwnd, SCI_SETUNDOCOLLECTION, 1, 0);
		SendMessage(hwnd, EM_EMPTYUNDOBUFFER, 0, 0);
		SendMessage(hwnd, SCI_GOTOPOS, 0, 0);
		SendMessage(hwnd, SCI_CHOOSECARETX, 0, 0);

		if (bSetSavePoint) {
			SendMessage(hwnd, SCI_SETSAVEPOINT, 0, 0);
		}
	} else {
		struct Sci_TextRange tr = { { 0, -1 }, NULL };
		int cbText, cbwText;
		char *pchText;
		WCHAR *pwchText;

		pchText = NP2HeapAlloc(length * sizeof(WCHAR) + 1);

		tr.lpstrText = pchText;
		SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

		pwchText = NP2HeapAlloc((length + 1) * sizeof(WCHAR));
		cbwText	 = MultiByteToWideChar(cpSource, 0, pchText, length, pwchText, length);
		cbText	 = WideCharToMultiByte(cpDest, 0, pwchText, cbwText, pchText, length * sizeof(WCHAR), NULL, NULL);

		SendMessage(hwnd, SCI_CANCEL, 0, 0);
		SendMessage(hwnd, SCI_SETUNDOCOLLECTION, 0, 0);
		SendMessage(hwnd, SCI_EMPTYUNDOBUFFER, 0, 0);
		SendMessage(hwnd, SCI_CLEARALL, 0, 0);
		SendMessage(hwnd, SCI_MARKERDELETEALL, (WPARAM) - 1, 0);
		SendMessage(hwnd, SCI_SETCODEPAGE, cpDest, 0);
		SendMessage(hwnd, SCI_ADDTEXT, cbText, (LPARAM)pchText);
		SendMessage(hwnd, SCI_EMPTYUNDOBUFFER, 0, 0);
		SendMessage(hwnd, SCI_SETUNDOCOLLECTION, 1, 0);
		SendMessage(hwnd, SCI_GOTOPOS, 0, 0);
		SendMessage(hwnd, SCI_CHOOSECARETX, 0, 0);

		NP2HeapFree(pchText);
		NP2HeapFree(pwchText);
	}

	return TRUE;
}

//=============================================================================
//
// EditGetClipboardText()
//
char *EditGetClipboardText(HWND hwnd) {
	HANDLE 	hmem;
	WCHAR 	*pwch;
	char 	*pmch;
	char 	*ptmp;
	int 	wlen, mlen;
	UINT	codepage;
	int 	eolmode;

	if (!IsClipboardFormatAvailable(CF_UNICODETEXT) || !OpenClipboard(GetParent(hwnd))) {
		return NULL;
	}

	hmem = GetClipboardData(CF_UNICODETEXT);
	pwch = GlobalLock(hmem);

	wlen = lstrlenW(pwch);

	codepage = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
	eolmode	 = (int)SendMessage(hwnd, SCI_GETEOLMODE, 0, 0);

	mlen = WideCharToMultiByte(codepage, 0, pwch, wlen + 1, NULL, 0, 0, 0) - 1;
	pmch = LocalAlloc(LPTR, mlen + 1);
	ptmp = NP2HeapAlloc(mlen * 4 + 1);

	if (pmch && ptmp) {
		int mlen2;
		const char *s = pmch;
		char *d = ptmp;

		WideCharToMultiByte(codepage, 0, pwch, wlen + 1, pmch, mlen + 1, NULL, NULL);

		for (int i = 0; (i < mlen) && (*s != 0); i++) {
			if (*s == '\n' || *s == '\r') {
				if (eolmode == SC_EOL_CR) {
					*d++ = '\r';
				} else if (eolmode == SC_EOL_LF) {
					*d++ = '\n';
				} else { // eolmode == SC_EOL_CRLF
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
		mlen2 = (int)(d - ptmp) - 1;

		LocalFree(pmch);
		pmch = LocalAlloc(LPTR, mlen2 + 1);
		lstrcpyA(pmch, ptmp);
		NP2HeapFree(ptmp);
	}

	GlobalUnlock(hmem);
	CloseClipboard();

	return pmch;
}

//=============================================================================
//
// EditCopyAppend()
//
BOOL EditCopyAppend(HWND hwnd) {
	char	*pszText;
	int		cchTextW;
	WCHAR *pszTextW;

	UINT	uCodePage;

	int iCurPos;
	int iAnchorPos;
	BOOL succ = FALSE;

	if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		SendMessage(hwnd, SCI_COPY, 0, 0);
		return TRUE;
	}

	iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos != iAnchorPos) {
		if (SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			MsgBox(MBWARN, IDS_SELRECT);
			return FALSE;
		}
		{
			int iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0);
			pszText = NP2HeapAlloc(iSelCount);
			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszText);
		}
	} else {
		int cchText = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
		pszText = NP2HeapAlloc(cchText + 1);
		SendMessage(hwnd, SCI_GETTEXT, (int)NP2HeapSize(pszText), (LPARAM)pszText);
	}

	uCodePage = (SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) ? CP_UTF8 : CP_ACP;

	cchTextW = MultiByteToWideChar(uCodePage, 0, pszText, -1, NULL, 0);
	if (cchTextW > 0) {
		const WCHAR *pszSep = L"\r\n\r\n";
		pszTextW = NP2HeapAlloc(sizeof(WCHAR) * (CSTRLEN(L"\r\n\r\n") + cchTextW + 1));
		lstrcpy(pszTextW, pszSep);
		MultiByteToWideChar(uCodePage, 0, pszText, -1, StrEnd(pszTextW), (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));
	} else {
		pszTextW = L"";
	}

	NP2HeapFree(pszText);

	if (OpenClipboard(GetParent(hwnd))) {
		HANDLE hOld;
		WCHAR *pszOld;

		HANDLE hNew;
		WCHAR *pszNew;

		hOld = GetClipboardData(CF_UNICODETEXT);
		pszOld = GlobalLock(hOld);

		hNew = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
						   sizeof(WCHAR) * (lstrlen(pszOld) + lstrlen(pszTextW) + 1));
		pszNew = GlobalLock(hNew);

		lstrcpy(pszNew, pszOld);
		lstrcat(pszNew, pszTextW);

		GlobalUnlock(hOld);
		GlobalUnlock(hNew);

		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hNew);
		CloseClipboard();

		succ = TRUE;
	}

	if (cchTextW > 0) {
		NP2HeapFree(pszTextW);
	}
	return succ;
}

//=============================================================================
//
// EditDetectEOLMode() - moved here to handle Unicode files correctly
//
int EditDetectEOLMode(HWND hwnd, LPCSTR lpData) {
	UNREFERENCED_PARAMETER(hwnd);

	int iEOLMode = iLineEndings[iDefaultEOLMode];
	LPCSTR cp = lpData ? StrPBrkA(lpData, "\r\n") : NULL;

	if (!cp) {
		return iEOLMode;
	}

	if (*cp == '\r') {
		if (*(cp + 1) == '\n') {
			iEOLMode = SC_EOL_CRLF;
		} else {
			iEOLMode = SC_EOL_CR;
		}
	} else {
		iEOLMode = SC_EOL_LF;
	}

	return iEOLMode;
}

//=============================================================================
//
// EditLoadFile()
//
extern DWORD dwFileLoadWarningMB;
BOOL EditLoadFile(HWND hwnd, LPWSTR pszFile, BOOL bSkipEncodingDetection,
				  int *iEncoding, int *iEOLMode, BOOL *pbUnicodeErr, BOOL *pbFileTooBig) {
	HANDLE hFile;

	DWORD	 dwFileSize;
	DWORD	 dwBufSize;
	BOOL	 bReadSuccess;

	char *lpData;
	DWORD cbData;
	//char	*cp;
	int _iDefaultEncoding;

	BOOL bBOM = FALSE;
	BOOL bReverse = FALSE;
	BOOL utf8Sig;
	BOOL bPreferOEM = FALSE;

	*pbUnicodeErr = FALSE;
	*pbFileTooBig = FALSE;

	hFile = CreateFile(pszFile,
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

	// calculate buffer limit
	dwFileSize = GetFileSize(hFile, NULL);
	dwBufSize	 = dwFileSize + 10;

	// Check if a warning message should be displayed for large files
	if (dwFileLoadWarningMB != 0 && dwFileLoadWarningMB * 1024 * 1024 < dwFileSize) {
		if (InfoBox(MBYESNO, L"MsgFileSizeWarning", IDS_WARNLOADBIGFILE) != IDYES) {
			CloseHandle(hFile);
			*pbFileTooBig = TRUE;
			iSrcEncoding = -1;
			iWeakSrcEncoding = -1;
			return FALSE;
		}
	}

	// display real path name
	if (IsVistaAndAbove()) {
		WCHAR path[MAX_PATH] = L"";
		FARPROC pfnGetFinalPathNameByHandle = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetFinalPathNameByHandleW");
		if (pfnGetFinalPathNameByHandle && pfnGetFinalPathNameByHandle(hFile, path, MAX_PATH, /*FILE_NAME_OPENED*/0x8) > 0) {
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

	lpData = NP2HeapAlloc(dwBufSize);
	bReadSuccess = ReadFile(hFile, lpData, (DWORD)NP2HeapSize(lpData) - 2, &cbData, NULL);
	dwLastIOError = GetLastError();
	CloseHandle(hFile);

	if (!bReadSuccess) {
		NP2HeapFree(lpData);
		iSrcEncoding = -1;
		iWeakSrcEncoding = -1;
		return FALSE;
	}

	if (bLoadNFOasOEM) {
		PCWSTR pszExt = pszFile + lstrlen(pszFile) - 4;
		if (pszExt >= pszFile && (StrCaseEqual(pszExt, L".nfo") || StrCaseEqual(pszExt, L".diz"))) {
			bPreferOEM = TRUE;
		}
	}

	if (!Encoding_IsValid(iDefaultEncoding)) {
		iDefaultEncoding = CPI_UTF8;
	}

	_iDefaultEncoding = (bPreferOEM) ? g_DOSEncoding : iDefaultEncoding;
	if (iWeakSrcEncoding != -1 && Encoding_IsValid(iWeakSrcEncoding)) {
		_iDefaultEncoding = iWeakSrcEncoding;
	}

	*iEncoding = CPI_DEFAULT;
	utf8Sig = cbData? IsUTF8Signature(lpData) : FALSE;

	if (cbData == 0) {
		FileVars_Init(NULL, 0, &fvCurFile);
		*iEOLMode = iLineEndings[iDefaultEOLMode];

		if (iSrcEncoding == -1) {
			if (bLoadASCIIasUTF8 && !bPreferOEM) {
				*iEncoding = CPI_UTF8;
			} else {
				*iEncoding = _iDefaultEncoding;
			}
		} else {
			*iEncoding = iSrcEncoding;
		}

		SendMessage(hwnd, SCI_SETCODEPAGE, (mEncoding[*iEncoding].uFlags & NCP_DEFAULT) ? iDefaultCodePage : SC_CP_UTF8, 0);
		EditSetNewText(hwnd, "", 0);
		SendMessage(hwnd, SCI_SETEOLMODE, iLineEndings[iDefaultEOLMode], 0);
	} else if (!bSkipEncodingDetection &&
			   (iSrcEncoding == -1 || iSrcEncoding == CPI_UNICODE || iSrcEncoding == CPI_UNICODEBE) &&
			   (iSrcEncoding == CPI_UNICODE || iSrcEncoding == CPI_UNICODEBE || IsUnicode(lpData, cbData, &bBOM, &bReverse)) &&
			   (iSrcEncoding == CPI_UNICODE || iSrcEncoding == CPI_UNICODEBE || !utf8Sig)) {

		char *lpDataUTF8;

		if (iSrcEncoding == CPI_UNICODE) {
			bBOM = (*((UNALIGNED PWCHAR)lpData) == 0xFEFF);
			bReverse = FALSE;
		} else if (iSrcEncoding == CPI_UNICODEBE) {
			bBOM = (*((UNALIGNED PWCHAR)lpData) == 0xFFFE);
		}

		if (iSrcEncoding == CPI_UNICODEBE || bReverse) {
			_swab(lpData, lpData, cbData);
			if (bBOM) {
				*iEncoding = CPI_UNICODEBEBOM;
			} else {
				*iEncoding = CPI_UNICODEBE;
			}
		} else {
			if (bBOM) {
				*iEncoding = CPI_UNICODEBOM;
			} else {
				*iEncoding = CPI_UNICODE;
			}
		}

		lpDataUTF8 = NP2HeapAlloc(cbData * kMaxMultiByteCount + 2);
		cbData = WideCharToMultiByte(CP_UTF8, 0, (bBOM) ? (LPWSTR)lpData + 1 : (LPWSTR)lpData,
									 (bBOM) ? (cbData) / sizeof(WCHAR) : cbData / sizeof(WCHAR) + 1,
									 lpDataUTF8, (int)NP2HeapSize(lpDataUTF8), NULL, NULL);
		if (cbData == 0) {
			cbData = WideCharToMultiByte(CP_ACP, 0, (bBOM) ? (LPWSTR)lpData + 1 : (LPWSTR)lpData,
										 (-1), lpDataUTF8, (int)NP2HeapSize(lpDataUTF8), NULL, NULL);
			*pbUnicodeErr = TRUE;
		}

		SendMessage(hwnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
		FileVars_Init(lpDataUTF8, cbData - 1, &fvCurFile);
		EditSetNewText(hwnd, lpDataUTF8, cbData - 1);
		*iEOLMode = EditDetectEOLMode(hwnd, lpDataUTF8);
		NP2HeapFree(lpDataUTF8);
	} else {
		FileVars_Init(lpData, cbData, &fvCurFile);
		if (!bSkipEncodingDetection
				&& (iSrcEncoding == -1 || iSrcEncoding == CPI_UTF8 || iSrcEncoding == CPI_UTF8SIGN)
				&& ((utf8Sig
					 || FileVars_IsUTF8(&fvCurFile)
					 || (iSrcEncoding == CPI_UTF8 || iSrcEncoding == CPI_UTF8SIGN)
					 || (!bPreferOEM && bLoadASCIIasUTF8) // from menu "Reload As... UTF-8"
					 || IsUTF8(lpData, cbData)
					)
				   )
				&& !(FileVars_IsNonUTF8(&fvCurFile) && (iSrcEncoding != CPI_UTF8 && iSrcEncoding != CPI_UTF8SIGN))) {
			SendMessage(hwnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			if (utf8Sig) {
				EditSetNewText(hwnd, lpData + 3, cbData - 3);
				*iEncoding = CPI_UTF8SIGN;
				*iEOLMode = EditDetectEOLMode(hwnd, lpData + 3);
			} else {
				EditSetNewText(hwnd, lpData, cbData);
				*iEncoding = CPI_UTF8;
				*iEOLMode = EditDetectEOLMode(hwnd, lpData);
			}
		} else {
			UINT uCodePage = CP_UTF8;

			if (iSrcEncoding != -1) {
				*iEncoding = iSrcEncoding;
			} else {
				*iEncoding = FileVars_GetEncoding(&fvCurFile);
				if (*iEncoding == -1) {
					if (fvCurFile.mask & FV_ENCODING) {
						*iEncoding = CPI_DEFAULT;
					} else {
						if (iWeakSrcEncoding == -1) {
							*iEncoding = _iDefaultEncoding;
						} else if (mEncoding[iWeakSrcEncoding].uFlags & NCP_INTERNAL) {
							*iEncoding = iDefaultEncoding;
						} else {
							*iEncoding = _iDefaultEncoding;
						}
					}
				}
			}

			if ((mEncoding[*iEncoding].uFlags & NCP_8BIT && mEncoding[*iEncoding].uCodePage != CP_UTF7) ||
					(mEncoding[*iEncoding].uCodePage == CP_UTF7 && IsUTF7(lpData, cbData))) {
				LPWSTR lpDataWide;
				int cbDataWide;
				uCodePage	 = mEncoding[*iEncoding].uCodePage;
				lpDataWide = NP2HeapAlloc(cbData * sizeof(WCHAR) + 16);
				cbDataWide = MultiByteToWideChar(uCodePage, 0, lpData, cbData, lpDataWide,
												 (int)NP2HeapSize(lpDataWide) / sizeof(WCHAR));
				NP2HeapFree(lpData);
				lpData = NP2HeapAlloc(cbDataWide * kMaxMultiByteCount + 16);
				cbData = WideCharToMultiByte(CP_UTF8, 0, lpDataWide, cbDataWide, lpData,
											 (int)NP2HeapSize(lpData), NULL, NULL);
				NP2HeapFree(lpDataWide);

				SendMessage(hwnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
				EditSetNewText(hwnd, lpData, cbData);
				*iEOLMode = EditDetectEOLMode(hwnd, lpData);
			} else {
				SendMessage(hwnd, SCI_SETCODEPAGE, iDefaultCodePage, 0);
				EditSetNewText(hwnd, lpData, cbData);
				*iEncoding = CPI_DEFAULT;
				*iEOLMode = EditDetectEOLMode(hwnd, lpData);
			}
		}
	}

	NP2HeapFree(lpData);
	iSrcEncoding = -1;
	iWeakSrcEncoding = -1;

	return TRUE;
}

//=============================================================================
//
// EditSaveFile()
//
BOOL EditSaveFile(HWND hwnd, LPCWSTR pszFile, int iEncoding, BOOL *pbCancelDataLoss, BOOL bSaveCopy) {
	HANDLE hFile;
	BOOL	 bWriteSuccess;

	char *lpData;
	DWORD cbData;
	DWORD dwBytesWritten;

	*pbCancelDataLoss = FALSE;

	hFile = CreateFile(pszFile,
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
		SendMessage(hwnd, SCI_CONVERTEOLS, SendMessage(hwnd, SCI_GETEOLMODE, 0, 0), 0);
		EditFixPositions(hwnd);
	}

	// strip trailing blanks
	if (bAutoStripBlanks) {
		EditStripTrailingBlanks(hwnd, TRUE);
	}

	// get text
	cbData = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	lpData = NP2HeapAlloc(cbData + 1);
	SendMessage(hwnd, SCI_GETTEXT, NP2HeapSize(lpData), (LPARAM)lpData);

	if (cbData == 0) {
		bWriteSuccess = SetEndOfFile(hFile);
		dwLastIOError = GetLastError();
	} else {
		// FIXME: move checks in front of disk file access
		/*if ((mEncoding[iEncoding].uFlags & NCP_UNICODE) == 0 && (mEncoding[iEncoding].uFlags & NCP_UTF8_SIGN) == 0) {
				BOOL bEncodingMismatch = TRUE;
				FILEVARS fv;
				FileVars_Init(lpData, cbData, &fv);
				if (fv.mask & FV_ENCODING) {
					int iAltEncoding;
					if (FileVars_IsValidEncoding(&fv)) {
						iAltEncoding = FileVars_GetEncoding(&fv);
						if (iAltEncoding == iEncoding)
							bEncodingMismatch = FALSE;
						else if ((mEncoding[iAltEncoding].uFlags & NCP_UTF8) && (mEncoding[iEncoding].uFlags & NCP_UTF8))
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
		if (mEncoding[iEncoding].uFlags & NCP_UNICODE) {
			LPWSTR lpDataWide;
			int		 cbDataWide;

			SetEndOfFile(hFile);

			lpDataWide = NP2HeapAlloc(cbData * sizeof(WCHAR) + 16);
			cbDataWide = MultiByteToWideChar(CP_UTF8, 0, lpData, cbData, lpDataWide,
											 (int)NP2HeapSize(lpDataWide) / sizeof(WCHAR));

			if (mEncoding[iEncoding].uFlags & NCP_UNICODE_BOM) {
				if (mEncoding[iEncoding].uFlags & NCP_UNICODE_REVERSE) {
					WriteFile(hFile, (LPCVOID)"\xFE\xFF", 2, &dwBytesWritten, NULL);
				} else {
					WriteFile(hFile, (LPCVOID)"\xFF\xFE", 2, &dwBytesWritten, NULL);
				}
			}

			if (mEncoding[iEncoding].uFlags & NCP_UNICODE_REVERSE) {
				_swab((char *)lpDataWide, (char *)lpDataWide, cbDataWide * sizeof(WCHAR));
			}

			bWriteSuccess = WriteFile(hFile, lpDataWide, cbDataWide * sizeof(WCHAR), &dwBytesWritten, NULL);
			dwLastIOError = GetLastError();

			NP2HeapFree(lpDataWide);
		} else if (mEncoding[iEncoding].uFlags & NCP_UTF8) {
			SetEndOfFile(hFile);

			if (mEncoding[iEncoding].uFlags & NCP_UTF8_SIGN) {
				WriteFile(hFile, (LPCVOID)"\xEF\xBB\xBF", 3, &dwBytesWritten, NULL);
			}

			bWriteSuccess = WriteFile(hFile, lpData, cbData, &dwBytesWritten, NULL);
			dwLastIOError = GetLastError();
		} else if (mEncoding[iEncoding].uFlags & NCP_8BIT) {
			BOOL bCancelDataLoss = FALSE;
			UINT uCodePage = mEncoding[iEncoding].uCodePage;

			LPWSTR lpDataWide = NP2HeapAlloc(cbData * 2 + 16);
			int cbDataWide = MultiByteToWideChar(CP_UTF8, 0, lpData, cbData, lpDataWide,
												 (int)NP2HeapSize(lpDataWide) / sizeof(WCHAR));
			// Special cases: 42, 50220, 50221, 50222, 50225, 50227, 50229, 54936 GB18030, 57002-11, 65000, 65001
			if (uCodePage == CP_UTF7 || uCodePage == 54936) {
				NP2HeapFree(lpData);
				lpData = NP2HeapAlloc(NP2HeapSize(lpDataWide) * 2);
			} else {
				ZeroMemory(lpData, NP2HeapSize(lpData));
			}

			if (uCodePage == CP_UTF7 || uCodePage == 54936) {
				cbData = WideCharToMultiByte(uCodePage, 0, lpDataWide, cbDataWide, lpData,
											 (int)NP2HeapSize(lpData), NULL, NULL);
			} else {
				cbData = WideCharToMultiByte(uCodePage, WC_NO_BEST_FIT_CHARS, lpDataWide, cbDataWide, lpData,
											 (int)NP2HeapSize(lpData), NULL, &bCancelDataLoss);
				if (!bCancelDataLoss) {
					cbData = WideCharToMultiByte(uCodePage, 0, lpDataWide, cbDataWide, lpData,
												 (int)NP2HeapSize(lpData), NULL, NULL);
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
				*pbCancelDataLoss = TRUE;
			}
		} else {
			SetEndOfFile(hFile);
			bWriteSuccess = WriteFile(hFile, lpData, cbData, &dwBytesWritten, NULL);
			dwLastIOError = GetLastError();
		}
	}

	NP2HeapFree(lpData);
	CloseHandle(hFile);

	if (bWriteSuccess) {
		if (!bSaveCopy) {
			SendMessage(hwnd, SCI_SETSAVEPOINT, 0, 0);
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditInvertCase()
//
void EditInvertCase(HWND hwnd) {
	int iCurPos;
	int iAnchorPos;
	BOOL bChanged = FALSE;

	iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos != iAnchorPos) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			int cchTextW;
			UINT cpEdit;
			int i;
			int iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;
			char *pszText = NP2HeapAlloc(iSelCount + 1);
			LPWSTR pszTextW = NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszText);

			cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
			cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, iSelCount, pszTextW,
										   (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));

			for (i = 0; i < cchTextW; i++) {
				if (IsCharUpperW(pszTextW[i])) {
					pszTextW[i] = LOWORD(CharLowerW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
					bChanged = TRUE;
				} else if (IsCharLowerW(pszTextW[i])) {
					pszTextW[i] = LOWORD(CharUpperW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
					bChanged = TRUE;
				}
			}

			if (bChanged) {
				WideCharToMultiByte(cpEdit, 0, pszTextW, cchTextW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

				SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
				SendMessage(hwnd, SCI_CLEAR, 0, 0);
				SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)iSelCount, (LPARAM)pszText);
				SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
				SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
			}

			NP2HeapFree(pszText);
			NP2HeapFree(pszTextW);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

//=============================================================================
//
// EditTitleCase()
//
void EditTitleCase(HWND hwnd) {
	int iCurPos;
	int iAnchorPos;

	BOOL bNewWord = TRUE;
	//BOOL bWordEnd = TRUE;
	BOOL bChanged = FALSE;
#ifdef BOOKMARK_EDITION
	BOOL bPrevWasSpace = FALSE;
#endif

	iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos != iAnchorPos) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			int cchTextW;
			UINT cpEdit;
			int iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;
			char *pszText = NP2HeapAlloc(iSelCount + 1);
			LPWSTR pszTextW = NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszText);
			cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
			cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, iSelCount, pszTextW,
										   (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));

			if (IsWin7AndAbove()) {
				LPWSTR pszMappedW = NP2HeapAlloc(NP2HeapSize(pszTextW));
				if (LCMapString(LOCALE_SYSTEM_DEFAULT,
								LCMAP_LINGUISTIC_CASING |/*LCMAP_TITLECASE*/0x00000300,
								pszTextW, cchTextW, pszMappedW, (int)NP2HeapSize(pszMappedW) / sizeof(WCHAR))) {
					StrCpyN(pszTextW, pszMappedW, (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));
					bChanged = TRUE;
				} else {
					bChanged = FALSE;
				}

				NP2HeapFree(pszMappedW);
			} else {
#ifdef BOOKMARK_EDITION
				int i;
				//Slightly enhanced function to make Title Case:
				//Added some '-characters and bPrevWasSpace makes it better (for example "'Don't'" will now work)
				bPrevWasSpace = TRUE;
				for (i = 0; i < cchTextW; i++) {
					if (!IsCharAlphaNumericW(pszTextW[i]) && (!StrChr(L"\x0027\x0060\x0384\x2019", pszTextW[i]) ||	bPrevWasSpace)) {
						bNewWord = TRUE;
					} else {
						if (bNewWord) {
							if (IsCharLowerW(pszTextW[i])) {
								pszTextW[i] = LOWORD(CharUpperW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
								bChanged = TRUE;
							}
						} else {
							if (IsCharUpperW(pszTextW[i])) {
								pszTextW[i] = LOWORD(CharLowerW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
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
				for (i = 0; i < cchTextW; i++) {
					BOOL bAlphaNumeric = IsCharAlphaNumericW(pszTextW[i]);
					if (!bAlphaNumeric && (!StrChr(L"\x0027\x2019\x0060\x00B4", pszTextW[i]) || bWordEnd)) {
						bNewWord = TRUE;
					} else {
						if (bNewWord) {
							if (IsCharLowerW(pszTextW[i])) {
								pszTextW[i] = LOWORD(CharUpperW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
								bChanged = TRUE;
							}
						} else {
							if (IsCharUpperW(pszTextW[i])) {
								pszTextW[i] = LOWORD(CharLowerW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
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

				SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
				SendMessage(hwnd, SCI_CLEAR, 0, 0);
				SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)iSelCount, (LPARAM)pszText);
				SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
				SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
			}

			NP2HeapFree(pszText);
			NP2HeapFree(pszTextW);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

//=============================================================================
//
// EditSentenceCase()
//
void EditSentenceCase(HWND hwnd) {
	int iCurPos;
	int iAnchorPos;
	BOOL bNewSentence = TRUE;
	BOOL bChanged = FALSE;

	iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos != iAnchorPos) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			int cchTextW;
			UINT cpEdit;
			int i;
			int iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;
			char *pszText = NP2HeapAlloc(iSelCount + 1);
			LPWSTR pszTextW = NP2HeapAlloc((iSelCount + 1) * sizeof(WCHAR));

			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszText);

			cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
			cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, iSelCount, pszTextW,
										   (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));
			for (i = 0; i < cchTextW; i++) {
				if (StrChr(L".;!?\r\n", pszTextW[i])) {
					bNewSentence = TRUE;
				} else {
					if (IsCharAlphaNumericW(pszTextW[i])) {
						if (bNewSentence) {
							if (IsCharLowerW(pszTextW[i])) {
								pszTextW[i] = LOWORD(CharUpperW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
								bChanged = TRUE;
							}
							bNewSentence = FALSE;
						} else {
							if (IsCharUpperW(pszTextW[i])) {
								pszTextW[i] = LOWORD(CharLowerW((LPWSTR)(LONG_PTR)MAKELONG(pszTextW[i], 0)));
								bChanged = TRUE;
							}
						}
					}
				}
			}

			if (bChanged) {
				WideCharToMultiByte(cpEdit, 0, pszTextW, cchTextW, pszText, (int)NP2HeapSize(pszText), NULL, NULL);

				SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
				SendMessage(hwnd, SCI_CLEAR, 0, 0);
				SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)iSelCount, (LPARAM)pszText);
				SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
				SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
			}

			NP2HeapFree(pszText);
			NP2HeapFree(pszTextW);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

//=============================================================================
//
// EditURLEncode()
//
void EditURLEncode(HWND hwnd) {
	int iCurPos;
	int iAnchorPos;

	iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos != iAnchorPos) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			const int iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0);

			char *pszText = NP2HeapAlloc(iSelCount);
			LPWSTR pszTextW = NP2HeapAlloc(iSelCount * sizeof(WCHAR));

			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszText);
			const UINT cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
			MultiByteToWideChar(cpEdit, 0, pszText, iSelCount, pszTextW,
								(int)NP2HeapSize(pszTextW) / sizeof(WCHAR));

			// https://msdn.microsoft.com/en-us/library/windows/desktop/bb773774(v=vs.85).aspx
			char *pszEscaped = NP2HeapAlloc(NP2HeapSize(pszText) * 3); // '&', H1, H0
			LPWSTR pszEscapedW = NP2HeapAlloc(NP2HeapSize(pszTextW) * 3);

			DWORD cchEscapedW = (int)NP2HeapSize(pszEscapedW) / sizeof(WCHAR);
			UrlEscape(pszTextW, pszEscapedW, &cchEscapedW, URL_ESCAPE_SEGMENT_ONLY);
			const int cchEscaped = WideCharToMultiByte(cpEdit, 0, pszEscapedW, cchEscapedW, pszEscaped,
											 (int)NP2HeapSize(pszEscaped), NULL, NULL);
			if (iCurPos < iAnchorPos) {
				iAnchorPos = iCurPos + cchEscaped;
			} else {
				iCurPos = iAnchorPos + cchEscaped;
			}

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
			SendMessage(hwnd, SCI_CLEAR, 0, 0);
			SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)cchEscaped, (LPARAM)pszEscaped);
			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

			NP2HeapFree(pszText);
			NP2HeapFree(pszTextW);
			NP2HeapFree(pszEscaped);
			NP2HeapFree(pszEscapedW);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

//=============================================================================
//
// EditURLDecode()
//
void EditURLDecode(HWND hwnd) {
	int iCurPos;
	int iAnchorPos;

	iCurPos	= (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos != iAnchorPos) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			const int iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0);

			char *pszText = NP2HeapAlloc(iSelCount);
			LPWSTR pszTextW = NP2HeapAlloc(iSelCount * sizeof(WCHAR));

			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszText);
			const UINT cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
			MultiByteToWideChar(cpEdit, 0, pszText, iSelCount, pszTextW,
								(int)NP2HeapSize(pszTextW) / sizeof(WCHAR));

			char *pszUnescaped = NP2HeapAlloc(NP2HeapSize(pszText) * 3);
			LPWSTR pszUnescapedW = NP2HeapAlloc(NP2HeapSize(pszTextW) * 3);

			DWORD cchUnescapedW = (int)NP2HeapSize(pszUnescapedW) / sizeof(WCHAR);
			UrlUnescape(pszTextW, pszUnescapedW, &cchUnescapedW, 0);
			DWORD cchUnescaped = WideCharToMultiByte(cpEdit, 0, pszUnescapedW, cchUnescapedW, pszUnescaped,
											   (int)NP2HeapSize(pszUnescaped), NULL, NULL);
			if (iCurPos < iAnchorPos) {
				iAnchorPos = iCurPos + cchUnescaped;
			} else {
				iCurPos = iAnchorPos + cchUnescaped;
			}

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
			SendMessage(hwnd, SCI_CLEAR, 0, 0);
			SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)cchUnescaped, (LPARAM)pszUnescaped);
			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

			NP2HeapFree(pszText);
			NP2HeapFree(pszTextW);
			NP2HeapFree(pszUnescaped);
			NP2HeapFree(pszUnescapedW);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

//=============================================================================
//
// EditEscapeCChars()
//
void EditEscapeCChars(HWND hwnd) {
	if (!EditIsEmptySelection()) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			EDITFINDREPLACE efr = {
				.hwnd = hwnd,
			};

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

			lstrcpyA(efr.szFind, "\\");
			lstrcpyA(efr.szReplace, "\\\\");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "\"");
			lstrcpyA(efr.szReplace, "\\\"");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "\'");
			lstrcpyA(efr.szReplace, "\\\'");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

//=============================================================================
//
// EditUnescapeCChars()
//
void EditUnescapeCChars(HWND hwnd) {
	if (!EditIsEmptySelection()) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			EDITFINDREPLACE efr = {
				.hwnd = hwnd,
			};

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

			lstrcpyA(efr.szFind, "\\\\");
			lstrcpyA(efr.szReplace, "\\");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "\\\"");
			lstrcpyA(efr.szReplace, "\"");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "\\\'");
			lstrcpyA(efr.szReplace, "\'");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

// XML/HTML predefined entity
// http://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
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
	if (!EditIsEmptySelection()) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			EDITFINDREPLACE efr = {
				.hwnd = hwnd,
			};

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

			lstrcpyA(efr.szFind, "&");
			lstrcpyA(efr.szReplace, "&amp;");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "\"");
			lstrcpyA(efr.szReplace, "&quot;");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "\'");
			lstrcpyA(efr.szReplace, "&apos;");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "<");
			lstrcpyA(efr.szReplace, "&lt;");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, ">");
			lstrcpyA(efr.szReplace, "&gt;");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			if (pLexCurrent->iLexer != SCLEX_XML) {
				lstrcpyA(efr.szFind, " ");
				lstrcpyA(efr.szReplace, "&nbsp;");
				EditReplaceAllInSelection(hwnd, &efr, FALSE);

				lstrcpyA(efr.szFind, "\t");
				lstrcpyA(efr.szReplace, "&emsp;");
				EditReplaceAllInSelection(hwnd, &efr, FALSE);
			}

			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
}

//=============================================================================
//
// EditUnescapeXHTMLChars()
//
void EditUnescapeXHTMLChars(HWND hwnd) {
	if (!EditIsEmptySelection()) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			EDITFINDREPLACE efr = {
				.hwnd = hwnd,
			};

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

			lstrcpyA(efr.szFind, "&quot;");
			lstrcpyA(efr.szReplace, "\"");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "&apos;");
			lstrcpyA(efr.szReplace, "\'");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "&lt;");
			lstrcpyA(efr.szReplace, "<");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "&gt;");
			lstrcpyA(efr.szReplace, ">");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "&nbsp;");
			lstrcpyA(efr.szReplace, " ");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "&amp;");
			lstrcpyA(efr.szReplace, "&");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);

			lstrcpyA(efr.szFind, "&emsp;");
			lstrcpyA(efr.szReplace, "\t");
			EditReplaceAllInSelection(hwnd, &efr, FALSE);
			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		} else {
			MsgBox(MBWARN, IDS_SELRECT);
		}
	}
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

void EditChar2Hex(HWND hwnd) {
	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		int count = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;

		if (count) {
			count *= 2 + MAX_ESCAPE_HEX_DIGIT;
			char *ch = NP2HeapAlloc(count + 1);
			WCHAR *wch = (WCHAR *)NP2HeapAlloc((count + 1) * sizeof(WCHAR));
			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)ch);

			if (ch[0] == 0) {
				lstrcpyA(ch, "\\x00");
			} else {
				const UINT cp = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
				int i = 0, j = 0;
				char uesc = 'u';
				if (pLexCurrent->rid == NP2LEX_CSHARP) {
					uesc = 'x';
				}
				count = MultiByteToWideChar(cp, 0, ch, -1, wch, count + 1) - 1; // '\0'
				for (i = 0; i < count; i++) {
					if (wch[i] <= 0xFF) {
						wsprintfA(ch + j, "\\x%02X", wch[i] & 0xFF); // \xhh
						j += 4;
					} else {
						wsprintfA(ch + j, "\\%c%04X", uesc, wch[i]); // \uhhhh \xhhhh
						j += 6;
					}
				}
			}

			SendMessage(hwnd, SCI_REPLACESEL, 0, (LPARAM)ch);
			SendMessage(hwnd, SCI_SETSEL, iSelStart, iSelStart + lstrlenA(ch));
			NP2HeapFree(ch);
			NP2HeapFree(wch);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditHex2Char()
//
void EditHex2Char(HWND hwnd) {
	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
		int count = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;

		if (count) {
			count *= 2 + MAX_ESCAPE_HEX_DIGIT;
			char *ch = NP2HeapAlloc(count + 1);
			WCHAR *wch = (WCHAR *)NP2HeapAlloc((count + 1) * sizeof(WCHAR));
			UINT cp = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
			int ci = 0, cch = 0, ucc = 0;
			char *p = ch;

			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)ch);

			while (*p) {
				if (*p == '\\') {
					p++;
					if (*p == 'x' || *p == 'u') {
						p++;
						ci = 0;
						ucc = 0;
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
			cch = WideCharToMultiByte(cp, 0, wch, -1, ch, count + 1, NULL, NULL) - 1; // '\0'

			SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelStart, 0);
			SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelEnd, 0);
			SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)cch, (LPARAM)ch);
			SendMessage(hwnd, SCI_SETSEL, iSelStart, iSelStart + cch);
			NP2HeapFree(ch);
			NP2HeapFree(wch);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

void EditShowHex(HWND hwnd) {
	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iSelEnd = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
		int count = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;

		if (count) {
			char *ch = NP2HeapAlloc(count + 1);
			char *cch = NP2HeapAlloc(count * 3 + 3);
			char *p = ch, *t = cch;
			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)ch);
			*t++ = '[';
			while (*p) {
				int c = *p++, v;
				if (c < 0) {
					c += 256;
				}
				v = c >> 4;
				*t++ = (char)((v >= 10) ? v - 10 + 'a' : v + '0');
				v = c & 0x0f;
				*t++ = (char)((v >= 10) ? v - 10 + 'a' : v + '0');
				*t++ = ' ';
			}
			*--t = ']';
			SendMessage(hwnd, SCI_INSERTTEXT, (WPARAM)iSelEnd, (LPARAM)cch);
			SendMessage(hwnd, SCI_SETSEL, iSelEnd, iSelEnd + lstrlenA(cch));
			NP2HeapFree(ch);
			NP2HeapFree(cch);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
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

static int ConvertNumRadix(char *tch, ULONG64 num, int radix) {
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
			int bit = num & 7;
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
		lstrcatA(tch, buf + index);
		return length;
	}
	break;

	case 2: {
		char buf[2 + 64 + 8 + 1] = "";
		int index = 2 + 64 + 8;
		int length = 0, bit_count = 0;
		while (num) {
			int bit = num & 1;
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
		lstrcatA(tch, buf + index);
		return length;
	}
	break;

	}
	return 0;
}

void EditConvertNumRadix(HWND hwnd, int radix) {
	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
		int count = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;

		if (count) {
			char *ch = NP2HeapAlloc(count + 1);
			char *tch = NP2HeapAlloc(2 + count * 4 + 8 + 1);
			int cch = 0;
			char *p = ch;
			ULONG64 ci = 0;

			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)ch);

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
			tch[cch] = 0;

			SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelStart, 0);
			SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelEnd, 0);
			SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)cch, (LPARAM)tch);
			SendMessage(hwnd, SCI_SETSEL, iSelStart, iSelStart + cch);
			NP2HeapFree(ch);
			NP2HeapFree(tch);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditModifyNumber()
//
void EditModifyNumber(HWND hwnd, BOOL bIncrease) {
	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

		if (iSelEnd - iSelStart) {
			if (SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) < 32) {
				char chNumber[32] = "";
				char chFormat[32] = "";
				int	 iNumber;
				int	 iWidth;
				SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)chNumber);

				if (StrChrA(chNumber, '-')) {
					return;
				}

				if (!StrChrIA(chNumber, 'x') && StrToIntExA(chNumber, STIF_DEFAULT, &iNumber)) {
					iWidth = lstrlenA(chNumber);
					if (iNumber >= 0) {
						if (bIncrease && iNumber < INT_MAX) {
							iNumber++;
						}
						if (!bIncrease && iNumber > 0) {
							iNumber--;
						}
						wsprintfA(chFormat, "%%0%ii", iWidth);
						wsprintfA(chNumber, chFormat, iNumber);
						SendMessage(hwnd, SCI_REPLACESEL, 0, (LPARAM)chNumber);
						SendMessage(hwnd, SCI_SETSEL, iSelStart, iSelStart + lstrlenA(chNumber));
					}
				} else if (StrToIntExA(chNumber, STIF_SUPPORT_HEX, &iNumber)) {
					BOOL bUppercase = FALSE;
					iWidth = lstrlenA(chNumber) - 2;
					if (iNumber >= 0) {
						if (bIncrease && iNumber < INT_MAX) {
							iNumber++;
						}
						if (!bIncrease && iNumber > 0) {
							iNumber--;
						}
						const int len = lstrlenA(chNumber) - 1;
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
							wsprintfA(chFormat, "%%#0%iX", iWidth);
						} else {
							wsprintfA(chFormat, "%%#0%ix", iWidth);
						}
						wsprintfA(chNumber, chFormat, iNumber);
						SendMessage(hwnd, SCI_REPLACESEL, 0, (LPARAM)chNumber);
						SendMessage(hwnd, SCI_SETSEL, iSelStart, iSelStart + lstrlenA(chNumber));
					}
				}
			}
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
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
void EditTabsToSpaces(HWND hwnd, int nTabWidth, BOOL bOnlyIndentingWS) {
	char *pszText;
	LPWSTR pszTextW;
	int cchTextW;
	int iTextW;
	LPWSTR pszConvW;
	int cchConvW;
	int i, j;
	int iLine;
	int iCurPos;
	int iAnchorPos;
	int iSelStart;
	int iSelEnd;
	int iSelCount;
	UINT cpEdit;
	struct Sci_TextRange tr;
	BOOL bIsLineStart = TRUE;
	BOOL bModified = FALSE;

	if (SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		MsgBox(MBWARN, IDS_SELRECT);
		return;
	}

	iCurPos		= (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos	= (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos == iAnchorPos) {
		//iSelStart = 0;
		//iSelEnd		= SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
		return;
	} else {
		iSelStart	= (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	}

	iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
	iSelStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);

	iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0);

	pszText = NP2HeapAlloc(iSelCount);
	pszTextW = NP2HeapAlloc(iSelCount * sizeof(WCHAR));

	tr.chrg.cpMin = iSelStart;
	tr.chrg.cpMax = iSelEnd;
	tr.lpstrText = pszText;
	SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

	cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
	cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, iSelCount - 1, pszTextW,
								   (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));
	NP2HeapFree(pszText);

	pszConvW = NP2HeapAlloc(cchTextW * sizeof(WCHAR) * nTabWidth + 2);
	cchConvW = 0;

	// Contributed by Homam
	// Thank you very much!
	i = 0;
	for (iTextW = 0; iTextW < cchTextW; iTextW++) {
		WCHAR w = pszTextW[iTextW];
		if (w == L'\t' && (!bOnlyIndentingWS || bIsLineStart)) {
			for (j = 0; j < nTabWidth - i % nTabWidth; j++) {
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
		int cchConvM;
		pszText = NP2HeapAlloc(cchConvW * kMaxMultiByteCount);

		cchConvM = WideCharToMultiByte(cpEdit, 0, pszConvW, cchConvW, pszText,
									   (int)NP2HeapSize(pszText), NULL, NULL);
		NP2HeapFree(pszConvW);

		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + cchConvM;
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + cchConvM;
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
		SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelStart, 0);
		SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelEnd, 0);
		SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)cchConvM, (LPARAM)pszText);
		//SendMessage(hwnd, SCI_CLEAR, 0, 0);
		//SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)cchConvW, (LPARAM)pszText);
		SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

		NP2HeapFree(pszText);
	} else {
		NP2HeapFree(pszConvW);
	}
}

//=============================================================================
//
// EditSpacesToTabs()
//
void EditSpacesToTabs(HWND hwnd, int nTabWidth, BOOL bOnlyIndentingWS) {
	char *pszText;
	LPWSTR pszTextW;
	int cchTextW;
	int iTextW;
	LPWSTR pszConvW;
	int cchConvW;
	int i, j, t;
	int iLine;
	int iCurPos;
	int iAnchorPos;
	int iSelStart;
	int iSelEnd;
	int iSelCount;
	UINT cpEdit;
	struct Sci_TextRange tr;
	WCHAR space[256];
	BOOL bIsLineStart = TRUE;
	BOOL bModified = FALSE;

	if (SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		MsgBox(MBWARN, IDS_SELRECT);
		return;
	}

	iCurPos		= (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos	= (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos == iAnchorPos) {
		//iSelStart = 0;
		//iSelEnd		= SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
		return;
	} else {
		iSelStart	= (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	}

	iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
	iSelStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);

	iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0);

	pszText = NP2HeapAlloc(iSelCount);
	pszTextW = NP2HeapAlloc(iSelCount * sizeof(WCHAR));

	tr.chrg.cpMin = iSelStart;
	tr.chrg.cpMax = iSelEnd;
	tr.lpstrText = pszText;
	SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

	cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);

	cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, iSelCount - 1, pszTextW,
								   (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));
	NP2HeapFree(pszText);

	pszConvW = NP2HeapAlloc(cchTextW * sizeof(WCHAR) + 2);
	cchConvW = 0;

	// Contributed by Homam
	// Thank you very much!
	i = j = 0;
	for (iTextW = 0; iTextW < cchTextW; iTextW++) {
		WCHAR w = pszTextW[iTextW];
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
				for (t = 0; t < j; t++) {
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
		for (t = 0; t < j; t++) {
			pszConvW[cchConvW++] = space[t];
		}
	}

	NP2HeapFree(pszTextW);

	if (bModified || cchConvW != cchTextW) {
		int cchConvM;
		pszText = NP2HeapAlloc(cchConvW * kMaxMultiByteCount + 1);
		cchConvM = WideCharToMultiByte(cpEdit, 0, pszConvW, cchConvW, pszText,
									   (int)NP2HeapSize(pszText), NULL, NULL);
		NP2HeapFree(pszConvW);

		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + cchConvM;
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + cchConvM;
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
		SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelStart, 0);
		SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelEnd, 0);
		SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)cchConvM, (LPARAM)pszText);
		//SendMessage(hwnd, SCI_CLEAR, 0, 0);
		//SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)cchConvW, (LPARAM)pszText);
		SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

		NP2HeapFree(pszText);
	} else {
		NP2HeapFree(pszConvW);
	}
}

//=============================================================================
//
// EditMoveUp()
//
void EditMoveUp(HWND hwnd) {
	int iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
	const int iCurLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurPos, 0);
	const int iAnchorLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iAnchorPos, 0);

	if (iCurLine == iAnchorLine) {
		int iLineCurPos = iCurPos - (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iCurLine, 0);
		int iLineAnchorPos = iAnchorPos - (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iAnchorLine, 0);
		//int iLineCur = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iCurLine, 0);
		//int iLineAnchor = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iAnchorLine, 0);
		//if (iLineCur == iLineAnchor) {
		//}

		if (iCurLine > 0) {
			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
			SendMessage(hwnd, SCI_LINETRANSPOSE, 0, 0);
			SendMessage(hwnd, SCI_SETSEL,
						(WPARAM)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iAnchorLine - 1, 0) + iLineAnchorPos,
						(LPARAM)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iCurLine - 1, 0) + iLineCurPos);
			SendMessage(hwnd, SCI_CHOOSECARETX, 0, 0);
			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		}
	} else if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iLineSrc = min_i(iCurLine, iAnchorLine) - 1;
		//int iLineCur = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iCurLine, 0);
		//int iLineAnchor = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iAnchorLine, 0);
		//if (iLineCur == iLineAnchor) {
		//}

		if (iLineSrc >= 0) {
			DWORD cLine;
			char *pLine;
			int iLineSrcStart;
			int iLineSrcEnd;
			int iLineDest;
			int iLineDestStart;

			cLine = (int)SendMessage(hwnd, SCI_GETLINE, (WPARAM)iLineSrc, 0);
			pLine = NP2HeapAlloc(cLine + 1);
			SendMessage(hwnd, SCI_GETLINE, (WPARAM)iLineSrc, (LPARAM)pLine);

			iLineSrcStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc, 0);
			iLineSrcEnd = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc + 1, 0);

			iLineDest = max_i(iCurLine, iAnchorLine);
			if (max_i(iCurPos, iAnchorPos) <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineDest, 0)) {
				if (iLineDest >= 1) {
					iLineDest--;
				}
			}

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

			SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iLineSrcStart, 0);
			SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iLineSrcEnd, 0);
			SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");

			iLineDestStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineDest, 0);

			SendMessage(hwnd, SCI_INSERTTEXT, (WPARAM)iLineDestStart, (LPARAM)pLine);

			NP2HeapFree(pLine);

			if (iLineDest == SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 1) {
				char chaEOL[] = "\r\n";
				int iEOLMode = (int)SendMessage(hwnd, SCI_GETEOLMODE, 0, 0);
				if (iEOLMode == SC_EOL_CR) {
					chaEOL[1] = 0;
				} else if (iEOLMode == SC_EOL_LF) {
					chaEOL[0] = '\n';
					chaEOL[1] = 0;
				}

				SendMessage(hwnd, SCI_INSERTTEXT, (WPARAM)iLineDestStart, (LPARAM)chaEOL);
				SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)
							SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineDest, 0), 0);
				SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)
							SendMessage(hwnd, SCI_GETLENGTH, 0, 0), 0);
				SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
			}

			if (iCurPos < iAnchorPos) {
				iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iCurLine - 1, 0);
				iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineDest, 0);
			} else {
				iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iAnchorLine - 1, 0);
				iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineDest, 0);
			}

			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);

			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditMoveDown()
//
void EditMoveDown(HWND hwnd) {
	int iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
	const int iCurLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurPos, 0);
	const int iAnchorLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iAnchorPos, 0);

	if (iCurLine == iAnchorLine) {
		int iLineCurPos = iCurPos - (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iCurLine, 0);
		int iLineAnchorPos = iAnchorPos - (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iAnchorLine, 0);
		//int iLineCur = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iCurLine, 0);
		//int iLineAnchor = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iAnchorLine, 0);
		//if (iLineCur == iLineAnchor) {
		//}

		if (iCurLine < SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 1) {
			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
			SendMessage(hwnd, SCI_GOTOLINE, (WPARAM)iCurLine + 1, 0);
			SendMessage(hwnd, SCI_LINETRANSPOSE, 0, 0);
			SendMessage(hwnd, SCI_SETSEL,
						(WPARAM)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iAnchorLine + 1, 0) + iLineAnchorPos,
						(LPARAM)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iCurLine + 1, 0) + iLineCurPos);
			SendMessage(hwnd, SCI_CHOOSECARETX, 0, 0);
			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		}
	} else if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iLineSrc = max_i(iCurLine, iAnchorLine) + 1;
		//int iLineCur = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iCurLine, 0);
		//int iLineAnchor = (int)SendMessage(hwnd, SCI_DOCLINEFROMVISIBLE, (WPARAM)iAnchorLine, 0);
		//if (iLineCur == iLineAnchor) {
		//}

		if (max_i(iCurPos, iAnchorPos) <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc - 1, 0)) {
			if (iLineSrc >= 1) {
				iLineSrc--;
			}
		}

		if (iLineSrc <= SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 1) {
			DWORD cLine;
			char *pLine;
			int iLineSrcStart;
			int iLineSrcEnd;
			int iLineDest;
			int iLineDestStart;

			BOOL bLastLine = (iLineSrc == SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 1);

			if (bLastLine &&
					(SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineSrc, 0) -
					 SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc, 0) == 0) &&
					(SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineSrc - 1, 0) -
					 SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc - 1, 0) == 0)) {
				return;
			}

			if (bLastLine) {
				char chaEOL[] = "\r\n";
				int iEOLMode = (int)SendMessage(hwnd, SCI_GETEOLMODE, 0, 0);
				if (iEOLMode == SC_EOL_CR) {
					chaEOL[1] = 0;
				} else if (iEOLMode == SC_EOL_LF) {
					chaEOL[0] = '\n';
					chaEOL[1] = 0;
				}
				SendMessage(hwnd, SCI_APPENDTEXT, (WPARAM)lstrlenA(chaEOL), (LPARAM)chaEOL);
			}

			cLine = (int)SendMessage(hwnd, SCI_GETLINE, (WPARAM)iLineSrc, 0);
			pLine = NP2HeapAlloc(cLine + 3);
			SendMessage(hwnd, SCI_GETLINE, (WPARAM)iLineSrc, (LPARAM)pLine);

			iLineSrcStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc, 0);
			iLineSrcEnd = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc + 1, 0);

			iLineDest = min_i(iCurLine, iAnchorLine);

			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

			SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iLineSrcStart, 0);
			SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iLineSrcEnd, 0);
			SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");

			iLineDestStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineDest, 0);

			SendMessage(hwnd, SCI_INSERTTEXT, (WPARAM)iLineDestStart, (LPARAM)pLine);

			if (bLastLine) {
				SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)
							SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)
										(SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 2), 0), 0);
				SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)
							SendMessage(hwnd, SCI_GETLENGTH, 0, 0), 0);
				SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
			}

			NP2HeapFree(pLine);

			if (iCurPos < iAnchorPos) {
				iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iCurLine + 1, 0);
				iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc + 1, 0);
			} else {
				iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iAnchorLine + 1, 0);
				iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineSrc + 1, 0);
			}

			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);

			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditModifyLines()
//
void EditModifyLines(HWND hwnd, LPCWSTR pwszPrefix, LPCWSTR pwszAppend) {
	char	mszPrefix1[256] = "";
	char	mszAppend1[256] = "";
	int		mbcp;

	if (SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) {
		mbcp = CP_UTF8;
	} else {
		mbcp = CP_ACP;
	}

	if (StrNotEmpty(pwszPrefix)) {
		WideCharToMultiByte(mbcp, 0, pwszPrefix, -1, mszPrefix1, COUNTOF(mszPrefix1), NULL, NULL);
	}
	if (StrNotEmpty(pwszAppend)) {
		WideCharToMultiByte(mbcp, 0, pwszAppend, -1, mszAppend1, COUNTOF(mszAppend1), NULL, NULL);
	}

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		char mszPrefix2[256] = "";
		int iPrefixNum = 0;
		int iPrefixNumWidth = 1;
		char *pszPrefixNumPad = "";
		char mszAppend2[256] = "";
		BOOL bAppendNum = FALSE;
		int iAppendNum = 0;
		BOOL bPrefixNum = FALSE;
		char *pszAppendNumPad = "";
		int iAppendNumWidth = 1;

		char *p;
		int	 i;
		int iLine;

		int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		int iSelEnd = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

		int iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		int iLineEnd = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		//if (iSelStart > SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0))
		//	iLineStart++;

		if (iSelEnd <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd, 0)) {
			if (iLineEnd - iLineStart >= 1) {
				iLineEnd--;
			}
		}

		if (StrNotEmptyA(mszPrefix1)) {
			p = mszPrefix1;
			while (!bPrefixNum && (p = StrStrA(p, "$(")) != NULL) {
				if (strncmp(p, "$(I)", CSTRLEN("$(I)")) == 0) {
					*p = 0;
					StrCpyA(mszPrefix2, p + CSTRLEN("$(I)"));
					bPrefixNum = TRUE;
					iPrefixNum = 0;
					for (i = iLineEnd - iLineStart; i >= 10; i = i / 10) {
						iPrefixNumWidth++;
					}
					pszPrefixNumPad = "";
				} else if (strncmp(p, "$(0I)", CSTRLEN("$(0I)")) == 0) {
					*p = 0;
					StrCpyA(mszPrefix2, p + CSTRLEN("$(0I)"));
					bPrefixNum = TRUE;
					iPrefixNum = 0;
					for (i = iLineEnd - iLineStart; i >= 10; i = i / 10) {
						iPrefixNumWidth++;
					}
					pszPrefixNumPad = "0";
				} else if (strncmp(p, "$(N)", CSTRLEN("$(N)")) == 0) {
					*p = 0;
					StrCpyA(mszPrefix2, p + CSTRLEN("$(N)"));
					bPrefixNum = TRUE;
					iPrefixNum = 1;
					for (i = iLineEnd - iLineStart + 1; i >= 10; i = i / 10) {
						iPrefixNumWidth++;
					}
					pszPrefixNumPad = "";
				} else if (strncmp(p, "$(0N)", CSTRLEN("$(0N)")) == 0) {
					*p = 0;
					StrCpyA(mszPrefix2, p + CSTRLEN("$(0N)"));
					bPrefixNum = TRUE;
					iPrefixNum = 1;
					for (i = iLineEnd - iLineStart + 1; i >= 10; i = i / 10) {
						iPrefixNumWidth++;
					}
					pszPrefixNumPad = "0";
				} else if (strncmp(p, "$(L)", CSTRLEN("$(L)")) == 0) {
					*p = 0;
					StrCpyA(mszPrefix2, p + CSTRLEN("$(L)"));
					bPrefixNum = TRUE;
					iPrefixNum = iLineStart + 1;
					for (i = iLineEnd + 1; i >= 10; i = i / 10) {
						iPrefixNumWidth++;
					}
					pszPrefixNumPad = "";
				} else if (strncmp(p, "$(0L)", CSTRLEN("$(0L)")) == 0) {
					*p = 0;
					StrCpyA(mszPrefix2, p + CSTRLEN("$(0L)"));
					bPrefixNum = TRUE;
					iPrefixNum = iLineStart + 1;
					for (i = iLineEnd + 1; i >= 10; i = i / 10) {
						iPrefixNumWidth++;
					}
					pszPrefixNumPad = "0";
				}
				p += CSTRLEN("$(");
			}
		}

		if (StrNotEmptyA(mszAppend1)) {
			p = mszAppend1;
			while (!bAppendNum && (p = StrStrA(p, "$(")) != NULL) {
				if (strncmp(p, "$(I)", CSTRLEN("$(I)")) == 0) {
					*p = 0;
					StrCpyA(mszAppend2, p + CSTRLEN("$(I)"));
					bAppendNum = TRUE;
					iAppendNum = 0;
					for (i = iLineEnd - iLineStart; i >= 10; i = i / 10) {
						iAppendNumWidth++;
					}
					pszAppendNumPad = "";
				} else if (strncmp(p, "$(0I)", CSTRLEN("$(0I)")) == 0) {
					*p = 0;
					StrCpyA(mszAppend2, p + CSTRLEN("$(0I)"));
					bAppendNum = TRUE;
					iAppendNum = 0;
					for (i = iLineEnd - iLineStart; i >= 10; i = i / 10) {
						iAppendNumWidth++;
					}
					pszAppendNumPad = "0";
				} else if (strncmp(p, "$(N)", CSTRLEN("$(N)")) == 0) {
					*p = 0;
					StrCpyA(mszAppend2, p + CSTRLEN("$(N)"));
					bAppendNum = TRUE;
					iAppendNum = 1;
					for (i = iLineEnd - iLineStart + 1; i >= 10; i = i / 10) {
						iAppendNumWidth++;
					}
					pszAppendNumPad = "";
				} else if (strncmp(p, "$(0N)", CSTRLEN("$(0N)")) == 0) {
					*p = 0;
					StrCpyA(mszAppend2, p + CSTRLEN("$(0N)"));
					bAppendNum = TRUE;
					iAppendNum = 1;
					for (i = iLineEnd - iLineStart + 1; i >= 10; i = i / 10) {
						iAppendNumWidth++;
					}
					pszAppendNumPad = "0";
				} else if (strncmp(p, "$(L)", CSTRLEN("$(L)")) == 0) {
					*p = 0;
					StrCpyA(mszAppend2, p + CSTRLEN("$(L)"));
					bAppendNum = TRUE;
					iAppendNum = iLineStart + 1;
					for (i = iLineEnd + 1; i >= 10; i = i / 10) {
						iAppendNumWidth++;
					}
					pszAppendNumPad = "";
				} else if (strncmp(p, "$(0L)", CSTRLEN("$(0L)")) == 0) {
					*p = 0;
					StrCpyA(mszAppend2, p + CSTRLEN("$(0L)"));
					bAppendNum = TRUE;
					iAppendNum = iLineStart + 1;
					for (i = iLineEnd + 1; i >= 10; i = i / 10) {
						iAppendNumWidth++;
					}
					pszAppendNumPad = "0";
				}
				p += CSTRLEN("$(");
			}
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iPos;

			if (StrNotEmpty(pwszPrefix)) {
				char mszInsert[512];
				lstrcpyA(mszInsert, mszPrefix1);

				if (bPrefixNum) {
					char tchFmt[64];
					char tchNum[64];
					wsprintfA(tchFmt, "%%%s%ii", pszPrefixNumPad, iPrefixNumWidth);
					wsprintfA(tchNum, tchFmt, iPrefixNum);
					lstrcatA(mszInsert, tchNum);
					lstrcatA(mszInsert, mszPrefix2);
					iPrefixNum++;
				}

				iPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
				SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPos, 0);
				SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iPos, 0);
				SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(mszInsert), (LPARAM)mszInsert);
			}

			if (StrNotEmpty(pwszAppend)) {
				char mszInsert[512];
				lstrcpyA(mszInsert, mszAppend1);

				if (bAppendNum) {
					char tchFmt[64];
					char tchNum[64];
					wsprintfA(tchFmt, "%%%s%ii", pszAppendNumPad, iAppendNumWidth);
					wsprintfA(tchNum, tchFmt, iAppendNum);
					lstrcatA(mszInsert, tchNum);
					lstrcatA(mszInsert, mszAppend2);
					iAppendNum++;
				}

				iPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
				SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPos, 0);
				SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iPos, 0);
				SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(mszInsert), (LPARAM)mszInsert);
			}
		}
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

		//// Fix selection
		//if (iSelStart != iSelEnd && SendMessage(hwnd, SCI_GETTARGETEND, 0, 0) > SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0))
		//{
		//	int iCurPos = SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
		//	int iAnchorPos = SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
		//	if (iCurPos > iAnchorPos)
		//		iCurPos = SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
		//	else
		//		iAnchorPos = SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
		//	SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
		//}

		// extend selection to start of first line
		// the above code is not required when last line has been excluded
		if (iSelStart != iSelEnd) {
			int iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
			int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
			if (iCurPos < iAnchorPos) {
				iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
				iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
			} else {
				iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
				iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
			}
			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditAlignText()
//
void EditAlignText(HWND hwnd, int nMode) {
#define BUFSIZE_ALIGN 1024
	int		mbcp;
	BOOL	bModified = FALSE;

	int iSelStart	 = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	int iSelEnd		 = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	int iCurPos		 = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) {
		mbcp = CP_UTF8;
	} else {
		mbcp = CP_ACP;
	}

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iLine;
		int iMinIndent = BUFSIZE_ALIGN;
		int iMaxLength = 0;

		int iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		int iLineEnd	 = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		if (iSelEnd <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd, 0)) {
			if (iLineEnd - iLineStart >= 1) {
				iLineEnd--;
			}
		}

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iLineEndPos		 = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
			int iLineIndentPos = (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, (WPARAM)iLine, 0);

			if (iLineIndentPos != iLineEndPos) {
				int iIndentCol = (int)SendMessage(hwnd, SCI_GETLINEINDENTATION, (WPARAM)iLine, 0);
				int iEndCol;
				char ch;
				int iTail;

				iTail = iLineEndPos - 1;
				ch = (char)SendMessage(hwnd, SCI_GETCHARAT, (WPARAM)iTail, 0);
				while (iTail >= iLineStart && (ch == ' ' || ch == '\t')) {
					iTail--;
					ch = (char)SendMessage(hwnd, SCI_GETCHARAT, (WPARAM)iTail, 0);
					iLineEndPos--;
				}
				iEndCol = (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iLineEndPos, 0);

				iMinIndent = min_i(iMinIndent, iIndentCol);
				iMaxLength = max_i(iMaxLength, iEndCol);
			}
		}

		if (iMaxLength < BUFSIZE_ALIGN) {
			for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
				int iIndentPos = (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, (WPARAM)iLine, 0);
				int iEndPos		 = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);

				if (iIndentPos == iEndPos && iEndPos > 0) {
					if (!bModified) {
						SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
						bModified = TRUE;
					}

					SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0), 0);
					SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iEndPos, 0);
					SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
				} else {
					char tchLineBuf[BUFSIZE_ALIGN * kMaxMultiByteCount] = "";
					WCHAR wchLineBuf[BUFSIZE_ALIGN] = L"";
					WCHAR *pWords[BUFSIZE_ALIGN];
					WCHAR *p = wchLineBuf;

					int iWords = 0;
					int iWordsLength = 0;
					int cchLine = (int)SendMessage(hwnd, SCI_GETLINE, (WPARAM)iLine, (LPARAM)tchLineBuf);

					if (!bModified) {
						SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
						bModified = TRUE;
					}

					MultiByteToWideChar(mbcp, 0, tchLineBuf, cchLine, wchLineBuf, COUNTOF(wchLineBuf));
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
								if (SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) <= iLine + 1) {
									bNextLineIsBlank = TRUE;
								} else {
									int iLineEndPos		 = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine + 1, 0);
									int iLineIndentPos = (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, (WPARAM)iLine + 1, 0);
									if (iLineIndentPos == iLineEndPos) {
										bNextLineIsBlank = TRUE;
									}
								}
							}

							if ((nMode == ALIGN_JUSTIFY || nMode == ALIGN_JUSTIFY_EX) &&
									iWords > 1 && iWordsLength >= 2 &&
									((nMode != ALIGN_JUSTIFY_EX || !bNextLineIsBlank || iLineStart == iLineEnd) ||
									 (bNextLineIsBlank && iWordsLength > (iMaxLength - iMinIndent) * 0.75))) {
								int iGaps = iWords - 1;
								int iSpacesPerGap = (iMaxLength - iMinIndent - iWordsLength) / iGaps;
								int iExtraSpaces = (iMaxLength - iMinIndent - iWordsLength) % iGaps;
								int i, j;
								int iPos;

								WCHAR wchNewLineBuf[BUFSIZE_ALIGN * 3];
								StrCpy(wchNewLineBuf, pWords[0]);
								p = StrEnd(wchNewLineBuf);

								for (i = 1; i < iWords; i++) {
									for (j = 0; j < iSpacesPerGap; j++) {
										*p++ = L' ';
										*p = 0;
									}
									if (i > iGaps - iExtraSpaces) {
										*p++ = L' ';
										*p = 0;
									}
									StrCat(p, pWords[i]);
									p = StrEnd(p);
								}

								WideCharToMultiByte(mbcp, 0, wchNewLineBuf, -1, tchLineBuf, COUNTOF(tchLineBuf), NULL, NULL);

								iPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
								SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPos, 0);
								iPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
								SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iPos, 0);
								SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(tchLineBuf), (LPARAM)tchLineBuf);

								SendMessage(hwnd, SCI_SETLINEINDENTATION, (WPARAM)iLine, (LPARAM)iMinIndent);
							} else {
								int i;
								int iPos;

								WCHAR wchNewLineBuf[BUFSIZE_ALIGN];
								StrCpy(wchNewLineBuf, pWords[0]);
								p = StrEnd(wchNewLineBuf);

								for (i = 1; i < iWords; i++) {
									*p++ = L' ';
									*p = 0;
									StrCat(wchNewLineBuf, pWords[i]);
									p = StrEnd(p);
								}

								WideCharToMultiByte(mbcp, 0, wchNewLineBuf, -1, tchLineBuf, COUNTOF(tchLineBuf), NULL, NULL);

								iPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
								SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPos, 0);
								iPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
								SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iPos, 0);
								SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(tchLineBuf), (LPARAM)tchLineBuf);

								SendMessage(hwnd, SCI_SETLINEINDENTATION, (WPARAM)iLine, (LPARAM)iMinIndent);
							}
						} else {
							int iExtraSpaces = iMaxLength - iMinIndent - iWordsLength - iWords + 1;
							int iOddSpaces	 = iExtraSpaces % 2;
							int i;
							int iPos;

							WCHAR wchNewLineBuf[BUFSIZE_ALIGN * 3] = L"";
							p = wchNewLineBuf;

							if (nMode == ALIGN_RIGHT) {
								for (i = 0; i < iExtraSpaces; i++) {
									*p++ = L' ';
								}
								*p = 0;
							}

							if (nMode == ALIGN_CENTER) {
								for (i = 1; i < iExtraSpaces - iOddSpaces; i += 2) {
									*p++ = L' ';
								}
								*p = 0;
							}

							for (i = 0; i < iWords; i++) {
								StrCat(p, pWords[i]);
								if (i < iWords - 1) {
									StrCat(p, L" ");
								}
								if (nMode == ALIGN_CENTER && iWords > 1 && iOddSpaces > 0 && i + 1 >= iWords / 2) {
									StrCat(p, L" ");
									iOddSpaces--;
								}
								p = StrEnd(p);
							}

							WideCharToMultiByte(mbcp, 0, wchNewLineBuf, -1, tchLineBuf, COUNTOF(tchLineBuf), NULL, NULL);

							if (nMode == ALIGN_RIGHT || nMode == ALIGN_CENTER) {
								SendMessage(hwnd, SCI_SETLINEINDENTATION, (WPARAM)iLine, (LPARAM)iMinIndent);
								iPos = (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, (WPARAM)iLine, 0);
							} else {
								iPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
							}

							SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPos, 0);
							iPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
							SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iPos, 0);
							SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(tchLineBuf), (LPARAM)tchLineBuf);

							if (nMode == ALIGN_LEFT) {
								SendMessage(hwnd, SCI_SETLINEINDENTATION, (WPARAM)iLine, (LPARAM)iMinIndent);
							}
						}
					}
				}
			}
			if (bModified) {
				SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
			}
		} else {
			MsgBox(MBINFO, IDS_BUFFERTOOSMALL);
		}

		if (iCurPos < iAnchorPos) {
			iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
			iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
		} else {
			iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
			iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
		}
		SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditEncloseSelection()
//
void EditEncloseSelection(HWND hwnd, LPCWSTR pwszOpen, LPCWSTR pwszClose) {
	char	mszOpen[256] = "";
	char	mszClose[256] = "";
	int		mbcp;

	int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

	if (SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) {
		mbcp = CP_UTF8;
	} else {
		mbcp = CP_ACP;
	}

	if (StrNotEmpty(pwszOpen)) {
		WideCharToMultiByte(mbcp, 0, pwszOpen, -1, mszOpen, COUNTOF(mszOpen), NULL, NULL);
	}

	if (StrNotEmpty(pwszClose)) {
		WideCharToMultiByte(mbcp, 0, pwszClose, -1, mszClose, COUNTOF(mszClose), NULL, NULL);
	}

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		if (StrNotEmptyA(mszOpen)) {
			SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelStart, 0);
			SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelStart, 0);
			SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(mszOpen), (LPARAM)mszOpen);
		}

		if (StrNotEmptyA(mszClose)) {
			SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelEnd + lstrlenA(mszOpen), 0);
			SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelEnd + lstrlenA(mszOpen), 0);
			SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(mszClose), (LPARAM)mszClose);
		}

		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

		// Fix selection
		if (iSelStart == iSelEnd) {
			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iSelStart + lstrlenA(mszOpen),
						(WPARAM)iSelStart + lstrlenA(mszOpen));
		} else {
			int iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
			int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

			if (iCurPos < iAnchorPos) {
				iCurPos = iSelStart + lstrlenA(mszOpen);
				iAnchorPos = iSelEnd + lstrlenA(mszOpen);
			} else {
				iAnchorPos = iSelStart + lstrlenA(mszOpen);
				iCurPos = iSelEnd + lstrlenA(mszOpen);
			}
			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditToggleLineComments()
//
void EditToggleLineComments(HWND hwnd, LPCWSTR pwszComment, BOOL bInsertAtStart) {
	char	mszComment[256] = "";
	int		cchComment;
	int		mbcp;

	int iSelStart	= (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	int iCurPos		= (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);

	if (SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) {
		mbcp = CP_UTF8;
	} else {
		mbcp = CP_ACP;
	}

	if (StrNotEmpty(pwszComment)) {
		WideCharToMultiByte(mbcp, 0, pwszComment, -1, mszComment, COUNTOF(mszComment), NULL, NULL);
	}
	cchComment = lstrlenA(mszComment);

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0) && cchComment) {
		int iAction = 0;
		int iLine;
		int iCommentCol = 0;

		int iLineStart	= (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		int iLineEnd	= (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		if (iSelEnd <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd, 0)) {
			if (iLineEnd - iLineStart >= 1) {
				iLineEnd--;
			}
		}

		if (!bInsertAtStart) {
			iCommentCol = 1024;
			for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
				int iLineEndPos		= (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
				int iLineIndentPos	= (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, (WPARAM)iLine, 0);

				if (iLineIndentPos != iLineEndPos) {
					int iIndentColumn = (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iLineIndentPos, 0);
					iCommentCol = min_i(iCommentCol, iIndentColumn);
				}
			}
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iCommentPos;
			int iIndentPos = (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, (WPARAM)iLine, 0);
			char tchBuf[32] = "";
			struct Sci_TextRange tr;
			BOOL bWhitespaceLine = FALSE;
			int ch;

			// a line with [space/tab] only
			if (iCommentCol && iIndentPos == SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0)) {
				//continue;
				bWhitespaceLine = TRUE;
			}

			tr.chrg.cpMin = iIndentPos;
			tr.chrg.cpMax = tr.chrg.cpMin + min_i(31, cchComment);
			tr.lpstrText = tchBuf;
			SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

			if (_strnicmp(tchBuf, mszComment, cchComment) == 0) {
				switch (iAction) {
				case 0:
					iAction = 2;
				// fall through
				case 2:
					iCommentPos = iIndentPos;
					// a line with [space/tab] comment only
					if ((ch = (int)SendMessage(hwnd, SCI_GETCHARAT, (iIndentPos + cchComment), 0)) != '\0' && (ch == '\n' || ch == '\r')) {
						iCommentPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
					}
					SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iCommentPos, 0);
					SendMessage(hwnd, SCI_SETTARGETEND, (iIndentPos + cchComment), 0);
					SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
					break;
				case 1:
					iCommentPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN, (WPARAM)iLine, (LPARAM)iCommentCol);
					if ((ch = (int)SendMessage(hwnd, SCI_GETCHARAT, (WPARAM)(iCommentPos), 0)) != '\0' && (ch == '\t' || ch == ' ')) {
						SendMessage(hwnd, SCI_INSERTTEXT, (WPARAM)iCommentPos, (LPARAM)mszComment);
					}
					break;
				}
			} else {
				switch (iAction) {
				case 0:
					iAction = 1;
				// fall through
				case 1:
					iCommentPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN, (WPARAM)iLine, (LPARAM)iCommentCol);
					if (!bWhitespaceLine || (iLineStart == iLineEnd)) {
						SendMessage(hwnd, SCI_INSERTTEXT, (WPARAM)iCommentPos, (LPARAM)mszComment);
					} else {
						char tchComment[1024] = "";
						ch = 0;
						mbcp = iCommentCol;
						if (!bTabsAsSpaces && iTabWidth > 0) {
							ch = iCommentCol / iTabWidth;
							memset(tchComment, '\t', ch);
							mbcp -= ch * iTabWidth;
						}
						memset(tchComment + ch, ' ', mbcp);
						lstrcatA(tchComment, mszComment);
						SendMessage(hwnd, SCI_INSERTTEXT, (WPARAM)iCommentPos, (LPARAM)tchComment);
					}
					break;
				case 2:
					break;
				}
			}
		}

		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

		if (iSelStart != iSelEnd) {
			int iAnchorPos;
			if (iCurPos == iSelStart) {
				iCurPos		= (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
				iAnchorPos	= (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
			} else {
				iAnchorPos	= (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
				iCurPos		= (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
			}
			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
		}
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditPadWithSpaces()
//
void EditPadWithSpaces(HWND hwnd, BOOL bSkipEmpty, BOOL bNoUndoGroup) {
	char *pmszPadStr;
	int iMaxColumn = 0;
	int iLine;
	BOOL bIsRectangular = FALSE;
	BOOL bReducedSelection = FALSE;

	int iSelStart = 0;
	int iSelEnd = 0;

	int iLineStart = 0;
	int iLineEnd = 0;

	int iRcCurLine = 0;
	int iRcAnchorLine = 0;
	int iRcCurCol = 0;
	int iRcAnchorCol = 0;

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

		iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		iLineEnd	 = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		if (iLineStart == iLineEnd) {
			iLineStart = 0;
			iLineEnd = (int)SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 1;
		} else {
			if (iSelEnd <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd, 0)) {
				if (iLineEnd - iLineStart >= 1) {
					iLineEnd--;
					bReducedSelection = TRUE;
				}
			}
		}

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
			iMaxColumn = max_i(iMaxColumn, (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iPos, 0));
		}
	} else {
		int iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
		int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

		iRcCurLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurPos, 0);
		iRcAnchorLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iAnchorPos, 0);

		iRcCurCol = (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iCurPos, 0);
		iRcAnchorCol = (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iAnchorPos, 0);

		bIsRectangular = TRUE;

		iLineStart = 0;
		iLineEnd = (int)SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 1;

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iPos = (int)SendMessage(hwnd, SCI_GETLINESELENDPOSITION, (WPARAM)iLine, 0);
			if (iPos != INVALID_POSITION) {
				iMaxColumn = max_i(iMaxColumn, (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iPos, 0));
			}
		}
	}

	pmszPadStr = NP2HeapAlloc((iMaxColumn + 1) * sizeof(char));
	if (pmszPadStr) {
		FillMemory(pmszPadStr, NP2HeapSize(pmszPadStr), ' ');

		if (!bNoUndoGroup) {
			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
		}

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iPos;
			int iPadLen;
			int iLineSelEndPos;

			iLineSelEndPos = (int)SendMessage(hwnd, SCI_GETLINESELENDPOSITION, (WPARAM)iLine, 0);
			if (bIsRectangular && INVALID_POSITION == iLineSelEndPos) {
				continue;
			}

			iPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);
			if (bIsRectangular && iPos > iLineSelEndPos) {
				continue;
			}

			if (bSkipEmpty && (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0) >= iPos) {
				continue;
			}

			iPadLen = iMaxColumn - (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iPos, 0);

			SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPos, 0);
			SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iPos, 0);
			SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)iPadLen, (LPARAM)pmszPadStr);
		}

		NP2HeapFree(pmszPadStr);

		if (!bNoUndoGroup) {
			SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
		}
	}

	if (!bIsRectangular &&
			SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0) !=
			SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0)) {
		int iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
		int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

		if (iCurPos < iAnchorPos) {
			iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
			if (!bReducedSelection) {
				iAnchorPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineEnd, 0);
			} else {
				iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
			}
		} else {
			iAnchorPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0);
			if (!bReducedSelection) {
				iCurPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineEnd, 0);
			} else {
				iCurPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0);
			}
		}
		SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
	} else if (bIsRectangular) {
		int iCurPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN, (WPARAM)iRcCurLine, (LPARAM)iRcCurCol);
		int iAnchorPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN, (WPARAM)iRcAnchorLine, (LPARAM)iRcAnchorCol);

		SendMessage(hwnd, SCI_SETRECTANGULARSELECTIONCARET, (WPARAM)iCurPos, 0);
		SendMessage(hwnd, SCI_SETRECTANGULARSELECTIONANCHOR, (WPARAM)iAnchorPos, 0);
	}
}

//=============================================================================
//
// EditStripFirstCharacter()
//
void EditStripFirstCharacter(HWND hwnd) {
	int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

	//if (iSelStart == iSelEnd) {
	//	iSelStart = 0;
	//	iSelEnd		= (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	//}

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iLine;

		int iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		int iLineEnd	 = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		if (iLineStart != iLineEnd) {
			if (iSelStart > SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0)) {
				iLineStart++;
			}

			if (iSelEnd <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd, 0)) {
				iLineEnd--;
			}
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
			if (SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0) - iPos > 0) {
				SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPos, 0);
				SendMessage(hwnd, SCI_SETTARGETEND,
							(WPARAM)SendMessage(hwnd, SCI_POSITIONAFTER, (WPARAM)iPos, 0), 0);
				SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
			}
		}
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditStripLastCharacter()
//
void EditStripLastCharacter(HWND hwnd) {
	int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

	//if (iSelStart == iSelEnd) {
	//	iSelStart = 0;
	//	iSelEnd		= (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	//}

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iLine;

		int iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		int iLineEnd	 = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		if (iLineStart != iLineEnd) {
			if (iSelStart >= SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineStart, 0)) {
				iLineStart++;
			}

			if (iSelEnd < SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineEnd, 0)) {
				iLineEnd--;
			}
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
			int iStartPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
			int iEndPos		= (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0);

			if (iEndPos - iStartPos > 0) {
				SendMessage(hwnd, SCI_SETTARGETSTART,
							(WPARAM)SendMessage(hwnd, SCI_POSITIONBEFORE, (WPARAM)iEndPos, 0), 0);
				SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iEndPos, 0);
				SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
			}
		}
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditStripTrailingBlanks()
//
void EditStripTrailingBlanks(HWND hwnd, BOOL bIgnoreSelection) {
	// Check if there is any selection... simply use a regular expression replace!
	if (!bIgnoreSelection && !EditIsEmptySelection()) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			EDITFINDREPLACE efrTrim = {
				.szFind = "[ \t]+$",
				.fuFlags = SCFIND_REGEXP,
				.hwnd = hwnd,
			};
			if (EditReplaceAllInSelection(hwnd, &efrTrim, FALSE)) {
				return;
			}
		}
	}
	// Code from SciTE...
	{
		int line;
		int maxLines;

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
		maxLines = (int)SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0);
		for (line = 0; line < maxLines; line++) {
			int lineStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, line, 0);
			int lineEnd = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, line, 0);
			int i = lineEnd - 1;
			char ch = (char)SendMessage(hwnd, SCI_GETCHARAT, i, 0);
			while ((i >= lineStart) && ((ch == ' ') || (ch == '\t'))) {
				i--;
				ch = (char)SendMessage(hwnd, SCI_GETCHARAT, i, 0);
			}
			if (i < (lineEnd - 1)) {
				SendMessage(hwnd, SCI_SETTARGETSTART, i + 1, 0);
				SendMessage(hwnd, SCI_SETTARGETEND, lineEnd, 0);
				SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
			}
		}
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	}
}

//=============================================================================
//
// EditStripLeadingBlanks()
//
void EditStripLeadingBlanks(HWND hwnd, BOOL bIgnoreSelection) {
	// Check if there is any selection... simply use a regular expression replace!
	if (!bIgnoreSelection && !EditIsEmptySelection()) {
		if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
			EDITFINDREPLACE efrTrim = {
				.szFind = "^[ \t]+",
				.fuFlags = SCFIND_REGEXP,
				.hwnd = hwnd,
			};
			if (EditReplaceAllInSelection(hwnd, &efrTrim, FALSE)) {
				return;
			}
		}
	}
	// Code from SciTE...
	{
		int line;
		int maxLines;

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
		maxLines = (int)SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0);
		for (line = 0; line < maxLines; line++) {
			int lineStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, line, 0);
			int lineEnd = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, line, 0);
			int i = lineStart;
			char ch = (char)SendMessage(hwnd, SCI_GETCHARAT, i, 0);
			while ((i <= lineEnd - 1) && ((ch == ' ') || (ch == '\t'))) {
				i++;
				ch = (char)SendMessage(hwnd, SCI_GETCHARAT, i, 0);
			}
			if (i > lineStart) {
				SendMessage(hwnd, SCI_SETTARGETSTART, lineStart, 0);
				SendMessage(hwnd, SCI_SETTARGETEND, i, 0);
				SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
			}
		}
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	}
}

//=============================================================================
//
// EditCompressSpaces()
//
void EditCompressSpaces(HWND hwnd) {
	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iSelStart	 = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		int iSelEnd		 = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
		int iCurPos		 = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
		int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
		int iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		int iLineEnd	 = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);
		int iLength		 = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);

		char *pszIn;
		char *pszOut;
		BOOL bIsLineStart, bIsLineEnd;
		BOOL bModified = FALSE;

		if (iSelStart != iSelEnd) {
			int cch = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0);
			pszIn = NP2HeapAlloc(cch);
			pszOut = NP2HeapAlloc(cch);
			SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszIn);
			bIsLineStart =
				(iSelStart == SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0));
			bIsLineEnd =
				(iSelEnd == SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLineEnd, 0));
		} else {
			int cch = iLength + 1;
			pszIn = NP2HeapAlloc(cch);
			pszOut = NP2HeapAlloc(cch);
			SendMessage(hwnd, SCI_GETTEXT, (WPARAM)cch, (LPARAM)pszIn);
			bIsLineStart = TRUE;
			bIsLineEnd	 = TRUE;
		}

		{
			char *ci, *co = pszOut;
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
					SendMessage(hwnd, SCI_TARGETFROMSELECTION, 0, 0);
				} else {
					SendMessage(hwnd, SCI_SETTARGETSTART, 0, 0);
					SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iLength, 0);
				}
				SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
				SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM) - 1, (LPARAM)pszOut);
				if (iCurPos > iAnchorPos) {
					iCurPos		 = (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
					iAnchorPos = (int)SendMessage(hwnd, SCI_GETTARGETSTART, 0, 0);
					SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
				} else if (iCurPos < iAnchorPos) {
					iCurPos		 = (int)SendMessage(hwnd, SCI_GETTARGETSTART, 0, 0);
					iAnchorPos = (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
					SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
				}
				SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
			}
		}

		NP2HeapFree(pszIn);
		NP2HeapFree(pszOut);
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditRemoveBlankLines()
//
void EditRemoveBlankLines(HWND hwnd, BOOL bMerge) {
	int iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

	if (iSelStart == iSelEnd) {
		iSelStart = 0;
		iSelEnd		= (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	}

	if (SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		int iLine;

		int iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		int iLineEnd	 = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		if (iSelStart > SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0)) {
			iLineStart++;
		}

		if (iSelEnd <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd, 0) &&
				iLineEnd != SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0) - 1) {
			iLineEnd--;
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		for (iLine = iLineStart; iLine <= iLineEnd;) {
			int nBlanks = 0;
			while (iLine + nBlanks <= iLineEnd &&
					SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine + nBlanks, 0) ==
					SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine + nBlanks, 0)) {
				nBlanks++;
			}

			if (nBlanks == 0 || (nBlanks == 1 && bMerge)) {
				iLine += nBlanks + 1;
			} else {
				int iTargetStart;
				int iTargetEnd;

				if (bMerge) {
					nBlanks--;
				}

				iTargetStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
				iTargetEnd	 = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine + nBlanks, 0);

				SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iTargetStart, 0);
				SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iTargetEnd, 0);
				SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");

				if (bMerge) {
					iLine++;
				}
				iLineEnd -= nBlanks;
			}
		}
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	} else {
		MsgBox(MBWARN, IDS_SELRECT);
	}
}

//=============================================================================
//
// EditWrapToColumn()
//
void EditWrapToColumn(HWND hwnd, int nColumn/*, int nTabWidth*/) {
	char *pszText;
	LPWSTR pszTextW;
	int cchTextW;
	int iTextW;
	LPWSTR pszConvW;
	int cchConvW;

	int iLineLength;
	int iLine;
	int iCurPos;
	int iAnchorPos;

	int iSelStart;
	int iSelEnd;
	int iSelCount;

	UINT cpEdit;
	struct Sci_TextRange tr;
	int		cEOLMode;
	WCHAR wszEOL[] = L"\r\n";
	int		cchEOL = 2;
	BOOL bModified = FALSE;

	if (SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		MsgBox(MBWARN, IDS_SELRECT);
		return;
	}

	iCurPos		 = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos == iAnchorPos) /*{
		iSelStart = 0;
		iSelEnd		= SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	}*/
	{
		return;
	} else {
		iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	}

	iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
	iSelStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);

	iSelCount = iSelEnd - iSelStart;

	pszText = NP2HeapAlloc((iSelCount) + 2);
	pszTextW = NP2HeapAlloc((iSelCount * 2) + 2);

	tr.chrg.cpMin = iSelStart;
	tr.chrg.cpMax = iSelEnd;
	tr.lpstrText = pszText;
	SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

	cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
	cchTextW = MultiByteToWideChar(cpEdit, 0, pszText, iSelCount, pszTextW,
								   (int)NP2HeapSize(pszTextW) / sizeof(WCHAR));
	NP2HeapFree(pszText);

	pszConvW = NP2HeapAlloc(cchTextW * sizeof(WCHAR) * 3 + 2);

	cEOLMode = (int)SendMessage(hwnd, SCI_GETEOLMODE, 0, 0);
	if (cEOLMode == SC_EOL_CR) {
		cchEOL = 1;
	} else if (cEOLMode == SC_EOL_LF) {
		cchEOL = 1;
		wszEOL[0] = L'\n';
	}

	cchConvW = 0;
	iLineLength = 0;

#define ISDELIMITER(wc) StrChr(L",;.:-+%&\xA6|/*?!\"\'~\xB4#=", wc)
#define ISWHITE(wc) StrChr(L" \t", wc)
#define ISWORDEND(wc) (/*ISDELIMITER(wc) ||*/ StrChr(L" \t\r\n", wc))

	for (iTextW = 0; iTextW < cchTextW; iTextW++) {
		WCHAR w;
		w = pszTextW[iTextW];

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
			int iNextWordLen = 0;
			WCHAR w2;

			while (pszTextW[iTextW + 1] == L' ' || pszTextW[iTextW + 1] == L'\t') {
				iTextW++;
				bModified = TRUE;
			} // Modified: left out some whitespaces

			w2 = pszTextW[iTextW + 1];

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
		int cchConvM;
		pszText = NP2HeapAlloc(cchConvW * kMaxMultiByteCount);

		cchConvM = WideCharToMultiByte(cpEdit, 0, pszConvW, cchConvW, pszText,
									   (int)NP2HeapSize(pszText), NULL, NULL);
		NP2HeapFree(pszConvW);

		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + cchConvM;
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + cchConvM;
		}

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelStart, 0);
		SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelEnd, 0);
		SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)cchConvM, (LPARAM)pszText);
		//SendMessage(hwnd, SCI_CLEAR, 0, 0);
		//SendMessage(hwnd, SCI_ADDTEXT, (WPARAM)cchConvW, (LPARAM)pszText);
		SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);

		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

		NP2HeapFree(pszText);
	} else {
		NP2HeapFree(pszConvW);
	}
}

//=============================================================================
//
// EditJoinLinesEx()
//
void EditJoinLinesEx(HWND hwnd) {
	char *pszText;
	char *pszJoin;
	int		cchJoin = 0;

	int i;
	int iLine;
	int iCurPos;
	int iAnchorPos;

	int iSelStart;
	int iSelEnd;
	int iSelCount;

	struct Sci_TextRange tr;
	int	 cEOLMode;
	char szEOL[] = "\r\n";
	int	 cchEOL = 2;
	BOOL bModified = FALSE;

	if (SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		MsgBox(MBWARN, IDS_SELRECT);
		return;
	}

	iCurPos		 = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos == iAnchorPos) {
		return;
	} else {
		iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	}

	iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
	iSelStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);

	iSelCount = iSelEnd - iSelStart;

	pszText = NP2HeapAlloc((iSelCount) + 2);
	pszJoin = NP2HeapAlloc(NP2HeapSize(pszText));

	tr.chrg.cpMin = iSelStart;
	tr.chrg.cpMax = iSelEnd;
	tr.lpstrText = pszText;
	SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

	cEOLMode = (int)SendMessage(hwnd, SCI_GETEOLMODE, 0, 0);
	if (cEOLMode == SC_EOL_CR) {
		cchEOL = 1;
	} else if (cEOLMode == SC_EOL_LF) {
		cchEOL = 1;
		szEOL[0] = '\n';
	}

	cchJoin = 0;

	for (i = 0; i < iSelCount; i++) {
		if (pszText[i] == '\r' || pszText[i] == '\n') {
			if (pszText[i] == '\r' && pszText[i + 1] == '\n') {
				i++;
			}
			if (!StrChrA("\r\n", pszText[i + 1]) && pszText[i + 1] != 0) {
				pszJoin[cchJoin++] = ' ';
				bModified = TRUE;
			} else {
				while (StrChrA("\r\n", pszText[i + 1])) {
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

		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);

		SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iSelStart, 0);
		SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iSelEnd, 0);
		SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)cchJoin, (LPARAM)pszJoin);
		SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);

		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
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

int CmpStd(const void *s1, const void *s2) {
	int cmp = StrCmp(((SORTLINE *)s1)->pwszSortEntry, ((SORTLINE *)s2)->pwszSortEntry);
	return (cmp) ? cmp : StrCmp(((SORTLINE *)s1)->pwszLine, ((SORTLINE *)s2)->pwszLine);
}

int CmpStdRev(const void *s1, const void *s2) {
	return CmpStd(s2, s1);
}

int CmpLogical(const void *s1, const void *s2) {
	int cmp = (int)StrCmpLogicalW(((SORTLINE *)s1)->pwszSortEntry, ((SORTLINE *)s2)->pwszSortEntry);
	if (cmp == 0) {
		cmp = (int)StrCmpLogicalW(((SORTLINE *)s1)->pwszLine, ((SORTLINE *)s2)->pwszLine);
	}
	if (cmp == 0) {
		cmp = StrCmp(((SORTLINE *)s1)->pwszSortEntry, ((SORTLINE *)s2)->pwszSortEntry);
	}
	return (cmp) ? cmp : StrCmp(((SORTLINE *)s1)->pwszLine, ((SORTLINE *)s2)->pwszLine);
}

int CmpLogicalRev(const void *s1, const void *s2) {
	return CmpLogical(s2, s1);
}

void EditSortLines(HWND hwnd, int iSortFlags) {
	int iCurPos;
	int iAnchorPos;
	int iSelStart = 0;
	int iLineStart;
	int iLineEnd;
	int iLineCount;

	BOOL bIsRectangular = FALSE;
	int iRcCurLine = 0;
	int iRcAnchorLine = 0;
	int iRcCurCol = 0;
	int iRcAnchorCol = 0;

	int	 i, iLine;
	int	 cchTotal = 0;
	int	 ichlMax	= 3;

	SORTLINE *pLines;
	char	*pmszResult;
	char	*pmszBuf;

	UINT uCodePage;
	DWORD cEOLMode;
	char mszEOL[] = "\r\n";

	UINT iSortColumn;

	BOOL bLastDup = FALSE;
	FNSTRCMP pfnStrCmp;

	pfnStrCmp = (iSortFlags & SORT_NOCASE) ? StrCmpIW : StrCmpW;

	iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos == iAnchorPos) {
		return;
	}

	if (SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		iRcCurLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurPos, 0);
		iRcAnchorLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iAnchorPos, 0);

		iRcCurCol = (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iCurPos, 0);
		iRcAnchorCol = (int)SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iAnchorPos, 0);

		bIsRectangular = TRUE;
		iLineStart = min_i(iRcCurLine, iRcAnchorLine);
		iLineEnd	 = max_i(iRcCurLine, iRcAnchorLine);
		iSortColumn = min_i(iRcCurCol, iRcAnchorCol);
	} else {
		int iSelEnd;
		iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
		iSelEnd = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

		iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		iSelStart = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);

		iLineStart = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelStart, 0);
		iLineEnd	 = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iSelEnd, 0);

		if (iSelEnd <= SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd, 0)) {
			iLineEnd--;
		}

		iSortColumn = (UINT)SendMessage(hwnd, SCI_GETCOLUMN,
										(WPARAM)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0), 0);
	}

	iLineCount = iLineEnd - iLineStart + 1;
	if (iLineCount < 2) {
		return;
	}

	uCodePage = (SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0) == SC_CP_UTF8) ? CP_UTF8 : CP_ACP;

	cEOLMode = (DWORD)SendMessage(hwnd, SCI_GETEOLMODE, 0, 0);
	if (cEOLMode == SC_EOL_CR) {
		mszEOL[1] = 0;
	} else if (cEOLMode == SC_EOL_LF) {
		mszEOL[0] = '\n';
		mszEOL[1] = 0;
	}

	iTabWidth = (UINT)SendMessage(hwnd, SCI_GETTABWIDTH, 0, 0);

	SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
	if (bIsRectangular) {
		EditPadWithSpaces(hwnd, !(iSortFlags & SORT_SHUFFLE), TRUE);
	}

	pLines = NP2HeapAlloc(sizeof(SORTLINE) * iLineCount);
	i = 0;
	for (iLine = iLineStart; iLine <= iLineEnd; iLine++) {
		char *pmsz;
		int cchw;
		int cchm = (int)SendMessage(hwnd, SCI_GETLINE, (WPARAM)iLine, 0);

		pmsz = NP2HeapAlloc(cchm + 1);
		SendMessage(hwnd, SCI_GETLINE, (WPARAM)iLine, (LPARAM)pmsz);
		StrTrimA(pmsz, "\r\n");
		cchTotal += cchm;
		ichlMax = max_i(ichlMax, cchm);

		cchw = MultiByteToWideChar(uCodePage, 0, pmsz, -1, NULL, 0) - 1;
		if (cchw > 0) {
			pLines[i].pwszLine = LocalAlloc(LPTR, sizeof(WCHAR) * (cchw + 1));
			MultiByteToWideChar(uCodePage, 0, pmsz, -1, pLines[i].pwszLine,
								(int)LocalSize(pLines[i].pwszLine) / sizeof(WCHAR));
			pLines[i].pwszSortEntry = pLines[i].pwszLine;

			if (iSortFlags & SORT_COLUMN) {
				UINT col = 0, tabs = iTabWidth;
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
		i++;
	}

	if (iSortFlags & SORT_DESCENDING) {
		if ((iSortFlags & SORT_LOGICAL) && IsWinXPAndAbove()) {
			qsort(pLines, iLineCount, sizeof(SORTLINE), CmpLogicalRev);
		} else {
			qsort(pLines, iLineCount, sizeof(SORTLINE), CmpStdRev);
		}
	} else if (iSortFlags & SORT_SHUFFLE) {
		srand((UINT)GetTickCount());
		for (i = iLineCount - 1; i > 0; i--) {
			int j = rand() % i;
			SORTLINE sLine;
			sLine.pwszLine = pLines[i].pwszLine;
			sLine.pwszSortEntry = pLines[i].pwszSortEntry;
			pLines[i] = pLines[j];
			pLines[j].pwszLine = sLine.pwszLine;
			pLines[j].pwszSortEntry = sLine.pwszSortEntry;
		}
	} else {
		if ((iSortFlags & SORT_LOGICAL) && IsWinXPAndAbove()) {
			qsort(pLines, iLineCount, sizeof(SORTLINE), CmpLogical);
		} else {
			qsort(pLines, iLineCount, sizeof(SORTLINE), CmpStd);
		}
	}

	pmszResult = NP2HeapAlloc(cchTotal + 2 * iLineCount + 1);
	pmszBuf = NP2HeapAlloc(ichlMax + 1);
	const int cbPmszBuf = (int)NP2HeapSize(pmszBuf);
	const int cbPmszResult = (int)NP2HeapSize(pmszResult);

	for (i = 0; i < iLineCount; i++) {
		BOOL bDropLine = FALSE;
		if (pLines[i].pwszLine && ((iSortFlags & SORT_SHUFFLE) || StrNotEmpty(pLines[i].pwszLine))) {
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
				WideCharToMultiByte(uCodePage, 0, pLines[i].pwszLine, -1, pmszBuf, cbPmszBuf, NULL, NULL);
				StrCatBuffA(pmszResult, pmszBuf, cbPmszResult);
				StrCatBuffA(pmszResult, mszEOL, cbPmszResult);
			}
		}
	}

	NP2HeapFree(pmszBuf);

	for (i = 0; i < iLineCount; i++) {
		if (pLines[i].pwszLine) {
			LocalFree(pLines[i].pwszLine);
		}
	}

	NP2HeapFree(pLines);

	if (!bIsRectangular) {
		if (iAnchorPos > iCurPos) {
			iCurPos = iSelStart;
			iAnchorPos = iSelStart + lstrlenA(pmszResult);
		} else {
			iAnchorPos = iSelStart;
			iCurPos = iSelStart + lstrlenA(pmszResult);
		}
	}

	SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineStart, 0), 0);
	SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLineEnd + 1, 0), 0);
	SendMessage(hwnd, SCI_REPLACETARGET, (WPARAM)lstrlenA(pmszResult), (LPARAM)pmszResult);
	SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

	NP2HeapFree(pmszResult);

	if (!bIsRectangular) {
		SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos, (LPARAM)iCurPos);
	} else {
		int iTargetStart = (int)SendMessage(hwnd, SCI_GETTARGETSTART, 0, 0);
		int iTargetEnd	 = (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
		SendMessage(hwnd, SCI_CLEARSELECTIONS, 0, 0);
		if (iTargetStart != iTargetEnd) {
			iTargetEnd -= lstrlenA(mszEOL);
			if (iRcAnchorLine > iRcCurLine) {
				iCurPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN,
										   (WPARAM)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iTargetStart, 0), (LPARAM)iRcCurCol);
				iAnchorPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN,
											  (WPARAM)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iTargetEnd, 0), (LPARAM)iRcAnchorCol);
			} else {
				iCurPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN,
										   (WPARAM)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iTargetEnd, 0), (LPARAM)iRcCurCol);
				iAnchorPos = (int)SendMessage(hwnd, SCI_FINDCOLUMN,
											  (WPARAM)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iTargetStart, 0), (LPARAM)iRcAnchorCol);
			}
			if (iCurPos != iAnchorPos) {
				SendMessage(hwnd, SCI_SETRECTANGULARSELECTIONCARET, (WPARAM)iCurPos, 0);
				SendMessage(hwnd, SCI_SETRECTANGULARSELECTIONANCHOR, (WPARAM)iAnchorPos, 0);
			} else {
				SendMessage(hwnd, SCI_SETSEL, (WPARAM)iTargetStart, (LPARAM)iTargetStart);
			}
		} else {
			SendMessage(hwnd, SCI_SETSEL, (WPARAM)iTargetStart, (LPARAM)iTargetStart);
		}
	}
}

//=============================================================================
//
// EditJumpTo()
//
void EditJumpTo(HWND hwnd, int iNewLine, int iNewCol) {
	int iMaxLine = (int)SendMessage(hwnd, SCI_GETLINECOUNT, 0, 0);

	// Jumpt to end with line set to -1
	if (iNewLine == -1) {
		SendMessage(hwnd, SCI_DOCUMENTEND, 0, 0);
		return;
	}

	// Line maximum is iMaxLine
	iNewLine = min_i(iNewLine, iMaxLine);

	// Column minimum is 1
	iNewCol = max_i(iNewCol, 1);

	if (iNewLine > 0 && iNewLine <= iMaxLine && iNewCol > 0) {
		int iNewPos	 = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iNewLine - 1, 0);
		int iLineEndPos = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iNewLine - 1, 0);

		while (iNewCol - 1 > SendMessage(hwnd, SCI_GETCOLUMN, (WPARAM)iNewPos, 0)) {
			if (iNewPos >= iLineEndPos) {
				break;
			}

			iNewPos = (int)SendMessage(hwnd, SCI_POSITIONAFTER, (WPARAM)iNewPos, 0);
		}

		iNewPos = min_i(iNewPos, iLineEndPos);
		EditSelectEx(hwnd, -1, iNewPos); // SCI_GOTOPOS(pos) is equivalent to SCI_SETSEL(-1, pos)
		SendMessage(hwnd, SCI_CHOOSECARETX, 0, 0);
	}
}

//=============================================================================
//
// EditSelectEx()
//
void EditSelectEx(HWND hwnd, int iAnchorPos, int iCurrentPos) {
	int iNewLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurrentPos, 0);
	int iAnchorLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iAnchorPos, 0);

	// Ensure that the first and last lines of a selection are always unfolded
	// This needs to be done *before* the SCI_SETSEL message
	SciCall_EnsureVisible(iAnchorLine);
	if (iAnchorLine != iNewLine) {
		SciCall_EnsureVisible(iNewLine);
	}

	SendMessage(hwnd, SCI_SETXCARETPOLICY, CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
	SendMessage(hwnd, SCI_SETYCARETPOLICY, CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
	SendMessage(hwnd, SCI_SETSEL, iAnchorPos, iCurrentPos);
	SendMessage(hwnd, SCI_SETXCARETPOLICY, CARET_SLOP | CARET_EVEN, 50);
	SendMessage(hwnd, SCI_SETYCARETPOLICY, CARET_EVEN, 0);
}

//=============================================================================
//
// EditFixPositions()
//
void EditFixPositions(HWND hwnd) {
	int iMaxPos = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	int iCurrentPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurrentPos > 0 && iCurrentPos < iMaxPos) {
		int iNewPos = (int)SendMessage(hwnd, SCI_POSITIONAFTER, (WPARAM)
									   (int)SendMessage(hwnd, SCI_POSITIONBEFORE, (WPARAM)iCurrentPos, 0), 0);
		if (iNewPos != iCurrentPos) {
			SendMessage(hwnd, SCI_SETCURRENTPOS, (WPARAM)iNewPos, 0);
			iCurrentPos = iNewPos;
		}
	}

	if (iAnchorPos != iCurrentPos && iAnchorPos > 0 && iAnchorPos < iMaxPos) {
		int iNewPos = (int)SendMessage(hwnd, SCI_POSITIONAFTER, (WPARAM)
									   (int)SendMessage(hwnd, SCI_POSITIONBEFORE, (WPARAM)iAnchorPos, 0), 0);
		if (iNewPos != iAnchorPos) {
			SendMessage(hwnd, SCI_SETANCHOR, (WPARAM)iNewPos, 0);
		}
	}
}

//=============================================================================
//
// EditEnsureSelectionVisible()
//
void EditEnsureSelectionVisible(HWND hwnd) {
	int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
	int iCurrentPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);

	SendMessage(hwnd, SCI_ENSUREVISIBLE,
				(WPARAM)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iAnchorPos, 0), 0);
	if (iAnchorPos != iCurrentPos) {
		SendMessage(hwnd, SCI_ENSUREVISIBLE,
					(WPARAM)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurrentPos, 0), 0);
	}
	EditSelectEx(hwnd, iAnchorPos, iCurrentPos);
}

//=============================================================================
//
// EditGetExcerpt()
//
void EditGetExcerpt(HWND hwnd, LPWSTR lpszExcerpt, DWORD cchExcerpt) {
	WCHAR tch[256] = L"";
	DWORD cch = 0;
	struct Sci_TextRange tr;
	char	 *pszText;
	LPWSTR pszTextW;

	int iCurPos		 = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	if (iCurPos == iAnchorPos || SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		lstrcpy(lpszExcerpt, L"");
		return;
	}

	/*if (iCurPos != iAnchorPos && SC_SEL_RECTANGLE != SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {*/
	tr.chrg.cpMin = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	tr.chrg.cpMax = min_i((int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0), (LONG)(tr.chrg.cpMin + COUNTOF(tch)));
	/*}
	else {
		int iLine = SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurPos, 0);
		tr.chrg.cpMin = SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);
		tr.chrg.cpMax = min_i(SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iLine, 0), (LONG)(tr.chrg.cpMin + COUNTOF(tch)));
	}*/

	tr.chrg.cpMax = min_i((int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0), tr.chrg.cpMax);

	pszText	 = NP2HeapAlloc((tr.chrg.cpMax - tr.chrg.cpMin) + 2);
	pszTextW = NP2HeapAlloc(((tr.chrg.cpMax - tr.chrg.cpMin) + 1) * sizeof(WCHAR));

	{
		WCHAR *p;
		tr.lpstrText = pszText;
		SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);
		const UINT cpEdit = (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0);
		MultiByteToWideChar(cpEdit, 0, pszText, tr.chrg.cpMax - tr.chrg.cpMin, pszTextW,
							(int)NP2HeapSize(pszTextW) / sizeof(WCHAR));

		for (p = pszTextW; *p && cch < COUNTOF(tch) - 1; p++) {
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
	}

	if (cch == 1) {
		lstrcpy(tch, L" ... ");
	}

	if (cch > cchExcerpt) {
		tch[cchExcerpt - 2] = L'.';
		tch[cchExcerpt - 3] = L'.';
		tch[cchExcerpt - 4] = L'.';
	}
	StrCpyN(lpszExcerpt, tch, cchExcerpt);

	NP2HeapFree(pszText);
	NP2HeapFree(pszTextW);
}

void EditSelectWord(HWND hwnd) {
	int iSel = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0) -
			   (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);

	int iPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);

	if (iSel == 0) {
		int iWordStart  = (int)SendMessage(hwnd, SCI_WORDSTARTPOSITION, iPos, TRUE);
		int iWordEnd	= (int)SendMessage(hwnd, SCI_WORDENDPOSITION, iPos, TRUE);

		if (iWordStart == iWordEnd) {// we are in whitespace salad...
			iWordStart	= (int)SendMessage(hwnd, SCI_WORDENDPOSITION, iPos, FALSE);
			iWordEnd	= (int)SendMessage(hwnd, SCI_WORDENDPOSITION, iWordStart, TRUE);
			if (iWordStart != iWordEnd) {
				//if (SCLEX_HTML == SendMessage(hwnd, SCI_GETLEXER, 0, 0) &&
				//		SCE_HPHP_VARIABLE == SendMessage(hwnd, SCI_GETSTYLEAT, (WPARAM)iWordStart, 0) &&
				//		'$' == (char)SendMessage(hwnd, SCI_GETCHARAT, (WPARAM)iWordStart-1, 0))
				//	iWordStart--;
				SendMessage(hwnd, SCI_SETSEL, iWordStart, iWordEnd);
			}
		} else {
			//if (SCLEX_HTML == SendMessage(hwnd, SCI_GETLEXER, 0, 0) &&
			//		SCE_HPHP_VARIABLE == SendMessage(hwnd, SCI_GETSTYLEAT, (WPARAM)iWordStart, 0) &&
			//		'$' == (char)SendMessage(hwnd, SCI_GETCHARAT, (WPARAM)iWordStart-1, 0))
			//	iWordStart--;
			SendMessage(hwnd, SCI_SETSEL, iWordStart, iWordEnd);
		}

		iSel =	(int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0) -
				(int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);

		if (iSel == 0) {
			int iLine		= (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, iPos, 0);
			int iLineStart	= (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, iLine, 0);
			int iLineEnd	= (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, iLine, 0);
			SendMessage(hwnd, SCI_SETSEL, iLineStart, iLineEnd);
		}
	} else {
		int iLine		= (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, iPos, 0);
		int iLineStart	= (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, iLine, 0);
		int iLineEnd	= (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, iLine, 0);
		SendMessage(hwnd, SCI_SETSEL, iLineStart, iLineEnd);
	}
}

void EditSelectLine(HWND hwnd) {
	int iSelStart	= (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	int iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	int iLineStart	= (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, iSelStart, 0);
	int iLineEnd	= (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, iSelEnd, 0);
	iSelStart		= (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, iLineStart, 0);
	iSelEnd			= (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, iLineEnd + 1, 0);
	SendMessage(hwnd, SCI_SETSEL, iSelStart, iSelEnd);
	SendMessage(hwnd, SCI_CHOOSECARETX, 0, 0);
}

//=============================================================================
//
// EditFindReplaceDlgProcW()
//
INT_PTR CALLBACK EditFindReplaceDlgProcW(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	LPEDITFINDREPLACE lpefr;
	WCHAR tch[NP2_FIND_REPLACE_LIMIT + 32];
	BOOL bCloseDlg;
	BOOL bIsFindDlg;

	static UINT uCPEdit;

	switch (umsg) {
	case WM_INITDIALOG: {
#ifdef BOOKMARK_EDITION
		static BOOL bFirstTime = TRUE;
#endif

		//WCHAR tch[128];
		HMENU hmenu;

		SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
		lpefr = (LPEDITFINDREPLACE)lParam;

		// Get the current code page for Unicode conversion
		uCPEdit = (UINT)SendMessage(lpefr->hwnd, SCI_GETCODEPAGE, 0, 0);

		// Load MRUs
		for (int i = 0; i < MRU_GetCount(mruFind); i++) {
			MRU_Enum(mruFind, i, tch, COUNTOF(tch));
			SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_ADDSTRING, 0, (LPARAM)tch);
		}

		for (int i = 0; i < MRU_GetCount(mruReplace); i++) {
			MRU_Enum(mruReplace, i, tch, COUNTOF(tch));
			SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_ADDSTRING, 0, (LPARAM)tch);
		}

		if (!bSwitchedFindReplace) {
			int cchSelection = (int)SendMessage(lpefr->hwnd, SCI_GETSELECTIONEND, 0, 0)
							   - (int)SendMessage(lpefr->hwnd, SCI_GETSELECTIONSTART, 0, 0);
			if (cchSelection <= NP2_FIND_REPLACE_LIMIT) {
				//char *lpsz;
				char *lpszEscSel;
				char *lpszSelection;
				cchSelection = (int)SendMessage(lpefr->hwnd, SCI_GETSELTEXT, 0, 0);
				lpszSelection = NP2HeapAlloc(cchSelection);
				SendMessage(lpefr->hwnd, SCI_GETSELTEXT, 0, (LPARAM)lpszSelection);

#ifdef BOOKMARK_EDITION
				// First time you bring up find/replace dialog, copy content from clipboard to find box (but only if nothing is selected in the editor)
				if (StrIsEmptyA(lpszSelection) && bFirstTime) {
					char *pClip = EditGetClipboardText(hwndEdit);
					const int len = lstrlenA(pClip);
					if (len > 0 && len <= NP2_FIND_REPLACE_LIMIT) {
						NP2HeapFree(lpszSelection);
						lpszSelection = NP2HeapAlloc(len + 2);
						lstrcpynA(lpszSelection, pClip, NP2_FIND_REPLACE_LIMIT);
					}
					LocalFree(pClip);
				}
				bFirstTime = FALSE;
#endif

				// Check lpszSelection and truncate bad chars
				//lpsz = StrPBrkA(lpszSelection, "\r\n\t");
				//if (lpsz) {
				//	*lpsz = '\0';
				//}
				lpszEscSel = NP2HeapAlloc((2 * NP2_FIND_REPLACE_LIMIT));
				lpefr->bTransformBS = AddBackslash(lpszEscSel, lpszSelection);

				SetDlgItemTextA2W(uCPEdit, hwnd, IDC_FINDTEXT, lpszEscSel);
				NP2HeapFree(lpszSelection);
				NP2HeapFree(lpszEscSel);
			}
		}

		SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_LIMITTEXT, NP2_FIND_REPLACE_LIMIT, 0);
		SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_SETEXTENDEDUI, TRUE, 0);

		if (!GetWindowTextLengthW(GetDlgItem(hwnd, IDC_FINDTEXT))) {
			SetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8);
		}

		if (GetDlgItem(hwnd, IDC_REPLACETEXT)) {
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

#ifdef BOOKMARK_EDITION
		if (lpefr->bWildcardSearch) {
			CheckDlgButton(hwnd, IDC_WILDCARDSEARCH, BST_CHECKED);
			CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
		}
#endif

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

		hmenu = GetSystemMenu(hwnd, FALSE);
		GetString(SC_SAVEPOS, tch, COUNTOF(tch));
		InsertMenu(hmenu, 0, MF_BYPOSITION | MF_STRING | MF_ENABLED, SC_SAVEPOS, tch);
		GetString(SC_RESETPOS, tch, COUNTOF(tch));
		InsertMenu(hmenu, 1, MF_BYPOSITION | MF_STRING | MF_ENABLED, SC_RESETPOS, tch);
		InsertMenu(hmenu, 2, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FINDTEXT:
		case IDC_REPLACETEXT: {
			BOOL bEnable = (GetWindowTextLengthW(GetDlgItem(hwnd, IDC_FINDTEXT)) ||
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
#ifdef BOOKMARK_EDITION
				// Can not use wildcard search together with regexp
				CheckDlgButton(hwnd, IDC_WILDCARDSEARCH, BST_UNCHECKED);
#endif
			}
			break;

		case IDC_FINDTRANSFORMBS:
			if (IsButtonChecked(hwnd, IDC_FINDTRANSFORMBS)) {
				CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			}
			break;

#ifdef BOOKMARK_EDITION
		// handle wildcard search checkbox

		case IDC_WILDCARDSEARCH:
			CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			//if (IsButtonChecked(hwnd, IDC_FINDWILDCARDS))
			//	CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			break;
#endif

		case IDOK:
		case IDC_FINDPREV:
		case IDC_REPLACE:
		case IDC_REPLACEALL:
		case IDC_REPLACEINSEL:
		case IDACC_SELTONEXT:
		case IDACC_SELTOPREV:
		case IDMSG_SWITCHTOFIND:
		case IDMSG_SWITCHTOREPLACE:
			lpefr = (LPEDITFINDREPLACE)GetWindowLongPtr(hwnd, DWLP_USER);

			bIsFindDlg = (GetDlgItem(hwnd, IDC_REPLACE) == NULL);

			if ((bIsFindDlg && LOWORD(wParam) == IDMSG_SWITCHTOREPLACE) ||
					(!bIsFindDlg && LOWORD(wParam) == IDMSG_SWITCHTOFIND)) {
				GetDlgPos(hwnd, &xFindReplaceDlgSave, &yFindReplaceDlgSave);
				bSwitchedFindReplace = TRUE;
				CopyMemory(&efrSave, lpefr, sizeof(EDITFINDREPLACE));
			}

			// Get current code page for Unicode conversion
			uCPEdit = (UINT)SendMessage(lpefr->hwnd, SCI_GETCODEPAGE, 0, 0);
			cpLastFind = uCPEdit;

			if (!bSwitchedFindReplace &&
					!GetDlgItemTextA2W(uCPEdit, hwnd, IDC_FINDTEXT, lpefr->szFind, COUNTOF(lpefr->szFind))) {
				EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_FINDPREV), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_REPLACE), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_REPLACEALL), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_REPLACEINSEL), FALSE);
				return TRUE;
			}

			if (GetDlgItem(hwnd, IDC_REPLACETEXT)) {
				GetDlgItemTextA2W(uCPEdit, hwnd, IDC_REPLACETEXT, lpefr->szReplace, COUNTOF(lpefr->szReplace));
			}

#ifdef BOOKMARK_EDITION
			lpefr->bWildcardSearch = IsButtonChecked(hwnd, IDC_WILDCARDSEARCH);
#endif

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
						MRU_Add(mruFind, tch);
					}
				}
				if (StrNotEmptyA(lpefr->szReplace)) {
					if (GetDlgItemTextA2W(CP_UTF8, hwnd, IDC_REPLACETEXT, lpefr->szReplaceUTF8, COUNTOF(lpefr->szReplaceUTF8))) {
						GetDlgItemText(hwnd, IDC_REPLACETEXT, tch, COUNTOF(tch));
						MRU_Add(mruReplace, tch);
					}
				} else {
					lstrcpyA(lpefr->szReplaceUTF8, "");
				}
			} else {
				GetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8, COUNTOF(lpefr->szFindUTF8));
				if (!GetDlgItemTextA2W(CP_UTF8, hwnd, IDC_REPLACETEXT, lpefr->szReplaceUTF8, COUNTOF(lpefr->szReplaceUTF8))) {
					lstrcpyA(lpefr->szReplaceUTF8, "");
				}
			}

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
				EditFindNext(lpefr->hwnd, lpefr, LOWORD(wParam) == IDACC_SELTONEXT || HIBYTE(GetKeyState(VK_SHIFT)));
				break;

			case IDC_FINDPREV: // find previous
			case IDACC_SELTOPREV:
				if (!bIsFindDlg) {
					bReplaceInitialized = TRUE;
				}
				EditFindPrev(lpefr->hwnd, lpefr, LOWORD(wParam) == IDACC_SELTOPREV || HIBYTE(GetKeyState(VK_SHIFT)));
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

#ifdef BOOKMARK_EDITION
			// Wildcard search will enable regexp, so I turn it off again otherwise it will be on in the gui
			if (lpefr->bWildcardSearch	&&	(lpefr->fuFlags & SCFIND_REGEXP)) {
				lpefr->fuFlags ^= SCFIND_REGEXP;
			}
#endif

			break;

		case IDCANCEL:
			//EndDialog(hwnd, IDCANCEL);
			DestroyWindow(hwnd);
			break;

		case IDACC_FIND:
			PostMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_EDIT_FIND, 1), 0);
			break;

		case IDACC_REPLACE:
			PostMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_EDIT_REPLACE, 1), 0);
			break;

		case IDACC_SAVEPOS:
			GetDlgPos(hwnd, &xFindReplaceDlg, &yFindReplaceDlg);
			break;

		case IDACC_RESETPOS:
			CenterDlgInParent(hwnd);
			xFindReplaceDlg = yFindReplaceDlg = 0;
			break;

		case IDACC_FINDNEXT:
			PostMessage(hwnd, WM_COMMAND, MAKELONG(IDOK, 1), 0);
			break;

		case IDACC_FINDPREV:
			PostMessage(hwnd, WM_COMMAND, MAKELONG(IDC_FINDPREV, 1), 0);
			break;

		case IDACC_REPLACENEXT:
			if (GetDlgItem(hwnd, IDC_REPLACE) != NULL) {
				PostMessage(hwnd, WM_COMMAND, MAKELONG(IDC_REPLACE, 1), 0);
			}
			break;

		case IDACC_SAVEFIND:
			SendMessage(hwndMain, WM_COMMAND, MAKELONG(IDM_EDIT_SAVEFIND, 1), 0);
			lpefr = (LPEDITFINDREPLACE)GetWindowLongPtr(hwnd, DWLP_USER);
			SetDlgItemTextA2W(CP_UTF8, hwnd, IDC_FINDTEXT, lpefr->szFindUTF8);
			CheckDlgButton(hwnd, IDC_FINDREGEXP, BST_UNCHECKED);
			CheckDlgButton(hwnd, IDC_FINDTRANSFORMBS, BST_UNCHECKED);
			PostMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)(GetDlgItem(hwnd, IDC_FINDTEXT)), 1);
			break;
		}
		return TRUE;

	case WM_SYSCOMMAND:
		if (wParam == SC_SAVEPOS) {
			PostMessage(hwnd, WM_COMMAND, MAKELONG(IDACC_SAVEPOS, 0), 0);
			return TRUE;
		} else if (wParam == SC_RESETPOS) {
			PostMessage(hwnd, WM_COMMAND, MAKELONG(IDACC_RESETPOS, 0), 0);
			return TRUE;
		}
		return FALSE;

	case WM_NOTIFY: {
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		switch (pnmhdr->code) {

		case NM_CLICK:
		case NM_RETURN:
			if (pnmhdr->idFrom == IDC_TOGGLEFINDREPLACE) {
				if (GetDlgItem(hwnd, IDC_REPLACE)) {
					PostMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_EDIT_FIND, 1), 0);
				} else {
					PostMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_EDIT_REPLACE, 1), 0);
				}
			}
#ifdef BOOKMARK_EDITION
			// Display help messages in the find/replace windows
			else if (pnmhdr->idFrom == IDC_BACKSLASHHELP) {
				MsgBox(MBINFO, IDS_BACKSLASHHELP);
			} else if (pnmhdr->idFrom == IDC_REGEXPHELP) {
				MsgBox(MBINFO, IDS_REGEXPHELP);
			} else if (pnmhdr->idFrom == IDC_WILDCARDHELP) {
				MsgBox(MBINFO, IDS_WILDCARDHELP);
			}
#endif
			else if (pnmhdr->idFrom == IDC_CLEAR_FIND) {
				GetDlgItemText(hwnd, IDC_FINDTEXT, tch, COUNTOF(tch));
				SendDlgItemMessage(hwnd, IDC_FINDTEXT, CB_RESETCONTENT, 0, 0);
				MRU_Empty(mruFind);
				MRU_Save(mruFind);
				SetDlgItemText(hwnd, IDC_FINDTEXT, tch);
			} else if (pnmhdr->idFrom == IDC_CLEAR_REPLACE) {
				GetDlgItemText(hwnd, IDC_REPLACETEXT, tch, COUNTOF(tch));
				SendDlgItemMessage(hwnd, IDC_REPLACETEXT, CB_RESETCONTENT, 0, 0);
				MRU_Empty(mruReplace);
				MRU_Save(mruReplace);
				SetDlgItemText(hwnd, IDC_REPLACETEXT, tch);
			}
			break;
		}
	}
	break;

	case WM_ACTIVATE :
		SetWindowTransparentMode(hwnd, LOWORD(wParam) == WA_INACTIVE);
		break;
	}

	return FALSE;
}

//=============================================================================
//
// EditFindReplaceDlg()
//
HWND EditFindReplaceDlg(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL bReplace) {
	HWND hDlg;
	lpefr->hwnd = hwnd;

	hDlg = CreateThemedDialogParam(g_hInstance,
								   (bReplace) ? MAKEINTRESOURCEW(IDD_REPLACE) : MAKEINTRESOURCEW(IDD_FIND),
								   GetParent(hwnd),
								   EditFindReplaceDlgProcW,
								   (LPARAM) lpefr);

	ShowWindow(hDlg, SW_SHOW);
	return hDlg;
}

#ifdef BOOKMARK_EDITION
// Wildcard search uses the regexp engine to perform a simple search with * ? as wildcards instead of more advanced and user-unfriendly regexp syntax
void EscapeWildcards(char *szFind2, LPCEDITFINDREPLACE lpefr) {
	char szWildcardEscaped[NP2_FIND_REPLACE_LIMIT];
	int iSource = 0;
	int iDest = 0;

	lpefr->fuFlags |= SCFIND_REGEXP;

	while (szFind2[iSource]) {
		char c = szFind2[iSource];
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
	lstrcpynA(szFind2, szWildcardEscaped, COUNTOF(szWildcardEscaped));
}
#endif

//=============================================================================
//
// EditFindNext()
//
BOOL EditFindNext(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL fExtendSelection) {
	struct Sci_TextToFind ttf;
	int iPos;
	int iSelPos, iSelAnchor;
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	BOOL bSuppressNotFound = FALSE;

	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, FALSE)*/FALSE;
	}

	lstrcpynA(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, (lpefr->fuFlags & SCFIND_REGEXP),
							 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

#ifdef BOOKMARK_EDITION
	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}
#endif

	iSelPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iSelAnchor = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	ZeroMemory(&ttf, sizeof(ttf));

	ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	ttf.chrg.cpMax = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	ttf.lpstrText = szFind2;

	iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);

	if (iPos == -1 && ttf.chrg.cpMin > 0 && !lpefr->bNoFindWrap && !fExtendSelection) {
		if (!lpefr->bNoFindWrap || (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap1", IDS_FIND_WRAPFW))) {
			ttf.chrg.cpMin = 0;
			iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);
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
		EditSelectEx(hwnd, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
	} else {
		EditSelectEx(hwnd, min_i(iSelAnchor, iSelPos), ttf.chrgText.cpMax);
	}

	return TRUE;
}

//=============================================================================
//
// EditFindPrev()
//
BOOL EditFindPrev(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL fExtendSelection) {
	struct Sci_TextToFind ttf;
	int iPos;
	int iSelPos, iSelAnchor;
	int iLength;
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	BOOL bSuppressNotFound = FALSE;

	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, FALSE)*/FALSE;
	}

	lstrcpynA(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, (lpefr->fuFlags & SCFIND_REGEXP),
							 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

#ifdef BOOKMARK_EDITION
	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}
#endif

	iSelPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	iSelAnchor = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);

	ZeroMemory(&ttf, sizeof(ttf));

	ttf.chrg.cpMin = max_i(0, (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0));
	ttf.chrg.cpMax = 0;
	ttf.lpstrText = szFind2;

	iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);
	iLength = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);

	if (iPos == -1 && ttf.chrg.cpMin < iLength && !lpefr->bNoFindWrap && !fExtendSelection) {
		if (!lpefr->bNoFindWrap || (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap2", IDS_FIND_WRAPRE))) {
			ttf.chrg.cpMin = iLength;
			iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);
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
		EditSelectEx(hwnd, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
	} else {
		EditSelectEx(hwnd, max_i(iSelPos, iSelAnchor), ttf.chrgText.cpMin);
	}

	return TRUE;
}

//=============================================================================
//
// EditReplace()
//
BOOL EditReplace(HWND hwnd, LPCEDITFINDREPLACE lpefr) {
	struct Sci_TextToFind ttf;
	int iPos;
	int iSelStart;
	int iSelEnd;
	int iReplaceMsg = (lpefr->fuFlags & SCFIND_REGEXP) ? SCI_REPLACETARGETRE : SCI_REPLACETARGET;
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	char *pszReplace2;
	BOOL bSuppressNotFound = FALSE;

	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, TRUE)*/FALSE;
	}

	lstrcpynA(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, (lpefr->fuFlags & SCFIND_REGEXP),
							 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

#ifdef BOOKMARK_EDITION
	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}
#endif

	if (strcmp(lpefr->szReplace, "^c") == 0) {
		iReplaceMsg = SCI_REPLACETARGET;
		pszReplace2 = EditGetClipboardText(hwnd);
	} else {
		//lstrcpyA(szReplace2, lpefr->szReplace);
		pszReplace2 = StrDupA(lpefr->szReplace);
		if (lpefr->bTransformBS) {
			TransformBackslashes(pszReplace2, (lpefr->fuFlags & SCFIND_REGEXP),
								 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
		}
	}

	if (!pszReplace2) {
		pszReplace2 = StrDupA("");
	}

	iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	iSelEnd		= (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);

	ZeroMemory(&ttf, sizeof(ttf));

	ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0); // Start!
	ttf.chrg.cpMax = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	ttf.lpstrText = szFind2;

	iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);

	if (iPos == -1 && ttf.chrg.cpMin > 0 && !lpefr->bNoFindWrap) {
		if (!lpefr->bNoFindWrap || (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap1", IDS_FIND_WRAPFW))) {
			ttf.chrg.cpMin = 0;
			iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);
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
		EditSelectEx(hwnd, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
		return FALSE;
	}

	SendMessage(hwnd, SCI_SETTARGETSTART, ttf.chrgText.cpMin, 0);
	SendMessage(hwnd, SCI_SETTARGETEND, ttf.chrgText.cpMax, 0);
	SendMessage(hwnd, iReplaceMsg, (WPARAM) - 1, (LPARAM)pszReplace2);

	ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
	ttf.chrg.cpMax = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);

	iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);

	bSuppressNotFound = FALSE;

	if (iPos == -1 && ttf.chrg.cpMin > 0 && !lpefr->bNoFindWrap) {
		if (!lpefr->bNoFindWrap || (IDOK == InfoBox(MBOKCANCEL, L"MsgFindWrap1", IDS_FIND_WRAPFW))) {
			ttf.chrg.cpMin = 0;
			iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf);
		} else {
			bSuppressNotFound = TRUE;
		}
	}

	if (iPos != -1) {
		EditSelectEx(hwnd, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
	} else {
		EditSelectEx(hwnd,
					 (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0),
					 (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0));
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

extern 	int iMatchesCount;

void EditMarkAll(HWND hwnd, int iMarkOccurrences, BOOL bMarkOccurrencesMatchCase, BOOL bMarkOccurrencesMatchWords) {
	struct Sci_TextToFind ttf;
	int iPos;
	char *pszText;
	int iTextLen;
	int iSelStart;
	int iSelEnd;
	int iSelCount;
	int findFlag;

	iMatchesCount = 0;

	// feature is off
	if (!iMarkOccurrences) {
		return;
	}

	iTextLen = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);

	// get current selection
	iSelStart = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	iSelEnd = (int)SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0);
	iSelCount = iSelEnd - iSelStart;

	// clear existing indicator
	SendMessage(hwnd, SCI_SETINDICATORCURRENT, 1, 0);
	SendMessage(hwnd, SCI_INDICATORCLEARRANGE, 0, iTextLen);

	// if nothing selected or multiple lines are selected exit
	if (iSelCount == 0 ||
			(int)SendMessage(hwnd, SCI_LINEFROMPOSITION, iSelStart, 0) !=
			(int)SendMessage(hwnd, SCI_LINEFROMPOSITION, iSelEnd, 0)) {
		return;
	}

	// scintilla/src/Editor.h SelectionText.LengthWithTerminator()
	iSelCount = (int)SendMessage(hwnd, SCI_GETSELTEXT, 0, 0) - 1;
	pszText = NP2HeapAlloc(iSelCount + 1);
	SendMessage(hwnd, SCI_GETSELTEXT, 0, (LPARAM)pszText);

	// exit if selection is not a word and Match whole words only is enabled
	if (bMarkOccurrencesMatchWords) {
		iSelStart = 0;
		while (iSelStart < iSelCount && pszText[iSelStart]) {
			if (IsDocWordChar(pszText[iSelStart]) || pszText[iSelStart] == '-') {
				iSelStart++;
			} else {
				NP2HeapFree(pszText);
				return;
			}
		}
	}

	ZeroMemory(&ttf, sizeof(ttf));

	ttf.chrg.cpMin = 0;
	ttf.chrg.cpMax = iTextLen;
	ttf.lpstrText = pszText;

	// set style
	const COLORREF fore = 0xff << ((iMarkOccurrences - 1) << 3);
	SendMessage(hwnd, SCI_INDICSETALPHA, 1, 100);
	SendMessage(hwnd, SCI_INDICSETFORE, 1, fore);
	SendMessage(hwnd, SCI_INDICSETSTYLE, 1, INDIC_ROUNDBOX);

	findFlag = (bMarkOccurrencesMatchCase ? SCFIND_MATCHCASE : 0) | (bMarkOccurrencesMatchWords ? SCFIND_WHOLEWORD : 0);
	while ((iPos = (int)SendMessage(hwnd, SCI_FINDTEXT, findFlag, (LPARAM)&ttf)) != -1) {
		// mark this match
		++iMatchesCount;
		SendMessage(hwnd, SCI_INDICATORFILLRANGE, iPos, iSelCount);
		ttf.chrg.cpMin = ttf.chrgText.cpMin + iSelCount;
		if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
			break;
		}
	}

	NP2HeapFree(pszText);
}

//=============================================================================
//
// EditReplaceAll()
//
BOOL EditReplaceAll(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL bShowInfo) {
	struct Sci_TextToFind ttf;
	int iCount = 0;
	int iReplaceMsg = (lpefr->fuFlags & SCFIND_REGEXP) ? SCI_REPLACETARGETRE : SCI_REPLACETARGET;
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	char *pszReplace2;

	BOOL bRegexStartOfLine;
	BOOL bRegexStartOrEndOfLine;

	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, TRUE)*/FALSE;
	}

	// Show wait cursor...
	BeginWaitCursor();

	lstrcpynA(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, (lpefr->fuFlags & SCFIND_REGEXP),
							 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

#ifdef BOOKMARK_EDITION
	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}
#endif

	bRegexStartOfLine = (lpefr->fuFlags & SCFIND_REGEXP) && (szFind2[0] == '^');
	bRegexStartOrEndOfLine =
		(lpefr->fuFlags & SCFIND_REGEXP) &&
		((!strcmp(szFind2, "$") || !strcmp(szFind2, "^") || !strcmp(szFind2, "^$")));

	if (strcmp(lpefr->szReplace, "^c") == 0) {
		iReplaceMsg = SCI_REPLACETARGET;
		pszReplace2 = EditGetClipboardText(hwnd);
	} else {
		//lstrcpyA(szReplace2, lpefr->szReplace);
		pszReplace2 = StrDupA(lpefr->szReplace);
		if (lpefr->bTransformBS) {
			TransformBackslashes(pszReplace2, (lpefr->fuFlags & SCFIND_REGEXP),
								 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
		}
	}

	if (!pszReplace2) {
		pszReplace2 = StrDupA("");
	}

	ZeroMemory(&ttf, sizeof(ttf));

	ttf.chrg.cpMin = 0;
	ttf.chrg.cpMax = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	ttf.lpstrText = szFind2;

	while ((int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf) != -1) {
		int iReplacedLen;
		//char ch;

		if (iCount == 0 && bRegexStartOrEndOfLine) {
			if (0 == SendMessage(hwnd, SCI_GETLINEENDPOSITION, 0, 0)) {
				ttf.chrgText.cpMin = 0;
				ttf.chrgText.cpMax = 0;
			}
		}

		if (++iCount == 1) {
			SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
		}

		SendMessage(hwnd, SCI_SETTARGETSTART, ttf.chrgText.cpMin, 0);
		SendMessage(hwnd, SCI_SETTARGETEND, ttf.chrgText.cpMax, 0);
		iReplacedLen = (int)SendMessage(hwnd, iReplaceMsg, (WPARAM) - 1, (LPARAM)pszReplace2);

		ttf.chrg.cpMin = ttf.chrgText.cpMin + iReplacedLen;
		ttf.chrg.cpMax = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);

		if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
			break;
		}

		//ch = (char)SendMessage(hwnd, SCI_GETCHARAT, SendMessage(hwnd, SCI_GETTARGETEND, 0, 0), 0);
		if (/*ch == '\r' || ch == '\n' || iReplacedLen == 0 || */
			ttf.chrgText.cpMin == ttf.chrgText.cpMax &&
			!(bRegexStartOrEndOfLine && iReplacedLen > 0)) {
			ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_POSITIONAFTER, ttf.chrg.cpMin, 0);
		}

		if (bRegexStartOfLine) {
			int iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)ttf.chrg.cpMin, 0);
			int ilPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);

			if (ilPos == ttf.chrg.cpMin) {
				ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine + 1, 0);
			}
			if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
				break;
			}
		}
	}

	if (iCount) {
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	}

	// Remove wait cursor
	EndWaitCursor();

	if (bShowInfo) {
		if (iCount > 0) {
			InfoBox(0, L"MsgReplaceCount", IDS_REPLCOUNT, iCount);
		} else {
			InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		}
	}

	LocalFree(pszReplace2);

	return TRUE;
}

//=============================================================================
//
// EditReplaceAllInSelection()
//
BOOL EditReplaceAllInSelection(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL bShowInfo) {
	struct Sci_TextToFind ttf;
	int iCount = 0;
	int iReplaceMsg = (lpefr->fuFlags & SCFIND_REGEXP) ? SCI_REPLACETARGETRE : SCI_REPLACETARGET;
	BOOL fCancel = FALSE;
	char szFind2[NP2_FIND_REPLACE_LIMIT];
	char *pszReplace2;
	BOOL bRegexStartOfLine;
	BOOL bRegexStartOrEndOfLine;

	if (SC_SEL_RECTANGLE == SendMessage(hwnd, SCI_GETSELECTIONMODE, 0, 0)) {
		MsgBox(MBWARN, IDS_SELRECT);
		return FALSE;
	}

	if (StrIsEmptyA(lpefr->szFind)) {
		return /*EditFindReplaceDlg(hwnd, lpefr, TRUE)*/FALSE;
	}

	// Show wait cursor...
	BeginWaitCursor();

	lstrcpynA(szFind2, lpefr->szFind, COUNTOF(szFind2));
	if (lpefr->bTransformBS) {
		TransformBackslashes(szFind2, (lpefr->fuFlags & SCFIND_REGEXP),
							 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
	}

	if (StrIsEmptyA(szFind2)) {
		InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		return FALSE;
	}

#ifdef BOOKMARK_EDITION
	if (lpefr->bWildcardSearch) {
		EscapeWildcards(szFind2, lpefr);
	}
#endif

	bRegexStartOfLine = (lpefr->fuFlags & SCFIND_REGEXP) && (szFind2[0] == '^');
	bRegexStartOrEndOfLine =
		(lpefr->fuFlags & SCFIND_REGEXP) &&
		((!strcmp(szFind2, "$") || !strcmp(szFind2, "^") || !strcmp(szFind2, "^$")));

	if (strcmp(lpefr->szReplace, "^c") == 0) {
		iReplaceMsg = SCI_REPLACETARGET;
		pszReplace2 = EditGetClipboardText(hwnd);
	} else {
		//lstrcpyA(szReplace2, lpefr->szReplace);
		pszReplace2 = StrDupA(lpefr->szReplace);
		if (lpefr->bTransformBS) {
			TransformBackslashes(pszReplace2, (lpefr->fuFlags & SCFIND_REGEXP),
								 (UINT)SendMessage(hwnd, SCI_GETCODEPAGE, 0, 0));
		}
	}

	if (!pszReplace2) {
		pszReplace2 = StrDupA("");
	}

	ZeroMemory(&ttf, sizeof(ttf));

	ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0);
	ttf.chrg.cpMax = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);
	ttf.lpstrText = szFind2;

	while ((int)SendMessage(hwnd, SCI_FINDTEXT, lpefr->fuFlags, (LPARAM)&ttf) != -1 && !fCancel) {
		if (ttf.chrgText.cpMin >= SendMessage(hwnd, SCI_GETSELECTIONSTART, 0, 0) &&
				ttf.chrgText.cpMax <= SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0)) {
			int iReplacedLen;
			//char ch;

			if (ttf.chrg.cpMin == 0 && iCount == 0 && bRegexStartOrEndOfLine) {
				if (0 == SendMessage(hwnd, SCI_GETLINEENDPOSITION, 0, 0)) {
					ttf.chrgText.cpMin = 0;
					ttf.chrgText.cpMax = 0;
				}
			}

			if (++iCount == 1) {
				SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
			}

			SendMessage(hwnd, SCI_SETTARGETSTART, ttf.chrgText.cpMin, 0);
			SendMessage(hwnd, SCI_SETTARGETEND, ttf.chrgText.cpMax, 0);
			iReplacedLen = (int)SendMessage(hwnd, iReplaceMsg, (WPARAM) - 1, (LPARAM)pszReplace2);

			ttf.chrg.cpMin = ttf.chrgText.cpMin + iReplacedLen;
			ttf.chrg.cpMax = (int)SendMessage(hwnd, SCI_GETLENGTH, 0, 0);

			if (ttf.chrg.cpMin == ttf.chrg.cpMax) {
				fCancel = TRUE;
			}

			//ch = (char)SendMessage(hwnd, SCI_GETCHARAT, SendMessage(hwnd, SCI_GETTARGETEND, 0, 0), 0);
			if (/*ch == '\r' || ch == '\n' || iReplacedLen == 0 || */
				ttf.chrgText.cpMin == ttf.chrgText.cpMax &&
				!(bRegexStartOrEndOfLine && iReplacedLen > 0)) {
				ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_POSITIONAFTER, ttf.chrg.cpMin, 0);
			}

			if (bRegexStartOfLine) {
				int iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)ttf.chrg.cpMin, 0);
				int ilPos = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine, 0);

				if (ilPos == ttf.chrg.cpMin) {
					ttf.chrg.cpMin = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iLine + 1, 0);
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
		if (SendMessage(hwnd, SCI_GETSELECTIONEND, 0, 0) <
				SendMessage(hwnd, SCI_GETTARGETEND, 0, 0)) {
			int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
			int iCurrentPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);

			if (iAnchorPos > iCurrentPos) {
				iAnchorPos = (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
			} else {
				iCurrentPos = (int)SendMessage(hwnd, SCI_GETTARGETEND, 0, 0);
			}

			EditSelectEx(hwnd, iAnchorPos, iCurrentPos);
		}

		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	}

	// Remove wait cursor
	EndWaitCursor();

	if (bShowInfo) {
		if (iCount > 0) {
			InfoBox(0, L"MsgReplaceCount", IDS_REPLCOUNT, iCount);
		} else {
			InfoBox(0, L"MsgNotFound", IDS_NOTFOUND);
		}
	}

	LocalFree(pszReplace2);

	return TRUE;
}

//=============================================================================
//
// EditLineNumDlgProc()
//
INT_PTR CALLBACK EditLineNumDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (umsg) {
	case WM_INITDIALOG: {
		int iCurLine = (int)SendMessage(hwndEdit, SCI_LINEFROMPOSITION,
										SendMessage(hwndEdit, SCI_GETCURRENTPOS, 0, 0), 0) + 1;

		SetDlgItemInt(hwnd, IDC_LINENUM, iCurLine, FALSE);
		SendDlgItemMessage(hwnd, IDC_LINENUM, EM_LIMITTEXT, 15, 0);

		SendDlgItemMessage(hwnd, IDC_COLNUM, EM_LIMITTEXT, 15, 0);

		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			BOOL fTranslated;
			BOOL fTranslated2;

			int iNewCol;

			int iNewLine = (int)GetDlgItemInt(hwnd, IDC_LINENUM, &fTranslated, FALSE);
			int iMaxLine = (int)SendMessage(hwndEdit, SCI_GETLINECOUNT, 0, 0);

			if (SendDlgItemMessage(hwnd, IDC_COLNUM, WM_GETTEXTLENGTH, 0, 0) > 0) {
				iNewCol = GetDlgItemInt(hwnd, IDC_COLNUM, &fTranslated2, FALSE);
			} else {
				iNewCol = 1;
				fTranslated2 = TRUE;
			}

			if (!fTranslated || !fTranslated2) {
				PostMessage(hwnd, WM_NEXTDLGCTL,
							(WPARAM)(GetDlgItem(hwnd, (!fTranslated) ? IDC_LINENUM : IDC_COLNUM)), 1);
				return TRUE;
			}

			if (iNewLine > 0 && iNewLine <= iMaxLine && iNewCol > 0) {
				//int iNewPos	 = SendMessage(hwndEdit, SCI_POSITIONFROMLINE, (WPARAM)iNewLine-1, 0);
				//int iLineEndPos = SendMessage(hwndEdit, SCI_GETLINEENDPOSITION, (WPARAM)iNewLine-1, 0);
				//while (iNewCol-1 > SendMessage(hwndEdit, SCI_GETCOLUMN, (WPARAM)iNewPos, 0))
				//{
				//	if (iNewPos >= iLineEndPos)
				//		break;
				//	iNewPos = SendMessage(hwndEdit, SCI_POSITIONAFTER, (WPARAM)iNewPos, 0);
				//}
				//iNewPos = min_i(iNewPos, iLineEndPos);
				//SendMessage(hwndEdit, SCI_GOTOPOS, (WPARAM)iNewPos, 0);
				//SendMessage(hwndEdit, SCI_CHOOSECARETX, 0, 0);
				EditJumpTo(hwndEdit, iNewLine, iNewCol);
				EndDialog(hwnd, IDOK);
			} else {
				PostMessage(hwnd, WM_NEXTDLGCTL,
							(WPARAM)(GetDlgItem(hwnd, (!(iNewLine > 0 && iNewLine <= iMaxLine)) ? IDC_LINENUM : IDC_COLNUM)), 1);
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
	if (IDOK == ThemedDialogBoxParam(g_hInstance,
									 MAKEINTRESOURCE(IDD_LINENUM),
									 GetParent(hwnd),
									 EditLineNumDlgProc,
									 (LPARAM)hwnd)) {
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// EditModifyLinesDlg()
//
// Controls: 100 Input
// 101 Input
//

typedef struct _modlinesdata {
	LPWSTR pwsz1;
	LPWSTR pwsz2;
} MODLINESDATA, *PMODLINESDATA;

INT_PTR CALLBACK EditModifyLinesDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static PMODLINESDATA pdata;

	static int id_hover;
	static int id_capture;

	static HFONT hFontNormal;
	static HFONT hFontHover;

	static HCURSOR hCursorNormal;
	static HCURSOR hCursorHover;

	switch (umsg) {
	case WM_INITDIALOG: {
		LOGFONT lf;

		id_hover = 0;
		id_capture = 0;

		if (NULL == (hFontNormal = (HFONT)SendDlgItemMessage(hwnd, 200, WM_GETFONT, 0, 0))) {
			hFontNormal = GetStockObject(DEFAULT_GUI_FONT);
		}

		GetObject(hFontNormal, sizeof(LOGFONT), &lf);
		lf.lfUnderline = TRUE;
		hFontHover = CreateFontIndirect(&lf);

		hCursorNormal = LoadCursor(NULL, IDC_ARROW);
		if ((hCursorHover = LoadCursor(NULL, IDC_HAND)) == NULL) {
			hCursorHover = LoadCursor(g_hInstance, IDC_ARROW);
		}

		pdata = (PMODLINESDATA)lParam;
		SetDlgItemTextW(hwnd, 100, pdata->pwsz1);
		SendDlgItemMessage(hwnd, 100, EM_LIMITTEXT, 255, 0);
		SetDlgItemTextW(hwnd, 101, pdata->pwsz2);
		SendDlgItemMessage(hwnd, 101, EM_LIMITTEXT, 255, 0);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_DESTROY:
		DeleteObject(hFontHover);
		return FALSE;

	case WM_NCACTIVATE:
		if (!(BOOL)wParam) {
			if (id_hover != 0) {
				//int _id_hover = id_hover;
				id_hover = 0;
				id_capture = 0;
				//InvalidateRect(GetDlgItem(hwnd, id_hover), NULL, FALSE);
			}
		}
		return FALSE;

	case WM_CTLCOLORSTATIC: {
		DWORD dwId = GetWindowLong((HWND)lParam, GWL_ID);
		HDC hdc = (HDC)wParam;

		if (dwId >= 200 && dwId <= 205) {
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
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		HWND hwndHover = ChildWindowFromPoint(hwnd, pt);
		DWORD dwId = GetWindowLong(hwndHover, GWL_ID);

		if (GetActiveWindow() == hwnd) {
			if (dwId >= 200 && dwId <= 205) {
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
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		HWND hwndHover = ChildWindowFromPoint(hwnd, pt);
		DWORD dwId = GetWindowLong(hwndHover, GWL_ID);

		if (dwId >= 200 && dwId <= 205) {
			GetCapture();
			id_hover = dwId;
			id_capture = dwId;
			//InvalidateRect(GetDlgItem(hwnd, dwId), NULL, FALSE);
		}
		SetCursor(id_hover != 0 ? hCursorHover : hCursorNormal);
	}
	break;

	case WM_LBUTTONUP: {
		//POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		//HWND hwndHover = ChildWindowFromPoint(hwnd, pt);
		//DWORD dwId = GetWindowLong(hwndHover, GWL_ID);

		if (id_capture != 0) {
			ReleaseCapture();
			if (id_hover == id_capture) {
				int id_focus = GetWindowLong(GetFocus(), GWL_ID);
				if (id_focus == 100 || id_focus == 101) {
					WCHAR wch[8];
					GetDlgItemText(hwnd, id_capture, wch, COUNTOF(wch));
					SendDlgItemMessage(hwnd, id_focus, EM_SETSEL, (WPARAM)0, (LPARAM) - 1);
					SendDlgItemMessage(hwnd, id_focus, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)wch);
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
			if (pdata) {
				GetDlgItemTextW(hwnd, 100, pdata->pwsz1, 256);
				GetDlgItemTextW(hwnd, 101, pdata->pwsz2, 256);
			}
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
BOOL EditModifyLinesDlg(HWND hwnd, LPWSTR pwsz1, LPWSTR pwsz2) {
	INT_PTR iResult;
	MODLINESDATA data = { pwsz1, pwsz2 };

	iResult = ThemedDialogBoxParam(g_hInstance,
								   MAKEINTRESOURCEW(IDD_MODIFYLINES),
								   hwnd,
								   EditModifyLinesDlgProc,
								   (LPARAM)&data);
	return iResult == IDOK;
}

//=============================================================================
//
// EditAlignDlgProc()
//
// Controls: 100 Radio Button
// 101 Radio Button
// 102 Radio Button
// 103 Radio Button
// 104 Radio Button
//
INT_PTR CALLBACK EditAlignDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static int *piAlignMode;
	switch (umsg) {
	case WM_INITDIALOG: {
		piAlignMode = (int *)lParam;
		CheckRadioButton(hwnd, 100, 104, *piAlignMode + 100);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			*piAlignMode = 0;
			if (IsButtonChecked(hwnd, 100)) {
				*piAlignMode = ALIGN_LEFT;
			} else if (IsButtonChecked(hwnd, 101)) {
				*piAlignMode = ALIGN_RIGHT;
			} else if (IsButtonChecked(hwnd, 102)) {
				*piAlignMode = ALIGN_CENTER;
			} else if (IsButtonChecked(hwnd, 103)) {
				*piAlignMode = ALIGN_JUSTIFY;
			} else if (IsButtonChecked(hwnd, 104)) {
				*piAlignMode = ALIGN_JUSTIFY_EX;
			}
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
	INT_PTR iResult;
	iResult = ThemedDialogBoxParam(g_hInstance,
								   MAKEINTRESOURCEW(IDD_ALIGN),
								   hwnd,
								   EditAlignDlgProc,
								   (LPARAM)piAlignMode);

	return iResult == IDOK;
}

//=============================================================================
//
// EditEncloseSelectionDlgProc()
//
// Controls: 100 Input
// 101 Input
//

typedef struct _encloseselectiondata {
	LPWSTR pwsz1;
	LPWSTR pwsz2;
} ENCLOSESELDATA, *PENCLOSESELDATA;

INT_PTR CALLBACK EditEncloseSelectionDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static PENCLOSESELDATA pdata;
	switch (umsg) {
	case WM_INITDIALOG: {
		pdata = (PENCLOSESELDATA)lParam;
		SendDlgItemMessage(hwnd, 100, EM_LIMITTEXT, 255, 0);
		SetDlgItemTextW(hwnd, 100, pdata->pwsz1);
		SendDlgItemMessage(hwnd, 101, EM_LIMITTEXT, 255, 0);
		SetDlgItemTextW(hwnd, 101, pdata->pwsz2);
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			if (pdata) {
				GetDlgItemTextW(hwnd, 100, pdata->pwsz1, 256);
				GetDlgItemTextW(hwnd, 101, pdata->pwsz2, 256);
			}
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
BOOL EditEncloseSelectionDlg(HWND hwnd, LPWSTR pwszOpen, LPWSTR pwszClose) {
	INT_PTR iResult;
	ENCLOSESELDATA data = { pwszOpen, pwszClose };
	iResult = ThemedDialogBoxParam(g_hInstance,
								   MAKEINTRESOURCEW(IDD_ENCLOSESELECTION),
								   hwnd,
								   EditEncloseSelectionDlgProc,
								   (LPARAM)&data);

	return iResult == IDOK;
}

//=============================================================================
//
// EditInsertTagDlgProc()
//
// Controls: 100 Input
// 101 Input
//

typedef struct _tagsdata {
	LPWSTR pwsz1;
	LPWSTR pwsz2;
} TAGSDATA, *PTAGSDATA;

INT_PTR CALLBACK EditInsertTagDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static PTAGSDATA pdata;
	switch (umsg) {
	case WM_INITDIALOG: {
		pdata = (PTAGSDATA)lParam;
		SendDlgItemMessage(hwnd, 100, EM_LIMITTEXT, 254, 0);
		SetDlgItemTextW(hwnd, 100, L"<tag>");

		SendDlgItemMessage(hwnd, 101, EM_LIMITTEXT, 255, 0);
		SetDlgItemTextW(hwnd, 101, L"</tag>");

		SetFocus(GetDlgItem(hwnd, 100));
		PostMessage(GetDlgItem(hwnd, 100), EM_SETSEL, 1, 4);
		CenterDlgInParent(hwnd);
	}
	return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 100: {
			if (HIWORD(wParam) == EN_CHANGE) {
				WCHAR wchBuf[256];
				BOOL bClear = TRUE;

				GetDlgItemTextW(hwnd, 100, wchBuf, 256);
				if (lstrlen(wchBuf) >= 3) {
					if (wchBuf[0] == L'<') {
						WCHAR wchIns[256] = L"</";
						int	cchIns = 2;
						const WCHAR *pwCur = wchBuf + 1;

						while (*pwCur &&
								*pwCur != L'<' &&
								*pwCur != L'>' &&
								*pwCur != L' ' &&
								*pwCur != L'\t' &&
								(StrChr(L":_-.", *pwCur) || IsCharAlphaNumericW(*pwCur))) {
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

								SetDlgItemTextW(hwnd, 101, wchIns);
								bClear = FALSE;
							}
						}
					}
				}

				if (bClear) {
					SetDlgItemTextW(hwnd, 101, L"");
				}
			}
		}
		break;

		case IDOK: {
			if (pdata) {
				GetDlgItemTextW(hwnd, 100, pdata->pwsz1, 256);
				GetDlgItemTextW(hwnd, 101, pdata->pwsz2, 256);
			}
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
BOOL EditInsertTagDlg(HWND hwnd, LPWSTR pwszOpen, LPWSTR pwszClose) {
	INT_PTR iResult;
	TAGSDATA data = { pwszOpen, pwszClose };
	iResult = ThemedDialogBoxParam(g_hInstance,
								   MAKEINTRESOURCEW(IDD_INSERTTAG),
								   hwnd,
								   EditInsertTagDlgProc,
								   (LPARAM)&data);

	return iResult == IDOK;
}

//=============================================================================
//
// EditSortDlgProc()
//
// Controls: 100-102 Radio Button
// 103-108 Check Box
//
INT_PTR CALLBACK EditSortDlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	static int *piSortFlags;
	static BOOL bEnableLogicalSort;

	switch (umsg) {
	case WM_INITDIALOG: {
		piSortFlags = (int *)lParam;
		if (*piSortFlags & SORT_DESCENDING) {
			CheckRadioButton(hwnd, 100, 102, 101);
		} else if (*piSortFlags & SORT_SHUFFLE) {
			CheckRadioButton(hwnd, 100, 102, 102);
			EnableWindow(GetDlgItem(hwnd, 103), FALSE);
			EnableWindow(GetDlgItem(hwnd, 104), FALSE);
			EnableWindow(GetDlgItem(hwnd, 105), FALSE);
			EnableWindow(GetDlgItem(hwnd, 106), FALSE);
			EnableWindow(GetDlgItem(hwnd, 107), FALSE);
		} else {
			CheckRadioButton(hwnd, 100, 102, 100);
		}

		if (*piSortFlags & SORT_MERGEDUP) {
			CheckDlgButton(hwnd, 103, BST_CHECKED);
		}

		if (*piSortFlags & SORT_UNIQDUP) {
			CheckDlgButton(hwnd, 104, BST_CHECKED);
			EnableWindow(GetDlgItem(hwnd, 103), FALSE);
		}

		if (*piSortFlags & SORT_UNIQUNIQ) {
			CheckDlgButton(hwnd, 105, BST_CHECKED);
		}

		if (*piSortFlags & SORT_NOCASE) {
			CheckDlgButton(hwnd, 106, BST_CHECKED);
		}

		if (IsWinXPAndAbove()) {
			if (*piSortFlags & SORT_LOGICAL) {
				CheckDlgButton(hwnd, 107, BST_CHECKED);
			}
			bEnableLogicalSort = TRUE;
		} else {
			EnableWindow(GetDlgItem(hwnd, 107), FALSE);
			bEnableLogicalSort = FALSE;
		}

		if (SC_SEL_RECTANGLE != SendMessage(hwndEdit, SCI_GETSELECTIONMODE, 0, 0)) {
			*piSortFlags &= ~SORT_COLUMN;
			EnableWindow(GetDlgItem(hwnd, 108), FALSE);
		} else {
			*piSortFlags |= SORT_COLUMN;
			CheckDlgButton(hwnd, 108, BST_CHECKED);
		}
		CenterDlgInParent(hwnd);
	}
	return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			*piSortFlags = 0;
			if (IsButtonChecked(hwnd, 101)) {
				*piSortFlags |= SORT_DESCENDING;
			}
			if (IsButtonChecked(hwnd, 102)) {
				*piSortFlags |= SORT_SHUFFLE;
			}
			if (IsButtonChecked(hwnd, 103)) {
				*piSortFlags |= SORT_MERGEDUP;
			}
			if (IsButtonChecked(hwnd, 104)) {
				*piSortFlags |= SORT_UNIQDUP;
			}
			if (IsButtonChecked(hwnd, 105)) {
				*piSortFlags |= SORT_UNIQUNIQ;
			}
			if (IsButtonChecked(hwnd, 106)) {
				*piSortFlags |= SORT_NOCASE;
			}
			if (IsButtonChecked(hwnd, 107)) {
				*piSortFlags |= SORT_LOGICAL;
			}
			if (IsButtonChecked(hwnd, 108)) {
				*piSortFlags |= SORT_COLUMN;
			}
			EndDialog(hwnd, IDOK);
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case 100:
		case 101:
			EnableWindow(GetDlgItem(hwnd, 103), !IsButtonChecked(hwnd, 105));
			EnableWindow(GetDlgItem(hwnd, 104), TRUE);
			EnableWindow(GetDlgItem(hwnd, 105), TRUE);
			EnableWindow(GetDlgItem(hwnd, 106), TRUE);
			EnableWindow(GetDlgItem(hwnd, 107), bEnableLogicalSort);
			break;

		case 102:
			EnableWindow(GetDlgItem(hwnd, 103), FALSE);
			EnableWindow(GetDlgItem(hwnd, 104), FALSE);
			EnableWindow(GetDlgItem(hwnd, 105), FALSE);
			EnableWindow(GetDlgItem(hwnd, 106), FALSE);
			EnableWindow(GetDlgItem(hwnd, 107), FALSE);
			break;

		case 104:
			EnableWindow(GetDlgItem(hwnd, 103), !IsButtonChecked(hwnd, 104));
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
	INT_PTR iResult;
	iResult = ThemedDialogBoxParam(g_hInstance,
								   MAKEINTRESOURCEW(IDD_SORT),
								   hwnd,
								   EditSortDlgProc,
								   (LPARAM)piSortFlags);

	return iResult == IDOK;
}

//=============================================================================
//
// FileVars_Init()
//

extern BOOL bNoEncodingTags;
extern int fNoFileVariables;

BOOL FileVars_Init(LPCSTR lpData, DWORD cbData, LPFILEVARS lpfv) {
	int i;
	char tch[512];
	BOOL bDisableFileVariables = FALSE;
	BOOL utf8Sig;

	ZeroMemory(lpfv, sizeof(FILEVARS));
	if ((fNoFileVariables && bNoEncodingTags) || !lpData || !cbData) {
		return TRUE;
	}

	lstrcpynA(tch, lpData, min_i(cbData + 1, COUNTOF(tch)));
	utf8Sig = IsUTF8Signature(lpData);

	if (!fNoFileVariables) {
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
		if (FileVars_ParseStr(tch, "encoding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
			lpfv->mask |= FV_ENCODING;
		} else if (FileVars_ParseStr(tch, "charset", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
			lpfv->mask |= FV_ENCODING;
		} else if (FileVars_ParseStr(tch, "coding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
			lpfv->mask |= FV_ENCODING;
		}
	}

	if (!fNoFileVariables && !bDisableFileVariables) {
		if (FileVars_ParseStr(tch, "mode", lpfv->tchMode, COUNTOF(lpfv->tchMode))) {
			lpfv->mask |= FV_MODE;
		}
	}

	if (lpfv->mask == 0 && cbData > COUNTOF(tch)) {
		lstrcpynA(tch, lpData + cbData - COUNTOF(tch) + 1, COUNTOF(tch));
		if (!fNoFileVariables) {
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
			if (FileVars_ParseStr(tch, "encoding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
				lpfv->mask |= FV_ENCODING;
			} else if (FileVars_ParseStr(tch, "charset", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
				lpfv->mask |= FV_ENCODING;
			} else if (FileVars_ParseStr(tch, "coding", lpfv->tchEncoding, COUNTOF(lpfv->tchEncoding))) {
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

BOOL FileVars_Apply(HWND hwnd, LPFILEVARS lpfv) {
	if (lpfv->mask & FV_TABWIDTH) {
		iTabWidth = lpfv->iTabWidth;
	} else {
		iTabWidth = iTabWidthG;
	}
	SendMessage(hwnd, SCI_SETTABWIDTH, iTabWidth, 0);

	if (lpfv->mask & FV_INDENTWIDTH) {
		iIndentWidth = lpfv->iIndentWidth;
	} else if (lpfv->mask & FV_TABWIDTH) {
		iIndentWidth = 0;
	} else {
		iIndentWidth = iIndentWidthG;
	}
	SendMessage(hwnd, SCI_SETINDENT, iIndentWidth, 0);

	if (lpfv->mask & FV_TABSASSPACES) {
		bTabsAsSpaces = lpfv->bTabsAsSpaces;
	} else {
		bTabsAsSpaces = bTabsAsSpacesG;
	}
	SendMessage(hwnd, SCI_SETUSETABS, !bTabsAsSpaces, 0);

	if (lpfv->mask & FV_TABINDENTS) {
		bTabIndents = lpfv->bTabIndents;
	} else {
		bTabIndents = bTabIndentsG;
	}
	SendMessage(hwndEdit, SCI_SETTABINDENTS, bTabIndents, 0);

	if (lpfv->mask & FV_WORDWRAP) {
		fWordWrap = lpfv->fWordWrap;
	} else {
		fWordWrap = fWordWrapG;
	}

	SendMessage(hwndEdit, SCI_SETWRAPMODE, (fWordWrap? iWordWrapMode : SC_WRAP_NONE), 0);

	if (lpfv->mask & FV_LONGLINESLIMIT) {
		iLongLinesLimit = lpfv->iLongLinesLimit;
	} else {
		iLongLinesLimit = iLongLinesLimitG;
	}
	SendMessage(hwnd, SCI_SETEDGECOLUMN, iLongLinesLimit, 0);

	iWrapCol = 0;

	return TRUE;
}

//=============================================================================
//
// FileVars_ParseInt()
//
BOOL FileVars_ParseInt(LPCSTR pszData, LPCSTR pszName, int *piValue) {
	char tch[32];
	LPCSTR pvStart = pszData;

	while ((pvStart = StrStrA(pvStart, pszName)) != NULL) {
		char chPrev = (pvStart > pszData) ? *(pvStart - 1) : 0;
		if (!IsCharAlphaNumericA(chPrev) && chPrev != '-' && chPrev != '_') {
			pvStart += lstrlenA(pszName);
			while (*pvStart == ' ') {
				pvStart++;
			}
			if (*pvStart == ':' || *pvStart == '=') {
				break;
			}
		} else {
			pvStart += lstrlenA(pszName);
		}
	}

	if (pvStart) {
		char *pvEnd;

		while (*pvStart && StrChrA(":=\"' \t", *pvStart)) {
			pvStart++;
		}

		lstrcpynA(tch, pvStart, COUNTOF(tch));

		pvEnd = tch;
		while (*pvEnd && IsCharAlphaNumericA(*pvEnd)) {
			pvEnd++;
		}
		*pvEnd = 0;
		StrTrimA(tch, " \t:=\"'");

		if (StrToIntExA(tch, STIF_DEFAULT, piValue)) {
			return TRUE;
		}

		if (tch[0] == 't') {
			*piValue = 1;
			return TRUE;
		}

		if (tch[0] == 'n' || tch[0] == 'f') {
			*piValue = 0;
			return TRUE;
		}
	}

	return FALSE;
}

//=============================================================================
//
// FileVars_ParseStr()
//
BOOL FileVars_ParseStr(LPCSTR pszData, LPCSTR pszName, char *pszValue, int cchValue) {
	char tch[32];
	LPCSTR pvStart = pszData;
	BOOL bQuoted = FALSE;

	while ((pvStart = StrStrA(pvStart, pszName)) != NULL) {
		char chPrev = (pvStart > pszData) ? *(pvStart - 1) : 0;
		if (!IsCharAlphaNumericA(chPrev) && chPrev != '-' && chPrev != '_') {
			pvStart += lstrlenA(pszName);
			while (*pvStart == ' ') {
				pvStart++;
			}
			if (*pvStart == ':' || *pvStart == '=') {
				break;
			}
		} else {
			pvStart += lstrlenA(pszName);
		}
	}

	if (pvStart) {
		char *pvEnd;

		while (*pvStart && StrChrA(":=\"' \t", *pvStart)) {
			if (*pvStart == '\'' || *pvStart == '"') {
				bQuoted = TRUE;
			}
			pvStart++;
		}
		lstrcpynA(tch, pvStart, COUNTOF(tch));

		pvEnd = tch;
		while (*pvEnd && (IsCharAlphaNumericA(*pvEnd) || StrChrA("+-/_", *pvEnd) || (bQuoted && *pvEnd == ' '))) {
			pvEnd++;
		}
		*pvEnd = 0;
		StrTrimA(tch, " \t:=\"'");

		lstrcpynA(pszValue, tch, cchValue);
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
//
// SciInitThemes()
//
//WNDPROC pfnSciWndProc = NULL;
//
//FARPROC pfnOpenThemeData = NULL;
//FARPROC pfnCloseThemeData = NULL;
//FARPROC pfnDrawThemeBackground = NULL;
//FARPROC pfnGetThemeBackgroundContentRect = NULL;
//FARPROC pfnIsThemeActive = NULL;
//FARPROC pfnDrawThemeParentBackground = NULL;
//FARPROC pfnIsThemeBackgroundPartiallyTransparent = NULL;
//
//BOOL bThemesPresent = FALSE;
//extern BOOL bIsAppThemed;
//extern HMODULE hModUxTheme;
//
//void SciInitThemes(HWND hwnd) {
//	if (hModUxTheme) {
//		pfnOpenThemeData = GetProcAddress(hModUxTheme, "OpenThemeData");
//		pfnCloseThemeData = GetProcAddress(hModUxTheme, "CloseThemeData");
//		pfnDrawThemeBackground = GetProcAddress(hModUxTheme, "DrawThemeBackground");
//		pfnGetThemeBackgroundContentRect = GetProcAddress(hModUxTheme, "GetThemeBackgroundContentRect");
//		pfnIsThemeActive = GetProcAddress(hModUxTheme, "IsThemeActive");
//		pfnDrawThemeParentBackground = GetProcAddress(hModUxTheme, "DrawThemeParentBackground");
//		pfnIsThemeBackgroundPartiallyTransparent = GetProcAddress(hModUxTheme, "IsThemeBackgroundPartiallyTransparent");
//
//		pfnSciWndProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)&SciThemedWndProc);
//		bThemesPresent = TRUE;
//	}
//}

//=============================================================================
//
// SciThemedWndProc()
//
//LRESULT CALLBACK SciThemedWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
//	static RECT rcContent;
//
//	if (umsg == WM_NCCALCSIZE) {
//		if (wParam) {
//			LRESULT lresult = CallWindowProcW(pfnSciWndProc, hwnd, WM_NCCALCSIZE, wParam, lParam);
//			NCCALCSIZE_PARAMS *csp = (NCCALCSIZE_PARAMS *)lParam;
//
//			if (bThemesPresent && bIsAppThemed) {
//				HANDLE hTheme = (HANDLE)pfnOpenThemeData(hwnd, L"edit");
//				if (hTheme) {
//					BOOL bSuccess = FALSE;
//					RECT rcClient;
//
//					if (pfnGetThemeBackgroundContentRect(
//								hTheme, NULL, /*EP_EDITTEXT*/1, /*ETS_NORMAL*/1, &csp->rgrc, &rcClient) == S_OK) {
//						InflateRect(&rcClient, -1, -1);
//
//						rcContent.left = rcClient.left - csp->rgrc[0].left;
//						rcContent.top = rcClient.top - csp->rgrc[0].top;
//						rcContent.right = csp->rgrc[0].right - rcClient.right;
//						rcContent.bottom = csp->rgrc[0].bottom - rcClient.bottom;
//
//						CopyRect(&csp->rgrc, &rcClient);
//						bSuccess = TRUE;
//					}
//					pfnCloseThemeData(hTheme);
//
//					if (bSuccess) {
//						return WVR_REDRAW;
//					}
//				}
//			}
//			return lresult;
//		}
//	} else if (umsg == WM_NCPAINT) {
//		LRESULT lresult = CallWindowProcW(pfnSciWndProc, hwnd, WM_NCPAINT, wParam, lParam);
//		if (bThemesPresent && bIsAppThemed) {
//
//			HANDLE hTheme = (HANDLE)pfnOpenThemeData(hwnd, L"edit");
//			if (hTheme) {
//				RECT rcBorder;
//				RECT rcClient;
//				int nState;
//
//				HDC hdc = GetWindowDC(hwnd);
//
//				GetWindowRect(hwnd, &rcBorder);
//				OffsetRect(&rcBorder, -rcBorder.left, -rcBorder.top);
//
//				CopyRect(&rcClient, &rcBorder);
//				rcClient.left += rcContent.left;
//				rcClient.top += rcContent.top;
//				rcClient.right -= rcContent.right;
//				rcClient.bottom -= rcContent.bottom;
//
//				ExcludeClipRect(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
//
//				if (pfnIsThemeBackgroundPartiallyTransparent(hTheme, /*EP_EDITTEXT*/1, /*ETS_NORMAL*/1)) {
//					pfnDrawThemeParentBackground(hwnd, hdc, &rcBorder);
//				}
//
//				/*
//				ETS_NORMAL = 1
//				ETS_HOT = 2
//				ETS_SELECTED = 3
//				ETS_DISABLED = 4
//				ETS_FOCUSED = 5
//				ETS_READONLY = 6
//				ETS_ASSIST = 7
//				*/
//
//				if (!IsWindowEnabled(hwnd)) {
//					nState = /*ETS_DISABLED*/4;
//				} else if (GetFocus() == hwnd) {
//					nState = /*ETS_FOCUSED*/5;
//				} else if (SendMessage(hwnd, SCI_GETREADONLY, 0, 0)) {
//					nState = /*ETS_READONLY*/6;
//				} else {
//					nState = /*ETS_NORMAL*/1;
//				}
//
//				pfnDrawThemeBackground(hTheme, hdc, /*EP_EDITTEXT*/1, nState, &rcBorder, NULL);
//				pfnCloseThemeData(hTheme);
//
//				ReleaseDC(hwnd, hdc);
//				return 0;
//			}
//		}
//		return lresult;
//	}
//
//	return CallWindowProcW(pfnSciWndProc, hwnd, umsg, wParam, lParam);
//}

// End of Edit.c
