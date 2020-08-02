// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Matlab, Octave, Scilab and Gnuplot (treated as same as Octave).

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

#define	LEX_MATLAB		40
#define	LEX_OCTAVE		61
#define	LEX_SCILAB		62

static constexpr bool IsMatlabOctave(int lexType) noexcept {
	return lexType == LEX_MATLAB || lexType == LEX_OCTAVE;
}

static bool IsLineCommentStart(int lexType, const StyleContext &sc, int visibleChars) noexcept {
	const int ch = sc.ch;
	const int chNext = sc.chNext;
	return ch == '#'	// Octave, Julia, Gnuplot, Shebang or invalid character
		|| (IsMatlabOctave(lexType) && (ch == '%' || (visibleChars == 0 && ch == '.' && chNext == '.' && sc.GetRelative(2) == '.')))
		|| (ch == '/' && chNext == '/'); // Scilab
}

static bool IsNestedCommentStart(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return visibleChars == 0 && chNext == '{'
		&& ((lexType == LEX_MATLAB && ch == '%') || (lexType == LEX_OCTAVE && (ch == '%' || ch == '#')))
		&& IsLexSpaceToEOL(styler, currentPos + 2);
}

static bool IsNestedCommentEnd(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return visibleChars == 0 && chNext == '}'
		&& ((lexType == LEX_MATLAB && ch == '%') || (lexType == LEX_OCTAVE && (ch == '%' || ch == '#')))
		&& IsLexSpaceToEOL(styler, currentPos + 2);
}

static bool IsBlockCommentStart(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return IsNestedCommentStart(lexType, ch, chNext, visibleChars, styler, currentPos)
		|| (ch == '/' && chNext == '*'); // Scilab
}

static bool IsBlockCommentEnd(int lexType, int ch, int chNext, int visibleChars, LexAccessor &styler, Sci_PositionU currentPos) noexcept {
	return IsNestedCommentEnd(lexType, ch, chNext, visibleChars, styler, currentPos)
		|| (ch == '*' && chNext == '/'); // Scilab
}

static bool IsBlockCommentStart(int lexType, StyleContext &sc, int visibleChars) noexcept {
	return IsBlockCommentStart(lexType, sc.ch, sc.chNext, visibleChars, sc.styler, sc.currentPos);
}

static bool IsBlockCommentEnd(int lexType, StyleContext &sc, int visibleChars) noexcept {
	return IsBlockCommentEnd(lexType, sc.ch, sc.chNext, visibleChars, sc.styler, sc.currentPos);
}

static bool IsNestedCommentStart(int lexType, StyleContext &sc, int visibleChars) noexcept {
	return IsNestedCommentStart(lexType, sc.ch, sc.chNext, visibleChars, sc.styler, sc.currentPos);
}

static constexpr bool IsMatOperator(int ch) noexcept {
	return isoperator(ch) || ch == '@' || ch == '\\' || ch == '$';
}

static constexpr bool IsInvalidFileName(int ch) noexcept {
	return isspacechar(ch) || ch == '<' || ch == '>' || ch == '/' || ch == '\\' || ch == '\'' || ch == '\"'
		|| ch == '|' || ch == '*' || ch == '?';
}

static void ColouriseMatlabDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &attributes = *keywordLists[1];
	const WordList &commands = *keywordLists[2];
	const WordList &function1 = *keywordLists[3];
	const WordList &function2 = *keywordLists[4];

	const int lexType = styler.GetPropertyInt("lexer.lang.type", LEX_MATLAB);

	int visibleChars = 0;
	StyleContext sc(startPos, length, initStyle, styler);
	int commentLevel = (sc.currentLine > 0) ? styler.GetLineState(sc.currentLine - 1) : 0;
	bool isTransposeOperator = false;

	bool hasTest = false; // Octave test/demo: %!demo %!test %testif %!assert %!error %!fail %!share %!function

	for (; sc.More(); sc.Forward()) {
		switch (sc.state) {
		case SCE_MAT_OPERATOR:
			sc.SetState(SCE_MAT_DEFAULT);
			break;
		case SCE_MAT_NUMBER:
			if (!IsADigit(sc.ch)) {
				if (IsFloatExponent(sc.ch, sc.chNext)) {
					sc.Forward();
				} else if (!(sc.ch == '.' && sc.chPrev != '.')) {
					if (sc.ch == 'i' || sc.ch == 'j' || sc.ch == 'I' || sc.ch == 'J') {
						// complex, 'I','J' in Octave
						sc.Forward();
					}
					sc.SetState(SCE_MAT_DEFAULT);
				}
			}
			break;
		case SCE_MAT_HEXNUM:
			if (!IsHexDigit(sc.ch)) {
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
					const int chNext = sc.GetNextNSChar();
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
			if (sc.ch == '\'') {
				if (sc.chNext == '\'') {
					sc.Forward();
				} else {
					sc.ForwardSetState(SCE_MAT_DEFAULT);
				}
			}
			break;
		case SCE_MAT_DOUBLEQUOTESTRING:
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_MAT_DEFAULT);
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
			} else if (sc.ch == '0' && (sc.chNext == 'x' || sc.chNext == 'X')) {
				isTransposeOperator = true;
				sc.SetState(SCE_MAT_HEXNUM);
				sc.Forward();
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				isTransposeOperator = true;
				sc.SetState(SCE_MAT_NUMBER);
			} else if (sc.ch == '@' && IsIdentifierStart(sc.chNext)) {
				sc.SetState(SCE_MAT_CALLBACK);
				sc.Forward();
			} else if (IsIdentifierStart(sc.ch)) {
				isTransposeOperator = true;
				sc.SetState(SCE_MAT_IDENTIFIER);
			} else if (IsMatOperator(sc.ch)) {
				sc.SetState(SCE_MAT_OPERATOR);
				isTransposeOperator = sc.ch == ')' || sc.ch == ']' || sc.ch == '}' || sc.ch == '.';
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, commentLevel);
			visibleChars = 0;
		}
	}

	sc.Complete();
}

// character after the "end" statement (skiped space and tabs)
static constexpr bool IsMatEndChar(char chEnd, int style) noexcept {
	return (chEnd == '\r' || chEnd == '\n' || chEnd == ';')
		|| (style == SCE_MAT_COMMENT || style == SCE_MAT_COMMENTBLOCK);
}

static constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_MAT_COMMENTBLOCK;
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_MAT_COMMENT)
#define StrEqu(str1, str2)		(strcmp(str1, str2) == 0)

static void FoldMatlabDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const int lexType = styler.GetPropertyInt("lexer.lang.type", LEX_MATLAB);
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	int numBrace = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
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

		if (foldComment && IsStreamCommentStyle(style)) {
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
		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (style == SCE_MAT_KEYWORD && stylePrev != SCE_MAT_KEYWORD && numBrace == 0 && chPrev != '.' && chPrev != ':') {
			constexpr int MaxFoldWordLength = 14 + 1; // unwind_protect
			char word[MaxFoldWordLength + 1];
			const Sci_PositionU len = LexGetRange(i, styler, iswordstart, word, sizeof(word));
			if ((StrEqu(word, "function") && LexGetNextChar(i + len, styler) != '(')
				|| StrEqu(word, "if")
				|| StrEqu(word, "for")
				|| StrEqu(word, "while")
				|| StrEqu(word, "try")
				|| (IsMatlabOctave(lexType) && (StrEqu(word, "switch") || StrEqu(word, "classdef") || StrEqu(word, "parfor")))
				|| ((lexType == LEX_OCTAVE) && (StrEqu(word, "do") || StrEqu(word, "unwind_protect")))
				|| ((lexType == LEX_SCILAB) && StrEqu(word, "select"))
				) {
				levelNext++;
			} else if ((lexType == LEX_OCTAVE) && StrEqu(word, "until")) {
				levelNext--;
			} else if (styler.Match(i, "end")) {
				levelNext--;
				//if (len == 3) {	// just "end"
				//	Sci_Position pos = LexSkipSpaceTab(i+3, endPos, styler);
				//	char chEnd = styler.SafeGetCharAt(pos);
				//	if (!(IsMatEndChar(chEnd, styler.StyleAt(pos)))) {
				//		levelNext++;
				//	}
				//}
			} else if (IsMatlabOctave(lexType) && chPrev != '@' && (StrEqu(word, "methods")
				|| StrEqu(word, "properties") || StrEqu(word, "events") || StrEqu(word, "enumeration"))) {
				// Matlab classdef
				Sci_Position pos = LexSkipSpaceTab(i + len, endPos, styler);
				const char chEnd = styler.SafeGetCharAt(pos);
				if (IsMatEndChar(chEnd, styler.StyleAt(pos))) {
					levelNext++;
				} else if (chEnd == '(') {
					pos++;
					pos = LexSkipSpaceTab(pos, endPos, styler);
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
			if (visibleChars == 0 && foldCompact)
				lev |= SC_FOLDLEVELWHITEFLAG;
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

LexerModule lmMatlab(SCLEX_MATLAB, ColouriseMatlabDoc, "matlab", FoldMatlabDoc);
