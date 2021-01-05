// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for NSIS.

#include <cstdlib>
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

enum {
	NsisLineTypeComment = 1,		// line comment
	NsisLineTypeInclude = 1 << 1,	// !include
	NsisLineTypeDefine = 2 << 1,	// !define

	NsisLineStateLineContinuation = 1 << 4,
	NsisLineTypeDefaultMask = (1 << 3) - 2,
	NsisLineTypeFullMask = (1 << 3) - 1,
};

constexpr bool IsEscapeChar(int ch) noexcept {
	return AnyOf(ch, '\'', '"', '`', 'n', 'r', 't');
}

void ColouriseNSISDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int visibleChars = 0;
	int lineContinuation = 0;
	int lineStateLineType = 0;
	int variableOuter = SCE_NSIS_DEFAULT;	// variable inside string

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		lineContinuation = lineState & NsisLineStateLineContinuation;
		if (lineContinuation) {
			++visibleChars;
			lineStateLineType = lineState & NsisLineTypeFullMask;
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_NSIS_OPERATOR:
			sc.SetState(SCE_NSIS_DEFAULT);
			break;

		case SCE_NSIS_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				if (sc.ch == '%') {
					sc.Forward();
				}
				sc.SetState(SCE_NSIS_DEFAULT);
			}
			break;

		case SCE_NSIS_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				char s[128];
				sc.GetCurrentLowered(s, sizeof(s));
				if (s[0] == '!') {
					sc.ChangeState(SCE_NSIS_PREPROCESSOR);
					const char *p = s + 1;
					if (strcmp(p, "include") == 0) {
						lineStateLineType = NsisLineTypeInclude;
					} else if (strcmp(p, "define") == 0) {
						lineStateLineType = NsisLineTypeDefine;
					}
				} else if (visibleChars == sc.LengthCurrent()) {
					if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_NSIS_WORD);
					} else if (sc.ch == ':' && sc.chNext != ':') {
						sc.ChangeState(SCE_NSIS_LABEL);
					} else {
						sc.ChangeState(SCE_NSIS_INSTRUCTION);
					}
				}
				sc.SetState(SCE_NSIS_DEFAULT);
			}
			break;

		case SCE_NSIS_STRINGSQ:
		case SCE_NSIS_STRINGDQ:
		case SCE_NSIS_STRINGBT:
			if (sc.ch == '$') {
				if (sc.chNext == '$' || (sc.chNext == '\\' && IsEscapeChar(sc.GetRelative(2)))) {
					const int state = sc.state;
					sc.SetState(SCE_NSIS_ESCAPECHAR);
					sc.Forward((sc.chNext == '\\') ? 2 : 1);
					sc.ForwardSetState(state);
					continue;
				}
				if (sc.chNext == '{' || sc.chNext == '(') {
					variableOuter = sc.state;
					sc.SetState((sc.chNext == '{') ? SCE_NSIS_VARIABLE_BRACE : SCE_NSIS_VARIABLE_PAREN);
				} else if (IsIdentifierChar(sc.chNext)) {
					variableOuter = sc.state;
					sc.SetState(SCE_NSIS_VARIABLE);
				}
			} else if (sc.atLineStart) {
				if (!lineContinuation) {
					sc.SetState(SCE_NSIS_DEFAULT);
				}
			} else if ((sc.state == SCE_NSIS_STRINGSQ && sc.ch == '\'')
				|| (sc.state == SCE_NSIS_STRINGDQ && sc.ch == '"')
				|| (sc.state == SCE_NSIS_STRINGBT && sc.ch == '`')) {
				sc.ForwardSetState(SCE_NSIS_DEFAULT);
			}
			break;

		case SCE_NSIS_VARIABLE:
			if (!IsIdentifierChar(sc.ch)) {
				sc.SetState(variableOuter);
				continue;
			}
			break;

		case SCE_NSIS_VARIABLE_BRACE:
		case SCE_NSIS_VARIABLE_PAREN:
			if ((sc.state == SCE_NSIS_VARIABLE_BRACE && sc.ch == '}')
				|| (sc.state == SCE_NSIS_VARIABLE_PAREN && sc.ch == ')')) {
				sc.ForwardSetState(variableOuter);
				continue;
			}
			break;

		case SCE_NSIS_COMMENTLINE:
			if (sc.atLineStart) {
				if (!lineContinuation) {
					sc.SetState(SCE_NSIS_DEFAULT);
				}
			}
			break;

		case SCE_NSIS_COMMENT:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_NSIS_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_NSIS_DEFAULT) {
			if (sc.ch == ';' || sc.ch == '#') {
				sc.SetState(SCE_NSIS_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineType = NsisLineTypeComment;
				}
			} else if (sc.Match('/', '*')) {
				sc.SetState(SCE_NSIS_COMMENT);
				sc.Forward();
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_NSIS_STRINGSQ);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_NSIS_STRINGDQ);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_NSIS_STRINGBT);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_NSIS_NUMBER);
			} else if (sc.ch == '$' && IsIdentifierChar(sc.chNext)) {
				variableOuter = SCE_NSIS_DEFAULT;
				sc.SetState(SCE_NSIS_VARIABLE);
			} else if (sc.ch == '$' && (sc.chNext == '{' || sc.chNext == '(')) {
				variableOuter = SCE_NSIS_DEFAULT;
				sc.SetState((sc.chNext == '{') ? SCE_NSIS_VARIABLE_BRACE : SCE_NSIS_VARIABLE_PAREN);
			} else if ((visibleChars == 0 && sc.ch == '!') || IsIdentifierStart(sc.ch)) {
				sc.SetState(SCE_NSIS_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_NSIS_OPERATOR);
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			lineContinuation = sc.LineEndsWith('\\') ? NsisLineStateLineContinuation : 0;
			const int lineState = lineContinuation | lineStateLineType;
			styler.SetLineState(sc.currentLine, lineState);
			if (!lineContinuation) {
				visibleChars = 0;
				lineStateLineType = 0;
			}
		}
		sc.Forward();
	}

	sc.Complete();
}

void FoldNSISDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const int foldComment = styler.GetPropertyInt("fold.comment", 1);
	const int lineTypeMask = NsisLineTypeDefaultMask | foldComment;

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineTypePrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineTypePrev = styler.GetLineState(lineCurrent - 1) & lineTypeMask;
	}

	int levelNext = levelCurrent;
	int lineTypeCurrent = styler.GetLineState(lineCurrent) & lineTypeMask;
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	constexpr int MaxFoldWordLength = 15 + 1; // SectionGroupEnd
	char buf[MaxFoldWordLength + 1];
	int wordLen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (style == SCE_NSIS_WORD || style == SCE_NSIS_PREPROCESSOR) {
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = MakeLowerCase(styler[i]);
			}
			if (styleNext != style) {
				buf[wordLen] = '\0';
				wordLen = 0;
				if (style == SCE_NSIS_WORD) {
					if (EqualsAny(buf, "section", "function", "sectiongroup", "pageex")) {
						levelNext++;
					} else if (EqualsAny(buf, "sectionend", "functionend", "sectiongroupend", "pageexend")) {
						levelNext--;
					}
				} else {
					if (StrStartsWith(buf, "!if") || strcmp(buf, "!macro") == 0) {
						levelNext++;
					} else if (StrStartsWith(buf, "!end") || strcmp(buf, "!macroend") == 0) {
						levelNext--;
					}
				}
			}
		} else if (style == SCE_NSIS_COMMENT && foldComment) {
			if (stylePrev != style) {
				levelNext++;
			} else if (styleNext != style) {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineTypeNext = styler.GetLineState(lineCurrent + 1) & lineTypeMask;
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

LexerModule lmNsis(SCLEX_NSIS, ColouriseNSISDoc, "nsis", FoldNSISDoc);
