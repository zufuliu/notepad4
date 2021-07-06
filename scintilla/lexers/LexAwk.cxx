// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Awk.

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>

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

using namespace Lexilla;

namespace {

struct EscapeSequence {
	int digitsLeft = 0;
	int numBase = 0;

	// highlight any character as escape sequence.
	// https://www.gnu.org/software/gawk/manual/html_node/Escape-Sequences.html
	void resetEscapeState(int chNext) noexcept {
		digitsLeft = 1;
		numBase = 16;
		if (chNext == 'x') {
			digitsLeft = 3;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			numBase = 8;
		}
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsADigit(ch, numBase);
	}
};

enum {
	AwkLineStateMaskLineComment = 1,	// line comment
	AwkLineStateMaskInclude = 1 << 1,	// @include
	AwkLineStateLineContinuation = 1 << 2,
};

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_AWK_TASKMARKER;
}

constexpr bool IsFormatSpecifier(char ch) noexcept {
	// https://www.gnu.org/software/gawk/manual/html_node/Control-Letters.html
	return AnyOf(ch, 'a', 'A',
					'c',
					'd',
					'e', 'E',
					'f', 'F',
					'g', 'G',
					'i',
					'o',
					's',
					'u',
					'x', 'X');
}

constexpr bool IsTimeFormatSpecifier(int ch) noexcept {
	// https://www.gnu.org/software/gawk/manual/html_node/Time-Functions.html
	return AnyOf(ch, 'a', 'A',
					'b', 'B',
					'c', 'C',
					'd', 'D',
					'e',
					'F',
					'g', 'G',
					'h', 'H',
					'I',
					'j',
					'm', 'M',
					'n',
					'p',
					'r', 'R',
					'S',
					't', 'T',
					'u', 'U',
					'V',
					'w', 'W',
					'x', 'X',
					'y', 'Y',
					'z', 'Z',
					// GNU C
					'k',
					'l',
					's'
	);
}

constexpr bool IsTimeFormatSpecifier(int ch, char chNext) noexcept {
	// Alternative representations
	return (ch == 'E' && AnyOf(chNext, 'c', 'C', 'x', 'X', 'y', 'Y'))
		|| (ch == 'O' && AnyOf(chNext, 'd', 'e', 'H', 'I', 'm', 'M', 'S', 'u', 'U', 'V', 'w', 'W', 'y'));
}

inline Sci_Position CheckFormatSpecifier(const StyleContext &sc, LexAccessor &styler, bool insideUrl) noexcept {
	if (sc.chNext == '%') {
		return 2;
	}
	if (insideUrl && IsHexDigit(sc.chNext)) {
		// percent encoded URL string
		return 0;
	}
	if (IsTimeFormatSpecifier(sc.chNext)) {
		return 2;
	}
	if (IsASpaceOrTab(sc.chNext) && IsADigit(sc.chPrev)) {
		// ignore word after percent: "5% x"
		return 0;
	}

	Sci_PositionU pos = sc.currentPos + 1;
	if (sc.chNext == 'E' || sc.chNext == 'O') {
		const char ch = styler.SafeGetCharAt(pos + 2);
		if (IsTimeFormatSpecifier(sc.chNext, ch)) {
			return 3;
		}
	}

	// https://www.gnu.org/software/gawk/manual/html_node/Format-Modifiers.html
	char ch = styler.SafeGetCharAt(pos);
	// positional specifier
	while (IsADigit(ch)) {
		ch = styler.SafeGetCharAt(++pos);
	}
	if (ch == '$' && IsADigit(sc.chNext)) {
		ch = styler.SafeGetCharAt(++pos);
	}
	// modifiers
	while (AnyOf(ch, ' ', '+', '-', '#', '0', '\'')) {
		ch = styler.SafeGetCharAt(++pos);
	}
	// width
	if (ch == '*') {
		ch = styler.SafeGetCharAt(++pos);
	} else {
		while (IsADigit(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
	}
	// .precision
	if (ch == '.') {
		ch = styler.SafeGetCharAt(++pos);
		if (ch == '*') {
			ch = styler.SafeGetCharAt(++pos);
		} else {
			while (IsADigit(ch)) {
				ch = styler.SafeGetCharAt(++pos);
			}
		}
	}
	// format-control letter
	if (IsFormatSpecifier(ch)) {
		return pos - sc.currentPos + 1;
	}
	return 0;
}

constexpr bool FollowExpression(int chPrevNonWhite, int stylePrevNonWhite) noexcept {
	return chPrevNonWhite == ')' || chPrevNonWhite == ']'
		|| stylePrevNonWhite == SCE_AWK_OPERATOR_PF
		|| IsIdentifierChar(chPrevNonWhite);
}

constexpr bool IsRegexStart(int chPrevNonWhite, int stylePrevNonWhite) noexcept {
	return stylePrevNonWhite == SCE_AWK_WORD || !FollowExpression(chPrevNonWhite, stylePrevNonWhite);
}

void ColouriseAwkDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;
	int lineContinuation = 0;
	bool insideRegexRange = false; // inside regex character range []
	bool insideUrl = false;

	int kwType = SCE_AWK_DEFAULT;
	int visibleChars = 0;
	int chPrevNonWhite = 0;
	int stylePrevNonWhite = SCE_AWK_DEFAULT;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		1: lineContinuation
		*/
		lineContinuation = lineState & AwkLineStateLineContinuation;
	}
	if (startPos != 0 && IsSpaceEquiv(initStyle)) {
		// look back for better regex colouring
		LookbackNonWhite(styler, startPos, SCE_AWK_TASKMARKER, chPrevNonWhite, stylePrevNonWhite);
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_AWK_OPERATOR:
		case SCE_AWK_OPERATOR_PF:
			sc.SetState(SCE_AWK_DEFAULT);
			break;

		case SCE_AWK_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_AWK_DEFAULT);
			}
			break;

		case SCE_AWK_VARIABLE:
		case SCE_AWK_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				if (sc.state == SCE_AWK_IDENTIFIER) {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_AWK_WORD);
						if (visibleChars == sc.LengthCurrent()) {
							if (StrEqual(s, "@include")) {
								lineStateLineType = AwkLineStateMaskInclude;
							} else if (StrEqual(s, "function")) {
								kwType = SCE_AWK_FUNCTION_DEFINE;
							}
						}
					} else if (keywordLists[1]->InList(s)) {
						sc.ChangeState(SCE_AWK_BUILTIN_VARIABLE);
					} else if (keywordLists[2]->InListPrefixed(s, '(')) {
						sc.ChangeState(SCE_AWK_BUILTIN_FUNCTION);
					} else {
						const int chNext = sc.GetLineNextChar();
						if (chNext == '(') {
							sc.ChangeState((kwType == SCE_AWK_FUNCTION_DEFINE) ? SCE_AWK_FUNCTION_DEFINE : SCE_AWK_FUNCTION);
						}
					}

					stylePrevNonWhite = sc.state;
					if (sc.state != SCE_AWK_WORD && sc.ch != ':') {
						kwType = SCE_AWK_DEFAULT;
					}
				}
				sc.SetState(SCE_AWK_DEFAULT);
			}
			break;

		case SCE_AWK_STRING:
			if (sc.ch == '\\') {
				if (IsEOLChar(sc.chNext)) {
					lineContinuation = AwkLineStateLineContinuation;
				} else {
					escSeq.resetEscapeState(sc.chNext);
					sc.SetState(SCE_AWK_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.atLineStart) {
				if (lineContinuation) {
					lineContinuation = 0;
				} else {
					sc.SetState(SCE_AWK_DEFAULT);
				}
			} else if (sc.ch == '%') {
				const Sci_Position length = CheckFormatSpecifier(sc, styler, insideUrl);
				if (length != 0) {
					sc.SetState(SCE_AWK_FORMAT_SPECIFIER);
					sc.Advance(length);
					sc.SetState(SCE_AWK_STRING);
					continue;
				}
			} else if (sc.ch == '"') {
				sc.ForwardSetState(SCE_AWK_DEFAULT);
			} else if (sc.Match(':', '/', '/') && IsLowerCase(sc.chPrev)) {
				insideUrl = true;
			} else if (insideUrl && IsInvalidUrlChar(sc.ch)) {
				insideUrl = false;
			}
			break;

		case SCE_AWK_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(SCE_AWK_STRING);
				continue;
			}
			break;

		case SCE_AWK_REGEX:
			if (sc.ch == '\\') {
				if (IsEOLChar(sc.chNext)) {
					lineContinuation = AwkLineStateLineContinuation;
				} else {
					sc.Forward();
				}
			} else if (sc.atLineStart) {
				if (lineContinuation) {
					lineContinuation = 0;
				} else {
					sc.SetState(SCE_AWK_DEFAULT);
				}
			} else if (sc.ch == '[' || sc.ch == ']') {
				insideRegexRange = sc.ch == '[';
			} else if (sc.ch == '/' && !insideRegexRange) {
				sc.ForwardSetState(SCE_AWK_DEFAULT);
			}
			break;

		case SCE_AWK_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_AWK_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_AWK_DEFAULT) {
			if (sc.ch == '#') {
				sc.SetState(SCE_AWK_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineType = AwkLineStateMaskLineComment;
				}
			} else if (sc.Match('@', '/')) {
				insideRegexRange = false;
				sc.SetState(SCE_AWK_REGEX);
				sc.Forward();
			} else if (sc.ch == '\"') {
				insideUrl = false;
				sc.SetState(SCE_AWK_STRING);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_AWK_NUMBER);
			} else if (IsIdentifierStart(sc.ch) || (sc.ch == '@' && IsIdentifierStart(sc.chNext))) {
				sc.SetState(SCE_AWK_IDENTIFIER);
			} else if (sc.ch == '+' || sc.ch == '-') {
				if (sc.ch == sc.chNext) {
					sc.SetState(SCE_AWK_OPERATOR_PF);
					sc.Forward();
				} else {
					sc.SetState(SCE_AWK_OPERATOR);
				}
			} else if (sc.ch == '/') {
				if (!IsEOLChar(sc.chNext) && IsRegexStart(chPrevNonWhite, stylePrevNonWhite)) {
					insideRegexRange = false;
					sc.SetState(SCE_AWK_REGEX);
				} else {
					sc.SetState(SCE_AWK_OPERATOR);
				}
			} else if (sc.ch == '$' && IsIdentifierChar(sc.chNext)) {
				sc.SetState(SCE_AWK_VARIABLE);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_AWK_OPERATOR);
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
			if (!IsSpaceEquiv(sc.state)) {
				chPrevNonWhite = sc.ch;
				stylePrevNonWhite = sc.state;
			}
		}
		if (sc.atLineEnd) {
			const int lineState = lineStateLineType | lineContinuation;
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			kwType = SCE_AWK_DEFAULT;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int fileInclude;
	int lineContinuation;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & AwkLineStateMaskLineComment),
		fileInclude((lineState >> 1) & 1),
		lineContinuation((lineState >> 2) & 1) {
	}
};

void FoldAwkDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_AWK_OPERATOR, SCE_AWK_TASKMARKER);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int style = styler.StyleAt(i);

		if (style == SCE_AWK_OPERATOR) {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.fileInclude) {
				levelNext += foldNext.fileInclude - foldPrev.fileInclude;
			} else if (foldCurrent.lineContinuation | foldPrev.lineContinuation) {
				levelNext += foldCurrent.lineContinuation - foldPrev.lineContinuation;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_AWK_OPERATOR, SCE_AWK_TASKMARKER);
				if (bracePos) {
					levelNext++;
					i = bracePos; // skip the brace
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

LexerModule lmAwk(SCLEX_AWK, ColouriseAwkDoc, "awk", FoldAwkDoc);
