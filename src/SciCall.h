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
#define SciCall0(m)			Scintilla_DirectFunction(g_hScintilla, (m), 0, 0)
#define SciCall1(m, w)		Scintilla_DirectFunction(g_hScintilla, (m), (w), 0)
#define SciCall2(m, w, l)	Scintilla_DirectFunction(g_hScintilla, (m), (w), (l))

//=============================================================================
//
//  Selection and information
//
//
NP2_inline UINT SciCall_GetCodePage(void) {
	return (UINT)SciCall0(SCI_GETCODEPAGE);
}

NP2_inline Sci_Position SciCall_GetLength(void) {
	return (Sci_Position)SciCall0(SCI_GETLENGTH);
}

NP2_inline int SciCall_GetLineCount(void) {
	return (int)SciCall0(SCI_GETLINECOUNT);
}

NP2_inline void SciCall_SetSel(Sci_Position anchor, Sci_Position caret) {
	SciCall2(SCI_SETSEL, anchor, caret);
}

NP2_inline void SciCall_GotoPos(Sci_Position caret) {
	SciCall1(SCI_GOTOPOS, caret);
}

NP2_inline void SciCall_GotoLine(int line) {
	SciCall1(SCI_GOTOLINE, line);
}

NP2_inline Sci_Position SciCall_GetCurrentPos(void) {
	return (Sci_Position)SciCall0(SCI_GETCURRENTPOS);
}

NP2_inline Sci_Position SciCall_PositionBefore(Sci_Position position) {
	return (Sci_Position)SciCall1(SCI_POSITIONBEFORE, position);
}

NP2_inline Sci_Position SciCall_PositionAfter(Sci_Position position) {
	return (Sci_Position)SciCall1(SCI_POSITIONAFTER, position);
}

NP2_inline int SciCall_GetColumn(Sci_Position position) {
	return (int)SciCall1(SCI_GETCOLUMN, position);
}

NP2_inline int SciCall_LineFromPosition(Sci_Position position) {
	return (int)SciCall1(SCI_LINEFROMPOSITION, position);
}

NP2_inline Sci_Position SciCall_PositionFromLine(int line) {
	return (Sci_Position)SciCall1(SCI_POSITIONFROMLINE, line);
}

NP2_inline int SciCall_GetCharAt(Sci_Position position) {
	return (int)SciCall1(SCI_GETCHARAT, position);
}

NP2_inline int SciCall_GetCharacterAndWidth(Sci_Position position, Sci_Position *width) {
	return (int)SciCall2(SCI_GETCHARACTERANDWIDTH, position, (LPARAM)width);
}

NP2_inline int SciCall_GetCharacterAt(Sci_Position position) {
	return (int)SciCall1(SCI_GETCHARACTERANDWIDTH, position);
}

NP2_inline BOOL SciCall_IsAutoCompletionWordCharacter(int ch) {
	return (BOOL)SciCall1(SCI_ISAUTOCOMPLETIONWORDCHARACTER, ch);
}

NP2_inline Sci_Position SciCall_WordStartPosition(Sci_Position position, BOOL onlyWordCharacters) {
	return (Sci_Position)SciCall2(SCI_WORDSTARTPOSITION, position, onlyWordCharacters);
}

NP2_inline Sci_Position SciCall_WordEndPosition(Sci_Position position, BOOL onlyWordCharacters) {
	return (Sci_Position)SciCall2(SCI_WORDENDPOSITION, position, onlyWordCharacters);
}

NP2_inline int SciCall_GetLineLength(int line) {
	return (int)SciCall1(SCI_GETLINE, line);
}

NP2_inline Sci_Position SciCall_GetLineEndPosition(int line) {
	return (Sci_Position)SciCall1(SCI_GETLINEENDPOSITION, line);
}

NP2_inline int SciCall_GetLine(int line, char *buffer) {
	return (int)SciCall2(SCI_GETLINE, line, (LPARAM)buffer);
}

NP2_inline int SciCall_GetSelTextLength(void) {
	return (int)SciCall0(SCI_GETSELTEXT);
}

NP2_inline int SciCall_GetSelText(char *buffer) {
	return (int)SciCall2(SCI_GETSELTEXT, 0, (LPARAM)buffer);
}

NP2_inline int SciCall_CountCharacters(Sci_Position start, Sci_Position end) {
	return (int)SciCall2(SCI_COUNTCHARACTERS, start, end);
}

NP2_inline Sci_Position SciCall_GetSelectionStart(void) {
	return (Sci_Position)SciCall0(SCI_GETSELECTIONSTART);
}

NP2_inline Sci_Position SciCall_GetSelectionEnd(void) {
	return (Sci_Position)SciCall0(SCI_GETSELECTIONEND);
}

NP2_inline int EditGetSelectedLineCount(void) {
	const int iLineStart = SciCall_LineFromPosition(SciCall_GetSelectionStart());
	const int iLineEnd = SciCall_LineFromPosition(SciCall_GetSelectionEnd());
	return iLineEnd - iLineStart + 1;
}

NP2_inline int SciCall_GetSelectionMode(void) {
	return (int)SciCall0(SCI_GETSELECTIONMODE);
}

NP2_inline BOOL SciCall_IsSelectionEmpty(void) {
	return (BOOL)SciCall0(SCI_GETSELECTIONEMPTY);
}

NP2_inline Sci_Position SciCall_FindText(int searchFlags, struct Sci_TextToFind *ft) {
	return (Sci_Position)SciCall2(SCI_FINDTEXT, searchFlags, (LPARAM)ft);
}

NP2_inline int SciCall_GetTextRange(struct Sci_TextRange *tr) {
	return (int)SciCall2(SCI_GETTEXTRANGE, 0, (LPARAM)tr);
}

NP2_inline BOOL SciCall_CanUndo(void) {
	return (BOOL)SciCall0(SCI_CANUNDO);
}

NP2_inline BOOL SciCall_CanRedo(void) {
	return (BOOL)SciCall0(SCI_CANREDO);
}

NP2_inline BOOL SciCall_CanPaste(void) {
	return (BOOL)SciCall0(SCI_CANPASTE);
}

//=============================================================================
//
//  Caret
//
//
NP2_inline BOOL SciCall_GetOvertype(void) {
	return (BOOL)SciCall0(SCI_GETOVERTYPE);
}

NP2_inline void SciCall_ScrollCaret(void) {
	SciCall0(SCI_SCROLLCARET);
}

NP2_inline void SciCall_SetXCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall2(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

NP2_inline void SciCall_SetYCaretPolicy(int caretPolicy, int caretSlop) {
	SciCall2(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

//=============================================================================
//
//  Style definition
//
//
NP2_inline int SciCall_GetStyleAt(Sci_Position position) {
	return (int)SciCall1(SCI_GETSTYLEAT, position);
}

NP2_inline COLORREF SciCall_StyleGetFore(int style) {
	return (COLORREF)SciCall1(SCI_STYLEGETFORE, style);
}

NP2_inline COLORREF SciCall_StyleGetBack(int style) {
	return (COLORREF)SciCall1(SCI_STYLEGETBACK, style);
}

//=============================================================================
//
//  Margins
//
//
NP2_inline void SciCall_SetMarginType(int margin, int marginType) {
	SciCall2(SCI_SETMARGINTYPEN, margin, marginType);
}

NP2_inline void SciCall_SetMarginWidth(int margin, int pixelWidth) {
	SciCall2(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

NP2_inline int SciCall_GetMarginWidth(int margin) {
	return (int)SciCall1(SCI_GETMARGINWIDTHN, margin);
}

NP2_inline int SciCall_TextWidth(int style, const char *text) {
	return (int)SciCall2(SCI_TEXTWIDTH, style, (LPARAM)text);
}

NP2_inline void SciCall_SetMarginMask(int margin, int mask) {
	SciCall2(SCI_SETMARGINMASKN, margin, mask);
}

NP2_inline void SciCall_SetMarginSensitive(int margin, BOOL sensitive) {
	SciCall2(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

NP2_inline void SciCall_SetFoldMarginColour(BOOL useSetting, COLORREF back) {
	SciCall2(SCI_SETFOLDMARGINCOLOUR, useSetting, back);
}

NP2_inline void SciCall_SetFoldMarginHiColour(BOOL useSetting, COLORREF fore) {
	SciCall2(SCI_SETFOLDMARGINHICOLOUR, useSetting, fore);
}

//=============================================================================
//
//  Markers
//
//
NP2_inline void SciCall_MarkerDefine(int markerNumber, int markerSymbol) {
	SciCall2(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

NP2_inline void SciCall_MarkerSetFore(int markerNumber, COLORREF fore) {
	SciCall2(SCI_MARKERSETFORE, markerNumber, fore);
}

NP2_inline void SciCall_MarkerSetBack(int markerNumber, COLORREF back) {
	SciCall2(SCI_MARKERSETBACK, markerNumber, back);
}

NP2_inline void SciCall_MarkerEnableHighlight(BOOL enabled) {
	SciCall1(SCI_MARKERENABLEHIGHLIGHT, enabled);
}

//=============================================================================
//
//  Indicators
//
//
NP2_inline void SciCall_SetIndicatorCurrent(int indicator) {
	SciCall1(SCI_SETINDICATORCURRENT, indicator);
}

NP2_inline void SciCall_IndicatorClearRange(Sci_Position start, Sci_Position length) {
	SciCall2(SCI_INDICATORCLEARRANGE, start, length);
}

NP2_inline void SciCall_IndicatorFillRange(Sci_Position start, Sci_Position length) {
	SciCall2(SCI_INDICATORFILLRANGE, start, length);
}

//=============================================================================
//
//  Folding
//
//
NP2_inline BOOL SciCall_GetLineVisible(int line) {
	return (BOOL)SciCall1(SCI_GETLINEVISIBLE, line);
}

NP2_inline int SciCall_GetFoldLevel(int line) {
	return (int)SciCall1(SCI_GETFOLDLEVEL, line);
}

NP2_inline void SciCall_SetFoldFlags(int flags) {
	SciCall1(SCI_SETFOLDFLAGS, flags);
}

NP2_inline int SciCall_GetLastChild(int line) {
	return (int)SciCall2(SCI_GETLASTCHILD, line, -1);
}

NP2_inline int SciCall_GetFoldParent(int line) {
	return (int)SciCall1(SCI_GETFOLDPARENT, line);
}

NP2_inline BOOL SciCall_GetFoldExpanded(int line) {
	return (BOOL)SciCall1(SCI_GETFOLDEXPANDED, line);
}

NP2_inline void SciCall_ToggleFold(int line) {
	SciCall1(SCI_TOGGLEFOLD, line);
}

NP2_inline void SciCall_ToggleFoldShowText(int line, const char *text) {
	SciCall2(SCI_TOGGLEFOLDSHOWTEXT, line, (LPARAM)text);
}

NP2_inline void SciCall_SetDefaultFoldDisplayText(const char *text) {
	SciCall2(SCI_SETDEFAULTFOLDDISPLAYTEXT, 0, (LPARAM)text);
}

NP2_inline void SciCall_FoldDisplayTextSetStyle(int style) {
	SciCall1(SCI_FOLDDISPLAYTEXTSETSTYLE, style);
}

NP2_inline void SciCall_EnsureVisible(int line) {
	SciCall1(SCI_ENSUREVISIBLE, line);
}

//=============================================================================
//
//  Lexer
//
//
NP2_inline void SciCall_SetProperty(const char *key, const char *value) {
	SciCall2(SCI_SETPROPERTY, (WPARAM)key, (LPARAM)value);
}

#endif // NOTEPAD2_SCICALL_H_

// End of SciCall.h
