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
#pragma once

// WideCharToMultiByte, UTF8 encoding of U+0800 to U+FFFF
#define kMaxMultiByteCount	3

#define NP2_FIND_REPLACE_LIMIT	2048
#define NP2_LONG_LINE_LIMIT		4096

typedef struct EDITFINDREPLACE {
	char	szFind[512];
	char	szReplace[512];
	char	szFindUTF8[3 * 512];
	char	szReplaceUTF8[3 * 512];
	HWND	hwnd;
	UINT	fuFlags;
	BOOL	bTransformBS;
	//BOOL	bFindUp;
	BOOL	bFindClose;
	BOOL	bReplaceClose;
	BOOL	bNoFindWrap;
	BOOL	bWildcardSearch;
} EDITFINDREPLACE, *LPEDITFINDREPLACE;

typedef const EDITFINDREPLACE * LPCEDITFINDREPLACE;

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

// wrap indent
enum {
	EditWrapIndentNone = 0,
	EditWrapIndentOneCharacter = 1,
	EditWrapIndentTwoCharacter = 2,
	EditWrapIndentOneLevel = 3,
	EditWrapIndentTwoLevel = 4,

	EditWrapIndentSameAsSubline = 5,
	EditWrapIndentOneLevelThanSubline = 6,
	EditWrapIndentTwoLevelThanSubline = 7,

	EditWrapIndentMaxValue = EditWrapIndentTwoLevelThanSubline,
	EditWrapIndentDefaultValue = EditWrapIndentNone,
};

// wrap symbol
enum {
	EditWrapSymbolBeforeNone = 0,
	EditWrapSymbolBeforeNearText = 1,
	EditWrapSymbolBeforeNearBorder = 2,

	EditWrapSymbolAfterNone = 0,
	EditWrapSymbolAfterNearText = 1,
	EditWrapSymbolAfterNearBorder = 2,
	EditWrapSymbolLineNumberMargin = 3,

	EditWrapSymbolBeforeMaxValue = EditWrapSymbolBeforeNearBorder,
	EditWrapSymbolAfterMaxValue = EditWrapSymbolLineNumberMargin,

	EditWrapSymbolMaxValue = EditWrapSymbolBeforeMaxValue + 10 * EditWrapSymbolAfterMaxValue,
	EditWrapSymbolDefaultValue = EditWrapSymbolBeforeNearBorder,
};

void	Edit_ReleaseResources(void);
HWND	EditCreate(HWND hwndParent);
void	EditSetNewText(LPCSTR lpstrText, DWORD cbText, Sci_Line lineCount);

static inline void EditSetEmptyText(void) {
	EditSetNewText("", 0, 1);
}

BOOL	EditConvertText(UINT cpSource, UINT cpDest, BOOL bSetSavePoint);
void	EditConvertToLargeMode(void);
void	EditReplaceDocument(HANDLE pdoc);

char*	EditGetClipboardText(HWND hwnd); // LocalFree()
BOOL	EditCopyAppend(HWND hwnd);

static inline int GetScintillaEOLMode(int mode) {
	const int mask = SC_EOL_CRLF | (SC_EOL_LF << 4) | (SC_EOL_CR << 8);
	return (mask >> (mode << 2)) & 0x0f;
}

struct EditFileIOStatus;
void 	EditDetectEOLMode(LPCSTR lpData, DWORD cbData, struct EditFileIOStatus *status);
BOOL	EditLoadFile(LPWSTR pszFile, BOOL bSkipEncodingDetection, struct EditFileIOStatus *status);
BOOL	EditSaveFile(HWND hwnd, LPCWSTR pszFile, BOOL bSaveCopy, struct EditFileIOStatus *status);

void	EditInvertCase(void);
void	EditMapTextCase(int menu);
void	EditSentenceCase(void);

void	EditURLEncode(void);
void	EditURLDecode(void);
void	EditEscapeCChars(HWND hwnd);
void	EditUnescapeCChars(HWND hwnd);
void	EditEscapeXHTMLChars(HWND hwnd);
void	EditUnescapeXHTMLChars(HWND hwnd);
void	EditChar2Hex(void);
void	EditHex2Char(void);
void	EditShowHex(void);
void	EditConvertNumRadix(int radix);
void	EditModifyNumber(BOOL bIncrease);

void	EditTabsToSpaces(int nTabWidth, BOOL bOnlyIndentingWS);
void	EditSpacesToTabs(int nTabWidth, BOOL bOnlyIndentingWS);

void	EditMoveUp(void);
void	EditMoveDown(void);
void	EditModifyLines(LPCWSTR pwszPrefix, LPCWSTR pwszAppend);
void	EditAlignText(int nMode);
void	EditEncloseSelection(LPCWSTR pwszOpen, LPCWSTR pwszClose);
void	EditToggleLineComments(LPCWSTR pwszComment, BOOL bInsertAtStart);
void	EditPadWithSpaces(BOOL bSkipEmpty, BOOL bNoUndoGroup);
void	EditStripFirstCharacter(void);
void	EditStripLastCharacter(void);
void	EditStripTrailingBlanks(HWND hwnd, BOOL bIgnoreSelection);
void	EditStripLeadingBlanks(HWND hwnd, BOOL bIgnoreSelection);
void	EditCompressSpaces(void);
void	EditRemoveBlankLines(BOOL bMerge);
void	EditWrapToColumn(int nColumn/*, int nTabWidth*/);
void	EditJoinLinesEx(void);
void	EditSortLines(int iSortFlags);

void	EditJumpTo(Sci_Line iNewLine, Sci_Position iNewCol);
void	EditSelectEx(Sci_Position iAnchorPos, Sci_Position iCurrentPos);
void	EditFixPositions(void);
void	EditEnsureSelectionVisible(void);
void	EditEnsureConsistentLineEndings(void);
void	EditGetExcerpt(LPWSTR lpszExcerpt, DWORD cchExcerpt);

void	EditSelectWord(void);
void	EditSelectLines(BOOL currentBlock, BOOL lineSelection);
HWND	EditFindReplaceDlg(HWND hwnd, LPEDITFINDREPLACE lpefr, BOOL bReplace);
BOOL	EditFindNext(LPEDITFINDREPLACE lpefr, BOOL fExtendSelection);
BOOL	EditFindPrev(LPEDITFINDREPLACE lpefr, BOOL fExtendSelection);
BOOL	EditReplace(HWND hwnd, LPEDITFINDREPLACE lpefr);
BOOL	EditReplaceAll(HWND hwnd, LPEDITFINDREPLACE lpefr, BOOL bShowInfo);
BOOL	EditReplaceAllInSelection(HWND hwnd, LPEDITFINDREPLACE lpefr, BOOL bShowInfo);
BOOL	EditLineNumDlg(HWND hwnd);
void	EditModifyLinesDlg(HWND hwnd);
void	EditEncloseSelectionDlg(HWND hwnd);
void	EditInsertTagDlg(HWND hwnd);
void	EditInsertDateTime(BOOL bShort);
void	EditUpdateTimestampMatchTemplate(HWND hwnd);
void	EditInsertUnicodeControlCharacter(int menu);
void	EditShowUnicodeControlCharacter(BOOL bShow);
BOOL	EditSortDlg(HWND hwnd, int *piSortFlags);
BOOL	EditAlignDlg(HWND hwnd, int *piAlignMode);
void	EditSelectionAction(int action);
void	TryBrowseFile(HWND hwnd, LPCWSTR pszFile, BOOL bWarn);
void	EditOpenSelection(int type);

// in Print.cpp
#ifdef __cplusplus
extern "C" {
#endif

BOOL	EditPrint(HWND hwnd, LPCWSTR pszDocTitle);
void	EditPrintSetup(HWND hwnd);

#ifdef __cplusplus
}
#endif

enum {
	MarkerNumber_Bookmark = 0,

	// [0, INDICATOR_CONTAINER) are reserved for lexer.
	IndicatorNumber_MarkOccurrence = INDICATOR_CONTAINER + 0,
	IndicatorNumber_MatchBrace = INDICATOR_CONTAINER + 1,
	IndicatorNumber_MatchBraceError = INDICATOR_CONTAINER + 2,
	// [INDICATOR_IME, INDICATOR_IME_MAX] are reserved for IME.

	MarginNumber_LineNumber = 0,
	MarginNumber_Bookmark = 1,
	MarginNumber_CodeFolding = 2,

	MarkerBitmask_Bookmark = 1 << MarkerNumber_Bookmark,
};

void	EditMarkAll_Clear(void);
void	EditMarkAll(BOOL bChanged, BOOL bMarkOccurrencesMatchCase, BOOL bMarkOccurrencesMatchWords);

// auto completion fill-up characters
#define MAX_AUTO_COMPLETION_FILLUP_LENGTH	32		// Only 32 ASCII punctuation
#define AUTO_COMPLETION_FILLUP_DEFAULT		L";,()[]{}\\/"
enum {
	AutoCompleteFillUpEnter = 1,
	AutoCompleteFillUpTab = 2,
	AutoCompleteFillUpSpace = 4,
	AutoCompleteFillUpPunctuation = 8,
	// default settings
	AutoCompleteFillUpDefault = 15,
};

// auto insert
enum {
	AutoInsertParenthesis = 1,			// ()
	AutoInsertBrace = 2,				// {}
	AutoInsertSquareBracket = 4,		// []
	AutoInsertAngleBracket = 8,			// <>
	AutoInsertDoubleQuote = 16,			// ""
	AutoInsertSingleQuote = 32,			// ''
	AutoInsertBacktick = 64,			// ``
	AutoInsertSpaceAfterComma = 128,	// ', '
	// default settings
	AutoInsertDefaultMask = 255,
};

// asm line comment
enum {
	AsmLineCommentCharSemicolon = 0,	// ';'
	AsmLineCommentCharSharp = 1,		// '#'
	AsmLineCommentCharSlash = 2,		// '//'
	AsmLineCommentCharAt = 3,			// '@'
};

#define MIN_AUTO_COMPLETION_VISIBLE_ITEM_COUNT	8
#define MIN_AUTO_COMPLETION_WORD_LENGTH			1
#define MIN_AUTO_COMPLETION_NUMBER_LENGTH		0

typedef struct EditAutoCompletionConfig {
	BOOL bIndentText;
	BOOL bCloseTags;
	BOOL bCompleteWord;
	BOOL bScanWordsInDocument;
	BOOL bEnglistIMEModeOnly;
	BOOL bIgnoreCase;
	UINT iVisibleItemCount;
	int iMinWordLength;
	int iMinNumberLength;
	int fAutoCompleteFillUpMask;
	int fAutoInsertMask;
	int iAsmLineCommentChar;
	UINT iPreviousItemCount;		// status
	char szAutoCompleteFillUp[MAX_AUTO_COMPLETION_FILLUP_LENGTH + 4];
	WCHAR wszAutoCompleteFillUp[MAX_AUTO_COMPLETION_FILLUP_LENGTH];
} EditAutoCompletionConfig;

enum {
	AutoCompleteCondition_Normal = 0,
	AutoCompleteCondition_OnCharAdded = 1,
	AutoCompleteCondition_OnCharDeleted = 2,
};

// in EditAutoC.c
void	EditCompleteUpdateConfig(void);
BOOL	IsDocWordChar(int ch);
BOOL	IsAutoCompletionWordCharacter(int ch);
void	EditCompleteWord(int iCondition, BOOL autoInsert);
BOOL	EditIsOpenBraceMatched(Sci_Position pos, Sci_Position startPos);
void	EditAutoCloseBraceQuote(int ch);
void	EditAutoCloseXMLTag(void);
void	EditAutoIndent(void);
void	EditToggleCommentLine(void);
void	EditToggleCommentBlock(void);
void	EditInsertScriptShebangLine(void);
void	EditShowCallTips(Sci_Position position);

#define NCP_DEFAULT					1
#define NCP_UTF8					2
#define NCP_UTF8_SIGN				4
#define NCP_UNICODE					8
#define NCP_UNICODE_REVERSE			16
#define NCP_UNICODE_BOM				32
#define NCP_8BIT					64
#define NCP_INTERNAL				(NCP_DEFAULT | NCP_UTF8 | NCP_UTF8_SIGN | NCP_UNICODE | NCP_UNICODE_REVERSE | NCP_UNICODE_BOM)
#define NCP_RECODE					128
#define NCP_7BIT					256		// encoded in ASCII with escapes: UTF-7, ISO-2022, HZ-GB-2312
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

typedef struct NP2ENCODING {
	const UINT uFlags;
	/*const*/UINT uCodePage;
	// string format: [normal name + ','] + [lower case parse name + ',']+
	const char * const pszParseNames;
	const UINT idsName;
	LPWSTR wchLabel;
} NP2ENCODING;

// 932 Shift-JIS, 936 GBK, 949 UHC, 950 Big5, 1361 Johab
static inline BOOL IsDBCSCodePage(UINT page) {
	return page == 932 || page == 936 || page == 949 || page == 950 || page == 1361;
}

// dwFlags must be 0 for MultiByteToWideChar() and WideCharToMultiByte()
static inline BOOL IsZeroFlagsCodePage(UINT page) {
	return page == CP_UTF8
		|| page == CP_UTF7
		|| page == 54936	// GB18030
		|| page == 50220	// ISO-2022-JP
		|| page == 50221	// CS ISO-2022-JP
		|| page == 50222	// ISO-2022-JP SO/SI
		|| page == 50225	// ISO-2022-KR
		|| page == 50227	// ISO-2022-CN Simplified
		|| page == 50229	// ISO-2022-CN Traditional
		//|| (page >= 57002 && page <= 57011) // ISCII
		//|| page == 42		// Symbol
		;
}

// in EditEncoding.c
extern NP2ENCODING mEncoding[];
void	Encoding_ReleaseResources(void);
BOOL	EditSetNewEncoding(int iCurrentEncoding, int iNewEncoding, BOOL bNoUI, BOOL bSetSavePoint);
void	EditOnCodePageChanged(UINT oldCodePage);
const char* GetFoldDisplayEllipsis(UINT cpEdit, UINT acp);
void	Encoding_InitDefaults(void);
int 	Encoding_MapIniSetting(BOOL bLoad, int iSetting);
void	Encoding_GetLabel(int iEncoding);
int 	Encoding_Match(LPCWSTR pwszTest);
int 	Encoding_MatchA(LPCSTR pchTest);
BOOL	Encoding_IsValid(int iTestEncoding);
void	Encoding_AddToTreeView(HWND hwnd, int idSel, BOOL bRecodeOnly);
BOOL	Encoding_GetFromTreeView(HWND hwnd, int *pidEncoding, BOOL bQuiet);
#if 0
void	Encoding_AddToListView(HWND hwnd, int idSel, BOOL bRecodeOnly);
BOOL	Encoding_GetFromListView(HWND hwnd, int *pidEncoding);
void	Encoding_AddToComboboxEx(HWND hwnd, int idSel, BOOL bRecodeOnly);
BOOL	Encoding_GetFromComboboxEx(HWND hwnd, int *pidEncoding);
#endif

BOOL	IsUnicode(const char *pBuffer, DWORD cb, LPBOOL lpbBOM, LPBOOL lpbReverse);
BOOL	IsUTF8(const char *pTest, DWORD nLength);
BOOL	IsUTF7(const char *pTest, DWORD nLength);
//INT		UTF8_mbslen(LPCSTR source, INT byte_length);
//INT		UTF8_mbslen_bytes(LPCSTR utf8_string);

static inline BOOL IsUTF8Signature(const char *p) {
	return p[0] == '\xEF' && p[1] == '\xBB' && p[2] == '\xBF';
}

//void SciInitThemes(HWND hwnd);

#define FV_TABWIDTH				1
#define FV_INDENTWIDTH			2
#define FV_TABSASSPACES			4
#define FV_TABINDENTS			8
#define FV_WORDWRAP				16
#define FV_LONGLINESLIMIT		32
#define FV_ENCODING				64
#define FV_MODE					128

typedef struct FILEVARS {
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

typedef const FILEVARS * LPCFILEVARS;

BOOL	FileVars_Init(LPCSTR lpData, DWORD cbData, LPFILEVARS lpfv);
BOOL	FileVars_Apply(LPCFILEVARS lpfv);
BOOL	FileVars_ParseInt(LPCSTR pszData, LPCSTR pszName, int *piValue);
BOOL	FileVars_ParseStr(LPCSTR pszData, LPCSTR pszName, char *pszValue, int cchValue);
// in EditEncoding.c
BOOL	FileVars_IsUTF8(LPCFILEVARS lpfv);
BOOL	FileVars_IsNonUTF8(LPCFILEVARS lpfv);
BOOL	FileVars_IsValidEncoding(LPCFILEVARS lpfv);
int 	FileVars_GetEncoding(LPCFILEVARS lpfv);

typedef enum {
	FOLD_ACTION_FOLD	= 0, // SC_FOLDACTION_CONTRACT
	FOLD_ACTION_EXPAND	= 1, // SC_FOLDACTION_EXPAND
	FOLD_ACTION_SNIFF	= 2, // SC_FOLDACTION_TOGGLE
} FOLD_ACTION;

void FoldToggleAll(FOLD_ACTION action);
void FoldToggleLevel(int lev, FOLD_ACTION action);
void FoldToggleCurrentBlock(FOLD_ACTION action);
void FoldToggleCurrentLevel(FOLD_ACTION action);
void FoldToggleDefault(FOLD_ACTION action);
void FoldClickAt(Sci_Position pos, int mode);
void FoldAltArrow(int key, int mode);
void EditGotoBlock(int menu);
