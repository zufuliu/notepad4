// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Matlab, Octave, Scilab and Gnuplot (treated as same as Octave).

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>

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

using namespace Lexilla;

namespace {

#define	LEX_MATLAB		0
#define	LEX_OCTAVE		1
#define	LEX_SCILAB		2

constexpr bool IsMatlabOctave(int lexType) noexcept {
	return lexType == LEX_MATLAB || lexType == LEX_OCTAVE;
}

bool IsLineCommentStart(int lexType, const StyleContext &sc, int visibleChars) noexcept {
	const int ch = sc.ch;
	const int chNext = sc.chNext;
	return ch == '#'	// Octave, Julia, Gnuplot, Shebang or invalid character
		|| (IsMatlabOctave(lexType) && (ch == '%' || (visibleChars == 0 && ch == '.' && chNext == '.' && sc.GetRelative(2) == '.')))
		|| (ch == '/' && chNext == '/'); // Scilab
}

bool IsNestedCommentStart(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return visibleChars == 0 && chNext == '{'
		&& ((lexType == LEX_MATLAB && ch == '%') || (lexType == LEX_OCTAVE && (ch == '%' || ch == '#')))
		&& IsLexSpaceToEOL(styler, currentPos + 2);
}

bool IsNestedCommentEnd(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return visibleChars == 0 && chNext == '}'
		&& ((lexType == LEX_MATLAB && ch == '%') || (lexType == LEX_OCTAVE && (ch == '%' || ch == '#')))
		&& IsLexSpaceToEOL(styler, currentPos + 2);
}

bool IsBlockCommentStart(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return IsNestedCommentStart(lexType, ch, chNext, visibleChars, styler, currentPos)
		|| (ch == '/' && chNext == '*'); // Scilab
}

bool IsBlockCommentEnd(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return IsNestedCommentEnd(lexType, ch, chNext, visibleChars, styler, currentPos)
		|| (ch == '*' && chNext == '/'); // Scilab
}

bool IsBlockCommentStart(int lexType, StyleContext &sc, int visibleChars) noexcept {
	return IsBlockCommentStart(lexType, sc.ch, sc.chNext, visibleChars, sc.styler, sc.currentPos);
}

bool IsBlockCommentEnd(int lexType, StyleContext &sc, int visibleChars) noexcept {
	return IsBlockCommentEnd(lexType, sc.ch, sc.chNext, visibleChars, sc.styler, sc.currentPos);
}

bool IsNestedCommentStart(int lexType, StyleContext &sc, int visibleChars) noexcept {
	return IsNestedCommentStart(lexType, sc.ch, sc.chNext, visibleChars, sc.styler, sc.currentPos);
}

constexpr bool IsInvalidFileName(int ch) noexcept {
	return isspacechar(ch) || ch == '<' || ch == '>' || ch == '/' || ch == '\\' || ch == '\'' || ch == '\"'
		|| ch == '|' || ch == '*' || ch == '?';
}

void ColouriseMatlabDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = keywordLists[0];
	const WordList &attributes = keywordLists[1];
	const WordList &commands = keywordLists[2];
	const WordList &function1 = keywordLists[3];
	const WordList &function2 = keywordLists[4];

	const int lexType = styler.GetPropertyInt("lexer.lang", LEX_MATLAB);

	StyleContext sc(startPos, length, initStyle, styler);
	int visibleChars = 0;
	int commentLevel = 0;
	bool lineContinuation = false;
	bool isTransposeOperator = false;
	bool hasTest = false; // Octave test/demo: %!demo %!test %testif %!assert %!error %!fail %!share %!function
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		lineContinuation = lineState & true;
		commentLevel = lineState >> 1;
	}

	for (; sc.More(); sc.Forward()) {
		switch (sc.state) {
		case SCE_MAT_OPERATOR:
			sc.SetState(SCE_MAT_DEFAULT);
			break;
		case SCE_MAT_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_MAT_DEFAULT);
			}
			break;
		case SCE_MAT_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				char s[128];	// Matlab max indentifer length = 63, Octave unlimited
				sc.GetCurrent(s, sizeof(s));

				if (keywords.InList(s)) {
					sc.ChangeState(SCE_MAT_KEYWORD);
					isTransposeOperator = false;
				} else if (attributes.InList(s)) {
					sc.ChangeState(SCE_MAT_ATTRIBUTE);
				} else if (commands.InList(s)) {
					sc.ChangeState(SCE_MAT_INTERNALCOMMAND);
				} else if (function1.InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_MAT_FUNCTION1);
				} else if (function2.InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_MAT_FUNCTION2);
				} else {
					const int chNext = sc.GetDocNextChar();
					if (chNext == '(') {
						sc.ChangeState(SCE_MAT_FUNCTION);
					}
				}
				if (sc.ch == '@') {
					sc.SetState(SCE_MAT_OPERATOR);
					sc.Forward();
				}
				sc.SetState(SCE_MAT_DEFAULT);
			}
			break;
		case SCE_MAT_CALLBACK:
			if (!IsIdentifierChar(sc.ch)) {
				if (sc.ch == '@') {
					sc.SetState(SCE_MAT_OPERATOR);
					sc.Forward();
				}
				sc.SetState(SCE_MAT_DEFAULT);
			}
			break;
		case SCE_MAT_COMMAND:
			if (IsInvalidFileName(sc.ch)) {
				sc.SetState(SCE_MAT_DEFAULT);
			}
			break;
		case SCE_MAT_STRING:
			if (sc.atLineStart) {
				sc.SetState(SCE_MAT_DEFAULT);
			} else if (sc.ch == '\'') {
				sc.Forward();
				if (sc.ch != '\'') {
					sc.SetState(SCE_MAT_DEFAULT);
				}
			}
			break;
		case SCE_MAT_DOUBLEQUOTESTRING:
			if (sc.atLineStart) {
				if (lineContinuation) {
					lineContinuation = false;
				} else {
					sc.SetState(SCE_MAT_DEFAULT);
				}
			} else if (sc.ch == '\\' && lexType != LEX_MATLAB) {
				sc.Forward();
				if (sc.MatchLineEnd()) {
					lineContinuation = true;
				}
			} else if (sc.ch == '\"') {
				sc.Forward();
				if (sc.ch != '\"') {
					sc.SetState(SCE_MAT_DEFAULT);
				}
			}
			break;
		case SCE_MAT_COMMENTBLOCK:
			if (IsBlockCommentEnd(lexType, sc, visibleChars)) {
				if (IsMatlabOctave(lexType)) {
					--commentLevel;
					if (commentLevel < 0) {
						commentLevel = 0;
					}
				}
				if (commentLevel == 0) {
					sc.Forward();
					sc.ForwardSetState(SCE_MAT_DEFAULT);
				}
			} else if (IsNestedCommentStart(lexType, sc, visibleChars)) {
				++commentLevel;
				sc.Forward();
			}
			break;
		case SCE_MAT_COMMENT:
			if (sc.atLineStart) {
				sc.SetState(SCE_MAT_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_MAT_DEFAULT) {
			const bool transposeOperator = isTransposeOperator && sc.ch == '\'';
			// Octave allows whitespace before transpose operator
			isTransposeOperator = isTransposeOperator && lexType == LEX_OCTAVE && isspacechar(sc.ch);
			if (transposeOperator) {
				sc.SetState(SCE_MAT_OPERATOR);
			} else if (IsBlockCommentStart(lexType, sc, visibleChars)) {
				if (IsMatlabOctave(lexType)) {
					++commentLevel;
				}
				sc.SetState(SCE_MAT_COMMENTBLOCK);
				sc.Forward();
			} else if (IsLineCommentStart(lexType, sc, visibleChars)) {
				{
					sc.SetState(SCE_MAT_COMMENT);
					// Octave demo/test section, always placed in end of file
					if ((lexType == LEX_OCTAVE) && sc.atLineStart && sc.ch == '%' && sc.chNext == '!') {
						const Sci_Position pos = static_cast<Sci_Position>(sc.currentPos) + 2;
						if (!hasTest && (styler.Match(pos, "test") || styler.Match(pos, "demo")
							|| styler.Match(pos, "assert") || styler.Match(pos, "error") || styler.Match(pos, "warning")
							|| styler.Match(pos, "fail") || styler.Match(pos, "shared") || styler.Match(pos, "function"))) {
							hasTest = true;
						}
						if (hasTest) {
							sc.Forward(2);
							if (iswordstart(sc.ch)) {
								sc.SetState(SCE_MAT_IDENTIFIER);
							} else {
								sc.SetState(SCE_MAT_DEFAULT);
							}
						}
					} else if (sc.ch == '.') {
						sc.Forward(2);
					}
				}
			} else if (IsMatlabOctave(lexType) && visibleChars == 0 && sc.ch == '!') {
				sc.SetState(SCE_MAT_COMMAND);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_MAT_STRING);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_MAT_DOUBLEQUOTESTRING);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				isTransposeOperator = true;
				sc.SetState(SCE_MAT_NUMBER);
			} else if (sc.ch == '@' && IsIdentifierStart(sc.chNext)) {
				sc.SetState(SCE_MAT_CALLBACK);
				sc.Forward();
			} else if (IsIdentifierStart(sc.ch)) {
				isTransposeOperator = true;
				sc.SetState(SCE_MAT_IDENTIFIER);
			} else if (IsAGraphic(sc.ch)) {
				sc.SetState(SCE_MAT_OPERATOR);
				isTransposeOperator = sc.ch == ')' || sc.ch == ']' || sc.ch == '}' || sc.ch == '.';
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			const int lineState = (commentLevel << 1) | static_cast<int>(lineContinuation);
			styler.SetLineState(sc.currentLine, lineState);
			visibleChars = 0;
		}
	}

	sc.Complete();
}

// character after the "end" statement (skiped space and tabs)
constexpr bool IsMatEndChar(char chEnd, int style) noexcept {
	return (chEnd == '\r' || chEnd == '\n' || chEnd == ';')
		|| (style == SCE_MAT_COMMENT || style == SCE_MAT_COMMENTBLOCK);
}

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_MAT_COMMENTBLOCK;
}

#define IsCommentLine(line)		IsLexCommentLine(styler, line, SCE_MAT_COMMENT)

void FoldMatlabDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const int lexType = styler.GetPropertyInt("lexer.lang", LEX_MATLAB);

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	int numBrace = 0;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char ch = '\0';
	char chNext = styler[startPos];
	int style = initStyle;
	int styleNext = styler.StyleAt(startPos);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (IsStreamCommentStyle(style)) {
			if (IsMatlabOctave(lexType)) {
				if (IsNestedCommentStart(lexType, ch, chNext, visibleChars, styler, i)) {
					levelNext++;
				} else if (IsNestedCommentEnd(lexType, ch, chNext, visibleChars, styler, i)) {
					levelNext--;
				}
			} else {
				if (!IsStreamCommentStyle(stylePrev)) {
					levelNext++;
				} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
					levelNext--;
				}
			}
		}
		if (atEOL && IsCommentLine(lineCurrent)) {
			levelNext += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
		}

		if (style == SCE_MAT_KEYWORD && stylePrev != SCE_MAT_KEYWORD && numBrace == 0 && chPrev != '.' && chPrev != ':') {
			char word[16]; // unwind_protect
			const Sci_PositionU len = LexGetRange(styler, i, iswordstart, word, sizeof(word));
			if ((StrEqual(word, "function") && LexGetNextChar(styler, i + len) != '(')
				|| StrEqualsAny(word, "if", "for", "while", "try")
				|| (IsMatlabOctave(lexType) && StrEqualsAny(word, "switch", "classdef", "parfor"))
				|| ((lexType == LEX_OCTAVE) && StrEqualsAny(word, "do", "unwind_protect"))
				|| ((lexType == LEX_SCILAB) && StrEqual(word, "select"))
				) {
				levelNext++;
			} else if ((lexType == LEX_OCTAVE) && StrEqual(word, "until")) {
				levelNext--;
			} else if (StrStartsWith(word, "end")) {
				levelNext--;
				//if (len == 3) {	// just "end"
				//	Sci_Position pos = LexSkipSpaceTab(i+3, endPos, styler);
				//	char chEnd = styler.SafeGetCharAt(pos);
				//	if (!(IsMatEndChar(chEnd, styler.StyleAt(pos)))) {
				//		levelNext++;
				//	}
				//}
			} else if (IsMatlabOctave(lexType) && chPrev != '@' && StrEqualsAny(word, "methods", "properties", "events", "enumeration")) {
				// Matlab classdef
				Sci_Position pos = LexSkipSpaceTab(styler, i + len, endPos);
				const char chEnd = styler.SafeGetCharAt(pos);
				if (IsMatEndChar(chEnd, styler.StyleAt(pos))) {
					levelNext++;
				} else if (chEnd == '(') {
					pos++;
					pos = LexSkipSpaceTab(styler, pos, endPos);
					if (styler.StyleAt(pos) == SCE_MAT_ATTRIBUTE)
						levelNext++;
				}
			}
		}

		if (style == SCE_MAT_OPERATOR) { // too many () [] {}
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
				numBrace++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
				numBrace--;
			}
		}

		if (!isspacechar(ch))
			visibleChars++;

		if (atEOL || (i == endPos - 1)) {
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			visibleChars = 0;
		}
	}
}

}

LexerModule lmMatlab(SCLEX_MATLAB, ColouriseMatlabDoc, "matlab", FoldMatlabDoc);
