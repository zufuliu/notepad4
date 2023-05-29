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
* Actual type for `int` based on ScintillaCall.h.
*
*
******************************************************************************/
#pragma once

#include "Scintilla.h"
#include "compiler.h"

#if defined(__cplusplus) && !NP2_FORCE_COMPILE_C_AS_CPP
extern "C" HANDLE g_hScintilla;
#else
extern HANDLE g_hScintilla;
#endif

NP2_inline void InitScintillaHandle(HWND hwnd) NP2_noexcept {
	g_hScintilla = (HANDLE)SendMessage(hwnd, SCI_GETDIRECTPOINTER, 0, 0);
}

#if defined(__cplusplus)
extern "C"
#endif
LRESULT SCI_METHOD Scintilla_DirectFunction(HANDLE handle, UINT msg, WPARAM wParam, LPARAM lParam);
#define SciCall(m, w, l)	Scintilla_DirectFunction(g_hScintilla, (m), (w), (l))

typedef unsigned int Sci_MarkerMask;

NP2_inline COLORREF ColorAlpha(COLORREF rgb, UINT alpha) NP2_noexcept {
	return rgb | (alpha << 24);
}

NP2_inline Sci_Position min_pos(Sci_Position x, Sci_Position y) NP2_noexcept {
	return (x < y) ? x : y;
}

NP2_inline Sci_Position max_pos(Sci_Position x, Sci_Position y) NP2_noexcept {
	return (x > y) ? x : y;
}

#if defined(_WIN64)
NP2_inline Sci_Position abs_pos(Sci_Position x) NP2_noexcept {
	return llabs(x);
}

NP2_inline void PosToStrW(Sci_Position pos, LPWSTR tch) NP2_noexcept {
	_i64tow(pos, tch, 10);
}

NP2_inline void PosToStrA(Sci_Position pos, LPSTR tch) NP2_noexcept {
	_i64toa(pos, tch, 10);
}
#else
NP2_inline Sci_Position abs_pos(Sci_Position x) NP2_noexcept {
	return labs(x);
}

NP2_inline void PosToStrW(Sci_Position pos, LPWSTR tch) NP2_noexcept {
	_ltow(pos, tch, 10);
}

NP2_inline void PosToStrA(Sci_Position pos, LPSTR tch) NP2_noexcept {
	_ltoa(pos, tch, 10);
}
#endif

// Text retrieval and modification

NP2_inline Sci_Position SciCall_GetText(Sci_Position length, char *text) NP2_noexcept {
	return SciCall(SCI_GETTEXT, length, (LPARAM)text);
}

NP2_inline void SciCall_SetSavePoint(void) NP2_noexcept {
	SciCall(SCI_SETSAVEPOINT, 0, 0);
}

NP2_inline Sci_Position SciCall_GetLine(Sci_Line line, char *text) NP2_noexcept {
	return SciCall(SCI_GETLINE, line, (LPARAM)text);
}

NP2_inline void SciCall_ReplaceSel(const char *text) NP2_noexcept {
	SciCall(SCI_REPLACESEL, 0, (LPARAM)text);
}

NP2_inline void SciCall_SetReadOnly(bool readOnly) NP2_noexcept {
	SciCall(SCI_SETREADONLY, readOnly, 0);
}

NP2_inline bool SciCall_GetReadOnly(void) NP2_noexcept {
	return (bool)SciCall(SCI_GETREADONLY, 0, 0);
}

NP2_inline Sci_Position SciCall_GetTextRangeFull(const struct Sci_TextRangeFull *tr) NP2_noexcept {
	return SciCall(SCI_GETTEXTRANGEFULL, 0, (LPARAM)tr);
}

NP2_inline void SciCall_AddText(Sci_Position length, const char *text) NP2_noexcept {
	SciCall(SCI_ADDTEXT, length, (LPARAM)text);
}

NP2_inline void SciCall_AppendText(Sci_Position length, const char *text) NP2_noexcept {
	SciCall(SCI_APPENDTEXT, length, (LPARAM)text);
}

NP2_inline void SciCall_InsertText(Sci_Position pos, const char *text) NP2_noexcept {
	SciCall(SCI_INSERTTEXT, pos, (LPARAM)text);
}

NP2_inline void SciCall_ClearAll(void) NP2_noexcept {
	SciCall(SCI_CLEARALL, 0, 0);
}

NP2_inline void SciCall_DeleteRange(Sci_Position start, Sci_Position lengthDelete) NP2_noexcept {
	SciCall(SCI_DELETERANGE, start, lengthDelete);
}

NP2_inline void SciCall_ClearDocumentStyle(void) NP2_noexcept {
	SciCall(SCI_CLEARDOCUMENTSTYLE, 0, 0);
}

NP2_inline int SciCall_GetCharAt(Sci_Position position) NP2_noexcept {
	return (int)SciCall(SCI_GETCHARAT, position, 0);
}

NP2_inline int SciCall_GetStyleIndexAt(Sci_Position position) NP2_noexcept {
	return (int)SciCall(SCI_GETSTYLEINDEXAT, position, 0);
}

NP2_inline size_t SciCall_GetStyledTextFull(const struct Sci_TextRangeFull *tr) NP2_noexcept {
	return SciCall(SCI_GETSTYLEDTEXTFULL, 0, (LPARAM)(tr));
}

NP2_inline int SciCall_GetCharacterAndWidth(Sci_Position position, Sci_Position *width) NP2_noexcept {
	return (int)SciCall(SCI_GETCHARACTERANDWIDTH, position, (LPARAM)width);
}

NP2_inline int SciCall_GetCharacterAt(Sci_Position position) NP2_noexcept {
	return (int)SciCall(SCI_GETCHARACTERANDWIDTH, position, 0);
}

// same as CharacterClass in ILexer.h
typedef enum CharacterClass {
	CharacterClass_Space,
	CharacterClass_NewLine,
	CharacterClass_Punctuation,
	CharacterClass_Word,
	CharacterClass_CJKWord
} CharacterClass;

NP2_inline CharacterClass SciCall_GetCharacterClass(UINT character) NP2_noexcept {
	return (CharacterClass)SciCall(SCI_GETCHARACTERCLASS, character, 0);
}

// Searching and replacing

NP2_inline Sci_Position SciCall_GetTargetStart(void) NP2_noexcept {
	return SciCall(SCI_GETTARGETSTART, 0, 0);
}

NP2_inline Sci_Position SciCall_GetTargetEnd(void) NP2_noexcept {
	return SciCall(SCI_GETTARGETEND, 0, 0);
}

NP2_inline void SciCall_SetTargetRange(Sci_Position start, Sci_Position end) NP2_noexcept {
	SciCall(SCI_SETTARGETRANGE, start, end);
}

NP2_inline void SciCall_TargetFromSelection(void) NP2_noexcept {
	SciCall(SCI_TARGETFROMSELECTION, 0, 0);
}

NP2_inline void SciCall_TargetWholeDocument(void) NP2_noexcept {
	SciCall(SCI_TARGETWHOLEDOCUMENT, 0, 0);
}

NP2_inline void SciCall_SetSearchFlags(int searchFlags) NP2_noexcept {
	SciCall(SCI_SETSEARCHFLAGS, searchFlags, 0);
}

NP2_inline Sci_Position SciCall_SearchInTarget(Sci_Position length, const char *text) NP2_noexcept {
	return SciCall(SCI_SEARCHINTARGET, length, (LPARAM)text);
}

NP2_inline Sci_Position SciCall_ReplaceTarget(Sci_Position length, const char *text) NP2_noexcept {
	return SciCall(SCI_REPLACETARGET, length, (LPARAM)text);
}

NP2_inline Sci_Position SciCall_ReplaceTargetRE(Sci_Position length, const char *text) NP2_noexcept {
	return SciCall(SCI_REPLACETARGETRE, length, (LPARAM)text);
}

NP2_inline Sci_Position SciCall_FindTextFull(int searchFlags, struct Sci_TextToFindFull *ft) NP2_noexcept {
	return SciCall(SCI_FINDTEXTFULL, searchFlags, (LPARAM)ft);
}

NP2_inline Sci_Position SciCall_ReplaceTargetEx(BOOL regex, Sci_Position length, const char *text) NP2_noexcept {
	return SciCall(regex ? SCI_REPLACETARGETRE : SCI_REPLACETARGET, length, (LPARAM)text);
}

// Overtype

NP2_inline BOOL SciCall_GetOvertype(void) NP2_noexcept {
	return (BOOL)SciCall(SCI_GETOVERTYPE, 0, 0);
}

// Cut, copy and paste

NP2_inline void SciCall_Cut(bool asBinary) NP2_noexcept {
	SciCall(SCI_CUT, asBinary, 0);
}

NP2_inline void SciCall_Copy(bool asBinary) NP2_noexcept {
	SciCall(SCI_COPY, asBinary, 0);
}

NP2_inline void SciCall_Paste(bool asBinary) NP2_noexcept {
	SciCall(SCI_PASTE, asBinary, 0);
}

NP2_inline void SciCall_Clear(void) NP2_noexcept {
	SciCall(SCI_CLEAR, 0, 0);
}

NP2_inline bool SciCall_CanPaste(void) NP2_noexcept {
	return (bool)SciCall(SCI_CANPASTE, 0, 0);
}

NP2_inline void SciCall_CopyAllowLine(void) NP2_noexcept {
	SciCall(SCI_COPYALLOWLINE, 0, 0);
}

NP2_inline void SciCall_CopyRange(Sci_Position start, Sci_Position end) NP2_noexcept {
	SciCall(SCI_COPYRANGE, start, end);
}

NP2_inline void SciCall_SetPasteConvertEndings(bool convert) NP2_noexcept {
	SciCall(SCI_SETPASTECONVERTENDINGS, convert, 0);
}

// Undo and Redo

NP2_inline void SciCall_Undo(void) NP2_noexcept {
	SciCall(SCI_UNDO, 0, 0);
}

NP2_inline bool SciCall_CanUndo(void) NP2_noexcept {
	return (bool)SciCall(SCI_CANUNDO, 0, 0);
}

NP2_inline void SciCall_EmptyUndoBuffer(void) NP2_noexcept {
	SciCall(SCI_EMPTYUNDOBUFFER, 0, 0);
}

NP2_inline void SciCall_Redo(void) NP2_noexcept {
	SciCall(SCI_REDO, 0, 0);
}

NP2_inline bool SciCall_CanRedo(void) NP2_noexcept {
	return (bool)SciCall(SCI_CANREDO, 0, 0);
}

NP2_inline void SciCall_SetUndoCollection(bool collectUndo) NP2_noexcept {
	SciCall(SCI_SETUNDOCOLLECTION, collectUndo, 0);
}

NP2_inline void SciCall_BeginUndoAction(void) NP2_noexcept {
	SciCall(SCI_BEGINUNDOACTION, 0, 0);
}

NP2_inline void SciCall_EndUndoAction(void) NP2_noexcept {
	SciCall(SCI_ENDUNDOACTION, 0, 0);
}

// Selection and information

NP2_inline Sci_Position SciCall_GetLength(void) NP2_noexcept {
	return SciCall(SCI_GETLENGTH, 0, 0);
}

NP2_inline Sci_Line SciCall_GetLineCount(void) NP2_noexcept {
	return SciCall(SCI_GETLINECOUNT, 0, 0);
}

NP2_inline void SciCall_AllocateLines(Sci_Line lineCount) NP2_noexcept {
	SciCall(SCI_ALLOCATELINES, lineCount, 0);
}

NP2_inline void SciCall_SetSel(Sci_Position anchor, Sci_Position caret) NP2_noexcept {
	SciCall(SCI_SETSEL, anchor, caret);
}

NP2_inline void SciCall_GotoPos(Sci_Position caret) NP2_noexcept {
	SciCall(SCI_GOTOPOS, caret, 0);
}

NP2_inline void SciCall_GotoLine(Sci_Line line) NP2_noexcept {
	SciCall(SCI_GOTOLINE, line, 0);
}

NP2_inline void SciCall_SetCurrentPos(Sci_Position caret) NP2_noexcept {
	SciCall(SCI_SETCURRENTPOS, caret, 0);
}

NP2_inline Sci_Position SciCall_GetCurrentPos(void) NP2_noexcept {
	return SciCall(SCI_GETCURRENTPOS, 0, 0);
}

NP2_inline void SciCall_SetAnchor(Sci_Position anchor) NP2_noexcept {
	SciCall(SCI_SETANCHOR, anchor, 0);
}

NP2_inline Sci_Position SciCall_GetAnchor(void) NP2_noexcept {
	return SciCall(SCI_GETANCHOR, 0, 0);
}

NP2_inline void SciCall_SetSelectionStart(Sci_Position anchor) NP2_noexcept {
	SciCall(SCI_SETSELECTIONSTART, anchor, 0);
}

NP2_inline Sci_Position SciCall_GetSelectionStart(void) NP2_noexcept {
	return SciCall(SCI_GETSELECTIONSTART, 0, 0);
}

NP2_inline void SciCall_SetSelectionEnd(Sci_Position caret) NP2_noexcept {
	SciCall(SCI_SETSELECTIONEND, caret, 0);
}

NP2_inline Sci_Position SciCall_GetSelectionEnd(void) NP2_noexcept {
	return SciCall(SCI_GETSELECTIONEND, 0, 0);
}

NP2_inline void SciCall_SelectAll(void) NP2_noexcept {
	SciCall(SCI_SELECTALL, 0, 0);
}

NP2_inline Sci_Line SciCall_LineFromPosition(Sci_Position position) NP2_noexcept {
	return SciCall(SCI_LINEFROMPOSITION, position, 0);
}

NP2_inline Sci_Position SciCall_PositionFromLine(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_POSITIONFROMLINE, line, 0);
}

NP2_inline Sci_Position SciCall_GetLineEndPosition(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_GETLINEENDPOSITION, line, 0);
}

NP2_inline Sci_Position SciCall_GetLineLength(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_LINELENGTH, line, 0);
}

NP2_inline Sci_Position SciCall_GetSelTextLength(void) NP2_noexcept {
	return SciCall(SCI_GETSELTEXT, 0, 0);
}

NP2_inline Sci_Position SciCall_GetSelText(char *buffer) NP2_noexcept {
	return SciCall(SCI_GETSELTEXT, FALSE, (LPARAM)buffer);
}

NP2_inline Sci_Position SciCall_GetSelBytes(char *buffer) NP2_noexcept {
	return SciCall(SCI_GETSELTEXT, TRUE, (LPARAM)buffer);
}

NP2_inline bool SciCall_IsRectangleSelection(void) NP2_noexcept {
	return (bool)SciCall(SCI_SELECTIONISRECTANGLE, 0, 0);
}

NP2_inline void SciCall_SetSelectionMode(int selectionMode) NP2_noexcept {
	SciCall(SCI_SETSELECTIONMODE, selectionMode, 0);
}

NP2_inline int SciCall_GetSelectionMode(void) NP2_noexcept {
	return (int)SciCall(SCI_GETSELECTIONMODE, 0, 0);
}

NP2_inline Sci_Position SciCall_GetLineSelStartPosition(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_GETLINESELSTARTPOSITION, line, 0);
}

NP2_inline Sci_Position SciCall_GetLineSelEndPosition(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_GETLINESELENDPOSITION, line, 0);
}

NP2_inline Sci_Position SciCall_PositionBefore(Sci_Position position) NP2_noexcept {
	return SciCall(SCI_POSITIONBEFORE, position, 0);
}

NP2_inline Sci_Position SciCall_PositionAfter(Sci_Position position) NP2_noexcept {
	return SciCall(SCI_POSITIONAFTER, position, 0);
}

NP2_inline int SciCall_TextWidth(int style, const char *text) NP2_noexcept {
	return (int)SciCall(SCI_TEXTWIDTH, style, (LPARAM)text);
}

NP2_inline int SciCall_TextHeight(void) NP2_noexcept {
	return (int)SciCall(SCI_TEXTHEIGHT, 0, 0);
}

NP2_inline Sci_Position SciCall_GetColumn(Sci_Position position) NP2_noexcept {
	return SciCall(SCI_GETCOLUMN, position, 0);
}

NP2_inline Sci_Position SciCall_FindColumn(Sci_Line line, Sci_Position column) NP2_noexcept {
	return SciCall(SCI_FINDCOLUMN, line, column);
}

NP2_inline Sci_Position SciCall_PositionFromPoint(int x, int y) NP2_noexcept {
	return SciCall(SCI_POSITIONFROMPOINT, x, y);
}

NP2_inline int SciCall_PointXFromPosition(Sci_Position pos) NP2_noexcept {
	return (int)SciCall(SCI_POINTXFROMPOSITION, 0, pos);
}

NP2_inline int SciCall_PointYFromPosition(Sci_Position pos) NP2_noexcept {
	return (int)SciCall(SCI_POINTYFROMPOSITION, 0, pos);
}

NP2_inline void SciCall_ChooseCaretX(void) NP2_noexcept {
	SciCall(SCI_CHOOSECARETX, 0, 0);
}

NP2_inline void SciCall_MoveSelectedLinesUp(void) NP2_noexcept {
	SciCall(SCI_MOVESELECTEDLINESUP, 0, 0);
}

NP2_inline void SciCall_MoveSelectedLinesDown(void) NP2_noexcept {
	SciCall(SCI_MOVESELECTEDLINESDOWN, 0, 0);
}

NP2_inline Sci_Line EditGetSelectedLineCount(void) NP2_noexcept {
	const Sci_Line iLineStart = SciCall_LineFromPosition(SciCall_GetSelectionStart());
	const Sci_Line iLineEnd = SciCall_LineFromPosition(SciCall_GetSelectionEnd());
	return iLineEnd - iLineStart + 1;
}

// By character or UTF-16 code unit

NP2_inline Sci_Position SciCall_CountCharacters(Sci_Position start, Sci_Position end) NP2_noexcept {
	return SciCall(SCI_COUNTCHARACTERS, start, end);
}

NP2_inline void SciCall_CountCharactersAndColumns(struct Sci_TextToFindFull *ft) NP2_noexcept {
	SciCall(SCI_COUNTCHARACTERSANDCOLUMNS, 0, (LPARAM)ft);
}

// Multiple Selection and Virtual Space

NP2_inline void SciCall_SetMultipleSelection(bool multipleSelection) NP2_noexcept {
	SciCall(SCI_SETMULTIPLESELECTION, multipleSelection, 0);
}

NP2_inline void SciCall_SetAdditionalSelectionTyping(bool additionalSelectionTyping) NP2_noexcept {
	SciCall(SCI_SETADDITIONALSELECTIONTYPING, additionalSelectionTyping, 0);
}

NP2_inline void SciCall_SetMultiPaste(int multiPaste) NP2_noexcept {
	SciCall(SCI_SETMULTIPASTE, multiPaste, 0);
}

NP2_inline void SciCall_SetVirtualSpaceOptions(int virtualSpaceOptions) NP2_noexcept {
	SciCall(SCI_SETVIRTUALSPACEOPTIONS, virtualSpaceOptions, 0);
}

NP2_inline size_t SciCall_GetSelectionCount(void) NP2_noexcept {
	return SciCall(SCI_GETSELECTIONS, 0, 0);
}

NP2_inline bool SciCall_IsMultipleSelection(void) NP2_noexcept {
	return SciCall(SCI_GETSELECTIONS, 0, 0) > 1;
}

NP2_inline bool SciCall_IsSelectionEmpty(void) NP2_noexcept {
	return (bool)SciCall(SCI_GETSELECTIONEMPTY, 0, 0);
}

NP2_inline void SciCall_ClearSelections(void) NP2_noexcept {
	SciCall(SCI_CLEARSELECTIONS, 0, 0);
}

NP2_inline void SciCall_SetSelection(Sci_Position caret, Sci_Position anchor) NP2_noexcept {
	SciCall(SCI_SETSELECTION, caret, anchor);
}

NP2_inline void SciCall_AddSelection(Sci_Position caret, Sci_Position anchor) NP2_noexcept {
	SciCall(SCI_ADDSELECTION, caret, anchor);
}

NP2_inline void SciCall_SetMainSelection(size_t selection) NP2_noexcept {
	SciCall(SCI_SETMAINSELECTION, selection, 0);
}

NP2_inline void SciCall_SetRectangularSelectionCaret(Sci_Position caret) NP2_noexcept {
	SciCall(SCI_SETRECTANGULARSELECTIONCARET, caret, 0);
}

NP2_inline void SciCall_SetRectangularSelectionAnchor(Sci_Position anchor) NP2_noexcept {
	SciCall(SCI_SETRECTANGULARSELECTIONANCHOR, anchor, 0);
}

NP2_inline void SciCall_SetAdditionalCaretsBlink(bool additionalCaretsBlink) NP2_noexcept {
	SciCall(SCI_SETADDITIONALCARETSBLINK, additionalCaretsBlink, 0);
}

NP2_inline void SciCall_SetAdditionalCaretsVisible(bool additionalCaretsVisible) NP2_noexcept {
	SciCall(SCI_SETADDITIONALCARETSVISIBLE, additionalCaretsVisible, 0);
}

// Scrolling and automatic scrolling

NP2_inline Sci_Line SciCall_GetFirstVisibleLine(void) NP2_noexcept {
	return SciCall(SCI_GETFIRSTVISIBLELINE, 0, 0);
}

NP2_inline Sci_Line SciCall_SetFirstVisibleLine(Sci_Line displayLine) NP2_noexcept {
	return SciCall(SCI_SETFIRSTVISIBLELINE, displayLine, 0);
}

NP2_inline void SciCall_SetXOffset(int xOffset) NP2_noexcept {
	SciCall(SCI_SETXOFFSET, xOffset, 0);
}

NP2_inline int SciCall_GetXOffset(void) NP2_noexcept {
	return (int)SciCall(SCI_GETXOFFSET, 0, 0);
}

NP2_inline void SciCall_LineScroll(Sci_Position columns, Sci_Line lines) NP2_noexcept {
	SciCall(SCI_LINESCROLL, columns, lines);
}

NP2_inline void SciCall_ScrollCaret(void) NP2_noexcept {
	SciCall(SCI_SCROLLCARET, 0, 0);
}

NP2_inline void SciCall_SetXCaretPolicy(int caretPolicy, int caretSlop) NP2_noexcept {
	SciCall(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

NP2_inline void SciCall_SetYCaretPolicy(int caretPolicy, int caretSlop) NP2_noexcept {
	SciCall(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

NP2_inline void SciCall_SetScrollWidthTracking(bool tracking) NP2_noexcept {
	SciCall(SCI_SETSCROLLWIDTHTRACKING, tracking, 0);
}

NP2_inline void SciCall_SetEndAtLastLine(int endAtLastLine) NP2_noexcept {
	SciCall(SCI_SETENDATLASTLINE, endAtLastLine, 0);
}

// White space

NP2_inline void SciCall_SetViewWS(int viewWS) NP2_noexcept {
	SciCall(SCI_SETVIEWWS, viewWS, 0);
}

NP2_inline void SciCall_SetWhitespaceSize(int size) NP2_noexcept {
	SciCall(SCI_SETWHITESPACESIZE, size, 0);
}

NP2_inline void SciCall_SetExtraAscent(int extraAscent) NP2_noexcept {
	SciCall(SCI_SETEXTRAASCENT, extraAscent, 0);
}

NP2_inline void SciCall_SetExtraDescent(int extraDescent) NP2_noexcept {
	SciCall(SCI_SETEXTRADESCENT, extraDescent, 0);
}

// Cursor

NP2_inline void SciCall_SetCursor(int cursorType) NP2_noexcept {
	SciCall(SCI_SETCURSOR, cursorType, 0);
}

NP2_inline void BeginWaitCursor(void) NP2_noexcept {
	SciCall_SetCursor(SC_CURSORWAIT);
}

NP2_inline void EndWaitCursor(void) NP2_noexcept {
	POINT pt;
	SciCall_SetCursor(SC_CURSORNORMAL);
	GetCursorPos(&pt);
	SetCursorPos(pt.x, pt.y);
}

// Line endings

NP2_inline void SciCall_SetEOLMode(int eolMode) NP2_noexcept {
	SciCall(SCI_SETEOLMODE, eolMode, 0);
}

NP2_inline int SciCall_GetEOLMode(void) NP2_noexcept {
	return (int)SciCall(SCI_GETEOLMODE, 0, 0);
}

NP2_inline void SciCall_ConvertEOLs(int eolMode) NP2_noexcept {
	SciCall(SCI_CONVERTEOLS, eolMode, 0);
}

NP2_inline void SciCall_SetViewEOL(bool visible) NP2_noexcept {
	SciCall(SCI_SETVIEWEOL, visible, 0);
}

// Words

NP2_inline Sci_Position SciCall_WordStartPosition(Sci_Position position, bool onlyWordCharacters) NP2_noexcept {
	return SciCall(SCI_WORDSTARTPOSITION, position, onlyWordCharacters);
}

NP2_inline Sci_Position SciCall_WordEndPosition(Sci_Position position, bool onlyWordCharacters) NP2_noexcept {
	return SciCall(SCI_WORDENDPOSITION, position, onlyWordCharacters);
}

NP2_inline void SciCall_SetCharClassesEx(int length, const unsigned char *characters) NP2_noexcept {
	SciCall(SCI_SETCHARCLASSESEX, length, (LPARAM)characters);
}

// Styling

NP2_inline Sci_Position SciCall_GetEndStyled(void) NP2_noexcept {
	return SciCall(SCI_GETENDSTYLED, 0, 0);
}

NP2_inline void SciCall_SetIdleStyling(int idleStyling) NP2_noexcept {
	SciCall(SCI_SETIDLESTYLING, idleStyling, 0);
}

NP2_inline void SciCall_StartStyling(Sci_Position start) NP2_noexcept {
	SciCall(SCI_STARTSTYLING, start, 0);
}

NP2_inline int SciCall_GetLineState(Sci_Line line) NP2_noexcept {
	return (int)SciCall(SCI_GETLINESTATE, line, 0);
}

// Style definition

NP2_inline void SciCall_StyleResetDefault(void) NP2_noexcept {
	SciCall(SCI_STYLERESETDEFAULT, 0, 0);
}

NP2_inline void SciCall_StyleClearAll(void) NP2_noexcept {
	SciCall(SCI_STYLECLEARALL, 0, 0);
}

NP2_inline void SciCall_CopyStyles(size_t sourceIndex, LPARAM destStyles) NP2_noexcept {
	SciCall(SCI_COPYSTYLES, sourceIndex, destStyles);
}

NP2_inline void SciCall_StyleSetFont(int style, const char *fontName) NP2_noexcept {
	SciCall(SCI_STYLESETFONT, style, (LPARAM)fontName);
}

NP2_inline void SciCall_StyleGetFont(int style, char *fontName) NP2_noexcept {
	SciCall(SCI_STYLEGETFONT, style, (LPARAM)fontName);
}

NP2_inline void SciCall_StyleSetSizeFractional(int style, int sizeHundredthPoints) NP2_noexcept {
	SciCall(SCI_STYLESETSIZEFRACTIONAL, style, sizeHundredthPoints);
}

NP2_inline int SciCall_StyleGetSizeFractional(int style) NP2_noexcept {
	return (int)SciCall(SCI_STYLEGETSIZEFRACTIONAL, style, 0);
}

NP2_inline void SciCall_StyleSetWeight(int style, int weight) NP2_noexcept {
	SciCall(SCI_STYLESETWEIGHT, style, weight);
}

NP2_inline int SciCall_StyleGetWeight(int style) NP2_noexcept {
	return (int)SciCall(SCI_STYLEGETWEIGHT, style, 0);
}

NP2_inline void SciCall_StyleSetItalic(int style, bool italic) NP2_noexcept {
	SciCall(SCI_STYLESETITALIC, style, italic);
}

NP2_inline bool SciCall_StyleGetItalic(int style) NP2_noexcept {
	return (bool)SciCall(SCI_STYLEGETITALIC, style, 0);
}

NP2_inline void SciCall_StyleSetUnderline(int style, bool underline) NP2_noexcept {
	SciCall(SCI_STYLESETUNDERLINE, style, underline);
}

NP2_inline bool SciCall_StyleGetUnderline(int style) NP2_noexcept {
	return (bool)SciCall(SCI_STYLEGETUNDERLINE, style, 0);
}

NP2_inline void SciCall_StyleSetStrike(int style, bool strike) NP2_noexcept {
	SciCall(SCI_STYLESETSTRIKE, style, strike);
}

NP2_inline bool SciCall_StyleGetStrike(int style) NP2_noexcept {
	return (bool)SciCall(SCI_STYLEGETSTRIKE, style, 0);
}

NP2_inline void SciCall_StyleSetOverline(int style, bool overline) NP2_noexcept {
	SciCall(SCI_STYLESETOVERLINE, style, overline);
}

NP2_inline void SciCall_StyleSetFore(int style, COLORREF fore) NP2_noexcept {
	SciCall(SCI_STYLESETFORE, style, fore);
}

NP2_inline COLORREF SciCall_StyleGetFore(int style) NP2_noexcept {
	return (COLORREF)SciCall(SCI_STYLEGETFORE, style, 0);
}

NP2_inline void SciCall_StyleSetBack(int style, COLORREF back) NP2_noexcept {
	SciCall(SCI_STYLESETBACK, style, back);
}

NP2_inline COLORREF SciCall_StyleGetBack(int style) NP2_noexcept {
	return (COLORREF)SciCall(SCI_STYLEGETBACK, style, 0);
}

NP2_inline void SciCall_StyleSetEOLFilled(int style, bool eolFilled) NP2_noexcept {
	SciCall(SCI_STYLESETEOLFILLED, style, eolFilled);
}

NP2_inline bool SciCall_StyleGetEOLFilled(int style) NP2_noexcept {
	return (bool)SciCall(SCI_STYLEGETEOLFILLED, style, 0);
}

NP2_inline void SciCall_StyleSetCharacterSet(int style, int characterSet) NP2_noexcept {
	SciCall(SCI_STYLESETCHARACTERSET, style, characterSet);
}

NP2_inline int SciCall_StyleGetCharacterSet(int style) NP2_noexcept {
	return (int)SciCall(SCI_STYLEGETCHARACTERSET, style, 0);
}

NP2_inline void SciCall_StyleSetHotSpot(int style, bool hotspot) NP2_noexcept {
	SciCall(SCI_STYLESETHOTSPOT, style, hotspot);
}

NP2_inline bool SciCall_StyleGetHotSpot(int style) NP2_noexcept {
	return (bool)SciCall(SCI_STYLEGETHOTSPOT, style, 0);
}

NP2_inline void SciCall_StyleSetCheckMonospaced(int style, bool checkMonospaced) NP2_noexcept {
	SciCall(SCI_STYLESETCHECKMONOSPACED, style, checkMonospaced);
}

// Caret, selection, and hotspot styles

NP2_inline void SciCall_SetElementColor(int element, COLORREF color) NP2_noexcept {
	SciCall(SCI_SETELEMENTCOLOUR, element, color);
}

NP2_inline COLORREF SciCall_GetElementColour(int element) NP2_noexcept {
	return (COLORREF)SciCall(SCI_GETELEMENTCOLOUR, element, 0);
}

NP2_inline void SciCall_ResetElementColor(int element) NP2_noexcept {
	SciCall(SCI_RESETELEMENTCOLOUR, element, 0);
}

NP2_inline void SciCall_SetSelectionLayer(int layer) NP2_noexcept {
	SciCall(SCI_SETSELECTIONLAYER, layer, 0);
}

NP2_inline void SciCall_SetSelEOLFilled(bool filled) NP2_noexcept {
	SciCall(SCI_SETSELEOLFILLED, filled, 0);
}

NP2_inline void SciCall_SetEOLSelectedWidth(int percent) NP2_noexcept {
	SciCall(SCI_SETEOLSELECTEDWIDTH, percent, 0);
}

NP2_inline void SciCall_SetCaretLineLayer(int layer) NP2_noexcept {
	SciCall(SCI_SETCARETLINELAYER, layer, 0);
}

NP2_inline void SciCall_SetCaretLineFrame(int width) NP2_noexcept {
	SciCall(SCI_SETCARETLINEFRAME, width, 0);
}

NP2_inline void SciCall_SetCaretLineVisibleAlways(bool alwaysVisible) NP2_noexcept {
	SciCall(SCI_SETCARETLINEVISIBLEALWAYS, alwaysVisible, 0);
}

NP2_inline void SciCall_SetCaretLineHighlightSubLine(bool subLine) NP2_noexcept {
	SciCall(SCI_SETCARETLINEHIGHLIGHTSUBLINE, subLine, 0);
}

NP2_inline void SciCall_SetCaretPeriod(int periodMilliseconds) NP2_noexcept {
	SciCall(SCI_SETCARETPERIOD, periodMilliseconds, 0);
}

NP2_inline void SciCall_SetCaretStyle(int caretStyle) NP2_noexcept {
	SciCall(SCI_SETCARETSTYLE, caretStyle, 0);
}

NP2_inline void SciCall_SetCaretWidth(int pixelWidth) NP2_noexcept {
	SciCall(SCI_SETCARETWIDTH, pixelWidth, 0);
}

NP2_inline void SciCall_SetCaretSticky(int useCaretStickyBehaviour) NP2_noexcept {
	SciCall(SCI_SETCARETSTICKY, useCaretStickyBehaviour, 0);
}

// Character representations

NP2_inline void SciCall_SetRepresentation(const char *encodedCharacter, const char *representation) NP2_noexcept {
	SciCall(SCI_SETREPRESENTATION, (WPARAM)encodedCharacter, (LPARAM)representation);
}

NP2_inline void SciCall_ClearRepresentation(const char *encodedCharacter) NP2_noexcept {
	SciCall(SCI_CLEARREPRESENTATION, (WPARAM)encodedCharacter, 0);
}

// Margins

NP2_inline void SciCall_SetMarginType(int margin, int marginType) NP2_noexcept {
	SciCall(SCI_SETMARGINTYPEN, margin, marginType);
}

NP2_inline void SciCall_SetMarginWidth(int margin, int pixelWidth) NP2_noexcept {
	SciCall(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

NP2_inline int SciCall_GetMarginWidth(int margin) NP2_noexcept {
	return (int)SciCall(SCI_GETMARGINWIDTHN, margin, 0);
}

NP2_inline void SciCall_SetMarginMask(int margin, int mask) NP2_noexcept {
	SciCall(SCI_SETMARGINMASKN, margin, mask);
}

NP2_inline void SciCall_SetMarginSensitive(int margin, bool sensitive) NP2_noexcept {
	SciCall(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

NP2_inline void SciCall_SetMarginCursor(int margin, int cursor) NP2_noexcept {
	SciCall(SCI_SETMARGINCURSORN, margin, cursor);
}

NP2_inline void SciCall_SetFoldMarginColor(bool useSetting, COLORREF back) NP2_noexcept {
	SciCall(SCI_SETFOLDMARGINCOLOUR, useSetting, back);
}

NP2_inline void SciCall_SetFoldMarginHiColor(bool useSetting, COLORREF fore) NP2_noexcept {
	SciCall(SCI_SETFOLDMARGINHICOLOUR, useSetting, fore);
}

NP2_inline void SciCall_SetMarginOptions(int marginOptions) NP2_noexcept {
	SciCall(SCI_SETMARGINOPTIONS, marginOptions, 0);
}

// Other settings

NP2_inline void SciCall_SetCodePage(UINT codePage) NP2_noexcept {
	SciCall(SCI_SETCODEPAGE, codePage, 0);
}

NP2_inline UINT SciCall_GetCodePage(void) NP2_noexcept {
	return (UINT)SciCall(SCI_GETCODEPAGE, 0, 0);
}

NP2_inline void SciCall_SetTechnology(int technology) NP2_noexcept {
	SciCall(SCI_SETTECHNOLOGY, technology, 0);
}

NP2_inline int SciCall_GetTechnology(void) NP2_noexcept {
	return (int)SciCall(SCI_GETTECHNOLOGY, 0, 0);
}

NP2_inline void SciCall_SetBidirectional(int bidirectional) NP2_noexcept {
	SciCall(SCI_SETBIDIRECTIONAL, bidirectional, 0);
}

NP2_inline int SciCall_GetBidirectional(void) NP2_noexcept {
	return (int)SciCall(SCI_GETBIDIRECTIONAL, 0, 0);
}

NP2_inline void SciCall_SetFontQuality(int fontQuality) NP2_noexcept {
	SciCall(SCI_SETFONTQUALITY, fontQuality, 0);
}

NP2_inline void SciCall_SetFontLocale(const char *localeName) NP2_noexcept {
	SciCall(SCI_SETFONTLOCALE, 0, (LPARAM)localeName);
}

NP2_inline void SciCall_SetIMEInteraction(int imeInteraction) NP2_noexcept {
	SciCall(SCI_SETIMEINTERACTION, imeInteraction, 0);
}


// Brace highlighting

NP2_inline void SciCall_BraceHighlight(Sci_Position posA, Sci_Position posB) NP2_noexcept {
	SciCall(SCI_BRACEHIGHLIGHT, posA, posB);
}

NP2_inline void SciCall_BraceHighlightIndicator(bool useSetting, int indicator) NP2_noexcept {
	SciCall(SCI_BRACEHIGHLIGHTINDICATOR, useSetting, indicator);
}

NP2_inline void SciCall_BraceBadLight(Sci_Position pos) NP2_noexcept {
	SciCall(SCI_BRACEBADLIGHT, pos, 0);
}

NP2_inline void SciCall_BraceBadLightIndicator(bool useSetting, int indicator) NP2_noexcept {
	SciCall(SCI_BRACEBADLIGHTINDICATOR, useSetting, indicator);
}

NP2_inline Sci_Position SciCall_BraceMatch(Sci_Position pos) NP2_noexcept {
	return SciCall(SCI_BRACEMATCH, pos, 0);
}

NP2_inline Sci_Position SciCall_BraceMatchNext(Sci_Position pos, Sci_Position startPos) NP2_noexcept {
	return SciCall(SCI_BRACEMATCHNEXT, pos, startPos);
}

// Tabs and Indentation Guides

NP2_inline void SciCall_SetTabWidth(int tabWidth) NP2_noexcept {
	SciCall(SCI_SETTABWIDTH, tabWidth, 0);
}

NP2_inline int SciCall_GetTabWidth(void) NP2_noexcept {
	return (int)SciCall(SCI_GETTABWIDTH, 0, 0);
}

NP2_inline void SciCall_SetTabMinimumWidth(int pixels) NP2_noexcept {
	SciCall(SCI_SETTABMINIMUMWIDTH, pixels, 0);
}

NP2_inline void SciCall_SetUseTabs(bool useTabs) NP2_noexcept {
	SciCall(SCI_SETUSETABS, useTabs, 0);
}

NP2_inline bool SciCall_GetUseTabs() NP2_noexcept {
	return (bool)SciCall(SCI_GETUSETABS, 0, 0);
}

NP2_inline void SciCall_SetIndent(int indentSize) NP2_noexcept {
	SciCall(SCI_SETINDENT, indentSize, 0);
}

NP2_inline void SciCall_SetTabIndents(bool tabIndents) NP2_noexcept {
	SciCall(SCI_SETTABINDENTS, tabIndents, 0);
}

NP2_inline void SciCall_SetBackSpaceUnIndents(uint8_t bsUnIndents) NP2_noexcept {
	SciCall(SCI_SETBACKSPACEUNINDENTS, bsUnIndents, 0);
}

NP2_inline void SciCall_SetLineIndentation(Sci_Line line, Sci_Position indentation) NP2_noexcept {
	SciCall(SCI_SETLINEINDENTATION, line, indentation);
}

NP2_inline Sci_Position SciCall_GetLineIndentation(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_GETLINEINDENTATION, line, 0);
}

NP2_inline Sci_Position SciCall_GetLineIndentPosition(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_GETLINEINDENTPOSITION, line, 0);
}

NP2_inline void SciCall_SetIndentationGuides(int indentView) NP2_noexcept {
	SciCall(SCI_SETINDENTATIONGUIDES, indentView, 0);
}

NP2_inline int SciCall_GetIndentationGuides(void) NP2_noexcept {
	return (int)SciCall(SCI_GETINDENTATIONGUIDES, 0, 0);
}

NP2_inline void SciCall_SetHighlightGuide(Sci_Position column) NP2_noexcept {
	SciCall(SCI_SETHIGHLIGHTGUIDE, column, 0);
}

// Markers

NP2_inline void SciCall_MarkerDefine(int markerNumber, int markerSymbol) NP2_noexcept {
	SciCall(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

NP2_inline void SciCall_MarkerDefinePixmap(int markerNumber, const char *pixmap) NP2_noexcept {
	SciCall(SCI_MARKERDEFINEPIXMAP, markerNumber, (LPARAM)pixmap);
}

NP2_inline int SciCall_MarkerSymbolDefined(int markerNumber) NP2_noexcept {
	return (int)SciCall(SCI_MARKERSYMBOLDEFINED, markerNumber, 0);
}

NP2_inline void SciCall_MarkerSetForeTranslucent(int markerNumber, COLORREF fore) NP2_noexcept {
	SciCall(SCI_MARKERSETFORETRANSLUCENT, markerNumber, fore);
}

NP2_inline void SciCall_MarkerSetBackTranslucent(int markerNumber, COLORREF back) NP2_noexcept {
	SciCall(SCI_MARKERSETBACKTRANSLUCENT, markerNumber, back);
}

NP2_inline void SciCall_MarkerSetBackSelectedTranslucent(int markerNumber, COLORREF back) NP2_noexcept {
	SciCall(SCI_MARKERSETBACKSELECTEDTRANSLUCENT, markerNumber, back);
}

NP2_inline void SciCall_MarkerSetLayer(int markerNumber, int layer) NP2_noexcept {
	SciCall(SCI_MARKERSETLAYER, markerNumber, layer);
}

NP2_inline void SciCall_MarkerSetStrokeWidth(int markerNumber, int hundredths) NP2_noexcept {
	SciCall(SCI_MARKERSETSTROKEWIDTH, markerNumber, hundredths);
}

NP2_inline void SciCall_MarkerEnableHighlight(bool enabled) NP2_noexcept {
	SciCall(SCI_MARKERENABLEHIGHLIGHT, enabled, 0);
}

NP2_inline int SciCall_MarkerAdd(Sci_Line line, int markerNumber) NP2_noexcept {
	return (int)SciCall(SCI_MARKERADD, line, markerNumber);
}

NP2_inline void SciCall_MarkerDelete(Sci_Line line, int markerNumber) NP2_noexcept {
	SciCall(SCI_MARKERDELETE, line, markerNumber);
}

NP2_inline void SciCall_MarkerDeleteAll(int markerNumber) NP2_noexcept {
	SciCall(SCI_MARKERDELETEALL, markerNumber, 0);
}

NP2_inline void SciCall_ClearMarker(void) NP2_noexcept {
	SciCall_MarkerDeleteAll(-1);
}

NP2_inline Sci_MarkerMask SciCall_MarkerGet(Sci_Line line) NP2_noexcept {
	return (Sci_MarkerMask)SciCall(SCI_MARKERGET, line, 0);
}

NP2_inline Sci_Line SciCall_MarkerNext(Sci_Line line, Sci_MarkerMask markerMask) NP2_noexcept {
	return SciCall(SCI_MARKERNEXT, line, markerMask);
}

NP2_inline Sci_Line SciCall_MarkerPrevious(Sci_Line line, Sci_MarkerMask markerMask) NP2_noexcept {
	return SciCall(SCI_MARKERPREVIOUS, line, markerMask);
}

// Indicators

NP2_inline void SciCall_IndicSetStyle(int indicator, int indicatorStyle) NP2_noexcept {
	SciCall(SCI_INDICSETSTYLE, indicator, indicatorStyle);
}

NP2_inline void SciCall_IndicSetFore(int indicator, COLORREF fore) NP2_noexcept {
	SciCall(SCI_INDICSETFORE, indicator, fore);
}

NP2_inline void SciCall_IndicSetAlpha(int indicator, int alpha) NP2_noexcept {
	SciCall(SCI_INDICSETALPHA, indicator, alpha);
}

NP2_inline void SciCall_IndicSetOutlineAlpha(int indicator, int alpha) NP2_noexcept {
	SciCall(SCI_INDICSETOUTLINEALPHA, indicator, alpha);
}

NP2_inline void SciCall_IndicSetStrokeWidth(int indicator, int hundredths) NP2_noexcept {
	SciCall(SCI_INDICSETSTROKEWIDTH, indicator, hundredths);
}

NP2_inline void SciCall_SetIndicatorCurrent(int indicator) NP2_noexcept {
	SciCall(SCI_SETINDICATORCURRENT, indicator, 0);
}

NP2_inline void SciCall_IndicatorClearRange(Sci_Position start, Sci_Position length) NP2_noexcept {
	SciCall(SCI_INDICATORCLEARRANGE, start, length);
}

NP2_inline void SciCall_IndicatorFillRange(Sci_Position start, Sci_Position length) NP2_noexcept {
	SciCall(SCI_INDICATORFILLRANGE, start, length);
}

// Autocompletion

NP2_inline void SciCall_AutoCShow(Sci_Position lengthEntered, const char *itemList) NP2_noexcept {
	SciCall(SCI_AUTOCSHOW, lengthEntered, (LPARAM)itemList);
}

NP2_inline void SciCall_AutoCCancel(void) NP2_noexcept {
	SciCall(SCI_AUTOCCANCEL, 0, 0);
}

NP2_inline bool SciCall_AutoCActive(void) NP2_noexcept {
	return (bool)SciCall(SCI_AUTOCACTIVE, 0, 0);
}

NP2_inline void SciCall_AutoCSetSeparator(char separatorCharacter) NP2_noexcept {
	SciCall(SCI_AUTOCSETSEPARATOR, separatorCharacter, 0);
}

NP2_inline void SciCall_AutoCSetTypeSeparator(char separatorCharacter) NP2_noexcept {
	SciCall(SCI_AUTOCSETTYPESEPARATOR, separatorCharacter, 0);
}

NP2_inline void SciCall_AutoCSetCancelAtStart(bool cancel) NP2_noexcept {
	SciCall(SCI_AUTOCSETCANCELATSTART, cancel, 0);
}

NP2_inline void SciCall_AutoCSetFillUps(const char *characterSet) NP2_noexcept {
	SciCall(SCI_AUTOCSETFILLUPS, 0, (LPARAM)characterSet);
}

NP2_inline void SciCall_AutoCSetChooseSingle(bool chooseSingle) NP2_noexcept {
	SciCall(SCI_AUTOCSETCHOOSESINGLE, chooseSingle, 0);
}

NP2_inline void SciCall_AutoCSetIgnoreCase(bool ignoreCase) NP2_noexcept {
	SciCall(SCI_AUTOCSETIGNORECASE, ignoreCase, 0);
}

NP2_inline void SciCall_AutoCSetCaseInsensitiveBehaviour(int behaviour) NP2_noexcept {
	SciCall(SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR, behaviour, 0);
}

NP2_inline void SciCall_AutoCSetOrder(int ordere) NP2_noexcept {
	SciCall(SCI_AUTOCSETORDER, ordere, 0);
}

NP2_inline void SciCall_AutoCSetOptions(int options) NP2_noexcept {
	SciCall(SCI_AUTOCSETOPTIONS, options, 0);
}

NP2_inline void SciCall_AutoCSetDropRestOfWord(bool dropRestOfWord) NP2_noexcept {
	SciCall(SCI_AUTOCSETDROPRESTOFWORD, dropRestOfWord, 0);
}

NP2_inline void SciCall_AutoCSetMaxHeight(int rowCount) NP2_noexcept {
	SciCall(SCI_AUTOCSETMAXHEIGHT, rowCount, 0);
}

NP2_inline void SciCall_SetAutoInsertMask(int mask) NP2_noexcept {
	SciCall(SCI_SETAUTOINSERTMASK, mask, 0);
}

// Call tips

NP2_inline void SciCall_CallTipShow(Sci_Position pos, const char *definition) NP2_noexcept {
	SciCall(SCI_CALLTIPSHOW, pos, (LPARAM)definition);
}

NP2_inline void SciCall_CallTipCancel(void) NP2_noexcept {
	SciCall(SCI_CALLTIPCANCEL, 0, 0);
}

NP2_inline bool SciCall_CallTipActive(void) NP2_noexcept {
	return (bool)SciCall(SCI_CALLTIPACTIVE, 0, 0);
}

NP2_inline void SciCall_CallTipSetBack(COLORREF back) NP2_noexcept {
	SciCall(SCI_CALLTIPSETBACK, back, 0);
}

NP2_inline void SciCall_CallTipSetFore(COLORREF fore) NP2_noexcept {
	SciCall(SCI_CALLTIPSETFORE, fore, 0);
}

NP2_inline void SciCall_CallTipUseStyle(int tabSize) NP2_noexcept {
	SciCall(SCI_CALLTIPUSESTYLE, tabSize, 0);
}

NP2_inline void SciCall_ShowNotification(int notifyPos, const char *definition) NP2_noexcept {
	SciCall(SCI_SHOWNOTIFICATION, notifyPos, (LPARAM)definition);
}

// Keyboard commands

NP2_inline void SciCall_VCHome(void) NP2_noexcept {
	SciCall(SCI_VCHOME, 0, 0);
}

NP2_inline void SciCall_DocumentStart(void) NP2_noexcept {
	SciCall(SCI_DOCUMENTSTART, 0, 0);
}

NP2_inline void SciCall_DocumentEnd(void) NP2_noexcept {
	SciCall(SCI_DOCUMENTEND, 0, 0);
}

NP2_inline void SciCall_DeleteBack(void) NP2_noexcept {
	SciCall(SCI_DELETEBACK, 0, 0);
}

NP2_inline void SciCall_DelWordLeft(void) NP2_noexcept {
	SciCall(SCI_DELWORDLEFT, 0, 0);
}

NP2_inline void SciCall_DelWordRight(void) NP2_noexcept {
	SciCall(SCI_DELWORDRIGHT, 0, 0);
}

NP2_inline void SciCall_DelLineLeft(void) NP2_noexcept {
	SciCall(SCI_DELLINELEFT, 0, 0);
}

NP2_inline void SciCall_DelLineRight(void) NP2_noexcept {
	SciCall(SCI_DELLINERIGHT, 0, 0);
}

NP2_inline void SciCall_LineDelete(void) NP2_noexcept {
	SciCall(SCI_LINEDELETE, 0, 0);
}

NP2_inline void SciCall_LineCut(BOOL lineCopy) NP2_noexcept {
	SciCall(SCI_LINECUT, lineCopy, 0);
}

NP2_inline void SciCall_LineCopy(BOOL lineCopy) NP2_noexcept {
	SciCall(SCI_LINECOPY, lineCopy, 0);
}

NP2_inline void SciCall_LineTranspose(void) NP2_noexcept {
	SciCall(SCI_LINETRANSPOSE, 0, 0);
}

NP2_inline void SciCall_LineDuplicate(void) NP2_noexcept {
	SciCall(SCI_LINEDUPLICATE, 0, 0);
}

NP2_inline void SciCall_LowerCase(void) NP2_noexcept {
	SciCall(SCI_LOWERCASE, 0, 0);
}

NP2_inline void SciCall_UpperCase(void) NP2_noexcept {
	SciCall(SCI_UPPERCASE, 0, 0);
}

NP2_inline void SciCall_Cancel(void) NP2_noexcept {
	SciCall(SCI_CANCEL, 0, 0);
}

NP2_inline void SciCall_EditToggleOvertype(void) NP2_noexcept {
	SciCall(SCI_EDITTOGGLEOVERTYPE, 0, 0);
}

NP2_inline void SciCall_NewLine(void) NP2_noexcept {
	SciCall(SCI_NEWLINE, 0, 0);
}

NP2_inline void SciCall_Tab(void) NP2_noexcept {
	SciCall(SCI_TAB, TAB_COMPLETION_DEFAULT, 0);
}

NP2_inline void SciCall_TabCompletion(int what) NP2_noexcept {
	SciCall(SCI_TAB, what, 0);
}

NP2_inline void SciCall_BackTab(void) NP2_noexcept {
	SciCall(SCI_BACKTAB, 0, 0);
}

NP2_inline void SciCall_SelectionDuplicate(void) NP2_noexcept {
	SciCall(SCI_SELECTIONDUPLICATE, 0, 0);
}

// Key bindings

NP2_inline void SciCall_AssignCmdKey(int keyDefinition, int sciCommand) NP2_noexcept {
	SciCall(SCI_ASSIGNCMDKEY, keyDefinition, sciCommand);
}

// Popup edit menu

NP2_inline void SciCall_UsePopUp(int popUpMode) NP2_noexcept {
	SciCall(SCI_USEPOPUP, popUpMode, 0);
}

// Printing

NP2_inline Sci_Position SciCall_FormatRangeFull(bool draw, const struct Sci_RangeToFormatFull *fr) NP2_noexcept {
	return SciCall(SCI_FORMATRANGEFULL, draw, (LPARAM)fr);
}

NP2_inline void SciCall_SetPrintMagnification(int magnification) NP2_noexcept {
	SciCall(SCI_SETPRINTMAGNIFICATION, magnification, 0);
}

NP2_inline void SciCall_SetPrintColorMode(int mode) NP2_noexcept {
	SciCall(SCI_SETPRINTCOLOURMODE, mode, 0);
}

// Direct access

NP2_inline const char* SciCall_GetRangePointer(Sci_Position start, Sci_Position lengthRange) NP2_noexcept {
	return (const char *)SciCall(SCI_GETRANGEPOINTER, start, lengthRange);
}

// Multiple views

NP2_inline void SciCall_SetDocPointer(HANDLE doc) NP2_noexcept {
	SciCall(SCI_SETDOCPOINTER, 0, (LPARAM)doc);
}

NP2_inline HANDLE SciCall_CreateDocument(Sci_Position bytes, int documentOptions) NP2_noexcept {
	return (HANDLE)SciCall(SCI_CREATEDOCUMENT, bytes, documentOptions);
}

NP2_inline void SciCall_ReleaseDocument(HANDLE doc) NP2_noexcept {
	SciCall(SCI_RELEASEDOCUMENT, 0, (LPARAM)doc);
}

NP2_inline int SciCall_GetDocumentOptions(void) NP2_noexcept {
	return (int)SciCall(SCI_GETDOCUMENTOPTIONS, 0, 0);
}

// Folding

NP2_inline Sci_Line SciCall_DocLineFromVisible(Sci_Line displayLine) NP2_noexcept {
	return SciCall(SCI_DOCLINEFROMVISIBLE, displayLine, 0);
}

NP2_inline bool SciCall_GetLineVisible(Sci_Line line) NP2_noexcept {
	return (bool)SciCall(SCI_GETLINEVISIBLE, line, 0);
}

NP2_inline int SciCall_GetFoldLevel(Sci_Line line) NP2_noexcept {
	return (int)SciCall(SCI_GETFOLDLEVEL, line, 0);
}

NP2_inline void SciCall_SetFoldFlags(int flags) NP2_noexcept {
	SciCall(SCI_SETFOLDFLAGS, flags, 0);
}

NP2_inline Sci_Line SciCall_GetLastChild(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_GETLASTCHILD, line, -1);
}

NP2_inline Sci_Line SciCall_GetLastChildEx(Sci_Line line, int level) NP2_noexcept {
	return SciCall(SCI_GETLASTCHILD, line, level);
}

NP2_inline Sci_Line SciCall_GetFoldParent(Sci_Line line) NP2_noexcept {
	return SciCall(SCI_GETFOLDPARENT, line, 0);
}

NP2_inline BOOL SciCall_GetFoldExpanded(Sci_Line line) NP2_noexcept {
	return (BOOL)SciCall(SCI_GETFOLDEXPANDED, line, 0);
}

NP2_inline void SciCall_FoldLine(Sci_Line line, int action) NP2_noexcept {
	SciCall(SCI_FOLDLINE, line, action);
}

NP2_inline void SciCall_FoldAll(int action) NP2_noexcept {
	SciCall(SCI_FOLDALL, action, 0);
}

NP2_inline void SciCall_ToggleFoldShowText(Sci_Line line, const char *text) NP2_noexcept {
	SciCall(SCI_TOGGLEFOLDSHOWTEXT, line, (LPARAM)text);
}

NP2_inline void SciCall_SetDefaultFoldDisplayText(const char *text) NP2_noexcept {
	SciCall(SCI_SETDEFAULTFOLDDISPLAYTEXT, 0, (LPARAM)text);
}

NP2_inline void SciCall_FoldDisplayTextSetStyle(int style) NP2_noexcept {
	SciCall(SCI_FOLDDISPLAYTEXTSETSTYLE, style, 0);
}

NP2_inline void SciCall_ExpandChildren(Sci_Line line, int level) NP2_noexcept {
	SciCall(SCI_EXPANDCHILDREN, line, level);
}

NP2_inline void SciCall_SetAutomaticFold(int automaticFold) NP2_noexcept {
	SciCall(SCI_SETAUTOMATICFOLD, automaticFold, 0);
}

NP2_inline void SciCall_EnsureVisible(Sci_Line line) NP2_noexcept {
	SciCall(SCI_ENSUREVISIBLE, line, 0);
}

// Line wrapping

NP2_inline void SciCall_SetWrapMode(int wrapMode) NP2_noexcept {
	SciCall(SCI_SETWRAPMODE, wrapMode, 0);
}

NP2_inline void SciCall_SetWrapVisualFlags(int wrapVisualFlags) NP2_noexcept {
	SciCall(SCI_SETWRAPVISUALFLAGS, wrapVisualFlags, 0);
}

NP2_inline void SciCall_SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation) NP2_noexcept {
	SciCall(SCI_SETWRAPVISUALFLAGSLOCATION, wrapVisualFlagsLocation, 0);
}

NP2_inline void SciCall_SetWrapIndentMode(int wrapIndentMode) NP2_noexcept {
	SciCall(SCI_SETWRAPINDENTMODE, wrapIndentMode, 0);
}

NP2_inline int SciCall_GetWrapIndentMode(void) NP2_noexcept {
	return (int)SciCall(SCI_GETWRAPINDENTMODE, 0, 0);
}

NP2_inline void SciCall_SetWrapStartIndent(int indent) NP2_noexcept {
	SciCall(SCI_SETWRAPSTARTINDENT, indent, 0);
}

NP2_inline void SciCall_SetLayoutCache(int cacheMode) NP2_noexcept {
	SciCall(SCI_SETLAYOUTCACHE, cacheMode, 0);
}

NP2_inline void SciCall_LinesSplit(int pixelWidth) NP2_noexcept {
	SciCall(SCI_LINESSPLIT, pixelWidth, 0);
}

NP2_inline void SciCall_LinesJoin(void) NP2_noexcept {
	SciCall(SCI_LINESJOIN, 0, 0);
}

// Zooming

NP2_inline void SciCall_SetZoom(int percent) NP2_noexcept {
	SciCall(SCI_SETZOOM, percent, 0);
}

NP2_inline int SciCall_GetZoom(void) NP2_noexcept {
	return (int)SciCall(SCI_GETZOOM, 0, 0);
}

NP2_inline void SciCall_ZoomIn(void) NP2_noexcept {
	SciCall(SCI_ZOOMIN, 0, 0);
}

NP2_inline void SciCall_ZoomOut(void) NP2_noexcept {
	SciCall(SCI_ZOOMOUT, 0, 0);
}

// Long lines

NP2_inline void SciCall_SetEdgeMode(int edgeMode) NP2_noexcept {
	SciCall(SCI_SETEDGEMODE, edgeMode, 0);
}

NP2_inline int SciCall_GetEdgeMode(void) NP2_noexcept {
	return (int)SciCall(SCI_GETEDGEMODE, 0, 0);
}

NP2_inline void SciCall_SetEdgeColumn(int column) NP2_noexcept {
	SciCall(SCI_SETEDGECOLUMN, column, 0);
}

NP2_inline void SciCall_SetEdgeColor(COLORREF edgeColor) NP2_noexcept {
	SciCall(SCI_SETEDGECOLOUR, edgeColor, 0);
}

// Lexer

NP2_inline void SciCall_SetLexer(int lexer) NP2_noexcept { //! removed in Scintilla 5
	SciCall(SCI_SETLEXER, lexer, 0);
}

NP2_inline void SciCall_ColouriseAll(void) NP2_noexcept {
	SciCall(SCI_COLOURISE, 0, -1);
}

NP2_inline void SciCall_EnsureStyledTo(Sci_Position end) NP2_noexcept {
	SciCall(SCI_COLOURISE, 0, end);
}

NP2_inline void SciCall_SetProperty(const char *key, const char *value) NP2_noexcept {
	SciCall(SCI_SETPROPERTY, (WPARAM)key, (LPARAM)value);
}

NP2_inline void SciCall_SetKeywords(int keywordSet, const char *keywords) NP2_noexcept {
	SciCall(SCI_SETKEYWORDS, keywordSet, (LPARAM)keywords);
}

// Notifications

NP2_inline void SciCall_SetModEventMask(int eventMask) NP2_noexcept {
	SciCall(SCI_SETMODEVENTMASK, eventMask, 0);
}

NP2_inline void SciCall_SetCommandEvents(bool commandEvents) NP2_noexcept {
	SciCall(SCI_SETCOMMANDEVENTS, commandEvents, 0);
}

NP2_inline void SciCall_SetMouseDwellTime(int periodMilliseconds) NP2_noexcept {
	SciCall(SCI_SETMOUSEDWELLTIME, periodMilliseconds, 0);
}
