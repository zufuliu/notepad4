// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Rebol, Red, Red/System.

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
#include "StringUtils.h"
#include "LexerModule.h"

using namespace Scintilla;

namespace {

enum {
	RebolLineTypeComment = 1,		// line comment
	RebolLineTypeInclude = 1 << 1,	// #include
	RebolLineTypeDefine = 2 << 1,	// #define

	RebolLineStateHtmlTag = 1 << 4,
	RebolLineTypeMask = (1 << 3) - 1,
};

struct EscapeSequence {
	int digitsLeft = 0;
	int outerState = SCE_REBOL_DEFAULT;
	bool quoted = false;

	bool resetEscapeState(int state, int chNext) noexcept {
		if (!IsGraphic(chNext)) {
			return false;
		}
		outerState = state;
		digitsLeft = (chNext == '(') ? 8 : 1;
		quoted = chNext == '(';
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || (quoted && ch == ')');
	}
};

constexpr bool IsRebolDelimiter(int ch) noexcept {
	return AnyOf(ch, '[', ']', '{', '}',
				'\"', '(', ')', '/',
				'\\', '@', '#', '$',
				'%', '^', ',', ':',
				';', '<', '>', '|');
}

constexpr bool IsRebolIdentifierChar(int ch) noexcept {
	return IsGraphic(ch) && !IsRebolDelimiter(ch);
}

constexpr bool IsRebolNumberStart(int ch, int chNext) noexcept {
	return IsADigit(ch) || ((ch == '+' || ch == '-') && IsADigit(chNext));
}

constexpr bool IsRebolDateTimeStart(int chPrev, int ch, int chNext) noexcept {
	// http://www.rebol.com/r3/docs/datatypes/date.html
	return (ch == ':' || ch == '/' || ch == '=' || (ch == '-' && !(chPrev == 'e' || chPrev == 'E')))
	&& (ch == chNext || IsAlphaNumeric(chNext));
}

constexpr bool IsRebolOperator(int ch, int chNext) noexcept {
	return (ch == '=' && (chNext == '=' || chNext == '?'))
		|| ((ch == '<' || ch == '>') && chNext == '=')
		|| (ch == '*' && chNext == '*');
}

bool HandleRebolOperator(StyleContext &sc) {
	// https://github.com/red/red/blob/master/environment/operators.red
	if (isspacechar(sc.chNext)) {
		if (AnyOf(sc.ch, '+', '-', '*', '/', '%', '=')) {
			sc.SetState(SCE_REBOL_OPERATOR);
		}
	} else if (isspacechar(sc.GetRelative(2))) {
		if (IsRebolOperator(sc.ch, sc.chNext)) {
			sc.SetState(SCE_REBOL_OPERATOR);
			sc.Forward();
		}
	}
	return sc.state == SCE_REBOL_OPERATOR;
}

void ColouriseRebolDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int nestedLevel = 0;
	int lineStateLineType = 0;
	int insideHtmlTag = 0;
	int dotCount = 0;

	EscapeSequence escSeq;
	int visibleChars = 0;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		1: RebolLineTypeComment
		3: lineStateLineType
		1: insideHtmlTag
		: nestedLevel
		*/
		insideHtmlTag = lineState & RebolLineStateHtmlTag;
		nestedLevel = lineState >> 8;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_REBOL_OPERATOR:
			sc.SetState(SCE_REBOL_DEFAULT);
			break;

		case SCE_REBOL_NUMBER:
		case SCE_REBOL_PAIR:
		case SCE_REBOL_TUPLE:
		case SCE_REBOL_MONEY:
		case SCE_REBOL_DATE:
		case SCE_REBOL_TIME:
			if (sc.ch == '@' && sc.state == SCE_REBOL_NUMBER) {
				sc.ChangeState(SCE_REBOL_EMAIL);
			} else if (sc.ch == ',' || sc.ch == '.') {
				if (sc.state == SCE_REBOL_NUMBER) {
					++dotCount;
					if (dotCount > 1) {
						sc.ChangeState(SCE_REBOL_TUPLE);
					}
				}
			} else if (sc.ch == 'x') {
				if (sc.state == SCE_REBOL_NUMBER) {
					sc.ChangeState(SCE_REBOL_PAIR);
				}
				const int state = sc.state;
				sc.SetState(SCE_REBOL_OPERATOR);
				sc.ForwardSetState(state);
				continue;
			} else if (sc.state == SCE_REBOL_NUMBER && IsRebolDateTimeStart(sc.chPrev, sc.ch, sc.chNext)) {
				sc.ChangeState((sc.ch == ':') ? SCE_REBOL_TIME : SCE_REBOL_DATE);
			} else if ((sc.state != SCE_REBOL_NUMBER && (sc.ch == '+' || sc.ch == '-'))
				|| ((sc.state == SCE_REBOL_DATE || sc.state == SCE_REBOL_TIME) && (sc.ch == ':' || sc.ch == '/' || sc.ch == '='))) {
				sc.Forward();
			}
			else if (!(sc.ch == '\'' || IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext))) {
				if (sc.state == SCE_REBOL_NUMBER) {
					if (sc.LengthCurrent() <= 2 && sc.Match('#', '{')) {
						// base 2, base 16, base 64
						sc.ChangeState(SCE_REBOL_BINARY);
						sc.Forward();
					} else {
						if (sc.ch == '%') {
							sc.Forward(); // percent!
						}
					}
				}
				if (sc.state != SCE_REBOL_BINARY) {
					if (sc.ch == '/') {
						sc.SetState(SCE_REBOL_OPERATOR);
						sc.Forward();
					}
					sc.SetState(SCE_REBOL_DEFAULT);
				}
			}
			break;

		case SCE_REBOL_IDENTIFIER:
			if (sc.Match(':', '/', '/')) {
				sc.ChangeState(SCE_REBOL_URL);
			} else if (sc.ch == '@') {
				sc.ChangeState(SCE_REBOL_EMAIL);
			} else if (sc.ch == '$' && (IsADigit(sc.chNext) || (sc.chNext == '+' || sc.chNext == '-'))) {
				sc.ChangeState(SCE_REBOL_MONEY);
			} else if (!IsRebolIdentifierChar(sc.ch) || sc.ch == '!') {
				if (sc.ch == '!') {
					sc.ChangeState(SCE_REBOL_TYPE);
					sc.Forward();
				} else if (sc.ch == ':') {
					sc.ChangeState(SCE_REBOL_PROPERTY);
				} else {
					char s[128];
					sc.GetCurrentLowered(s, sizeof(s));
					const int chNext = sc.GetLineNextChar();
					if (keywordLists[0]->InList(s)) {
						if (chNext == '{' && StrEqual(s, "comment")) {
							sc.ChangeState(SCE_REBOL_COMMENTBLOCK);
							if (sc.ch == '{') {
								++nestedLevel;
							}
						} else {
							sc.ChangeState(SCE_REBOL_WORD);
						}
					} else if (chNext == '(') {
						sc.ChangeState(SCE_REBOL_MACRO);
					}
				}
				if (sc.state != SCE_REBOL_COMMENTBLOCK) {
					if (sc.ch == '/') {
						sc.SetState(SCE_REBOL_OPERATOR);
						sc.Forward();
					}
					sc.SetState(SCE_REBOL_DEFAULT);
				}
			}
			break;

		case SCE_REBOL_SYMBOL:
		case SCE_REBOL_ISSUE:
			if (!(sc.ch == '/' || IsRebolIdentifierChar(sc.ch))) {
				if (sc.state == SCE_REBOL_ISSUE) {
					char s[128];
					sc.GetCurrentLowered(s, sizeof(s));
					const char *p = s + 1;
					if (keywordLists[1]->InList(p)) {
						sc.ChangeState(SCE_REBOL_DIRECTIVE);
						if (StrEqual(p, "include")) {
							lineStateLineType = RebolLineTypeInclude;
						} else if (StrEqual(p, "define")) {
							lineStateLineType = RebolLineTypeDefine;
						}
					}
				}
				sc.SetState(SCE_REBOL_DEFAULT);
			}
			break;

		case SCE_REBOL_FILE:
		case SCE_REBOL_EMAIL:
		case SCE_REBOL_URL:
			if (sc.atLineEnd || isspacechar(sc.ch) || sc.ch == ']') {
				sc.SetState(SCE_REBOL_DEFAULT);
			}
			break;

		case SCE_REBOL_CHARACTER:
		case SCE_REBOL_QUOTEDSTRING:
		case SCE_REBOL_QUOTEDFILE:
		case SCE_REBOL_TAG_VALUE:
			if (sc.ch == '^') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_REBOL_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_REBOL_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_REBOL_DEFAULT);
			}
			break;

		case SCE_REBOL_BRACEDSTRING:
		case SCE_REBOL_COMMENTBLOCK:
			if (sc.ch == '^') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_REBOL_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '{') {
				++nestedLevel;
			} else if (sc.ch == '}') {
				--nestedLevel;
				if (nestedLevel == 0) {
					sc.ForwardSetState(SCE_REBOL_DEFAULT);
				}
			}
			break;

		case SCE_REBOL_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				if (escSeq.quoted && sc.ch == ')') {
					sc.Forward();
				}
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_REBOL_COMMENTLINE:
			// TODO: comment-rule
			if (sc.atLineStart) {
				sc.SetState(SCE_REBOL_DEFAULT);
			}
			break;

		case SCE_REBOL_BINARY:
			if (sc.ch == ';') {
				sc.SetState(SCE_REBOL_BINARYCOMMENT);
			} else if (sc.ch == '}') {
				sc.ForwardSetState(SCE_REBOL_DEFAULT);
			}
			break;
		case SCE_REBOL_BINARYCOMMENT:
			if (sc.atLineStart) {
				sc.SetState(SCE_REBOL_BINARY);
				continue;
			}
			break;

		case SCE_REBOL_TAG:
		case SCE_REBOL_TAG_ATTR:
			if (!(IsAlphaNumeric(sc.ch) || sc.ch == '-')) {
				sc.SetState(SCE_REBOL_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_REBOL_DEFAULT) {
			if (insideHtmlTag) {
				if (sc.ch == '>' || sc.Match('/', '>')) {
					sc.SetState(SCE_REBOL_TAG);
					sc.Forward((sc.ch == '>') ? 1 : 2);
					sc.SetState(SCE_REBOL_DEFAULT);
					insideHtmlTag = 0;
					continue;
				}
				if (sc.ch == '=') {
					sc.SetState(SCE_REBOL_OPERATOR);
				} else if (sc.ch == '\"') {
					sc.SetState(SCE_REBOL_TAG_VALUE);
				} else if (IsAlpha(sc.ch)) {
					sc.SetState(SCE_REBOL_TAG_ATTR);
				}
			}
			// literal-value
			else if (sc.ch == ';') {
				sc.SetState(SCE_REBOL_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineType = RebolLineTypeComment;
				}
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_REBOL_QUOTEDSTRING);
			} else if (sc.Match('#', '\"')) {
				sc.SetState(SCE_REBOL_CHARACTER);
				sc.Forward();
			} else if (sc.ch == '%' && !isspacechar(sc.chNext)) {
				if (sc.chNext == '\"') {
					sc.SetState(SCE_REBOL_QUOTEDFILE);
					sc.Forward();
				} else {
					sc.SetState(SCE_REBOL_FILE);
				}
			} else if (sc.Match('#', '{')) {
				sc.SetState(SCE_REBOL_BINARY);
				sc.Forward();
			} else if (sc.ch == '#' && IsRebolIdentifierChar(sc.chNext)) {
				sc.SetState(SCE_REBOL_ISSUE);
				sc.Forward();
			} else if (sc.ch == '{') {
				sc.SetState(SCE_REBOL_BRACEDSTRING);
				nestedLevel = 1;
			} else if (IsRebolNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_REBOL_NUMBER);
				dotCount = 0;
			} else if (sc.ch == '$' && (IsADigit(sc.chNext) || sc.chNext == '+' || sc.chNext == '-')) {
				sc.SetState(SCE_REBOL_MONEY);
				sc.Forward();
			} else if (sc.ch == '<' && (sc.chNext == '/' || IsAlpha(sc.chNext))) {
				sc.SetState(SCE_REBOL_TAG);
				insideHtmlTag = RebolLineStateHtmlTag;
				if (sc.chNext == '/') {
					sc.Forward();
				}
			} else if ((sc.ch == '/' || sc.ch == '\'' || sc.ch == ':') && IsRebolIdentifierChar(sc.chNext)) {
				sc.SetState(SCE_REBOL_SYMBOL);
			} else if (isspacechar(sc.chPrev) && HandleRebolOperator(sc)) {
				// nop
			} else if (IsRebolDelimiter(sc.ch)) {
				sc.SetState(SCE_REBOL_OPERATOR);
			} else if (IsGraphic(sc.ch)) {
				sc.SetState(SCE_REBOL_IDENTIFIER);
			}
		}

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			++visibleChars;
		}
		if (sc.atLineEnd) {
			const int lineState = (nestedLevel << 8) | insideHtmlTag | lineStateLineType;
			styler.SetLineState(sc.currentLine, lineState);
			visibleChars = 0;
			lineStateLineType = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

void FoldRebolDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineTypePrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineTypePrev = styler.GetLineState(lineCurrent - 1) & RebolLineTypeMask;
	}

	int levelNext = levelCurrent;
	int lineTypeCurrent = styler.GetLineState(lineCurrent) & RebolLineTypeMask;
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int style = styler.StyleAt(i);
		const char ch = styler[i];

		switch (style) {
		case SCE_REBOL_OPERATOR:
			if (ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == ']' || ch == ')') {
				levelNext--;
			}
			break;

		case SCE_REBOL_BRACEDSTRING:
		case SCE_REBOL_BINARY:
		case SCE_REBOL_COMMENTBLOCK:
			if (ch == '{') {
				levelNext++;
			} else if (ch == '}') {
				levelNext--;
			}
			break;
		}

		if (i == lineEndPos) {
			const int lineTypeNext = styler.GetLineState(lineCurrent + 1) & RebolLineTypeMask;
			if (lineTypeCurrent) {
				levelNext += (lineTypeNext == lineTypeCurrent) - (lineTypePrev == lineTypeCurrent);
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
			lineTypePrev = lineTypeCurrent;
			lineTypeCurrent = lineTypeNext;
		}
	}
}

}

LexerModule lmREBOL(SCLEX_REBOL, ColouriseRebolDoc, "rebol", FoldRebolDoc);
