// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Boost Jam.

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

enum {
	JamLineStateMaskLineComment = 1, // line comment
	JamLineStateMaskImport = 1 << 1, // import, using, include
};

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_JAM_TASKMARKER;
}

constexpr bool IsJamEscapeChar(int ch) noexcept {
	return AnyOf(ch, '\\', '"', '\'', 'b', 'f', 'n', 'r', 't');
}

constexpr bool IsJamOperator(int ch) noexcept {
	return AnyOf(ch, '{', '}', '[', ']', '(', ')', '<', '>',
					'!', '&', ':', ';', '=', '|',
					'.', '+', '*', '?', '@');
}

constexpr bool IsJamIdentifierChar(int ch) noexcept {
	return IsIdentifierChar(ch) || ch == '-' || ch == '+';
}

void ColouriseJamDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;
	int kwType = SCE_JAM_DEFAULT;
	int visibleChars = 0;

	std::vector<int> nestedState; // for $()
	StyleContext sc(startPos, lengthDoc, initStyle, styler);

	while (sc.More()) {
		switch (sc.state) {
		case SCE_JAM_OPERATOR:
			sc.SetState(SCE_JAM_DEFAULT);
			break;

		case SCE_JAM_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_JAM_DEFAULT);
			}
			break;

		case SCE_JAM_IDENTIFIER:
			if (!IsJamIdentifierChar(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_JAM_WORD);
					if (StrEqualsAny(s, "import", "using", "include")) {
						if (visibleChars == sc.LengthCurrent()) {
							lineStateLineType = JamLineStateMaskImport;
						}
					} else if (StrEqual(s, "rule")) {
						kwType = SCE_JAM_RULE;
					} else if (StrEqual(s, "class")) {
						kwType = SCE_JAM_CLASS;
					} else if (StrEqual(s, "module")) {
						kwType = SCE_JAM_MODULE;
					}
					if (kwType != SCE_JAM_DEFAULT) {
						const int chNext = sc.GetLineNextChar();
						if (!IsIdentifierStart(chNext)) {
							kwType = SCE_JAM_DEFAULT;
						}
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_JAM_MODULE);
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_JAM_CLASS);
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_JAM_RULE);
				} else if (keywordLists[4]->InList(s)) {
					sc.ChangeState(SCE_JAM_CONSTANT);
				} else if (kwType != SCE_JAM_DEFAULT) {
					sc.ChangeState(kwType);
				}
				if (sc.state != SCE_JAM_WORD) {
					kwType = SCE_JAM_DEFAULT;
				}
				sc.SetState(SCE_JAM_DEFAULT);
			}
			break;

		case SCE_JAM_FEATURE:
			if (sc.ch == '>') {
				sc.ForwardSetState(SCE_JAM_DEFAULT);
			} else if (!IsJamIdentifierChar(sc.ch)) {
				if (IsJamOperator(sc.ch)) {
					sc.SetState(SCE_JAM_OPERATOR);
					sc.ForwardSetState(SCE_JAM_FEATURE);
					continue;
				}
				sc.ChangeState(SCE_JAM_DEFAULT);
			}
			break;

		case SCE_JAM_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_JAM_DEFAULT);
			}
			break;

		case SCE_JAM_COMMENTBLOCK:
			if (sc.Match('|', '#')) {
				sc.Forward();
				sc.ForwardSetState(SCE_JAM_DEFAULT);
			}
			break;

		case SCE_JAM_STRING:
			if (sc.ch == '\\' && IsJamEscapeChar(sc.chNext)) {
				sc.SetState(SCE_JAM_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_JAM_STRING);
				continue;
			}
			if (sc.Match('$', '(')) {
				nestedState.push_back(SCE_JAM_STRING);
				sc.SetState(SCE_JAM_OPERATOR);
				sc.Forward();
			} else if (sc.ch == '"') {
				sc.ForwardSetState(SCE_JAM_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_JAM_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_JAM_DEFAULT) {
			if (sc.ch == '#') {
				if (sc.chNext == '|') {
					sc.SetState(SCE_JAM_COMMENTBLOCK);
					sc.Forward();
				} else {
					sc.SetState(SCE_JAM_COMMENTLINE);
					if (visibleChars == 0) {
						lineStateLineType = JamLineStateMaskLineComment;
					}
				}
			} else if (sc.ch == '\\' && IsAGraphic(sc.chNext)) {
				sc.SetState(SCE_JAM_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_JAM_DEFAULT);
				continue;
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_JAM_STRING);
			} else if (IsADigit(sc.ch)) {
				sc.SetState(SCE_JAM_NUMBER);
			} else if (IsIdentifierStart(sc.ch)) {
				sc.SetState(SCE_JAM_IDENTIFIER);
			} else if (sc.Match('$', '(')) {
				nestedState.push_back(SCE_JAM_DEFAULT);
				sc.SetState(SCE_JAM_OPERATOR);
				sc.Forward();
			} else if (sc.ch == '<' && (sc.chNext == '.' || IsIdentifierStart(sc.chNext))) {
				sc.SetState(SCE_JAM_FEATURE);
			} else if (IsJamOperator(sc.ch)) {
				sc.SetState(SCE_JAM_OPERATOR);
				if (!nestedState.empty()) {
					if (sc.ch == '(') {
						nestedState.push_back(SCE_JAM_DEFAULT);
					} else if (sc.ch == ')') {
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
			styler.SetLineState(sc.currentLine, lineStateLineType);
			lineStateLineType = 0;
			visibleChars = 0;
			kwType = SCE_JAM_DEFAULT;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int moduleImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & JamLineStateMaskLineComment),
		moduleImport((lineState >> 1) & 1) {
	}
};

void FoldJamDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_JAM_OPERATOR, SCE_JAM_TASKMARKER);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_JAM_COMMENTBLOCK:
			if (style != stylePrev) {
				levelNext++;
			} else if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_JAM_OPERATOR: {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		} break;
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.moduleImport) {
				levelNext += foldNext.moduleImport - foldPrev.moduleImport;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_JAM_OPERATOR, SCE_JAM_TASKMARKER);
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

LexerModule lmJam(SCLEX_JAM, ColouriseJamDoc, "jam", FoldJamDoc);
