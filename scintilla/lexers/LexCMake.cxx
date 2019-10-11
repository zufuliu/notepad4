// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for CMake.
// https://cmake.org/cmake/help/v3.15/manual/cmake-language.7.html

#include <cstring>
#include <cassert>

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

constexpr bool IsCmakeOperator(int ch) noexcept {
	return ch == '(' || ch == ')' || ch == '=' || ch == ':' || ch == ';'
		|| ch == '$' || ch == '<' || ch == '>' || ch == ','; // Generator expressions
}

constexpr bool IsCmakeChar(int ch) noexcept {
	return iswordchar(ch) || ch == '-' || ch == '+';
}

bool IsBracketArgument(Accessor &styler, Sci_PositionU pos, bool start, int &bracketNumber) noexcept {
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

void ColouriseCmakeDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];

	int outerStyle = SCE_CMAKE_DEFAULT;
	int varNestedLevel = 0; // nested variable: ${${}}
	int generatorExpr = 0; // nested generator expressions: $<$<>>
	int bracketNumber = 0; // number of '=' in bracket: [[]]
	int userDefType = SCE_CMAKE_DEFAULT;
	int chBeforeNumber = 0;

	StyleContext sc(startPos, length, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		outerStyle = lineState & 0xff;
		bracketNumber = lineState >> 8;
		if (outerStyle != SCE_CMAKE_DEFAULT) {
			sc.SetState(outerStyle);
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_CMAKE_OPERATOR:
			sc.SetState(SCE_CMAKE_DEFAULT);
			break;
		case SCE_CMAKE_NUMBER:
			if (!(IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext)))) {
				if (IsCmakeChar(sc.ch) || IsCmakeChar(chBeforeNumber)) {
					sc.ChangeState(SCE_CMAKE_DEFAULT);
				}
				sc.SetState(SCE_CMAKE_DEFAULT);
			}
			break;
		case SCE_CMAKE_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				const int chNext = sc.GetNextNSChar();
				if (chNext == '(') {
					// command, function and macro are case insensitive
					// see Command Invocations: space* identifier space* '(' arguments ')'
					char s[128];
					sc.GetCurrentLowered(s, sizeof(s));
					userDefType = SCE_CMAKE_DEFAULT;
					if (keywords.InListPrefixed(s, '(')) {
						sc.ChangeState(SCE_CMAKE_WORD);
						if (strcmp(s, "function") == 0) {
							userDefType = SCE_CMAKE_FUNCATION;
						} else if (strcmp(s, "macro") == 0) {
							userDefType = SCE_CMAKE_MACRO;
						}
					} else if (keywords2.InListPrefixed(s, '(')) {
						sc.ChangeState(SCE_CMAKE_COMMANDS);
					} else {
						sc.ChangeState(SCE_CMAKE_FUNCATION);
					}
				} else if (userDefType != SCE_CMAKE_DEFAULT) {
					sc.ChangeState(userDefType);
					userDefType = SCE_CMAKE_DEFAULT;
				} else {
					// case sensitive
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
			if (sc.ch == ']' && (sc.chNext == '=' || sc.chNext == ']')) {
				if (IsBracketArgument(styler, sc.currentPos, false, bracketNumber)) {
					sc.Forward(1 + bracketNumber);
					sc.ForwardSetState(SCE_CMAKE_DEFAULT);
					bracketNumber = 0;
				}
			}
			break;
		case SCE_CMAKE_STRING:
			if (sc.ch == '\\') {
				if (sc.chNext == '\n' || sc.chNext == '\r') {
					sc.SetState(SCE_CMAKE_LINE_CONTINUE);
					sc.ForwardSetState(SCE_CMAKE_STRING);
				} else {
					sc.SetState(SCE_CMAKE_ESCAPE_SEQUENCE);
					sc.Forward();
				}
			} else if (sc.Match('$', '{')) {
				varNestedLevel = 1;
				sc.SetState(SCE_CMAKE_VARIABLE);
			} else if (sc.Match('$', '<')) {
				generatorExpr = 1;
				sc.SetState(SCE_CMAKE_OPERATOR);
			} else if ((sc.ch == '$' || sc.ch == '@') && IsIdentifierStart(sc.chNext)) {
				sc.SetState((sc.ch == '$') ? SCE_CMAKE_VARIABLE_DOLLAR : SCE_CMAKE_VARIABLE_AT);
			} else if (generatorExpr && IsCmakeOperator(sc.ch)) {
				if (sc.ch == '>') {
					--generatorExpr;
				}
				sc.SetState(SCE_CMAKE_OPERATOR);
				sc.ForwardSetState(SCE_CMAKE_STRING);
				continue;
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_CMAKE_DEFAULT);
				outerStyle = SCE_CMAKE_DEFAULT;
			}
			break;
		case SCE_CMAKE_ESCAPE_SEQUENCE:
			if (sc.ch == '\\') {
				if (sc.chNext == '\n' || sc.chNext == '\r') {
					sc.SetState(SCE_CMAKE_LINE_CONTINUE);
					sc.ForwardSetState(outerStyle);
				} else {
					sc.Forward();
				}
			} else {
				sc.SetState(outerStyle);
				if (outerStyle != SCE_CMAKE_DEFAULT) {
					continue;
				}
			}
			break;
		case SCE_CMAKE_BRACKET_ARGUMENT:
			if (sc.ch == ']' && (sc.chNext == '=' || sc.chNext == ']')) {
				if (IsBracketArgument(styler, sc.currentPos, false, bracketNumber)) {
					sc.Forward(1 + bracketNumber);
					sc.ForwardSetState(SCE_CMAKE_DEFAULT);
					bracketNumber = 0;
				}
			}
			break;
		case SCE_CMAKE_VARIABLE:
			if (sc.ch == '}') {
				--varNestedLevel;
				if (varNestedLevel == 0) {
					sc.ForwardSetState(outerStyle);
					if (outerStyle != SCE_CMAKE_DEFAULT) {
						continue;
					}
				}
			} else if (sc.Match('$', '{')) {
				++varNestedLevel;
			}
			break;
		case SCE_CMAKE_VARIABLE_DOLLAR:
			if (!IsIdentifierChar(sc.ch)) {
				bool done = false;
				if (sc.ch == '{') {
					char s[8];
					sc.GetCurrent(s, sizeof(s));
					if (strcmp(s, "$ENV") == 0 || strcmp(s, "$CACHE") == 0) {
						sc.SetState(SCE_CMAKE_VARIABLE);
						varNestedLevel = 1;
						done = true;
					}
				}
				if (!done) {
					sc.SetState(outerStyle);
					if (outerStyle != SCE_CMAKE_DEFAULT) {
						continue;
					}
				}
			}
			break;
		case SCE_CMAKE_VARIABLE_AT:
			if (!IsIdentifierChar(sc.ch)) {
				if (sc.ch == '@') {
					sc.Forward();
				}
				sc.SetState(outerStyle);
				if (outerStyle != SCE_CMAKE_DEFAULT) {
					continue;
				}
			}
			break;
		}

		if (sc.state == SCE_CMAKE_DEFAULT) {
			if (sc.ch == '#') {
				if (sc.chNext == '[' && IsBracketArgument(styler, sc.currentPos + 1, true, bracketNumber)) {
					sc.SetState(SCE_CMAKE_BLOCK_COMMENT);
					sc.Forward(2 + bracketNumber);
				} else {
					sc.SetState(SCE_CMAKE_COMMENT);
				}
			} else if (sc.ch == '[' && (sc.chNext == '=' || sc.chNext == '[')) {
				if (IsBracketArgument(styler, sc.currentPos, true, bracketNumber)) {
					sc.SetState(SCE_CMAKE_BRACKET_ARGUMENT);
					sc.Forward(2 + bracketNumber);
				}
			} else if (sc.Match('/', '/')) { // CMakeCache.txt
				sc.SetState(SCE_CMAKE_COMMENT);
			} else if (sc.ch == '\"') {
				outerStyle = SCE_CMAKE_STRING;
				sc.SetState(SCE_CMAKE_STRING);
			} else if (sc.Match('$', '{')) {
				varNestedLevel = 1;
				outerStyle = generatorExpr ? outerStyle : SCE_CMAKE_DEFAULT;
				sc.SetState(SCE_CMAKE_VARIABLE);
			} else if ((sc.ch == '$' || sc.ch == '@') && IsIdentifierStart(sc.chNext)) {
				outerStyle = generatorExpr ? outerStyle : SCE_CMAKE_DEFAULT;
				sc.SetState((sc.ch == '$') ? SCE_CMAKE_VARIABLE_DOLLAR : SCE_CMAKE_VARIABLE_AT);
				sc.Forward();
			} else if (sc.ch == '\\') {
				sc.SetState(SCE_CMAKE_ESCAPE_SEQUENCE);
				sc.Forward();
			} else if (IsIdentifierStart(sc.ch)) {
				sc.SetState(SCE_CMAKE_IDENTIFIER);
			} else if (IsADigit(sc.ch) || (sc.ch == '-' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_CMAKE_NUMBER);
				chBeforeNumber = sc.chPrev;
			} else if (IsCmakeOperator(sc.ch)) {
				sc.SetState(SCE_CMAKE_OPERATOR);
				if (generatorExpr) {
					if (sc.Match('$', '<')) {
						++generatorExpr;
					} else if (sc.ch == '>') {
						--generatorExpr;
						sc.ForwardSetState(generatorExpr ? SCE_CMAKE_DEFAULT : outerStyle);
						continue;
					}
				}
			}
		}

		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, (bracketNumber << 8) | outerStyle);
		}
		sc.Forward();
	}

	sc.Complete();
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_CMAKE_COMMENT)
#define CMakeMatch(str)			styler.MatchIgnoreCase(i, str)

void FoldCmakeDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
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
		if (style == SCE_CMAKE_BLOCK_COMMENT || style == SCE_CMAKE_BRACKET_ARGUMENT) {
			if (style != stylePrev) {
				levelNext++;
			} else if (style != styleNext) {
				levelNext--;
			}
		} else if (style == SCE_CMAKE_OPERATOR) {
			if (ch == '(') {
				levelNext++;
			} else if (ch == ')') {
				levelNext--;
			}
		} else if (style == SCE_CMAKE_WORD && stylePrev != SCE_CMAKE_WORD) {
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

}

LexerModule lmCmake(SCLEX_CMAKE, ColouriseCmakeDoc, "cmake", FoldCmakeDoc);
