// Lexer for CMake.

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

/*static const char* const cmakeWordLists[] = {
	"Control Command",
	"Command",
	"Information Variables",
	"Behavior Variables",
	"System Variables",
	0
}*/

static constexpr bool IsCmakeOperator(int ch) noexcept {
	return ch == '(' || ch == ')' || ch == '=' || ch == ':' || ch == ';';
}

static bool IsBracketArgument(Accessor &styler, Sci_PositionU pos, bool start, int &bracketNumber) noexcept {
	int offset = 0;
	++pos; // bracket
	while (styler.SafeGetCharAt(pos) == '=') {
		++offset;
		++pos;
	}

	const char ch = styler.SafeGetCharAt(pos);
	if (start) {
		if (ch == '[') {
			bracketNumber = offset;
			return true;
		}
	} else {
		if (ch == ']' && offset == bracketNumber) {
			return true;
		}
	}
	return false;
}

static void ColouriseCmakeDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];

	int varStyle = SCE_CMAKE_DEFAULT;
	int nvarLevel = 0; // nested variable ${${}}
	int bracketNumber = 0;
	int userDefType = 0;
	StyleContext sc(startPos, length, initStyle, styler);

	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		nvarLevel = lineState & 0xffff;
		bracketNumber = lineState >> 16;
	}

	while (sc.More()) {
		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_CMAKE_OPERATOR:
			sc.SetState(SCE_CMAKE_DEFAULT);
			break;
		case SCE_CMAKE_IDENTIFIER:
			if (!iswordstart(sc.ch)) {
				char s[128];
				sc.GetCurrentLowered(s, sizeof(s) - 3);
				if (keywords.InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_CMAKE_WORD);
					if (strcmp(s, "function()") == 0 || strcmp(s, "endfunction()") == 0)
						userDefType = 1;
					else if (strcmp(s, "macro()") == 0 || strcmp(s, "endmacro()") == 0)
						userDefType = 2;
				} else if (keywords2.InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_CMAKE_COMMANDS);
				} else if (sc.GetNextNSChar() == '(') {
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
		case SCE_CMAKE_BLOCK_COMMENT:
			if (sc.chPrev == ']' && sc.ch == ']') {
				sc.ForwardSetState(SCE_CMAKE_DEFAULT);
			}
			break;
		case SCE_CMAKE_STRINGDQ:
		case SCE_CMAKE_STRINGSQ:
		case SCE_CMAKE_STRINGBT:
		case SCE_CMAKE_BRACKET_ARGUMENT:
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
			} else if (sc.state == SCE_CMAKE_BRACKET_ARGUMENT && sc.ch == ']') {
				if (IsBracketArgument(styler, sc.currentPos, false, bracketNumber)) {
					sc.Forward(1 + bracketNumber);
					sc.ForwardSetState(SCE_CMAKE_DEFAULT);
					bracketNumber = 0;
				}
			}
			break;
		case SCE_CMAKE_VARIABLE:
			if (sc.ch == '}') {
				if (nvarLevel > 0)
					--nvarLevel;
				if (nvarLevel == 0) {
					sc.ForwardSetState(varStyle);
					if (varStyle != SCE_CMAKE_DEFAULT) {
						continue;
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
				if (sc.chNext == '[' && sc.GetRelative(2) == '[') {
					sc.SetState(SCE_CMAKE_BLOCK_COMMENT);
					sc.Forward(2);
				} else {
					sc.SetState(SCE_CMAKE_COMMENT);
				}
			} else if (sc.ch == '[') {
				if (IsBracketArgument(styler, sc.currentPos, true, bracketNumber)) {
					sc.SetState(SCE_CMAKE_BRACKET_ARGUMENT);
					sc.Forward(2 + bracketNumber);
				}
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
			} else if (IsCmakeOperator(sc.ch)) {
				sc.SetState(SCE_CMAKE_OPERATOR);
				if (userDefType && sc.ch == ')')
					userDefType = 0;
			}
		}

		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, (bracketNumber << 16) | nvarLevel);
		}
		sc.Forward();
	}

	sc.Complete();
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_CMAKE_COMMENT)
#define CMakeMatch(str)			styler.MatchIgnoreCase(i, str)

static constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_CMAKE_BLOCK_COMMENT || style == SCE_CMAKE_BRACKET_ARGUMENT;
}

static void FoldCmakeDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	//const bool foldCompact = styler.GetPropertyInt("fold.compact") != 0;

	const Sci_PositionU endPos = startPos + length;
	//int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}
		if (foldComment && IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				levelNext--;
			}
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
		if (atEOL || (i == endPos - 1)) {
			const int levelUse = levelCurrent;
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
