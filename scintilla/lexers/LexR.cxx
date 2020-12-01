// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for R.

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

namespace {

struct EscapeSequence {
	int outerState = SCE_R_DEFAULT;
	int digitsLeft = 0;
	int numBase = 16;

	// highlight any character as escape sequence, no highlight for hex in '\u{hex}' or '\U{hex}'.
	void resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = 1;
		numBase = 16;
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
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

void ColouriseRDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int chBeforeIdentifier = 0;
	int visibleChars = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);

	while (sc.More()) {
		switch (sc.state) {
		case SCE_R_OPERATOR:
			sc.SetState(SCE_R_DEFAULT);
			break;

		case SCE_R_INFIX:
			if (sc.ch == '%') {
				sc.ForwardSetState(SCE_R_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (sc.ch != '.' && chBeforeIdentifier != '.') {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_R_KEYWORD);
					} else if (keywordLists[1]->InList(s)) {
						sc.ChangeState(SCE_R_WORD2);
					} else if (keywordLists[2]->InList(s)) {
						sc.ChangeState(SCE_R_WORD3);
					}
				}
				if (sc.state == SCE_R_IDENTIFIER && sc.GetLineNextChar() == '(') {
					sc.ChangeState(SCE_R_FUNCTION);
				}
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_COMMENT:
		case SCE_R_DIRECTIVE:
			if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_STRING:
		case SCE_R_STRING2:
			if (sc.ch == '\\') {
				escSeq.resetEscapeState(sc.state, sc.chNext);
				sc.SetState(SCE_R_ESCAPECHAR);
				sc.Forward();
			} else if ((sc.state == SCE_R_STRING && sc.ch == '\'')
				|| (sc.state == SCE_R_STRING2 && sc.ch == '\"')) {
				sc.ForwardSetState(SCE_R_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_R_BACKTICKS:
			if (sc.ch == '`') {
				sc.ForwardSetState(SCE_R_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_R_DEFAULT) {
			if (sc.ch == '#') {
				if (visibleChars == 0 && sc.Match("#line")) {
					sc.SetState(SCE_R_DIRECTIVE);
				} else {
					sc.SetState(SCE_R_COMMENT);
					if (visibleChars == 0) {
						lineStateLineComment = SimpleLineStateMaskLineComment;
					}
				}
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_R_STRING2);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_R_STRING);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_R_BACKTICKS);
			} else if (IsADigit(sc.ch) || (sc.chPrev != '.' && sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_R_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				chBeforeIdentifier = sc.chPrev;
				sc.SetState(SCE_R_IDENTIFIER);
			} else if (sc.ch == '%') {
				sc.SetState(SCE_R_INFIX);
			} else if (isoperator(sc.ch) || sc.ch == '$' || sc.ch == '@') {
				sc.SetState(SCE_R_OPERATOR);
			}
		}

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			visibleChars = 1;
		}
		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineStateLineComment);
			lineStateLineComment = 0;
			visibleChars = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & SimpleLineStateMaskLineComment;
}

static_assert(SCE_R_OPERATOR == SCE_SIMPLE_OPERATOR);
}

namespace Scintilla {

void FoldSimpleDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList /*keywordLists*/, Accessor &styler) {
	const int foldComment = styler.GetPropertyInt("fold.comment", 1);

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = GetLineCommentState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = GetLineCommentState(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int style = styler.StyleAt(i);

		if (style == SCE_SIMPLE_OPERATOR) {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineCommentNext = GetLineCommentState(styler.GetLineState(lineCurrent + 1));
			if (foldComment & lineCommentCurrent) {
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
			lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;
			levelCurrent = levelNext;
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
		}
	}
}

}

LexerModule lmR(SCLEX_R, ColouriseRDoc, "r", FoldSimpleDoc);
