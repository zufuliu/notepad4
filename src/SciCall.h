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
#include "compiler.h"

//=============================================================================
//
//  g_hScintilla
//
//
extern HANDLE g_hScintilla;

NP2_inline void InitScintillaHandle(HWND hwnd) {
	g_hScintilla = (HANDLE)SendMessage(hwnd, SCI_GETDIRECTPOINTER, 0, 0);
}

//=============================================================================
//
//  SciCall()
//
//
#if defined(__cplusplus)
extern "C"
#endif
LRESULT WINAPI Scintilla_DirectFunction(HANDLE handle, UINT msg, WPARAM wParam, LPARAM lParam);
#define SciCall(m, w, l)	Scintilla_DirectFunction(g_hScintilla, (m), (w), (l))

//=============================================================================
//
//  Selection and information
//
//
NP2_inline UINT SciCall_GetCodePage(void) {
	return (UINT)SciCall(SCI_GETCODEPAGE, 0, 0);
}

NP2_inline Sci_Position SciCall_GetLength(void) {
	return (Sci_Position)SciCall(SCI_GETLENGTH, 0, 0);
}

NP2_inline int SciCall_GetLineCount(void) {
	return (int)SciCall(SCI_GETLINECOUNT, 0, 0);
}

NP2_inline void SciCall_SetSel(Sci_Position anchor, Sci_Position caret) {
	SciCall(SCI_SETSEL, anchor, caret);
}

NP2_inline void SciCall_GotoPos(Sci_Position caret) {
	SciCall(SCI_GOTOPOS, caret, 0);
}

NP2_inline void SciCall_GotoLine(int line) {
	SciCall(SCI_GOTOLINE, line, 0);
}

NP2_inline Sci_Position SciCall_GetCurrentPos(void) {
	return (Sci_Position)SciCall(SCI_GETCURRENTPOS, 0, 0);
}

NP2_inline Sci_Position SciCall_PositionBefore(Sci_Position position) {
	return (Sci_Position)SciCall(SCI_POSITIONBEFORE, position, 0);
}

NP2_inline Sci_Position SciCall_PositionAfter(Sci_Position position) {
	return (Sci_Position)SciCall(SCI_POSITIONAFTER, position, 0);
}

NP2_inline int SciCall_GetColumn(Sci_Position position) {
	return (int)SciCall(SCI_GETCOLUMN, position, 0);
}

NP2_inline int SciCall_LineFromPosition(Sci_Position position) {
	return (int)SciCall(SCI_LINEFROMPOSITION, position, 0);
}

NP2_inline Sci_Position SciCall_PositionFromLine(int line) {
	return (Sci_Position)SciCall(SCI_POSITIONFROMLINE, line, 0);
}

NP2_inline int SciCall_GetCharAt(Sci_Position position) {
	return (int)SciCall(SCI_GETCHARAT, position, 0);
}

NP2_inline int SciCall_GetCharacterAndWidth(Sci_Position position, Sci_Position *width) {
	return (int)SciCall(SCI_GETCHARACTERANDWIDTH, position, (LPARAM)width);
}

NP2_inline int SciCall_GetCharacterAt(Sci_Position position) {
	return (int)SciCall(SCI_GETCHARACTERANDWIDTH, position, 0);
}

NP2_inline BOOL SciCall_IsAutoCompletionWordCharacter(int ch) {
	return (BOOL)SciCall(SCI_ISAUTOCOMPLETIONWORDCHARACTER, ch, 0);
}

NP2_inline Sci_Position SciCall_WordStartPosition(Sci_Position position, BOOL onlyWordCharacters) {
	return (Sci_Position)SciCall(SCI_WORDSTARTPOSITION, position, onlyWordCharacters);
}

NP2_inline Sci_Position SciCall_WordEndPosition(Sci_Position position, BOOL onlyWordCharacters) {
	return (Sci_Position)SciCall(SCI_WORDENDPOSITION, position, onlyWordCharacters);
}

NP2_inline int SciCall_GetLineLength(int line) {
	return (int)SciCall(SCI_GETLINE, line, 0);
}

NP2_inline Sci_Position SciCall_GetLineEndPosition(int line) {
	return (Sci_Position)SciCall(SCI_GETLINEENDPOSITION, line, 0);
}

NP2_inline int SciCall_GetLine(int line, char *buffer) {
	return (int)SciCall(SCI_GETLINE, line, (LPARAM)buffer);
}

NP2_inline int SciCall_GetSelTextLength(void) {
	return (int)SciCall(SCI_GETSELTEXT, 0, 0);
}

NP2_inline int SciCall_GetSelText(char *buffer) {
	return (int)SciCall(SCI_GETSELTEXT, 0, (LPARAM)buffer);
}

NP2_inline int SciCall_CountCharacters(Sci_Position start, Sci_Position end) {
	return (int)SciCall(SCI_COUNTCHARACTERS, start, end);
}

NP2_inline Sci_Position SciCall_GetSelectionStart(void) {
	return (Sci_Position)SciCall(SCI_GETSELECTIONSTART, 0, 0);
}

NP2_inline Sci_Position SciCall_GetSelectionEnd(void) {
	return (Sci_Position)SciCall(SCI_GETSELECTIONEND, 0, 0);
}

NP2_inline int EditGetSelectedLineCount(void) {
	const int iLineStart = SciCall_LineFromPosition(SciCall_GetSelectionStart());
	const int iLineEnd = SciCall_LineFromPosition(SciCall_GetSelectionEnd());
	return iLineEnd - iLineStart + 1;
}

NP2_inline int SciCall_GetSelectionMode(void) {
	return (int)SciCall(SCI_GETSELECTIONMODE, 0, 0);
}

NP2_inline BOOL SciCall_IsSelectionEmpty(void) {
	return (BOOL)SciCall(SCI_GETSELECTIONEMPTY, 0, 0);
}

NP2_inline Sci_Position SciCall_FindText(int searchFlags, struct Sci_TextToFind *ft) {
	return (Sci_Position)SciCall(SCI_FINDTEXT, searchFlags, (LPARAM)ft);
}

NP2_inline int SciCall_GetTextRange(struct Sci_TextRange *tr) {
	return (int)SciCall(SCI_GETTEXTRANGE, 0, (LPARAM)tr);
}

NP2_inline void SciCall_SetTargetRange(Sci_Position start, Sci_Position end) {
	SciCall(SCI_SETTARGETRANGE, start, end);
}

NP2_inline void SciCall_SetSearchFlags(int searchFlags) {
	SciCall(SCI_SETSEARCHFLAGS, searchFlags, 0);
}

NP2_inline Sci_Position SciCall_SearchInTarget(Sci_Position length, const char *text) {
	return SciCall(SCI_SEARCHINTARGET, length, (LPARAM)text);
}

NP2_inline BOOL SciCall_CanUndo(void) {
	return (BOOL)SciCall(SCI_CANUNDO, 0, 0);
}

NP2_inline BOOL SciCall_CanRedo(void) {
	return (BOOL)SciCall(SCI_CANREDO, 0, 0);
}

NP2_inline BOOL SciCall_CanPaste(void) {
	return (BOOL)SciCall(SCI_CANPASTE, 0, 0);
}

//=============================================================================
//
//  Caret
//
//
NP2_inline BOOL SciCall_GetOvertype(void) {
	return (BOOL)SciCall(SCI_GETOVERTYPE, 0, 0);
}

NP2_inline void SciCall_ScrollCaret(void) {
	SciCall(SCI_SCROLLCARET, 0, 0);
}

NP2_inline void SciCall_SetXCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

NP2_inline void SciCall_SetYCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

//=============================================================================
//
//  Style definition
//
//
NP2_inline int SciCall_GetStyleAt(Sci_Position position) {
	return (int)SciCall(SCI_GETSTYLEAT, position, 0);
}

NP2_inline COLORREF SciCall_StyleGetFore(int style) {
	return (COLORREF)SciCall(SCI_STYLEGETFORE, style, 0);
}

NP2_inline COLORREF SciCall_StyleGetBack(int style) {
	return (COLORREF)SciCall(SCI_STYLEGETBACK, style, 0);
}

//=============================================================================
//
//  Margins
//
//
NP2_inline void SciCall_SetMarginType(int margin, int marginType) {
	SciCall(SCI_SETMARGINTYPEN, margin, marginType);
}

NP2_inline void SciCall_SetMarginWidth(int margin, int pixelWidth) {
	SciCall(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

NP2_inline int SciCall_GetMarginWidth(int margin) {
	return (int)SciCall(SCI_GETMARGINWIDTHN, margin, 0);
}

NP2_inline int SciCall_TextWidth(int style, const char *text) {
	return (int)SciCall(SCI_TEXTWIDTH, style, (LPARAM)text);
}

NP2_inline void SciCall_SetMarginMask(int margin, int mask) {
	SciCall(SCI_SETMARGINMASKN, margin, mask);
}

NP2_inline void SciCall_SetMarginSensitive(int margin, BOOL sensitive) {
	SciCall(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

NP2_inline void SciCall_SetFoldMarginColour(BOOL useSetting, COLORREF back) {
	SciCall(SCI_SETFOLDMARGINCOLOUR, useSetting, back);
}

NP2_inline void SciCall_SetFoldMarginHiColour(BOOL useSetting, COLORREF fore) {
	SciCall(SCI_SETFOLDMARGINHICOLOUR, useSetting, fore);
}

//=============================================================================
//
//  Markers
//
//
NP2_inline void SciCall_MarkerDefine(int markerNumber, int markerSymbol) {
	SciCall(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

NP2_inline void SciCall_MarkerSetFore(int markerNumber, COLORREF fore) {
	SciCall(SCI_MARKERSETFORE, markerNumber, fore);
}

NP2_inline void SciCall_MarkerSetBack(int markerNumber, COLORREF back) {
	SciCall(SCI_MARKERSETBACK, markerNumber, back);
}

NP2_inline void SciCall_MarkerEnableHighlight(BOOL enabled) {
	SciCall(SCI_MARKERENABLEHIGHLIGHT, enabled, 0);
}

//=============================================================================
//
//  Indicators
//
//
NP2_inline void SciCall_SetIndicatorCurrent(int indicator) {
	SciCall(SCI_SETINDICATORCURRENT, indicator, 0);
}

NP2_inline void SciCall_IndicatorClearRange(Sci_Position start, Sci_Position length) {
	SciCall(SCI_INDICATORCLEARRANGE, start, length);
}

NP2_inline void SciCall_IndicatorFillRange(Sci_Position start, Sci_Position length) {
	SciCall(SCI_INDICATORFILLRANGE, start, length);
}

//=============================================================================
//
//  Folding
//
//
NP2_inline BOOL SciCall_GetLineVisible(int line) {
	return (BOOL)SciCall(SCI_GETLINEVISIBLE, line, 0);
}

NP2_inline int SciCall_GetFoldLevel(int line) {
	return (int)SciCall(SCI_GETFOLDLEVEL, line, 0);
}

NP2_inline void SciCall_SetFoldFlags(int flags) {
	SciCall(SCI_SETFOLDFLAGS, flags, 0);
}

NP2_inline int SciCall_GetLastChild(int line) {
	return (int)SciCall(SCI_GETLASTCHILD, line, -1);
}

NP2_inline int SciCall_GetFoldParent(int line) {
	return (int)SciCall(SCI_GETFOLDPARENT, line, 0);
}

NP2_inline BOOL SciCall_GetFoldExpanded(int line) {
	return (BOOL)SciCall(SCI_GETFOLDEXPANDED, line, 0);
}

NP2_inline void SciCall_ToggleFold(int line) {
	SciCall(SCI_TOGGLEFOLD, line, 0);
}

NP2_inline void SciCall_ToggleFoldShowText(int line, const char *text) {
	SciCall(SCI_TOGGLEFOLDSHOWTEXT, line, (LPARAM)text);
}

NP2_inline void SciCall_SetDefaultFoldDisplayText(const char *text) {
	SciCall(SCI_SETDEFAULTFOLDDISPLAYTEXT, 0, (LPARAM)text);
}

NP2_inline void SciCall_FoldDisplayTextSetStyle(int style) {
	SciCall(SCI_FOLDDISPLAYTEXTSETSTYLE, style, 0);
}

NP2_inline void SciCall_EnsureVisible(int line) {
	SciCall(SCI_ENSUREVISIBLE, line, 0);
}

//=============================================================================
//
//  Lexer
//
//
NP2_inline void SciCall_SetProperty(const char *key, const char *value) {
	SciCall(SCI_SETPROPERTY, (WPARAM)key, (LPARAM)value);
}

#endif // NOTEPAD2_SCICALL_H_

// End of SciCall.h
