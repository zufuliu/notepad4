// Lexer for CMake.

#include <string.h>
#include <assert.h>
#include <ctype.h>

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

/*static const char* const cmakeWordLists[] = {
	"Control Command",
	"Command",
	"Information Variables",
	"Behavior Variables",
	"System Variables",
	0
}*/

static inline bool IsCmakeOperator(char ch) {
	return ch == '(' || ch == ')' || ch == '=' || ch == ':' || ch == ';';
}

static void ColouriseCmakeDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];

	int varStyle = SCE_CMAKE_DEFAULT;
	static int nvarLevel = 0; // nested variable ${${}}
	int userDefType = 0;
	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {
		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_CMAKE_OPERATOR:
			sc.SetState(SCE_CMAKE_DEFAULT);
			break;
		case SCE_CMAKE_IDENTIFIER:
_label_identifier:
			if (!iswordstart(sc.ch)) {
				char s[128];
				sc.GetCurrentLowered(s, sizeof(s) - 3);
				if (keywords.InListAbbreviated(s, '(')) {
					sc.ChangeState(SCE_CMAKE_WORD);
					if (strcmp(s, "function()") == 0 || strcmp(s, "endfunction()") == 0)
						userDefType = 1;
					else if (strcmp(s, "macro()") == 0 || strcmp(s, "endmacro()") == 0)
						userDefType = 2;
				} else if (keywords2.InListAbbreviated(s, '(')) {
					sc.ChangeState(SCE_CMAKE_COMMANDS);
				} else if (LexGetNextChar(sc.currentPos, styler) == '(') {
					sc.ChangeState(SCE_CMAKE_FUNCATION);
				} else if (userDefType) {
					sc.ChangeState(userDefType == 1 ? SCE_CMAKE_FUNCATION : SCE_CMAKE_MACRO);
					userDefType = 0;
				}
				sc.SetState(SCE_CMAKE_DEFAULT);
			}
			break;

		case SCE_CMAKE_COMMENT:
			if (sc.atLineStart) {
				sc.SetState(SCE_CMAKE_DEFAULT);
			}
			break;
		case SCE_CMAKE_STRINGDQ:
		case SCE_CMAKE_STRINGSQ:
		case SCE_CMAKE_STRINGBT:
_label_var_string:
			if (sc.ch == '\\' && (sc.chNext == '\\' || sc.chNext == '\"' || sc.chNext == '\'')) {
				sc.Forward();
			} else if (sc.Match('$', '{')) {
				varStyle = sc.state;
				nvarLevel = 1;
				sc.SetState(SCE_CMAKE_VARIABLE);
				sc.Forward();
			} else if ((sc.state == SCE_CMAKE_STRINGDQ && sc.ch == '\"')
				|| (sc.state == SCE_CMAKE_STRINGSQ && sc.ch == '\'')
				|| (sc.state == SCE_CMAKE_STRINGBT && sc.ch == '`')
				) {
				sc.ForwardSetState(SCE_CMAKE_DEFAULT);
			}
			break;
		case SCE_CMAKE_VARIABLE:
			if (sc.ch == '}') {
				if (nvarLevel > 0)
					--nvarLevel;
				if (nvarLevel == 0) {
					sc.ForwardSetState(varStyle);
					if (varStyle != SCE_CMAKE_DEFAULT) {
						goto _label_var_string;
					}
				}
			} else if (sc.Match('$', '{')) {
				++nvarLevel;
				sc.Forward();
			}
			break;
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_CMAKE_DEFAULT) {
			varStyle = SCE_CMAKE_DEFAULT;
			if (sc.ch == '#') {
				sc.SetState(SCE_CMAKE_COMMENT);
			} else if (sc.ch == '/' && sc.chNext == '/') { // CMakeCache.txt
				sc.SetState(SCE_CMAKE_COMMENT);
			} else if (sc.ch == '\\' && (sc.chNext == '\"' || sc.chNext == '\'')) {
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_CMAKE_STRINGDQ);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_CMAKE_STRINGSQ);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_CMAKE_STRINGBT);
			} else if (iswordstart(sc.ch)) {
				sc.SetState(SCE_CMAKE_IDENTIFIER);
			} else if (sc.Match('$', '{')) {
				nvarLevel = 1;
				sc.SetState(SCE_CMAKE_VARIABLE);
				sc.Forward();
			} else if (IsCmakeOperator(static_cast<char>(sc.ch))) {
				sc.SetState(SCE_CMAKE_OPERATOR);
				if (userDefType && sc.ch == ')')
					userDefType = 0;
			}
		}
	}

	if (sc.state == SCE_CMAKE_IDENTIFIER)
		goto _label_identifier;

	sc.Complete();
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_CMAKE_COMMENT)
#define CMakeMatch(str)			LexMatchIgnoreCase(i, styler, str)

static void FoldCmakeDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	//const bool foldCompact = styler.GetPropertyInt("fold.compact") != 0;

	Sci_PositionU endPos = startPos + length;
	//int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (style == SCE_CMAKE_OPERATOR) {
			if (ch == '(') {
				levelNext++;
			} else if (ch == ')') {
				levelNext--;
			}
		}

		if (style == SCE_CMAKE_WORD && stylePrev != SCE_CMAKE_WORD) {
			if (CMakeMatch("end")) {
				levelNext--;
			} else if (CMakeMatch("if") || CMakeMatch("function") || CMakeMatch("macro")
				|| CMakeMatch("foreach") || CMakeMatch("while")) {
				levelNext++;
			}
		}

		//if (!isspacechar(ch))
		//	visibleChars++;
		if (atEOL || (i == endPos-1)) {
			int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			//if (visibleChars == 0 && foldCompact)
			//	lev |= SC_FOLDLEVELWHITEFLAG;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			//visibleChars = 0;
		}
	}
}

LexerModule lmCmake(SCLEX_CMAKE, ColouriseCmakeDoc, "cmake", FoldCmakeDoc);
