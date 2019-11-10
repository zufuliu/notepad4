// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for GraphViz/Dot.

#include <cassert>
#include <cstring>
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

static constexpr bool IsGraphOp(int ch) noexcept {
	return ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == '='
		|| ch == ';' || ch == ',' || ch == '>' || ch == '+' || ch == '-'
		|| ch == ':' || ch == '<' || ch == '/';
}

#define MAX_WORD_LENGTH	15
static void ColouriseGraphDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const bool fold = styler.GetPropertyInt("fold", 1) != 0;
	const WordList &keywords = *keywordLists[0]; // command

	int state = initStyle;
	int ch = 0;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;
	char buf[MAX_WORD_LENGTH + 1] = "";
	int wordLen = 0;
	int chPrevNonWhite = 0;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int chPrev = ch;
		if (!IsASpace(ch) && state != SCE_C_COMMENTLINE && state != SCE_C_COMMENT && state != SCE_C_COMMENTDOC)
			chPrevNonWhite = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent));
		if (atLineStart) {
			visibleChars = 0;
		}
		if (atEOL || i == endPos - 1) {
			if (fold) {
				const int levelUse = levelCurrent;
				int lev = levelUse | levelNext << 16;
				if (levelUse < levelNext)
					lev |= SC_FOLDLEVELHEADERFLAG;
				if (lev != styler.LevelAt(lineCurrent)) {
					styler.SetLevel(lineCurrent, lev);
				}
				levelCurrent = levelNext;
			}
			lineCurrent++;
		}

		switch (state) {
		case SCE_C_OPERATOR:
			styler.ColourTo(i - 1, state);
			state = SCE_C_DEFAULT;
			break;
		case SCE_C_NUMBER:
			if (!IsDecimalNumber(chPrev, ch, chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			break;
		case SCE_C_IDENTIFIER:
			if (!(iswordstart(ch) || ch == '-')) {
				buf[wordLen] = '\0';
				Sci_PositionU pos = i;
				while (IsASpace(styler.SafeGetCharAt(pos++)));
				if (styler[pos - 1] == '=') {
					styler.ColourTo(i - 1, SCE_C_WORD2);
				} else if (keywords.InList(buf)) {
					styler.ColourTo(i - 1, SCE_C_WORD);
				}
				state = SCE_C_DEFAULT;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_C_LABEL:
			if (!(iswordstart(ch) || ch == '-')) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			break;
		case SCE_C_DIRECTIVE:
			if (chPrev == '>' || IsASpace(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			break;
		case SCE_C_STRING:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			} else if (ch == '\\' && (chNext == '\\' || chNext == '\"')) {
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if (ch == '\"') {
				styler.ColourTo(i, SCE_C_STRING);
				state = SCE_C_DEFAULT;
				continue;
			}
			break;
		case SCE_C_COMMENTLINE:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			break;
		case SCE_C_COMMENT:
			if (ch == '*' && chNext == '/') {
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
				styler.ColourTo(i, state);
				state = SCE_C_DEFAULT;
				levelNext--;
				continue;
			}
			break;
		case SCE_C_COMMENTDOC:
			if (chPrev == '-' && ch == '>') {
				styler.ColourTo(i, state);
				state = SCE_C_DEFAULT;
				continue;
			}
			break;
		}

		if (state == SCE_C_DEFAULT) {
			if ((ch == '/' && chNext == '/') || (visibleChars == 0 && ch == '#')) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENT;
				levelNext++;
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_STRING;
			} else if (chPrevNonWhite == '=' && (IsADigit(ch) || (ch == '.' && IsADigit(chNext)))) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_NUMBER;
			} else if (chPrevNonWhite == '=' && iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_LABEL;
			} else if (ch == '<' && chNext == '!') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTDOC;
			} else if (ch == '<' && (IsAlpha(chNext) || chNext == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DIRECTIVE;
			} else if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_IDENTIFIER;
				buf[0] = static_cast<char>(ch);
				wordLen = 1;
			} else if (ch == '>' && !(chPrevNonWhite == '-' || chPrevNonWhite == '>')) {
				styler.ColourTo(i, SCE_C_DIRECTIVE);
			} else if (IsGraphOp(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_OPERATOR;
				if (ch == '{' || ch == '[') {
					levelNext++;
				} else if (ch == '}' || ch == ']') {
					levelNext--;
				}
			}
		}

		visibleChars = visibleChars || !isspacechar(ch);
	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

LexerModule lmGraphViz(SCLEX_GRAPHVIZ, ColouriseGraphDoc, "gv");
