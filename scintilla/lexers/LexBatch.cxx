// Lexer for Batch.

#include <cstring>
#include <cassert>
#include <cctype>

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
inline bool IsWordStart(int ch) noexcept {
	return (ch >= 0x80) || (isgraph(ch) && !(IsBatOp(ch, false) || IsBatSpec(ch) || ch == '.'));
}
inline bool IsWordChar(int ch) noexcept {
	return (ch >= 0x80) || (isgraph(ch) && !(IsBatOp(ch, false) || IsBatSpec(ch)));
}
constexpr bool IsBatVariable(int ch) noexcept {
	return iswordchar(ch) || ch == '-' || ch == ':' || ch == '=' || ch == '$';
}
// Escape Characters https://www.robvanderwoude.com/escapechars.php
bool GetBatEscapeLen(int state, int& length, int ch, int chNext, int chNext2) noexcept {
	length = 0;
	if (ch == '^') {
		if (chNext == '^') {
			length = (chNext2 == '!') ? 2 : 1;
		} else if (chNext < 0x7F && ispunct(chNext)) {
			length = 1;
		}
	} else if (ch == '%') {
		if (chNext == '%' && !(chNext2 == '~' || chNext2 == '=' || IsAlphaNumeric(chNext2))) {
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

}

/*static const char *const batchWordListDesc[] = {
	"Internal Commands",
	"External Commands",
	0
};*/

static void ColouriseBatchDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	bool quatedVar = false;		// %var%
	bool markVar = false;		// !var!		SetLocal EnableDelayedExpansion
	bool numVar = false;		// %1, %~1
	int visibleChars = 0;
	bool inEcho = false;
	bool isGoto = false;
	bool isCall = false;
	int escapeLen = 0;

	StyleContext sc(startPos, length, initStyle, styler);
	std::vector<int> nestedState;

	for (; sc.More(); sc.Forward()) {
		if (sc.atLineStart) {
			quatedVar = false;
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
_label_identifier:
			if ((sc.ch == '%' || sc.ch == '^') && GetBatEscapeLen(sc.state, escapeLen, sc.ch, sc.chNext, sc.GetRelative(2))) {
				sc.SetState(SCE_BAT_ESCAPE);
				sc.Forward(escapeLen);
			} else if (!IsWordChar(sc.ch)) {
				char s[256];
				sc.GetCurrentLowered(s, sizeof(s));
				if (strcmp(s, "rem") == 0) {
					sc.ChangeState(SCE_BAT_COMMENT);
				} else {
					if (!inEcho && keywords.InList(s)) { // not in echo ?
						sc.ChangeState(SCE_BAT_WORD);
						inEcho = strcmp(s, "echo") == 0;
						isGoto = strcmp(s, "goto") == 0;
						isCall = strcmp(s, "call") == 0;
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
		case SCE_BAT_VARIABLE: {
			bool end_var = false;
			if (quatedVar) {
				if (sc.ch == '%') {
					if (sc.chNext == '%') {
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
				if (sc.ch == '%' && sc.chNext == '%') {
					const int chNext2 = sc.GetRelative(2);
					if (!(chNext2 == '~' || IsAlphaNumeric(chNext2))) {
						sc.Forward(2);
					}
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
					goto _label_string;
				}
			}
		} break;
		case SCE_BAT_STRINGDQ:
		case SCE_BAT_STRINGSQ:
		case SCE_BAT_STRINGBT:
_label_string:
			if (GetBatEscapeLen(sc.state, escapeLen, sc.ch, sc.chNext, sc.GetRelative(2))) {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_ESCAPE);
				sc.Forward(escapeLen);
			} else if (sc.ch == '%') {
				nestedState.push_back(sc.state);
				goto _label_variable;
			} else if (sc.ch == '!' && (iswordstart(sc.chNext) || sc.chNext == '=')) {
				nestedState.push_back(sc.state);
				quatedVar = false;
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
					goto _label_string;
				}
			} else if (sc.ch == '\"') {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_STRINGDQ);
				sc.Forward();
				goto _label_string;
			} else if (!inEcho && sc.ch == '\'') {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_STRINGSQ);
				sc.Forward();
				goto _label_string;
			} else if (sc.ch == '`') {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_BAT_STRINGBT);
				sc.Forward();
				goto _label_string;
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
					goto _label_string;
				}
			}
			break;
		}

		if (sc.state == SCE_BAT_DEFAULT) {
			if (sc.Match(':', ':')) {
				sc.SetState(SCE_BAT_COMMENT);
			} else if ((visibleChars == 0 || isGoto || isCall) && sc.ch == ':' && iswordstart(sc.chNext) && sc.chNext != '\\') {
				sc.SetState(SCE_BAT_LABEL);
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
			} else if (!inEcho && sc.ch == '\'') {
				nestedState.clear();
				sc.SetState(SCE_BAT_STRINGSQ);
			} else if (sc.ch == '`') {
				nestedState.clear();
				sc.SetState(SCE_BAT_STRINGBT);
			} else if (sc.ch == '%') {
_label_variable:
				quatedVar = false;
				markVar = false;
				numVar = false;
				sc.SetState(SCE_BAT_VARIABLE);
				if (sc.chNext == '*' || sc.chNext == '~' || (!markVar && sc.chNext == '%')) {
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
					quatedVar = true;
				}
			} else if (sc.ch == '!' && (iswordstart(sc.chNext) || sc.chNext == '=')) {
				quatedVar = false;
				markVar = true;
				numVar = false;
				sc.SetState(SCE_BAT_VARIABLE);
			} else if (sc.ch == '@' && sc.MatchIgnoreCase("@rem")) {
				sc.SetState(SCE_BAT_COMMENT);
			} else if (IsWordStart(sc.ch)) { // all file name
				sc.SetState(SCE_BAT_IDENTIFIER);
			} else if (IsBatOp(sc.ch, inEcho)) {
				sc.SetState(SCE_BAT_OPERATOR);
				if (inEcho && sc.ch == ')') {
					inEcho = false;
				}
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

		if (sc.atLineEnd) {
			visibleChars = 0;
		}
		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
	}

	if (sc.state == SCE_BAT_IDENTIFIER) {
		goto _label_identifier;
	}

	sc.Complete();
}

LexerModule lmBatch(SCLEX_BATCH, ColouriseBatchDoc, "batch");
