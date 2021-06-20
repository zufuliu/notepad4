// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Batch.

#include <cassert>
#include <cstring>

#include <string_view>
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

using namespace Lexilla;

namespace {

enum {
	BatchLineStateMaskEmptyLine = 1 << 0,
	BatchLineStateMaskVisibleChars = 1 << 1,
	BatchLineStateLineContinuation = 1 << 2,
};

// / \ : * ? < > " |
constexpr bool IsBatSpec(int ch) noexcept {
	return ch == ':' || ch == '?' || ch == '%' || ch == '!' || ch == '\'' || ch == '\"' || ch == '`';
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

constexpr bool IsLabelStart(int ch) noexcept {
	// ! is not allowed with SetLocal EnableDelayedExpansion
	//return !AnyOf(ch, '\r', '\n', ' ', '\t', '|', '&', '%', '!', ',', ';', '=', '+', '<', '>');
	return IsIdentifierChar(ch) || ch == '.';
}

constexpr bool IsLabelChar(int ch) noexcept {
	//return !AnyOf(ch, '\r', '\n', ' ', '\t', '|', '&', '%', '!', ',', ';', '=');
	return IsIdentifierChar(ch) || ch == '.';
}

constexpr bool IsBatVariableNext(int chNext) noexcept {
	return chNext == '~' || chNext == '=' || chNext == '*' || chNext == '#' || iswordstart(chNext);
}
constexpr bool IsMarkVariableNext(int chNext) noexcept {
	return chNext == '=' || iswordstart(chNext);
}
// someone's, don't
constexpr bool IsSingleQuotedString(int ch, int chPrev, int chNext) noexcept {
	return ch == '\'' && !((chPrev >= 0x80 || IsAlphaNumeric(chPrev)) && (chNext == 's' || chNext == 't' || chNext == 'S' || chNext == 'T'));
}

bool GetBatEscapeLen(StyleContext &sc, int& length) noexcept {
	// Escape Characters https://www.robvanderwoude.com/escapechars.php
	length = 0;
	switch (sc.ch) {
	case '^':
		if (IsEOLChar(sc.chNext)) {
			// line continuation
			return true;
		}
		if (sc.chNext == '^') {
			length = (sc.GetRelative(2) == '!') ? 2 : 1;
		} else if (IsPunctuation(sc.chNext)) {
			length = 1;
		}
		break;

	case '%':
		if (sc.chNext == '%' && !IsBatVariableNext(sc.GetRelative(2))) {
			length = 1;
		}
		break;

	case '\"':
		// Inside the search pattern of FIND
		if (sc.chNext == '"' && (sc.state == SCE_BAT_STRINGDQ || sc.state == SCE_BAT_STRINGSQ || sc.state == SCE_BAT_STRINGBT)) {
			length = 1;
		}
		break;

	case '\\':
		// Inside the regex pattern of FINDSTR
		break;
	}

	return length != 0;
}

constexpr bool IsTildeExpansion(int ch) noexcept {
	return AnyOf(ch, 'f', 'd', 'p', 'n', 'x', 's', 'a', 't', 'z');
}

char DetectBatVariable(StyleContext &sc) {
	char varQuoteChar = '\0';
	sc.SetState(SCE_BAT_VARIABLE);
	if (sc.chNext == '*' || IsADigit(sc.chNext)) { // %*, %1 ... %9
		sc.Forward();
	} else if (sc.chNext == '~' || sc.chNext == '%') {
		sc.Forward();
		if (sc.ch == '%') {
			sc.Forward();
		}
		if (sc.ch == '~') {
			// see help for CALL and FOR commands
			sc.Forward();
			while (IsTildeExpansion(sc.ch)) {
				sc.Forward();
			}
			if (sc.ch == '$') {
				while (sc.More() && !(sc.ch == ':' || isspacechar(sc.chNext))) {
					sc.Forward();
				}
				if (sc.ch == ':') {
					sc.Forward();
				}
			}
		}
		if (sc.ch == '^') {
			// see https://www.robvanderwoude.com/clevertricks.php
			sc.Forward();
		}
	} else {
		varQuoteChar = '%';
	}
	return varQuoteChar;
}

static_assert(DefaultNestedStateBaseStyle + 1 == SCE_BAT_STRINGDQ);
static_assert(DefaultNestedStateBaseStyle + 2 == SCE_BAT_STRINGSQ);
static_assert(DefaultNestedStateBaseStyle + 3 == SCE_BAT_STRINGBT);

void ColouriseBatchDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const bool fold = styler.GetPropertyInt("fold", 1) & true;
	char varQuoteChar = '\0'; // %var% or !var! after SetLocal EnableDelayedExpansion
	int logicalVisibleChars = 0;
	int lineVisibleChars = 0;
	int prevLineState = 0;
	bool inEcho = false;
	int escapeLen = 0;
	int parenCount = 0;

	StyleContext sc(startPos, length, initStyle, styler);
	std::vector<int> nestedState;
	int levelCurrent = SC_FOLDLEVELBASE;
	if (sc.currentLine > 0) {
		levelCurrent = styler.LevelAt(sc.currentLine - 1) >> 16;
		prevLineState = styler.GetLineState(sc.currentLine - 1);
		/*
		1: empty line
		1: logicalVisibleChars
		1: line continuation
		8: parenCount
		: nestedState
		*/
		parenCount = (prevLineState >> 8) & 0xff;
		if (prevLineState & BatchLineStateLineContinuation) {
			logicalVisibleChars = prevLineState & BatchLineStateMaskVisibleChars;
		}
		const int lineState = prevLineState >> 16;
		if (lineState) {
			UnpackLineState(lineState, nestedState);
		}
	}

	int levelNext = levelCurrent;

	while (sc.More()) {
		switch (sc.state) {
		case SCE_BAT_OPERATOR:
			sc.SetState(SCE_BAT_DEFAULT);
			break;

		case SCE_BAT_IDENTIFIER:
			if ((sc.ch == '%' || sc.ch == '^') && GetBatEscapeLen(sc, escapeLen)) {
				sc.SetState(SCE_BAT_ESCAPECHAR);
				sc.Forward(escapeLen);
			} else if (!IsWordChar(sc.ch)) {
				char s[64];
				sc.GetCurrentLowered(s, sizeof(s));
				if (StrEqual(s, "rem")) {
					sc.ChangeState(SCE_BAT_COMMENT);
				} else {
					if (!inEcho && keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_BAT_WORD);
						inEcho = false;
						if (StrEqualsAny(s, "echo", "echo.", "title", "cd", "chdir")) {
							// commands with unquoted string argument
							inEcho = true;
						} else if (StrEqualsAny(s, "do", "else")) {
							logicalVisibleChars = 0;
						} else {
							const bool isGoto = StrEqual(s, "goto");
							if (isGoto || StrEqual(s, "call")) {
								sc.SetState(SCE_BAT_DEFAULT);
								while (IsASpaceOrTab(sc.ch)) {
									sc.Forward();
								}
								if (sc.ch == ':' || (isGoto && IsGraphic(sc.ch))) {
									sc.SetState(SCE_BAT_LABEL);
								}
							}
						}
					} else if (!inEcho && logicalVisibleChars == sc.LengthCurrent()) {
						sc.ChangeState(SCE_BAT_COMMAND);
					}
					if (sc.state != SCE_BAT_LABEL) {
						sc.SetState(SCE_BAT_DEFAULT);
					}
				}
			}
			break;

		case SCE_BAT_COMMENT:
			if (sc.atLineStart) {
				sc.SetState(SCE_BAT_DEFAULT);
			}
			break;

		case SCE_BAT_LABEL:
			if (!IsLabelChar(sc.ch)) {
				if (sc.ch == ':') {
					sc.Forward();
				}
				sc.SetState(SCE_BAT_DEFAULT);
			}
			break;

		case SCE_BAT_LABEL_LINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_BAT_DEFAULT);
			} else if (IsGraphic(sc.ch) && !IsLabelChar(sc.ch)) {
				sc.ChangeState(SCE_BAT_NOT_BATCH);
				levelNext++;
			}
			break;

		case SCE_BAT_VARIABLE:
			if (varQuoteChar) {
				if (sc.ch == varQuoteChar) {
					varQuoteChar = '\0';
					sc.Forward();
				} else if (sc.ch == '\"' || sc.ch == '\'' || sc.ch == '`' || IsEOLChar(sc.ch)) {
					// something went wrong
					varQuoteChar = '\0';
				}
			}
			if (varQuoteChar == '\0') {
				if (nestedState.empty()) {
					sc.SetState(SCE_BAT_DEFAULT);
				} else {
					sc.SetState(nestedState.back());
					nestedState.pop_back();
					continue;
				}
			}
			break;

		case SCE_BAT_STRINGDQ:
		case SCE_BAT_STRINGSQ:
		case SCE_BAT_STRINGBT:
			if (GetBatEscapeLen(sc, escapeLen)) {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_ESCAPECHAR);
				sc.Forward(escapeLen);
			} else if (sc.ch == '%') {
				nestedState.push_back(sc.state);
				varQuoteChar = DetectBatVariable(sc);
			} else if (sc.ch == '!' && IsMarkVariableNext(sc.chNext)) {
				nestedState.push_back(sc.state);
				varQuoteChar = '!';
				sc.SetState(SCE_BAT_VARIABLE);
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

		case SCE_BAT_ESCAPECHAR:
			if (GetBatEscapeLen(sc, escapeLen)) {
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

		case SCE_BAT_NOT_BATCH:
			if (lineVisibleChars == 0 && sc.ch == ':') {
				// resume batch parsing on new label
				if (IsLabelStart(sc.chNext)) {
					sc.SetState(SCE_BAT_LABEL_LINE);
					levelNext--;
				}
			}
			break;
		}

		if (sc.state == SCE_BAT_DEFAULT) {
			if (sc.Match(':', ':')) {
				sc.SetState(SCE_BAT_COMMENT);
			} else if (sc.atLineStart && sc.Match('#', '!')) {
				// shell shebang starts embedded script
				parenCount = 0;
				sc.SetState(SCE_BAT_NOT_BATCH);
				levelNext++;
			} else if (lineVisibleChars == 0 && sc.ch == ':') {
				parenCount = 0;
				if (IsLabelStart(sc.chNext)) {
					sc.SetState(SCE_BAT_LABEL_LINE);
				} else {
					// unreachable label starts skipped block
					sc.SetState(SCE_BAT_NOT_BATCH);
					levelNext++;
				}
			} else if (GetBatEscapeLen(sc, escapeLen)) {
				sc.SetState(SCE_BAT_ESCAPECHAR);
				sc.Forward(escapeLen);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_BAT_STRINGDQ);
			} else if (IsSingleQuotedString(sc.ch, sc.chPrev, sc.chNext)) {
				sc.SetState(SCE_BAT_STRINGSQ);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_BAT_STRINGBT);
			} else if (sc.ch == '%') {
				varQuoteChar = DetectBatVariable(sc);
			} else if (sc.ch == '!' && IsMarkVariableNext(sc.chNext)) {
				varQuoteChar = '!';
				sc.SetState(SCE_BAT_VARIABLE);
			} else if (sc.ch == '(' || sc.ch == ')') {
				sc.SetState(SCE_BAT_OPERATOR);
				if (!inEcho || parenCount > 0) {
					if (sc.ch == '(') {
						parenCount++;
						levelNext++;
					} else {
						parenCount--;
						levelNext--;
						inEcho = false;
					}
				}
			} else if (IsBatOp(sc.ch, inEcho)) {
				sc.SetState(SCE_BAT_OPERATOR);
				if (sc.ch == '>') {
					if (sc.chNext == '&') {
						// output redirect: 2>&1
						sc.Forward();
					}
					inEcho = false;
				} else if (sc.ch == '|' || sc.ch == '&') { // pipe
					if (sc.ch == sc.chNext) {	// cmd1 || cmd2, cmd1 && cmd2
						sc.Forward();
					}
					if (IsWordStart(sc.chNext)) {
						sc.ForwardSetState(SCE_BAT_IDENTIFIER);
					} else {
						sc.ForwardSetState(SCE_BAT_DEFAULT);
					}
					logicalVisibleChars = 0;
					inEcho = false;
					continue;
				}
				if (logicalVisibleChars == 0 && sc.ch == '@' && IsWordStart(sc.chNext)) {
					sc.Forward();
					if (sc.MatchIgnoreCase("rem") && !IsGraphic(sc.GetRelative(3))) {
						sc.ChangeState(SCE_BAT_COMMENT);
					} else {
						sc.SetState(SCE_BAT_IDENTIFIER);
					}
				}
			} else if (IsWordStart(sc.ch)) {
				sc.SetState(SCE_BAT_IDENTIFIER);
			}
		}

		if (!isspacechar(sc.ch)) {
			logicalVisibleChars++;
			lineVisibleChars++;
		}
		if (sc.atLineEnd) {
			const int chPrev = sc.LineLastChar();
			varQuoteChar = '\0';

			int lineState = lineVisibleChars ? 0 : BatchLineStateMaskEmptyLine;
			lineVisibleChars = 0;
			if (chPrev == '^') {
				lineState = BatchLineStateLineContinuation | (logicalVisibleChars ? BatchLineStateMaskVisibleChars : 0);
			} else {
				inEcho = false;
				logicalVisibleChars = 0;
			}
			lineState |= parenCount << 8;
			if (sc.state == SCE_BAT_STRINGDQ || sc.state == SCE_BAT_STRINGSQ || sc.state == SCE_BAT_STRINGBT) {
				if (sc.state != SCE_BAT_STRINGDQ && (chPrev == '^' || chPrev == '|' || chPrev == '&')) {
					if (!nestedState.empty()) {
						lineState |= PackLineState(nestedState) << 16;
					}
				} else {
					nestedState.clear();
					sc.SetState(SCE_BAT_DEFAULT);
				}
			}
			styler.SetLineState(sc.currentLine, lineState);

			if (fold) {
				if (sc.state == SCE_BAT_LABEL_LINE) {
					levelCurrent = SC_FOLDLEVELBASE;
					levelNext = SC_FOLDLEVELBASE + 1;
					if (prevLineState & BatchLineStateMaskEmptyLine) {
						styler.SetLevel(sc.currentLine - 1, SC_FOLDLEVELBASE | (SC_FOLDLEVELBASE << 16));
					}
				}

				const int levelUse = levelCurrent;
				int lev = levelUse | levelNext << 16;
				if (levelUse < levelNext) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				}
				if (lev != styler.LevelAt(sc.currentLine)) {
					styler.SetLevel(sc.currentLine, lev);
				}
				levelCurrent = levelNext;
				prevLineState = lineState;
			}
		}
		sc.Forward();
	}

	sc.Complete();
}

}

LexerModule lmBatch(SCLEX_BATCH, ColouriseBatchDoc, "batch");
