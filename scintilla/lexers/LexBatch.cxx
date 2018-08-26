// Lexer for Batch.

#include <cstring>
#include <cassert>
#include <cctype>

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

// / \ : * ? < > " |
static constexpr bool IsBatSpec(int ch) noexcept {
	return (ch < 0x80) && (ch == ':' || ch == '?' || ch == '%' || ch == '\'' || ch == '\"' || ch == '`');
}
static constexpr bool IsBatOp(int ch) noexcept {
	return (ch < 0x80) && (ch == '(' || ch == ')' || ch == '=' || ch == '@' || ch == '|' || ch == '<' || ch == '>' || ch == ';' || ch == '*' || ch == '&');
}
static inline bool IsWordStart(int ch) noexcept {
	return (ch >= 0x80) || ((ch < 0x80) && isgraph(ch) && !(IsBatOp(ch) || IsBatSpec(ch) || ch == '.'));
}
static inline bool IsWordChar(int ch) noexcept {
	if (isspacechar(ch))
		return false;
	else if (ch >= 0x80)
		return true;
	else
		return isgraph(ch) && !(IsBatOp(ch) || IsBatSpec(ch));
}

/*static const char *const batchWordListDesc[] = {
	"Internal Commands",
	"External Commands",
	0
};*/

static void ColouriseBatchDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	bool quatedVar = false;
	bool numVar = false;
	int visibleChars = 0;
	bool inEcho = false;
	bool isGoto = false;
	bool isCall = false;
	int varStyle = SCE_BAT_DEFAULT;

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {
		if (sc.atLineStart) {
			quatedVar = false;
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
			if (!IsWordChar(sc.ch)) {
				char s[256];
				sc.GetCurrentLowered(s, sizeof(s));
				if (strcmp(s, "rem") == 0)
					sc.ChangeState(SCE_BAT_COMMENT);
				else {
					if (!inEcho && keywords.InList(s)) { // not in echo ?
						sc.ChangeState(SCE_BAT_WORD);
						inEcho = strcmp(s, "echo") == 0;
						isGoto = strcmp(s, "goto") == 0;
						isCall = strcmp(s, "call") == 0;
					} else if (!inEcho && visibleChars == static_cast<int>(strlen(s))) {
						if (visibleChars == 1 && sc.ch == ':' && sc.chNext == '\\') {
							sc.Forward(2);
							while (IsWordChar(sc.ch) || sc.ch == '\\')
								sc.Forward();
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
			if (sc.ch == ':')
				sc.ForwardSetState(SCE_BAT_DEFAULT);
			else if (!(iswordstart(sc.ch) || sc.ch == '-'))
				sc.SetState(SCE_BAT_DEFAULT);
			break;
		case SCE_BAT_VARIABLE:
		{
			bool end_var = false;
			if (quatedVar) {
				if (sc.ch == '%') {
					end_var = true;
					sc.ForwardSetState(varStyle);
				}
			} else {
				if (sc.ch == '*') {
					end_var = true;
					sc.ForwardSetState(varStyle);
				} else if (numVar) {
					if (!IsADigit(sc.ch)) {
						end_var = true;
						sc.SetState(varStyle);
					}
				} else if (!(iswordstart(sc.ch) || sc.ch == '-')) {
					end_var = true;
					sc.SetState(varStyle);
				}
			}
			if (end_var && varStyle != SCE_BAT_DEFAULT) {
				goto _label_var_string;
			}
		} break;
		case SCE_BAT_STRINGDQ:
		case SCE_BAT_STRINGSQ:
		case SCE_BAT_STRINGBT:
_label_var_string:
			if (sc.ch == '%') {
				varStyle = sc.state;
				goto _label_variable;
			} else if (sc.atLineEnd || (sc.state == SCE_BAT_STRINGDQ && sc.ch == '\"')
				|| (sc.state == SCE_BAT_STRINGSQ && sc.ch == '\'')
				|| (sc.state == SCE_BAT_STRINGBT && sc.ch == '`')) {
				sc.ForwardSetState(SCE_BAT_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_BAT_DEFAULT) {
			varStyle = SCE_BAT_DEFAULT;
			if (sc.Match(':', ':')) {
				sc.SetState(SCE_BAT_COMMENT);
			} else if ((visibleChars == 0 || isGoto || isCall) && sc.ch == ':' && IsWordStart(sc.chNext) && sc.chNext != '\\') {
				sc.SetState(SCE_BAT_LABEL);
				if (isGoto)
					isGoto = false;
				if (isCall)
					isCall = false;
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_BAT_STRINGDQ);
			} else if (!inEcho && sc.ch == '\'') {
				sc.SetState(SCE_BAT_STRINGSQ);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_BAT_STRINGBT);
			} else if (sc.ch == '%') {
_label_variable:
				sc.SetState(SCE_BAT_VARIABLE);
				if (sc.chNext == '*' || sc.chNext == '~' || sc.chNext == '%') {
					quatedVar = false;
					if (sc.chNext == '%')
						sc.Forward();
					if (sc.chNext == '~') {
						sc.Forward();
						while (sc.More() && !(IsADigit(sc.ch) || isspacechar(sc.ch))) sc.Forward();
						numVar = true;
					}
				} else if (IsADigit(sc.chNext)) {
					quatedVar = false;
					numVar = true;
				} else {
					quatedVar = true;
				}
			} else if (sc.ch == '@' && sc.MatchIgnoreCase("@rem")) {
				sc.SetState(SCE_BAT_COMMENT);
			} else if (IsWordStart(sc.ch)) { // all file name
				sc.SetState(SCE_BAT_IDENTIFIER);
			} else if (IsBatOp(sc.ch)) {
				sc.SetState(SCE_BAT_OPERATOR);
				if (inEcho && sc.ch == ')')
					inEcho = false;
				if (sc.ch == '|' || sc.ch == '&') { // pipe
					if (sc.chNext == '&') {	// cmd1 && cmd2
						sc.Forward();
					}
					sc.ForwardSetState(SCE_BAT_DEFAULT);
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

	if (sc.state == SCE_BAT_IDENTIFIER)
		goto _label_identifier;

	sc.Complete();
}

LexerModule lmBatch(SCLEX_BATCH, ColouriseBatchDoc, "batch");
