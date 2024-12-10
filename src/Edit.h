/******************************************************************************
*
*
* Notepad4
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
*                                              https://www.flos-freeware.ch
*
*
******************************************************************************/
#pragma once

// WideCharToMultiByte, UTF8 encoding of U+0800 to U+FFFF
#define kMaxMultiByteCount	3

#define NP2_FIND_REPLACE_LIMIT	2048
#define NP2_LONG_LINE_LIMIT		4096

#define NP2_InvalidSearchFlags	(-1)
#define NP2_MarkAllMultiline	0x00001000
#define NP2_MarkAllBookmark		0x00002000
#define NP2_MarkAllSelectAll	0x00004000
#define NP2_FromFindAll			0x00008000

enum {
	FindReplaceOption_None = 0,
	FindReplaceOption_UseCxxRegex = 1,
	FindReplaceOption_TransparentMode = 2,
	FindReplaceOption_UseMonospacedFont = 4,
	FindReplaceOption_FindAllBookmark = 8,
	FindReplaceOption_Default = 2,

	FindReplaceOption_CloseFind = 1,
	FindReplaceOption_CloseReplace = 2,
	FindReplaceOption_NoFindWrap = 4,
	FindReplaceOption_BehaviorMask = 7,

	FindReplaceOption_TransformBackslash = 8,
	FindReplaceOption_WildcardSearch = 16,
	FindReplaceOption_SearchMask = 24,
};

struct EDITFINDREPLACE {
	HWND	hwnd;
	UINT	fuFlags;
	UINT	option;
	char	szFind[512];
	char	szReplace[512];
	char	szFindUTF8[512 * kMaxMultiByteCount];
	char	szReplaceUTF8[512 * kMaxMultiByteCount];
};

enum EditAlignMode {
	EditAlignMode_Left = 0,
	EditAlignMode_Right = 1,
	EditAlignMode_Center = 2,
	EditAlignMode_Justify = 3,
	EditAlignMode_JustifyEx = 4,
};

enum EditSortFlag {
	EditSortFlag_Ascending = 0,
	EditSortFlag_Descending = 1,
	EditSortFlag_IgnoreCase = 2,
	EditSortFlag_LogicalNumber = 4,
	EditSortFlag_ColumnSort = 8,
	EditSortFlag_GroupByFileType = 16,
	EditSortFlag_DontSort = 32,
	EditSortFlag_Shuffle = 64,
	EditSortFlag_MergeDuplicate = 128,
	EditSortFlag_RemoveDuplicate = 256,
	EditSortFlag_RemoveUnique = 512,
};

// wrap indent
enum {
	EditWrapIndent_None = 0,
	EditWrapIndent_OneCharacter = 1,
	EditWrapIndent_TwoCharacter = 2,
	EditWrapIndent_OneLevel = 3,
	EditWrapIndent_TwoLevel = 4,

	EditWrapIndent_SameAsSubline = 5,
	EditWrapIndent_OneLevelThanSubline = 6,
	EditWrapIndent_TwoLevelThanSubline = 7,

	EditWrapIndent_MaxValue = EditWrapIndent_TwoLevelThanSubline,
	EditWrapIndent_DefaultValue = EditWrapIndent_None,
};

// wrap symbol
enum {
	EditWrapSymbolBefore_None = 0,
	EditWrapSymbolBefore_NearText = 1,
	EditWrapSymbolBefore_NearBorder = 2,
	EditWrapSymbolBefore_MaxValue = EditWrapSymbolBefore_NearBorder,

	EditWrapSymbolAfter_None = 0,
	EditWrapSymbolAfter_NearText = 1,
	EditWrapSymbolAfter_NearBorder = 2,
	EditWrapSymbolAfter_LineNumberMargin = 3,
	EditWrapSymbolAfter_MaxValue = EditWrapSymbolAfter_LineNumberMargin,

	EditWrapSymbol_MaxValue = EditWrapSymbolBefore_MaxValue + 10 * EditWrapSymbolAfter_MaxValue,
	EditWrapSymbol_DefaultValue = EditWrapSymbolBefore_NearBorder,
};

void	Edit_ReleaseResources() noexcept;
void	EditCreate(HWND hwndParent) noexcept;
void	EditSetNewText(LPCSTR lpstrText, DWORD cbText, Sci_Line lineCount) noexcept;

static inline void EditSetEmptyText() noexcept{
	EditSetNewText("", 0, 1);
}

bool	EditConvertText(UINT cpSource, UINT cpDest) noexcept;
void	EditConvertToLargeMode() noexcept;
void	EditReplaceDocument(HANDLE pdoc) noexcept;

char*	EditGetClipboardText(HWND hwnd) noexcept; // LocalFree()
bool	EditCopyAppend(HWND hwnd) noexcept;

constexpr int GetScintillaEOLMode(int mode) noexcept {
	constexpr UINT mask = (SC_EOL_CRLF << 2*0) | (SC_EOL_LF << 2*1) | (SC_EOL_CR << 2*2);
	return (mask >> (mode << 1)) & 3;
}
constexpr int GetSettingsEOLMode(int mode) noexcept {
	constexpr UINT mask = (0 << 2*SC_EOL_CRLF) | (1 << 2*SC_EOL_LF) | (2 << 2*SC_EOL_CR);
	return (mask >> (mode << 1)) & 3;
}

struct EditFileIOStatus;
void 	EditDetectEOLMode(LPCSTR lpData, DWORD cbData, EditFileIOStatus &status) noexcept;
bool	EditLoadFile(LPWSTR pszFile, EditFileIOStatus &status) noexcept;
bool	EditSaveFile(HWND hwnd, LPCWSTR pszFile, int saveFlag, EditFileIOStatus &status) noexcept;

void	EditReplaceMainSelection(Sci_Position cchText, LPCSTR pszText) noexcept;
void	EditMapTextCase(int menu) noexcept;

void	EditURLEncode(bool component) noexcept;
void	EditURLDecode() noexcept;
void	EditEscapeCChars(HWND hwnd) noexcept;
void	EditUnescapeCChars(HWND hwnd) noexcept;
void	EditEscapeXHTMLChars(HWND hwnd) noexcept;
void	EditUnescapeXHTMLChars(HWND hwnd) noexcept;
void	EditChar2Hex() noexcept;
void	EditHex2Char() noexcept;
void	EditShowHex() noexcept;

enum Base64EncodingFlag {
	Base64EncodingFlag_Default,
	Base64EncodingFlag_UrlSafe,
	Base64EncodingFlag_HtmlEmbeddedImage,
};
void	EditBase64Encode(Base64EncodingFlag encodingFlag) noexcept;
void	EditBase64Decode(bool decodeAsHex) noexcept;
void	EditConvertNumRadix(int radix) noexcept;
void	EditModifyNumber(bool bIncrease);

void	EditTabsToSpaces(int nTabWidth, bool bOnlyIndentingWS) noexcept;
void	EditSpacesToTabs(int nTabWidth, bool bOnlyIndentingWS) noexcept;

void	EditMoveUp() noexcept;
void	EditMoveDown() noexcept;
void	EditModifyLines(LPCWSTR pwszPrefix, LPCWSTR pwszAppend, bool skipEmptyLine) noexcept;
void	EditAlignText(EditAlignMode nMode) noexcept;
void	EditEncloseSelection(LPCWSTR pwszOpen, LPCWSTR pwszClose) noexcept;
void	EditToggleLineComments(LPCWSTR pwszComment, int commentFlag) noexcept;
void	EditPadWithSpaces(bool bSkipEmpty, bool bNoUndoGroup) noexcept;
void	EditStripFirstCharacter() noexcept;
void	EditStripLastCharacter() noexcept;
void	EditStripTrailingBlanks(HWND hwnd, bool bIgnoreSelection) noexcept;
void	EditStripLeadingBlanks(HWND hwnd, bool bIgnoreSelection) noexcept;
void	EditCompressSpaces() noexcept;
void	EditRemoveBlankLines(bool bMerge) noexcept;
void	EditWrapToColumn(int nColumn/*, int nTabWidth*/) noexcept;
void	EditJoinLinesEx() noexcept;
void	EditSortLines(EditSortFlag iSortFlags) noexcept;

void	EditJumpTo(Sci_Line iNewLine, Sci_Position iNewCol) noexcept;
void	EditSelectEx(Sci_Position iAnchorPos, Sci_Position iCurrentPos) noexcept;
void	EditFixPositions() noexcept;
void	EditEnsureSelectionVisible() noexcept;
void	EditEnsureConsistentLineEndings() noexcept;
void	EditGetExcerpt(LPWSTR lpszExcerpt, DWORD cchExcerpt) noexcept;

void	EditSelectWord() noexcept;
void	EditSelectLines(bool currentBlock, bool lineSelection) noexcept;
void	EditSaveSelectionAsFindText(EDITFINDREPLACE *lpefr, int menu, bool findSelection) noexcept;
HWND	EditFindReplaceDlg(HWND hwnd, EDITFINDREPLACE *lpefr, bool bReplace) noexcept;
void	EditFindNext(const EDITFINDREPLACE *lpefr, bool fExtendSelection) noexcept;
void	EditFindPrev(const EDITFINDREPLACE *lpefr, bool fExtendSelection) noexcept;
void	EditFindAll(const EDITFINDREPLACE *lpefr, bool selectAll) noexcept;
void	EditReplace(HWND hwnd, const EDITFINDREPLACE *lpefr) noexcept;
enum EditReplaceAllFlag {
	EditReplaceAllFlag_None,
	EditReplaceAllFlag_UndoGroup,
	EditReplaceAllFlag_ShowInfo,
	EditReplaceAllFlag_Default,
};
void	EditReplaceAll(HWND hwnd, const EDITFINDREPLACE *lpefr) noexcept;
void	EditReplaceAllInSelection(HWND hwnd, const EDITFINDREPLACE *lpefr, EditReplaceAllFlag flag = EditReplaceAllFlag_None) noexcept;
bool	EditLineNumDlg(HWND hwnd) noexcept;
void	EditModifyLinesDlg(HWND hwnd) noexcept;
void	EditEncloseSelectionDlg(HWND hwnd) noexcept;
void	EditInsertTagDlg(HWND hwnd) noexcept;
void	EditInsertDateTime(bool bShort) noexcept;
void	EditUpdateTimestampMatchTemplate(HWND hwnd) noexcept;
void	EditInsertUnicodeControlCharacter(int menu) noexcept;
void	EditShowUnicodeControlCharacter(bool bShow) noexcept;
bool	EditSortDlg(HWND hwnd, EditSortFlag *piSortFlags) noexcept;
bool	EditAlignDlg(HWND hwnd, EditAlignMode *piAlignMode) noexcept;
void	EditSelectionAction(int action) noexcept;
void	TryBrowseFile(HWND hwnd, LPCWSTR pszFile, bool bWarn) noexcept;

enum OpenSelectionType {
	OpenSelectionType_None,
	OpenSelectionType_Link,
	OpenSelectionType_File,
	OpenSelectionType_Folder,
	OpenSelectionType_ContainingFolder,
};
void EditOpenSelection(OpenSelectionType type);

// in Bridge.cpp
bool	EditPrint(HWND hwnd, LPCWSTR pszDocTitle, BOOL bDefault) noexcept;
void	EditPrintSetup(HWND hwnd) noexcept;
void	EditFormatCode(int menu) noexcept;

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

enum {
	MarkOccurrences_None = 0,
	MarkOccurrences_Enable = 1,
	MarkOccurrences_MatchCase = 2,
	MarkOccurrences_WholeWord = 4,
	MarkOccurrences_Bookmark = 8,
};

struct EditMarkAll {
	bool pending;
	bool ignoreSelectionUpdate;
	bool bookmarkForFindAll;
	int markFlag;
	int incrementSize;			// increment search size
	Sci_Position length;		// length for pszText
	LPSTR pszText;				// pattern or text to find
	double duration;			// search duration in milliseconds
	Sci_Position matchCount;	// total match count
	Sci_Position lastMatchPos;	// last matching position
	Sci_Position prevStopPos;	// previous stop position
	Sci_Line prevBookmarkLine;	// previous bookmark line
	StopWatch watch;			// used to dynamic compute increment size

	void Reset(int findFlag, Sci_Position iSelCount, LPSTR text) noexcept;
	void Clear() noexcept {
		Reset(0, 0, nullptr);
	}
	void Start(BOOL bChanged, int findFlag, Sci_Position iSelCount, LPSTR text) noexcept;
	void Continue(HANDLE timer) noexcept;
	void Stop() noexcept;
	void MarkAll(BOOL bChanged, int option) noexcept;
};

void EditToggleBookmarkAt(Sci_Position iPos) noexcept;
void EditBookmarkSelectAll() noexcept;

// auto completion fill-up characters
#define MAX_AUTO_COMPLETION_FILLUP_LENGTH	32		// Only 32 ASCII punctuation
#define AUTO_COMPLETION_FILLUP_DEFAULT		L";,()[]{}\\/"
// timeout for scanning words in document
#define AUTOC_SCAN_WORDS_MIN_TIMEOUT		50
#define AUTOC_SCAN_WORDS_DEFAULT_TIMEOUT	500
enum {
	AutoCompleteScope_None = 0,
	AutoCompleteScope_Commont = 1,
	AutoCompleteScope_String = 2,
	AutoCompleteScope_PlainText = 4,
	AutoCompleteScope_Other = 8,
	AutoCompleteScope_Default = 0xff,
};
enum {
	AutoCompleteFillUpMask_None = 0,
	AutoCompleteFillUpMask_Enter = 1,
	AutoCompleteFillUpMask_Tab = 2,
	AutoCompleteFillUpMask_Space = 4,
	AutoCompleteFillUpMask_Punctuation = 8,
	// default settings
	AutoCompleteFillUpMask_Default = 15,
};

// auto insert
enum AutoInsertCharacter {
	AutoInsertCharacter_Parenthesis,
	AutoInsertCharacter_Brace,
	AutoInsertCharacter_SquareBracket,
	AutoInsertCharacter_AngleBracket,
	AutoInsertCharacter_DoubleQuote,
	AutoInsertCharacter_SingleQuote,
	AutoInsertCharacter_Backtick,
	AutoInsertCharacter_Comma,
};
enum {
	AutoInsertMask_None = 0,
	AutoInsertMask_Parenthesis = 1,			// ()
	AutoInsertMask_Brace = 2,				// {}
	AutoInsertMask_SquareBracket = 4,		// []
	AutoInsertMask_AngleBracket = 8,		// <>
	AutoInsertMask_DoubleQuote = 16,		// ""
	AutoInsertMask_SingleQuote = 32,		// ''
	AutoInsertMask_Backtick = 64,			// ``
	AutoInsertMask_SpaceAfterComma = 128,	// ', '
	AutoInsertMask_CommentAtStart = true,	// '// '
	AutoInsertMask_SpaceAfterComment = 256,	// '// '
	// default settings
	AutoInsertMask_Default = 511,
};

// asm line comment
enum {
	AsmLineCommentChar_Semicolon = 0,	// ';'
	AsmLineCommentChar_Sharp = 1,		// '#'
	AsmLineCommentChar_Slash = 2,		// '//'
	AsmLineCommentChar_At = 3,			// '@'
};

#define MIN_AUTO_COMPLETION_VISIBLE_ITEM_COUNT	8
#define MIN_AUTO_COMPLETION_WORD_LENGTH			1
#define MIN_AUTO_COMPLETION_NUMBER_LENGTH		0

enum {
	AutoCompletionOption_None = 0,
	AutoCompletionOption_CloseTags = 1,
	AutoCompletionOption_CompleteWord = 2,
	AutoCompletionOption_ScanWordsInDocument = 4,
	AutoCompletionOption_OnlyWordsInDocument = 8,
	AutoCompletionOption_EnglishIMEModeOnly = 16,
	AutoCompletionOption_Default = 7,
};

struct EditAutoCompletionConfig {
	bool bIndentText;
	bool bIgnoreCase;
	bool bLaTeXInputMethod;
	int iCompleteOption;
	int fCompleteScope;
	int fScanWordScope;
	UINT dwScanWordsTimeout;
	UINT iVisibleItemCount;
	int iMinWordLength;
	int iMinNumberLength;
	int fAutoCompleteFillUpMask;
	int fAutoInsertMask;
	int iAsmLineCommentChar;
	UINT iPreviousItemCount;		// status
	char szAutoCompleteFillUp[MAX_AUTO_COMPLETION_FILLUP_LENGTH + 4];
	WCHAR wszAutoCompleteFillUp[MAX_AUTO_COMPLETION_FILLUP_LENGTH];
};

enum {
	AutoCompleteCondition_Normal = 0,
	AutoCompleteCondition_OnCharAdded = 1,
	AutoCompleteCondition_OnCharDeleted = 2,
};

// in EditAutoC.cpp
extern const uint32_t DefaultWordCharSet[8];
void	EditCompleteUpdateConfig() noexcept;
bool	IsDocWordChar(uint32_t ch) noexcept;
bool	IsAutoCompletionWordCharacter(uint32_t ch) noexcept;
void	EditCompleteWord(int iCondition, bool autoInsert) noexcept;
bool	EditIsOpenBraceMatched(Sci_Position pos, Sci_Position startPos) noexcept;
void	EditAutoCloseBraceQuote(int ch, AutoInsertCharacter what) noexcept;
void	EditAutoCloseXMLTag() noexcept;
void	EditAutoIndent() noexcept;
void	EditToggleCommentLine(bool alternative) noexcept;
void	EditToggleCommentBlock(bool alternative) noexcept;
void	EditInsertScriptShebangLine() noexcept;

enum CallTipType {
	CallTipType_None,
	CallTipType_Notification,
	CallTipType_ColorHex,
};

enum ShowCallTip {
	ShowCallTip_None,
	ShowCallTip_ColorRGBA,
	ShowCallTip_ColorARGB,
	ShowCallTip_ColorBGRA,
	ShowCallTip_ColorABGR,
};

struct CallTipInfo {
	ShowCallTip showCallTip;
	CallTipType type;
	Sci_Position startPos;
	Sci_Position endPos;
	Sci_Position hexStart;
	COLORREF currentColor;
	COLORREF backColor;
	COLORREF foreColor;
};
void	EditShowCallTip(Sci_Position position) noexcept;
void	EditClickCallTip(HWND hwnd) noexcept;

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
#define CPI_FIRST					0
#define CPI_DEFAULT					0
#define CPI_OEM						1
#define CPI_UNICODEBOM				2
#define CPI_UNICODEBEBOM			3
#define CPI_UNICODE					4
#define CPI_UNICODEBE				5
#define CPI_UTF8					6
#define CPI_UTF8SIGN				7
#define CPI_UTF7					8
// global default encoding
#define CPI_GLOBAL_DEFAULT			CPI_UTF8

#define MAX_ENCODING_LABEL_SIZE		32
// MultiByteToWideChar() and WideCharToMultiByte() uses int as length.
#define MAX_NON_UTF8_SIZE	((1U << 31) - 16)
// added 32 bytes padding as encoding detection may read beyond cbData.
#define NP2_ENCODING_DETECTION_PADDING	32

enum {
	EncodingFlag_None = 0,
	EncodingFlag_Binary = 1,
	EncodingFlag_UTF7 = 2,
	EncodingFlag_Reversed = 4,
	EncodingFlag_Invalid = 8,
};

struct NP2ENCODING {
	const UINT uFlags;
	/*const*/UINT uCodePage;
	// string format: [normal name + ','] + [lower case parse name + ',']+
	const char * const pszParseNames;
	const UINT idsName;
	LPWSTR wchLabel;
};

// see UniConversion.h and https://www.unicode.org/faq/utf_bom.html
#define SURROGATE_LEAD_FIRST		0xD800
#define SURROGATE_TRAIL_FIRST		0xDC00
#define SUPPLEMENTAL_PLANE_FIRST	0x10000
#define MAX_UNICODE					0x10ffff
#define SURROGATE_OFFSET			(0x10000 - (0xD800 << 10) - 0xDC00)
#define UTF16_TO_UTF32(lead, trail)	(((lead) << 10) + (trail) + SURROGATE_OFFSET)

// https://docs.microsoft.com/en-us/windows/win32/intl/locale_iuseutf8legacyacp
#ifndef LOCALE_IUSEUTF8LEGACYACP
#define LOCALE_IUSEUTF8LEGACYACP     0x00000666	// NTDDI_VERSION >= NTDDI_WIN10_MN
#endif
// https://docs.microsoft.com/en-us/windows/win32/intl/locale_iuseutf8legacyoemcp
#ifndef LOCALE_IUSEUTF8LEGACYOEMCP
#define LOCALE_IUSEUTF8LEGACYOEMCP   0x00000999	// NTDDI_VERSION >= NTDDI_WIN10_MN
#endif
// https://docs.microsoft.com/en-us/windows/win32/intl/locale-sname
#ifndef LOCALE_SNAME
#define LOCALE_SNAME				0x0000005c	// _WIN32_WINNT >= _WIN32_WINNT_VISTA
#endif

inline BOOL GetLegacyACP(UINT *acp) noexcept {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IUSEUTF8LEGACYACP | LOCALE_RETURN_NUMBER,
		reinterpret_cast<LPWSTR>(acp), sizeof(UINT) / sizeof(WCHAR));
#else
	return GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, LOCALE_IUSEUTF8LEGACYACP | LOCALE_RETURN_NUMBER,
		reinterpret_cast<LPWSTR>(acp), sizeof(UINT) / sizeof(WCHAR));
#endif
}

inline BOOL GetLegacyOEMCP(UINT *oemcp) noexcept {
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IUSEUTF8LEGACYOEMCP | LOCALE_RETURN_NUMBER,
		reinterpret_cast<LPWSTR>(oemcp), sizeof(UINT) / sizeof(WCHAR));
#else
	return GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, LOCALE_IUSEUTF8LEGACYOEMCP | LOCALE_RETURN_NUMBER,
		reinterpret_cast<LPWSTR>(oemcp), sizeof(UINT) / sizeof(WCHAR));
#endif
}

// 932 Shift-JIS, 936 GBK, 949 UHC, 950 Big5, 1361 Johab
constexpr bool IsDBCSCodePage(UINT page) noexcept {
	return page == 932 || page == 936 || page == 949 || page == 950 || page == 1361;
}

// dwFlags must be 0 for MultiByteToWideChar() and WideCharToMultiByte()
constexpr bool IsZeroFlagsCodePage(UINT page) noexcept {
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

// in EditEncoding.cpp
extern NP2ENCODING mEncoding[];
constexpr bool Encoding_IsUnicode(int iEncoding) noexcept {
	return iEncoding == CPI_UNICODEBOM
		|| iEncoding == CPI_UNICODEBEBOM
		|| iEncoding == CPI_UNICODE
		|| iEncoding == CPI_UNICODEBE;
}

constexpr bool Encoding_IsUTF8(int iEncoding) noexcept {
	return iEncoding == CPI_UTF8
		|| iEncoding == CPI_UTF8SIGN;
}

void	Encoding_ReleaseResources() noexcept;
bool	EditSetNewEncoding(int iEncoding, int iNewEncoding, BOOL bNoUI) noexcept;
void	EditOnCodePageChanged(UINT oldCodePage, bool showControlCharacter, EDITFINDREPLACE *lpefr) noexcept;
const char* GetFoldDisplayEllipsis(UINT cpEdit, UINT acp) noexcept;
void	Encoding_InitDefaults() noexcept;
int 	Encoding_MapIniSetting(bool bLoad, UINT iSetting) noexcept;
void	Encoding_GetLabel(int iEncoding) noexcept;
int 	Encoding_Match(LPCWSTR pwszTest) noexcept;
int 	Encoding_MatchA(LPCSTR pchTest) noexcept;
bool	Encoding_IsValid(int iEncoding) noexcept;
int		Encoding_GetIndex(UINT codePage) noexcept;
int		Encoding_GetAnsiIndex() noexcept;
void	Encoding_AddToTreeView(HWND hwnd, int idSel, bool bRecodeOnly) noexcept;
bool	Encoding_GetFromTreeView(HWND hwnd, int *pidEncoding, bool bQuiet) noexcept;
#if 0
void	Encoding_AddToListView(HWND hwnd, int idSel, bool bRecodeOnly) noexcept;
bool	Encoding_GetFromListView(HWND hwnd, int *pidEncoding) noexcept;
void	Encoding_AddToComboboxEx(HWND hwnd, int idSel, bool bRecodeOnly) noexcept;
bool	Encoding_GetFromComboboxEx(HWND hwnd, int *pidEncoding) noexcept;
#endif

UINT	CodePageFromCharSet(UINT uCharSet) noexcept;
bool	IsUTF8(const char *pTest, DWORD nLength) noexcept;
bool	IsUTF7(const char *pTest, DWORD nLength) noexcept;

#define BOM_UTF8		0xBFBBEF
#define BOM_UTF16LE		0xFEFF
#define BOM_UTF16BE		0xFFFE
inline bool IsUTF8Signature(const char *p) noexcept {
	//return p[0] == '\xEF' && p[1] == '\xBB' && p[2] == '\xBF';
	return (*(reinterpret_cast<const UINT *>(p)) & 0xFFFFFF) == BOM_UTF8;
}

constexpr BOOL Encoding_HasBOM(int iEncoding) noexcept {
	return (iEncoding == CPI_UNICODEBOM
		|| iEncoding == CPI_UNICODEBEBOM
		|| iEncoding == CPI_UTF8SIGN) ? iEncoding : FALSE;
}

LPSTR RecodeAsUTF8(LPSTR lpData, DWORD *cbData, UINT codePage, DWORD flags) noexcept;
int EditDetermineEncoding(LPCWSTR pszFile, char *lpData, DWORD cbData, int *encodingFlag) noexcept;
bool IsStringCaseSensitiveW(LPCWSTR pszTextW) noexcept;
bool IsStringCaseSensitiveA(LPCSTR pszText) noexcept;

//void SciInitThemes(HWND hwnd) noexcept;

#define FV_TABWIDTH				1
#define FV_INDENTWIDTH			2
#define FV_TABSASSPACES			4
#define FV_TABINDENTS			8
#define FV_WORDWRAP				16
#define FV_LONGLINESLIMIT		32
#define FV_ENCODING				64
#define FV_MODE					128
#define FV_MaskHasTabIndentWidth	(FV_TABWIDTH | FV_INDENTWIDTH)
#define FV_MaskHasFileTabSettings	(FV_TABWIDTH | FV_INDENTWIDTH | FV_TABSASSPACES)

struct EditTabSettings {
	int 	globalTabWidth;
	int 	globalIndentWidth;
	bool	globalTabsAsSpaces;
	bool	bTabIndents;
	uint8_t	bBackspaceUnindents;
	bool	bDetectIndentation;

	int		schemeTabWidth;
	int		schemeIndentWidth;
	bool	schemeTabsAsSpaces;
	bool	schemeUseGlobalTabSettings;
};

struct EditFileVars {
	int 	mask;
	int 	iTabWidth;
	int 	iIndentWidth;
	bool	bTabsAsSpaces;
	bool	bTabIndents;
	bool	fWordWrap;
	int 	iLongLinesLimit;
	int 	iEncoding;
	char	tchEncoding[32];
	char	tchMode[32];
	void Init(LPCSTR lpData, DWORD cbData) noexcept;
	void Apply() noexcept;
	int GetEncoding() const noexcept {
		return (mask & FV_ENCODING) ? iEncoding : CPI_NONE;
	}
};

extern EditTabSettings tabSettings;
extern EditFileVars fvCurFile;

void	EditSetWrapStartIndent(int tabWidth, int indentWidth) noexcept;
void	EditSetWrapIndentMode(int tabWidth, int indentWidth) noexcept;
bool	FileVars_ParseInt(LPCSTR pszData, LPCSTR pszName, int *piValue) noexcept;
bool	FileVars_ParseStr(LPCSTR pszData, LPCSTR pszName, char *pszValue, int cchValue) noexcept;

enum FOLD_ACTION {
	FOLD_ACTION_FOLD	= SC_FOLDACTION_CONTRACT,
	FOLD_ACTION_EXPAND	= SC_FOLDACTION_EXPAND,
	FOLD_ACTION_SNIFF	= SC_FOLDACTION_TOGGLE,
};

void FoldExpandRange(Sci_Line lineStart, Sci_Line lineEnd) noexcept;
void FoldToggleAll(FOLD_ACTION action) noexcept;
void FoldToggleLevel(int lev, FOLD_ACTION action) noexcept;
void FoldToggleCurrentBlock(FOLD_ACTION action) noexcept;
void FoldToggleCurrentLevel(FOLD_ACTION action) noexcept;
void FoldToggleDefault(FOLD_ACTION action) noexcept;
void FoldClickAt(Sci_Position pos, int mode) noexcept;
void FoldAltArrow(int key, int mode) noexcept;
void EditGotoBlock(int menu) noexcept;

enum SelectOption {
	SelectOption_None = 0,
	SelectOption_EnableMultipleSelection = 1,
	SelectOption_CopySelectionAsFindText = 2,
	SelectOption_CopyPasteBufferAsFindText = 4,
	SelectOption_Default = 7,
};

enum LineSelectionMode {
	LineSelectionMode_None,
	LineSelectionMode_VisualStudio,
	LineSelectionMode_Normal,
	LineSelectionMode_OldVisualStudio,
};
