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
#include "LexerModule.h"
#include "LexerUtils.h"

using namespace Scintilla;

namespace {

struct EscapeSequence {
	int outerState = SCE_JULIA_DEFAULT;
	int digitsLeft = 0;
	int numBase = 16;

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

enum {
	MaxJuliaNestedStateCount = 3,
	JuliaLineStateMaskLineComment = (1 << 21), // line comment
};

constexpr int PackState(int state) noexcept {
	switch (state) {
	case SCE_JULIA_STRING:
		return 1;
	case SCE_JULIA_TRIPLE_STRING:
		return 2;
	case SCE_JULIA_BACKTICKS:
		return 3;
	case SCE_JULIA_TRIPLE_BACKTICKS:
		return 4;
	default:
		return 0;
	}
}

constexpr int UnpackState(int state) noexcept  {
	switch (state) {
	case 1:
		return SCE_JULIA_STRING;
	case 2:
		return SCE_JULIA_TRIPLE_STRING;
	case 3:
		return SCE_JULIA_BACKTICKS;
	case 4:
		return SCE_JULIA_TRIPLE_BACKTICKS;
	default:
		return SCE_JULIA_DEFAULT;
	}
}

int PackNestedState(const std::vector<int>& nestedState) noexcept {
	return PackLineState<3, MaxJuliaNestedStateCount, PackState>(nestedState) << 22;
}

void UnpackNestedState(int lineState, int count, std::vector<int>& nestedState) {
	UnpackLineState<3, MaxJuliaNestedStateCount, UnpackState>(lineState, count, nestedState);
}

void ColouriseJuliaDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int commentLevel = 0;

	int numBase = 10;
	int kwType = SCE_JULIA_DEFAULT;
	bool maybeType = false;

	int braceCount = 0;
	int parenCount = 0; // $()
	int variableOuter = SCE_JULIA_DEFAULT;	// variable inside string
	std::vector<int> nestedState;

	int chBeforeIdentifier = 0;
	bool isTransposeOperator = false; // "'"

	int visibleChars = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		8: braceCount
		8: parenCount
		5: commentLevel
		1: lineStateLineComment
		9: nestedState
		*/
		braceCount = lineState & 0xff;
		parenCount = (lineState >> 8) & 0xff;
		commentLevel = (lineState >> 16) & 0x1f;
		if (parenCount != 0) {
			UnpackNestedState(lineState >> 22, parenCount, nestedState);
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
			if (!(IsADigit(sc.ch, numBase) || sc.ch == '_')) {
				if (IsJuliaExponent(numBase, sc.ch, sc.chNext)) {
					sc.Forward();
				} else if (!(numBase == 10 && sc.ch == '.' && sc.chPrev != '.')) {
					sc.SetState(SCE_JULIA_DEFAULT);
				}
			}
			break;

		case SCE_JULIA_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					if (strcmp(s, "type") == 0 ) {
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
						if (strcmp(s, "struct") == 0) {
							kwType = SCE_JULIA_TYPE;
						} else if (strcmp(s, "macro") == 0) {
							kwType = SCE_JULIA_MACRO;
						} else if (strcmp(s, "function") == 0) {
							kwType = SCE_JULIA_FUNCTION_DEFINE;
						} else if (strcmp(s, "abstract") == 0 || strcmp(s, "primitive") == 0) {
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
					const int chNext = sc.GetNextNSChar();
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
				if (sc.state == SCE_JULIA_VARIABLE && variableOuter != SCE_JULIA_DEFAULT) {
					sc.SetState(variableOuter);
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
				variableOuter = sc.state;
				sc.SetState(SCE_JULIA_VARIABLE);
			} else if (sc.Match('$', '(')) {
				++braceCount;
				++parenCount;
				nestedState.push_back(sc.state);
				sc.SetState(SCE_JULIA_OPERATOR2);
				sc.Forward();
			} else if (parenCount && sc.ch == ')') {
				const int outerState = nestedState.empty()? SCE_JULIA_DEFAULT : nestedState.back();
				if (!nestedState.empty()) {
					nestedState.pop_back();
				}
				--braceCount;
				--parenCount;
				sc.SetState(SCE_JULIA_OPERATOR2);
				sc.ForwardSetState(outerState);
				continue;
			} else if ((sc.state == SCE_JULIA_STRING && sc.ch == '\"')
				|| (sc.state == SCE_JULIA_TRIPLE_STRING && sc.Match(R"(""")"))
				|| (sc.state == SCE_JULIA_BACKTICKS && sc.ch == '`')
				|| (sc.state == SCE_JULIA_TRIPLE_BACKTICKS && sc.Match("```"))) {
				if (sc.state == SCE_JULIA_TRIPLE_STRING || sc.state == SCE_JULIA_TRIPLE_BACKTICKS) {
					sc.Forward(2);
					sc.SetState((sc.state == SCE_JULIA_TRIPLE_STRING) ? SCE_JULIA_TRIPLE_STRINGEND : SCE_JULIA_TRIPLE_BACKTICKSEND);
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
				const int outerState = escSeq.outerState;
				if (outerState != SCE_JULIA_CHARACTER && sc.ch == '\\' && escSeq.resetEscapeState(outerState, sc.chNext)) {
					sc.Forward();
				} else {
					sc.SetState(outerState);
					continue;
				}
			}
			break;

		case SCE_JULIA_REGEX:
		case SCE_JULIA_TRIPLE_REGEX:
			if (sc.ch == '\\') {
				sc.Forward();
			//} else if (sc.ch == '#') { // regex comment
			} else if (sc.ch == '\"' && (sc.state == SCE_JULIA_REGEX || sc.Match(R"(""")"))) {
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
			} else if (sc.ch == '\"' && (sc.state == SCE_JULIA_RAWSTRING || sc.state == SCE_JULIA_BYTESTRING || sc.Match(R"(""")"))) {
				if (sc.state == SCE_JULIA_TRIPLE_RAWSTRING || sc.state == SCE_JULIA_TRIPLE_BYTESTRING) {
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_JULIA_DEFAULT);
			}
			break;

		case SCE_JULIA_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_JULIA_DEFAULT);
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
				sc.Forward(2);
				++commentLevel;
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
				if (sc.chNext == '=') {
					commentLevel = 1;
					sc.SetState(SCE_JULIA_COMMENTBLOCK);
					sc.Forward();
				} else {
					sc.SetState(SCE_JULIA_COMMENTLINE);
					if (visibleChars == 0) {
						lineStateLineComment = JuliaLineStateMaskLineComment;
					}
				}
			} else if (sc.Match(R"(""")")) {
				sc.SetState(SCE_JULIA_TRIPLE_STRINGSTART);
				sc.ForwardSetState(SCE_JULIA_TRIPLE_STRING);
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_JULIA_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_JULIA_CHARACTER);
			} else if (sc.Match("```")) {
				sc.SetState(SCE_JULIA_TRIPLE_BACKTICKSSTART);
				sc.ForwardSetState(SCE_JULIA_TRIPLE_BACKTICKS);
				sc.Forward();
			} else if (sc.ch == '`') {
				sc.SetState(SCE_JULIA_BACKTICKS);
			} else if (sc.Match('r', '\"')) {
				sc.SetState(SCE_JULIA_REGEX);
				sc.Forward();
				if (sc.Match(R"(""")")) {
					sc.ChangeState(SCE_JULIA_TRIPLE_REGEX);
					sc.Forward(2);
				}
			} else if (sc.Match("raw\"")) {
				sc.SetState(SCE_JULIA_RAWSTRING);
				sc.Forward(3);
				if (sc.Match(R"(""")")) {
					sc.ChangeState(SCE_JULIA_TRIPLE_RAWSTRING);
					sc.Forward(2);
				}
			} else if (sc.Match('b', '\"')) {
				sc.SetState(SCE_JULIA_BYTESTRING);
				sc.Forward();
				if (sc.Match(R"(""")")) {
					sc.ChangeState(SCE_JULIA_TRIPLE_BYTESTRING);
					sc.Forward(2);
				}
			} else if (sc.ch == '@' && IsIdentifierStartEx(sc.chNext)) {
				sc.SetState(SCE_JULIA_MACRO);
			} else if (symbol && IsIdentifierStartEx(sc.chNext)) {
				sc.SetState(SCE_JULIA_SYMBOL);
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				variableOuter = SCE_JULIA_DEFAULT;
				isTransposeOperator = true;
				sc.SetState(SCE_JULIA_VARIABLE);
			} else if (sc.ch == '0') {
				numBase = 10;
				isTransposeOperator = true;
				sc.SetState(SCE_JULIA_NUMBER);
				if (sc.chNext == 'x' || sc.chNext == 'X') {
					numBase = 16;
				} else if (sc.chNext == 'b' || sc.chNext == 'B') {
					numBase = 2;
				} else if (sc.chNext == 'o' || sc.chNext == 'O') {
					numBase = 8;
				}
				if (numBase != 10) {
					sc.Forward();
				}
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				numBase = 10;
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

				sc.SetState(parenCount ? SCE_JULIA_OPERATOR2 : SCE_JULIA_OPERATOR);
				if (parenCount) {
					if (sc.ch == '(') {
						++parenCount;
						nestedState.push_back(SCE_JULIA_DEFAULT);
					} else if (sc.ch == ')') {
						--parenCount;
						const int outerState = nestedState.empty() ? SCE_JULIA_DEFAULT : nestedState.back();
						if (!nestedState.empty()) {
							nestedState.pop_back();
						}
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

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			int lineState = braceCount | (parenCount << 8) | (commentLevel << 16) | lineStateLineComment;
			if (parenCount) {
				lineState |= PackNestedState(nestedState);
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineComment = 0;
			visibleChars = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr bool IsMultilineStringStyle(int style) noexcept {
	return style == SCE_JULIA_RAWSTRING || style == SCE_JULIA_TRIPLE_RAWSTRING
		|| style == SCE_JULIA_BYTESTRING || style == SCE_JULIA_TRIPLE_BYTESTRING
		|| style == SCE_JULIA_REGEX || style == SCE_JULIA_TRIPLE_REGEX;
}

constexpr bool IsStringInnerStyle(int style) noexcept {
	return style == SCE_JULIA_ESCAPECHAR;
}

void FoldJuliaDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = styler.GetLineState(lineCurrent - 1) & JuliaLineStateMaskLineComment;
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = styler.GetLineState(lineCurrent) & JuliaLineStateMaskLineComment;
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	constexpr int MaxFoldWordLength = 10 + 1; // baremodule
	char buf[MaxFoldWordLength + 1] = "";
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

		if (style == SCE_JULIA_COMMENTBLOCK) {
			if (foldComment) {
				const int level = (ch == '#' && chNext == '=') ? 1 : ((ch == '=' && chNext == '#') ? -1 : 0);
				if (level != 0) {
					levelNext += level;
					i++;
					style = styleNext;
					chNext = styler.SafeGetCharAt(i + 1);
					styleNext = styler.StyleAt(i + 1);
				}
			}
		} else if (style == SCE_JULIA_WORD) {
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = ch;
			}
			if (styleNext != SCE_JULIA_WORD) {
				buf[wordLen] = '\0';
				wordLen = 0;
				if (strcmp(buf, "end") == 0) {
					levelNext--;
				} else if (keywordLists[1]->InList(buf)) {
					levelNext++;
				}
			}
		} else if (style == SCE_JULIA_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		} else if (style == SCE_JULIA_TRIPLE_STRINGSTART || style == SCE_JULIA_TRIPLE_BACKTICKSSTART) {
			levelNext++;
		} else if (style == SCE_JULIA_TRIPLE_STRINGEND || style == SCE_JULIA_TRIPLE_BACKTICKSEND) {
			levelNext--;
		} else if (IsMultilineStringStyle(style)) {
			if (style != stylePrev && !IsStringInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsStringInnerStyle(styleNext)) {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineCommentNext = styler.GetLineState(lineCurrent + 1) & JuliaLineStateMaskLineComment;
			if (foldComment && lineCommentCurrent) {
				if (!lineCommentPrev && lineCommentNext) {
					levelNext++;
				} else if (lineCommentPrev && !lineCommentNext) {
					levelNext--;
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
			lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;
			levelCurrent = levelNext;
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
		}
	}
}

}

LexerModule lmJulia(SCLEX_JULIA, ColouriseJuliaDoc, "julia", FoldJuliaDoc);
