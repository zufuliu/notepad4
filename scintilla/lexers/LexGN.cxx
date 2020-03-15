// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for GN build.

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
	int digitsLeft = 0;

	bool resetEscapeState(const StyleContext &sc) noexcept {
		if (sc.ch == '\\') {
			if (sc.chNext == '\\' || sc.chNext == '\"' || sc.chNext == '$') {
				digitsLeft = 1;
				return true;
			}
		} else if (sc.ch == '$' && sc.chNext == '0' && sc.GetRelative(2) == 'x') {
			digitsLeft = 3;
			return true;
		}
		return false;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

enum {
	GNLineStateMaskLineComment = 1, // line comment
};

void ColouriseGNDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int visibleChars = 0;
	int lineStateLineComment = 0;
	int braceCount = 0;
	int expansionLevel = -1;	// for BracketExpansion
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		braceCount = styler.GetLineState(sc.currentLine - 1) >> 1;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_GN_OPERATOR:
			sc.SetState(SCE_GN_DEFAULT);
			break;

		case SCE_GN_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_GN_DEFAULT);
			}
			break;

		case SCE_GN_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_GN_KEYWORD);
				} else if (sc.GetNextNSChar() == '(') {
					const bool builtin = keywordLists[1]->InListPrefixed(s, '(');
					sc.ChangeState(builtin ? SCE_GN_BUILTIN_FUNCTION : SCE_GN_FUNCTION);
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_GN_BUILTIN_VARIABLE);
				}
				sc.SetState(SCE_GN_DEFAULT);
			}
			break;

		case SCE_GN_STRING:
			if (escSeq.resetEscapeState(sc)) {
				sc.SetState(SCE_GN_ESCAPECHAR);
				sc.Forward((escSeq.digitsLeft == 1)? 1 : 2);
			} else if (sc.ch == ':' || sc.ch == '*') {
				sc.SetState(SCE_GN_OPERATOR);
				sc.ForwardSetState(SCE_GN_STRING);
				continue;
			} else if (sc.ch == '$') {
				if (sc.chNext == '{') {
					sc.SetState(SCE_GN_OPERATOR);
					sc.Forward();
					++braceCount;
					expansionLevel = braceCount;
				} else if (IsIdentifierStart(sc.chNext)) {
					sc.SetState(SCE_GN_VARIABLE);
				}
			} else if (sc.Match('{', '{')) {
				sc.SetState(SCE_GN_PLACEHOLDER);
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_GN_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_GN_DEFAULT);
			}
			break;

		case SCE_GN_VARIABLE:
			if (!IsIdentifierChar(sc.ch)) {
				sc.SetState(SCE_GN_STRING);
				continue;
			}
			break;

		case SCE_GN_PLACEHOLDER:
			if (sc.Match('}', '}')) {
				sc.Forward(2);
				sc.SetState(SCE_GN_STRING);
				continue;
			} else if (!(IsIdentifierChar(sc.ch) || sc.ch == '*')) {
				sc.ChangeState(SCE_GN_STRING);
				continue;
			}
			break;

		case SCE_GN_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				if (escSeq.resetEscapeState(sc)) {
					sc.Forward((escSeq.digitsLeft == 1)? 1 : 2);
				} else {
					sc.SetState(SCE_GN_STRING);
					continue;
				}
			}
			break;

		case SCE_GN_COMMENT:
			if (sc.atLineStart) {
				sc.SetState(SCE_GN_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_GN_DEFAULT) {
			if (sc.ch == '#') {
				sc.SetState(SCE_GN_COMMENT);
				if (visibleChars == 0) {
					lineStateLineComment = GNLineStateMaskLineComment;
				}
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_GN_STRING);
			} else if (IsADigit(sc.ch)) {
				sc.SetState(SCE_GN_NUMBER);
			} else if (IsIdentifierStart(sc.ch)) {
				sc.SetState(SCE_GN_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_GN_OPERATOR);
				if (sc.ch == '{') {
					++braceCount;
				} else if (sc.ch == '}') {
					--braceCount;
					if (expansionLevel && expansionLevel == braceCount + 1) {
						expansionLevel = 0;
						sc.ForwardSetState(SCE_GN_STRING);
						continue;
					}
				}
			}
		}

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			++visibleChars;
		}
		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineStateLineComment | (braceCount << 1));
			visibleChars = 0;
			lineStateLineComment = 0;
			expansionLevel = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

void FoldGNDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList /*keywordLists*/, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = styler.GetLineState(lineCurrent - 1) & GNLineStateMaskLineComment;
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = styler.GetLineState(lineCurrent) & GNLineStateMaskLineComment;
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (style == SCE_GN_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineCommentNext = styler.GetLineState(lineCurrent + 1) & GNLineStateMaskLineComment;
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

LexerModule lmGN(SCLEX_GN, ColouriseGNDoc, "gn", FoldGNDoc);
