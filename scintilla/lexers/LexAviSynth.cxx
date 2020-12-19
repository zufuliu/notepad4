// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for AviSynth, AviSynth+, GScript.

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
	AviSynthLineStateMaskLineComment = (1 << 0),
	AviSynthLineStateMaskLineContinuation = (1 << 1),
	AviSynthLineStateMaskInsideScript = (1 << 2),

	ScriptEvalState_None = 0,
	ScriptEvalState_Name = 1,
	ScriptEvalState_Paren = 2,
};

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & AviSynthLineStateMaskLineComment;
}

void ColouriseAvsDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int visibleChars = 0;
	int lineStateLineComment = 0;
	int nestedLevel = 0;
	int lineContinuation = 0;
	int insideScript = 0;
	int scriptEval = ScriptEvalState_None;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		1: lineStateLineComment
		1: lineContinuation
		1: insideScript
		: nestedLevel
		*/
		lineContinuation = lineState & AviSynthLineStateMaskLineContinuation;
		insideScript = lineState & AviSynthLineStateMaskInsideScript;
		nestedLevel = lineState >> 3;
		if (lineContinuation) {
			lineStateLineComment = GetLineCommentState(lineState);
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_AVS_OPERATOR:
			sc.SetState(SCE_AVS_DEFAULT);
			break;

		case SCE_AVS_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_AVS_DEFAULT);
			}
			break;

		case SCE_AVS_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				char s[128];
				sc.GetCurrentLowered(s, sizeof(s));
				scriptEval = ScriptEvalState_None;
				if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_AVS_KEYWORD);
				} else if (sc.GetLineNextChar() == '(') {
					int state = SCE_AVS_USERDFN;
					if (keywordLists[1]->InListPrefixed(s, '(')) {
						state = SCE_AVS_FUNCTION;
						if (!insideScript && EqualsAny(s, "eval", "gscript", "geval")) {
							scriptEval = ScriptEvalState_Name;
						}
					} else if (keywordLists[2]->InListPrefixed(s, '(')) {
						state = SCE_AVS_FILTER;
					} else if (keywordLists[3]->InListPrefixed(s, '(')) {
						state = SCE_AVS_PLUGIN;
					}
					sc.ChangeState(state);
				} else if (keywordLists[4]->InList(s)) {
					sc.ChangeState(SCE_AVS_CLIPPROP);
				}
				sc.SetState(SCE_AVS_DEFAULT);
			}
			break;

		case SCE_AVS_STRING:
			if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_AVS_DEFAULT);
			}
			break;

		case SCE_AVS_ESCAPESTRING:
			if (sc.ch == '\\') {
				// highlight any character as escape sequence.
				sc.SetState(SCE_AVS_ESCAPECHAR);
				sc.Forward(2);
				sc.SetState(SCE_AVS_STRING);
				continue;
			}
			if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_AVS_DEFAULT);
			}
			break;

		case SCE_AVS_TRIPLESTRING:
			if (sc.Match('\"', '\"', '\"')) {
				sc.Forward(2);
				sc.ForwardSetState(SCE_AVS_DEFAULT);
			}
			break;

		case SCE_AVS_COMMENTLINE:
			if (sc.ch == '\\' && IsEOLChar(sc.chNext)) {
				lineContinuation = AviSynthLineStateMaskLineContinuation;
			} else if (sc.atLineStart) {
				if (lineContinuation) {
					lineContinuation = 0;
				} else {
					sc.SetState(SCE_AVS_DEFAULT);
				}
			}
			break;

		case SCE_AVS_COMMENTBLOCK:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_AVS_DEFAULT);
			}
			break;

		case SCE_AVS_COMMENTBLOCKN:
			if (sc.Match('[', '*')) {
				sc.Forward();
				++nestedLevel;
			} else if (sc.Match('*', ']')) {
				sc.Forward();
				--nestedLevel;
				if (nestedLevel <= 0) {
					sc.ForwardSetState(SCE_AVS_DEFAULT);
				}
			}
			break;
		}

		if (sc.state == SCE_AVS_DEFAULT) {
			if (sc.ch == '#') {
				sc.SetState(SCE_AVS_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineComment = AviSynthLineStateMaskLineComment;
				}
			} else if (sc.Match('/', '*')) {
				sc.SetState(SCE_AVS_COMMENTBLOCK);
				sc.Forward();
			} else if (sc.Match('[', '*')) {
				sc.SetState(SCE_AVS_COMMENTBLOCKN);
				sc.Forward();
				nestedLevel = 1;
			} else if (sc.Match('\"', '\"', '\"')) {
				sc.SetState(SCE_AVS_TRIPLESTRING);
				sc.Forward(2);
				if (scriptEval == ScriptEvalState_Paren) {
					// first argument
					insideScript = AviSynthLineStateMaskInsideScript;
				}
				if (insideScript) {
					sc.ForwardSetState(SCE_AVS_DEFAULT);
					continue;
				}
				insideScript = 0;
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_AVS_STRING);
			} else if (sc.Match('e', '\"')) {
				// Avisynth+ 3.6 escaped string
				sc.SetState(SCE_AVS_ESCAPESTRING);
				sc.Forward();
			} else if (IsNumberStart(sc.ch, sc.chNext) || (sc.ch == '$' && IsHexDigit(sc.chNext))) {
				sc.SetState(SCE_AVS_NUMBER);
			} else if (IsIdentifierStart(sc.ch)) {
				sc.SetState(SCE_AVS_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_AVS_OPERATOR);
			}
			if (scriptEval && !isspacechar(sc.ch)) {
				if (sc.ch == '(' && scriptEval == ScriptEvalState_Name) {
					scriptEval = ScriptEvalState_Paren;
				} else {
					scriptEval = ScriptEvalState_None;
				}
			}
		}

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			++visibleChars;
		}
		if (sc.atLineEnd) {
			const int lineState = lineStateLineComment | lineContinuation | insideScript | (nestedLevel << 3);
			styler.SetLineState(sc.currentLine, lineState);
			visibleChars = 0;
			scriptEval = ScriptEvalState_None;
			if (!lineContinuation) {
				lineStateLineComment = 0;
			}
		}
		sc.Forward();
	}

	sc.Complete();
}

void FoldAvsDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
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

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (style == SCE_AVS_COMMENTBLOCKN) {
			if (foldComment) {
				const int level = (ch == '[' && chNext == '*') ? 1 : ((ch == '*' && chNext == ']') ? -1 : 0);
				if (level != 0) {
					levelNext += level;
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
					styleNext = styler.StyleAt(i + 1);
				}
			}
		} else if (style == SCE_AVS_TRIPLESTRING || (foldComment && style == SCE_AVS_COMMENTBLOCK)) {
			if (style != stylePrev) {
				levelNext++;
			} else if (style != styleNext) {
				levelNext--;
			}
		} else if (style == SCE_AVS_OPERATOR) {
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

LexerModule lmAVS(SCLEX_AVS, ColouriseAvsDoc, "avs", FoldAvsDoc);
