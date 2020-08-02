// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for F#

#include <cstdlib>
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

static constexpr bool IsFSOperator(int ch) noexcept {
	return isoperator(ch) || (ch == '\'' || ch == '@' || ch == '$' || ch == '#' || ch == '`');
}

/*static const char *const fsharpWordLists[] = {
	"Primary keywords",
	"Type Keywords",
	"preprocessor",
	"Attributes",
	0
};*/

static void ColouriseFSharpDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];
	const WordList &keywords3 = *keywordLists[2];
	const WordList &keywords4 = *keywordLists[3];

	int visibleChars = 0;
	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {
		if (sc.atLineStart) {
			if (sc.state == SCE_FSHARP_STRING) {
				sc.SetState(sc.state);
			}
		}
		// Handle line continuation generically.
		if (sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continue;
			}
		}

		switch (sc.state) {
		case SCE_FSHARP_OPERATOR:
			sc.SetState(SCE_FSHARP_DEFAULT);
			break;
		case SCE_FSHARP_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_FSHARP_DEFAULT);
			}
			break;
		case SCE_FSHARP_IDENTIFIER:
			if (!iswordstart(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));

				if (keywords.InList(s)) {
					sc.ChangeState(SCE_FSHARP_KEYWORD);
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_FSHARP_TYPEKEYWORD);
				} else if (s[0] == '#' && keywords3.InList(s + 1)) {
					sc.ChangeState(SCE_FSHARP_PREPROCESSOR);
				} else if (keywords4.InList(s)) {
					sc.ChangeState(SCE_FSHARP_ATTRIBUTE);
				}
				sc.SetState(SCE_FSHARP_DEFAULT);
			}
			break;

		case SCE_FSHARP_COMMENT:
			if (sc.Match('*', ')')) {
				sc.Forward();
				sc.ForwardSetState(SCE_FSHARP_DEFAULT);
			}
			break;
		case SCE_FSHARP_COMMENTLINE:
			if (sc.atLineStart) {
				visibleChars = 0;
				sc.SetState(SCE_FSHARP_DEFAULT);
			}
			break;
		case SCE_FSHARP_QUOTATION:
			if (sc.Match('@', '>')) {
				sc.Forward();
				sc.ForwardSetState(SCE_FSHARP_DEFAULT);
			}
			break;

		case SCE_FSHARP_CHARACTER:
		case SCE_FSHARP_STRING:
		case SCE_FSHARP_VERBATIM:
			if (sc.atLineEnd) {
				visibleChars = 0;
				sc.ChangeState(SCE_FSHARP_STRINGEOL);
			} else if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if ((sc.state == SCE_FSHARP_CHARACTER && sc.ch == '\'')
				|| ((sc.state == SCE_FSHARP_STRING || sc.state == SCE_FSHARP_VERBATIM) && sc.ch == '\"')) {
				if (sc.chNext == 'B')
					sc.Forward();
				sc.ForwardSetState(SCE_FSHARP_DEFAULT);
			}
			break;
		case SCE_FSHARP_STRINGEOL:
			if (sc.atLineStart) {
				sc.SetState(SCE_FSHARP_DEFAULT);
			}
			break;
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_FSHARP_DEFAULT) {
			if (sc.Match('(', '*')) {
				sc.SetState(SCE_FSHARP_COMMENT);
				sc.Forward();
			} else if (sc.Match('/', '/')) {
				sc.SetState(SCE_FSHARP_COMMENTLINE);
			} else if (sc.ch == '@' && sc.chNext == '\"') {
				sc.SetState(SCE_FSHARP_VERBATIM);
				sc.Forward(2);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_FSHARP_STRING);
			} else if (sc.ch == '\'' && (sc.chNext == '\\' || (sc.chNext != '\'' && sc.GetRelative(2) == '\''))) {
				sc.SetState(SCE_FSHARP_CHARACTER);
			} else if (sc.ch == '<' && sc.chNext == '@') {
				sc.SetState(SCE_FSHARP_QUOTATION);
			} else if (sc.ch == '0' && (sc.chNext == 'x' || sc.chNext == 'X')) {
				sc.SetState(SCE_FSHARP_NUMBER);
				sc.Forward();
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_FSHARP_NUMBER);
			} else if (sc.ch == '#' && visibleChars == 0) {
				if (isspacechar(sc.chNext)) {
					sc.SetState(SCE_FSHARP_PREPROCESSOR);
					sc.ForwardSetState(SCE_FSHARP_DEFAULT);
				} else if (iswordstart(sc.chNext)) {
					sc.SetState(SCE_FSHARP_IDENTIFIER);
				} else {
					sc.SetState(SCE_FSHARP_OPERATOR);
				}
			} else if (sc.ch == '_' && !(iswordstart(sc.chPrev) || iswordstart(sc.chNext))) {
				sc.SetState(SCE_FSHARP_OPERATOR);
			} else if (iswordstart(sc.ch)) {
				sc.SetState(SCE_FSHARP_IDENTIFIER);
			} else if (IsFSOperator(sc.ch)) {
				sc.SetState(SCE_FSHARP_OPERATOR);
			}
		}

		if (sc.atLineEnd) {
			visibleChars = 0;
		}
		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}

	}

	sc.Complete();
}

#define IsFSLine(line, word)	IsLexLineStartsWith(line, styler, word, true, SCE_FSHARP_KEYWORD)
#define IsCommentLine(line) 	IsLexCommentLine(line, styler, SCE_FSHARP_COMMENTLINE)

static constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_FSHARP_COMMENT;
}
#define IsOpenLine(line)		IsFSLine(line, "open")

static void FoldFSharpDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	const bool foldPreprocessor = styler.GetPropertyInt("fold.preprocessor", 1) != 0;
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

		if (foldComment && IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				levelNext--;
			}
		}
		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (atEOL && IsOpenLine(lineCurrent)) {
			if (!IsOpenLine(lineCurrent - 1) && IsOpenLine(lineCurrent + 1))
				levelNext++;
			else if (IsOpenLine(lineCurrent - 1) && !IsOpenLine(lineCurrent + 1))
				levelNext--;
		}

		if (foldPreprocessor && styleNext == SCE_FSHARP_PREPROCESSOR) {
			if (styler.Match(i, "#if")) {
				levelNext++;
			} else if (styler.Match(i, "#endif")) {
				levelNext--;
			}
		}

		if (style == SCE_FSHARP_OPERATOR) {
			if (ch == '{' || ch == '[') {
				levelNext++;
			} else if (ch == '}' || ch == ']') {
				levelNext--;
			}
		}

		if (style == SCE_FSHARP_QUOTATION) {
			if (ch == '<' && chNext == '@') {
				levelNext++;
			} else if (ch == '@' && chNext == '>') {
				levelNext--;
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

LexerModule lmFSharp(SCLEX_FSHARP, ColouriseFSharpDoc, "fsharp", FoldFSharpDoc);
