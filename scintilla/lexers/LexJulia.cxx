// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Julia

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
	int outerState = SCE_JULIA_DEFAULT;
	int digitsLeft = 0;
	int numBase = 0;

	// highlight any character as escape sequence.
	bool resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = 1;
		numBase = 16;
		if (state == SCE_JULIA_RAWSTRING || state == SCE_JULIA_TRIPLE_RAWSTRING) {
			// TODO: only when backslash followed by double quote
			return chNext == '\\' || chNext == '\"';
		}

		if (chNext == 'x') {
			digitsLeft = 3;
		} else if (chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'U') {
			digitsLeft = 9;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			numBase = 8;
		}
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsADigit(ch, numBase);
	}
};

constexpr bool IsJuliaExponent(int base, int ch, int chNext) noexcept {
	return ((base == 10 && (ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F'))
		|| (base == 16 && (ch == 'p' || ch == 'P')))
		&& (chNext == '+' || chNext == '-' || IsADigit(chNext));
}

constexpr bool IsJuliaRegexFlag(int ch) noexcept {
	return ch == 'i' || ch == 'm' || ch == 's' || ch == 'x' || ch == 'a';
}

static_assert(DefaultNestedStateBaseStyle + 1 == SCE_JULIA_STRING);
static_assert(DefaultNestedStateBaseStyle + 2 == SCE_JULIA_TRIPLE_STRING);
static_assert(DefaultNestedStateBaseStyle + 3 == SCE_JULIA_BACKTICKS);
static_assert(DefaultNestedStateBaseStyle + 4 == SCE_JULIA_TRIPLE_BACKTICKS);

void ColouriseJuliaDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int commentLevel = 0;

	int kwType = SCE_JULIA_DEFAULT;
	bool maybeType = false;

	int braceCount = 0;
	std::vector<int> nestedState; // string interpolation "$()"

	int chBeforeIdentifier = 0;
	bool isTransposeOperator = false; // "'"

	int visibleChars = 0;
	int visibleCharsBefore = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		1: lineStateLineComment
		5: commentLevel
		8: braceCount
		3: nestedState count
		3*4: nestedState
		*/
		commentLevel = (lineState >> 1) & 0x1f;
		braceCount = (lineState >> 6) & 0xff;
		lineState >>= 14;
		if (lineState) {
			UnpackLineState(lineState, nestedState);
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_JULIA_OPERATOR:
		case SCE_JULIA_OPERATOR2:
			sc.SetState(SCE_JULIA_DEFAULT);
			break;

		case SCE_JULIA_NUMBER:
			// strict to avoid color multiply expression as number
			if (!(IsADigit(sc.ch, escSeq.numBase) || sc.ch == '_')) {
				if (IsJuliaExponent(escSeq.numBase, sc.ch, sc.chNext)) {
					sc.Forward();
				} else if (!(escSeq.numBase == 10 && sc.ch == '.' && sc.chPrev != '.')) {
					sc.SetState(SCE_JULIA_DEFAULT);
				}
			}
			break;

		case SCE_JULIA_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					if (StrEqual(s, "type")) {
						// only in `abstract type` or `primitive type`
						if (kwType == SCE_JULIA_WORD) {
							sc.ChangeState(SCE_JULIA_WORD);
							kwType = SCE_JULIA_TYPE;
						}
					} else if (braceCount > 0 || (chBeforeIdentifier == '.' || chBeforeIdentifier == ':' || sc.ch == '.' || sc.ch == ':')) {
						sc.ChangeState(SCE_JULIA_WORD_DEMOTED);
					} else {
						isTransposeOperator = false;
						sc.ChangeState(SCE_JULIA_WORD);
						if (StrEqual(s, "struct")) {
							kwType = SCE_JULIA_TYPE;
						} else if (StrEqual(s, "macro")) {
							kwType = SCE_JULIA_MACRO;
						} else if (StrEqual(s, "function")) {
							kwType = SCE_JULIA_FUNCTION_DEFINE;
						} else if (StrEqualsAny(s, "abstract", "primitive")) {
							kwType = SCE_JULIA_WORD;
						}
					}
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_JULIA_TYPE);
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_JULIA_CONSTANT);
				} else if (keywordLists[4]->InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_JULIA_BASIC_FUNCTION);
				} else if (kwType != SCE_JULIA_DEFAULT && kwType != SCE_JULIA_WORD) {
					sc.ChangeState(kwType);
					if (kwType == SCE_JULIA_FUNCTION_DEFINE && sc.ch == '!') {
						sc.Forward();
					}
				} else {
					const int chNext = sc.GetDocNextChar();
					if (sc.ch == '!' || chNext == '(') {
						sc.ChangeState(SCE_JULIA_FUNCTION);
						if (sc.ch == '!') {
							sc.Forward();
						}
					} else if (maybeType || chNext == '{') {
						sc.ChangeState(SCE_JULIA_TYPE);
					}
				}
				if (sc.state != SCE_JULIA_WORD) {
					kwType = SCE_JULIA_DEFAULT;
				}
				maybeType = false;
				sc.SetState(SCE_JULIA_DEFAULT);
			}
			break;

		case SCE_JULIA_VARIABLE:
		case SCE_JULIA_MACRO:
		case SCE_JULIA_SYMBOL:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (sc.state == SCE_JULIA_VARIABLE && escSeq.outerState != SCE_JULIA_DEFAULT) {
					sc.SetState(escSeq.outerState);
					continue;
				}
				sc.SetState(SCE_JULIA_DEFAULT);
			}
			break;

		case SCE_JULIA_STRING:
		case SCE_JULIA_TRIPLE_STRING:
		case SCE_JULIA_BACKTICKS:
		case SCE_JULIA_TRIPLE_BACKTICKS:
			if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_JULIA_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				escSeq.outerState = sc.state;
				sc.SetState(SCE_JULIA_VARIABLE);
			} else if (sc.Match('$', '(')) {
				++braceCount;
				nestedState.push_back(sc.state);
				sc.SetState(SCE_JULIA_OPERATOR2);
				sc.Forward();
			} else if ((sc.state == SCE_JULIA_STRING && sc.ch == '\"')
				|| (sc.state == SCE_JULIA_TRIPLE_STRING && sc.Match('"', '"', '"'))
				|| (sc.state == SCE_JULIA_BACKTICKS && sc.ch == '`')
				|| (sc.state == SCE_JULIA_TRIPLE_BACKTICKS && sc.Match('`', '`', '`'))) {
				if (sc.state == SCE_JULIA_TRIPLE_STRING || sc.state == SCE_JULIA_TRIPLE_BACKTICKS) {
					sc.SetState((sc.state == SCE_JULIA_TRIPLE_STRING) ? SCE_JULIA_TRIPLE_STRINGEND : SCE_JULIA_TRIPLE_BACKTICKSEND);
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_JULIA_DEFAULT);
			}
			break;

		case SCE_JULIA_CHARACTER:
			if (sc.atLineStart) {
				// multiline when inside backticks
				sc.SetState(SCE_JULIA_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_JULIA_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_JULIA_DEFAULT);
			}
			break;

		case SCE_JULIA_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_JULIA_REGEX:
		case SCE_JULIA_TRIPLE_REGEX:
			if (sc.ch == '\\') {
				sc.Forward();
			//} else if (sc.ch == '#') { // regex comment
			} else if (sc.ch == '\"' && (sc.state == SCE_JULIA_REGEX || sc.Match('"', '"', '"'))) {
				if (sc.state == SCE_JULIA_TRIPLE_REGEX) {
					sc.Forward(2);
				}
				sc.Forward();
				while (IsJuliaRegexFlag(sc.ch)) {
					sc.Forward();
				}
				sc.SetState(SCE_JULIA_DEFAULT);
			}
			break;

		case SCE_JULIA_RAWSTRING:
		case SCE_JULIA_TRIPLE_RAWSTRING:
		case SCE_JULIA_BYTESTRING:
		case SCE_JULIA_TRIPLE_BYTESTRING:
			if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_JULIA_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\"' && (sc.state == SCE_JULIA_RAWSTRING || sc.state == SCE_JULIA_BYTESTRING || sc.Match('"', '"', '"'))) {
				if (sc.state == SCE_JULIA_TRIPLE_RAWSTRING || sc.state == SCE_JULIA_TRIPLE_BYTESTRING) {
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_JULIA_DEFAULT);
			}
			break;

		case SCE_JULIA_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_JULIA_DEFAULT);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_JULIA_TASKMARKER);
			}
			break;

		case SCE_JULIA_COMMENTBLOCK:
			if (sc.Match('=', '#')) {
				sc.Forward();
				--commentLevel;
				if (commentLevel == 0) {
					sc.ForwardSetState(SCE_JULIA_DEFAULT);
				}
			} else if (sc.Match('#', '=')) {
				sc.Forward();
				++commentLevel;
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_JULIA_TASKMARKER)) {
				continue;
			}
			break;
		}

		if (sc.state == SCE_JULIA_DEFAULT) {
			const bool transposeOperator = isTransposeOperator && sc.ch == '\'';
			const bool symbol = !isTransposeOperator && sc.ch == ':'; // not range expression
			isTransposeOperator = false; // space not allowed before "'"
			if (transposeOperator) {
				sc.SetState(SCE_JULIA_OPERATOR);
			} else if (sc.ch == '#') {
				visibleCharsBefore = visibleChars;
				if (sc.chNext == '=') {
					commentLevel = 1;
					sc.SetState(SCE_JULIA_COMMENTBLOCK);
					sc.Forward();
				} else {
					sc.SetState(SCE_JULIA_COMMENTLINE);
					if (visibleChars == 0) {
						lineStateLineComment = SimpleLineStateMaskLineComment;
					}
				}
			} else if (sc.Match('"', '"', '"')) {
				sc.SetState(SCE_JULIA_TRIPLE_STRINGSTART);
				sc.Forward(2);
				sc.ForwardSetState(SCE_JULIA_TRIPLE_STRING);
				continue;
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_JULIA_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_JULIA_CHARACTER);
			} else if (sc.Match('`', '`', '`')) {
				sc.SetState(SCE_JULIA_TRIPLE_BACKTICKSSTART);
				sc.Forward(2);
				sc.ForwardSetState(SCE_JULIA_TRIPLE_BACKTICKS);
				continue;
			} else if (sc.ch == '`') {
				sc.SetState(SCE_JULIA_BACKTICKS);
			} else if (sc.Match('r', '\"')) {
				sc.SetState(SCE_JULIA_REGEX);
				sc.Forward();
				if (sc.Match('"', '"', '"')) {
					sc.ChangeState(SCE_JULIA_TRIPLE_REGEX);
					sc.Forward(2);
				}
			} else if (sc.Match('r', 'a', 'w', '"')) {
				sc.SetState(SCE_JULIA_RAWSTRING);
				sc.Forward(3);
				if (sc.Match('"', '"', '"')) {
					sc.ChangeState(SCE_JULIA_TRIPLE_RAWSTRING);
					sc.Forward(2);
				}
			} else if (sc.Match('b', '\"')) {
				sc.SetState(SCE_JULIA_BYTESTRING);
				sc.Forward();
				if (sc.Match('"', '"', '"')) {
					sc.ChangeState(SCE_JULIA_TRIPLE_BYTESTRING);
					sc.Forward(2);
				}
			} else if (sc.ch == '@' && IsIdentifierStartEx(sc.chNext)) {
				sc.SetState(SCE_JULIA_MACRO);
			} else if (symbol && IsIdentifierStartEx(sc.chNext)) {
				sc.SetState(SCE_JULIA_SYMBOL);
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				escSeq.outerState = SCE_JULIA_DEFAULT;
				isTransposeOperator = true;
				sc.SetState(SCE_JULIA_VARIABLE);
			} else if (sc.ch == '0') {
				escSeq.numBase = 10;
				isTransposeOperator = true;
				sc.SetState(SCE_JULIA_NUMBER);
				if (sc.chNext == 'x' || sc.chNext == 'X') {
					escSeq.numBase = 16;
				} else if (sc.chNext == 'b' || sc.chNext == 'B') {
					escSeq.numBase = 2;
				} else if (sc.chNext == 'o' || sc.chNext == 'O') {
					escSeq.numBase = 8;
				}
				if (escSeq.numBase != 10) {
					sc.Forward();
				}
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				escSeq.numBase = 10;
				isTransposeOperator = true;
				sc.SetState(SCE_JULIA_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				chBeforeIdentifier = sc.chPrev;
				isTransposeOperator = true;
				sc.SetState(SCE_JULIA_IDENTIFIER);
			} else if (isoperator(sc.ch) || sc.ch == '@' || sc.ch == '\\' || sc.ch == '$') {
				if (sc.ch == '{' || sc.ch == '[' || sc.ch == '(') {
					++braceCount;
				} else if (sc.ch == '}' || sc.ch == ']' || sc.ch == ')') {
					--braceCount;
					isTransposeOperator = true;
				}

				const bool interpolating = !nestedState.empty();
				sc.SetState(interpolating ? SCE_JULIA_OPERATOR2 : SCE_JULIA_OPERATOR);
				if (interpolating) {
					if (sc.ch == '(') {
						nestedState.push_back(SCE_JULIA_DEFAULT);
					} else if (sc.ch == ')') {
						const int outerState = TakeAndPop(nestedState);
						sc.ForwardSetState(outerState);
						continue;
					}
				}

				if (sc.chNext == ':' && ( sc.ch == ':' || sc.ch == '<' || sc.ch == '>')) {
					// type after ::, <:, >:
					maybeType = true;
					sc.Forward();
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			int lineState = (braceCount << 6) | (commentLevel << 1) | lineStateLineComment;
			if (!nestedState.empty()) {
				lineState |= PackLineState(nestedState) << 14;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineComment = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr bool IsStringInnerStyle(int style) noexcept {
	return style == SCE_JULIA_ESCAPECHAR;
}

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & SimpleLineStateMaskLineComment;
}

void FoldJuliaDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = GetLineCommentState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = GetLineCommentState(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	char buf[12]; // baremodule
	constexpr int MaxFoldWordLength = sizeof(buf) - 1;
	int wordLen = 0;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_JULIA_COMMENTBLOCK: {
			const int level = (ch == '#' && chNext == '=') ? 1 : ((ch == '=' && chNext == '#') ? -1 : 0);
			if (level != 0) {
				levelNext += level;
				i++;
				style = styleNext;
				chNext = styler.SafeGetCharAt(i + 1);
				styleNext = styler.StyleAt(i + 1);
			}
		} break;

		case SCE_JULIA_WORD:
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = ch;
			}
			if (styleNext != SCE_JULIA_WORD) {
				buf[wordLen] = '\0';
				wordLen = 0;
				if (StrEqual(buf, "end")) {
					levelNext--;
				} else if (keywordLists[1]->InList(buf)) {
					levelNext++;
				}
			}
			break;

		case SCE_JULIA_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
			break;

		case SCE_JULIA_TRIPLE_STRINGSTART:
		case SCE_JULIA_TRIPLE_BACKTICKSSTART:
			if (style != stylePrev) {
				levelNext++;
			}
			break;

		case SCE_JULIA_TRIPLE_STRINGEND:
		case SCE_JULIA_TRIPLE_BACKTICKSEND:
			if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_JULIA_RAWSTRING:
		case SCE_JULIA_TRIPLE_RAWSTRING:
		case SCE_JULIA_BYTESTRING:
		case SCE_JULIA_TRIPLE_BYTESTRING:
		case SCE_JULIA_REGEX:
		case SCE_JULIA_TRIPLE_REGEX:
			if (style != stylePrev && !IsStringInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsStringInnerStyle(styleNext)) {
				levelNext--;
			}
			break;
		}

		if (i == lineEndPos) {
			const int lineCommentNext = GetLineCommentState(styler.GetLineState(lineCurrent + 1));
			if (lineCommentCurrent) {
				levelNext += lineCommentNext - lineCommentPrev;
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
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
		}
	}
}

}

LexerModule lmJulia(SCLEX_JULIA, ColouriseJuliaDoc, "julia", FoldJuliaDoc);
