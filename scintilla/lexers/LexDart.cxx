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
	MaxDartNestedStateCount = 4,
	DartLineStateMaskLineComment = 1,	// line comment
	DartLineStateMaskImport = (1 << 1),	// import
};

constexpr int PackState(int state) noexcept {
	switch (state) {
	case SCE_DART_STRING_SQ:
		return 1;
	case SCE_DART_STRING_DQ:
		return 2;
	case SCE_DART_TRIPLE_STRING_SQ:
		return 3;
	case SCE_DART_TRIPLE_STRING_DQ:
		return 4;
	default:
		return 0;
	}
}

constexpr int UnpackState(int state) noexcept  {
	switch (state) {
	case 1:
		return SCE_DART_STRING_SQ;
	case 2:
		return SCE_DART_STRING_DQ;
	case 3:
		return SCE_DART_TRIPLE_STRING_SQ;
	case 4:
		return SCE_DART_TRIPLE_STRING_DQ;
	default:
		return SCE_DART_DEFAULT;
	}
}

int PackNestedState(const std::vector<int>& nestedState) noexcept {
	return PackLineState<3, MaxDartNestedStateCount, PackState>(nestedState) << 16;
}

void UnpackNestedState(int lineState, int count, std::vector<int>& nestedState) {
	UnpackLineState<3, MaxDartNestedStateCount, UnpackState>(lineState, count, nestedState);
}

void ColouriseDartDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int lineStateImport = 0;
	int commentLevel = 0;	// nested block comment level

	int kwType = SCE_DART_DEFAULT;
	int chBeforeIdentifier = 0;

	int curlyBrace = 0; // "${}"
	int variableOuter = SCE_DART_DEFAULT;	// variable inside string
	std::vector<int> nestedState;

	int visibleChars = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		1: lineStateLineComment
		1: lineStateImport
		6: commentLevel
		8: curlyBrace
		3*4: nestedState
		*/
		commentLevel = (lineState >> 2) & 0x3f;
		curlyBrace = (lineState >> 8) & 0xff;
		if (curlyBrace) {
			UnpackNestedState((lineState >> 16) & 0xff, curlyBrace, nestedState);
		}
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_DART_COMMENTLINE);
		lineStateLineComment = DartLineStateMaskLineComment;
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
					if (EqualsAny(s, "import", "part")) {
						if (visibleChars == sc.LengthCurrent()) {
							lineStateImport = DartLineStateMaskImport;
						}
					} else if (EqualsAny(s, "as", "class", "extends", "implements", "is", "new", "throw")) {
						kwType = SCE_DART_CLASS;
					} else if (strcmp(s, "enum") == 0) {
						kwType = SCE_DART_ENUM;
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_DART_WORD2);
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_DART_CLASS);
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_DART_ENUM);
				} else if (kwType != SCE_DART_DEFAULT) {
					sc.ChangeState(kwType);
				} else {
					const int chNext = sc.GetNextNSChar();
					if (chNext == '(') {
						sc.ChangeState(SCE_DART_FUNCTION);
					} else if ((chBeforeIdentifier == '<' && chNext == '>')
						|| IsIdentifierStartEx(chNext)) {
						// type<type>
						// type identifier
						sc.ChangeState(SCE_DART_CLASS);
					}
				}
				if (sc.state != SCE_DART_WORD) {
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
			if (!isoperator(sc.ch)) {
				sc.SetState(SCE_DART_DEFAULT);
			}
			break;

		case SCE_DART_COMMENTLINE:
		case SCE_DART_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_DART_DEFAULT);
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
				sc.Forward(2);
				++commentLevel;
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
			if (curlyBrace == 0 && (sc.state == SCE_DART_STRING_SQ || sc.state == SCE_DART_STRING_DQ) && sc.atLineStart) {
				sc.SetState(SCE_DART_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_DART_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				variableOuter = sc.state;
				sc.SetState(SCE_DART_VARIABLE);
			} else if (sc.Match('$', '{')) {
				++curlyBrace;
				nestedState.push_back(sc.state);
				sc.SetState(SCE_DART_OPERATOR2);
				sc.Forward();
			} else if (curlyBrace && sc.ch == '}') {
				const int outerState = TryPopBack(nestedState);
				--curlyBrace;
				sc.SetState(SCE_DART_OPERATOR2);
				sc.ForwardSetState(outerState);
				continue;
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
				const int outerState = escSeq.outerState;
				if (sc.ch == '\\' && escSeq.resetEscapeState(outerState, sc.chNext)) {
					sc.Forward();
				} else {
					sc.SetState(outerState);
					continue;
				}
			}
			break;
		case SCE_DART_VARIABLE:
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(variableOuter);
				continue;
			}
			break;
		}

		if (sc.state == SCE_DART_DEFAULT) {
			if (sc.Match('/', '/')) {
				const int chNext = sc.GetRelative(2);
				sc.SetState((chNext == '/') ? SCE_DART_COMMENTLINEDOC : SCE_DART_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineComment = DartLineStateMaskLineComment;
				}
			} else if (sc.Match('/', '*')) {
				const int chNext = sc.GetRelative(2);
				sc.SetState((chNext == '*') ? SCE_DART_COMMENTBLOCKDOC : SCE_DART_COMMENTBLOCK);
				sc.Forward();
				commentLevel = 1;
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
				sc.ChangeState(SCE_DART_TRIPLE_STRING_DQSTART);
				sc.Forward(2);
				sc.ForwardSetState(SCE_DART_TRIPLE_STRING_DQ);
				continue;
			} else if (sc.ch == '"') {
				sc.SetState(SCE_DART_STRING_DQ);
			} else if (sc.Match('\'', '\'', '\'')) {
				sc.ChangeState(SCE_DART_TRIPLE_STRING_SQSTART);
				sc.Forward(2);
				sc.ForwardSetState(SCE_DART_TRIPLE_STRING_SQ);
				continue;
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_DART_STRING_SQ);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_DART_NUMBER);
			} else if ((sc.ch == '@' || sc.ch == '$') && IsIdentifierStartEx(sc.chNext)) {
				variableOuter = SCE_DART_DEFAULT;
				sc.SetState((sc.ch == '@') ? SCE_DART_METADATA : SCE_DART_VARIABLE);
			} else if (sc.ch == '#') {
				if (IsIdentifierStartEx(sc.chNext)) {
					sc.SetState(SCE_DART_SYMBOL_IDENTIFIER);
				} else if (isoperator(sc.ch)) {
					sc.SetState(SCE_DART_SYMBOL_OPERATOR);
				}
			} else if (IsIdentifierStartEx(sc.ch)) {
				chBeforeIdentifier = sc.chPrev;
				sc.SetState(SCE_DART_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(curlyBrace ? SCE_DART_OPERATOR2 : SCE_DART_OPERATOR);
				if (curlyBrace) {
					if (sc.ch == '{') {
						++curlyBrace;
						nestedState.push_back(SCE_DART_DEFAULT);
					} else if (sc.ch == '}') {
						--curlyBrace;
						const int outerState = TryPopBack(nestedState);
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
			int lineState = (curlyBrace << 8) | (commentLevel << 2) | lineStateLineComment | lineStateImport;
			if (curlyBrace) {
				lineState |= PackNestedState(nestedState);
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineComment = 0;
			lineStateImport = 0;
			visibleChars = 0;
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

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_DART_COMMENTBLOCK || style == SCE_DART_COMMENTBLOCKDOC;
}

void FoldDartDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const int foldComment = styler.GetPropertyInt("fold.comment", 1);

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (IsStreamCommentStyle(style)) {
			if (foldComment) {
				const int level = (ch == '/' && chNext == '*') ? 1 : ((ch == '*' && chNext == '/') ? -1 : 0);
				if (level != 0) {
					levelNext += level;
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
					styleNext = styler.StyleAt(i + 1);
				}
			}
		} else if (style == SCE_DART_TRIPLE_RAWSTRING_SQ || style == SCE_DART_TRIPLE_RAWSTRING_DQ) {
			if (style != stylePrev) {
				levelNext++;
			} else if (style != styleNext) {
				levelNext--;
			}
		} else if (style == SCE_DART_TRIPLE_STRING_SQSTART || style == SCE_DART_TRIPLE_STRING_DQSTART) {
			if (style != stylePrev) {
				levelNext++;
			}
		} else if (style == SCE_DART_TRIPLE_STRING_SQEND || style == SCE_DART_TRIPLE_STRING_DQEND) {
			if (style != styleNext) {
				levelNext--;
			}
		} else if (style == SCE_DART_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldComment & foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.packageImport) {
				levelNext += foldNext.packageImport - foldPrev.packageImport;
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
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
		}
	}
}

}

LexerModule lmDart(SCLEX_DART, ColouriseDartDoc, "dart", FoldDartDoc);
