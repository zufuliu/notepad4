// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Batch.

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

using namespace Scintilla;

namespace {

// / \ : * ? < > " |
constexpr bool IsBatSpec(int ch) noexcept {
	return ch == ':' || ch == '?' || ch == '%' || ch == '\'' || ch == '\"' || ch == '`';
}
constexpr bool IsBatOp(int ch, bool inEcho) noexcept {
	return ch == '&' || ch == '|' || ch == '<' || ch == '>' || ch == '(' || ch == ')'
		|| (!inEcho && (ch == '=' || ch == '@' || ch == ';' || ch == '*' || ch == ','));
}
constexpr bool IsWordStart(int ch) noexcept {
	return IsGraphic(ch) && !(IsBatOp(ch, false) || IsBatSpec(ch) || ch == '.');
}
constexpr bool IsWordChar(int ch) noexcept {
	return IsGraphic(ch) && !(IsBatOp(ch, false) || IsBatSpec(ch));
}
constexpr bool IsBatVariable(int ch) noexcept {
	return iswordchar(ch) || ch == '-' || ch == ':' || ch == '=' || ch == '$';
}
constexpr bool IsBatVariableNext(int chNext) noexcept {
	return chNext == '~' || chNext == '=' || chNext == '*' || iswordstart(chNext);
}
constexpr bool IsMarkVariableNext(int chNext) noexcept {
	return chNext == '=' || iswordstart(chNext);
}
// someone's, don't
constexpr bool IsSingleQuotedString(int ch, int chPrev, int chNext) noexcept {
	return ch == '\'' && !((chPrev >= 0x80 || IsAlphaNumeric(chPrev)) && (chNext == 's' || chNext == 't' || chNext == 'S' || chNext == 'T'));
}
// Escape Characters https://www.robvanderwoude.com/escapechars.php
constexpr bool GetBatEscapeLen(int state, int& length, int ch, int chNext, int chNext2) noexcept {
	length = 0;
	if (ch == '^') {
		if (chNext == '^') {
			length = (chNext2 == '!') ? 2 : 1;
		} else if (IsPunctuation(chNext)) {
			length = 1;
		}
	} else if (ch == '%') {
		if (chNext == '%' && !IsBatVariableNext(chNext2)) {
			length = 1;
		}
	} else if (ch == '"') {
		// Inside the search pattern of FIND
		if (chNext == '"' && (state == SCE_BAT_STRINGDQ || state == SCE_BAT_STRINGSQ || state == SCE_BAT_STRINGBT)) {
			length = 1;
		}
	} else if (ch == '\\') {
		// Inside the regex pattern of FINDSTR
	}

	return length != 0;
}

constexpr int LevelNumber(int level) noexcept {
	return (level & SC_FOLDLEVELNUMBERMASK) - SC_FOLDLEVELBASE;
}

void DetectBatVariable(StyleContext &sc, bool &quotedVar, bool &markVar, bool &numVar) {
	quotedVar = false;
	markVar = false;
	numVar = false;
	sc.SetState(SCE_BAT_VARIABLE);
	if (sc.chNext == '*' || sc.chNext == '~' || sc.chNext == '%') {
		if (sc.chNext == '%') {
			sc.Forward();
		}
		if (sc.chNext == '~') {
			sc.Forward();
			numVar = sc.chNext != '$';
			if (numVar) {
				while (sc.More() && !(IsADigit(sc.ch) || isspacechar(sc.ch))) {
					sc.Forward();
				}
			}
		}
	} else if (IsADigit(sc.chNext)) {
		numVar = true;
	} else {
		quotedVar = true;
	}
}

}

/*static const char *const batchWordListDesc[] = {
	"Internal Commands",
	"External Commands",
	0
};*/

static void ColouriseBatchDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	bool quotedVar = false;		// %var%
	bool markVar = false;		// !var!		SetLocal EnableDelayedExpansion
	bool numVar = false;		// %1, %~1
	int visibleChars = 0;
	bool inEcho = false;
	bool isGoto = false;
	bool isCall = false;
	int escapeLen = 0;

	StyleContext sc(startPos, length, initStyle, styler);
	std::vector<int> nestedState;

	int levelCurrent = SC_FOLDLEVELBASE;
	if (sc.currentLine > 0) {
		levelCurrent = styler.LevelAt(sc.currentLine - 1) >> 16;
	}
	int levelNext = levelCurrent;
	int parenCount = 0;

	while (sc.More()) {
		if (sc.atLineStart) {
			quotedVar = false;
			markVar = false;
			numVar = false;
			inEcho = false;
			isGoto = false;
			isCall = false;
			visibleChars = 0;
		}

		switch (sc.state) {
		case SCE_BAT_OPERATOR:
			sc.SetState(SCE_BAT_DEFAULT);
			break;
		case SCE_BAT_IDENTIFIER:
			if ((sc.ch == '%' || sc.ch == '^') && GetBatEscapeLen(sc.state, escapeLen, sc.ch, sc.chNext, sc.GetRelative(2))) {
				sc.SetState(SCE_BAT_ESCAPE);
				sc.Forward(escapeLen);
			} else if (sc.ch == '.' && !inEcho && sc.LengthCurrent() == 4 && sc.styler.MatchIgnoreCase(sc.styler.GetStartSegment(), "echo")) {
				inEcho = true;
				parenCount = levelNext;
				sc.ChangeState(SCE_BAT_WORD);
				sc.ForwardSetState(SCE_BAT_DEFAULT);
			} else if (!IsWordChar(sc.ch)) {
				char s[256];
				sc.GetCurrentLowered(s, sizeof(s));
				if (strcmp(s, "rem") == 0) {
					sc.ChangeState(SCE_BAT_COMMENT);
				} else {
					if (!inEcho && keywords.InList(s)) { // not in echo ?
						sc.ChangeState(SCE_BAT_WORD);
						inEcho = strcmp(s, "echo") == 0 || strcmp(s, "echo.") == 0 || strcmp(s, "title") == 0 || strcmp(s, "prompt") == 0;
						isGoto = strcmp(s, "goto") == 0;
						isCall = strcmp(s, "call") == 0;
						if (inEcho) {
							parenCount = levelNext;
						}
					} else if (!inEcho && visibleChars == static_cast<int>(strlen(s))) {
						if (visibleChars == 1 && sc.ch == ':' && sc.chNext == '\\') {
							sc.Forward(2);
							while (IsWordChar(sc.ch) || sc.ch == '\\') {
								sc.Forward();
							}
						}
						sc.ChangeState(SCE_BAT_COMMAND);
					} else if (isGoto) {
						sc.ChangeState(SCE_BAT_LABEL);
						isGoto = false;
					}
					sc.SetState(SCE_BAT_DEFAULT);
				}
			}
			break;
		case SCE_BAT_COMMENT:
			if (sc.atLineStart) {
				visibleChars = 0;
				sc.SetState(SCE_BAT_DEFAULT);
			}
			break;
		case SCE_BAT_LABEL:
			if (sc.ch == ':') {
				sc.ForwardSetState(SCE_BAT_DEFAULT);
			} else if (!(iswordstart(sc.ch) || sc.ch == '-')) {
				sc.SetState(SCE_BAT_DEFAULT);
			}
			break;
		case SCE_BAT_LABEL_LINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_BAT_DEFAULT);
			}
			break;
		case SCE_BAT_VARIABLE: {
			bool end_var = false;
			if (quotedVar) {
				if (sc.ch == '%') {
					if (sc.chNext == '%' && !IsBatVariableNext(sc.GetRelative(2))) {
						sc.Forward();
					}
					end_var = true;
					sc.Forward();
				}
			} else if (markVar) {
				if (sc.ch == '!') {
					end_var = true;
					sc.Forward();
				}
			} else {
				if (sc.ch == '*') {
					end_var = true;
					sc.Forward();
				} else if (numVar) {
					if (!IsADigit(sc.ch)) {
						end_var = true;
					}
				} else if (!IsBatVariable(sc.ch)) {
					end_var = true;
				}
				if (sc.ch == '%' && sc.chNext == '%' && !IsBatVariableNext(sc.GetRelative(2))) {
					sc.Forward(2);
				}
			}
			if (!end_var && isspacechar(sc.ch)) {
				end_var = true;
			}
			if (end_var) {
				if (nestedState.empty()) {
					sc.SetState(SCE_BAT_DEFAULT);
				} else {
					sc.SetState(nestedState.back());
					nestedState.pop_back();
					continue;
				}
			}
		}
		break;
		case SCE_BAT_STRINGDQ:
		case SCE_BAT_STRINGSQ:
		case SCE_BAT_STRINGBT:
			if (GetBatEscapeLen(sc.state, escapeLen, sc.ch, sc.chNext, sc.GetRelative(2))) {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_ESCAPE);
				sc.Forward(escapeLen);
			} else if (sc.ch == '%') {
				nestedState.push_back(sc.state);
				DetectBatVariable(sc, quotedVar, markVar, numVar);
			} else if (sc.ch == '!' && IsMarkVariableNext(sc.chNext)) {
				nestedState.push_back(sc.state);
				quotedVar = false;
				markVar = true;
				numVar = false;
				sc.SetState(SCE_BAT_VARIABLE);
			} else if (sc.atLineEnd) {
				bool multiline = false;
				if (sc.state == SCE_BAT_STRINGSQ || sc.state == SCE_BAT_STRINGBT) {
					const Sci_Position start = styler.LineStart(sc.currentLine);
					Sci_Position pos = sc.currentPos;
					Sci_Position offset = 0;
					while (pos >= start + 2 && IsASpace(styler.SafeGetCharAt(pos))) {
						--pos;
						--offset;
					}

					const int chNext = sc.GetRelative(offset);
					if ((chNext == '|' || chNext == '&')) {
						const int ch = sc.GetRelative(offset - 1);
						multiline = ch == '^';
					}
				}
				if (!multiline) {
					nestedState.clear();
					sc.SetState(SCE_BAT_DEFAULT);
				}
			} else if ((sc.state == SCE_BAT_STRINGDQ && sc.ch == '\"')
				|| (sc.state == SCE_BAT_STRINGSQ && sc.ch == '\'')
				|| (sc.state == SCE_BAT_STRINGBT && sc.ch == '`')) {
				if (nestedState.empty()) {
					sc.ForwardSetState(SCE_BAT_DEFAULT);
				} else {
					sc.ForwardSetState(nestedState.back());
					nestedState.pop_back();
					continue;
				}
			} else if (sc.ch == '\"') {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_STRINGDQ);
			} else if (IsSingleQuotedString(sc.ch, sc.chPrev, sc.chNext)) {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_STRINGSQ);
			} else if (sc.ch == '`') {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_STRINGBT);
			}
			break;
		case SCE_BAT_ESCAPE:
			if (GetBatEscapeLen(sc.state, escapeLen, sc.ch, sc.chNext, sc.GetRelative(2))) {
				sc.Forward(escapeLen);
			} else {
				if (nestedState.empty()) {
					sc.SetState(SCE_BAT_DEFAULT);
				} else {
					sc.SetState(nestedState.back());
					nestedState.pop_back();
					continue;
				}
			}
			break;
		}

		if (sc.state == SCE_BAT_DEFAULT) {
			if (sc.Match(':', ':')) {
				sc.SetState(SCE_BAT_COMMENT);
			} else if ((visibleChars == 0 || isGoto || isCall) && sc.ch == ':' && iswordstart(sc.chNext) && sc.chNext != '\\') {
				sc.SetState((visibleChars == 0) ? SCE_BAT_LABEL_LINE : SCE_BAT_LABEL);
				if (isGoto) {
					isGoto = false;
				}
				if (isCall) {
					isCall = false;
				}
			} else if (GetBatEscapeLen(sc.state, escapeLen, sc.ch, sc.chNext, sc.GetRelative(2))) {
				sc.SetState(SCE_BAT_ESCAPE);
				sc.Forward(escapeLen);
			} else if (sc.ch == '\"') {
				nestedState.clear();
				sc.SetState(SCE_BAT_STRINGDQ);
			} else if (IsSingleQuotedString(sc.ch, sc.chPrev, sc.chNext)) {
				nestedState.clear();
				sc.SetState(SCE_BAT_STRINGSQ);
			} else if (sc.ch == '`') {
				nestedState.clear();
				sc.SetState(SCE_BAT_STRINGBT);
			} else if (sc.ch == '%') {
				DetectBatVariable(sc, quotedVar, markVar, numVar);
			} else if (sc.ch == '!' && IsMarkVariableNext(sc.chNext)) {
				quotedVar = false;
				markVar = true;
				numVar = false;
				sc.SetState(SCE_BAT_VARIABLE);
			} else if (sc.MatchIgnoreCase("@rem")) {
				sc.SetState(SCE_BAT_COMMENT);
			} else if (IsWordStart(sc.ch)) { // all file name
				sc.SetState(SCE_BAT_IDENTIFIER);
			} else if (sc.ch == '(' || sc.ch == ')') {
				if (!inEcho || LevelNumber(parenCount) > 0) {
					sc.SetState(SCE_BAT_OPERATOR);
					if (sc.ch == '(') {
						levelNext++;
					} else if (sc.ch == ')') {
						levelNext--;
						inEcho = false;
					}
				}
			} else if (IsBatOp(sc.ch, inEcho)) {
				sc.SetState(SCE_BAT_OPERATOR);
				if (sc.ch == '|' || sc.ch == '&') { // pipe
					if (sc.ch == sc.chNext) {	// cmd1 || cmd2, cmd1 && cmd2
						sc.Forward();
					}
					if (IsWordStart(sc.chNext)) {
						sc.ForwardSetState(SCE_BAT_IDENTIFIER);
					} else {
						sc.ForwardSetState(SCE_BAT_DEFAULT);
					}
					visibleChars = 0;
					inEcho = false;
					continue;
				}
				if (visibleChars == 0 && sc.ch == '@' && IsWordStart(sc.chNext)) {
					sc.ForwardSetState(SCE_BAT_IDENTIFIER);
					visibleChars = 0;
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			visibleChars = 0;
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(sc.currentLine)) {
				styler.SetLevel(sc.currentLine, lev);
			}
			levelCurrent = levelNext;
		}
		sc.Forward();
	}

	sc.Complete();
}

LexerModule lmBatch(SCLEX_BATCH, ColouriseBatchDoc, "batch");
