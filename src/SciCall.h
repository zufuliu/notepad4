/******************************************************************************
*
*
* Notepad2
*
* SciCall.h
*   Inline wrappers for Scintilla API calls, arranged in the order and grouping
*   in which they appear in the Scintilla documentation.
*
* The use of these inline wrapper functions with declared types will ensure
* that we get the benefit of the compiler's type checking.
*
*
******************************************************************************/

#ifndef NOTEPAD2_SCICALL_H_
#define NOTEPAD2_SCICALL_H_

//=============================================================================
//
//  g_hScintilla
//
//
extern HANDLE g_hScintilla;

static inline void InitScintillaHandle(HWND hwnd) {
	g_hScintilla = (HANDLE)SendMessage(hwnd, SCI_GETDIRECTPOINTER, 0, 0);
}

//=============================================================================
//
//  SciCall()
//
//
LRESULT WINAPI Scintilla_DirectFunction(HANDLE, UINT, WPARAM, LPARAM);
#define SciCall0(m)			Scintilla_DirectFunction(g_hScintilla, (m), 0, 0)
#define SciCall1(m, w)		Scintilla_DirectFunction(g_hScintilla, (m), (WPARAM)(w), 0)
#define SciCall2(m, w, l)	Scintilla_DirectFunction(g_hScintilla, (m), (WPARAM)(w), (LPARAM)(l))

//=============================================================================
//
//  Selection and information
//
//
static inline int SciCall_GetLength(void) {
	return (int)SciCall0(SCI_GETLENGTH);
}

static inline int SciCall_GetLineCount(void) {
	return (int)SciCall0(SCI_GETLINECOUNT);
}

static inline void SciCall_SetSel(Sci_Position anchor, Sci_Position caret) {
	SciCall2(SCI_SETSEL, anchor, caret);
}

static inline void SciCall_GotoPos(Sci_Position caret) {
	SciCall1(SCI_GOTOPOS, caret);
}

static inline void SciCall_GotoLine(int line) {
	SciCall1(SCI_GOTOLINE, line);
}

static inline Sci_Position SciCall_GetCurrentPos(void) {
	return (Sci_Position)SciCall0(SCI_GETCURRENTPOS);
}

static inline int SciCall_GetColumn(Sci_Position position) {
	return (int)SciCall1(SCI_GETCOLUMN, position);
}

static inline int SciCall_LineFromPosition(Sci_Position position) {
	return (int)SciCall1(SCI_LINEFROMPOSITION, position);
}

static inline Sci_Position SciCall_PositionFromLine(int line) {
	return (Sci_Position)SciCall1(SCI_POSITIONFROMLINE, line);
}

static inline int SciCall_GetCharAt(Sci_Position position) {
	return (int)SciCall1(SCI_GETCHARAT, position);
}

static inline int SciCall_GetLineLength(int line) {
	return (int)SciCall1(SCI_GETLINE, line);
}

static inline int SciCall_GetLine(int line, char *buffer) {
	return (int)SciCall2(SCI_GETLINE, line, buffer);
}

static inline int SciCall_GetSelTextLength(void) {
	return (int)SciCall0(SCI_GETSELTEXT);
}

static inline int SciCall_GetSelText(char *buffer) {
	return (int)SciCall2(SCI_GETSELTEXT, 0, buffer);
}

static inline int SciCall_CountCharacters(Sci_Position start, Sci_Position end) {
	return (int)SciCall2(SCI_COUNTCHARACTERS, start, end);
}

static inline Sci_Position SciCall_GetSelectionStart(void) {
	return (Sci_Position)SciCall0(SCI_GETSELECTIONSTART);
}

static inline Sci_Position SciCall_GetSelectionEnd(void) {
	return (Sci_Position)SciCall0(SCI_GETSELECTIONEND);
}

static inline int SciCall_GetSelectionMode(void) {
	return (int)SciCall0(SCI_GETSELECTIONMODE);
}

static inline BOOL EditIsEmptySelection(void) {
	return SciCall_GetSelectionStart() == SciCall_GetSelectionEnd();
}

static inline Sci_Position SciCall_FindText(int searchFlags, struct Sci_TextToFind *ft) {
	return (Sci_Position)SciCall2(SCI_FINDTEXT, searchFlags, ft);
}

static inline int SciCall_GetTextRange(struct Sci_TextRange *tr) {
	return (int)SciCall2(SCI_GETTEXTRANGE, 0, tr);
}

//=============================================================================
//
//  Scrolling and automatic scrolling
//
//
static inline void SciCall_ScrollCaret(void) {
	SciCall0(SCI_SCROLLCARET);
}

static inline void SciCall_SetXCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall2(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

static inline void SciCall_SetYCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall2(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

//=============================================================================
//
//  Style definition
//
//
static inline int SciCall_GetStyleAt(Sci_Position position) {
	return (int)SciCall1(SCI_GETSTYLEAT, position);
}

static inline COLORREF SciCall_StyleGetFore(int style) {
	return (COLORREF)SciCall1(SCI_STYLEGETFORE, style);
}

static inline COLORREF SciCall_StyleGetBack(int style) {
	return (COLORREF)SciCall1(SCI_STYLEGETBACK, style);
}

//=============================================================================
//
//  Margins
//
//
static inline void SciCall_SetMarginType(int margin, int marginType) {
	SciCall2(SCI_SETMARGINTYPEN, margin, marginType);
}

static inline void SciCall_SetMarginWidth(int margin, int pixelWidth) {
	SciCall2(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

static inline void SciCall_SetMarginMask(int margin, int mask) {
	SciCall2(SCI_SETMARGINMASKN, margin, mask);
}

static inline void SciCall_SetMarginSensitive(int margin, BOOL sensitive) {
	SciCall2(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

static inline void SciCall_SetFoldMarginColour(BOOL useSetting, COLORREF back) {
	SciCall2(SCI_SETFOLDMARGINCOLOUR, useSetting, back);
}

static inline void SciCall_SetFoldMarginHiColour(BOOL useSetting, COLORREF fore) {
	SciCall2(SCI_SETFOLDMARGINHICOLOUR, useSetting, fore);
}

//=============================================================================
//
//  Markers
//
//
static inline void SciCall_MarkerDefine(int markerNumber, int markerSymbol) {
	SciCall2(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

static inline void SciCall_MarkerSetFore(int markerNumber, COLORREF fore) {
	SciCall2(SCI_MARKERSETFORE, markerNumber, fore);
}

static inline void SciCall_MarkerSetBack(int markerNumber, COLORREF back) {
	SciCall2(SCI_MARKERSETBACK, markerNumber, back);
}

static inline void SciCall_MarkerEnableHighlight(BOOL enabled) {
	SciCall1(SCI_MARKERENABLEHIGHLIGHT, enabled);
}

//=============================================================================
//
//  Folding
//
//
static inline BOOL SciCall_GetLineVisible(int line) {
	return (BOOL)SciCall1(SCI_GETLINEVISIBLE, line);
}

static inline int SciCall_GetFoldLevel(int line) {
	return (int)SciCall1(SCI_GETFOLDLEVEL, line);
}

static inline void SciCall_SetFoldFlags(int flags) {
	SciCall1(SCI_SETFOLDFLAGS, flags);
}

static inline int SciCall_GetFoldParent(int line) {
	return (int)SciCall1(SCI_GETFOLDPARENT, line);
}

static inline BOOL SciCall_GetFoldExpanded(int line) {
	return (BOOL)SciCall1(SCI_GETFOLDEXPANDED, line);
}

static inline void SciCall_ToggleFold(int line) {
	SciCall1(SCI_TOGGLEFOLD, line);
}

static inline void SciCall_EnsureVisible(int line) {
	SciCall1(SCI_ENSUREVISIBLE, line);
}

//=============================================================================
//
//  Lexer
//
//
static inline void SciCall_SetProperty(const char *key, const char *value) {
	SciCall2(SCI_SETPROPERTY, key, value);
}

#endif // NOTEPAD2_SCICALL_H_

// End of SciCall.h
