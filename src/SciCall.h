/******************************************************************************
*
*
* Notepad4
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

extern HANDLE g_hScintilla;

inline void InitScintillaHandle(HWND hwnd) noexcept {
	g_hScintilla = AsPointer<HANDLE>(SendMessage(hwnd, SCI_GETDIRECTPOINTER, 0, 0));
}

extern "C"
LRESULT SCI_METHOD Scintilla_DirectFunction(HANDLE handle, UINT msg, WPARAM wParam, LPARAM lParam);
#define SciCall(m, w, l)	Scintilla_DirectFunction(g_hScintilla, (m), (w), (l))

using Sci_MarkerMask = unsigned int;

constexpr COLORREF ColorAlpha(COLORREF rgb, UINT alpha) noexcept {
	return rgb | (alpha << 24);
}

#if defined(_WIN64)
inline void PosToStr(Sci_Position pos, LPWSTR tch) noexcept {
	_i64tow(pos, tch, 10);
}

inline void PosToStr(Sci_Position pos, LPSTR tch) noexcept {
	_i64toa(pos, tch, 10);
}
#else
inline void PosToStr(Sci_Position pos, LPWSTR tch) noexcept {
	_ltow(pos, tch, 10);
}

inline void PosToStr(Sci_Position pos, LPSTR tch) noexcept {
	_ltoa(pos, tch, 10);
}
#endif

// Text retrieval and modification

inline Sci_Position SciCall_GetText(Sci_Position length, char *text) noexcept {
	return SciCall(SCI_GETTEXT, length, AsInteger<LPARAM>(text));
}

inline void SciCall_SetSavePoint() noexcept {
	SciCall(SCI_SETSAVEPOINT, 0, 0);
}

inline Sci_Position SciCall_GetLine(Sci_Line line, char *text) noexcept {
	return SciCall(SCI_GETLINE, line, AsInteger<LPARAM>(text));
}

inline void SciCall_ReplaceSel(const char *text) noexcept {
	SciCall(SCI_REPLACESEL, 0, AsInteger<LPARAM>(text));
}

inline void SciCall_SetReadOnly(bool readOnly) noexcept {
	SciCall(SCI_SETREADONLY, readOnly, 0);
}

inline bool SciCall_GetReadOnly() noexcept {
	return static_cast<bool>(SciCall(SCI_GETREADONLY, 0, 0));
}

inline Sci_Position SciCall_GetTextRangeFull(const Sci_TextRangeFull *tr) noexcept {
	return SciCall(SCI_GETTEXTRANGEFULL, 0, AsInteger<LPARAM>(tr));
}

inline void SciCall_AddText(Sci_Position length, const char *text) noexcept {
	SciCall(SCI_ADDTEXT, length, AsInteger<LPARAM>(text));
}

inline void SciCall_AppendText(Sci_Position length, const char *text) noexcept {
	SciCall(SCI_APPENDTEXT, length, AsInteger<LPARAM>(text));
}

inline void SciCall_InsertText(Sci_Position pos, const char *text) noexcept {
	SciCall(SCI_INSERTTEXT, pos, AsInteger<LPARAM>(text));
}

inline void SciCall_ClearAll() noexcept {
	SciCall(SCI_CLEARALL, 0, 0);
}

inline void SciCall_DeleteRange(Sci_Position start, Sci_Position lengthDelete) noexcept {
	SciCall(SCI_DELETERANGE, start, lengthDelete);
}

inline void SciCall_ClearDocumentStyle() noexcept {
	SciCall(SCI_CLEARDOCUMENTSTYLE, 0, 0);
}

inline int SciCall_GetCharAt(Sci_Position position) noexcept {
	return static_cast<int>(SciCall(SCI_GETCHARAT, position, 0));
}

inline int SciCall_GetStyleIndexAt(Sci_Position position) noexcept {
	return static_cast<int>(SciCall(SCI_GETSTYLEINDEXAT, position, 0));
}

inline size_t SciCall_GetStyledTextFull(const Sci_TextRangeFull *tr) noexcept {
	return SciCall(SCI_GETSTYLEDTEXTFULL, 0, AsInteger<LPARAM>(tr));
}

inline int SciCall_GetCharacterAndWidth(Sci_Position position, Sci_Position *width) noexcept {
	return static_cast<int>(SciCall(SCI_GETCHARACTERANDWIDTH, position, AsInteger<LPARAM>(width)));
}

inline int SciCall_GetCharacterAt(Sci_Position position) noexcept {
	return static_cast<int>(SciCall(SCI_GETCHARACTERANDWIDTH, position, 0));
}

// same as CharacterClass in ILexer.h
enum CharacterClass {
	CharacterClass_Space,
	CharacterClass_NewLine,
	CharacterClass_Punctuation,
	CharacterClass_Word,
	CharacterClass_CJKWord
};

inline CharacterClass SciCall_GetCharacterClass(UINT character) noexcept {
	return static_cast<CharacterClass>(SciCall(SCI_GETCHARACTERCLASS, character, 0));
}

// Searching and replacing

inline Sci_Position SciCall_GetTargetStart() noexcept {
	return SciCall(SCI_GETTARGETSTART, 0, 0);
}

inline Sci_Position SciCall_GetTargetEnd() noexcept {
	return SciCall(SCI_GETTARGETEND, 0, 0);
}

inline void SciCall_SetTargetRange(Sci_Position start, Sci_Position end) noexcept {
	SciCall(SCI_SETTARGETRANGE, start, end);
}

inline void SciCall_TargetFromSelection() noexcept {
	SciCall(SCI_TARGETFROMSELECTION, 0, 0);
}

inline void SciCall_TargetWholeDocument() noexcept {
	SciCall(SCI_TARGETWHOLEDOCUMENT, 0, 0);
}

inline void SciCall_SetSearchFlags(int searchFlags) noexcept {
	SciCall(SCI_SETSEARCHFLAGS, searchFlags, 0);
}

inline Sci_Position SciCall_SearchInTarget(Sci_Position length, const char *text) noexcept {
	return SciCall(SCI_SEARCHINTARGET, length, AsInteger<LPARAM>(text));
}

inline Sci_Position SciCall_ReplaceTarget(Sci_Position length, const char *text) noexcept {
	return SciCall(SCI_REPLACETARGET, length, AsInteger<LPARAM>(text));
}

inline Sci_Position SciCall_ReplaceTargetRE(Sci_Position length, const char *text) noexcept {
	return SciCall(SCI_REPLACETARGETRE, length, AsInteger<LPARAM>(text));
}

inline Sci_Position SciCall_FindTextFull(int searchFlags, Sci_TextToFindFull *ft) noexcept {
	return SciCall(SCI_FINDTEXTFULL, searchFlags, AsInteger<LPARAM>(ft));
}

inline Sci_Position SciCall_ReplaceTargetEx(BOOL regex, Sci_Position length, const char *text) noexcept {
	return SciCall(regex ? SCI_REPLACETARGETRE : SCI_REPLACETARGET, length, AsInteger<LPARAM>(text));
}

// Overtype

inline BOOL SciCall_GetOvertype() noexcept {
	return static_cast<BOOL>(SciCall(SCI_GETOVERTYPE, 0, 0));
}

// Cut, copy and paste

inline void SciCall_Cut(bool asBinary) noexcept {
	SciCall(SCI_CUT, asBinary, 0);
}

inline void SciCall_Copy(bool asBinary) noexcept {
	SciCall(SCI_COPY, asBinary, 0);
}

inline void SciCall_Paste(bool asBinary) noexcept {
	SciCall(SCI_PASTE, asBinary, 0);
}

inline void SciCall_Clear() noexcept {
	SciCall(SCI_CLEAR, 0, 0);
}

inline bool SciCall_CanPaste() noexcept {
	return static_cast<bool>(SciCall(SCI_CANPASTE, 0, 0));
}

inline void SciCall_CopyAllowLine() noexcept {
	SciCall(SCI_COPYALLOWLINE, 0, 0);
}

inline void SciCall_CopyRange(Sci_Position start, Sci_Position end) noexcept {
	SciCall(SCI_COPYRANGE, start, end);
}

inline void SciCall_SetPasteConvertEndings(bool convert) noexcept {
	SciCall(SCI_SETPASTECONVERTENDINGS, convert, 0);
}

// Undo and Redo

inline void SciCall_Undo() noexcept {
	SciCall(SCI_UNDO, 0, 0);
}

inline bool SciCall_CanUndo() noexcept {
	return static_cast<bool>(SciCall(SCI_CANUNDO, 0, 0));
}

inline void SciCall_EmptyUndoBuffer() noexcept {
	SciCall(SCI_EMPTYUNDOBUFFER, 0, 0);
}

inline void SciCall_Redo() noexcept {
	SciCall(SCI_REDO, 0, 0);
}

inline bool SciCall_CanRedo() noexcept {
	return static_cast<bool>(SciCall(SCI_CANREDO, 0, 0));
}

inline void SciCall_SetUndoCollection(bool collectUndo) noexcept {
	SciCall(SCI_SETUNDOCOLLECTION, collectUndo, 0);
}

inline void SciCall_BeginUndoAction() noexcept {
	SciCall(SCI_BEGINUNDOACTION, 0, 0);
}

inline void SciCall_EndUndoAction() noexcept {
	SciCall(SCI_ENDUNDOACTION, 0, 0);
}

inline void SciCall_BeginBatchUpdate(bool bNoUndoGroup = false) noexcept {
	SciCall(SCI_BEGINUNDOACTION, bNoUndoGroup, true);
}

inline void SciCall_EndBatchUpdate(bool bNoUndoGroup = false) noexcept {
	SciCall(SCI_ENDUNDOACTION, bNoUndoGroup, true);
}

inline size_t SciCall_GetUndoActions() noexcept {
	return SciCall(SCI_GETUNDOACTIONS, 0, 0);
}

inline void SciCall_SetChangeHistory(int changeHistory) noexcept {
	SciCall(SCI_SETCHANGEHISTORY, changeHistory, 0);
}

// Selection and information

inline Sci_Position SciCall_GetLength() noexcept {
	return SciCall(SCI_GETLENGTH, 0, 0);
}

inline Sci_Line SciCall_GetLineCount() noexcept {
	return SciCall(SCI_GETLINECOUNT, 0, 0);
}

inline void SciCall_AllocateLines(Sci_Line lineCount) noexcept {
	SciCall(SCI_ALLOCATELINES, lineCount, 0);
}

inline void SciCall_SetSel(Sci_Position anchor, Sci_Position caret) noexcept {
	SciCall(SCI_SETSEL, anchor, caret);
}

inline void SciCall_GotoPos(Sci_Position caret) noexcept {
	SciCall(SCI_GOTOPOS, caret, 0);
}

inline void SciCall_GotoLine(Sci_Line line) noexcept {
	SciCall(SCI_GOTOLINE, line, 0);
}

inline void SciCall_SetCurrentPos(Sci_Position caret) noexcept {
	SciCall(SCI_SETCURRENTPOS, caret, 0);
}

inline Sci_Position SciCall_GetCurrentPos() noexcept {
	return SciCall(SCI_GETCURRENTPOS, 0, 0);
}

inline void SciCall_SetAnchor(Sci_Position anchor) noexcept {
	SciCall(SCI_SETANCHOR, anchor, 0);
}

inline Sci_Position SciCall_GetAnchor() noexcept {
	return SciCall(SCI_GETANCHOR, 0, 0);
}

inline void SciCall_SetSelectionStart(Sci_Position anchor) noexcept {
	SciCall(SCI_SETSELECTIONSTART, anchor, 0);
}

inline Sci_Position SciCall_GetSelectionStart() noexcept {
	return SciCall(SCI_GETSELECTIONSTART, 0, 0);
}

inline void SciCall_SetSelectionEnd(Sci_Position caret) noexcept {
	SciCall(SCI_SETSELECTIONEND, caret, 0);
}

inline Sci_Position SciCall_GetSelectionEnd() noexcept {
	return SciCall(SCI_GETSELECTIONEND, 0, 0);
}

inline void SciCall_SelectAll() noexcept {
	SciCall(SCI_SELECTALL, 0, 0);
}

inline Sci_Line SciCall_LineFromPosition(Sci_Position position) noexcept {
	return SciCall(SCI_LINEFROMPOSITION, position, 0);
}

inline Sci_Position SciCall_PositionFromLine(Sci_Line line) noexcept {
	return SciCall(SCI_POSITIONFROMLINE, line, 0);
}

inline Sci_Position SciCall_GetLineEndPosition(Sci_Line line) noexcept {
	return SciCall(SCI_GETLINEENDPOSITION, line, 0);
}

inline Sci_Position SciCall_GetLineLength(Sci_Line line) noexcept {
	return SciCall(SCI_LINELENGTH, line, 0);
}

inline Sci_Position SciCall_GetSelTextLength() noexcept {
	return SciCall(SCI_GETSELTEXT, 0, 0);
}

inline Sci_Position SciCall_GetSelText(char *buffer) noexcept {
	return SciCall(SCI_GETSELTEXT, FALSE, AsInteger<LPARAM>(buffer));
}

inline Sci_Position SciCall_GetSelBytes(char *buffer) noexcept {
	return SciCall(SCI_GETSELTEXT, TRUE, AsInteger<LPARAM>(buffer));
}

inline bool SciCall_IsRectangleSelection() noexcept {
	return static_cast<bool>(SciCall(SCI_SELECTIONISRECTANGLE, 0, 0));
}

inline void SciCall_SetSelectionMode(int selectionMode) noexcept {
	SciCall(SCI_SETSELECTIONMODE, selectionMode, 0);
}

inline int SciCall_GetSelectionMode() noexcept {
	return static_cast<int>(SciCall(SCI_GETSELECTIONMODE, 0, 0));
}

inline Sci_Position SciCall_GetLineSelStartPosition(Sci_Line line) noexcept {
	return SciCall(SCI_GETLINESELSTARTPOSITION, line, 0);
}

inline Sci_Position SciCall_GetLineSelEndPosition(Sci_Line line) noexcept {
	return SciCall(SCI_GETLINESELENDPOSITION, line, 0);
}

inline Sci_Position SciCall_PositionBefore(Sci_Position position) noexcept {
	return SciCall(SCI_POSITIONBEFORE, position, 0);
}

inline Sci_Position SciCall_PositionAfter(Sci_Position position) noexcept {
	return SciCall(SCI_POSITIONAFTER, position, 0);
}

inline int SciCall_TextWidth(int style, const char *text) noexcept {
	return static_cast<int>(SciCall(SCI_TEXTWIDTH, style, AsInteger<LPARAM>(text)));
}

inline int SciCall_TextHeight() noexcept {
	return static_cast<int>(SciCall(SCI_TEXTHEIGHT, 0, 0));
}

inline Sci_Position SciCall_GetColumn(Sci_Position position) noexcept {
	return SciCall(SCI_GETCOLUMN, position, 0);
}

inline Sci_Position SciCall_FindColumn(Sci_Line line, Sci_Position column) noexcept {
	return SciCall(SCI_FINDCOLUMN, line, column);
}

inline Sci_Position SciCall_PositionFromPoint(int x, int y) noexcept {
	return SciCall(SCI_POSITIONFROMPOINT, x, y);
}

inline int SciCall_PointXFromPosition(Sci_Position pos) noexcept {
	return static_cast<int>(SciCall(SCI_POINTXFROMPOSITION, 0, pos));
}

inline int SciCall_PointYFromPosition(Sci_Position pos) noexcept {
	return static_cast<int>(SciCall(SCI_POINTYFROMPOSITION, 0, pos));
}

inline void SciCall_ChooseCaretX() noexcept {
	SciCall(SCI_CHOOSECARETX, 0, 0);
}

inline void SciCall_MoveSelectedLinesUp() noexcept {
	SciCall(SCI_MOVESELECTEDLINESUP, 0, 0);
}

inline void SciCall_MoveSelectedLinesDown() noexcept {
	SciCall(SCI_MOVESELECTEDLINESDOWN, 0, 0);
}

inline Sci_Line EditGetSelectedLineCount() noexcept {
	const Sci_Line iLineStart = SciCall_LineFromPosition(SciCall_GetSelectionStart());
	const Sci_Line iLineEnd = SciCall_LineFromPosition(SciCall_GetSelectionEnd());
	return iLineEnd - iLineStart + 1;
}

// By character or UTF-16 code unit

inline Sci_Position SciCall_CountCharacters(Sci_Position start, Sci_Position end) noexcept {
	return SciCall(SCI_COUNTCHARACTERS, start, end);
}

inline void SciCall_CountCharactersAndColumns(Sci_TextToFindFull *ft) noexcept {
	SciCall(SCI_COUNTCHARACTERSANDCOLUMNS, 0, AsInteger<LPARAM>(ft));
}

// Multiple Selection and Virtual Space

inline void SciCall_SetMultipleSelection(bool multipleSelection) noexcept {
	SciCall(SCI_SETMULTIPLESELECTION, multipleSelection, 0);
}

inline void SciCall_SetAdditionalSelectionTyping(bool additionalSelectionTyping) noexcept {
	SciCall(SCI_SETADDITIONALSELECTIONTYPING, additionalSelectionTyping, 0);
}

inline void SciCall_SetMultiPaste(int multiPaste) noexcept {
	SciCall(SCI_SETMULTIPASTE, multiPaste, 0);
}

inline void SciCall_SetVirtualSpaceOptions(int virtualSpaceOptions) noexcept {
	SciCall(SCI_SETVIRTUALSPACEOPTIONS, virtualSpaceOptions, 0);
}

inline size_t SciCall_GetSelectionCount() noexcept {
	return SciCall(SCI_GETSELECTIONS, 0, 0);
}

inline bool SciCall_IsMultipleSelection() noexcept {
	return SciCall(SCI_GETSELECTIONS, 0, 0) > 1;
}

inline bool SciCall_IsSelectionEmpty() noexcept {
	return static_cast<bool>(SciCall(SCI_GETSELECTIONEMPTY, 0, 0));
}

inline void SciCall_ClearSelections() noexcept {
	SciCall(SCI_CLEARSELECTIONS, 0, 0);
}

inline void SciCall_SetSelection(Sci_Position caret, Sci_Position anchor) noexcept {
	SciCall(SCI_SETSELECTION, caret, anchor);
}

inline void SciCall_AddSelection(Sci_Position caret, Sci_Position anchor) noexcept {
	SciCall(SCI_ADDSELECTION, caret, anchor);
}

inline void SciCall_SetMainSelection(size_t selection) noexcept {
	SciCall(SCI_SETMAINSELECTION, selection, 0);
}

inline void SciCall_SetRectangularSelectionCaret(Sci_Position caret) noexcept {
	SciCall(SCI_SETRECTANGULARSELECTIONCARET, caret, 0);
}

inline void SciCall_SetRectangularSelectionAnchor(Sci_Position anchor) noexcept {
	SciCall(SCI_SETRECTANGULARSELECTIONANCHOR, anchor, 0);
}

inline void SciCall_SetAdditionalCaretsBlink(bool additionalCaretsBlink) noexcept {
	SciCall(SCI_SETADDITIONALCARETSBLINK, additionalCaretsBlink, 0);
}

inline void SciCall_SetAdditionalCaretsVisible(bool additionalCaretsVisible) noexcept {
	SciCall(SCI_SETADDITIONALCARETSVISIBLE, additionalCaretsVisible, 0);
}

// Scrolling and automatic scrolling

inline Sci_Line SciCall_GetFirstVisibleLine() noexcept {
	return SciCall(SCI_GETFIRSTVISIBLELINE, 0, 0);
}

inline Sci_Line SciCall_SetFirstVisibleLine(Sci_Line displayLine) noexcept {
	return SciCall(SCI_SETFIRSTVISIBLELINE, displayLine, 0);
}

inline void SciCall_SetXOffset(int xOffset) noexcept {
	SciCall(SCI_SETXOFFSET, xOffset, 0);
}

inline int SciCall_GetXOffset() noexcept {
	return static_cast<int>(SciCall(SCI_GETXOFFSET, 0, 0));
}

inline void SciCall_LineScroll(Sci_Position columns, Sci_Line lines) noexcept {
	SciCall(SCI_LINESCROLL, columns, lines);
}

inline void SciCall_ScrollCaret() noexcept {
	SciCall(SCI_SCROLLCARET, 0, 0);
}

inline void SciCall_SetXCaretPolicy(int caretPolicy, int caretSlop) noexcept {
	SciCall(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

inline void SciCall_SetYCaretPolicy(int caretPolicy, int caretSlop) noexcept {
	SciCall(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

inline void SciCall_SetScrollWidthTracking(bool tracking) noexcept {
	SciCall(SCI_SETSCROLLWIDTHTRACKING, tracking, 0);
}

inline void SciCall_SetEndAtLastLine(int endAtLastLine) noexcept {
	SciCall(SCI_SETENDATLASTLINE, endAtLastLine, 0);
}

// White space

inline void SciCall_SetViewWS(int viewWS) noexcept {
	SciCall(SCI_SETVIEWWS, viewWS, 0);
}

inline void SciCall_SetWhitespaceSize(int size) noexcept {
	SciCall(SCI_SETWHITESPACESIZE, size, 0);
}

inline void SciCall_SetExtraAscent(int extraAscent) noexcept {
	SciCall(SCI_SETEXTRAASCENT, extraAscent, 0);
}

inline void SciCall_SetExtraDescent(int extraDescent) noexcept {
	SciCall(SCI_SETEXTRADESCENT, extraDescent, 0);
}

// Cursor

inline void SciCall_SetCursor(int cursorType) noexcept {
	SciCall(SCI_SETCURSOR, cursorType, 0);
}

inline void BeginWaitCursor() noexcept {
	SciCall_SetCursor(SC_CURSORWAIT);
}

inline void EndWaitCursor() noexcept {
	POINT pt;
	SciCall_SetCursor(SC_CURSORNORMAL);
	GetCursorPos(&pt);
	SetCursorPos(pt.x, pt.y);
}

// Line endings

inline void SciCall_SetEOLMode(int eolMode) noexcept {
	SciCall(SCI_SETEOLMODE, eolMode, 0);
}

inline int SciCall_GetEOLMode() noexcept {
	return static_cast<int>(SciCall(SCI_GETEOLMODE, 0, 0));
}

inline void SciCall_ConvertEOLs(int eolMode) noexcept {
	SciCall(SCI_CONVERTEOLS, eolMode, 0);
}

inline void SciCall_SetViewEOL(bool visible) noexcept {
	SciCall(SCI_SETVIEWEOL, visible, 0);
}

// Words

inline Sci_Position SciCall_WordStartPosition(Sci_Position position, bool onlyWordCharacters) noexcept {
	return SciCall(SCI_WORDSTARTPOSITION, position, onlyWordCharacters);
}

inline Sci_Position SciCall_WordEndPosition(Sci_Position position, bool onlyWordCharacters) noexcept {
	return SciCall(SCI_WORDENDPOSITION, position, onlyWordCharacters);
}

inline void SciCall_SetCharClassesEx(int length, const unsigned char *characters) noexcept {
	SciCall(SCI_SETCHARCLASSESEX, length, AsInteger<LPARAM>(characters));
}

// Styling

inline Sci_Position SciCall_GetEndStyled() noexcept {
	return SciCall(SCI_GETENDSTYLED, 0, 0);
}

inline void SciCall_SetIdleStyling(int idleStyling) noexcept {
	SciCall(SCI_SETIDLESTYLING, idleStyling, 0);
}

inline void SciCall_StartStyling(Sci_Position start) noexcept {
	SciCall(SCI_STARTSTYLING, start, 0);
}

inline int SciCall_GetLineState(Sci_Line line) noexcept {
	return static_cast<int>(SciCall(SCI_GETLINESTATE, line, 0));
}

// Style definition

inline void SciCall_StyleResetDefault() noexcept {
	SciCall(SCI_STYLERESETDEFAULT, 0, 0);
}

inline void SciCall_StyleClearAll() noexcept {
	SciCall(SCI_STYLECLEARALL, 0, 0);
}

inline void SciCall_CopyStyles(size_t sourceIndex, LPARAM destStyles) noexcept {
	SciCall(SCI_COPYSTYLES, sourceIndex, destStyles);
}

inline void SciCall_StyleSetFont(int style, const char *fontName) noexcept {
	SciCall(SCI_STYLESETFONT, style, AsInteger<LPARAM>(fontName));
}

inline void SciCall_StyleGetFont(int style, char *fontName) noexcept {
	SciCall(SCI_STYLEGETFONT, style, AsInteger<LPARAM>(fontName));
}

inline void SciCall_StyleSetSizeFractional(int style, int sizeHundredthPoints) noexcept {
	SciCall(SCI_STYLESETSIZEFRACTIONAL, style, sizeHundredthPoints);
}

inline int SciCall_StyleGetSizeFractional(int style) noexcept {
	return static_cast<int>(SciCall(SCI_STYLEGETSIZEFRACTIONAL, style, 0));
}

inline void SciCall_StyleSetWeight(int style, int weight) noexcept {
	SciCall(SCI_STYLESETWEIGHT, style, weight);
}

inline int SciCall_StyleGetWeight(int style) noexcept {
	return static_cast<int>(SciCall(SCI_STYLEGETWEIGHT, style, 0));
}

inline void SciCall_StyleSetItalic(int style, bool italic) noexcept {
	SciCall(SCI_STYLESETITALIC, style, italic);
}

inline bool SciCall_StyleGetItalic(int style) noexcept {
	return static_cast<bool>(SciCall(SCI_STYLEGETITALIC, style, 0));
}

inline void SciCall_StyleSetUnderline(int style, bool underline) noexcept {
	SciCall(SCI_STYLESETUNDERLINE, style, underline);
}

inline bool SciCall_StyleGetUnderline(int style) noexcept {
	return static_cast<bool>(SciCall(SCI_STYLEGETUNDERLINE, style, 0));
}

inline void SciCall_StyleSetStrike(int style, bool strike) noexcept {
	SciCall(SCI_STYLESETSTRIKE, style, strike);
}

inline bool SciCall_StyleGetStrike(int style) noexcept {
	return static_cast<bool>(SciCall(SCI_STYLEGETSTRIKE, style, 0));
}

inline void SciCall_StyleSetOverline(int style, bool overline) noexcept {
	SciCall(SCI_STYLESETOVERLINE, style, overline);
}

inline void SciCall_StyleSetFore(int style, COLORREF fore) noexcept {
	SciCall(SCI_STYLESETFORE, style, fore);
}

inline COLORREF SciCall_StyleGetFore(int style) noexcept {
	return static_cast<COLORREF>(SciCall(SCI_STYLEGETFORE, style, 0));
}

inline void SciCall_StyleSetBack(int style, COLORREF back) noexcept {
	SciCall(SCI_STYLESETBACK, style, back);
}

inline COLORREF SciCall_StyleGetBack(int style) noexcept {
	return static_cast<COLORREF>(SciCall(SCI_STYLEGETBACK, style, 0));
}

inline void SciCall_StyleSetEOLFilled(int style, bool eolFilled) noexcept {
	SciCall(SCI_STYLESETEOLFILLED, style, eolFilled);
}

inline bool SciCall_StyleGetEOLFilled(int style) noexcept {
	return static_cast<bool>(SciCall(SCI_STYLEGETEOLFILLED, style, 0));
}

inline void SciCall_StyleSetCharacterSet(int style, int characterSet) noexcept {
	SciCall(SCI_STYLESETCHARACTERSET, style, characterSet);
}

inline int SciCall_StyleGetCharacterSet(int style) noexcept {
	return static_cast<int>(SciCall(SCI_STYLEGETCHARACTERSET, style, 0));
}

inline void SciCall_StyleSetHotSpot(int style, bool hotspot) noexcept {
	SciCall(SCI_STYLESETHOTSPOT, style, hotspot);
}

inline bool SciCall_StyleGetHotSpot(int style) noexcept {
	return static_cast<bool>(SciCall(SCI_STYLEGETHOTSPOT, style, 0));
}

inline void SciCall_StyleSetCheckMonospaced(int style, bool checkMonospaced) noexcept {
	SciCall(SCI_STYLESETCHECKMONOSPACED, style, checkMonospaced);
}

// Caret, selection, and hotspot styles

inline void SciCall_SetElementColor(int element, COLORREF color) noexcept {
	SciCall(SCI_SETELEMENTCOLOUR, element, color);
}

inline COLORREF SciCall_GetElementColour(int element) noexcept {
	return static_cast<COLORREF>(SciCall(SCI_GETELEMENTCOLOUR, element, 0));
}

inline void SciCall_ResetElementColor(int element) noexcept {
	SciCall(SCI_RESETELEMENTCOLOUR, element, 0);
}

inline void SciCall_SetSelectionLayer(int layer) noexcept {
	SciCall(SCI_SETSELECTIONLAYER, layer, 0);
}

inline void SciCall_SetSelEOLFilled(bool filled) noexcept {
	SciCall(SCI_SETSELEOLFILLED, filled, 0);
}

inline void SciCall_SetEOLSelectedWidth(int percent) noexcept {
	SciCall(SCI_SETEOLSELECTEDWIDTH, percent, 0);
}

inline void SciCall_SetCaretLineLayer(int layer) noexcept {
	SciCall(SCI_SETCARETLINELAYER, layer, 0);
}

inline void SciCall_SetCaretLineFrame(int width) noexcept {
	SciCall(SCI_SETCARETLINEFRAME, width, 0);
}

inline void SciCall_SetCaretLineVisibleAlways(bool alwaysVisible) noexcept {
	SciCall(SCI_SETCARETLINEVISIBLEALWAYS, alwaysVisible, 0);
}

inline void SciCall_SetCaretLineHighlightSubLine(bool subLine) noexcept {
	SciCall(SCI_SETCARETLINEHIGHLIGHTSUBLINE, subLine, 0);
}

inline void SciCall_SetCaretPeriod(int periodMilliseconds) noexcept {
	SciCall(SCI_SETCARETPERIOD, periodMilliseconds, 0);
}

inline void SciCall_SetCaretStyle(int caretStyle) noexcept {
	SciCall(SCI_SETCARETSTYLE, caretStyle, 0);
}

inline void SciCall_SetCaretWidth(int pixelWidth) noexcept {
	SciCall(SCI_SETCARETWIDTH, pixelWidth, 0);
}

inline void SciCall_SetCaretSticky(int useCaretStickyBehaviour) noexcept {
	SciCall(SCI_SETCARETSTICKY, useCaretStickyBehaviour, 0);
}

// Character representations

inline void SciCall_SetRepresentation(const char *encodedCharacter, const char *representation) noexcept {
	SciCall(SCI_SETREPRESENTATION, AsInteger<WPARAM>(encodedCharacter), AsInteger<LPARAM>(representation));
}

inline void SciCall_ClearRepresentation(const char *encodedCharacter) noexcept {
	SciCall(SCI_CLEARREPRESENTATION, AsInteger<WPARAM>(encodedCharacter), 0);
}

// Margins

inline void SciCall_SetMarginType(int margin, int marginType) noexcept {
	SciCall(SCI_SETMARGINTYPEN, margin, marginType);
}

inline void SciCall_SetMarginWidth(int margin, int pixelWidth) noexcept {
	SciCall(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

inline int SciCall_GetMarginWidth(int margin) noexcept {
	return static_cast<int>(SciCall(SCI_GETMARGINWIDTHN, margin, 0));
}

inline void SciCall_SetMarginMask(int margin, int mask) noexcept {
	SciCall(SCI_SETMARGINMASKN, margin, mask);
}

inline void SciCall_SetMarginSensitive(int margin, bool sensitive) noexcept {
	SciCall(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

inline void SciCall_SetMarginCursor(int margin, int cursor) noexcept {
	SciCall(SCI_SETMARGINCURSORN, margin, cursor);
}

inline void SciCall_SetFoldMarginColor(bool useSetting, COLORREF back) noexcept {
	SciCall(SCI_SETFOLDMARGINCOLOUR, useSetting, back);
}

inline void SciCall_SetFoldMarginHiColor(bool useSetting, COLORREF fore) noexcept {
	SciCall(SCI_SETFOLDMARGINHICOLOUR, useSetting, fore);
}

inline void SciCall_SetMarginOptions(int marginOptions) noexcept {
	SciCall(SCI_SETMARGINOPTIONS, marginOptions, 0);
}

// Other settings

inline void SciCall_SetCodePage(UINT codePage) noexcept {
	SciCall(SCI_SETCODEPAGE, codePage, 0);
}

inline UINT SciCall_GetCodePage() noexcept {
	return static_cast<UINT>(SciCall(SCI_GETCODEPAGE, 0, 0));
}

inline void SciCall_SetTechnology(int technology) noexcept {
	SciCall(SCI_SETTECHNOLOGY, technology, 0);
}

inline int SciCall_GetTechnology() noexcept {
	return static_cast<int>(SciCall(SCI_GETTECHNOLOGY, 0, 0));
}

inline void SciCall_SetBidirectional(int bidirectional) noexcept {
	SciCall(SCI_SETBIDIRECTIONAL, bidirectional, 0);
}

inline int SciCall_GetBidirectional() noexcept {
	return static_cast<int>(SciCall(SCI_GETBIDIRECTIONAL, 0, 0));
}

inline void SciCall_SetFontQuality(int fontQuality) noexcept {
	SciCall(SCI_SETFONTQUALITY, fontQuality, 0);
}

inline void SciCall_SetFontLocale(const char *localeName) noexcept {
	SciCall(SCI_SETFONTLOCALE, 0, AsInteger<LPARAM>(localeName));
}

inline void SciCall_SetIMEInteraction(int imeInteraction) noexcept {
	SciCall(SCI_SETIMEINTERACTION, imeInteraction, 0);
}


// Brace highlighting

inline void SciCall_BraceHighlight(Sci_Position posA, Sci_Position posB) noexcept {
	SciCall(SCI_BRACEHIGHLIGHT, posA, posB);
}

inline void SciCall_BraceHighlightIndicator(bool useSetting, int indicator) noexcept {
	SciCall(SCI_BRACEHIGHLIGHTINDICATOR, useSetting, indicator);
}

inline void SciCall_BraceBadLight(Sci_Position pos) noexcept {
	SciCall(SCI_BRACEBADLIGHT, pos, 0);
}

inline void SciCall_BraceBadLightIndicator(bool useSetting, int indicator) noexcept {
	SciCall(SCI_BRACEBADLIGHTINDICATOR, useSetting, indicator);
}

inline Sci_Position SciCall_BraceMatch(Sci_Position pos) noexcept {
	return SciCall(SCI_BRACEMATCH, pos, 0);
}

inline Sci_Position SciCall_BraceMatchNext(Sci_Position pos, Sci_Position startPos) noexcept {
	return SciCall(SCI_BRACEMATCHNEXT, pos, startPos);
}

// Tabs and Indentation Guides

inline void SciCall_SetTabWidth(int tabWidth) noexcept {
	SciCall(SCI_SETTABWIDTH, tabWidth, 0);
}

inline int SciCall_GetTabWidth() noexcept {
	return static_cast<int>(SciCall(SCI_GETTABWIDTH, 0, 0));
}

inline void SciCall_SetTabMinimumWidth(int pixels) noexcept {
	SciCall(SCI_SETTABMINIMUMWIDTH, pixels, 0);
}

inline void SciCall_SetUseTabs(bool useTabs) noexcept {
	SciCall(SCI_SETUSETABS, useTabs, 0);
}

inline bool SciCall_GetUseTabs() noexcept {
	return static_cast<bool>(SciCall(SCI_GETUSETABS, 0, 0));
}

inline void SciCall_SetIndent(int indentSize) noexcept {
	SciCall(SCI_SETINDENT, indentSize, 0);
}

inline void SciCall_SetTabIndents(bool tabIndents) noexcept {
	SciCall(SCI_SETTABINDENTS, tabIndents, 0);
}

inline void SciCall_SetBackSpaceUnIndents(uint8_t bsUnIndents) noexcept {
	SciCall(SCI_SETBACKSPACEUNINDENTS, bsUnIndents, 0);
}

inline void SciCall_SetLineIndentation(Sci_Line line, Sci_Position indentation) noexcept {
	SciCall(SCI_SETLINEINDENTATION, line, indentation);
}

inline Sci_Position SciCall_GetLineIndentation(Sci_Line line) noexcept {
	return SciCall(SCI_GETLINEINDENTATION, line, 0);
}

inline Sci_Position SciCall_GetLineIndentPosition(Sci_Line line) noexcept {
	return SciCall(SCI_GETLINEINDENTPOSITION, line, 0);
}

inline void SciCall_SetIndentationGuides(int indentView) noexcept {
	SciCall(SCI_SETINDENTATIONGUIDES, indentView, 0);
}

inline int SciCall_GetIndentationGuides() noexcept {
	return static_cast<int>(SciCall(SCI_GETINDENTATIONGUIDES, 0, 0));
}

inline void SciCall_SetHighlightGuide(Sci_Position column) noexcept {
	SciCall(SCI_SETHIGHLIGHTGUIDE, column, 0);
}

// Markers

inline void SciCall_MarkerDefine(int markerNumber, int markerSymbol) noexcept {
	SciCall(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

inline void SciCall_MarkerDefinePixmap(int markerNumber, const char *pixmap) noexcept {
	SciCall(SCI_MARKERDEFINEPIXMAP, markerNumber, AsInteger<LPARAM>(pixmap));
}

inline int SciCall_MarkerSymbolDefined(int markerNumber) noexcept {
	return static_cast<int>(SciCall(SCI_MARKERSYMBOLDEFINED, markerNumber, 0));
}

inline void SciCall_MarkerSetForeTranslucent(int markerNumber, COLORREF fore) noexcept {
	SciCall(SCI_MARKERSETFORETRANSLUCENT, markerNumber, fore);
}

inline void SciCall_MarkerSetBackTranslucent(int markerNumber, COLORREF back) noexcept {
	SciCall(SCI_MARKERSETBACKTRANSLUCENT, markerNumber, back);
}

inline void SciCall_MarkerSetBackSelectedTranslucent(int markerNumber, COLORREF back) noexcept {
	SciCall(SCI_MARKERSETBACKSELECTEDTRANSLUCENT, markerNumber, back);
}

inline void SciCall_MarkerSetLayer(int markerNumber, int layer) noexcept {
	SciCall(SCI_MARKERSETLAYER, markerNumber, layer);
}

inline void SciCall_MarkerSetStrokeWidth(int markerNumber, int hundredths) noexcept {
	SciCall(SCI_MARKERSETSTROKEWIDTH, markerNumber, hundredths);
}

inline void SciCall_MarkerEnableHighlight(bool enabled) noexcept {
	SciCall(SCI_MARKERENABLEHIGHLIGHT, enabled, 0);
}

inline int SciCall_MarkerAdd(Sci_Line line, int markerNumber) noexcept {
	return static_cast<int>(SciCall(SCI_MARKERADD, line, markerNumber));
}

inline void SciCall_MarkerDelete(Sci_Line line, int markerNumber) noexcept {
	SciCall(SCI_MARKERDELETE, line, markerNumber);
}

inline void SciCall_MarkerDeleteAll(int markerNumber) noexcept {
	SciCall(SCI_MARKERDELETEALL, markerNumber, 0);
}

inline void SciCall_ClearMarker() noexcept {
	SciCall_MarkerDeleteAll(-1);
}

inline Sci_MarkerMask SciCall_MarkerGet(Sci_Line line) noexcept {
	return static_cast<Sci_MarkerMask>(SciCall(SCI_MARKERGET, line, 0));
}

inline Sci_Line SciCall_MarkerNext(Sci_Line line, Sci_MarkerMask markerMask) noexcept {
	return SciCall(SCI_MARKERNEXT, line, markerMask);
}

inline Sci_Line SciCall_MarkerPrevious(Sci_Line line, Sci_MarkerMask markerMask) noexcept {
	return SciCall(SCI_MARKERPREVIOUS, line, markerMask);
}

// Indicators

inline void SciCall_IndicSetStyle(int indicator, int indicatorStyle) noexcept {
	SciCall(SCI_INDICSETSTYLE, indicator, indicatorStyle);
}

inline void SciCall_IndicSetFore(int indicator, COLORREF fore) noexcept {
	SciCall(SCI_INDICSETFORE, indicator, fore);
}

inline void SciCall_IndicSetAlpha(int indicator, int alpha) noexcept {
	SciCall(SCI_INDICSETALPHA, indicator, alpha);
}

inline void SciCall_IndicSetOutlineAlpha(int indicator, int alpha) noexcept {
	SciCall(SCI_INDICSETOUTLINEALPHA, indicator, alpha);
}

inline void SciCall_IndicSetStrokeWidth(int indicator, int hundredths) noexcept {
	SciCall(SCI_INDICSETSTROKEWIDTH, indicator, hundredths);
}

inline void SciCall_SetIndicatorCurrent(int indicator) noexcept {
	SciCall(SCI_SETINDICATORCURRENT, indicator, 0);
}

inline void SciCall_IndicatorClearRange(Sci_Position start, Sci_Position length) noexcept {
	SciCall(SCI_INDICATORCLEARRANGE, start, length);
}

inline void SciCall_IndicatorFillRange(Sci_Position start, Sci_Position length) noexcept {
	SciCall(SCI_INDICATORFILLRANGE, start, length);
}

// Autocompletion

inline void SciCall_AutoCShow(Sci_Position lengthEntered, const char *itemList) noexcept {
	SciCall(SCI_AUTOCSHOW, lengthEntered, AsInteger<LPARAM>(itemList));
}

inline void SciCall_AutoCCancel() noexcept {
	SciCall(SCI_AUTOCCANCEL, 0, 0);
}

inline bool SciCall_AutoCActive() noexcept {
	return static_cast<bool>(SciCall(SCI_AUTOCACTIVE, 0, 0));
}

inline void SciCall_AutoCSetSeparator(char separatorCharacter) noexcept {
	SciCall(SCI_AUTOCSETSEPARATOR, separatorCharacter, 0);
}

inline void SciCall_AutoCSetTypeSeparator(char separatorCharacter) noexcept {
	SciCall(SCI_AUTOCSETTYPESEPARATOR, separatorCharacter, 0);
}

inline void SciCall_AutoCSetCancelAtStart(bool cancel) noexcept {
	SciCall(SCI_AUTOCSETCANCELATSTART, cancel, 0);
}

inline void SciCall_AutoCSetFillUps(const char *characterSet) noexcept {
	SciCall(SCI_AUTOCSETFILLUPS, 0, AsInteger<LPARAM>(characterSet));
}

inline void SciCall_AutoCSetChooseSingle(bool chooseSingle) noexcept {
	SciCall(SCI_AUTOCSETCHOOSESINGLE, chooseSingle, 0);
}

inline void SciCall_AutoCSetIgnoreCase(bool ignoreCase) noexcept {
	SciCall(SCI_AUTOCSETIGNORECASE, ignoreCase, 0);
}

inline void SciCall_AutoCSetCaseInsensitiveBehaviour(int behaviour) noexcept {
	SciCall(SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR, behaviour, 0);
}

inline void SciCall_AutoCSetOrder(int ordere) noexcept {
	SciCall(SCI_AUTOCSETORDER, ordere, 0);
}

inline void SciCall_AutoCSetOptions(int options) noexcept {
	SciCall(SCI_AUTOCSETOPTIONS, options, 0);
}

inline void SciCall_AutoCSetDropRestOfWord(bool dropRestOfWord) noexcept {
	SciCall(SCI_AUTOCSETDROPRESTOFWORD, dropRestOfWord, 0);
}

inline void SciCall_AutoCSetMaxHeight(int rowCount) noexcept {
	SciCall(SCI_AUTOCSETMAXHEIGHT, rowCount, 0);
}

inline void SciCall_SetAutoInsertMask(int mask) noexcept {
	SciCall(SCI_SETAUTOINSERTMASK, mask, 0);
}

// Call tips

inline void SciCall_CallTipShow(Sci_Position pos, const char *definition) noexcept {
	SciCall(SCI_CALLTIPSHOW, pos, AsInteger<LPARAM>(definition));
}

inline void SciCall_CallTipCancel() noexcept {
	SciCall(SCI_CALLTIPCANCEL, 0, 0);
}

inline bool SciCall_CallTipActive() noexcept {
	return static_cast<bool>(SciCall(SCI_CALLTIPACTIVE, 0, 0));
}

inline void SciCall_CallTipSetBack(COLORREF back) noexcept {
	SciCall(SCI_CALLTIPSETBACK, back, 0);
}

inline void SciCall_CallTipSetFore(COLORREF fore) noexcept {
	SciCall(SCI_CALLTIPSETFORE, fore, 0);
}

inline void SciCall_CallTipUseStyle(int tabSize) noexcept {
	SciCall(SCI_CALLTIPUSESTYLE, tabSize, 0);
}

inline void SciCall_ShowNotification(WPARAM notifyPos, const char *definition) noexcept {
	SciCall(SCI_SHOWNOTIFICATION, notifyPos, AsInteger<LPARAM>(definition));
}

// Keyboard commands

inline void SciCall_VCHome() noexcept {
	SciCall(SCI_VCHOME, 0, 0);
}

inline void SciCall_DocumentStart() noexcept {
	SciCall(SCI_DOCUMENTSTART, 0, 0);
}

inline void SciCall_DocumentEnd() noexcept {
	SciCall(SCI_DOCUMENTEND, 0, 0);
}

inline void SciCall_DeleteBack() noexcept {
	SciCall(SCI_DELETEBACK, 0, 0);
}

inline void SciCall_DelWordLeft() noexcept {
	SciCall(SCI_DELWORDLEFT, 0, 0);
}

inline void SciCall_DelWordRight() noexcept {
	SciCall(SCI_DELWORDRIGHT, 0, 0);
}

inline void SciCall_DelLineLeft() noexcept {
	SciCall(SCI_DELLINELEFT, 0, 0);
}

inline void SciCall_DelLineRight() noexcept {
	SciCall(SCI_DELLINERIGHT, 0, 0);
}

inline void SciCall_LineDelete() noexcept {
	SciCall(SCI_LINEDELETE, 0, 0);
}

inline void SciCall_LineCut(BOOL lineCopy) noexcept {
	SciCall(SCI_LINECUT, lineCopy, 0);
}

inline void SciCall_LineCopy(BOOL lineCopy) noexcept {
	SciCall(SCI_LINECOPY, lineCopy, 0);
}

inline void SciCall_LineTranspose() noexcept {
	SciCall(SCI_LINETRANSPOSE, 0, 0);
}

inline void SciCall_LineDuplicate() noexcept {
	SciCall(SCI_LINEDUPLICATE, 0, 0);
}

inline void SciCall_LowerCase() noexcept {
	SciCall(SCI_LOWERCASE, 0, 0);
}

inline void SciCall_UpperCase() noexcept {
	SciCall(SCI_UPPERCASE, 0, 0);
}

inline void SciCall_Cancel() noexcept {
	SciCall(SCI_CANCEL, 0, 0);
}

inline void SciCall_EditToggleOvertype() noexcept {
	SciCall(SCI_EDITTOGGLEOVERTYPE, 0, 0);
}

inline void SciCall_NewLine() noexcept {
	SciCall(SCI_NEWLINE, 0, 0);
}

inline void SciCall_Tab() noexcept {
	SciCall(SCI_TAB, TAB_COMPLETION_DEFAULT, 0);
}

inline void SciCall_TabCompletion(int what) noexcept {
	SciCall(SCI_TAB, what, 0);
}

inline void SciCall_LineIndent() noexcept {
	SciCall(SCI_LINEINDENT, 0, 0);
}

inline void SciCall_LineDedent() noexcept {
	SciCall(SCI_LINEDEDENT, 0, 0);
}

inline void SciCall_SelectionDuplicate() noexcept {
	SciCall(SCI_SELECTIONDUPLICATE, 0, 0);
}

// Key bindings

inline void SciCall_AssignCmdKey(int keyDefinition, int sciCommand) noexcept {
	SciCall(SCI_ASSIGNCMDKEY, keyDefinition, sciCommand);
}

// Popup edit menu

inline void SciCall_UsePopUp(int popUpMode) noexcept {
	SciCall(SCI_USEPOPUP, popUpMode, 0);
}

// Printing

inline Sci_Position SciCall_FormatRangeFull(bool draw, const Sci_RangeToFormatFull *fr) noexcept {
	return SciCall(SCI_FORMATRANGEFULL, draw, AsInteger<LPARAM>(fr));
}

inline void SciCall_SetPrintMagnification(int magnification) noexcept {
	SciCall(SCI_SETPRINTMAGNIFICATION, magnification, 0);
}

inline void SciCall_SetPrintColorMode(int mode) noexcept {
	SciCall(SCI_SETPRINTCOLOURMODE, mode, 0);
}

// Direct access

inline const char* SciCall_GetRangePointer(Sci_Position start, Sci_Position lengthRange) noexcept {
	return AsPointer<const char *>(SciCall(SCI_GETRANGEPOINTER, start, lengthRange));
}

// Multiple views

inline void SciCall_SetDocPointer(HANDLE doc) noexcept {
	SciCall(SCI_SETDOCPOINTER, 0, AsInteger<LPARAM>(doc));
}

inline HANDLE SciCall_CreateDocument(Sci_Position bytes, int documentOptions) noexcept {
	return AsPointer<HANDLE>(SciCall(SCI_CREATEDOCUMENT, bytes, documentOptions));
}

inline void SciCall_ReleaseDocument(HANDLE doc) noexcept {
	SciCall(SCI_RELEASEDOCUMENT, 0, AsInteger<LPARAM>(doc));
}

inline int SciCall_GetDocumentOptions() noexcept {
	return static_cast<int>(SciCall(SCI_GETDOCUMENTOPTIONS, 0, 0));
}

// Folding

inline Sci_Line SciCall_DocLineFromVisible(Sci_Line displayLine) noexcept {
	return SciCall(SCI_DOCLINEFROMVISIBLE, displayLine, 0);
}

inline bool SciCall_GetLineVisible(Sci_Line line) noexcept {
	return static_cast<bool>(SciCall(SCI_GETLINEVISIBLE, line, 0));
}

inline int SciCall_GetFoldLevel(Sci_Line line) noexcept {
	return static_cast<int>(SciCall(SCI_GETFOLDLEVEL, line, 0));
}

inline void SciCall_SetFoldFlags(int flags) noexcept {
	SciCall(SCI_SETFOLDFLAGS, flags, 0);
}

inline Sci_Line SciCall_GetLastChild(Sci_Line line) noexcept {
	return SciCall(SCI_GETLASTCHILD, line, -1);
}

inline Sci_Line SciCall_GetLastChildEx(Sci_Line line, int level) noexcept {
	return SciCall(SCI_GETLASTCHILD, line, level);
}

inline Sci_Line SciCall_GetFoldParent(Sci_Line line) noexcept {
	return SciCall(SCI_GETFOLDPARENT, line, 0);
}

inline BOOL SciCall_GetFoldExpanded(Sci_Line line) noexcept {
	return static_cast<BOOL>(SciCall(SCI_GETFOLDEXPANDED, line, 0));
}

inline void SciCall_FoldLine(Sci_Line line, int action) noexcept {
	SciCall(SCI_FOLDLINE, line, action);
}

inline void SciCall_FoldAll(int action) noexcept {
	SciCall(SCI_FOLDALL, action, 0);
}

inline void SciCall_ToggleFoldShowText(Sci_Line line, const char *text) noexcept {
	SciCall(SCI_TOGGLEFOLDSHOWTEXT, line, AsInteger<LPARAM>(text));
}

inline void SciCall_SetDefaultFoldDisplayText(const char *text) noexcept {
	SciCall(SCI_SETDEFAULTFOLDDISPLAYTEXT, 0, AsInteger<LPARAM>(text));
}

inline void SciCall_FoldDisplayTextSetStyle(int style) noexcept {
	SciCall(SCI_FOLDDISPLAYTEXTSETSTYLE, style, 0);
}

inline void SciCall_ExpandChildren(Sci_Line line, int level) noexcept {
	SciCall(SCI_EXPANDCHILDREN, line, level);
}

inline void SciCall_SetAutomaticFold(int automaticFold) noexcept {
	SciCall(SCI_SETAUTOMATICFOLD, automaticFold, 0);
}

inline void SciCall_EnsureVisible(Sci_Line line) noexcept {
	SciCall(SCI_ENSUREVISIBLE, line, 0);
}

inline void SciCall_EnsureVisibleEnforcePolicy(Sci_Line line) noexcept {
	SciCall(SCI_ENSUREVISIBLEENFORCEPOLICY, line, 0);
}

// Line wrapping

inline void SciCall_SetWrapMode(int wrapMode) noexcept {
	SciCall(SCI_SETWRAPMODE, wrapMode, 0);
}

inline void SciCall_SetWrapVisualFlags(int wrapVisualFlags) noexcept {
	SciCall(SCI_SETWRAPVISUALFLAGS, wrapVisualFlags, 0);
}

inline void SciCall_SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation) noexcept {
	SciCall(SCI_SETWRAPVISUALFLAGSLOCATION, wrapVisualFlagsLocation, 0);
}

inline void SciCall_SetWrapIndentMode(int wrapIndentMode) noexcept {
	SciCall(SCI_SETWRAPINDENTMODE, wrapIndentMode, 0);
}

inline int SciCall_GetWrapIndentMode() noexcept {
	return static_cast<int>(SciCall(SCI_GETWRAPINDENTMODE, 0, 0));
}

inline void SciCall_SetWrapStartIndent(int indent) noexcept {
	SciCall(SCI_SETWRAPSTARTINDENT, indent, 0);
}

inline void SciCall_SetLayoutCache(int cacheMode) noexcept {
	SciCall(SCI_SETLAYOUTCACHE, cacheMode, 0);
}

inline void SciCall_LinesSplit(int pixelWidth) noexcept {
	SciCall(SCI_LINESSPLIT, pixelWidth, 0);
}

inline void SciCall_LinesJoin() noexcept {
	SciCall(SCI_LINESJOIN, 0, 0);
}

// Zooming

inline void SciCall_SetZoom(int percent) noexcept {
	SciCall(SCI_SETZOOM, percent, 0);
}

inline int SciCall_GetZoom() noexcept {
	return static_cast<int>(SciCall(SCI_GETZOOM, 0, 0));
}

inline void SciCall_ZoomIn() noexcept {
	SciCall(SCI_ZOOMIN, 0, 0);
}

inline void SciCall_ZoomOut() noexcept {
	SciCall(SCI_ZOOMOUT, 0, 0);
}

// Long lines

inline void SciCall_SetEdgeMode(int edgeMode) noexcept {
	SciCall(SCI_SETEDGEMODE, edgeMode, 0);
}

inline int SciCall_GetEdgeMode() noexcept {
	return static_cast<int>(SciCall(SCI_GETEDGEMODE, 0, 0));
}

inline void SciCall_SetEdgeColumn(int column) noexcept {
	SciCall(SCI_SETEDGECOLUMN, column, 0);
}

inline void SciCall_SetEdgeColor(COLORREF edgeColor) noexcept {
	SciCall(SCI_SETEDGECOLOUR, edgeColor, 0);
}

// Lexer

inline void SciCall_SetLexer(int lexer) noexcept { //! removed in Scintilla 5
	SciCall(SCI_SETLEXER, lexer, 0);
}

inline void SciCall_ColouriseAll() noexcept {
	SciCall(SCI_COLOURISE, 0, -1);
}

inline void SciCall_EnsureStyledTo(Sci_Position end) noexcept {
	SciCall(SCI_COLOURISE, 0, end);
}

inline void SciCall_SetProperty(const char *key, const char *value) noexcept {
	SciCall(SCI_SETPROPERTY, AsInteger<WPARAM>(key), AsInteger<LPARAM>(value));
}

inline void SciCall_SetKeywords(int keywordSet, const char *keywords) noexcept {
	SciCall(SCI_SETKEYWORDS, keywordSet, AsInteger<LPARAM>(keywords));
}

// Notifications

inline void SciCall_SetModEventMask(int eventMask) noexcept {
	SciCall(SCI_SETMODEVENTMASK, eventMask, 0);
}

inline void SciCall_SetCommandEvents(bool commandEvents) noexcept {
	SciCall(SCI_SETCOMMANDEVENTS, commandEvents, 0);
}

inline void SciCall_SetMouseDwellTime(int periodMilliseconds) noexcept {
	SciCall(SCI_SETMOUSEDWELLTIME, periodMilliseconds, 0);
}
