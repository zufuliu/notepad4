// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Python.

#include <cassert>
#include <cstring>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;

#define		LEX_PY		32	// Python
#define		LEX_BOO		55	// Boo

#define PY_DEF_CLASS 1
#define PY_DEF_FUNC	2
#define PY_DEF_ENUM	3	// Boo

static constexpr bool IsPyStringPrefix(int ch) noexcept {
	ch |= 32;
	return ch == 'r' || ch == 'u' || ch == 'b' || ch == 'f';
}
static constexpr bool IsPyTripleStyle(int style) noexcept {
	return style == SCE_PY_TRIPLE_STRING1 || style == SCE_PY_TRIPLE_STRING2
		|| style == SCE_PY_TRIPLE_BYTES1 || style == SCE_PY_TRIPLE_BYTES2
		|| style == SCE_PY_TRIPLE_FMT_STRING1 || style == SCE_PY_TRIPLE_FMT_STRING2;
}
static constexpr bool IsPyStringStyle(int style) noexcept {
	return style == SCE_PY_STRING1 || style == SCE_PY_STRING2
		|| style == SCE_PY_BYTES1 || style == SCE_PY_BYTES2
		|| style == SCE_PY_RAW_STRING1 || style == SCE_PY_RAW_STRING2
		|| style == SCE_PY_RAW_BYTES1 || style == SCE_PY_RAW_BYTES2
		|| style == SCE_PY_FMT_STRING1 || style == SCE_PY_FMT_STRING2;
}
static constexpr bool IsSpaceEquiv(int state) noexcept {
	// including SCE_PY_DEFAULT, SCE_PY_COMMENTLINE, SCE_PY_COMMENTBLOCK
	return (state <= SCE_PY_COMMENTLINE) || (state == SCE_PY_COMMENTBLOCK);
}

#define PyStringPrefix_Empty	0
#define PyStringPrefix_Raw		1		// 'r'
#define PyStringPrefix_Unicode	2		// 'u'
#define PyStringPrefix_Bytes	4		// 'b'
#define PyStringPrefix_Formatted	8	// 'f'

// r, u, b, f
// ru, rb, rf
// ur, br, fr

static inline int GetPyStringPrefix(int ch) noexcept {
	switch ((ch | 32)) {
	case 'r':
		return PyStringPrefix_Raw;
	case 'u':
		return PyStringPrefix_Unicode;
	case 'b':
		return PyStringPrefix_Bytes;
	case 'f':
		return PyStringPrefix_Formatted;
	default:
		return PyStringPrefix_Empty;
	}
}

static inline int GetPyStringStyle(int quote, bool is_raw, bool is_bytes, bool is_fmt) noexcept {
	switch (quote) {
	case 1:
		if (is_bytes)
			return is_raw ? SCE_PY_RAW_BYTES1 : SCE_PY_BYTES1;
		if (is_fmt)
			return SCE_PY_FMT_STRING1;
		return is_raw ? SCE_PY_RAW_STRING1 : SCE_PY_STRING1;

	case 2:
		if (is_bytes)
			return is_raw ? SCE_PY_RAW_BYTES2 : SCE_PY_BYTES2;
		if (is_fmt)
			return SCE_PY_FMT_STRING2;
		return is_raw ? SCE_PY_RAW_STRING2 : SCE_PY_STRING2;

	case 3:
		if (is_bytes)
			return SCE_PY_TRIPLE_BYTES1;
		if (is_fmt)
			return SCE_PY_TRIPLE_FMT_STRING1;
		return SCE_PY_TRIPLE_STRING1;

	case 6:
		if (is_bytes)
			return SCE_PY_TRIPLE_BYTES2;
		if (is_fmt)
			return SCE_PY_TRIPLE_FMT_STRING2;
		return SCE_PY_TRIPLE_STRING2;

	default:
		return SCE_PY_DEFAULT;
	}
}

static void ColourisePyDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];
	const WordList &keywords_const = *keywordLists[2];
	//const WordList &keywords4 = *keywordLists[3];
	const WordList &keywords_func = *keywordLists[4];
	const WordList &keywords_attr = *keywordLists[5];
	const WordList &keywords_objm = *keywordLists[6];
	const WordList &keywords_class = *keywordLists[7];

	int defType = 0;
	int visibleChars = 0;
	bool continuationLine = false;

	//const int lexType = styler.GetPropertyInt("lexer.lang.type", LEX_PY);
	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {
		if (sc.atLineStart) {
			if (IsPyTripleStyle(sc.state) || IsPyStringStyle(sc.state)) {
				sc.SetState(sc.state);
			}
			defType = 0;
			visibleChars = 0;
		}

		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_PY_OPERATOR:
			sc.SetState(SCE_PY_DEFAULT);
			break;
		case SCE_PY_NUMBER:
			if (!iswordstart(sc.ch)) {
				if ((sc.ch == '+' || sc.ch == '-') && (sc.chPrev == 'e' || sc.chPrev == 'E')) {
					sc.Forward();
				} else if (sc.ch == '.' && sc.chNext != '.') {
					sc.ForwardSetState(SCE_PY_DEFAULT);
				} else {
					sc.SetState(SCE_PY_DEFAULT);
				}
			}
			break;
		case SCE_PY_IDENTIFIER:
			if (!iswordstart(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywords.InList(s)) {
					sc.ChangeState(SCE_PY_WORD);
					if (strcmp(s, "def") == 0)
						defType = PY_DEF_FUNC;
					else if (strcmp(s, "class") == 0 || strcmp(s, "raise") == 0 || strcmp(s, "except") == 0)
						defType = PY_DEF_CLASS;
					//else if (strcmp(s, "enum") == 0)
					//	defType = PY_DEF_ENUM;
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_PY_WORD2);
				} else if (keywords_const.InList(s)) {
					sc.ChangeState(SCE_PY_BUILDIN_CONST);
				} else if (keywords_func.InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_PY_BUILDIN_FUNC);
				} else if (keywords_attr.InList(s)) {
					sc.ChangeState(SCE_PY_ATTR);
				} else if (keywords_objm.InList(s)) {
					sc.ChangeState(SCE_PY_OBJ_FUNC);
				} else if (defType == PY_DEF_CLASS) {
					defType = 0;
					sc.ChangeState(SCE_PY_CLASSNAME);
				} else if (defType == PY_DEF_FUNC) {
					defType = 0;
					sc.ChangeState(SCE_PY_DEFNAME);
				} else if (keywords_class.InList(s)) {
					defType = 0;
					sc.ChangeState(SCE_PY_CLASSNAME);
				} else if (sc.GetNextNSChar() == '(') {
					sc.ChangeState(SCE_PY_FUNCTION);
				}
				sc.SetState(SCE_PY_DEFAULT);
			}
			break;
		case SCE_PY_DECORATOR:
			if (!iswordchar(sc.ch))
				sc.SetState(SCE_PY_DEFAULT);
			break;

		case SCE_PY_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_PY_DEFAULT);
			}
			break;

		case SCE_PY_STRING1:
		case SCE_PY_BYTES1:
		case SCE_PY_RAW_STRING1:
		case SCE_PY_RAW_BYTES1:
		case SCE_PY_FMT_STRING1:
			if (sc.atLineStart && !continuationLine) {
				sc.SetState(SCE_PY_DEFAULT);
			} else if (sc.ch == '\\' && (sc.chNext == '\\' || sc.chNext == '\"' || sc.chNext == '\'')) {
				sc.Forward();
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_PY_DEFAULT);
			}
			break;
		case SCE_PY_STRING2:
		case SCE_PY_BYTES2:
		case SCE_PY_RAW_STRING2:
		case SCE_PY_RAW_BYTES2:
		case SCE_PY_FMT_STRING2:
			if (sc.atLineStart && !continuationLine) {
				sc.SetState(SCE_PY_DEFAULT);
			} else if (sc.ch == '\\' && (sc.chNext == '\\' || sc.chNext == '\"' || sc.chNext == '\'')) {
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_PY_DEFAULT);
			}
			break;
		case SCE_PY_TRIPLE_STRING1:
		case SCE_PY_TRIPLE_BYTES1:
		case SCE_PY_TRIPLE_FMT_STRING1:
			if (sc.ch == '\\') {
				sc.Forward();
			} else if (sc.Match(R"(''')")) {
				sc.Forward(2);
				sc.ForwardSetState(SCE_PY_DEFAULT);
			}
			break;
		case SCE_PY_TRIPLE_STRING2:
		case SCE_PY_TRIPLE_BYTES2:
		case SCE_PY_TRIPLE_FMT_STRING2:
			if (sc.ch == '\\') {
				sc.Forward();
			} else if (sc.Match(R"(""")")) {
				sc.Forward(2);
				sc.ForwardSetState(SCE_PY_DEFAULT);
			}
			break;
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_PY_DEFAULT) {
			if (sc.ch == '#') {
				sc.SetState(SCE_PY_COMMENTLINE);
			} else if (sc.Match(R"(''')")) {
				sc.SetState(SCE_PY_TRIPLE_STRING1);
				sc.Forward(2);
			} else if (sc.Match(R"(""")")) {
				sc.SetState(SCE_PY_TRIPLE_STRING2);
				sc.Forward(2);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_PY_STRING1);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_PY_STRING2);
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_PY_NUMBER);
			} else if (IsPyStringPrefix(sc.ch)) {
				int offset = 0;
				int prefix = GetPyStringPrefix(sc.ch);
				bool is_bytes = prefix == PyStringPrefix_Bytes;
				bool is_fmt = prefix == PyStringPrefix_Formatted;
				bool is_raw = prefix == PyStringPrefix_Raw;

				if (sc.chNext == '\"' || sc.chNext == '\'') {
					offset = 1;
				} else if (IsPyStringPrefix(sc.chNext) && (sc.GetRelative(2) == '\"' || sc.GetRelative(2) == '\'')) {
					prefix = GetPyStringPrefix(sc.chNext);
					offset = 2;
					is_bytes = (is_bytes && prefix == PyStringPrefix_Raw) || (is_raw && prefix == PyStringPrefix_Bytes);
					is_fmt = (is_fmt && prefix == PyStringPrefix_Raw) || (is_raw && prefix == PyStringPrefix_Formatted);
					is_raw = (is_raw && prefix != PyStringPrefix_Raw) || (!is_raw && prefix == PyStringPrefix_Raw);
					if (!(is_bytes || is_fmt || is_raw)) {
						--offset;
						sc.ForwardSetState(SCE_PY_IDENTIFIER);
					}
				}
				if (!offset) {
					sc.SetState(SCE_PY_IDENTIFIER);
				} else {
					sc.Forward(offset);
					if (sc.Match(R"(''')")) {
						sc.ChangeState(GetPyStringStyle(3, is_raw, is_bytes, is_fmt));
						sc.Forward(2);
					} else if (sc.Match(R"(""")")) {
						sc.ChangeState(GetPyStringStyle(6, is_raw, is_bytes, is_fmt));
						sc.Forward(2);
					} else if (sc.ch == '\'') {
						sc.ChangeState(GetPyStringStyle(1, is_raw, is_bytes, is_fmt));
					} else if (sc.ch == '\"') {
						sc.ChangeState(GetPyStringStyle(2, is_raw, is_bytes, is_fmt));
					}
				}
			} else if (iswordstart(sc.ch)) {
				sc.SetState(SCE_PY_IDENTIFIER);
			} else if (sc.ch == '@') {
				if (visibleChars == 1 && iswordstart(sc.chNext))
					sc.SetState(SCE_PY_OPERATOR);
				else
					sc.SetState(SCE_PY_DECORATOR);
			} else if (isoperator(sc.ch) || sc.ch == '`') {
				sc.SetState(SCE_PY_OPERATOR);
				if (defType > 0 && (sc.ch == '(' || sc.ch == ':'))
					defType = 0;
			}
		}

		// Handle line continuation generically.
		if (sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continuationLine = true;
				continue;
			}
		}
		if (!(isspacechar(sc.ch) || IsSpaceEquiv(sc.state))) {
			visibleChars++;
		}
		continuationLine = false;
	}

	sc.Complete();
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, MultiStyle(SCE_PY_COMMENTLINE, SCE_PY_COMMENTBLOCK))

static inline bool IsQuoteLine(Sci_Position line, const Accessor &styler) noexcept {
	const int style = styler.StyleAt(styler.LineStart(line));
	return IsPyTripleStyle(style);
}

// based on original folding code
static void FoldPyDoc(Sci_PositionU startPos, Sci_Position length, int, LexerWordList, Accessor &styler) {
	const Sci_Position maxPos = startPos + length;
	const Sci_Position docLines = styler.GetLine(styler.Length());	// Available last line
	const Sci_Position maxLines = (maxPos == styler.Length()) ? docLines : styler.GetLine(maxPos - 1);	// Requested last line

	// property fold.quotes.python
	//	This option enables folding multi-line quoted strings when using the Python lexer.
	const bool foldQuotes = styler.GetPropertyInt("fold.quotes.python", 1) != 0;

	const bool foldCompact = styler.GetPropertyInt("fold.compact") != 0;

	// Backtrack to previous non-blank line so we can determine indent level
	// for any white space lines (needed esp. within triple quoted strings)
	// and so we can fix any preceding fold level (which is why we go back
	// at least one line in all cases)
	int spaceFlags = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, nullptr);
	while (lineCurrent > 0) {
		lineCurrent--;
		indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, nullptr);
		if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG) && (!IsCommentLine(lineCurrent)) &&
			(!IsQuoteLine(lineCurrent, styler)))
			break;
	}
	int indentCurrentLevel = indentCurrent & SC_FOLDLEVELNUMBERMASK;

	// Set up initial loop state
	startPos = styler.LineStart(lineCurrent);
	int prev_state = SCE_PY_DEFAULT;
	if (lineCurrent >= 1)
		prev_state = styler.StyleAt(startPos - 1);
	int prevQuote = foldQuotes && IsPyTripleStyle(prev_state);

	// Process all characters to end of requested range or end of any triple quote
	//that hangs over the end of the range.  Cap processing in all cases
	// to end of document (in case of unclosed quote at end).
	while ((lineCurrent <= maxLines) || prevQuote) {

		// Gather info
		int lev = indentCurrent;
		Sci_Position lineNext = lineCurrent + 1;
		int indentNext = indentCurrent;
		int quote = false;
		if (lineNext <= docLines) {
			// Information about next line is only available if not at end of document
			indentNext = styler.IndentAmount(lineNext, &spaceFlags, nullptr);
			const Sci_Position lookAtPos = (styler.LineStart(lineNext) == styler.Length()) ? styler.Length() - 1 : styler.LineStart(lineNext);
			const int style = styler.StyleAt(lookAtPos);
			quote = foldQuotes && IsPyTripleStyle(style);
		}
		const int quote_start = (quote && !prevQuote);
		const int quote_continue = (quote && prevQuote);
		if (!quote || !prevQuote)
			indentCurrentLevel = indentCurrent & SC_FOLDLEVELNUMBERMASK;
		if (quote)
			indentNext = indentCurrentLevel;
		if (indentNext & SC_FOLDLEVELWHITEFLAG)
			indentNext = SC_FOLDLEVELWHITEFLAG | indentCurrentLevel;

		if (quote_start) {
			// Place fold point at start of triple quoted string
			lev |= SC_FOLDLEVELHEADERFLAG;
		} else if (quote_continue || prevQuote) {
			// Add level to rest of lines in the string
			lev = lev + 1;
		}

		// Skip past any blank lines for next indent level info; we skip also
		// comments (all comments, not just those starting in column 0)
		// which effectively folds them into surrounding code rather
		// than screwing up folding.  If comments end file, use the min
		// comment indent as the level after

		int minCommentLevel = indentCurrentLevel;
		while (!quote
			&& (lineNext < docLines)
			&& ((indentNext & SC_FOLDLEVELWHITEFLAG) || (lineNext <= docLines && IsCommentLine(lineNext)))) {

			if (IsCommentLine(lineNext) && indentNext < minCommentLevel) {
				minCommentLevel = indentNext;
			}

			lineNext++;
			indentNext = styler.IndentAmount(lineNext, &spaceFlags, nullptr);
		}

		const int levelAfterComments = ((lineNext < docLines) ? indentNext & SC_FOLDLEVELNUMBERMASK : minCommentLevel);
		const int levelBeforeComments = (indentCurrentLevel > levelAfterComments) ? indentCurrentLevel : levelAfterComments;

		// Now set all the indent levels on the lines we skipped
		// Do this from end to start.  Once we encounter one line
		// which is indented more than the line after the end of
		// the comment-block, use the level of the block before

		Sci_Position skipLine = lineNext;
		int skipLevel = levelAfterComments;

		while (--skipLine > lineCurrent) {
			const int skipLineIndent = styler.IndentAmount(skipLine, &spaceFlags, nullptr);

			if (foldCompact) {
				if ((skipLineIndent & SC_FOLDLEVELNUMBERMASK) > levelAfterComments)
					skipLevel = levelBeforeComments;

				const int whiteFlag = skipLineIndent & SC_FOLDLEVELWHITEFLAG;

				styler.SetLevel(skipLine, skipLevel | whiteFlag);
			} else {
				if ((skipLineIndent & SC_FOLDLEVELNUMBERMASK) > levelAfterComments &&
					!(skipLineIndent & SC_FOLDLEVELWHITEFLAG) &&
					!IsCommentLine(skipLine))
					skipLevel = levelBeforeComments;

				styler.SetLevel(skipLine, skipLevel);
			}
		}

		// Set fold header on non-quote line
		if (!quote && !(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
			if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK))
				lev |= SC_FOLDLEVELHEADERFLAG;
		}

		// Keep track of triple quote state of previous line
		prevQuote = quote;

		// Set fold level for this line and move to next line
		styler.SetLevel(lineCurrent, foldCompact ? lev : lev & ~SC_FOLDLEVELWHITEFLAG);
		indentCurrent = indentNext;
		lineCurrent = lineNext;
	}

	// NOTE: Cannot set level of last line here because indentCurrent doesn't have
	// header flag set; the loop above is crafted to take care of this case!
	//styler.SetLevel(lineCurrent, indentCurrent);
}

LexerModule lmPython(SCLEX_PYTHON, ColourisePyDoc, "python", FoldPyDoc);

