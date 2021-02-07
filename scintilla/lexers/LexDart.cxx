// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Dart.

#include <cassert>
#include <cstring>

#include <vector>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "StringUtils.h"
#include "LexerModule.h"
#include "LexerUtils.h"

using namespace Scintilla;

namespace {

struct EscapeSequence {
	int outerState = SCE_DART_DEFAULT;
	int digitsLeft = 0;

	// highlight any character as escape sequence, no highlight for hex in '\u{hex}'.
	bool resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = (chNext == 'x')? 3 : ((chNext == 'u') ? 5 : 1);
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

enum {
	DartLineStateMaskLineComment = 1,	// line comment
	DartLineStateMaskImport = (1 << 1),	// import
};

static_assert(DefaultNestedStateBaseStyle + 1 == SCE_DART_STRING_SQ);
static_assert(DefaultNestedStateBaseStyle + 2 == SCE_DART_STRING_DQ);
static_assert(DefaultNestedStateBaseStyle + 3 == SCE_DART_TRIPLE_STRING_SQ);
static_assert(DefaultNestedStateBaseStyle + 4 == SCE_DART_TRIPLE_STRING_DQ);

constexpr bool IsDeclarableOperator(int ch) noexcept {
	// https://github.com/dart-lang/sdk/blob/master/sdk/lib/core/symbol.dart
	return AnyOf(ch, '+', '-', '*', '/', '%', '~', '&', '|',
					 '^', '<', '>', '=', '[', ']');
}

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_DART_TASKMARKER;
}

void ColouriseDartDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;
	int commentLevel = 0;	// nested block comment level

	int kwType = SCE_DART_DEFAULT;
	int chBeforeIdentifier = 0;

	std::vector<int> nestedState; // string interpolation "${}"

	int visibleChars = 0;
	int visibleCharsBefore = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		6: commentLevel
		3: nestedState count
		3*4: nestedState
		*/
		commentLevel = (lineState >> 2) & 0x3f;
		lineState >>= 8;
		if (lineState) {
			UnpackLineState(lineState, nestedState);
		}
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_DART_COMMENTLINE);
		sc.Forward();
		lineStateLineType = DartLineStateMaskLineComment;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_DART_OPERATOR:
		case SCE_DART_OPERATOR2:
			sc.SetState(SCE_DART_DEFAULT);
			break;

		case SCE_DART_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_DART_WORD);
					if (StrEqualsAny(s, "import", "part")) {
						if (visibleChars == sc.LengthCurrent()) {
							lineStateLineType = DartLineStateMaskImport;
						}
					} else if (StrEqualsAny(s, "class", "extends", "implements", "new", "throw", "as", "is")) {
						kwType = SCE_DART_CLASS;
					} else if (StrEqual(s, "enum")) {
						kwType = SCE_DART_ENUM;
					} else if (StrEqualsAny(s, "break", "continue")) {
						kwType = SCE_DART_LABEL;
					}
					if (kwType != SCE_DART_DEFAULT) {
						const int chNext = sc.GetLineNextChar();
						if (!IsIdentifierStartEx(chNext)) {
							kwType = SCE_DART_DEFAULT;
						}
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_DART_WORD2);
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_DART_CLASS);
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_DART_ENUM);
				} else if (sc.ch == ':') {
					if (visibleChars == sc.LengthCurrent()) {
						const int chNext = sc.GetLineNextChar(true);
						if (IsJumpLabelNextChar(chNext)) {
							sc.ChangeState(SCE_DART_LABEL);
						}
					}
				} else if (sc.ch != '.') {
					if (kwType != SCE_DART_DEFAULT) {
						sc.ChangeState(kwType);
					} else {
						const int chNext = sc.GetDocNextChar(sc.ch == '?');
						if (chNext == '(') {
							sc.ChangeState(SCE_DART_FUNCTION);
						} else if ((chBeforeIdentifier == '<' && (chNext == '>' || chNext == '<'))
							|| IsIdentifierStartEx(chNext)) {
							// type<type>
							// type<type?>
							// type<type<type>>
							// type identifier
							// type? identifier
							sc.ChangeState(SCE_DART_CLASS);
						}
					}
				}
				if (sc.state != SCE_DART_WORD && sc.ch != '.') {
					kwType = SCE_DART_DEFAULT;
				}
				sc.SetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_METADATA:
		case SCE_DART_SYMBOL_IDENTIFIER:
			if (sc.ch == '.') {
				const int state = sc.state;
				sc.SetState(SCE_DART_OPERATOR);
				sc.ForwardSetState(state);
				continue;
			}
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_SYMBOL_OPERATOR:
			if (!IsDeclarableOperator(sc.ch)) {
				sc.SetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_COMMENTLINE:
		case SCE_DART_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_DART_DEFAULT);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_DART_TASKMARKER);
			}
			break;

		case SCE_DART_COMMENTBLOCK:
		case SCE_DART_COMMENTBLOCKDOC:
			if (sc.Match('*', '/')) {
				sc.Forward();
				--commentLevel;
				if (commentLevel == 0) {
					sc.ForwardSetState(SCE_DART_DEFAULT);
				}
			} else if (sc.Match('/', '*')) {
				sc.Forward();
				++commentLevel;
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_DART_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_DART_RAWSTRING_SQ:
		case SCE_DART_RAWSTRING_DQ:
			if (sc.atLineStart) {
				sc.SetState(SCE_DART_DEFAULT);
			} else if ((sc.state == SCE_DART_RAWSTRING_SQ && sc.ch == '\'')
				|| (sc.state == SCE_DART_RAWSTRING_DQ && sc.ch == '"')) {
				sc.ForwardSetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_TRIPLE_RAWSTRING_SQ:
		case SCE_DART_TRIPLE_RAWSTRING_DQ:
			if ((sc.state == SCE_DART_TRIPLE_RAWSTRING_SQ && sc.Match('\'', '\'', '\''))
				|| (sc.state == SCE_DART_TRIPLE_RAWSTRING_DQ && sc.Match('"', '"', '"'))) {
				sc.Forward(2);
				sc.ForwardSetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_STRING_SQ:
		case SCE_DART_STRING_DQ:
		case SCE_DART_TRIPLE_STRING_SQ:
		case SCE_DART_TRIPLE_STRING_DQ:
			if ((sc.state == SCE_DART_STRING_SQ || sc.state == SCE_DART_STRING_DQ) && sc.atLineStart) {
				sc.SetState(SCE_DART_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_DART_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				escSeq.outerState = sc.state;
				sc.SetState(SCE_DART_VARIABLE);
			} else if (sc.Match('$', '{')) {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_DART_OPERATOR2);
				sc.Forward();
			} else if (sc.ch == '\'' && (sc.state == SCE_DART_STRING_SQ
				|| (sc.state == SCE_DART_TRIPLE_STRING_SQ && sc.Match('\'', '\'', '\'')))) {
				if (sc.state == SCE_DART_TRIPLE_STRING_SQ) {
					sc.SetState(SCE_DART_TRIPLE_STRING_SQEND);
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_DART_DEFAULT);
			} else if (sc.ch == '"' && (sc.state == SCE_DART_STRING_DQ
				|| (sc.state == SCE_DART_TRIPLE_STRING_DQ && sc.Match('"', '"', '"')))) {
				if (sc.state == SCE_DART_TRIPLE_STRING_DQ) {
					sc.SetState(SCE_DART_TRIPLE_STRING_DQEND);
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_DART_VARIABLE:
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;
		}

		if (sc.state == SCE_DART_DEFAULT) {
			if (sc.ch == '/' && (sc.chNext == '/' || sc.chNext == '*')) {
				visibleCharsBefore = visibleChars;
				const int chNext = sc.chNext;
				sc.SetState((chNext == '/') ? SCE_DART_COMMENTLINE : SCE_DART_COMMENTBLOCK);
				sc.Forward(2);
				if (sc.ch == chNext && sc.chNext != chNext) {
					sc.ChangeState((chNext == '/') ? SCE_DART_COMMENTLINEDOC : SCE_DART_COMMENTBLOCKDOC);
				}
				if (chNext == '/') {
					if (visibleChars == 0) {
						lineStateLineType = DartLineStateMaskLineComment;
					}
				 } else {
					commentLevel = 1;
				 }
				 continue;
			} else if (sc.ch == 'r' && (sc.chNext == '\'' || sc.chNext == '"')) {
				sc.SetState((sc.chNext == '\'') ? SCE_DART_RAWSTRING_SQ : SCE_DART_RAWSTRING_DQ);
				sc.Forward(2);
				if (sc.chPrev == '\'' && sc.Match('\'', '\'')) {
					sc.ChangeState(SCE_DART_TRIPLE_RAWSTRING_SQ);
					sc.Forward(2);
				} else if (sc.chPrev == '"' && sc.Match('"', '"')) {
					sc.ChangeState(SCE_DART_TRIPLE_RAWSTRING_DQ);
					sc.Forward(2);
				}
				continue;
			} else if (sc.Match('"', '"', '"')) {
				sc.SetState(SCE_DART_TRIPLE_STRING_DQSTART);
				sc.Forward(2);
				sc.ForwardSetState(SCE_DART_TRIPLE_STRING_DQ);
				continue;
			} else if (sc.ch == '"') {
				sc.SetState(SCE_DART_STRING_DQ);
			} else if (sc.Match('\'', '\'', '\'')) {
				sc.SetState(SCE_DART_TRIPLE_STRING_SQSTART);
				sc.Forward(2);
				sc.ForwardSetState(SCE_DART_TRIPLE_STRING_SQ);
				continue;
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_DART_STRING_SQ);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_DART_NUMBER);
			} else if ((sc.ch == '@' || sc.ch == '$') && IsIdentifierStartEx(sc.chNext)) {
				escSeq.outerState = SCE_DART_DEFAULT;
				sc.SetState((sc.ch == '@') ? SCE_DART_METADATA : SCE_DART_VARIABLE);
			} else if (sc.ch == '#') {
				if (IsIdentifierStartEx(sc.chNext)) {
					sc.SetState(SCE_DART_SYMBOL_IDENTIFIER);
				} else if (IsDeclarableOperator(sc.chNext)) {
					sc.SetState(SCE_DART_SYMBOL_OPERATOR);
				}
			} else if (IsIdentifierStartEx(sc.ch)) {
				if (sc.chPrev != '.') {
					chBeforeIdentifier = sc.chPrev;
				}
				sc.SetState(SCE_DART_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				const bool interpolating = !nestedState.empty();
				sc.SetState(interpolating ? SCE_DART_OPERATOR2 : SCE_DART_OPERATOR);
				if (interpolating) {
					if (sc.ch == '{') {
						nestedState.push_back(SCE_DART_DEFAULT);
					} else if (sc.ch == '}') {
						const int outerState = TakeAndPop(nestedState);
						sc.ForwardSetState(outerState);
						continue;
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			int lineState = (commentLevel << 2) | lineStateLineType;
			if (!nestedState.empty()) {
				lineState |= PackLineState(nestedState) << 8;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			kwType = SCE_DART_DEFAULT;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int packageImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & DartLineStateMaskLineComment),
		packageImport((lineState >> 1) & 1) {
	}
};

void FoldDartDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_DART_OPERATOR, SCE_DART_TASKMARKER);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_DART_COMMENTBLOCKDOC:
		case SCE_DART_COMMENTBLOCK:  {
			const int level = (ch == '/' && chNext == '*') ? 1 : ((ch == '*' && chNext == '/') ? -1 : 0);
			if (level != 0) {
				levelNext += level;
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
				styleNext = styler.StyleAt(i + 1);
			}
		} break;

		case SCE_DART_TRIPLE_RAWSTRING_SQ:
		case SCE_DART_TRIPLE_RAWSTRING_DQ:
			if (style != stylePrev) {
				levelNext++;
			} else if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_DART_TRIPLE_STRING_SQSTART:
		case SCE_DART_TRIPLE_STRING_DQSTART:
			if (style != stylePrev) {
				levelNext++;
			}
			break;

		case SCE_DART_TRIPLE_STRING_SQEND:
		case SCE_DART_TRIPLE_STRING_DQEND:
			if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_DART_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
			break;
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.packageImport) {
				levelNext += foldNext.packageImport - foldPrev.packageImport;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_DART_OPERATOR, SCE_DART_TASKMARKER);
				if (bracePos) {
					levelNext++;
					i = bracePos; // skip the brace
					chNext = '\0';
				}
			}

			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}

			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineEndPos = sci::min(lineStartNext, endPos) - 1;
			levelCurrent = levelNext;
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
			visibleChars = 0;
		}
	}
}

}

LexerModule lmDart(SCLEX_DART, ColouriseDartDoc, "dart", FoldDartDoc);
