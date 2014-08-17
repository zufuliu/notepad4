// Lexer for Nsis.

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "CharacterSet.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

/*static const char* const nsisWordLists[] = {
	"Preprocessor Word",
	"Keyword",
	"Functions",
	0
}*/

static void ColouriseNsisDoc(unsigned int startPos, int length, int initStyle, WordList *keywordLists[], Accessor &styler) {
	WordList &keywords = *keywordLists[0];
	WordList &keywords2 = *keywordLists[1];

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {
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

		// Determine if the current state should terminate.
		switch (sc.state) {
			case SCE_NSIS_OPERATOR:
				sc.SetState(SCE_NSIS_DEFAULT);
				break;
			case SCE_NSIS_IDENTIFIER:
_label_identifier:
				if (!iswordstart(sc.ch)) {
					char s[128];
					sc.GetCurrentLowered(s, sizeof(s));
					if (s[0] == '!' && keywords.InList(&s[1])) {
						sc.ChangeState(SCE_NSIS_PREPROCESSOR);
					} else if (keywords2.InList(s)) {
						sc.ChangeState(SCE_NSIS_WORD);
					}
					sc.SetState(SCE_NSIS_DEFAULT);
				}
				break;
			case SCE_NSIS_NUMBER:
			case SCE_NSIS_VARIABLE:
				if (!iswordstart(sc.ch)) {
					sc.SetState(SCE_NSIS_DEFAULT);
				}
				break;

			case SCE_NSIS_COMMENT:
				if (sc.Match('*', '/')) {
					sc.Forward();
					sc.ForwardSetState(SCE_NSIS_DEFAULT);
				}
				break;
			case SCE_NSIS_COMMENTLINE:
				if (sc.atLineEnd) {
					sc.SetState(SCE_NSIS_DEFAULT);
				}
				break;

			case SCE_NSIS_STRINGDQ:
			case SCE_NSIS_STRINGSQ:
			case SCE_NSIS_STRINGBT:
				if (sc.atLineEnd) {
					sc.SetState(SCE_NSIS_DEFAULT);
				} else if ((sc.state == SCE_NSIS_STRINGDQ && sc.ch == '\"')
					|| (sc.state == SCE_NSIS_STRINGSQ && sc.ch == '\'')
					|| (sc.state == SCE_NSIS_STRINGBT && sc.ch == '`')) {
					sc.ForwardSetState(SCE_NSIS_DEFAULT);
				}
				break;
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_NSIS_DEFAULT) {
			if (sc.Match('/', '*')) {
				sc.SetState(SCE_NSIS_COMMENT);
				sc.Forward();
			} else if (sc.ch == '#' || sc.ch == ';') {
				sc.SetState(SCE_NSIS_COMMENTLINE);
			} else if (sc.ch == '$' && (sc.chNext == '$' || iswordchar(sc.chNext))) {
				sc.SetState(SCE_NSIS_VARIABLE);
				if (sc.chNext == '$')
					sc.Forward();
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_NSIS_STRINGDQ);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_NSIS_STRINGSQ);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_NSIS_STRINGBT);
			} else if (IsADigit(sc.ch)) {
				sc.SetState(SCE_NSIS_NUMBER);
			} else if (iswordstart(sc.ch) || sc.ch == '!') {
				sc.SetState(SCE_NSIS_IDENTIFIER);
			} else if (isoperator(static_cast<char>(sc.ch))) {
				sc.SetState(SCE_NSIS_OPERATOR);
			}
		}
	}

	if (sc.state == SCE_NSIS_IDENTIFIER)
		goto _label_identifier;

	sc.Complete();
}

static inline bool IsStreamCommentStyle(int style) {
	return style == SCE_NSIS_COMMENT;
}
static bool IsNsisCommentLine(int line, Accessor &styler) {
	int pos = LexLineSkipSpaceTab(line, styler);
	char ch = styler.SafeGetCharAt(pos);
	return (ch == '#' || ch == ';') && (styler.StyleAt(pos) == SCE_NSIS_COMMENTLINE);
}
#define IsCommentLine(line)		IsNsisCommentLine(line, styler)
#define NsisMatch(str)			LexMatchIgnoreCase(i, styler, str)

static void FoldNsisDoc(unsigned int startPos, int length, int initStyle, WordList *[], Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	//const bool foldCompact = styler.GetPropertyInt("fold.compact") != 0;

	unsigned int endPos = startPos + length;
	//int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (unsigned int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

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

LexerModule lmNsis(SCLEX_NSIS, ColouriseNsisDoc, "nsis", FoldNsisDoc);
