// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Fortran

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

static constexpr bool IsFWordChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '_' || ch == '%';
}
/*static const char *const fortranWordLists[] = {
	"Primary keywords",
	"user1",
	"user2",
	0
};*/

static void ColouriseFortranDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];
	const WordList &keywords3 = *keywordLists[2];

	int visibleChars = 0;
	styler.StartAt(startPos);

	// Do not leak onto next line
	if (initStyle == SCE_F_STRINGEOL || initStyle == SCE_F_COMMENT || initStyle == SCE_F_PREPROCESSOR) {
		initStyle = SCE_F_DEFAULT;
	}

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {

		if (sc.atLineStart) {
			if (sc.state == SCE_F_STRING2) {
				sc.SetState(sc.state);
			}
			visibleChars = 0;
		}

		if (sc.state == SCE_F_COMMENT) {
			if (sc.atLineStart) {
				sc.SetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_STRING1) {
			if (sc.atLineEnd) {
				sc.ChangeState(SCE_F_STRINGEOL);
			} else if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_STRING2) {
			if (sc.atLineEnd) {
				sc.ChangeState(SCE_F_STRINGEOL);
			} else if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_STRINGEOL) {
			if (sc.atLineStart) {
				sc.SetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_PREPROCESSOR) {
			if (sc.atLineStart) {
				sc.SetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_NUMBER) {
			if (!IsHexDigit(sc.ch)) {
				sc.SetState(SCE_F_DEFAULT);
			}
		} else if (sc.state == SCE_F_OPERATOR) {
			sc.SetState(SCE_F_DEFAULT);
		} else if (sc.state == SCE_F_OPERATOR2) {
			if (sc.ch == '.')
				sc.ForwardSetState(SCE_F_DEFAULT);
		} else if (sc.state == SCE_F_IDENTIFIER) {
			if (!IsFWordChar(sc.ch)) {
				char s[128];
				sc.GetCurrentLowered(s, sizeof(s));

				if (keywords.InList(s)) {
					sc.ChangeState(SCE_F_WORD);
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_F_WORD2);
				} else if (keywords3.InList(s)) {
					sc.ChangeState(SCE_F_WORD3);
				}
				sc.SetState(SCE_F_DEFAULT);
			}
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_F_DEFAULT) {
			if (visibleChars == 0 && (sc.ch == 'C' || sc.ch == 'c' || sc.ch == '*' || sc.ch == '!')) {
				if ((styler.MatchIgnoreCase(sc.currentPos + 1, "dec") && sc.GetRelative(4) == '$')
					|| (styler.MatchIgnoreCase(sc.currentPos + 1, "dir") && sc.GetRelative(4) == '$')
					|| (styler.MatchIgnoreCase(sc.currentPos + 1, "gcc") && sc.GetRelative(4) == '$')
					|| (styler.MatchIgnoreCase(sc.currentPos + 1, "$omp") && sc.GetRelative(5) == '$')
					|| (styler.MatchIgnoreCase(sc.currentPos + 1, "ms") && sc.GetRelative(3) == '$')
					) {
					sc.SetState(SCE_F_PREPROCESSOR);
				} else if ((sc.ch == 'C' || sc.ch == 'c') && !isspacechar(sc.chNext)) {
					sc.SetState(SCE_F_IDENTIFIER);
				} else {
					sc.SetState(SCE_F_COMMENT);
				}
			} else if (visibleChars == 0 && sc.ch == '#') {
				sc.SetState(SCE_F_PREPROCESSOR); // C preprocessor
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_F_STRING2);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_F_STRING1);
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_F_NUMBER);
			} else if (IsAlphaNumeric(sc.ch)) {
				sc.SetState(SCE_F_IDENTIFIER);
			} else if (sc.ch == '.' && IsAlpha(sc.chNext)) {
				sc.SetState(SCE_F_OPERATOR2);
				sc.Forward();
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_F_OPERATOR);
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

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_F_COMMENT)
#define StrEqu(str1, str2)		(strcmp(str1, str2) == 0)

static void FoldFortranDoc(Sci_PositionU startPos, Sci_Position length, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	//int styleNext = styler.StyleAt(startPos);
	//int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		//int stylePrev = style;
		//style = styleNext;
		//styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		//if (style == SCE_F_WORD && stylePrev != SCE_F_WORD) {
		//	char word[32];
		//	LexGetRangeLowered(i, styler, IsAlphaNumeric, word, sizeof(word));
		//}

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

LexerModule lmFortran(SCLEX_FORTRAN, ColouriseFortranDoc, "fortran", FoldFortranDoc);
