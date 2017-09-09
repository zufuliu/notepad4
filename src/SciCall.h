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

__forceinline void InitScintillaHandle(HWND hwnd) {
	g_hScintilla = (HANDLE)SendMessage(hwnd, SCI_GETDIRECTPOINTER, 0, 0);
}

//=============================================================================
//
//  SciCall()
//
//
LRESULT WINAPI Scintilla_DirectFunction(HANDLE, UINT, WPARAM, LPARAM);
#define SciCall0(m)			Scintilla_DirectFunction(g_hScintilla, m, 0, 0)
#define SciCall1(m, w)		Scintilla_DirectFunction(g_hScintilla, m, (WPARAM)w, 0)
#define SciCall2(m, w, l)	Scintilla_DirectFunction(g_hScintilla, m, (WPARAM)w, (LPARAM)l)

//=============================================================================
//
//  Selection and information
//
//
__forceinline int SciCall_GetLength(void) {
	return (int)SciCall0(SCI_GETLENGTH);
}

__forceinline int SciCall_GetLineCount(void) {
	return (int)SciCall0(SCI_GETLINECOUNT);
}

__forceinline void SciCall_SetSel(Sci_Position anchor, Sci_Position caret) {
	SciCall2(SCI_SETSEL, anchor, caret);
}

__forceinline void SciCall_GotoPos(Sci_Position caret) {
	SciCall1(SCI_GOTOPOS, caret);
}

__forceinline void SciCall_GotoLine(int line) {
	SciCall1(SCI_GOTOLINE, line);
}

__forceinline Sci_Position SciCall_GetCurrentPos(void) {
	return (Sci_Position)SciCall0(SCI_GETCURRENTPOS);
}

__forceinline int SciCall_GetColumn(Sci_Position position) {
	return (int)SciCall1(SCI_GETCOLUMN, position);
}

__forceinline int SciCall_LineFromPosition(Sci_Position position) {
	return (int)SciCall1(SCI_LINEFROMPOSITION, position);
}

__forceinline Sci_Position SciCall_PositionFromLine(int line) {
	return (Sci_Position)SciCall1(SCI_POSITIONFROMLINE, line);
}

__forceinline int SciCall_GetCharAt(Sci_Position position) {
	return (int)SciCall1(SCI_GETCHARAT, position);
}

__forceinline int SciCall_GetLineLength(int line) {
	return (int)SciCall1(SCI_GETLINE, line);
}

__forceinline int SciCall_GetLine(int line, char *buffer) {
	return (int)SciCall2(SCI_GETLINE, line, buffer);
}

__forceinline int SciCall_GetSelTextLength(void) {
	return (int)SciCall0(SCI_GETSELTEXT);
}

__forceinline int SciCall_GetSelText(char *buffer) {
	return (int)SciCall2(SCI_GETSELTEXT, 0, buffer);
}

__forceinline int SciCall_CountCharacters(Sci_Position start, Sci_Position end) {
	return (int)SciCall2(SCI_COUNTCHARACTERS, start, end);
}

__forceinline Sci_Position SciCall_GetSelectionStart(void) {
	return (Sci_Position)SciCall0(SCI_GETSELECTIONSTART);
}

__forceinline Sci_Position SciCall_GetSelectionEnd(void) {
	return (Sci_Position)SciCall0(SCI_GETSELECTIONEND);
}

__forceinline int SciCall_GetSelectionMode(void) {
	return (int)SciCall0(SCI_GETSELECTIONMODE);
}

__forceinline BOOL EditIsEmptySelection(void) {
	return SciCall_GetSelectionStart() == SciCall_GetSelectionEnd();
}

//=============================================================================
//
//  Scrolling and automatic scrolling
//
//
__forceinline void SciCall_ScrollCaret(void) {
	SciCall0(SCI_SCROLLCARET);
}

__forceinline void SciCall_SetXCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall2(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

__forceinline void SciCall_SetYCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall2(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

//=============================================================================
//
//  Style definition
//
//
__forceinline int SciCall_GetStyleAt(Sci_Position position) {
	return (int)SciCall1(SCI_GETSTYLEAT, position);
}

__forceinline COLORREF SciCall_StyleGetFore(int style) {
	return (COLORREF)SciCall1(SCI_STYLEGETFORE, style);
}

__forceinline COLORREF SciCall_StyleGetBack(int style) {
	return (COLORREF)SciCall1(SCI_STYLEGETBACK, style);
}

//=============================================================================
//
//  Margins
//
//
__forceinline void SciCall_SetMarginType(int margin, int marginType) {
	SciCall2(SCI_SETMARGINTYPEN, margin, marginType);
}

__forceinline void SciCall_SetMarginWidth(int margin, int pixelWidth) {
	SciCall2(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

__forceinline void SciCall_SetMarginMask(int margin, int mask) {
	SciCall2(SCI_SETMARGINMASKN, margin, mask);
}

__forceinline void SciCall_SetMarginSensitive(int margin, BOOL sensitive) {
	SciCall2(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

__forceinline void SciCall_SetFoldMarginColour(BOOL useSetting, COLORREF back) {
	SciCall2(SCI_SETFOLDMARGINCOLOUR, useSetting, back);
}

__forceinline void SciCall_SetFoldMarginHiColour(BOOL useSetting, COLORREF fore) {
	SciCall2(SCI_SETFOLDMARGINHICOLOUR, useSetting, fore);
}

//=============================================================================
//
//  Markers
//
//
__forceinline void SciCall_MarkerDefine(int markerNumber, int markerSymbol) {
	SciCall2(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

__forceinline void SciCall_MarkerSetFore(int markerNumber, COLORREF fore) {
	SciCall2(SCI_MARKERSETFORE, markerNumber, fore);
}

__forceinline void SciCall_MarkerSetBack(int markerNumber, COLORREF back) {
	SciCall2(SCI_MARKERSETBACK, markerNumber, back);
}

__forceinline void SciCall_MarkerEnableHighlight(BOOL enabled) {
	SciCall1(SCI_MARKERENABLEHIGHLIGHT, enabled);
}

//=============================================================================
//
//  Folding
//
//
__forceinline BOOL SciCall_GetLineVisible(int line) {
	return (BOOL)SciCall1(SCI_GETLINEVISIBLE, line);
}

__forceinline int SciCall_GetFoldLevel(int line) {
	return (int)SciCall1(SCI_GETFOLDLEVEL, line);
}

__forceinline void SciCall_SetFoldFlags(int flags) {
	SciCall1(SCI_SETFOLDFLAGS, flags);
}

__forceinline int SciCall_GetFoldParent(int line) {
	return (int)SciCall1(SCI_GETFOLDPARENT, line);
}

__forceinline BOOL SciCall_GetFoldExpanded(int line) {
	return (BOOL)SciCall1(SCI_GETFOLDEXPANDED, line);
}

__forceinline void SciCall_ToggleFold(int line) {
	SciCall1(SCI_TOGGLEFOLD, line);
}

__forceinline void SciCall_EnsureVisible(int line) {
	SciCall1(SCI_ENSUREVISIBLE, line);
}

//=============================================================================
//
//  Lexer
//
//
__forceinline void SciCall_SetProperty(const char *key, const char *value) {
	SciCall2(SCI_SETPROPERTY, key, value);
}

#endif // NOTEPAD2_SCICALL_H_

// End of SciCall.h
