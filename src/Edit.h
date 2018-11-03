/******************************************************************************
*
*
* Notepad2
*
* Edit.h
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

#ifndef NOTEPAD2_EDIT_H_
#define NOTEPAD2_EDIT_H_

#include "Sci_Position.h"

typedef struct _editfindreplace {
	char	szFind[512];
	char	szReplace[512];
	char	szFindUTF8[3 * 512];
	char	szReplaceUTF8[3 * 512];
	UINT	fuFlags;
	BOOL	bTransformBS;
	BOOL	bObsolete /* was bFindUp */;
	BOOL	bFindClose;
	BOOL	bReplaceClose;
	BOOL	bNoFindWrap;
	HWND	hwnd;
#ifdef BOOKMARK_EDITION
	BOOL	bWildcardSearch;
#endif
	//HANDLE hMRUFind;
	//HANDLE hMRUReplace;
} EDITFINDREPLACE, *LPEDITFINDREPLACE, *LPCEDITFINDREPLACE;

#define IDMSG_SWITCHTOFIND		300
#define IDMSG_SWITCHTOREPLACE	301

#define ALIGN_LEFT			0
#define ALIGN_RIGHT			1
#define ALIGN_CENTER		2
#define ALIGN_JUSTIFY		3
#define ALIGN_JUSTIFY_EX	4

#define SORT_ASCENDING		0
#define SORT_DESCENDING		1
#define SORT_SHUFFLE		2
#define SORT_MERGEDUP		4
#define SORT_UNIQDUP		8
#define SORT_UNIQUNIQ		16
#define SORT_NOCASE			32
#define SORT_LOGICAL		64
#define SORT_COLUMN			128

HWND	EditCreate(HWND hwndParent);
void	EditSetNewText(HWND hwnd, LPCSTR lpstrText, DWORD cbText);
BOOL	EditConvertText(HWND hwnd, UINT cpSource, UINT cpDest, BOOL bSetSavePoint);
BOOL	EditSetNewEncoding(HWND hwnd, int iCurrentEncoding,
						   int iNewEncoding, BOOL bNoUI, BOOL bSetSavePoint);

char*	EditGetClipboardText(HWND hwnd); // LocalFree()
BOOL	EditCopyAppend(HWND hwnd);

struct EditFileIOStatus;
void 	EditDetectEOLMode(LPCSTR lpData, DWORD cbData, struct EditFileIOStatus *status);
BOOL	EditLoadFile(HWND hwnd, LPWSTR pszFile, BOOL bSkipEncodingDetection, struct EditFileIOStatus *status);
BOOL	EditSaveFile(HWND hwnd, LPCWSTR pszFile, BOOL bSaveCopy, struct EditFileIOStatus *status);

void	EditInvertCase(HWND hwnd);
void	EditTitleCase(HWND hwnd);
void	EditSentenceCase(HWND hwnd);

void	EditURLEncode(HWND hwnd);
void	EditURLDecode(HWND hwnd);
void	EditEscapeCChars(HWND hwnd);
void	EditUnescapeCChars(HWND hwnd);
void	EditEscapeXHTMLChars(HWND hwnd);
void	EditUnescapeXHTMLChars(HWND hwnd);
void	EditChar2Hex(HWND hwnd);
void	EditHex2Char(HWND hwnd);
void	EditShowHex(HWND hwnd);
void	EditConvertNumRadix(HWND hwnd, int radix);
void	EditModifyNumber(HWND hwnd, BOOL bIncrease);

void	EditTabsToSpaces(HWND hwnd, int nTabWidth, BOOL bOnlyIndentingWS);
void	EditSpacesToTabs(HWND hwnd, int nTabWidth, BOOL bOnlyIndentingWS);

void	EditMoveUp(HWND hwnd);
void	EditMoveDown(HWND hwnd);
void	EditModifyLines(HWND hwnd, LPCWSTR pwszPrefix, LPCWSTR pwszAppend);
void	EditAlignText(HWND hwnd, int nMode);
void	EditEncloseSelection(HWND hwnd, LPCWSTR pwszOpen, LPCWSTR pwszClose);
void	EditToggleLineComments(HWND hwnd, LPCWSTR pwszComment, BOOL bInsertAtStart);
void	EditPadWithSpaces(HWND hwnd, BOOL bSkipEmpty, BOOL bNoUndoGroup);
void	EditStripFirstCharacter(HWND hwnd);
void	EditStripLastCharacter(HWND hwnd);
void	EditStripTrailingBlanks(HWND hwnd, BOOL bIgnoreSelection);
void	EditStripLeadingBlanks(HWND hwnd, BOOL bIgnoreSelection);
void	EditCompressSpaces(HWND hwnd);
void	EditRemoveBlankLines(HWND hwnd, BOOL bMerge);
void	EditWrapToColumn(HWND hwnd, int nColumn/*, int nTabWidth*/);
void	EditJoinLinesEx(HWND hwnd);
void	EditSortLines(HWND hwnd, int iSortFlags);

void	EditJumpTo(HWND hwnd, int iNewLine, int iNewCol);
void	EditSelectEx(HWND hwnd, int iAnchorPos, int iCurrentPos);
void	EditFixPositions(HWND hwnd);
void	EditEnsureSelectionVisible(HWND hwnd);
void	EditEnsureConsistentLineEndings(HWND hwnd);
void	EditGetExcerpt(HWND hwnd, LPWSTR lpszExcerpt, DWORD cchExcerpt);

#define NP2_FIND_REPLACE_LIMIT	2048
#define NP2_LONG_LINE_LIMIT		4096

void	EditSelectWord(HWND hwnd);
void	EditSelectLine(HWND hwnd);
HWND	EditFindReplaceDlg(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL bReplace);
BOOL	EditFindNext(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL fExtendSelection);
BOOL	EditFindPrev(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL fExtendSelection);
BOOL	EditReplace(HWND hwnd, LPCEDITFINDREPLACE lpefr);
BOOL	EditReplaceAll(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL bShowInfo);
BOOL	EditReplaceAllInSelection(HWND hwnd, LPCEDITFINDREPLACE lpefr, BOOL bShowInfo);
BOOL	EditLineNumDlg(HWND hwnd);
BOOL	EditModifyLinesDlg(HWND hwnd, LPWSTR pwsz1, LPWSTR pwsz2);
BOOL	EditEncloseSelectionDlg(HWND hwnd, LPWSTR pwszOpen, LPWSTR pwszClose);
BOOL	EditInsertTagDlg(HWND hwnd, LPWSTR pwszOpen, LPWSTR pwszClose);
void	EditInsertUnicodeControlCharacter(HWND hwnd, int menu);
void	EditShowUnicodeControlCharacter(HWND hwnd, BOOL bShow);
BOOL	EditSortDlg(HWND hwnd, int *piSortFlags);
BOOL	EditAlignDlg(HWND hwnd, int *piAlignMode);
void	EditSelectionAction(HWND hwnd, int action);
void	TryBrowseFile(HWND hwnd, LPCWSTR pszFile, BOOL bWarn);
void	EditOpenSelection(HWND hwnd, int type);

// in Print.cpp
BOOL	EditPrint(HWND hwnd, LPCWSTR pszDocTitle, LPCWSTR pszPageFormat);
void	EditPrintSetup(HWND hwnd);
void	EditPrintInit(void);

void	EditMarkAll(HWND hwnd, int iMarkOccurrences,
					BOOL bMarkOccurrencesMatchCase, BOOL bMarkOccurrencesMatchWords);

// in EditAutoC.c
void	EditCompleteWord(HWND hwnd, BOOL autoInsert);
void	EditAutoCloseBraceQuote(HWND hwnd, int ch);
void	EditAutoCloseXMLTag(HWND hwnd);
void	EditAutoIndent(HWND hwnd);
void	EditToggleCommentLine(HWND hwnd);
void	EditToggleCommentBlock(HWND hwnd);
void	EditShowCallTips(HWND hwnd, Sci_Position position);

#define NCP_DEFAULT					1
#define NCP_UTF8					2
#define NCP_UTF8_SIGN				4
#define NCP_UNICODE					8
#define NCP_UNICODE_REVERSE			16
#define NCP_UNICODE_BOM				32
#define NCP_8BIT					64
#define NCP_INTERNAL				(NCP_DEFAULT|NCP_UTF8|NCP_UTF8_SIGN|NCP_UNICODE|NCP_UNICODE_REVERSE|NCP_UNICODE_BOM)
#define NCP_RECODE					128
#define CPI_NONE					(-1)
#define CPI_DEFAULT					0
#define CPI_OEM						1
#define CPI_UNICODEBOM				2
#define CPI_UNICODEBEBOM			3
#define CPI_UNICODE					4
#define CPI_UNICODEBE				5
#define CPI_UTF8					6
#define CPI_UTF8SIGN				7
#define CPI_UTF7					8

#define MAX_ENCODING_LABEL_SIZE		32

typedef struct _np2encoding {
	const UINT uFlags;
	/*const*/UINT uCodePage;
	const char * const pszParseNames;
	const int idsName;
	LPWSTR wchLabel;
} NP2ENCODING;

// 932 Shift-JIS, 936 GBK, 949 UHC, 950 Big5, 1361 Johab
static inline BOOL IsDBCSCodePage(UINT page) {
	return page == 932 || page == 936 || page == 949 || page == 950 || page == 1361;
}

// in EditEncoding.c
void	Encoding_ReleaseResources(void);
void	Encoding_InitDefaults(void);
int 	Encoding_MapIniSetting(BOOL bLoad, int iSetting);
void	Encoding_GetLabel(int iEncoding);
int 	Encoding_Match(LPCWSTR pwszTest);
int 	Encoding_MatchA(char *pchTest);
BOOL	Encoding_IsValid(int iTestEncoding);
void	Encoding_AddToListView(HWND hwnd, int idSel, BOOL bRecodeOnly);
BOOL	Encoding_GetFromListView(HWND hwnd, int *pidEncoding);
void	Encoding_AddToComboboxEx(HWND hwnd, int idSel, BOOL bRecodeOnly);
BOOL	Encoding_GetFromComboboxEx(HWND hwnd, int *pidEncoding);

BOOL	IsUnicode(const char *pBuffer, DWORD cb, LPBOOL lpbBOM, LPBOOL lpbReverse);
BOOL	IsUTF8(const char *pTest, DWORD nLength);
BOOL	IsUTF7(const char *pTest, DWORD nLength);
INT		UTF8_mbslen(LPCSTR source, INT byte_length);
INT		UTF8_mbslen_bytes(LPCSTR utf8_string);

// WideCharToMultiByte, UTF8 encoding of U+0800 to U+FFFF
#define kMaxMultiByteCount	3

static inline BOOL IsUTF8Signature(const char *p) {
	UINT value = (*((UNALIGNED UINT*)p)) & 0xFFFFFF;
	// 0xEF, 0xBB, 0xBF in little endian
	return value == 0xBFBBEF;
}

//void SciInitThemes(HWND hwnd);
//LRESULT CALLBACK SciThemedWndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

#define FV_TABWIDTH				1
#define FV_INDENTWIDTH			2
#define FV_TABSASSPACES			4
#define FV_TABINDENTS			8
#define FV_WORDWRAP				16
#define FV_LONGLINESLIMIT		32
#define FV_ENCODING				64
#define FV_MODE					128

typedef struct _filevars {
	int 	mask;
	int 	iTabWidth;
	int 	iIndentWidth;
	BOOL	bTabsAsSpaces;
	BOOL	bTabIndents;
	BOOL	fWordWrap;
	int 	iLongLinesLimit;
	char	tchEncoding[32];
	int 	iEncoding;
	char	tchMode[32];
} FILEVARS, *LPFILEVARS;

BOOL	FileVars_Init(LPCSTR lpData, DWORD cbData, LPFILEVARS lpfv);
BOOL	FileVars_Apply(HWND hwnd, LPFILEVARS lpfv);
BOOL	FileVars_ParseInt(LPCSTR pszData, LPCSTR pszName, int *piValue);
BOOL	FileVars_ParseStr(LPCSTR pszData, LPCSTR pszName, char *pszValue, int cchValue);
// in EditEncoding.c
BOOL	FileVars_IsUTF8(LPFILEVARS lpfv);
BOOL	FileVars_IsNonUTF8(LPFILEVARS lpfv);
BOOL	FileVars_IsValidEncoding(LPFILEVARS lpfv);
int 	FileVars_GetEncoding(LPFILEVARS lpfv);

#endif //NOTEPAD2_EDIT_H_

// End of Edit.h
